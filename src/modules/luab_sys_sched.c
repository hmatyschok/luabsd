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

#include <sys/sched.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_SYS_SCHED_LIB_ID    1611714232
#define LUAB_SYS_SCHED_LIB_KEY   "sched"

extern luab_module_t luab_sys_sched_lib;

/*
 * Service primitives
 */

/***
 * sched_get_priority_max(2) - get schedeuling parameter limits
 *
 * @function sched_get_priority_max
 *
 * @param policy            Specifies policy by an instance of
 *                          (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.sched.sched_get_priority_max(policy)
 */
static int
luab_sched_get_priority_max(lua_State *L)
{
    luab_module_t *m;
    int policy, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    policy = (int)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    status = sched_get_priority_max(policy);
    return (luab_pushxinteger(L, status));
}

/***
 * sched_get_priority_min(2) - get schedeuling parameter limits
 *
 * @function sched_get_priority_min
 *
 * @param policy            Specifies policy by an instance of
 *                          (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.sched.sched_get_priority_min(policy)
 */
static int
luab_sched_get_priority_min(lua_State *L)
{
    luab_module_t *m;
    int policy, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    policy = (int)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    status = sched_get_priority_min(policy);
    return (luab_pushxinteger(L, status));
}

/***
 * sched_getparam(2) - set/get schedeuling parameters
 *
 * @function sched_getparam
 *
 * @param pid               Specifies process ID by an instance
 *                          of (LUA_T{NUMBER,USERDATA(PID)}).
 * @param param             Result argument by an instance of
 *                          (LUA_TUSERDATA(SCHED_PARAM)),
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.sched.sched_getparam(pid, param)
 */
static int
luab_sched_getparam(lua_State *L)
{
    luab_module_t *m0, *m1;
    pid_t pid;
    struct sched_param *param;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PID, TYPE, __func__);
    m1 = luab_xmod(SCHED_PARAM, TYPE, __func__);

    pid = (pid_t)luab_checkxinteger(L, 1, m0, luab_env_uint_max);
    param = luab_udata(L, 2, m1, struct sched_param *);
    status = sched_getparam(pid, param);
    return (luab_pushxinteger(L, status));
}




/***
 * sched_rr_get_interval(2) - get schedeuling parameter limits
 *
 * @function sched_rr_get_interval
 *
 * @param pid               Specifies process ID by an instance
 *                          of (LUA_T{NUMBER,USERDATA(INT)}).
 * @param interval          Result argument by an instance
 *                          of (LUA_TUSERDATA(TIMESPEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.sched.sched_rr_get_interval(pid, interval)
 */
static int
luab_sched_rr_get_interval(lua_State *L)
{
    luab_module_t *m0, *m1;
    pid_t pid;
    struct timespec *interval;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PID, TYPE, __func__);
    m1 = luab_xmod(TIMESPEC, TYPE, __func__);

    pid = (pid_t)luab_checkxinteger(L, 1, m0, luab_env_uint_max);
    interval = luab_udata(L, 2, m1, struct timespec *);

    status = sched_rr_get_interval(pid, interval);
    return (luab_pushxinteger(L, status));
}

/***
 * sched_setparam(2) - set/get schedeuling parameters
 *
 * @function sched_setparam
 *
 * @param pid               Specifies process ID by an instance
 *                          of (LUA_T{NUMBER,USERDATA(PID)}).
 * @param param             Value argument by an instance of
 *                          (LUA_TUSERDATA(SCHED_PARAM)),
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.sched.sched_setparam(pid, param)
 */
static int
luab_sched_setparam(lua_State *L)
{
    luab_module_t *m0, *m1;
    pid_t pid;
    struct sched_param *param;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PID, TYPE, __func__);
    m1 = luab_xmod(SCHED_PARAM, TYPE, __func__);

    pid = (pid_t)luab_checkxinteger(L, 1, m0, luab_env_uint_max);
    param = luab_udata(L, 2, m1, struct sched_param *);
    status = sched_setparam(pid, param);
    return (luab_pushxinteger(L, status));
}

/*
 * Generator functions
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(SCHED_PARAM)).
 *
 * @function create_sched_param
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(SCHED_PARAM)}).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sched_param [, err, msg ] = bsd.sys.sched.create_sched_param(arg)
 */
static int
luab_type_create_sched_param(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SCHED_PARAM, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/*
 * Interface against <sys/sched.h>
 */

static luab_module_table_t luab_sys_sched_vec[] = {
    LUAB_INT("SCHED_FIFO",                  SCHED_FIFO),
    LUAB_INT("SCHED_OTHER",                 SCHED_OTHER),
    LUAB_INT("SCHED_RR",                    SCHED_RR),
    LUAB_FUNC("sched_get_priority_max",     luab_sched_get_priority_max),
    LUAB_FUNC("sched_get_priority_min",     luab_sched_get_priority_min),
    LUAB_FUNC("sched_getparam",             luab_sched_getparam),
    LUAB_FUNC("sched_rr_get_interval",      luab_sched_rr_get_interval),
    LUAB_FUNC("sched_setparam",             luab_sched_setparam),
    LUAB_FUNC("create_sched_param",         luab_type_create_sched_param),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_sched_lib = {
    .m_id       = LUAB_SYS_SCHED_LIB_ID,
    .m_name     = LUAB_SYS_SCHED_LIB_KEY,
    .m_vec      = luab_sys_sched_vec,
};
