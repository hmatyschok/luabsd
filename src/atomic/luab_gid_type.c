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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS GIDERRUPTION)
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

extern luab_module_t luab_gid_type;

/*
 * Interface against
 *
 *  gid
 *
 */

typedef struct luab_gid {
    luab_udata_t    ud_softc;
    gid_t             ud_x;
} luab_gid_t;

#define luab_new_gid(L, arg) \
    ((luab_gid_t *)luab_newudata(L, &luab_gid_type, (arg)))
#define luab_to_gid(L, narg) \
    (luab_todata((L), (narg), &luab_gid_type, luab_gid_t *))

#define LUAB_GID_TYPE_ID    1605908910
#define LUAB_GID_TYPE   "GID*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(GID)) gido (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              x   = (LUA_TNUMBER),
 *          }
 *
 * @usage t = gid:get()
 */
static int
GID_get(lua_State *L)
{
    luab_gid_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_gid(L, 1);

    lua_newtable(L);
    luab_setnumber(L, -2, "x", self->ud_x);
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
 * @usage iovec [, err, msg ] = gid:dump()
 */
static int
GID_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set gid.
 *
 * @function set_x
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = gid:set_x(data)
 */
static int
GID_set_x(lua_State *L)
{
    luab_gid_t *self;
    gid_t x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_gid(L, 1);
    x = (gid_t)luab_checkinteger(L, 2, luab_env_int_max);

    self->ud_x = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get gid.
 *
 * @function get_x
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = gid:get_x()
 */
static int
GID_get_x(lua_State *L)
{
    luab_gid_t *self;
    gid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_gid(L, 1);
    x = self->ud_x;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
GID_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_gid_type));
}

static int
GID_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_gid_type));
}

static int
GID_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_gid_type));
}

/*
 * Internal giderface.
 */

static luab_module_table_t gid_methods[] = {
    LUAB_FUNC("set_x",          GID_set_x),
    LUAB_FUNC("get",            GID_get),
    LUAB_FUNC("get_x",          GID_get_x),
    LUAB_FUNC("dump",           GID_dump),
    LUAB_FUNC("__gc",           GID_gc),
    LUAB_FUNC("__len",          GID_len),
    LUAB_FUNC("__tostring",     GID_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
gid_create(lua_State *L, void *arg)
{
    return (luab_new_gid(L, arg));
}

static void
gid_init(void *ud, void *arg)
{
    luab_udata_init(&luab_gid_type, ud, arg);
}

static void *
gid_udata(lua_State *L, int narg)
{
    luab_gid_t *self;
    self = luab_to_gid(L, narg);
    return ((void *)&(self->ud_x));
}

static luab_table_t *
gid_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    gid_t *x, y;
    size_t m, n;

    if ((tbl = luab_newvectornil(L, narg, sizeof(gid_t))) != NULL) {

        if (((x = (gid_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (gid_t)luab_checkinteger(L, -1, luab_env_int_max);
                        x[m] = (gid_t)y;
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
gid_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    gid_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (gid_t *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetinteger(L, narg, k, x[m]);

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

luab_module_t luab_gid_type = {
    .m_cookie   = LUAB_GID_TYPE_ID,
    .m_name     = LUAB_GID_TYPE,
    .m_vec      = gid_methods,
    .m_create   = gid_create,
    .m_init     = gid_init,
    .m_get      = gid_udata,
    .m_get_tbl  = gid_checktable,
    .m_set_tbl  = gid_pushtable,
    .m_sz       = sizeof(luab_gid_t),
};
