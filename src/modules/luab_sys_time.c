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

#include <pthread.h>
#include <signal.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_SYS_TIME_LIB_ID    1593623310
#define LUAB_SYS_TIME_LIB_KEY    "time"

extern luab_module_t luab_sys_time_lib;

/*
 * Subr.
 */

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

/*
 * Service primitves.
 */

static int
luab_setitimer(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 4);
    int which;
    struct itimerval *value;
    struct itimerval *ovalue;
    int status;

    which = (int)luab_checkinteger(L, 1, INT_MAX);
    value = luab_udataisnil(L, 2, luab_mx(ITIMERVAL), struct itimerval *);
    ovalue = luab_udataisnil(L, 3, luab_mx(ITIMERVAL), struct itimerval *);

    if (lua_type(L, narg) != LUA_TFUNCTION) /* XXX */
        return luaL_error(L, "Missing callout handler.");

    lua_settop(L, narg);
    lua_setfield(L, LUA_REGISTRYINDEX, "l_callout");

    saved_L = L;    /* XXX race condition */

    if ((status = sigfillset(&nsigset)) != 0)
        goto out;

    if ((status = pthread_sigmask(SIG_BLOCK, &nsigset, NULL)) != 0)
        goto out;

    if ((status = pthread_create(&tid, NULL, h_signal, NULL)) != 0)
        goto out;

    if ((status = setitimer(which, value, ovalue)) != 0)
        pthread_cancel(tid);
out:
    return (luab_pusherr(L, status));
}

#if __XSI_VISIBLE
static int
luab_getitimer(lua_State *L)
{
    int which;
    struct itimerval *value;
    int status;

    (void)luab_checkmaxargs(L, 2);

    which = (int)luab_checkinteger(L, 1, INT_MAX);
    value = luab_udata(L, 2, luab_mx(ITIMERVAL), struct itimerval *);

    status = getitimer(which, value);

    return (luab_pusherr(L, status));
}
#endif

/*
 * Generator functions.
 */

#if __BSD_VISIBLE
/***
 * Generator function - create an instance of (LUA_TUSERDATA(BINTIME)).
 *
 * @function bintime_create
 *
 * @param data          Instance of (LUA_TUSERDATA(BINTIME)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (bintime [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage bintime [, err, msg ] = bsd.sys.time.bintime_create([ data ])
 */
static int
luab_bintime_create(lua_State *L)
{
    return (luab_create(L, 1, luab_mx(BINTIME), NULL));
}
#endif

/***
 * Generator function - create an instance of (LUA_TUSERDATA(CLOCKINFO)).
 *
 * @function clockinfo_create
 *
 * @param data          Instance of (LUA_TUSERDATA(CLOCKINFO)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (clockinfo [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage clockinfo [, err, msg ] = bsd.sys.time.clockinfo_create([ data ])
 */
