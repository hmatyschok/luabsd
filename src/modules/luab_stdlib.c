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

#include <sys/param.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t hook_type;
extern luab_module_t div_type;
extern luab_module_t ldiv_type;
extern luab_module_t lldiv_type;

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
 * @param nptr              Specifies number by ASCII string.
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
 * @param nptr              Specifies number by ASCII string.
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
 * @param nptr              Specifies number by ASCII string.
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

    j = luab_checkinteger(L, 1, LONG_MAX);
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
 * @usage ldiv [, err, msg ] = bsd.stdlib.ldiv(num, denom)
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
    size_t nbytes;
    caddr_t bp;
    int len;

    (void)luab_checkmaxargs(L, 1);

    buf = luab_udata(L, 1, &iovec_type, luab_iovec_t *);
    nbytes = (size_t)luab_checkinteger(L, 2,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if (((bp = buf->iov.iov_base) != NULL) &&
        (nbytes <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            len = mblen(bp, nbytes);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            len = -1;
        }
    } else {
        errno = ENXIO;
        len = -1;
    }
    return (luab_pusherr(L, len));
}

/***
 * mbstowcs(3) - convert a character to wide-character code
 *
 * @function mbstowcs
 *
 * @param wcharp            Multy-byte character, (LUA_TUSERDATA(HOOK)).
 * @param mbchar            Single-byte character, (LUA_TUSERDATA(HOOK)).
 * @param nbytes            Specifies number of bytes for examination.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.mbtowcs(wcstring, mbstring, nbytes)
 */
static int
luab_mbstowcs(lua_State *L)
{
    luab_iovec_t *buf;
    const char *mbstring;
    size_t nbytes;
    wchar_t *bp;
    int len;

    (void)luab_checkmaxargs(L, 3);

    buf = luab_udata(L, 1, &iovec_type, luab_iovec_t *);
    mbstring = luab_checklstring(L, 2, LUAL_BUFFERSIZE);
    nbytes = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if (((bp = buf->iov.iov_base) != NULL) &&
        (nbytes <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((len = mbstowcs(bp, mbstring, nbytes)) > 0)
                buf->iov.iov_len = len;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            len = -1;
        }
    } else {
        errno = ENXIO;
        len = -1;
    }
    return (luab_pusherr(L, len));
}

/***
 * mbtowc(3) - convert a character to wide-character code
 *
 * @function mbtowc
 *
 * @param wcharp            Multy-byte character, (LUA_TUSERDATA(HOOK)).
 * @param mbchar            Single-byte character, (LUA_TUSERDATA(HOOK)).
 * @param nbytes            Specifies number of bytes for examination.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.mbtowc(wcharp, mbchar, nbytes)
 */
static int
luab_mbtowc(lua_State *L)
{
    luab_type_u *h0, *h1;
    size_t nbytes;
    wchar_t *wcharp;
    const char *mbchar;
    int status;

    (void)luab_checkmaxargs(L, 3);

    h0 = luab_udata(L, 1, &hook_type, luab_type_u *);
    h1 = luab_udata(L, 2, &hook_type, luab_type_u *);
    nbytes = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    wcharp = &(h0->un_wc);
    mbchar = &(h1->un_char);

    status = mbtowc(wcharp, mbchar, nbytes);

    return (luab_pusherr(L, status));
}

/***
 * rand(3) - bad random number generator
 *
 * @function rand
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.rand()
 */
static int
luab_rand(lua_State *L)
{
    int n;

    (void)luab_checkmaxargs(L, 0);

    n = rand();

    return (luab_pusherr(L, n));
}

/***
 * srand(3) - bad random number generator
 *
 * @function srand
 *
 * @param seed              Seed for next random sequence.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.stdlib.srand(seed)
 */
static int
luab_srand(lua_State *L)
{
    u_int seed;

    (void)luab_checkmaxargs(L, 1);

    seed = (u_int)luab_checkinteger(L, 1, INT_MAX);
    srand(seed);

    return (luab_pusherr(L, 0));
}

/***
 * strtod(3) - convert ASCII string to floating point
 *
 * @function strtod
 *
 * @param nptr              Specifies number by ASCII string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.strtod(nptr)
 */
static int
luab_strtod(lua_State *L)
{
    const char *nptr;
    double n;

    (void)luab_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    n = strtod(nptr, NULL);

    return (luab_pushnumber(L, n));
}

/***
 * strtof(3) - convert ASCII string to floating point
 *
 * @function strtof
 *
 * @param nptr              Specifies number by ASCII string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.strtof(nptr)
 */
