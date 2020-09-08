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
#include <stdlib.h>
#include <string.h>
#include <uuid.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

/*
 * Operations on Complex Data Types.
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

const char *
luab_iovec_islxarg(lua_State *L, int narg, size_t len)
{
    luab_iovec_t *iov;
    const char *buf;

    if (((iov = luab_isiovec(L, narg)) != NULL) &&
        (iov->iov_flags & (IOV_BUFF|IOV_PROXY)) &&
        (len <= iov->iov_max_len))
        buf = iov->iov.iov_base;
    else
        buf = luab_islstring(L, narg, len);

    return (buf);
}

const char *
luab_iovec_checklxarg(lua_State *L, int narg, size_t len)
{
    const char *buf;

    if ((buf = luab_iovec_islxarg(L, narg, len)) == NULL)
        luaL_argerror(L, narg, "Invalid argument");

    return (buf);
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
        } else
            errno = ENOMEM;
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

    if (m != NULL && m->ctor != NULL) {

        if ((*m->ctor)(L, arg) != NULL) {

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

int
luab_pushiovec(lua_State *L, void *v, size_t len, size_t max_len)
{
    luab_iovec_param_t iop;
    int status;

    if (v != NULL && len > 0 && max_len >= len) {
        (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

        iop.iop_buf.buf_len = max_len;
        iop.iop_data.buf_len = len;
        iop.iop_data.buf_data = v;

        status = luab_pushudata(L, &iovec_type, &iop);
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

    if (m != NULL && m->ctor != NULL) {
        /*
         * Best effort, this means try to push things on
         * stack at least as it's possible, regardless
         * if allocation of memory is possible or not.
         */
        if ((*m->ctor)(L, v) != NULL)
            lua_rawseti(L, narg, k);
    }
}

void
luab_rawsetiovec(lua_State *L, int narg, lua_Integer k, void *v, size_t len)
{
    luab_iovec_param_t iop;

    if (v != NULL && len > 0) {
        (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

        iop.iop_buf.buf_len = len;
        iop.iop_data.buf_len = len;
        iop.iop_data.buf_data = v;

        luab_rawsetudata(L, narg, &iovec_type, k, &iop);
    }
}

void
luab_setudata(lua_State *L, int narg, luab_module_t *m, const char *k, void *v)
{
    if (m != NULL && m->ctor != NULL) {
        /*
         * Best effort, this means try to push things on
         * stack at least as it's possible, regardless
         * if allocation of memory is possible or not.
         */
        if ((*m->ctor)(L, v) != NULL)
            lua_setfield(L, narg, k);
    }
}

void
luab_setiovec(lua_State *L, int narg, const char *k, void *v, size_t len)
{
    luab_iovec_param_t iop;

    if (v != NULL && len > 0) {
        (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

        iop.iop_buf.buf_len = len;
        iop.iop_data.buf_len = len;
        iop.iop_data.buf_data = v;

        luab_setudata(L, narg, &iovec_type, k, &iop);
    }
}
