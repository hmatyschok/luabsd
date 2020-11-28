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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS SIZEERRUPTION)
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

extern luab_module_t luab_size_type;

/*
 * Interface against
 *
 *  size_t
 *
 */

typedef struct luab_size {
    luab_udata_t    ud_softc;
    size_t          ud_value;
} luab_size_t;

#define luab_new_size(L, arg) \
    ((luab_size_t *)luab_newudata(L, &luab_size_type, (arg)))
#define luab_to_size(L, narg) \
    (luab_todata((L), (narg), &luab_size_type, luab_size_t *))

/*
 * Subr.
 */

static void
size_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_size_t *self;

    if ((self = (luab_size_t *)arg) != NULL) {

        luab_setinteger(L, narg, "value", self->ud_value);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SIZE)) sizeo (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = size:get_table()
 */
static int
SIZE_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = size_fillxtable;
    xtp.xtp_arg = (void *)luab_to_size(L, 1);
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
 * @usage iovec [, err, msg ] = size:dump()
 */
static int
SIZE_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set size.
 *
 * @function set_value
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = size:set_value(data)
 */
static int
SIZE_set_value(lua_State *L)
{
    luab_size_t *self;
    size_t x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_size(L, 1);
    x = (size_t)luab_checklinteger(L, 2, 0);

    self->ud_value = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get size.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = size:get_value()
 */
static int
SIZE_get_value(lua_State *L)
{
    luab_size_t *self;
    size_t x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_size(L, 1);
    x = self->ud_value;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
SIZE_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_size_type));
}

static int
SIZE_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_size_type));
}

static int
SIZE_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_size_type));
}

/*
 * Internal sizeerface.
 */

static luab_module_table_t size_methods[] = {
    LUAB_FUNC("set_value",      SIZE_set_value),
    LUAB_FUNC("get_table",      SIZE_get_table),
    LUAB_FUNC("get_value",      SIZE_get_value),
    LUAB_FUNC("dump",           SIZE_dump),
    LUAB_FUNC("__gc",           SIZE_gc),
    LUAB_FUNC("__len",          SIZE_len),
    LUAB_FUNC("__tostring",     SIZE_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
size_create(lua_State *L, void *arg)
{
    return (luab_new_size(L, arg));
}

static void
size_init(void *ud, void *arg)
{
    luab_udata_init(&luab_size_type, ud, arg);
}

static void *
size_udata(lua_State *L, int narg)
{
    luab_size_t *self;
    self = luab_to_size(L, narg);
    return ((void *)&(self->ud_value));
}

static luab_table_t *
size_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    size_t *x, y;
    size_t m, n;

    if ((tbl = luab_newvectornil(L, narg, sizeof(size_t))) != NULL) {

        if (((x = (size_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (size_t)luab_tolinteger(L, -1, 0);
                        x[m] = (size_t)y;
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
size_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    size_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (size_t *)tbl->tbl_vec) != NULL) &&
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

luab_module_t luab_size_type = {
    .m_cookie   = LUAB_SIZE_TYPE_ID,
    .m_name     = LUAB_SIZE_TYPE,
    .m_vec      = size_methods,
    .m_create   = size_create,
    .m_init     = size_init,
    .m_get      = size_udata,
    .m_get_tbl  = size_checktable,
    .m_set_tbl  = size_pushtable,
    .m_sz       = sizeof(luab_size_t),
};
