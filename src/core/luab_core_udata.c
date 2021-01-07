/*
 * Copyright (c) 2020, 2021 Henning Matyschok
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
#include "luab_udata.h"

/*
 * Generator function, [Lua -> stack].
 */

void *
luab_newudata(lua_State *L, luab_module_t *m, void *arg)
{
    luab_udata_t *ud = NULL;

    if (m != NULL) {

        if ((ud = lua_newuserdata(L, m->m_len)) != NULL) {
            (void)memset_s(ud, m->m_len, 0, m->m_len);

            if (m->m_init != NULL && arg != NULL)
                (*m->m_init)(ud, arg);  /* XXX upcall */

            ud->ud_m = m;
            ud->ud_ts = time(NULL);
            LIST_INIT(&ud->ud_list);

            luaL_setmetatable(L, m->m_name);
        }
    } else
        errno = ENOENT;

    return (ud);
}

/*
 * Generic service primitives.
 */

void
luab_udata_init(luab_module_t *m, luab_udata_t *ud, void *arg)
{
    if (m != NULL) {

        if (ud != NULL && arg != NULL)
            (void)memmove(ud + 1, arg, m->m_sz);
        else
            errno = EINVAL;
    } else
        errno = ENOENT;
}

void
luab_udata_remove(luab_udata_t *ud)
{
    if (ud != NULL) {

        if (ud->ud_x != NULL) {
            *(ud->ud_x) = NULL;
            ud->ud_x = NULL;
            ud->ud_xhd = NULL;
        }
        LIST_REMOVE(ud, ud_next);
    } else
        errno = ENOENT;
}

luab_udata_t *
luab_udata_find(luab_udata_t *udx, void **x)
{
    luab_udata_t *ud, *ud_tmp;

    if (udx != NULL && x != NULL) {
        LIST_FOREACH_SAFE(ud, &udx->ud_list, ud_next, ud_tmp) {

            if (*(ud->ud_x) == *x)
                break;
        }
    } else
        ud = NULL;

    if (ud == NULL)
        errno = ENOENT;

    return (ud);
}

void *
luab_udata_insert(luab_udata_t *udx, luab_udata_t *ud, void **x)
{
    void *dp;

    if (udx != NULL) {

        if (ud != NULL && x != NULL) {
            LIST_INSERT_HEAD(&udx->ud_list, ud, ud_next);

            *(void **)x = (void *)(ud + 1);

            ud->ud_x = x;
            ud->ud_xhd = &udx->ud_list;

            dp = *x;
        } else {
            errno = EINVAL;
            dp = NULL;
        }
    } else {
        errno = ENOENT;
        dp = NULL;
    }
    return (dp);
}

/*
 * Access functions, [stack -> C].
 */

void *
luab_isudata(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *ud;

    if ((ud = luab_isdata(L, narg, m, luab_udata_t *)) != NULL)
        return (ud + 1);

    return (NULL);
}

void *
luab_checkudata(lua_State *L, int narg, luab_module_t *m)
{
    if (m != NULL)
        return (luaL_checkudata(L, narg, m->m_name));
    else
        luab_core_argerror(L, narg, NULL, 0, 0, EINVAL);

    return (NULL);
}

