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

#include <string.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

#include "rc_mem.h"
#include "linkedlist.h"

typedef struct _list_entry_i {
    void *data;
    struct _list_entry_i *next;
    struct _list_entry_i *prev;
} list_entry_i;

typedef struct list_iterator_i {
    list_t   *lst;
    list_entry_i *current;
    list_entry_i *next;
    int expected_mod_count;
} list_iterator_i;

static_assert(sizeof(list_entry_t) >= sizeof(list_entry_i),
              "List entry size miss match.");
static_assert(sizeof(list_iterator_t) >= sizeof(list_iterator_i),
              "List iterator size miss match.");

struct _linked_list_t {
    size_t size;
    int mod_count;
    int synced;
    pthread_rwlock_t lock;
    int (*compare)(list_entry_t *entry1, list_entry_t *entry2);

    list_entry_i head;
};

static void list_destroy(void *lst);

list_t *list_create(int synced,
        int (*compare)(list_entry_t *entry1, list_entry_t *entry2))
{
    list_t *lst = (list_t *)rc_zalloc(sizeof(list_t), list_destroy);
    if (lst == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    lst->head.next = &lst->head;
    lst->head.prev = &lst->head;
    lst->synced = !(synced == 0);

    if (synced != 0)
        pthread_rwlock_init(&lst->lock, NULL);

    return lst;
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

static inline void list_rlock(list_t *lst)
{
    if (lst->synced) {
        int rc = pthread_rwlock_rdlock(&lst->lock);
        assert(rc == 0);
    }
}

static inline void list_wlock(list_t *lst)
{
    if (lst->synced) {
        int rc = pthread_rwlock_wrlock(&lst->lock);
        assert(rc == 0);
    }
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

static inline void list_unlock(list_t *lst)
{
    if (lst->synced) {
        pthread_rwlock_unlock(&lst->lock);
    }
}

int list_insert(list_t *lst, int index, list_entry_t *entry)
{
    list_entry_i *cur;
    list_entry_i *ent = (list_entry_i *)entry;

    assert(lst && entry && entry->data);
    assert(index >= -(int)(lst->size + 1) && index <= (int)lst->size);

    if (!lst || !entry || index < -(int)(lst->size + 1) || index > (int)lst->size) {
        errno = EINVAL;
        return 0;
    }

    list_wlock(lst);

    ref(entry->data);

    cur = &lst->head;
    if (index >= 0) {
        for (; index > 0; index--, cur = cur->next);
    } else {
        for (; index < 0; index++, cur = cur->prev);
    }

    ent->prev = cur;
    ent->next = cur->next;

    cur->next->prev = ent;
    cur->next = ent;

    lst->size++;
    lst->mod_count++;
    list_unlock(lst);

    return 1;
}

static void *list_remove_entry_nolock(list_t *lst, list_entry_t *entry)
{
    void *val;
    list_entry_i *ent = (list_entry_i *)entry;

    assert(lst && ent && ent->data && ent->next && ent->prev);

    val = ent->data;

    ent->next->prev = ent->prev;
    ent->prev->next = ent->next;
    ent->prev = NULL;
    ent->next = NULL;

    lst->size--;
    lst->mod_count++;

    return val;
}

void *list_remove_entry(list_t *lst, list_entry_t *entry)
{
    void *val;
    list_entry_i *ent = (list_entry_i *)entry;

    assert(lst && ent);

    if (!lst || !ent || !ent->data || !ent->next || !ent->prev) {
        errno = EINVAL;
        return NULL;
    }

    list_wlock(lst);

    val = list_remove_entry_nolock(lst, entry);

    // deref(val);
    // Pass reference to caller

    list_unlock(lst);

    return val;
}

void *list_remove(list_t *lst, int index)
{
    void *val;
    list_entry_i *ent;

    assert(lst);
    assert(index >= -(int)lst->size && index < (int)lst->size);

    if (!lst || index < -(int)lst->size || index >= (int)lst->size) {
        errno = EINVAL;
        return NULL;
    }

    list_wlock(lst);

    if (index >= 0) {
        for (ent = lst->head.next; index > 0; index--, ent = ent->next);
    } else {
        for (ent = &lst->head; index < 0; index++, ent = ent->prev);
    }

    val = ent->data;

    ent->next->prev = ent->prev;
    ent->prev->next = ent->next;
    ent->prev = NULL;
    ent->next = NULL;

    lst->size--;
    lst->mod_count++;

    // deref(val);
    // Pass reference to caller

    list_unlock(lst);

    return val;
}

void *list_get(list_t *lst, int index)
{
    void *val;
    list_entry_i *ent;

    assert(lst);
    assert(index >= -(int)lst->size && index < (int)lst->size);

    if (!lst || index < -(int)lst->size || index >= (int)lst->size) {
        errno = EINVAL;
        return NULL;
    }

    list_rlock(lst);

    if (index >= 0) {
        for (ent = lst->head.next; index > 0; index--, ent = ent->next);
    } else {
        for (ent = &lst->head; index < 0; index++, ent = ent->prev);
    }

    val = ent->data;

    ref(val);

    list_unlock(lst);

    return val;
}

size_t list_size(list_t *lst)
{
    assert(lst);
    if (!lst) {
        errno = EINVAL;
        return 0;
    }

    return lst->size;
}

static void list_clear_i(list_t *lst)
{
    list_entry_i *ent;
    list_entry_i *cur;

    ent = lst->head.next;
    while (ent != &lst->head) {
        cur = ent;
        ent = ent->next;

        deref(cur->data);
    }

    lst->head.next = &lst->head;
    lst->head.prev = &lst->head;

    lst->size = 0;
    lst->mod_count++;
}

void list_clear(list_t *lst)
{
    assert(lst);
    if (!lst) {
        errno = EINVAL;
        return;
    }

    list_wlock(lst);
    list_clear_i(lst);
    list_unlock(lst);
}

static void list_destroy(void *lst)
{
    int synced;
    pthread_rwlock_t lock;

    if (!lst)
        return;

    synced = ((list_t *)lst)->synced;

    if (synced) {
        lock = ((list_t *)lst)->lock;
        if (pthread_rwlock_wrlock(&lock) != 0)
            return;
    }

    list_clear_i((list_t *)lst);
    memset(lst, 0, sizeof(list_t));

    if (synced) {
        pthread_rwlock_unlock(&lock);
        pthread_rwlock_destroy(&lock);
    }
}

int list_find(list_t *lst, list_entry_t *entry)
{
    int index;
    list_entry_i *ent;

    assert(lst && entry && entry->data);
    if (!lst || !entry || !entry->data) {
        errno = EINVAL;
        return -1;
    }

    list_rlock(lst);

    for (ent = lst->head.next, index = 0; index < (int)lst->size;
            ent = ent->next, index++) {
        if (lst->compare ? (lst->compare((list_entry_t *)ent, entry) == 0)
                         : (ent->data == entry->data)) {
            break;
        }
    }

    if (index >= (int)lst->size)
        index = -1;

    list_unlock(lst);

    return index;
}

list_iterator_t *list_iterate(list_t *lst, list_iterator_t *iterator)
{
    list_iterator_i *it = (list_iterator_i *)iterator;

    assert(lst && it);
    if (!lst || !it) {
        errno = EINVAL;
        return NULL;
    }

    list_rlock(lst);

    it->lst = lst;
    it->current = NULL;
    it->next = lst->head.next;
    it->expected_mod_count = lst->mod_count;

    list_unlock(lst);

    return iterator;
}

// return 1 on success, 0 end of iterator, -1 on modified conflict or error.
int list_iterator_next(list_iterator_t *iterator, void **data)
{
    int rc;
    list_iterator_i *it = (list_iterator_i *)iterator;

    assert(it && it->lst && it->next && data);
    if (!it || !it->lst || !it->next || !data) {
        errno = EINVAL;
        return -1;
    }

    list_rlock(it->lst);

    if (it->expected_mod_count != it->lst->mod_count) {
        errno = EAGAIN;
        rc = -1;
    } else {
        if (it->next == &it->lst->head) { // end
            rc = 0;
        } else {
            it->current = it->next;
            it->next = it->next->next;

            *data = it->current->data;
            ref(*data);

            rc = 1;
        }
    }

    list_unlock(it->lst);

    return rc;
}

int list_iterator_has_next(list_iterator_t *iterator)
{
    list_iterator_i *it = (list_iterator_i *)iterator;

    assert(it && it->lst && it->next);
    if (!it || !it->lst || !it->next) {
        errno = EINVAL;
        return 0;
    }

    return it->next != &it->lst->head;
}

// return 1 on success, 0 nothing removed, -1 on modified conflict or error.
int list_iterator_remove(list_iterator_t *iterator)
{
    void *ptr;
    list_iterator_i *it = (list_iterator_i *)iterator;

    assert(it && it->lst && it->next && it->current);
    if (!it || !it->lst || !it->next || !it->current) {
        errno = EINVAL;
        return -1;
    }

    list_wlock(it->lst);

    if (it->expected_mod_count != it->lst->mod_count) {
        errno = EAGAIN;
        list_unlock(it->lst);
        return -1;
    }

    ptr = list_remove_entry_nolock(it->lst, (list_entry_t *)it->current);
    if (ptr) {
        deref(ptr);
        it->current = NULL;
        it->expected_mod_count++;
        list_unlock(it->lst);
        return 1;
    }

    list_unlock(it->lst);
    return 0;
}

