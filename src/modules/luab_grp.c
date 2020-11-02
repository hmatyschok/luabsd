/*
 * Copyright (c) 2020 Henning Matyschok
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

#include <grp.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

/*
 * XXX
 *
 *      int  pwcache_groupdb(int (*)(int), void (*)(void),
 *              struct group * (*)(const char *),
 *              struct group * (*)(gid_t));
 *
 *  Implementation depends on specific luab_type(3) implements autoboxing
 *  for (LUA_TCFUNCTION) objects or set of callbacks over vector table.
 */

#define LUAB_GRP_LIB_ID    1604324195
#define LUAB_GRP_LIB_KEY    "grp"

extern luab_module_t luab_grp_lib;

/*
 * Service primitves.
 */


/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(GROUP)).
 *
 * @function group_create
 *
 * @param group             Instance of (LUA_TUSERDATA(GROUP)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage group [, err, msg ] = bsd.grp.group_create([ group ])
 */
static int
luab_group_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(GROUP), NULL));
}

/*
 * Interface against <grp.h>.
 */

static luab_module_table_t luab_grp_vec[] = { /* grp.h */
    LUAB_STR("_PATH_GROUP",         _PATH_GROUP),
    LUAB_FUNC("group_create",       luab_group_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_grp_lib = {
    .m_cookie   = LUAB_GRP_LIB_ID,
    .m_name     = LUAB_GRP_LIB_KEY,
    .m_vec      = luab_grp_vec,
};
