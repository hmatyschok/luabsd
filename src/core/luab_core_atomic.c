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
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(USHRT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ushrt [, err, msg ] = bsd.core.atomic.ushrt_create(arg)
 */
static int
luab_ushrt_create(lua_State *L)
{
    luab_module_t *m;
    u_short x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(USHRT, TYPE, __func__);
    x = (u_short)luab_checkxinteger(L, 1, m, luab_env_ushrt_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(UINT)).
 *
 * @function uint_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(UINT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uint [, err, msg ] = bsd.core.atomic.uint_create(arg)
 */
static int
luab_uint_create(lua_State *L)
{
    luab_module_t *m;
    u_int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT, TYPE, __func__);
    x = (u_int)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(ULONG)).
 *
 * @function ulong_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(ULONG)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ulong [, err, msg ] = bsd.core.atomic.ulong_create(arg)
 */
static int
luab_ulong_create(lua_State *L)
{
    luab_module_t *m;
    u_long x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(ULONG, TYPE, __func__);
    x = (u_long)luab_checkxinteger(L, 1, m, luab_env_ulong_max);
    return (luab_pushxdata(L, m, &x));
}
#endif /* __BSD_VISIBLE */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(CHAR)).
 *
 * @function char_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(CHAR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage char [, err, msg ] = bsd.core.atomic.char_create(arg)
 */
static int
luab_char_create(lua_State *L)
{
    luab_module_t *m;
    char x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CHAR, TYPE, __func__);
    x = (char)luab_checkxinteger(L, 1, m, luab_env_uchar_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(SHORT)).
 *
 * @function short_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(SHORT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage short [, err, msg ] = bsd.core.atomic.short_create(arg)
 */
static int
luab_short_create(lua_State *L)
{
    luab_module_t *m;
    short x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SHORT, TYPE, __func__);
    x = (short)luab_checkxinteger(L, 1, m, luab_env_ushrt_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INT)).
 *
 * @function int_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(INT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage int [, err, msg ] = bsd.core.atomic.int_create(arg)
 */
static int
luab_int_create(lua_State *L)
{
    luab_module_t *m;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    x = (int)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(LONG)).
 *
 * @function long_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(LONG)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage long [, err, msg ] = bsd.core.atomic.long_create(arg)
 */
static int
luab_long_create(lua_State *L)
{
    luab_module_t *m;
    long x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LONG, TYPE, __func__);
    x = (long)luab_checkxinteger(L, 1, m, luab_env_ulong_max);
    return (luab_pushxdata(L, m, &x));
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
 * @usage double [, err, msg ] = bsd.core.atomic.double_create(x)
 */
static int
luab_double_create(lua_State *L)
{
    luab_module_t *m;
    double x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DOUBLE, TYPE, __func__);
    x = (double)luab_checkxnumber(L, 1, m);

    return (luab_pushxdata(L, m, &x));
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
 * @usage float [, err, msg ] = bsd.core.atomic.float_create(x)
 */
static int
luab_float_create(lua_State *L)
{
    luab_module_t *m;
    float x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FLOAT, TYPE, __func__);
    x = (float)luab_checkxnumber(L, 1, m);

    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(OFF)).
 *
 * @function off_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(OFF)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage off [, err, msg ] = bsd.core.atomic.off_create(arg)
 */
static int
luab_off_create(lua_State *L)
{
    luab_module_t *m;
    off_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(OFF, TYPE, __func__);
    x = (off_t)luab_checkxinteger(L, 1, m, luab_env_ulong_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(SIZE)).
 *
 * @function size_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(SIZE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage size [, err, msg ] = bsd.core.atomic.size_create(arg)
 */
static int
luab_size_create(lua_State *L)
{
    luab_module_t *m;
    size_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIZE, TYPE, __func__);
    x = (size_t)luab_checkxlinteger(L, 1, m, 0);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(SOCKLEN)).
 *
 * @function socklen_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(SOCKLEN)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage socklen [, err, msg ] = bsd.core.atomic.socklen_create(arg)
 */
static int
luab_socklen_create(lua_State *L)
{
    luab_module_t *m;
    socklen_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SOCKLEN, TYPE, __func__);
    x = (socklen_t)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(SSIZE)).
 *
 * @function ssize_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(SSIZE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ssize [, err, msg ] = bsd.core.atomic.ssize_create(arg)
 */
static int
luab_ssize_create(lua_State *L)
{
    luab_module_t *m;
    ssize_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SSIZE, TYPE, __func__);
    x = (ssize_t)luab_checkxlinteger(L, 1, m, 0);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(UID)).
 *
 * @function uid_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(UID)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uid [, err, msg ] = bsd.core.atomic.uid_create(arg)
 */
static int
luab_uid_create(lua_State *L)
{
    luab_module_t *m;
    uid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UID, TYPE, __func__);
    x = (uid_t)luab_checkxinteger(L, 1, m, luab_env_uid_max);
    return (luab_pushxdata(L, m, &x));
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
 * @usage wchar [, err, msg ] = bsd.core.atomic.wchar_create(x)
 */
static int
luab_wchar_create(lua_State *L)
{
    luab_module_t *m;
    wchar_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(WCHAR, TYPE, __func__);
    x = (wchar_t)luab_checkxinteger(L, 1, m, luab_env_uint_max);

    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(TIME)).
 *
 * @function time_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(TIME)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage time [, err, msg ] = bsd.core.atomic.time_create(arg)
 */
static int
luab_time_create(lua_State *L)
{
    luab_module_t *m;
    time_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIME, TYPE, __func__);
    x = (time_t)luab_checkxinteger(L, 1, m, luab_env_ulong_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(CLOCK)).
 *
 * @function clock_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage time [, err, msg ] = bsd.core.atomic.clock_create(x)
 */
static int
luab_clock_create(lua_State *L)
{
    luab_module_t *m;
    clock_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CLOCK, TYPE, __func__);
    x = (clock_t)luab_checkxlinteger(L, 1, m, 1);

    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(VM_OFFSET)).
 *
 * @function vm_offset_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(VM_OFFSET)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage vm_offset [, err, msg ] = bsd.core.atomic.vm_offset_create(arg)
 */
static int
luab_vm_offset_create(lua_State *L)
{
    luab_module_t *m;
    vm_offset_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(VM_OFFSET, TYPE, __func__);
    x = (vm_offset_t)luab_checkxlinteger(L, 1, m, 0);

    return (luab_pushxdata(L, m, &x));
}

static luab_module_table_t luab_core_atomic_vec[] = {
    /* integer types */
#if __BSD_VISIBLE
    LUAB_FUNC("ushrt_create",       luab_ushrt_create),
    LUAB_FUNC("uint_create",        luab_uint_create),
    LUAB_FUNC("ulong_create",       luab_ulong_create),
#endif /* __BSD_VISIBLE */
    LUAB_FUNC("char_create",        luab_char_create),
    LUAB_FUNC("short_create",       luab_short_create),
    LUAB_FUNC("int_create",         luab_int_create),
    LUAB_FUNC("long_create",        luab_long_create),

    /* floating point number types */
    LUAB_FUNC("double_create",      luab_double_create),
    LUAB_FUNC("float_create",       luab_float_create),

    /* stadard types */
    LUAB_FUNC("off_create",         luab_off_create),
    LUAB_FUNC("size_create",        luab_size_create),
    LUAB_FUNC("socklen_create",     luab_socklen_create),
    LUAB_FUNC("ssize_create",       luab_ssize_create),
    LUAB_FUNC("uid_create",         luab_uid_create),
    LUAB_FUNC("wchar_create",       luab_wchar_create),
    LUAB_FUNC("time_create",        luab_time_create),
    LUAB_FUNC("clock_create",       luab_clock_create),
    LUAB_FUNC("vm_offset_create",   luab_vm_offset_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_core_atomic_lib = {
    .m_id       = LUAB_CORE_ATOMIC_LIB_ID,
    .m_name     = LUAB_CORE_ATOMIC_LIB_KEY,
    .m_vec      = luab_core_atomic_vec,
};
