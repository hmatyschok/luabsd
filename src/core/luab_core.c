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
extern luab_module_t div_type;
extern luab_module_t flock_type;
extern luab_module_t hook_type;
extern luab_module_t if_nameindex_type;
extern luab_module_t in_addr_type;
extern luab_module_t in6_addr_type;
extern luab_module_t itimerval_type;
extern luab_module_t ldiv_type;
extern luab_module_t linger_type;
extern luab_module_t msghdr_type;
extern luab_module_t sockaddr_type;
extern luab_module_t stat_type;
extern luab_module_t timespec_type;
extern luab_module_t timeval_type;
extern luab_module_t timezone_type;
extern luab_module_t tm_type;
extern luab_module_t uuid_type;

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
extern luab_module_t luab_sys_reboot_lib;
extern luab_module_t luab_sys_socket_lib;

extern luab_module_t luab_core_lib;

extern luab_module_t luab_db_lib;
extern luab_module_t luab_fcntl_lib;
extern luab_module_t luab_stdlib_lib;
extern luab_module_t luab_time_lib;
extern luab_module_t luab_unistd_lib;
extern luab_module_t luab_uuid_lib;

#define LUABSD_CORE_LIB_ID    1595987973
#define LUABSD_CORE_LIB_KEY   "core"

LUAMOD_API int  luaopen_bsd(lua_State *);

/*
 * Generic operations on atomic (or primitive) data types.
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
luab_islstring(lua_State *L, int narg, size_t len)
{
    const char *dp;
    size_t n;

    if ((dp = luaL_tolstring(L, narg, &n)) != NULL) {
        if (n <= len)
            return (dp);
    }
    return (NULL);
}

const char *
luab_tolstring(lua_State *L, int narg, size_t len)
{
    const char *dp;
    size_t n;

    if ((dp = luaL_tolstring(L, narg, &n)) != NULL) {
        if (n == len)
            return (dp);
    }
    return (NULL);
}

const char *
luab_checklstring(lua_State *L, int narg, size_t max_len)
{
    const char *dp;
    size_t len;

    dp = luaL_checklstring(L, narg, &len);

    if (len > max_len)    /* XXX err_msg */
        luaL_argerror(L, narg, "Value too large to be stored in data type");

    return (dp);
}

/*
 * Generic operations on stack.
 */

