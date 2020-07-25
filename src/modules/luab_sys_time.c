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
/*
 * The implementation of the interface against setitimer(2) is derived from:
 *
 * lalarm.c
 * an alarm library for Lua based on signal
 * Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
 * 28 Jul 2018 12:47:52
 * This code is hereby placed in the public domain and also under the MIT license
 */

#include <sys/limits.h>
#include <sys/time.h>

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t timezone_type;
extern int luab_StructTimeZone(lua_State *);

#if __BSD_VISIBLE
extern luab_module_t bintime_type;
extern int luab_StructBinTime(lua_State *);
#endif

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

typedef struct luab_clockinfo {
    struct clockinfo    clockinfo;
} luab_clockinfo_t;

#define luab_new_clockinfo(L, arg) \
    ((luab_clockinfo_t *)luab_newuserdata(L, &clockinfo_type, (arg)))
#define luab_to_clockinfo(L, narg) \
    (luab_todata((L), (narg), &clockinfo_type, luab_clockinfo_t *))

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
static int
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

typedef struct luab_timespec {
    struct timespec    timespec;
} luab_timespec_t;

#define luab_new_timespec(L, arg) \
    ((luab_timespec_t *)luab_newuserdata(L, &timespec_type, (arg)))
#define luab_to_timespec(L, narg) \
    (luab_todata((L), (narg), &timespec_type, luab_timespec_t *))

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
    luab_timespec_t *self;
    time_t tv_sec;

    luab_checkmaxargs(L, 2);

    self = luab_to_timespec(L, 1);
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
 * @usage sec = timespec:get_tv_sec()
 */
static int
TimeSpec_get_tv_sec(lua_State *L)
{
    luab_timespec_t *self;
    time_t tv_sec;

    luab_checkmaxargs(L, 1);

    self = luab_to_timespec(L, 1);
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
 * @usage timespec:set_tv_nsec(nsec)
 */
static int
TimeSpec_set_tv_nsec(lua_State *L)
{
    luab_timespec_t *self;
    long tv_nsec;

    luab_checkmaxargs(L, 2);

    self = luab_to_timespec(L, 1);
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
 * @usage nsec = timespec:get_tv_nsec()
 */
static int
TimeSpec_get_tv_nsec(lua_State *L)
{
    luab_timespec_t *self;
    long tv_nsec;

    luab_checkmaxargs(L, 1);

    self = luab_to_timespec(L, 1);
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
 * @usage t = timespec:get()
 */
static int
TimeSpec_get(lua_State *L)
{
    luab_timespec_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_to_timespec(L, 1);

    lua_newtable(L);

    luab_setinteger(L, -2, "tv_sec", self->timespec.tv_sec);
    luab_setinteger(L, -2, "tv_nsec", self->timespec.tv_nsec);

    lua_pushvalue(L, -1);

    return 1;
}

static int
TimeSpec_gc(lua_State *L)
{
    luab_timespec_t *self;
    luab_module_t *m;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_timespec(L, 1);
    m = &timespec_type;

    (void)memset_s(self, m->sz, 0, m->sz);

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
 * @param tv                    Optional.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage tv = bsd.sys.time.StructTimeSpec([ tv ])
 */
static int
luab_StructTimeSpec(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 1);
    struct timespec *timespec;
    int status;

    if (narg == 0)
        timespec = NULL;
    else
        timespec = timespec_udata(L, narg);

    if (luab_new_timespec(L, timespec) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
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

typedef struct luab_itimerval {
    struct itimerval    itimerval;
} luab_itimerval_t;

#define luab_new_itimerval(L, arg) \
    ((luab_itimerval_t *)luab_newuserdata(L, &itimerval_type, (arg)))
#define luab_to_itimerval(L, narg) \
    (luab_todata((L), (narg), &itimerval_type, luab_itimerval_t *))

/* timer interval */
/***
 * Set timer interval.
 *
 * @function set_it_interval
 *
 * @param tv            Instance of timespec{}.
 *
 * @usage itimerval:set_it_interval(tv)
 */
static int
ItimerVal_set_it_interval(lua_State *L)
{
    luab_itimerval_t *self;
    void *ud;

    luab_checkmaxargs(L, 2);

    self = luab_to_itimerval(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->itimerval.it_interval, ud, timespec_type.sz);

    return 0;
}

/***
 * Get timer interval.
 *
 * @function get_it_interval
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage tv = itimerval:get_it_interval()
 */
static int
ItimerVal_get_it_interval(lua_State *L)
{
    luab_itimerval_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_to_itimerval(L, 1);

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
 * @usage itimerval:set_it_value(tv)
 */
static int
ItimerVal_set_it_value(lua_State *L)
{
    luab_itimerval_t *self;
    void *ud;

    luab_checkmaxargs(L, 2);

    self = luab_to_itimerval(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->itimerval.it_value, ud, timespec_type.sz);

    return 0;
}

/***
 * Get timer value.
 *
 * @function get_it_value
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage tv = itimerval:get_it_value()
 */
static int
ItimerVal_get_it_value(lua_State *L)
{
    luab_itimerval_t *self;
    int status;

    luab_checkmaxargs(L, 1);

    self = luab_to_itimerval(L, 1);

    if (luab_newuserdata(L, &timespec_type, &self->itimerval.it_value) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}

/***
 * Translate itimerval{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = itimerval:get()
 */
static int
ItimerVal_get(lua_State *L)
{
    luab_itimerval_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_to_itimerval(L, 1);

    lua_newtable(L);   /* XXX */

    luab_setudata(L, -2, &timespec_type, "it_interval", &self->itimerval.it_interval);
    luab_setudata(L, -2, &timespec_type, "it_value", &self->itimerval.it_value);

    lua_pushvalue(L, -1);

    return 1;
}

static int
ItimerVal_gc(lua_State *L)
{
    luab_itimerval_t *self;
    luab_module_t *m;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_itimerval(L, 1);
    m = &itimerval_type;

    (void)memset_s(self, m->sz, 0, m->sz);

    return 0;
}

static int
ItimerVal_tostring(lua_State *L)
{
    luab_itimerval_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_itimerval(L, 1);
    lua_pushfstring(L, "itimerval (%p)", self);

    return 1;
}

static luab_table_t itimerval_methods[] = {
    LUABSD_FUNC("set_it_interval",  ItimerVal_set_it_interval),
    LUABSD_FUNC("set_it_value", ItimerVal_set_it_value),
    LUABSD_FUNC("get",  ItimerVal_get),
    LUABSD_FUNC("get_it_interval",  ItimerVal_get_it_interval),
    LUABSD_FUNC("get_it_value", ItimerVal_get_it_value),
    LUABSD_FUNC("__gc", ItimerVal_gc),
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
    luab_itimerval_t *self = luab_to_itimerval(L, narg);

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
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage tv = bsd.sys.time.StructItimerVal()
 */
static int
luab_StructItimerVal(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 1);
    struct itimerval *itimerval;
    int status;

    if (narg == 0)
        itimerval = NULL;
    else
        itimerval = itimerval_udata(L, narg);

    if (luab_new_itimerval(L, itimerval) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
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
h_callout(lua_State *L, lua_Debug *arg __unused)
{
    L = saved_L;

    lua_sethook(L, h, h_msk, h_cnt);
    lua_getfield(L, LUA_REGISTRYINDEX, "l_callout");

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

            lua_sethook(saved_L, h_callout, l_msk, 1);
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
    lua_setfield(L, LUA_REGISTRYINDEX, "l_callout");

    saved_L = L;    /* XXX race condition */

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
