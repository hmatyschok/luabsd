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

#include <sys/limits.h>
#include <sys/time.h>

#include <pthread.h>
#include <signal.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

/*
 * Interface against
 *
 *  struct timezone {
 *      int tz_minuteswest;
 *      int tz_dsttime;
 *  };
 */

#define LUABSD_TIMEZONE_TYPE_ID    1594159943
#define LUABSD_TIMEZONE_TYPE    "TIMEZONE*"

typedef struct {
    struct timezone    timezone;
} luab_timezone_t;

#define luab_newtimezone(L, arg) \
    ((luab_timezone_t *)luab_newuserdata(L, &timezone_type, (arg)))
#define luab_totimezone(L, narg) \
    (luab_todata((L), (narg), &timezone_type, luab_timezone_t *))

/***
 * Set value for minutes west of Greenwich.
 *
 * @function set_tz_minuteswest
 *
 * @param zone          Specifies value in minutes.
 *
 * @usage tz:set_tz_minuteswest(min)
 */
static int
TimeZone_set_tz_minuteswest(lua_State *L)
{
    luab_timezone_t *self;
    int tz_minuteswest;

    luab_checkmaxargs(L, 2);

    self = luab_totimezone(L, 1);
    tz_minuteswest = luab_checkinteger(L, 2, INT_MAX);

    self->timezone.tz_minuteswest = tz_minuteswest;

    return 0;
}

/***
 * Get value for minutes west of Greenwich.
 *
 * @function get_tz_minuteswest
 *
 * @return (LUA_TNUMBER)
 *
 * @usage zone = tz:get_tz_minuteswest()
 */
static int
TimeZone_get_tz_minuteswest(lua_State *L)
{
    luab_timezone_t *self;
    int tz_minuteswest;

    luab_checkmaxargs(L, 1);

    self = luab_totimezone(L, 1);
    tz_minuteswest = self->timezone.tz_minuteswest;

    lua_pushinteger(L, tz_minuteswest);

    return 1;
}

/***
 * Set value for type of dst correction.
 *
 * @function set_tz_dsttime
 *
 * @param dst           Specifies dst correction.
 *
 * @usage tz:set_tz_dsttime(dst)
 */
static int
TimeZone_set_tz_dsttime(lua_State *L)
{
    luab_timezone_t *self;
    int tz_dsttime;

    luab_checkmaxargs(L, 2);

    self = luab_totimezone(L, 1);
    tz_dsttime = luab_checkinteger(L, 2, LONG_MAX);

    self->timezone.tz_dsttime = tz_dsttime;

    return 0;
}

/***
 * Get value for type of dst correction.
 *
 * @function get_tz_dsttime
 *
 * @return (LUA_TNUMBER)
 *
 * @usage dst = tz:get_tz_dsttime()
 */
static int
TimeZone_get_tz_dsttime(lua_State *L)
{
    luab_timezone_t *self;
    int tz_dsttime;

    luab_checkmaxargs(L, 1);

    self = luab_totimezone(L, 1);
    tz_dsttime = self->timezone.tz_dsttime;

    lua_pushinteger(L, tz_dsttime);

    return 1;
}

/***
 * Translate timezone{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = tz:get()
 */
static int
TimeZone_get(lua_State *L)
{
    luab_timezone_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_totimezone(L, 1);

    lua_newtable(L);

    luab_setinteger(L, -2, "tz_minuteswest", self->timezone.tz_minuteswest);
    luab_setinteger(L, -2, "tz_dsttime", self->timezone.tz_dsttime);

    lua_pushvalue(L, -1);

    return 1;
}

static int
TimeZone_tostring(lua_State *L)
{
    luab_timezone_t *self = luab_totimezone(L, 1);
    lua_pushfstring(L, "timezone (%p)", self);

    return 1;
}

