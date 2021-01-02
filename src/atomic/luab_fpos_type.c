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

extern luab_module_t luab_fpos_type;

/*
 * Interface against
 *
 *  fpos_t
 *
 */

typedef struct luab_fpos {
    luab_udata_t    ud_softc;
    fpos_t          ud_sdu;
} luab_fpos_t;

/*
 * Subr.
 */

static void
fpos_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_fpos_t *self;

    if ((self = (luab_fpos_t *)arg) != NULL) {

        luab_setinteger(L, narg, "value", self->ud_sdu);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(FPOS)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = fpos:get_table()
 */
static int
FPOS_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FPOS, TYPE, __func__);

    xtp.xtp_fill = fpos_fillxtable;
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
 * @usage iovec [, err, msg ] = fpos:dump()
 */
static int
FPOS_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set value over (fpos_t).
 *
 * @function set_value
 *
 * @param arg               Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = fpos:set_value(arg)
 */
static int
FPOS_set_value(lua_State *L)
{
    luab_module_t *m;
    luab_fpos_t *self;
    fpos_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(FPOS, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_fpos_t *);
    x = (fpos_t)luab_checkxinteger(L, 2, m, luab_env_ulong_max);
    self->ud_sdu = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value over (fpos_t).
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = fpos:get_value()
 */
static int
FPOS_get_value(lua_State *L)
{
    luab_module_t *m;
    luab_fpos_t *self;
    fpos_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FPOS, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_fpos_t *);
    x = self->ud_sdu;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
FPOS_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FPOS, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
FPOS_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FPOS, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
FPOS_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FPOS, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t fpos_methods[] = {
    LUAB_FUNC("set_value",      FPOS_set_value),
    LUAB_FUNC("get_table",      FPOS_get_table),
    LUAB_FUNC("get_value",      FPOS_get_value),
    LUAB_FUNC("dump",           FPOS_dump),
    LUAB_FUNC("__gc",           FPOS_gc),
    LUAB_FUNC("__len",          FPOS_len),
    LUAB_FUNC("__tostring",     FPOS_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
fpos_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(FPOS, TYPE, __func__);
    return (luab_newudata(L, m, arg));
}

static void
fpos_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(FPOS, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
fpos_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_fpos_t *self;

    m = luab_xmod(FPOS, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_fpos_t *);
    return ((void *)&(self->ud_sdu));
}

static luab_table_t *
fpos_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    fpos_t *x, y;
    size_t i, j;

    m = luab_xmod(FPOS, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (fpos_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (fpos_t)luab_toxinteger(L, -1, m, luab_env_ulong_max);
                        x[i] = (fpos_t)y;
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        } else
            errno = ERANGE;
    }
    return (tbl);
}

static void
fpos_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    fpos_t *x;
    size_t i, j, k;

    if (tbl != NULL) {

        if (((x = (fpos_t *)tbl->tbl_vec) != NULL) &&
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
fpos_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(FPOS, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_fpos_type = {
    .m_id           = LUAB_FPOS_TYPE_ID,
    .m_name         = LUAB_FPOS_TYPE,
    .m_vec          = fpos_methods,
    .m_create       = fpos_create,
    .m_init         = fpos_init,
    .m_get          = fpos_udata,
    .m_get_tbl      = fpos_checktable,
    .m_set_tbl      = fpos_pushtable,
    .m_alloc_tbl    = fpos_alloctable,
    .m_len          = sizeof(luab_fpos_t),
    .m_sz           = sizeof(fpos_t),
};
