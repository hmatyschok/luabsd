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
    LUAB_FUNC("create_pthread_key",             luab_type_create_pthread_key),
    LUAB_FUNC("create_pthread",                 luab_type_create_pthread),
    LUAB_FUNC("create_pthread_attr",            luab_type_create_pthread_attr),
    LUAB_FUNC("create_pthread_mutex",           luab_type_create_pthread_mutex),
    LUAB_FUNC("create_pthread_mutexattr",       luab_type_create_pthread_mutexattr),
    LUAB_FUNC("create_pthread_cond",            luab_type_create_pthread_cond),
    LUAB_FUNC("create_pthread_condattr",        luab_type_create_pthread_condattr),
    LUAB_FUNC("create_pthread_rwlock",          luab_type_create_pthread_rwlock),
    LUAB_FUNC("create_pthread_rwlockattr",      luab_type_create_pthread_rwlockattr),
    LUAB_FUNC("create_pthread_once",            luab_type_create_pthread_once),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_pthread_lib = {
    .m_id       = LUAB_PTHREAD_LIB_ID,
    .m_name     = LUAB_PTHREAD_LIB_KEY,
    .m_vec      = luab_pthread_vec,
};
