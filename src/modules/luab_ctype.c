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
    __ct_rune_t c;
    luab_locale_t *xloc;
    locale_t locale;
    unsigned long x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(CT_RUNE, TYPE, __func__);
    m1 = luab_xmod(LOCALE, TYPE, __func__);

    c = (__ct_rune_t)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    xloc = luab_udata(L, 2, m1, luab_locale_t *);

    if ((locale = xloc->ud_sdu) != NULL)
        x = ___runetype_l(c, locale);
    else {
        errno = ENXIO;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
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
    luab_module_t *m;
    __ct_rune_t c;
    unsigned long x;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(CT_RUNE, TYPE, __func__);

    c = (__ct_rune_t)luab_checkxinteger(L, 1, m, luab_env_int_max);
    x = ___runetype(c);

    return (luab_pushxinteger(L, x));
}

/***
 * isalnum(3) - alphanumeric character set
 *
 * @function isalnum
 *
 * @param c                 Specifies either value over (unsigned char)
 *                          or an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.isalnum(c)
 */
static int
luab_isalnum(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_char_max);
    status = isalnum(c);

    return (luab_pushxinteger(L, status));
}

/***
 * isalpha(3) - alphabetic character set
 *
 * @function isalpha
 *
 * @param c                 Specifies either value over (unsigned char)
 *                          or an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.isalpha(c)
 */
static int
luab_isalpha(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_char_max);
    status = isalpha(c);

    return (luab_pushxinteger(L, status));
}

/***
 * iscntrl(3) - control character set
 *
 * @function iscntrl
 *
 * @param c                 Specifies either value over (unsigned char)
 *                          or an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.iscntrl(c)
 */
static int
luab_iscntrl(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_char_max);
    status = iscntrl(c);

    return (luab_pushxinteger(L, status));
}

/***
 * isdigit(3) - decimal-digit character set
 *
 * @function isdigit
 *
 * @param c                 Specifies either value over (unsigned char)
 *                          or an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.isdigit(c)
 */
static int
luab_isdigit(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_char_max);
    status = isdigit(c);

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
    luab_module_t *m;
    __ct_rune_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CT_RUNE, TYPE, __func__);
    x = (__ct_rune_t)luab_checkxinteger(L, 1, m, luab_env_int_max);

    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(CT_RUNE)).
 *
 * @function ct_rune_tolower
 *
 * @param x                 Specifies initial value about converted.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ct_rune [, err, msg ] = bsd.ctype.ct_rune_tolower(x)
 */
static int
luab_ct_rune_tolower(lua_State *L)
{
    luab_module_t *m;
    __ct_rune_t c, x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CT_RUNE, TYPE, __func__);
    c = (__ct_rune_t)luab_checkxinteger(L, 1, m, luab_env_int_max);

    x = ___tolower(c);

    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(CT_RUNE)).
 *
 * @function ct_rune_toupper
 *
 * @param x                 Specifies initial value about converted.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ct_rune [, err, msg ] = bsd.ctype.ct_rune_toupper(x)
 */
static int
luab_ct_rune_toupper(lua_State *L)
{
    luab_module_t *m;
    __ct_rune_t c, x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CT_RUNE, TYPE, __func__);
    c = (__ct_rune_t)luab_checkxinteger(L, 1, m, luab_env_int_max);

    x = ___toupper(c);

    return (luab_pushxdata(L, m, &x));
}

/***
 * isgraph(3) - printing character set (space character exclusive)
 *
 * @function isgraph
 *
 * @param c                 Specifies either value over (unsigned char)
 *                          or an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.isgraph(c)
 */
static int
luab_isgraph(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_char_max);
    status = isgraph(c);

    return (luab_pushxinteger(L, status));
}

/***
 * islower(3) - lower-case character set
 *
 * @function islower
 *
 * @param c                 Specifies either value over (unsigned char)
 *                          or an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.islower(c)
 */
