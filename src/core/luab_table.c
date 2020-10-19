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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

/*
 * XXX the whole implementation violates the DRY principle.
 *
 *  (a) Therefore, components shall reimplemented by boiler-plate code or
 *      utilizing macros.
 *
 *  (b) Condition tests against constraints on mapping from cardinality
 *      and the Lua stacksize shall implemented.
 *
 *  (c) Extension for luab_table_xsentinel(3) by setup of custom flag.
 */

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
        luab_sysexits_err(EX_DATAERR, __func__, ERANGE);
}

static void
luab_table_iovec_init(lua_State *L, int narg, struct iovec *iov)
{
    luab_iovec_t *buf;
    struct iovec *src;

    if (((buf = luab_isiovec(L, narg)) != NULL) &&
        ((buf->iov_flags & IOV_LOCK) == 0)) {
        buf->iov_flags |= IOV_LOCK;

        src = &(buf->iov);

        if ((luab_iov_alloc(iov, src->iov_len)) == 0) /* deep copy */
            (void)luab_iov_copyin(iov, src->iov_base, src->iov_len);
                /* XXX */
        buf->iov_flags &= ~IOV_LOCK;
    }
}

/*
 * Service primitives.
 *
 * The family of luab_check{l}able{isnil}(3) functions performs condition tests
 * about if n-th arg is an instance of (LUA_TTABLE) and/or cardinality, if any.
 */

