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

extern luab_module_t luab_div_type;

/*
 * Interface against
 *
 *  typedef struct {
 *      int quot;
 *      int rem;
 *  } div_t;
 */

typedef struct luab_div {
    luab_udata_t    ud_softc;
    div_t           ud_div;
} luab_div_t;

/*
 * Subr.
 */

static void
div_fillxtable(lua_State *L, int narg, void *arg)
{
    div_t *div;

    if ((div = (div_t *)arg) != NULL) {

        luab_setinteger(L, narg, "quot",  div->quot);
        luab_setinteger(L, narg, "rem",   div->rem);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(DIV)) into (LUA_TTABLE).
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
 * @usage t [, err, msg ] = div:get_table()
 */
static int
DIV_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DIV, TYPE, __func__);

    xtp.xtp_fill = div_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate div{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = div:dump()
 */
static int
DIV_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(DIV, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get quotient for integral division operations.
 *
 * @function get_quot
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = div:get_quot()
 */
static int
DIV_quot(lua_State *L)
{
    luab_module_t *m;
    div_t *div;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DIV, TYPE, __func__);
    div = luab_udata(L, 1, m, div_t *);
    x = div->quot;

    return (luab_pushxinteger(L, x));
}

/***
 * Get reminder of integal devision operations.
 *
 * @function get_rem
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = div:get_rem()
 */
static int
DIV_rem(lua_State *L)
{
    luab_module_t *m;
    div_t *div;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DIV, TYPE, __func__);
    div = luab_udata(L, 1, m, div_t *);
    x = div->rem;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
DIV_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(DIV, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
DIV_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(DIV, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
DIV_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(DIV, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t div_methods[] = {
    LUAB_FUNC("quot",           DIV_quot),
    LUAB_FUNC("rem",            DIV_rem),
    LUAB_FUNC("get_table",      DIV_get_table),
    LUAB_FUNC("dump",           DIV_dump),
    LUAB_FUNC("__gc",           DIV_gc),
    LUAB_FUNC("__len",          DIV_len),
    LUAB_FUNC("__tostring",     DIV_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
div_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(DIV, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
div_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(DIV, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
div_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(DIV, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
div_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    div_t *x, *y;
    size_t i, j;

    m = luab_xmod(DIV, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (div_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, div_t *);
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
div_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    div_t *x;
    size_t i, j, k;

    m = luab_xmod(DIV, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (div_t *)tbl->tbl_vec) != NULL) &&
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
div_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(DIV, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_div_type = {
    .m_id           = LUAB_DIV_TYPE_ID,
    .m_name         = LUAB_DIV_TYPE,
    .m_vec          = div_methods,
    .m_create       = div_create,
    .m_init         = div_init,
    .m_get          = div_udata,
    .m_get_tbl      = div_checktable,
    .m_set_tbl      = div_pushtable,
    .m_alloc_tbl    = div_alloctable,
    .m_len          = sizeof(luab_div_t),
    .m_sz           = sizeof(div_t),
};
