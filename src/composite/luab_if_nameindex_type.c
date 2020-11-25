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

#include <net/if.h>

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
    luab_udata_t    ud_softc;
    u_int   if_index;
    char    if_name[IFNAMSIZ];
} luab_if_nameindex_t;

#define luab_new_if_nameindex(L, arg) \
    ((luab_if_nameindex_t *)luab_newudata(L, &luab_if_nameindex_type, (arg)))
#define luab_to_if_nameindex(L, narg) \
    ((struct if_nameindex *)luab_toudata((L), (narg), &luab_if_nameindex_type))

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(IF_NAMEINDEX)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              if_index    = (LUA_TNUMBER),
 *              if_name     = (LUA_TSTRING),
 *          }
 *
 * @usage t = if_nameindex:get()
 */
static int
IF_NAMEINDEX_get(lua_State *L)
{
    struct if_nameindex *ifni;

    (void)luab_core_checkmaxargs(L, 1);

    ifni = luab_udata(L, 1, &luab_if_nameindex_type, struct if_nameindex *);

    lua_newtable(L);
    luab_setinteger(L, -2, "if_index",  ifni->if_index);
    luab_setstring(L, -2, "if_name",    ifni->if_name);
    lua_pushvalue(L, -1);

    return (1);
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
    struct if_nameindex *ifni;
    u_int if_index;

    (void)luab_core_checkmaxargs(L, 1);

    ifni = luab_udata(L, 1, &luab_if_nameindex_type, struct if_nameindex *);
    if_index = ifni->if_index;

    return (luab_pushxinteger(L, if_index));
}

/***
 * Get value for if_name.
 *
 * @function if_name
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage name [, err, msg ]= if_nameindex:if_name()
 */
static int
IF_NAMEINDEX_if_name(lua_State *L)
{
    struct if_nameindex *ifni;
    char *if_name;

    (void)luab_core_checkmaxargs(L, 1);

    ifni = luab_udata(L, 1, &luab_if_nameindex_type, struct if_nameindex *);
    if_name = ifni->if_name;

    return (luab_pushstring(L, ifni->if_name));
}

/*
 * Meta-medthods.
 */

static int
IF_NAMEINDEX_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_if_nameindex_type));
}

static int
IF_NAMEINDEX_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_if_nameindex_type));
}

static int
IF_NAMEINDEX_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_if_nameindex_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t if_nameindex_methods[] = {
    LUAB_FUNC("if_index",       IF_NAMEINDEX_if_index),
    LUAB_FUNC("if_name",        IF_NAMEINDEX_if_name),
    LUAB_FUNC("get",            IF_NAMEINDEX_get),
    LUAB_FUNC("dump",           IF_NAMEINDEX_dump),
    LUAB_FUNC("__gc",           IF_NAMEINDEX_gc),
    LUAB_FUNC("__len",          IF_NAMEINDEX_len),
    LUAB_FUNC("__tostring",     IF_NAMEINDEX_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
if_nameindex_create(lua_State *L, void *arg)
{
    return (luab_new_if_nameindex(L, arg));
}

static void
if_nameindex_init(void *ud, void *arg)
{
    luab_if_nameindex_t *self;
    struct if_nameindex *ifni;
    size_t len;

    if (((self = (luab_if_nameindex_t *)ud) != NULL) &&
        ((ifni = (struct if_nameindex *)arg) != NULL)) {
        self->if_index = ifni->if_index;
        len = strnlen(ifni->if_name, IFNAMSIZ);
        (void)memmove(self->if_name, ifni->if_name, len);
    }
}

static void *
if_nameindex_udata(lua_State *L, int narg)
{
    return (luab_to_if_nameindex(L, narg));
}

static luab_table_t *
if_nameindex_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct if_nameindex *x, *y;
    size_t m, n, sz;

    sz = sizeof(struct if_nameindex);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (struct if_nameindex *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_if_nameindex_type, struct if_nameindex *);
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
if_nameindex_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct if_nameindex *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (struct if_nameindex *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_if_nameindex_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

luab_module_t luab_if_nameindex_type = {
    .m_cookie   = LUAB_IF_NAMEINDEX_TYPE_ID,
    .m_name     = LUAB_IF_NAMEINDEX_TYPE,
    .m_vec      = if_nameindex_methods,
    .m_create   = if_nameindex_create,
    .m_init     = if_nameindex_init,
    .m_get      = if_nameindex_udata,
    .m_get_tbl  = if_nameindex_checktable,
    .m_set_tbl  = if_nameindex_pushtable,
    .m_sz       = sizeof(luab_if_nameindex_t),
};
