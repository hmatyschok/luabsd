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

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

/*
 * XXX
 *
 *  (a) Condition tests against constraints on mapping from cardinality
 *      to the Lua stacksize shall implemented.
 *
 *  (b) Reimplementation of the luab_table_xsentinel() functor.
 *
 *  (c) We should refactor luab_table_toxargp(3).
 */

#if 0
static u_char luab_table_xsentinel_flag = LUAB_TABLE_XS_FLAG;
static void
luab_table_xsentinel(void *v, size_t n, size_t sz)
{
    caddr_t x0;
    size_t off;

    if ((x0 = (caddr_t)v) != NULL && n > 0 && sz > 0) {
        off = ((n + sz) - sz);
        *(x0 + off) = luab_table_xsentinel_flag;
    } else
        luab_core_err(EX_DATAERR, __func__, ERANGE);
}
#endif

/*
 * Service primitives.
 *
 * The family of luab_check{l}able{isnil}(3) functions performs condition tests
 * agains n-th arg is an instance of (LUA_TTABLE) and/or cardinality, if any.
 */

size_t
luab_checktable(lua_State *L, int narg)
{
    if (lua_istable(L, narg) == 0)
        luab_core_argerror(L, narg, NULL, 0, 0, ENOENT);

    return (lua_rawlen(L, narg));
}

size_t
luab_checktableisnil(lua_State *L, int narg)
{
    if (lua_isnil(L, narg) != 0)
        return (0);

    return (luab_checktable(L, narg));
}

size_t
luab_checkltable(lua_State *L, int narg, size_t card)
{
    size_t n;

    if ((n = luab_checktable(L, narg)) != card)
        luab_core_argerror(L, narg, NULL, 0, 0, ERANGE);

    return (card);
}

size_t
luab_checkltableisnil(lua_State *L, int narg, size_t card)
{
    if (lua_isnil(L, narg) != 0)
        return (0);

    return (luab_checkltable(L, narg, card));
}

void
luab_table_init(lua_State *L, int new)
{
    if (new != 0)   /* populate Table, if any */
        lua_newtable(L);
    else
        lua_pushnil(L);
}

void
luab_table_free(luab_table_t *tbl)
{
    size_t nbytes;

    if (tbl != NULL) {
        nbytes = (tbl->tbl_card * tbl->tbl_sz);
        luab_core_free(tbl->tbl_vec, nbytes);
        luab_core_free(tbl, sizeof(*tbl));
    } else
        errno = ERANGE;
}

/*
 * Error handler.
 */

void
luab_table_argerror(lua_State *L, int narg, luab_table_t *tbl, int up_call)
{
    if (tbl != NULL) {
        luab_table_free(tbl);
        luab_core_argerror(L, narg, NULL, 0, 0, up_call);
    } else
        luab_core_err(EX_DATAERR, __func__, ERANGE);
}

/*
 * Generator functions.
 *
 * Pre-allocates data region for by luab_table{} encapsulated arrays over
 * cardinality from focussed instance of (LUA_TTABLE) as precondition for
 * performing operations, e. g. deep copying.
 */

luab_table_t *
luab_table_allocnil(size_t n, size_t sz, luab_id_t id)
{
    luab_table_t *tbl;

    if ((tbl = luab_core_alloc(1, sizeof(luab_table_t))) != NULL) {

        if ((n * sz) > 0) {

            /* sentinel, (n + 1) := LUAB_TABLE_XS_FLAG */
            if ((tbl->tbl_vec = luab_core_alloc(n, sz)) != NULL) {
#if 0
                luab_table_xsentinel(vec, n + 1, sz);
#endif
                tbl->tbl_card = n;
                tbl->tbl_sz = sz;
            } else
                luab_core_err(EX_OSERR, __func__, errno);
        } else
            errno = ERANGE;

        tbl->tbl_id = id;
    } else
        luab_core_err(EX_OSERR, __func__, errno);

    return (tbl);
}

luab_table_t *
luab_table_alloc(size_t n, size_t sz, luab_id_t id)
{
    luab_table_t *tbl = NULL;

    if ((tbl = luab_table_allocnil(n, sz, id)) != NULL) {

        if (tbl->tbl_vec == NULL)
            luab_core_err(EX_CONFIG, __func__, errno);
    }
    return (tbl);
}

