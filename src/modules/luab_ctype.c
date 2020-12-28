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

#include <ctype.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_CTYPE_LIB_ID    1608691384
#define LUAB_CTYPE_LIB_KEY    "ctype"

extern luab_module_t luab_ctype_lib;

/*
 * Service primitives.
 */

/***
 * runetype_l(3) - convert rune literal
 *
 * @function runetype_l
 *
 * @param c                 Specifies argument type for ctype functions, either
 *                          by an instance of (LUA_TNUMBER) or by an instance
 *                          of (LUA_TUSERDATA(CT_RUNE)).
 *
 * @param locale            Specifies locale about duplicated by an
 *                          instance of (LUA_TUSERDATA(LOCALE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.runetype_l(c, locale)
 */
static int
luab_runetype_l(lua_State *L)
{
    luab_module_t *m0, *m1;
    __ct_rune_t x, *xp;
    luab_locale_t *xloc;
    locale_t locale;
    unsigned long status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(CT_RUNE, TYPE, __func__);
    m1 = luab_xmod(LOCALE, TYPE, __func__);

    if (lua_isnumber(L, 1) != 0) {
        x = (__ct_rune_t)luab_checkinteger(L, 1, luab_env_int_max);
        xp = &x;
    } else
        xp = luab_udata(L, 1, m0, __ct_rune_t *);

    xloc = luab_udata(L, 2, m1, luab_locale_t *);

    if ((locale = xloc->ud_sdu) != NULL) {

        if (xp != NULL)
            status = ___runetype_l(*xp, locale);
        else {
            errno = ERANGE;
            status = -1;
        }
    } else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * runetype(3) - convert rune literal
 *
 * @function runetype
 *
 * @param c                 Specifies argument type for ctype functions, either
 *                          by an instance of (LUA_TNUMBER) or by an instance
 *                          of (LUA_TUSERDATA(CT_RUNE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.runetype(c)
 */
static int
luab_runetype(lua_State *L)
{
    luab_module_t *m0;
    __ct_rune_t x, *xp;
    unsigned long status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(CT_RUNE, TYPE, __func__);

    if (lua_isnumber(L, 1) != 0) {
        x = (__ct_rune_t)luab_checkinteger(L, 1, luab_env_int_max);
        xp = &x;
    } else
        xp = luab_udata(L, 1, m0, __ct_rune_t *);

    if (xp != NULL)
        status = ___runetype(*xp);
    else {
        errno = ERANGE;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}


/*
 * Generator functions.
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(CT_RUNE)).
 *
 * @function ct_rune_create
 *
 * @param x                 Specifies initial value.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ct_rune [, err, msg ] = bsd.ctype.ct_rune_create(x)
 */
static int
luab_ct_rune_create(lua_State *L)
{
    luab_xmodule_t xm;
    __ct_rune_t x;

    xm.xm_idx = luab_idx(CT_RUNE);
    xm.xm_id = luab_xcookie(CT_RUNE, TYPE);
    xm.xm_fname = __func__;

    x = (__ct_rune_t)luab_checkxinteger(L, 1, &xm, luab_env_int_max);

    return (luab_pushxdata(L, xm.xm_mod, &x));
}

/*
 * Interface against <ctype.h>.
 */

static luab_module_table_t luab_ctype_vec[] = {
    LUAB_INT("_CTYPE_A",                _CTYPE_A),
    LUAB_INT("_CTYPE_C",                _CTYPE_C),
    LUAB_INT("_CTYPE_D",                _CTYPE_D),
    LUAB_INT("_CTYPE_G",                _CTYPE_G),
    LUAB_INT("_CTYPE_L",                _CTYPE_L),
    LUAB_INT("_CTYPE_P",                _CTYPE_P),
    LUAB_INT("_CTYPE_S",                _CTYPE_S),
    LUAB_INT("_CTYPE_U",                _CTYPE_U),
    LUAB_INT("_CTYPE_X",                _CTYPE_X),
    LUAB_INT("_CTYPE_B",                _CTYPE_B),
    LUAB_INT("_CTYPE_R",                _CTYPE_R),
    LUAB_INT("_CTYPE_I",                _CTYPE_I),
    LUAB_INT("_CTYPE_T",                _CTYPE_T),
    LUAB_INT("_CTYPE_Q",                _CTYPE_Q),
    LUAB_INT("_CTYPE_N",                _CTYPE_N),
    LUAB_INT("_CTYPE_SW0",              _CTYPE_SW0),
    LUAB_INT("_CTYPE_SW1",              _CTYPE_SW1),
    LUAB_INT("_CTYPE_SW2",              _CTYPE_SW2),
    LUAB_INT("_CTYPE_SW3",              _CTYPE_SW3),
    LUAB_INT("_CTYPE_SWM",              _CTYPE_SWM),
    LUAB_INT("_CTYPE_SWS",              _CTYPE_SWS),
    LUAB_FUNC("runetype_l",             luab_runetype_l),
    LUAB_FUNC("runetype",               luab_runetype),
    LUAB_FUNC("ct_rune_create",         luab_ct_rune_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_ctype_lib = {
    .m_id       = LUAB_CTYPE_LIB_ID,
    .m_name     = LUAB_CTYPE_LIB_KEY,
    .m_vec      = luab_ctype_vec,
};
