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

#include <langinfo.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_XLOCALE_LANGINFO_LIB_ID    1608640141
#define LUAB_XLOCALE_LANGINFO_LIB_KEY    "langinfo"

extern luab_module_t luab_xlocale_langinfo_lib;

/*
 * Service primitives.
 */

/***
 * nl_langinfo_l(3) - language information
 *
 * @function nl_langinfo_l
 *
 * @param item              The specified item obout requested information,
 *                          either by an instance of (LUA_TNUMBER) or by an
 *                          instance of (LUA_TUSERDARA(NL_ITEM)).
 *
 * @param loc               Specifies locale about duplicated by an
 *                          instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.langinfo.nl_langinfo_l(item, loc)
 */
static int
luab_nl_langinfo_l(lua_State *L)
{
    luab_module_t *m0, *m1;
    nl_item x, *xp;
    luab_locale_t *xloc;
    locale_t loc;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(NL_ITEM, TYPE, __func__);
    m1 = luab_xmod(LOCALE, TYPE, __func__);

    if (lua_isnumber(L, 1) != 0) {
        x = (nl_item)luab_checkinteger(L, 1, luab_env_int_max);
        xp = &x;
    } else
        xp = luab_udataisnil(L, 1, m0, nl_item *);

    xloc = luab_udata(L, 2, m1, luab_locale_t *);

    if ((loc = xloc->ud_sdu) != NULL) {

        if (xp != NULL)
            dp = nl_langinfo_l(*xp, loc);
        else
            dp = NULL;
    } else
        dp = NULL;

    return (luab_pushstring(L, dp));
}

/*
 * Interface against <xlocale/_langinfo.h>.
 */

static luab_module_table_t luab_xlocale_langinfo_vec[] = {
    LUAB_FUNC("nl_langinfo_l",          luab_nl_langinfo_l),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_xlocale_langinfo_lib = {
    .m_id       = LUAB_XLOCALE_LANGINFO_LIB_ID,
    .m_name     = LUAB_XLOCALE_LANGINFO_LIB_KEY,
    .m_vec      = luab_xlocale_langinfo_vec,
};
