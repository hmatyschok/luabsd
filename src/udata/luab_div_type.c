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

extern luab_module_t div_type;

/*
 * Interface against
 *
 *  typedef struct {
 *      int	quot;
 *      int	rem;
 *  } div_t;
 */

typedef struct luab_div {
    luab_udata_t        ud_softc;
    div_t    div;
} luab_div_t;

#define luab_new_div(L, arg) \
    ((luab_div_t *)luab_newuserdata(L, &div_type, (arg)))
#define luab_to_div(L, narg) \
    (luab_toldata((L), (narg), &div_type, \
        div_t *, sizeof(div_t)))

#define LUABSD_DIV_TYPE_ID    1600388163
#define LUABSD_DIV_TYPE    "DIV*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(DIV)) into (LUA_TTABLE).
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
 * @usage t = div:get()
 */
static int
DIV_get(lua_State *L)
{
    div_t *div;

    (void)luab_checkmaxargs(L, 1);

    div = luab_udata(L, 1, &div_type, div_t *);

    lua_newtable(L);
    luab_setinteger(L, -2, "quot", div->quot);
    luab_setinteger(L, -2, "rem", div->rem);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate div{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = div:dump()
 */
static int
DIV_dump(lua_State *L)
{
    return (luab_dump(L, 1, &div_type, sizeof(div_t)));
}

/*
 * Accessor.
 */

/***
 * Set quotient for integral division operations.
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
 * @usage data [, err, msg ] = div:set_quot(data)
 */
static int
DIV_set_quot(lua_State *L)
{
    div_t *div;
    int data;

    (void)luab_checkmaxargs(L, 2);

    div = luab_udata(L, 1, &div_type, div_t *);
    data = (int)luab_checkinteger(L, 2, INT_MAX);

    div->quot = data;

    return (luab_pusherr(L, data));
}

/***
 * Get quotient for integral division operations.
 *
 * @function get_quot
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = div:get_quot()
 */
static int
DIV_get_quot(lua_State *L)
{
    div_t *div;
    int data;

    (void)luab_checkmaxargs(L, 1);

    div = luab_udata(L, 1, &div_type, div_t *);
    data = div->quot;

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
 * @usage data [, err, msg ] = div:set_rem(data)
 */
static int
DIV_set_rem(lua_State *L)
{
    div_t *div;
    int data;

    (void)luab_checkmaxargs(L, 2);

    div = luab_udata(L, 1, &div_type, div_t *);
    data = (int)luab_checkinteger(L, 2, INT_MAX);

    div->rem = data;

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
 * @usage data [, err, msg ] = div:get_rem()
 */
static int
DIV_get_rem(lua_State *L)
{
    div_t *div;
    int data;

    (void)luab_checkmaxargs(L, 1);

    div = luab_udata(L, 1, &div_type, div_t *);
    data = div->rem;

    return (luab_pusherr(L, data));
}

/*
 * Meta-methods.
 */

static int
DIV_gc(lua_State *L)
{
    return (luab_gc(L, 1, &div_type));
}

static int
DIV_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &div_type));
}

/*
 * Internal interface.
 */

static luab_table_t div_methods[] = {
    LUABSD_FUNC("set_quot",   DIV_set_quot),
    LUABSD_FUNC("set_rem",    DIV_set_rem),
    LUABSD_FUNC("get",  DIV_get),
    LUABSD_FUNC("get_quot",  DIV_get_quot),
    LUABSD_FUNC("get_rem", DIV_get_rem),
    LUABSD_FUNC("dump", DIV_dump),
    LUABSD_FUNC("__gc", DIV_gc),
    LUABSD_FUNC("__tostring",   DIV_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
div_create(lua_State *L, void *arg)
{
    return (luab_new_div(L, arg));
}

static void
div_init(void *ud, void *arg)
{
    luab_div_t *self;

    if (((self = (luab_div_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->div, arg, sizeof(self->div));
}

static void *
div_udata(lua_State *L, int narg)
{
    return (luab_to_div(L, narg));
}

luab_module_t div_type = {
    .cookie = LUABSD_DIV_TYPE_ID,
    .name = LUABSD_DIV_TYPE,
    .vec = div_methods,
    .create = div_create,
    .init = div_init,
    .get = div_udata,
    .sz = sizeof(luab_div_t),
};