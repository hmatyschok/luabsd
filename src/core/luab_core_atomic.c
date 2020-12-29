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
 * @usage ushrt [, err, msg ] = bsd.core.atomic.ushrt_create(x)
 */
static int
luab_ushrt_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    u_short x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), USHRT, TYPE, __func__, NULL);
    x = (u_short)luab_checkxinteger(L, 1, xmp, luab_env_ushrt_max);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
 * @usage uint [, err, msg ] = bsd.core.atomic.uint_create(x)
 */
static int
luab_uint_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    u_int x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), UINT, TYPE, __func__, NULL);
    x = (u_int)luab_checkxinteger(L, 1, xmp, luab_env_uint_max);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
 * @usage char [, err, msg ] = bsd.core.atomic.char_create(x)
 */
static int
luab_char_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    char x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), CHAR, TYPE, __func__, NULL);
    x = (char)luab_checkxinteger(L, 1, xmp, luab_env_uchar_max);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
 * @usage short [, err, msg ] = bsd.core.atomic.short_create(x)
 */
static int
luab_short_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    short x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), SHORT, TYPE, __func__, NULL);
    x = (short)luab_checkxinteger(L, 1, xmp, luab_env_uchar_max);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
 * @usage int [, err, msg ] = bsd.core.atomic.int_create(x)
 */
static int
luab_int_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), INT, TYPE, __func__, NULL);
    x = (int)luab_checkxinteger(L, 1, xmp, luab_env_uint_max);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
 * @usage long [, err, msg ] = bsd.core.atomic.long_create(x)
 */
static int
luab_long_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    long x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), LONG, TYPE, __func__, NULL);
    x = (long)luab_checkxinteger(L, 1, xmp, luab_env_long_max);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
    luab_xmodule_t xm, *xmp;
    double x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), DOUBLE, TYPE, __func__, NULL);
    x = (double)luab_checkxnumber(L, 1, xmp);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
    luab_xmodule_t xm, *xmp;
    float x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), FLOAT, TYPE, __func__, NULL);
    x = (float)luab_checkxnumber(L, 1, xmp);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
 * @usage fpos [, err, msg ] = bsd.core.atomic.fpos_create(x)
 */
static int
luab_fpos_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    fpos_t x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), FPOS, TYPE, __func__, NULL);
    x = (fpos_t)luab_checkxinteger(L, 1, xmp, luab_env_long_max);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
 * @usage gid [, err, msg ] = bsd.core.atomic.gid_create(x)
 */
static int
luab_gid_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    gid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), GID, TYPE, __func__, NULL);
    x = (gid_t)luab_checkxinteger(L, 1, xmp, luab_env_int_max);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
 * @usage off [, err, msg ] = bsd.core.atomic.off_create(x)
 */
static int
luab_off_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    off_t x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), OFF, TYPE, __func__, NULL);
    x = (off_t)luab_checkxinteger(L, 1, xmp, luab_env_long_max);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
 * @usage size [, err, msg ] = bsd.core.atomic.size_create(x)
 */
static int
luab_size_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    size_t x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), SIZE, TYPE, __func__, NULL);
    x = (size_t)luab_checkxlinteger(L, 1, xmp, 0);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
 * @usage socklen [, err, msg ] = bsd.core.atomic.socklen_create(x)
 */
static int
luab_socklen_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    socklen_t x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), SOCKLEN, TYPE, __func__, NULL);
    x = (socklen_t)luab_checkxinteger(L, 1, xmp, luab_env_int_max);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
 * @usage ssize [, err, msg ] = bsd.core.atomic.ssize_create(x)
 */
static int
luab_ssize_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    ssize_t x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), SSIZE, TYPE, __func__, NULL);
    x = (ssize_t)luab_checkxlinteger(L, 1, xmp, 1);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
 * @usage uid [, err, msg ] = bsd.core.atomic.uid_create(x)
 */
static int
luab_uid_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    uid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), UID, TYPE, __func__, NULL);
    x = (uid_t)luab_checkxinteger(L, 1, xmp, luab_env_uid_max);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
    luab_xmodule_t xm, *xmp;
    wchar_t x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), WCHAR, TYPE, __func__, NULL);
    x = (wchar_t)luab_checkxinteger(L, 1, xmp, luab_env_uint_max);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
 * @usage time [, err, msg ] = bsd.core.atomic.time_create(x)
 */
static int
luab_time_create(lua_State *L)
{
    luab_xmodule_t xm, *xmp;
    time_t x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), TIME, TYPE, __func__, NULL);
    x = (time_t)luab_checkxlinteger(L, 1, xmp, 1);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
    luab_xmodule_t xm, *xmp;
    clock_t x;

    (void)luab_core_checkmaxargs(L, 1);

    luab_initxmodule((xmp = &xm), CLOCK, TYPE, __func__, NULL);
    x = (clock_t)luab_checkxlinteger(L, 1, xmp, 1);

    return (luab_pushxdata(L, xmp->xm_mod, &x));
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
    LUAB_FUNC("clock_create",       luab_clock_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_core_atomic_lib = {
    .m_id       = LUAB_CORE_ATOMIC_LIB_ID,
    .m_name     = LUAB_CORE_ATOMIC_LIB_KEY,
    .m_vec      = luab_core_atomic_vec,
};