void *
luab_checkxdata(lua_State *L, int narg, luab_module_t *m, luab_udata_t **udx)
{
    if (udx != NULL) {
        *udx = luab_todata(L, narg, m, luab_udata_t *);
        return (*udx + 1);
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, EINVAL);

    return (NULL);
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
luab_checkxdataisnil(lua_State *L, int narg, luab_module_t *m)
{
    if (lua_isnil(L, narg) != 0)
        return (NULL);

    return (luab_todata(L, narg, m, luab_udata_t *));
}

void *
luab_isxdata(lua_State *L, int narg, luab_xarg_t *pci)
{
    luab_udata_t *ud = NULL;
    luab_module_vec_t *vec;

    if ((vec = luab_typevec) != NULL) {

        while (vec->mv_mod != NULL) {

            if ((ud = luab_isudata(L, narg, vec->mv_mod)) != NULL) {
                ud = luab_todata(L, narg, vec->mv_mod, luab_udata_t *);
                break;
            }
            vec++;
        }

        if (pci != NULL) {

            if (ud != NULL) {
                pci->xarg_mod = vec->mv_mod;
                pci->xarg_len = vec->mv_mod->m_sz;
            } else {
                pci->xarg_mod = NULL;
                pci->xarg_len = 0;
            }
        }
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ENXIO);

    return (ud);
}

void *
luab_toxdata(lua_State *L, int narg, luab_xarg_t *pci)
{
    luab_udata_t *ud;

    if ((ud = luab_isxdata(L, narg, pci)) != NULL)
        return (ud + 1);

    return (NULL);
}

void *
luab_checkludata(lua_State *L, int narg, luab_module_t *m, size_t len)
{
    luab_iovec_t *iov;

    if ((iov = luab_isiovec(L, narg)) != NULL) {

        if (iov->iov.iov_base != NULL) {

            if (iov->iov.iov_len == len)
                return (iov->iov.iov_base);
            else
                luab_core_argerror(L, narg, NULL, 0, 0, ERANGE);
        } else
            luab_core_argerror(L, narg, NULL, 0, 0, EINVAL);
    } else
        return (luab_toudata(L, narg, m));

    return (NULL);
}

void *
luab_udata_xlink(lua_State *L, int narg, luab_udata_t *udx, void **x)
{
    luab_udata_t *ud;

    if (udx != NULL && x != NULL) {

        if ((ud = luab_udata_find(udx, x)) != NULL)
            luab_udata_remove(ud);

        if ((ud = luab_isxdata(L, narg, NULL)) != NULL)
            return (luab_udata_insert(udx, ud, x));

        errno = ENOENT;
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, EINVAL);

    return (NULL);
}

void *
luab_udata_checkxlink(lua_State *L, int narg, luab_module_t *m,
    luab_udata_t *udx, void **x)
{
    luab_udata_t *ud;

    if (udx != NULL && x != NULL) {

        if ((ud = luab_udata_find(udx, x)) != NULL)
            luab_udata_remove(ud);

        if ((ud = luab_checkxdataisnil(L, narg, m)) != NULL)
            return (luab_udata_insert(udx, ud, x));

        errno = ENOENT;
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, EINVAL);


    return (NULL);
}

/*
 * Access functions, [C -> stack].
 */

int
luab_pushxdata(lua_State *L, luab_module_t *m, void *arg)
{
    int up_call;
    int status;

    up_call = errno;

    if (m != NULL) {

        if (m->m_create != NULL) {

            if ((*m->m_create)(L, arg) != NULL)
                status = luab_pusherr(L, up_call, 1);
            else
                status = luab_pushnil(L);
        } else {
            errno = ENXIO;
            status = luab_pushnil(L);
        }
    } else {
        errno = ENOENT;
        status = luab_pushnil(L);
    }
    return (status);
}

/*
 * Access functions for (LUA_TTABLE), [C -> stack].
 *
 * Best effort, this means try to push things on
 * stack at least as it is possible, regardless
 * if allocation of memory is possible or not.
 */

void
luab_rawsetxdata(lua_State *L, int narg, luab_module_t *m, lua_Integer k, void *v)
{
    if (m != NULL) {
        if (m->m_create != NULL && v != NULL) {

            if ((*m->m_create)(L, v) != NULL)
                lua_rawseti(L, narg, k);
        }
    }
}

void
luab_setxdata(lua_State *L, int narg, luab_module_t *m, const char *k, void *v)
{
    if (m != NULL && k != NULL) {
        if (m->m_create != NULL && v != NULL) {

            if ((*m->m_create)(L, v) != NULL)
                lua_setfield(L, narg, k);
        }
    }
}
