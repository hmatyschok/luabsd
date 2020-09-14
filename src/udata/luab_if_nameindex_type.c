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

#include <net/if.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t if_nameindex_type;

/*
 * Interface against
 *
 *  struct if_nameindex {
 *      unsigned int	if_index;
 *      char		*if_name;
 *  };
 *
 * Attributes are immutable.
 */
typedef struct luab_if_nameindex {
    luab_udata_t    ud_softc;
    u_int   if_index;
    char    if_name[IFNAMSIZ];
} luab_if_nameindex_t;

#define luab_new_if_nameindex(L, arg) \
    ((luab_if_nameindex_t *)luab_newuserdata(L, &if_nameindex_type, (arg)))
#define luab_to_if_nameindex(L, narg) \
    (luab_toudata((L), (narg), &if_nameindex_type))

#define LUABSD_IF_NAMEINDEX_TYPE_ID    1596840702
#define LUABSD_IF_NAMEINDEX_TYPE    "IF_NAMEINDEX*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(IF_NAMEINDEX)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              if_index    = (LUA_TNUMBER),
 *              if_name     = (LUA_TSTRING),
 *          }
 *
 * @usage t = if_nameindex:get()
 */
static int
IF_NAMEINDEX_get(lua_State *L)
{
    struct if_nameindex *ifni;

    (void)luab_checkmaxargs(L, 1);

    ifni = luab_udata(L, 1, if_nameindex_type, struct if_nameindex *);

    lua_newtable(L);
    luab_setinteger(L, -2, "if_index", ifni->if_index);
    luab_setstring(L, -2, "if_name", ifni->if_name);
    lua_pushvalue(L, -1);

    return (1);
}

/*
 * Accessor, immutable properties.
 */

/***
 * Get value for if_index.
 *
 * @function if_index
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (index [, nil, nil]) on success or
 *          (index, (errno, strerror(errno)))
 *
 * @usage index [, err, msg ] = if_nameindex:if_index()
 */
static int
IF_NAMEINDEX_if_index(lua_State *L)
{
    struct if_nameindex *ifni;
    u_int if_index;

    (void)luab_checkmaxargs(L, 1);

    ifni = luab_udata(L, 1, if_nameindex_type, struct if_nameindex *);
    if_index = ifni->if_index;

    return (luab_pusherr(L, if_index));
}

/***
 * Get value for if_name.
 *
 * @function if_name
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (name [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage name [, err, msg ]= if_nameindex:if_name()
 */
static int
IF_NAMEINDEX_if_name(lua_State *L)
{
    struct if_nameindex *ifni;
    char *if_name;

    (void)luab_checkmaxargs(L, 1);

    ifni = luab_udata(L, 1, if_nameindex_type, struct if_nameindex *);
    if_name = ifni->if_name;

    return (luab_pushstring(L, ifni->if_name));
}

/*
 * Meta-medthods.
 */

static int
IF_NAMEINDEX_gc(lua_State *L)
{
    return (luab_gc(L, 1, &if_nameindex_type));
}

static int
IF_NAMEINDEX_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &if_nameindex_type));
}

/*
 * Internal interface.
 */

static luab_table_t if_nameindex_methods[] = {
    LUABSD_FUNC("if_index",   IF_NAMEINDEX_if_index),
    LUABSD_FUNC("if_name",  IF_NAMEINDEX_if_name),
    LUABSD_FUNC("get",  IF_NAMEINDEX_get),
    LUABSD_FUNC("__gc", IF_NAMEINDEX_gc),
    LUABSD_FUNC("__tostring",   IF_NAMEINDEX_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
if_nameindex_create(lua_State *L, void *arg)
{
    return (luab_new_if_nameindex(L, arg));
}

static void
if_nameindex_init(void *ud, void *arg)
{
    luab_if_nameindex_t *self;
    struct if_nameindex *ifni;
    size_t len;

    if (((self = (luab_if_nameindex_t *)ud) != NULL) &&
        ((ifni = (struct if_nameindex *)arg) != NULL)) {
        self->if_index = ifni->if_index;
        len = strnlen(ifni->if_name, IFNAMSIZ);
        (void)memmove(self->if_name, ifni->if_name, len);
    }
}

static void *
if_nameindex_udata(lua_State *L, int narg)
{
    return (luab_to_if_nameindex(L, narg));
}

luab_module_t if_nameindex_type = {
    .cookie = LUABSD_IF_NAMEINDEX_TYPE_ID,
    .name = LUABSD_IF_NAMEINDEX_TYPE,
    .vec = if_nameindex_methods,
    .create = if_nameindex_create,
    .init = if_nameindex_init,
    .get = if_nameindex_udata,
    .sz = sizeof(luab_if_nameindex_t),
};
