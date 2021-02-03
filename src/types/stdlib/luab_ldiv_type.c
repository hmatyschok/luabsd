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

extern luab_module_t luab_ldiv_type;

/*
 * Interface against
 *
 *  typedef struct {
 *      long    quot;
 *      long    rem;
 *  } ldiv_t;
 */

typedef struct luab_ldiv {
    luab_udata_t    ud_softc;
    ldiv_t          ud_ldiv;
} luab_ldiv_t;

/*
 * Subr.
 */

static void
ldiv_fillxtable(lua_State *L, int narg, void *arg)
{
    ldiv_t *ldiv;

    if ((ldiv = (ldiv_t *)arg) != NULL) {

        luab_setinteger(L, narg, "quot",  ldiv->quot);
        luab_setinteger(L, narg, "rem",   ldiv->rem);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(LDIV)) into (LUA_TTABLE).
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
 * @usage t [, err, msg ] = ldiv:get_table()
 */
static int
LDIV_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LDIV, TYPE, __func__);

    xtp.xtp_fill = ldiv_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate ldiv{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = ldiv:dump()
 */
static int
LDIV_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LDIV, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get quotient for integral ldivision operations.
 *
 * @function quot
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ldiv:quot()
 */
static int
LDIV_quot(lua_State *L)
{
    luab_module_t *m;
    ldiv_t *ldiv;
    long x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LDIV, TYPE, __func__);
    ldiv = luab_udata(L, 1, m, ldiv_t *);
    x = ldiv->quot;

    return (luab_pushxinteger(L, x));
}

/***
 * Get reminder of integal devision operations.
 *
 * @function rem
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ldiv:rem()
 */
static int
LDIV_rem(lua_State *L)
{
    luab_module_t *m;
    ldiv_t *ldiv;
    long x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LDIV, TYPE, __func__);
    ldiv = luab_udata(L, 1, m, ldiv_t *);
    x = ldiv->rem;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
LDIV_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LDIV, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
LDIV_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LDIV, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
LDIV_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LDIV, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t ldiv_methods[] = {
    LUAB_FUNC("quot",           LDIV_quot),
    LUAB_FUNC("rem",            LDIV_rem),
    LUAB_FUNC("get_table",      LDIV_get_table),
    LUAB_FUNC("dump",           LDIV_dump),
    LUAB_FUNC("__gc",           LDIV_gc),
    LUAB_FUNC("__len",          LDIV_len),
    LUAB_FUNC("__tostring",     LDIV_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
ldiv_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(LDIV, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
ldiv_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(LDIV, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
ldiv_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(LDIV, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
ldiv_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    ldiv_t *x, *y;
    size_t i, j;

    m = luab_xmod(LDIV, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (ldiv_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, ldiv_t *);
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
ldiv_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    ldiv_t *x;
    size_t i, j, k;

    m = luab_xmod(LDIV, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (ldiv_t *)tbl->tbl_vec) != NULL) &&
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
ldiv_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(LDIV, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_ldiv_type = {
    .m_id           = LUAB_LDIV_TYPE_ID,
    .m_name         = LUAB_LDIV_TYPE,
    .m_vec          = ldiv_methods,
    .m_create       = ldiv_create,
    .m_init         = ldiv_init,
    .m_get          = ldiv_udata,
    .m_get_tbl      = ldiv_checktable,
    .m_set_tbl      = ldiv_pushtable,
    .m_alloc_tbl    = ldiv_alloctable,
    .m_len          = sizeof(luab_ldiv_t),
    .m_sz           = sizeof(ldiv_t),
};
