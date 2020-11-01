/*
 * Copyright (c) 2020 Henning Matyschok
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

#include <err.h>
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
#include "luab_udata.h"

#define LUAB_CORE_LIB_ID    1595987973
#define LUAB_CORE_LIB_KEY   "core"

LUAMOD_API int  luaopen_bsd(lua_State *);

/*
 * Generic service primitives, subset of <core>.
 */

void
luab_core_free(void *v, size_t sz)
{
    if (v != NULL) {
        if (sz > 0)
            (void)memset_s(v, sz, 0, sz);

        free(v);
        errno = EEXIST;
    } else
        errno = ENOENT;
}

void
luab_core_errx(int eval, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    verrx(eval, fmt, ap);
    va_end(ap);
}

void
luab_core_err(int eval, const char *fname, int up_call)
{
    err(eval, "%s: %s", fname, strerror(up_call));
}

void
luab_core_warn(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vwarn(fmt, ap);
    va_end(ap);
}

void
luab_core_argerror(lua_State *L, int narg, void *v, size_t n, size_t sz, int up_call)
{
    size_t len;

    if ((len = n * sz) != 0)
        luab_core_free(v, len);

    errno = up_call;
    luaL_argerror(L, narg, strerror(up_call));
}

int
luab_core_checkmaxargs(lua_State *L, int nmax)
{
    int narg;

    if ((narg = lua_gettop(L)) > nmax)  /* XXX */
        luaL_error(L, "#%d args, but #%d expected", narg, nmax);

    return (narg);
}

/*
 * Access functions, n-th arg over argv, [stack -> C].
 *
 * luab_check{l}xxx(3) accessor evaluates, if n-th arg exists, otherwise
 * lua_error will be thrown. Finally luab_{is,to}{l}xxx(3) does the same
 * thing without throwing an error, but return NULL, if n-th arg does
 * not exist.
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

    if (len > max_len)
        luab_core_argerror(L, narg, NULL, 0, 0, ERANGE);

    return (dp);
}

/*
 * Access functions, [C -> stack].
 */

void
luab_rawsetinteger(lua_State *L, int narg, lua_Integer k, lua_Integer v)
{
    lua_pushinteger(L, v);
    lua_rawseti(L, narg, k);
}

void
luab_rawsetnumber(lua_State *L, int narg, lua_Integer k, lua_Number v)
{
    lua_pushnumber(L, v);
    lua_rawseti(L, narg, k);
}

void
luab_rawsetstring(lua_State *L, int narg, lua_Integer k, const char *v)
{
    lua_pushstring(L, v);
    lua_rawseti(L, narg, k);
}

void
luab_rawsetfstring(lua_State *L, int narg, lua_Integer k, const char *fmt, ...)
{
    va_list ap;
    char buf[LUAL_BUFFERSIZE];

    va_start(ap, fmt);
    (void)vsnprintf(buf, LUAL_BUFFERSIZE, fmt, ap);
    va_end(ap);

    lua_pushstring(L, buf);
    lua_rawseti(L, narg, k);
}

