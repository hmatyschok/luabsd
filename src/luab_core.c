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
 * Common ops. and subr.
 */

int
luab_checkmaxargs(lua_State *L, int nmax)
{
    int narg;

    if ((narg = lua_gettop(L)) > nmax)
        luaL_error(L, "#%d args, but #%d expected", narg, nmax);

    return narg;
}

int
luab_pusherr(lua_State *L, lua_Integer res)
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
luab_pushstring(lua_State *L, const char *s)
{
    int save_errno = errno;
    caddr_t msg;
    size_t len;
    int status;

    if (s != NULL) {
        len = strnlen(s, LUAL_BUFFERSIZE);
        lua_pushlstring(L, s, len);

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
 * Operations on Atomic Data Types.
 */

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

/*
 * Operations on Complex Data Types.
 */

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

/* Translate an instance of LUA_TTABLE into an argv. */
const char **
luab_checkargv(lua_State *L, int narg)
{
    const char **argv;
    size_t n;

    luab_checktable(L, narg);

    if ((n = lua_rawlen(L, narg)) == 0) /* XXX */
        luaL_argerror(L, narg, "Empty table");

    if ((argv = calloc((n + 1), sizeof(*argv))) == NULL)
        luaL_argerror(L, narg, "Cannot allocate memory");

    lua_pushnil(L);

    for (n = 0; lua_next(L, narg) != 0; n++) {
        /*
         * (k,v) := (-2,-1) -> (LUA_TNUMBER,LUA_TSTRING)
         */
        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isstring(L, -1) != 0)) {
            argv[n] = lua_tostring(L, -1);
        } else {
            free(argv);
            luaL_argerror(L, narg, "Invalid argument");
        }
        lua_pop(L, 1);
    }
    return argv;
}

/* Allocate an array by cardinality of given table */
void *
luab_newvector(lua_State *L, int narg, size_t len, size_t size)
{
    size_t n = luab_checkltable(L, narg, len);
    void *vec;

    if (size == 0)
        luaL_argerror(L, narg, "Invalid argument");

    if ((vec = calloc(n, size)) == NULL)
        luaL_argerror(L, narg, "Cannot allocate memory");

    return (vec);
}

/* Translate an instance of LUA_TTABLE into an array of integers. */
int *
luab_checkintvector(lua_State *L, int narg, size_t len)
{
    int *vec, k, v;

    vec = luab_newvector(L, narg, len, sizeof(int));

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (int)luab_tointeger(L, -1, UINT_MAX);
            vec[k] = v;
        } else {
            free(vec);
            luaL_argerror(L, narg, "Invalid argument");
        }
        lua_pop(L, 1);
    }
    return vec;
}

/* Translate a LUA_TTABLE over LUA_TUSERDATA into an array of timespec{} items. */
struct timespec *
luab_checktimesvector(lua_State *L, int narg, size_t len)
{
    struct timespec *vec, *v;
    int k;

    vec = luab_newvector(L, narg, len, sizeof(struct timespec));

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isuserdata(L, -1) != 0)) {  /* XXX */
            v = (struct timespec *)(*timespec_type.get)(L, -1);
            (void)memmove(&vec[k], v, sizeof(struct timespec));
        } else {
            free(vec);
            luaL_argerror(L, narg, "Invalid argument");
        }
        lua_pop(L, 1);
    }
    return vec;
}

void
luab_pushtimesvector(lua_State *L, int narg, size_t len, void *arg)
{
    struct timespec *vec, *v;
    int k;

    (void)luab_checkltable(L, narg, len);

    vec = (struct timespec *)arg;

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isuserdata(L, -1) != 0)) {
            v = (struct timespec *)(*timespec_type.get)(L, -1);
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
 * Common subr. for initializiation, those are
 * called during runtime of package.loadlib().
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
    luab_newmetatable(L, &crypt_data_type);
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
