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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS USHRTERRUPTION)
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

extern luab_module_t luab_ushrt_type;

/*
 * Interface against
 *
 *  ushrt
 *
 */

typedef struct luab_ushrt {
    luab_udata_t    ud_softc;
    u_short         ud_x;
} luab_ushrt_t;

#define luab_new_ushrt(L, arg) \
    ((luab_ushrt_t *)luab_newudata(L, &luab_ushrt_type, (arg)))
#define luab_to_ushrt(L, narg) \
    (luab_todata((L), (narg), &luab_ushrt_type, luab_ushrt_t *))

#define LUAB_USHRT_TYPE_ID    1605911568
#define LUAB_USHRT_TYPE   "USHRT*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(USHRT)) ushrto (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              x   = (LUA_TNUMBER),
 *          }
 *
 * @usage t = ushrt:get()
 */
static int
USHRT_get(lua_State *L)
{
    luab_ushrt_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_ushrt(L, 1);

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
 * @usage iovec [, err, msg ] = ushrt:dump()
 */
static int
USHRT_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set ushrt.
 *
 * @function set_x
 *
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = ushrt:set_x(data)
 */
static int
USHRT_set_x(lua_State *L)
{
    luab_ushrt_t *self;
    u_short x;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_ushrt(L, 1);
    x = (u_short)luaL_checknumber(L, 2);

    self->ud_x = x;

    return (luab_pushxnumber(L, x));
}

/***
 * Get ushrt.
 *
 * @function get_x
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = ushrt:get_x()
 */
static int
USHRT_get_x(lua_State *L)
{
    luab_ushrt_t *self;
    u_short x;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_ushrt(L, 1);
    x = self->ud_x;

    return (luab_pushxnumber(L, x));
}

/*
 * Metamethods.
 */

static int
USHRT_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_ushrt_type));
}

static int
USHRT_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_ushrt_type));
}

static int
USHRT_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_ushrt_type));
}

/*
 * Internal ushrterface.
 */

static luab_module_table_t ushrt_methods[] = {
    LUAB_FUNC("set_x",          USHRT_set_x),
    LUAB_FUNC("get",            USHRT_get),
    LUAB_FUNC("get_x",          USHRT_get_x),
    LUAB_FUNC("dump",           USHRT_dump),
    LUAB_FUNC("__gc",           USHRT_gc),
    LUAB_FUNC("__len",          USHRT_len),
    LUAB_FUNC("__tostring",     USHRT_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
ushrt_create(lua_State *L, void *arg)
{
    return (luab_new_ushrt(L, arg));
}

static void
ushrt_init(void *ud, void *arg)
{
    luab_udata_init(&luab_ushrt_type, ud, arg);
}

static void *
ushrt_udata(lua_State *L, int narg)
{
    luab_ushrt_t *self;
    self = luab_to_ushrt(L, narg);
    return ((void *)&(self->ud_x));
}

static luab_table_t *
ushrt_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    u_short *x, y;
    size_t m, n;

    if ((tbl = luab_newvectornil(L, narg, sizeof(u_short))) != NULL) {

        if (((x = (u_short *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (u_short)lua_tonumber(L, -1);
                        x[m] = (u_short)y;
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
ushrt_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    u_short *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (u_short *)tbl->tbl_vec) != NULL) &&
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

luab_module_t luab_ushrt_type = {
    .m_cookie   = LUAB_USHRT_TYPE_ID,
    .m_name     = LUAB_USHRT_TYPE,
    .m_vec      = ushrt_methods,
    .m_create   = ushrt_create,
    .m_init     = ushrt_init,
    .m_get      = ushrt_udata,
    .m_get_tbl  = ushrt_checktable,
    .m_set_tbl  = ushrt_pushtable,
    .m_sz       = sizeof(luab_ushrt_t),
};