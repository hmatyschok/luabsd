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
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t timespec_type;
extern luab_module_t stat_type;

#define LUABSD_SYS_STAT_LIB_ID    1593623310
#define LUABSD_SYS_STAT_LIB_KEY    "stat"

extern luab_module_t luab_sys_stat_lib;

/*
 * Subr.
 *
 * Translate an instance of
 *
 *  (LUA_TTABLE(LUA_TNUMBER,LUA_TUSERDATA(TIMESPEC)))
 *
 * into an array of timespec{} items.
 */
static struct timespec *
luab_checkltimesvector(lua_State *L, int narg, size_t len)
{
    struct timespec *vec, *v;
    int k;

    vec = luab_newlvector(L, narg, len, sizeof(struct timespec));

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isuserdata(L, -1) != 0)) {
            v = luab_udata(L, -1, timespec_type, struct timespec *);
            (void)memmove(&vec[k], v, sizeof(struct timespec));
        } else {
            free(vec);
            luaL_argerror(L, narg, "Invalid argument");
        }
        lua_pop(L, 1);
    }
    return (vec);
}

static void
luab_pushltimesvector(lua_State *L, int narg, size_t len, void *arg)
{
    struct timespec *vec, *v;
    int k;

    (void)luab_checkltable(L, narg, len);

    vec = (struct timespec *)arg;

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isuserdata(L, -1) != 0)) {
            v = luab_udata(L, -1, timespec_type, struct timespec *);
            (void)memmove(v, &vec[k], sizeof(struct timespec));
        } else {
            free(vec);
            luaL_argerror(L, narg, "Invalid argument");
        }
        lua_pop(L, 1);
    }
    free(vec);
}

/*
 * Service primitives.
 */

#if __BSD_VISIBLE
/***
 * chflags(2) - set file flags
 *
 * @function chflags
 *
 * @param path              File pointed to by path.
 * @param flags             File flags
 *
 *                             #1 bsd.sys.stat.SF_{APPEND,ARCHIVED,IMMUTABLE,
 *                                  NOUNLINK,SNAPSHOT}
 *
 *                             #2 bsd.sys.stat.UF_{APPEND,ARCHIVE,HIDDEN,
 *                                  IMMUTABLE,NODUMP,NOUNLINK,OFFLINE,
 *                                  OPAQUE,READONLY,REPARSE,SPARSE,SYSTEM}
 *
 *                          are formatted by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.chflags(path, flags)
 */
static int
luab_chflags(lua_State *L)
{
    const char *path;
    u_long flags;
    int status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    flags = (u_long)luab_checkinteger(L, 2, ULONG_MAX);

    status = chflags(path, flags);

    return (luab_pusherr(L, status));
}

/***
 * chflagsat(2) - set file flags
 *
 * @function chflagsat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes referenced file.
 *
 *                            #2 By path named file is relative to the directory
 *                               associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of chflagsat(2).
 *
 * @param path              File named by path.
 * @param flags             File flags
 *
 *                             #1 bsd.sys.stat.SF_{APPEND,ARCHIVED,IMMUTABLE,
 *                                  NOUNLINK,SNAPSHOT}
 *
 *                             #2 bsd.sys.stat.UF_{APPEND,ARCHIVE,HIDDEN,
 *                                  IMMUTABLE,NODUMP,NOUNLINK,OFFLINE,
 *                                  OPAQUE,READONLY,REPARSE,SPARSE,SYSTEM}
 *
 *                          are formatted by inclusive or.
 * @param atflags           Passed values are formatted by inclusive or:
 *
 *                              bsd.fcntl.AT_SYMLINK_NOFOLLOW.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.chflagsat(fd, path, flags, atflags)
 */
static int
luab_chflagsat(lua_State *L)
{
    int fd;
    const char *path;
    u_long flags;
    int atflag;
    int status;

    (void)luab_checkmaxargs(L, 4);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    flags = (u_long)luab_checkinteger(L, 3, ULONG_MAX);
    atflag = (int)luab_checkinteger(L, 4, INT_MAX);

    status = chflagsat(fd, path, flags, atflag);

    return (luab_pusherr(L, status));
}
#endif

