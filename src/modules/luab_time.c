/*
 * Copyright (c) 2020, 2021 Henning Matyschok
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

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_TIME_LIB_ID    1594167179
#define LUAB_TIME_LIB_KEY    "time"

extern luab_module_t luab_time_lib;

/*
 * Service primitives.
 */

/***
 * asctime(3) - transform binary data and time
 *
 * @function asctime
 *
 * @param tm                Specifies broken down time by an
 *                          instance of (LUA_TUSERDATA(TM)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.asctime(tm)
 */
static int
luab_asctime(lua_State *L)
{
    luab_module_t *m;
    struct tm *tm;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TM, TYPE, __func__);
    tm = luab_udata(L, 1, m, struct tm *);

    dp = asctime(tm);

    return (luab_pushstring(L, dp));
}

/***
 * clock(3) - determine processor time used
 *
 * @function clock
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.clock()
 */
static int
luab_clock(lua_State *L)
{
    time_t ret;

    (void)luab_core_checkmaxargs(L, 0);

    ret = clock();
    return (luab_pushxinteger(L, ret));
}

/***
 * ctime(3) - transform binary data and time
 *
 * @function ctime
 *
 * @param clock             Result argument, time value in seconds by
 *                          an instance of (LUA_TUSERDATA(TIME)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.ctime(clock)
 */
static int
luab_ctime(lua_State *L)
{
    luab_module_t *m;
    const time_t *clock;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIME, TYPE, __func__);
    clock = luab_udata(L, 1, m, const time_t *);

    dp = ctime(clock);

    return (luab_pushstring(L, dp));
}

#ifndef _STANDALONE
/***
 * difftime(3) - transform binary data and time
 *
 * @function difftime
 *
 * @param time1             Specifies time value in seconds.
 * @param time0             Same as above.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.ctime(time1, time0)
 */
static int
luab_difftime(lua_State *L)
{
    luab_module_t *m;
    time_t time1;
    time_t time0;
    double ret;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(TIME, TYPE, __func__);
    time1 = (time_t)luab_checklxinteger(L, 1, m, 1);
    time0 = (time_t)luab_checklxinteger(L, 2, m, 1);

    ret = difftime(time1, time0);

    return (luab_pushnumber(L, ret, 0));
}
#endif /* _STANDALONE */

/***
 * gmtime(3) - transform binary data and time
 *
 * @function gmtime
 *
 * @param clock             Value / result argument, specifies time value in
 *                          seconds by an instance of (LUA_TUSERDATA(TIME)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage tm [, err, msg ] = bsd.time.localtime(clock)
 */
static int
luab_gmtime(lua_State *L)
{
    luab_module_t *m0, *m1;
    const time_t *clock;
    struct tm *x;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(TIME, TYPE, __func__);
    m1 = luab_xmod(TM, TYPE, __func__);

    clock = luab_udata(L, 1, m0, const time_t *);
    x = gmtime(clock);

    return (luab_pushxdata(L, m1, x));
}

/***
 * localtime(3) - transform binary data and time
 *
 * @function localtime
 *
 * @param clock             Value / result argument, specifies time value in
 *                          seconds by an instance of (LUA_TUSERDATA(TIME)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage tm [, err, msg ] = bsd.time.localtime(clock)
 */
static int
luab_localtime(lua_State *L)
{
    luab_module_t *m0, *m1;
    const time_t *clock;
    struct tm *x;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(TIME, TYPE, __func__);
    m1 = luab_xmod(TM, TYPE, __func__);

    clock = luab_udata(L, 1, m0, const time_t *);
    x = localtime(clock);

    return (luab_pushxdata(L, m1, x));
}

/***
 * mktime(3) - transform binary data and time
 *
 * @function mktime
 *
 * @param tm                Specifies broken down time by an
 *                          instance of (LUA_TUSERDATA(TM)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.mktime(tm)
 */
static int
luab_mktime(lua_State *L)
{
    luab_module_t *m;
    struct tm *tm;
    time_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TM, TYPE, __func__);
    tm = luab_udata(L, 1, m, struct tm *);
    x = mktime(tm);

    return (luab_pushxinteger(L, x));
}

