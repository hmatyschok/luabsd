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

#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t in6_addr_type;

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
    struct in6_addr    in6_addr;
} luab_in6_addr_t;
#define s6_addr32   __u6_addr.__u6_addr32

#define luab_new_in6_addr(L, arg) \
    ((luab_in6_addr_t *)luab_newuserdata(L, &in6_addr_type, (arg)))
#define luab_to_in6_addr(L, narg) \
    (luab_toldata((L), (narg), &in6_addr_type, \
        luab_in6_addr_t *, sizeof(struct in6_addr)))

#define LUABSD_IN6ADDR_TYPE_ID    1595890830
#define LUABSD_IN6ADDR_TYPE    "IN6ADDR*"

int luab_StructIn6Addr(lua_State *);

/***
 * Copyin IPv6 address.
 *
 * @function set_s6_addr
 *
 * @param s6_addr           LUA_TTABLE(uint32_t) with cardinality of 4.
 *
 * @usage in6_addr:set_s6_addr(s_addr)
 */
static int
In6Addr_set_s6_addr(lua_State *L)
{
    struct in6_addr *ia;
    uint32_t k, v;

    (void)luab_checkmaxargs(L, 2);

    ia = (struct in6_addr *)(*in6_addr_type.get)(L, 1);

    (void)luab_checkltable(L, 2, 4);

    lua_pushnil(L);

    for (k = 0; lua_next(L, 2) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (uint32_t)luab_tointeger(L, -1, UINT_MAX);
            ia->s6_addr32[k] = v;
        } else
            luaL_argerror(L, 2, "Invalid argument");

        lua_pop(L, 1);
    }
    return (0);
}

/***
 * Copyout IPv6 address.
 *
 * @function get_s6_addr
 *
 * @return (LUA_TTABLE) with cardinality of 4 over uint32_t.
 *
 * @usage s6_addr = in6_addr:get_s6_addr()
 */
static int
In6Addr_get_s6_addr(lua_State *L)
{
    struct in6_addr *ia;
    int i, j;

    (void)luab_checkmaxargs(L, 1);

    ia = (struct in6_addr *)(*in6_addr_type.get)(L, 1);

    lua_newtable(L);

    for (i = 0, j = 1; i < 4; i++, j++)
        luab_rawsetinteger(L, -2, j, ia->s6_addr32[i]);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Translate in6_addr{} into byte string.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = in6_addr:get()
 */
static int
In6Addr_get(lua_State *L)
{
    struct in6_addr *ia;
    luaL_Buffer buf;
    caddr_t dst, src;
    size_t len;

    (void)luab_checkmaxargs(L, 1);

    ia = (struct in6_addr *)(*in6_addr_type.get)(L, 1);
    len = sizeof(ia->__u6_addr);
    src = ia->s6_addr;

    lua_newtable(L);

    luaL_buffinit(L, &buf);
    dst = luaL_prepbuffsize(&buf, len);

    (void)memmove(dst, src, len);

    luaL_addsize(&buf, len);
    luaL_pushresult(&buf);

    lua_setfield(L, -2, "s6_addr");

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Copy in6_addr{} into LUA_TUSERDATA(luab_iovec_t).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = in6_addr:dump()
 */
static int
In6Addr_dump(lua_State *L)
{
    luab_iovec_param_t iop;
    size_t len, max_len;
    caddr_t data;
    int status;

    (void)luab_checkmaxargs(L, 1);

    (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

    data = (caddr_t)(*in6_addr_type.get)(L, 1);

    len = sizeof(struct in6_addr);
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
In6Addr_gc(lua_State *L)
{
    luab_in6_addr_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_in6_addr(L, 1);

    (void)memset_s(self, in6_addr_type.sz, 0, in6_addr_type.sz);

    return (0);
}

static int
In6Addr_tostring(lua_State *L)
{
    luab_in6_addr_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_in6_addr(L, 1);
    lua_pushfstring(L, "in6_addr (%p)", self);

    return (1);
}

static luab_table_t in6_addr_methods[] = {
    LUABSD_FUNC("set_s6_addr",  In6Addr_set_s6_addr),
    LUABSD_FUNC("get",  In6Addr_get),
    LUABSD_FUNC("get_s6_addr",  In6Addr_get_s6_addr),
    LUABSD_FUNC("dump", In6Addr_dump),
    LUABSD_FUNC("__gc", In6Addr_gc),
    LUABSD_FUNC("__tostring",   In6Addr_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
in6_addr_create(lua_State *L, void *arg)
{
    return (luab_new_in6_addr(L, arg));
}

static void
in6_addr_init(void *ud, void *arg)
{
    luab_in6_addr_t *self;

    if (((self = (luab_in6_addr_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->in6_addr, arg, sizeof(self->in6_addr));
}

static void *
in6_addr_udata(lua_State *L, int narg)
{
    luab_in6_addr_t *self = luab_to_in6_addr(L, narg);

    return (&self->in6_addr);
}

luab_module_t in6_addr_type = {
    .cookie = LUABSD_IN6ADDR_TYPE_ID,
    .name = LUABSD_IN6ADDR_TYPE,
    .vec = in6_addr_methods,
    .ctor = in6_addr_create,
    .init = in6_addr_init,
    .get = in6_addr_udata,
    .sz = sizeof(luab_in6_addr_t),
};

/***
 * Ctor.
 *
 * @function StructIn6Addr
 *
 * @param in6_addr           Instance of LUA_TUSERDATA(luab_in6_addr_t), optional.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (in6_addr [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage in6_addr [, err, msg ] = bsd.sys.time.StructIn6Addr([ in6_addr ])
 */
int
luab_StructIn6Addr(lua_State *L)
{
    struct in6_addr *in6_addr;
    int narg, status;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        in6_addr = NULL;
    else
        in6_addr = in6_addr_udata(L, narg);

    if (in6_addr_create(L, in6_addr) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
