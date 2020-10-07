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
        if ((ud = lua_newuserdata(L, m->m_sz)) != NULL) {
            (void)memset_s(ud, m->m_sz, 0, m->m_sz);

            if (m->m_init != NULL && arg != NULL)
                (*m->m_init)(ud, arg);

            ud->ud_m = m;
            ud->ud_ts = time(NULL);
            LIST_INIT(&ud->ud_list);

            luaL_setmetatable(L, m->m_name);
        }
    } else
        errno = EINVAL;

    return (ud);
}

/*
 * Generic service primitves, complex data types.
 */

void *
luab_udata_add(lua_State *L, int narg, luab_module_t *m, int xarg, void **x)
{
    luab_udata_t *self, *ud;

    self = luab_todata(L, narg, m, luab_udata_t *);
    ud = luab_toxudata(L, xarg, NULL);  /* XXX */

    if (ud != NULL && x != NULL) {
        LIST_INSERT_HEAD(&self->ud_list, ud, ud_next);

        *(void **)x = (void *)(ud + 1);
        ud->ud_x = (caddr_t *)x;
        ud->ud_xhd = &self->ud_list;

        return (*ud->ud_x);
    }
    return (NULL);
}

void
luab_udata_init(luab_module_t *m, luab_udata_t *ud, void *arg)
{
    if (m != NULL && ud != NULL && arg != NULL)
        (void)memmove(ud + 1, arg, luab_xlen(m));
}

/*
 * Accessor, [stack -> C].
 */

void *
luab_checkudata(lua_State *L, int narg, luab_module_t *m)
{
    if (m == NULL)
        luaL_argerror(L, narg, "Invalid argument");

    return (luaL_checkudata(L, narg, m->m_name));
}

void *
luab_isudata(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *ud;

    if ((ud = luab_isdata(L, narg, m, luab_udata_t *)) == NULL)
        return (NULL);

    return (ud + 1);
}

void *
luab_toudata(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *ud;

    ud = luab_todata(L, narg, m, luab_udata_t *);
    return (ud + 1);
}

void *
luab_checkudataisnil(lua_State *L, int narg, luab_module_t *m)
{
    if (lua_isnil(L, narg) != 0)
        return (NULL);

    if (m != NULL && m->m_get != NULL)
        return ((*m->m_get)(L, narg));

    return (NULL);
}

void *
luab_toxudata(lua_State *L, int narg, luab_xarg_t *pci)
{
    luab_module_vec_t *vec;
    luab_udata_t *ud;

    for (vec = luab_typevec; vec->mv_mod != NULL; vec++) {
        if ((ud = luab_isudata(L, narg, vec->mv_mod)) != NULL) {
            ud = luab_todata(L, narg, vec->mv_mod, luab_udata_t *);
            break;
        }
    }

    if (pci != NULL) {

        if (ud != NULL) {
            pci->xarg_idx = vec->mv_idx;
            pci->xarg_len = luab_xlen(vec->mv_mod);
        } else {
            pci->xarg_idx = -1;
            pci->xarg_len = 0;
        }
    }
    return (ud);
}

void *
luab_toxdata(lua_State *L, int narg, luab_xarg_t *pci)
{
    luab_udata_t *ud;

    if ((ud = luab_toxudata(L, narg, pci)) != NULL)
        return (ud + 1);

    return (NULL);
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

/*
 * Accessor, [C -> stack].
 */

int
luab_pushudata(lua_State *L, luab_module_t *m, void *arg)
{
    int save_errno = errno;
    caddr_t msg;
    int status;

    if (m != NULL && m->m_create != NULL) {

        if ((*m->m_create)(L, arg) != NULL) {

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
 * Accessor for (LUA_TTABLE), [C -> stack].
 */

void
luab_rawsetudata(lua_State *L, int narg, luab_module_t *m, lua_Integer k, void *v)
{

    if (m != NULL && m->m_create != NULL) {
        /*
         * Best effort, this means try to push things on
         * stack at least as it is possible, regardless
         * if allocation of memory is possible or not.
         */
        if ((*m->m_create)(L, v) != NULL)
            lua_rawseti(L, narg, k);
    }
}

void
luab_setudata(lua_State *L, int narg, luab_module_t *m, const char *k, void *v)
{
    if (m != NULL && m->m_create != NULL) {
        /*
         * Best effort, this means try to push things on
         * stack at least as it is possible, regardless
         * if allocation of memory is possible or not.
         */
        if ((*m->m_create)(L, v) != NULL)
            lua_setfield(L, narg, k);
    }
}
