/*
 * Copyright (c) 2017-2018 iwhisper.io
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

#include "rc_mem.h"
#include "linkedhashtable.h"

#ifdef _MSC_VER
#include "builtins.h"
#endif

typedef struct _hash_entry_i
{
    const void *         key;
    size_t               keylen;
    void *               data;
    uint32_t             hash_code;
    struct _hash_entry_i *next;
    struct _hash_entry_i *lst_prev;
    struct _hash_entry_i *lst_next;
} hash_entry_i;

typedef struct hashtable_iterator_i {
    hashtable_t *htab;
    hash_entry_i *current;
    hash_entry_i *next;
    int expected_mod_count;
} hashtable_iterator_i;

static_assert(sizeof(hash_entry_t) >= sizeof(hash_entry_i),
              "List entry size miss match.");
static_assert(sizeof(hashtable_iterator_t) >= sizeof(hashtable_iterator_i),
              "List iterator size miss match.");

struct _hashtable_t {
    size_t      capacity;
    size_t      count;
    int         mod_count;
    int         synced;
    pthread_rwlock_t lock;

    uint32_t (*hash_code)(const void *key, size_t len);
    int (*key_compare)(const void *key1, size_t len1,
                       const void *key2, size_t len2);

    hash_entry_i lst_head;
    hash_entry_i *buckets[];
};

static uint32_t default_hash_code(const void *key, size_t keylen)
{
    uint32_t h = 0;
    size_t i;

    for (i = 0; i < keylen; i++)
        h = 31 * h + ((const char *)key)[i];

    return h;
}

static int default_key_compare(const void *key1, size_t len1,
                               const void *key2, size_t len2)
{
    if (len1 != len2)
        return len1 < len2 ? -1 : 1;

    return memcmp(key1, key2, len1);
}

static void hashtable_destroy(void *htab);

static int BUCKET_SIZES[] = {
    7,      7,      7,      17,     31,     67,     127,    257,
    509,    1021,   2053,   4093,   8191,   16381,  32771,  65521
};

static size_t bucket_size(size_t capacity)
{
    int msb = (sizeof(unsigned long long) << 3) - __builtin_clzll(capacity) - 1;

    if (msb > 15)
        msb = 15;

    return BUCKET_SIZES[msb];
}

hashtable_t *hashtable_create(size_t capacity, int synced,
          uint32_t (*hash_code)(const void *key, size_t len),
          int (*key_compare)(const void *key1, size_t len1,
                             const void *key2, size_t len2))
{
    hashtable_t *htab;

    capacity = bucket_size(capacity ? capacity : 127);

    htab = (hashtable_t *)rc_zalloc(sizeof(hashtable_t)
                + sizeof(hash_entry_i *) * capacity, hashtable_destroy);
    if (!htab) {
        errno = ENOMEM;
        return NULL;
    }

    if (synced) {
        if (pthread_rwlock_init(&htab->lock, NULL) != 0) {
            deref(htab);
            return NULL;
        }
    }

    htab->capacity = capacity;
    htab->count = 0;
    htab->mod_count = 0;
    htab->synced = synced;

    htab->hash_code = hash_code ? hash_code : default_hash_code;
    htab->key_compare = key_compare ? key_compare : default_key_compare;

    htab->lst_head.lst_next = &htab->lst_head;
    htab->lst_head.lst_prev = &htab->lst_head;

    return htab;
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

static inline void hashtable_rlock(hashtable_t *htab)
{
    if (htab->synced) {
        int rc = pthread_rwlock_rdlock(&htab->lock);
        assert(rc == 0);
    }
}

static inline void hashtable_wlock(hashtable_t *htab)
{
    if (htab->synced) {
        int rc = pthread_rwlock_wrlock(&htab->lock);
        assert(rc == 0);
    }
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

static inline void hashtable_unlock(hashtable_t *htab)
{
    if (htab->synced) {
        pthread_rwlock_unlock(&htab->lock);
    }
}

static void hashtable_clear_i(hashtable_t *htab)
{
    hash_entry_i *entry;
    hash_entry_i *cur;

    if (htab->count == 0)
        return;

    entry = htab->lst_head.lst_next;
    while (entry != &htab->lst_head) {
        cur = entry;
        entry = entry->lst_next;

        deref(cur->data);
    }

    memset(htab->buckets, 0, sizeof(hash_entry_i *) * htab->capacity);

    htab->lst_head.lst_next = &htab->lst_head;
    htab->lst_head.lst_prev = &htab->lst_head;

    htab->count = 0;
    htab->mod_count++;
}

static void hashtable_destroy(void *htab)
{
    int synced;
    pthread_rwlock_t lock;

    if (!htab)
        return;

    synced = ((hashtable_t *)htab)->synced;

    if (synced) {
        lock = ((hashtable_t *)htab)->lock;
        if (pthread_rwlock_wrlock(&lock) != 0)
            return;
    }

    hashtable_clear_i(htab);
    memset(htab, 0, sizeof(hashtable_t));

    if (synced) {
        pthread_rwlock_unlock(&lock);
        pthread_rwlock_destroy(&lock);
    }
}

static void hashtable_add(hashtable_t *htab, hash_entry_t *entry)
{
    hash_entry_i *ent = (hash_entry_i *)entry;
    int idx;
    uint32_t hash_code;

    hash_code = htab->hash_code(ent->key, ent->keylen);
    idx = hash_code % htab->capacity;

    ent->hash_code = hash_code;
    ent->next = htab->buckets[idx];

    /* Add new entry to linked list tail */
    ent->lst_prev = htab->lst_head.lst_prev;
    ent->lst_next = &htab->lst_head;
    htab->lst_head.lst_prev->lst_next = ent;
    htab->lst_head.lst_prev = ent;

    htab->buckets[idx] = ent;

    htab->count++;
}