static int
luab_clockinfo_create(lua_State *L)
{
    return (luab_create(L, 1, luab_mx(CLOCKINFO), NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(ITIMERVAL)).
 *
 * @function itimerval_create
 *
 * @param data          Instance of (LUA_TUSERDATA(ITIMERVAL)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (itimerval [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage itimerval [, err, msg ] = bsd.sys.time.itimerval_create([ data ])
 */
static int
luab_itimerval_create(lua_State *L)
{
    return (luab_create(L, 1, luab_mx(ITIMERVAL), NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(TIMESPEC)).
 *
 * @function timespec_create
 *
 * @param data          Instance of (LUA_TUSERDATA(TIMESPEC)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (timespec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage timespec [, err, msg ] = bsd.sys.time.timespec_create([ data ])
 */
static int
luab_timespec_create(lua_State *L)
{
    return (luab_create(L, 1, luab_mx(TIMESPEC), NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(TIMEVAL)).
 *
 * @function timeval_create
 *
 * @param data          Instance of (LUA_TUSERDATA(TIMEVAL)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (timeval [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage timeval [, err, msg ] = bsd.sys.time.timeval_create([ data ])
 */
static int
luab_timeval_create(lua_State *L)
{
    return (luab_create(L, 1, luab_mx(TIMEVAL), NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(TIMEZONE)).
 *
 * @function timezone_create
 *
 * @param data          Instance of (LUA_TUSERDATA(TIMEZONE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (timezone [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage timezone [, err, msg ] = bsd.sys.time.timezone_create([ data ])
 */
static int
luab_timezone_create(lua_State *L)
{
    return (luab_create(L, 1, luab_mx(TIMEZONE), NULL));
}

/*
 * Interface against <sys/time.h>.
 */

static luab_module_table_t luab_sys_time_vec[] = { /* sys/time.h */
    LUAB_INT("DST_NONE",                  DST_NONE),
    LUAB_INT("DST_USA",                   DST_USA),
    LUAB_INT("DST_AUST",                  DST_AUST),
    LUAB_INT("DST_WET",                   DST_WET),
    LUAB_INT("DST_MET",                   DST_MET),
    LUAB_INT("DST_EET",                   DST_EET),
    LUAB_INT("DST_CAN",                   DST_CAN),
#ifndef CLOCK_REALTIME
    LUAB_INT("CLOCK_REALTIME",            CLOCK_REALTIME),
    LUAB_INT("CLOCK_VIRTUAL",             CLOCK_VIRTUAL),
    LUAB_INT("CLOCK_PROF",                CLOCK_PROF),
    LUAB_INT("CLOCK_MONOTONIC",           CLOCK_MONOTONIC),
    LUAB_INT("CLOCK_UPTIME",              CLOCK_UPTIME),
    LUAB_INT("CLOCK_UPTIME_PRECISE",      CLOCK_UPTIME_PRECISE),
    LUAB_INT("CLOCK_UPTIME_FAST",         CLOCK_UPTIME_FAST),
    LUAB_INT("CLOCK_REALTIME_PRECISE",    CLOCK_REALTIME_PRECISE),
    LUAB_INT("CLOCK_REALTIME_FAST",       CLOCK_REALTIME_FAST),
    LUAB_INT("CLOCK_MONOTONIC_PRECISE",   CLOCK_MONOTONIC_PRECISE),
    LUAB_INT("CLOCK_MONOTONIC_FAST",      CLOCK_MONOTONIC_FAST),
    LUAB_INT("CLOCK_SECOND",              CLOCK_SECOND),
    LUAB_INT("CLOCK_THREAD_CPUTIME_ID",   CLOCK_THREAD_CPUTIME_ID),
    LUAB_INT("CLOCK_PROCESS_CPUTIME_ID",  CLOCK_PROCESS_CPUTIME_ID),
#endif
#ifndef TIMER_ABSTIME
    LUAB_INT("TIMER_RELTIME",             TIMER_RELTIME),
    LUAB_INT("TIMER_ABSTIME",             TIMER_ABSTIME),
#endif
#if __BSD_VISIBLE
    LUAB_INT("CPUCLOCK_WHICH_PID",        CPUCLOCK_WHICH_PID),
    LUAB_INT("CPUCLOCK_WHICH_TID",        CPUCLOCK_WHICH_TID),
#endif
    LUAB_INT("ITIMER_REAL",               ITIMER_REAL),
    LUAB_INT("ITIMER_VIRTUAL",            ITIMER_VIRTUAL),
    LUAB_INT("ITIMER_PROF",               ITIMER_PROF),
    LUAB_FUNC("setitimer",                luab_setitimer),
#if __XSI_VISIBLE
    LUAB_FUNC("getitimer",                luab_getitimer),
#endif
#if __BSD_VISIBLE
    LUAB_FUNC("bintime_create",           luab_bintime_create),
#endif
    LUAB_FUNC("clockinfo_create",         luab_clockinfo_create),
    LUAB_FUNC("itimerval_create",         luab_itimerval_create),
    LUAB_FUNC("timespec_create",          luab_timespec_create),
    LUAB_FUNC("timeval_create",           luab_timeval_create),
    LUAB_FUNC("timezone_create",          luab_timezone_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_time_lib = {
    .m_cookie   = LUAB_SYS_TIME_LIB_ID,
    .m_name     = LUAB_SYS_TIME_LIB_KEY,
    .m_vec      = luab_sys_time_vec,
};
