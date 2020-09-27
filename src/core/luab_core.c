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
#include "luab_modules.h"
#include "luab_types.h"

#define LUABSD_CORE_LIB_ID    1595987973
#define LUABSD_CORE_LIB_KEY   "core"

LUAMOD_API int  luaopen_bsd(lua_State *);

/*
 * Generic operations on atomic (or primitive) data types, [stack -> C].
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
 * Generic operations on stack, [C -> stack].
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
luab_pushnumber(lua_State *L, lua_Number res)
{
    int save_errno = errno;
    caddr_t msg;
    int status;

    lua_pushnumber(L, res);

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

/*
 * Common service primitives, subset of <core>.
 */

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

static luab_table_t luab_core_util_vec[] = {
    LUABSD_FUNC("uuid",         luab_uuid),
    LUABSD_FUNC("hook_create",  luab_hook_create),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_core_lib = {
    .cookie = LUABSD_CORE_LIB_ID,
    .name = LUABSD_CORE_LIB_KEY,
    .vec = luab_core_util_vec,
};

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
luab_newmetatable(lua_State *L, int narg, luab_module_t *m)
{
    luaL_newmetatable(L, m->name);  /* XXX */
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luab_populate(L, narg, m);

    lua_pop(L, 1);
}

/*
 * Vector-table set, provides meta-information for Lua bindings.
 */

/* Interface against <arpa/xxx.h>. */
static luab_modulevec_t luab_arpa_vec[] = {
    {
        .mv_mod = &luab_arpa_inet_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = NULL,
        .mv_init = NULL,
        .mv_idx = 0,
    }
};

/* Interface against <if/xxx.h>. */
static luab_modulevec_t luab_net_vec[] = {
    {
        .mv_mod = &luab_net_if_dl_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_net_if_lib,
        .mv_init = luab_populate,
    },{
        .mv_mod = NULL,
        .mv_init = NULL,
        .mv_idx = 0,
    }
};

/* Interface against <sys/xxx.h>. */
static luab_modulevec_t luab_sys_vec[] = {
    {
        .mv_mod = &luab_sys_file_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_sys_stat_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_sys_time_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_sys_uio_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_sys_un_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_sys_unistd_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_sys_reboot_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_sys_socket_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = NULL,
        .mv_init = NULL,
        .mv_idx = 0,
    }
};

/* Interface against <xxx.h> or <core>. */
static luab_modulevec_t luab_core_vec[] = {
    {
        .mv_mod = &luab_core_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_db_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_fcntl_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_stdlib_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_time_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_unistd_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_uuid_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = NULL,
        .mv_init = NULL,
        .mv_idx = 0,
    }
};

/* Bindings against complex data types. */
luab_modulevec_t luab_typevec[] = {
    {
        .mv_mod = &clockinfo_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_CLOCKINFO_IDX,
    },{
        .mv_mod = &div_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_DIV_IDX,
    },{
        .mv_mod = &flock_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_FLOCK_IDX,
    },{
        .mv_mod = &hook_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_HOOK_IDX,
    },{
        .mv_mod = &if_nameindex_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_IF_NAMEINDEX_IDX,
    },{
        .mv_mod = &in_addr_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_IN_ADDR_IDX,
    },{
        .mv_mod = &in6_addr_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_IN6_ADDR_IDX,
    },{
        .mv_mod = &itimerval_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_ITIMERVAL_IDX,
    },{
        .mv_mod = &ldiv_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_LDIV_IDX,
    },{
        .mv_mod = &lldiv_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_LLDIV_IDX,
    },{
        .mv_mod = &linger_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_LINGER_IDX,
    },{
        .mv_mod = &msghdr_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_MSGHDR_IDX,
    },{
        .mv_mod = &sockaddr_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_SOCKADDR_IDX,
    },{
        .mv_mod = &stat_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_STAT_IDX,
    },{
        .mv_mod = &timespec_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_TIMESPEC_IDX,
    },{
        .mv_mod = &timeval_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_TIMEVAL_IDX,
    },{
        .mv_mod = &timezone_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_TIMEZONE_IDX,
    },{
        .mv_mod = &tm_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_TM_IDX,
    },{
        .mv_mod = &uuid_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_UUID_IDX,
    },{
        .mv_mod = &iovec_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_IOVEC_IDX,
    },
#if __BSD_VISIBLE
    {
        .mv_mod = &dbt_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_DBT_IDX,
    },{
        .mv_mod = &db_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_DB_IDX,
    },{
        .mv_mod = &bintime_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_BINTIME_IDX,
    },{
        .mv_mod = &crypt_data_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_CRYPT_DATA_IDX,
    },{
        .mv_mod = &cap_rbuf_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_CAP_RBUF_IDX,
    },
#endif
    {
        .mv_mod = NULL,
        .mv_init = NULL,
        .mv_idx = 0,
    }
};

/*
 * Reflects and maps interface against API over </include/>.
 */

static void
luab_initmodule(lua_State *L, int narg, luab_modulevec_t *vec,
    const char *name, int new)
{
    luab_modulevec_t *mv;

    if (name != NULL && new != 0)
        lua_newtable(L);

    for (mv = vec; mv->mv_mod != NULL; mv++)
        (*mv->mv_init)(L, narg, mv->mv_mod);

    if (name != NULL && new != 0)
        lua_setfield(L, narg, name);
}

static void
luab_registerlib(lua_State *L, int narg, luab_modulevec_t *vec, const char *name)
{
    luab_initmodule(L, narg, vec, name, 1);
}

static void
luab_registertype(lua_State *L, int narg, luab_modulevec_t *vec)
{
    luab_initmodule(L, narg, vec, NULL, 0);
}

LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    (void)printf("%s", copyright);

    lua_newtable(L);

    luab_registerlib(L, -2, luab_arpa_vec, "arpa");
    luab_registerlib(L, -2, luab_net_vec, "net");
    luab_registerlib(L, -2, luab_sys_vec, "sys");
    luab_registerlib(L, -2, luab_core_vec, NULL);

    lua_pushvalue(L, -1);

    /* register complex data-types. */
    luab_registertype(L, -2, luab_typevec);

    return (1);
}
