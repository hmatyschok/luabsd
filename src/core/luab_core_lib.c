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
        luab_core_free(dp, strnlen(dp, luab_env_buf_max));
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

luab_module_t *
luab_core_checktype(luab_type_t idx, const char *fname)
{
    luab_module_t *m;

    if ((m = luab_xmv(idx).mv_mod) == NULL)
        luab_core_err(EX_UNAVAILABLE, fname, ENOSYS);

    return (m);
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
luab_tolinteger(lua_State *L, int narg, int s)
{
    lua_Integer b_msk;

    b_msk = (s != 0) ? (
#if defined(__LP64__) || defined(__mips_n64)
    luab_env_ulong_max
#else
    luab_env_uint_max
#endif
    ) : (
#if defined(__LP64__) || defined(__mips_n64)
    luab_env_long_max
#else
    luab_env_int_max
#endif
    );
    return (luab_checkinteger(L, narg, b_msk));
}

lua_Integer
luab_checklinteger(lua_State *L, int narg, int s)
{
    lua_Integer b_msk;

    b_msk = (s != 0) ? (
#if defined(__LP64__) || defined(__mips_n64)
    luab_env_ulong_max
#else
    luab_env_uint_max
#endif
    ) : (
#if defined(__LP64__) || defined(__mips_n64)
    luab_env_long_max
#else
    luab_env_int_max
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
    char buf[luab_env_buf_max];

    va_start(ap, fmt);
    (void)vsnprintf(buf, luab_env_buf_max, fmt, ap);
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
        (len < luab_env_buf_max)) {
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
luab_setnumber(lua_State *L, int narg, const char *k, lua_Number v)
{
    lua_pushnumber(L, v);
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
    char buf[luab_env_buf_max];

    va_start(ap, fmt);
    (void)vsnprintf(buf, luab_env_buf_max, fmt, ap);
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
        (len < luab_env_buf_max)) {
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
        len = strnlen(dp, luab_env_buf_max);
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
    char buf[luab_env_buf_max];
    va_list ap;

    va_start(ap, fmt);
    (void)vsnprintf(buf, luab_env_buf_max, fmt, ap);
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
        (len < luab_env_buf_max)) {
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
 * XXX
 *  Generic service primitives, should moved to <luab_core_udata>
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

/* atomic data types */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(CHAR)).
 *
 * @function char_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage char [, err, msg ] = bsd.core.char_create(x)
 */
static int
luab_char_create(lua_State *L)
{
    char x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (char)luab_checkinteger(L, 1, luab_env_uchar_max);

    return (luab_pushudata(L, luab_xtype(CHAR), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(USHRT)).
 *
 * @function ushrt_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ushrt [, err, msg ] = bsd.core.ushrt_create(x)
 */
static int
luab_ushrt_create(lua_State *L)
{
    u_short x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (u_short)luab_checkinteger(L, 1, luab_env_ushrt_max);

    return (luab_pushudata(L, luab_xtype(USHRT), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(UINT)).
 *
 * @function uint_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uint [, err, msg ] = bsd.core.uint_create(x)
 */
static int
luab_uint_create(lua_State *L)
{
    u_int x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (u_int)luab_checkinteger(L, 1, luab_env_uint_max);

    return (luab_pushudata(L, luab_xtype(UINT), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INT)).
 *
 * @function int_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage int [, err, msg ] = bsd.core.int_create(x)
 */
static int
luab_int_create(lua_State *L)
{
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (int)luab_checkinteger(L, 1, luab_env_uint_max);

    return (luab_pushudata(L, luab_xtype(INT), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(LONG)).
 *
 * @function long_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage long [, err, msg ] = bsd.core.long_create(x)
 */
static int
luab_long_create(lua_State *L)
{
    long x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (long)luab_checkinteger(L, 1, luab_env_long_max);

    return (luab_pushudata(L, luab_xtype(LONG), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(WCHAR)).
 *
 * @function wchar_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage wchar [, err, msg ] = bsd.core.wchar_create(x)
 */
static int
luab_wchar_create(lua_State *L)
{
    wchar_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (wchar_t)luab_checkinteger(L, 1, luab_env_uint_max);

    return (luab_pushudata(L, luab_xtype(WCHAR), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(FPOS)).
 *
 * @function fpos_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage fpos [, err, msg ] = bsd.core.fpos_create(x)
 */
static int
luab_fpos_create(lua_State *L)
{
    fpos_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (fpos_t)luab_checkinteger(L, 1, luab_env_long_max);

    return (luab_pushudata(L, luab_xtype(FPOS), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(GID)).
 *
 * @function gid_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage gid [, err, msg ] = bsd.core.gid_create(x)
 */
static int
luab_gid_create(lua_State *L)
{
    gid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (gid_t)luab_checkinteger(L, 1, luab_env_int_max);

    return (luab_pushudata(L, luab_xtype(GID), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(OFF)).
 *
 * @function off_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage off [, err, msg ] = bsd.core.off_create(x)
 */
static int
luab_off_create(lua_State *L)
{
    off_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (off_t)luab_checkinteger(L, 1, luab_env_long_max);

    return (luab_pushudata(L, luab_xtype(OFF), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(SIZE)).
 *
 * @function size_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage size [, err, msg ] = bsd.core.size_create(x)
 */
static int
luab_size_create(lua_State *L)
{
    size_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (size_t)luab_checklinteger(L, 1, 0);

    return (luab_pushudata(L, luab_xtype(SIZE), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(SOCKLEN)).
 *
 * @function socklen_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage socklen [, err, msg ] = bsd.core.socklen_create(x)
 */
static int
luab_socklen_create(lua_State *L)
{
    socklen_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (socklen_t)luab_checkinteger(L, 1, luab_env_int_max);

    return (luab_pushudata(L, luab_xtype(SOCKLEN), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(SSIZE)).
 *
 * @function ssize_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ssize [, err, msg ] = bsd.core.ssize_create(x)
 */
static int
luab_ssize_create(lua_State *L)
{
    ssize_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (ssize_t)luab_checklinteger(L, 1, 1);

    return (luab_pushudata(L, luab_xtype(SSIZE), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(UID)).
 *
 * @function uid_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uid [, err, msg ] = bsd.core.uid_create(x)
 */
static int
luab_uid_create(lua_State *L)
{
    uid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (uid_t)luab_checkinteger(L, 1, luab_env_uid_max);

    return (luab_pushudata(L, luab_xtype(UID), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(DOUBLE)).
 *
 * @function double_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage double [, err, msg ] = bsd.core.double_create(x)
 */
static int
luab_double_create(lua_State *L)
{
    double x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (double)luaL_checknumber(L, 1); /* XXX */

    return (luab_pushudata(L, luab_xtype(DOUBLE), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(FLOAT)).
 *
 * @function float_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage float [, err, msg ] = bsd.core.float_create(x)
 */
static int
luab_float_create(lua_State *L)
{
    float x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (float)luaL_checknumber(L, 1); /* XXX */

    return (luab_pushudata(L, luab_xtype(FLOAT), &x));
}

/* composite data types */
/***
 * Generator function - create an instance of (LUA_TUSERDATA(INTEGER)).
 *
 * @function integer_create
 *
 * @param integer           Instance of (LUA_TUSERDATA(INTEGER)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage integer [, err, msg ] = bsd.core.integer_create([ integer ])
 */
static int
luab_integer_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_xtype(INTEGER), NULL));
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
    return (luab_core_create(L, 0, luab_xtype(LINK), NULL));
}
#endif /* LUAB_DEBUG */

static luab_module_table_t luab_core_vec[] = {
    LUAB_FUNC("uuid",               luab_uuid),
    LUAB_FUNC("char_create",        luab_char_create),
    LUAB_FUNC("ushrt_create",       luab_ushrt_create),
    LUAB_FUNC("uint_create",        luab_uint_create),
    LUAB_FUNC("int_create",         luab_int_create),
    LUAB_FUNC("long_create",        luab_long_create),
    LUAB_FUNC("wchar_create",       luab_wchar_create),
    LUAB_FUNC("fpos_type",          luab_fpos_create),
    LUAB_FUNC("gid_create",         luab_gid_create),
    LUAB_FUNC("off_create",         luab_off_create),
    LUAB_FUNC("size_create",        luab_size_create),
    LUAB_FUNC("socklen_create",     luab_socklen_create),
    LUAB_FUNC("ssize_create",       luab_ssize_create),
    LUAB_FUNC("uid_create",         luab_uid_create),
    LUAB_FUNC("double_create",      luab_double_create),
    LUAB_FUNC("float_create",       luab_float_create),
    LUAB_FUNC("integer_create",     luab_integer_create),
#if LUAB_DEBUG
    LUAB_FUNC("link_create",        luab_link_create),
#endif
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_core_lib = {
    .m_cookie   = LUAB_CORE_LIB_ID,
    .m_name     = LUAB_CORE_LIB_KEY,
    .m_vec      = luab_core_vec,
};
