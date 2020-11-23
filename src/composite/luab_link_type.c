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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#if LUAB_DEBUG
extern luab_module_t luab_link_type;

typedef struct link {
    void            *link_dp;
    struct sockaddr *link_sa;
    struct iovec    *link_iov;
} link_t;

/*
 * Interface against
 *
 *  typedef struct link {
 *      void            *link_dp;
 *      struct sockaddr *link_sa;
 *      struct iovec    *link_iov;
 *  } link_t;
 *
 * implements test-case for linkage.
 */

typedef struct luab_link {
    luab_udata_t    ud_softc;
    link_t           ud_link;
} luab_link_t;

#define luab_new_link(L, arg) \
    ((luab_link_t *)luab_newudata(L, &luab_link_type, (arg)))
#define luab_to_link(L, narg) \
    ((link_t *)luab_toudata((L), (narg), &luab_link_type))

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(LINK)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              link_dp = (LUA_TSTRING),
 *              link_sa = (LUA_TSTRING),
 *          }
 *
 * @usage t = link:get()
 */
static int
LINK_get(lua_State *L)
{
    link_t *link;

    (void)luab_core_checkmaxargs(L, 1);

    link = luab_udata(L, 1, &luab_link_type, link_t *);

    lua_newtable(L);
    luab_setfstring(L, -2, "link_dp", "(%p)", link->link_dp);
    luab_setudata(L, -2, luab_xmod(SOCKADDR, TYPE, __func__), "link_sa", link->link_sa);
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
LINK_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Link objects in between together.
 *
 * @function set_ptr
 *
 * @param data              Refferred object.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = link:set_ptr(data)
 */
static int
LINK_set_ptr(lua_State *L)
{
    luab_udata_t *udx;
    link_t *link;
    void **dp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    link = (link_t *)luab_checkxdata(L, 1, &luab_link_type, &udx);
    dp = luab_dptox(link->link_dp);

    if (luab_udata_xlink(L, 2, udx, dp) != NULL)
        status = 0;
    else
        status = -1;

    return (luab_pushxinteger(L, status));
}

/***
 * Get base address of referred object.
 *
 * @function get_ptr
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage base [, err, msg ] = link:get_ptr()
 */
static int
LINK_get_ptr(lua_State *L)
{
    link_t *link;
    void *dp;

    (void)luab_core_checkmaxargs(L, 1);

    link = luab_udata(L, 1, &luab_link_type, link_t *);
    dp = link->link_dp;

    return (luab_pushfstring(L, "(%p)", dp));
}

/***
 * Set reference to a sockaddr{}.
 *
 * @function set_sockaddr
 *
 * @param data              Refferred object.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = link:set_sockaddr(data)
 */
static int
LINK_set_sockaddr(lua_State *L)
{
    luab_udata_t *udx;
    link_t *link;
    void **dp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    link = (link_t *)luab_checkxdata(L, 1, &luab_link_type, &udx);
    dp = luab_dptox(link->link_sa);

    if (luab_udata_checkxlink(L, 2, luab_xmod(SOCKADDR, TYPE, __func__), udx, dp) != NULL)
        status = 0;
    else
        status = -1;

    return (luab_pushxinteger(L, status));
}

/***
 * Get base address of referred sockaddr{}.
 *
 * @function get_sockaddr
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage base [, err, msg ] = link:get_sockaddr()
 */
static int
LINK_get_sockaddr(lua_State *L)
{
    link_t *link;
    void *dp;

    (void)luab_core_checkmaxargs(L, 1);

    link = luab_udata(L, 1, &luab_link_type, link_t *);
    dp = link->link_sa;

    return (luab_pushfstring(L, "(%p)", dp));
}

/*
 * Metamethods.
 */

static int
LINK_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_link_type));
}

static int
LINK_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_link_type));
}

static int
LINK_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_link_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t link_methods[] = {
    LUAB_FUNC("set_ptr",        LINK_set_ptr),
    LUAB_FUNC("set_sockaddr",   LINK_set_sockaddr),
    LUAB_FUNC("get",            LINK_get),
    LUAB_FUNC("get_ptr",        LINK_get_ptr),
    LUAB_FUNC("get_sockaddr",   LINK_get_sockaddr),
    LUAB_FUNC("dump",           LINK_dump),
    LUAB_FUNC("__gc",           LINK_gc),
    LUAB_FUNC("__len",          LINK_len),
    LUAB_FUNC("__tostring",     LINK_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
link_create(lua_State *L, void *arg)
{
    return (luab_new_link(L, arg));
}

static void
link_init(void *ud, void *arg)
{
    luab_udata_init(&luab_link_type, ud, arg);
}

static void *
link_udata(lua_State *L, int narg)
{
    return (luab_to_link(L, narg));
}

luab_module_t luab_link_type = {
    .m_cookie   = LUAB_LINK_TYPE_ID,
    .m_name     = LUAB_LINK_TYPE,
    .m_vec      = link_methods,
    .m_create   = link_create,
    .m_init     = link_init,
    .m_get      = link_udata,
    .m_sz       = sizeof(luab_link_t),
};
#endif /* LUAB_DEBUG */
