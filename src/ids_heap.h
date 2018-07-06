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

#ifndef __IDS_HEAP_H__
#define __IDS_HEAP_H__

#include <pthread.h>
#include <bitset.h>

#include <crystal_config.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ids_heap_t {
    pthread_mutex_t bitset_lock;
    int latest_index;
    int max_index;
    bitset_t bitset_ids;
} ids_heap_t;

#define IDS_HEAP(name, n)               \
    struct IdsHeap_##name {             \
        pthread_mutex_t bitset_lock;    \
        int latest_index;               \
        int max_index;                  \
        BITSET(name, n);                \
    } name

CRYSTAL_API
int ids_heap_init(ids_heap_t *idsheap, int max_index);

CRYSTAL_API
void ids_heap_destroy(ids_heap_t *idsheap);

/*
 * return value:
 * > 0: new bitset id;
 * < 0: errno -- ENOMEM.
 */
CRYSTAL_API
int ids_heap_alloc(ids_heap_t *idsheap);

/*
 * return value:
 * = 0: success;
 * < 0: errno -- EDEADLK.
 */
CRYSTAL_API
int ids_heap_free(ids_heap_t *idsheap, int id);

#ifdef __cplusplus
}
#endif

#endif /* __BITSET_WRAPPER_H__ */
