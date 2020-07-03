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

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

const char *
luab_checklstring(lua_State *L, int narg, size_t n)
{
    const char *buf;
    size_t len;

    buf = luaL_checklstring(L, narg, &len);

    if (len > n)    /* XXX err_msg */
        luaL_argerror(L, narg, "is too big");

    return buf;
}

void
luab_pushinteger(lua_State *L, luab_un_t *u)
{
    lua_pushinteger(L, u->x_i);
}

void
luab_pushcfunction(lua_State *L, luab_un_t *u)
{
    lua_pushcfunction(L, u->x_f);
}

int
luab_pusherr(lua_State *L, int status)
{
    int saved_errno = errno;
    char *msg;

    lua_pushinteger(L, status);
    msg = strerror(saved_errno);
    lua_pushstring(L, msg);

    return 2;
}

/*
 * Called, when package.loadlib takes place.
 */

static void
luab_populate(lua_State *L, luab_table_t *vec)
{
    luab_table_t *tok;

    for (tok = vec; tok->key != NULL; tok++) {
        tok->func(L, &tok->val);
        lua_setfield(L, -2, tok->key);
    }
    lua_pop(L, 0);
}

static void
luab_newtable(lua_State *L, luab_module_t *arg)
{
    lua_newtable(L);
    luab_populate(L, arg->vec);
    lua_setfield(L, -2, arg->name);
}

static void
luab_newmetatable(lua_State *L, luab_module_t *arg)
{
    luaL_newmetatable(L, arg->name);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luab_populate(L, arg->vec);

    lua_pop(L, 1);
}

LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    lua_newtable(L);

    lua_newtable(L);

    luab_newtable(L, &luab_sys_file_lib);
    luab_newtable(L, &luab_sys_stat_lib);
    luab_newtable(L, &luab_sys_time_lib);
    luab_newtable(L, &luab_sys_unistd_lib);

    lua_setfield(L, -2, "sys");

    luab_newtable(L, &luab_db_lib);
    luab_newtable(L, &luab_fcntl_lib);
    luab_newtable(L, &luab_stdlib_lib);
    luab_newtable(L, &luab_unistd_lib);
    luab_newtable(L, &luab_uuid_lib);

    lua_pushvalue(L, -1);

    luab_newmetatable(L, &db_type);
    luab_newmetatable(L, &flock_type);

    return 1;
}
