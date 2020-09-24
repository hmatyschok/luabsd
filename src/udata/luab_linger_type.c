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

#include <sys/time.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t linger_type;

/*
 * Interface against
 *
 *  struct linger {
 *      int	l_onoff;
 *      int	l_linger;
 *  };
 */

typedef struct luab_linger {
    luab_udata_t    ud_softc;
    struct linger    linger;
} luab_linger_t;

#define luab_new_linger(L, arg) \
    ((luab_linger_t *)luab_newuserdata(L, &linger_type, (arg)))
#define luab_to_linger(L, narg) \
    (luab_toldata((L), (narg), &linger_type, \
        struct linger *, sizeof(struct linger)))

#define LUABSD_LINGER_TYPE_ID    1597012436
#define LUABSD_LINGER_TYPE    "LINGER*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(LINGER)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              l_onoff     = (LUA_TNUMBER),
 *              l_linger    = (LUA_TNUMBER),
 *          }
 *
 * @usage t = linger:get()
 */
static int
LINGER_get(lua_State *L)
{
    struct linger *l;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, &linger_type, struct linger *);

    lua_newtable(L);
    luab_setinteger(L, -2, "l_onoff",   l->l_onoff);
    luab_setinteger(L, -2, "l_linger",  l->l_linger);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate linger{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = linger:dump()
 */
static int
LINGER_dump(lua_State *L)
{
    return (luab_dump(L, 1, &linger_type, sizeof(struct linger)));
}

/*
 * Service primitives.
 */

/***
 * Set value for option on / off.
 *
 * @function set_l_onoff
 *
 * @param data              Option.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = linger:set_l_onoff(data)
 */
static int
LINGER_set_l_onoff(lua_State *L)
{
    struct linger *l;
    int data;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, &linger_type, struct linger *);
    data = (int)luab_checkinteger(L, 2, INT_MAX);

    l->l_onoff = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for option on / off.
 *
 * @function get_l_onoff
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = linger:get_l_onoff()
 */
static int
LINGER_get_l_onoff(lua_State *L)
{
    struct linger *l;
    int data;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, &linger_type, struct linger *);
    data = l->l_onoff;

    return (luab_pusherr(L, data));
}

/***
 * Set value for for system time.
 *
 * @function set_l_linger
 *
 * @param data              Specifies l_linger.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = linger:set_l_linger(data)
 */
static int
LINGER_set_l_linger(lua_State *L)
{
    struct linger *l;
    int data;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, &linger_type, struct linger *);
    data = (int)luab_checkinteger(L, 2, INT_MAX);

    l->l_linger = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for for system time.
 *
 * @function get_l_linger
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = linger:get_l_linger()
 */
static int
LINGER_get_l_linger(lua_State *L)
{
    struct linger *l;
    int data;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, &linger_type, struct linger *);
    data = l->l_linger;

    return (luab_pusherr(L, data));
}

/*
 * Meta-methods.
 */

static int
LINGER_gc(lua_State *L)
{
    return (luab_gc(L, 1, &linger_type));
}

static int
LINGER_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &linger_type));
}

/*
 * Internal interface.
 */

static luab_table_t linger_methods[] = {
    LUABSD_FUNC("set_l_onoff",  LINGER_set_l_onoff),
    LUABSD_FUNC("set_l_linger", LINGER_set_l_linger),
    LUABSD_FUNC("get",          LINGER_get),
    LUABSD_FUNC("get_l_onoff",  LINGER_get_l_onoff),
    LUABSD_FUNC("get_l_linger", LINGER_get_l_linger),
    LUABSD_FUNC("dump",         LINGER_dump),
    LUABSD_FUNC("__gc",         LINGER_gc),
    LUABSD_FUNC("__tostring",   LINGER_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
linger_create(lua_State *L, void *arg)
{
    return (luab_new_linger(L, arg));
}

static void
linger_init(void *ud, void *arg)
{
    luab_linger_t *self;

    if (((self = (luab_linger_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->linger, arg, sizeof(self->linger));
}

static void *
linger_udata(lua_State *L, int narg)
{
    return (luab_to_linger(L, narg));
}

luab_module_t linger_type = {
    .cookie = LUABSD_LINGER_TYPE_ID,
    .name = LUABSD_LINGER_TYPE,
    .vec = linger_methods,
    .create = linger_create,
    .init = linger_init,
    .get = linger_udata,
    .sz = sizeof(luab_linger_t),
};
