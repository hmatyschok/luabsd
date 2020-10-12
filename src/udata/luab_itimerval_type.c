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

#include <sys/time.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

extern luab_module_t itimerval_type;

/*
 * Interface against
 *
 *  struct itimerval {
 *      struct  timeval it_interval;
 *      struct  timeval it_value;
 *  };
 *
 */

typedef struct luab_itimerval {
    luab_udata_t        ud_softc;
    struct itimerval    ud_it;
} luab_itimerval_t;

#define luab_new_itimerval(L, arg) \
    ((luab_itimerval_t *)luab_newudata(L, &itimerval_type, (arg)))
#define luab_to_itimerval(L, narg) \
    (luab_toldata((L), (narg), &itimerval_type, \
        struct itimerval *, sizeof(struct itimerval)))

#define LUAB_ITIMERVAL_TYPE_ID    1594110231
#define LUAB_ITIMERVAL_TYPE    "ITIMERVAL*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(ITIMERVAL)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              it_interval = (LUA_TUSERDATA(TIMEVAL)),
 *              it_value    = (LUA_TUSERDATA(TIMEVAL)),
 *          }
 *
 * @usage t = itimerval:get()
 */
static int
ITIMERVAL_get(lua_State *L)
{
    struct itimerval *it;

    (void)luab_checkmaxargs(L, 1);

    it = luab_udata(L, 1, &itimerval_type, struct itimerval *);

    lua_newtable(L);
    luab_setudata(L, -2, luab_mx(TIMEVAL), "it_interval",   &it->it_interval);
    luab_setudata(L, -2, luab_mx(TIMEVAL), "it_value",      &it->it_value);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate itimerval{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA(TIMEVAL)} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = itimerval:dump()
 */
static int
ITIMERVAL_dump(lua_State *L)
{
    return (luab_dump(L, 1, &itimerval_type, sizeof(struct itimerval)));
}

/*
 * Access functions.
 */

/***
 * Set timer interval.
 *
 * @function set_it_interval
 *
 * @param timeval          Instance of (LUA_TUSERDATA(TIMEVAL)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (0, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = itimerval:set_it_interval(timeval)
 */
static int
ITIMERVAL_set_it_interval(lua_State *L)
{
    struct itimerval *it;
    struct timeval *tv;

    (void)luab_checkmaxargs(L, 2);

    it = luab_udata(L, 1, &itimerval_type, struct itimerval *);
    tv = luab_udata(L, 2, luab_mx(TIMEVAL), struct timeval *);

    (void)memmove(&it->it_interval, tv, sizeof(*tv));

    return (luab_pusherr(L, 0));
}

/***
 * Get timer interval.
 *
 * @function get_it_interval
 *
 * @return (LUA_T{NIL,USERDATA(TIMEVAL)} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage timeval [, err, msg ] = itimerval:get_it_interval()
 */
static int
ITIMERVAL_get_it_interval(lua_State *L)
{
    struct itimerval *it;
    struct timeval *tv;

    (void)luab_checkmaxargs(L, 1);

    it = luab_udata(L, 1, &itimerval_type, struct itimerval *);
    tv = &(it->it_interval);

    return (luab_pushudata(L, luab_mx(TIMEVAL), tv));
}

/* current value */
/***
 * Set current value.
 *
 * @function set_it_value
 *
 * @param timeval          Instance of (LUA_TUSERDATA(TIMEVAL)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (0, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = itimerval:set_it_value(timeval)
 */
static int
ITIMERVAL_set_it_value(lua_State *L)
{
    struct itimerval *it;
    struct timeval *tv;

    (void)luab_checkmaxargs(L, 2);

    it = luab_udata(L, 1, &itimerval_type, struct itimerval *);
    tv = luab_udata(L, 2, luab_mx(TIMEVAL), struct timeval *);

    (void)memmove(&it->it_value, tv, sizeof(*tv));

    return (luab_pusherr(L, 0));
}

/***
 * Get timer value.
 *
 * @function get_it_value
 *
 * @return (LUA_T{NIL,USERDATA(TIMEVAL)} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage timeval [, err, msg ] = itimerval:get_it_value()
 */
static int
ITIMERVAL_get_it_value(lua_State *L)
{
    struct itimerval *it;
    struct timeval *tv;

    (void)luab_checkmaxargs(L, 1);

    it = luab_udata(L, 1, &itimerval_type, struct itimerval *);
    tv = &(it->it_value);

    return (luab_pushudata(L, luab_mx(TIMEVAL), tv));
}

/*
 * Metamethods.
 */

static int
ITIMERVAL_gc(lua_State *L)
{
    return (luab_gc(L, 1, &itimerval_type));
}

static int
ITIMERVAL_len(lua_State *L)
{
    return (luab_len(L, 2, &itimerval_type));
}

static int
ITIMERVAL_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &itimerval_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t itimerval_methods[] = {
    LUAB_FUNC("set_it_interval",  ITIMERVAL_set_it_interval),
    LUAB_FUNC("set_it_value",     ITIMERVAL_set_it_value),
    LUAB_FUNC("get",              ITIMERVAL_get),
    LUAB_FUNC("get_it_interval",  ITIMERVAL_get_it_interval),
    LUAB_FUNC("get_it_value",     ITIMERVAL_get_it_value),
    LUAB_FUNC("dump",             ITIMERVAL_dump),
    LUAB_FUNC("__gc",             ITIMERVAL_gc),
    LUAB_FUNC("__len",            ITIMERVAL_len),
    LUAB_FUNC("__tostring",       ITIMERVAL_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
itimerval_create(lua_State *L, void *arg)
{
    return (luab_new_itimerval(L, arg));
}

static void
itimerval_init(void *ud, void *arg)
{
    luab_udata_init(&itimerval_type, ud, arg);
}

static void *
itimerval_udata(lua_State *L, int narg)
{
    return (luab_to_itimerval(L, narg));
}

luab_module_t itimerval_type = {
    .m_cookie   = LUAB_ITIMERVAL_TYPE_ID,
    .m_name     = LUAB_ITIMERVAL_TYPE,
    .m_vec      = itimerval_methods,
    .m_create   = itimerval_create,
    .m_init     = itimerval_init,
    .m_get      = itimerval_udata,
    .m_sz       = sizeof(luab_itimerval_t),
};