/***
 * chmod(2) - change mode of file
 *
 * @function chmod
 *
 * @param path              File named by path.
 * @param mode              Permission bit masks
 *
 *                              bsd.sys.stat.S_{
 *                                  IRWXU,          -- RWX mask for owner
 *                                  IRUSR,          -- R for owner
 *                                  IWUSR,          -- W for owner
 *                                  IXUSR,          -- X for owner
 *                                  IRWXG,          -- RWX mask for group
 *                                  IRGRP,          -- R for group
 *                                  IWGRP,          -- W for group
 *                                  IXGRP,          -- X for group
 *                                  IRWXO,          -- RWX mask for other
 *                                  IROTH,          -- R for other
 *                                  IWOTH,          -- W for other
 *                                  IXOTH,          -- X for other
 *                                  ISUID,          -- set user id on
 *                                  ISGID,          -- set group id on
 *                                  ISVTX           -- sticky bit
 *                              }
 *
 *                          are formatted by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.chmod(path, mode)
 */
static int
luab_chmod(lua_State *L)
{
    const char *path;
    mode_t mode;
    int status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    mode = (mode_t)luab_checkinteger(L, 2, ALLPERMS);

    status = chmod(path, mode);

    return (luab_pusherr(L, status));
}

#if __BSD_VISIBLE
/***
 * fchflags(2) - set file flags
 *
 * @function fchflags
 *
 * @param fd                Filedescriptor.
 * @param flags             File flags
 *
 *                             #1 bsd.sys.stat.SF_{APPEND,ARCHIVED,IMMUTABLE,
 *                                  NOUNLINK,SNAPSHOT}
 *
 *                             #2 bsd.sys.stat.UF_{APPEND,ARCHIVE,HIDDEN,
 *                                  IMMUTABLE,NODUMP,NOUNLINK,OFFLINE,
 *                                  OPAQUE,READONLY,REPARSE,SPARSE,SYSTEM}
 *
 *                          are formatted by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.fchflags(path, flags)
 */
static int
luab_fchflags(lua_State *L)
{
    int fd;
    u_long flags;
    int status;

    (void)luab_checkmaxargs(L, 2);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    flags = (u_long)luab_checkinteger(L, 2, ULONG_MAX);

    status = fchflags(fd, flags);

    return (luab_pusherr(L, status));
}
#endif
#if __POSIX_VISIBLE >= 200112
/***
 * fchmod(2) - change mode of file
 *
 * @function fchmod
 *
 * @param fd                File named by path.
 * @param mode              Permission bit masks
 *
 *                              bsd.sys.stat.S_{
 *                                  IRWXU,          -- RWX mask for owner
 *                                  IRUSR,          -- R for owner
 *                                  IWUSR,          -- W for owner
 *                                  IXUSR,          -- X for owner
 *                                  IRWXG,          -- RWX mask for group
 *                                  IRGRP,          -- R for group
 *                                  IWGRP,          -- W for group
 *                                  IXGRP,          -- X for group
 *                                  IRWXO,          -- RWX mask for other
 *                                  IROTH,          -- R for other
 *                                  IWOTH,          -- W for other
 *                                  IXOTH,          -- X for other
 *                                  ISUID,          -- set user id on
 *                                  ISGID,          -- set group id on
 *                                  ISVTX           -- sticky bit
 *                              }
 *
 *                          are formatted by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.fchmod(fd, mode)
 */
