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

#include <sys/time.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_types.h"

/*
 * Generator functions, [Lua -> stack].
 */

void *
luab_newudata(lua_State *L, luab_module_t *m, void *arg)
{
    luab_udata_t *ud = NULL;

    if (m != NULL) {
        if ((ud = lua_newuserdata(L, m->sz)) != NULL) {
            (void)memset_s(ud, m->sz, 0, m->sz);

            LIST_INIT(&ud->ud_list);
    
            if (m->init != NULL && arg != NULL)
                (*m->init)(ud, arg);

            luaL_setmetatable(L, m->name);

            ud->ud_m = m;
            ud->ud_ts = time(NULL);
        }
    } else
        errno = EINVAL;

    return (ud);
}

int
luab_create(lua_State *L, int narg, luab_module_t *m0, luab_module_t *m1)
{
    luab_module_t *m;
    caddr_t arg;

    if ((m = (m1 != NULL) ? m1 : m0) != NULL) {
        if (luab_checkmaxargs(L, narg) == 0)
            arg = NULL;
        else
            arg = luab_udata(L, narg, m, caddr_t);
    } else
        arg = NULL;

    return (luab_pushudata(L, m0, arg));
}

/*
 * Accessor, [stack -> C].
 */

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

/* XXX experimental "feature" for developement of bindings. */
void *
luab_addudata(lua_State *L, int narg, luab_module_t *m,
    int xarg, luab_xarg_t *pci)
{
    luab_udata_t *ud0 = luab_todata(L, narg, m, luab_udata_t *);
    luab_udata_t *ud1 = luab_toxudata(L, xarg, pci);

    return (luab_udata_insert(ud0, ud1));
}

/*
 * Acceesor, [C -> stack].
 */

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
 * Acceesor for (LUA_TTABLE), [C -> stack].
 */

void
luab_rawsetudata(lua_State *L, int narg, luab_module_t *m, lua_Integer k, void *v)
{

    if (m != NULL && m->create != NULL) {
        /*
         * Best effort, this means try to push things on
         * stack at least as it is possible, regardless
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
         * stack at least as it is possible, regardless
         * if allocation of memory is possible or not.
         */
        if ((*m->create)(L, v) != NULL)
            lua_setfield(L, narg, k);
    }
}
