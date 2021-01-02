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

#include <time.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_XLOCALE_TIME_LIB_ID    1608192127
#define LUAB_XLOCALE_TIME_LIB_KEY    "time"

extern luab_module_t luab_xlocale_time_lib;

/*
 * Service primitives.
 */

/***
 * strftime_l(3) - format data and time
 *
 * @function strftime_l
 *
 * @param buf               Buffer for formatted time information by
 *                          an instance of (LUA_TUSERDATA(IOVEC)).
 * @param maxsize           Specifies constraint for buffer.
 * @param format            Specifies format string for conversion.
 * @param timeptr           Specifies broken down time by an
 *                          instance of (LUA_TUSERDATA(TM)).
 * @param loc               Specifies current locale by an
 *                          instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.xlocale.time.strftime_l(buf, maxsize, format, timeptr, loc)
 */
static int
luab_strftime_l(lua_State *L)
{
    luab_module_t *m0, *m1, *m2, *m3;
    luab_iovec_t *buf;
    size_t maxsize;
    const char *format;
    struct tm *timeptr;
    luab_locale_t *xloc;
    locale_t loc;
    caddr_t bp;
    ssize_t status;

    (void)luab_core_checkmaxargs(L, 5);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);
    m2 = luab_xmod(TM, TYPE, __func__);
    m3 = luab_xmod(LOCALE, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    maxsize = luab_checkxlinteger(L, 2, m1, 0);
    format = luab_checklstring(L, 3, maxsize, NULL);
    timeptr = luab_udata(L, 4, m2, struct tm *);
    xloc = luab_udata(L, 5, m3, luab_locale_t *);

    if (((loc = xloc->ud_sdu) != NULL) &&
        ((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (maxsize <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = strftime_l(bp, maxsize, format, timeptr, loc)) > 0)
                buf->iov.iov_len = status;
            else
                buf->iov.iov_len = maxsize;

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
 * strptime_l(3) - format data and time
 *
 * @function strptime_l
 *
 * @param buf               Buffer for formatted time information by
 *                          an instance of (LUA_TUSERDATA(IOVEC)).
 * @param format            Specifies format string for conversion.
 * @param timeptr           Specifies broken down time by an
 *                          instance of (LUA_TUSERDATA(TM)).
 * @param loc               Specifies current locale by an
 *                          instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.xlocale.time.strptime_l(buf, format, timeptr, loc)
 */
static int
luab_strptime_l(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    luab_iovec_t *buf;
    const char *format;
    struct tm *timeptr;
    luab_locale_t *xloc;
    locale_t loc;
    caddr_t bp, dp;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(TM, TYPE, __func__);
    m2 = luab_xmod(LOCALE, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    format = luab_checklstring(L, 2, luab_env_buf_max, NULL);
    timeptr = luab_udata(L, 3, m1, struct tm *);
    xloc = luab_udata(L, 4, m2, luab_locale_t *);

    if (((loc = xloc->ud_sdu) != NULL) &&
        ((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((dp = strptime_l(bp, format, timeptr, loc)) > 0)
                buf->iov.iov_len = strnlen(dp, luab_env_buf_max);
            else
                buf->iov.iov_len = 0;

            buf->iov_flags &= ~IOV_LOCK;
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

/*
 * Interface against <xlocale/_time.h>.
 */

static luab_module_table_t luab_xlocale_time_vec[] = {
    LUAB_FUNC("strftime_l",         luab_strftime_l),
    LUAB_FUNC("strptime_l",         luab_strptime_l),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_xlocale_time_lib = {
    .m_id       = LUAB_XLOCALE_TIME_LIB_ID,
    .m_name     = LUAB_XLOCALE_TIME_LIB_KEY,
    .m_vec      = luab_xlocale_time_vec,
};
