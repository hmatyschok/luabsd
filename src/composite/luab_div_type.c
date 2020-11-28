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

#define luab_new_div(L, arg) \
    ((luab_div_t *)luab_newudata(L, &luab_div_type, (arg)))
#define luab_to_div(L, narg) \
    (luab_toldata((L), (narg), &luab_div_type, \
        div_t *, sizeof(div_t)))

/*
 * Subr.
 */

static void
div_initxtable(lua_State *L, int narg, void *arg)
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
 * @function get
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              quot      = (LUA_TNUMBER),
 *              rem    = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = div:get()
 */
static int
DIV_get(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_init = div_initxtable;
    xtp.xtp_arg = luab_xdata(L, 1, &luab_div_type);
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
    return (luab_core_dump(L, 1, &luab_div_type, sizeof(div_t)));
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
 * @usage data [, err, msg ] = div:get_quot()
 */
static int
DIV_quot(lua_State *L)
{
    div_t *div;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    div = luab_udata(L, 1, &luab_div_type, div_t *);
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
 * @usage data [, err, msg ] = div:get_rem()
 */
static int
DIV_rem(lua_State *L)
{
    div_t *div;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    div = luab_udata(L, 1, &luab_div_type, div_t *);
    x = div->rem;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
DIV_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_div_type));
}

static int
DIV_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_div_type));
}

static int
DIV_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_div_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t div_methods[] = {
    LUAB_FUNC("quot",           DIV_quot),
    LUAB_FUNC("rem",            DIV_rem),
    LUAB_FUNC("get",            DIV_get),
    LUAB_FUNC("dump",           DIV_dump),
    LUAB_FUNC("__gc",           DIV_gc),
    LUAB_FUNC("__len",          DIV_len),
    LUAB_FUNC("__tostring",     DIV_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
div_create(lua_State *L, void *arg)
{
    return (luab_new_div(L, arg));
}

static void
div_init(void *ud, void *arg)
{
    luab_udata_init(&luab_div_type, ud, arg);
}

static void *
div_udata(lua_State *L, int narg)
{
    return (luab_to_div(L, narg));
}

static luab_table_t *
div_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    div_t *x, *y;
    size_t m, n, sz;

    sz = sizeof(div_t);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (div_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_div_type, div_t *);
                        (void)memmove(&(x[m]), y, sz);
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
    div_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (div_t *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_div_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

luab_module_t luab_div_type = {
    .m_cookie   = LUAB_DIV_TYPE_ID,
    .m_name     = LUAB_DIV_TYPE,
    .m_vec      = div_methods,
    .m_create   = div_create,
    .m_init     = div_init,
    .m_get      = div_udata,
    .m_get_tbl  = div_checktable,
    .m_set_tbl  = div_pushtable,
    .m_sz       = sizeof(luab_div_t),
};
