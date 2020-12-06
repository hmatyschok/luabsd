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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_modules.h"
#include "luab_udata.h"

#define LUAB_CORE_ATOMIC_LIB_ID    1607258006
#define LUAB_CORE_ATOMIC_LIB_KEY   "atomic"

/*
 * Interface of <core_atomic>, atomic data types.
 */

#if __BSD_VISIBLE
/***
 * Generator function, creates an instance of (LUA_TUSERDATA(USHRT)).
 *
 * @function ushrt_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ushrt [, err, msg ] = bsd.atomic.ushrt_create(x)
 */
static int
luab_ushrt_create(lua_State *L)
{
    u_short x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (u_short)luab_checkinteger(L, 1, luab_env_ushrt_max);

    return (luab_pushudata(L, luab_xmod(USHRT, TYPE, __func__), &x));
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
 * @usage uint [, err, msg ] = bsd.atomic.uint_create(x)
 */
static int
luab_uint_create(lua_State *L)
{
    u_int x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (u_int)luab_checkinteger(L, 1, luab_env_uint_max);

    return (luab_pushudata(L, luab_xmod(UINT, TYPE, __func__), &x));
}
#endif /* __BSD_VISIBLE */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(CHAR)).
 *
 * @function char_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage char [, err, msg ] = bsd.atomic.char_create(x)
 */
static int
luab_char_create(lua_State *L)
{
    char x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (char)luab_checkinteger(L, 1, luab_env_uchar_max);

    return (luab_pushudata(L, luab_xmod(CHAR, TYPE, __func__), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(SHORT)).
 *
 * @function short_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage short [, err, msg ] = bsd.atomic.short_create(x)
 */
static int
luab_short_create(lua_State *L)
{
    short x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (short)luab_checkinteger(L, 1, luab_env_uchar_max);

    return (luab_pushudata(L, luab_xmod(SHORT, TYPE, __func__), &x));
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
 * @usage int [, err, msg ] = bsd.atomic.int_create(x)
 */
static int
luab_int_create(lua_State *L)
{
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (int)luab_checkinteger(L, 1, luab_env_uint_max);

    return (luab_pushudata(L, luab_xmod(INT, TYPE, __func__), &x));
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
 * @usage long [, err, msg ] = bsd.atomic.long_create(x)
 */
static int
luab_long_create(lua_State *L)
{
    long x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (long)luab_checkinteger(L, 1, luab_env_long_max);

    return (luab_pushudata(L, luab_xmod(LONG, TYPE, __func__), &x));
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
 * @usage double [, err, msg ] = bsd.atomic.double_create(x)
 */
static int
luab_double_create(lua_State *L)
{
    double x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (double)luaL_checknumber(L, 1); /* XXX */

    return (luab_pushudata(L, luab_xmod(DOUBLE, TYPE, __func__), &x));
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
 * @usage float [, err, msg ] = bsd.atomic.float_create(x)
 */
static int
luab_float_create(lua_State *L)
{
    float x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (float)luaL_checknumber(L, 1); /* XXX */

    return (luab_pushudata(L, luab_xmod(FLOAT, TYPE, __func__), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INT8)).
 *
 * @function int8_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage int8 [, err, msg ] = bsd.atomic.int8_create(x)
 */
static int
luab_int8_create(lua_State *L)
{
    int8_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (int8_t)luab_checkinteger(L, 1, luab_env_uchar_max);

    return (luab_pushudata(L, luab_xmod(INT8, TYPE, __func__), &x));
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
 * @usage int16 [, err, msg ] = bsd.atomic.int16_create(x)
 */
static int
luab_int16_create(lua_State *L)
{
    int16_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (int16_t)luab_checkinteger(L, 1, luab_env_ushrt_max);

    return (luab_pushudata(L, luab_xmod(INT16, TYPE, __func__), &x));
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
 * @usage int32 [, err, msg ] = bsd.atomic.int32_create(x)
 */
static int
luab_int32_create(lua_State *L)
{
    int32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (int32_t)luab_checkinteger(L, 1, luab_env_uint_max);

    return (luab_pushudata(L, luab_xmod(INT32, TYPE, __func__), &x));
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
 * @usage int64 [, err, msg ] = bsd.atomic.int64_create(x)
 */
static int
luab_int64_create(lua_State *L)
{
    int64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (int64_t)luab_checkinteger(L, 1, luab_env_ullong_max);

    return (luab_pushudata(L, luab_xmod(INT64, TYPE, __func__), &x));
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
 * @usage fpos [, err, msg ] = bsd.atomic.fpos_create(x)
 */
static int
luab_fpos_create(lua_State *L)
{
    fpos_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (fpos_t)luab_checkinteger(L, 1, luab_env_long_max);

    return (luab_pushudata(L, luab_xmod(INT64, TYPE, __func__), &x));
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
 * @usage gid [, err, msg ] = bsd.atomic.gid_create(x)
 */
static int
luab_gid_create(lua_State *L)
{
    gid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (gid_t)luab_checkinteger(L, 1, luab_env_int_max);

    return (luab_pushudata(L, luab_xmod(GID, TYPE, __func__), &x));
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
 * @usage off [, err, msg ] = bsd.atomic.off_create(x)
 */
static int
luab_off_create(lua_State *L)
{
    off_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (off_t)luab_checkinteger(L, 1, luab_env_long_max);

    return (luab_pushudata(L, luab_xmod(OFF, TYPE, __func__), &x));
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
 * @usage size [, err, msg ] = bsd.atomic.size_create(x)
 */
static int
luab_size_create(lua_State *L)
{
    size_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (size_t)luab_checklinteger(L, 1, 0);

    return (luab_pushudata(L, luab_xmod(SIZE, TYPE, __func__), &x));
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
 * @usage socklen [, err, msg ] = bsd.atomic.socklen_create(x)
 */
static int
luab_socklen_create(lua_State *L)
{
    socklen_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (socklen_t)luab_checkinteger(L, 1, luab_env_int_max);

    return (luab_pushudata(L, luab_xmod(SOCKLEN, TYPE, __func__), &x));
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
 * @usage ssize [, err, msg ] = bsd.atomic.ssize_create(x)
 */
static int
luab_ssize_create(lua_State *L)
{
    ssize_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (ssize_t)luab_checklinteger(L, 1, 1);

    return (luab_pushudata(L, luab_xmod(SSIZE, TYPE, __func__), &x));
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
 * @usage uid [, err, msg ] = bsd.atomic.uid_create(x)
 */
static int
luab_uid_create(lua_State *L)
{
    uid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (uid_t)luab_checkinteger(L, 1, luab_env_uid_max);

    return (luab_pushudata(L, luab_xmod(UID, TYPE, __func__), &x));
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
 * @usage wchar [, err, msg ] = bsd.atomic.wchar_create(x)
 */
static int
luab_wchar_create(lua_State *L)
{
    wchar_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (wchar_t)luab_checkinteger(L, 1, luab_env_uint_max);

    return (luab_pushudata(L, luab_xmod(WCHAR, TYPE, __func__), &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(TIME)).
 *
 * @function time_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage time [, err, msg ] = bsd.atomic.time_create(x)
 */
static int
luab_time_create(lua_State *L)
{
    time_t x;

    (void)luab_core_checkmaxargs(L, 1);

    x = (time_t)luab_checklinteger(L, 1, 1);

    return (luab_pushudata(L, luab_xmod(TIME, TYPE, __func__), &x));
}

static luab_module_table_t luab_core_atomic_vec[] = {
    /* integer types */
#if __BSD_VISIBLE
    LUAB_FUNC("ushrt_create",       luab_ushrt_create),
    LUAB_FUNC("uint_create",        luab_uint_create),
#endif /* __BSD_VISIBLE */
    LUAB_FUNC("char_create",        luab_char_create),
    LUAB_FUNC("short_create",       luab_short_create),
    LUAB_FUNC("int_create",         luab_int_create),
    LUAB_FUNC("long_create",        luab_long_create),

    /* floating point number types */
    LUAB_FUNC("double_create",      luab_double_create),
    LUAB_FUNC("float_create",       luab_float_create),

    /* POSIX sized integrals */
    LUAB_FUNC("int8_create",        luab_int8_create),
    LUAB_FUNC("int16_create",       luab_int16_create),
    LUAB_FUNC("int32_create",       luab_int32_create),
    LUAB_FUNC("int64_create",       luab_int64_create),

    /* stadard types */
    LUAB_FUNC("fpos_type",          luab_fpos_create),
    LUAB_FUNC("gid_create",         luab_gid_create),
    LUAB_FUNC("off_create",         luab_off_create),
    LUAB_FUNC("size_create",        luab_size_create),
    LUAB_FUNC("socklen_create",     luab_socklen_create),
    LUAB_FUNC("ssize_create",       luab_ssize_create),
    LUAB_FUNC("uid_create",         luab_uid_create),
    LUAB_FUNC("wchar_create",       luab_wchar_create),
    LUAB_FUNC("time_create",        luab_time_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_core_atomic_lib = {
    .m_id       = LUAB_CORE_ATOMIC_LIB_ID,
    .m_name     = LUAB_CORE_ATOMIC_LIB_KEY,
    .m_vec      = luab_core_atomic_vec,
};