static int
luab_strtof(lua_State *L)
{
    const char *nptr;
    float n;

    (void)luab_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    n = strtof(nptr, NULL);

    return (luab_pushnumber(L, n));
}

/***
 * strtold(3) - convert ASCII string to floating point
 *
 * @function strtold
 *
 * @param nptr              Specifies number by ASCII string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.strtold(nptr)
 */
static int
luab_strtold(lua_State *L)
{
    const char *nptr;
    long double n;

    (void)luab_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    n = strtold(nptr, NULL);

    return (luab_pushnumber(L, n));
}

/***
 * strtoul(3) - convert ASCII string to an unsigned number
 *
 * @function strtoul
 *
 * @param nptr              Specifies number by ASCII string.
 * @param base              Specifies conversion base.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.strtoul(nptr, base)
 */
static int
luab_strtoul(lua_State *L)
{
    const char *nptr;
    int base;
    u_long n;

    (void)luab_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    base = luab_checkinteger(L, 2, INT_MAX);

    n = strtoul(nptr, NULL, base);

    return (luab_pushnumber(L, n));
}

/***
 * system(3) - pass a command to the shell
 *
 * @function system
 *
 * @param string             Specifies by /bin/sh executed command.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.stdlib.strtoul(nptr)
 */
static int
luab_system(lua_State *L)
{
    const char *string;
    int status;

    (void)luab_checkmaxargs(L, 1);

    string = luab_checklstring(L, 1, MAXPATHLEN);
    status = system(string);

    return (luab_pusherr(L, status));
}

/***
 * wctomb(3) - convert a wide-character code character to a character
 *
 * @function wctomb
 *
 * @param mbchar           Character, (LUA_TUSERDATA(HOOK)).
 * @param wchar            Wide-character, (LUA_TUSERDATA(HOOK)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.wctomb(mbchar, wchar)
 */
static int
luab_wctomb(lua_State *L)
{
    luab_type_u *h0;
    wchar_t wchar;
    char *mbchar;
    int status;

    (void)luab_checkmaxargs(L, 2);

    h0 = luab_udata(L, 1, &hook_type, luab_type_u *);
    wchar = (wchar_t)luab_checkinteger(L, 2, LONG_MAX);

    mbchar = &(h0->un_char);
    status = wctomb(mbchar, wchar);

    return (luab_pusherr(L, status));
}

/***
 * wcstombs(3) - convert a wide-character string character to a character string
 *
 * @function wcstombs
 *
 * @param msbstring         Character string, (LUA_TUSERDATA(IOVEC)).
 * @param wcstring          Wide-character string, (LUA_TUSERDATA(IOVEC)).
 * @param nbytes            Number of bytes of character string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.wcstombs(mbchar, wchar, nbytes)
 */
static int
luab_wcstombs(lua_State *L)
{
    luab_iovec_t *buf1, *buf2;
    size_t nbytes;
    caddr_t dst;
    wchar_t *src;
    ssize_t len;

    (void)luab_checkmaxargs(L, 3);

    buf1 = luab_udata(L, 1, &iovec_type, luab_iovec_t *);
    buf2 = luab_udata(L, 2, &iovec_type, luab_iovec_t *);
    nbytes = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if (((dst = buf1->iov.iov_base) != NULL) &&
        ((src = (wchar_t *)buf2->iov.iov_base) != NULL) &&
        (nbytes <= (size_t)buf2->iov_max_len) &&
        (nbytes <= (size_t)buf1->iov_max_len) &&
        (buf1->iov_flags & IOV_BUFF) &&
        (buf2->iov_flags & IOV_BUFF)) {

        if (((buf2->iov_flags & IOV_LOCK) == 0) &&
            ((buf1->iov_flags & IOV_LOCK) == 0)) {
            buf1->iov_flags |= IOV_LOCK;
            buf2->iov_flags |= IOV_LOCK;

            if ((len = wcstombs(dst, src, nbytes)) > 0)
                buf1->iov.iov_len = len;

            buf2->iov_flags &= ~IOV_LOCK;
            buf1->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            len = -1;
        }
    } else {
        errno = ENXIO;
        len = -1;
    }
    return (luab_pusherr(L, len));
}

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

/***
 * llabs(3) - return the absolute value of a long long integer
 *
 * @function llabs
 *
 * @param j                 Integer.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.llabs(j)
 */
static int
luab_llabs(lua_State *L)
{
    long j, n;

    (void)luab_checkmaxargs(L, 1);

    j = luab_checkinteger(L, 1, LONG_MAX);
    n = llabs(j);

    return (luab_pusherr(L, n));
}

