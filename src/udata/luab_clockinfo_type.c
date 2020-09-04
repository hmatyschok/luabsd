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

extern luab_module_t clockinfo_type;

/*
 * Interface against getkerninfo clock information structure
 *
 *  struct clockinfo {
 *      int hz;
 *      int tick;
 *      int spare;
 *      int stathz;
 *      int profhz;
 *  };
 */

typedef struct luab_clockinfo {
    luab_udata_t        ud_softc;
    struct clockinfo    clockinfo;
} luab_clockinfo_t;

#define luab_new_clockinfo(L, arg) \
    ((luab_clockinfo_t *)luab_newuserdata(L, &clockinfo_type, (arg)))
#define luab_to_clockinfo(L, narg) \
    (luab_toldata((L), (narg), &clockinfo_type, \
        struct clockinfo *, sizeof(struct clockinfo)))

#define LUABSD_CLOCKINFO_TYPE_ID    1594164272
#define LUABSD_CLOCKINFO_TYPE    "CLOCKINFO*"

int luab_StructClockInfo(lua_State *L);

/***
 * Set clock frequency.
 *
 * @function set_hz
 *
 * @param attr              Frequency.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = clockinfo:set_hz(attr)
 */
static int
ClockInfo_set_hz(lua_State *L)
{
    struct clockinfo *ci;
    int hz;

    (void)luab_checkmaxargs(L, 2);

    ci = luab_udata(L, 1, clockinfo_type, struct clockinfo *);
    hz = (int)luab_checkinteger(L, 2, INT_MAX);

    ci->hz = hz;

    return (luab_pusherr(L, hz));
}

/***
 * Get clock frequency.
 *
 * @function get_hz
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = clockinfo:get_hz()
 */
static int
ClockInfo_get_hz(lua_State *L)
{
    struct clockinfo *ci;
    int hz;

    (void)luab_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, clockinfo_type, struct clockinfo *);
    hz = ci->hz;

    return (luab_pusherr(L, hz));
}

/***
 * Set micro-seconds per hz tick.
 *
 * @function set_tick
 *
 * @param attr              Tick.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = clockinfo:set_tick(attr)
 */
static int
ClockInfo_set_tick(lua_State *L)
{
    struct clockinfo *ci;
    int tick;

    (void)luab_checkmaxargs(L, 2);

    ci = luab_udata(L, 1, clockinfo_type, struct clockinfo *);
    tick = (int)luab_checkinteger(L, 2, INT_MAX);

    ci->tick = tick;

    return (luab_pusherr(L, tick));
}

/***
 * Get micro-seconds per hz tick.
 *
 * @function get_tick
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = clockinfo:get_tick()
 */
static int
ClockInfo_get_tick(lua_State *L)
{
    struct clockinfo *ci;
    int tick;

    (void)luab_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, clockinfo_type, struct clockinfo *);
    tick = ci->tick;

    return (luab_pusherr(L, tick));
}

/***
 * Set statistics clock frequency.
 *
 * @function set_stathz
 *
 * @param attr              Frequency.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = clockinfo:set_stathz(attr)
 */
static int
ClockInfo_set_stathz(lua_State *L)
{
    struct clockinfo *ci;
    int stathz;

    (void)luab_checkmaxargs(L, 2);

    ci = luab_udata(L, 1, clockinfo_type, struct clockinfo *);
    stathz = (int)luab_checkinteger(L, 2, INT_MAX);

    ci->stathz = stathz;

    return (luab_pusherr(L, stathz));
}

/***
 * Get statistics clock frequency.
 *
 * @function get_stathz
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = clockinfo:get_stathz()
 */
static int
ClockInfo_get_stathz(lua_State *L)
{
    struct clockinfo *ci;
    int stathz;

    (void)luab_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, clockinfo_type, struct clockinfo *);
    stathz = ci->stathz;

    return (luab_pusherr(L, stathz));
}

/***
 * Set profiling clock frequency.
 *
 * @function set_profhz
 *
 * @param attr              Frequency.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = clockinfo:set_profhz(attr)
 */