static hash_entry_i **hashtable_get_entry(hashtable_t *htab,
                                      const void *key, size_t keylen)
{
    hash_entry_i **entry;
    int idx;
    uint32_t hash_code;

    hash_code = htab->hash_code(key, keylen);
    idx = hash_code % htab->capacity;
    entry = &htab->buckets[idx];

    while (*entry) {
        if ((*entry)->hash_code == hash_code &&
            htab->key_compare((*entry)->key, (*entry)->keylen, key, keylen) == 0)
            return entry;

        entry = &(*entry)->next;
    }

    return NULL;
}

void *hashtable_put(hashtable_t *htab, hash_entry_t *entry)
{
    hash_entry_i **ent;
    hash_entry_i *new_entry = (hash_entry_i *)entry;

    assert(htab && entry && entry->key && entry->keylen && entry->data);
    if (!htab || !entry || !entry->key || !entry->keylen || !entry->data) {
        errno = EINVAL;
        return NULL;
    }

    hashtable_wlock(htab);

    ref(entry->data);

    ent = hashtable_get_entry(htab, entry->key, entry->keylen);
    if (ent) {
        new_entry->next = (*ent)->next;
        new_entry->lst_prev = (*ent)->lst_prev;
        new_entry->lst_next = (*ent)->lst_next;
        new_entry->hash_code = (*ent)->hash_code;

        new_entry->lst_prev->lst_next = new_entry;
        new_entry->lst_next->lst_prev = new_entry;

        deref((*ent)->data);
        *ent = new_entry;
    } else {
        hashtable_add(htab, entry);
    }

    htab->mod_count++;

    hashtable_unlock(htab);

    return entry->data;
}

void *hashtable_get(hashtable_t *htab, const void *key, size_t keylen)
{
    hash_entry_i **entry;
    void *val;

    assert(htab && key && keylen);
    if (!htab || !key || !keylen) {
        errno = EINVAL;
        return NULL;
    }

    hashtable_rlock(htab);

    entry = hashtable_get_entry(htab, key, keylen);
    val = entry ? ref((*entry)->data) : NULL;

    hashtable_unlock(htab);

    return val;

}

int hashtable_exist(hashtable_t *htab, const void *key, size_t keylen)
{
    int exist;

    assert(htab && key && keylen);
    if (!htab || !key || !keylen) {
        errno = EINVAL;
        return 0;
    }

    hashtable_rlock(htab);
    exist = hashtable_get_entry(htab, key, keylen) != NULL;
    hashtable_unlock(htab);

    return exist;
}

