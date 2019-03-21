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

#ifndef __CRYSTAL_LINKED_HASHTABLE_H__
#define __CRYSTAL_LINKED_HASHTABLE_H__

#include <stdint.h>
#include <stddef.h>

#include <crystal/crystal_config.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _hashtable_t hashtable_t;

typedef struct _hash_entry_t
{
    const void *        key;
    size_t              keylen;
    void *              data;
    char                __opaque[sizeof(void *) * 4];
} hash_entry_t;

typedef struct hashtable_iterator_t {
    char __opaque[sizeof(void *) * 4];
} hashtable_iterator_t;

CRYSTAL_API
hashtable_t *hashtable_create(size_t capacity, int synced,
          uint32_t (*hash_code)(const void *key, size_t len),
          int (*key_compare)(const void *key1, size_t len1,
                             const void *key2, size_t len2));

CRYSTAL_API
void *hashtable_put(hashtable_t *htab, hash_entry_t *entry);

CRYSTAL_API
void *hashtable_get(hashtable_t *htab, const void *key, size_t keylen);

CRYSTAL_API
int hashtable_exist(hashtable_t *htab, const void *key, size_t keylen);

CRYSTAL_API
int hashtable_is_empty(hashtable_t *htab);

CRYSTAL_API
void *hashtable_remove(hashtable_t *htab, const void *key, size_t keylen);

CRYSTAL_API
void hashtable_clear(hashtable_t *htab);

CRYSTAL_API
hashtable_iterator_t *hashtable_iterate(hashtable_t *htab,
                                        hashtable_iterator_t *iterator);

// return 1 on success, 0 end of iterator, -1 on modified conflict or error.
CRYSTAL_API
int hashtable_iterator_next(hashtable_iterator_t *iterator, void **key,
                              size_t *keylen, void **data);

CRYSTAL_API
int hashtable_iterator_has_next(hashtable_iterator_t *iterator);

// return 1 on success, 0 nothing removed, -1 on modified conflict or error.
CRYSTAL_API
int hashtable_iterator_remove(hashtable_iterator_t *iterator);

#ifdef __cplusplus
}
#endif

#endif /* __CRYSTAL_LINKED_HASHTABLE_H__ */
