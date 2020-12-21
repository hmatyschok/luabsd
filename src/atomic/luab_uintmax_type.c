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
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_uintmax_type;

/*
 * Interface against
 *
 *  uintmax_t
 *
 */

typedef struct luab_uintmax {
    luab_udata_t    ud_softc;
    uintmax_t        ud_sdu;
} luab_uintmax_t;

#define luab_new_uintmax(L, arg) \
    ((luab_uintmax_t *)luab_newudata(L, &luab_uintmax_type, (arg)))
#define luab_to_uintmax(L, narg) \
    (luab_todata((L), (narg), &luab_uintmax_type, luab_uintmax_t *))

/*
 * Subr.
 */

static void
uintmax_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_uintmax_t *self;

    if ((self = (luab_uintmax_t *)arg) != NULL) {

        luab_setinteger(L, narg, "value", self->ud_sdu);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(UINTMAX)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ] = uintmax:get_table()
 */
static int
UINTMAX_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = uintmax_fillxtable;
    xtp.xtp_arg = (void *)luab_to_uintmax(L, 1);
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
 * @usage iovec [, err, msg ] = uintmax:dump()
 */
static int
UINTMAX_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set uintmax.
 *
 * @function set_value
 *
 * @param x                 Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = uintmax:set_value(x)
 */
static int
UINTMAX_set_value(lua_State *L)
{
    luab_uintmax_t *self;
    uintmax_t x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_uintmax(L, 1);
    x = (uintmax_t)luab_checkinteger(L, 2, luab_env_ullong_max);

    self->ud_sdu = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get uintmax.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = uintmax:get_value()
 */
static int
UINTMAX_get_value(lua_State *L)
{
    luab_uintmax_t *self;
    uintmax_t x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_uintmax(L, 1);
    x = self->ud_sdu;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
UINTMAX_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_uintmax_type));
}

static int
UINTMAX_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_uintmax_type));
}

static int
UINTMAX_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_uintmax_type));
}

/*
 * Internal uintmaxerface.
 */

static luab_module_table_t uintmax_methods[] = {
    LUAB_FUNC("set_value",      UINTMAX_set_value),
    LUAB_FUNC("get_table",      UINTMAX_get_table),
    LUAB_FUNC("get_value",      UINTMAX_get_value),
    LUAB_FUNC("dump",           UINTMAX_dump),
    LUAB_FUNC("__gc",           UINTMAX_gc),
    LUAB_FUNC("__len",          UINTMAX_len),
    LUAB_FUNC("__tostring",     UINTMAX_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
uintmax_create(lua_State *L, void *arg)
{
    return (luab_new_uintmax(L, arg));
}

static void
uintmax_init(void *ud, void *arg)
{
    luab_udata_init(&luab_uintmax_type, ud, arg);
}

static void *
uintmax_udata(lua_State *L, int narg)
{
    luab_uintmax_t *self;
    self = luab_to_uintmax(L, narg);
    return ((void *)&(self->ud_sdu));
}

static luab_table_t *
uintmax_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    uintmax_t *x, y;
    size_t m, n;

    if ((tbl = luab_table_newvectornil(L, narg, &luab_uintmax_type)) != NULL) {

        if (((x = (uintmax_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (m = 0, n = tbl->tbl_card; m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (uintmax_t)luab_tointeger(L, -1, luab_env_ullong_max);
                        x[m] = (uintmax_t)y;
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
uintmax_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    uintmax_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (uintmax_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, new);

            for (m = 0, n = tbl->tbl_card, k = 1; m < n; m++, k++)
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
uintmax_alloctable(void *vec, size_t card)
{
    return (luab_table_create(&luab_uintmax_type, vec, card));
}

luab_module_t luab_uintmax_type = {
    .m_id           = LUAB_UINTMAX_TYPE_ID,
    .m_name         = LUAB_UINTMAX_TYPE,
    .m_vec          = uintmax_methods,
    .m_create       = uintmax_create,
    .m_init         = uintmax_init,
    .m_get          = uintmax_udata,
    .m_get_tbl      = uintmax_checktable,
    .m_set_tbl      = uintmax_pushtable,
    .m_alloc_tbl    = uintmax_alloctable,
    .m_len          = sizeof(luab_uintmax_t),
    .m_sz           = sizeof(uintmax_t),
};
