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
#include "luab_types.h"
#include "luab_table.h"

void
luab_table_populate(lua_State *L, int new)
{
    if (new != 0)   /* populate Table, if any */
        lua_newtable(L);
    else
        lua_pushnil(L);
}

/*
 * Generator functions, (LUA_TTABLE).
 */

void *
luab_alloctable(lua_State *L, int narg, size_t n, size_t sz)
{
    void *vec;

    if (n == 0 && sz == 0)
        luab_argerror(L, narg, NULL, 0, 0, EINVAL);

    if ((vec = calloc(n, sz)) == NULL)
        luab_argerror(L, narg, NULL, 0, 0, ENOMEM);

    return (vec);
}

/*
 * Access functions, (LUA_TTABLE), [stack -> C].
 */

size_t
luab_checktable(lua_State *L, int narg)
{
    if (lua_istable(L, narg) == 0)
        luab_argerror(L, narg, NULL, 0, 0, EINVAL);

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
        luab_argerror(L, narg, NULL, 0, 0, ERANGE);

    return (card);
}

size_t
luab_checkltableisnil(lua_State *L, int narg, size_t card)
{
    if (lua_isnil(L, narg) != 0)
        return (0);

    return (luab_checkltable(L, narg, card));
}

/* Allocate a C array by cardinality of (LUA_TTABLE). */
void *
luab_newvector(lua_State *L, int narg, size_t *card, size_t sz)
{
    size_t n;

    if ((n = luab_checktable(L, narg)) == 0)
        luab_argerror(L, narg, NULL, 0, 0, EINVAL);

    if (card != NULL)
        *card = n;

    return (luab_alloctable(L, narg, n, sz));
}

/*
 * Allocates an array by cardinality of (LUA_TTABLE) at n-th index.
 *
 *  (a) Result argument *card returns cardinality, if not NULL.
 *
 *  (b) Throws lua_error, if (LUA_TTABLE) not exists.
 *
 *  (c) Returns an array, if allocation was performed successfully,
 *      but throws lua_error when allocation was not possible,
 *
 *  (d) Returns NULL, if cardinality of (LUA_TTABLE) is 0.
 */
void *
luab_newvectornil(lua_State *L, int narg, size_t *card, size_t sz)
{
    void *vec;
    size_t n;

    if ((n = luab_checktable(L, narg)) != 0)
        vec = luab_alloctable(L, narg, n, sz);
    else
        vec = NULL;

    if (card != NULL)
        *card = n;

    return (vec);
}

/*
 * Similar as luab_newvector(3), but the cardinality is constrained by value
 * argument len.
 */
void *
luab_newlvector(lua_State *L, int narg, size_t card, size_t sz)
{
    size_t n = luab_checkltable(L, narg, card);
    return (luab_alloctable(L, narg, n, sz));
}

/*
 * Access functions, [stack -> C].
 */

/* Translate an instance of (LUA_TTABLE) into an array of C pointer. */
const char **
luab_table_checkargv(lua_State *L, int narg)
{
    const char **argv;
    size_t n, k;

    if ((n = luab_checktable(L, narg)) == 0)    /* XXX DRY */
        luab_argerror(L, narg, NULL, 0, 0, ERANGE);

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
            luab_argerror(L, narg, argv, n + 1, sizeof(*argv), EINVAL);

        lua_pop(L, 1);
    }
    return (argv);
}

const void **
luab_table_tolxargp(lua_State *L, int narg, size_t card)
{
    const void **argv;
    size_t n, k;

    if ((n = luab_checkltableisnil(L, narg, card)) != 0) {
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
                luab_argerror(L, narg, argv, n, sizeof(void *), EINVAL);

            lua_pop(L, 1);
        }
    } else
        argv = NULL;

    return (argv);
}

/*
 * Translates instances of (LUA_TTABLE) into arrays over specific data types.
 *
 * XXX DRY, components will be replaced by so called boiler-plate code.
 */

