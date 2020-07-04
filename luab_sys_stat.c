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

#include <sys/limits.h>
#include <sys/param.h>
#include <sys/stat.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

#define LUABSD_SYS_STAT_LIB_ID    1593623310
#define LUABSD_SYS_STAT_LIB_KEY    "stat"

/*
 * Components or service primitives on sys/stat.h.
 */

static int
luab_chflags(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    u_long flags = luab_checkinteger(L, 2, ULONG_MAX);
    int status;

    luab_checkmaxargs(L, 2);
    status = chflags(path, flags);

    return luab_pusherr(L, status);
}

static int
luab_lchflags(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    u_long flags = luab_checkinteger(L, 2, ULONG_MAX);
    int status;

    luab_checkmaxargs(L, 2);
    status = lchflags(path, flags);

    return luab_pusherr(L, status);
}

static int
luab_fchflags(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    u_long flags = luab_checkinteger(L, 2, ULONG_MAX);
    int status;

    status = fchflags(fd, flags);

    return luab_pusherr(L, status);
}

static int
luab_chflagsat(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    const char *path = luab_checklstring(L, 2, MAXPATHLEN);
    u_long flags = luab_checkinteger(L, 3, ULONG_MAX);
    int atflag = luab_checkinteger(L, 4, INT_MAX);
    int status;

    luab_checkmaxargs(L, 4);
    status = chflagsat(fd, path, flags, atflag);

    return luab_pusherr(L, status);
}

static int
luab_chmod(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 2, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 2);
    status = chmod(path, mode);

    return luab_pusherr(L, status);
}

static int
luab_fchmod(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    mode_t mode = luab_checkinteger(L, 2, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 2);
    status = fchmod(fd, mode);

    return luab_pusherr(L, status);
}

static int
luab_lchmod(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 2, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 2);
    status = lchmod(path, mode);

    return luab_pusherr(L, status);
}

static int
luab_fchmodat(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    const char *path = luab_checklstring(L, 2, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 3, ALLPERMS);
    int flag = luab_checkinteger(L, 4, INT_MAX);
    int status;

    luab_checkmaxargs(L, 4);
    status = fchmodat(fd, path, mode, flag);

    return luab_pusherr(L, status);
}

static int
luab_mkdir(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 2, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 2);
    status = mkdir(path, mode);

    return luab_pusherr(L, status);
}

static int
luab_mkdirat(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    const char *path = luab_checklstring(L, 2, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 3, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 3);
    status = mkdirat(fd, path, mode);

    return luab_pusherr(L, status);
}

static int
luab_mknod(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 2, ALLPERMS);
    dev_t dev = luab_checkinteger(L, 3, ULONG_MAX);
    int status;

    luab_checkmaxargs(L, 4);
    status = mknod(path, mode, dev);

    return luab_pusherr(L, status);
}

static int
luab_mknodat(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    const char *path = luab_checklstring(L, 2, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 3, ALLPERMS);
    dev_t dev = luab_checkinteger(L, 4, ULONG_MAX);
    int status;

    luab_checkmaxargs(L, 4);
    status = mknodat(fd, path, mode, dev);

    return luab_pusherr(L, status);
}

static int
luab_mkfifo(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 2, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 2);
    status = mkfifo(path, mode);

    return luab_pusherr(L, status);
}

static int
luab_mkfifoat(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    const char *path = luab_checklstring(L, 2, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 3, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 3);
    status = mkfifoat(fd, path, mode);

    return luab_pusherr(L, status);
}

static int
luab_umask(lua_State *L)
{
    mode_t numask = luab_checkinteger(L, 1, ALLPERMS);
    mode_t oumask;

    luab_checkmaxargs(L, 1);
    oumask = umask(numask);

    lua_pushinteger(L, oumask);

    return 1;
}

static luab_table_t luab_sys_stat_vec[] = { /* sys/stat.h */
    LUABSD_INT("S_ISUID",    S_ISUID),
    LUABSD_INT("S_ISGID",    S_ISGID),
    LUABSD_INT("S_ISTXT",    S_ISTXT),
    LUABSD_INT("S_IRWXU",    S_IRWXU),
    LUABSD_INT("S_IRUSR",    S_IRUSR),
    LUABSD_INT("S_IWUSR",    S_IWUSR),
    LUABSD_INT("S_IXUSR",    S_IXUSR),
    LUABSD_INT("S_IREAD",    S_IREAD),
    LUABSD_INT("S_IWRITE",   S_IWRITE),
    LUABSD_INT("S_IEXEC",    S_IEXEC),
    LUABSD_INT("S_IRWXG",    S_IRWXG),
    LUABSD_INT("S_IRGRP",    S_IRGRP),
    LUABSD_INT("S_IWGRP",    S_IWGRP),
    LUABSD_INT("S_IXGRP",    S_IXGRP),
    LUABSD_INT("S_IRWXO",    S_IRWXO),
    LUABSD_INT("S_IROTH",    S_IROTH),
    LUABSD_INT("S_IWOTH",    S_IWOTH),
    LUABSD_INT("S_IXOTH",    S_IXOTH),
    LUABSD_INT("S_IFMT", S_IFMT),
    LUABSD_INT("S_IFIFO",    S_IFIFO),
    LUABSD_INT("S_IFCHR",    S_IFCHR),
    LUABSD_INT("S_IFDIR",    S_IFDIR),
    LUABSD_INT("S_IFBLK",    S_IFBLK),
    LUABSD_INT("S_IFREG",    S_IFREG),
    LUABSD_INT("S_IFLNK",    S_IFLNK),
    LUABSD_INT("S_IFSOCK",   S_IFSOCK),
    LUABSD_INT("S_ISVTX",    S_ISVTX),
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
    LUABSD_FUNC("chflags",  luab_chflags),
    LUABSD_FUNC("chflagsat", luab_chflagsat),
    LUABSD_FUNC("chmod",    luab_chmod),
    LUABSD_FUNC("fchflags", luab_fchflags),
    LUABSD_FUNC("fchmod",   luab_fchmod),
    LUABSD_FUNC("fchmodat", luab_fchmodat),
    LUABSD_FUNC("lchflags", luab_lchflags),
    LUABSD_FUNC("lchmod",   luab_lchmod),
    LUABSD_FUNC("mkdir",   luab_mkdir),
    LUABSD_FUNC("mkdirat",   luab_mkdirat),
    LUABSD_FUNC("mkfifo",   luab_mkfifo),
    LUABSD_FUNC("mkfifoat",   luab_mkfifoat),
    LUABSD_FUNC("mknod",    luab_mknod),
    LUABSD_FUNC("mknodat",    luab_mknodat),
    LUABSD_FUNC("umask",    luab_umask),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_sys_stat_lib = {
    .cookie = LUABSD_SYS_STAT_LIB_ID,
    .name = LUABSD_SYS_STAT_LIB_KEY,
    .vec = luab_sys_stat_vec,
};
