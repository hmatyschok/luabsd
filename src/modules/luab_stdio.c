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

#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_STDIO_LIB_ID    1605194991
#define LUAB_STDIO_LIB_KEY    "stdio"

extern luab_module_t luab_stdio_lib;

/*
 * Service primitives.
 */

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(__SBUF)).
 *
 * @function sbuf_create
 *
 * @param __sbuf          Instance of (LUA_TUSERDATA(__SBUUF)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage __sbuf [, err, msg ] = bsd.stdio.sbuf_create([ __sbuf ])
 */
static int
luab_sbuf_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(__SBUF), NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(SFILE)).
 *
 * @function sfile_create
 *
 * @param sfile          Instance of (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sfile [, err, msg ] = bsd.stdio.sfile_create([ sfile ])
 */
static int
luab_sfile_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(SFILE), NULL));
}

/*
 * Interface against <stdio.h>.
 */

static luab_module_table_t luab_stdio_vec[] = { /* stdio.h */
    LUAB_FUNC("sbuf_create",            luab_sbuf_create),
    LUAB_FUNC("sfile_create",           luab_sfile_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_stdio_lib = {
    .m_cookie   = LUAB_STDIO_LIB_ID,
    .m_name     = LUAB_STDIO_LIB_KEY,
    .m_vec      = luab_stdio_vec,
};
