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

#define luab_newtimespec(L, arg) \
    ((luab_timespec_t *)luab_newuserdata(L, &timespec_type, (arg)))
#define luab_totimespec(L, narg) \
    (luab_todata((L), (narg), &timespec_type, luab_timespec_t *))

/***
 * Set value for tv_sec over timespec{}.
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

    self->tv.tv_sec = tv_sec;

    return 0;
}

/***
 * Get value for tv_sec over timespec{}.
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
    tv_sec = self->tv.tv_sec;

    lua_pushinteger(L, tv_sec);

    return 1;
}

/***
 * Set value for tv_nsec over timespec{}.
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

    self->tv.tv_nsec = tv_nsec;

    return 0;
}

/***
 * Get value for tv_nsec over timespec{}.
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
    time_t tv_nsec;

    luab_checkmaxargs(L, 1);

    self = luab_totimespec(L, 1);
    tv_nsec = self->tv.tv_nsec;

    lua_pushinteger(L, tv_nsec);

    return 1;
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
    luab_timespec_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_totimespec(L, 1);

    lua_newtable(L);

    luab_setinteger(L, -2, "tv_sec", self->tv.tv_sec);
    luab_setinteger(L, -2, "tv_nsec", self->tv.tv_nsec);

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

    (void)memmove(&self->tv, arg, sizeof(self->tv));
}

luab_module_t timespec_type = {
    .cookie = LUABSD_TIMESPEC_TYPE_ID,
    .name = LUABSD_TIMESPEC_TYPE,
    .vec = timespec_methods,
    .init = timespec_init,
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
    struct itimerval    it;
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

    (void)memmove(&self->it.it_interval, ud, timespec_type.sz);

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

    (void)luab_newuserdata(L, &timespec_type, &self->it.it_interval);

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

    (void)memmove(&self->it.it_value, ud, timespec_type.sz);

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

    (void)luab_newuserdata(L, &timespec_type, &self->it.it_value);

    return 1;
}

/***
 * Get attributes over itimerval{} as LUA_TTABLE.
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

    luab_setudata(L, -2, &timespec_type, "it_interval", &self->it.it_interval);
    luab_setudata(L, -2, &timespec_type, "it_value", &self->it.it_value);

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

    (void)memmove(&self->it, arg, sizeof(self->it));
}

luab_module_t itimerval_type = {
    .cookie = LUABSD_ITIMERVAL_TYPE_ID,
    .name = LUABSD_ITIMERVAL_TYPE,
    .vec = itimerval_methods,
    .init = itimerval_init,
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

#if __XSI_VISIBLE
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
#endif

static luab_table_t luab_sys_time_vec[] = { /* sys/time.h */
    LUABSD_INT("ITIMER_REAL",   ITIMER_REAL),
    LUABSD_INT("ITIMER_VIRTUAL",    ITIMER_VIRTUAL),
    LUABSD_INT("ITIMER_PROF",   ITIMER_PROF),
    LUABSD_FUNC("setitimer",    luab_setitimer),
#if __XSI_VISIBLE
    LUABSD_FUNC("getitimer",    luab_getitimer),
#endif
    LUABSD_FUNC("StructTimeSpec",   luab_StructTimeSpec),
    LUABSD_FUNC("StructItimerVal",  luab_StructItimerVal),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_sys_time_lib = {
    .cookie = LUABSD_SYS_TIME_LIB_ID,
    .name = LUABSD_SYS_TIME_LIB_KEY,
    .vec = luab_sys_time_vec,
};
