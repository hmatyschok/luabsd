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

#include <sys/types.h>

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t div_type;
extern luab_module_t ldiv_type;

#define LUABSD_STDLIB_LIB_ID    1593623310
#define LUABSD_STDLIB_LIB_KEY    "stdlib"

extern luab_module_t luab_stdlib_lib;

/*
 * Service primitives.
 */

/***
 * abort(3) - cause abnormal programm termination
 *
 * @function abort
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.stdlib.abort()
 */
static int
luab_abort(lua_State *L)
{
    (void)luab_checkmaxargs(L, 0);

    abort();
        /* NOTREACHED */
    return (luab_pusherr(L, 0));
}

/***
 * abs(3) - integer absoloute value function
 *
 * @function abs
 *
 * @param j                 Integer.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage k [, err, msg ] = bsd.stdlib.abs(j)
 */
static int
luab_abs(lua_State *L)
{
    int j, n;

    (void)luab_checkmaxargs(L, 1);

    j = (int)luab_checkinteger(L, 1, INT_MAX);
    n = abs(j);

    return (luab_pusherr(L, n));
}

/***
 * atof(3) - convert ASCII string to double
 *
 * @function atof
 *
 * @param nptr              ASCII string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.atof(nptr)
 */
static int
luab_atof(lua_State *L)
{
    const char *nptr;
    double n;

    (void)luab_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    n = atof(nptr);

    return (luab_pushnumber(L, n));
}

/***
 * atoi(3) - convert ASCII string to integer
 *
 * @function atoi
 *
 * @param nptr              ASCII string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.atoi(nptr)
 */
static int
luab_atoi(lua_State *L)
{
    const char *nptr;
    double n;

    (void)luab_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    n = atoi(nptr);

    return (luab_pushnumber(L, n));
}

/***
 * atol(3) - convert ASCII string to long
 *
 * @function atol
 *
 * @param nptr              ASCII string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.atol(nptr)
 */
static int
luab_atol(lua_State *L)
{
    const char *nptr;
    double n;

    (void)luab_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    n = atol(nptr);

    return (luab_pushnumber(L, n));
}

/***
 * div(3) - return quotient and reminder from division
 *
 * @function div
 *
 * @param num               Number.
 * @param denom             Denominator.
 *
 * @return (LUA_T{NIL,USERDATA(DIV)} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (div [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage div [, err, msg ] = bsd.stdlib.div(num, denom)
 */
static int
luab_div(lua_State *L)
{
    int num, denom;
    div_t data;

    (void)luab_checkmaxargs(L, 2);

    num = (int)luab_checkinteger(L, 1, INT_MAX);
    denom = (int)luab_checkinteger(L, 2, INT_MAX);

    data = div(num, denom);

    return (luab_pushudata(L, &div_type, &data));
}

/***
 * exit(3) - perform normal programm termination
 *
 * @function exit
 *
 * @param status            Possible values from
 *
 *                              bsd.stdlib.EXIT_{FAILURE,SUCCESS}
 *
 *                          are passed as argument.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage div [, err, msg ] = bsd.stdlib.exit(status)
 */
static int
luab_exit(lua_State *L)
{
    int status;

    (void)luab_checkmaxargs(L, 1);

    status = (int)luab_checkinteger(L, 1, INT_MAX);
    exit(status);
                        /* NOTREACHED */
    return (luab_pusherr(L, 0));
}

/***
 * getenv(3) - perform normal programm termination
 *
 * @function getenv
 *
 * @param name              Requested variable.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (value [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage value [, err, msg ] = bsd.stdlib.getenv(status)
 */
static int
luab_getenv(lua_State *L)
{
    const char *name;
    caddr_t value;

    (void)luab_checkmaxargs(L, 1);

    name = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    value = getenv(name);

    return (luab_pushstring(L, value));
}

/***
 * labs(3) - return the absolute value of a long integer
 *
 * @function labs
 *
 * @param j                 Integer.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.labs(j)
 */
static int
luab_labs(lua_State *L)
{
    long j, n;

    (void)luab_checkmaxargs(L, 1);

    j = luab_checkinteger(L, 1, j);
    n = labs(j);

    return (luab_pusherr(L, n));
}

/***
 * ldiv(3) - return quotient and reminder from division
 *
 * @function ldiv
 *
 * @param num               Number.
 * @param denom             Denominator.
 *
 * @return (LUA_T{NIL,USERDATA(DIV)} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (ldiv [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage ldiv [, err, msg ] = bsd.stdlib.div(num, denom)
 */
static int
luab_ldiv(lua_State *L)
{
    int num, denom;
    ldiv_t data;

    (void)luab_checkmaxargs(L, 2);

    num = (int)luab_checkinteger(L, 1, INT_MAX);
    denom = (int)luab_checkinteger(L, 2, INT_MAX);

    data = ldiv(num, denom);

    return (luab_pushudata(L, &ldiv_type, &data));
}

