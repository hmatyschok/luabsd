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

#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_types.h"

#if __BSD_VISIBLE
extern luab_module_t sockproto_type;

/*
 * Interface against
 *
 *  struct sockproto {
 *      unsigned short  sp_family;
 *      unsigned short  sp_protocol;
 *  };
 */

typedef struct luab_sockproto {
    luab_udata_t        ud_softc;
    struct sockproto    ud_sp;
} luab_sockproto_t;

#define luab_new_sockproto(L, arg) \
    ((luab_sockproto_t *)luab_newudata(L, &sockproto_type, (arg)))
#define luab_to_sockproto(L, narg) \
    (luab_toldata((L), (narg), &sockproto_type, \
        struct sockproto *, sizeof(struct sockproto)))

#define LUAB_SOCKPROTO_TYPE_ID    1601383780
#define LUAB_SOCKPROTO_TYPE    "SOCKPROTO*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SOCKPROTO)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              sp_family   = (LUA_TSTRING),
 *              sp_protocol = (LUA_TSTRING),
 *          }
 *
 * @usage t = sockproto:get()
 */
static int
SOCKPROTO_get(lua_State *L)
{
    struct sockproto *sp;

    (void)luab_checkmaxargs(L, 1);

    sp = luab_udata(L, 1, &sockproto_type, struct sockproto *);

    lua_newtable(L);
    luab_setinteger(L, -2, "sp_family",     sp->sp_family);
    luab_setinteger(L, -2, "sp_protocol",   sp->sp_protocol);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate sockproto{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = sockproto:dump()
 */
static int
SOCKPROTO_dump(lua_State *L)
{
    return (luab_dump(L, 1, &sockproto_type, sizeof(struct sockproto)));
}

/*
 * Accessor.
 */

/***
 * Set protocol family.
 *
 * @function set_sp_family
 *
 * @param data              Specifies protocol domain(9) by name, (LUA_TSTRING).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = sockproto:set_sp_family(data)
 */
static int
SOCKPROTO_set_sp_family(lua_State *L)
{
    struct sockproto *sp;
    u_short data;

    (void)luab_checkmaxargs(L, 2);

    sp = luab_udata(L, 1, &sockproto_type, struct sockproto *);
    data = (u_short)luab_checkinteger(L, 2, SHRT_MAX);
    sp->sp_family = data;

    return (luab_pusherr(L, data));
}

/***
 * Get protocol family.
 *
 * @function get_sp_family
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = sockproto:get_sp_family()
 */
static int
SOCKPROTO_get_sp_family(lua_State *L)
{
    struct sockproto *sp;
    u_short data;

    (void)luab_checkmaxargs(L, 1);

    sp = luab_udata(L, 1, &sockproto_type, struct sockproto *);
    data = sp->sp_family;

    return (luab_pusherr(L, data));
}

/***
 * Set protocol.
 *
 * @function set_sp_protocol
 *
 * @param data              Specifies accept filter string, (LUA_TSTRING).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = sockproto:set_sp_protocol(data)
 */
static int
SOCKPROTO_set_sp_protocol(lua_State *L)
{
    struct sockproto *sp;
    u_short data;

    (void)luab_checkmaxargs(L, 2);

    sp = luab_udata(L, 1, &sockproto_type, struct sockproto *);
    data = (u_short)luab_checkinteger(L, 2, SHRT_MAX);
    sp->sp_protocol = data;

    return (luab_pusherr(L, data));
}

/***
 * Get protocol.
 *
 * @function get_sp_protocol
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = sockproto:get_sp_protocol()
 */
static int
SOCKPROTO_get_sp_protocol(lua_State *L)
{
    struct sockproto *sp;
    u_short data;

    (void)luab_checkmaxargs(L, 1);

    sp = luab_udata(L, 1, &sockproto_type, struct sockproto *);
    data = sp->sp_protocol;

    return (luab_pusherr(L, data));
}

/*
 * Metamethods
 */

static int
SOCKPROTO_gc(lua_State *L)
{
    return (luab_gc(L, 1, &sockproto_type));
}

static int
SOCKPROTO_len(lua_State *L)
{
    return (luab_len(L, 2, &sockproto_type));
}

static int
SOCKPROTO_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &sockproto_type));
}

/*
 * Internal interface.
 */

static luab_table_t sockproto_methods[] = {
    LUAB_FUNC("set_sp_family",    SOCKPROTO_set_sp_family),
    LUAB_FUNC("set_sp_protocol",  SOCKPROTO_set_sp_protocol),
    LUAB_FUNC("get",              SOCKPROTO_get),
    LUAB_FUNC("get_sp_family",    SOCKPROTO_get_sp_family),
    LUAB_FUNC("get_sp_protocol",  SOCKPROTO_get_sp_protocol),
    LUAB_FUNC("dump",             SOCKPROTO_dump),
    LUAB_FUNC("__gc",             SOCKPROTO_gc),
    LUAB_FUNC("__len",            SOCKPROTO_len),
    LUAB_FUNC("__tostring",       SOCKPROTO_tostring),
    LUAB_FUNC(NULL, NULL)
};

static void *
sockproto_create(lua_State *L, void *arg)
{
    return (luab_new_sockproto(L, arg));
}

static void
sockproto_init(void *ud, void *arg)
{
    luab_sockproto_t *self = (luab_sockproto_t *)ud;

    (void)memmove(&self->ud_sp, arg, sizeof(self->ud_sp));
}

static void *
sockproto_udata(lua_State *L, int narg)
{
    return (luab_to_sockproto(L, narg));
}

luab_module_t sockproto_type = {
    .cookie = LUAB_SOCKPROTO_TYPE_ID,
    .name = LUAB_SOCKPROTO_TYPE,
    .vec = sockproto_methods,
    .create = sockproto_create,
    .init = sockproto_init,
    .get = sockproto_udata,
    .sz = sizeof(luab_sockproto_t),
};
#endif /* __BSD_VISBLE */
