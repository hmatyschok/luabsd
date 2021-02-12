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

static void *
luab_h_itimer(void *arg)
{
    luab_thread_t *thr;
    int sig, cv;

    if ((thr = (luab_thread_t *)arg) != NULL) {

        cv = 1;

        while (cv != 0) {

            if (sigwait(&thr->thr_sigset, &sig) == 0) {

                switch (sig) {
                case SIGALRM:
                case SIGVTALRM:
                case SIGPROF:

                    (void)luab_thread_pcall(arg);
                default:
                    cv = 0;
                    break;
                }
            }
        }
    }
    pthread_exit(NULL);
}

/*
 * Service primitves.
 */

static int
luab_setitimer(lua_State *L)
{
    luab_module_t *m0, *m1;
    int narg, which;
    struct itimerval *value;
    struct itimerval *ovalue;
    luab_thread_t *thr;
    int status;

    narg = luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(ITIMERVAL, TYPE, __func__);

    which = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    value = luab_udataisnil(L, 2, m1, struct itimerval *);
    ovalue = luab_udataisnil(L, 3, m1, struct itimerval *);

    if ((thr = luab_newthread(L, narg, "h_callout", luab_h_itimer)) != NULL) {

        if ((status = setitimer(which, value, ovalue)) != 0) {
            pthread_cancel(thr->thr_id);
            luab_thread_close(thr, 1);
        }
    } else
        status = luab_env_error;

    return (luab_pushxinteger(L, status));
}

#if __XSI_VISIBLE
static int
luab_getitimer(lua_State *L)
{
    luab_module_t *m0, *m1;
    int which;
    struct itimerval *value;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(ITIMERVAL, TYPE, __func__);

    which = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    value = luab_udata(L, 2, m1, struct itimerval *);

    status = getitimer(which, value);

    return (luab_pushxinteger(L, status));
}
#endif

/*
 * Generator functions.
 */

#if __BSD_VISIBLE
/***
 * Generator function - create an instance of (LUA_TUSERDATA(BINTIME)).
 *
 * @function create_bintime
 *
 * @param arg           Instance of (LUA_TUSERDATA(BINTIME)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage bintime [, err, msg ] = bsd.sys.time.create_bintime([ arg ])
 */
static int
luab_type_create_bintime(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(BINTIME, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}
#endif

/***
 * Generator function - create an instance of (LUA_TUSERDATA(CLOCKINFO)).
 *
 * @function create_clockinfo
 *
 * @param arg           Instance of (LUA_TUSERDATA(CLOCKINFO)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage clockinfo [, err, msg ] = bsd.sys.time.create_clockinfo([ arg ])
 */
static int
luab_type_create_clockinfo(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(CLOCKINFO, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(ITIMERVAL)).
 *
 * @function create_itimerval
 *
 * @param arg           Instance of (LUA_TUSERDATA(ITIMERVAL)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage itimerval [, err, msg ] = bsd.sys.time.create_itimerval([ arg ])
 */
static int
luab_type_create_itimerval(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(ITIMERVAL, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(TIMEVAL)).
 *
 * @function create_timeval
 *
 * @param arg           Instance of (LUA_TUSERDATA(TIMEVAL)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage timeval [, err, msg ] = bsd.sys.time.create_timeval([ arg ])
 */
static int
luab_type_create_timeval(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TIMEVAL, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(TIMEZONE)).
 *
 * @function create_timezone
 *
 * @param arg           Instance of (LUA_TUSERDATA(TIMEZONE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage timezone [, err, msg ] = bsd.sys.time.create_timezone([ arg ])
 */
static int
luab_type_create_timezone(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TIMEZONE, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
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
    LUAB_FUNC("create_bintime",           luab_type_create_bintime),
#endif
    LUAB_FUNC("create_clockinfo",         luab_type_create_clockinfo),
    LUAB_FUNC("create_itimerval",         luab_type_create_itimerval),
    LUAB_FUNC("create_timeval",           luab_type_create_timeval),
    LUAB_FUNC("create_timezone",          luab_type_create_timezone),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_time_lib = {
    .m_id       = LUAB_SYS_TIME_LIB_ID,
    .m_name     = LUAB_SYS_TIME_LIB_KEY,
    .m_vec      = luab_sys_time_vec,
};
