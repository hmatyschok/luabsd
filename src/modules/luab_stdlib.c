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

extern luab_module_t div_type;
extern luab_module_t ldiv_type;

#define LUABSD_STDLIB_LIB_ID    1593623310
#define LUABSD_STDLIB_LIB_KEY    "stdlib"

extern luab_module_t luab_stdlib_lib;

/*
 * Service primitives.
 */

#if __BSD_VISIBLE
static int
luab_arc4random(lua_State *L)
{
    uint32_t n;
    
    (void)luab_checkmaxargs(L, 0);

    n = arc4random();

    return (luab_pusherr(L, n));
}

static int
luab_arc4random_uniform(lua_State *L)
{
    uint32_t ub, n;
    
    (void)luab_checkmaxargs(L, 1);

    ub = luaL_checkinteger(L, 1);
    n = arc4random_uniform(ub);

    return (luab_pusherr(L, n));
}
#endif

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(DIV)).
 *
 * @function div_create
 *
 * @param data          Instance of (LUA_TUSERDATA(DIV)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (div [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage div [, err, msg ] = bsd.stdlib.div_create([ data ])
 */
static int
luab_div_create(lua_State *L)
{
    return (luab_create(L, 1, &div_type, NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(LDIV)).
 *
 * @function ldiv_create
 *
 * @param data          Instance of (LUA_TUSERDATA(LDIV)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (ldiv [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage ldiv [, err, msg ] = bsd.stdlib.ldiv_create([ data ])
 */
static int
luab_ldiv_create(lua_State *L)
{
    return (luab_create(L, 1, &ldiv_type, NULL));
}

/*
 * Interface against <stdlib.h>.
 */

static luab_table_t luab_stdlib_vec[] = {
    LUABSD_INT("EXIT_FAILURE",  EXIT_FAILURE),
    LUABSD_INT("EXIT_SUCCESS",  EXIT_SUCCESS),
    LUABSD_INT("RAND_MAX",  RAND_MAX),
#if __BSD_VISIBLE    
    LUABSD_FUNC("arc4random", luab_arc4random),
    LUABSD_FUNC("arc4random_uniform", luab_arc4random_uniform),
#endif
    LUABSD_FUNC("div_create",   luab_div_create),
    LUABSD_FUNC("ldiv_create",   luab_ldiv_create),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_stdlib_lib = {
    .cookie = LUABSD_STDLIB_LIB_ID,
    .name = LUABSD_STDLIB_LIB_KEY,
    .vec = luab_stdlib_vec,
};
