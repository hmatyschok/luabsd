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

extern int luab_stat_create(lua_State *);

#define LUABSD_SYS_STAT_LIB_ID    1593623310
#define LUABSD_SYS_STAT_LIB_KEY    "stat"

extern luab_module_t luab_sys_stat_lib;


/* Translate a LUA_TTABLE over LUA_TUSERDATA into an array of timespec{} items. */
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
 * Components or service primitives over sys/stat.h.
 */

#if __BSD_VISIBLE
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

static int
luab_umask(lua_State *L)
{
    mode_t numask;
    mode_t oumask;

    (void)luab_checkmaxargs(L, 1);

    numask = (mode_t)luab_checkinteger(L, 1, ALLPERMS);

    oumask = umask(numask);

    lua_pushinteger(L, oumask);

    return 1;
}

#if __POSIX_VISIBLE >= 200809
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



static luab_table_t luab_sys_stat_vec[] = { /* sys/stat.h */
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
    LUABSD_FUNC("fchmodat", luab_fchmodat),
    LUABSD_FUNC("futimens", luab_futimens),
    LUABSD_FUNC("utimensat", luab_utimensat),
#endif
    LUABSD_FUNC("fstat",    luab_fstat),
#if __BSD_VISIBLE
    LUABSD_FUNC("lchflags", luab_lchflags),
#endif
#if __POSIX_VISIBLE >= 200112
/*
 * XXX
 *
 *  int lstat(const char * __restrict, struct stat * __restrict);
 */
#endif
    LUABSD_FUNC("lchmod",   luab_lchmod),
    LUABSD_FUNC("mkdir",   luab_mkdir),
    LUABSD_FUNC("mkdirat",   luab_mkdirat),
    LUABSD_FUNC("mkfifo",   luab_mkfifo),
    LUABSD_FUNC("mkfifoat",   luab_mkfifoat),
#if !defined(_MKNOD_DECLARED) && __XSI_VISIBLE
    LUABSD_FUNC("mknod",    luab_mknod),
#define _MKNOD_DECLARED
#endif
/*
 * XXX
 *
 *  int stat(const char * __restrict, struct stat * __restrict);
 */
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
