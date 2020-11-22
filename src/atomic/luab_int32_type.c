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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INT32ERRUPTION)
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

extern luab_module_t luab_int32_type;

/*
 * Interface against
 *
 *  int32_t
 *
 */

typedef struct luab_int32 {
    luab_udata_t    ud_softc;
    int32_t          ud_value;
} luab_int32_t;

#define luab_new_int32(L, arg) \
    ((luab_int32_t *)luab_newudata(L, &luab_int32_type, (arg)))
#define luab_to_int32(L, narg) \
    (luab_todata((L), (narg), &luab_int32_type, luab_int32_t *))

#define LUAB_INT32_TYPE_ID    1606004102
#define LUAB_INT32_TYPE   "INT32*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(INT32)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t = int32:get()
 */
static int
INT32_get(lua_State *L)
{
    luab_int32_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_int32(L, 1);

    lua_newtable(L);
    luab_setinteger(L, -2, "value", self->ud_value);
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
 * @usage iovec [, err, msg ] = int32:dump()
 */
static int
INT32_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set int32.
 *
 * @function set_value
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = int32:set_value(data)
 */
static int
INT32_set_value(lua_State *L)
{
    luab_int32_t *self;
    int32_t x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_int32(L, 1);
    x = (int32_t)luab_checkinteger(L, 2, luab_env_uint_max);

    self->ud_value = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get int32.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = int32:get_value()
 */
static int
INT32_get_value(lua_State *L)
{
    luab_int32_t *self;
    int32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_int32(L, 1);
    x = self->ud_value;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
INT32_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_int32_type));
}

static int
INT32_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_int32_type));
}

static int
INT32_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_int32_type));
}

/*
 * Internal int32erface.
 */

static luab_module_table_t int32_methods[] = {
    LUAB_FUNC("set_value",      INT32_set_value),
    LUAB_FUNC("get",            INT32_get),
    LUAB_FUNC("get_value",      INT32_get_value),
    LUAB_FUNC("dump",           INT32_dump),
    LUAB_FUNC("__gc",           INT32_gc),
    LUAB_FUNC("__len",          INT32_len),
    LUAB_FUNC("__tostring",     INT32_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
int32_create(lua_State *L, void *arg)
{
    return (luab_new_int32(L, arg));
}

static void
int32_init(void *ud, void *arg)
{
    luab_udata_init(&luab_int32_type, ud, arg);
}

static void *
int32_udata(lua_State *L, int narg)
{
    luab_int32_t *self;
    self = luab_to_int32(L, narg);
    return ((void *)&(self->ud_value));
}

static luab_table_t *
int32_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    int32_t *x, y;
    size_t m, n;

    if ((tbl = luab_newvectornil(L, narg, sizeof(int32_t))) != NULL) {

        if (((x = (int32_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (int32_t)luab_tointeger(L, -1, luab_env_uint_max);
                        x[m] = (int32_t)y;
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
int32_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    int32_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (int32_t *)tbl->tbl_vec) != NULL) &&
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

luab_module_t luab_int32_type = {
    .m_cookie   = LUAB_INT32_TYPE_ID,
    .m_name     = LUAB_INT32_TYPE,
    .m_vec      = int32_methods,
    .m_create   = int32_create,
    .m_init     = int32_init,
    .m_get      = int32_udata,
    .m_get_tbl  = int32_checktable,
    .m_set_tbl  = int32_pushtable,
    .m_sz       = sizeof(luab_int32_t),
};