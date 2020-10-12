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
#include "luab_udata.h"

#if __BSD_VISIBLE
extern luab_module_t cap_rbuf_type;

/*
 * Interface against
 *
 *  struct iovec {
 *      void    *iov_base;
 *      size_t   iov_len;
 *  };
 *
 * maps to
 *
 *  typedef struct luab_cap_rbuf {
 *      luab_udata_t    ud_softc;
 *      struct iovec    iov;
 *  } luab_cap_rbuf_t;
 */

typedef struct luab_cap_rbuf {
    luab_udata_t    ud_softc;
    struct iovec    ud_iov;
} luab_cap_rbuf_t;

#define luab_new_cap(L, arg) \
    ((luab_cap_rbuf_t *)luab_newudata(L, &cap_rbuf_type, (arg)))
#define luab_to_cap(L, narg) \
    ((luab_cap_rbuf_t *)luab_toudata((L), (narg), &cap_rbuf_type))

#define LUAB_CAP_RBUF_TYPE_ID    1601143331
#define LUAB_CAP_RBUF_TYPE    "CAP_RBUF*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(CAP_RBUF)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              iov_base    = (LUA_TSTRING),
 *              iov_len     = (LUA_NUMBER),
 *          }
 *
 * @usage t = cap_rbuf:get()
 */
static int
CAP_RBUF_get(lua_State *L)
{
    struct iovec *iov;

    (void)luab_checkmaxargs(L, 1);

    iov = luab_udata(L, 1, &cap_rbuf_type, struct iovec *);

    lua_newtable(L);

    luab_setldata(L, -2, "iov_base",    iov->iov_base, iov->iov_len);
    luab_setinteger(L, -2, "iov_len",   iov->iov_len);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - returns (LUA_TNIL).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = cmsgcred:dump()
 */
static int
CAP_RBUF_dump(lua_State *L)
{
    return (luab_dump(L, 1, NULL, 0));
}

/*
 * Access functions for immutable properties.
 */

/***
 * Copy byte string from data region.
 *
 * @function iov_base
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage str [, err, msg ] = cap_rbuf:iov_base()
 */
static int
CAP_RBUF_iov_base(lua_State *L)
{
    struct iovec *iov;

    (void)luab_checkmaxargs(L, 1);

    iov = luab_udata(L, 1, &cap_rbuf_type, struct iovec *);

    return (luab_iov_pushdata(L, iov));
}

/***
 * Get length of stored data from data region.
 *
 * @function iov_len
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage len [, err, msg ] = cap_rbuf:iov_len()
 */
static int
CAP_RBUF_iov_len(lua_State *L)
{
    struct iovec *iov;

    (void)luab_checkmaxargs(L, 1);

    iov = luab_udata(L, 1, &cap_rbuf_type, struct iovec *);

    return (luab_iov_pushlen(L, iov));
}

/*
 * Meta-methods
 */

static int
CAP_RBUF_gc(lua_State *L)
{
    struct iovec *iov;

    (void)luab_checkmaxargs(L, 1);

    iov = luab_udata(L, 1, &cap_rbuf_type, struct iovec *);
    (void)luab_iov_free(iov);

    return (luab_gc(L, 1, &cap_rbuf_type));
}

static int
CAP_RBUF_len(lua_State *L)
{
    return (luab_len(L, 2, &cap_rbuf_type));
}

static int
CAP_RBUF_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &cap_rbuf_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t cap_rbuf_methods[] = {
    LUAB_FUNC("iov_base",       CAP_RBUF_iov_base),
    LUAB_FUNC("iov_len",        CAP_RBUF_iov_len),
    LUAB_FUNC("get",            CAP_RBUF_get),
    LUAB_FUNC("dump",           CAP_RBUF_dump),
    LUAB_FUNC("__gc",           CAP_RBUF_gc),
    LUAB_FUNC("__len",          CAP_RBUF_len),
    LUAB_FUNC("__tostring",     CAP_RBUF_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
cap_rbuf_create(lua_State *L, void *arg)
{
    return (luab_new_cap(L, arg));
}

static void
cap_rbuf_init(void *ud, void *arg)
{
    luab_udata_init(&cap_rbuf_type, ud, arg);
}

static void *
cap_rbuf_udata(lua_State *L, int narg)
{
    return (luab_to_cap(L, narg));
}

luab_module_t cap_rbuf_type = {
    .m_cookie   = LUAB_CAP_RBUF_TYPE_ID,
    .m_name     = LUAB_CAP_RBUF_TYPE,
    .m_vec      = cap_rbuf_methods,
    .m_create   = cap_rbuf_create,
    .m_init     = cap_rbuf_init,
    .m_get      = cap_rbuf_udata,
    .m_sz       = sizeof(luab_cap_rbuf_t),
};
#endif /* __BSD_VISIBLE */
