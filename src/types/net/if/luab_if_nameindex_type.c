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

#include <net/if.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_if_nameindex_type;

/*
 * Interface against
 *
 *  struct if_nameindex {
 *      unsigned int    if_index;
 *      char        *if_name;
 *  };
 *
 * Attributes are immutable.
 */
typedef struct luab_if_nameindex {
    luab_udata_t            ud_softc;
    struct if_nameindex     ud_ifni;
} luab_if_nameindex_t;

/*
 * Subr.
 */

static void
if_nameindex_fillxtable(lua_State *L, int narg, void *arg)
{
    struct if_nameindex *ifni;

    if ((ifni = (struct if_nameindex *)arg) != NULL) {

        luab_setinteger(L, narg, "if_index",  ifni->if_index);
        luab_setstring(L, narg, "if_name",    ifni->if_name);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(IF_NAMEINDEX)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              if_index    = (LUA_TNUMBER),
 *              if_name     = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ] = if_nameindex:get_table()
 */
static int
IF_NAMEINDEX_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IF_NAMEINDEX, TYPE, __func__);

    xtp.xtp_fill = if_nameindex_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
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
 * @usage iovec [, err, msg ] = cmsgcred:dump()
 */
static int
IF_NAMEINDEX_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get value for if_index.
 *
 * @function if_index
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage index [, err, msg ] = if_nameindex:if_index()
 */
static int
IF_NAMEINDEX_if_index(lua_State *L)
{
    luab_module_t *m;
    struct if_nameindex *ifni;
    u_int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IF_NAMEINDEX, TYPE, __func__);
    ifni = luab_udata(L, 1, m, struct if_nameindex *);
    x = ifni->if_index;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for if_name.
 *
 * @function if_name
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage name [, err, msg ] = if_nameindex:if_name()
 */
static int
IF_NAMEINDEX_if_name(lua_State *L)
{
    luab_module_t *m;
    struct if_nameindex *ifni;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IF_NAMEINDEX, TYPE, __func__);
    ifni = luab_udata(L, 1, m, struct if_nameindex *);
    dp = ifni->if_name;

    return (luab_pushstring(L, dp));
}

/*
 * Meta-medthods.
 */

static int
IF_NAMEINDEX_gc(lua_State *L)
{
    luab_module_t *m;
    struct if_nameindex *ifni;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IF_NAMEINDEX, TYPE, __func__);
    ifni = luab_udata(L, 1, m, struct if_nameindex *);

    if (ifni->if_name != NULL)
        luab_core_freestr(ifni->if_name);

    return (luab_core_gc(L, 1, m));
}

static int
IF_NAMEINDEX_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IF_NAMEINDEX, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
IF_NAMEINDEX_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IF_NAMEINDEX, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t if_nameindex_methods[] = {
    LUAB_FUNC("if_index",       IF_NAMEINDEX_if_index),
    LUAB_FUNC("if_name",        IF_NAMEINDEX_if_name),
    LUAB_FUNC("get_table",      IF_NAMEINDEX_get_table),
    LUAB_FUNC("dump",           IF_NAMEINDEX_dump),
    LUAB_FUNC("__gc",           IF_NAMEINDEX_gc),
    LUAB_FUNC("__len",          IF_NAMEINDEX_len),
    LUAB_FUNC("__tostring",     IF_NAMEINDEX_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
if_nameindex_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(IF_NAMEINDEX, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
if_nameindex_init(void *ud, void *arg)
{
    luab_if_nameindex_t *self;
    struct if_nameindex *ifni;
    size_t len;
    caddr_t dp;

    if (((self = (luab_if_nameindex_t *)ud) != NULL) &&
        ((ifni = (struct if_nameindex *)arg) != NULL)) {
        self->ud_ifni.if_index = ifni->if_index;

        if ((len = strnlen(ifni->if_name, luab_env_ifname_max)) > 0) {

            dp = luab_core_alloc(luab_env_ifname_max, sizeof(char));

            if ((self->ud_ifni.if_name = dp) != NULL)
                (void)memmove(self->ud_ifni.if_name, ifni->if_name, len);
        }
    }
}

static void *
if_nameindex_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_if_nameindex_t *self;

    m = luab_xmod(IF_NAMEINDEX, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_if_nameindex_t *);
    return (&self->ud_ifni);
}

static luab_table_t *
if_nameindex_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct if_nameindex *x, *y;
    size_t i, j;

    m = luab_xmod(IF_NAMEINDEX, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct if_nameindex *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct if_nameindex *);
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
if_nameindex_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct if_nameindex *x;
    size_t i, j, k;

    m = luab_xmod(IF_NAMEINDEX, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct if_nameindex *)tbl->tbl_vec) != NULL) &&
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
if_nameindex_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(IF_NAMEINDEX, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_if_nameindex_type = {
    .m_id           = LUAB_IF_NAMEINDEX_TYPE_ID,
    .m_name         = LUAB_IF_NAMEINDEX_TYPE,
    .m_vec          = if_nameindex_methods,
    .m_create       = if_nameindex_create,
    .m_init         = if_nameindex_init,
    .m_get          = if_nameindex_udata,
    .m_get_tbl      = if_nameindex_checktable,
    .m_set_tbl      = if_nameindex_pushtable,
    .m_alloc_tbl    = if_nameindex_alloctable,
    .m_len          = sizeof(luab_if_nameindex_t),
    .m_sz           = sizeof(struct if_nameindex),
};
