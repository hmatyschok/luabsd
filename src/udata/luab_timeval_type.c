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
#include "luab_types.h"

extern luab_module_t timeval_type;

/*
 * Interface against
 *
 *  struct timeval {
 *      time_t      tv_sec;
 *      suseconds_t tv_usec;
 *  };
 */

typedef struct luab_timeval {
    luab_udata_t        ud_softc;
    struct timeval      ud_tv;
} luab_timeval_t;

#define luab_new_timeval(L, arg) \
    ((luab_timeval_t *)luab_newudata(L, &timeval_type, (arg)))
#define luab_to_timeval(L, narg) \
    (luab_toldata((L), (narg), &timeval_type, \
        struct timeval *, sizeof(struct timeval)))

#define LUAB_TIMEVAL_TYPE_ID    1599788349
#define LUAB_TIMEVAL_TYPE    "TIMEVAL*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(TIMEVAL)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              tv_sec  = (LUA_TNUMBER),
 *              tv_usec = (LUA_TNUMBER),
 *          }
 *
 * @usage t = timeval:get()
 */
static int
TIMEVAL_get(lua_State *L)
{
    struct timeval *tv;

    (void)luab_checkmaxargs(L, 1);

    tv = luab_udata(L, 1, &timeval_type, struct timeval *);

    lua_newtable(L);
    luab_setinteger(L, -2, "tv_sec",    tv->tv_sec);
    luab_setinteger(L, -2, "tv_usec",   tv->tv_usec);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate timeval{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = timeval:dump()
 */
static int
TIMEVAL_dump(lua_State *L)
{
    return (luab_dump(L, 1, &timeval_type, sizeof(struct timeval)));
}

/*
 * Accessor.
 */

/***
 * Set value for tv_sec.
 *
 * @function set_tv_sec
 *
 * @param data              Specifies value in seconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = timeval:set_tv_sec(data)
 */
static int
TIMEVAL_set_tv_sec(lua_State *L)
{
    struct timeval *tv;
    time_t data;

    (void)luab_checkmaxargs(L, 2);

    tv = luab_udata(L, 1, &timeval_type, struct timeval *);
    data = (time_t)luab_checkinteger(L, 2, INT_MAX);

    tv->tv_sec = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for tv_sec.
 *
 * @function get_tv_sec
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = timeval:get_tv_sec()
 */
static int
TIMEVAL_get_tv_sec(lua_State *L)
{
    struct timeval *tv;
    time_t data;

    (void)luab_checkmaxargs(L, 1);

    tv = luab_udata(L, 1, &timeval_type, struct timeval *);
    data = tv->tv_sec;

    return (luab_pusherr(L, data));
}

/***
 * Set value for tv_usec.
 *
 * @function set_tv_usec
 *
 * @param data              Specifies value in nanoneconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = timeval:set_tv_usec(data)
 */
static int
TIMEVAL_set_tv_usec(lua_State *L)
{
    struct timeval *tv;
    long data;

    (void)luab_checkmaxargs(L, 2);

    tv = luab_udata(L, 1, &timeval_type, struct timeval *);
    data = (long)luab_checkinteger(L, 2, LONG_MAX);

    tv->tv_usec = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for tv_usec.
 *
 * @function get_tv_usec
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = timeval:get_tv_usec()
 */
static int
TIMEVAL_get_tv_usec(lua_State *L)
{
    struct timeval *tv;
    long data;

    (void)luab_checkmaxargs(L, 1);

    tv = luab_udata(L, 1, &timeval_type, struct timeval *);
    data = tv->tv_usec;

    return (luab_pusherr(L, data));
}

/*
 * Metamethods.
 */

static int
TIMEVAL_gc(lua_State *L)
{
    return (luab_gc(L, 1, &timeval_type));
}

static int
TIMEVAL_len(lua_State *L)
{
    return (luab_len(L, 2, &timeval_type));
}

static int
TIMEVAL_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &timeval_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t timeval_methods[] = {
    LUAB_FUNC("set_tv_sec",   TIMEVAL_set_tv_sec),
    LUAB_FUNC("set_tv_usec",  TIMEVAL_set_tv_usec),
    LUAB_FUNC("get",          TIMEVAL_get),
    LUAB_FUNC("get_tv_sec",   TIMEVAL_get_tv_sec),
    LUAB_FUNC("get_tv_usec",  TIMEVAL_get_tv_usec),
    LUAB_FUNC("dump",         TIMEVAL_dump),
    LUAB_FUNC("__gc",         TIMEVAL_gc),
    LUAB_FUNC("__len",        TIMEVAL_len),
    LUAB_FUNC("__tostring",   TIMEVAL_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
timeval_create(lua_State *L, void *arg)
{
    return (luab_new_timeval(L, arg));
}

static void
timeval_init(void *ud, void *arg)
{
    luab_timeval_t *self;

    if (((self = (luab_timeval_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->ud_tv, arg, sizeof(self->ud_tv));
}

static void *
timeval_udata(lua_State *L, int narg)
{
    return (luab_to_timeval(L, narg));
}

luab_module_t timeval_type = {
    .cookie = LUAB_TIMEVAL_TYPE_ID,
    .name = LUAB_TIMEVAL_TYPE,
    .vec = timeval_methods,
    .create = timeval_create,
    .init = timeval_init,
    .get = timeval_udata,
    .sz = sizeof(luab_timeval_t),
};