int
luab_pusherr(lua_State *L, lua_Integer res)
{
    int save_errno = errno;
    caddr_t msg;
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
    caddr_t msg;
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
luab_pushstring(lua_State *L, const char *dp)
{
    int save_errno = errno;
    caddr_t msg;
    size_t len;
    int status;

    if (dp != NULL) {
        len = strnlen(dp, LUAL_BUFFERSIZE);
        lua_pushlstring(L, dp, len);

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
luab_pushldata(lua_State *L, void *v, size_t len)
{
    int save_errno = errno;
    luaL_Buffer b;
    caddr_t dp, msg;
    int status;

    if (v != NULL && len > 1) {
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

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
luab_rawsetstring(lua_State *L, int narg, lua_Integer k, const char *v)
{
    lua_pushstring(L, v);
    lua_rawseti(L, narg, k);
}

void
luab_rawsetldata(lua_State *L, int narg, lua_Integer k, void *v, size_t len)
{
    luaL_Buffer b;
    caddr_t dp;

    if (v != NULL && len > 1) {  /* XXX redundant code-section */
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        lua_rawseti(L, narg, k);
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
luab_setldata(lua_State *L, int narg, const char *k, void *v, size_t len)
{
    luaL_Buffer b;
    caddr_t dp;

    if (v != NULL && len > 1) { /* XXX redundant code-section */
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        lua_setfield(L, narg, k);
    }
}

/*
 * Generic service primitives, subset of <core>.
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
luab_create(lua_State *L, int narg, luab_module_t *m0, luab_module_t *m1)
{
    caddr_t arg;

    if ((narg = luab_checkmaxargs(L, narg)) == 0)
        arg = NULL;
    else
        arg = luab_udata(L, narg, m1, caddr_t);

    return (luab_pushudata(L, m0, arg));
}

int
luab_dump(lua_State *L, int narg, luab_module_t *m, size_t len)
{
    caddr_t dp;

    (void)luab_checkmaxargs(L, narg);

    if (m != NULL && m->get != NULL)
        dp = (caddr_t)(*m->get)(L, narg);
    else
        dp = NULL;

    return (luab_pushiovec(L, dp, len, len));
}

int
luab_gc(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *self;

    (void)luab_checkmaxargs(L, narg);

    if ((self = luab_todata(L, narg, m, luab_udata_t *)) != NULL)
        (void)memset_s(self, m->sz, 0, m->sz);

    return (0);
}

int
luab_tostring(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *self;

    (void)luab_checkmaxargs(L, narg);

    if ((self = luab_todata(L, narg, m, luab_udata_t *)) != NULL)
        lua_pushfstring(L, "%s (%p)", m->name, self);

    return (1);
}

/***
 * Interface against uuidgen(2), derived from implementation of uuidgen(1).
 *
 * @function uuid
 *
 * @return (LUA_TSTRING [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (uuid [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage uuid [, err, msg ] = bsd.core.uuid()
 */
static int
luab_uuid(lua_State *L)
{
    uuid_t uuid;
    caddr_t buf;
    int status;

    (void)luab_checkmaxargs(L, 0);

    if ((status = uuidgen(&uuid, 1)) != 0)
        status = luab_pushnil(L);
    else {
        uuid_to_string(&uuid, &buf, &status);

        if (status == (int)uuid_s_ok) {
            status = luab_pushldata(L, buf, strlen(buf));
            free(buf);
        } else {
            errno = ENOMEM;
            status = luab_pushnil(L);
        }
    }
    return (status);
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(HOOK)).
 *
 * @function hook_create
 *
 * @param data          Instance of (LUA_TUSERDATA(HOOK)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (hook [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage hook [, err, msg ] = bsd.core.hook_create([ data ])
 */
static int
luab_hook_create(lua_State *L)
{
    luab_type_u *data;
    int narg;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        data = NULL;
    else
        data = luab_udata(L, narg, &hook_type, luab_type_u *);

    return (luab_pushudata(L, &hook_type, data));
}

static luab_table_t luab_core_vec[] = {
    LUABSD_FUNC("uuid",    luab_uuid),
    LUABSD_FUNC("hook_create",   luab_hook_create),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_core_lib = {
    .cookie = LUABSD_CORE_LIB_ID,
    .name = LUABSD_CORE_LIB_KEY,
    .vec = luab_core_vec,
};

/*
 * Common subr. for initializiation, those are
 * called during runtime of package.loadlib().
 */

static const char *copyright =
    " Copyright (c) 2020 Henning Matyschok <hmatyschok@outlook.com>\n"
    " All rights reserved.\n"
    "\n"
    "  The implementation of the interface against alarm(3) and setitimer(2)\n"
    "  is derived from:\n"
    "\n"
    "   lalarm.c\n"
    "   an alarm library for Lua based on signal(3)\n"
    "   Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>\n"
    "   28 Jul 2018 12:47:52\n\n"
    "\n";

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
    (void)printf("%s", copyright);

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
    luab_newtable(L, -2, &luab_sys_reboot_lib);
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
    luab_newmetatable(L, &div_type);
    luab_newmetatable(L, &flock_type);
    luab_newmetatable(L, &hook_type);
    luab_newmetatable(L, &if_nameindex_type);
    luab_newmetatable(L, &in_addr_type);
    luab_newmetatable(L, &in6_addr_type);
    luab_newmetatable(L, &iovec_type);
    luab_newmetatable(L, &itimerval_type);
    luab_newmetatable(L, &ldiv_type);
    luab_newmetatable(L, &linger_type);
    luab_newmetatable(L, &msghdr_type);
    luab_newmetatable(L, &sockaddr_type);
    luab_newmetatable(L, &stat_type);
    luab_newmetatable(L, &timezone_type);
    luab_newmetatable(L, &timespec_type);
    luab_newmetatable(L, &timeval_type);
    luab_newmetatable(L, &uuid_type);
#if __BSD_VISIBLE
    luab_newmetatable(L, &bintime_type);
    luab_newmetatable(L, &crypt_data_type);
    luab_newmetatable(L, &dbt_type);
    luab_newmetatable(L, &db_type);
#endif
    return (1);
}
