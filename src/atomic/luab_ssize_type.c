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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS SSIZEERRUPTION)
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

extern luab_module_t luab_ssize_type;

/*
 * Interface against
 *
 *  ssize_t
 *
 */

typedef struct luab_ssize {
    luab_udata_t    ud_softc;
    ssize_t           ud_x;
} luab_ssize_t;

#define luab_new_ssize(L, arg) \
    ((luab_ssize_t *)luab_newudata(L, &luab_ssize_type, (arg)))
#define luab_to_ssize(L, narg) \
    (luab_todata((L), (narg), &luab_ssize_type, luab_ssize_t *))

#define LUAB_SSIZE_TYPE_ID    1605991182
#define LUAB_SSIZE_TYPE   "SSIZE*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SSIZE)) ssizeo (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              x   = (LUA_TNUMBER),
 *          }
 *
 * @usage t = ssize:get()
 */
static int
SSIZE_get(lua_State *L)
{
    luab_ssize_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_ssize(L, 1);

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
 * @usage iovec [, err, msg ] = ssize:dump()
 */
static int
SSIZE_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set ssize.
 *
 * @function set_x
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = ssize:set_x(data)
 */
static int
SSIZE_set_x(lua_State *L)
{
    luab_ssize_t *self;
    ssize_t x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_ssize(L, 1);
    x = (ssize_t)luaL_checknumber(L, 2);

    self->ud_x = x;

    return (luab_pushxnumber(L, x));
}

/***
 * Get ssize.
 *
 * @function get_x
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = ssize:get_x()
 */
static int
SSIZE_get_x(lua_State *L)
{
    luab_ssize_t *self;
    ssize_t x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_ssize(L, 1);
    x = self->ud_x;

    return (luab_pushxnumber(L, x));
}

/*
 * Metamethods.
 */

static int
SSIZE_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_ssize_type));
}

static int
SSIZE_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_ssize_type));
}

static int
SSIZE_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_ssize_type));
}

/*
 * Internal ssizeerface.
 */

static luab_module_table_t ssize_methods[] = {
    LUAB_FUNC("set_x",          SSIZE_set_x),
    LUAB_FUNC("get",            SSIZE_get),
    LUAB_FUNC("get_x",          SSIZE_get_x),
    LUAB_FUNC("dump",           SSIZE_dump),
    LUAB_FUNC("__gc",           SSIZE_gc),
    LUAB_FUNC("__len",          SSIZE_len),
    LUAB_FUNC("__tostring",     SSIZE_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
ssize_create(lua_State *L, void *arg)
{
    return (luab_new_ssize(L, arg));
}

static void
ssize_init(void *ud, void *arg)
{
    luab_udata_init(&luab_ssize_type, ud, arg);
}

static void *
ssize_udata(lua_State *L, int narg)
{
    luab_ssize_t *self;
    self = luab_to_ssize(L, narg);
    return ((void *)&(self->ud_x));
}

static luab_table_t *
ssize_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    ssize_t *x, y;
    size_t m, n;

    if ((tbl = luab_newvectornil(L, narg, sizeof(ssize_t))) != NULL) {

        if (((x = (ssize_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (ssize_t)luab_checklinteger(L, -1, 1);
                        x[m] = (ssize_t)y;
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
ssize_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    ssize_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (ssize_t *)tbl->tbl_vec) != NULL) &&
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

luab_module_t luab_ssize_type = {
    .m_cookie   = LUAB_SSIZE_TYPE_ID,
    .m_name     = LUAB_SSIZE_TYPE,
    .m_vec      = ssize_methods,
    .m_create   = ssize_create,
    .m_init     = ssize_init,
    .m_get      = ssize_udata,
    .m_get_tbl  = ssize_checktable,
    .m_set_tbl  = ssize_pushtable,
    .m_sz       = sizeof(luab_ssize_t),
};
