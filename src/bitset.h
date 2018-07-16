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

#ifndef __BITSET_H__
#define __BITSET_H__

#include <string.h>
#include <stdint.h>
#include <assert.h>

#include <crystal_config.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _bitset_t {
    size_t size;
    uint64_t bits[1];
} bitset_t;

#define BITSET(name, n)                     \
    struct bitset_##name {                  \
        size_t size;                        \
        uint64_t bits[((n) + 63) >> 6];     \
    } name

static inline void bitset_reset(bitset_t *set)
{
    assert(set);

    memset(set->bits, 0, ((set->size + 63) >> 6) << 3);
}

static inline void bitset_init(bitset_t *set, size_t size)
{
    assert(set);

    set->size = size;
    bitset_reset(set);
}

static inline size_t bitset_size(bitset_t *set)
{
    assert(set);
    return set->size;
}

static inline int bitset_set(bitset_t *set, int bit)
{
    int index, offset;

    assert(set);

    if (bit < 0 || bit >= set->size)
        return -1;

    index = bit >> 6;
    offset = bit % 64;

    set->bits[index] |= ((uint64_t)1 << offset);
    return 0;
}

static inline int bitset_clear(bitset_t *set, int bit)
{
    int index, offset;

    assert(set);

    if (bit < 0 || bit >= set->size)
        return -1;

    index = bit >> 6;
    offset = bit % 64;

    set->bits[index] &= ~((uint64_t)1 << offset);
    return 0;
}

static inline int bitset_isset(bitset_t *set, int bit)
{
    int index, offset;

    assert(set);

    if (bit < 0 || bit >= set->size)
        return -1;

    index = bit >> 6;
    offset = bit % 64;

    return (set->bits[index] & ((uint64_t)1 << offset)) != 0;
}

static inline int bitset_isset2(const uint64_t *bits, int bit)
{
    int index, offset;

    assert(bits);

    if (bit < 0)
        return -1;

    index = bit >> 6;
    offset = bit % 64;

    return (bits[index] & ((uint64_t)1 << offset)) != 0;
}

static inline int bitset_compare(bitset_t *set1, bitset_t *set2, size_t len)
{
    assert(set1 && set2);
    assert(len <= set1->size && len <= set2->size);

    return memcmp(set1->bits, set2->bits, (len + 63) >> 6);
}

static inline int bitset_compare2(bitset_t *set, const uint64_t *bits, size_t len)
{
    assert(set && bits);
    assert(len <= set->size);

    if (len == 0)
        len = set->size;

    return memcmp(set->bits, bits, (len + 63) >> 6);
}

CRYSTAL_API
int bitset_prev_set_bit(bitset_t *set, int from);

CRYSTAL_API
int bitset_next_set_bit(bitset_t *set, int from);

CRYSTAL_API
int bitset_prev_clear_bit(bitset_t *set, int from);

CRYSTAL_API
int bitset_next_clear_bit(bitset_t *set, int from);

#ifdef __cplusplus
}
#endif

#endif /* __BITSET_H__ */
