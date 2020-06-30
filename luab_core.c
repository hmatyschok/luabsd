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

void
luab_newtable(lua_State *L, luab_table_t *reg, const char *name)
{
    luab_table_t *tok;

    lua_newtable(L);

    for (tok = reg; tok->key; tok++) {
        tok->func(L, &tok->val);
        lua_setfield(L, -2, tok->key);
    }
    lua_setfield(L, -2, name);
}

void
luab_newmetatable(lua_State *L, luaL_Reg *reg, const char *tname)
{
    luaL_newmetatable(L, tname);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, reg, 0);
    lua_pop(L, 1);
}

/*
 * Called, when package.loadlib takes place.
 */
LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    lua_newtable(L);

    lua_newtable(L);

    luab_newtable(L, luab_sys_file, "file");
    luab_newtable(L, luab_sys_stat, "stat");
    luab_newtable(L, luab_sys_time, "time");

    lua_setfield(L, -2, "sys");

    luab_newtable(L, luab_db, "db");
    luab_newtable(L, luab_fcntl, "fcntl");
    luab_newtable(L, luab_stdlib, "stdlib");
    luab_newtable(L, luab_unistd, "unistd");
    luab_newtable(L, luab_uuid, "uuid");

    lua_pushvalue(L, -1);

    luab_newmetatable(L, luab_dblib, LUABSD_DB);
    luab_newmetatable(L, luab_flocklib, LUABSD_FLOCK);

    return 1;
}
