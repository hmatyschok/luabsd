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

#include <errno.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t hook_type;

/*
 * Interface against
 *
 *  typedef union luab_type {
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
 *  } luab_type_u;
 *
 * by
 *
 *  typedef struct luab_hook {
 *      luab_type_u hook;
 *  } luab_hook_t;
 */

typedef struct luab_hook {
    luab_udata_t    ud_softc;
    luab_type_u     hook;
} luab_hook_t;

#define luab_new_hook(L, arg) \
    ((luab_hook_t *)luab_newuserdata(L, &hook_type, (arg)))
#define luab_to_hook(L, narg) \
    (luab_toudata((L), (narg), &hook_type))

#define LUABSD_HOOK_TYPE_ID    1595975665
#define LUABSD_HOOK_TYPE   "HOOK*"

int luab_hook_create(lua_State *);

static int
HOOK_set_char(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    hook = luab_udata(L, 1, hook_type, luab_type_u *);
    value = luab_checkinteger(L, 2, UCHAR_MAX);

    hook->un_char = (char)value;

    return (luab_pusherr(L, hook->un_char));
}

static int
HOOK_get_char(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    hook = luab_udata(L, 1, hook_type, luab_type_u *);

    value = (hook->un_intx & UCHAR_MAX);

    return (luab_pusherr(L, value));
}

static int
HOOK_set_short(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    hook = luab_udata(L, 1, hook_type, luab_type_u *);
    value = luab_checkinteger(L, 2, USHRT_MAX);

    hook->un_short = (short)value;

    return (luab_pusherr(L, hook->un_short));
}

static int
HOOK_get_short(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    hook = luab_udata(L, 1, hook_type, luab_type_u *);

    value = (hook->un_intx & USHRT_MAX);

    return (luab_pusherr(L, value));
}


static int
HOOK_set_int(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    hook = luab_udata(L, 1, hook_type, luab_type_u *);
    value = luab_checkinteger(L, 2, UINT_MAX);

    hook->un_int = (int)value;

    return (luab_pusherr(L, hook->un_int));
}

static int
HOOK_get_int(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    hook = luab_udata(L, 1, hook_type, luab_type_u *);

    value = (hook->un_intx & UINT_MAX);

    return (luab_pusherr(L, value));
}

static int
HOOK_set_long(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    hook = luab_udata(L, 1, hook_type, luab_type_u *);
    value = luab_checkinteger(L, 2, ULONG_MAX);

    hook->un_long = (long)value;

    return (luab_pusherr(L, hook->un_long));
}

static int
HOOK_get_long(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    hook = luab_udata(L, 1, hook_type, luab_type_u *);

    value = (hook->un_intx & ULONG_MAX);

    return (luab_pusherr(L, value));
}

static int
HOOK_set_socklen(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    hook = luab_udata(L, 1, hook_type, luab_type_u *);
    value = luab_checkinteger(L, 2, INT_MAX);

    hook->un_socklen = (socklen_t)value;

    return (luab_pusherr(L, hook->un_socklen));
}

static int
HOOK_get_socklen(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    hook = luab_udata(L, 1, hook_type, luab_type_u *);

    value = (hook->un_intx & INT_MAX);

    return (luab_pusherr(L, value));
}

static int
HOOK_gc(lua_State *L)
{
    return (luab_gc(L, 1, &hook_type));
}

static int
HOOK_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &hook_type));
}

static luab_table_t hook_methods[] = {
    LUABSD_FUNC("set_char", HOOK_set_char),
    LUABSD_FUNC("set_int",  HOOK_set_int),
    LUABSD_FUNC("set_long", HOOK_set_long),
    LUABSD_FUNC("set_short",    HOOK_set_short),
    LUABSD_FUNC("set_socklen",  HOOK_set_socklen),
    LUABSD_FUNC("get_char", HOOK_get_char),
    LUABSD_FUNC("get_int",  HOOK_get_int),
    LUABSD_FUNC("get_long", HOOK_get_long),
    LUABSD_FUNC("get_short",    HOOK_get_short),
    LUABSD_FUNC("get_socklen",  HOOK_get_socklen),
    LUABSD_FUNC("__gc", HOOK_gc),
    LUABSD_FUNC("__tostring",   HOOK_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
hook_create(lua_State *L, void *arg)
{
    return (luab_new_hook(L, arg));
}

static void
hook_init(void *ud, void *arg)
{
    luab_hook_t *self;

    if (((self = (luab_hook_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->hook, arg, sizeof(self->hook));
}

static void *
hook_udata(lua_State *L, int narg)
{
    return (luab_to_hook(L, narg));
}

luab_module_t hook_type = {
    .cookie = LUABSD_HOOK_TYPE_ID,
    .name = LUABSD_HOOK_TYPE,
    .vec = hook_methods,
    .create = hook_create,
    .init = hook_init,
    .get = hook_udata,
    .sz = sizeof(luab_hook_t),
};

int
luab_hook_create(lua_State *L)
{
    luab_type_u *hook;
    int narg, status;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        hook = NULL;
    else
        hook = hook_udata(L, narg);

    if (hook_create(L, hook) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
