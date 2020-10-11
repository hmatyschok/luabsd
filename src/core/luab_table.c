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
#include <sysexits.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_types.h"
#include "luab_table.h"

/*
 * XXX the whole implementation violates the DRY principle. Therefore,
 *  components will be reimplemented by boiler-plate code or utilizing
 *  macros.
 */

/*
 * Service primitives.
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

void
luab_table_populate(lua_State *L, int new)
{
    if (new != 0)   /* populate Table, if any */
        lua_newtable(L);
    else
        lua_pushnil(L);
}

void
luab_table_iovec_free(struct iovec *vec, size_t card)
{
    size_t idx;

    if (vec != NULL && card > 0) {
        for (idx = 0; idx < card; idx++)
            (void)luab_iov_free(&vec[idx]);
    }
}

void
luab_table_iovec_argerror(lua_State *L, int narg, struct iovec *vec, size_t idx)
{
    size_t sz, card;

    if (vec != NULL) {
        sz = sizeof(struct iovec);
        card = luab_table_xlen(vec, sz);

        luab_table_iovec_free(vec, idx);
        luab_argerror(L, narg, vec, card, sz, errno);
    } else
        exit(EX_DATAERR);
}

/* Performs deep copying. */
void
luab_table_iovec_init(lua_State *L, int narg, struct iovec *vec, size_t idx)
{
    luab_iovec_t *buf;
    struct iovec *src;
    struct iovec *dst;
    int status;

    if (((buf = luab_isiovec(L, narg)) != NULL) &&
        ((buf->iov_flags & IOV_LOCK) == 0)) {
        buf->iov_flags |= IOV_LOCK;

        src = &(buf->iov);
        dst = &(vec[idx]);

        if ((status = luab_iov_alloc(dst, src->iov_len)) == 0)
            status = luab_iov_copyin(dst, src->iov_base, src->iov_len);

        buf->iov_flags &= ~IOV_LOCK;
    } else {
        errno = ENXIO;
        status = -1;
    }

    if (status != 0)
        luab_table_iovec_argerror(L, narg, vec, idx);
}

void
luab_table_iovec_populate(lua_State *L, int narg, struct iovec *vec, int new)
{
    struct iovec *iov;
    size_t i, j, card;

    if ((iov = vec) != NULL) {
        card = luab_table_xlen(vec, sizeof(struct iovec));

        luab_table_populate(L, new);

        for (i = 0, j = 1; i < card; i++, j++, iov++)
            luab_iovec_rawsetldata(L, narg, j, iov->iov_base, iov->iov_len);

        lua_pop(L, 0);
    }
}

/*
 * Generator functions.
 *
 * luab_alloctable:
 *
 *  Wrapper for an allocator, throws lua_error if
 *
 *   (a) invalid parameter or
 *
 *   (b) something went wrong during allocation.
 *
 * luab_newvector:
 *
 *  Wrapper for luab_alloctable(3).
 *
 *   (a) Throws lua_error, if cardinality is zero.
 *
 *   (b) Result argument *card returns cardinality, if not NULL.
 *
 * luab_newvectornil:
 *
 *  Allocates an array by cardinality of (LUA_TTABLE) at n-th index.
 *
 *   (a) Throws lua_error, if (LUA_TTABLE) not exists.
 *
 *   (b) Returns an array, if allocation was performed successfully,
 *       but throws lua_error when allocation was not possible.
 *
 *   (c) Returns NULL, if cardinality of (LUA_TTABLE) is 0.
 *
 *   (d) Result argument *card returns cardinality, if not NULL.
 *
 * luab_newlvectornil:
 *
 *  Similar as above, but the cardinality is constrained by value argument len.
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
    size_t n, k, sz;

    if ((n = luab_checktable(L, narg)) == 0)    /* XXX DRY */
        luab_argerror(L, narg, NULL, 0, 0, ERANGE);

    sz = sizeof(*argv);
    argv = luab_alloctable(L, narg, n + 1, sz);

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {
        /*
         * (k,v) := (-2,-1) -> (LUA_TNUMBER,LUA_TSTRING)
         */
        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isstring(L, -1) != 0)) {
            argv[k] = lua_tostring(L, -1);
        } else
            luab_argerror(L, narg, argv, n + 1, sz, EINVAL);

        lua_pop(L, 1);
    }
    return (argv);
}

const void **
luab_table_tolxargp(lua_State *L, int narg, size_t card)
{
    const void **argv;
    size_t n, k, sz;

    sz = sizeof(void *);

    if ((n = luab_checkltableisnil(L, narg, card)) != 0) {
        argv = luab_alloctable(L, narg, n, sz);

        lua_pushnil(L);

        for (k = 0; lua_next(L, narg) != 0; k++) {
            /*
             * (k,v) := (-2,-1) -> (LUA_TNUMBER,LUA_TXXX)
             */
            if ((lua_isnumber(L, -2) != 0) &&
                (lua_type(L, -1) != -1)) {
                argv[k] = lua_topointer(L, -1);
            } else
                luab_argerror(L, narg, argv, n, sz, EINVAL);

            lua_pop(L, 1);
        }
    } else
        argv = NULL;

    return (argv);
}

/*
 * Translates instances of (LUA_TTABLE) into arrays over specific data types.
 */

