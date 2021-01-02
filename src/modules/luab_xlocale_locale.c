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

/*
 * XXX
 *  We shall implement a SAP against (LC_GLOBAL_LOCALE).
 */

#define LUAB_XLOCALE_LOCALE_LIB_ID    1608192109
#define LUAB_XLOCALE_LOCALE_LIB_KEY    "locale"

extern luab_module_t luab_xlocale_locale_lib;

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
    luab_locale_t *xloc;
    locale_t locale, ret;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LOCALE, TYPE, __func__);
    xloc = luab_udata(L, 1, m, luab_locale_t *);

    if ((locale = xloc->ud_sdu) != NULL)
        ret = duplocale(locale);
    else {
        ret = NULL;
        m = NULL;
    }
    return (luab_pushxdata(L, m, ret));
}

/***
 * freelocale(3) - frees a locale created with duplocale(3) or newlocale(3)
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
    luab_locale_t *xloc;
    locale_t locale;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LOCALE, TYPE, __func__);
    xloc = luab_udata(L, 1, m, luab_locale_t *);

    if ((locale = xloc->ud_sdu) != NULL) {
        freelocale(locale);
        xloc->ud_sdu = NULL;
        status = luab_env_success;
    } else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * newlocale(3) - create a new locale
 *
 * @function newlocale
 *
 * @param mask              Specfies locale components with values from
 *
 *                              LC_{
 *                                  COLLATE_MASK,
 *                                  CTYPE_MASK,
 *                                  MESSAGES_MASK,
 *                                  MONETARY_MASK,
 *                                  NUMERIC_MASK,
 *                                  TIME_MASK
 *                              }
 *
 *                          are constructed by inclusive OR.
 * @param locale            Specifies its name.
 * @param base              Specifies locale about duplicated by an
 *                          instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage locale [, err, msg ] = bsd.xlocale.locale.newlocale(mask, locale, base)
 */
static int
luab_newlocale(lua_State *L)
{
    luab_module_t *m;
    int mask;
    const char *locale;
    luab_locale_t *xloc;
    locale_t base, ret;

    (void)luab_core_checkmaxargs(L, 3);

    m = luab_xmod(LOCALE, TYPE, __func__);
    mask = luab_checkinteger(L, 1, INT_MAX);
    locale = luab_checklstring(L, 2, luab_env_name_max, NULL);
    xloc = luab_udata(L, 3, m, luab_locale_t *);

    if ((base = xloc->ud_sdu) != NULL) {

        if ((ret = newlocale(mask, locale, base)) == NULL)
            m = NULL;
    } else {
        ret = NULL;
        m = NULL;
    }
    return (luab_pushxdata(L, m, ret));
}

/***
 * querylocale(3) - look up the locale name for a specified category
 *
 * @function querylocale
 *
 * @param mask              Specfies locale components with values from
 *
 *                              LC_{
 *                                  COLLATE_MASK,
 *                                  CTYPE_MASK,
 *                                  MESSAGES_MASK,
 *                                  MONETARY_MASK,
 *                                  NUMERIC_MASK,
 *                                  TIME_MASK
 *                              }
 *
 *                          are constructed by inclusive OR.
 * @param locale            Specifies locale about queried by an
 *                          instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.xlocale.locale.querylocale(mask, locale)
 */
static int
luab_querylocale(lua_State *L)
{
    luab_module_t *m;
    int mask;
    luab_locale_t *xloc;
    locale_t locale;
    const char *dp;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(LOCALE, TYPE, __func__);
    mask = luab_checkinteger(L, 1, INT_MAX);
    xloc = luab_udata(L, 2, m, luab_locale_t *);

    if ((locale = xloc->ud_sdu) != NULL)
        dp = querylocale(mask, locale);
    else {
        errno = ENOENT;
        dp = NULL;
    }
    return (luab_pushstring(L, dp));
}

/***
 * uselocale(3) - set a thread-local locale
 *
 * @function uselocale
 *
 * @param loc               Specifies locale by an instance
 *                          of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage locale [, err, msg ] = bsd.xlocale.locale.uselocale(loc)
 */
static int
luab_uselocale(lua_State *L)
{
    luab_module_t *m;
    luab_locale_t *xloc;
    locale_t loc, ret;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LOCALE, TYPE, __func__);
    xloc = luab_udata(L, 1, m, luab_locale_t *);

    if ((loc = xloc->ud_sdu) != NULL)
        ret = uselocale(loc);
    else {
        ret = NULL;
        m = NULL;
    }
    return (luab_pushxdata(L, m, ret));
}

/*
 * Generator functions.
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @function locale_create
 *
 * @param arg               Specifies its initial value by an instance of
 *
 *                              (LUA_T{NIL,USERDATA(LOCALE)}).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage time [, err, msg ] = bsd.xlocale.locale.locale_create(x)
 */
static int
luab_locale_create(lua_State *L)
{
    luab_module_t *m;
    luab_locale_t *xloc;
    locale_t locale;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LOCALE, TYPE, __func__);

    if ((xloc = luab_udataisnil(L, 1, m, luab_locale_t *)) != NULL)
        locale = xloc->ud_sdu;
    else {
        errno = ENOENT;
        locale = NULL;
    }
    return (luab_pushxdata(L, m, locale));
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
    LUAB_FUNC("newlocale",                  luab_newlocale),
    LUAB_FUNC("querylocale",                luab_querylocale),
    LUAB_FUNC("uselocale",                  luab_uselocale),
    LUAB_FUNC("locale_create",              luab_locale_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_xlocale_locale_lib = {
    .m_id       = LUAB_XLOCALE_LOCALE_LIB_ID,
    .m_name     = LUAB_XLOCALE_LOCALE_LIB_KEY,
    .m_vec      = luab_xlocale_locale_vec,
};