luab_table_t *
luab_table_create(luab_module_t *m, void *vec, size_t card)
{
    luab_table_t *tbl = NULL;

    if (m != NULL) {

        if (vec != NULL) {

            if ((tbl = luab_table_alloc(card, m->m_sz, m->m_id)) != NULL)
                (void)memmove(tbl->tbl_vec, vec, (card * m->m_sz));
        } else
            luab_core_err(EX_NOINPUT, __func__, ERANGE);
    } else
        luab_core_err(EX_CONFIG, __func__, ENOSYS);

    return (tbl);
}

/*
 * Access functions, [stack -> C].
 */

luab_table_t *
luab_table_newvector(lua_State *L, int narg, luab_module_t *m)
{
    luab_table_t *tbl = NULL;
    size_t n;

    if (m != NULL) {

        if ((n = luab_checktable(L, narg)) != 0)
            tbl = luab_table_alloc(n, m->m_sz, m->m_id);
        else
            luab_core_argerror(L, narg, NULL, 0, 0, ERANGE);
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ENOSYS);

    return (tbl);
}

luab_table_t *
luab_table_newvectornil(lua_State *L, int narg, luab_module_t *m)
{
    luab_table_t *tbl = NULL;
    size_t n;

    if (m != NULL) {
        n = luab_checktable(L, narg);
        tbl = luab_table_allocnil(n, m->m_sz, m->m_id);
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ENOSYS);

    return (tbl);
}

luab_table_t *
luab_table_newlvector(lua_State *L, int narg, luab_module_t *m, size_t card)
{
    luab_table_t *tbl = NULL;
    size_t n;

    if (m != NULL) {
        n = luab_checkltable(L, narg, card);
        tbl = luab_table_alloc(n, m->m_sz, m->m_id);
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ENOSYS);

    return (tbl);
}

luab_table_t *
luab_table_newlvectornil(lua_State *L, int narg, luab_module_t *m, size_t card)
{
    luab_table_t *tbl = NULL;
    size_t n;

    if (m != NULL) {

        n = luab_checkltableisnil(L, narg, card);
        tbl = luab_table_allocnil(n, m->m_sz, m->m_id);
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ENOSYS);

    return (tbl);
}

luab_table_t *
luab_table_checkargv(lua_State *L, int narg)
{
    luab_table_t *tbl;
    intptr_t **argv, *x;
    size_t i, j, n;

    n = luab_checktable(L, narg);

    if ((tbl = luab_table_alloc(n, sizeof(void *), 0)) != NULL) {
        luab_table_init(L, 0);

        argv = (void *)(tbl->tbl_vec);
        x = (void *)(intptr_t)(argv);

        for (i = 0, j = tbl->tbl_card; i < j; i++) {

            if (lua_next(L, narg) != 0) {
                /*
                 * (k,v) := (-2,-1) -> (LUA_TNUMBER,LUA_TSTRING)
                 */
                if ((lua_isnumber(L, -2) != 0) &&
                    (lua_isstring(L, -1) != 0)) {
                    x[i] = (intptr_t)lua_tostring(L, -1);
                } else
                    luab_core_err(EX_DATAERR, __func__, EINVAL);
            } else {
                errno = ENOENT;
                break;
            }
            lua_pop(L, 1);
        }
    }
    return (tbl);
}

luab_table_t *
luab_table_toxargp(lua_State *L, int narg)
{
    luab_table_t *tbl;
    intptr_t **argv, *x;
    size_t i, j, n;

    n = luab_checktable(L, narg);

    if ((tbl = luab_table_alloc(n, sizeof(void *), 0)) != NULL) {
        luab_table_init(L, 0);

        argv = (void *)(tbl->tbl_vec);
        x = (void *)(intptr_t)(argv);

        for (i = 0, j = tbl->tbl_card; i < j; i++) {

            if (lua_next(L, narg) != 0) {
                /*
                 * (k,v) := (-2,-1) -> (LUA_TNUMBER,LUA_TXXX)
                 */
                if ((lua_isnumber(L, -2) != 0) &&
                    (lua_type(L, -1) != -1)) {
                    x[i] = (intptr_t)lua_topointer(L, -1);
                } else
                    luab_core_err(EX_DATAERR, __func__, EINVAL);
            } else {
                errno = ENOENT;
                break;
            }
            lua_pop(L, 1);
        }
    }
    return (tbl);
}

luab_table_t *
luab_table_checkxdata(lua_State *L, int narg, luab_module_t *m)
{
    luab_table_t *tbl;

    if (m != NULL) {

        if (m->m_get_tbl != NULL)
            tbl = (*m->m_get_tbl)(L, narg);
        else {
            errno = ENOSYS;
            tbl = NULL;
        }
    } else {
        errno = ENOENT;
        tbl = NULL;
    }
    return (tbl);
}

