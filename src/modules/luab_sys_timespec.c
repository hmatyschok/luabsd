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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_SYS_TIMESPEC_LIB_ID    1610496938
#define LUAB_SYS_TIMESPEC_LIB_KEY    "timespec"

extern luab_module_t luab_sys_timespec_lib;

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(TIMESPEC)).
 *
 * @function create_timespec
 *
 * @param arg           Instance of (LUA_TUSERDATA(TIMESPEC)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage timespec [, err, msg ] = bsd.sys.timespec.create_timespec([ arg ])
 */
static int
luab_type_create_timespec(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TIMESPEC, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/*
 * Interface against <sys/timespec.h>.
 */

static luab_module_table_t luab_sys_timespec_vec[] = { /* sys/timespec.h */
    LUAB_FUNC("create_timespec",          luab_type_create_timespec),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_timespec_lib = {
    .m_id       = LUAB_SYS_TIMESPEC_LIB_ID,
    .m_name     = LUAB_SYS_TIMESPEC_LIB_KEY,
    .m_vec      = luab_sys_timespec_vec,
};
