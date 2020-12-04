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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS OFFERRUPTION)
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

extern luab_module_t luab_off_type;

/*
 * Interface against
 *
 *  off_t
 *
 */

typedef struct luab_off {
    luab_udata_t    ud_softc;
    off_t          ud_value;
} luab_off_t;

#define luab_new_off(L, arg) \
    ((luab_off_t *)luab_newudata(L, &luab_off_type, (arg)))
#define luab_to_off(L, narg) \
    (luab_todata((L), (narg), &luab_off_type, luab_off_t *))

/*
 * Subr.
 */

static void
off_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_off_t *self;

    if ((self = (luab_off_t *)arg) != NULL) {

        luab_setinteger(L, narg, "value", self->ud_value);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(OFF)) offo (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = off:get_table()
 */
static int
OFF_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = off_fillxtable;
    xtp.xtp_arg = (void *)luab_to_off(L, 1);
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
 * @usage iovec [, err, msg ] = off:dump()
 */
static int
OFF_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set off.
 *
 * @function set_value
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = off:set_value(data)
 */
static int
OFF_set_value(lua_State *L)
{
    luab_off_t *self;
    off_t x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_off(L, 1);
    x = (off_t)luab_checkinteger(L, 2, luab_env_ulong_max);

    self->ud_value = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get off.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = off:get_value()
 */
static int
OFF_get_value(lua_State *L)
{
    luab_off_t *self;
    off_t x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_off(L, 1);
    x = self->ud_value;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
OFF_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_off_type));
}

static int
OFF_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_off_type));
}

static int
OFF_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_off_type));
}

/*
 * Internal offerface.
 */

static luab_module_table_t off_methods[] = {
    LUAB_FUNC("set_value",      OFF_set_value),
    LUAB_FUNC("get_table",      OFF_get_table),
    LUAB_FUNC("get_value",      OFF_get_value),
    LUAB_FUNC("dump",           OFF_dump),
    LUAB_FUNC("__gc",           OFF_gc),
    LUAB_FUNC("__len",          OFF_len),
    LUAB_FUNC("__tostring",     OFF_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
off_create(lua_State *L, void *arg)
{
    return (luab_new_off(L, arg));
}

static void
off_init(void *ud, void *arg)
{
    luab_udata_init(&luab_off_type, ud, arg);
}

static void *
off_udata(lua_State *L, int narg)
{
    luab_off_t *self;
    self = luab_to_off(L, narg);
    return ((void *)&(self->ud_value));
}

static luab_table_t *
off_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    off_t *x, y;
    size_t m, n;

    if ((tbl = luab_table_newvectornil(L, narg, &luab_off_type)) != NULL) {

        if (((x = (off_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (off_t)luab_tointeger(L, -1, luab_env_ulong_max);
                        x[m] = (off_t)y;
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
off_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    off_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (off_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, new);

            for (m = 0, n = (tbl->tbl_card - 1), k = 1; m < n; m++, k++)
                luab_rawsetinteger(L, narg, k, x[m]);

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

static luab_table_t *
off_alloctable(void *vec, size_t card)
{
    return (luab_table_create(&luab_off_type, vec, card));
}

luab_module_t luab_off_type = {
    .m_id           = LUAB_OFF_TYPE_ID,
    .m_name         = LUAB_OFF_TYPE,
    .m_vec          = off_methods,
    .m_create       = off_create,
    .m_init         = off_init,
    .m_get          = off_udata,
    .m_get_tbl      = off_checktable,
    .m_set_tbl      = off_pushtable,
    .m_alloc_tbl    = off_alloctable,
    .m_len          = sizeof(luab_off_t),
    .m_sz           = sizeof(off_t),
};
