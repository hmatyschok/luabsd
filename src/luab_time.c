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

#include <sys/param.h>

#include <errno.h>
#include <string.h>
#include <time.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

/*
 * Interface against
 *
 *  struct tm {
 *      int tm_sec;
 *      int tm_min;
 *      int tm_hour;
 *      int tm_mday;
 *      int tm_mon;
 *      int tm_year;
 *      int tm_wday;
 *      int tm_yday;
 *      int tm_isdst;
 *      long    tm_gmtoff;
 *      char    *tm_zone;
 *  };
 */

#define LUABSD_TM_TYPE_ID    1594168426
#define LUABSD_TM_TYPE    "TM*"

typedef struct luab_tm {
    struct tm    tm;
} luab_tm_t;

#define luab_newtm(L, arg) \
    ((luab_tm_t *)luab_newuserdata(L, &tm_type, (arg)))
#define luab_totm(L, narg) \
    (luab_todata((L), (narg), &tm_type, luab_tm_t *))

/***
 * Set value for seconds after the minute [0-60].
 *
 * @function set_tm_sec
 *
 * @param sec              Seconds.
 *
 * @usage tm:set_tm_sec(sec)
 */
static int
TM_set_tm_sec(lua_State *L)
{
    luab_tm_t *self;
    int tm_sec;

    luab_checkmaxargs(L, 2);

    self = luab_totm(L, 1);
    tm_sec = luab_checkinteger(L, 2, INT_MAX);

    self->tm.tm_sec = tm_sec;

    return 0;
}

/***
 * Get value for seconds after the minute [0-60].
 *
 * @function get_tm_sec
 *
 * @return (LUA_TNUMBER)
 *
 * @usage sec = tm:get_tm_sec()
 */
static int
TM_get_tm_sec(lua_State *L)
{
    luab_tm_t *self;
    int tm_sec;

    luab_checkmaxargs(L, 1);

    self = luab_totm(L, 1);
    tm_sec = self->tm.tm_sec;

    lua_pushinteger(L, tm_sec);

    return 1;
}

/***
 * Set value for minutes after the hour [0-59].
 *
 * @function set_tm_min
 *
 * @param min              Seconds.
 *
 * @usage tm:set_tm_min(min)
 */
static int
TM_set_tm_min(lua_State *L)
{
    luab_tm_t *self;
    int tm_min;

    luab_checkmaxargs(L, 2);

    self = luab_totm(L, 1);
    tm_min = luab_checkinteger(L, 2, INT_MAX);

    self->tm.tm_min = tm_min;

    return 0;
}

/***
 * Get value for minutes after the hour [0-59].
 *
 * @function get_tm_min
 *
 * @return (LUA_TNUMBER)
 *
 * @usage min = tm:get_tm_min()
 */
static int
TM_get_tm_min(lua_State *L)
{
    luab_tm_t *self;
    int tm_min;

    luab_checkmaxargs(L, 1);

    self = luab_totm(L, 1);
    tm_min = self->tm.tm_min;

    lua_pushinteger(L, tm_min);

    return 1;
}

/***
 * Set value for hours since midnight [0-23].
 *
 * @function set_tm_hour
 *
 * @param hour              Seconds.
 *
 * @usage tm:set_tm_hour(hour)
 */
static int
TM_set_tm_hour(lua_State *L)
{
    luab_tm_t *self;
    int tm_hour;

    luab_checkmaxargs(L, 2);

    self = luab_totm(L, 1);
    tm_hour = luab_checkinteger(L, 2, INT_MAX);

    self->tm.tm_hour = tm_hour;

    return 0;
}

/***
 * Get value for hours since midnight [0-23].
 *
 * @function get_tm_hour
 *
 * @return (LUA_TNUMBER)
 *
 * @usage hour = tm:get_tm_hour()
 */