/***
 * strftime(3) - format data and time
 *
 * @function strftime
 *
 * @param buf               Buffer for formatted time information by
 *                          an instance of (LUA_TUSERDATA(IOVEC)).
 * @param maxsize           Specifies constraint for buffer.
 * @param format            Specifies format string for conversion.
 * @param timeptr           Specifies broken down time by an
 *                          instance of (LUA_TUSERDATA(TM)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.strftime(buf, maxsize, format, timeptr)
 */
static int
luab_strftime(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    luab_iovec_t *buf;
    size_t maxsize;
    const char *format;
    struct tm *timeptr;
    caddr_t bp;
    ssize_t status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);
    m2 = luab_xmod(TM, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    maxsize = (size_t)luab_checklxinteger(L, 2, m1, 0);
    format = luab_checklstring(L, 3, maxsize, NULL);
    timeptr = luab_udata(L, 4, m2, struct tm *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (maxsize <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if ((status = strftime(bp, maxsize, format, timeptr)) > 0)
            buf->iov.iov_len = status;
        else
            buf->iov.iov_len = maxsize;

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * time(3) - get time of day
 *
 * @function strftime
 *
 * @param tloc              Result argument, tnstance of (LUA_TUSERDATA(TIME)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.time(tloc)
 */
static int
luab_time(lua_State *L)
{
    luab_module_t *m;
    time_t *tloc, x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIME, TYPE, __func__);

    tloc = luab_udata(L, 1, m, time_t *);
    x = time(tloc);

    return (luab_pushxinteger(L, x));
}

#if __POSIX_VISIBLE >= 200112
/***
 * timer_create(2) - create a per-process timer (REALTIME)
 *
 * @function timer_create
 *
 * @param clockid           Specifies per-process utilized clock, by an
 *                          instance of (LUA_T{NUMBER,USERDATA(CLOCKID)}).
 * @param evp               Defines asynchronous notification by an instance
 *                          of (LUA_TUSERDATA(SIGEVENT)), when timer expires.
 * @param timerid           Specifies the location of per-process used timer.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.timer_create(clockid, evp, timerid)
 */
static int
luab_timer_create(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    clockid_t clockid;
    struct sigevent *evp;
    luab_timer_t *xtmr;
    timer_t timerid;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(CLOCKID, TYPE, __func__);
    m1 = luab_xmod(SIGEVENT, TYPE, __func__);
    m2 = luab_xmod(TIMER, TYPE, __func__);

    clockid = (clockid_t)luab_checkxinteger(L, 1, m0, luab_env_uint_max);
    evp = luab_udataisnil(L, 2, m1, struct sigevent *);
    xtmr = luab_udata(L, 3, m2, luab_timer_t *);

    if ((timerid = xtmr->ud_sdu) != NULL)
        status = timer_create(clockid, evp, &timerid);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * timer_delete(2) - delete a per-process timer (REALTIME)
 *
 * @function timer_delete
 *
 * @param timerid           Specifies the location of per-process timer
 *                          by an instance of (LUA_TUSERDATA(TIMER)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.timer_delete(timerd)
 */
static int
luab_timer_delete(lua_State *L)
{
    luab_module_t *m;
    luab_timer_t *xtmr;
    timer_t timerid;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIMER, TYPE, __func__);
    xtmr = luab_udata(L, 1, m, luab_timer_t *);

    if ((timerid = xtmr->ud_sdu) != NULL)
        status = timer_delete(timerid);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * timer_gettime(2) - per-process timers (REALTIME)
 *
 * @function timer_gettime
 *
 * @param timerid           Specifies the location of per-process timer
 *                          by an instance of (LUA_TUSERDATA(TIMER)).
 * @param value             Contains amount of time until the timer expires.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.timer_gettime(timerid, value)
 */
static int
luab_timer_gettime(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_timer_t *xtmr;
    struct itimerspec *value;
    timer_t timerid;
    int status;

    m0 = luab_xmod(TIMER, TYPE, __func__);
    m1 = luab_xmod(ITIMERSPEC, TYPE, __func__);

    xtmr = luab_udata(L, 1, m0, luab_timer_t *);
    value = luab_udata(L, 2, m1, struct itimerspec *);

    if ((timerid = xtmr->ud_sdu) != NULL)
        status = timer_gettime(timerid, value);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * timer_getoverrun(2) - per-process timers (REALTIME)
 *
 * @function timer_getoverrun
 *
 * @param timerid           Specifies the location of per-process timer
 *                          by an instance of (LUA_TUSERDATA(TIMER)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.timer_getoverrun(timerid)
 */
static int
luab_timer_getoverrun(lua_State *L)
{
    luab_module_t *m;
    luab_timer_t *xtmr;
    timer_t timerid;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIMER, TYPE, __func__);
    xtmr = luab_udata(L, 1, m, luab_timer_t *);

    if ((timerid = xtmr->ud_sdu) != NULL)
        status = timer_getoverrun(timerid);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * timer_settime(2) - per-process timers (REALTIME)
 *
 * @function timer_settime
 *
 * @param timerid           Specifies the location of per-process timer
 *                          by an instance of (LUA_TUSERDATA(TIMER)).
 * @param flags             Specifies utilized per-process timer.
 * @param value             Specifies by (LUA_TUSERDATA(ITIMERSPEC)) the amount
 *                          of time, until the timer expires.
 * @param ovalue            Stores previous amount of time by an instance of
 *                          (LUA_TUSERDATA(ITIMERSPEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.timer_settime(timerid, value)
 */
static int
luab_timer_settime(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    luab_timer_t *xtmr;
    int flags;
    struct itimerspec *value;
    struct itimerspec *ovalue;
    timer_t timerid;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(TIMER, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);
    m2 = luab_xmod(ITIMERSPEC, TYPE, __func__);

    xtmr = luab_udata(L, 1, m0, luab_timer_t *);
    flags = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);
    value = luab_udata(L, 3, m2, struct itimerspec *);
    ovalue = luab_udataisnil(L, 4, m2, struct itimerspec *);

    if ((timerid = xtmr->ud_sdu) != NULL)
        status = timer_settime(timerid, flags, value, ovalue);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}


#endif /* __POSIX_VISIBLE >= 200112 */

#if __BSD_VISIBLE
/***
 * tzset(3) - initialize time conversion information
 *
 * @function tzset
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.tzset()
 */
static int
luab_tzset(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);
    tzset();
    return (luab_pushxinteger(L, luab_env_success));
}
#endif /* __BSD_VISIBLE */

#if __POSIX_VISIBLE >= 199309
/***
 * clock_getres(2) - calibrate date and time
 *
 * @function clock_getres
 *
 * @param clock_id          Specifies the location of per-process timer, by
 *                          an instance of (LUA_T{NUMBER,USERDATA(CLOCKID)}).
 * @param tp                Value / result argument, (LUA_TUSERDATA(TIMESPEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.clock_getres(clock_id, tp)
 */
static int
luab_clock_getres(lua_State *L)
{
    luab_module_t *m0, *m1;
    clockid_t clock_id;
    struct timespec *tp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(CLOCKID, TYPE, __func__);
    m1 = luab_xmod(TIMESPEC, TYPE, __func__);

    clock_id = (clockid_t)luab_checkxinteger(L, 1, m0, luab_env_uint_max);
    tp = luab_udata(L, 2, m1, struct timespec *);
    status = clock_getres(clock_id, tp);

    return (luab_pushxinteger(L, status));
}

/***
 * clock_gettime(2) - get date and time
 *
 * @function clock_gettime
 *
 * @param clock_id          Specifies the location of per-process timer, by
 *                          an instance of (LUA_T{NUMBER,USERDATA(CLOCKID)}).
 * @param tp                Value / result argument, (LUA_TUSERDATA(TIMESPEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.clock_gettime(clock_id, tp)
 */
static int
luab_clock_gettime(lua_State *L)
{
    luab_module_t *m0, *m1;
    clockid_t clock_id;
    struct timespec *tp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(CLOCKID, TYPE, __func__);
    m1 = luab_xmod(TIMESPEC, TYPE, __func__);

    clock_id = (clockid_t)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    tp = luab_udata(L, 2, m1, struct timespec *);
    status = clock_gettime(clock_id, tp);

    return (luab_pushxinteger(L, status));
}

/***
 * clock_settime(2) - set date and time
 *
 * @function clock_gettime
 *
 * @param clock_id          Specifies the location of per-process timer, by
 *                          an instance of (LUA_T{NUMBER,USERDATA(CLOCKID)}).
 * @param tp                Value / result argument, (LUA_TUSERDATA(TIMESPEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.clock_settime(clock_id, tp)
 */
static int
luab_clock_settime(lua_State *L)
{
    luab_module_t *m0, *m1;
    clockid_t clock_id;
    struct timespec *tp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(CLOCKID, TYPE, __func__);
    m1 = luab_xmod(TIMESPEC, TYPE, __func__);

    clock_id = (clockid_t)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    tp = luab_udata(L, 2, m1, struct timespec *);
    status = clock_settime(clock_id, tp);

    return (luab_pushxinteger(L, status));
}

/***
 * nanosleep(2) - high resoloution sleep
 *
 * @function nanosleep
 *
 * @param rqtp              Requested time interval, (LUA_TUSERDATA(TIMESPEC)).
 * @param rmtp              Result argument, remaining amount of time, either by
 *                          an instance of(LUA_TUSERDATA(TIMESPEC)) or (LUA_TNIL).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.nanosleep(rqtp, rmtp)
 */
static int
luab_nanosleep(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    clockid_t clock_id;
    int flags;
    struct timespec *rqtp;
    struct timespec *rmtp;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(CLOCKID, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);
    m2 = luab_xmod(TIMESPEC, TYPE, __func__);

    clock_id = (clockid_t)luab_checkxinteger(L, 1, m0, luab_env_uint_max);
    flags = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);
    rqtp = luab_udata(L, 3, m2, struct timespec *);
    rmtp = luab_udataisnil(L, 4, m2, struct timespec *);

    status = nanosleep(rqtp, rmtp);

    return (luab_pushxinteger(L, status));
}
#endif /* __POSIX_VISIBLE >= 199309 */

#if __POSIX_VISIBLE >= 200112
/***
 * clock_getcpuclockid(2) - access a process CPU-time clock
 *
 * @function clock_getcpuclockid
 *
 * @param clock_id          Specifies the location of per-process timer, by
 *                          an instance of (LUA_T{NUMBER,USERDATA(CLOCKID)}).
 * @param flags             Specifies type of per-process utilized clock.
 * @param rqtp              Requested time interval, (LUA_TUSERDATA(TIMESPEC)).
 * @param rmtp              Result argument, remaining amount of time, either by
 *                          an instance of(LUA_TUSERDATA(TIMESPEC)) or (LUA_TNIL).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.clock_getcpuclockid(clock_id, tp)
 */
static int
luab_clock_getcpuclockid(lua_State *L)
{
    luab_module_t *m0, *m1;
    pid_t pid;
    clockid_t *clock_id;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PID, TYPE, __func__);
    m1 = luab_xmod(CLOCKID, TYPE, __func__);

    pid = (pid_t)luab_checkxinteger(L, 1, m0, luab_env_uint_max);
    clock_id = luab_udata(L, 2, m1, clockid_t *);
    status = clock_getcpuclockid(pid, clock_id);

    return (luab_pushxinteger(L, status));
}

/***
 * clock_nanosleep(2) - high resoloution sleep
 *
 * @function clock_nanosleep
 *
 * @param clock_id          Specifies the location of per-process used timer.
 * @param flags             Specifies type of per-process utilized clock.
 * @param rqtp              Requested time interval, (LUA_TUSERDATA(TIMESPEC)).
 * @param rmtp              Result argument, remaining amount of time, either by
 *                          an instance of(LUA_TUSERDATA(TIMESPEC)) or (LUA_TNIL).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.clock_nanosleep(clock_id, tp)
 */
static int
luab_clock_nanosleep(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    clockid_t clock_id;
    int flags;
    struct timespec *rqtp;
    struct timespec *rmtp;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(CLOCKID, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);
    m2 = luab_xmod(TIMESPEC, TYPE, __func__);

    clock_id = (clockid_t)luab_checkxinteger(L, 1, m0, luab_env_uint_max);
    flags = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);
    rqtp = luab_udata(L, 3, m2, struct timespec *);
    rmtp = luab_udataisnil(L, 4, m2, struct timespec *);

    status = clock_nanosleep(clock_id, flags, rqtp, rmtp);
    return (luab_pushxinteger(L, status));
}
#endif /* __POSIX_VISIBLE >= 200112 */

#if __POSIX_VISIBLE >= 199506
/***
 * asctime_r(3) - transform binary data and time
 *
 * @function asctime_r
 *
 * @param tm                Value  argument, specifies time value
 *                          by an instance of (LUA_TUSERDATA(TM)).
 * @param buf               Result argument, buffer  with a minimum capacity
 *                          at least of 26 bytes for timestamp by an
 *                          instance of (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.asctime_r(tm, buf)
 */
static int
luab_asctime_r(lua_State *L)
{
    luab_module_t *m0, *m1;
    const struct tm *tm;
    luab_iovec_t *buf;
    caddr_t bp, dp;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(TM, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);

    tm = luab_udata(L, 1, m0, const struct tm *);
    buf = luab_udata(L, 2, m1, luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (buf->iov_max_len >= luab_env_ts_min) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if ((dp = asctime_r(tm, bp)) != NULL)
            buf->iov.iov_len = strlen(bp);

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        dp = NULL;
    }
    return (luab_pushstring(L, dp));
}

/***
 * ctime_r(3) - transform binary data and time
 *
 * @function ctime_r
 *
 * @param clock             Value / result argument, specifies time value in
 *                          seconds by an instance of (LUA_TUSERDATA(TIME)).
 * @param buf               Result argument, buffer  with a minimum capacity
 *                          at least of 26 bytes for timestamp by an
 *                          instance of (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.ctime_r(clock, buf)
 */
static int
luab_ctime_r(lua_State *L)
{
    luab_module_t *m0, *m1;
    const time_t *clock;
    luab_iovec_t *buf;
    caddr_t bp, dp;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(TIME, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);

    clock = luab_udata(L, 1, m0, const time_t *);
    buf = luab_udata(L, 2, m1, luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (buf->iov_max_len >= luab_env_ts_min) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if ((dp = ctime_r(clock, bp)) != NULL)
            buf->iov.iov_len = strlen(bp);

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        dp = NULL;
    }
    return (luab_pushstring(L, dp));
}

/***
 * gmtime_r(3) - transform binary data and time
 *
 * @function gmtime_r
 *
 * @param clock             Value / result argument, specifies time value in
 *                          seconds by an instance of (LUA_TUSERDATA(TIME)).
 * @param result            Result argument, buffer for tm{} structure by
 *                          an instance of (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage tm [, err, msg ] = bsd.time.gmtime_r(clock, result)
 */
static int
luab_gmtime_r(lua_State *L)
{
    luab_module_t *m0, *m1;
    const time_t *clock;
    struct tm *result;
    struct tm *x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(TIME, TYPE, __func__);
    m1 = luab_xmod(TM, TYPE, __func__);

    clock = luab_udata(L, 1, m0, const time_t *);
    result = luab_udata(L, 2, m1, struct tm *);

    x = gmtime_r(clock, result);

    return (luab_pushxdata(L, m1, x));
}

/***
 * localtime_r(3) - transform binary data and time
 *
 * @function localtime_r
 *
 * @param clock             Value / result argument, specifies time value in
 *                          seconds by an instance of (LUA_TUSERDATA(TIME)).
 * @param result            Result argument, buffer for tm{} structure by
 *                          an instance of (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage tm [, err, msg ] = bsd.time.localtime_r(clock, result)
 */
static int
luab_localtime_r(lua_State *L)
{
    luab_module_t *m0, *m1;
    const time_t *clock;
    struct tm *result;
    struct tm *x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(TIME, TYPE, __func__);
    m1 = luab_xmod(TM, TYPE, __func__);

    clock = luab_udata(L, 1, m0, const time_t *);
    result = luab_udata(L, 2, m1, struct tm *);

    x = localtime_r(clock, result);

    return (luab_pushxdata(L, m1, x));
}
#endif /* __POSIX_VISIBLE >= 199506 */

#if __XSI_VISIBLE
/***
 * strptime(3) - parse data and time string
 *
 * @function strptime
 *
 * @param buf               String buffer, instance of (LUA_TUSERDATA(TIME)).
 * @param format            Format string, (LUA_TSTRING).
 * @param timeptr           Result argument, instance of (LUA_TUSERDATA(TM)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage tm [, err, msg ] = bsd.time.strptime(clock, result)
 */
static int
luab_strptime(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *buf;
    const char *format;
    struct tm *timeptr;
    caddr_t bp, dp;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(TM, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    format = luab_checklstring(L, 2, luab_env_buf_max, NULL);
    timeptr = luab_udata(L, 3, m1, struct tm *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if ((dp = strptime(bp, format, timeptr)) != NULL)
            buf->iov.iov_len = strnlen(dp, luab_env_buf_max);

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        dp = NULL;
    }
    return (luab_pushstring(L, dp));
}
#endif /* __XSI_VISIBLE */

#if __BSD_VISIBLE
/***
 * timezone(3) - return the timezone abbreviation
 *
 * @function timezone
 *
 * @param zone              Specifies zone by (LUA_T{NUMBER,USERDATA(INT)}).
 * @param dst               Specifies destinaton by (LUA_T{NIL,USERDATA(INT)}).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.timegm(tm)
 */
static int
luab_timezone(lua_State *L)
{
    luab_module_t *m;
    int zone, dst;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(INT, TYPE, __func__);

    zone = (int)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    dst = (int)luab_checkxinteger(L, 2, m, luab_env_uint_max);

    dp = timezone(zone, dst);
    return (luab_pushstring(L, dp));
}

/***
 * tzsetwall(3) - initialize time conversion information
 *
 * @function tzsetwall
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.tzsetwall()
 */
static int
luab_tzsetwall(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);
    tzsetwall();
    return (luab_pushxinteger(L, luab_env_success));
}

/***
 * timelocal(3) - transform binary data and time
 *
 * @function timelocal
 *
 * @param tm                Specifies broken down time by an
 *                          instance of (LUA_TUSERDATA(TM)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.timelocal(tm)
 */
static int
luab_timelocal(lua_State *L)
{
    luab_module_t *m;
    struct tm *tm;
    time_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TM, TYPE, __func__);
    tm = luab_udata(L, 1, m, struct tm *);

    x = timelocal(tm);

    return (luab_pushxinteger(L, x));
}

/***
 * timegm(3) - transform binary data and time
 *
 * @function timegm
 *
 * @param tm                Specifies broken down time by an
 *                          instance of (LUA_TUSERDATA(TM)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.timegm(tm)
 */
static int
luab_timegm(lua_State *L)
{
    luab_module_t *m;
    struct tm *tm;
    time_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TM, TYPE, __func__);
    tm = luab_udata(L, 1, m, struct tm *);

    x = timegm(tm);

    return (luab_pushxinteger(L, x));
}

/***
 * timer_oshandle_np(3) - fetch oshandle property
 *
 * @function timer_oshandle_np
 *
 * @param timerid           Specifies the location of per-process timer
 *                          by an instance of (LUA_TUSERDATA(TIMER)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.timer_oshandle_np(timerid)
 */
static int
luab_timer_oshandle_np(lua_State *L)
{
    luab_module_t *m;
    luab_timer_t *xtmr;
    timer_t timerid;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIMER, TYPE, __func__);

    xtmr = luab_udata(L, 1, m, luab_timer_t *);

    if ((timerid = xtmr->ud_sdu) != NULL)
        status = timer_oshandle_np(timerid);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * time2posix(3) - convert seconds since the Epoch
 *
 * @function time2posix
 *
 * @param t                 Specifies time value by an instance
 *                          of (LUA_T{NUMBER,USERDATA(TIMER))}.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.time2posix(t)
 */
static int
luab_time2posix(lua_State *L)
{
    luab_module_t *m;
    time_t t, x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIME, TYPE, __func__);
    t = luab_checklxinteger(L, 1, m, 0);
    x = time2posix(t);

    return (luab_pushxinteger(L, x));
}

/***
 * posix2time(3) - convert seconds since the Epoch
 *
 * @function posix2time
 *
 * @param t                 Specifies time value by an instance
 *                          of (LUA_T{NUMBER,USERDATA(TIMER))}.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.time.posix2time(t)
 */
static int
luab_posix2time(lua_State *L)
{
    luab_module_t *m;
    time_t t, x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIME, TYPE, __func__);
    t = luab_checklxinteger(L, 1, m, 0);
    x = posix2time(t);

    return (luab_pushxinteger(L, x));
}
#endif /* __BSD_VISIBLE */