static luab_table_t timezone_methods[] = {
    LUABSD_FUNC("set_tz_minuteswest",   TimeZone_set_tz_minuteswest),
    LUABSD_FUNC("set_tz_dsttime",   TimeZone_set_tz_dsttime),
    LUABSD_FUNC("get",  TimeZone_get),
    LUABSD_FUNC("get_tz_minuteswest",   TimeZone_get_tz_minuteswest),
    LUABSD_FUNC("get_tz_dsttime",   TimeZone_get_tz_dsttime),
    LUABSD_FUNC("__tostring",   TimeZone_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void
timezone_init(void *ud, void *arg)
{
    luab_timezone_t *self = (luab_timezone_t *)ud;

    (void)memmove(&self->timezone, arg, sizeof(self->timezone));
}

static void *
timezone_udata(lua_State *L, int narg)
{
    luab_timezone_t *self = luab_totimezone(L, narg);

    return (&self->timezone);
}

luab_module_t timezone_type = {
    .cookie = LUABSD_TIMEZONE_TYPE_ID,
    .name = LUABSD_TIMEZONE_TYPE,
    .vec = timezone_methods,
    .init = timezone_init,
    .get = timezone_udata,
    .sz = sizeof(luab_timezone_t),
};

/***
 * Ctor.
 *
 * @function StructTimeZone
 *
 * @return (LUA_TUSERDATA)
 *
 * @usage tz = bsd.sys.time.StructTimeZone()
 */
static int
luab_StructTimeZone(lua_State *L)
{
    luab_checkmaxargs(L, 0);

    (void)luab_newtimezone(L, NULL);

    return 1;
}

#if __BSD_VISIBLE
/*
 * Interface against
 *
 *  struct bintime {
 *      time_t  sec;
 *      uint64_t frac;
 *  };
 */

#define LUABSD_BINTIME_TYPE_ID    1594161740
#define LUABSD_BINTIME_TYPE    "BINTIME*"

typedef struct {
    struct bintime    bintime;
} luab_bintime_t;

#define luab_newbintime(L, arg) \
    ((luab_bintime_t *)luab_newuserdata(L, &bintime_type, (arg)))
#define luab_tobintime(L, narg) \
    (luab_todata((L), (narg), &bintime_type, luab_bintime_t *))

/***
 * Set value for sytem time.
 *
 * @function set_sec
 *
 * @param sec           Seconds.
 *
 * @usage bt:set_sec(sec)
 */
static int
BinTime_set_sec(lua_State *L)
{
    luab_bintime_t *self;
    int sec;

    luab_checkmaxargs(L, 2);

    self = luab_tobintime(L, 1);
    sec = luab_checkinteger(L, 2, INT_MAX);

    self->bintime.sec = sec;

    return 0;
}

/***
 * Get value for system time.
 *
 * @function get_sec
 *
 * @return (LUA_TNUMBER)
 *
 * @usage sec = bt:get_sec()
 */
static int
BinTime_get_sec(lua_State *L)
{
    luab_bintime_t *self;
    int sec;

    luab_checkmaxargs(L, 1);

    self = luab_tobintime(L, 1);
    sec = self->bintime.sec;

    lua_pushinteger(L, sec);

    return 1;
}

/***
 * Set value for for system time.
 *
 * @function set_frac
 *
 * @param frac          Specifies frac.
 *
 * @usage bt:set_frac(frac)
 */
static int
BinTime_set_frac(lua_State *L)
{
    luab_bintime_t *self;
    uint64_t frac;

    luab_checkmaxargs(L, 2);

    self = luab_tobintime(L, 1);
    frac = luab_checkinteger(L, 2, LONG_MAX);

    self->bintime.frac = frac;

    return 0;
}

/***
 * Get value for for system time.
 *
 * @function get_frac
 *
 * @return (LUA_TNUMBER)
 *
 * @usage frac = bt:get_frac()
 */
static int
BinTime_get_frac(lua_State *L)
{
    luab_bintime_t *self;
    uint64_t frac;

    luab_checkmaxargs(L, 1);

    self = luab_tobintime(L, 1);
    frac = self->bintime.frac;

    lua_pushinteger(L, frac);

    return 1;
}

/***
 * Translate bintime{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = bt:get()
 */
static int
BinTime_get(lua_State *L)
{
    luab_bintime_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_tobintime(L, 1);

    lua_newtable(L);

    luab_setinteger(L, -2, "sec", self->bintime.sec);
    luab_setinteger(L, -2, "frac", self->bintime.frac);

    lua_pushvalue(L, -1);

    return 1;
}

static int
BinTime_tostring(lua_State *L)
{
    luab_bintime_t *self = luab_tobintime(L, 1);
    lua_pushfstring(L, "bintime (%p)", self);

    return 1;
}

static luab_table_t bintime_methods[] = {
    LUABSD_FUNC("set_sec",  BinTime_set_sec),
    LUABSD_FUNC("set_frac", BinTime_set_frac),
    LUABSD_FUNC("get",  BinTime_get),
    LUABSD_FUNC("get_sec",  BinTime_get_sec),
    LUABSD_FUNC("get_frac", BinTime_get_frac),
    LUABSD_FUNC("__tostring",   BinTime_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void
bintime_init(void *ud, void *arg)
{
    luab_bintime_t *self = (luab_bintime_t *)ud;

    (void)memmove(&self->bintime, arg, sizeof(self->bintime));
}

static void *
bintime_udata(lua_State *L, int narg)
{
    luab_bintime_t *self = luab_tobintime(L, narg);

    return (&self->bintime);
}

luab_module_t bintime_type = {
    .cookie = LUABSD_BINTIME_TYPE_ID,
    .name = LUABSD_BINTIME_TYPE,
    .vec = bintime_methods,
    .init = bintime_init,
    .get = bintime_udata,
    .sz = sizeof(luab_bintime_t),
};

/***
 * Ctor.
 *
 * @function StructBinTime
 *
 * @return (LUA_TUSERDATA)
 *
 * @usage bt = bsd.sys.time.StructBinTime()
 */
static int
luab_StructBinTime(lua_State *L)
{
    luab_checkmaxargs(L, 0);

    (void)luab_newbintime(L, NULL);

    return 1;
}
#endif /* __BSD_VISIBLE */

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

#define LUABSD_CLOCKINFO_TYPE_ID    1594164272
#define LUABSD_CLOCKINFO_TYPE    "CLOCKINFO*"

typedef struct {
    struct clockinfo    clockinfo;
} luab_clockinfo_t;

#define luab_newclockinfo(L, arg) \
    ((luab_clockinfo_t *)luab_newuserdata(L, &clockinfo_type, (arg)))
#define luab_toclockinfo(L, narg) \
    (luab_todata((L), (narg), &clockinfo_type, luab_clockinfo_t *))

/***
 * Set clock frequency.
 *
 * @function set_hz
 *
 * @param hz            Frequency.
 *
 * @usage ci:set_hz(hz)
 */
static int
ClockInfo_set_hz(lua_State *L)
{
    luab_clockinfo_t *self;
    int hz;

    luab_checkmaxargs(L, 2);

    self = luab_toclockinfo(L, 1);
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
 * @usage hz = ci:get_hz()
 */
static int
ClockInfo_get_hz(lua_State *L)
{
    luab_clockinfo_t *self;
    int hz;

    luab_checkmaxargs(L, 1);

    self = luab_toclockinfo(L, 1);
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
 * @usage ci:set_tick(tick)
 */
static int
ClockInfo_set_tick(lua_State *L)
{
    luab_clockinfo_t *self;
    int tick;

    luab_checkmaxargs(L, 2);

    self = luab_toclockinfo(L, 1);
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
 * @usage tick = ci:get_tick()
 */
static int
ClockInfo_get_tick(lua_State *L)
{
    luab_clockinfo_t *self;
    int tick;

    luab_checkmaxargs(L, 1);

    self = luab_toclockinfo(L, 1);
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
 * @usage ci:set_stathz(stathz)
 */
static int
ClockInfo_set_stathz(lua_State *L)
{
    luab_clockinfo_t *self;
    int stathz;

    luab_checkmaxargs(L, 2);

    self = luab_toclockinfo(L, 1);
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
 * @usage stathz = ci:get_stathz()
 */
static int
ClockInfo_get_stathz(lua_State *L)
{
    luab_clockinfo_t *self;
    int stathz;

    luab_checkmaxargs(L, 1);

    self = luab_toclockinfo(L, 1);
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
 * @usage ci:set_profhz(profhz)
 */
static int
ClockInfo_set_profhz(lua_State *L)
{
    luab_clockinfo_t *self;
    int profhz;

    luab_checkmaxargs(L, 2);

    self = luab_toclockinfo(L, 1);
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
 * @usage profhz = ci:get_profhz()
 */
static int
ClockInfo_get_profhz(lua_State *L)
{
    luab_clockinfo_t *self;
    int profhz;

    luab_checkmaxargs(L, 1);

    self = luab_toclockinfo(L, 1);
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
 * @usage t = ci:get()
 */
static int
ClockInfo_get(lua_State *L)
{
    luab_clockinfo_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_toclockinfo(L, 1);

    lua_newtable(L);

    luab_setinteger(L, -2, "hz", self->clockinfo.hz);
    luab_setinteger(L, -2, "tick", self->clockinfo.tick);
    luab_setinteger(L, -2, "stathz", self->clockinfo.stathz);
    luab_setinteger(L, -2, "profhz", self->clockinfo.stathz);

    lua_pushvalue(L, -1);

    return 1;
}

static int
ClockInfo_tostring(lua_State *L)
{
    luab_clockinfo_t *self = luab_toclockinfo(L, 1);
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
    luab_clockinfo_t *self = luab_toclockinfo(L, narg);

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
 * @return (LUA_TUSERDATA)
 *
 * @usage ci = bsd.sys.time.StructClockInfo()
 */
static int
luab_StructClockInfo(lua_State *L)
{
    luab_checkmaxargs(L, 0);

    (void)luab_newclockinfo(L, NULL);

    return 1;
}

/*
 * Interface against
 *
 *  struct timespec {
 *      time_t  tv_sec;
 *      long    tv_nsec;
 *  };
 */

#define LUABSD_TIMESPEC_TYPE_ID    1594034844
#define LUABSD_TIMESPEC_TYPE    "TIMESPEC*"

typedef struct {
    struct timespec    timespec;
} luab_timespec_t;

#define luab_newtimespec(L, arg) \
    ((luab_timespec_t *)luab_newuserdata(L, &timespec_type, (arg)))
#define luab_totimespec(L, narg) \
    (luab_todata((L), (narg), &timespec_type, luab_timespec_t *))

/***
 * Set value for tv_sec.
 *
 * @function set_tv_sec
 *
 * @param sec           Specifies value in seconds.
 *
 * @usage tv:set_tv_sec(sec)
 */
static int
TimeSpec_set_tv_sec(lua_State *L)
{
    luab_timespec_t *self;
    time_t tv_sec;

    luab_checkmaxargs(L, 2);

    self = luab_totimespec(L, 1);
    tv_sec = luab_checkinteger(L, 2, INT_MAX);

    self->timespec.tv_sec = tv_sec;

    return 0;
}

/***
 * Get value for tv_sec.
 *
 * @function get_tv_sec
 *
 * @return (LUA_TNUMBER)
 *
 * @usage sec = tv:get_tv_sec()
 */
static int
TimeSpec_get_tv_sec(lua_State *L)
{
    luab_timespec_t *self;
    time_t tv_sec;

    luab_checkmaxargs(L, 1);

    self = luab_totimespec(L, 1);
    tv_sec = self->timespec.tv_sec;

    lua_pushinteger(L, tv_sec);

    return 1;
}

/***
 * Set value for tv_nsec.
 *
 * @function set_tv_nsec
 *
 * @param nsec           Specifies value in nanoneconds.
 *
 * @usage tv:set_tv_nsec(nsec)
 */
static int
TimeSpec_set_tv_nsec(lua_State *L)
{
    luab_timespec_t *self;
    long tv_nsec;

    luab_checkmaxargs(L, 2);

    self = luab_totimespec(L, 1);
    tv_nsec = luab_checkinteger(L, 2, LONG_MAX);

    self->timespec.tv_nsec = tv_nsec;

    return 0;
}

/***
 * Get value for tv_nsec.
 *
 * @function get_tv_nsec
 *
 * @return (LUA_TNUMBER)
 *
 * @usage nsec = tv:get_tv_nsec()
 */
static int
TimeSpec_get_tv_nsec(lua_State *L)
{
    luab_timespec_t *self;
    long tv_nsec;

    luab_checkmaxargs(L, 1);

    self = luab_totimespec(L, 1);
    tv_nsec = self->timespec.tv_nsec;

    lua_pushinteger(L, tv_nsec);

    return 1;
}

/***
 * Translate timespec{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = tv:get()
 */
static int
TimeSpec_get(lua_State *L)
{
    luab_timespec_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_totimespec(L, 1);

    lua_newtable(L);

    luab_setinteger(L, -2, "tv_sec", self->timespec.tv_sec);
    luab_setinteger(L, -2, "tv_nsec", self->timespec.tv_nsec);

    lua_pushvalue(L, -1);

    return 1;
}

static int
TimeSpec_tostring(lua_State *L)
{
    luab_timespec_t *self = luab_totimespec(L, 1);
    lua_pushfstring(L, "timespec (%p)", self);

    return 1;
}

static luab_table_t timespec_methods[] = {
    LUABSD_FUNC("set_tv_sec",   TimeSpec_set_tv_sec),
    LUABSD_FUNC("set_tv_nsec",  TimeSpec_set_tv_nsec),
    LUABSD_FUNC("get",  TimeSpec_get),
    LUABSD_FUNC("get_tv_sec",   TimeSpec_get_tv_sec),
    LUABSD_FUNC("get_tv_nsec",  TimeSpec_get_tv_nsec),
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
    luab_timespec_t *self = luab_totimespec(L, narg);

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
 * @return (LUA_TUSERDATA)
 *
 * @usage tv = bsd.sys.time.StructTimeSpec()
 */
static int
luab_StructTimeSpec(lua_State *L)
{
    luab_checkmaxargs(L, 0);

    (void)luab_newtimespec(L, NULL);

    return 1;
}

/*
 * Interface against
 *
 *  struct itimerval {
 *      struct  timeval it_interval;
 *      struct  timeval it_value;
 *  };
 *
 */

#define LUABSD_ITIMERVAL_TYPE_ID    1594110231
#define LUABSD_ITIMERVAL_TYPE    "ITIMERVAL*"

typedef struct {
    struct itimerval    itimerval;
} luab_itimerval_t;

#define luab_newitimerval(L, arg) \
    ((luab_itimerval_t *)luab_newuserdata(L, &itimerval_type, (arg)))
#define luab_toitimerval(L, narg) \
    (luab_todata((L), (narg), &itimerval_type, luab_itimerval_t *))

/* timer interval */
/***
 * Set timer interval.
 *
 * @function set_it_interval
 *
 * @param tv            Instance of timespec{}.
 *
 * @usage it:set_it_interval(tv)
 */
static int
ItimerVal_set_it_interval(lua_State *L)
{
    luab_itimerval_t *self;
    void *ud;

    luab_checkmaxargs(L, 2);

    self = luab_toitimerval(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->itimerval.it_interval, ud, timespec_type.sz);

    return 0;
}

/***
 * Get timer interval.
 *
 * @function get_it_interval
 *
 * @return (LUA_TUSERDATA)
 *
 * @usage tv = it:get_it_interval()
 */
static int
ItimerVal_get_it_interval(lua_State *L)
{
    luab_itimerval_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_toitimerval(L, 1);

    (void)luab_newuserdata(L, &timespec_type, &self->itimerval.it_interval);

    return 1;
}

/* current value */
/***
 * Set current value.
 *
 * @function set_it_value
 *
 * @param tv            Instance of timespec{}.
 *
 * @usage it:set_it_value(tv)
 */
static int
ItimerVal_set_it_value(lua_State *L)
{
    luab_itimerval_t *self;
    void *ud;

    luab_checkmaxargs(L, 2);

    self = luab_toitimerval(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->itimerval.it_value, ud, timespec_type.sz);

    return 0;
}

/***
 * Get timer value.
 *
 * @function get_it_value
 *
 * @return (LUA_TUSERDATA)
 *
 * @usage tv = it:get_it_value()
 */
static int
ItimerVal_get_it_value(lua_State *L)
{
    luab_itimerval_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_toitimerval(L, 1);

    (void)luab_newuserdata(L, &timespec_type, &self->itimerval.it_value);

    return 1;
}

/***
 * Translate itimerval{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = it:get()
 */
static int
ItimerVal_get(lua_State *L)
{
    luab_itimerval_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_toitimerval(L, 1);

    lua_newtable(L);   /* XXX */

    luab_setudata(L, -2, &timespec_type, "it_interval", &self->itimerval.it_interval);
    luab_setudata(L, -2, &timespec_type, "it_value", &self->itimerval.it_value);

    lua_pushvalue(L, -1);

    return 1;
}

static int
ItimerVal_tostring(lua_State *L)
{
    luab_itimerval_t *self = luab_toitimerval(L, 1);
    lua_pushfstring(L, "itimerval (%p)", self);

    return 1;
}

static luab_table_t itimerval_methods[] = {
    LUABSD_FUNC("set_it_interval",  ItimerVal_set_it_interval),
    LUABSD_FUNC("set_it_value", ItimerVal_set_it_value),
    LUABSD_FUNC("get",  ItimerVal_get),
    LUABSD_FUNC("get_it_interval",  ItimerVal_get_it_interval),
    LUABSD_FUNC("get_it_value", ItimerVal_get_it_value),
    LUABSD_FUNC("__tostring",   ItimerVal_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void
itimerval_init(void *ud, void *arg)
{
    luab_itimerval_t *self = ud;    /* XXX */

    (void)memmove(&self->itimerval, arg, sizeof(self->itimerval));
}

static void *
itimerval_udata(lua_State *L, int narg)
{
    luab_itimerval_t *self = luab_toitimerval(L, narg);

    return (&self->itimerval);
}

luab_module_t itimerval_type = {
    .cookie = LUABSD_ITIMERVAL_TYPE_ID,
    .name = LUABSD_ITIMERVAL_TYPE,
    .vec = itimerval_methods,
    .init = itimerval_init,
    .get = itimerval_udata,
    .sz = sizeof(luab_itimerval_t),
};

/***
 * Ctor.
 *
 * @function StructItimerVal
 *
 * @return (LUA_TUSERDATA)
 *
 * @usage tv = bsd.sys.time.StructItimerVal()
 */
static int
luab_StructItimerVal(lua_State *L)
{
    luab_checkmaxargs(L, 0);

    (void)luab_newitimerval(L, NULL);

    return 1;
}

/*
 * Interface against components or service primitives on sys/time.h.
 */

#define LUABSD_SYS_TIME_LIB_ID    1593623310
#define LUABSD_SYS_TIME_LIB_KEY    "time"

static sigset_t nsigset;
static pthread_t tid;

static lua_State *saved_L;
static lua_Hook h;

static int h_msk;
static int h_cnt;

static void
h_callback(lua_State *L, lua_Debug *arg __unused)
{
    L = saved_L;

    lua_sethook(L, h, h_msk, h_cnt);
    lua_getfield(L, LUA_REGISTRYINDEX, "l_callback");

    if (lua_pcall(L, 0, 0, 0) != 0)
        lua_error(L);
}

static void *
h_signal(void *arg __unused)
{
    int l_msk = (LUA_MASKCALL|LUA_MASKRET|LUA_MASKCOUNT);
    int sig;

    for (;;) {
        if (sigwait(&nsigset, &sig) != 0)
            goto out;   /* XXX up-call */

        switch (sig) {
        case SIGALRM:
        case SIGVTALRM:
        case SIGPROF:

            h = lua_gethook(saved_L);
            h_msk = lua_gethookmask(saved_L);
            h_cnt = lua_gethookcount(saved_L);

            lua_sethook(saved_L, h_callback, l_msk, 1);
            goto out;
        default:
            break;
        }
    }
out:
    pthread_exit(NULL);
}

static int
luab_setitimer(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 4);
    int which;
    struct itimerval *value;
    struct itimerval *ovalue;
    int status;

    which = luab_checkinteger(L, 1, INT_MAX);
    value = (struct itimerval *)luab_checkudataisnil(L, 2, &itimerval_type);
    ovalue = (struct itimerval *)luab_checkudataisnil(L, 3, &itimerval_type);

    if (lua_type(L, narg) != LUA_TFUNCTION)
        return luaL_error(L, "Missing callout handler.");

    lua_settop(L, narg);
    lua_setfield(L, LUA_REGISTRYINDEX, "l_callback");

    saved_L = L;

    if ((status = sigfillset(&nsigset)) != 0)
        return luab_pusherr(L, status);

    if ((status = pthread_sigmask(SIG_BLOCK, &nsigset, NULL)) != 0)
        return luab_pusherr(L, status);

    if ((status = pthread_create(&tid, NULL, h_signal, NULL)) != 0)
        return luab_pusherr(L, status);

    if ((status = setitimer(which, value, ovalue)) != 0)
        pthread_cancel(tid);

    return luab_pusherr(L, status);
}

#if __XSI_VISIBLE
static int
luab_getitimer(lua_State *L)
{
    int which;
    struct itimerval *value;
    int status;

    (void)luab_checkmaxargs(L, 2);

    which = luab_checkinteger(L, 1, INT_MAX);
    value = (struct itimerval *)luab_checkudata(L, 2, &itimerval_type);

    status = getitimer(which, value);

    return luab_pusherr(L, status);
}
#endif

static luab_table_t luab_sys_time_vec[] = { /* sys/time.h */
    LUABSD_INT("DST_NONE",  DST_NONE),
    LUABSD_INT("DST_USA",   DST_USA),
    LUABSD_INT("DST_AUST",  DST_AUST),
    LUABSD_INT("DST_WET",   DST_WET),
    LUABSD_INT("DST_MET",   DST_MET),
    LUABSD_INT("DST_EET",   DST_EET),
    LUABSD_INT("DST_CAN",   DST_CAN),
#ifndef CLOCK_REALTIME
    LUABSD_INT("CLOCK_REALTIME",    CLOCK_REALTIME),
    LUABSD_INT("CLOCK_VIRTUAL", CLOCK_VIRTUAL),
    LUABSD_INT("CLOCK_PROF",    CLOCK_PROF),
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
#endif
#ifndef TIMER_ABSTIME
    LUABSD_INT("TIMER_RELTIME", TIMER_RELTIME),
    LUABSD_INT("TIMER_ABSTIME", TIMER_ABSTIME),
#endif
#if __BSD_VISIBLE
    LUABSD_INT("CPUCLOCK_WHICH_PID",   CPUCLOCK_WHICH_PID),
    LUABSD_INT("CPUCLOCK_WHICH_TID",   CPUCLOCK_WHICH_TID),
#endif
    LUABSD_INT("ITIMER_REAL",   ITIMER_REAL),
    LUABSD_INT("ITIMER_VIRTUAL",    ITIMER_VIRTUAL),
    LUABSD_INT("ITIMER_PROF",   ITIMER_PROF),
    LUABSD_FUNC("setitimer",    luab_setitimer),
#if __XSI_VISIBLE
    LUABSD_FUNC("getitimer",    luab_getitimer),
#endif
#if __BSD_VISIBLE
    LUABSD_FUNC("StructBinTime",    luab_StructBinTime),
#endif
    LUABSD_FUNC("StructClockInfo",  luab_StructClockInfo),
    LUABSD_FUNC("StructItimerVal",  luab_StructItimerVal),
    LUABSD_FUNC("StructTimeSpec",   luab_StructTimeSpec),
    LUABSD_FUNC("StructTimeZone",   luab_StructTimeZone),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_sys_time_lib = {
    .cookie = LUABSD_SYS_TIME_LIB_ID,
    .name = LUABSD_SYS_TIME_LIB_KEY,
    .vec = luab_sys_time_vec,
};
