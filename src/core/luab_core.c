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
#include <uuid.h>

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
extern luab_module_t msghdr_type;
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
extern luab_module_t luab_sys_un_lib;
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

LUAMOD_API int  luaopen_bsd(lua_State *);

/*
 * Common subr. for initializiation, those are
 * called during runtime of package.loadlib().
 */

static void
luab_populate(lua_State *L, int narg, luab_module_t *m)
{
    luab_table_t *tok;

    for (tok = m->vec; tok->key != NULL; tok++) {
        (void)(*tok->init)(L, &tok->val);
        lua_setfield(L, narg, tok->key);
    }
    lua_pop(L, 0);
}

static void
luab_newtable(lua_State *L, int narg, luab_module_t *m)
{
    lua_newtable(L);
    luab_populate(L, narg, m);
    lua_setfield(L, narg, m->name);
}

static void
luab_newmetatable(lua_State *L, luab_module_t *m)
{
    luaL_newmetatable(L, m->name);  /* XXX */
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luab_populate(L, -2, m);

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
    luab_newtable(L, -2, &luab_arpa_inet_lib);
    lua_setfield(L, -2, "arpa");

    lua_newtable(L);
    luab_newtable(L, -2, &luab_net_if_dl_lib);
    luab_populate(L, -2, &luab_net_if_lib);
    lua_setfield(L, -2, "net");

    lua_newtable(L);
    luab_newtable(L, -2, &luab_sys_file_lib);
    luab_newtable(L, -2, &luab_sys_stat_lib);
    luab_newtable(L, -2, &luab_sys_time_lib);
    luab_newtable(L, -2, &luab_sys_uio_lib);
    luab_newtable(L, -2, &luab_sys_un_lib);
    luab_newtable(L, -2, &luab_sys_unistd_lib);
    luab_newtable(L, -2, &luab_sys_socket_lib);
    lua_setfield(L, -2, "sys");

    luab_newtable(L, -2, &luab_core_lib);
    luab_newtable(L, -2, &luab_db_lib);
    luab_newtable(L, -2, &luab_fcntl_lib);
    luab_newtable(L, -2, &luab_stdlib_lib);
    luab_newtable(L, -2, &luab_time_lib);
    luab_newtable(L, -2, &luab_unistd_lib);
    luab_newtable(L, -2, &luab_uuid_lib);

    lua_pushvalue(L, -1);

    luab_newmetatable(L, &clockinfo_type);
    luab_newmetatable(L, &flock_type);
    luab_newmetatable(L, &if_nameindex_type);
    luab_newmetatable(L, &in_addr_type);
    luab_newmetatable(L, &in6_addr_type);
    luab_newmetatable(L, &iovec_type);
    luab_newmetatable(L, &itimerval_type);
    luab_newmetatable(L, &linger_type);
    luab_newmetatable(L, &msghdr_type);
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

/*
 * Operations on stack.
 */

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
    } else {
        errno = EINVAL;
        status = luab_pushnil(L);
    }
    return (status);
}