/*
 * Generator functions.
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(CLOCK)).
 *
 * @function clock_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(CLOCK)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage clock [, err, msg ] = bsd.time.clock_create(arg)
 */
static int
luab_type_clock_create(lua_State *L)
{
    luab_module_t *m;
    clock_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CLOCK, TYPE, __func__);
    x = (clock_t)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(TIME)).
 *
 * @function create_time
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(TIME)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage time [, err, msg ] = bsd.time.create_time(arg)
 */
static int
luab_type_create_time(lua_State *L)
{
    luab_module_t *m;
    time_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIME, TYPE, __func__);
    x = (time_t)luab_checkxinteger(L, 1, m, luab_env_ulong_max);
    return (luab_pushxdata(L, m, &x));
}

#if __POSIX_VISIBLE >= 199309
/***
 * Generator function, creates an instance of (LUA_TUSERDATA(CLOCKID)).
 *
 * @function create_clockid
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(CLOCKID)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage clockid [, err, msg ] = bsd.time.create_clockid(arg)
 */
static int
luab_type_create_clockid(lua_State *L)
{
    luab_module_t *m;
    clockid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CLOCKID, TYPE, __func__);
    x = (clockid_t)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(TIMER)).
 *
 * @function create_timer
 *
 * @param arg               Specifies its initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(TIMER)}).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage time [, err, msg ] = bsd.time.create_timer(arg)
 */