static int
luab_fchmod(lua_State *L)
{
    int fd;
    mode_t mode;
    int status;

    (void)luab_checkmaxargs(L, 2);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    mode = (mode_t)luab_checkinteger(L, 2, ALLPERMS);

    status = fchmod(fd, mode);

    return (luab_pusherr(L, status));
}
#endif
#if __POSIX_VISIBLE >= 200809
/***
 * fchmodat(2) - change mode of file
 *
 * @function fchmodat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes newly created directory.
 *
 *                            #2 The newly created directory is relative to the
 *                               directory associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of fchmodat(2).
 *
 * @param mode              Permission bit masks
 *
 *                              bsd.sys.stat.S_{
 *                                  IRWXU,          -- RWX mask for owner
 *                                  IRUSR,          -- R for owner
 *                                  IWUSR,          -- W for owner
 *                                  IXUSR,          -- X for owner
 *                                  IRWXG,          -- RWX mask for group
 *                                  IRGRP,          -- R for group
 *                                  IWGRP,          -- W for group
 *                                  IXGRP,          -- X for group
 *                                  IRWXO,          -- RWX mask for other
 *                                  IROTH,          -- R for other
 *                                  IWOTH,          -- W for other
 *                                  IXOTH,          -- X for other
 *                                  ISUID,          -- set user id on
 *                                  ISGID,          -- set group id on
 *                                  ISVTX           -- sticky bit
 *                              }
 *
 *                          are formatted by inclusive or.
 *
 * @param flag              Passed values are formatted by inclusive or:
 *
 *                              bsd.fcntl.AT_SYMLINK_NOFOLLOW.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.fchmodat(fd, path, mode, flag)
 */
static int
luab_fchmodat(lua_State *L)
{
    int fd;
    const char *path;
    mode_t mode;
    int flag;
    int status;

    (void)luab_checkmaxargs(L, 4);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    mode = (mode_t)luab_checkinteger(L, 3, ALLPERMS);
    flag = (int)luab_checkinteger(L, 4, INT_MAX);

    status = fchmodat(fd, path, mode, flag);

    return (luab_pusherr(L, status));
}

/***
 * futimens(2) - set file access and modifications times
 *
 * @function futimens
 *
 * @param fd                Filedescriptor.
 * @param times             An instance of (LUA_TTABLE) carrying at least
 *
 *                              {
 *                                  (LUA_TUSERDATA(TIMESPEC)),
 *                                  (LUA_TUSERDATA(TIMESPEC))
 *                              }
 *
 *                          two instances of (LUA_TUSERDATA(TIMESPEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.futimens(fd, times)
 */
static int
luab_futimens(lua_State *L)
{
    int fd;
    struct timespec *times;
    int status;

    (void)luab_checkmaxargs(L, 2);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);

    if (lua_isnil(L, 2) != 0)   /* XXX */
        times = luab_checkltimesvector(L, 2, 2);
    else
        times = NULL;

    status = futimens(fd, times);

    if (times != NULL)
        luab_pushltimesvector(L, 2, 2, times);

    return (luab_pusherr(L, status));
}

/***
 * utimensat(2) - set file access and modifications times
 *
 * @function utimensat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes referred file.
 *
 *                            #2 By path named file is relative to the directory
 *                               associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of utimensat(2).
 *
 * @param times             An instance of (LUA_TTABLE) carrying at least
 *
 *                              {
 *                                  (LUA_TUSERDATA(TIMESPEC)),
 *                                  (LUA_TUSERDATA(TIMESPEC))
 *                              }
 *
 *                          two instances of (LUA_TUSERDATA(TIMESPEC)).
 *
 * @param flag              Passed values are formatted by inclusive or:
 *
 *                              bsd.fcntl.AT_SYMLINK_NOFOLLOW.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.utimensat(fd, path, times, flag)
 */
static int
luab_utimensat(lua_State *L)
{
    int fd;
    const char *path;
    struct timespec *times;
    int flag, status;

    (void)luab_checkmaxargs(L, 4);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);

    if (lua_isnil(L, 3) != 0)
        times = luab_checkltimesvector(L, 2, 2);
    else
        times = NULL;

    flag = (int)luab_checkinteger(L, 4, INT_MAX);

    status = utimensat(fd, path, times, flag);

    if (times != NULL)
        luab_pushltimesvector(L, 2, 2, times);

    return (luab_pusherr(L, status));
}

#endif /* __POSIX_VISIBLE >= 200809 */

