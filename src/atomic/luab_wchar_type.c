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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS WCHARERRUPTION)
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

extern luab_module_t luab_wchar_type;

/*
 * Interface against
 *
 *  wchar_t
 *
 */

typedef struct luab_wchar {
    luab_udata_t    ud_softc;
    wchar_t         ud_sdu;
} luab_wchar_t;

#define luab_new_wchar(L, arg) \
    ((luab_wchar_t *)luab_newudata(L, &luab_wchar_type, (arg)))
#define luab_to_wchar(L, narg) \
    (luab_todata((L), (narg), &luab_wchar_type, luab_wchar_t *))

/*
 * Subr.
 */

static void
wchar_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_wchar_t *self;

    if ((self = (luab_wchar_t *)arg) != NULL) {

        luab_setinteger(L, narg, "value", self->ud_sdu);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(WCHAR)) wcharo (LUA_TTABLE).
 *
 * @function get_table
 *
  * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value   = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = wchar:get_table()
 */
static int
WCHAR_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = wchar_fillxtable;
    xtp.xtp_arg = (void *)luab_to_wchar(L, 1);
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
 * @usage iovec [, err, msg ] = wchar:dump()
 */
static int
WCHAR_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set wchar.
 *
 * @function set_value
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = wchar:set_value(data)
 */
static int
WCHAR_set_value(lua_State *L)
{
    luab_wchar_t *self;
    wchar_t x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_wchar(L, 1);
    x = (wchar_t)luab_checkinteger(L, 2, luab_env_uint_max);

    self->ud_sdu = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get wchar.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = wchar:get_value()
 */
static int
WCHAR_get_value(lua_State *L)
{
    luab_wchar_t *self;
    wchar_t x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_wchar(L, 1);
    x = self->ud_sdu;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
WCHAR_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_wchar_type));
}

static int
WCHAR_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_wchar_type));
}

static int
WCHAR_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_wchar_type));
}

/*
 * Internal wcharerface.
 */

static luab_module_table_t wchar_methods[] = {
    LUAB_FUNC("set_value",      WCHAR_set_value),
    LUAB_FUNC("get_table",      WCHAR_get_table),
    LUAB_FUNC("get_value",      WCHAR_get_value),
    LUAB_FUNC("dump",           WCHAR_dump),
    LUAB_FUNC("__gc",           WCHAR_gc),
    LUAB_FUNC("__len",          WCHAR_len),
    LUAB_FUNC("__tostring",     WCHAR_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
wchar_create(lua_State *L, void *arg)
{
    return (luab_new_wchar(L, arg));
}

static void
wchar_init(void *ud, void *arg)
{
    luab_udata_init(&luab_wchar_type, ud, arg);
}

static void *
wchar_udata(lua_State *L, int narg)
{
    luab_wchar_t *self;
    self = luab_to_wchar(L, narg);
    return ((void *)&(self->ud_sdu));
}

static luab_table_t *
wchar_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    wchar_t *x, y;
    size_t m, n;

    if ((tbl = luab_table_newvectornil(L, narg, &luab_wchar_type)) != NULL) {

        if (((x = (wchar_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (m = 0, n = tbl->tbl_card; m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (wchar_t)luab_tointeger(L, -1, luab_env_uint_max);
                        x[m] = (wchar_t)y;
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
wchar_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    wchar_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (wchar_t *)tbl->tbl_vec) != NULL) &&
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
wchar_alloctable(void *vec, size_t card)
{
    return (luab_table_create(&luab_wchar_type, vec, card));
}

luab_module_t luab_wchar_type = {
    .m_id           = LUAB_WCHAR_TYPE_ID,
    .m_name         = LUAB_WCHAR_TYPE,
    .m_vec          = wchar_methods,
    .m_create       = wchar_create,
    .m_init         = wchar_init,
    .m_get          = wchar_udata,
    .m_get_tbl      = wchar_checktable,
    .m_set_tbl      = wchar_pushtable,
    .m_alloc_tbl    = wchar_alloctable,
    .m_len          = sizeof(luab_wchar_t),
    .m_sz           = sizeof(wchar_t),
};
