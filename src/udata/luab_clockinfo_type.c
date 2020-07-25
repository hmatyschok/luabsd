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
    struct clockinfo    clockinfo;
} luab_clockinfo_t;

#define luab_new_clockinfo(L, arg) \
    ((luab_clockinfo_t *)luab_newuserdata(L, &clockinfo_type, (arg)))
#define luab_to_clockinfo(L, narg) \
    (luab_todata((L), (narg), &clockinfo_type, luab_clockinfo_t *))

#define LUABSD_CLOCKINFO_TYPE_ID    1594164272
#define LUABSD_CLOCKINFO_TYPE    "CLOCKINFO*"

int luab_StructClockInfo(lua_State *L);

/***
 * Set clock frequency.
 *
 * @function set_hz
 *
 * @param hz            Frequency.
 *
 * @usage clockinfo:set_hz(hz)
 */
static int
ClockInfo_set_hz(lua_State *L)
{
    luab_clockinfo_t *self;
    int hz;

    luab_checkmaxargs(L, 2);

    self = luab_to_clockinfo(L, 1);
    hz = luab_checkinteger(L, 2, INT_MAX);

    self->clockinfo.hz = hz;

    return 0;
}

/***
 * Get clock frequency.
 *
 * @function get_hz
 *
 * @return (LUA_TNUMBER)
 *
 * @usage hz = clockinfo:get_hz()
 */
static int
ClockInfo_get_hz(lua_State *L)
{
    luab_clockinfo_t *self;
    int hz;

    luab_checkmaxargs(L, 1);

    self = luab_to_clockinfo(L, 1);
    hz = self->clockinfo.hz;

    lua_pushinteger(L, hz);

    return 1;
}

/***
 * Set micro-seconds per hz tick.
 *
 * @function set_tick
 *
 * @param tick            Tick.
 *
 * @usage clockinfo:set_tick(tick)
 */
static int
ClockInfo_set_tick(lua_State *L)
{
    luab_clockinfo_t *self;
    int tick;

    luab_checkmaxargs(L, 2);

    self = luab_to_clockinfo(L, 1);
    tick = luab_checkinteger(L, 2, INT_MAX);

    self->clockinfo.tick = tick;

    return 0;
}

/***
 * Get micro-seconds per hz tick.
 *
 * @function get_tick
 *
 * @return (LUA_TNUMBER)
 *
 * @usage tick = clockinfo:get_tick()
 */
static int
ClockInfo_get_tick(lua_State *L)
{
    luab_clockinfo_t *self;
    int tick;

    luab_checkmaxargs(L, 1);

    self = luab_to_clockinfo(L, 1);
    tick = self->clockinfo.tick;

    lua_pushinteger(L, tick);

    return 1;
}

/***
 * Set statistics clock frequency.
 *
 * @function set_stathz
 *
 * @param stathz            Frequency.
 *
 * @usage clockinfo:set_stathz(stathz)
 */
static int
ClockInfo_set_stathz(lua_State *L)
{
    luab_clockinfo_t *self;
    int stathz;

    luab_checkmaxargs(L, 2);

    self = luab_to_clockinfo(L, 1);
    stathz = luab_checkinteger(L, 2, INT_MAX);

    self->clockinfo.stathz = stathz;

    return 0;
}

/***
 * Get statistics clock frequency.
 *
 * @function get_stathz
 *
 * @return (LUA_TNUMBER)
 *
 * @usage stathz = clockinfo:get_stathz()
 */
static int
ClockInfo_get_stathz(lua_State *L)
{
    luab_clockinfo_t *self;
    int stathz;

    luab_checkmaxargs(L, 1);

    self = luab_to_clockinfo(L, 1);
    stathz = self->clockinfo.stathz;

    lua_pushinteger(L, stathz);

    return 1;
}

/***
 * Set profiling clock frequency.
 *
 * @function set_profhz
 *
 * @param profhz            Frequency.
 *
 * @usage clockinfo:set_profhz(profhz)
 */
static int
ClockInfo_set_profhz(lua_State *L)
{
    luab_clockinfo_t *self;
    int profhz;

    luab_checkmaxargs(L, 2);

    self = luab_to_clockinfo(L, 1);
    profhz = luab_checkinteger(L, 2, INT_MAX);

    self->clockinfo.profhz = profhz;

    return 0;
}

/***
 * Get profiling clock frequency.
 *
 * @function get_profhz
 *
 * @return (LUA_TNUMBER)
 *
 * @usage profhz = clockinfo:get_profhz()
 */
static int
ClockInfo_get_profhz(lua_State *L)
{
    luab_clockinfo_t *self;
    int profhz;

    luab_checkmaxargs(L, 1);

    self = luab_to_clockinfo(L, 1);
    profhz = self->clockinfo.profhz;

    lua_pushinteger(L, profhz);

    return 1;
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
    luab_clockinfo_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_to_clockinfo(L, 1);

    lua_newtable(L);

    luab_setinteger(L, -2, "hz", self->clockinfo.hz);
    luab_setinteger(L, -2, "tick", self->clockinfo.tick);
    luab_setinteger(L, -2, "stathz", self->clockinfo.stathz);
    luab_setinteger(L, -2, "profhz", self->clockinfo.stathz);

    lua_pushvalue(L, -1);

    return 1;
}

static int
ClockInfo_gc(lua_State *L)
{
    luab_clockinfo_t *self;
    luab_module_t *m;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_clockinfo(L, 1);
    m = &clockinfo_type;

    (void)memset_s(self, m->sz, 0, m->sz);

    return 0;
}

static int
ClockInfo_tostring(lua_State *L)
{
    luab_clockinfo_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_clockinfo(L, 1);
    lua_pushfstring(L, "clockinfo (%p)", self);

    return 1;
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
    LUABSD_FUNC("__gc", ClockInfo_gc),
    LUABSD_FUNC("__tostring",   ClockInfo_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void
clockinfo_init(void *ud, void *arg)
{
    luab_clockinfo_t *self = (luab_clockinfo_t *)ud;

    (void)memmove(&self->clockinfo, arg, sizeof(self->clockinfo));
}

static void *
clockinfo_udata(lua_State *L, int narg)
{
    luab_clockinfo_t *self = luab_to_clockinfo(L, narg);

    return (&self->clockinfo);
}

luab_module_t clockinfo_type = {
    .cookie = LUABSD_CLOCKINFO_TYPE_ID,
    .name = LUABSD_CLOCKINFO_TYPE,
    .vec = clockinfo_methods,
    .init = clockinfo_init,
    .get = clockinfo_udata,
    .sz = sizeof(luab_clockinfo_t),
};

/***
 * Ctor.
 *
 * @function StructClockInfo
 *
 * @param ci                    Optional.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage ci = bsd.sys.time.StructClockInfo([ ci ])
 */
int
luab_StructClockInfo(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 1);
    struct clockinfo *clockinfo;
    int status;

    if (narg == 0)
        clockinfo = NULL;
    else
        clockinfo = clockinfo_udata(L, narg);

    if (luab_new_clockinfo(L, clockinfo) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}