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
#include <strings.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

#define LUABSD_SYS_TIME_LIB_ID    1593623310
#define LUABSD_SYS_TIME_LIB_KEY    "time"

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
    int which = luab_checkinteger(L, 1, INT_MAX);
    time_t sec = luab_checkinteger(L, 2, LONG_MAX); /* LP64 */
    int narg = lua_gettop(L), status;
    struct itimerval itv;

    if (sec <= 0)
        return luaL_error(L, "Invalid value for timeout.");

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

    bzero(&itv, sizeof(itv));
    itv.it_value.tv_sec = sec;

    if ((status = setitimer(which, &itv, NULL)) != 0) {
        pthread_cancel(tid);
        return luab_pusherr(L, status);
    }
    lua_pushinteger(L, status);

    return 1;
}

static int
luab_getitimer(lua_State *L)
{
    int which = luab_checkinteger(L, 1, INT_MAX);
    struct itimerval itv;
    int status;

    if ((status = getitimer(which, &itv)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, itv.it_value.tv_sec);

    return 1;
}

static luab_table_t luab_sys_time_vec[] = { /* sys/time.h */
    LUABSD_INT("ITIMER_REAL",    ITIMER_REAL),
    LUABSD_INT("ITIMER_VIRTUAL",    ITIMER_VIRTUAL),
    LUABSD_INT("ITIMER_PROF",    ITIMER_PROF),
    LUABSD_FUNC("getitimer",  luab_getitimer),
    LUABSD_FUNC("setitimer",  luab_setitimer),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_sys_time_lib = {
    .cookie = LUABSD_SYS_TIME_LIB_ID,
    .name = LUABSD_SYS_TIME_LIB_KEY,
    .vec = luab_sys_time_vec,
};