double *
luab_table_checkdouble(lua_State *L, int narg, size_t *card)
{
    double *vec;
    size_t n, k, v;

    vec = luab_newvector(L, narg, &n, sizeof(double));

    if (card != NULL)
        *card = n;

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (int)luab_tointeger(L, -1, UINT_MAX);
            vec[k] = v;
        } else
            luab_argerror(L, narg, vec, n, sizeof(double), EINVAL);

        lua_pop(L, 1);
    }
    return (vec);
}

u_short *
luab_table_checklu_short(lua_State *L, int narg, size_t card)
{
    u_short *vec;
    size_t k, v;

    vec = luab_newlvector(L, narg, card, sizeof(u_short));

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (u_short)luab_tointeger(L, -1, USHRT_MAX);
            vec[k] = v;
        } else
            luab_argerror(L, narg, vec, card, sizeof(u_short), EINVAL);

        lua_pop(L, 1);
    }
    return (vec);
}

int *
luab_table_checklint(lua_State *L, int narg, size_t card)
{
    int *vec;
    size_t k, v;

    vec = luab_newlvector(L, narg, card, sizeof(int));

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (int)luab_tointeger(L, -1, UINT_MAX);
            vec[k] = v;
        } else
            luab_argerror(L, narg, vec, card, sizeof(int), EINVAL);

        lua_pop(L, 1);
    }
    return (vec);
}

gid_t *
luab_table_checklgid(lua_State *L, int narg, size_t card)
{
    gid_t *vec, v;
    size_t k;

    vec = luab_newlvector(L, narg, card, sizeof(gid_t));

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (gid_t)luab_tointeger(L, -1, INT_MAX);
            vec[k] = v;
        } else
            luab_argerror(L, narg, vec, card, sizeof(gid_t), EINVAL);

        lua_pop(L, 1);
    }
    return (vec);
}

/* C structures */

/*
 * Translates an instance of (LUA_TTABLE) into an array of iovec{}s. The result
 * argument *card returns the cardinality of (LUA_TTABLE).
 */
struct iovec *
luab_table_checkiovec(lua_State *L, int narg, size_t *card)
{
    struct iovec *vec, *v;
    size_t n, k;

    vec = luab_newvector(L, narg, &n, sizeof(struct iovec));

    if (card != NULL)
        *card = n;

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {   /* XXX locking? */
            v = luab_udata(L, -1, luab_mx(IOVEC), struct iovec *);
            (void)memmove(&vec[k], v, sizeof(struct iovec));
        } else
            luab_argerror(L, narg, vec, n, sizeof(struct iovec), EINVAL);

        lua_pop(L, 1);
    }
    return (vec);
}

struct iovec *
luab_table_checkliovec(lua_State *L, int narg, size_t card)
{
    struct iovec *vec, *v;
    size_t k;

    vec = luab_newlvector(L, narg, card, sizeof(struct iovec));

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {   /* XXX locking? */
            v = luab_udata(L, -1, luab_mx(IOVEC), struct iovec *);
            (void)memmove(&vec[k], v, sizeof(struct iovec));
        } else
            luab_argerror(L, narg, vec, card, sizeof(struct iovec), EINVAL);

        lua_pop(L, 1);
    }
    return (vec);
}

/* (LUA_TTABLE) -> ([timespec{}]) */
struct timespec *
luab_table_checkltimespec(lua_State *L, int narg, size_t card)
{
    struct timespec *vec, *v;
    size_t k;

    vec = luab_newlvector(L, narg, card, sizeof(struct timespec));

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isuserdata(L, -1) != 0)) {
            v = luab_udata(L, -1, luab_mx(TIMESPEC), struct timespec *);
            (void)memmove(&vec[k], v, sizeof(struct timespec));
        } else
            luab_argerror(L, narg, vec, card, sizeof(struct timespec), EINVAL);

        lua_pop(L, 1);
    }
    return (vec);
}

