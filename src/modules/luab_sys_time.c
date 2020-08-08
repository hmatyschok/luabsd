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

extern luab_module_t clockinfo_type;
extern luab_module_t itimerval_type;
extern luab_module_t timespec_type;
extern luab_module_t timezone_type;

#if __BSD_VISIBLE
extern luab_module_t bintime_type;
#endif

extern int  luab_StructClockInfo(lua_State *);
extern int  luab_StructItimerVal(lua_State *);
extern int  luab_StructTimeSpec(lua_State *);
extern int  luab_StructTimeZone(lua_State *);

#if __BSD_VISIBLE
extern int luab_StructBinTime(lua_State *);
#endif

#define LUABSD_SYS_TIME_LIB_ID    1593623310
#define LUABSD_SYS_TIME_LIB_KEY    "time"

extern luab_module_t luab_sys_time_lib;

/*
 * Interface against components or service primitives on sys/time.h.
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

    which = luab_checkinteger(L, 1, INT_MAX);
    value = (struct itimerval *)(*itimerval_type.get)(L, 2);

    status = getitimer(which, value);

    return (luab_pusherr(L, status));
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