luab_table_t *
luab_table_checklxdata(lua_State *L, int narg, luab_module_t *m, size_t nmax)
{
    luab_table_t *tbl;

    if ((tbl = luab_table_checkxdata(L, narg, m)) != NULL) {

        if (tbl->tbl_vec != NULL && tbl->tbl_card > 0) {

            if (tbl->tbl_card != nmax)
                luab_table_argerror(L, narg, tbl, ERANGE);
        } else
            luab_table_argerror(L, narg, tbl, ERANGE);
    }
    return (tbl);
}

luab_table_t *
luab_table_tolxargp(lua_State *L, int narg, size_t nmax)
{
    luab_table_t *tbl;

    if ((tbl = luab_table_toxargp(L, narg)) != NULL) {

        if (tbl->tbl_vec != NULL && tbl->tbl_card > 0) {

            if (tbl->tbl_card != nmax)
                luab_table_argerror(L, narg, tbl, ERANGE);
        } else
            luab_table_argerror(L, narg, tbl, ERANGE);
    }
    return (tbl);
}

/*
 * Access functions, [C -> stack].
 */

int
luab_table_pusherr(lua_State *L, int up_call, int ret)
{
    int status;

    if (up_call != 0 && up_call != ENOENT)
        status = luab_pushnil(L);
    else
        status = luab_pusherr(L, errno, ret);

    return (status);
}

void
luab_table_pushxdata(lua_State *L, int narg, luab_module_t *m,
    luab_table_t *tbl, int new, int clr)
{
    if (m != NULL && tbl != NULL) {

        if (m->m_id == tbl->tbl_id) {

            if (m->m_set_tbl != NULL)
                (*m->m_set_tbl)(L, narg, tbl, new, clr);
            else
                luab_core_err(EX_UNAVAILABLE, __func__, ENOSYS);
        } else
            luab_core_err(EX_DATAERR, __func__, ENXIO);
    } else
        errno = EINVAL;
}

int
luab_table_pushxtable(lua_State *L, int narg, luab_xtable_param_t *xtp)
{
    if (xtp != NULL) {

        if (xtp->xtp_fill != NULL) {
            luab_table_init(L, xtp->xtp_new);

            (*xtp->xtp_fill)(L, narg, xtp->xtp_arg);

            if (xtp->xtp_k != NULL)
                lua_setfield(L, narg, xtp->xtp_k);
            else {
                if (narg < 0)
                    lua_pushvalue(L, narg + 1);
                else
                    lua_pushvalue(L, narg - 1);
            }
            errno = 0;
        } else
            errno = ENXIO;
    } else
        errno = ERANGE;

    return (luab_table_pusherr(L, errno, 1));
}

int
luab_table_pushxvector(lua_State *L, int narg, luab_module_t *m,
    void *vec, size_t card, int new, int clr)
{
    luab_table_t *tbl = NULL;

    if (m != NULL) {

        if (m->m_alloc_tbl != NULL) {

            if ((tbl = (*m->m_alloc_tbl)(vec, card)) != NULL)
                luab_table_pushxdata(L, narg, m, tbl, new, clr);

            errno = 0;
        } else
            errno = ENXIO;
    } else
        errno = ENOSYS;

    return (luab_table_pusherr(L, errno, 1));
}

int
luab_pushxvector(lua_State *L, luab_module_t *m, void *vec, size_t card)
{
    return (luab_table_pushxvector(L, -2, m, vec, card, 1, 1));
}

int
luab_table_setxvector(lua_State *L, int narg, luab_module_t *m,
    const char *k, void *vec, size_t card, int new, int clr)
{
    luab_table_t *tbl = NULL;

    if (m != NULL) {

        if (m->m_alloc_tbl != NULL) {

            if ((tbl = (*m->m_alloc_tbl)(vec, card)) != NULL) {
                luab_table_pushxdata(L, narg, m, tbl, new, clr);

                if (k != NULL)
                    lua_setfield(L, narg, k);
                else {
                    if (narg < 0)
                        lua_pushvalue(L, narg + 1);
                    else
                        lua_pushvalue(L, narg - 1);
                }
            }
            errno = 0;
        } else
            errno = ENXIO;
    } else
        errno = ENOSYS;

    return (luab_table_pusherr(L, errno, 1));
}
