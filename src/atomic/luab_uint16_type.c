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

extern luab_module_t luab_uint16_type;

/*
 * Interface against
 *
 *  uint16_t
 *
 */

typedef struct luab_uint16 {
    luab_udata_t    ud_softc;
    uint16_t          ud_sdu;
} luab_uint16_t;

#define luab_new_uint16(L, arg) \
    ((luab_uint16_t *)luab_newudata(L, &luab_uint16_type, (arg)))
#define luab_to_uint16(L, narg) \
    (luab_todata((L), (narg), &luab_uint16_type, luab_uint16_t *))

/*
 * Subr.
 */

static void
uint16_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_uint16_t *self;

    if ((self = (luab_uint16_t *)arg) != NULL) {

        luab_setinteger(L, narg, "value", self->ud_sdu);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(UINT16)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = uint16:get_table()
 */
static int
UINT16_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = uint16_fillxtable;
    xtp.xtp_arg = (void *)luab_to_uint16(L, 1);
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
 * @usage iovec [, err, msg ] = uint16:dump()
 */
static int
UINT16_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set uint16.
 *
 * @function set_value
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = uint16:set_value(data)
 */
static int
UINT16_set_value(lua_State *L)
{
    luab_uint16_t *self;
    uint16_t x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_uint16(L, 1);
    x = (uint16_t)luab_checkinteger(L, 2, luab_env_ushrt_max);

    self->ud_sdu = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get uint16.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = uint16:get_value()
 */
static int
UINT16_get_value(lua_State *L)
{
    luab_uint16_t *self;
    uint16_t x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_uint16(L, 1);
    x = self->ud_sdu;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
UINT16_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_uint16_type));
}

static int
UINT16_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_uint16_type));
}

static int
UINT16_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_uint16_type));
}

/*
 * Internal uint16erface.
 */

static luab_module_table_t uint16_methods[] = {
    LUAB_FUNC("set_value",      UINT16_set_value),
    LUAB_FUNC("get_table",      UINT16_get_table),
    LUAB_FUNC("get_value",      UINT16_get_value),
    LUAB_FUNC("dump",           UINT16_dump),
    LUAB_FUNC("__gc",           UINT16_gc),
    LUAB_FUNC("__len",          UINT16_len),
    LUAB_FUNC("__tostring",     UINT16_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
uint16_create(lua_State *L, void *arg)
{
    return (luab_new_uint16(L, arg));
}

static void
uint16_init(void *ud, void *arg)
{
    luab_udata_init(&luab_uint16_type, ud, arg);
}

static void *
uint16_udata(lua_State *L, int narg)
{
    luab_uint16_t *self;
    self = luab_to_uint16(L, narg);
    return ((void *)&(self->ud_sdu));
}

static luab_table_t *
uint16_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    uint16_t *x, y;
    size_t m, n;

    if ((tbl = luab_table_newvectornil(L, narg, &luab_uint16_type)) != NULL) {

        if (((x = (uint16_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (m = 0, n = tbl->tbl_card; m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (uint16_t)luab_tointeger(L, -1, luab_env_ushrt_max);
                        x[m] = (uint16_t)y;
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
uint16_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    uint16_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (uint16_t *)tbl->tbl_vec) != NULL) &&
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
uint16_alloctable(void *vec, size_t card)
{
    return (luab_table_create(&luab_uint16_type, vec, card));
}

luab_module_t luab_uint16_type = {
    .m_id           = LUAB_UINT16_TYPE_ID,
    .m_name         = LUAB_UINT16_TYPE,
    .m_vec          = uint16_methods,
    .m_create       = uint16_create,
    .m_init         = uint16_init,
    .m_get          = uint16_udata,
    .m_get_tbl      = uint16_checktable,
    .m_set_tbl      = uint16_pushtable,
    .m_alloc_tbl    = uint16_alloctable,
    .m_len          = sizeof(luab_uint16_t),
    .m_sz           = sizeof(uint16_t),
};