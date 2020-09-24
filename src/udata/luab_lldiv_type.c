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

#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t lldiv_type;

/*
 * Interface against
 *
 *  typedef struct {
 *      long long	quot;
 *      long long	rem;
 *  } lldiv_t;
 */

typedef struct luab_lldiv {
    luab_udata_t        ud_softc;
    lldiv_t    lldiv;
} luab_lldiv_t;

#define luab_new_lldiv(L, arg) \
    ((luab_lldiv_t *)luab_newuserdata(L, &lldiv_type, (arg)))
#define luab_to_lldiv(L, narg) \
    (luab_toldata((L), (narg), &lldiv_type, \
        lldiv_t *, sizeof(lldiv_t)))

#define LUABSD_LLDIV_TYPE_ID    1600628549
#define LUABSD_LLDIV_TYPE    "LLDIV*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(LLDIV)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              quot      = (LUA_TNUMBER),
 *              rem    = (LUA_TNUMBER),
 *          }
 *
 * @usage t = lldiv:get()
 */
static int
LLDIV_get(lua_State *L)
{
    lldiv_t *lldiv;

    (void)luab_checkmaxargs(L, 1);

    lldiv = luab_udata(L, 1, &lldiv_type, lldiv_t *);

    lua_newtable(L);
    luab_setinteger(L, -2, "quot",  lldiv->quot);
    luab_setinteger(L, -2, "rem",   lldiv->rem);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate lldiv{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = lldiv:dump()
 */
static int
LLDIV_dump(lua_State *L)
{
    return (luab_dump(L, 1, &lldiv_type, sizeof(lldiv_t)));
}

/*
 * Accessor.
 */

/***
 * Set quotient for integral lldivision operations.
 *
 * @function set_quot
 *
 * @param data              Quotient.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = lldiv:set_quot(data)
 */
static int
LLDIV_set_quot(lua_State *L)
{
    lldiv_t *lldiv;
    long data;

    (void)luab_checkmaxargs(L, 2);

    lldiv = luab_udata(L, 1, &lldiv_type, lldiv_t *);
    data = (long)luab_checkinteger(L, 2, INT_MAX);

    lldiv->quot = data;

    return (luab_pusherr(L, data));
}

/***
 * Get quotient for integral lldivision operations.
 *
 * @function get_quot
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = lldiv:get_quot()
 */
static int
LLDIV_get_quot(lua_State *L)
{
    lldiv_t *lldiv;
    long data;

    (void)luab_checkmaxargs(L, 1);

    lldiv = luab_udata(L, 1, &lldiv_type, lldiv_t *);
    data = lldiv->quot;

    return (luab_pusherr(L, data));
}

/***
 * Set reminder of integal devision operations.
 *
 * @function set_rem
 *
 * @param data              Reminder.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = lldiv:set_rem(data)
 */
static int
LLDIV_set_rem(lua_State *L)
{
    lldiv_t *lldiv;
    long data;

    (void)luab_checkmaxargs(L, 2);

    lldiv = luab_udata(L, 1, &lldiv_type, lldiv_t *);
    data = (long)luab_checkinteger(L, 2, INT_MAX);

    lldiv->rem = data;

    return (luab_pusherr(L, data));
}

/***
 * Get reminder of integal devision operations.
 *
 * @function get_rem
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = lldiv:get_rem()
 */
static int
LLDIV_get_rem(lua_State *L)
{
    lldiv_t *lldiv;
    long data;

    (void)luab_checkmaxargs(L, 1);

    lldiv = luab_udata(L, 1, &lldiv_type, lldiv_t *);
    data = lldiv->rem;

    return (luab_pusherr(L, data));
}

/*
 * Meta-methods.
 */

static int
LLDIV_gc(lua_State *L)
{
    return (luab_gc(L, 1, &lldiv_type));
}

static int
LLDIV_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &lldiv_type));
}

/*
 * Internal interface.
 */

static luab_table_t lldiv_methods[] = {
    LUABSD_FUNC("set_quot",     LLDIV_set_quot),
    LUABSD_FUNC("set_rem",      LLDIV_set_rem),
    LUABSD_FUNC("get",          LLDIV_get),
    LUABSD_FUNC("get_quot",     LLDIV_get_quot),
    LUABSD_FUNC("get_rem",      LLDIV_get_rem),
    LUABSD_FUNC("dump",         LLDIV_dump),
    LUABSD_FUNC("__gc",         LLDIV_gc),
    LUABSD_FUNC("__tostring",   LLDIV_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
lldiv_create(lua_State *L, void *arg)
{
    return (luab_new_lldiv(L, arg));
}

static void
lldiv_init(void *ud, void *arg)
{
    luab_lldiv_t *self;

    if (((self = (luab_lldiv_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->lldiv, arg, sizeof(self->lldiv));
}

static void *
lldiv_udata(lua_State *L, int narg)
{
    return (luab_to_lldiv(L, narg));
}

luab_module_t lldiv_type = {
    .cookie = LUABSD_LLDIV_TYPE_ID,
    .name = LUABSD_LLDIV_TYPE,
    .vec = lldiv_methods,
    .create = lldiv_create,
    .init = lldiv_init,
    .get = lldiv_udata,
    .sz = sizeof(luab_lldiv_t),
};
