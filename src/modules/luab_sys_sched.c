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
    LUAB_INT("SCHED_FIFO",              SCHED_FIFO),
    LUAB_INT("SCHED_OTHER",             SCHED_OTHER),
    LUAB_INT("SCHED_RR",                SCHED_RR),
    LUAB_FUNC("create_sched_param",     luab_type_create_sched_param),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_sched_lib = {
    .m_id       = LUAB_SYS_SCHED_LIB_ID,
    .m_name     = LUAB_SYS_SCHED_LIB_KEY,
    .m_vec      = luab_sys_sched_vec,
};
