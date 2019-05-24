#ifndef __CRYSTAL_POSIX_HELPER__
#define __CRYSTAL_POSIX_HELPER__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <direct.h>
#include <stdio.h>

#include <winsock2.h>
#include <windows.h>
#include <crystal/gettimeofday.h>

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

#define F_OK            ((int)00)
#define W_OK            ((int)02)
#define R_OK            ((int)04)
#define X_OK            R_OK

#define PATH_MAX       MAX_PATH
#define MAXPATHLEN     MAX_PATH

#define mkdir(dir, mode)        _mkdir(dir)

#define fsync(fd)               _commit(fd)

#define fdopen(fd, mode)        _fdopen(fd, mode)

#undef strcasecmp
#define strcasecmp(s1, s2)      _stricmp(s1, s2)

#undef strncasecmp
#define strncasecmp(s1, s2, n)  _strnicmp(s1, s2, n)

#define usleep(s)               Sleep((DWORD)(((s) + 500) / 1000))
#define sleep(s)                Sleep((DWORD)((s) * 1000))

#define timerisset(tvp)        ((tvp)->tv_sec || (tvp)->tv_usec)

#define timeradd(a, b, result)                                                \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;                          \
    if ((result)->tv_usec >= 1000000)                                         \
      {                                                                       \
        ++(result)->tv_sec;                                                   \
        (result)->tv_usec -= 1000000;                                         \
      }                                                                       \
  } while (0)

#define timersub(a, b, result)                                                \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                          \
    if ((result)->tv_usec < 0) {                                              \
      --(result)->tv_sec;                                                     \
      (result)->tv_usec += 1000000;                                           \
    }                                                                         \
  } while (0)

static __inline
char *realpath(const char *path, char *resolved_path)
{
   return _fullpath(resolved_path, path, _MAX_PATH);
}

static __inline
char *basename(const char *path)
{
    static char base[_MAX_FNAME + _MAX_EXT + 1];
    char fpath[_MAX_DIR + _MAX_FNAME + _MAX_EXT + 1];
    char fname[_MAX_FNAME + 1];
    char ext[_MAX_EXT + 1];
    errno_t err;
    int rc;
    size_t len;

    if(!path)
      return ".";

    len = strlen(path);
    if (len >= sizeof(fpath))
      return NULL;

    memcpy(fpath, path, len+1);
    if ( len > 1 && ( path[len-1] == '/' || path[len-1] == '\\') )
      fpath[len-1] = '\0';

    err = _splitpath_s(fpath, NULL, 0, NULL, 0,
                       fname, sizeof(fname), ext, sizeof(ext));
    if (err)
        return NULL;

    rc = snprintf(base, sizeof(base), "%s%s", fname, ext);
    return rc < 0 || rc >= sizeof(base) ? NULL : base;
}

static __inline
char *dirname(const char *path)
{
    static char dir[_MAX_DIR + 1];
    char fpath[_MAX_DIR + _MAX_FNAME + _MAX_EXT + 1];
    size_t len;
    errno_t rc;

    if(!path)
      return ".";

    len = strlen(path);
    if (len >= sizeof(fpath))
      return NULL;

    memcpy(fpath, path, len+1);
    if ( len > 1 && ( path[len-1] == '/' || path[len-1] == '\\') )
      fpath[len-1] = '\0';

    rc = _splitpath_s(fpath, NULL, 0, dir, sizeof(dir),
                      NULL, 0, NULL, 0);
    return rc ? NULL : dir;
}

#ifdef __cplusplus
}
#endif

#endif /* __CRYSTAL_POSIX_HELPER__ */