/***
 * fstat(2) - get file status
 *
 * @function fstat
 *
 * @param fd                Filedescriptor referres used file for examination.
 * @param sb                Result argument, instance of (LUA_TUSERDATA(STAT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.fstat(fd, sb)
 */
static int
luab_fstat(lua_State *L)
{
    int fd;
    struct stat *sb;
    int status;

    (void)luab_checkmaxargs(L, 2);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    sb = luab_udata(L, 2, stat_type, struct stat *);

    status = fstat(fd, sb);

    return (luab_pusherr(L, status));
}

#if __BSD_VISIBLE
/***
 * lchflags(2) - set file flags
 *
 * @function lchflags
 *
 * @param path              File named by path.
 * @param flags             File flags
 *
 *                             #1 bsd.sys.stat.SF_{APPEND,ARCHIVED,IMMUTABLE,
 *                                  NOUNLINK,SNAPSHOT}
 *
 *                             #2 bsd.sys.stat.UF_{APPEND,ARCHIVE,HIDDEN,
 *                                  IMMUTABLE,NODUMP,NOUNLINK,OFFLINE,
 *                                  OPAQUE,READONLY,REPARSE,SPARSE,SYSTEM}
 *
 *                          are formatted by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.lchflags(path, flags)
 */
static int
luab_lchflags(lua_State *L)
{
    const char *path;
    u_long flags;
    int status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    flags = (u_long)luab_checkinteger(L, 2, ULONG_MAX);

    status = lchflags(path, flags);

    return (luab_pusherr(L, status));
}

/***
 * lchmod(2) - change mode of file
 *
 * @function lchmod
 *
 * @param path              File named by path.
 * @param mode              Permission bit masks
 *
 *                              bsd.sys.stat.S_{
 *                                  IRWXU,          -- RWX mask for owner
 *                                  IRUSR,          -- R for owner
 *                                  IWUSR,          -- W for owner
 *                                  IXUSR,          -- X for owner
 *                                  IRWXG,          -- RWX mask for group
 *                                  IRGRP,          -- R for group
 *                                  IWGRP,          -- W for group
 *                                  IXGRP,          -- X for group
 *                                  IRWXO,          -- RWX mask for other
 *                                  IROTH,          -- R for other
 *                                  IWOTH,          -- W for other
 *                                  IXOTH,          -- X for other
 *                                  ISUID,          -- set user id on
 *                                  ISGID,          -- set group id on
 *                                  ISVTX           -- sticky bit
 *                              }
 *
 *                          are formatted by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.lchmod(path, mode)
 */
static int
luab_lchmod(lua_State *L)
{
    const char *path;
    mode_t mode;
    int status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    mode = (mode_t)luab_checkinteger(L, 2, ALLPERMS);

    status = lchmod(path, mode);

    return (luab_pusherr(L, status));
}
#endif
#if __POSIX_VISIBLE >= 200112
/***
 * lstat(2) - get file status
 *
 * @function lstat
 *
 * @param path              File pointed to by path.
 * @param sb                Result argument, instance of (LUA_TUSERDATA(STAT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.lstat(path, sb)
 */
static int
luab_lstat(lua_State *L)
{
    const char *path;
    struct stat *sb;
    int status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    sb = luab_udata(L, 2, stat_type, struct stat *);

    status = lstat(path, sb);

    return (luab_pusherr(L, status));
}
#endif

/***
 * mkdir(2) - make a directory file
 *
 * @function mkdir
 *
 * @param path              The directory path.
 * @param mode              Specifies access permissions, restricted
 *                          by the umask(2) of calling process.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.mkdir(path, mode)
 */
static int
luab_mkdir(lua_State *L)
{
    const char *path;
    mode_t mode;
    int status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    mode = (mode_t)luab_checkinteger(L, 2, ALLPERMS);

    status = mkdir(path, mode);

    return (luab_pusherr(L, status));
}

/***
 * mkfifo(2) - make a fifo file
 *
 * @function mkfifo
 *
 * @param path              Specifies the FIFO (with name path).
 * @param mode              Specifies access permissions of newly created
 *                          FIFO.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.mkfifo(path, mode)
 */
