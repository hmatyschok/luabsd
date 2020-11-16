/*
 * Copyright (c) 2020 Henning Matyschok
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

#include <sys/mount.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab___sbuf_type;

/*
 * Interface against
 *
 *  struct __sbuf {
 *      unsigned char *_base;
 *      int _size;
 *  };
 *
 */

typedef struct luab___sbuf {
    luab_udata_t        ud_softc;
    struct __sbuf       ud_sb;
} luab___sbuf_type_t;

#define luab_new___sbuf(L, arg) \
    ((luab___sbuf_type_t *)luab_newudata(L, &luab___sbuf_type, (arg)))
#define luab_to___sbuf(L, narg) \
    (luab_toldata((L), (narg), &luab___sbuf_type, \
        struct __sbuf *, sizeof(struct __sbuf)))

#define LUAB___SBUF_TYPE_ID    1605302940
#define LUAB___SBUF_TYPE    "__SBUF*"


/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(__SBUF)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              _base       = (LUA_TSTRING),
 *              _size       = (LUA_TNUMBER),
 *          }
 *
 * @usage t = __sbuf:get()
 */
static int
SBUF_get(lua_State *L)
{
    struct __sbuf *sb;

    (void)luab_core_checkmaxargs(L, 1);

    sb = luab_udata(L, 1, &luab___sbuf_type, struct __sbuf *);

    lua_newtable(L);
    luab_setldata(L, -2, "_base",      sb->_base, sb->_size);
    luab_setinteger(L, -2, "_size",    sb->_size);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate __sbuf{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = __sbuf:dump()
 */
static int
SBUF_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab___sbuf_type, sizeof(struct __sbuf)));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get contents of data region _base.
 *
 * @function _base
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = __sbuf:_base()
 */
static int
SBUF_base(lua_State *L)
{
    struct __sbuf *sb;
    caddr_t dp;
    size_t sz;

    (void)luab_core_checkmaxargs(L, 1);

    sb = luab_udata(L, 1, &luab___sbuf_type, struct __sbuf *);
    dp = sb->_base;
    sz = sb->_size;

    return (luab_pushldata(L, dp, sz));
}

/***
 * Get size of data region.
 *
 * @function _size
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = __sbuf:_size()
 */
static int
SBUF_size(lua_State *L)
{
    struct __sbuf *sb;
    size_t sz;

    (void)luab_core_checkmaxargs(L, 1);

    sb = luab_udata(L, 1, &luab___sbuf_type, struct __sbuf *);
    sz = sb->_size;

    return (luab_pushxinteger(L, sz));
}

/*
 * Meta-methods.
 */

static int
SBUF_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab___sbuf_type));
}

static int
SBUF_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab___sbuf_type));
}

static int
SBUF_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab___sbuf_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t sbuf_methods[] = {
    LUAB_FUNC("_base",          SBUF_base),
    LUAB_FUNC("_size",          SBUF_size),
    LUAB_FUNC("get",            SBUF_get),
    LUAB_FUNC("dump",           SBUF_dump),
    LUAB_FUNC("__gc",           SBUF_gc),
    LUAB_FUNC("__len",          SBUF_len),
    LUAB_FUNC("__tostring",     SBUF_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
sbuf_create(lua_State *L, void *arg)
{
    return (luab_new___sbuf(L, arg));
}

static void
sbuf_init(void *ud, void *arg)
{
    luab_udata_init(&luab___sbuf_type, ud, arg);
}

static void *
sbuf_udata(lua_State *L, int narg)
{
    return (luab_to___sbuf(L, narg));
}

luab_module_t luab___sbuf_type = {
    .m_cookie   = LUAB___SBUF_TYPE_ID,
    .m_name     = LUAB___SBUF_TYPE,
    .m_vec      = sbuf_methods,
    .m_create   = sbuf_create,
    .m_init     = sbuf_init,
    .m_get      = sbuf_udata,
    .m_sz       = sizeof(luab___sbuf_type_t),
};