/***
 * lldiv(3) - return quotient and reminder from division
 *
 * @function lldiv
 *
 * @param num               Number.
 * @param denom             Denominator.
 *
 * @return (LUA_T{NIL,USERDATA(DIV)} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (lldiv [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage lldiv [, err, msg ] = bsd.stdlib.lldiv(num, denom)
 */
static int
luab_lldiv(lua_State *L)
{
    int num, denom;
    lldiv_t data;

    (void)luab_checkmaxargs(L, 2);

    num = (int)luab_checkinteger(L, 1, INT_MAX);
    denom = (int)luab_checkinteger(L, 2, INT_MAX);

    data = lldiv(num, denom);

    return (luab_pushudata(L, &lldiv_type, &data));
}

/***
 * strtoll(3) - convert ASCII string to a (long long) number
 *
 * @function strtoll
 *
 * @param nptr              Specifies number by ASCII string.
 * @param base              Specifies conversion base.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.strtoll(nptr, base)
 */
static int
luab_strtoll(lua_State *L)
{
    const char *nptr;
    int base;
    long long n;

    (void)luab_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    base = luab_checkinteger(L, 2, INT_MAX);

    n = strtoll(nptr, NULL, base);

    return (luab_pushnumber(L, n));
}

/***
 * strtoull(3) - convert ASCII string to an (unsigned long long) number
 *
 * @function strtoull
 *
 * @param nptr              Specifies number by ASCII string.
 * @param base              Specifies conversion base.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage n [, err, msg ] = bsd.stdlib.strtoull(nptr, base)
 */
static int
luab_strtoull(lua_State *L)
{
    const char *nptr;
    int base;
    unsigned long long n;

    (void)luab_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    base = luab_checkinteger(L, 2, INT_MAX);

    n = strtoull(nptr, NULL, base);

    return (luab_pushnumber(L, n));
}
#endif /* __LONG_LONG_SUPPORTED */
/***
 * _Exit(3) - perform normal programm termination
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
 * @usage rat [, err, msg ] = bsd.stdlib._Exit(status)
 */
static int
luab_Exit(lua_State *L)
{
    int status;

    (void)luab_checkmaxargs(L, 1);

    status = (int)luab_checkinteger(L, 1, INT_MAX);
    _Exit(status);
                        /* NOTREACHED */
    return (luab_pusherr(L, 0));
}
#endif /* __ISO_C_VISIBLE >= 1999 */

#if __ISO_C_VISIBLE >= 2011 || __cplusplus >= 201103L
/***
 * quick_exit(3) - perform normal programm termination
 *
 * @function quick_exit
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
 * @usage ret [, err, msg ] = bsd.stdlib.quick_exit(status)
 */
static int
luab_quick_exit(lua_State *L)
{
    int status;

    (void)luab_checkmaxargs(L, 1);

    status = (int)luab_checkinteger(L, 1, INT_MAX);
    quick_exit(status);

    return (luab_pusherr(L, 0));
}
#endif /* __ISO_C_VISIBLE >= 2011 */

#if __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE
/***
 * realpath(3) - returns the canoncicalized absoulte pathname
 *
 * @function realpath
 *
 * @param pathanme          Specifies the pathname.
 * @param resolved_path     Result argument, (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.stdlib.realpath(pathname, resolved_path)
 */
static int
luab_realpath(lua_State *L)
{
    const char *pathname;
    luab_iovec_t *buf;
    caddr_t bp;
    int status;

    (void)luab_checkmaxargs(L, 2);

    pathname = luab_checklstring(L, 1, MAXPATHLEN);
    buf = luab_udata(L, 2, &iovec_type, luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len >= MAXPATHLEN) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if (realpath(pathname, bp) != NULL) {
                buf->iov.iov_len = strlen(bp);
                status = 0;
            } else
                status = -1;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}
#endif

#if __POSIX_VISIBLE >= 199506
/***
 * rand_r(3) - bad random number generator
 *
 * @function rand_r
 *
 * @param ctx               Result argument, (LUA_TUSERDATA(HOOK)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (n [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.stdlib.rand_r(ctx)
 */
static int
luab_rand_r(lua_State *L)
{
    luab_type_u *h0;
    u_int *ctx;
    int n;

    (void)luab_checkmaxargs(L, 1);

    h0 = luab_udata(L, 1, &hook_type, luab_type_u *);
    ctx = &(h0->un_u_int);

    n = rand_r(ctx);

    return (luab_pusherr(L, n));
}
#endif

#if __POSIX_VISIBLE >= 200112
/***
 * setenv(3) - environment variable function
 *
 * @function setenv
 *
 * @param name              Name of environment variable.
 * @param value             Value of environment variable.
 * @param overwrite         If not 0, then (name,value) will be set.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.stdlib.setenv(name, value, overwrite)
 */
