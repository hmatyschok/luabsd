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

#define luab_new_lldiv(L, arg) \
    ((luab_lldiv_t *)luab_newudata(L, &luab_lldiv_type, (arg)))
#define luab_to_lldiv(L, narg) \
    (luab_toldata((L), (narg), &luab_lldiv_type, \
        lldiv_t *, luab_lldiv_type.m_sz))

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
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = lldiv_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, &luab_lldiv_type);
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
    return (luab_core_dump(L, 1, &luab_lldiv_type, luab_lldiv_type.m_sz));
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
 * @usage data [, err, msg ] = lldiv:quot()
 */
static int
LLDIV_quot(lua_State *L)
{
    lldiv_t *lldiv;
    long long x;

    (void)luab_core_checkmaxargs(L, 1);

    lldiv = luab_udata(L, 1, &luab_lldiv_type, lldiv_t *);
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
 * @usage data [, err, msg ] = lldiv:rem()
 */
static int
LLDIV_rem(lua_State *L)
{
    lldiv_t *lldiv;
    long long x;

    (void)luab_core_checkmaxargs(L, 1);

    lldiv = luab_udata(L, 1, &luab_lldiv_type, lldiv_t *);
    x = lldiv->rem;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
LLDIV_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_lldiv_type));
}

static int
LLDIV_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_lldiv_type));
}

static int
LLDIV_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_lldiv_type));
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
    return (luab_new_lldiv(L, arg));
}

static void
lldiv_init(void *ud, void *arg)
{
    luab_udata_init(&luab_lldiv_type, ud, arg);
}

static void *
lldiv_udata(lua_State *L, int narg)
{
    return (luab_to_lldiv(L, narg));
}

static luab_table_t *
lldiv_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    lldiv_t *x, *y;
    size_t m, n;

    if ((tbl = luab_table_newvectornil(L, narg, &luab_lldiv_type)) != NULL) {

        if (((x = (lldiv_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_lldiv_type, lldiv_t *);
                        (void)memmove(&(x[m]), y, luab_lldiv_type.m_sz);
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
    lldiv_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (lldiv_t *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) > 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_lldiv_type, k, &(x[m]));

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
    return (luab_table_create(&luab_lldiv_type, vec, card));
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
