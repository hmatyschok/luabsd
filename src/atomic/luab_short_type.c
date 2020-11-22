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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS SHORTERRUPTION)
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

extern luab_module_t luab_short_type;

/*
 * Interface against
 *
 *  short
 *
 */

typedef struct luab_short {
    luab_udata_t    ud_softc;
    short           ud_value;
} luab_short_t;

#define luab_new_short(L, arg) \
    ((luab_short_t *)luab_newudata(L, &luab_short_type, (arg)))
#define luab_to_short(L, narg) \
    (luab_todata((L), (narg), &luab_short_type, luab_short_t *))

#define LUAB_SHORT_TYPE_ID    1605997500
#define LUAB_SHORT_TYPE   "SHORT*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SHORT)) shorto (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t = short:get()
 */
static int
SHORT_get(lua_State *L)
{
    luab_short_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_short(L, 1);

    lua_newtable(L);
    luab_setinteger(L, -2, "value", self->ud_value);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - returns (LUA_TNIL).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = short:dump()
 */
static int
SHORT_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set short.
 *
 * @function set_value
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = short:set_value(data)
 */
static int
SHORT_set_value(lua_State *L)
{
    luab_short_t *self;
    short x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_short(L, 1);
    x = (short)luab_checkinteger(L, 2, luab_env_ushrt_max);

    self->ud_value = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get short.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = short:get_value()
 */
static int
SHORT_get_value(lua_State *L)
{
    luab_short_t *self;
    short x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_short(L, 1);
    x = self->ud_value;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
SHORT_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_short_type));
}

static int
SHORT_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_short_type));
}

static int
SHORT_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_short_type));
}

/*
 * Internal shorterface.
 */

static luab_module_table_t short_methods[] = {
    LUAB_FUNC("set_value",      SHORT_set_value),
    LUAB_FUNC("get",            SHORT_get),
    LUAB_FUNC("get_value",      SHORT_get_value),
    LUAB_FUNC("dump",           SHORT_dump),
    LUAB_FUNC("__gc",           SHORT_gc),
    LUAB_FUNC("__len",          SHORT_len),
    LUAB_FUNC("__tostring",     SHORT_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
short_create(lua_State *L, void *arg)
{
    return (luab_new_short(L, arg));
}

static void
short_init(void *ud, void *arg)
{
    luab_udata_init(&luab_short_type, ud, arg);
}

static void *
short_udata(lua_State *L, int narg)
{
    luab_short_t *self;
    self = luab_to_short(L, narg);
    return ((void *)&(self->ud_value));
}

static luab_table_t *
short_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    short *x, y;
    size_t m, n;

    if ((tbl = luab_newvectornil(L, narg, sizeof(short))) != NULL) {

        if (((x = (short *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (short)luab_tointeger(L, -1, luab_env_ushrt_max);
                        x[m] = (short)y;
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        }
    }
    return (tbl);
}

static void
short_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    short *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (short *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetinteger(L, narg, k, x[m]);

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

luab_module_t luab_short_type = {
    .m_cookie   = LUAB_SHORT_TYPE_ID,
    .m_name     = LUAB_SHORT_TYPE,
    .m_vec      = short_methods,
    .m_create   = short_create,
    .m_init     = short_init,
    .m_get      = short_udata,
    .m_get_tbl  = short_checktable,
    .m_set_tbl  = short_pushtable,
    .m_sz       = sizeof(luab_short_t),
};