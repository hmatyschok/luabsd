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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INT8ERRUPTION)
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

extern luab_module_t luab_int8_type;

/*
 * Interface against
 *
 *  int8_t
 *
 */

typedef struct luab_int8 {
    luab_udata_t    ud_softc;
    int8_t          ud_value;
} luab_int8_t;

#define luab_new_int8(L, arg) \
    ((luab_int8_t *)luab_newudata(L, &luab_int8_type, (arg)))
#define luab_to_int8(L, narg) \
    (luab_todata((L), (narg), &luab_int8_type, luab_int8_t *))

#define LUAB_INT8_TYPE_ID    1606001349
#define LUAB_INT8_TYPE   "INT8*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(INT8)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t = int8:get()
 */
static int
INT8_get(lua_State *L)
{
    luab_int8_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_int8(L, 1);

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
 * @usage iovec [, err, msg ] = int8:dump()
 */
static int
INT8_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set int8.
 *
 * @function set_value
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = int8:set_value(data)
 */
static int
INT8_set_value(lua_State *L)
{
    luab_int8_t *self;
    int8_t x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_int8(L, 1);
    x = (int8_t)luab_checkinteger(L, 2, luab_env_uchar_max);

    self->ud_value = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get int8.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = int8:get_value()
 */
static int
INT8_get_value(lua_State *L)
{
    luab_int8_t *self;
    int8_t x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_int8(L, 1);
    x = self->ud_value;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
INT8_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_int8_type));
}

static int
INT8_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_int8_type));
}

static int
INT8_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_int8_type));
}

/*
 * Internal int8erface.
 */

static luab_module_table_t int8_methods[] = {
    LUAB_FUNC("set_value",      INT8_set_value),
    LUAB_FUNC("get",            INT8_get),
    LUAB_FUNC("get_value",      INT8_get_value),
    LUAB_FUNC("dump",           INT8_dump),
    LUAB_FUNC("__gc",           INT8_gc),
    LUAB_FUNC("__len",          INT8_len),
    LUAB_FUNC("__tostring",     INT8_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
int8_create(lua_State *L, void *arg)
{
    return (luab_new_int8(L, arg));
}

static void
int8_init(void *ud, void *arg)
{
    luab_udata_init(&luab_int8_type, ud, arg);
}

static void *
int8_udata(lua_State *L, int narg)
{
    luab_int8_t *self;
    self = luab_to_int8(L, narg);
    return ((void *)&(self->ud_value));
}

static luab_table_t *
int8_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    int8_t *x, y;
    size_t m, n;

    if ((tbl = luab_newvectornil(L, narg, sizeof(int8_t))) != NULL) {

        if (((x = (int8_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (int8_t)luab_tointeger(L, -1, luab_env_uchar_max);
                        x[m] = (int8_t)y;
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
int8_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    int8_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (int8_t *)tbl->tbl_vec) != NULL) &&
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

luab_module_t luab_int8_type = {
    .m_cookie   = LUAB_INT8_TYPE_ID,
    .m_name     = LUAB_INT8_TYPE,
    .m_vec      = int8_methods,
    .m_create   = int8_create,
    .m_init     = int8_init,
    .m_get      = int8_udata,
    .m_get_tbl  = int8_checktable,
    .m_set_tbl  = int8_pushtable,
    .m_sz       = sizeof(luab_int8_t),
};