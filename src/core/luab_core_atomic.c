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
 * Generator function, creates an instance of (LUA_TUSERDATA(UCHAR)).
 *
 * @function create_uchar
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(UCHAR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uchar [, err, msg ] = bsd.core.atomic.create_uchar(arg)
 */
static int
luab_type_create_uchar(lua_State *L)
{
    luab_module_t *m;
    u_char x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UCHAR, TYPE, __func__);
    x = (u_char)luab_checkxinteger(L, 1, m, luab_env_uchar_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(USHRT)).
 *
 * @function create_ushrt
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(USHRT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ushrt [, err, msg ] = bsd.core.atomic.create_ushrt(arg)
 */
static int
luab_type_create_ushrt(lua_State *L)
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
 * @function create_uint
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(UINT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uint [, err, msg ] = bsd.core.atomic.create_uint(arg)
 */
static int
luab_type_create_uint(lua_State *L)
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
 * @function create_ulong
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(ULONG)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ulong [, err, msg ] = bsd.core.atomic.create_ulong(arg)
 */
static int
luab_type_create_ulong(lua_State *L)
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
 * @function create_char
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(CHAR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage char [, err, msg ] = bsd.core.atomic.create_char(arg)
 */
static int
luab_type_create_char(lua_State *L)
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
 * @function create_short
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(SHORT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage short [, err, msg ] = bsd.core.atomic.create_short(arg)
 */
static int
luab_type_create_short(lua_State *L)
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
 * @function create_int
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(INT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage int [, err, msg ] = bsd.core.atomic.create_int(arg)
 */
static int
luab_type_create_int(lua_State *L)
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
 * @function create_long
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(LONG)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage long [, err, msg ] = bsd.core.atomic.create_long(arg)
 */
static int
luab_type_create_long(lua_State *L)
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
 * @function create_double
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(DOUBLE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage double [, err, msg ] = bsd.core.atomic.create_double(arg)
 */
static int
luab_type_create_double(lua_State *L)
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
 * @function create_float
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(FLOAT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage float [, err, msg ] = bsd.core.atomic.create_float(arg)
 */
static int
luab_type_create_float(lua_State *L)
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
 * @function create_off
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(OFF)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage off [, err, msg ] = bsd.core.atomic.create_off(arg)
 */
static int
luab_type_create_off(lua_State *L)
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
 * @function create_size
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(SIZE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage size [, err, msg ] = bsd.core.atomic.create_size(arg)
 */
static int
luab_type_create_size(lua_State *L)
{
    luab_module_t *m;
    size_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIZE, TYPE, __func__);
    x = (size_t)luab_checklxinteger(L, 1, m, 0);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(SOCKLEN)).
 *
 * @function create_socklen
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(SOCKLEN)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage socklen [, err, msg ] = bsd.core.atomic.create_socklen(arg)
 */
static int
luab_type_create_socklen(lua_State *L)
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
 * @function create_ssize
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(SSIZE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ssize [, err, msg ] = bsd.core.atomic.create_ssize(arg)
 */
static int
luab_type_create_ssize(lua_State *L)
{
    luab_module_t *m;
    ssize_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SSIZE, TYPE, __func__);
    x = (ssize_t)luab_checklxinteger(L, 1, m, 0);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(UID)).
 *
 * @function create_uid
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(UID)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uid [, err, msg ] = bsd.core.atomic.create_uid(arg)
 */
static int
luab_type_create_uid(lua_State *L)
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
 * @function create_wchar
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(WCHAR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage wchar [, err, msg ] = bsd.core.atomic.create_wchar(arg)
 */
static int
luab_type_create_wchar(lua_State *L)
{
    luab_module_t *m;
    wchar_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(WCHAR, TYPE, __func__);
    x = (wchar_t)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(VM_OFFSET)).
 *
 * @function create_vm_offset
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(VM_OFFSET)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage vm_offset [, err, msg ] = bsd.core.atomic.create_vm_offset(arg)
 */
static int
luab_type_create_vm_offset(lua_State *L)
{
    luab_module_t *m;
    vm_offset_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(VM_OFFSET, TYPE, __func__);
    x = (vm_offset_t)luab_checklxinteger(L, 1, m, 0);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(LUAL_INTEGER)).
 *
 * @function lual_integer_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(LUAL_INTEGER)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage lual_integer [, err, msg ] = bsd.core.atomic.lual_integer_create(arg)
 */
static int
luab_type_create_lual_integer(lua_State *L)
{
    luab_module_t *m;
    lua_Integer x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LUAL_INTEGER, TYPE, __func__);
    x = (lua_Integer)luab_checklxinteger(L, 1, m, 0);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(LUAL_NUMBER)).
 *
 * @function create_lual_number
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(LUAL_NUMBER)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage lual_number [, err, msg ] = bsd.core.atomic.create_lual_number(arg)
 */
static int
luab_type_create_lual_number(lua_State *L)
{
    luab_module_t *m;
    lua_Number x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LUAL_NUMBER, TYPE, __func__);
    x = (lua_Number)luab_checklxinteger(L, 1, m, 0);
    return (luab_pushxdata(L, m, &x));
}

/*
 * Internal interface.
 */

static luab_module_table_t luab_core_atomic_vec[] = {
    /* integer types */
#if __BSD_VISIBLE
    LUAB_FUNC("create_uchar",           luab_type_create_uchar),
    LUAB_FUNC("create_ushrt",           luab_type_create_ushrt),
    LUAB_FUNC("create_uint",            luab_type_create_uint),
    LUAB_FUNC("create_ulong",           luab_type_create_ulong),
#endif /* __BSD_VISIBLE */
    LUAB_FUNC("create_char",            luab_type_create_char),
    LUAB_FUNC("create_short",           luab_type_create_short),
    LUAB_FUNC("create_int",             luab_type_create_int),
    LUAB_FUNC("create_long",            luab_type_create_long),

    /* floating point number types */
    LUAB_FUNC("create_double",          luab_type_create_double),
    LUAB_FUNC("create_float",           luab_type_create_float),

    /* stadard types */
    LUAB_FUNC("create_off",             luab_type_create_off),
    LUAB_FUNC("create_size",            luab_type_create_size),
    LUAB_FUNC("create_socklen",         luab_type_create_socklen),
    LUAB_FUNC("create_ssize",           luab_type_create_ssize),
    LUAB_FUNC("create_uid",             luab_type_create_uid),
    LUAB_FUNC("create_wchar",           luab_type_create_wchar),
    LUAB_FUNC("create_vm_offset",       luab_type_create_vm_offset),
    LUAB_FUNC("create_lua_integer",     luab_type_create_lual_integer),
    LUAB_FUNC("create_lua_number",      luab_type_create_lual_number),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_core_atomic_lib = {
    .m_id       = LUAB_CORE_ATOMIC_LIB_ID,
    .m_name     = LUAB_CORE_ATOMIC_LIB_KEY,
    .m_vec      = luab_core_atomic_vec,
};
