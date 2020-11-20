/*
 * Copyright (c) 2020 Henning Matyschok
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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

/*
 * XXX
 *
 *  (a) The implementation violates the DRY principle. Therefore, components
 *      shall reimplemented by boiler-plate code, e. g. utilizing macros or
 *      something else.
 *
 *  (b) Condition tests against constraints on mapping from cardinality
 *      to the Lua stacksize shall implemented.
 *
 *  (c) Reimplementation of the luab_table_xsentinel() functor.
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

void
luab_table_iovec_free(luab_table_t *tbl)
{
    struct iovec *x;
    size_t m, n;

    if (tbl != NULL) {

        if (((x = tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0) &&
            (tbl->tbl_sz == sizeof(struct iovec))) {

            for (m = 0; m < n; m++)
                (void)luab_iov_free(&(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        luab_table_free(tbl);
    } else
        errno = EINVAL;
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
luab_table_allocnil(size_t m, size_t sz)
{
    luab_table_t *tbl;
    size_t nbytes, n;

    if ((tbl = malloc(sizeof(luab_table_t))) != NULL) {
        (void)memset_s(tbl, sizeof(*tbl), 0, sizeof(*tbl));

        if (m > 0 && sz > 0) {
            n = (m + 1);
            nbytes = (n * sz);

            /* sentinel, (n + 1) := LUAB_TABLE_XS_FLAG */
            if ((tbl->tbl_vec = malloc(nbytes)) != NULL) {
                (void)memset_s(tbl->tbl_vec, nbytes, 0, nbytes);
#if 0
                luab_table_xsentinel(vec, n, sz);
#endif
                tbl->tbl_card = n;
                tbl->tbl_sz = sz;
            } else
                luab_core_err(EX_OSERR, __func__, errno);
        }
    } else
        luab_core_err(EX_OSERR, __func__, errno);

    return (tbl);
}

luab_table_t *
luab_table_alloc(lua_State *L, int narg, size_t m, size_t sz)
{
    luab_table_t *tbl = NULL;

    if (m > 0 && sz > 0) {
        if ((tbl = luab_table_allocnil(m, sz)) != NULL) {
            if (tbl->tbl_vec == NULL)
                luab_table_argerror(L, narg, tbl, errno);
        }
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ERANGE);

    return (tbl);
}

luab_table_t *
luab_newvector(lua_State *L, int narg, size_t sz)
{
    size_t n;

    if ((n = luab_checktable(L, narg)) == 0)
        luab_core_argerror(L, narg, NULL, 0, 0, EINVAL);

    return (luab_table_alloc(L, narg, n, sz));
}

luab_table_t *
luab_newvectornil(lua_State *L, int narg, size_t sz)
{
    size_t n;

    n = luab_checktable(L, narg);
    return (luab_table_allocnil(n, sz));
}

luab_table_t *
luab_newlvector(lua_State *L, int narg, size_t n, size_t sz)
{
    size_t m = luab_checkltable(L, narg, n);
    return (luab_table_alloc(L, narg, m, sz));
}

luab_table_t *
luab_newlvectornil(lua_State *L, int narg, size_t n, size_t sz)
{
    size_t m = luab_checkltableisnil(L, narg, n);
    return (luab_table_allocnil(m, sz));
}

/*
 * Access functions, [stack -> C].
 */

