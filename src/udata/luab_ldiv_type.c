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

extern luab_module_t ldiv_type;

/*
 * Interface against
 *
 *  typedef struct {
 *      long	quot;
 *      long	rem;
 *  } ldiv_t;
 */

typedef struct luab_ldiv {
    luab_udata_t        ud_softc;
    ldiv_t    ldiv;
} luab_ldiv_t;

#define luab_new_ldiv(L, arg) \
    ((luab_ldiv_t *)luab_newuserdata(L, &ldiv_type, (arg)))
#define luab_to_ldiv(L, narg) \
    (luab_toldata((L), (narg), &ldiv_type, \
        ldiv_t *, sizeof(ldiv_t)))

#define LUABSD_LDIV_TYPE_ID    1600394297
#define LUABSD_LDIV_TYPE    "LDIV*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(LDIV)) into (LUA_TTABLE).
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
 * @usage t = ldiv:get()
 */
static int
LDIV_get(lua_State *L)
{
    ldiv_t *ldiv;

    (void)luab_checkmaxargs(L, 1);

    ldiv = luab_udata(L, 1, &ldiv_type, ldiv_t *);

    lua_newtable(L);
    luab_setinteger(L, -2, "quot", ldiv->quot);
    luab_setinteger(L, -2, "rem", ldiv->rem);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate ldiv{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = ldiv:dump()
 */
static int
LDIV_dump(lua_State *L)
{
    return (luab_dump(L, 1, &ldiv_type, sizeof(ldiv_t)));
}

/*
 * Accessor.
 */

/***
 * Set quotient for integral ldivision operations.
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
 * @usage data [, err, msg ] = ldiv:set_quot(data)
 */
static int
LDIV_set_quot(lua_State *L)
{
    ldiv_t *ldiv;
    long data;

    (void)luab_checkmaxargs(L, 2);

    ldiv = luab_udata(L, 1, &ldiv_type, ldiv_t *);
    data = (long)luab_checkinteger(L, 2, INT_MAX);

    ldiv->quot = data;

    return (luab_pusherr(L, data));
}

/***
 * Get quotient for integral ldivision operations.
 *
 * @function get_quot
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = ldiv:get_quot()
 */
static int
LDIV_get_quot(lua_State *L)
{
    ldiv_t *ldiv;
    long data;

    (void)luab_checkmaxargs(L, 1);

    ldiv = luab_udata(L, 1, &ldiv_type, ldiv_t *);
    data = ldiv->quot;

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
 * @usage data [, err, msg ] = ldiv:set_rem(data)
 */
static int
LDIV_set_rem(lua_State *L)
{
    ldiv_t *ldiv;
    long data;

    (void)luab_checkmaxargs(L, 2);

    ldiv = luab_udata(L, 1, &ldiv_type, ldiv_t *);
    data = (long)luab_checkinteger(L, 2, INT_MAX);

    ldiv->rem = data;

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
 * @usage data [, err, msg ] = ldiv:get_rem()
 */
static int
LDIV_get_rem(lua_State *L)
{
    ldiv_t *ldiv;
    long data;

    (void)luab_checkmaxargs(L, 1);

    ldiv = luab_udata(L, 1, &ldiv_type, ldiv_t *);
    data = ldiv->rem;

    return (luab_pusherr(L, data));
}

/*
 * Meta-methods.
 */

static int
LDIV_gc(lua_State *L)
{
    return (luab_gc(L, 1, &ldiv_type));
}

static int
LDIV_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &ldiv_type));
}

/*
 * Internal interface.
 */

static luab_table_t ldiv_methods[] = {
    LUABSD_FUNC("set_quot",   LDIV_set_quot),
    LUABSD_FUNC("set_rem",    LDIV_set_rem),
    LUABSD_FUNC("get",  LDIV_get),
    LUABSD_FUNC("get_quot",  LDIV_get_quot),
    LUABSD_FUNC("get_rem", LDIV_get_rem),
    LUABSD_FUNC("dump", LDIV_dump),
    LUABSD_FUNC("__gc", LDIV_gc),
    LUABSD_FUNC("__tostring",   LDIV_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
ldiv_create(lua_State *L, void *arg)
{
    return (luab_new_ldiv(L, arg));
}

static void
ldiv_init(void *ud, void *arg)
{
    luab_ldiv_t *self;

    if (((self = (luab_ldiv_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->ldiv, arg, sizeof(self->ldiv));
}

static void *
ldiv_udata(lua_State *L, int narg)
{
    return (luab_to_ldiv(L, narg));
}

luab_module_t ldiv_type = {
    .cookie = LUABSD_LDIV_TYPE_ID,
    .name = LUABSD_LDIV_TYPE,
    .vec = ldiv_methods,
    .create = ldiv_create,
    .init = ldiv_init,
    .get = ldiv_udata,
    .sz = sizeof(luab_ldiv_t),
};