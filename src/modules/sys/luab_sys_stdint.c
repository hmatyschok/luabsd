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
 * @function create_int8
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(INT8)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage int8 [, err, msg ] = bsd.sys.stdint.create_int8(arg)
 */
static int
luab_type_create_int8(lua_State *L)
{
    luab_module_t *m;
    int8_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT8, TYPE, __func__);
    x = (int8_t)luab_checkxinteger(L, 1, m, luab_env_uchar_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INT16)).
 *
 * @function create_int16
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(INT16)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage int16 [, err, msg ] = bsd.sys.stdint.create_int16(arg)
 */
static int
luab_type_create_int16(lua_State *L)
{
    luab_module_t *m;
    int16_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT16, TYPE, __func__);
    x = (int16_t)luab_checkxinteger(L, 1, m, luab_env_ushrt_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INT32)).
 *
 * @function create_int32
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(INT32)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage int32 [, err, msg ] = bsd.sys.stdint.create_int32(arg)
 */
static int
luab_type_create_int32(lua_State *L)
{
    luab_module_t *m;
    int32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT32, TYPE, __func__);
    x = (int32_t)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INT64)).
 *
 * @function create_int64
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(INT64)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage int64 [, err, msg ] = bsd.sys.stdint.create_int64(arg)
 */
static int
luab_type_create_int64(lua_State *L)
{
    luab_module_t *m;
    int64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT64, TYPE, __func__);
    x = (int64_t)luab_checkxinteger(L, 1, m, luab_env_ulong_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @function create_uint8
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(UINT8)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uint8 [, err, msg ] = bsd.sys.stdint.create_uint8(arg)
 */
static int
luab_type_create_uint8(lua_State *L)
{
    luab_module_t *m;
    uint8_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    x = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_uchar_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(UINT16)).
 *
 * @function create_uint16
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(UINT16)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uint16 [, err, msg ] = bsd.sys.stdint.create_uint16(arg)
 */
static int
luab_type_create_uint16(lua_State *L)
{
    luab_module_t *m;
    uint16_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT16, TYPE, __func__);
    x = (uint16_t)luab_checkxinteger(L, 1, m, luab_env_ushrt_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(UINT32)).
 *
 * @function create_uint32
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(UINT32)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uint32 [, err, msg ] = bsd.sys.stdint.create_uint32(arg)
 */
static int
luab_type_create_uint32(lua_State *L)
{
    luab_module_t *m;
    uint32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT32, TYPE, __func__);
    x = (uint32_t)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(UINT64)).
 *
 * @function create_uint64
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(UINT64)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uint64 [, err, msg ] = bsd.sys.stdint.create_uint64(arg)
 */
static int
luab_type_create_uint64(lua_State *L)
{
    luab_module_t *m;
    uint64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT64, TYPE, __func__);
    x = (uint64_t)luab_checkxinteger(L, 1, m, luab_env_ulong_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INTPTR)).
 *
 * @function create_intptr
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(INTPTR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage intptr [, err, msg ] = bsd.sys.stdint.create_intptr(arg)
 */
static int
luab_type_create_intptr(lua_State *L)
{
    luab_module_t *m;
    intptr_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INTPTR, TYPE, __func__);
    x = (intptr_t)luab_checklxinteger(L, 1, m, 0);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(UINTPTR)).
 *
 * @function create_uintptr
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(UINTPTR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uintptr [, err, msg ] = bsd.sys.stdint.create_uintptr(arg)
 */
static int
luab_type_create_uintptr(lua_State *L)
{
    luab_module_t *m;
    uintptr_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINTPTR, TYPE, __func__);
    x = (uintptr_t)luab_checklxinteger(L, 1, m, 0);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INTMAX)).
 *
 * @function create_intmax
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(INTMAX)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage intmax [, err, msg ] = bsd.sys.stdint.create_intmax(arg)
 */
static int
luab_type_create_intmax(lua_State *L)
{
    luab_module_t *m;
    intmax_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INTMAX, TYPE, __func__);
    x = (intmax_t)luab_checkxinteger(L, 1, m, luab_env_ulong_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(UINTMAX)).
 *
 * @function create_uintmax
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(UINTMAX)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uintmax [, err, msg ] = bsd.sys.stdint.create_uintmax(arg)
 */
static int
luab_type_create_uintmax(lua_State *L)
{
    luab_module_t *m;
    uintmax_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINTMAX, TYPE, __func__);
    x = (uintmax_t)luab_checkxinteger(L, 1, m, luab_env_ulong_max);
    return (luab_pushxdata(L, m, &x));
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
    LUAB_FUNC("create_int8",            luab_type_create_int8),
    LUAB_FUNC("create_int16",           luab_type_create_int16),
    LUAB_FUNC("create_int32",           luab_type_create_int32),
    LUAB_FUNC("create_int64",           luab_type_create_int64),
    LUAB_FUNC("create_uint8",           luab_type_create_uint8),
    LUAB_FUNC("create_uint16",          luab_type_create_uint16),
    LUAB_FUNC("create_uint32",          luab_type_create_uint32),
    LUAB_FUNC("create_uint64",          luab_type_create_uint64),
    LUAB_FUNC("create_intptr",          luab_type_create_intptr),
    LUAB_FUNC("create_uintptr",         luab_type_create_uintptr),
    LUAB_FUNC("create_intmax",          luab_type_create_intmax),
    LUAB_FUNC("create_uintmax",         luab_type_create_uintmax),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_stdint_lib = {
    .m_id       = LUAB_SYS_STDINT_LIB_ID,
    .m_name     = LUAB_SYS_STDINT_LIB_KEY,
    .m_vec      = luab_sys_stdint_vec,
};
