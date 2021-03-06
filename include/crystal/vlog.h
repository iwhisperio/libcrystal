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

#ifndef __CRYSTAL_VLOG_H__
#define __CRYSTAL_VLOG_H__

#include <crystal/crystal_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VLOG_NONE       0
#define VLOG_FATAL      1
#define VLOG_ERROR      2
#define VLOG_WARN       3
#define VLOG_INFO       4
#define VLOG_DEBUG      5
#define VLOG_TRACE      6
#define VLOG_VERBOSE    7

#include <stdarg.h>

CRYSTAL_API
extern int log_level;

#define vlogF(format, ...) \
    do { \
        if (log_level >= VLOG_FATAL) \
            vlog(VLOG_FATAL, format, ##__VA_ARGS__); \
    } while(0)

#define vlogE(format, ...) \
    do { \
        if (log_level >= VLOG_ERROR) \
            vlog(VLOG_ERROR, format, ##__VA_ARGS__); \
    } while(0)

#define vlogW(format, ...) \
    do { \
        if (log_level >= VLOG_WARN) \
            vlog(VLOG_WARN, format, ##__VA_ARGS__); \
    } while(0)

#define vlogI(format, ...) \
    do { \
        if (log_level >= VLOG_INFO) \
            vlog(VLOG_INFO, format, ##__VA_ARGS__); \
    } while(0)

#define vlogD(format, ...) \
    do { \
        if (log_level >= VLOG_DEBUG) \
            vlog(VLOG_DEBUG, format, ##__VA_ARGS__); \
    } while(0)

#define vlogT(format, ...) \
    do { \
        if (log_level >= VLOG_TRACE) \
            vlog(VLOG_TRACE, format, ##__VA_ARGS__); \
    } while(0)

#define vlogV(format, ...) \
    do { \
        if (log_level >= VLOG_VERBOSE) \
            vlog(VLOG_VERBOSE, format, ##__VA_ARGS__); \
    } while(0)

typedef void log_printer(const char *format, va_list args);

CRYSTAL_API
void vlog_init(int level, const char *logfile, log_printer *printer);

CRYSTAL_API
void vlog_set_level(int level);

CRYSTAL_API
void vlog(int level, const char *format, ...);

CRYSTAL_API
void vlogv(int level, const char *format, va_list args);

CRYSTAL_API
void vlog_long(int level, const char *format, ...);

CRYSTAL_API
void vlogv_long(int level, const char *format, va_list args);

#ifdef __cplusplus
}
#endif

#endif /*__CRYSTAL_VLOG_H__*/
