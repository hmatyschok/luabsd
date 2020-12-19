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

#include <locale.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_XLOCALE_LOCALE_LIB_ID    1608192109
#define LUAB_XLOCALE_LOCALE_LIB_KEY    "locale"

extern luab_module_t luab_xlocale_locale_lib;

/*
 * XXX
 *  We shall implement a SAP against (LC_GLOBAL_LOCALE).
 */


/*
 * Service primitives.
 */

/***
 * duplocale(3) - duplicate an locale
 *
 * @function duplocale
 *
 * @param locale            Specifies locale about duplicated by an
 *                          instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage locale [, err, msg ] = bsd.xlocale.locale.duplocale(locale)
 */
static int
luab_duplocale(lua_State *L)
{
    luab_module_t *m;
    locale_t locale;
    locale_t ret;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LOCALE, TYPE, __func__);
    locale = luab_udata(L, 1, m, locale_t);

    if (locale != NULL)
        ret = duplocale(locale);
    else {
        ret = NULL;
        m = NULL;
    }
    return (luab_pushxdata(L, m, ret));
}

/***
 * freelocale(3) - Frees a locale created with duplocale(3) or newlocale(3)
 *
 * @function freelocale
 *
 * @param locale            Specifies locale, instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.xlocale.locale.freelocale(locale)
 */
static int
luab_freelocale(lua_State *L)
{
    luab_module_t *m;
    locale_t locale;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LOCALE, TYPE, __func__);
    locale = luab_udata(L, 1, m, locale_t);

    if (locale != NULL) {
        freelocale(locale);
        status = 0;
    } else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}

/*
 * Generator functions.
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @function locale_create
 *
 * @param x                 Specifies initial value or (LUA_TNIL).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage time [, err, msg ] = bsd.xlocale.locale.locale_create(x)
 */
static int
luab_locale_create(lua_State *L)
{
    luab_module_t *m;
    locale_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LOCALE, TYPE, __func__);
    x = luab_udataisnil(L, 1, m, locale_t);
    return (luab_pushxdata(L, m, x));
}

/*
 * Interface against <xlocale/_locale.h>.
 */

static luab_module_table_t luab_xlocale_locale_vec[] = {
    LUAB_INT("LC_COLLATE_MASK",             LC_COLLATE_MASK),
    LUAB_INT("LC_CTYPE_MASK",               LC_CTYPE_MASK),
    LUAB_INT("LC_MONETARY_MASK",            LC_MONETARY_MASK),
    LUAB_INT("LC_NUMERIC_MASK",             LC_NUMERIC_MASK),
    LUAB_INT("LC_TIME_MASK",                LC_TIME_MASK),
    LUAB_INT("LC_MESSAGES_MASK",            LC_MESSAGES_MASK),
    LUAB_INT("LC_ALL_MASK",                 LC_ALL_MASK),
    LUAB_FUNC("duplocale",                  luab_duplocale),
    LUAB_FUNC("freelocale",                 luab_freelocale),
    LUAB_FUNC("locale_create",              luab_locale_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_xlocale_locale_lib = {
    .m_id       = LUAB_XLOCALE_LOCALE_LIB_ID,
    .m_name     = LUAB_XLOCALE_LOCALE_LIB_KEY,
    .m_vec      = luab_xlocale_locale_vec,
};