static int
luab_islower(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_char_max);
    status = islower(c);

    return (luab_pushxinteger(L, status));
}

/***
 * isprint(3) - printing character set (space character inclusive)
 *
 * @function isprint
 *
 * @param c                 Specifies either value over (unsigned char)
 *                          or an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.isprint(c)
 */
static int
luab_isprint(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_char_max);
    status = isprint(c);

    return (luab_pushxinteger(L, status));
}

/***
 * ispunct(3) - punctuation character set
 *
 * @function ispunct
 *
 * @param c                 Specifies either value over (unsigned char)
 *                          or an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.ispunct(c)
 */
static int
luab_ispunct(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_char_max);
    status = ispunct(c);

    return (luab_pushxinteger(L, status));
}

/***
 * isspace(3) - white-space character set
 *
 * @function isspace
 *
 * @param c                 Specifies either value over (unsigned char)
 *                          or an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.isspace(c)
 */
static int
luab_isspace(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_char_max);
    status = isspace(c);

    return (luab_pushxinteger(L, status));
}

/***
 * isupper(3) - upper-case character set
 *
 * @function isupper
 *
 * @param c                 Specifies either value over (unsigned char)
 *                          or an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.isupper(c)
 */
static int
luab_isupper(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_char_max);
    status = isupper(c);

    return (luab_pushxinteger(L, status));
}

/***
 * isxdigit(3) - hexadecimal-digit character set
 *
 * @function isxdigit
 *
 * @param c                 Specifies either value over (unsigned char)
 *                          or an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.isxdigit(c)
 */
static int
luab_isxdigit(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_char_max);
    status = isxdigit(c);

    return (luab_pushxinteger(L, status));
}

/***
 * tolower(3) - upper case to lower case letter conversion
 *
 * @function tolower
 *
 * @param c                 Specifies either value over (unsigned char)
 *                          or an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.tolower(c)
 */
static int
luab_tolower(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_char_max);
    status = tolower(c);

    return (luab_pushxinteger(L, status));
}

/***
 * toupper(3) - lower case to upper case letter conversion
 *
 * @function toupper
 *
 * @param c                 Specifies either value over (unsigned char)
 *                          or an instance of (LUA_TUSERDATA(UINT8)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ctype.toupper(c)
 */
static int
luab_toupper(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = (uint8_t)luab_checkxinteger(L, 1, m, luab_env_char_max);
    status = toupper(c);

    return (luab_pushxinteger(L, status));
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
    LUAB_FUNC("runetype_l",             luab_runetype_l),   /* XXX */
    LUAB_FUNC("runetype",               luab_runetype),
    LUAB_FUNC("isalnum",                luab_isalnum),
    LUAB_FUNC("isalpha",                luab_isalpha),
    LUAB_FUNC("iscntrl",                luab_iscntrl),
    LUAB_FUNC("isdigit",                luab_isdigit),
    LUAB_FUNC("isgraph",                luab_isgraph),
    LUAB_FUNC("islower",                luab_islower),
    LUAB_FUNC("isprint",                luab_isprint),
    LUAB_FUNC("ispunct",                luab_ispunct),
    LUAB_FUNC("isspace",                luab_isspace),
    LUAB_FUNC("isupper",                luab_isupper),
    LUAB_FUNC("isxdigit",               luab_isxdigit),
    LUAB_FUNC("tolower",                luab_tolower),
    LUAB_FUNC("toupper",                luab_toupper),
    LUAB_FUNC("ct_rune_create",         luab_ct_rune_create),
    LUAB_FUNC("ct_rune_tolower",        luab_ct_rune_tolower),
    LUAB_FUNC("ct_rune_toupper",        luab_ct_rune_toupper),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_ctype_lib = {
    .m_id       = LUAB_CTYPE_LIB_ID,
    .m_name     = LUAB_CTYPE_LIB_KEY,
    .m_vec      = luab_ctype_vec,
};
