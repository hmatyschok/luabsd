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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

LUAMOD_API int  luaopen_bsd(lua_State *);

/*
 * Translate an instance of LUA_TTABLE into an argv.
 */
const char **
luab_checkargv(lua_State *L, int narg)
{
    const char **argv;
    int n;

    luab_checktable(L, narg);

    if ((n = lua_rawlen(L, narg)) == 0)
        luaL_argerror(L, narg, "Empty table");

    if ((argv = calloc((n + 1), sizeof(*argv))) == NULL)
        luaL_argerror(L, narg, "Cannot allocate memory");

    n = 0;

    lua_pushnil(L);

    while (lua_next(L, narg) != 0) {
        /*
         * (k,v) := (-2,-1) -> (LUA_TNUMBER,LUA_TSTRING)
         */
        if ((lua_isnumber(L, -2) != 0)
            && (lua_isstring(L, -1) != 0)) {
            argv[n] = lua_tostring(L, -1);
            lua_pop(L, 1);
        } else {
            free(argv);
            lua_pop(L, 1);
            luaL_argerror(L, narg, "Invalid argument");
        }

        n++;
    }

    return argv;
}

/*
 * Translate an instance of LUA_TTABLE into an array of integers.
 */
int *
luab_checkintvector(lua_State *L, int narg, size_t len)
{
    int *vec;
    size_t n;
                                    /* XXX redundant code-section */
    luab_checktable(L, narg);

    if ((n = lua_rawlen(L, narg)) != len)
        luaL_argerror(L, narg, "Size mismatch");

    if ((vec = calloc(len, sizeof(int))) == NULL)
        luaL_argerror(L, narg, "Cannot allocate memory");

    n = 0;

    lua_pushnil(L);

    while (lua_next(L, narg) != 0) {

        if ((lua_isnumber(L, -2) != 0)
            && (lua_isnumber(L, -1) != 0)) {
            vec[n] = lua_tointeger(L, -1);
            lua_pop(L, 1);
        } else {
            free(vec);
            lua_pop(L, 1);
            luaL_argerror(L, narg, "Invalid argument");
        }

        n++;
    }

    return vec;
}

const char *
luab_checklstring(lua_State *L, int narg, size_t n)
{
    const char *buf;
    size_t len;

    buf = luaL_checklstring(L, narg, &len);

    if (len > n)    /* XXX err_msg */
        luaL_argerror(L, narg, "Value too large to be stored in data type");

    return buf;
}

int
luab_checkmaxargs(lua_State *L, int nmax)
{
    int narg;

    if ((narg = lua_gettop(L)) > nmax)
        luaL_error(L, "#%d args, but #%d expected", narg, nmax);

    return narg;
}

void *
luab_newuserdata(lua_State *L, luab_module_t *m, void *arg)
{
    void *ud = NULL;

    if (m != NULL) {
        if ((ud = lua_newuserdata(L, m->sz)) != NULL) {
            (void)memset_s(ud, m->sz, 0, m->sz);

            if (m->init != NULL && arg != NULL)
                (*m->init)(ud, arg);

            luaL_setmetatable(L, m->name);
        } else
            errno = ENOMEM;
    } else
        errno = EINVAL;

    return ud;
}

int
luab_pusherr(lua_State *L, long res)
{
    int save_errno = errno;
    char *msg;
    int status;

    lua_pushinteger(L, res);

    if (save_errno != 0 && res < 0) {
        msg = strerror(save_errno);
        lua_pushstring(L, msg);
        status = 2;
    } else
        status = 1;

    return status;
}

int
luab_pushnil(lua_State *L)
{
    int save_errno = errno;
    char *msg;
    int status;

    lua_pushnil(L);

    if (save_errno != 0) {
        msg = strerror(save_errno);
        lua_pushstring(L, msg);
        status = 2;
    } else
        status = 1;

    return status;
}

int
luab_pushstring(lua_State *L, char *res)
{
    int save_errno = errno;
    char *msg;
    int status;

    if (res != NULL) {
        lua_pushstring(L, res);

        if (save_errno != 0) {
            msg = strerror(save_errno);
            lua_pushstring(L, msg);
            status = 2;
        } else
            status = 1;
    } else
        status = luab_pushnil(L);

    return status;
}

/*
 * Called, when package.loadlib takes place.
 */

static void
luab_populate(lua_State *L, luab_table_t *vec)
{
    luab_table_t *tok;

    for (tok = vec; tok->key != NULL; tok++) {
        (*tok->init)(L, &tok->val);
        lua_setfield(L, -2, tok->key);
    }
    lua_pop(L, 0);
}

static void
luab_newtable(lua_State *L, luab_module_t *arg)
{
    lua_newtable(L);
    luab_populate(L, arg->vec);
    lua_setfield(L, -2, arg->name);
}

static void
luab_newmetatable(lua_State *L, luab_module_t *arg)
{
    luaL_newmetatable(L, arg->name);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luab_populate(L, arg->vec);

    lua_pop(L, 1);
}

LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    lua_newtable(L);
                        /* XXX */
    lua_newtable(L);

    luab_newtable(L, &luab_sys_file_lib);
    luab_newtable(L, &luab_sys_stat_lib);
    luab_newtable(L, &luab_sys_time_lib);
    luab_newtable(L, &luab_sys_uio_lib);
    luab_newtable(L, &luab_sys_unistd_lib);

    lua_setfield(L, -2, "sys");

    luab_newtable(L, &luab_db_lib);
    luab_newtable(L, &luab_fcntl_lib);
    luab_newtable(L, &luab_stdlib_lib);
    luab_newtable(L, &luab_time_lib);
    luab_newtable(L, &luab_unistd_lib);
    luab_newtable(L, &luab_uuid_lib);

    lua_pushvalue(L, -1);

#if __BSD_VISIBLE
    luab_newmetatable(L, &bintime_type);
    luab_newmetatable(L, &db_type);
#endif
    luab_newmetatable(L, &clockinfo_type);
    luab_newmetatable(L, &flock_type);
    luab_newmetatable(L, &timezone_type);
    luab_newmetatable(L, &timespec_type);
    luab_newmetatable(L, &itimerval_type);
    luab_newmetatable(L, &stat_type);
    luab_newmetatable(L, &tm_type);
    luab_newmetatable(L, &iovec_type);

    return 1;
}
