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

#define LUAB_STDLIB_LIB_ID    1593623310
#define LUAB_STDLIB_LIB_KEY    "stdlib"

extern luab_module_t luab_stdlib_lib;

#define LUAB_ABORT2_MAXBUFLEN    128

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
 * @usage ret [, err, msg ] = bsd.stdlib.abort()
 */
static int
luab_abort(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);

    abort();
        /* NOTREACHED */
    return (luab_pushxinteger(L, luab_env_success));
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
 * @usage k [, err, msg ] = bsd.stdlib.abs(j)
 */
static int
luab_abs(lua_State *L)
{
    luab_module_t *m;
    int j, k;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    j = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    k = abs(j);

    return (luab_pushxinteger(L, k));
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
 * @usage n [, err, msg ] = bsd.stdlib.atof(nptr)
 */
static int
luab_atof(lua_State *L)
{
    const char *nptr;
    double n;

    (void)luab_core_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    n = atof(nptr);

    return (luab_pushxnumber(L, n, 0));
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

    (void)luab_core_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    n = atoi(nptr);

    return (luab_pushxnumber(L, n, 0));
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
 * @usage n [, err, msg ] = bsd.stdlib.atol(nptr)
 */
static int
luab_atol(lua_State *L)
{
    const char *nptr;
    double n;

    (void)luab_core_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    n = atol(nptr);

    return (luab_pushxnumber(L, n, 0));
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
 * @usage div [, err, msg ] = bsd.stdlib.div(num, denom)
 */
static int
luab_div(lua_State *L)
{
    luab_module_t *m;
    int num, denom;
    div_t data;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(INT, TYPE, __func__);
    num = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    denom = (int)luab_checkxinteger(L, 2, m, luab_env_int_max);

    data = div(num, denom);

    return (luab_pushxdata(L, luab_xmod(DIV, TYPE, __func__), &data));
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
 * @usage div [, err, msg ] = bsd.stdlib.exit(status)
 */
static int
luab_exit(lua_State *L)
{
    luab_module_t *m;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    status = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    exit(status);
                        /* NOTREACHED */
    return (luab_pushxinteger(L, luab_env_success));
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
 * @usage value [, err, msg ] = bsd.stdlib.getenv(status)
 */
static int
luab_getenv(lua_State *L)
{
    const char *name;
    caddr_t value;

    (void)luab_core_checkmaxargs(L, 1);

    name = luab_checklstring(L, 1, luab_env_buf_max, NULL);
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
 * @usage k [, err, msg ] = bsd.stdlib.labs(j)
 */
static int
luab_labs(lua_State *L)
{
    luab_module_t *m;
    long j, k;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LONG, TYPE, __func__);
    j = (long)luab_checkxinteger(L, 1, m, luab_env_long_max);
    k = labs(j);

    return (luab_pushxinteger(L, k));
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
 * @usage ldiv [, err, msg ] = bsd.stdlib.ldiv(num, denom)
 */
static int
luab_ldiv(lua_State *L)
{
    luab_module_t *m;
    int num, denom;
    ldiv_t data;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(INT, TYPE, __func__);
    num = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    denom = (int)luab_checkxinteger(L, 2, m, luab_env_int_max);

    data = ldiv(num, denom);

    return (luab_pushxdata(L, luab_xmod(LINGER, TYPE, __func__), &data));
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
 * @usage ldiv [, err, msg ] = bsd.stdlib.mblen(mbchar, nbytes)
 */
static int
luab_mblen(lua_State *L)
{
    luab_module_t *m;
    luab_iovec_t *buf;
    size_t nbytes;
    caddr_t bp;
    int len;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IOVEC, TYPE, __func__);
    buf = luab_udata(L, 1, m, luab_iovec_t *);
    nbytes = (size_t)luab_checklinteger(L, 2, 0);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (nbytes <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            len = mblen(bp, nbytes);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            len = luab_env_error;
        }
    } else {
        errno = ERANGE;
        len = luab_env_error;
    }
    return (luab_pushxinteger(L, len));
}

/***
 * mbstowcs(3) - convert a character to wide-character code
 *
 * @function mbstowcs
 *
 * @param wcharp            Multy-byte character, (LUA_TUSERDATA(WCHAR)).
 * @param mbchar            Single-byte character, (LUA_TUSERDATA(CHAR)).
 * @param nbytes            Specifies number of bytes for examination.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.mbtowcs(wcstring, mbstring, nbytes)
 */
static int
luab_mbstowcs(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *buf;
    const char *mbstring;
    size_t nbytes;
    wchar_t *bp;
    int len;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    mbstring = luab_checklstring(L, 2, luab_env_buf_max, NULL);
    nbytes = (size_t)luab_checkxlinteger(L, 3, m1, 0);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (nbytes <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((len = mbstowcs(bp, mbstring, nbytes)) > 0)
                buf->iov.iov_len = len;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            len = luab_env_error;
        }
    } else {
        errno = ERANGE;
        len = luab_env_error;
    }
    return (luab_pushxinteger(L, len));
}

/***
 * mbtowc(3) - convert a character to wide-character code
 *
 * @function mbtowc
 *
 * @param wcharp            Multy-byte character, (LUA_TUSERDATA(WCHAR)).
 * @param mbchar            Single-byte character, (LUA_TUSERDATA(CHAR)).
 * @param nbytes            Specifies number of bytes for examination.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.mbtowc(wcharp, mbchar, nbytes)
 */
static int
luab_mbtowc(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    wchar_t *wcharp;
    const char *mbchar;
    size_t nbytes;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(WCHAR, TYPE, __func__);
    m1 = luab_xmod(CHAR, TYPE, __func__);
    m2 = luab_xmod(SIZE, TYPE, __func__);

    wcharp = luab_udata(L, 1, m0, wchar_t *);
    mbchar = luab_udata(L, 2, m1, const char *);
    nbytes = (size_t)luab_checkxlinteger(L, 3, m2, 0);

    status = mbtowc(wcharp, mbchar, nbytes);

    return (luab_pushxinteger(L, status));
}

/***
 * rand(3) - bad random number generator
 *
 * @function rand
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.rand()
 */
static int
luab_rand(lua_State *L)
{
    int n;

    (void)luab_core_checkmaxargs(L, 0);

    n = rand();

    return (luab_pushxinteger(L, n));
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
 * @usage ret [, err, msg ] = bsd.stdlib.srand(seed)
 */
static int
luab_srand(lua_State *L)
{
    luab_module_t *m;
    u_int seed;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT, TYPE, __func__);
    seed = (u_int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    srand(seed);

    return (luab_pushxinteger(L, luab_env_success));
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
 * @usage n [, err, msg ] = bsd.stdlib.strtod(nptr)
 */
static int
luab_strtod(lua_State *L)
{
    const char *nptr;
    double n;

    (void)luab_core_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    n = strtod(nptr, NULL);

    return (luab_pushxnumber(L, n, 0));
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
 * @usage n [, err, msg ] = bsd.stdlib.strtof(nptr)
 */
static int
luab_strtof(lua_State *L)
{
    const char *nptr;
    float n;

    (void)luab_core_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    n = strtof(nptr, NULL);

    return (luab_pushxnumber(L, n, 0));
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
 * @usage n [, err, msg ] = bsd.stdlib.strtold(nptr)
 */
static int
luab_strtold(lua_State *L)
{
    const char *nptr;
    long double n;

    (void)luab_core_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    n = strtold(nptr, NULL);

    return (luab_pushxnumber(L, n, 0));
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
    luab_module_t *m;
    const char *nptr;
    int base;
    u_long n;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    nptr = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    base = luab_checkxinteger(L, 2, m, luab_env_int_max);

    n = strtoul(nptr, NULL, base);

    return (luab_pushxnumber(L, n, 0));
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
 * @usage ret [, err, msg ] = bsd.stdlib.strtoul(nptr)
 */
static int
luab_system(lua_State *L)
{
    const char *string;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    string = luab_checklstring(L, 1, luab_env_path_max, NULL);
    status = system(string);

    return (luab_pushxinteger(L, status));
}

/***
 * wctomb(3) - convert a wide-character code character to a character
 *
 * @function wctomb
 *
 * @param mbchar           Character, (LUA_TUSERDATA(CHAR)).
 * @param wchar            Wide-character, (LUA_TUSERDATA(WCHAR)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.wctomb(mbchar, wchar)
 */
static int
luab_wctomb(lua_State *L)
{
    luab_module_t *m0, *m1;
    char *mbchar;
    wchar_t wchar;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(CHAR, TYPE, __func__);
    m1 = luab_xmod(WCHAR, TYPE, __func__);

    mbchar = luab_udata(L, 1, m0, char *);
    wchar = (wchar_t)luab_checkxinteger(L, 2, m1, luab_env_long_max);

    status = wctomb(mbchar, wchar);

    return (luab_pushxinteger(L, status));
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
 * @usage n [, err, msg ] = bsd.stdlib.wcstombs(mbchar, wchar, nbytes)
 */
static int
luab_wcstombs(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *buf1, *buf2;
    size_t nbytes;
    caddr_t dst;
    wchar_t *src;
    ssize_t len;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);

    buf1 = luab_udata(L, 1, m0, luab_iovec_t *);
    buf2 = luab_udata(L, 2, m0, luab_iovec_t *);
    nbytes = (size_t)luab_checkxlinteger(L, 3, m1, 0);

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
            len = luab_env_error;
        }
    } else {
        errno = ERANGE;
        len = luab_env_error;
    }
    return (luab_pushxinteger(L, len));
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
 * @usage n [, err, msg ] = bsd.stdlib.atoll(nptr)
 */
static int
luab_atoll(lua_State *L)
{
    const char *nptr;
    double n;

    (void)luab_core_checkmaxargs(L, 1);

    nptr = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    n = atoll(nptr);

    return (luab_pushxnumber(L, n, 0));
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
 * @usage k [, err, msg ] = bsd.stdlib.llabs(j)
 */
static int
luab_llabs(lua_State *L)
{
    luab_module_t *m;
    long j, k;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LONG, TYPE, __func__);
    j = (long)luab_checkxinteger(L, 1, m, luab_env_long_max);
    k = llabs(j);

    return (luab_pushxinteger(L, k));
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
 * @usage lldiv [, err, msg ] = bsd.stdlib.lldiv(num, denom)
 */
static int
luab_lldiv(lua_State *L)
{
    luab_module_t *m;
    int num, denom;
    lldiv_t data;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(INT, TYPE, __func__);
    num = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    denom = (int)luab_checkxinteger(L, 2, m, luab_env_int_max);

    data = lldiv(num, denom);

    return (luab_pushxdata(L, luab_xmod(LLDIV, TYPE, __func__), &data));
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
 * @usage n [, err, msg ] = bsd.stdlib.strtoll(nptr, base)
 */
static int
luab_strtoll(lua_State *L)
{
    luab_module_t *m;
    const char *nptr;
    int base;
    long long n;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);

    nptr = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    base = (int)luab_checkxinteger(L, 2, m, luab_env_int_max);

    n = strtoll(nptr, NULL, base);

    return (luab_pushxnumber(L, n, 0));
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
 * @usage n [, err, msg ] = bsd.stdlib.strtoull(nptr, base)
 */
static int
luab_strtoull(lua_State *L)
{
    luab_module_t *m;
    const char *nptr;
    int base;
    unsigned long long n;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);

    nptr = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    base = (int)luab_checkxinteger(L, 2, m, luab_env_int_max);

    n = strtoull(nptr, NULL, base);

    return (luab_pushxnumber(L, n, 0));
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
 * @usage rat [, err, msg ] = bsd.stdlib._Exit(status)
 */
static int
luab_Exit(lua_State *L)
{
    luab_module_t *m;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    status = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    _Exit(status);
                        /* NOTREACHED */
    return (luab_pushxinteger(L, luab_env_success));
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
    luab_module_t *m;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    status = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    quick_exit(status);

    return (luab_pushxinteger(L, luab_env_success));
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
 * @usage ret [, err, msg ] = bsd.stdlib.realpath(pathname, resolved_path)
 */
static int
luab_realpath(lua_State *L)
{
    luab_module_t *m;
    const char *pathname;
    luab_iovec_t *buf;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(IOVEC, TYPE, __func__);

    pathname = luab_checklstring(L, 1, luab_env_path_max, NULL);
    buf = luab_udata(L, 2, m, luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (luab_env_path_max <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if (realpath(pathname, bp) != NULL) {
                buf->iov.iov_len = strlen(bp);
                status = luab_env_success;
            } else
                status = luab_env_error;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = luab_env_error;
        }
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}
#endif

#if __POSIX_VISIBLE >= 199506
/***
 * rand_r(3) - bad random number generator
 *
 * @function rand_r
 *
 * @param ctx               Result argument, (LUA_TUSERDATA(UINT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.rand_r(ctx)
 */
static int
luab_rand_r(lua_State *L)
{
    luab_module_t *m;
    u_int *ctx;
    int n;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT, TYPE, __func__);
    ctx = luab_udata(L, 1, m, u_int *);

    n = rand_r(ctx);

    return (luab_pushxinteger(L, n));
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
 * @usage ret [, err, msg ] = bsd.stdlib.setenv(name, value, overwrite)
 */
static int
luab_setenv(lua_State *L)
{
    luab_module_t *m;
    const char *name;
    const char *value;
    int overwrite;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m = luab_xmod(INT, TYPE, __func__);

    name = luab_checklstring(L, 1, luab_env_name_max, NULL);
    value = luab_checklstring(L, 2, luab_env_name_max, NULL);
    overwrite = (int)luab_checkxinteger(L, 3, m, luab_env_int_max);

    status = setenv(name, value, overwrite);

    return (luab_pushxinteger(L, status));
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
 * @usage ret [, err, msg ] = bsd.stdlib.unsetenv(name)
 */
static int
luab_unsetenv(lua_State *L)
{
    const char *name;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    name = luab_checklstring(L, 1, luab_env_name_max, NULL);
    status = unsetenv(name);

    return (luab_pushxinteger(L, status));
}
#endif

#if __XSI_VISIBLE
/***
 * a64l(3) - convert between a long initeger and a base-64 ASCII string
 *
 * @function a64l
 *
 * @param s                 Base-64 string encodes long integer.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.a64l(s)
 */
static int
luab_a64l(lua_State *L)
{
    const char *s;
    long n;

    (void)luab_core_checkmaxargs(L, 1);

    s = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    n = a64l(s);

    return (luab_pushxinteger(L, n));
}

/***
 * drand48(3) - pseudo random number generators and initializiation routines
 *
 * @function drand48
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.drand48()
 */
static int
luab_drand48(lua_State *L)
{
    double n;

    (void)luab_core_checkmaxargs(L, 0);

    n = drand48();

    return (luab_pushxnumber(L, n, 0));
}

/***
 * erand48(3) - pseudo random number generators and initializiation routines
 *
 * @function erand48
 *
 * @param xseed             Array of 3 shorts
 *
 *                              { xseed0, xseed1, xseed2 }
 *
 *                          by (LUA_TTABLE(LUA_TNUMBER,LUA_TNUMBER)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.erand48(xseed)
 */
static int
luab_erand48(lua_State *L)
{
    luab_module_t *m;
    luab_table_t *tbl;
    u_short *xseed;
    double n;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(USHRT, TYPE, __func__);

    if ((tbl = luab_table_checklxdata(L, 1, m, 3)) != NULL) {
        xseed = (u_short *)(tbl->tbl_vec);
        n = erand48(xseed);
        luab_table_free(tbl);
    } else
        n = 0.0;

    return (luab_pushxnumber(L, n, 1));
}

/***
 * grantpt(3) - pseudo-terminal access functions
 *
 * @function grantpt
 *
 * @param fildes            Open file descriptor.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.grantpt(fildes)
 */
static int
luab_grantpt(lua_State *L)
{
    luab_module_t *m;
    int fildes;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    fildes = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    status = grantpt(fildes);

    return (luab_pushxinteger(L, status));
}

/***
 * initstate(3) - better random number generator; routines for changing generators
 *
 * @function initstate
 *
 * @param seed              Specifies seed.
 * @param state             State array, (LUA_TUSERDATA(IOVEC)).
 * @param n                 Specifies the size of the state array.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.initstate(seed, state, n)
 */
static int
luab_initstate(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    u_int seed;
    luab_iovec_t *buf;
    size_t n;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(UINT, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);
    m2 = luab_xmod(SIZE, TYPE, __func__);

    seed = (u_int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    buf = luab_udata(L, 2, m1, luab_iovec_t *);
    n = (size_t)luab_checkxlinteger(L, 3, m2, 0);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (n <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if (initstate(seed, bp, n) != NULL) {
                buf->iov.iov_len = n;
                status = luab_env_success;
            } else {
                errno = EINVAL;
                status = luab_env_error;
            }
            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = luab_env_error;
        }
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * jrand48(3) - pseudo random number generators and initializiation routines
 *
 * @function jrand48
 *
 * @param xseed             Array of 3 shorts
 *
 *                              { xseed0, xseed1, xseed2 }
 *
 *                          by (LUA_TTABLE(LUA_TNUMBER,LUA_TNUMBER)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.jrand48(xseed)
 */
static int
luab_jrand48(lua_State *L)
{
    luab_module_t *m;
    luab_table_t *tbl;
    u_short *xseed;
    double n;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(USHRT, TYPE, __func__);

    if ((tbl = luab_table_checklxdata(L, 1, m, 3)) != NULL) {
        xseed = (u_short *)(tbl->tbl_vec);
        n = jrand48(xseed);
        luab_table_free(tbl);
    } else
        n = 0.0;

    return (luab_pushxnumber(L, n, 1));
}

/***
 * l64a(3) - convert between a long integer and a base-64 ASCII string
 *
 * @function l64a
 *
 * @param l                 Long integer.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.l64a(s)
 */
static int
luab_l64a(lua_State *L)
{
    luab_module_t *m;
    long l;
    char *s;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LONG, TYPE, __func__);
    l = luab_checkxinteger(L, 1, m, luab_env_long_max);
    s = l64a(l);    /* value points to a static buffer, see l64a(3) */

    return (luab_pushstring(L, s));
}

/***
 * lcong48(3) - pseudo random number generators and initialization routes
 *
 * @function lcong48
 *
 * @param p                 Initializiation-vector
 *
 *                              {                   -- Initializiation-vector.
 *                                  (LUA_TNUMBER),      -- p[0]:seed
 *                                  (LUA_TNUMBER),      -- p[1]:seed
 *                                  (LUA_TNUMBER),      -- p[2]:seed
 *                                  (LUA_TNUMBER),      -- p[3]:multiplicand
 *                                  (LUA_TNUMBER),      -- p[4]:multiplicand
 *                                  (LUA_TNUMBER),      -- p[5]:multiplicand
 *                                  (LUA_TNUMBER)       -- p[6]:addend
 *                              }
 *
 *                          by (LUA_TTABLE(LUA_TNUMBER,LUA_TNUMBER)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.lcong48(p)
 */
static int
luab_lcong48(lua_State *L)
{
    luab_module_t *m;
    luab_table_t *tbl;
    u_short *p;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(USHRT, TYPE, __func__);

    if ((tbl = luab_table_checklxdata(L, 1, m, 7)) != NULL) {
        p = (u_short *)(tbl->tbl_vec);
        lcong48(p);
        luab_table_free(tbl);
    }
    return (luab_pushxinteger(L, luab_env_success));
}

/***
 * lrand48(3) - pseudo random number generators and initialization routes
 *
 * @function lrand48
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.lrand48()
 */
static int
luab_lrand48(lua_State *L)
{
    long n;

    (void)luab_core_checkmaxargs(L, 0);

    n = lrand48();

    return (luab_pushxinteger(L, n));
}

/***
 * mrand48(3) - pseudo random number generators and initialization routes
 *
 * @function mrand48
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.mrand48()
 */
static int
luab_mrand48(lua_State *L)
{
    long n;

    (void)luab_core_checkmaxargs(L, 0);

    n = mrand48();

    return (luab_pushxinteger(L, n));
}

/***
 * nrand48(3) - pseudo random number generators and initializiation routines
 *
 * @function nrand48
 *
 * @param xseed             Array of 3 shorts
 *
 *                              { xseed0, xseed1, xseed2 }
 *
 *                          by (LUA_TTABLE(LUA_TNUMBER,LUA_TNUMBER)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.nrand48(xseed)
 */
static int
luab_nrand48(lua_State *L)
{
    luab_module_t *m;
    luab_table_t *tbl;
    u_short *xseed;
    double n;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(USHRT, TYPE, __func__);

    if ((tbl = luab_table_checklxdata(L, 1, m, 3)) != NULL) {
        xseed = (u_short *)(tbl->tbl_vec);
        n = nrand48(xseed);
        luab_table_free(tbl);
    } else
        n = 0.0;

    return (luab_pushxnumber(L, n, 1));
}

/***
 * posix_openpt(3) - open a pseudo-terminal device
 *
 * @function posix_openpt
 *
 * @param oflag             The values are contsructed from
 *
 *                              bsd.fcntl.O_{RDWR,NOCTTY,CLOEXEC}
 *
 *                          by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage fd [, err, msg ] = bsd.stdlib.posix_openpt(oflag)
 */
static int
luab_posix_openpt(lua_State *L)
{
    luab_module_t *m;
    int oflag;
    int fd;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    oflag = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    fd = posix_openpt(oflag);

    return (luab_pushxinteger(L, fd));
}

/***
 * ptsname(3) - pseudo-terminal access functions
 *
 * @function ptsname
 *
 * @param fildes            Open file descriptor.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pts [, err, msg ] = bsd.stdlib.ptsname(fildes)
 */
static int
luab_ptsname(lua_State *L)
{
    luab_module_t *m;
    int fildes;
    caddr_t pts;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    fildes = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    pts = ptsname(fildes);

    return (luab_pushstring(L, pts));
}

/***
 * putenv(3) - environment variable functions
 *
 * @function putenv
 *
 * @param string            Specifies "name=value" pair, (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.putenv(string)
 */
static int
luab_putenv(lua_State *L)
{
    luab_module_t *m;
    luab_iovec_t *buf;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IOVEC, TYPE, __func__);
    buf = luab_udata(L, 1, m, luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags && IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            status = putenv(bp);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = luab_env_error;
        }
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * random(3) - better random nummer generator; routines for changing generators
 *
 * @function random
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.random()
 */
static int
luab_random(lua_State *L)
{
    long n;

    (void)luab_core_checkmaxargs(L, 0);

    n = random();

    return (luab_pushxinteger(L, n));
}

/***
 * seed48(3) - pseudo random number generators and initializiation routines
 *
 * @function seed48
 *
 * @param xseed             Array of 3 shorts
 *
 *                              { xseed0, xseed1, xseed2 }
 *
 *                          by (LUA_TTABLE(LUA_TNUMBER,LUA_TNUMBER)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.seed48(xseed)
 */
static int
luab_seed48(lua_State *L)
{
    luab_module_t *m;
    luab_table_t *tbl;
    u_short *xseed;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(USHRT, TYPE, __func__);

    if ((tbl = luab_table_checklxdata(L, 1, m, 3)) != NULL) {
        xseed = (u_short *)(tbl->tbl_vec);
        if (seed48(xseed) == NULL) {
            errno = EINVAL;
            status = luab_env_error;
        } else
            status = luab_env_success;
    } else
        status = luab_env_error;

    return (luab_pushxinteger(L, status));
}

/***
 * setstate(3) - better random number generator; routines for changing generators
 *
 * @function setstate
 *
 * @param state             State array, (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.setstate(state)
 */
static int
luab_setstate(lua_State *L)
{
    luab_module_t *m;
    luab_iovec_t *buf;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IOVEC, TYPE, __func__);
    buf = luab_udata(L, 1, m, luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if (setstate(bp) == NULL) {
                errno = EINVAL;
                status = luab_env_error;
            } else
                status = luab_env_success;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = luab_env_error;
        }
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * srand48(3) - pseudo random nuber generators and initializiation routines
 *
 * @function srand48
 *
 * @param seed              Specifies seed.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.srand48(seed)
 */
static int
luab_srand48(lua_State *L)
{
    luab_module_t *m;
    long seed;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LONG, TYPE, __func__);
    seed = (long)luab_checkxinteger(L, 1, m, luab_env_long_max);
    srand48(seed);

    return (luab_pushxinteger(L, luab_env_success));
}

/***
 * srandom(3) - better random number generator; routines for changing generators
 *
 * @function srandom
 *
 * @param seed              Specifies seed.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.srandom(seed)
 */
static int
luab_srandom(lua_State *L)
{
    luab_module_t *m;
    u_int seed;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT, TYPE, __func__);
    seed = (u_int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    srandom(seed);

    return (luab_pushxinteger(L, luab_env_success));
}

/***
 * unlockpt(3) - pseudo terminal access functions
 *
 * @function unlockpt
 *
 * @param fildes            Open filedescriptor.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.unlockpt(fildes)
 */
static int
luab_unlockpt(lua_State *L)
{
    luab_module_t *m;
    int fildes;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    fildes = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    status = unlockpt(fildes);

    return (luab_pushxinteger(L, status));
}
#endif /* __XSI_VISIBLE */
#if __BSD_VISIBLE
/***
 * abort2(2) - abort process with diagnostics
 *
 * @function abort2
 *
 * @param why               Descriptive string idicates reason.
 * @param nargs             Cardinality of args.
 * @param args              Instance of (LUA_TTABLE).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.abort2(fildes)
 */
static int
luab_abort2(lua_State *L)
{
    luab_module_t *m;
    const char *why;
    int nargs;
    luab_table_t *tbl;
    const void **args;

    (void)luab_core_checkmaxargs(L, 3);

    m = luab_xmod(INT, TYPE, __func__);

    why = luab_checklstring(L, 1, LUAB_ABORT2_MAXBUFLEN, NULL); /* XXX */
    nargs = luab_checkxinteger(L, 2, m, luab_env_int_max);

    if ((tbl = luab_table_tolxargp(L, 3, nargs)) != NULL)
        args = (const void **)(tbl->tbl_vec);
    else
        args = NULL;

    abort2(why, nargs, (void *)(intptr_t *)args);
        /* NOTREACHED */
    return (luab_pushxinteger(L, luab_env_success));
}

/***
 * arc4random(3) - random number generator
 *
 * @function arc4random
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.arc4random()
 */
static int
luab_arc4random(lua_State *L)
{
    uint32_t n;

    (void)luab_core_checkmaxargs(L, 0);

    n = arc4random();

    return (luab_pushxinteger(L, n));
}

/***
 * arc4random_buf(3) - random number generator
 *
 * @function arc4random_buf
 *
 * @param buf               Storage for random data, (LUA_TUSERDATA(IOVEC)).
 * @param nbytes            Length in bytes of by random data filled region.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.arc4random_buf(buf, nbytes)
 */
static int
luab_arc4random_buf(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *buf;
    size_t nbytes;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    nbytes = (size_t)luab_checkxlinteger(L, 2, m1, 0);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (nbytes <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            arc4random_buf(bp, nbytes);
            status = luab_env_success;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = luab_env_error;
        }
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * arc4random_uniform(3) - random number generator
 *
 * @function arc4random_uniform
 *
 * @param upper_bound       Specifies constraint for random integer.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage n [, err, msg ] = bsd.stdlib.arc4random_uniform(upper_bound)
 */
static int
luab_arc4random_uniform(lua_State *L)
{
    luab_module_t *m;
    uint32_t ub, n;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT32, TYPE, __func__);
    ub = luab_checkinteger(L, 1, luab_env_int_max);
    n = arc4random_uniform(ub);

    return (luab_pushxinteger(L, n));
}

/***
 * getbsize(3) - get preffered block size
 *
 * @function getbsize
 *
 * @param headerlenp        Specifies length of bytes by (LUA_TUSERDATA(INT))
 *                          filled in region referred by result argument.
 * @param blocksizep        Result argument, (LUA_TUSERDATA(LONG)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage str [, err, msg ] = bsd.stdlib.getbsize(headerlenp, blocksizep)
 */
static int
luab_getbsize(lua_State *L)
{
    luab_module_t *m0, *m1;
    int *headerlenp;
    long *blocksizep;
    const char *str;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(LONG, TYPE, __func__);

    headerlenp = luab_udata(L, 1, m0, int *);
    blocksizep = luab_udata(L, 2, m1, long *);

    str = getbsize(headerlenp, blocksizep);

    return (luab_pushstring(L, str));
}

/***
 * getcap(3) - capability database access routines
 *
 * @function getcap
 *
 * @param buf               Capability record buffer, (LUA_TUSERDATA(CAP_RBUF)).
 * @param cap               Capability string, (LUA_TSTRING).
 * @param type              Specifies capability type by a single character.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage str [, err, msg ] = bsd.stdlib.getcap(buf, cap, type)
 */
static int
luab_cgetcap(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct iovec *buf;
    const char *cap;
    int type;
    caddr_t bp;
    char *str;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(CAP_RBUF, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    buf = luab_udata(L, 1, m0, struct iovec *);
    cap = luab_checklstring(L, 2, luab_env_buf_max, NULL);
    type = (int)luab_checkxinteger(L, 3, m1, luab_env_int_max);

    if ((bp = buf->iov_base) != NULL)
        str = cgetcap(bp, cap, type);
    else {
        errno = ERANGE;
        str = NULL;
    }
    return (luab_pushstring(L, str));
}

/***
 * cgetclose(3) - capability database access routines
 *
 * @function cgetclose
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.cgetclose()
 */
static int
luab_cgetclose(lua_State *L)
{
    int status;

    (void)luab_core_checkmaxargs(L, 0);

    status = cgetclose();

    return (luab_pushxinteger(L, status));
}

/***
 * cgetent(3) - capability database access routines
 *
 * @function cgetent
 *
 * @param buf               Capability record buffer, (LUA_TUSERDATA(CAP_RBUF)).
 * @param db_array          File array,
 *
 *                              { file0, "/path/to/file", ... , fileN },
 *
 *                          as instance of
 *
 *                              (LUA_TTABLE(LUA_TNUMBER,LUA_TSTRING)).
 *
 * @param name              Capability string, (LUA_TSTRING).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.cgetent(buf, db_array, name)
 */
static int
luab_cgetent(lua_State *L)
{
    luab_module_t *m;
    struct iovec *buf;
    char **db_array;
    const char *name;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m = luab_xmod(CAP_RBUF, TYPE, __func__);

    buf = luab_udata(L, 1, m, struct iovec *);
    db_array = (void *)(intptr_t *)luab_table_checkargv(L, 2);
    name = luab_checklstring(L, 3, luab_env_buf_max, NULL);

    if ((bp = buf->iov_base) == NULL) {
        if ((status = cgetent(&bp, db_array, name)) == 0)
            buf->iov_len = strnlen(bp, luab_env_buf_max);
    } else {
        errno = EBUSY;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * cgetfirst(3) - capability database access routines
 *
 * @function cgetfirst
 *
 * @param buf               Capability record buffer, (LUA_TUSERDATA(CAP_RBUF)).
 * @param db_array          File array,
 *
 *                              { file0, "/path/to/file", ... , fileN },
 *
 *                          as instance of
 *
 *                              (LUA_TTABLE(LUA_TNUMBER,LUA_TSTRING)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.cgetfirst(buf, db_array)
 */
static int
luab_cgetfirst(lua_State *L)
{
    luab_module_t *m;
    struct iovec *buf;
    char **db_array;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(CAP_RBUF, TYPE, __func__);

    buf = luab_udata(L, 1, m, struct iovec *);
    db_array = (void *)(intptr_t *)luab_table_checkargv(L, 2);

    if ((bp = buf->iov_base) != NULL) {
        if ((status = cgetfirst(&bp, db_array)) == 0)
            buf->iov_len = strnlen(bp, luab_env_buf_max);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * cgetmatch(3) - capability database access routines
 *
 * @function cgetmatch
 *
 * @param buf               Capability record buffer, (LUA_TUSERDATA(CAP_RBUF)).
 * @param name              Capability string, (LUA_TSTRING).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.cgetmatch(buf, name)
 */
static int
luab_cgetmatch(lua_State *L)
{
    luab_module_t *m;
    struct iovec *buf;
    const char *name;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(CAP_RBUF, TYPE, __func__);

    buf = luab_udata(L, 1, m, struct iovec *);
    name = luab_checklstring(L, 2, luab_env_buf_max, NULL);

    if ((bp = buf->iov_base) != NULL)
        status = cgetmatch(bp, name);
    else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * cgetnext(3) - capability database access routines
 *
 * @function cgetnext
 *
 * @param buf               Capability record buffer, (LUA_TUSERDATA(CAP_RBUF)).
 * @param db_array          File array,
 *
 *                              { file0, "/path/to/file", ... , fileN },
 *
 *                          as instance of
 *
 *                              (LUA_TTABLE(LUA_TNUMBER,LUA_TSTRING)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.cgetnext(buf, db_array)
 */
static int
luab_cgetnext(lua_State *L)
{
    luab_module_t *m;
    struct iovec *buf;
    char **db_array;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(CAP_RBUF, TYPE, __func__);

    buf = luab_udata(L, 1, m, struct iovec *);
    db_array = (void *)(intptr_t *)luab_table_checkargv(L, 2);

    if ((bp = buf->iov_base) != NULL)
        status = cgetnext(&bp, db_array);
    else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * cgetnum(3) - capability database access routines
 *
 * @function cgetnum
 *
 * @param buf               Capability record buffer, (LUA_TUSERDATA(CAP_RBUF)).
 * @param cap               Capability string, (LUA_TSTRING).
 * @param num               Numeric capability, (LUA_TUSERDATA(LONG)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.cgetnum(buf, db_array)
 */
static int
luab_cgetnum(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct iovec *buf;
    const char *cap;
    long *num;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(CAP_RBUF, TYPE, __func__);
    m1 = luab_xmod(LONG, TYPE, __func__);

    buf = luab_udata(L, 1, m0, struct iovec *);
    cap = luab_checklstring(L, 2, luab_env_buf_max, NULL);
    num = luab_udata(L, 3, m1, long *);

    if ((bp = buf->iov_base) != NULL)
        status = cgetnum(bp, cap, num);
    else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * cgetset(3) - capability database access routines
 *
 * @function cgetset
 *
 * @param ent               Capability record entry, (LUA_TSTRING).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.cgetset(ent)
 */
static int
luab_cgetset(lua_State *L)
{
    const char *ent;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    ent = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    status = cgetset(ent);

    return (luab_pushxinteger(L, status));
}

/***
 * cgetstr(3) - capability database access routines
 *
 * @function cgetstr
 *
 * @param buf               Capability record buffer, (LUA_TUSERDATA(CAP_RBUF)).
 * @param cap               Capability string, (LUA_TSTRING).
 * @param str               Result argument, (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.cgetstr(buf, cap, str)
 */
static int
luab_cgetstr(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct iovec *buf;
    const char *cap;
    luab_iovec_t *res;
    caddr_t bp, dp;
    ssize_t len;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(CAP_RBUF, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);

    buf = luab_udata(L, 1, m0, struct iovec *);
    cap = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    res = luab_udata(L, 3, m1, luab_iovec_t *);

    if ((bp = buf->iov_base) != NULL) {
        if ((len = cgetstr(bp, cap, &dp)) < 0)
            dp = NULL;
    } else {
        dp = NULL;
        len = luab_env_error;
    }
    status = luab_iovec_copyin(res, dp, len);
    return (luab_pushxinteger(L, status));
}

/***
 * cgetustr(3) - capability database access routines
 *
 * @function cgetustr
 *
 * @param buf               Capability record buffer, (LUA_TUSERDATA(CAP_RBUF)).
 * @param cap               Capability string, (LUA_TSTRING).
 * @param str               Result argument, (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.cgetustr(buf, cap, str)
 */
static int
luab_cgetustr(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct iovec *buf;
    const char *cap;
    luab_iovec_t *res;
    caddr_t bp, dp;
    ssize_t len;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(CAP_RBUF, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);

    buf = luab_udata(L, 1, m0, struct iovec *);
    cap = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    res = luab_udata(L, 3, m1, luab_iovec_t *);

    if ((bp = buf->iov_base) != NULL) {
        if ((len = cgetustr(bp, cap, &dp)) < 0)
            dp = NULL;
    } else {
        dp = NULL;
        len = luab_env_error;
    }
    status = luab_iovec_copyin(res, dp, len);
    return (luab_pushxinteger(L, status));
}

/***
 * daemon(3) - run in the background
 *
 * @function daemon
 *
 * @param nochdir           Directory may changed to "/", if value is non-zero.
 * @param noclose           Dataflow of channels std{in,out,err} is redirected
 *                          to "/dev/null" and those are not closed, if value
 *                          is non-zero.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.daemon(nochdir, noclose)
 */
static int
luab_daemon(lua_State *L)
{
    luab_module_t *m;
    int nochdir;
    int noclose;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(INT, TYPE, __func__);

    nochdir = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    noclose = (int)luab_checkxinteger(L, 2, m, luab_env_int_max);

    status = daemon(nochdir, noclose);

    return (luab_pushxinteger(L, status));
}

/***
 * daemonfd(3) - run in the background
 *
 * @function daemonfd
 *
 * @param chdirfd           If value set to -1, current directory is not changed.
 * @param nullfd            If value set to -1, redirection of std{in,out,err}
 *                          to "/dev/zero".
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.daemonfd(chdirfd, nullfd)
 */
static int
luab_daemonfd(lua_State *L)
{
    luab_module_t *m;
    int chdirfd;
    int nullfd;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(INT, TYPE, __func__);

    chdirfd = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    nullfd = (int)luab_checkxinteger(L, 2, m, luab_env_int_max);

    status = daemon(chdirfd, nullfd);

    return (luab_pushxinteger(L, status));
}

/***
 * devname(3) - get device name
 *
 * @function devname
 *
 * @param dev               Specifies device number over set of "/dev".
 * @param type              Specifies file type by unique values from:
 *
 *                              bsd.sys.stat.S_{IFBLK,S_IFCHR}.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage name [, err, msg ] = bsd.stdlib.devname(dev, type)
 */
static int
luab_devname(lua_State *L)
{
    luab_module_t *m0, *m1;
    dev_t dev;
    mode_t type;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(DEV, TYPE, __func__);
    m1 = luab_xmod(MODE, TYPE, __func__);

    dev = (dev_t)luab_checkxinteger(L, 1, m0, luab_env_long_max);
    type = (mode_t)luab_checkxinteger(L, 2, m1, luab_env_shrt_max);

    dp = devname(dev, type);

    return (luab_pushstring(L, dp));
}

/***
 * devname_r(3) - get device name
 *
 * @function devname_r
 *
 * @param dev               Specifies device number over set of "/dev".
 * @param type              Specifies file type by unique values from:
 *
 *                              bsd.sys.stat.S_{IFBLK,S_IFCHR}.
 *
 * @param buf               Result argument holds a copy of requested
 *                          device node name, (LUA_TUSERDATA(IOVEC)).
 * @param len               Specifies length of requested node name.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage name [, err, msg ] = bsd.stdlib.devname_r(dev, type, buf, len)
 */
static int
luab_devname_r(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    dev_t dev;
    mode_t type;
    luab_iovec_t *buf;
    int len;
    caddr_t bp;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(DEV, TYPE, __func__);
    m1 = luab_xmod(MODE, TYPE, __func__);
    m2 = luab_xmod(IOVEC, TYPE, __func__);

    dev = (dev_t)luab_checkxinteger(L, 1, m0, luab_env_long_max);
    type = (mode_t)luab_checkxinteger(L, 2, m1, luab_env_shrt_max);
    buf = luab_udata(L, 3, m2, luab_iovec_t *);
    len = (int)luab_checkinteger(L, 4, luab_env_int_max);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (luab_env_path_max <= buf->iov_max_len) &&
        ((size_t)len <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {

            if ((dp = devname_r(dev, type, bp, len)) != NULL)
                buf->iov.iov_len = strnlen(bp, len);

        } else {
            errno = EBUSY;
            dp = NULL;
        }
    } else {
        errno = ERANGE;
        dp = NULL;
    }
    return (luab_pushstring(L, dp));
}

/***
 * fdevname(3) - get device name
 *
 * @function fdevname
 *
 * @param fd                Open fildescriptor, character device, (LUA_TNUMBER).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage name [, err, msg ] = bsd.stdlib.fdevname(fd)
 */
static int
luab_fdevname(lua_State *L)
{
    luab_module_t *m;
    int fd;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    dp = fdevname(fd);

    return (luab_pushstring(L, dp));
}

/***
 * fdevname_r(3) - get device name
 *
 * @function fdevname_r
 *
 * @param fd                Open fildescriptor, character device, (LUA_TNUMBER).
 * @param buf               Result argument holds a copy of requested
 *                          device node name, (LUA_TUSERDATA(IOVEC)).
 * @param len               Specifies length of requested node name.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage name [, err, msg ] = bsd.stdlib.fdevname_r(fd, buf, len)
 */
static int
luab_fdevname_r(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd;
    luab_iovec_t *buf;
    int len;
    caddr_t bp;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    buf = luab_udata(L, 2, m1, luab_iovec_t *);
    len = (int)luab_checkxinteger(L, 3, m0, luab_env_int_max);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (luab_env_path_max <= buf->iov_max_len) &&
        ((size_t)len <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {

            if ((dp = fdevname_r(fd, bp, len)) != NULL)
                buf->iov.iov_len = strnlen(bp, len);

        } else {
            errno = EBUSY;
            dp = NULL;
        }
    } else {
        errno = ERANGE;
        dp = NULL;
    }
    return (luab_pushstring(L, dp));
}

/***
 * getloadavg(3) - get system load averages
 *
 * @function getloadavg
 *
 * @param loadavg           Set of samples, (LUA_TSTRING).
 *
 *                              { loadavg0, loadavg1, ... , loadavgN }
 *
 * @param nelem             Specifies number of requested samples.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.getloadavg(loadavg, nelem)
 */
static int
luab_getloadavg(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_table_t *tbl;
    double *loadavg;
    int nelem;
    size_t nmax;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(DOUBLE, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    if ((tbl = luab_table_checkxdata(L, 1, m0)) != NULL)
        loadavg = (double *)(tbl->tbl_vec);
    else
        loadavg = NULL;

    nelem = (int)luab_checkxinteger(L, 2, m1, luab_env_int_max);

    if (tbl != NULL) {
        nmax = tbl->tbl_card;

        if (nmax == (size_t)nelem) {
            if ((status = getloadavg(loadavg, nelem)) > 0)
                luab_table_pushxdata(L, 1, m0, tbl, 0, 1);
        } else {
            errno = ERANGE;
            status = luab_env_error;
        }
    } else {
        errno = EINVAL;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * getprogname(3) - get or set the program name
 *
 * @function getprogname
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage name [, err, msg ] = bsd.stdlib.getprogname()
 */
static int
luab_getprogname(lua_State *L)
{
    const char *progname;

    (void)luab_core_checkmaxargs(L, 0);

    progname = getprogname();

    return (luab_pushstring(L, progname));
}

/***
 * l64a_r(3) - convert between a long integer and a base-64 ASCII string
 *
 * @function l64a_r
 *
 * @param l                 Long integer.
 * @param buffer            Result argument, (LUA_TUSERDATA(IOVEC)).
 * @param buflen            Buffer size.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.l64a_r(l, buffer, buflen)
 */
static int
luab_l64a_r(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    long l;
    luab_iovec_t *buf;
    int buflen;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(LONG, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);
    m2 = luab_xmod(INT, TYPE, __func__);

    l = (long)luab_checkxinteger(L, 1, m0, luab_env_long_max);
    buf = luab_udata(L, 2, m1, luab_iovec_t *);
    buflen = (int)luab_checkxinteger(L, 3, m2, luab_env_int_max);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        ((size_t)buflen <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = l64a_r(l, bp, buflen)) == 0)
                buf->iov.iov_len = buflen;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = luab_env_error;
        }
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * mkostemp(3) - make tempory file name (unique)
 *
 * @function mkostemp
 *
 * @param template          File name template, (LUA_TUSERDATA(IOVEC)).
 * @param oflags            Permitted open(2) flags:
 *
 *                              bsd.fcntl.O_{
 *                                  APPEND,
 *                                  DIRECT,
 *                                  SHLOCK,
 *                                  EXLOCK,
 *                                  SYNC,
 *                                  CLOEXEC
 *                              }.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.mkostemp(template, oflags)
 */
static int
luab_mkostemp(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *buf;
    int oflags;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    oflags = (int)luab_checkxinteger(L, 2, m1, luab_env_int_max);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (buf->iov_max_len <= luab_env_path_max) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            status = mkostemp(bp, oflags);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = luab_env_error;
        }
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * mkostemps(3) - make tempory file name (unique)
 *
 * @function mkostemps
 *
 * @param template          File name template, (LUA_TUSERDATA(IOVEC)).
 * @param suffixlen         Specifies the length of the suffix string.
 * @param oflags            Permitted open(2) flags:
 *
 *                              bsd.fcntl.O_{
 *                                  APPEND,
 *                                  DIRECT,
 *                                  SHLOCK,
 *                                  EXLOCK,
 *                                  SYNC,
 *                                  CLOEXEC
 *                              }.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.mkostemps(template, suffixlen, oflags)
 */
static int
luab_mkostemps(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *buf;
    int suffixlen;
    int oflags;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    suffixlen = (int)luab_checkxinteger(L, 2, m1, luab_env_int_max);
    oflags = (int)luab_checkxinteger(L, 3, m1, luab_env_int_max);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (buf->iov_max_len <= luab_env_path_max) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            status = mkostemps(bp, suffixlen, oflags);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = luab_env_error;
        }
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * setprogname(3) - get or set the program name
 *
 * @function setprogname
 *
 * @param progname          Specifies name of current process.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.setprogname(progname)
 */
static int
luab_setprogname(lua_State *L)
{
    const char *progname;

    (void)luab_core_checkmaxargs(L, 1);

    progname = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    setprogname(progname);

    return (luab_pushxinteger(L, luab_env_success));
}

/***
 * sranddev(3) - bad random number generator
 *
 * @function sranddev
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.sranddev()
 */
static int
luab_sranddev(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);

    sranddev();

    return (luab_pushxinteger(L, luab_env_success));
}

/***
 * srandomdev(3) - better random number generator; routines for changing generators
 *
 * @function srandomdev
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.srandomdev()
 */
static int
luab_srandomdev(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);

    srandomdev();

    return (luab_pushxinteger(L, luab_env_success));
}

/***
 * strtonum(3) - reliably convert string value to an integer
 *
 * @function strtonum
 *
 * @param nptr              Specifies string value, (LUA_TSTRING).
 * @param minval            Specifies lower bound, (LUA_TNUMBER).
 * @param maxval            Specifies upper bound, (LUA_TNUMBER).
 * @param errstr            Result argument, reason, (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdlib.strtonum(nptr, minval, maxval, errstr)
 */
static int
luab_strtonum(lua_State *L)
{
    luab_module_t *m;
    const char *nptr;
    long long minval;
    long long maxval;
    luab_iovec_t *buf;
    const char *dp;
    long long n;

    (void)luab_core_checkmaxargs(L, 4);

    /*
     * XXX
     *  We should implement <luab_llong_type>.
     */

    m = luab_xmod(IOVEC, TYPE, __func__);

    nptr = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    minval = (long long)luab_checkinteger(L, 2, luab_env_llong_max);
    maxval = (long long)luab_checkinteger(L, 3, luab_env_llong_max);
    buf = luab_udata(L, 4, m, luab_iovec_t *);

    if ((n = strtonum(nptr, minval, maxval, &dp)) == 0)
        (void)luab_iovec_copyin(buf, dp, strlen(dp));

    return (luab_pushxinteger(L, n));
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
 * @usage div [, err, msg ] = bsd.stdlib.div_create([ data ])
 */
static int
luab_div_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(DIV, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
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
 * @usage ldiv [, err, msg ] = bsd.stdlib.ldiv_create([ data ])
 */
static int
luab_ldiv_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LINGER, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
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
 * @usage lldiv [, err, msg ] = bsd.stdlib.lldiv_create([ data ])
 */
static int
luab_lldiv_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LLDIV, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}
#endif /* __LONG_LONG_SUPPORTED */
#endif /* __ISO_C_VISIBLE >= 1999 */

/*
 * Interface against <stdlib.h>.
 */

static luab_module_table_t luab_stdlib_vec[] = {
    LUAB_INT("EXIT_FAILURE",          EXIT_FAILURE),
    LUAB_INT("EXIT_SUCCESS",          EXIT_SUCCESS),
    LUAB_INT("RAND_MAX",              RAND_MAX),
    LUAB_FUNC("abort",                luab_abort),
    LUAB_FUNC("abs",                  luab_abs),
    LUAB_FUNC("atof",                 luab_atof),
    LUAB_FUNC("atoi",                 luab_atoi),
    LUAB_FUNC("atol",                 luab_atol),
    LUAB_FUNC("div",                  luab_div),
    LUAB_FUNC("exit",                 luab_exit),
    LUAB_FUNC("getenv",               luab_getenv),
    LUAB_FUNC("labs",                 luab_labs),
    LUAB_FUNC("ldiv",                 luab_ldiv),
    LUAB_FUNC("mblen",                luab_mblen),
    LUAB_FUNC("mbstowcs",             luab_mbstowcs),
    LUAB_FUNC("mbtowc",               luab_mbtowc),
    LUAB_FUNC("rand",                 luab_rand),
    LUAB_FUNC("srand",                luab_srand),
    LUAB_FUNC("strtod",               luab_strtod),
    LUAB_FUNC("strtof",               luab_strtof),
    LUAB_FUNC("strtold",              luab_strtold),
    LUAB_FUNC("strtoul",              luab_strtoul),
    LUAB_FUNC("system",               luab_system),
    LUAB_FUNC("wctomb",               luab_wctomb),
    LUAB_FUNC("wcstombs",             luab_wcstombs),
#if __ISO_C_VISIBLE >= 1999 || defined(__cplusplus)
#ifdef __LONG_LONG_SUPPORTED
    LUAB_FUNC("atoll",                luab_atoll),
    LUAB_FUNC("llabs",                luab_llabs),
    LUAB_FUNC("lldiv",                luab_lldiv),
    LUAB_FUNC("strtoll",              luab_strtoll),
    LUAB_FUNC("strtoull",             luab_strtoull),
#endif /* __LONG_LONG_SUPPORTED */
    LUAB_FUNC("_Exit",                luab_Exit),
#endif /* __ISO_C_VISIBLE >= 1999 */
#if __ISO_C_VISIBLE >= 2011 || __cplusplus >= 201103L
    LUAB_FUNC("quick_exit",           luab_quick_exit),
#endif /* __ISO_C_VISIBLE >= 2011 */
#if __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE
    LUAB_FUNC("realpath",             luab_realpath),
#endif
#if __POSIX_VISIBLE >= 199506
    LUAB_FUNC("rand_r",               luab_rand_r),
#endif
#if __POSIX_VISIBLE >= 200112
    LUAB_FUNC("setenv",               luab_setenv),
    LUAB_FUNC("unsetenv",             luab_unsetenv),
#endif
#if __XSI_VISIBLE
    LUAB_FUNC("a64l",                 luab_a64l),
    LUAB_FUNC("drand48",              luab_drand48),
    LUAB_FUNC("erand48",              luab_erand48),
    LUAB_FUNC("grantpt",              luab_grantpt),
    LUAB_FUNC("initstate",            luab_initstate),
    LUAB_FUNC("jrand48",              luab_jrand48),
    LUAB_FUNC("l64a",                 luab_l64a),
    LUAB_FUNC("lcong48",              luab_lcong48),
    LUAB_FUNC("lrand48",              luab_lrand48),
    LUAB_FUNC("mrand48",              luab_mrand48),
    LUAB_FUNC("nrand48",              luab_nrand48),
    LUAB_FUNC("posix_openpt",         luab_posix_openpt),
    LUAB_FUNC("ptsname",              luab_ptsname),
    LUAB_FUNC("putenv",               luab_putenv),
    LUAB_FUNC("random",               luab_random),
    LUAB_FUNC("seed48",               luab_seed48),
    LUAB_FUNC("setstate",             luab_setstate),
    LUAB_FUNC("srand48",              luab_srand48),
    LUAB_FUNC("srandom",              luab_srandom),
    LUAB_FUNC("unlockpt",             luab_unlockpt),
#endif /* __XSI_VISIBLE */
#if __BSD_VISIBLE
    LUAB_FUNC("abort2",               luab_abort2),
    LUAB_FUNC("arc4random",           luab_arc4random),
    LUAB_FUNC("arc4random_buf",       luab_arc4random_buf),
    LUAB_FUNC("arc4random_uniform",   luab_arc4random_uniform),
    LUAB_FUNC("getbsize",             luab_getbsize),
    LUAB_FUNC("cgetcap",              luab_cgetcap),
    LUAB_FUNC("cgetclose",            luab_cgetclose),
    LUAB_FUNC("cgetent",              luab_cgetent),
    LUAB_FUNC("cgetfirst",            luab_cgetfirst),
    LUAB_FUNC("cgetmatch",            luab_cgetmatch),
    LUAB_FUNC("cgetnext",             luab_cgetnext),
    LUAB_FUNC("cgetnum",              luab_cgetnum),
    LUAB_FUNC("cgetset",              luab_cgetset),
    LUAB_FUNC("cgetstr",              luab_cgetstr),
    LUAB_FUNC("cgetustr",             luab_cgetustr),
    LUAB_FUNC("daemon",               luab_daemon),
    LUAB_FUNC("daemonfd",             luab_daemonfd),
    LUAB_FUNC("devname",              luab_devname),
    LUAB_FUNC("devname_r",            luab_devname_r),
    LUAB_FUNC("fdevname",             luab_fdevname),
    LUAB_FUNC("fdevname_r",           luab_fdevname_r),
    LUAB_FUNC("getloadavg",           luab_getloadavg),
    LUAB_FUNC("getprogname",          luab_getprogname),
    LUAB_FUNC("l64a_r",               luab_l64a_r),
    LUAB_FUNC("mkostemp",             luab_mkostemp),
    LUAB_FUNC("mkostemps",            luab_mkostemps),
    LUAB_FUNC("setprogname",          luab_setprogname),
    LUAB_FUNC("sranddev",             luab_sranddev),
    LUAB_FUNC("srandomdev",           luab_srandomdev),
    LUAB_FUNC("strtonum",             luab_strtonum),
#endif
    LUAB_FUNC("div_create",           luab_div_create),
    LUAB_FUNC("ldiv_create",          luab_ldiv_create),
#if __ISO_C_VISIBLE >= 1999 || defined(__cplusplus)
#ifdef __LONG_LONG_SUPPORTED
    LUAB_FUNC("lldiv_create",         luab_lldiv_create),
#endif /* __LONG_LONG_SUPPORTED */
#endif /* __ISO_C_VISIBLE >= 1999 */
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_stdlib_lib = {
    .m_id       = LUAB_STDLIB_LIB_ID,
    .m_name     = LUAB_STDLIB_LIB_KEY,
    .m_vec      = luab_stdlib_vec,
};
