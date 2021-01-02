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
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_long_type;

/*
 * Interface against
 *
 *  long
 *
 */

typedef struct luab_long {
    luab_udata_t    ud_softc;
    long         ud_sdu;
} luab_long_t;

/*
 * Subr.
 */

static void
long_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_long_t *self;

    if ((self = (luab_long_t *)arg) != NULL) {

        luab_setinteger(L, narg, "value", self->ud_sdu);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(LONG)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = long:get_table()
 */
static int
LONG_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LONG, TYPE, __func__);

    xtp.xtp_fill = long_fillxtable;
    xtp.xtp_arg = luab_todata(L, 1, m, void *);
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
 * @usage iovec [, err, msg ] = long:dump()
 */
static int
LONG_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set value over (long).
 *
 * @function set_value
 *
 * @param arg               Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = long:set_value(arg)
 */
static int
LONG_set_value(lua_State *L)
{
    luab_module_t *m;
    luab_long_t *self;
    long x;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(LONG, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_long_t *);
    x = (long)luab_checkxinteger(L, 2, m, luab_env_ulong_max);
    self->ud_sdu = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value over (long).
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = long:get_value()
 */
static int
LONG_get_value(lua_State *L)
{
    luab_module_t *m;
    luab_long_t *self;
    long x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LONG, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_long_t *);
    x = self->ud_sdu;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
LONG_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LONG, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
LONG_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LONG, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
LONG_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LONG, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t long_methods[] = {
    LUAB_FUNC("set_value",      LONG_set_value),
    LUAB_FUNC("get_table",      LONG_get_table),
    LUAB_FUNC("get_value",      LONG_get_value),
    LUAB_FUNC("dump",           LONG_dump),
    LUAB_FUNC("__gc",           LONG_gc),
    LUAB_FUNC("__len",          LONG_len),
    LUAB_FUNC("__tostring",     LONG_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
long_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(LONG, TYPE, __func__);
    return (luab_newudata(L, m, arg));
}

static void
long_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(LONG, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
long_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_long_t *self;

    m = luab_xmod(LONG, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_long_t *);
    return ((void *)&(self->ud_sdu));
}

static luab_table_t *
long_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    long *x, y;
    size_t i, j;

    m = luab_xmod(LONG, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (long *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (long)luab_toxinteger(L, -1, m, luab_env_ulong_max);
                        x[i] = (long)y;
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
long_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    long *x;
    size_t i, j, k;

    if (tbl != NULL) {

        if (((x = (long *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, new);

            for (i = 0, j = tbl->tbl_card, k = 1; i < j; i++, k++)
                luab_rawsetinteger(L, narg, k, x[i]);

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

static luab_table_t *
long_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(LONG, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_long_type = {
    .m_id           = LUAB_LONG_TYPE_ID,
    .m_name         = LUAB_LONG_TYPE,
    .m_vec          = long_methods,
    .m_create       = long_create,
    .m_init         = long_init,
    .m_get          = long_udata,
    .m_get_tbl      = long_checktable,
    .m_set_tbl      = long_pushtable,
    .m_alloc_tbl    = long_alloctable,
    .m_len          = sizeof(luab_long_t),
    .m_sz           = sizeof(long),
};