int hashtable_is_empty(hashtable_t *htab)
{
    assert(htab);
    if (!htab) {
        errno = EINVAL;
        return 0;
    }

    return htab->count == 0;
}

void *hashtable_remove(hashtable_t *htab, const void *key, size_t keylen)
{
    hash_entry_i **entry;
    hash_entry_i *to_remove;
    void *val = NULL;

    assert(htab && key && keylen);
    if (!htab || !key || !keylen) {
        errno = EINVAL;
        return NULL;
    }

    hashtable_wlock(htab);

    entry = hashtable_get_entry(htab, key, keylen);
    if (entry) {
        to_remove = *entry;
        *entry = (*entry)->next;

        /* Remove entry from linkedlist */
        to_remove->lst_prev->lst_next = to_remove->lst_next;
        to_remove->lst_next->lst_prev = to_remove->lst_prev;

        // val = deref(to_remove->data);
        // Pass reference to caller
        val = to_remove->data;

        htab->count--;
        htab->mod_count++;
    }

    hashtable_unlock(htab);

    return val;
}

void hashtable_clear(hashtable_t *htab)
{
    assert(htab);
    if (!htab) {
        errno = EINVAL;
        return;
    }

    hashtable_wlock(htab);
    hashtable_clear_i(htab);
    hashtable_unlock(htab);
}

hashtable_iterator_t *hashtable_iterate(hashtable_t *htab,
                                        hashtable_iterator_t *iterator)
{
    hashtable_iterator_i *it = (hashtable_iterator_i *)iterator;

    assert(htab && it);
    if (!htab || !it) {
        errno = EINVAL;
        return NULL;
    }

    hashtable_rlock(htab);

    it->htab = htab;
    it->current = NULL;
    it->next = htab->lst_head.lst_next;
    it->expected_mod_count = htab->mod_count;

    hashtable_unlock(htab);

    return iterator;
}

// return 1 on success, 0 end of iterator, -1 on modified conflict or error.
int hashtable_iterator_next(hashtable_iterator_t *iterator, void **key,
                              size_t *keylen, void **data)
{
    int rc;
    hashtable_iterator_i *it = (hashtable_iterator_i *)iterator;

    assert(it && it->htab && it->next && data);
    if (!it || !it->htab || !it->next || !data) {
        errno = EINVAL;
        return -1;
    }

    hashtable_rlock(it->htab);

    if (it->expected_mod_count != it->htab->mod_count) {
        errno = EAGAIN;
        rc = -1;
    } else {
        if (it->next == &it->htab->lst_head) { // end
            rc = 0;
        } else {
            it->current = it->next;
            it->next = it->next->lst_next;

            if (key)
                *key = (void *)it->current->key;
            if (keylen)
                *keylen = it->current->keylen;

            *data = it->current->data;
            ref(*data);

            rc = 1;
        }
    }

    hashtable_unlock(it->htab);

    return rc;
}

int hashtable_iterator_has_next(hashtable_iterator_t *iterator)
{
    hashtable_iterator_i *it = (hashtable_iterator_i *)iterator;

    assert(it && it->htab && it->next);
    if (!it || !it->htab || !it->next) {
        errno = EINVAL;
        return 0;
    }

    return it->next != &it->htab->lst_head;
}

// return 1 on success, 0 nothing removed, -1 on modified conflict or error.
int hashtable_iterator_remove(hashtable_iterator_t *iterator)
{
    void *ptr;
    hashtable_iterator_i *it = (hashtable_iterator_i *)iterator;

    assert(it && it->htab && it->next && it->current);
    if (!it || !it->htab || !it->next || !it->current) {
        errno = EINVAL;
        return -1;
    }

    if (it->expected_mod_count != it->htab->mod_count) {
        errno = EAGAIN;
        return -1;
    }

    ptr = hashtable_remove(it->htab, it->current->key, it->current->keylen);
    if (ptr) {
        deref(ptr);
        it->current = NULL;
        it->expected_mod_count++;
        return 1;
    }

    return 0;
}
