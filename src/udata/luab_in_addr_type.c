/*
 * Copyright (c) 2020 Henning Matyschok <hmatyschok@outlook.com>
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

#include <arpa/inet.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t in_addr_type;

/*
 * Interface against
 *
 *  struct in_addr {
 *      in_addr_t s_addr;
 *  };
 */

typedef struct luab_in_addr {
    luab_udata_t    ud_softc;
    struct in_addr    in_addr;
} luab_in_addr_t;

#define luab_new_in_addr(L, arg) \
    ((luab_in_addr_t *)luab_newuserdata(L, &in_addr_type, (arg)))
#define luab_to_in_addr(L, narg) \
    (luab_toldata((L), (narg), &in_addr_type, \
        struct in_addr *, sizeof(struct in_addr)))

#define LUABSD_IN_ADDR_TYPE_ID    1595799233
#define LUABSD_IN_ADDR_TYPE    "IN_ADDR*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(IN_ADDR)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              s_addr  = (LUA_TNUMBER),
 *          }
 *
 * @usage t = in_addr:get()
 */
static int
IN_ADDR_get(lua_State *L)
{
    struct in_addr *ia;

    (void)luab_checkmaxargs(L, 1);

    ia = luab_udata(L, 1, in_addr_type, struct in_addr *);

    lua_newtable(L);
    luab_setinteger(L, -2, "s_addr", ia->s_addr);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate in_addr{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = in_addr:dump()
 */
static int
IN_ADDR_dump(lua_State *L)
{
    return (luab_dump(L, 1, &in_addr_type, sizeof(struct in_addr)));
}

/*
 * Accessor.
 */

/***
 * Set IPv4 address.
 *
 * @function set_s_addr
 *
 * @param id                IPv4 address.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (id [, nil, nil]) on success or
 *          (id, (errno, strerror(errno)))
 *
 * @usage id [, err, mag ] = in_addr:set_s_addr(id)
 */
static int
IN_ADDR_set_s_addr(lua_State *L)
{
    struct in_addr *ia;
    in_addr_t id;

    (void)luab_checkmaxargs(L, 2);

    ia = luab_udata(L, 1, in_addr_type, struct in_addr *);
    id = (in_addr_t)luab_checkinteger(L, 2, UINT_MAX);

    ia->s_addr = id;

    return (luab_pusherr(L, id));
}

/***
 * Get IPv4 address.
 *
 * @function get_s_addr
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (id [, nil, nil]) on success or
 *          (id, (errno, strerror(errno)))
 *
 * @usage id [, err, msg ] = in_addr:get_s_addr()
 */
static int
IN_ADDR_get_s_addr(lua_State *L)
{
    struct in_addr *ia;
    in_addr_t id;

    (void)luab_checkmaxargs(L, 1);

    ia = luab_udata(L, 1, in_addr_type, struct in_addr *);
    id = ia->s_addr;

    return (luab_pusherr(L, id));
}

/*
 * Meta-methods.
 */

static int
IN_ADDR_gc(lua_State *L)
{
    return (luab_gc(L, 1, &in_addr_type));
}

static int
IN_ADDR_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &in_addr_type));
}

/*
 * Internal interface.
 */

static luab_table_t in_addr_methods[] = {
    LUABSD_FUNC("set_s_addr",  IN_ADDR_set_s_addr),
    LUABSD_FUNC("get",  IN_ADDR_get),
    LUABSD_FUNC("get_s_addr",  IN_ADDR_get_s_addr),
    LUABSD_FUNC("dump", IN_ADDR_dump),
    LUABSD_FUNC("__gc", IN_ADDR_gc),
    LUABSD_FUNC("__tostring",   IN_ADDR_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
in_addr_create(lua_State *L, void *arg)
{
    return (luab_new_in_addr(L, arg));
}

static void
in_addr_init(void *ud, void *arg)
{
    luab_in_addr_t *self;

    if (((self = (luab_in_addr_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->in_addr, arg, sizeof(self->in_addr));
}

static void *
in_addr_udata(lua_State *L, int narg)
{
    return (luab_to_in_addr(L, narg));
}

luab_module_t in_addr_type = {
    .cookie = LUABSD_IN_ADDR_TYPE_ID,
    .name = LUABSD_IN_ADDR_TYPE,
    .vec = in_addr_methods,
    .create = in_addr_create,
    .init = in_addr_init,
    .get = in_addr_udata,
    .sz = sizeof(luab_in_addr_t),
};
