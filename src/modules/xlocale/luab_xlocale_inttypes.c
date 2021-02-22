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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_XLOCALE_INTTYPES_LIB_ID    1608593662
#define LUAB_XLOCALE_INTTYPES_LIB_KEY    "inttypes"

extern luab_module_t luab_xlocale_inttypes_lib;

/*
 * Service primitives.
 */

/***
 * strtoimax_l(3) - convert a string to an intmax_t integer
 *
 * @function strtoimax_l
 *
 * @param nptr              String buffer, by an instance
 *                          of (LUA_TUSERDATA(IOVEC)).
 * @param endptr            Instance of (LUA_TNIL).
 * @param base              Specifies numerical base.
 * @param locale            Specifies current locale by an
 *                          instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.xlocale.inttypes.strtoimax_l(nptr, endptr, base, locale)
 */
static int
luab_strtoimax_l(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    luab_iovec_t *nptr;
    char **endptr;
    int base;
    luab_locale_t *xloc;
    locale_t locale;
    caddr_t bp;
    intmax_t status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);
    m2 = luab_xmod(LOCALE, TYPE, __func__);

    nptr = luab_udata(L, 1, m0, luab_iovec_t *);
    endptr = (char **)luab_checknil(L, 2);
    base = (int)luab_checkxinteger(L, 3, m1, luab_env_int_max);
    xloc = luab_udata(L, 4, m2, luab_locale_t *);

    if (((locale = xloc->ud_sdu) != NULL) &&
        ((bp = nptr->iov.iov_base) != NULL) &&
        (nptr->iov.iov_len <= nptr->iov_max_len) &&
        (nptr->iov_max_len <= luab_env_buf_max) &&
        ((nptr->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);
        status = strtoimax_l(bp, endptr, base, locale);
        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * strtoumax_l(3) - convert a string to an uintmax_t integer
 *
 * @function strtoumax_l
 *
 * @param nptr              String buffer, by an instance
 *                          of (LUA_TUSERDATA(IOVEC)).
 * @param endptr            Instance of (LUA_TNIL).
 * @param base              Specifies numerical base.
 * @param locale            Specifies current locale by an
 *                          instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.xlocale.inttypes.strtoumax_l(nptr, endptr, base, locale)
 */
static int
luab_strtoumax_l(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    luab_iovec_t *nptr;
    char **endptr;
    int base;
    luab_locale_t *xloc;
    locale_t locale;
    caddr_t bp;
    uintmax_t status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);
    m2 = luab_xmod(LOCALE, TYPE, __func__);

    nptr = luab_udata(L, 1, m0, luab_iovec_t *);
    endptr = (char **)luab_checknil(L, 2);
    base = (int)luab_checkxinteger(L, 3, m1, luab_env_int_max);
    xloc = luab_udata(L, 4, m2, luab_locale_t *);

    if (((locale = xloc->ud_sdu) != NULL) &&
        ((bp = nptr->iov.iov_base) != NULL) &&
        (nptr->iov.iov_len <= nptr->iov_max_len) &&
        (nptr->iov_max_len <= luab_env_buf_max) &&
        ((nptr->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);
        status = strtoumax_l(bp, endptr, base, locale);
        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * wcstoimax_l(3) - convert a wide character string to an intmax_t integer
 *
 * @function wcstoimax_l
 *
 * @param nptr              String buffer, by an instance
 *                          of (LUA_TUSERDATA(IOVEC)).
 * @param endptr            Instance of (LUA_TNIL).
 * @param base              Specifies numerical base.
 * @param locale            Specifies current locale by an
 *                          instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.xlocale.inttypes.wcstoimax_l(nptr, endptr, base, locale)
 */
static int
luab_wcstoimax_l(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    luab_iovec_t *nptr;
    wchar_t **endptr;
    int base;
    luab_locale_t *xloc;
    locale_t locale;
    wchar_t *bp;
    intmax_t status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);
    m2 = luab_xmod(LOCALE, TYPE, __func__);

    nptr = luab_udata(L, 1, m0, luab_iovec_t *);
    endptr = (wchar_t **)luab_checknil(L, 2);
    base = (int)luab_checkxinteger(L, 3, m1, luab_env_int_max);
    xloc = luab_udata(L, 4, m2, luab_locale_t *);

    if (((locale = xloc->ud_sdu) != NULL) &&
        ((bp = (wchar_t *)nptr->iov.iov_base) != NULL) &&
        (nptr->iov.iov_len <= nptr->iov_max_len) &&
        (nptr->iov_max_len <= luab_env_buf_max) &&
        ((nptr->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);
        status = wcstoimax_l(bp, endptr, base, locale);
        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * wcstoumax_l(3) - convert a wide character string to an uintmax_t integer
 *
 * @function wcstoumax_l
 *
 * @param nptr              String buffer, by an instance
 *                          of (LUA_TUSERDATA(IOVEC)).
 * @param endptr            Instance of (LUA_TNIL).
 * @param base              Specifies numerical base.
 * @param locale            Specifies current locale by an
 *                          instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.xlocale.inttypes.wcstoumax_l(nptr, endptr, base, locale)
 */
static int
luab_wcstoumax_l(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    luab_iovec_t *nptr;
    wchar_t **endptr;
    int base;
    luab_locale_t *xloc;
    locale_t locale;
    wchar_t *bp;
    intmax_t status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);
    m2 = luab_xmod(LOCALE, TYPE, __func__);

    nptr = luab_udata(L, 1, m0, luab_iovec_t *);
    endptr = (wchar_t **)luab_checknil(L, 2);
    base = (int)luab_checkxinteger(L, 3, m1, luab_env_int_max);
    xloc = luab_udata(L, 4, m2, luab_locale_t *);

    if (((locale = xloc->ud_sdu) != NULL) &&
        ((bp = (wchar_t *)nptr->iov.iov_base) != NULL) &&
        (nptr->iov.iov_len <= nptr->iov_max_len) &&
        (nptr->iov_max_len <= luab_env_buf_max) &&
        ((nptr->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);
        status = wcstoumax_l(bp, endptr, base, locale);
        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/*
 * Interface against <xlocale/_inttypes.h>.
 */

static luab_module_table_t luab_xlocale_inttypes_vec[] = {
    LUAB_FUNC("strtoimax_l",            luab_strtoimax_l),
    LUAB_FUNC("strtoumax_l",            luab_strtoumax_l),
    LUAB_FUNC("wcstoimax_l",            luab_wcstoimax_l),
    LUAB_FUNC("wcstoumax_l",            luab_wcstoumax_l),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_xlocale_inttypes_lib = {
    .m_id       = LUAB_XLOCALE_INTTYPES_LIB_ID,
    .m_name     = LUAB_XLOCALE_INTTYPES_LIB_KEY,
    .m_vec      = luab_xlocale_inttypes_vec,
};
