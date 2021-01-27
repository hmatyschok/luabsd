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

#include <pthread.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#define LUAB_PTHREAD_LIB_ID    1610772009
#define LUAB_PTHREAD_LIB_KEY   "pthread"

extern luab_module_t luab_pthread_lib;

/*
 * Service primitives
 */

/*
 * XXX
 *  int     pthread_atfork(void (*)(void), void (*)(void), void (*)(void));
 */

/***
 * pthread_attr_destroy(3) - POSIX threads library
 *
 * @function pthread_attr_destroy
 *
 * @param attr              Result argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_destroy(attr)
 */
static int
luab_pthread_attr_destroy(lua_State *L)
{
    luab_module_t *m;
    pthread_attr_t attr;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    attr = luab_udata(L, 1, m, pthread_attr_t);
    status = pthread_attr_destroy(&attr);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_getstack(3) - POSIX threads library
 *
 * @function pthread_attr_getstack
 *
 * @param attr              Result argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param stackaddr         Result argument by an instance
 *                          of (LUA_TUSERDATA(CADDR)).
 * @param stacksize         Result argument by an instance of
 *                          (LUA_TUSERDATA(SIZE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_getstack(attr, stackaddr, stacksize)
 */
static int
luab_pthread_attr_getstack(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    pthread_attr_t attr;
    void *stackaddr;
    size_t *stacksize;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(CADDR, TYPE, __func__);
    m2 = luab_xmod(SIZE, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    stackaddr = luab_udata(L, 2, m1, void *);
    stacksize = luab_udata(L, 3, m2, size_t *);

    status = pthread_attr_getstack(&attr, &stackaddr, stacksize);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_getstacksize(3) - POSIX threads library
 *
 * @function pthread_attr_getstacksize
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param stacksize         Result argument by an instance of
 *                          (LUA_TUSERDATA(SIZE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_getstacksize(attr, stacksize)
 */
static int
luab_pthread_attr_getstacksize(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    size_t *stacksize;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    stacksize = luab_udata(L, 2, m1, size_t *);

    status = pthread_attr_getstacksize(&attr, stacksize);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_getguardsize(3) - POSIX threads library
 *
 * @function pthread_attr_getguardsize
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param guardsize         Result argument by an instance of
 *                          (LUA_TUSERDATA(SIZE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_getguardsize(attr, guardsize)
 */
static int
luab_pthread_attr_getguardsize(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    size_t *guardsize;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    guardsize = luab_udata(L, 2, m1, size_t *);

    status = pthread_attr_getguardsize(&attr, guardsize);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_getstackaddr(3) - POSIX threads library
 *
 * @function pthread_attr_getstackaddr
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param stackaddr         Result argument by an instance
 *                          of (LUA_TUSERDATA(CADDR)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_getstackaddr(attr, stackaddr)
 */
static int
luab_pthread_attr_getstackaddr(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    void *stackaddr;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(CADDR, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    stackaddr = luab_udata(L, 2, m1, void *);

    status = pthread_attr_getstackaddr(&attr, &stackaddr);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_getdetachstate(3) - POSIX threads library
 *
 * @function pthread_attr_getdetachstate
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param detachstate       Result argument by an instance of
 *                          (LUA_TUSERDATA(INT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_getdetachstate(attr, detachstate)
 */
static int
luab_pthread_attr_getdetachstate(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    int *detachstate;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    detachstate = luab_udata(L, 2, m1, int *);

    status = pthread_attr_getdetachstate(&attr, detachstate);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_init(3) - POSIX threads library
 *
 * @function pthread_attr_init
 *
 * @param attr              Result argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_init(attr)
 */
static int
luab_pthread_attr_init(lua_State *L)
{
    luab_module_t *m;
    pthread_attr_t attr;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    attr = luab_udata(L, 1, m, pthread_attr_t);
    status = pthread_attr_init(&attr);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_setstacksize(3) - POSIX threads library
 *
 * @function pthread_attr_setstacksize
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param stacksize         Value argument by an instance of
 *                          (LUA_T{NUMBER,USERDATA(SIZE)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_getstacksize(attr, stacksize)
 */
static int
luab_pthread_attr_setstacksize(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    size_t stacksize;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    stacksize = (size_t)luab_checklxinteger(L, 2, m1, 0);

    status = pthread_attr_setstacksize(&attr, stacksize);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_setguardsize(3) - POSIX threads library
 *
 * @function pthread_attr_setguardsize
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param guardsize         Value argument by an instance of
 *                          (LUA_T{NUMBER,USERDATA(SIZE)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_setguardsize(attr, guardsize)
 */
static int
luab_pthread_attr_setguardsize(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    size_t guardsize;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    guardsize = (size_t)luab_checklxinteger(L, 2, m1, 0);

    status = pthread_attr_setguardsize(&attr, guardsize);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_setstack(3) - POSIX threads library
 *
 * @function pthread_attr_setstack
 *
 * @param attr              Result argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param stackaddr         Result argument by an instance
 *                          of (LUA_TUSERDATA(CADDR)).
 * @param stacksize         Result argument by an instance of
 *                          (LUA_T{NUMBER,USERDATA(SIZE)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_setstack(attr, stackaddr, stacksize)
 */
static int
luab_pthread_attr_setstack(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    pthread_attr_t attr;
    void *stackaddr;
    size_t stacksize;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(CADDR, TYPE, __func__);
    m2 = luab_xmod(SIZE, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    stackaddr = luab_udata(L, 2, m1, void *);
    stacksize = (size_t)luab_checklxinteger(L, 3, m2, 0);

    status = pthread_attr_setstack(&attr, stackaddr, stacksize);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_setstackaddr(3) - POSIX threads library
 *
 * @function pthread_attr_setstackaddr
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param stackaddr         Value argument by an instance
 *                          of (LUA_TUSERDATA(CADDR)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_setstackaddr(attr, stackaddr)
 */
static int
luab_pthread_attr_setstackaddr(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    void *stackaddr;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(CADDR, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    stackaddr = luab_udata(L, 2, m1, void *);

    status = pthread_attr_setstackaddr(&attr, stackaddr);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_setdetachstate(3) - POSIX threads library
 *
 * @function pthread_attr_setdetachstate
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param detachstate       Value argument by an instance of
 *                          (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_setdetachstate(attr, detachstate)
 */
static int
luab_pthread_attr_setdetachstate(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    int detachstate;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    detachstate = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    status = pthread_attr_setdetachstate(&attr, detachstate);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_barrier_destroy(3) - destroy, initialize or wait on a barrier object
 *
 * @function pthread_barrier_destroy
 *
 * @param barrier           Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_BARRIER)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_barrier_destroy(barrier)
 */
static int
luab_pthread_barrier_destroy(lua_State *L)
{
    luab_module_t *m;
    pthread_barrier_t barrier;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PTHREAD_BARRIER, TYPE, __func__);
    barrier = luab_udata(L, 1, m, pthread_barrier_t);

    status = pthread_barrier_destroy(&barrier);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_barrier_init(3) - destroy, initialize or wait on a barrier object
 *
 * @function pthread_barrier_init
 *
 * @param barrier           Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_BARRIER)).
 * @param attr              Value argument, specified by an instance
 *                          of (LUA_TUSERDATA(PTHREAD_BARRIERATTR)).
 * @param count             Value argument, by an instance of
 *                          (LUA_T{NUMBER,USERDATA(UINT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_barrier_int(barrier, attr, count)
 */
static int
luab_pthread_barrier_init(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    pthread_barrier_t barrier;
    pthread_barrierattr_t attr;
    u_int count;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(PTHREAD_BARRIER, TYPE, __func__);
    m1 = luab_xmod(PTHREAD_BARRIERATTR, TYPE, __func__);
    m2 = luab_xmod(UINT, TYPE, __func__);

    barrier = luab_udata(L, 1, m0, pthread_barrier_t);
    attr = luab_udata(L, 2, m1, pthread_barrierattr_t);
    count = (u_int)luab_checkxinteger(L, 3, m2, luab_env_uint_max);

    status = pthread_barrier_init(&barrier, &attr, count);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_barrier_wait(3) - destroy, initialize or wait on a barrier object
 *
 * @function pthread_barrier_wait
 *
 * @param barrier           Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_BARRIER)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_barrier_wait(barrier)
 */
static int
luab_pthread_barrier_wait(lua_State *L)
{
    luab_module_t *m;
    pthread_barrier_t barrier;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PTHREAD_BARRIER, TYPE, __func__);
    barrier = luab_udata(L, 1, m, pthread_barrier_t);

    status = pthread_barrier_wait(&barrier);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_barrierattr_destroy(3) - manipulate a barrier attribute object
 *
 * @function pthread_barrierattr_destroy
 *
 * @param attr              Value argument, specified by an instance
 *                          of (LUA_TUSERDATA(PTHREAD_BARRIERATTR)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_barrierattr_destroy(attr)
 */
static int
luab_pthread_barrierattr_destroy(lua_State *L)
{
    luab_module_t *m;
    pthread_barrierattr_t attr;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PTHREAD_BARRIERATTR, TYPE, __func__);
    attr = luab_udata(L, 1, m, pthread_barrierattr_t);
    status = pthread_barrierattr_destroy(&attr);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_barrierattr_getpshared(3) - manipulate a barrier attribute object
 *
 * @function pthread_barrierattr_getpshared
 *
 * @param attr              Value argument, specified by an instance
 *                          of (LUA_TUSERDATA(PTHREAD_BARRIERATTR)).
 * @param pshared           Result argument, specified by an
 *                          instance of (LUA_TUSERDATA(INT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_barrierattr_getpshared(attr)
 */
static int
luab_pthread_barrierattr_getpshared(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_barrierattr_t attr;
    int *pshared;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_BARRIERATTR, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_barrierattr_t);
    pshared = luab_udata(L, 2, m1, int *);

    status = pthread_barrierattr_getpshared(&attr, pshared);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_barrierattr_init(3) - manipulate a barrier attribute object
 *
 * @function pthread_barrierattr_init
 *
 * @param attr              Value argument, specified by an instance
 *                          of (LUA_TUSERDATA(PTHREAD_BARRIERATTR)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_barrierattr_init(attr)
 */
static int
luab_pthread_barrierattr_init(lua_State *L)
{
    luab_module_t *m;
    pthread_barrierattr_t attr;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PTHREAD_BARRIERATTR, TYPE, __func__);
    attr = luab_udata(L, 1, m, pthread_barrierattr_t);
    status = pthread_barrierattr_init(&attr);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_barrierattr_setpshared(3) - manipulate a barrier attribute object
 *
 * @function pthread_barrierattr_setpshared
 *
 * @param attr              Value argument, specified by an instance
 *                          of (LUA_TUSERDATA(PTHREAD_BARRIERATTR)).
 * @param pshared           Result argument, by an instance
 *                          of (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_barrierattr_setpshared(attr)
 */
static int
luab_pthread_barrierattr_setpshared(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_barrierattr_t attr;
    int pshared;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_BARRIERATTR, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_barrierattr_t);
    pshared = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    status = pthread_barrierattr_setpshared(&attr, pshared);
    return (luab_pushxinteger(L, status));
}









/***
 * pthread_attr_getinheritsched(3) - POSIX threads library
 *
 * @function pthread_attr_getinheritsched
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param inheritsched      Result argument by an instance of
 *                          (LUA_TUSERDATA(INT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_getinheritsched(attr, inheritsched)
 */
static int
luab_pthread_attr_getinheritsched(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    int *inheritsched;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    inheritsched = luab_udata(L, 2, m1, int *);

    status = pthread_attr_getinheritsched(&attr, inheritsched);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_getschedparam(3) - POSIX threads library
 *
 * @function pthread_attr_getschedparam
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param param             Result argument by an instance
 *                          of (LUA_TUSERDATA(SCHED_PARAM)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_getschedparam(attr, param)
 */
static int
luab_pthread_attr_getschedparam(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    struct sched_param *param;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(SCHED_PARAM, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    param = luab_udata(L, 2, m1, struct sched_param *);
    status = pthread_attr_getschedparam(&attr, param);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_getschedpolicy(3) - POSIX threads library
 *
 * @function pthread_attr_getschedpolicy
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param policy            Result argument by an instance of
 *                          (LUA_TUSERDATA(INT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_getschedpolicy(attr, policy)
 */
static int
luab_pthread_attr_getschedpolicy(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    int *policy;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    policy = luab_udata(L, 2, m1, int *);

    status = pthread_attr_getschedpolicy(&attr, policy);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_getscope(3) - POSIX threads library
 *
 * @function pthread_attr_getscope
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param contentionscope   Result argument by an instance of
 *                          (LUA_TUSERDATA(INT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_getscope(attr, policy)
 */
static int
luab_pthread_attr_getscope(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    int *contentionscope;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    contentionscope = luab_udata(L, 2, m1, int *);

    status = pthread_attr_getscope(&attr, contentionscope);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_setinheritsched(3) - POSIX threads library
 *
 * @function pthread_attr_setinheritsched
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param inheritsched      Result argument by an instance of
 *                          (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_setinheritsched(attr, inheritsched)
 */
static int
luab_pthread_attr_setinheritsched(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    int inheritsched;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    inheritsched = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    status = pthread_attr_setinheritsched(&attr, inheritsched);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_setschedparam(3) - POSIX threads library
 *
 * @function pthread_attr_setschedparam
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param param             Value argument by an instance
 *                          of (LUA_TUSERDATA(SCHED_PARAM)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_setschedparam(attr, param)
 */
static int
luab_pthread_attr_setschedparam(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    struct sched_param *param;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(SCHED_PARAM, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    param = luab_udata(L, 2, m1, struct sched_param *);
    status = pthread_attr_setschedparam(&attr, param);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_setschedpolicy(3) - POSIX threads library
 *
 * @function pthread_attr_setschedpolicy
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param policy            Value argument by an instance of
 *                          (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_setschedpolicy(attr, policy)
 */
static int
luab_pthread_attr_setschedpolicy(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    int policy;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    policy = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    status = pthread_attr_setschedpolicy(&attr, policy);
    return (luab_pushxinteger(L, status));
}

/***
 * pthread_attr_setscope(3) - POSIX threads library
 *
 * @function pthread_attr_setscope
 *
 * @param attr              Value argument by an instance of
 *                          (LUA_TUSERDATA(PTHREAD_ATTR)).
 * @param contentionscope   Value argument by an instance of
 *                          (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pthread.pthread_attr_setscope(attr, policy)
 */
static int
luab_pthread_attr_setscope(lua_State *L)
{
    luab_module_t *m0, *m1;
    pthread_attr_t attr;
    int contentionscope;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    attr = luab_udata(L, 1, m0, pthread_attr_t);
    contentionscope = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    status = pthread_attr_setscope(&attr, contentionscope);
    return (luab_pushxinteger(L, status));
}

/*
 * Generator functions
 */

/* atomic data types */
/***
 * Generator function, creates an instance of (LUA_TUSERDATA(PTHREAD_KEY)).
 *
 * @function create_pthread_key
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(PTHREAD_KEY)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pthread_key [, err, msg ] = bsd.pthread.create_pthread_key(arg)
 */
static int
luab_type_create_pthread_key(lua_State *L)
{
    luab_module_t *m;
    pthread_key_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PTHREAD_KEY, TYPE, __func__);
    x = (pthread_key_t)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    return (luab_pushxdata(L, m, &x));
}

/* reference data types */
/***
 * Generator function, creates an instance of (LUA_TUSERDATA(PTHREAD)).
 *
 * @function create_pthread
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(PTHREAD)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pthread [, err, msg ] = bsd.pthread.create_pthread(arg)
 */
static int
luab_type_create_pthread(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(PTHREAD_ATTR)).
 *
 * @function create_pthread_attr
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(PTHREAD_ATTR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pthread_attr [, err, msg ] = bsd.pthread.create_pthread_attr(arg)
 */
static int
luab_type_create_pthread_attr(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_ATTR, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(PTHREAD_MUTEX)).
 *
 * @function create_pthread_mutex
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(PTHREAD_MUTEX)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pthread_mutex [, err, msg ] = bsd.pthread.create_pthread_mutex(arg)
 */
static int
luab_type_create_pthread_mutex(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_MUTEX, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(PTHREAD_MUTEXATTR)).
 *
 * @function create_pthread_mutexattr
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(PTHREAD_MUTEXATTR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pthread_mutexattr [, err, msg ] = bsd.pthread.create_pthread_mutexattr(arg)
 */
static int
luab_type_create_pthread_mutexattr(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_MUTEXATTR, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(PTHREAD_COND)).
 *
 * @function create_pthread_cond
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(PTHREAD_COND)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pthread_cond [, err, msg ] = bsd.pthread.create_pthread_cond(arg)
 */
static int
luab_type_create_pthread_cond(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_COND, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(PTHREAD_CONDATTR)).
 *
 * @function create_pthread_condattr
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(PTHREAD_CONDATTR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pthread_condattr [, err, msg ] = bsd.pthread.create_pthread_condattr(arg)
 */
static int
luab_type_create_pthread_condattr(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_CONDATTR, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(PTHREAD_RWLOCK)).
 *
 * @function create_pthread_rwlock
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(PTHREAD_RWLOCK)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pthread_rwlock [, err, msg ] = bsd.pthread.create_pthread_rwlock(arg)
 */
static int
luab_type_create_pthread_rwlock(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_RWLOCK, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(PTHREAD_RWLOCKATTR)).
 *
 * @function create_pthread_rwlockattr
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(PTHREAD_RWLOCKATTR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pthread_rwlockattr [, err, msg ] = bsd.pthread.create_pthread_rwlockattr(arg)
 */
static int
luab_type_create_pthread_rwlockattr(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_RWLOCKATTR, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(PTHREAD_BARRIER)).
 *
 * @function create_pthread_barrier
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(PTHREAD_BARRIER)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pthread_barrier [, err, msg ] = bsd.pthread.create_pthread_barrier(arg)
 */
static int
luab_type_create_pthread_barrier(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_BARRIER, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(PTHREAD_BARRIERATTR)).
 *
 * @function create_pthread_barrierattr
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(PTHREAD_BARRIERATTR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pthread_barrierattr [, err, msg ] = bsd.pthread.create_pthread_barrierattr(arg)
 */
static int
luab_type_create_pthread_barrierattr(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_BARRIERATTR, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(PTHREAD_ADDR)).
 *
 * @function create_pthread_addr
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(PTHREAD_ADDR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pthread_addr [, err, msg ] = bsd.pthread.create_pthread_addr(arg)
 */
static int
luab_type_create_pthread_addr(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_ADDR, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/* composite data types */
/***
 * Generator function, creates an instance of (LUA_TUSERDATA(PTHREAD_ONCE)).
 *
 * @function create_pthread_once
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(PTHREAD_ONCE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pthread_once [, err, msg ] = bsd.pthread.create_pthread_once(arg)
 */
static int
luab_type_create_pthread_once(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_ONCE, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/*
 * Interface against <pthread.h>
 */

static luab_module_table_t luab_pthread_vec[] = {
    LUAB_INT("PTHREAD_DESTRUCTOR_ITERATIONS",   PTHREAD_DESTRUCTOR_ITERATIONS),
    LUAB_INT("PTHREAD_KEYS_MAX",                PTHREAD_KEYS_MAX),
    LUAB_INT("PTHREAD_STACK_MIN",               PTHREAD_STACK_MIN),
    LUAB_INT("PTHREAD_THREADS_MAX",             PTHREAD_THREADS_MAX),
    LUAB_INT("PTHREAD_BARRIER_SERIAL_THREAD",   PTHREAD_BARRIER_SERIAL_THREAD),
    LUAB_INT("PTHREAD_DETACHED",                PTHREAD_DETACHED),
    LUAB_INT("PTHREAD_SCOPE_SYSTEM",            PTHREAD_SCOPE_SYSTEM),
    LUAB_INT("PTHREAD_INHERIT_SCHED",           PTHREAD_INHERIT_SCHED),
    LUAB_INT("PTHREAD_NOFLOAT",                 PTHREAD_NOFLOAT),
    LUAB_INT("PTHREAD_CREATE_DETACHED",         PTHREAD_CREATE_DETACHED),
    LUAB_INT("PTHREAD_CREATE_JOINABLE",         PTHREAD_CREATE_JOINABLE),
    LUAB_INT("PTHREAD_SCOPE_PROCESS",           PTHREAD_SCOPE_PROCESS),
    LUAB_INT("PTHREAD_EXPLICIT_SCHED",          PTHREAD_EXPLICIT_SCHED),
    LUAB_INT("PTHREAD_PROCESS_PRIVATE",         PTHREAD_PROCESS_PRIVATE),
    LUAB_INT("PTHREAD_PROCESS_SHARED",          PTHREAD_PROCESS_SHARED),
    LUAB_INT("PTHREAD_CANCEL_ENABLE",           PTHREAD_CANCEL_ENABLE),
    LUAB_INT("PTHREAD_CANCEL_DISABLE",          PTHREAD_CANCEL_DISABLE),
    LUAB_INT("PTHREAD_CANCEL_DEFERRED",         PTHREAD_CANCEL_DEFERRED),
    LUAB_INT("PTHREAD_CANCEL_ASYNCHRONOUS",     PTHREAD_CANCEL_ASYNCHRONOUS),
    LUAB_INT("PTHREAD_NEEDS_INIT",              PTHREAD_NEEDS_INIT),
    LUAB_INT("PTHREAD_DONE_INIT",               PTHREAD_DONE_INIT),
    LUAB_INT("PTHREAD_PRIO_NONE",               PTHREAD_PRIO_NONE),
    LUAB_INT("PTHREAD_PRIO_INHERIT",            PTHREAD_PRIO_INHERIT),
    LUAB_INT("PTHREAD_PRIO_PROTECT",            PTHREAD_PRIO_PROTECT),
    LUAB_INT("PTHREAD_MUTEX_ERRORCHECK",        PTHREAD_MUTEX_ERRORCHECK),
    LUAB_INT("PTHREAD_MUTEX_RECURSIVE",         PTHREAD_MUTEX_RECURSIVE),
    LUAB_INT("PTHREAD_MUTEX_NORMAL",            PTHREAD_MUTEX_NORMAL),
    LUAB_INT("PTHREAD_MUTEX_ADAPTIVE_NP",       PTHREAD_MUTEX_ADAPTIVE_NP),
    LUAB_INT("PTHREAD_MUTEX_TYPE_MAX",          PTHREAD_MUTEX_TYPE_MAX),
    LUAB_INT("PTHREAD_MUTEX_DEFAULT",           PTHREAD_MUTEX_DEFAULT),
    LUAB_INT("PTHREAD_MUTEX_STALLED",           PTHREAD_MUTEX_STALLED),
    LUAB_INT("PTHREAD_MUTEX_ROBUST",            PTHREAD_MUTEX_ROBUST),
/*
 * XXX
 *  LUAB_FUNC("pthread_atfork",                 luab_pthread_atfork),
 */
    LUAB_FUNC("pthread_attr_destroy",           luab_pthread_attr_destroy),
    LUAB_FUNC("pthread_attr_getstack",          luab_pthread_attr_getstack),
    LUAB_FUNC("pthread_attr_getstacksize",      luab_pthread_attr_getstacksize),
    LUAB_FUNC("pthread_attr_getguardsize",      luab_pthread_attr_getguardsize),
    LUAB_FUNC("pthread_attr_getstackaddr",      luab_pthread_attr_getstackaddr),
    LUAB_FUNC("pthread_attr_getdetachstate",    luab_pthread_attr_getdetachstate),
    LUAB_FUNC("pthread_attr_init",              luab_pthread_attr_init),
    LUAB_FUNC("pthread_attr_setstacksize",      luab_pthread_attr_setstacksize),
    LUAB_FUNC("pthread_attr_setguardsize",      luab_pthread_attr_setguardsize),
    LUAB_FUNC("pthread_attr_setstack",          luab_pthread_attr_setstack),
    LUAB_FUNC("pthread_attr_setstackaddr",      luab_pthread_attr_setstackaddr),
    LUAB_FUNC("pthread_attr_setdetachstate",    luab_pthread_attr_setdetachstate),
    LUAB_FUNC("pthread_barrier_destroy",        luab_pthread_barrier_destroy),
    LUAB_FUNC("pthread_barrier_init",           luab_pthread_barrier_init),
    LUAB_FUNC("pthread_barrier_wait",           luab_pthread_barrier_wait),
    LUAB_FUNC("pthread_barrierattr_destroy",    luab_pthread_barrierattr_destroy),
    LUAB_FUNC("pthread_barrierattr_getpshared", luab_pthread_barrierattr_getpshared),
    LUAB_FUNC("pthread_barrierattr_init",       luab_pthread_barrierattr_init),
    LUAB_FUNC("pthread_barrierattr_setpshared", luab_pthread_barrierattr_setpshared),


    LUAB_FUNC("pthread_attr_getinheritsched",   luab_pthread_attr_getinheritsched),
    LUAB_FUNC("pthread_attr_getschedparam",     luab_pthread_attr_getschedparam),
    LUAB_FUNC("pthread_attr_getschedpolicy",    luab_pthread_attr_getschedpolicy),
    LUAB_FUNC("pthread_attr_getscope",          luab_pthread_attr_getscope),
    LUAB_FUNC("pthread_attr_setinheritsched",   luab_pthread_attr_setinheritsched),
    LUAB_FUNC("pthread_attr_setschedparam",     luab_pthread_attr_setschedparam),
    LUAB_FUNC("pthread_attr_setschedpolicy",    luab_pthread_attr_setschedpolicy),
    LUAB_FUNC("pthread_attr_setscope",          luab_pthread_attr_setscope),

    LUAB_FUNC("create_pthread_key",             luab_type_create_pthread_key),
    LUAB_FUNC("create_pthread",                 luab_type_create_pthread),
    LUAB_FUNC("create_pthread_attr",            luab_type_create_pthread_attr),
    LUAB_FUNC("create_pthread_mutex",           luab_type_create_pthread_mutex),
    LUAB_FUNC("create_pthread_mutexattr",       luab_type_create_pthread_mutexattr),
    LUAB_FUNC("create_pthread_cond",            luab_type_create_pthread_cond),
    LUAB_FUNC("create_pthread_condattr",        luab_type_create_pthread_condattr),
    LUAB_FUNC("create_pthread_rwlock",          luab_type_create_pthread_rwlock),
    LUAB_FUNC("create_pthread_rwlockattr",      luab_type_create_pthread_rwlockattr),
    LUAB_FUNC("create_pthread_barrier",         luab_type_create_pthread_barrier),
    LUAB_FUNC("create_pthread_barrierattr",     luab_type_create_pthread_barrierattr),
    LUAB_FUNC("create_pthread_addr",            luab_type_create_pthread_addr),
    LUAB_FUNC("create_pthread_once",            luab_type_create_pthread_once),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_pthread_lib = {
    .m_id       = LUAB_PTHREAD_LIB_ID,
    .m_name     = LUAB_PTHREAD_LIB_KEY,
    .m_vec      = luab_pthread_vec,
};
