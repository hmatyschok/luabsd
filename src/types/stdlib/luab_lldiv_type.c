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

#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_lldiv_type;

/*
 * Interface against
 *
 *  typedef struct {
 *      long long   quot;
 *      long long   rem;
 *  } lldiv_t;
 */

typedef struct luab_lldiv {
    luab_udata_t    ud_softc;
    lldiv_t         ud_lldiv;
} luab_lldiv_t;

/*
 * Subr.
 */

static void
lldiv_fillxtable(lua_State *L, int narg, void *arg)
{
    lldiv_t *lldiv;

    if ((lldiv = (lldiv_t *)arg) != NULL) {

        luab_setinteger(L, narg, "quot",  lldiv->quot);
        luab_setinteger(L, narg, "rem",   lldiv->rem);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(LLDIV)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              quot      = (LUA_TNUMBER),
 *              rem    = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = lldiv:get_table()
 */
static int
LLDIV_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LLDIV, TYPE, __func__);

    xtp.xtp_fill = lldiv_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate lldiv{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = lldiv:dump()
 */
static int
LLDIV_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LLDIV, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions.
 */

/***
 * Get quotient for integral lldivision operations.
 *
 * @function quot
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lldiv:quot()
 */
static int
LLDIV_quot(lua_State *L)
{
    luab_module_t *m;
    lldiv_t *lldiv;
    long long x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LLDIV, TYPE, __func__);
    lldiv = luab_udata(L, 1, m, lldiv_t *);
    x = lldiv->quot;

    return (luab_pushxinteger(L, x));
}

/***
 * Get reminder of integal devision operations.
 *
 * @function rem
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lldiv:rem()
 */
static int
LLDIV_rem(lua_State *L)
{
    luab_module_t *m;
    lldiv_t *lldiv;
    long long x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LLDIV, TYPE, __func__);
    lldiv = luab_udata(L, 1, m, lldiv_t *);
    x = lldiv->rem;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
LLDIV_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LLDIV, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
LLDIV_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LLDIV, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
LLDIV_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LLDIV, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t lldiv_methods[] = {
    LUAB_FUNC("quot",           LLDIV_quot),
    LUAB_FUNC("rem",            LLDIV_rem),
    LUAB_FUNC("get_table",      LLDIV_get_table),
    LUAB_FUNC("dump",           LLDIV_dump),
    LUAB_FUNC("__gc",           LLDIV_gc),
    LUAB_FUNC("__len",          LLDIV_len),
    LUAB_FUNC("__tostring",     LLDIV_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
lldiv_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(LLDIV, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
lldiv_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(LLDIV, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
lldiv_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(LLDIV, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
lldiv_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    lldiv_t *x, *y;
    size_t i, j;

    m = luab_xmod(LLDIV, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (lldiv_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, lldiv_t *);
                        (void)memmove(&(x[i]), y, m->m_sz);
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
lldiv_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    lldiv_t *x;
    size_t i, j, k;

    m = luab_xmod(LLDIV, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (lldiv_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, new);

            for (i = 0, j = tbl->tbl_card, k = 1; i < j; i++, k++)
                luab_rawsetxdata(L, narg, m, k, &(x[i]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

static luab_table_t *
lldiv_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(LLDIV, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_lldiv_type = {
    .m_id           = LUAB_LLDIV_TYPE_ID,
    .m_name         = LUAB_LLDIV_TYPE,
    .m_vec          = lldiv_methods,
    .m_create       = lldiv_create,
    .m_init         = lldiv_init,
    .m_get          = lldiv_udata,
    .m_get_tbl      = lldiv_checktable,
    .m_set_tbl      = lldiv_pushtable,
    .m_alloc_tbl    = lldiv_alloctable,
    .m_len          = sizeof(luab_lldiv_t),
    .m_sz           = sizeof(lldiv_t),
};
