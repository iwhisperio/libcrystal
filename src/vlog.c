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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>
#include <stddef.h>

#include "crystal/vlog.h"
#include "crystal/posix_helper.h"

int log_level = VLOG_INFO;

#if defined(__ANDROID__)
#include <android/log.h>
#define LOG_TAG "droid"

static int android_log_levels[] = {
    ANDROID_LOG_SILENT,     // VLOG_NONE
    ANDROID_LOG_FATAL,      // VLOG_FATAL
    ANDROID_LOG_ERROR,      // VLOG_ERROR
    ANDROID_LOG_WARN,       // VLOG_WARN
    ANDROID_LOG_INFO,       // VLOG_INFO
    ANDROID_LOG_DEBUG,      // VLOG_DEBUG
    ANDROID_LOG_VERBOSE,    // VLOG_TRACE
    ANDROID_LOG_VERBOSE     // VLOG_VERBOSE
};

void vlog_init(int level, const char *logfile, log_printer *printer) {}
void vlog_set_level(int level) {}

void vlog(int level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    __android_log_vprint(android_log_levels[level], LOG_TAG, format, args);
    va_end(args);
}

void vlogv(int level, const char *format, va_list args)
{
    __android_log_vprint(android_log_levels[level], LOG_TAG, format, args);
}

#else /* defined(__ANDROID__) */

#define TIME_FORMAT     "%Y-%m-%d %H:%M:%S"

static const char* level_names[] = {
    "-",
    "FATAL",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "TRACE",
    "VERBOSE"
};

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <Windows.h>

static HANDLE hConsole = NULL;

static void set_color(int level)
{
    WORD attr;

    if (!hConsole)
        hConsole = GetStdHandle(STD_ERROR_HANDLE);

    if (level <= VLOG_ERROR)
        attr = FOREGROUND_RED;
    else if (level == VLOG_WARN)
        attr = FOREGROUND_RED | FOREGROUND_GREEN;
    else if (level == VLOG_INFO)
        attr = FOREGROUND_GREEN;
    else if (level >= VLOG_DEBUG)
        attr = FOREGROUND_INTENSITY;

    SetConsoleTextAttribute(hConsole, attr);
}

static void reset_color()
{
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE
                            | FOREGROUND_GREEN | FOREGROUND_RED);
}

#else

#if defined(__APPLE__)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

#endif /* __APPLE__ */

static void set_color(int level)
{
    const char *color = "";

    if (level <= VLOG_ERROR)
        color = "\e[0;31m";
    else if (level == VLOG_WARN)
        color = "\e[0;33m";
    else if (level == VLOG_INFO)
        color = "\e[0;32m";
    else if (level >= VLOG_DEBUG)
        color = "\e[1;30m";

    fprintf(stderr, "%s", color);
}

static void reset_color(void)
{
    fprintf(stderr, "\e[0m");
}

#if defined(__APPLE__)

#include <TargetConditionals.h>

#if defined(TARGET_OS_IOS)

#define set_color(c)  ((void)0)
#define reset_color() ((void)0)

#endif /* TARGET_OS_IOS */

#pragma GCC diagnostic pop

#endif /* __APPLE__ */

#endif /* defined(_WIN32) || defined(_WIN64) */

static log_printer *__printer;
static FILE *__logfile;
static int __exit_hook_registered;

static void close_log_file(void)
{
    if (__logfile) {
        fclose(__logfile);
        __logfile = NULL;
    }
}

void vlog_init(int level, const char *logfile, log_printer *printer)
{
    vlog_set_level(level);
    __printer = printer;

    if (logfile && *logfile) {
        if (__logfile)
            fclose(__logfile);

        __logfile = fopen(logfile, "a");
    }

    if (!__exit_hook_registered) {
        __exit_hook_registered = 1;
        atexit(close_log_file);
    }
}

void output(int level, const char *format, ...)
{
    va_list args;

    pthread_mutex_lock(&lock);

    va_start(args, format);
    if (__printer)
        __printer(format, args);
    else {
        //set_color(level);
        vfprintf(stderr, format, args);
        //reset_color();
    }
    va_end(args);

    if (__logfile) {
        va_start(args, format);
        vfprintf(__logfile, format, args);
        va_end(args);
        fflush(__logfile);
    }

    pthread_mutex_unlock(&lock);

}

void vlogv(int level, const char *format, va_list args)
{
    char timestr[20];
    char buf[1024];
    time_t cur = time(NULL);

    if (level > log_level)
        return;

    if (level > VLOG_VERBOSE)
        level = VLOG_VERBOSE;

    strftime(timestr, 20, TIME_FORMAT, localtime(&cur));
    vsnprintf(buf, sizeof(buf), format, args);
    buf[1023] = 0;

    output(level, "%s - %-7s : %s\n",
            timestr, level_names[level], buf);
}

void vlog(int level, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vlogv(level, format, args);
    va_end(args);
}

void vlogv_long(int level, const char *format, va_list args)
{
    char timestr[20];
    char *buf;
    time_t cur = time(NULL);

    va_list _args;
    int _len;

    if (level > log_level)
        return;

    if (level > VLOG_VERBOSE)
        level = VLOG_VERBOSE;

    va_copy(_args, args);
    _len = vsnprintf(NULL, 0, format, _args) + 1;
    va_end(_args);

    if (_len <= 0)
        return;

    if (_len > 1024)
        buf = (char *)calloc(1, _len);
    else
        buf = (char *)alloca(_len);

    if (!buf)
        return;

    strftime(timestr, 20, TIME_FORMAT, localtime(&cur));
    vsnprintf(buf, _len, format, args);

    output(level, "%s - %-7s : %s\n",
            timestr, level_names[level], buf);

    if (_len > 1024)
        free(buf);
}

void vlog_long(int level, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vlogv_long(level, format, args);
    va_end(args);
}

void vlog_set_level(int level)
{
    if (level > VLOG_VERBOSE)
        level = VLOG_VERBOSE;
    else if (level < VLOG_NONE)
        level = VLOG_NONE;

    log_level = level;
}

#endif  /* defined(__ANDROID__) */
