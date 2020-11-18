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
#include <unistd.h>
#include <uuid.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_modules.h"
#include "luab_udata.h"

#define LUAB_CORE_LIB_ID    1595987973
#define LUAB_CORE_LIB_KEY   "core"

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
luab_core_freestr(caddr_t dp)
{
    if (dp != NULL)
        luab_core_free(dp, strnlen(dp, luab_param_buf_max));
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

    luaL_argerror(L, narg, strerror(up_call));
}

int
luab_core_checkmaxargs(lua_State *L, int nmax)
{
    int narg;

    if ((narg = lua_gettop(L)) > nmax)
        luaL_error(L, "#%d args, but #%d expected", narg, nmax);

    return (narg);
}

size_t
luab_core_sysconf(int name, size_t x)
{
    ssize_t sc;
    size_t value;

    if ((sc = sysconf(name)) < 0)
        value = x;
    else
        value = (size_t)sc;

    return (value);
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

lua_Integer
luab_checklinteger(lua_State *L, int narg)
{
    lua_Integer b_msk;

    b_msk = (lua_Integer)(
#if defined(__LP64__) || defined(__mips_n64)
    LONG_MAX
#else
    INT_MAX
#endif
    );
    return (luab_checkinteger(L, narg, b_msk));
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

const char *
luab_checklstringisnil(lua_State *L, int narg, size_t max_len)
{
    if (lua_isnil(L, narg) != 0)
        return (NULL);

    return (luab_checklstring(L, narg, max_len));
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
    char buf[luab_param_buf_max];

    va_start(ap, fmt);
    (void)vsnprintf(buf, luab_param_buf_max, fmt, ap);
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
        (len < luab_param_buf_max)) {
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
    char buf[luab_param_buf_max];

    va_start(ap, fmt);
    (void)vsnprintf(buf, luab_param_buf_max, fmt, ap);
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
        (len < luab_param_buf_max)) {
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
        len = strnlen(dp, luab_param_buf_max);
        lua_pushlstring(L, dp, len);

        if (up_call != 0) {
            lua_pushinteger(L, up_call);
            msg = strerror(up_call);
            lua_pushstring(L, msg);
            status = 3;
        } else
            status = 1;
    } else {
        errno = ENOENT;
        status = luab_pushnil(L);
    }
    return (status);
}

int
luab_pushfstring(lua_State *L, const char *fmt, ...)
{
    char buf[luab_param_buf_max];
    va_list ap;

    va_start(ap, fmt);
    (void)vsnprintf(buf, luab_param_buf_max, fmt, ap);
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
        (len < luab_param_buf_max)) {
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
 * Common service primitives.
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

/*
 * Interface of <core>.
 */

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
