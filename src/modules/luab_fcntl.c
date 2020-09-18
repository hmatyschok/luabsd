/*
 * Copyright (c) 2020 Henning Matyschok <hmatyschok@outlook.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t flock_type;

#define LUABSD_FCNTL_LIB_ID    1593623310
#define LUABSD_FCNTL_LIB_KEY    "fcntl"

extern luab_module_t luab_fcntl_lib;

/*
 * Service primitives.
 */

/***
 * open(2) - open or create for reading, writing or executing
 *
 * @function open
 *
 * @param path              The specified file name.
 * @param flags             Values are constructed over
 *
 *                              bsd.fcntl.O_*
 *
 *                          by bitwise-inclusive OR.
 * @param mode              Values are constructed over
 *
 *                              bsd.sys.stat.S_*
 *
 *                          by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (fd [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage fd [, err, msg ] = bsd.fcntl.open(path, flags [, mode ])
 */
static int
luab_open(lua_State *L)
{
    int narg;
    const char *path;
    int flags;
    mode_t mode;
    int fd;

    narg = luab_checkmaxargs(L, 3);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    flags = (int)luab_checkinteger(L, 2, INT_MAX);
    mode = 0;

    if (narg == 3 && (flags & O_CREAT) != 0)
        mode = luab_checkinteger(L, narg, ALLPERMS);

    fd = open(path, flags, mode);

    return (luab_pusherr(L, fd));
}

/***
 * creat(2) - create a new file
 *
 * @function creat
 *
 * @param path              The specified file name.
 * @param mode              Values are constructed over
 *
 *                              bsd.sys.stat.S_*
 *
 *                          by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (fd [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage fd [, err, msg ] = bsd.fcntl.creat(path, mode)
 */
static int
luab_creat(lua_State *L)
{
    const char *path;
    mode_t mode;
    int fd;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    mode = (mode_t)luab_checkinteger(L, 2, ALLPERMS);

    fd = creat(path, mode);

    return (luab_pusherr(L, fd));
}

/***
 * fcntl(2) - file control
 *
 * @function fcntl
 *
 * @param fd                The specified descriptor.
 * @param cmd               Command operated on fd over
 *
 *                              bsd.fcntl.F_*,
 *
 *                          as described in fcntl(2).
 * @param arg               Optional, depends on applied cmd.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (value [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage value [, err, msg ] = bsd.fcntl.fcntl(fd, cmd [, arg ])
 */
static int
luab_fcntl(lua_State *L)
{
    int narg;
    int fd, cmd, arg, status;
    struct flock *argp;

    narg = luab_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    cmd = (int)luab_checkinteger(L, 2, INT_MAX);
    arg = 0;
    argp = NULL;

    if (narg == 3) {
        if (lua_type(L, narg) == LUA_TUSERDATA)
            argp = luab_udata(L, narg, &flock_type, struct flock *);
        else
            arg = luab_checkinteger(L, narg, INT_MAX);
    }

    if (argp != NULL)
        status = fcntl(fd, cmd, argp);
    else
        status = fcntl(fd, cmd, arg);

    return (luab_pusherr(L, status));
}

#if __BSD_VISIBLE
/***
 * flock(2) - apply or remove an advisory lock on an open file
 *
 * @function flock
 *
 * @param fd                The specified descriptor.
 * @param operation         Specified by
 *
 *                              bsd.sys.file.LOCK_*,
 *
 *                          over <sys/file.h>.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (value [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage value [, err, msg ] = bsd.fcntl.flock(fd, operation)
 */
static int
luab_flock(lua_State *L)
{
    int fd, operation, status;

    (void)luab_checkmaxargs(L, 2);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    operation = (int)luab_checkinteger(L, 2, INT_MAX);

    status = flock(fd, operation);

    return (luab_pusherr(L, status));
}
#endif

#if __POSIX_VISIBLE >= 200809
static int
luab_openat(lua_State *L)
{
    int narg, dirfd;
    const char *path;
    int flags;
    mode_t mode;
    int fd;

    narg = luab_checkmaxargs(L, 4);

    dirfd = (int)luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    flags = (int)luab_checkinteger(L, 3, INT_MAX);
    mode = 0;

    if (narg == 4 && (flags & O_CREAT) != 0)
        mode = luab_checkinteger(L, narg, ALLPERMS);

    fd = openat(dirfd, path, flags, mode);

    return (luab_pusherr(L, fd));
}
#endif