static int
luab_type_create_timer(lua_State *L)
{
    luab_module_t *m;
    luab_timer_t *xtmr;
    timer_t timer;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIMER, TYPE, __func__);

    if ((xtmr = luab_udataisnil(L, 1, m, luab_timer_t *)) != NULL)
        timer = xtmr->ud_sdu;
    else {
        errno = ENOENT;
        timer = NULL;
    }
    return (luab_pushxdata(L, m, timer));
}
#endif /* __POSIX_VISIBLE >= 199309 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(TM)).
 *
 * @function tm_create
 *
 * @param arg           Instance of (LUA_TUSERDATA(TM)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage tm [, err, msg ] = bsd.time.tm_create([ arg ])
 */
static int
luab_type_create_tm(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TM, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/*
 * Interface against <time.h>.
 */

static luab_module_table_t luab_time_vec[] = { /* time.h */
#if __POSIX_VISIBLE > 0 && __POSIX_VISIBLE < 200112 || __BSD_VISIBLE
    LUAB_INT("CLK_TCK",                     CLK_TCK),
#endif
#if !defined(CLOCK_REALTIME) && __POSIX_VISIBLE >= 200112
    LUAB_INT("CLOCK_REALTIME",              CLOCK_REALTIME),
#ifdef __BSD_VISIBLE
    LUAB_INT("CLOCK_VIRTUAL",               CLOCK_VIRTUAL),
    LUAB_INT("CLOCK_PROF",                  CLOCK_PROF),
#endif
    LUAB_INT("CLOCK_MONOTONIC",             CLOCK_MONOTONIC),
    LUAB_INT("CLOCK_UPTIME",                CLOCK_UPTIME),
    LUAB_INT("CLOCK_UPTIME_PRECISE",        CLOCK_UPTIME_PRECISE),
    LUAB_INT("CLOCK_UPTIME_FAST",           CLOCK_UPTIME_FAST),
    LUAB_INT("CLOCK_REALTIME_PRECISE",      CLOCK_REALTIME_PRECISE),
    LUAB_INT("CLOCK_REALTIME_FAST",         CLOCK_REALTIME_FAST),
    LUAB_INT("CLOCK_MONOTONIC_PRECISE",     CLOCK_MONOTONIC_PRECISE),
    LUAB_INT("CLOCK_MONOTONIC_FAST",        CLOCK_MONOTONIC_FAST),
    LUAB_INT("CLOCK_SECOND",                CLOCK_SECOND),
    LUAB_INT("CLOCK_THREAD_CPUTIME_ID",     CLOCK_THREAD_CPUTIME_ID),
    LUAB_INT("CLOCK_PROCESS_CPUTIME_ID",    CLOCK_PROCESS_CPUTIME_ID),
#endif /* !defined(CLOCK_REALTIME) && __POSIX_VISIBLE >= 200112 */
#if !defined(TIMER_ABSTIME) && __POSIX_VISIBLE >= 200112
#if __BSD_VISIBLE
    LUAB_INT("TIMER_RELTIME",               TIMER_RELTIME),
#endif
    LUAB_INT("TIMER_ABSTIME",               TIMER_ABSTIME),
#endif /* !defined(TIMER_ABSTIME) && __POSIX_VISIBLE >= 200112 */
    LUAB_INT("CLOCKS_PER_SEC",              CLOCKS_PER_SEC),
    LUAB_FUNC("asctime",                    luab_asctime),
    LUAB_FUNC("clock",                      luab_clock),
    LUAB_FUNC("ctime",                      luab_ctime),
#ifndef _STANDALONE
    LUAB_FUNC("difftime",                   luab_difftime),
#endif
    LUAB_FUNC("gmtime",                     luab_gmtime),
    LUAB_FUNC("localtime",                  luab_localtime),
    LUAB_FUNC("mktime",                     luab_mktime),
    LUAB_FUNC("strftime",                   luab_strftime),
    LUAB_FUNC("time",                       luab_time),
#if __POSIX_VISIBLE >= 200112
    LUAB_FUNC("timer_create",               luab_timer_create),
    LUAB_FUNC("timer_delete",               luab_timer_delete),
    LUAB_FUNC("timer_gettime",              luab_timer_gettime),
    LUAB_FUNC("timer_getoverrun",           luab_timer_getoverrun),
    LUAB_FUNC("timer_settime",              luab_timer_settime),
#endif /* __POSIX_VISIBLE >= 200112 */
#if __BSD_VISIBLE
    LUAB_FUNC("tzset",                      luab_tzset),
#endif /* __BSD_VISIBLE */
#if __POSIX_VISIBLE >= 199309
    LUAB_FUNC("clock_getres",               luab_clock_getres),
    LUAB_FUNC("clock_gettime",              luab_clock_gettime),
    LUAB_FUNC("clock_settime",              luab_clock_settime),
    LUAB_FUNC("nanosleep",                  luab_nanosleep),
#endif /* __POSIX_VISIBLE >= 199309 */
#if __POSIX_VISIBLE >= 200112
    LUAB_FUNC("clock_getcpuclockid",        luab_clock_getcpuclockid),
    LUAB_FUNC("clock_nanosleep",            luab_clock_nanosleep),
#endif /* __POSIX_VISIBLE */
#if __POSIX_VISIBLE >= 199506
    LUAB_FUNC("asctime_r",                  luab_asctime_r),
    LUAB_FUNC("ctime_r",                    luab_ctime_r),
    LUAB_FUNC("gmtime_r",                   luab_gmtime_r),
    LUAB_FUNC("localtime_r",                luab_localtime_r),
#endif  /* __POSIX_VISIBLE >= 199506 */
#if __XSI_VISIBLE
    LUAB_FUNC("strptime",                   luab_strptime),
#endif
#if __BSD_VISIBLE
    LUAB_FUNC("timezone",                   luab_timezone),
    LUAB_FUNC("tzsetwall",                  luab_tzsetwall),
    LUAB_FUNC("timelocal",                  luab_timelocal),
    LUAB_FUNC("timegm",                     luab_timegm),
    LUAB_FUNC("timer_oshandle_np",          luab_timer_oshandle_np),
    LUAB_FUNC("time2posix",                 luab_time2posix),
    LUAB_FUNC("posix2time",                 luab_posix2time),
#endif /* __BSD_VISIBLE */
    LUAB_FUNC("clock_create",               luab_type_clock_create),
    LUAB_FUNC("create_time",                luab_type_create_time),
#if __POSIX_VISIBLE >= 199309
    LUAB_FUNC("create_clockid",             luab_type_create_clockid),
    LUAB_FUNC("create_timer",               luab_type_create_timer),
#endif /* __POSIX_VISIBLE >= 199309 */
    LUAB_FUNC("create_tm",                  luab_type_create_tm),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_time_lib = {
    .m_id       = LUAB_TIME_LIB_ID,
    .m_name     = LUAB_TIME_LIB_KEY,
    .m_vec      = luab_time_vec,
};