static int
TM_get_tm_hour(lua_State *L)
{
    luab_tm_t *self;
    int tm_hour;

    luab_checkmaxargs(L, 1);

    self = luab_totm(L, 1);
    tm_hour = self->tm.tm_hour;

    lua_pushinteger(L, tm_hour);

    return 1;
}

/***
 * Set value for day of the month [1-31].
 *
 * @function set_tm_mday
 *
 * @param mday              Seconds.
 *
 * @usage tm:set_tm_mday(mday)
 */
static int
TM_set_tm_mday(lua_State *L)
{
    luab_tm_t *self;
    int tm_mday;

    luab_checkmaxargs(L, 2);

    self = luab_totm(L, 1);
    tm_mday = luab_checkinteger(L, 2, INT_MAX);

    self->tm.tm_mday = tm_mday;

    return 0;
}

/***
 * Get value for day of the month [1-31].
 *
 * @function get_tm_mday
 *
 * @return (LUA_TNUMBER)
 *
 * @usage mday = tm:get_tm_mday()
 */
static int
TM_get_tm_mday(lua_State *L)
{
    luab_tm_t *self;
    int tm_mday;

    luab_checkmaxargs(L, 1);

    self = luab_totm(L, 1);
    tm_mday = self->tm.tm_mday;

    lua_pushinteger(L, tm_mday);

    return 1;
}

/***
 * Set value for months since January [0-11].
 *
 * @function set_tm_mon
 *
 * @param mon              Seconds.
 *
 * @usage tm:set_tm_mon(mon)
 */
static int
TM_set_tm_mon(lua_State *L)
{
    luab_tm_t *self;
    int tm_mon;

    luab_checkmaxargs(L, 2);

    self = luab_totm(L, 1);
    tm_mon = luab_checkinteger(L, 2, INT_MAX);

    self->tm.tm_mon = tm_mon;

    return 0;
}

/***
 * Get value for months since January [0-11].
 *
 * @function get_tm_mon
 *
 * @return (LUA_TNUMBER)
 *
 * @usage mon = tm:get_tm_mon()
 */
static int
TM_get_tm_mon(lua_State *L)
{
    luab_tm_t *self;
    int tm_mon;

    luab_checkmaxargs(L, 1);

    self = luab_totm(L, 1);
    tm_mon = self->tm.tm_mon;

    lua_pushinteger(L, tm_mon);

    return 1;
}

/***
 * Set value for years since 1900.
 *
 * @function set_tm_year
 *
 * @param year              Seconds.
 *
 * @usage tm:set_tm_year(year)
 */
static int
TM_set_tm_year(lua_State *L)
{
    luab_tm_t *self;
    int tm_year;

    luab_checkmaxargs(L, 2);

    self = luab_totm(L, 1);
    tm_year = luab_checkinteger(L, 2, INT_MAX);

    self->tm.tm_year = tm_year;

    return 0;
}

/***
 * Get value for years since 1900.
 *
 * @function get_tm_year
 *
 * @return (LUA_TNUMBER)
 *
 * @usage year = tm:get_tm_year()
 */
static int
TM_get_tm_year(lua_State *L)
{
    luab_tm_t *self;
    int tm_year;

    luab_checkmaxargs(L, 1);

    self = luab_totm(L, 1);
    tm_year = self->tm.tm_year;

    lua_pushinteger(L, tm_year);

    return 1;
}

/***
 * Set value for days since Sunday [0-6].
 *
 * @function set_tm_wday
 *
 * @param wday              Seconds.
 *
 * @usage tm:set_tm_wday(wday)
 */
static int
TM_set_tm_wday(lua_State *L)
{
    luab_tm_t *self;
    int tm_wday;

    luab_checkmaxargs(L, 2);

    self = luab_totm(L, 1);
    tm_wday = luab_checkinteger(L, 2, INT_MAX);

    self->tm.tm_wday = tm_wday;

    return 0;
}