static int
luab_setenv(lua_State *L)
{
    const char *name;
    const char *value;
    int overwrite;
    int status;

    (void)luab_checkmaxargs(L, 3);

    name = luab_checklstring(L, 1, NAME_MAX);
    value = luab_checklstring(L, 2, NAME_MAX);
    overwrite = (int)luab_checkinteger(L, 3, INT_MAX);

    status = setenv(name, value, overwrite);

    return (luab_pusherr(L, status));
}

/***
 * unsetenv(3) - environment variable function
 *
 * @function unsetenv
 *
 * @param name              Name of environment variable.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.stdlib.unsetenv(name)
 */
static int
luab_unsetenv(lua_State *L)
{
    const char *name;
    int status;

    (void)luab_checkmaxargs(L, 1);

    name = luab_checklstring(L, 1, NAME_MAX);
    status = unsetenv(name);

    return (luab_pusherr(L, status));
}
#endif

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

#if __ISO_C_VISIBLE >= 1999 || defined(__cplusplus)
#ifdef __LONG_LONG_SUPPORTED
/***
 * Generator function - create an instance of (LUA_TUSERDATA(LLDIV)).
 *
 * @function lldiv_create
 *
 * @param data          Instance of (LUA_TUSERDATA(LLDIV)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (lldiv [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage lldiv [, err, msg ] = bsd.stdlib.lldiv_create([ data ])
 */
static int
luab_lldiv_create(lua_State *L)
{
    return (luab_create(L, 1, &lldiv_type, NULL));
}
#endif /* __LONG_LONG_SUPPORTED */
#endif /* __ISO_C_VISIBLE >= 1999 */

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
    LUABSD_FUNC("mbstowcs", luab_mbstowcs),
    LUABSD_FUNC("mbtowc",   luab_mbtowc),
    LUABSD_FUNC("rand",   luab_rand),
    LUABSD_FUNC("srand",    luab_srand),
    LUABSD_FUNC("strtod",   luab_strtod),
    LUABSD_FUNC("strtof",   luab_strtof),
    LUABSD_FUNC("strtold",  luab_strtold),
    LUABSD_FUNC("strtoul",  luab_strtoul),
    LUABSD_FUNC("system",   luab_system),
    LUABSD_FUNC("wctomb",   luab_wctomb),
    LUABSD_FUNC("wcstombs", luab_wcstombs),
#if __ISO_C_VISIBLE >= 1999 || defined(__cplusplus)
#ifdef __LONG_LONG_SUPPORTED
    LUABSD_FUNC("atoll",    luab_atoll),
    LUABSD_FUNC("llabs",    luab_llabs),
    LUABSD_FUNC("lldiv",    luab_lldiv),
    LUABSD_FUNC("strtoll",  luab_strtoll),
    LUABSD_FUNC("strtoull", luab_strtoull),
#endif /* __LONG_LONG_SUPPORTED */
    LUABSD_FUNC("_Exit",    luab_Exit),
#endif /* __ISO_C_VISIBLE >= 1999 */
#if __ISO_C_VISIBLE >= 2011 || __cplusplus >= 201103L
    LUABSD_FUNC("quick_exit",   luab_quick_exit),
#endif /* __ISO_C_VISIBLE >= 2011 */
#if __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE
    LUABSD_FUNC("realpath", luab_realpath),
#endif
#if __POSIX_VISIBLE >= 199506
    LUABSD_FUNC("rand_r",   luab_rand_r),
#endif
#if __POSIX_VISIBLE >= 200112
    LUABSD_FUNC("setenv",   luab_setenv),
    LUABSD_FUNC("unsetenv", luab_unsetenv),
#endif
#if __BSD_VISIBLE
    LUABSD_FUNC("arc4random", luab_arc4random),
    LUABSD_FUNC("arc4random_uniform", luab_arc4random_uniform),
#endif
    LUABSD_FUNC("div_create",   luab_div_create),
    LUABSD_FUNC("ldiv_create",   luab_ldiv_create),
#if __ISO_C_VISIBLE >= 1999 || defined(__cplusplus)
#ifdef __LONG_LONG_SUPPORTED
    LUABSD_FUNC("lldiv_create", luab_lldiv_create),
#endif /* __LONG_LONG_SUPPORTED */
#endif /* __ISO_C_VISIBLE >= 1999 */
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_stdlib_lib = {
    .cookie = LUABSD_STDLIB_LIB_ID,
    .name = LUABSD_STDLIB_LIB_KEY,
    .vec = luab_stdlib_vec,
};
