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

/* XXX */

extern luab_module_t clockinfo_type;
extern luab_module_t flock_type;
extern luab_module_t if_nameindex_type;
extern luab_module_t in_addr_type;
extern luab_module_t in6_addr_type;
extern luab_module_t itimerval_type;
extern luab_module_t linger_type;
extern luab_module_t sockaddr_type;
extern luab_module_t stat_type;
extern luab_module_t timespec_type;
extern luab_module_t timezone_type;
extern luab_module_t tm_type;

#if __BSD_VISIBLE
extern luab_module_t dbt_type;
extern luab_module_t db_type;
extern luab_module_t bintime_type;
extern luab_module_t crypt_data_type;
#endif

extern luab_module_t luab_arpa_inet_lib;

extern luab_module_t luab_net_if_lib;
extern luab_module_t luab_net_if_dl_lib;

extern luab_module_t luab_sys_file_lib;
extern luab_module_t luab_sys_stat_lib;
extern luab_module_t luab_sys_time_lib;
extern luab_module_t luab_sys_uio_lib;
extern luab_module_t luab_sys_unistd_lib;
extern luab_module_t luab_sys_socket_lib;

extern luab_module_t luab_core_lib;

extern luab_module_t luab_db_lib;
extern luab_module_t luab_fcntl_lib;
extern luab_module_t luab_stdlib_lib;
extern luab_module_t luab_time_lib;
extern luab_module_t luab_unistd_lib;
extern luab_module_t luab_uuid_lib;

extern int luab_CreateHook(lua_State *);

#define LUABSD_CORE_LIB_ID    1595987973
#define LUABSD_CORE_LIB_KEY   "core"

static luab_table_t luab_core_vec[] = {
    LUABSD_FUNC("CreateHook",   luab_CreateHook),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_core_lib = {
    .cookie = LUABSD_CORE_LIB_ID,
    .name = LUABSD_CORE_LIB_KEY,
    .vec = luab_core_vec,
};

LUAMOD_API int  luaopen_bsd(lua_State *);

/*
 * Common ops. and subr.
 */

int
luab_buf_alloc(luab_buf_t *buf, size_t len)
{
    caddr_t bp;
    int status;

    if (buf != NULL && len > 0) {
        if (buf->buf_data != NULL)
            bp = realloc(buf->buf_data, len);
        else
            bp = malloc(len);

        if (bp != NULL) {
            (void)memset_s(bp, len, 0, len);

            buf->buf_data = bp;
            buf->buf_len = len;

            status = 0;
        } else
            status = -1;
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (status);
}

int
luab_buf_free(luab_buf_t *buf)
{
    int status;

    if (buf != NULL) {
        if (buf->buf_data != NULL)
            free(buf->buf_data);

        buf->buf_len = 0;
        status = 0;
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (status);
}

int
luab_checkmaxargs(lua_State *L, int nmax)
{
    int narg;

    if ((narg = lua_gettop(L)) > nmax)
        luaL_error(L, "#%d args, but #%d expected", narg, nmax);

    return (narg);
}

int
luab_pusherr(lua_State *L, lua_Integer res)
{
    int save_errno = errno;
    char *msg;
    int status;

    lua_pushinteger(L, res);

    if (save_errno != 0 && res < 0) {
        lua_pushinteger(L, save_errno);
        msg = strerror(save_errno);
        lua_pushstring(L, msg);
        status = 3;
    } else if (save_errno == res) {
        msg = strerror(save_errno);
        lua_pushstring(L, msg);
        status = 2;
    } else
        status = 1;

    return (status);
}

int
luab_pushnil(lua_State *L)
{
    int save_errno = errno;
    char *msg;
    int status;

    lua_pushnil(L);

    if (save_errno != 0) {
        lua_pushinteger(L, save_errno);
        msg = strerror(save_errno);
        lua_pushstring(L, msg);
        status = 3;
    } else
        status = 1;

    return (status);
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
            lua_pushinteger(L, save_errno);
            msg = strerror(save_errno);
            lua_pushstring(L, msg);
            status = 3;
        } else
            status = 1;
    } else
        status = luab_pushnil(L);

    return (status);
}

/*
 * Operations on atomic (or primitive) data types.
 */

const char *
luab_checklstring(lua_State *L, int narg, size_t n)
{
    const char *buf;
    size_t len;

    buf = luaL_checklstring(L, narg, &len);

    if (len > n)    /* XXX err_msg */
        luaL_argerror(L, narg, "Value too large to be stored in data type");

    return (buf);
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

    return (ud);
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
    return (vec);
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
 * Common subr. for initializiation, those are
 * called during runtime of package.loadlib().
 */

static void
luab_populate(lua_State *L, luab_module_t *m)
{
    luab_table_t *tok;

    for (tok = m->vec; tok->key != NULL; tok++) {
        (void)(*tok->init)(L, &tok->val);
        lua_setfield(L, -2, tok->key);
    }
    lua_pop(L, 0);
}

static void
luab_newtable(lua_State *L, luab_module_t *m)
{
    lua_newtable(L);
    luab_populate(L, m);
    lua_setfield(L, -2, m->name);
}

static void
luab_newmetatable(lua_State *L, luab_module_t *m)
{
    luaL_newmetatable(L, m->name);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luab_populate(L, m);

    lua_pop(L, 1);
}

/*
 * Reflects and maps interface against API over /include.
 *
 * XXX Well, this will be refactored (partially), soon.
 */
LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    lua_newtable(L);

    lua_newtable(L);
    luab_newtable(L, &luab_arpa_inet_lib);
    lua_setfield(L, -2, "arpa");

    lua_newtable(L);
    luab_newtable(L, &luab_net_if_dl_lib);
    luab_populate(L, &luab_net_if_lib);
    lua_setfield(L, -2, "net");

    lua_newtable(L);
    luab_newtable(L, &luab_sys_file_lib);
    luab_newtable(L, &luab_sys_stat_lib);
    luab_newtable(L, &luab_sys_time_lib);
    luab_newtable(L, &luab_sys_uio_lib);
    luab_newtable(L, &luab_sys_unistd_lib);
    luab_newtable(L, &luab_sys_socket_lib);
    lua_setfield(L, -2, "sys");

    luab_newtable(L, &luab_core_lib);
    luab_newtable(L, &luab_db_lib);
    luab_newtable(L, &luab_fcntl_lib);
    luab_newtable(L, &luab_stdlib_lib);
    luab_newtable(L, &luab_time_lib);
    luab_newtable(L, &luab_unistd_lib);
    luab_newtable(L, &luab_uuid_lib);

    lua_pushvalue(L, -1);

    luab_newmetatable(L, &clockinfo_type);
    luab_newmetatable(L, &flock_type);
    luab_newmetatable(L, &if_nameindex_type);
    luab_newmetatable(L, &in_addr_type);
    luab_newmetatable(L, &in6_addr_type);
    luab_newmetatable(L, &iovec_type);
    luab_newmetatable(L, &itimerval_type);
    luab_newmetatable(L, &linger_type);
    luab_newmetatable(L, &sockaddr_type);
    luab_newmetatable(L, &stat_type);
    luab_newmetatable(L, &timezone_type);
    luab_newmetatable(L, &timespec_type);
#if __BSD_VISIBLE
    luab_newmetatable(L, &bintime_type);
    luab_newmetatable(L, &crypt_data_type);
    luab_newmetatable(L, &dbt_type);
    luab_newmetatable(L, &db_type);
#endif
    return (1);
}
