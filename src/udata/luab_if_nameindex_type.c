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
    u_int   if_index;
    char    if_name[IFNAMSIZ];
} luab_if_nameindex_t;

#define luab_new_if_nameindex(L, arg) \
    ((luab_if_nameindex_t *)luab_newuserdata(L, &if_nameindex_type, (arg)))
#define luab_to_if_nameindex(L, narg) \
    (luab_toldata((L), (narg), &if_nameindex_type, \
        luab_if_nameindex_t *, sizeof(struct luab_if_nameindex)))

#define LUABSD_IF_NAMEINDEX_TYPE_ID    1596840702
#define LUABSD_IF_NAMEINDEX_TYPE    "IF_NAMEINDEX*"

int luab_StructIfNameIndex(lua_State *);

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
IfNameIndex_if_index(lua_State *L)
{
    luab_if_nameindex_t *self;
    u_int if_index;

    (void)luab_checkmaxargs(L, 1);

    self = luab_udata(L, 1, if_nameindex_type, luab_if_nameindex_t *);
    if_index = self->if_index;

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
IfNameIndex_if_name(lua_State *L)
{
    luab_if_nameindex_t *self;
    char *if_name;

    (void)luab_checkmaxargs(L, 1);

    self = luab_udata(L, 1, if_nameindex_type, luab_if_nameindex_t *);
    if_name = self->if_name;

    return (luab_pushstring(L, self->if_name));
}

/***
 * Translate if_nameindex{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = if_nameindex:get()
 */
static int
IfNameIndex_get(lua_State *L)
{
    luab_if_nameindex_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_udata(L, 1, if_nameindex_type, luab_if_nameindex_t *);

    lua_newtable(L);

    luab_setinteger(L, -2, "if_index", self->if_index);
    luab_setstring(L, -2, "if_name", self->if_name);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Copy if_nameindex{} into LUA_TUSERDATA(luab_iovec_t).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = if_nameindex:dump()
 */
static int
IfNameIndex_dump(lua_State *L)
{
    luab_iovec_param_t iop;
    size_t len, max_len;
    caddr_t data;
    int status;

    (void)luab_checkmaxargs(L, 1);

    (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

    data = luab_udata(L, 1, if_nameindex_type, caddr_t);

    len = sizeof(luab_if_nameindex_t);
    max_len = len + sizeof(uint32_t);

    iop.iop_buf.buf_len = max_len;
    iop.iop_data.buf_data = data;
    iop.iop_data.buf_len = len;

    if ((*iovec_type.ctor)(L, &iop) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

static int
IfNameIndex_gc(lua_State *L)
{
    luab_if_nameindex_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_if_nameindex(L, 1);

    (void)memset_s(self, if_nameindex_type.sz, 0, if_nameindex_type.sz);

    return (0);
}

static int
IfNameIndex_tostring(lua_State *L)
{
    luab_if_nameindex_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_if_nameindex(L, 1);
    lua_pushfstring(L, "if_nameindex (%p)", self);

    return (1);
}

static luab_table_t if_nameindex_methods[] = {
    LUABSD_FUNC("get",  IfNameIndex_get),
    LUABSD_FUNC("if_index",   IfNameIndex_if_index),
    LUABSD_FUNC("if_name",  IfNameIndex_if_name),
    LUABSD_FUNC("dump", IfNameIndex_dump),
    LUABSD_FUNC("__gc", IfNameIndex_gc),
    LUABSD_FUNC("__tostring",   IfNameIndex_tostring),
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
    .ctor = if_nameindex_create,
    .init = if_nameindex_init,
    .get = if_nameindex_udata,
    .sz = sizeof(luab_if_nameindex_t),
};

/***
 * Ctor.
 *
 * @function StructIfNameIndex
 *
 * @param if_nameindex                    Instance of LUA_TUSERDATA(luab_if_nameindex_t).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage if_nameindex [, err, msg ] = bsd.sys.time.StructIfNameIndex([ if_nameindex ])
 */
int
luab_StructIfNameIndex(lua_State *L)
{
    struct if_nameindex *ifni;
    int narg, status;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        ifni = NULL;
    else
        ifni = (struct if_nameindex *)if_nameindex_udata(L, narg);

    if (if_nameindex_create(L, ifni) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
