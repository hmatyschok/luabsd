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
    struct clockinfo    ud_info;
} luab_clockinfo_t;

#define luab_new_clockinfo(L, arg) \
    ((luab_clockinfo_t *)luab_newudata(L, &clockinfo_type, (arg)))
#define luab_to_clockinfo(L, narg) \
    (luab_toldata((L), (narg), &clockinfo_type, \
        struct clockinfo *, sizeof(struct clockinfo)))

#define LUAB_CLOCKINFO_TYPE_ID    1594164272
#define LUAB_CLOCKINFO_TYPE    "CLOCKINFO*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(CLOCKINFO)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              hz      = (LUA_TNUMBER),
 *              tick    = (LUA_TNUMBER),
 *              spare   = (LUA_TNUMBER),
 *              stathz  = (LUA_TNUMBER),
 *              profhz  = (LUA_TNUMBER),
 *          }
 *
 * @usage t = clockinfo:get()
 */
static int
CLOCKINFO_get(lua_State *L)
{
    struct clockinfo *ci;

    (void)luab_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, &clockinfo_type, struct clockinfo *);

    lua_newtable(L);

    luab_setinteger(L, -2, "hz",        ci->hz);
    luab_setinteger(L, -2, "tick",      ci->tick);
    luab_setinteger(L, -2, "stathz",    ci->stathz);
    luab_setinteger(L, -2, "profhz",    ci->stathz);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate clockinfo{} into (LUA_TUSERDATA(IOVEC)).
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
CLOCKINFO_dump(lua_State *L)
{
    return (luab_dump(L, 1, &clockinfo_type, sizeof(struct clockinfo)));
}

/*
 * Accessor.
 */

/***
 * Set clock frequency.
 *
 * @function set_hz
 *
 * @param data              Frequency.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = clockinfo:set_hz(data)
 */
static int
CLOCKINFO_set_hz(lua_State *L)
{
    struct clockinfo *ci;
    int data;

    (void)luab_checkmaxargs(L, 2);

    ci = luab_udata(L, 1, &clockinfo_type, struct clockinfo *);
    data = (int)luab_checkinteger(L, 2, INT_MAX);

    ci->hz = data;

    return (luab_pusherr(L, data));
}

/***
 * Get clock frequency.
 *
 * @function get_hz
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = clockinfo:get_hz()
 */
static int
CLOCKINFO_get_hz(lua_State *L)
{
    struct clockinfo *ci;
    int data;

    (void)luab_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, &clockinfo_type, struct clockinfo *);
    data = ci->hz;

    return (luab_pusherr(L, data));
}

/***
 * Set micro-seconds per hz tick.
 *
 * @function set_tick
 *
 * @param data              Tick.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = clockinfo:set_tick(data)
 */
static int
CLOCKINFO_set_tick(lua_State *L)
{
    struct clockinfo *ci;
    int data;

    (void)luab_checkmaxargs(L, 2);

    ci = luab_udata(L, 1, &clockinfo_type, struct clockinfo *);
    data = (int)luab_checkinteger(L, 2, INT_MAX);

    ci->tick = data;

    return (luab_pusherr(L, data));
}

/***
 * Get micro-seconds per hz tick.
 *
 * @function get_tick
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = clockinfo:get_tick()
 */
static int
CLOCKINFO_get_tick(lua_State *L)
{
    struct clockinfo *ci;
    int data;

    (void)luab_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, &clockinfo_type, struct clockinfo *);
    data = ci->tick;

    return (luab_pusherr(L, data));
}

/***
 * Set statistics clock frequency.
 *
 * @function set_stathz
 *
 * @param data              Frequency.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = clockinfo:set_stathz(data)
 */
static int
CLOCKINFO_set_stathz(lua_State *L)
{
    struct clockinfo *ci;
    int data;

    (void)luab_checkmaxargs(L, 2);

    ci = luab_udata(L, 1, &clockinfo_type, struct clockinfo *);
    data = (int)luab_checkinteger(L, 2, INT_MAX);

    ci->stathz = data;

    return (luab_pusherr(L, data));
}

/***
 * Get statistics clock frequency.
 *
 * @function get_stathz
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = clockinfo:get_stathz()
 */
static int
CLOCKINFO_get_stathz(lua_State *L)
{
    struct clockinfo *ci;
    int data;

    (void)luab_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, &clockinfo_type, struct clockinfo *);
    data = ci->stathz;

    return (luab_pusherr(L, data));
}

/***
 * Set profiling clock frequency.
 *
 * @function set_profhz
 *
 * @param data              Frequency.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = clockinfo:set_profhz(data)
 */
static int
CLOCKINFO_set_profhz(lua_State *L)
{
    struct clockinfo *ci;
    int data;

    (void)luab_checkmaxargs(L, 2);

    ci = luab_udata(L, 1, &clockinfo_type, struct clockinfo *);
    data = (int)luab_checkinteger(L, 2, INT_MAX);

    ci->profhz = data;

    return (luab_pusherr(L, data));
}

/***
 * Get profiling clock frequency.
 *
 * @function get_profhz
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = clockinfo:get_profhz()
 */
static int
CLOCKINFO_get_profhz(lua_State *L)
{
    struct clockinfo *ci;
    int data;

    (void)luab_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, &clockinfo_type, struct clockinfo *);
    data = ci->profhz;

    return (luab_pusherr(L, data));
}

/*
 * Metamethods.
 */

static int
CLOCKINFO_gc(lua_State *L)
{
    return (luab_gc(L, 1, &clockinfo_type));
}

static int
CLOCKINFO_len(lua_State *L)
{
    return (luab_len(L, 2, &clockinfo_type));
}

static int
CLOCKINFO_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &clockinfo_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t clockinfo_methods[] = {
    LUAB_FUNC("set_hz",       CLOCKINFO_set_hz),
    LUAB_FUNC("set_tickc",    CLOCKINFO_set_tick),
    LUAB_FUNC("set_stathz",   CLOCKINFO_set_stathz),
    LUAB_FUNC("set_profhz",   CLOCKINFO_set_profhz),
    LUAB_FUNC("get",          CLOCKINFO_get),
    LUAB_FUNC("get_hz",       CLOCKINFO_get_hz),
    LUAB_FUNC("get_tick",     CLOCKINFO_get_tick),
    LUAB_FUNC("get_stathz",   CLOCKINFO_get_stathz),
    LUAB_FUNC("get_profhz",   CLOCKINFO_get_profhz),
    LUAB_FUNC("dump",         CLOCKINFO_dump),
    LUAB_FUNC("__gc",         CLOCKINFO_gc),
    LUAB_FUNC("__len",        CLOCKINFO_len),
    LUAB_FUNC("__tostring",   CLOCKINFO_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
clockinfo_create(lua_State *L, void *arg)
{
    return (luab_new_clockinfo(L, arg));
}

static void
clockinfo_init(void *ud, void *arg)
{
    luab_udata_init(&clockinfo_type, ud, arg);
}

static void *
clockinfo_udata(lua_State *L, int narg)
{
    return (luab_to_clockinfo(L, narg));
}

luab_module_t clockinfo_type = {
    .m_cookie   = LUAB_CLOCKINFO_TYPE_ID,
    .m_name     = LUAB_CLOCKINFO_TYPE,
    .m_vec      = clockinfo_methods,
    .m_create   = clockinfo_create,
    .m_init     = clockinfo_init,
    .m_get      = clockinfo_udata,
    .m_sz       = sizeof(luab_clockinfo_t),
};
