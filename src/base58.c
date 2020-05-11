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

#include "BRBase58.h"

#include "crystal/base58.h"

char *base58_encode(const void *data, size_t len, char *text, size_t *textlen)
{
    size_t rc;

    if (!text || !textlen || *textlen == 0 || !data || len == 0)
        return NULL;

    rc = BRBase58Encode(text, *textlen,  (const uint8_t *)data, len);
    if (rc == 0)
        return NULL;

    *textlen = rc;
    return text;
}

ssize_t base58_decode(const char *text, size_t textlen, void *data, size_t datalen)
{
    size_t rc;

    (void)textlen;

    if (!text || !data || datalen == 0)
        return -1;

    rc = BRBase58Decode(data, datalen, text);
    return rc == 0 ? -1: (ssize_t)rc;
}