luab_table_t *
luab_table_checkargv(lua_State *L, int narg)
{
    luab_table_t *tbl;
    intptr_t **argv, *x;
    size_t m, n;

    if ((tbl = luab_newvectornil(L, narg, sizeof(void *))) != NULL) {

        if (((argv = (void *)(tbl->tbl_vec)) != NULL) &&
            (tbl->tbl_card > 1)){
            luab_table_init(L, 0);

            x = (void *)(intptr_t)(argv);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {
                if (lua_next(L, narg) != 0) {
                    /*
                     * (k,v) := (-2,-1) -> (LUA_TNUMBER,LUA_TSTRING)
                     */
                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isstring(L, -1) != 0)) {
                        x[m] = (intptr_t)lua_tostring(L, -1);
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        }
    }
    return (tbl);
}

luab_table_t *
luab_table_toxargp(lua_State *L, int narg)
{
    luab_table_t *tbl;
    intptr_t **argv, *x;
    size_t m, n;

    if ((tbl = luab_newvectornil(L, narg, sizeof(void *))) != NULL) {

        if (((argv = (void *)(tbl->tbl_vec)) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            x = (void *)(intptr_t)(argv);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {
                if (lua_next(L, narg) != 0) {
                    /*
                     * (k,v) := (-2,-1) -> (LUA_TNUMBER,LUA_TXXX)
                     */
                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_type(L, -1) != -1)) {
                        x[m] = (intptr_t)lua_topointer(L, -1);
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        }
    }
    return (tbl);
}


luab_table_t *
luab_table_checku_short(lua_State *L, int narg)
{
    luab_table_t *tbl;
    u_short *x, y;
    size_t m, n;

    if ((tbl = luab_newvectornil(L, narg, sizeof(double))) != NULL) {

        if (((x = (u_short *)(tbl->tbl_vec)) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isnumber(L, -1) != 0)) {
                        y = (u_short)luab_tointeger(L, -1, luab_env_ushrt_max);
                        x[m] = (u_short)y;
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        }
    }
    return (tbl);
}

luab_table_t *
luab_table_checkxdata(lua_State *L, int narg, luab_module_t *m)
{
    luab_table_t *tbl;

    if (m != NULL) {
        if (m->m_get_tbl != NULL) {

            if ((tbl = (*m->m_get_tbl)(L, narg)) != NULL)
                tbl->tbl_cookie = m->m_cookie;
        } else {
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

        if ((tbl->tbl_vec == NULL) ||
            (tbl->tbl_card == 0) ||
            (tbl->tbl_sz != luab_xlen(m)))
            luab_table_argerror(L, narg, tbl, ERANGE);

        if ((tbl->tbl_card - 1) != nmax)
            luab_table_argerror(L, narg, tbl, ERANGE);
    }
    return (tbl);
}

/*
 * XXX DRY this set of primitives shall merged into one by callback.
 */

luab_table_t *
luab_table_checklu_short(lua_State *L, int narg, size_t nmax)
{
    luab_table_t *tbl;
    size_t n, sz;

    sz = sizeof(int);

    if ((tbl = luab_table_checku_short(L, narg)) != NULL) {
        if ((tbl->tbl_vec == NULL) ||
            (tbl->tbl_card == 0) ||
            (tbl->tbl_sz != sz))
            luab_table_argerror(L, narg, tbl, ERANGE);

        if ((n = (tbl->tbl_card - 1)) != nmax)
            luab_table_argerror(L, narg, tbl, ERANGE);
    }
    return (tbl);
}

luab_table_t *
luab_table_tolxargp(lua_State *L, int narg, size_t nmax)
{
    luab_table_t *tbl;
    size_t n, sz;

    sz = sizeof(void *);

    if ((tbl = luab_table_toxargp(L, narg)) != NULL) {
        if ((tbl->tbl_vec == NULL) ||
            (tbl->tbl_card == 0) ||
            (tbl->tbl_sz != sz))
            luab_table_argerror(L, narg, tbl, ERANGE);

        if ((n = (tbl->tbl_card - 1)) != nmax)
            luab_table_argerror(L, narg, tbl, ERANGE);
    }
    return (tbl);
}

/*
 * Access functions, [C -> stack].
 */

void
luab_table_pushxdata(lua_State *L, int narg, luab_module_t *m,
    luab_table_t *tbl, int new, int clr)
{
    if (m != NULL && tbl != NULL) {

        if (m->m_cookie == tbl->tbl_cookie) {

            if (m->m_set_tbl != NULL)
                (*m->m_set_tbl)(L, narg, tbl, new, clr);
            else
                luab_core_err(EX_UNAVAILABLE, __func__, ENOSYS);
        } else
            luab_core_err(EX_DATAERR, __func__, ENXIO);
    } else
        errno = ENOENT;
}