#if __POSIX_VISIBLE >= 200112
static int
luab_posix_fadvise(lua_State *L)
{
    int fd;
    off_t offset;
    off_t len;
    int advice;
    int status;

    (void)luab_checkmaxargs(L, 4);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    offset = (off_t)luab_checkinteger(L, 2, INT_MAX);   /* XXX */
    len = (off_t)luab_checkinteger(L, 3, INT_MAX);
    advice = (int)luab_checkinteger(L, 4, INT_MAX);

    if ((errno = posix_fadvise(fd, offset, len, advice)) != 0)
        status = -1;
    else
        status = 0;

    return (luab_pusherr(L, status));
}

static int
luab_posix_fallocate(lua_State *L)
{
    int fd;
    off_t offset;
    off_t len;
    int status;

    (void)luab_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    offset = (off_t)luab_checkinteger(L, 2, INT_MAX);  /* XXX */
    len = (off_t)luab_checkinteger(L, 3, INT_MAX);

    if ((errno = posix_fallocate(fd, offset, len)) != 0)
        status = -1;
    else
        status = 0;

    return (luab_pusherr(L, status));
}
#endif

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(FLOCK)).
 *
 * @function flock_create
 *
 * @param data          Instance of (LUA_TUSERDATA(FLOCK)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (flock [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage flock [, err, msg ] = bsd.fcntl.flock_create([ data ])
 */
static int
luab_flock_create(lua_State *L)
{
    return (luab_create(L, 1, &flock_type, &flock_type));
}

/*
 * Interface against <fcntl.h>.
 */

static luab_table_t luab_fcntl_vec[] = {
    LUABSD_INT("O_RDONLY", O_RDONLY),
    LUABSD_INT("O_WRONLY", O_WRONLY),
    LUABSD_INT("O_RDWR",   O_RDWR),
    LUABSD_INT("O_ACCMODE",    O_ACCMODE),
#if __BSD_VISIBLE
    LUABSD_INT("FREAD",    FREAD),
    LUABSD_INT("FWRITE",   FWRITE),
#endif
    LUABSD_INT("O_NONBLOCK",   O_NONBLOCK),
    LUABSD_INT("O_APPEND", O_APPEND),
#if __BSD_VISIBLE
    LUABSD_INT("O_SHLOCK", O_SHLOCK),
    LUABSD_INT("O_EXLOCK", O_EXLOCK),
    LUABSD_INT("O_ASYNC",  O_ASYNC),
    LUABSD_INT("O_FSYNC",  O_FSYNC),
#endif
    LUABSD_INT("O_SYNC",   O_SYNC),
#if __POSIX_VISIBLE >= 200809
    LUABSD_INT("O_NOFOLLOW",   O_NOFOLLOW),
#endif
    LUABSD_INT("O_CREAT",  O_CREAT),
    LUABSD_INT("O_TRUNC",  O_TRUNC),
    LUABSD_INT("O_EXCL",   O_EXCL),
    LUABSD_INT("O_NOCTTY", O_NOCTTY),
#if __BSD_VISIBLE
    LUABSD_INT("O_DIRECT", O_DIRECT),
#endif
#if __POSIX_VISIBLE >= 200809
    LUABSD_INT("O_DIRECTORY",  O_DIRECTORY),
    LUABSD_INT("O_EXEC",   O_EXEC),
#endif
#if __POSIX_VISIBLE >= 200809
    LUABSD_INT("O_TTY_INIT",   O_TTY_INIT),
    LUABSD_INT("O_CLOEXEC",    O_CLOEXEC),
#endif
#if __BSD_VISIBLE
    LUABSD_INT("O_VERIFY", O_VERIFY),
    LUABSD_INT("FAPPEND",  FAPPEND),
    LUABSD_INT("FASYNC",   FASYNC),
    LUABSD_INT("FFSYNC",   FFSYNC),
    LUABSD_INT("FNONBLOCK",    FNONBLOCK),
    LUABSD_INT("FNDELAY",  FNDELAY),
    LUABSD_INT("O_NDELAY", O_NDELAY),
    LUABSD_INT("FRDAHEAD", FRDAHEAD),
#endif
#if __POSIX_VISIBLE >= 200809
    LUABSD_INT("AT_FDCWD", AT_FDCWD),
    LUABSD_INT("AT_EACCESS",   AT_EACCESS),
    LUABSD_INT("AT_SYMLINK_NOFOLLOW",  AT_SYMLINK_NOFOLLOW),
    LUABSD_INT("AT_SYMLINK_FOLLOW",    AT_SYMLINK_FOLLOW),
    LUABSD_INT("AT_REMOVEDIR", AT_REMOVEDIR),
#endif
    LUABSD_INT("F_DUPFD",  F_DUPFD),
    LUABSD_INT("F_GETFD",  F_GETFD),
    LUABSD_INT("F_SETFD",  F_SETFD),
    LUABSD_INT("F_GETFL",  F_GETFL),
    LUABSD_INT("F_SETFL",  F_SETFL),
#if __XSI_VISIBLE || __POSIX_VISIBLE >= 200112
    LUABSD_INT("F_GETOWN", F_GETOWN),
    LUABSD_INT("F_SETOWN", F_SETOWN),
#endif
#if __BSD_VISIBLE
    LUABSD_INT("F_OGETLK", F_OGETLK),
    LUABSD_INT("F_OSETLK", F_OSETLK),
    LUABSD_INT("F_OSETLKW",    F_OSETLKW),
    LUABSD_INT("F_DUP2FD", F_DUP2FD),
#endif
    LUABSD_INT("F_GETLK",  F_GETLK),
    LUABSD_INT("F_SETLK",  F_SETLK),
    LUABSD_INT("F_SETLKW", F_SETLKW),
#if __BSD_VISIBLE
    LUABSD_INT("F_SETLK_REMOTE",   F_SETLK_REMOTE),
    LUABSD_INT("F_READAHEAD",  F_READAHEAD),
    LUABSD_INT("F_RDAHEAD",    F_RDAHEAD),
#endif
#if __POSIX_VISIBLE >= 200809
    LUABSD_INT("F_DUPFD_CLOEXEC",  F_DUPFD_CLOEXEC),
#endif
#if __BSD_VISIBLE
    LUABSD_INT("F_DUP2FD_CLOEXEC", F_DUP2FD_CLOEXEC),
#endif
    LUABSD_INT("FD_CLOEXEC",   FD_CLOEXEC),
    LUABSD_INT("F_RDLCK",  F_RDLCK),
    LUABSD_INT("F_UNLCK",  F_UNLCK),
    LUABSD_INT("F_WRLCK",  F_WRLCK),
#if __BSD_VISIBLE
    LUABSD_INT("F_UNLCKSYS",   F_UNLCKSYS),
    LUABSD_INT("F_CANCEL", F_CANCEL),
    LUABSD_INT("LOCK_SH",  LOCK_SH),
    LUABSD_INT("LOCK_EX",  LOCK_EX),
    LUABSD_INT("LOCK_NB",  LOCK_NB),
    LUABSD_INT("LOCK_UN",  LOCK_UN),
#endif
#if __POSIX_VISIBLE >= 200112
    LUABSD_INT("POSIX_FADV_NORMAL",    POSIX_FADV_NORMAL),
    LUABSD_INT("POSIX_FADV_RANDOM",    POSIX_FADV_RANDOM),
    LUABSD_INT("POSIX_FADV_SEQUENTIAL",    POSIX_FADV_SEQUENTIAL),
    LUABSD_INT("POSIX_FADV_WILLNEED",  POSIX_FADV_WILLNEED),
    LUABSD_INT("POSIX_FADV_DONTNEED",  POSIX_FADV_DONTNEED),
    LUABSD_INT("POSIX_FADV_NOREUSE",   POSIX_FADV_NOREUSE),
#endif
    LUABSD_FUNC("open", luab_open),
    LUABSD_FUNC("creat",    luab_creat),
    LUABSD_FUNC("fcntl",   luab_fcntl),
#if __BSD_VISIBLE
    LUABSD_FUNC("flock",    luab_flock),
#endif
#if __POSIX_VISIBLE >= 200809
    LUABSD_FUNC("openat",   luab_openat),
#endif
#if __POSIX_VISIBLE >= 200112
    LUABSD_FUNC("posix_fadvise",    luab_posix_fadvise),
    LUABSD_FUNC("posix_fallocate",  luab_posix_fallocate),
#endif
    LUABSD_FUNC("flock_create", luab_flock_create),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_fcntl_lib = {
    .cookie = LUABSD_FCNTL_LIB_ID,
    .name = LUABSD_FCNTL_LIB_KEY,
    .vec = luab_fcntl_vec,
};
