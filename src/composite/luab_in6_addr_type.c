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

#include <netinet/in.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_in6_addr_type;

/*
 * Interface against
 *
 *  struct in6_addr {
 *      union {
 *          uint8_t     __u6_addr8[16];
 *          uint16_t    __u6_addr16[8];
 *          uint32_t    __u6_addr32[4];
 *      } __u6_addr;
 *  };
 *
 * 128-bit IP6 address,
 */

typedef struct luab_in6_addr {
    luab_udata_t    ud_softc;
    struct in6_addr ud_ia;
} luab_in6_addr_t;
#define s6_addr32   __u6_addr.__u6_addr32

#define luab_new_in6_addr(L, arg) \
    ((luab_in6_addr_t *)luab_newudata(L, &luab_in6_addr_type, (arg)))
#define luab_to_in6_addr(L, narg) \
    (luab_toldata((L), (narg), &luab_in6_addr_type, \
        struct in6_addr *, sizeof(struct in6_addr)))

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(IN6_ADDR)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              s6_addr = (LUA_TNUMBER),
 *          }
 *
 * @usage t = in6_addr:get()
 */
static int
IN6_ADDR_get(lua_State *L)
{
    struct in6_addr *ia;

    (void)luab_core_checkmaxargs(L, 1);

    ia = luab_udata(L, 1, &luab_in6_addr_type, struct in6_addr *);

    lua_newtable(L);
    luab_setldata(L, -2, "s6_addr", &ia->s6_addr, sizeof(ia->__u6_addr));
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate in6_addr{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = in6_addr:dump()
 */
static int
IN6_ADDR_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_in6_addr_type, sizeof(struct in6_addr)));
}

/*
 * Access functions.
 */

/***
 * Copyin IPv6 address.
 *
 * @function set_s6_addr
 *
 * @param data              LUA_TTABLE(uint32_t) with cardinality of #4.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = in6_addr:set_s6_addr(id)
 */
static int
IN6_ADDR_set_s6_addr(lua_State *L)
{
    struct in6_addr *ia;
    uint32_t k, v;

    (void)luab_core_checkmaxargs(L, 2);

    ia = luab_udata(L, 1, &luab_in6_addr_type, struct in6_addr *);

    (void)luab_checkltable(L, 2, 4);

    lua_pushnil(L); /* Traverse through whole table. */

    for (k = 0; lua_next(L, 2) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (uint32_t)luab_tointeger(L, -1, luab_env_uint_max);
            ia->s6_addr32[k] = v;
        } else
            luaL_argerror(L, 2, "Invalid argument");

        lua_pop(L, 1);
    }
    return (luab_pushxinteger(L, 0));
}

/***
 * Copyout IPv6 address.
 *
 * @function get_s6_addr
 *
 * @return (LUA_TTABLE) with cardinality of #4 (over uint32_t).
 *
 * @usage t = in6_addr:get_s6_addr()
 */
static int
IN6_ADDR_get_s6_addr(lua_State *L)
{
    struct in6_addr *ia;
    int i, j;

    (void)luab_core_checkmaxargs(L, 1);

    ia = luab_udata(L, 1, &luab_in6_addr_type, struct in6_addr *);

    lua_newtable(L);

    for (i = 0, j = 1; i < 4; i++, j++)
        luab_rawsetinteger(L, -2, j, ia->s6_addr32[i]);

    lua_pushvalue(L, -1);

    return (1);
}

/*
 * Metamethods
 */

static int
IN6_ADDR_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_in6_addr_type));
}

static int
IN6_ADDR_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_in6_addr_type));
}

static int
IN6_ADDR_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_in6_addr_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t in6_addr_methods[] = {
    LUAB_FUNC("set_s6_addr",  IN6_ADDR_set_s6_addr),
    LUAB_FUNC("get",          IN6_ADDR_get),
    LUAB_FUNC("get_s6_addr",  IN6_ADDR_get_s6_addr),
    LUAB_FUNC("dump",         IN6_ADDR_dump),
    LUAB_FUNC("__gc",         IN6_ADDR_gc),
    LUAB_FUNC("__len",        IN6_ADDR_len),
    LUAB_FUNC("__tostring",   IN6_ADDR_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
in6_addr_create(lua_State *L, void *arg)
{
    return (luab_new_in6_addr(L, arg));
}

static void
in6_addr_init(void *ud, void *arg)
{
    luab_udata_init(&luab_in6_addr_type, ud, arg);
}

static void *
in6_addr_udata(lua_State *L, int narg)
{
    return (luab_to_in6_addr(L, narg));
}

static luab_table_t *
in6_addr_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct in6_addr *x, *y;
    size_t m, n, sz;

    sz = sizeof(struct in6_addr);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (struct in6_addr *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_in6_addr_type, struct in6_addr *);
                        (void)memmove(&(x[m]), y, sz);
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
in6_addr_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct in6_addr *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (struct in6_addr *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_in6_addr_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

luab_module_t luab_in6_addr_type = {
    .m_cookie   = LUAB_IN6_ADDR_TYPE_ID,
    .m_name     = LUAB_IN6_ADDR_TYPE,
    .m_vec      = in6_addr_methods,
    .m_create   = in6_addr_create,
    .m_init     = in6_addr_init,
    .m_get      = in6_addr_udata,
    .m_get_tbl  = in6_addr_checktable,
    .m_set_tbl  = in6_addr_pushtable,
    .m_sz       = sizeof(luab_in6_addr_t),
};
