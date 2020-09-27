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
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

/*
 * Subr.
 */

static void
luab_table_argerror(lua_State *L, int narg, void *v, size_t n, size_t sz)
{
    size_t len;

    if (v != NULL) {
        len = n * sz;
        (void)memset_s(v, len, 0, len);
        free(v);
    }
    luaL_argerror(L, narg, "Invalid argument");
}

/*
 * Accessor, [stack -> C].
 */

/* Translate an instance of (LUA_TTABLE) into an argv. */
const char **
luab_checkargv(lua_State *L, int narg)
{
    const char **argv;
    size_t n, k;

    if ((n = luab_checktable(L, narg)) == 0)    /* XXX DRY */
        luaL_argerror(L, narg, "Empty table");

    argv = luab_alloctable(L, narg, n + 1, sizeof(*argv));

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {
        /*
         * (k,v) := (-2,-1) -> (LUA_TNUMBER,LUA_TSTRING)
         */
        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isstring(L, -1) != 0)) {
            argv[k] = lua_tostring(L, -1);
        } else
            luab_table_argerror(L, narg, argv, n + 1, sizeof(*argv));

        lua_pop(L, 1);
    }
    return (argv);
}

/*
 * Accessor, [stack -> C].
 */

const void **
luab_table_tolxargp(lua_State *L, int narg, size_t len)
{
    const void **argv;
    size_t n, k;

    if ((n = luab_checkltableisnil(L, narg, len)) != 0) {
        argv = luab_alloctable(L, narg, n, sizeof(void *));

        lua_pushnil(L);

        for (k = 0; lua_next(L, narg) != 0; k++) {
            /*
             * (k,v) := (-2,-1) -> (LUA_TNUMBER,LUA_TXXX)
             */
            if ((lua_isnumber(L, -2) != 0) &&
                (lua_type(L, -1) != -1)) {
                argv[k] = lua_topointer(L, -1);
            } else
                luab_table_argerror(L, narg, argv, n, sizeof(void *));

            lua_pop(L, 1);
        }
    } else
        argv = NULL;

    return (argv);
}

/*
 * Translate an array by (LUA_TTABLE) into an array of specific data types.
 *
 * XXX DRY, components will be replaced by so called boiler-plate code.
 */

double *
luab_table_checkdouble(lua_State *L, int narg, size_t *len)
{
    double *vec;
    size_t n;
    int k, v;

    vec = luab_newvector(L, narg, &n, sizeof(double));

    if (len != NULL)
        *len = n;

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (int)luab_tointeger(L, -1, UINT_MAX);
            vec[k] = v;
        } else
            luab_table_argerror(L, narg, vec, n, sizeof(double));

        lua_pop(L, 1);
    }
    return (vec);
}

u_short *
luab_table_checklu_short(lua_State *L, int narg, size_t len)
{
    u_short *vec;
    int k, v;

    vec = luab_newlvector(L, narg, len, sizeof(u_short));

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (u_short)luab_tointeger(L, -1, USHRT_MAX);
            vec[k] = v;
        } else
            luab_table_argerror(L, narg, vec, len, sizeof(u_short));

        lua_pop(L, 1);
    }
    return (vec);
}

int *
luab_table_checklint(lua_State *L, int narg, size_t len)
{
    int *vec, k, v;

    vec = luab_newlvector(L, narg, len, sizeof(int));

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (int)luab_tointeger(L, -1, UINT_MAX);
            vec[k] = v;
        } else
            luab_table_argerror(L, narg, vec, len, sizeof(int));

        lua_pop(L, 1);
    }
    return (vec);
}

gid_t *
luab_table_checklgid(lua_State *L, int narg, size_t len)
{
    gid_t *vec, v;
    int k;

    vec = luab_newlvector(L, narg, len, sizeof(gid_t));

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (gid_t)luab_tointeger(L, -1, INT_MAX);
            vec[k] = v;
        } else
            luab_table_argerror(L, narg, vec, len, sizeof(gid_t));

        lua_pop(L, 1);
    }
    return (vec);
}

/*
 * Accessor, [C -> stack].
 */

void
luab_rawsetinteger(lua_State *L, int narg, lua_Integer k, lua_Integer v)
{
    lua_pushinteger(L, v);
    lua_rawseti(L, narg, k);
}

void
luab_rawsetnumber(lua_State *L, int narg, lua_Integer k, lua_Number v)
{
    lua_pushnumber(L, v);
    lua_rawseti(L, narg, k);
}

void
luab_rawsetstring(lua_State *L, int narg, lua_Integer k, const char *v)
{
    lua_pushstring(L, v);
    lua_rawseti(L, narg, k);
}

void
luab_rawsetldata(lua_State *L, int narg, lua_Integer k, void *v, size_t len)
{
    luaL_Buffer b;
    caddr_t dp;

    if (v != NULL && len > 1) {  /* XXX redundant code-section */
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        lua_rawseti(L, narg, k);
    }
}

void
luab_setcfunction(lua_State *L, int narg, const char* k, lua_CFunction v)
{
    lua_pushcfunction(L, v);
    lua_setfield(L, narg, k);
}

void
luab_setinteger(lua_State *L, int narg, const char *k, lua_Integer v)
{
    lua_pushinteger(L, v);
    lua_setfield(L, narg, k);
}

void
luab_setstring(lua_State *L, int narg, const char *k, const char *v)
{
    lua_pushstring(L, v);
    lua_setfield(L, narg, k);
}

void
luab_setldata(lua_State *L, int narg, const char *k, void *v, size_t len)
{
    luaL_Buffer b;
    caddr_t dp;

    if (v != NULL && len > 1) { /* XXX redundant code-section */
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        lua_setfield(L, narg, k);
    }
}

/*
 * Accessor, (LUA_TTABLE) as result argument at n-th index, [C -> stack].
 */

/*
 * Populate an array by (LUA_TTABLE) with elemts from an array of gid_t.
 *
 * XXX well, should refactored and components redefined as boiler-plate
 *  code by use of specified macros.
 */

void
luab_table_pushlgidset(lua_State *L, int narg, gid_t *gids, int ngroups, int new)
{
    int i, j;

    if (gids != NULL) {

        if (new != 0)   /* populate Table, if any */
            lua_newtable(L);
        else
            lua_pushnil(L);

        for (i = 0, j = 1; i < ngroups; i++, j++)
            luab_rawsetinteger(L, narg, j, gids[i]);

        lua_pop(L, 0);
    }
}

void
luab_table_pushldouble(lua_State *L, int narg, double *vec, size_t n, int new)
{
    size_t i, j;

    if (vec!= NULL) {

        if (new != 0)   /* populate Table, if any */
            lua_newtable(L);
        else
            lua_pushnil(L);

        for (i = 0, j = 1; i < n; i++, j++)
            luab_rawsetnumber(L, narg, j, vec[i]);

        lua_pop(L, 0);
    }
}
