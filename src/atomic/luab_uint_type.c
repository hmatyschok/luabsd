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

#if __BSD_VISIBLE
extern luab_module_t luab_uint_type;

/*
 * Interface against
 *
 *  u_int
 *
 */

typedef struct luab_uint {
    luab_udata_t    ud_softc;
    u_int           ud_sdu;
} luab_uint_t;

#define luab_new_uint(L, arg) \
    ((luab_uint_t *)luab_newudata(L, &luab_uint_type, (arg)))
#define luab_to_uint(L, narg) \
    (luab_todata((L), (narg), &luab_uint_type, luab_uint_t *))

/*
 * Subr.
 */

static void
uint_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_uint_t *self;

    if ((self = (luab_uint_t *)arg) != NULL) {

        luab_setinteger(L, narg, "value", self->ud_sdu);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(UINT)) to (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = uint:get_table()
 */
static int
UINT_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = uint_fillxtable;
    xtp.xtp_arg = (void *)luab_to_uint(L, 1);
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
 * @usage iovec [, err, msg ] = uint:dump()
 */
static int
UINT_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set uint.
 *
 * @function set_value
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = uint:set_value(data)
 */
static int
UINT_set_value(lua_State *L)
{
    luab_uint_t *self;
    u_int x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_uint(L, 1);
    x = (u_int)luab_checkinteger(L, 2, luab_env_uint_max);

    self->ud_sdu = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get uint.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = uint:get_value()
 */
static int
UINT_get_value(lua_State *L)
{
    luab_uint_t *self;
    u_int x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_uint(L, 1);
    x = self->ud_sdu;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
UINT_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_uint_type));
}

static int
UINT_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_uint_type));
}

static int
UINT_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_uint_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t uint_methods[] = {
    LUAB_FUNC("set_value",      UINT_set_value),
    LUAB_FUNC("get_table",      UINT_get_table),
    LUAB_FUNC("get_value",      UINT_get_value),
    LUAB_FUNC("dump",           UINT_dump),
    LUAB_FUNC("__gc",           UINT_gc),
    LUAB_FUNC("__len",          UINT_len),
    LUAB_FUNC("__tostring",     UINT_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
uint_create(lua_State *L, void *arg)
{
    return (luab_new_uint(L, arg));
}

static void
uint_init(void *ud, void *arg)
{
    luab_udata_init(&luab_uint_type, ud, arg);
}

static void *
uint_udata(lua_State *L, int narg)
{
    luab_uint_t *self;
    self = luab_to_uint(L, narg);
    return ((void *)&(self->ud_sdu));
}

static luab_table_t *
uint_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    u_int *x, y;
    size_t m, n;

    if ((tbl = luab_table_newvectornil(L, narg, &luab_uint_type)) != NULL) {

        if (((x = (u_int *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (m = 0, n = tbl->tbl_card; m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (u_int)luab_tointeger(L, -1, luab_env_uint_max);
                        x[m] = (u_int)y;
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
uint_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    u_int *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (u_int *)tbl->tbl_vec) != NULL) &&
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
uint_alloctable(void *vec, size_t card)
{
    return (luab_table_create(&luab_uint_type, vec, card));
}

luab_module_t luab_uint_type = {
    .m_id           = LUAB_UINT_TYPE_ID,
    .m_name         = LUAB_UINT_TYPE,
    .m_vec          = uint_methods,
    .m_create       = uint_create,
    .m_init         = uint_init,
    .m_get          = uint_udata,
    .m_get_tbl      = uint_checktable,
    .m_set_tbl      = uint_pushtable,
    .m_alloc_tbl    = uint_alloctable,
    .m_len          = sizeof(luab_uint_t),
    .m_sz           = sizeof(u_int),
};
#endif /* __BSD_VISIBLE */
