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

#ifndef __CRYSTAL_CONFIG_H__
#define __CRYSTAL_CONFIG_H__

#if defined(CRYSTAL_STATIC)
  #define CRYSTAL_API
#elif defined(CRYSTAL_DYNAMIC)
  #ifdef CRYSTAL_BUILD
    #if defined(_WIN32) || defined(_WIN64)
      #define CRYSTAL_API        __declspec(dllexport)
    #else
      #define CRYSTAL_API        __attribute__((visibility("default")))
    #endif
  #else
    #if defined(_WIN32) || defined(_WIN64)
      #define CRYSTAL_API        __declspec(dllimport)
    #else
      #define CRYSTAL_API
    #endif
  #endif
#else
  #define CRYSTAL_API
#endif

#ifdef _MSC_VER
#define inline   __inline
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <crtdefs.h>

typedef ptrdiff_t       ssize_t;
#endif

#ifndef static_assert
#define static_assert(exp, str)
#endif

#endif /* __CRYSTAL_CONFIG_H__ */
