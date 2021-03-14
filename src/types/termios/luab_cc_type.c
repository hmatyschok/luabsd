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

#include <termios.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_cc_type;

/*
 * Interface against
 *
 *  cc_t
 *
 */

typedef struct luab_cc {
    luab_udata_t    ud_softc;
    cc_t         ud_x;
} luab_cc_t;

/*
 * Subr.
 */

static void
cc_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_cc_t *self;

    if ((self = (luab_cc_t *)arg) != NULL) {
        luab_setinteger(L, narg, "value", self->ud_x);

   } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(CC)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = cc:get_table()
 */
static int
CC_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CC, TYPE, __func__);

    xtp.xtp_fill = cc_fillxtable;
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
 * @usage iovec [, err, msg ] = cc:dump()
 */
static int
CC_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set value over (cc_t).
 *
 * @function set_value
 *
 * @param arg               Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = cc:set_value(arg)
 */
static int
CC_set_value(lua_State *L)
{
    luab_module_t *m;
    luab_cc_t *self;
    cc_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(CC, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_cc_t *);
    x = (cc_t)luab_checkxinteger(L, 2, m, luab_env_uchar_max);
    self->ud_x = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value over (cc_t).
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = cc:get_value()
 */
static int
CC_get_value(lua_State *L)
{
    luab_module_t *m;
    luab_cc_t *self;
    cc_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CC, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_cc_t *);
    x = self->ud_x;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
CC_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(CC, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
CC_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(CC, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
CC_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(CC, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t cc_methods[] = {
    LUAB_FUNC("set_value",      CC_set_value),
    LUAB_FUNC("get_table",      CC_get_table),
    LUAB_FUNC("get_value",      CC_get_value),
    LUAB_FUNC("dump",           CC_dump),
    LUAB_FUNC("__gc",           CC_gc),
    LUAB_FUNC("__len",          CC_len),
    LUAB_FUNC("__tostring",     CC_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
cc_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(CC, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
cc_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(CC, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
cc_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_cc_t *self;

    m = luab_xmod(CC, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_cc_t *);
    return ((void *)&(self->ud_x));
}

static luab_table_t *
cc_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    cc_t *x, y;
    size_t i, j;

    m = luab_xmod(CC, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (cc_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (cc_t)luab_toxinteger(L, -1, m, luab_env_uchar_max);
                        x[i] = (cc_t)y;
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
cc_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    cc_t *x;
    size_t i, j, k;

    if (tbl != NULL) {

        if (((x = (cc_t *)tbl->tbl_vec) != NULL) &&
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
cc_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(CC, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_cc_type = {
    .m_id           = LUAB_CC_TYPE_ID,
    .m_name         = LUAB_CC_TYPE,
    .m_vec          = cc_methods,
    .m_create       = cc_create,
    .m_init         = cc_init,
    .m_get          = cc_udata,
    .m_get_tbl      = cc_checktable,
    .m_set_tbl      = cc_pushtable,
    .m_alloc_tbl    = cc_alloctable,
    .m_len          = sizeof(luab_cc_t),
    .m_sz           = sizeof(cc_t),
};
