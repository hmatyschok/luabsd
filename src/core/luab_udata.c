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

/*
 * Generic operations on complex data types.
 */

void *
luab_checkudata(lua_State *L, int narg, luab_module_t *m)
{
    if (m == NULL)
        luaL_argerror(L, narg, "Invalid argument");

    return (luaL_checkudata(L, narg, m->name));
}

void *
luab_toudata(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *ud = luab_todata(L, narg, m, luab_udata_t *);

    return (ud + 1);
}

void *
luab_checkludata(lua_State *L, int narg, luab_module_t *m, size_t len)
{
    luab_iovec_t *iov;
    void *buf;

    if ((iov = luab_isiovec(L, narg)) != NULL) {
        if (iov->iov.iov_base == NULL)
            luaL_argerror(L, narg, "Invalid argument.");

        if (iov->iov.iov_len != len)
            luaL_argerror(L, narg, "Invalid argument.");

        buf = iov->iov.iov_base;
    } else
        buf = luab_toudata(L, narg, m);

    return (buf);
}

void *
luab_checkudataisnil(lua_State *L, int narg, luab_module_t *m)
{
    if (lua_isnil(L, narg) != 0)
        return (NULL);

    if (m != NULL && m->get != NULL)
        return ((*m->get)(L, narg));

    return (NULL);
}

/*
 * Operations on stack.
 */

void *
luab_newuserdata(lua_State *L, luab_module_t *m, void *arg)
{
    luab_udata_t *ud = NULL;

    if (m != NULL) {
        if ((ud = lua_newuserdata(L, m->sz)) != NULL) {
            (void)memset_s(ud, m->sz, 0, m->sz);

            TAILQ_INIT(&ud->ud_hooks);

            ud->ud_m = m;

            if (m->init != NULL && arg != NULL)
                (*m->init)(ud, arg);

            luaL_setmetatable(L, m->name);
        }
    } else
        errno = EINVAL;

    return (ud);
}

int
luab_pushudata(lua_State *L, luab_module_t *m, void *arg)
{
    int save_errno = errno;
    caddr_t msg;
    int status;

    if (m != NULL && m->create != NULL) {

        if ((*m->create)(L, arg) != NULL) {

            if (save_errno != 0) {
                lua_pushinteger(L, save_errno);
                msg = strerror(save_errno);
                lua_pushstring(L, msg);
                status = 3;
            } else {
                msg = NULL;
                status = 1;
            }
        } else
            status = luab_pushnil(L);
    } else {
        errno = EINVAL;
        status = luab_pushnil(L);
    }
    return (status);
}

/*
 * Operations on LUA_TTABLE.
 */

void
luab_rawsetudata(lua_State *L, int narg, luab_module_t *m, lua_Integer k, void *v)
{

    if (m != NULL && m->create != NULL) {
        /*
         * Best effort, this means try to push things on
         * stack at least as it's possible, regardless
         * if allocation of memory is possible or not.
         */
        if ((*m->create)(L, v) != NULL)
            lua_rawseti(L, narg, k);
    }
}

void
luab_setudata(lua_State *L, int narg, luab_module_t *m, const char *k, void *v)
{
    if (m != NULL && m->create != NULL) {
        /*
         * Best effort, this means try to push things on
         * stack at least as it's possible, regardless
         * if allocation of memory is possible or not.
         */
        if ((*m->create)(L, v) != NULL)
            lua_setfield(L, narg, k);
    }
}
