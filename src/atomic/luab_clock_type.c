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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS CLOCKERRUPTION)
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

extern luab_module_t luab_clock_type;

/*
 * Interface against
 *
 *  clock_t
 *
 */

typedef struct luab_clock {
    luab_udata_t    ud_softc;
    clock_t         ud_value;
} luab_clock_t;

#define luab_new_clock(L, arg) \
    ((luab_clock_t *)luab_newudata(L, &luab_clock_type, (arg)))
#define luab_to_clock(L, narg) \
    (luab_todata((L), (narg), &luab_clock_type, luab_clock_t *))

/*
 * Subr.
 */

static void
clock_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_clock_t *self;

    if ((self = (luab_clock_t *)arg) != NULL) {

        luab_setinteger(L, narg, "value", self->ud_value);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(CLOCK)) clocko (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = clock:get_table()
 */
static int
CLOCK_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = clock_fillxtable;
    xtp.xtp_arg = (void *)luab_to_clock(L, 1);
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
 * @usage iovec [, err, msg ] = clock:dump()
 */
static int
CLOCK_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set clock.
 *
 * @function set_value
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = clock:set_value(data)
 */
static int
CLOCK_set_value(lua_State *L)
{
    luab_clock_t *self;
    clock_t x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_clock(L, 1);
    x = (clock_t)luab_checklinteger(L, 2, 1);

    self->ud_value = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get clock.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = clock:get_value()
 */
static int
CLOCK_get_value(lua_State *L)
{
    luab_clock_t *self;
    clock_t x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_clock(L, 1);
    x = self->ud_value;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
CLOCK_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_clock_type));
}

static int
CLOCK_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_clock_type));
}

static int
CLOCK_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_clock_type));
}

/*
 * Internal clockerface.
 */

static luab_module_table_t clock_methods[] = {
    LUAB_FUNC("set_value",      CLOCK_set_value),
    LUAB_FUNC("get_table",      CLOCK_get_table),
    LUAB_FUNC("get_value",      CLOCK_get_value),
    LUAB_FUNC("dump",           CLOCK_dump),
    LUAB_FUNC("__gc",           CLOCK_gc),
    LUAB_FUNC("__len",          CLOCK_len),
    LUAB_FUNC("__tostring",     CLOCK_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
clock_create(lua_State *L, void *arg)
{
    return (luab_new_clock(L, arg));
}

static void
clock_init(void *ud, void *arg)
{
    luab_udata_init(&luab_clock_type, ud, arg);
}

static void *
clock_udata(lua_State *L, int narg)
{
    luab_clock_t *self;
    self = luab_to_clock(L, narg);
    return ((void *)&(self->ud_value));
}

static luab_table_t *
clock_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    clock_t *x, y;
    size_t m, n;

    if ((tbl = luab_table_newvectornil(L, narg, &luab_clock_type)) != NULL) {

        if (((x = (clock_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (m = 0, n = tbl->tbl_card; m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (clock_t)luab_tolinteger(L, -1, 1);
                        x[m] = (clock_t)y;
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
clock_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    clock_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (clock_t *)tbl->tbl_vec) != NULL) &&
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
clock_alloctable(void *vec, size_t card)
{
    return (luab_table_create(&luab_clock_type, vec, card));
}

luab_module_t luab_clock_type = {
    .m_id           = LUAB_CLOCK_TYPE_ID,
    .m_name         = LUAB_CLOCK_TYPE,
    .m_vec          = clock_methods,
    .m_create       = clock_create,
    .m_init         = clock_init,
    .m_get          = clock_udata,
    .m_get_tbl      = clock_checktable,
    .m_set_tbl      = clock_pushtable,
    .m_alloc_tbl    = clock_alloctable,
    .m_len          = sizeof(luab_clock_t),
    .m_sz           = sizeof(clock_t),
};
