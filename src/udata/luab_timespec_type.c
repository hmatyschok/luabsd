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
    struct timespec    timespec;
} luab_timespec_t;

#define luab_new_timespec(L, arg) \
    ((luab_timespec_t *)luab_newuserdata(L, &timespec_type, (arg)))
#define luab_to_timespec(L, narg) \
    (luab_todata((L), (narg), &timespec_type, luab_timespec_t *))

#define LUABSD_TIMESPEC_TYPE_ID    1594034844
#define LUABSD_TIMESPEC_TYPE    "TIMESPEC*"

int luab_StructTimeSpec(lua_State *);

/***
 * Set value for tv_sec.
 *
 * @function set_tv_sec
 *
 * @param sec           Specifies value in seconds.
 *
 * @usage timespec:set_tv_sec(sec)
 */
static int
TimeSpec_set_tv_sec(lua_State *L)
{
    struct timespec *tv;
    time_t tv_sec;

    (void)luab_checkmaxargs(L, 2);

    tv = (struct timespec *)(*timespec_type.get)(L, 1);
    tv_sec = luab_checkinteger(L, 2, INT_MAX);

    tv->tv_sec = tv_sec;

    return 0;
}

/***
 * Get value for tv_sec.
 *
 * @function get_tv_sec
 *
 * @return (LUA_TNUMBER)
 *
 * @usage sec = timespec:get_tv_sec()
 */
static int
TimeSpec_get_tv_sec(lua_State *L)
{
    struct timespec *tv;
    time_t tv_sec;

    (void)luab_checkmaxargs(L, 1);

    tv = (struct timespec *)(*timespec_type.get)(L, 1);
    tv_sec = tv->tv_sec;

    return luab_pusherr(L, tv_sec);
}

/***
 * Set value for tv_nsec.
 *
 * @function set_tv_nsec
 *
 * @param nsec           Specifies value in nanoneconds.
 *
 * @usage timespec:set_tv_nsec(nsec)
 */
static int
TimeSpec_set_tv_nsec(lua_State *L)
{
    struct timespec *tv;
    long tv_nsec;

    (void)luab_checkmaxargs(L, 2);

    tv = (struct timespec *)(*timespec_type.get)(L, 1);
    tv_nsec = luab_checkinteger(L, 2, LONG_MAX);

    tv->tv_nsec = tv_nsec;

    return 0;
}

/***
 * Get value for tv_nsec.
 *
 * @function get_tv_nsec
 *
 * @return (LUA_TNUMBER)
 *
 * @usage nsec = timespec:get_tv_nsec()
 */
static int
TimeSpec_get_tv_nsec(lua_State *L)
{
    struct timespec *tv;
    long tv_nsec;

    (void)luab_checkmaxargs(L, 1);

    tv = (struct timespec *)(*timespec_type.get)(L, 1);
    tv_nsec = tv->tv_nsec;

    return luab_pusherr(L, tv_nsec);
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
TimeSpec_get(lua_State *L)
{
    struct timespec *tv;

    (void)luab_checkmaxargs(L, 1);

    tv = (struct timespec *)(*timespec_type.get)(L, 1);

    lua_newtable(L);

    luab_setinteger(L, -2, "tv_sec", tv->tv_sec);
    luab_setinteger(L, -2, "tv_nsec", tv->tv_nsec);

    lua_pushvalue(L, -1);

    return 1;
}

static int
TimeSpec_gc(lua_State *L)
{
    luab_timespec_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_timespec(L, 1);

    (void)memset_s(self, timespec_type.sz, 0, timespec_type.sz);

    return 0;
}

static int
TimeSpec_tostring(lua_State *L)
{
    luab_timespec_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_timespec(L, 1);
    lua_pushfstring(L, "timespec (%p)", self);

    return 1;
}

static luab_table_t timespec_methods[] = {
    LUABSD_FUNC("set_tv_sec",   TimeSpec_set_tv_sec),
    LUABSD_FUNC("set_tv_nsec",  TimeSpec_set_tv_nsec),
    LUABSD_FUNC("get",  TimeSpec_get),
    LUABSD_FUNC("get_tv_sec",   TimeSpec_get_tv_sec),
    LUABSD_FUNC("get_tv_nsec",  TimeSpec_get_tv_nsec),
    LUABSD_FUNC("__gc", TimeSpec_gc),
    LUABSD_FUNC("__tostring",   TimeSpec_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void
timespec_init(void *ud, void *arg)
{
    luab_timespec_t *self = (luab_timespec_t *)ud;

    (void)memmove(&self->timespec, arg, sizeof(self->timespec));
}

static void *
timespec_udata(lua_State *L, int narg)
{
    luab_timespec_t *self = luab_to_timespec(L, narg);

    return (&self->timespec);
}

luab_module_t timespec_type = {
    .cookie = LUABSD_TIMESPEC_TYPE_ID,
    .name = LUABSD_TIMESPEC_TYPE,
    .vec = timespec_methods,
    .init = timespec_init,
    .get = timespec_udata,
    .sz = sizeof(luab_timespec_t),
};

/***
 * Ctor.
 *
 * @function StructTimeSpec
 *
 * @param timespec                    Instance of LUA_TUSERDATA(luab_timespec_t).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage timespec = bsd.sys.time.StructTimeSpec([ timespec ])
 */
int
luab_StructTimeSpec(lua_State *L)
{
    int narg;
    struct timespec *timespec;
    int status;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        timespec = NULL;
    else
        timespec = timespec_udata(L, narg);

    if (luab_new_timespec(L, timespec) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}
