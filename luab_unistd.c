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

#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

#define LUABSD_UNISTD_LIB_COOKIE    1593623310

/*
 * Interface against components or service primitives on unistd.h.
 */

static int
luab_fork(lua_State *L)
{
    pid_t pid;

    if ((pid = fork()) < 0)
        return luab_pusherr(L, pid);

    lua_pushinteger(L, pid);

    return 1;
}

static int
luab_getegid(lua_State *L)
{
    gid_t egid = getegid();

    lua_pushinteger(L, egid);

    return 1;
}

static int
luab_geteuid(lua_State *L)
{
    uid_t euid = geteuid();

    lua_pushinteger(L, euid);

    return 1;
}

static int
luab_getgid(lua_State *L)
{
    gid_t gid = getgid();

    lua_pushinteger(L, gid);

    return 1;
}

static int
luab_getlogin(lua_State *L)
{
    char *p;

    if ((p = getlogin()) == NULL)
        lua_pushnil(L);
    else
        lua_pushstring(L, p);

    return 1;
}

static int
luab_getpid(lua_State *L)
{
    pid_t pid = getpid();

    lua_pushinteger(L, pid);

    return 1;
}

static int
luab_getppid(lua_State *L)
{
    pid_t pid = getppid();

    lua_pushinteger(L, pid);

    return 1;
}

static int
luab_getpgrp(lua_State *L)
{
    pid_t pgrp = getpgrp();

    lua_pushinteger(L, pgrp);

    return 1;
}

static int
luab_getpgid(lua_State *L)
{
    pid_t pid = luab_checkinteger(L, 1, UINT_MAX);
    pid_t pgrp;

    if ((pgrp = getpgid(pid)) < 0)
        return luab_pusherr(L, pgrp);

    lua_pushinteger(L, pgrp);

    return 1;
}

static int
luab_getuid(lua_State *L)
{
    uid_t uid = getuid();

    lua_pushinteger(L, uid);

    return 1;
}

static int
luab_getsid(lua_State *L)
{
    pid_t pid = luab_checkinteger(L, 1, UINT_MAX);
    pid_t sid;

    if ((sid = getsid(pid)) < 0)
        return luab_pusherr(L, sid);

    lua_pushinteger(L, sid);

    return 1;
}

static int
luab_setegid(lua_State *L)
{
    gid_t egid = luab_checkinteger(L, 1, UINT_MAX);
    int status;

    if ((status = setegid(egid)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
luab_seteuid(lua_State *L)
{
    uid_t euid = luab_checkinteger(L, 1, UINT_MAX);
    int status;

    if ((status = seteuid(euid)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
luab_setgid(lua_State *L)
{
    gid_t gid = luab_checkinteger(L, 1, UINT_MAX);
    int status;

    if ((status = setgid(gid)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
luab_setlogin(lua_State *L)
{
    const char *name = luab_checklstring(L, 1, MAXLOGNAME);
    int status;

    if ((status = setlogin(name)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
luab_setpgid(lua_State *L)
{
    pid_t pid = luab_checkinteger(L, 1, UINT_MAX);
    pid_t pgrp = luab_checkinteger(L, 2, UINT_MAX);
    int status;

    if ((status = setpgid(pid, pgrp)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
luab_setpgrp(lua_State *L)
{
    pid_t pid = luab_checkinteger(L, 1, UINT_MAX);
    pid_t pgrp = luab_checkinteger(L, 2, UINT_MAX);
    int status;

    if ((status = setpgrp(pid, pgrp)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
luab_setsid(lua_State *L)
{
    pid_t sid;

    if ((sid = setsid()) < 0)
        return luab_pusherr(L, sid);

    lua_pushinteger(L, sid);

    return 1;
}

static int
luab_setuid(lua_State *L)
{
    uid_t uid = luab_checkinteger(L, 1, UINT_MAX);
    int status;

    if ((status = setuid(uid)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static luab_table_t luab_unistd_vec[] = {   /* unistd.h */
    LUABSD_FUNC("fork",   luab_fork),
    LUABSD_FUNC("getegid",    luab_getegid),
    LUABSD_FUNC("geteuid",    luab_geteuid),
    LUABSD_FUNC("getgid",    luab_getgid),
    LUABSD_FUNC("getlogin",   luab_getlogin),
    LUABSD_FUNC("getpid", luab_getpid),
    LUABSD_FUNC("getppid",    luab_getppid),
    LUABSD_FUNC("getpgid",    luab_getpgid),
    LUABSD_FUNC("getpgrp",    luab_getpgrp),
    LUABSD_FUNC("getuid", luab_getuid),
    LUABSD_FUNC("getsid", luab_getsid),
    LUABSD_FUNC("setegid",    luab_setegid),
    LUABSD_FUNC("seteuid",    luab_seteuid),
    LUABSD_FUNC("setgid",    luab_setgid),
    LUABSD_FUNC("setlogin",   luab_setlogin),
    LUABSD_FUNC("setpgid",    luab_setpgid),
    LUABSD_FUNC("setpgrp",    luab_setpgrp),
    LUABSD_FUNC("setsid", luab_setsid),
    LUABSD_FUNC("setuid", luab_setuid),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_unistd_lib = {
    .id = LUABSD_UNISTD_LIB_COOKIE,
    .name = "unistd",
    .vec = luab_unistd_vec,
};