double *
luab_table_checkdouble(lua_State *L, int narg, size_t *card)
{
    double *vec;
    size_t n, k, v, sz;

    sz = sizeof(double);

    if ((vec = luab_newvectornil(L, narg, &n, sz)) != NULL) {
        lua_pushnil(L);

        for (k = 0; lua_next(L, narg) != 0; k++) {

            if ((lua_isnumber(L, -2) != 0) &&
                (lua_isnumber(L, -1) != 0)) {
                v = (int)luab_tointeger(L, -1, UINT_MAX);
                vec[k] = v;
            } else
                luab_argerror(L, narg, vec, n, sz, EINVAL);

            lua_pop(L, 1);
        }
    }

    if (card != NULL)
        *card = n;

    return (vec);
}

u_short *
luab_table_checklu_short(lua_State *L, int narg, size_t card)
{
    u_short *vec;
    size_t k, v, sz;

    sz = sizeof(u_short);
    vec = luab_newlvector(L, narg, card, sz);

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (u_short)luab_tointeger(L, -1, USHRT_MAX);
            vec[k] = v;
        } else
            luab_argerror(L, narg, vec, card, sz, EINVAL);

        lua_pop(L, 1);
    }
    return (vec);
}

int *
luab_table_checklint(lua_State *L, int narg, size_t card)
{
    int *vec;
    size_t k, v, sz;

    sz = sizeof(int);
    vec = luab_newlvector(L, narg, card, sz);

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (int)luab_tointeger(L, -1, UINT_MAX);
            vec[k] = v;
        } else
            luab_argerror(L, narg, vec, card, sz, EINVAL);

        lua_pop(L, 1);
    }
    return (vec);
}

gid_t *
luab_table_checklgid(lua_State *L, int narg, size_t card)
{
    gid_t *vec, v;
    size_t k, sz;

    sz = sizeof(gid_t);
    vec = luab_newlvector(L, narg, card, sz);

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (gid_t)luab_tointeger(L, -1, INT_MAX);
            vec[k] = v;
        } else
            luab_argerror(L, narg, vec, card, sz, EINVAL);

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
    struct iovec *vec;
    size_t n, k, sz;

    sz = sizeof(struct iovec);

    if ((vec = luab_newvectornil(L, narg, &n, sz)) != NULL) {

        lua_pushnil(L);

        for (k = 0; lua_next(L, narg) != 0; k++) {

            if ((lua_isnumber(L, -2) != 0) &&
                (lua_isnumber(L, -1) != 0))
                luab_table_iovec_init(L, -1, vec, k);
            else
                luab_argerror(L, narg, vec, n, sz, EINVAL);

            lua_pop(L, 1);
        }
    }

    if (card != NULL)
        *card = n;

    return (vec);
}

struct iovec *
luab_table_checkliovec(lua_State *L, int narg, size_t card)
{
    struct iovec *vec;
    size_t k, sz;

    sz = sizeof(struct iovec);
    vec = luab_newlvector(L, narg, card, sz);

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0))
            luab_table_iovec_init(L, -1, vec, k);
        else
            luab_argerror(L, narg, vec, card, sz, EINVAL);

        lua_pop(L, 1);
    }
    return (vec);
}

/* (LUA_TTABLE) -> ([timespec{}]) */
struct timespec *
luab_table_checkltimespec(lua_State *L, int narg, size_t card)
{
    struct timespec *vec, *v;
    size_t k, sz;

    sz = sizeof(struct timespec);
    vec = luab_newlvector(L, narg, card, sz);

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isuserdata(L, -1) != 0)) {
            v = luab_udata(L, -1, luab_mx(TIMESPEC), struct timespec *);
            (void)memmove(&vec[k], v, sz);
        } else
            luab_argerror(L, narg, vec, card, sz, EINVAL);

        lua_pop(L, 1);
    }
    return (vec);
}

/*
 * Access functions, (LUA_TTABLE) as result argument at n-th index, [C -> stack].
 */

/*
 * Populates the (LUA_TTABLE) with elemts from an array of primitives.
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
    struct iovec *vec;

    (void)luab_checkltable(L, narg, card);

    if ((vec = (struct iovec *)v) != NULL) {
        luab_table_iovec_populate(L, narg, vec, new);
        luab_table_iovec_free(vec, card);
        free(vec);
    }
}

/* ([timespec{}]) -> (LUA_TTABLE) */
void
luab_table_pushltimespec(lua_State *L, int narg, void *v, size_t card, int new)
{
    struct timespec *vec, *x;
    size_t k, sz;

    (void)luab_checkltable(L, narg, card);

    if ((vec = (struct timespec *)v) != NULL) {

        luab_table_populate(L, new);

        for (k = 0, sz = sizeof(struct timespec); lua_next(L, narg) != 0; k++) {

            if ((lua_isnumber(L, -2) != 0) &&
                (lua_isuserdata(L, -1) != 0)) {
                x = luab_udata(L, -1, luab_mx(TIMESPEC), struct timespec *);
                (void)memmove(x, &vec[k], sz);
            } else
                luab_argerror(L, narg, vec, card, sz, EINVAL);

            lua_pop(L, 1);
        }
        free(vec);
    }
}