static int
luab_mkfifo(lua_State *L)
{
    const char *path;
    mode_t mode;
    int status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    mode = (mode_t)luab_checkinteger(L, 2, ALLPERMS);

    status = mkfifo(path, mode);

    return (luab_pusherr(L, status));
}

#if !defined(_MKNOD_DECLARED) && __XSI_VISIBLE
/***
 * mknod(2) - make a special file node
 *
 * @function mknodat
 *
 * @param path              Specifies the system file node (path).
 * @param mode              Specifies access permissions of newly created
 *                          system file node
 * @param dev               Specification, denotes particular device.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.mknodat(fd, path, mode)
 */
static int
luab_mknod(lua_State *L)
{
    const char *path;
    mode_t mode;
    dev_t dev;
    int status;

    (void)luab_checkmaxargs(L, 4);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    mode = (mode_t)luab_checkinteger(L, 2, ALLPERMS);
    dev = (dev_t)luab_checkinteger(L, 3, ULONG_MAX);

    status = mknod(path, mode, dev);

    return (luab_pusherr(L, status));
}
#define _MKNOD_DECLARED
#endif

/***
 * stat(2) - get file status
 *
 * @function stat
 *
 * @param path              File pointed to by path.
 * @param sb                Result argument, instance of (LUA_TUSERDATA(STAT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.stat(path, sb)
 */
static int
luab_stat(lua_State *L)
{
    const char *path;
    struct stat *sb;
    int status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    sb = luab_udata(L, 2, stat_type, struct stat *);

    status = stat(path, sb);

    return (luab_pusherr(L, status));
}

/***
 * umask(2) - set file creation mode mask
 *
 * @function umask
 *
 * @param numask            Mode mask.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (oumask [, nil, nil]) on success or
 *          (oumask, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.umask(umask)
 */
static int
luab_umask(lua_State *L)
{
    mode_t numask;
    mode_t oumask;

    (void)luab_checkmaxargs(L, 1);

    numask = (mode_t)luab_checkinteger(L, 1, ALLPERMS);
    oumask = umask(numask);

    return (luab_pusherr(L, oumask));
}

#if __POSIX_VISIBLE >= 200809
/***
 * fstatat(2) - get file status
 *
 * @function fstatat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Referres used file for examination.
 *
 *                            #2 The focussed file is relative to the directory
 *                               associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of fstatat(2).
 *
 * @param path              By path named file.
 * @param sb                Result argument, instance of (LUA_TUSERDATA(STAT)).
 * @param flag              Passed values are formatted by inclusive or:
 *
 *                              bsd.fcntl.AT_SYMLINK_NOFOLLOW.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.fstatat(fd, path, sb, flag)
 */
static int
luab_fstatat(lua_State *L)
{
    int fd;
    const char *path;
    struct stat *sb;
    int flag;
    int status;

    (void)luab_checkmaxargs(L, 2);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    sb = luab_udata(L, 3, stat_type, struct stat *);
    flag = (int)luab_checkinteger(L, 4, INT_MAX);

    status = fstatat(fd, path, sb, flag);

    return (luab_pusherr(L, status));
}

/***
 * mkdirat(2) - make a directory file
 *
 * @function mkdirat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes newly created directory.
 *
 *                            #2 The newly created directory is relative to the
 *                               directory associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of mkdirat(2).
 *
 * @param path              Specifies the directory (path).
 * @param mode              Specifies access permissions of newly created
 *                          directory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.mkdirat(fd, path, mode)
 */
static int
luab_mkdirat(lua_State *L)
{
    int fd;
    const char *path;
    mode_t mode;
    int status;

    (void)luab_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    mode = (mode_t)luab_checkinteger(L, 3, ALLPERMS);

    status = mkdirat(fd, path, mode);

    return (luab_pusherr(L, status));
}

