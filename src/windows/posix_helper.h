#ifndef __POSIX_HELPER__
#define __POSIX_HELPER__

#include <stdint.h>
#include <windows.h>
#include <gettimeofday.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t        mode_t;

typedef long int        suseconds_t;

#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)
#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)

#define S_IRWXU         _S_IREAD | _S_IWRITE
#define S_IRUSR         _S_IREAD
#define S_IWUSR         _S_IWRITE

#define PATH_MAX       MAX_PATH

#define mkdir(dir, mode)        _mkdir(dir)
#define strcasecmp(s1, s2)      _stricmp(s1, s2)
#define strncasecmp(s1, s2, n)  _strnicmp(s1, s2, n)

#define usleep(s)               Sleep((DWORD)(((s) + 500) / 1000))
#define sleep(s)                Sleep((DWORD)((s) * 1000))

# define timerisset(tvp)        ((tvp)->tv_sec || (tvp)->tv_usec)
# define timerclear(tvp)        ((tvp)->tv_sec = (tvp)->tv_usec = 0)
# define timercmp(a, b, CMP)                                                  \
  (((a)->tv_sec == (b)->tv_sec) ?                                             \
   ((a)->tv_usec CMP (b)->tv_usec) :                                          \
   ((a)->tv_sec CMP (b)->tv_sec))
# define timeradd(a, b, result)                                               \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;                          \
    if ((result)->tv_usec >= 1000000)                                         \
      {                                                                       \
        ++(result)->tv_sec;                                                   \
        (result)->tv_usec -= 1000000;                                         \
      }                                                                       \
  } while (0)
# define timersub(a, b, result)                                               \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                          \
    if ((result)->tv_usec < 0) {                                              \
      --(result)->tv_sec;                                                     \
      (result)->tv_usec += 1000000;                                           \
    }                                                                         \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif /* __POSIX_HELPER__ */
