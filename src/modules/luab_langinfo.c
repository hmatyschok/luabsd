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

#define LUAB_LANGINFO_LIB_ID    1608651964
#define LUAB_LANGINFO_LIB_KEY    "langinfo"

extern luab_module_t luab_langinfo_lib;

/*
 * Service primitives.
 */

/***
 * nl_langinfo(3) - language information
 *
 * @function nl_langinfo
 *
 * @param item              The specified item obout requested information,
 *                          either by an instance of (LUA_TNUMBER) or by an
 *                          instance of (LUA_TUSERDARA(NL_ITEM)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.langinfo.nl_langinfo(item)
 */
static int
luab_nl_langinfo(lua_State *L)
{
    luab_module_t *m;
    nl_item x, *xp;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(NL_ITEM, TYPE, __func__);

    if (lua_isnumber(L, 1) != 0) {
        x = (nl_item)luab_checkinteger(L, 1, luab_env_int_max);
        xp = &x;
    } else
        xp = luab_udataisnil(L, 1, m, nl_item *);

    if (xp != NULL)
        dp = nl_langinfo(*xp);
    else
        dp = NULL;

    return (luab_pushstring(L, dp));
}

/*
 * Generator functions.
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(NL_ITEM)).
 *
 * @function nl_item_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage nl_item [, err, msg ] = bsd.sys.stdint.nl_item_create(x)
 */
static int
luab_nl_item_create(lua_State *L)
{
    luab_module_t *m;
    nl_item x, *xp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(NL_ITEM, TYPE, __func__);

    if (lua_isnumber(L, 1) != 0) {
        x = (nl_item)luab_checkinteger(L, 1, luab_env_ulong_max);
        xp = &x;
    } else
        xp = luab_udataisnil(L, 1, m, nl_item *);

    return (luab_pushxdata(L, m, xp));
}

/*
 * Interface against <langinfo.h>.
 */

static luab_module_table_t luab_langinfo_vec[] = {
    LUAB_INT("CODESET",             CODESET),
    LUAB_INT("D_T_FMT",             D_T_FMT),
    LUAB_INT("D_FMT",               D_FMT),
    LUAB_INT("T_FMT",               T_FMT),
    LUAB_INT("T_FMT_AMPM",          T_FMT_AMPM),
    LUAB_INT("AM_STR",              AM_STR),
    LUAB_INT("PM_STR",              PM_STR),
    LUAB_INT("DAY_1",               DAY_1),
    LUAB_INT("DAY_2",               DAY_2),
    LUAB_INT("DAY_3",               DAY_3),
    LUAB_INT("DAY_4",               DAY_4),
    LUAB_INT("DAY_5",               DAY_5),
    LUAB_INT("DAY_6",               DAY_6),
    LUAB_INT("DAY_7",               DAY_7),
    LUAB_INT("ABDAY_1",             ABDAY_1),
    LUAB_INT("ABDAY_2",             ABDAY_2),
    LUAB_INT("ABDAY_3",             ABDAY_3),
    LUAB_INT("ABDAY_4",             ABDAY_4),
    LUAB_INT("ABDAY_5",             ABDAY_5),
    LUAB_INT("ABDAY_6",             ABDAY_6),
    LUAB_INT("ABDAY_7",             ABDAY_7),
    LUAB_INT("MON_1",               MON_1),
    LUAB_INT("MON_2",               MON_2),
    LUAB_INT("MON_3",               MON_3),
    LUAB_INT("MON_4",               MON_4),
    LUAB_INT("MON_5",               MON_5),
    LUAB_INT("MON_6",               MON_6),
    LUAB_INT("MON_7",               MON_7),
    LUAB_INT("MON_8",               MON_8),
    LUAB_INT("MON_9",               MON_9),
    LUAB_INT("MON_10",              MON_10),
    LUAB_INT("MON_11",              MON_11),
    LUAB_INT("MON_12",              MON_12),
    LUAB_INT("ABMON_1",             ABMON_1),
    LUAB_INT("ABMON_2",             ABMON_2),
    LUAB_INT("ABMON_3",             ABMON_3),
    LUAB_INT("ABMON_4",             ABMON_4),
    LUAB_INT("ABMON_5",             ABMON_5),
    LUAB_INT("ABMON_6",             ABMON_6),
    LUAB_INT("ABMON_7",             ABMON_7),
    LUAB_INT("ABMON_8",             ABMON_8),
    LUAB_INT("ABMON_9",             ABMON_9),
    LUAB_INT("ABMON_10",            ABMON_10),
    LUAB_INT("ABMON_11",            ABMON_11),
    LUAB_INT("ABMON_12",            ABMON_12),
    LUAB_INT("ERA",                 ERA),
    LUAB_INT("ERA_D_FMT",           ERA_D_FMT),
    LUAB_INT("ERA_D_T_FMT",         ERA_D_T_FMT),
    LUAB_INT("ERA_T_FMT",           ERA_T_FMT),
    LUAB_INT("ALT_DIGITS",          ALT_DIGITS),
    LUAB_INT("RADIXCHAR",           RADIXCHAR),
    LUAB_INT("THOUSEP",             THOUSEP),
    LUAB_INT("YESEXPR",             YESEXPR),
    LUAB_INT("NOEXPR",              NOEXPR),
#if __BSD_VISIBLE || (__XSI_VISIBLE && __XSI_VISIBLE <= 500)
    LUAB_INT("YESSTR",              YESSTR),
    LUAB_INT("NOSTR",               NOSTR),
#endif
    LUAB_INT("CRNCYSTR",            CRNCYSTR),
#if __BSD_VISIBLE
    LUAB_INT("D_MD_ORDER",          D_MD_ORDER),
#endif
    LUAB_INT("ALTMON_1",            ALTMON_1),
    LUAB_INT("ALTMON_2",            ALTMON_2),
    LUAB_INT("ALTMON_3",            ALTMON_3),
    LUAB_INT("ALTMON_4",            ALTMON_4),
    LUAB_INT("ALTMON_5",            ALTMON_5),
    LUAB_INT("ALTMON_6",            ALTMON_6),
    LUAB_INT("ALTMON_7",            ALTMON_7),
    LUAB_INT("ALTMON_8",            ALTMON_8),
    LUAB_INT("ALTMON_9",            ALTMON_9),
    LUAB_INT("ALTMON_10",           ALTMON_10),
    LUAB_INT("ALTMON_11",           ALTMON_11),
    LUAB_INT("ALTMON_12",           ALTMON_12),
    LUAB_FUNC("nl_langinfo",        luab_nl_langinfo),
    LUAB_FUNC("nl_item_create",     luab_nl_item_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_langinfo_lib = {
    .m_id       = LUAB_LANGINFO_LIB_ID,
    .m_name     = LUAB_LANGINFO_LIB_KEY,
    .m_vec      = luab_langinfo_vec,
};
