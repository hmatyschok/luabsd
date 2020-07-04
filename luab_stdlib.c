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

#include <sys/types.h>

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

#define LUABSD_STDLIB_LIB_ID    1593623310
#define LUABSD_STDLIB_LIB_KEY    "stdlib"

/*
 * Components or service primitives on stdlib.h.
 */

static int
luab_arc4random(lua_State *L)
{
    uint32_t n = arc4random();
    
    luab_checkmaxargs(L, 0);
    lua_pushinteger(L, n);

    return 1;
}

static int
luab_arc4random_uniform(lua_State *L)
{
    uint32_t ub = luaL_checkinteger(L, 1);
    uint32_t n;
    
    luab_checkmaxargs(L, 1);
    n = arc4random_uniform(ub);

    lua_pushinteger(L, n);

    return 1;
}

static luab_table_t luab_stdlib_vec[] = {   /* stdlib.h */
    LUABSD_FUNC("arc4random", luab_arc4random),
    LUABSD_FUNC("arc4random_uniform", luab_arc4random_uniform),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_stdlib_lib = {
    .cookie = LUABSD_STDLIB_LIB_ID,
    .name = LUABSD_STDLIB_LIB_KEY,
    .vec = luab_stdlib_vec,
};