/***
 * Get value for days since Sunday [0-6].
 *
 * @function get_tm_wday
 *
 * @return (LUA_TNUMBER)
 *
 * @usage wday = tm:get_tm_wday()
 */
static int
TM_get_tm_wday(lua_State *L)
{
    luab_tm_t *self;
    int tm_wday;

    luab_checkmaxargs(L, 1);

    self = luab_totm(L, 1);
    tm_wday = self->tm.tm_wday;

    lua_pushinteger(L, tm_wday);

    return 1;
}

/***
 * Set value for days since January 1 [0-365].
 *
 * @function set_tm_yday
 *
 * @param yday              Seconds.
 *
 * @usage tm:set_tm_yday(yday)
 */
static int
TM_set_tm_yday(lua_State *L)
{
    luab_tm_t *self;
    int tm_yday;

    luab_checkmaxargs(L, 2);

    self = luab_totm(L, 1);
    tm_yday = luab_checkinteger(L, 2, INT_MAX);

    self->tm.tm_yday = tm_yday;

    return 0;
}

/***
 * Get value for days since January 1 [0-365].
 *
 * @function get_tm_yday
 *
 * @return (LUA_TNUMBER)
 *
 * @usage yday = tm:get_tm_yday()
 */
static int
TM_get_tm_yday(lua_State *L)
{
    luab_tm_t *self;
    int tm_yday;

    luab_checkmaxargs(L, 1);

    self = luab_totm(L, 1);
    tm_yday = self->tm.tm_yday;

    lua_pushinteger(L, tm_yday);

    return 1;
}

/***
 * Set value for Daylight Savings Time flag.
 *
 * @function set_tm_isdst
 *
 * @param isdst              Seconds.
 *
 * @usage tm:set_tm_isdst(isdst)
 */
static int
TM_set_tm_isdst(lua_State *L)
{
    luab_tm_t *self;
    int tm_isdst;

    luab_checkmaxargs(L, 2);

    self = luab_totm(L, 1);
    tm_isdst = luab_checkinteger(L, 2, INT_MAX);

    self->tm.tm_isdst = tm_isdst;

    return 0;
}

/***
 * Get value for Daylight Savings Time flag.
 *
 * @function get_tm_isdst
 *
 * @return (LUA_TNUMBER)
 *
 * @usage isdst = tm:get_tm_isdst()
 */
static int
TM_get_tm_isdst(lua_State *L)
{
    luab_tm_t *self;
    int tm_isdst;

    luab_checkmaxargs(L, 1);

    self = luab_totm(L, 1);
    tm_isdst = self->tm.tm_isdst;

    lua_pushinteger(L, tm_isdst);

    return 1;
}

/***
 * Set value for offset from UTC in seconds.
 *
 * @function set_tm_gmtoff
 *
 * @param gmtoff              Seconds.
 *
 * @usage tm:set_tm_gmtoff(gmtoff)
 */
static int
TM_set_tm_gmtoff(lua_State *L)
{
    luab_tm_t *self;
    long tm_gmtoff;

    luab_checkmaxargs(L, 2);

    self = luab_totm(L, 1);
    tm_gmtoff = luab_checkinteger(L, 2, LONG_MAX);

    self->tm.tm_gmtoff = tm_gmtoff;

    return 0;
}

/***
 * Get value for offset from UTC in seconds.
 *
 * @function get_tm_gmtoff
 *
 * @return (LUA_TNUMBER)
 *
 * @usage gmtoff = tm:get_tm_gmtoff()
 */
static int
TM_get_tm_gmtoff(lua_State *L)
{
    luab_tm_t *self;
    long tm_gmtoff;

    luab_checkmaxargs(L, 1);

    self = luab_totm(L, 1);
    tm_gmtoff = self->tm.tm_gmtoff;

    lua_pushinteger(L, tm_gmtoff);

    return 1;
}

/***
 * Get value for tm abbreviation.
 *
 * @function get_tm_zone
 *
 * @return (LUA_TSTRING)
 *
 * @usage zone = tm:get_tm_zone()
 */
