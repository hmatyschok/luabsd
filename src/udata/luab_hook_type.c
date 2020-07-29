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
 *      lua_Integer un_int;
 *      lua_Number  un_num;
 *      lua_CFunction   un_fn;
 *  } luab_type_u;
 *
 * by
 *
 *  typedef struct luab_hook {
 *      luab_type_u hook;
 *  } luab_hook_t;
 */

typedef struct luab_hook {
    luab_type_u hook;
} luab_hook_t;

#define luab_newhook(L, arg) \
    ((luab_hook_t *)luab_newuserdata(L, &hook_type, (arg)))
#define luab_to_hook(L, narg) \
    (luab_todata((L), (narg), &hook_type, luab_hook_t *))

#define LUABSD_HOOK_TYPE_ID    1595975665
#define LUABSD_HOOK_TYPE   "HOOK*"

int luab_CreateHook(lua_State *);

static int
Hook_set_char(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    hook = (luab_type_u *)(*hook_type.get)(L, 1);
    value = luab_checkinteger(L, 2, UCHAR_MAX);

    hook->un_int = value;

    return luab_pusherr(L, 0);
}

static int
Hook_get_char(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    hook = (luab_type_u *)(*hook_type.get)(L, 1);

    value = (hook->un_int & UCHAR_MAX);

    return luab_pusherr(L, value);
}

static int
Hook_set_integer(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    hook = (luab_type_u *)(*hook_type.get)(L, 1);
    value = luab_checkinteger(L, 2, UINT_MAX);

    hook->un_int = value;

    return luab_pusherr(L, 0);
}

static int
Hook_get_integer(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    hook = (luab_type_u *)(*hook_type.get)(L, 1);

    value = (hook->un_int & UINT_MAX);

    return luab_pusherr(L, value);
}

static int
Hook_set_long(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    hook = (luab_type_u *)(*hook_type.get)(L, 1);
    value = luab_checkinteger(L, 2, ULONG_MAX);

    hook->un_int = value;

    return luab_pusherr(L, 0);
}

static int
Hook_get_long(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    hook = (luab_type_u *)(*hook_type.get)(L, 1);

    value = (hook->un_int & ULONG_MAX);

    return luab_pusherr(L, value);
}

static int
Hook_set_short(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 2);

    hook = (luab_type_u *)(*hook_type.get)(L, 1);
    value = luab_checkinteger(L, 2, USHRT_MAX);

    hook->un_int = value;

    return luab_pusherr(L, 0);
}

static int
Hook_get_short(lua_State *L)
{
    luab_type_u *hook;
    lua_Integer value;

    (void)luab_checkmaxargs(L, 1);

    hook = (luab_type_u *)(*hook_type.get)(L, 1);

    value = (hook->un_int & USHRT_MAX);

    return luab_pusherr(L, value);
}

static int
Hook_gc(lua_State *L)
{
    luab_hook_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_hook(L, 1);

    (void)memset_s(self, hook_type.sz, 0, hook_type.sz);

    return 0;
}

static int
Hook_tostring(lua_State *L)
{
    luab_hook_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_hook(L, 1);
    lua_pushfstring(L, "Hook (%p)", self);

    return 1;
}

static luab_table_t hook_methods[] = {
    LUABSD_FUNC("set_char", Hook_set_char),
    LUABSD_FUNC("set_integer",  Hook_set_integer),
    LUABSD_FUNC("set_long", Hook_set_long),
    LUABSD_FUNC("set_short",    Hook_set_short),
    LUABSD_FUNC("get_char", Hook_get_char),
    LUABSD_FUNC("get_integer",  Hook_get_integer),
    LUABSD_FUNC("get_long", Hook_get_long),
    LUABSD_FUNC("get_short",    Hook_get_short),
    LUABSD_FUNC("__gc", Hook_gc),
    LUABSD_FUNC("__tostring",   Hook_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void
hook_init(void *ud, void *arg)
{                                           /* XXX */
    luab_hook_t *self = (luab_hook_t *)ud;

    (void)memmove(&self->hook, arg, sizeof(self->hook));
}

static void *
hook_udata(lua_State *L, int narg)
{
    luab_hook_t *self = luab_to_hook(L, narg);

    return &self->hook;
}

luab_module_t hook_type = {
    .cookie = LUABSD_HOOK_TYPE_ID,
    .name = LUABSD_HOOK_TYPE,
    .vec = hook_methods,
    .init = hook_init,
    .get = hook_udata,
    .sz = sizeof(luab_hook_t),
};

int
luab_CreateHook(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 1);
    luab_type_u *hook;
    int status;

    if (narg == 0)
        hook = NULL;
    else
        hook = hook_udata(L, narg);

    if (luab_newhook(L, hook) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}
