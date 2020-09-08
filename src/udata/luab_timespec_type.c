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

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t timespec_type;

/*
 * Interface against
 *
 *  struct timespec {
 *      time_t  tv_sec;
 *      long    tv_nsec;
 *  };
 */

typedef struct luab_timespec {
    luab_udata_t        ud_softc;
    struct timespec    timespec;
} luab_timespec_t;

#define luab_new_timespec(L, arg) \
    ((luab_timespec_t *)luab_newuserdata(L, &timespec_type, (arg)))
#define luab_to_timespec(L, narg) \
    (luab_toldata((L), (narg), &timespec_type, \
        struct timespec *, sizeof(struct timespec)))

#define LUABSD_TIMESPEC_TYPE_ID    1594034844
#define LUABSD_TIMESPEC_TYPE    "TIMESPEC*"

int luab_timespec_create(lua_State *);

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
 * @usage data [, err, msg ] = timespec:set_tv_sec(data)
 */
static int
TIMESPEC_set_tv_sec(lua_State *L)
{
    struct timespec *tv;
    time_t data;

    (void)luab_checkmaxargs(L, 2);

    tv = luab_udata(L, 1, timespec_type, struct timespec *);
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
 * @usage data [, err, msg ] = timespec:get_tv_sec()
 */
static int
TIMESPEC_get_tv_sec(lua_State *L)
{
    struct timespec *tv;
    time_t data;

    (void)luab_checkmaxargs(L, 1);

    tv = luab_udata(L, 1, timespec_type, struct timespec *);
    data = tv->tv_sec;

    return (luab_pusherr(L, data));
}

/***
 * Set value for tv_nsec.
 *
 * @function set_tv_nsec
 *
 * @param data              Specifies value in nanoneconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = timespec:set_tv_nsec(data)
 */
static int
TIMESPEC_set_tv_nsec(lua_State *L)
{
    struct timespec *tv;
    long data;

    (void)luab_checkmaxargs(L, 2);

    tv = luab_udata(L, 1, timespec_type, struct timespec *);
    data = (long)luab_checkinteger(L, 2, LONG_MAX);

    tv->tv_nsec = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for tv_nsec.
 *
 * @function get_tv_nsec
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = timespec:get_tv_nsec()
 */
static int
TIMESPEC_get_tv_nsec(lua_State *L)
{
    struct timespec *tv;
    long data;

    (void)luab_checkmaxargs(L, 1);

    tv = luab_udata(L, 1, timespec_type, struct timespec *);
    data = tv->tv_nsec;

    return (luab_pusherr(L, data));
}

/***
 * Translate timespec{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = timespec:get()
 */
static int
TIMESPEC_get(lua_State *L)
{
    struct timespec *tv;

    (void)luab_checkmaxargs(L, 1);

    tv = luab_udata(L, 1, timespec_type, struct timespec *);

    lua_newtable(L);
    luab_setinteger(L, -2, "tv_sec", tv->tv_sec);
    luab_setinteger(L, -2, "tv_nsec", tv->tv_nsec);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Copy timespec{} into LUA_TUSERDATA(luab_iovec_t).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = timespec:dump()
 */
static int
TIMESPEC_dump(lua_State *L)
{
    return (luab_dump(L, 1, &timespec_type, sizeof(struct timespec)));
}

static int
TIMESPEC_gc(lua_State *L)
{
    return (luab_gc(L, 1, &timespec_type));
}

static int
TIMESPEC_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &timespec_type));
}

static luab_table_t timespec_methods[] = {
    LUABSD_FUNC("set_tv_sec",   TIMESPEC_set_tv_sec),
    LUABSD_FUNC("set_tv_nsec",  TIMESPEC_set_tv_nsec),
    LUABSD_FUNC("get",  TIMESPEC_get),
    LUABSD_FUNC("get_tv_sec",   TIMESPEC_get_tv_sec),
    LUABSD_FUNC("get_tv_nsec",  TIMESPEC_get_tv_nsec),
    LUABSD_FUNC("dump", TIMESPEC_dump),
    LUABSD_FUNC("__gc", TIMESPEC_gc),
    LUABSD_FUNC("__tostring",   TIMESPEC_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
timespec_create(lua_State *L, void *arg)
{
    return (luab_new_timespec(L, arg));
}

static void
timespec_init(void *ud, void *arg)
{
    luab_timespec_t *self;

    if (((self = (luab_timespec_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->timespec, arg, sizeof(self->timespec));
}

static void *
timespec_udata(lua_State *L, int narg)
{
    return (luab_to_timespec(L, narg));
}

luab_module_t timespec_type = {
    .cookie = LUABSD_TIMESPEC_TYPE_ID,
    .name = LUABSD_TIMESPEC_TYPE,
    .vec = timespec_methods,
    .create = timespec_create,
    .init = timespec_init,
    .get = timespec_udata,
    .sz = sizeof(luab_timespec_t),
};

/***
 * Ctor.
 *
 * @function timespec_create
 *
 * @param timespec                    Instance of LUA_TUSERDATA(luab_timespec_t).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage timespec [, err, msg ] = bsd.sys.time.timespec_create([ timespec ])
 */
int
luab_timespec_create(lua_State *L)
{
    struct timespec *timespec;
    int narg, status;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        timespec = NULL;
    else
        timespec = timespec_udata(L, narg);

    if (timespec_create(L, timespec) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