static int
TM_get_tm_zone(lua_State *L)
{
    luab_tm_t *self;
    char *tm_zone;
    size_t len;

    luab_checkmaxargs(L, 1);

    self = luab_totm(L, 1);
    tm_zone = self->tm.tm_zone;
    len = strlen(tm_zone);

    lua_pushlstring(L, tm_zone, len);

    return 1;
}

/***
 * Translate tm{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = tm:get()
 */
static int
TM_get(lua_State *L)
{
    luab_tm_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_totm(L, 1);

    lua_newtable(L);

    luab_setinteger(L, -2, "tm_sec", self->tm.tm_sec);
    luab_setinteger(L, -2, "tm_min", self->tm.tm_min);
    luab_setinteger(L, -2, "tm_hour", self->tm.tm_hour);
    luab_setinteger(L, -2, "tm_mday", self->tm.tm_mday);
    luab_setinteger(L, -2, "tm_mon", self->tm.tm_mon);
    luab_setinteger(L, -2, "tm_year", self->tm.tm_year);
    luab_setinteger(L, -2, "tm_wday", self->tm.tm_wday);
    luab_setinteger(L, -2, "tm_yday", self->tm.tm_yday);
    luab_setinteger(L, -2, "tm_isdst", self->tm.tm_isdst);
    luab_setinteger(L, -2, "tm_gmtoff", self->tm.tm_gmtoff);
    luab_setstring(L, -2, "tm_zone", self->tm.tm_zone);

    lua_pushvalue(L, -1);

    return 1;
}

static int
TM_tostring(lua_State *L)
{
    luab_tm_t *self = luab_totm(L, 1);
    lua_pushfstring(L, "tm (%p)", self);

    return 1;
}