int
luab_pushldata(lua_State *L, caddr_t s, size_t len)
{
    int save_errno = errno;
    luaL_Buffer b;
    caddr_t buf, msg;
    int status;

    if (s != NULL && len > 0) {
        luaL_buffinit(L, &b);
        buf = luaL_prepbuffsize(&b, len);

        (void)memmove(buf, s, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        if (save_errno != 0) {
            lua_pushinteger(L, save_errno);
            msg = strerror(save_errno);
            lua_pushstring(L, msg);
            status = 3;
        } else {
            msg = NULL;
            status = 1;
        }
    } else {
        errno = EINVAL;
        status = luab_pushnil(L);
    }
    return (status);
}

int
luab_pushliovec(lua_State *L, caddr_t s, size_t len, size_t max_len)
{
    int save_errno = errno;
    luab_iovec_param_t iop;
    caddr_t msg;
    int status;

    if (s != NULL && len > 0 && max_len >= len) {
        (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

        iop.iop_buf.buf_len = max_len;
        iop.iop_data.buf_len = len;
        iop.iop_data.buf_data = s;

        if ((*iovec_type.ctor)(L, &iop) != NULL) {
            if (save_errno != 0) {
                lua_pushinteger(L, save_errno);
                msg = strerror(save_errno);
                lua_pushstring(L, msg);
                status = 3;
            } else {
                msg = NULL;
                status = 1;
            }
        } else
            status = luab_pushnil(L);
    } else {
        errno = EINVAL;
        status = luab_pushnil(L);
    }
    return (status);
}

/*
 * Operations on LUA_TTABLE.
 */

void
luab_rawsetinteger(lua_State *L, int narg, lua_Integer k, lua_Integer v)
{
    lua_pushinteger(L, v);
    lua_rawseti(L, narg, k);
}

void
luab_rawsetudata(lua_State *L, int narg, luab_module_t *m, lua_Integer k, void *v)
{
    /*
     * Best effort, this means try to push things on
     * stack at least as it's possible, regardless
     * if allocation of memory is possible or not.
     */
    if ((*m->ctor)(L, v) != NULL)
        lua_rawseti(L, narg, k);
}

void
luab_rawsetiovec(lua_State *L, int narg, lua_Integer k, void *v, size_t len)
{
    luab_iovec_param_t iop;

    if (len > 0) {  /* XXX redundant code-section */
        (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

        iop.iop_buf.buf_len = len;
        iop.iop_data.buf_len = len;
        iop.iop_data.buf_data = v;

        luab_rawsetudata(L, narg, &iovec_type, k, &iop);
    }
}

void
luab_setcfunction(lua_State *L, int narg, const char* k, lua_CFunction v)
{
    lua_pushcfunction(L, v);
    lua_setfield(L, narg, k);
}

void
luab_setinteger(lua_State *L, int narg, const char *k, lua_Integer v)
{
    lua_pushinteger(L, v);
    lua_setfield(L, narg, k);
}

void
luab_setstring(lua_State *L, int narg, const char *k, const char *v)
{
    lua_pushstring(L, v);
    lua_setfield(L, narg, k);
}

void
luab_setudata(lua_State *L, int narg, luab_module_t *m, const char *k, void *v)
{
    /*
     * Best effort, this means try to push things on
     * stack at least as it's possible, regardless
     * if allocation of memory is possible or not.
     */
    if ((*m->ctor)(L, v) != NULL)
        lua_setfield(L, narg, k);
}

void
luab_setiovec(lua_State *L, int narg, const char *k, void *v, size_t len)
{
    luab_iovec_param_t iop;

    if (len > 0) {  /* XXX redundant code-section */
        (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

        iop.iop_buf.buf_len = len;
        iop.iop_data.buf_len = len;
        iop.iop_data.buf_data = v;

        luab_setudata(L, narg, &iovec_type, k, &iop);
    }
}

/*
 * Operations on atomic (or primitive) data types.
 */

lua_Integer
luab_checkinteger(lua_State *L, int narg, lua_Integer b_msk)
{
    return ((luaL_checkinteger(L, narg)) & (b_msk));
}

lua_Integer
luab_tointeger(lua_State *L, int narg, lua_Integer b_msk)
{
    return ((lua_tointeger(L, narg)) & (b_msk));
}

const char *
luab_islstring(lua_State *L, int narg, size_t n)
{
    const char *buf;
    size_t len;

    if ((buf = luaL_tolstring(L, narg, &len)) != NULL) {
        if (n <= len)
            return (buf);
    }
    return (NULL);
}

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
 * Vector operations.
 */

int
luab_checkmaxargs(lua_State *L, int nmax)
{
    int narg;

    if ((narg = lua_gettop(L)) > nmax)
        luaL_error(L, "#%d args, but #%d expected", narg, nmax);

    return (narg);
}

int
luab_checktable(lua_State *L, int narg)
{
    if (lua_istable(L, narg) == 0)
        luaL_argerror(L, narg, "Table expected");

    return (lua_rawlen(L, narg));
}

size_t
luab_checkltable(lua_State *L, int narg, size_t len)
{
    size_t n;

    if ((n = luab_checktable(L, narg)) != len)
        luaL_argerror(L, narg, "Size mismatch");

    return (n);
}

/* Allocate an array by cardinality of given table. */
void *
luab_newvector(lua_State *L, int narg, size_t size)
{
    size_t n;
    void *vec;

    if ((n = luab_checktable(L, narg)) == 0)
        luaL_argerror(L, narg, "Empty table");

    if (size == 0)
        luaL_argerror(L, narg, "Invalid argument");

    if ((vec = calloc(n, size)) == NULL)
        luaL_argerror(L, narg, "Cannot allocate memory");

    return (vec);
}

/* Allocate an array by constraint less equal from cardinality of given table. */
void *
luab_newlvector(lua_State *L, int narg, size_t len, size_t size)
{
    size_t n;
    void *vec;

    n = luab_checkltable(L, narg, len);

    if (size == 0)
        luaL_argerror(L, narg, "Invalid argument");

    if ((vec = calloc(n, size)) == NULL)
        luaL_argerror(L, narg, "Cannot allocate memory");

    return (vec);
}

/* Translate an instance of LUA_TTABLE into an array of integers. */
int *
luab_checklintvector(lua_State *L, int narg, size_t len)
{
    int *vec, k, v;

    vec = luab_newlvector(L, narg, len, sizeof(int));

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

/*
 * Operations on Complex Data Types.
 */

void *
luab_newuserdata(lua_State *L, luab_module_t *m, void *arg)
{
    luab_udata_t *ud = NULL;

    if (m != NULL) {
        if ((ud = lua_newuserdata(L, m->sz)) != NULL) {
            (void)memset_s(ud, m->sz, 0, m->sz);

            TAILQ_INIT(&ud->ud_hooks);

            ud->ud_m = m;

            if (m->init != NULL && arg != NULL)
                (*m->init)(ud, arg);

            luaL_setmetatable(L, m->name);
        } else
            errno = ENOMEM;
    } else
        errno = EINVAL;

    return (ud);
}

/*
 * Operations on argv for family (of functions) over exec(2).
 *
 * Translate an instance of LUA_TTABLE into an argv.
 */
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

void *
luab_checkudata(lua_State *L, int narg, luab_module_t *m)
{
    return (luaL_checkudata(L, narg, m->name));
}

void *
luab_toudata(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *ud = luab_todata(L, narg, m, luab_udata_t *);

    return (ud + 1);
}

void *
luab_checkludata(lua_State *L, int narg, luab_module_t *m, size_t len)
{
    luab_iovec_t *buf;  /* XXX namespace */

    if ((buf = luab_isiovec(L, narg)) != NULL) {
        if (buf->iov.iov_base == NULL)
            luaL_argerror(L, narg, "Invalid argument.");

        if (buf->iov.iov_len != len)
            luaL_argerror(L, narg, "Invalid argument.");

        return (buf->iov.iov_base);
    }
    return (luab_toudata(L, narg, m));
}

void *
luab_checkudataisnil(lua_State *L, int narg, luab_module_t *m)
{
    if (lua_isnil(L, narg) != 0)
        return (NULL);

    return ((*m->get)(L, narg));
}

const char *
luab_iovec_islxarg(lua_State *L, int narg, size_t len)
{
    luab_iovec_t *buf;

    if (((buf = luab_isiovec(L, narg)) != NULL) &&
        (buf->iov_flags & (IOV_BUFF|IOV_PROXY)) &&
        (len <= buf->iov_max_len))
        return (buf->iov.iov_base);

    return (luab_islstring(L, narg, len));
}

const char *
luab_iovec_checklxarg(lua_State *L, int narg, size_t len)
{
    const char *buf;

    if ((buf = luab_iovec_islxarg(L, narg, len)) == NULL)
        luaL_argerror(L, narg, "Invalid argument");

    return (buf);
}

/*
 * Service primitives subset of <core>.
 */

int
luab_dump(lua_State *L, int narg, luab_module_t *m, size_t len)
{
    caddr_t data;

    (void)luab_checkmaxargs(L, narg);
    data = (caddr_t)(*m->get)(L, narg);

    return (luab_pushliovec(L, data, len, len));
}

int
luab_gc(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *self;

    (void)luab_checkmaxargs(L, narg);

    self = luab_todata(L, narg, m, luab_udata_t *);

    (void)memset_s(self, m->sz, 0, m->sz);

    return (0);
}

int
luab_tostring(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *self;

    (void)luab_checkmaxargs(L, narg);

    self = luab_todata(L, narg, m, luab_udata_t *);
    lua_pushfstring(L, "%s (%p)", m->name, self);

    return (1);
}

#define LUABSD_CORE_LIB_ID    1595987973
#define LUABSD_CORE_LIB_KEY   "core"

/*
 * Interface against uuidgen(2), derived from implementation of uuidgen(1).
 */

static int
luab_uuid(lua_State *L)
{
    uuid_t uuid;
    caddr_t buf;
    int status;

    (void)luab_checkmaxargs(L, 0);

    if ((status = uuidgen(&uuid, 1)) != 0)
        status = luab_pusherr(L, status);
    else {
        uuid_to_string(&uuid, &buf, &status);

        if (status == (int)uuid_s_ok) {
            status = luab_pushldata(L, buf, strlen(buf));
            free(buf);
        } else {
            errno = ENOMEM;
            status = luab_pusherr(L, status);
        }
    }
    return (status);
}

static luab_table_t luab_core_vec[] = {
    LUABSD_FUNC("uuid",    luab_uuid),
    LUABSD_FUNC("CreateHook",   luab_CreateHook),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_core_lib = {
    .cookie = LUABSD_CORE_LIB_ID,
    .name = LUABSD_CORE_LIB_KEY,
    .vec = luab_core_vec,
};
