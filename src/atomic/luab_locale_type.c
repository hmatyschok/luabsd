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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS LOCALEERRUPTION)
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
#include "luab_table.h"

extern luab_module_t luab_locale_type;

/*
 * Interface against
 *
 *  locale_t
 *
 */

#define luab_new_locale(L, arg) \
    ((luab_locale_t *)luab_newudata(L, &luab_locale_type, (arg)))
#define luab_to_locale(L, narg) \
    (luab_todata((L), (narg), &luab_locale_type, luab_locale_t *))

/*
 * Subr.
 */

static void
locale_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_locale_t *self;

    if ((self = (luab_locale_t *)arg) != NULL) {

        luab_setfstring(L, narg, "value", "(%p)", self->ud_sdu);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(LOCALE)) localeo (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ] = locale:get_table()
 */
static int
LOCALE_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = locale_fillxtable;
    xtp.xtp_arg = (void *)luab_to_locale(L, 1);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - returns (LUA_TNIL).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = locale:dump()
 */
static int
LOCALE_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get addr of locale.
 *
 * @function value
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = locale:value()
 */
static int
LOCALE_value(lua_State *L)
{
    luab_locale_t *self;
    locale_t x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_locale(L, 1);
    x = self->ud_sdu;

    return (luab_pushfstring(L, "(%p)", x));
}

/*
 * Metamethods.
 */

static int
LOCALE_gc(lua_State *L)
{
    luab_locale_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_locale(L, 1);

    if (self->ud_sdu != NULL)
        freelocale(self->ud_sdu);

    return (luab_core_gc(L, 1, &luab_locale_type));
}

static int
LOCALE_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_locale_type));
}

static int
LOCALE_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_locale_type));
}

/*
 * Internal localeerface.
 */

static luab_module_table_t locale_methods[] = {
    LUAB_FUNC("value",          LOCALE_value),
    LUAB_FUNC("get_table",      LOCALE_get_table),
    LUAB_FUNC("dump",           LOCALE_dump),
    LUAB_FUNC("__gc",           LOCALE_gc),
    LUAB_FUNC("__len",          LOCALE_len),
    LUAB_FUNC("__tostring",     LOCALE_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
locale_create(lua_State *L, void *arg)
{
    return (luab_new_locale(L, arg));
}

static void
locale_init(void *ud, void *arg)
{
    luab_udata_init(&luab_locale_type, ud, arg);
}

static void *
locale_udata(lua_State *L, int narg)
{
    luab_locale_t *self;
    self = luab_to_locale(L, narg);
    return ((void *)self);
}

luab_module_t luab_locale_type = {
    .m_id           = LUAB_LOCALE_TYPE_ID,
    .m_name         = LUAB_LOCALE_TYPE,
    .m_vec          = locale_methods,
    .m_create       = locale_create,
    .m_init         = locale_init,
    .m_get          = locale_udata,
    .m_len          = sizeof(luab_locale_t),
    .m_sz           = sizeof(locale_t),
};