void
luab_rawsetldata(lua_State *L, int narg, lua_Integer k, void *v, size_t len)
{
    luaL_Buffer b;
    caddr_t dp;

    if ((v != NULL) &&
        (len > 1) &&
        (len < LUAL_BUFFERSIZE)) {
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
luab_setfstring(lua_State *L, int narg, const char *k, const char *fmt, ...)
{
    va_list ap;
    char buf[LUAL_BUFFERSIZE];

    va_start(ap, fmt);
    (void)vsnprintf(buf, LUAL_BUFFERSIZE, fmt, ap);
    va_end(ap);

    lua_pushstring(L, buf);
    lua_setfield(L, narg, k);
}

void
luab_setldata(lua_State *L, int narg, const char *k, void *v, size_t len)
{
    luaL_Buffer b;
    caddr_t dp;

    if ((v != NULL) &&
        (len > 1) &&
        (len < LUAL_BUFFERSIZE)) {
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        lua_setfield(L, narg, k);
    }
}

int
luab_pusherr(lua_State *L, lua_Integer res)
{
    int up_call;
    caddr_t msg;
    int status;

    up_call = errno;
    lua_pushinteger(L, res);

    if (up_call != 0 && res < 0) {
        lua_pushinteger(L, up_call);
        msg = strerror(up_call);
        lua_pushstring(L, msg);
        status = 3;
    } else if (up_call == res) {
        msg = strerror(up_call);
        lua_pushstring(L, msg);
        status = 2;
    } else
        status = 1;

    return (status);
}

int
luab_pushnumber(lua_State *L, lua_Number res)
{
    int up_call;
    caddr_t msg;
    int status;

    up_call = errno;
    lua_pushnumber(L, res);

    if (up_call != 0 && res < 0) {
        lua_pushinteger(L, up_call);
        msg = strerror(up_call);
        lua_pushstring(L, msg);
        status = 3;
    } else if (up_call == res) {
        msg = strerror(up_call);
        lua_pushstring(L, msg);
        status = 2;
    } else
        status = 1;

    return (status);
}

int
luab_pushnil(lua_State *L)
{
    int up_call;
    caddr_t msg;
    int status;

    up_call = errno;
    lua_pushnil(L);

    if (up_call != 0) {
        lua_pushinteger(L, up_call);
        msg = strerror(up_call);
        lua_pushstring(L, msg);
        status = 3;
    } else
        status = 1;

    return (status);
}

int
luab_pushstring(lua_State *L, const char *dp)
{
    int up_call;
    caddr_t msg;
    size_t len;
    int status;

    up_call = errno;

    if (dp != NULL) {
        len = strnlen(dp, LUAL_BUFFERSIZE);
        lua_pushlstring(L, dp, len);

        if (up_call != 0) {
            lua_pushinteger(L, up_call);
            msg = strerror(up_call);
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
luab_pushfstring(lua_State *L, const char *fmt, ...)
{
    char buf[LUAL_BUFFERSIZE];
    va_list ap;

    va_start(ap, fmt);
    (void)vsnprintf(buf, LUAL_BUFFERSIZE, fmt, ap);
    va_end(ap);

    return (luab_pushstring(L, buf));
}

int
luab_pushldata(lua_State *L, void *v, size_t len)
{
    int up_call;
    luaL_Buffer b;
    caddr_t dp, msg;
    int status;

    up_call = errno;

    if ((v != NULL) &&
        (len > 1) &&
        (len < LUAL_BUFFERSIZE)) {
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        if (up_call != 0) {
            lua_pushinteger(L, up_call);
            msg = strerror(up_call);
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
 * Common service primitives, interface of <core>.
 */

int
luab_core_create(lua_State *L, int narg, luab_module_t *m0, luab_module_t *m1)
{
    luab_module_t *m;
    caddr_t arg;

    if ((m = (m1 != NULL) ? m1 : m0) != NULL) {
        if (luab_core_checkmaxargs(L, narg) == 0)
            arg = NULL;
        else
            arg = luab_udata(L, narg, m, caddr_t);
    } else
        arg = NULL;

    return (luab_pushudata(L, m0, arg));
}

int
luab_core_dump(lua_State *L, int narg, luab_module_t *m, size_t len)
{
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, narg);

    if (m != NULL && m->m_get != NULL)
        dp = (caddr_t)(*m->m_get)(L, narg);
    else
        dp = NULL;

    return (luab_iovec_pushudata(L, dp, len, len));
}

int
luab_core_gc(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *self, *ud, *ud_tmp;

    (void)luab_core_checkmaxargs(L, narg);

    self = luab_todata(L, narg, m, luab_udata_t *);

    LIST_FOREACH_SAFE(ud, &self->ud_list, ud_next, ud_tmp)
        luab_udata_remove(ud);

    if (self->ud_xhd != NULL)
        luab_udata_remove(self);

    (void)memset_s(self, m->m_sz, 0, m->m_sz);

    return (0);
}

int
luab_core_len(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *ud;
    ssize_t len;

    (void)luab_core_checkmaxargs(L, narg);

    if ((ud = luab_todata(L, narg, m, luab_udata_t *)) != NULL)
        len = luab_xlen(m);
    else
        len = -1;

    return (luab_pusherr(L, len));
}

int
luab_core_tostring(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *ud;

    (void)luab_core_checkmaxargs(L, narg);

    if ((ud = luab_todata(L, narg, m, luab_udata_t *)) != NULL)
        lua_pushfstring(L, "%s (%p,%d)", m->m_name, ud, ud->ud_ts);

    return (1);
}

/***
 * Interface against uuidgen(2), derived from implementation of uuidgen(1).
 *
 * @function uuid
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
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

    (void)luab_core_checkmaxargs(L, 0);

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
 * Generator function - create an instance of (LUA_TUSERDATA(PRIMITIVE)).
 *
 * @function primitive_create
 *
 * @param data          Instance of (LUA_TUSERDATA(PRIMITIVE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (primitive [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage primitive [, err, msg ] = bsd.core.primitive_create([ data ])
 */
static int
luab_primitive_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(PRIMITIVE), NULL));
}

#if LUAB_DEBUG
/***
 * Generator function - create an instance of (LUA_TUSERDATA(LINK)).
 *
 * @function link_create
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (link [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage link [, err, msg ] = bsd.core.link_create()
 */
static int
luab_link_create(lua_State *L)
{
    return (luab_core_create(L, 0, luab_mx(LINK), NULL));
}
#endif /* LUAB_DEBUG */

static luab_module_table_t luab_core_util_vec[] = {
    LUAB_FUNC("uuid",               luab_uuid),
    LUAB_FUNC("primitive_create",   luab_primitive_create),
#if LUAB_DEBUG
    LUAB_FUNC("link_create",        luab_link_create),
#endif
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_core_lib = {
    .m_cookie   = LUAB_CORE_LIB_ID,
    .m_name     = LUAB_CORE_LIB_KEY,
    .m_vec      = luab_core_util_vec,
};

static const char *copyright =
    " Copyright (c) 2020 Henning Matyschok\n"
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
    luab_module_table_t *tok;

    for (tok = m->m_vec; tok->mt_key != NULL; tok++) {
        (void)(*tok->mt_init)(L, &tok->mt_val);
        lua_setfield(L, narg, tok->mt_key);
    }
    lua_pop(L, 0);
}

static void
luab_newtable(lua_State *L, int narg, luab_module_t *m)
{
    lua_newtable(L);
    luab_populate(L, narg, m);
    lua_setfield(L, narg, m->m_name);
}

static void
luab_newmetatable(lua_State *L, int narg, luab_module_t *m)
{
    luaL_newmetatable(L, m->m_name);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luab_populate(L, narg, m);

    lua_pop(L, 1);
}

/*
 * Vector-table set, provides meta-information for Lua bindings.
 */

/* Interface against <arpa/xxx.h>. */
static luab_module_vec_t luab_arpa_vec[] = {
    {
        .mv_mod = &luab_arpa_inet_lib,
        .mv_init = luab_newtable,
    },
    LUAB_MOD_VEC_SENTINEL
};

/* Interface against <net/xxx.h>. */
static luab_module_vec_t luab_net_vec[] = {
    {
        .mv_mod = &luab_net_if_dl_lib,
        .mv_init = luab_newtable,
    },{
        .mv_mod = &luab_net_if_lib,
        .mv_init = luab_populate,
    },
    LUAB_MOD_VEC_SENTINEL
};

/* Interface against <sys/xxx.h>. */
static luab_module_vec_t luab_sys_vec[] = {
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
    },
    LUAB_MOD_VEC_SENTINEL
};

/* Interface against <xxx.h> or <core>. */
static luab_module_vec_t luab_core_vec[] = {
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
    },
    LUAB_MOD_VEC_SENTINEL
};

/* Bindings against complex data types. */
luab_module_vec_t luab_typevec[] = {
#if LUAB_DEBUG
    {
        .mv_mod = &link_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_LINK_IDX,
    },
#endif /* LUAB_DEBUG */
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
        .mv_mod = &primitive_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_PRIMITIVE_IDX,
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
    },{
        .mv_mod = &accept_filter_arg_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_ACCEPT_FILTER_ARG_IDX,
    },{
        .mv_mod = &sockproto_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_SOCKPROTO_IDX,
    },{
        .mv_mod = &cmsgcred_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_CMSGCRED_IDX,
    },{
        .mv_mod = &sf_hdtr_type,
        .mv_init = luab_newmetatable,
        .mv_idx = LUAB_SF_HDTR_IDX,
    },
#endif  /* __BSD_VISIBLE */
    LUAB_MOD_VEC_SENTINEL
};

/*
 * Reflects and maps interface against API over </include/>.
 */

static void
luab_initmodule(lua_State *L, int narg, luab_module_vec_t *vec,
    const char *name, int new)
{
    luab_module_vec_t *mv;

    if (name != NULL && new != 0)
        lua_newtable(L);

    for (mv = vec; mv->mv_mod != NULL; mv++)
        (*mv->mv_init)(L, narg, mv->mv_mod);

    if (name != NULL && new != 0)
        lua_setfield(L, narg, name);
}

static void
luab_registerlib(lua_State *L, int narg, luab_module_vec_t *vec, const char *name)
{
    luab_initmodule(L, narg, vec, name, 1);
}

static void
luab_registertype(lua_State *L, int narg, luab_module_vec_t *vec)
{
    luab_initmodule(L, narg, vec, NULL, 0);
}

LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    (void)printf("%s", copyright);

    lua_newtable(L);

    luab_registerlib(L, -2, luab_arpa_vec,  "arpa");
    luab_registerlib(L, -2, luab_net_vec,   "net");
    luab_registerlib(L, -2, luab_sys_vec,   "sys");
    luab_registerlib(L, -2, luab_core_vec,  NULL);

    lua_pushvalue(L, -1);

    /* register complex data-types. */
    luab_registertype(L, -2, luab_typevec);

    return (1);
}