/***
 * mblen(3) - get number of bytes in a character
 *
 * @function mblen
 *
 * @param mbchar            Multy-byte character, (LUA_TUSERDATA(IOVEC)).
 * @param nbytes            Length in bytes about to be axaminated.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (len [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ldiv [, err, msg ] = bsd.stdlib.mblen(mbchar, nbytes)
 */
static int
luab_mblen(lua_State *L)
{
    luab_iovec_t *buf;
    size_t mblen;
    caddr_t bp;
    int len;

    (void)luab_checkmaxargs(L, 1)

    if (((bp = buf->iov.iov_base) != NULL) &&
        (size <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = getcwd(bp, size)) != NULL)
                buf->iov.iov_len = size;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = NULL;
        }
    } else {
        errno = ENXIO;
        status = NULL;
    }
    return (luab_pusherr(L, len));
}


#if __BSD_VISIBLE
static int
luab_arc4random(lua_State *L)
{
    uint32_t n;

    (void)luab_checkmaxargs(L, 0);

    n = arc4random();

    return (luab_pusherr(L, n));
}

static int
luab_arc4random_uniform(lua_State *L)
{
    uint32_t ub, n;

    (void)luab_checkmaxargs(L, 1);

    ub = luab_checkinteger(L, 1, INT_MAX);
    n = arc4random_uniform(ub);

    return (luab_pusherr(L, n));
}
#endif

#if __ISO_C_VISIBLE >= 1999 || defined(__cplusplus)
#ifdef __LONG_LONG_SUPPORTED
/***
 * atoll(3) - convert ASCII string to long long
 *
 * @function atoll
 *
 * @param nptr              ASCII string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.atoll(nptr)
 */
static int
luab_atoll(lua_State *L)
{
    const char *nptr;
    double n;

    (void)luab_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    n = atoll(nptr);

    return (luab_pushnumber(L, n));
}
#endif /* __LONG_LONG_SUPPORTED */
#endif /* __ISO_C_VISIBLE >= 1999 */

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(DIV)).
 *
 * @function div_create
 *
 * @param data          Instance of (LUA_TUSERDATA(DIV)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (div [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage div [, err, msg ] = bsd.stdlib.div_create([ data ])
 */
static int
luab_div_create(lua_State *L)
{
    return (luab_create(L, 1, &div_type, NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(LDIV)).
 *
 * @function ldiv_create
 *
 * @param data          Instance of (LUA_TUSERDATA(LDIV)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (ldiv [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage ldiv [, err, msg ] = bsd.stdlib.ldiv_create([ data ])
 */
static int
luab_ldiv_create(lua_State *L)
{
    return (luab_create(L, 1, &ldiv_type, NULL));
}

/*
 * Interface against <stdlib.h>.
 */

static luab_table_t luab_stdlib_vec[] = {
    LUABSD_INT("EXIT_FAILURE",  EXIT_FAILURE),
    LUABSD_INT("EXIT_SUCCESS",  EXIT_SUCCESS),
    LUABSD_INT("RAND_MAX",  RAND_MAX),
    LUABSD_FUNC("abort",    luab_abort),
    LUABSD_FUNC("abs",  luab_abs),
    LUABSD_FUNC("atof", luab_atof),
    LUABSD_FUNC("atoi", luab_atoi),
    LUABSD_FUNC("atol", luab_atol),
    LUABSD_FUNC("div",  luab_div),
    LUABSD_FUNC("exit", luab_exit),
    LUABSD_FUNC("getenv",   luab_getenv),
    LUABSD_FUNC("labs", luab_labs),
    LUABSD_FUNC("ldiv", luab_ldiv),
    LUABSD_FUNC("mblen",   luab_mblen),
#if __BSD_VISIBLE
    LUABSD_FUNC("arc4random", luab_arc4random),
    LUABSD_FUNC("arc4random_uniform", luab_arc4random_uniform),
#endif
#if __ISO_C_VISIBLE >= 1999 || defined(__cplusplus)
#ifdef __LONG_LONG_SUPPORTED
    LUABSD_FUNC("atoll",    luab_atoll),
#endif /* __LONG_LONG_SUPPORTED */
#endif /* __ISO_C_VISIBLE >= 1999 */
    LUABSD_FUNC("div_create",   luab_div_create),
    LUABSD_FUNC("ldiv_create",   luab_ldiv_create),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_stdlib_lib = {
    .cookie = LUABSD_STDLIB_LIB_ID,
    .name = LUABSD_STDLIB_LIB_KEY,
    .vec = luab_stdlib_vec,
};
