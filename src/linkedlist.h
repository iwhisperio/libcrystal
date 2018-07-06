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

#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <pthread.h>

#include <crystal_config.h>

#ifdef __cplusplus
extern "C" {
#endif

struct linked_list;

typedef struct _linked_list_entry_t {
    void *data;
    char __opaque[sizeof(void *) * 2];
} list_entry_t;

typedef struct _list_iterator_t {
    char __opaque[sizeof(void *) * 4];
} list_iterator_t;

typedef struct _linked_list_t list_t;

CRYSTAL_API
list_t *list_create(int synced,
        int (*compare)(list_entry_t *entry1, list_entry_t *entry2));

CRYSTAL_API
int list_insert(list_t *lst, int index, list_entry_t *entry);

static inline
void list_add(list_t *lst, list_entry_t *entry)
{
    list_insert(lst, -1, entry);
}

static inline
void list_push_head(list_t *lst, list_entry_t *entry)
{
    list_insert(lst, 0, entry);
}

static inline
void list_push_tail(list_t *lst, list_entry_t *entry)
{
    list_insert(lst, -1, entry);
}

CRYSTAL_API
void *list_remove_entry(list_t *lst, list_entry_t *entry);

CRYSTAL_API
void *list_remove(list_t *lst, int index);

static inline
void *list_pop_head(list_t *lst)
{
    return list_remove(lst, 0);
}

static inline
void *list_pop_tail(list_t *lst)
{
    return list_remove(lst, -1);
}

CRYSTAL_API
void *list_get(list_t *lst, int index);

CRYSTAL_API
size_t list_size(list_t *lst);

static inline
int list_is_empty(list_t *lst)
{
    return list_size(lst) == 0;
}

CRYSTAL_API
void list_clear(list_t *lst);

CRYSTAL_API
int list_find(list_t *lst, list_entry_t *entry);

static inline
int list_contains(list_t *lst, list_entry_t *entry)
{
    return list_find(lst, entry) >= 0;
}

static inline
int list_index_of(list_t *lst, list_entry_t *entry)
{
    return list_find(lst, entry);
}

CRYSTAL_API
list_iterator_t *list_iterate(list_t *lst, list_iterator_t *iterator);

// return 1 on success, 0 end of iterator, -1 on modified conflict or error.
CRYSTAL_API
int list_iterator_next(list_iterator_t *iterator, void **data);

CRYSTAL_API
int list_iterator_has_next(list_iterator_t *iterator);

// return 1 on success, 0 nothing removed, -1 on modified conflict or error.
CRYSTAL_API
int list_iterator_remove(list_iterator_t *iterator);

#ifdef __cplusplus
}
#endif

#endif
