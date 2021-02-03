/*
 * Copyright (c) 2020, 2021 Henning Matyschok
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

#include <net/if.h> /* XXX */
#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

/*
 * Service primitives.
 */

lua_Integer
luab_core_Integer_promotion_msk(int s)
{
    lua_Integer b_msk;

    b_msk = (s != 0) ? (lua_Integer)(
#if defined(__LP64__) || defined(__mips_n64)
    luab_env_ulong_max
#else
    luab_env_uint_max
#endif
    ) : (lua_Integer)(
#if defined(__LP64__) || defined(__mips_n64)
    luab_env_long_max
#else
    luab_env_int_max
#endif
    );
    return (b_msk);
}

/*
 * Access functions, n-th arg over argv, [stack -> C].
 *
 * luab_check{l}xxx(3) accessor evaluates, if n-th arg exists, otherwise
 * lua_error will be thrown. Finally luab_{is,to}{l}xxx(3) does the same
 * thing without throwing an error, but return NULL, if n-th arg does
 * not exist.
 */

void *
luab_checknil(lua_State *L, int narg)
{
    if (lua_isnil(L, narg) == 0)
        luab_core_argerror(L, narg, NULL, 0, 0, EINVAL);

    return (NULL);
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

    b_msk = luab_core_Integer_promotion_msk(s);
    return (luab_tointeger(L, narg, b_msk));
}

lua_Integer
luab_toxinteger(lua_State *L, int narg, luab_module_t *m, lua_Integer b_msk)
{
    lua_Integer *xp;

    if (m != NULL) {

        if (lua_isnumber(L, narg) != 0)
            return (luab_tointeger(L, narg, b_msk));

        xp = luab_udataisnil(L, 1, m, lua_Integer *);

        if (xp != NULL)
            return (*xp & b_msk);
    } else
        errno = ENOSYS;

    return (0);
}

lua_Integer
luab_tolxinteger(lua_State *L, int narg, luab_module_t *m, int s)
{
    lua_Integer *xp, b_msk;

    if (m != NULL) {

        if (lua_isnumber(L, narg) != 0)
            return (luab_tolinteger(L, narg, s));

        xp = luab_udataisnil(L, 1, m, lua_Integer *);

        if (xp != NULL) {
            b_msk = luab_core_Integer_promotion_msk(s);
            return (*xp & b_msk);
        }
    } else
        errno = ENOSYS;

    return (0);
}

lua_Integer
luab_checkinteger(lua_State *L, int narg, lua_Integer b_msk)
{
    return ((luaL_checkinteger(L, narg)) & (b_msk));
}

lua_Integer
luab_checklinteger(lua_State *L, int narg, int s)
{
    lua_Integer b_msk;

    b_msk = luab_core_Integer_promotion_msk(s);
    return (luab_checkinteger(L, narg, b_msk));
}

lua_Integer
luab_checkxinteger(lua_State *L, int narg, luab_module_t *m, lua_Integer b_msk)
{
    lua_Integer *xp;

    if (m != NULL) {

        if (lua_isnumber(L, narg) != 0)
            return (luab_checkinteger(L, narg, b_msk));

        xp = luab_udataisnil(L, 1, m, lua_Integer *);

        if (xp != NULL)
            return (*xp & b_msk);
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ENOSYS);

    return (0);
}

lua_Integer
luab_checklxinteger(lua_State *L, int narg, luab_module_t *m, int s)
{
    lua_Integer *xp, b_msk;

    if (m != NULL) {

        if (lua_isnumber(L, narg) != 0)
            return (luab_checklinteger(L, narg, s));

        xp = luab_udataisnil(L, 1, m, lua_Integer *);

        if (xp != NULL) {
            b_msk = luab_core_Integer_promotion_msk(s);
            return (*xp & b_msk);
        }
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ENOSYS);

    return (0);
}

lua_Number
luab_toxnumber(lua_State *L, int narg, luab_module_t *m)
{
    lua_Number *xp;

    if (m != NULL) {

        if (lua_isnumber(L, narg) != 0)
            return (lua_tonumber(L, narg));

        xp = luab_udataisnil(L, 1, m, lua_Number *);

        if (xp != NULL)
            return (*xp);
    } else
        errno = ENOSYS;

    return (0.0);
}

lua_Number
luab_checkxnumber(lua_State *L, int narg, luab_module_t *m)
{
    lua_Number *xp;

    if (m != NULL) {

        if (lua_isnumber(L, narg) != 0)
            return (luaL_checknumber(L, narg));

        xp = luab_udataisnil(L, 1, m, lua_Number *);

        if (xp != NULL)
            return (*xp);
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ENOSYS);

    return (0.0);
}

