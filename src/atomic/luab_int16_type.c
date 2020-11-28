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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INT16ERRUPTION)
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

extern luab_module_t luab_int16_type;

/*
 * Interface against
 *
 *  int16_t
 *
 */

typedef struct luab_int16 {
    luab_udata_t    ud_softc;
    int16_t          ud_value;
} luab_int16_t;

#define luab_new_int16(L, arg) \
    ((luab_int16_t *)luab_newudata(L, &luab_int16_type, (arg)))
#define luab_to_int16(L, narg) \
    (luab_todata((L), (narg), &luab_int16_type, luab_int16_t *))

/*
 * Subr.
 */

static void
int16_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_int16_t *self;

    if ((self = (luab_int16_t *)arg) != NULL) {

        luab_setinteger(L, narg, "value", self->ud_value);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(INT16)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = int16:get_table()
 */
static int
INT16_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = int16_fillxtable;
    xtp.xtp_arg = (void *)luab_to_int16(L, 1);
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
 * @usage iovec [, err, msg ] = int16:dump()
 */
static int
INT16_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set int16.
 *
 * @function set_value
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = int16:set_value(data)
 */
static int
INT16_set_value(lua_State *L)
{
    luab_int16_t *self;
    int16_t x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_int16(L, 1);
    x = (int16_t)luab_checkinteger(L, 2, luab_env_ushrt_max);

    self->ud_value = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get int16.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = int16:get_value()
 */
static int
INT16_get_value(lua_State *L)
{
    luab_int16_t *self;
    int16_t x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_int16(L, 1);
    x = self->ud_value;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
INT16_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_int16_type));
}

static int
INT16_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_int16_type));
}

static int
INT16_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_int16_type));
}

/*
 * Internal int16erface.
 */

static luab_module_table_t int16_methods[] = {
    LUAB_FUNC("set_value",      INT16_set_value),
    LUAB_FUNC("get_table",      INT16_get_table),
    LUAB_FUNC("get_value",      INT16_get_value),
    LUAB_FUNC("dump",           INT16_dump),
    LUAB_FUNC("__gc",           INT16_gc),
    LUAB_FUNC("__len",          INT16_len),
    LUAB_FUNC("__tostring",     INT16_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
int16_create(lua_State *L, void *arg)
{
    return (luab_new_int16(L, arg));
}

static void
int16_init(void *ud, void *arg)
{
    luab_udata_init(&luab_int16_type, ud, arg);
}

static void *
int16_udata(lua_State *L, int narg)
{
    luab_int16_t *self;
    self = luab_to_int16(L, narg);
    return ((void *)&(self->ud_value));
}

static luab_table_t *
int16_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    int16_t *x, y;
    size_t m, n;

    if ((tbl = luab_newvectornil(L, narg, sizeof(int16_t))) != NULL) {

        if (((x = (int16_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (int16_t)luab_tointeger(L, -1, luab_env_ushrt_max);
                        x[m] = (int16_t)y;
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
int16_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    int16_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (int16_t *)tbl->tbl_vec) != NULL) &&
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

luab_module_t luab_int16_type = {
    .m_cookie   = LUAB_INT16_TYPE_ID,
    .m_name     = LUAB_INT16_TYPE,
    .m_vec      = int16_methods,
    .m_create   = int16_create,
    .m_init     = int16_init,
    .m_get      = int16_udata,
    .m_get_tbl  = int16_checktable,
    .m_set_tbl  = int16_pushtable,
    .m_sz       = sizeof(luab_int16_t),
};
