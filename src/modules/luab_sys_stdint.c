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

#include <sys/stdint.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_SYS_STDINT_LIB_ID    1608394734
#define LUAB_SYS_STDINT_LIB_KEY   "stdint"

extern luab_module_t luab_sys_stdint_lib;

/*
 * Generator functions.
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INT8)).
 *
 * @function int8_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage int8 [, err, msg ] = bsd.sys.stdint.int8_create(x)
 */
static int
luab_int8_create(lua_State *L)
{
    luab_module_t *m;
    int8_t x, *xp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT8, TYPE, __func__);

    if (lua_isnumber(L, 1) != 0) {
        x = (int8_t)luab_checkinteger(L, 1, luab_env_uchar_max);
        xp = &x;
    } else
        xp = luab_udataisnil(L, 1, m, int8_t *);

    return (luab_pushxdata(L, m, xp));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INT16)).
 *
 * @function int16_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage int16 [, err, msg ] = bsd.sys.stdint.int16_create(x)
 */
static int
luab_int16_create(lua_State *L)
{
    luab_module_t *m;
    int16_t x, *xp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT16, TYPE, __func__);

    if (lua_isnumber(L, 1) != 0) {
        x = (int16_t)luab_checkinteger(L, 1, luab_env_ushrt_max);
        xp = &x;
    } else
        xp = luab_udataisnil(L, 1, m, int16_t *);

    return (luab_pushxdata(L, m, xp));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INT32)).
 *
 * @function int32_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage int32 [, err, msg ] = bsd.sys.stdint.int32_create(x)
 */
static int
luab_int32_create(lua_State *L)
{
    luab_module_t *m;
    int32_t x, *xp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT32, TYPE, __func__);

    if (lua_isnumber(L, 1) != 0) {
        x = (int32_t)luab_checkinteger(L, 1, luab_env_uint_max);
        xp = &x;
    } else
        xp = luab_udataisnil(L, 1, m, int32_t *);

    return (luab_pushxdata(L, m, xp));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INT64)).
 *
 * @function int64_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage int64 [, err, msg ] = bsd.sys.stdint.int64_create(x)
 */
static int
luab_int64_create(lua_State *L)
{
    luab_module_t *m;
    int64_t x, *xp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT64, TYPE, __func__);

    if (lua_isnumber(L, 1) != 0) {
        x = (int64_t)luab_checkinteger(L, 1, luab_env_ullong_max);
        xp = &x;
    } else
        xp = luab_udataisnil(L, 1, m, int64_t *);

    return (luab_pushxdata(L, m, xp));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INTPTR)).
 *
 * @function intptr_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage intptr [, err, msg ] = bsd.sys.stdint.intptr_create(x)
 */
static int
luab_intptr_create(lua_State *L)
{
    luab_module_t *m;
    intptr_t x, *xp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INTPTR, TYPE, __func__);

    if (lua_isnumber(L, 1) != 0) {
        x = (intptr_t)luab_checklinteger(L, 1, 1);
        xp = &x;
    } else
        xp = luab_udataisnil(L, 1, m, intptr_t *);

    return (luab_pushxdata(L, m, xp));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(UINTPTR)).
 *
 * @function uintptr_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uintptr [, err, msg ] = bsd.sys.stdint.uintptr_create(x)
 */
static int
luab_uintptr_create(lua_State *L)
{
    luab_module_t *m;
    uintptr_t x, *xp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINTPTR, TYPE, __func__);

    if (lua_isnumber(L, 1) != 0) {
        x = (uintptr_t)luab_checklinteger(L, 1, 0);
        xp = &x;
    } else
        xp = luab_udataisnil(L, 1, m, uintptr_t *);

    return (luab_pushxdata(L, m, xp));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INTMAX)).
 *
 * @function intmax_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage intmax [, err, msg ] = bsd.sys.stdint.intmax_create(x)
 */
static int
luab_intmax_create(lua_State *L)
{
    luab_module_t *m;
    intmax_t x, *xp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INTMAX, TYPE, __func__);

    if (lua_isnumber(L, 1) != 0) {
        x = (intmax_t)luab_checkinteger(L, 1, luab_env_int_max);
        xp = &x;
    } else
        xp = luab_udataisnil(L, 1, m, intmax_t *);

    return (luab_pushxdata(L, m, xp));
}

/*
 * Interface against <sys/stdint.h>
 */

static luab_module_table_t luab_sys_stdint_vec[] = {
    LUAB_INT("__WORDSIZE",              __WORDSIZE),
    LUAB_INT("WCHAR_MIN",               WCHAR_MIN),
    LUAB_INT("WCHAR_MAXN",              WCHAR_MAX),
#if __EXT1_VISIBLE
    LUAB_INT("RSIZE_MAX",               RSIZE_MAX),
#endif /* __EXT1_VISIBLE */

    /* POSIX sized integrals */
    LUAB_FUNC("int8_create",            luab_int8_create),
    LUAB_FUNC("int16_create",           luab_int16_create),
    LUAB_FUNC("int32_create",           luab_int32_create),
    LUAB_FUNC("int64_create",           luab_int64_create),
    LUAB_FUNC("intptr_create",          luab_intptr_create),
    LUAB_FUNC("uintptr_create",         luab_uintptr_create),
    LUAB_FUNC("intmax_create",          luab_intmax_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_stdint_lib = {
    .m_id       = LUAB_SYS_STDINT_LIB_ID,
    .m_name     = LUAB_SYS_STDINT_LIB_KEY,
    .m_vec      = luab_sys_stdint_vec,
};
