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

#if __BSD_VISIBLE
extern luab_module_t luab_ushrt_type;

/*
 * Interface against
 *
 *  u_short
 *
 */

typedef struct luab_ushrt {
    luab_udata_t    ud_softc;
    u_short         ud_sdu;
} luab_ushrt_t;

/*
 * Subr.
 */

static void
ushrt_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_ushrt_t *self;

    if ((self = (luab_ushrt_t *)arg) != NULL) {

        luab_setinteger(L, narg, "value", self->ud_sdu);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(USHRT)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = ushrt:get_table()
 */
static int
USHRT_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(USHRT, TYPE, __func__);

    xtp.xtp_fill = ushrt_fillxtable;
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
 * @usage iovec [, err, msg ] = ushrt:dump()
 */
static int
USHRT_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set value over (u_short).
 *
 * @function set_value
 *
 * @param arg               Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ushrt:set_value(arg)
 */
static int
USHRT_set_value(lua_State *L)
{
    luab_module_t *m;
    luab_ushrt_t *self;
    u_short x;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(USHRT, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_ushrt_t *);
    x = (u_short)luab_checkxinteger(L, 2, m, luab_env_ushrt_max);
    self->ud_sdu = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value over (u_short).
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ushrt:get_value()
 */
static int
USHRT_get_value(lua_State *L)
{
    luab_module_t *m;
    luab_ushrt_t *self;
    u_short x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(USHRT, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_ushrt_t *);
    x = self->ud_sdu;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
USHRT_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(USHRT, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
USHRT_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(USHRT, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
USHRT_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(USHRT, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t ushrt_methods[] = {
    LUAB_FUNC("set_value",      USHRT_set_value),
    LUAB_FUNC("get_table",      USHRT_get_table),
    LUAB_FUNC("get_value",      USHRT_get_value),
    LUAB_FUNC("dump",           USHRT_dump),
    LUAB_FUNC("__gc",           USHRT_gc),
    LUAB_FUNC("__len",          USHRT_len),
    LUAB_FUNC("__tostring",     USHRT_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
ushrt_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(USHRT, TYPE, __func__);
    return (luab_newudata(L, m, arg));
}

static void
ushrt_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(USHRT, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
ushrt_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_ushrt_t *self;

    m = luab_xmod(USHRT, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_ushrt_t *);
    return ((void *)&(self->ud_sdu));
}

static luab_table_t *
ushrt_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    u_short *x, y;
    size_t i, j;

    m = luab_xmod(USHRT, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (u_short *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (u_short)luab_toxinteger(L, -1, m, luab_env_ushrt_max);
                        x[i] = (u_short)y;
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
ushrt_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    u_short *x;
    size_t i, j, k;

    if (tbl != NULL) {

        if (((x = (u_short *)tbl->tbl_vec) != NULL) &&
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
ushrt_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(USHRT, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_ushrt_type = {
    .m_id           = LUAB_USHRT_TYPE_ID,
    .m_name         = LUAB_USHRT_TYPE,
    .m_vec          = ushrt_methods,
    .m_create       = ushrt_create,
    .m_init         = ushrt_init,
    .m_get          = ushrt_udata,
    .m_get_tbl      = ushrt_checktable,
    .m_set_tbl      = ushrt_pushtable,
    .m_alloc_tbl    = ushrt_alloctable,
    .m_len          = sizeof(luab_ushrt_t),
    .m_sz           = sizeof(u_short),
};
#endif /* __BSD_VISIBLE */
