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

#include <langinfo.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_nl_item_type;

/*
 * Interface against
 *
 *  nl_item
 *
 */

typedef struct luab_nl_item {
    luab_udata_t    ud_softc;
    nl_item          ud_sdu;
} luab_nl_item;

#define luab_new_nl_item(L, arg) \
    ((luab_nl_item *)luab_newudata(L, &luab_nl_item_type, (arg)))
#define luab_to_nl_item(L, narg) \
    (luab_todata((L), (narg), &luab_nl_item_type, luab_nl_item *))

/*
 * Subr.
 */

static void
nl_item_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_nl_item *self;

    if ((self = (luab_nl_item *)arg) != NULL) {

        luab_setinteger(L, narg, "value", self->ud_sdu);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(NL_ITEM)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = nl_item:get_table()
 */
static int
NL_ITEM_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = nl_item_fillxtable;
    xtp.xtp_arg = (void *)luab_to_nl_item(L, 1);
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
 * @usage iovec [, err, msg ] = nl_item:dump()
 */
static int
NL_ITEM_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set nl_item.
 *
 * @function set_value
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = nl_item:set_value(data)
 */
static int
NL_ITEM_set_value(lua_State *L)
{
    luab_nl_item *self;
    nl_item x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_nl_item(L, 1);
    x = (nl_item)luab_checkinteger(L, 2, luab_env_ullong_max);

    self->ud_sdu = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get nl_item.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = nl_item:get_value()
 */
static int
NL_ITEM_get_value(lua_State *L)
{
    luab_nl_item *self;
    nl_item x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_nl_item(L, 1);
    x = self->ud_sdu;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
NL_ITEM_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_nl_item_type));
}

static int
NL_ITEM_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_nl_item_type));
}

static int
NL_ITEM_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_nl_item_type));
}

/*
 * Internal nl_itemerface.
 */

static luab_module_table_t nl_item_methods[] = {
    LUAB_FUNC("set_value",      NL_ITEM_set_value),
    LUAB_FUNC("get_table",      NL_ITEM_get_table),
    LUAB_FUNC("get_value",      NL_ITEM_get_value),
    LUAB_FUNC("dump",           NL_ITEM_dump),
    LUAB_FUNC("__gc",           NL_ITEM_gc),
    LUAB_FUNC("__len",          NL_ITEM_len),
    LUAB_FUNC("__tostring",     NL_ITEM_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
nl_item_create(lua_State *L, void *arg)
{
    return (luab_new_nl_item(L, arg));
}

static void
nl_item_init(void *ud, void *arg)
{
    luab_udata_init(&luab_nl_item_type, ud, arg);
}

static void *
nl_item_udata(lua_State *L, int narg)
{
    luab_nl_item *self;
    self = luab_to_nl_item(L, narg);
    return ((void *)&(self->ud_sdu));
}

static luab_table_t *
nl_item_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    nl_item *x, y;
    size_t m, n;

    if ((tbl = luab_table_newvectornil(L, narg, &luab_nl_item_type)) != NULL) {

        if (((x = (nl_item *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (m = 0, n = tbl->tbl_card; m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (nl_item)luab_tointeger(L, -1, luab_env_ullong_max);
                        x[m] = (nl_item)y;
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
nl_item_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    nl_item *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (nl_item *)tbl->tbl_vec) != NULL) &&
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
nl_item_alloctable(void *vec, size_t card)
{
    return (luab_table_create(&luab_nl_item_type, vec, card));
}

luab_module_t luab_nl_item_type = {
    .m_id           = LUAB_NL_ITEM_TYPE_ID,
    .m_name         = LUAB_NL_ITEM_TYPE,
    .m_vec          = nl_item_methods,
    .m_create       = nl_item_create,
    .m_init         = nl_item_init,
    .m_get          = nl_item_udata,
    .m_get_tbl      = nl_item_checktable,
    .m_set_tbl      = nl_item_pushtable,
    .m_alloc_tbl    = nl_item_alloctable,
    .m_len          = sizeof(luab_nl_item),
    .m_sz           = sizeof(nl_item),
};