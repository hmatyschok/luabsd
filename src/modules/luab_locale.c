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

#define LUAB_LOCALE_LIB_ID    1610217541
#define LUAB_LOCALE_LIB_KEY   "locale"

extern luab_module_t luab_locale_lib;

/*
 * Service primitives.
 */

/***
 * localeconv(3) - natural language formatting for C
 *
 * @function localeconv
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.locale.localeconv()
 */
static int
luab_localeconv(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;

    (void)luab_core_checkmaxargs(L, 0);

    m = luab_xmod(LCONV, TYPE, __func__);

    if ((lc = localeconv()) == NULL)
        m = NULL;

    return (luab_pushxdata(L, m, lc));
}

/***
 * setlocale(3) - language information
 *
 * @function setlocale
 *
 * @param category          Specifies routine by its category,
 *                          (LUA_T{NIL,NUMBER,USERDATA(INT)).
 * @param locale            Specifies locale by (LUA_T{NIL,STRING}.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.locale.setlocale(category, locale)
 */
static int
luab_setlocale(lua_State *L)
{
    luab_module_t *m;
    int category;
    const char *locale;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(INT, TYPE, __func__);

    category = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    locale = luab_checklstringisnil(L, 2, luab_env_name_max, NULL);

    if ((dp = setlocale(category, locale)) == NULL)
        errno = ENOENT;

    return (luab_pushstring(L, dp));
}

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(LCONV)).
 *
 * @function lconv_create
 *
 * @param arg           Instance of (LUA_TUSERDATA(LCONV)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage lconv [, err, msg ] = bsd.locale.lconv_create([ arg ])
 */
static int
luab_lconv_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LCONV, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/*
 * Interface against <locale.h>
 */

static luab_module_table_t luab_locale_vec[] = {
    LUAB_INT("LC_ALL",                  LC_ALL),
    LUAB_INT("LC_COLLATE",              LC_COLLATE),
    LUAB_INT("LC_CTYPE",                LC_CTYPE),
    LUAB_INT("LC_MONETARY",             LC_MONETARY),
    LUAB_INT("LC_NUMERIC",              LC_NUMERIC),
    LUAB_INT("LC_TIME",                 LC_TIME),
    LUAB_INT("LC_MESSAGES",             LC_MESSAGES),
    LUAB_INT("_LC_LAST",                _LC_LAST),
    LUAB_FUNC("localeconv",             luab_localeconv),
    LUAB_FUNC("setlocale",              luab_setlocale),
    LUAB_FUNC("lconv_create",           luab_lconv_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_locale_lib = {
    .m_id       = LUAB_LOCALE_LIB_ID,
    .m_name     = LUAB_LOCALE_LIB_KEY,
    .m_vec      = luab_locale_vec,
};
