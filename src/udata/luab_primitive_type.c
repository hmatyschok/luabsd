/*
 * Copyright (c) 2020 Henning Matyschok <hmatyschok@outlook.com>
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
#include "luab_udata.h"

extern luab_module_t primitive_type;

/*
 * Interface against
 *
 *  typedef union luab_primitive {
 *      char        un_char;
 *      short       un_short;
 *      int         un_int;
 *      long        un_long;
 *      uint8_t     un_uint8;
 *      uint16_t    un_uint16;
 *      uint32_t    un_uint32;
 *      uint64_t    un_uint64;
 *      socklen_t   un_socklen;
 *      lua_Integer un_intx;
 *      lua_Number  un_numx;
 *      lua_CFunction   un_fn;
 *      const char  *un_cp;
 *  } luab_primitive_u;
 *
 * by
 *
 *  typedef struct luab_primitive {
 *      luab_primitive_u primitive;
 *  } luab_primitive_t;
 */

typedef struct luab_primitive {
    luab_udata_t    ud_softc;
    luab_primitive_u     ud_x;
} luab_primitive_t;

#define luab_new_primitive(L, arg) \
    ((luab_primitive_t *)luab_newudata(L, &primitive_type, (arg)))
#define luab_to_primitive(L, narg) \
    ((luab_primitive_u *)luab_toudata((L), (narg), &primitive_type))

#define LUAB_PRIMITIVE_TYPE_ID    1595975665
#define LUAB_PRIMITIVE_TYPE   "PRIMITIVE*"

/*
 * Generator functions.
 */

/***
 * Generator function - returns (LUA_TNIL).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = cmsgcred:dump()
 */
static int
PRIMITIVE_dump(lua_State *L)
{
    return (luab_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

static int
PRIMITIVE_set_char(lua_State *L)
{
    luab_primitive_u *xp;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    xp = luab_udata(L, 1, &primitive_type, luab_primitive_u *);
    value = luab_checkinteger(L, 2, UCHAR_MAX);

    xp->un_char = (char)value;

    return (luab_pusherr(L, xp->un_char));
}

static int
PRIMITIVE_get_char(lua_State *L)
{
    luab_primitive_u *xp;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    xp = luab_udata(L, 1, &primitive_type, luab_primitive_u *);

    value = (xp->un_intx & UCHAR_MAX);

    return (luab_pusherr(L, value));
}

static int
PRIMITIVE_set_short(lua_State *L)
{
    luab_primitive_u *xp;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    xp = luab_udata(L, 1, &primitive_type, luab_primitive_u *);
    value = luab_checkinteger(L, 2, USHRT_MAX);

    xp->un_short = (short)value;

    return (luab_pusherr(L, xp->un_short));
}

static int
PRIMITIVE_get_short(lua_State *L)
{
    luab_primitive_u *xp;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    xp = luab_udata(L, 1, &primitive_type, luab_primitive_u *);

    value = (xp->un_intx & USHRT_MAX);

    return (luab_pusherr(L, value));
}


static int
PRIMITIVE_set_int(lua_State *L)
{
    luab_primitive_u *xp;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    xp = luab_udata(L, 1, &primitive_type, luab_primitive_u *);
    value = luab_checkinteger(L, 2, UINT_MAX);

    xp->un_int = (int)value;

    return (luab_pusherr(L, xp->un_int));
}

static int
PRIMITIVE_get_int(lua_State *L)
{
    luab_primitive_u *xp;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    xp = luab_udata(L, 1, &primitive_type, luab_primitive_u *);

    value = (xp->un_intx & UINT_MAX);

    return (luab_pusherr(L, value));
}

static int
PRIMITIVE_set_long(lua_State *L)
{
    luab_primitive_u *xp;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    xp = luab_udata(L, 1, &primitive_type, luab_primitive_u *);
    value = luab_checkinteger(L, 2, ULONG_MAX);

    xp->un_long = (long)value;

    return (luab_pusherr(L, xp->un_long));
}

static int
PRIMITIVE_get_long(lua_State *L)
{
    luab_primitive_u *xp;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    xp = luab_udata(L, 1, &primitive_type, luab_primitive_u *);

    value = (xp->un_intx & ULONG_MAX);

    return (luab_pusherr(L, value));
}

static int
PRIMITIVE_set_socklen(lua_State *L)
{
    luab_primitive_u *xp;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    xp = luab_udata(L, 1, &primitive_type, luab_primitive_u *);
    value = luab_checkinteger(L, 2, INT_MAX);

    xp->un_socklen = (socklen_t)value;

    return (luab_pusherr(L, xp->un_socklen));
}

static int
PRIMITIVE_get_socklen(lua_State *L)
{
    luab_primitive_u *xp;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    xp = luab_udata(L, 1, &primitive_type, luab_primitive_u *);

    value = (xp->un_intx & INT_MAX);

    return (luab_pusherr(L, value));
}

/*
 * Metamethods.
 */

static int
PRIMITIVE_gc(lua_State *L)
{
    return (luab_gc(L, 1, &primitive_type));
}

static int
PRIMITIVE_len(lua_State *L)
{
    return (luab_len(L, 2, &primitive_type));
}

static int
PRIMITIVE_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &primitive_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t primitive_methods[] = {
    LUAB_FUNC("set_char",       PRIMITIVE_set_char),
    LUAB_FUNC("set_int",        PRIMITIVE_set_int),
    LUAB_FUNC("set_long",       PRIMITIVE_set_long),
    LUAB_FUNC("set_short",      PRIMITIVE_set_short),
    LUAB_FUNC("set_socklen",    PRIMITIVE_set_socklen),
    LUAB_FUNC("get_char",       PRIMITIVE_get_char),
    LUAB_FUNC("get_int",        PRIMITIVE_get_int),
    LUAB_FUNC("get_long",       PRIMITIVE_get_long),
    LUAB_FUNC("get_short",      PRIMITIVE_get_short),
    LUAB_FUNC("get_socklen",    PRIMITIVE_get_socklen),
    LUAB_FUNC("dump",           PRIMITIVE_dump),
    LUAB_FUNC("__gc",           PRIMITIVE_gc),
    LUAB_FUNC("__len",          PRIMITIVE_len),
    LUAB_FUNC("__tostring",     PRIMITIVE_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
primitive_create(lua_State *L, void *arg)
{
    return (luab_new_primitive(L, arg));
}

static void
primitive_init(void *ud, void *arg)
{
    luab_udata_init(&primitive_type, ud, arg);
}

static void *
primitive_udata(lua_State *L, int narg)
{
    return (luab_to_primitive(L, narg));
}

luab_module_t primitive_type = {
    .m_cookie   = LUAB_PRIMITIVE_TYPE_ID,
    .m_name     = LUAB_PRIMITIVE_TYPE,
    .m_vec      = primitive_methods,
    .m_create   = primitive_create,
    .m_init     = primitive_init,
    .m_get      = primitive_udata,
    .m_sz       = sizeof(luab_primitive_t),
};