const char *
luab_islstring(lua_State *L, int narg, size_t nmax)
{
    const char *dp;
    size_t n;

    if ((dp = luaL_tolstring(L, narg, &n)) != NULL) {
        if (n <= nmax && nmax <= luab_env_buf_max)
            return (dp);
    }
    return (NULL);
}

const char *
luab_tolstring(lua_State *L, int narg, size_t nmax)
{
    const char *dp;
    size_t n;

    if ((dp = luaL_tolstring(L, narg, &n)) != NULL) {
        if (n == nmax && nmax <= luab_env_buf_max)
            return (dp);
    }
    return (NULL);
}

const char *
luab_checklstring(lua_State *L, int narg, size_t nmax, size_t *np)
{
    const char *dp;
    size_t n;

    dp = luaL_checklstring(L, narg, &n);

    if (n <= nmax && nmax <= luab_env_buf_max) {

        if (np != NULL)
            *np = n;

        return (dp);
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ERANGE);

    return (NULL);
}

const char *
luab_checklstringisnil(lua_State *L, int narg, size_t nmax, size_t *np)
{
    if (lua_isnil(L, narg) != 0) {

        if (np != NULL)
            *np = 0;

        return (NULL);
    }
    return (luab_checklstring(L, narg, nmax, np));
}

char *
luab_checklstringalloc(lua_State *L, int narg, size_t nmax, size_t *np)
{
    const char *dp;
    size_t len;
    caddr_t bp;

    if ((dp = luab_checklstringisnil(L, narg, nmax, &len)) != NULL) {

        if ((bp = luab_core_allocstring(dp, &len)) == NULL)
            luab_core_argerror(L, narg, NULL, 0, 0, errno);
    } else
        bp = NULL;

    if (np != NULL)
        *np = len;

    return (bp);
}

char *
luab_checklxstring(lua_State *L, int narg, size_t nmax, size_t *np)
{
    luab_module_t *m;
    const char *dp;

    m = luab_xmod(STRING, TYPE, __func__);

    if (lua_isuserdata(L, narg) != 0)
        dp = luab_udata(L, narg, m, const char *);
    else
        dp = luab_checklstringisnil(L, narg, nmax, NULL);

    return (luab_core_allocstring(dp, np));
}

luab_thread_t *
luab_checkfunction(lua_State *L, int narg, const char *fname)
{
    if (lua_type(L, narg) == LUA_TFUNCTION && fname != NULL) {
        lua_settop(L, narg);
        lua_setfield(L, LUA_REGISTRYINDEX, fname);
        return (luab_core_newthread(L, fname));
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, EINVAL);

    return (NULL);
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
        (len < (luab_env_buf_max + 1))) {
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
        (len < (luab_env_buf_max + 1))) {
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        lua_setfield(L, narg, k);
    }
}

int
luab_pusherr(lua_State *L, int up_call, int ret)
{
    caddr_t msg;
    int status;

    if (up_call != 0) {
        lua_pushinteger(L, up_call);
        msg = strerror(up_call);
        lua_pushstring(L, msg);
        status = 2 + ret;
    } else
        status = ret;

    return (status);
}

int
luab_pushinteger(lua_State *L, lua_Integer x)
{
    int up_call;

    up_call = errno;
    lua_pushinteger(L, x);

    return (luab_pusherr(L, up_call, 1));
}

int
luab_pushnumber(lua_State *L, lua_Number x, int err)
{
    int up_call;

    if ((up_call = errno) != 0 && err != 0)
        lua_pushinteger(L, luab_env_error);
    else
        lua_pushnumber(L, x);

    return (luab_pusherr(L, up_call, 1));
}

int
luab_pushnil(lua_State *L)
{
    int up_call;

    up_call = errno;
    lua_pushnil(L);

    return (luab_pusherr(L, up_call, 1));
}

int
luab_pushstring(lua_State *L, const char *dp)
{
    int up_call;
    size_t n;
    int status;

    up_call = errno;

    if (dp != NULL) {
        n = strnlen(dp, luab_env_buf_max);
        lua_pushlstring(L, dp, n);

        status = luab_pusherr(L, up_call, 1);
    } else {
        errno = (up_call != 0) ? up_call : ENOENT;
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
    caddr_t dp;
    int status;

    up_call = errno;

    if ((v != NULL) &&
        (len > 1) &&
        (len < (luab_env_buf_max + 1))) {
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        status = luab_pusherr(L, up_call, 1);
    } else {
        errno = (up_call != 0) ? up_call : EINVAL;
        status = luab_pushnil(L);
    }
    return (status);
}
