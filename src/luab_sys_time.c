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
    struct timespec    tv;
} luab_timespec_t;

#define luab_totimespec(L, narg) \
    (luab_todata((L), (narg), &timespec_type, luab_timespec_t *))

/***
 * Set value for tv_sec over timespec{}.
 *
 * @function tv_sec
 *
 * @param sec           Specifies value in seconds.
 *
 * @usage tv:tv_sec(sec)
 */
static int
TimeSpec_tv_sec(lua_State *L)
{
    luab_timespec_t *self = luab_totimespec(L, 1);
    time_t tv_sec = luab_checkinteger(L, 2, INT_MAX);

    luab_checkmaxargs(L, 2);

    self->tv.tv_sec = tv_sec;

    return 0;
}

/***
 * Set value for tv_nsec over timespec{}.
 *
 * @function tv_nsec
 *
 * @param nsec           Specifies value in nanoneconds.
 *
 * @usage tv:tv_nsec(nsec)
 */
static int
TimeSpec_tv_nsec(lua_State *L)
{
    luab_timespec_t *self = luab_totimespec(L, 1);
    long tv_nsec = luab_checkinteger(L, 2, LONG_MAX);

    luab_checkmaxargs(L, 2);

    self->tv.tv_nsec = tv_nsec;

    return 0;
}

/***
 * Get attributes over timespec{} as LUA_TTABLE.
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
    luab_timespec_t *self = luab_totimespec(L, 1);

    luab_checkmaxargs(L, 1);

    lua_newtable(L);   /* XXX */

    lua_pushinteger(L, self->tv.tv_sec);
    lua_setfield(L, -2, "tv_sec");

    lua_pushinteger(L, self->tv.tv_nsec);
    lua_setfield(L, -2, "tv_nsec");

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
    LUABSD_FUNC("tv_sec",   TimeSpec_tv_sec),
    LUABSD_FUNC("tv_nsec",  TimeSpec_tv_nsec),
    LUABSD_FUNC("get",  TimeSpec_get),
    LUABSD_FUNC("__tostring",   TimeSpec_tostring),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t timespec_type = {
    .cookie = LUABSD_TIMESPEC_TYPE_ID,
    .name = LUABSD_TIMESPEC_TYPE,
    .vec = timespec_methods,
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
    luab_timespec_t *self;

    luab_checkmaxargs(L, 0);

    self = (luab_timespec_t *)luab_newuserdata(L, &timespec_type);

    bzero(&self->tv, sizeof(struct timespec));

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
    int narg = luab_checkmaxargs(L, 3);
    int which;
    time_t sec;
    int status;
    struct itimerval itv;

    which = luab_checkinteger(L, 1, INT_MAX);
    sec = luab_checkinteger(L, 2, LONG_MAX); /* LP64 */

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

    if ((status = setitimer(which, &itv, NULL)) != 0)
        pthread_cancel(tid);

    return luab_pusherr(L, status);
}

static int
luab_getitimer(lua_State *L)
{
    int which;
    struct itimerval itv;
    int status;

    (void)luab_checkmaxargs(L, 1);

    which = luab_checkinteger(L, 1, INT_MAX);

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
    LUABSD_FUNC("StructTimeSpec",  luab_StructTimeSpec),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_sys_time_lib = {
    .cookie = LUABSD_SYS_TIME_LIB_ID,
    .name = LUABSD_SYS_TIME_LIB_KEY,
    .vec = luab_sys_time_vec,
};
