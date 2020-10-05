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

#ifdef __BSD_VISIBLE
extern luab_module_t accept_filter_arg_type;

/*
 * Interface against
 *
 *  struct accept_filter_arg {
 *      char    af_name[16];
 *      char    af_arg[256-16];
 *  };
 */

typedef struct luab_accept_filter_arg {
    luab_udata_t                ud_softc;
    struct accept_filter_arg    ud_af;
} luab_accept_filter_arg_t;

#define LUAB_AF_NAMEMAXLEN  16
#define LUAB_AF_ARGMAXLEN   240

#define luab_new_accept_filter_arg(L, arg) \
    ((luab_accept_filter_arg_t *)luab_newudata(L, &accept_filter_arg_type, (arg)))
#define luab_to_accept_filter_arg(L, narg) \
    (luab_toldata((L), (narg), &accept_filter_arg_type, \
        struct accept_filter_arg *, sizeof(struct accept_filter_arg)))

#define LUABSD_ACCEPT_FILTER_ARG_TYPE_ID    1601330449
#define LUABSD_ACCEPT_FILTER_ARG_TYPE    "ACCEPT_FILTER_ARG*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(ACCEPT_FILTER_ARG)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              af_name    = (LUA_TSTRING),
 *              af_arg     = (LUA_TSTRING),
 *          }
 *
 * @usage t = accept_filter_arg:get()
 */
static int
ACCEPT_FILTER_ARG_get(lua_State *L)
{
    struct accept_filter_arg *af;

    (void)luab_checkmaxargs(L, 1);

    af = luab_udata(L, 1, &accept_filter_arg_type, struct accept_filter_arg *);

    lua_newtable(L);
    luab_setldata(L, -2, "af_name", af->af_name, strlen(af->af_name));
    luab_setldata(L, -2, "af_arg",  af->af_arg, strlen(af->af_arg));
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate accept_filter_arg{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = accept_filter_arg:dump()
 */
static int
ACCEPT_FILTER_ARG_dump(lua_State *L)
{
    return (luab_dump(L, 1, &accept_filter_arg_type, sizeof(struct accept_filter_arg)));
}

/*
 * Accessor.
 */

/***
 * Copy protocol domain(9) name into data region.
 *
 * @function set_af_name
 *
 * @param data              Specifies protocol domain(9) by name, (LUA_TSTRING).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = accept_filter_arg:set_af_name(data)
 */
static int
ACCEPT_FILTER_ARG_set_af_name(lua_State *L)
{
    struct accept_filter_arg *af;
    const char *data;
    size_t len;

    (void)luab_checkmaxargs(L, 2);

    af = luab_udata(L, 1, &accept_filter_arg_type, struct accept_filter_arg *);
    data = luab_checklstring(L, 2, LUAB_AF_NAMEMAXLEN);
    len = strlen(data);
    (void)memmove(af->af_name, data, len);
    return (luab_pushldata(L, af->af_name, len));
}

/***
 * Copy name of protocol domain(9) from data region.
 *
 * @function get_af_name
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = accept_filter_arg:get_af_name()
 */
static int
ACCEPT_FILTER_ARG_get_af_name(lua_State *L)
{
    struct accept_filter_arg *af;
    caddr_t data;
    size_t len;

    (void)luab_checkmaxargs(L, 1);

    af = luab_udata(L, 1, &accept_filter_arg_type, struct accept_filter_arg *);
    data = af->af_name;
    len = strlen(data);

    return (luab_pushldata(L, data, len));
}

/***
 * Copy accept filter string into data region.
 *
 * @function set_af_arg
 *
 * @param data              Specifies accept filter string, (LUA_TSTRING).
 *
 * @return (LUA_T{NIL,NUMBER} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = accept_filter_arg:set_af_name(data)
 */
static int
ACCEPT_FILTER_ARG_set_af_arg(lua_State *L)
{
    struct accept_filter_arg *af;
    const char *data;
    size_t len;

    (void)luab_checkmaxargs(L, 2);

    af = luab_udata(L, 1, &accept_filter_arg_type, struct accept_filter_arg *);
    data = luab_checklstring(L, 2, LUAB_AF_ARGMAXLEN);
    len = strlen(data);
    (void)memmove(af->af_arg, data, len);
    return (luab_pushldata(L, af->af_arg, len));
}

/***
 * Copy accept filter string from data region.
 *
 * @function get_af_name
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = accept_filter_arg:get_af_name()
 */
static int
ACCEPT_FILTER_ARG_get_af_arg(lua_State *L)
{
    struct accept_filter_arg *af;
    caddr_t data;
    size_t len;

    (void)luab_checkmaxargs(L, 1);

    af = luab_udata(L, 1, &accept_filter_arg_type, struct accept_filter_arg *);
    data = af->af_arg;
    len = strlen(data);

    return (luab_pushldata(L, data, len));
}

/*
 * Metamethods
 */

static int
ACCEPT_FILTER_ARG_gc(lua_State *L)
{
    return (luab_gc(L, 1, &accept_filter_arg_type));
}

static int
ACCEPT_FILTER_ARG_len(lua_State *L)
{
    return (luab_len(L, 2, &accept_filter_arg_type));
}

static int
ACCEPT_FILTER_ARG_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &accept_filter_arg_type));
}

/*
 * Internal interface.
 */

static luab_table_t accept_filter_arg_methods[] = {
    LUABSD_FUNC("set_af_name",  ACCEPT_FILTER_ARG_set_af_name),
    LUABSD_FUNC("set_af_arg",   ACCEPT_FILTER_ARG_set_af_arg),
    LUABSD_FUNC("get",          ACCEPT_FILTER_ARG_get),
    LUABSD_FUNC("get_af_name",  ACCEPT_FILTER_ARG_get_af_name),
    LUABSD_FUNC("get_af_arg",   ACCEPT_FILTER_ARG_get_af_arg),
    LUABSD_FUNC("dump",         ACCEPT_FILTER_ARG_dump),
    LUABSD_FUNC("__gc",         ACCEPT_FILTER_ARG_gc),
    LUABSD_FUNC("__len",        ACCEPT_FILTER_ARG_len),
    LUABSD_FUNC("__tostring",   ACCEPT_FILTER_ARG_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
accept_filter_arg_create(lua_State *L, void *arg)
{
    return (luab_new_accept_filter_arg(L, arg));
}

static void
accept_filter_arg_init(void *ud, void *arg)
{
    luab_accept_filter_arg_t *self = (luab_accept_filter_arg_t *)ud;

    (void)memmove(&self->ud_af, arg, sizeof(self->ud_af));
}

static void *
accept_filter_arg_udata(lua_State *L, int narg)
{
    return (luab_to_accept_filter_arg(L, narg));
}

luab_module_t accept_filter_arg_type = {
    .cookie = LUABSD_ACCEPT_FILTER_ARG_TYPE_ID,
    .name = LUABSD_ACCEPT_FILTER_ARG_TYPE,
    .vec = accept_filter_arg_methods,
    .create = accept_filter_arg_create,
    .init = accept_filter_arg_init,
    .get = accept_filter_arg_udata,
    .sz = sizeof(luab_accept_filter_arg_t),
};
#endif /* __BSD_VISIBLE */
