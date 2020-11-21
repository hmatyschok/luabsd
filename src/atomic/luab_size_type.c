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
    size_t           ud_x;
} luab_size_t;

#define luab_new_size(L, arg) \
    ((luab_size_t *)luab_newudata(L, &luab_size_type, (arg)))
#define luab_to_size(L, narg) \
    (luab_todata((L), (narg), &luab_size_type, luab_size_t *))

#define LUAB_SIZE_TYPE_ID    1605967268
#define LUAB_SIZE_TYPE   "SIZE*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SIZE)) sizeo (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              x   = (LUA_TNUMBER),
 *          }
 *
 * @usage t = size:get()
 */
static int
SIZE_get(lua_State *L)
{
    luab_size_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_size(L, 1);

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
 * @function set_x
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = size:set_x(data)
 */
static int
SIZE_set_x(lua_State *L)
{
    luab_size_t *self;
    size_t x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_size(L, 1);
    x = (size_t)luaL_checknumber(L, 2);

    self->ud_x = x;

    return (luab_pushxnumber(L, x));
}

/***
 * Get size.
 *
 * @function get_x
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = size:get_x()
 */
static int
SIZE_get_x(lua_State *L)
{
    luab_size_t *self;
    size_t x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_size(L, 1);
    x = self->ud_x;

    return (luab_pushxnumber(L, x));
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
    LUAB_FUNC("set_x",          SIZE_set_x),
    LUAB_FUNC("get",            SIZE_get),
    LUAB_FUNC("get_x",          SIZE_get_x),
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
    return ((void *)&(self->ud_x));
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
                        y = (size_t)lua_tonumber(L, -1);
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
                luab_rawsetnumber(L, narg, k, x[m]);

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
