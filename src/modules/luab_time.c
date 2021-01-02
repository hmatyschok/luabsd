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

#include <time.h>

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
    time1 = luab_checkxlinteger(L, 1, m, 1);
    time0 = luab_checkxlinteger(L, 2, m, 1);

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

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = strftime(bp, maxsize, format, timeptr)) > 0)
                buf->iov.iov_len = status;
            else
                buf->iov.iov_len = maxsize;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = luab_env_error;
        }
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}



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

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((dp = asctime_r(tm, bp)) != NULL)
                buf->iov.iov_len = strlen(bp);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            dp = NULL;
        }
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

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((dp = ctime_r(clock, bp)) != NULL)
                buf->iov.iov_len = strlen(bp);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            dp = NULL;
        }
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

#if __BSD_VISIBLE
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
#endif /* __BSD_VISIBLE */

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(TM)).
 *
 * @function tm_create
 *
 * @param data          Instance of (LUA_TUSERDATA(TM)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage tm [, err, msg ] = bsd.time.tm_create([ data ])
 */
static int
luab_tm_create(lua_State *L)
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
#if __POSIX_VISIBLE >= 199506
    LUAB_FUNC("asctime_r",                  luab_asctime_r),
    LUAB_FUNC("ctime_r",                    luab_ctime_r),
    LUAB_FUNC("gmtime_r",                   luab_gmtime_r),
    LUAB_FUNC("localtime_r",                luab_localtime_r),
#endif  /* __POSIX_VISIBLE >= 199506 */
#if __BSD_VISIBLE
    LUAB_FUNC("timegm",                     luab_timegm),
#endif /* __BSD_VISIBLE */
    LUAB_FUNC("tm_create",                  luab_tm_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_time_lib = {
    .m_id       = LUAB_TIME_LIB_ID,
    .m_name     = LUAB_TIME_LIB_KEY,
    .m_vec      = luab_time_vec,
};
