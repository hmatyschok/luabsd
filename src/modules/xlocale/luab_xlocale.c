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

#define LUAB_XLOCALE_LIB_ID    1610326058
#define LUAB_XLOCALE_LIB_KEY    "xlocale"

extern luab_module_t luab_xlocale_lib;

/*
 * Service primitives.
 */

/***
 * localeconv_l(3) - natural language formatting for C
 *
 * @function localeconv_l
 *
 * @param locale            Specifies current locale by an
 *                          instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage lconv [, err, msg ] = bsd.xlocale.localeconv_l(locale)
 */
static int
luab_localeconv_l(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_locale_t *xloc;
    locale_t locale;
    struct lconv *lc;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(LOCALE, TYPE, __func__);
    m1 = luab_xmod(LCONV, TYPE, __func__);

    xloc = luab_udata(L, 1, m0, luab_locale_t *);

    if ((locale = xloc->ud_sdu) != NULL)
        lc = localeconv_l(locale);
    else {
        lc = NULL;
        m1 = NULL;
    }
    return (luab_pushxdata(L, m1, lc));
}

/*
 * Interface against <xlocale.h>.
 */

static luab_module_table_t luab_xlocale_vec[] = {
    LUAB_FUNC("localeconv_l",   luab_localeconv_l),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_xlocale_lib = {
    .m_id       = LUAB_XLOCALE_LIB_ID,
    .m_name     = LUAB_XLOCALE_LIB_KEY,
    .m_vec      = luab_xlocale_vec,
};