/***
 * mkfifoat(2) - make a fifo file
 *
 * @function mkfifoat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes newly created FIFO.
 *
 *                            #2 The newly created FIFO is relative to the
 *                               directory associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of mkfifoat(2).
 *
 * @param path              Specifies the FIFO (with name path).
 * @param mode              Specifies access permissions of newly created
 *                          FIFO.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.mkfifoat(fd, path, mode)
 */
static int
luab_mkfifoat(lua_State *L)
{
    int fd;
    const char *path;
    mode_t mode;
    int status;

    (void)luab_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    mode = (mode_t)luab_checkinteger(L, 3, ALLPERMS);

    status = mkfifoat(fd, path, mode);

    return (luab_pusherr(L, status));
}

/***
 * mknodat(2) - make a special file node
 *
 * @function mknodat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes newly created node.
 *
 *                            #2 The newly created node is relative to the
 *                               directory associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of mknodat(2).
 *
 * @param path              Specifies the system file node (path).
 * @param mode              Specifies access permissions of newly created
 *                          system file node.
 * @param dev               Specification, denotes particular device.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.mknodat(fd, path, mode)
 */
static int
luab_mknodat(lua_State *L)
{
    int fd;
    const char *path;
    mode_t mode;
    dev_t dev;
    int status;

    (void)luab_checkmaxargs(L, 4);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    mode = (mode_t)luab_checkinteger(L, 3, ALLPERMS);
    dev = (dev_t)luab_checkinteger(L, 4, ULONG_MAX);

    status = mknodat(fd, path, mode, dev);

    return (luab_pusherr(L, status));
}
#endif /* __POSIX_VISIBLE >= 200809 */

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(STAT)).
 *
 * @function stat_create
 *
 * @param data          Instance of (LUA_TUSERDATA(STAT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (stat [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage hook [, err, msg ] = bsd.sys.stat.stat_create([ data ])
 */
static int
luab_stat_create(lua_State *L)
{
    struct stat *data;
    int narg;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        data = NULL;
    else
        data = luab_udata(L, narg, stat_type, struct stat *);

    return (luab_pushudata(L, &stat_type, data));
}

/*
 * Interface against <sys/stat.h>.
 */

static luab_table_t luab_sys_stat_vec[] = {
    LUABSD_INT("S_ISUID",    S_ISUID),
    LUABSD_INT("S_ISGID",    S_ISGID),
#if __BSD_VISIBLE
    LUABSD_INT("S_ISTXT",    S_ISTXT),
#endif
    LUABSD_INT("S_IRWXU",    S_IRWXU),
    LUABSD_INT("S_IRUSR",    S_IRUSR),
    LUABSD_INT("S_IWUSR",    S_IWUSR),
    LUABSD_INT("S_IXUSR",    S_IXUSR),
#if __BSD_VISIBLE
    LUABSD_INT("S_IREAD",    S_IREAD),
    LUABSD_INT("S_IWRITE",   S_IWRITE),
    LUABSD_INT("S_IEXEC",    S_IEXEC),
#endif
    LUABSD_INT("S_IRWXG",    S_IRWXG),
    LUABSD_INT("S_IRGRP",    S_IRGRP),
    LUABSD_INT("S_IWGRP",    S_IWGRP),
    LUABSD_INT("S_IXGRP",    S_IXGRP),
    LUABSD_INT("S_IRWXO",    S_IRWXO),
    LUABSD_INT("S_IROTH",    S_IROTH),
    LUABSD_INT("S_IWOTH",    S_IWOTH),
    LUABSD_INT("S_IXOTH",    S_IXOTH),
#if __XSI_VISIBLE
    LUABSD_INT("S_IFMT", S_IFMT),
    LUABSD_INT("S_IFIFO",    S_IFIFO),
    LUABSD_INT("S_IFCHR",    S_IFCHR),
    LUABSD_INT("S_IFDIR",    S_IFDIR),
    LUABSD_INT("S_IFBLK",    S_IFBLK),
    LUABSD_INT("S_IFREG",    S_IFREG),
    LUABSD_INT("S_IFLNK",    S_IFLNK),
    LUABSD_INT("S_IFSOCK",   S_IFSOCK),
    LUABSD_INT("S_ISVTX",    S_ISVTX),
#endif
#if __BSD_VISIBLE
    LUABSD_INT("S_IFWHT",    S_IFWHT),
    LUABSD_INT("ACCESSPERMS",   ACCESSPERMS),
    LUABSD_INT("ALLPERMS",  ALLPERMS),
    LUABSD_INT("DEFFILEMODE",   DEFFILEMODE),
    LUABSD_INT("UF_SETTABLE",   UF_SETTABLE),
    LUABSD_INT("UF_NODUMP", UF_NODUMP),
    LUABSD_INT("UF_IMMUTABLE",  UF_IMMUTABLE),
    LUABSD_INT("UF_APPEND", UF_APPEND),
    LUABSD_INT("UF_OPAQUE", UF_OPAQUE),
    LUABSD_INT("UF_NOUNLINK",   UF_NOUNLINK),
#if 0
    LUABSD_INT("UF_COMPRESSED", UF_COMPRESSED),
    LUABSD_INT("UF_TRACKED",    UF_TRACKED),
#endif
    LUABSD_INT("SF_SETTABLE",   SF_SETTABLE),
    LUABSD_INT("SF_ARCHIVED",   SF_ARCHIVED),
    LUABSD_INT("SF_IMMUTABLE",  SF_IMMUTABLE),
    LUABSD_INT("SF_APPEND", SF_APPEND),
    LUABSD_INT("SF_NOUNLINK",   SF_NOUNLINK),
    LUABSD_INT("SF_SNAPSHOT",   SF_SNAPSHOT),
#endif /* __BSD_VISIBLE */
#if __POSIX_VISIBLE >= 200809
    LUABSD_INT("UTIME_NOW", UTIME_NOW),
    LUABSD_INT("UTIME_OMIT",    UTIME_OMIT),
#endif
#if __BSD_VISIBLE
    LUABSD_FUNC("chflags",  luab_chflags),
    LUABSD_FUNC("chflagsat", luab_chflagsat),
#endif
    LUABSD_FUNC("chmod",    luab_chmod),
#if __BSD_VISIBLE
    LUABSD_FUNC("fchflags", luab_fchflags),
#endif
#if __POSIX_VISIBLE >= 200112
    LUABSD_FUNC("fchmod",   luab_fchmod),
#endif
#if __POSIX_VISIBLE >= 200809
    LUABSD_FUNC("fstatat", luab_fstatat),
    LUABSD_FUNC("fchmodat", luab_fchmodat),
    LUABSD_FUNC("futimens", luab_futimens),
    LUABSD_FUNC("utimensat", luab_utimensat),
#endif
    LUABSD_FUNC("fstat",    luab_fstat),
#if __BSD_VISIBLE
    LUABSD_FUNC("lchflags", luab_lchflags),
    LUABSD_FUNC("lchmod",   luab_lchmod),
#endif
#if __POSIX_VISIBLE >= 200112
    LUABSD_FUNC("lstat",    luab_lstat),
#endif
    LUABSD_FUNC("mkdir",   luab_mkdir),
    LUABSD_FUNC("mkdirat",   luab_mkdirat),
    LUABSD_FUNC("mkfifo",   luab_mkfifo),
    LUABSD_FUNC("mkfifoat",   luab_mkfifoat),
#if !defined(_MKNOD_DECLARED) && __XSI_VISIBLE
    LUABSD_FUNC("mknod",    luab_mknod),
#define _MKNOD_DECLARED
#endif
    LUABSD_FUNC("stat", luab_stat),
    LUABSD_FUNC("umask",    luab_umask),
#if __XSI_VISIBLE >= 700
    LUABSD_FUNC("mknodat",    luab_mknodat),
#endif
    LUABSD_FUNC("stat_create",   luab_stat_create),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_sys_stat_lib = {
    .cookie = LUABSD_SYS_STAT_LIB_ID,
    .name = LUABSD_SYS_STAT_LIB_KEY,
    .vec = luab_sys_stat_vec,
};
