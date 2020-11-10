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

#include <fstab.h>
#undef FSTAB        /* XXX */

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_FSTAB_LIB_ID    1604963910
#define LUAB_FSTAB_LIB_KEY    "fstab"

extern luab_module_t luab_fstab_lib;

/*
 * Service primitives.
 */

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(FSTAB)).
 *
 * @function fstab_create
 *
 * @param fstab          Instance of (LUA_TUSERDATA(FSTAB)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage fstab [, err, msg ] = bsd.fstab.fstab_create([ fstab ])
 */
static int
luab_fstab_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(FSTAB), NULL));
}

/*
 * Interface against <fstab.h>.
 */

static luab_module_table_t luab_fstab_vec[] = { /* fstab.h */
    LUAB_STR("_PATH_FSTAB",         _PATH_FSTAB),
    LUAB_STR("FSTAB_RW",            FSTAB_RW),
    LUAB_STR("FSTAB_RQ",            FSTAB_RQ),
    LUAB_STR("FSTAB_RO",            FSTAB_RO),
    LUAB_STR("FSTAB_SW",            FSTAB_SW),
    LUAB_STR("FSTAB_XX",            FSTAB_XX),
    LUAB_FUNC("fstab_create",       luab_fstab_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_fstab_lib = {
    .m_cookie   = LUAB_FSTAB_LIB_ID,
    .m_name     = LUAB_FSTAB_LIB_KEY,
    .m_vec      = luab_fstab_vec,
};