static int
ClockInfo_set_profhz(lua_State *L)
{
    struct clockinfo *ci;
    int profhz;

    (void)luab_checkmaxargs(L, 2);

    ci = luab_udata(L, 1, clockinfo_type, struct clockinfo *);
    profhz = (int)luab_checkinteger(L, 2, INT_MAX);

    ci->profhz = profhz;

    return (luab_pusherr(L, profhz));
}

/***
 * Get profiling clock frequency.
 *
 * @function get_profhz
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = clockinfo:get_profhz()
 */
static int
ClockInfo_get_profhz(lua_State *L)
{
    struct clockinfo *ci;
    int profhz;

    (void)luab_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, clockinfo_type, struct clockinfo *);
    profhz = ci->profhz;

    return (luab_pusherr(L, profhz));
}

/***
 * Translate clockinfo{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = clockinfo:get()
 */
static int
ClockInfo_get(lua_State *L)
{
    struct clockinfo *ci;

    (void)luab_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, clockinfo_type, struct clockinfo *);

    lua_newtable(L);

    luab_setinteger(L, -2, "hz", ci->hz);
    luab_setinteger(L, -2, "tick", ci->tick);
    luab_setinteger(L, -2, "stathz", ci->stathz);
    luab_setinteger(L, -2, "profhz", ci->stathz);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Copy clockinfo{} into LUA_TUSERDATA(luab_iovec_t).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = clockinfo:dump()
 */
static int
ClockInfo_dump(lua_State *L)
{
    return (luab_dump(L, 1, &clockinfo_type, sizeof(struct clockinfo)));
}

static int
ClockInfo_gc(lua_State *L)
{
    return (luab_gc(L, 1, &clockinfo_type));
}

static int
ClockInfo_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &clockinfo_type));
}

static luab_table_t clockinfo_methods[] = {
    LUABSD_FUNC("set_hz",   ClockInfo_set_hz),
    LUABSD_FUNC("set_tickc",    ClockInfo_set_tick),
    LUABSD_FUNC("set_stathz",   ClockInfo_set_stathz),
    LUABSD_FUNC("set_profhz",   ClockInfo_set_profhz),
    LUABSD_FUNC("get",  ClockInfo_get),
    LUABSD_FUNC("get_hz",  ClockInfo_get_hz),
    LUABSD_FUNC("get_tick", ClockInfo_get_tick),
    LUABSD_FUNC("get_stathz",   ClockInfo_get_stathz),
    LUABSD_FUNC("get_profhz",   ClockInfo_get_profhz),
    LUABSD_FUNC("dump", ClockInfo_dump),
    LUABSD_FUNC("__gc", ClockInfo_gc),
    LUABSD_FUNC("__tostring",   ClockInfo_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
clockinfo_create(lua_State *L, void *arg)
{
    return (luab_new_clockinfo(L, arg));
}

static void
clockinfo_init(void *ud, void *arg)
{
    luab_clockinfo_t *self;

    if (((self = (luab_clockinfo_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->clockinfo, arg, sizeof(self->clockinfo));
}

static void *
clockinfo_udata(lua_State *L, int narg)
{
    return (luab_to_clockinfo(L, narg));
}

luab_module_t clockinfo_type = {
    .cookie = LUABSD_CLOCKINFO_TYPE_ID,
    .name = LUABSD_CLOCKINFO_TYPE,
    .vec = clockinfo_methods,
    .ctor = clockinfo_create,
    .init = clockinfo_init,
    .get = clockinfo_udata,
    .sz = sizeof(luab_clockinfo_t),
};

/***
 * Ctor.
 *
 * @function StructClockInfo
 *
 * @param clockinfo                 Instance of LUA_TUSERDATA(luab_clockinfo_t).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (clockinfo [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage clockinfo [, err, msg ] = bsd.sys.time.StructClockInfo([ clockinfo ])
 */
int
luab_StructClockInfo(lua_State *L)
{
    struct clockinfo *clockinfo;
    int narg, status;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        clockinfo = NULL;
    else
        clockinfo = clockinfo_udata(L, narg);

    if (clockinfo_create(L, clockinfo) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