/*
 * Access functions, (LUA_TTABLE) as result argument at n-th index, [C -> stack].
 */

/*
 * Populate an array by (LUA_TTABLE) with elemts from an array of primitives.
 *
 * XXX well, should refactored and components redefined as boiler-plate
 *  code by use of specified macros.
 */

void
luab_table_pushdouble(lua_State *L, int narg, void *v, int new)
{
    double *vec;
    size_t i, j, card;

    if ((vec = (double *)v) != NULL) {
        card = luab_table_xlen(vec, double);

        luab_table_populate(L, new);

        for (i = 0, j = 1; i < card; i++, j++)
            luab_rawsetnumber(L, narg, j, vec[i]);

        lua_pop(L, 0);
        free(vec);
    }
}

void
luab_table_pushint(lua_State *L, int narg, void *v, int new)
{
    int *vec;
    size_t i, j, card;

    if ((vec = (int *)v) != NULL) {
        card = luab_table_xlen(vec, int);

        luab_table_populate(L, new);

        for (i = 0, j = 1; i < card; i++, j++)
            luab_rawsetinteger(L, narg, j, vec[i]);

        lua_pop(L, 0);
        free(vec);
    }
}

void
luab_table_pushldouble(lua_State *L, int narg, void *v, size_t card, int new)
{
    double *vec;
    size_t i, j;

    if ((vec = (double *)v) != NULL) {
        luab_table_populate(L, new);

        for (i = 0, j = 1; i < card; i++, j++)
            luab_rawsetnumber(L, narg, j, vec[i]);

        lua_pop(L, 0);
        free(vec);
    }
}

void
luab_table_pushlgid(lua_State *L, int narg, void *v, size_t card, int new)
{
    gid_t *vec;
    size_t i, j;

    if ((vec = (gid_t *)v) != NULL) {
        luab_table_populate(L, new);

        for (i = 0, j = 1; i < card; i++, j++)
            luab_rawsetinteger(L, narg, j, vec[i]);

        lua_pop(L, 0);
        free(vec);
    }
}

/* C structures */

/* ([iovec{}]) -> (LUA_TTABLE) */
void
luab_table_pushliovec(lua_State *L, int narg, void *v, size_t card, int new)
{
    struct iovec *vec, *x;
    size_t k;

    (void)luab_checkltable(L, narg, card);

    if ((vec = (struct iovec *)v) != NULL) {

        luab_table_populate(L, new);

        for (k = 0; lua_next(L, narg) != 0; k++) {

            if ((lua_isnumber(L, -2) != 0) &&
                (lua_isuserdata(L, -1) != 0)) {
                x = luab_udata(L, -1, luab_mx(IOVEC), struct iovec *);
                (void)memmove(x, &vec[k], sizeof(struct iovec));
            } else
                luab_argerror(L, narg, vec, card, sizeof(struct iovec), EINVAL);

            lua_pop(L, 1);
        }
        free(vec);
    }
}

/* ([timespec{}]) -> (LUA_TTABLE) */
void
luab_table_pushltimespec(lua_State *L, int narg, void *v, size_t card, int new)
{
    struct timespec *vec, *x;
    size_t k;

    (void)luab_checkltable(L, narg, card);

    if ((vec = (struct timespec *)v) != NULL) {

        luab_table_populate(L, new);

        for (k = 0; lua_next(L, narg) != 0; k++) {

            if ((lua_isnumber(L, -2) != 0) &&
                (lua_isuserdata(L, -1) != 0)) {
                x = luab_udata(L, -1, luab_mx(TIMESPEC), struct timespec *);
                (void)memmove(x, &vec[k], sizeof(struct timespec));
            } else
                luab_argerror(L, narg, vec, card, sizeof(struct timespec), EINVAL);

            lua_pop(L, 1);
        }
        free(vec);
    }
}