size_t
luab_checktable(lua_State *L, int narg)
{
    if (lua_istable(L, narg) == 0)
        luab_argerror(L, narg, NULL, 0, 0, ENOENT);

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
luab_table_init(lua_State *L, int new)
{
    if (new != 0)   /* populate Table, if any */
        lua_newtable(L);
    else
        lua_pushnil(L);
}

void
luab_table_free(void *v, size_t n, size_t sz)
{
    size_t nbytes;

    nbytes = (n * sz);
    luab_free(v, nbytes);
}

void
luab_table_iovec_free(struct iovec *vec)
{
    struct iovec *iov;
    size_t n, sz;
    u_char f;

    if ((iov = vec) != NULL) {
        sz = sizeof(struct iovec);
        n = 0;

        while (1) {
            f = *(caddr_t)iov;

            if (f != luab_table_xsentinel_flag) {
                (void)luab_iov_free(iov);
                iov++; n++;
            } else {
                errno = ENOENT;
                break;
            }
        }
        luab_table_free(vec, n, sz);
    } else
        errno = ERANGE;
}

/*
 * Error handler.
 */

void
luab_table_iovec_argerror(lua_State *L, int narg, struct iovec *vec, size_t card)
{
    size_t sz;

    if (vec != NULL) {
        sz = sizeof(struct iovec);
        luab_table_iovec_free(vec);
        luab_argerror(L, narg, vec, card, sz, errno);
    } else
        luab_sysexits_err(EX_DATAERR, __func__, ERANGE);
}

/*
 * Generator functions.
 *
 * Pre-allocates data region for arrays by cardinality from an instance
 * of (LUA_TTABLE) as precondition for performing a deep copy.
 */

void *
luab_alloctablenil(lua_State *L, int narg, size_t m, size_t sz, size_t *card)
{
    size_t nbytes = 0, n;
    void *vec = NULL;

    if (card == NULL)
        luab_argerror(L, narg, NULL, 0, 0, EINVAL);

    if (m > 0 && sz > 0) {
        n = (m + 1);
        nbytes = (n * sz);

        /* sentinel, (n + 1) := LUAB_TABLE_XS_FLAG */
        if ((vec = malloc(nbytes)) != NULL) {
            (void)memset_s(vec, nbytes, 0, nbytes);
            luab_table_xsentinel(vec, n, sz);
            *card = m;
        } else
            *card = 0;

    } else
        luab_sysexits_err(EX_NOINPUT, __func__, ERANGE);

    return (vec);
}

void *
luab_alloctable(lua_State *L, int narg, size_t m, size_t sz, size_t *card)
{
    void *vec;

    if (m == 0 && sz == 0)
        luab_argerror(L, narg, NULL, 0, 0, EINVAL);

    if ((vec = luab_alloctablenil(L, narg, m, sz, card)) == NULL)
        luab_argerror(L, narg, NULL, 0, 0, ENOMEM);

    return (vec);
}

void *
luab_newvector(lua_State *L, int narg, size_t *card, size_t sz)
{
    size_t n;

    if ((n = luab_checktable(L, narg)) == 0)
        luab_argerror(L, narg, NULL, 0, 0, EINVAL);

    return (luab_alloctable(L, narg, n, sz, card));
}

void *
luab_newvectornil(lua_State *L, int narg, size_t *card, size_t sz)
{
    void *vec;
    size_t n;

    if ((n = luab_checktable(L, narg)) != 0)
        vec = luab_alloctable(L, narg, n, sz, card);
    else
        vec = NULL;

    return (vec);
}

void *
luab_newlvector(lua_State *L, int narg, size_t n, size_t sz, size_t *card)
{
    size_t m = luab_checkltable(L, narg, n);
    return (luab_alloctable(L, narg, m, sz, card));
}

void *
luab_newlvectornil(lua_State *L, int narg, size_t n, size_t sz, size_t *card)
{
    size_t m = luab_checkltableisnil(L, narg, n);
    return (luab_alloctablenil(L, narg, m, sz, card));
}

/*
 * Access functions, [stack -> C].
 */

const char **
luab_table_checkargv(lua_State *L, int narg)
{
    intptr_t **argv, *x;
    size_t card, sz;

    sz = sizeof(void *);

    if ((argv = luab_newvectornil(L, narg, &card, sz)) != NULL) {

        luab_table_init(L, 0);
        x = (void *)(intptr_t)argv;

        do {
            if (lua_next(L, narg) == 0) {
                errno = ENOENT;
                x = NULL;
            } else {
                /*
                 * (k,v) := (-2,-1) -> (LUA_TNUMBER,LUA_TSTRING)
                 */
                if ((lua_isnumber(L, -2) != 0) &&
                    (lua_isstring(L, -1) != 0)) {
                    x = (void *)(intptr_t)lua_tostring(L, -1);
                } else
                    luab_sysexits_err(EX_DATAERR, __func__, EINVAL);

                x++;
            }
            lua_pop(L, 1);
        } while (x != NULL);
    } else
        argv = NULL;

    return ((void *)argv);  /* XXX terrible */
}

const void **
luab_table_tolxargp(lua_State *L, int narg, size_t n)
{
    intptr_t **argv, *x;
    size_t card, sz;

    sz = sizeof(void *);

    if ((argv = luab_newlvectornil(L, narg, n, sz, &card)) != NULL) {

        luab_table_init(L, 0);
        x = (void *)(intptr_t)argv;

        do {
            if (lua_next(L, narg) == 0) {
                errno = ENOENT;
                x = NULL;
            } else {
                /*
                 * (k,v) := (-2,-1) -> (LUA_TNUMBER,LUA_TXXX)
                 */
                if ((lua_isnumber(L, -2) != 0) &&
                    (lua_type(L, -1) != -1)) {
                    x = (void *)(intptr_t)lua_topointer(L, -1);
                } else
                    luab_sysexits_err(EX_DATAERR, __func__, EINVAL);

                x++;
            }
            lua_pop(L, 1);
        } while (x != NULL);
    } else
        argv = NULL;

    return ((void *)argv);
}

double *
luab_table_checkdouble(lua_State *L, int narg, size_t *n)
{
    double *vec, *x, v;
    size_t card, sz;
    u_char f;

    sz = sizeof(double);

    if ((vec = luab_newvectornil(L, narg, &card, sz)) != NULL) {

        luab_table_init(L, 0);
        x = vec;

        do {
            f = *(caddr_t)x;

            if (lua_next(L, narg) == 0)
                errno = ENOENT;
            else {

                if ((lua_isnumber(L, -2) != 0) &&
                    (lua_isnumber(L, -1) != 0)) {
                    v = (double)lua_tonumber(L, -1);
                    *x = (double)v;
                } else
                    luab_sysexits_err(EX_DATAERR, __func__, EINVAL);

                x++;
            }
            lua_pop(L, 1);
        } while (f != luab_table_xsentinel_flag);
    }

    if (n != NULL)
        *n = card - 1;

    return (vec);
}

struct iovec *
luab_table_checkiovec(lua_State *L, int narg, size_t *n)
{
    struct iovec *vec, *x;
    size_t card, sz;
    u_char f;

    sz = sizeof(struct iovec);

    if ((vec = luab_newvectornil(L, narg, &card, sz)) != NULL) {

        luab_table_init(L, 0);
        x = vec;

        do {
            f = *(caddr_t)x;

            if (lua_next(L, narg) == 0)
                errno = ENOENT;
            else {

                if ((lua_isnumber(L, -2) != 0) &&
                    (lua_isuserdata(L, -1) != 0)) {
                    luab_table_iovec_init(L, -1, x);
                } else
                    luab_sysexits_err(EX_DATAERR, __func__, EINVAL);

                x++;
            }
            lua_pop(L, 1);
        } while (f != luab_table_xsentinel_flag);
    }

    if (n != NULL)
        *n = card - 1;

    return (vec);
}

u_short *
luab_table_checklu_short(lua_State *L, int narg, size_t n)
{
    u_short *vec, *x, v;
    size_t card, sz;
    u_char f;

    sz = sizeof(u_short);
    vec = luab_newlvector(L, narg, n, sz, &card);

    luab_table_init(L, 0);
    x = vec;

    do {
        f = *(caddr_t)x;

        if (lua_next(L, narg) == 0)
            errno = ENOENT;
        else {

            if ((lua_isnumber(L, -2) != 0) &&
                (lua_isnumber(L, -1) != 0)) {
                v = (u_short)luab_tointeger(L, -1, USHRT_MAX);
                *x = (u_short)v;
            } else
                luab_sysexits_err(EX_DATAERR, __func__, EINVAL);

            x++;
        }
        lua_pop(L, 1);
    } while (f != luab_table_xsentinel_flag);

    return (vec);
}

int *
luab_table_checklint(lua_State *L, int narg, size_t n)
{
    int *vec, *x, v;
    size_t card, sz;
    u_char f;

    sz = sizeof(int);
    vec = luab_newlvector(L, narg, n, sz, &card);

    luab_table_init(L, 0);
    x = vec;

    do {
        f = *(caddr_t)x;

        if (lua_next(L, narg) == 0)
            errno = ENOENT;
        else {

            if ((lua_isnumber(L, -2) != 0) &&
                (lua_isnumber(L, -1) != 0)) {
                v = (int)luab_tointeger(L, -1, UINT_MAX);
                *x = (int)v;
            } else
                luab_sysexits_err(EX_DATAERR, __func__, EINVAL);

            x++;
        }
        lua_pop(L, 1);
    } while (f != luab_table_xsentinel_flag);

    return (vec);
}

gid_t *
luab_table_checklgid(lua_State *L, int narg, size_t n)
{
    gid_t *vec, *x, v;
    size_t card, sz;
    u_char f;

    sz = sizeof(gid_t);
    vec = luab_newlvector(L, narg, n, sz, &card);

    luab_table_init(L, 0);
    x = vec;

    do {
        f = *(caddr_t)x;

        if (lua_next(L, narg) == 0)
            errno = ENOENT;
        else {

            if ((lua_isnumber(L, -2) != 0) &&
                (lua_isnumber(L, -1) != 0)) {
                v = (gid_t)luab_tointeger(L, -1, INT_MAX);
                *x = (gid_t)v;
            } else
                luab_sysexits_err(EX_DATAERR, __func__, EINVAL);

            x++;
        }
        lua_pop(L, 1);
    } while (f != luab_table_xsentinel_flag);

    return (vec);
}

struct iovec *
luab_table_checkliovec(lua_State *L, int narg, size_t n)
{
    struct iovec *vec, *x;
    size_t card, sz;
    u_char f;

    sz = sizeof(struct iovec);
    vec = luab_newlvector(L, narg, n, sz, &card);

    luab_table_init(L, 0);
    x = vec;

    do {
        f = *(caddr_t)x;

        if (lua_next(L, narg) == 0)
            errno = ENOENT;
        else {

            if ((lua_isnumber(L, -2) != 0) &&
                (lua_isuserdata(L, -1) != 0)) {
                luab_table_iovec_init(L, -1, x);
            } else
                luab_sysexits_err(EX_DATAERR, __func__, EINVAL);

            x++;
        }
        lua_pop(L, 1);
    } while (f != luab_table_xsentinel_flag);

    return (vec);
}

struct timespec *
luab_table_checkltimespec(lua_State *L, int narg, size_t n)
{
    struct timespec *vec, *x, *v;
    size_t card, sz;
    u_char f;

    sz = sizeof(struct timespec);
    vec = luab_newlvector(L, narg, n, sz, &card);

    luab_table_init(L, 0);
    x = vec;

    do {
        f = *(caddr_t)x;
        if (lua_next(L, narg) == 0)
            errno = ENOENT;
        else {

            if ((lua_isnumber(L, -2) != 0) &&
                (lua_isuserdata(L, -1) != 0)) {
                v = luab_udata(L, -1, luab_mx(TIMESPEC), struct timespec *);
                (void)memmove(x, v, sz);
            } else
                luab_sysexits_err(EX_DATAERR, __func__, EINVAL);

            x++;
        }
        lua_pop(L, 1);
    } while (f != luab_table_xsentinel_flag);

    return (vec);
}

/*
 * Access functions, [C -> stack].
 *
 * Populates (LUA_TTABLE)s by elements from arrays either with
 *
 *  (a) primitives or
 *
 *  (b) C structures xxx{} by (LUA_TUSERDATA(XXX)).
 *
 */

void
luab_table_pushdouble(lua_State *L, int narg, double *vec, int new, int clr)
{
    double *x;
    size_t n;
    u_char f;

    if ((x = vec) != NULL) {
        luab_table_init(L, new);

        n = 1;
        
        while (1) {
            f = *(caddr_t)x;

            if (f != luab_table_xsentinel_flag) {
                luab_rawsetnumber(L, narg, n, *x);
                x++; n++;
            } else {
                errno = ENOENT;
                break;
            }
        }
        lua_pop(L, 0);

        if (clr == 1)
            luab_table_free(vec, n, sizeof(double));
    } else
        errno = ERANGE;
}

void
luab_table_pushgid(lua_State *L, int narg, gid_t *vec, int new, int clr)
{
    gid_t *x;
    size_t n;
    u_char f;

    if ((x = vec) != NULL) {
        luab_table_init(L, new);

        n = 1;
        
        while (1) {
            f = *(caddr_t)x;

            if (f != luab_table_xsentinel_flag) {
                luab_rawsetinteger(L, narg, n, *x);
                x++; n++;
            } else {
                errno = ENOENT;
                break;
            }
        }
        lua_pop(L, 0);

        if (clr == 1)
            luab_table_free(vec, n, sizeof(gid_t));
    } else
        errno = ERANGE;
}

void
luab_table_pushint(lua_State *L, int narg, int *vec, int new, int clr)
{
    int *x;
    size_t n;
    u_char f;

    if ((x = vec) != NULL) {
        luab_table_init(L, new);

        n = 1;
        
        while (1) {
            f = *(caddr_t)x;

            if (f != luab_table_xsentinel_flag) {
                luab_rawsetinteger(L, narg, n, *x);
                x++; n++;
            } else {
                errno = ENOENT;
                break;
            }
        }
        lua_pop(L, 0);

        if (clr == 1)
            luab_table_free(vec, n, sizeof(int));
    } else
        errno = ERANGE;
}

void
luab_table_pushiovec(lua_State *L, int narg, struct iovec *vec, int new, int clr)
{
    struct iovec *iov;
    size_t n;
    u_char f;

    if ((iov = vec) != NULL) {
        luab_table_init(L, new);

        n = 1;
        
        while (1) {
            f = *(caddr_t)iov;

            if (f != luab_table_xsentinel_flag) {
                luab_iov_rawsetxdata(L, narg, n, iov);
                iov++; n++;
            } else {
                errno = ENOENT;
                break;
            }
        }
        lua_pop(L, 0);

        if (clr == 1)
            luab_table_iovec_free(vec);
    } else
        errno = ERANGE;
}

void
luab_table_pushtimespec(lua_State *L, int narg, struct timespec *vec,
    int new, int clr)
{
    struct timespec *ts;
    size_t n;
    u_char f;

    if ((ts = vec) != NULL) {
        luab_table_init(L, new);

        n = 1;

        while (1) {
            f = *(caddr_t)ts;

            if (f != luab_table_xsentinel_flag) {
                luab_rawsetudata(L, narg, luab_mx(TIMESPEC), n, ts);
                ts++; n++;
            } else {
                errno = ENOENT;
                break;
            }
        }
        lua_pop(L, 0);

        if (clr == 1)
            luab_table_free(vec, n, sizeof(struct timespec));
    } else
        errno = ERANGE;
}