static luab_table_t tm_methods[] = {
    LUABSD_FUNC("set_tm_sec",   TM_set_tm_sec),
    LUABSD_FUNC("set_tm_min",   TM_set_tm_min),
    LUABSD_FUNC("set_tm_hour",   TM_set_tm_hour),
    LUABSD_FUNC("set_tm_mday",   TM_set_tm_mday),
    LUABSD_FUNC("set_tm_mon",   TM_set_tm_mon),
    LUABSD_FUNC("set_tm_year",   TM_set_tm_year),
    LUABSD_FUNC("set_tm_wday",   TM_set_tm_wday),
    LUABSD_FUNC("set_tm_yday",   TM_set_tm_yday),
    LUABSD_FUNC("set_tm_isdst",   TM_set_tm_isdst),
    LUABSD_FUNC("set_tm_gmtoff",   TM_set_tm_gmtoff),
    LUABSD_FUNC("get",  TM_get),
    LUABSD_FUNC("get_tm_sec",   TM_get_tm_sec),
    LUABSD_FUNC("get_tm_min",   TM_get_tm_min),
    LUABSD_FUNC("get_tm_hour",   TM_get_tm_hour),
    LUABSD_FUNC("get_tm_mday",   TM_get_tm_mday),
    LUABSD_FUNC("get_tm_mon",   TM_get_tm_mon),
    LUABSD_FUNC("get_tm_year",   TM_get_tm_year),
    LUABSD_FUNC("get_tm_wday",   TM_get_tm_wday),
    LUABSD_FUNC("get_tm_yday",   TM_get_tm_yday),
    LUABSD_FUNC("get_tm_isdst",   TM_get_tm_isdst),
    LUABSD_FUNC("get_tm_gmtoff",   TM_get_tm_gmtoff),
    LUABSD_FUNC("get_tm_zone",   TM_get_tm_zone),
    LUABSD_FUNC("__tostring",   TM_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void
tm_init(void *ud, void *arg)
{
    luab_tm_t *self = (luab_tm_t *)ud;

    (void)memmove(&self->tm, arg, sizeof(self->tm));
}

static void *
tm_udata(lua_State *L, int narg)
{
    luab_tm_t *self = luab_totm(L, narg);

    return (&self->tm);
}

luab_module_t tm_type = {
    .cookie = LUABSD_TM_TYPE_ID,
    .name = LUABSD_TM_TYPE,
    .vec = tm_methods,
    .init = tm_init,
    .get = tm_udata,
    .sz = sizeof(luab_tm_t),
};

/***
 * Ctor.
 *
 * @function StructTM
 *
 * @return (LUA_TUSERDATA)
 *
 * @usage tz = bsd.sys.time.StructTM()
 */
static int
luab_StructTM(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 1);
    struct tm *tm;

    if (narg == 0)
        tm = NULL;
    else
        tm = tm_udata(L, narg);

    if (luab_newtm(L, tm) == NULL)
        luaL_error(L, "%s", strerror(errno));

    return 1;
}

/*
 * Interface against components or service primitives over <time.h>.
 */

#define LUABSD_TIME_LIB_ID    1594167179
#define LUABSD_TIME_LIB_KEY    "time"


static luab_table_t luab_time_vec[] = { /* time.h */
#if __POSIX_VISIBLE > 0 && __POSIX_VISIBLE < 200112 || __BSD_VISIBLE
    LUABSD_INT("CLK_TCK",   CLK_TCK),
#endif
#if !defined(CLOCK_REALTIME) && __POSIX_VISIBLE >= 200112
    LUABSD_INT("CLOCK_REALTIME",    CLOCK_REALTIME),
#ifdef __BSD_VISIBLE
    LUABSD_INT("CLOCK_VIRTUAL", CLOCK_VIRTUAL),
    LUABSD_INT("CLOCK_PROF",    CLOCK_PROF),
#endif
    LUABSD_INT("CLOCK_MONOTONIC",   CLOCK_MONOTONIC),
    LUABSD_INT("CLOCK_UPTIME",  CLOCK_UPTIME),
    LUABSD_INT("CLOCK_UPTIME_PRECISE",  CLOCK_UPTIME_PRECISE),
    LUABSD_INT("CLOCK_UPTIME_FAST", CLOCK_UPTIME_FAST),
    LUABSD_INT("CLOCK_REALTIME_PRECISE",    CLOCK_REALTIME_PRECISE),
    LUABSD_INT("CLOCK_REALTIME_FAST",   CLOCK_REALTIME_FAST),
    LUABSD_INT("CLOCK_MONOTONIC_PRECISE",   CLOCK_MONOTONIC_PRECISE),
    LUABSD_INT("CLOCK_MONOTONIC_FAST",  CLOCK_MONOTONIC_FAST),
    LUABSD_INT("CLOCK_SECOND",  CLOCK_SECOND),
    LUABSD_INT("CLOCK_THREAD_CPUTIME_ID",   CLOCK_THREAD_CPUTIME_ID),
    LUABSD_INT("CLOCK_PROCESS_CPUTIME_ID",  CLOCK_PROCESS_CPUTIME_ID),
#endif /* !defined(CLOCK_REALTIME) && __POSIX_VISIBLE >= 200112 */
#if !defined(TIMER_ABSTIME) && __POSIX_VISIBLE >= 200112
#if __BSD_VISIBLE
    LUABSD_INT("TIMER_RELTIME", TIMER_RELTIME),
#endif
    LUABSD_INT("TIMER_ABSTIME", TIMER_ABSTIME),
#endif /* !defined(TIMER_ABSTIME) && __POSIX_VISIBLE >= 200112 */



    LUABSD_INT("CLOCKS_PER_SEC",    CLOCKS_PER_SEC),
    LUABSD_FUNC("StructTM", luab_StructTM),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_time_lib = {
    .cookie = LUABSD_TIME_LIB_ID,
    .name = LUABSD_TIME_LIB_KEY,
    .vec = luab_time_vec,
};
