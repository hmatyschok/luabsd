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

#include <regex.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_REGEX_LIB_ID    1604366574
#define LUAB_REGEX_LIB_KEY    "regex"

extern luab_module_t luab_regex_lib;

/*
 * Service primitives.
 */

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(REGEX)).
 *
 * @function regex_create
 *
 * @param regex             Instance of (LUA_TUSERDATA(REGEX)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage regex [, err, msg ] = bsd.regex.regex_create([ regex ])
 */
static int
luab_regex_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(REGEX), NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(REGMATCH)).
 *
 * @function regmatch_create
 *
 * @param regmatch          Instance of (LUA_TUSERDATA(REGMATCH)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage regmatch [, err, msg ] = bsd.regex.regmatch_create([ regmatch ])
 */
static int
luab_regmatch_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(REGMATCH), NULL));
}

/*
 * Interface against <regex.h>.
 */

static luab_module_table_t luab_regex_vec[] = { /* regex.h */
    LUAB_INT("REG_BASIC",           REG_BASIC),
    LUAB_INT("REG_EXTENDED",        REG_EXTENDED),
    LUAB_INT("REG_ICASE",           REG_ICASE),
    LUAB_INT("REG_NOSUB",           REG_NOSUB),
    LUAB_INT("REG_NEWLINE",         REG_NEWLINE),
    LUAB_INT("REG_NOSPEC",          REG_NOSPEC),
    LUAB_INT("REG_PEND",            REG_PEND),
    LUAB_INT("REG_DUMP",            REG_DUMP),
    LUAB_INT("REG_ENOSYS",          REG_ENOSYS),
    LUAB_INT("REG_NOMATCH",         REG_NOMATCH),
    LUAB_INT("REG_BADPAT",          REG_BADPAT),
    LUAB_INT("REG_ECOLLATE",        REG_ECOLLATE),
    LUAB_INT("REG_ECTYPE",          REG_ECTYPE),
    LUAB_INT("REG_EESCAPE",         REG_EESCAPE),
    LUAB_INT("REG_ESUBREG",         REG_ESUBREG),
    LUAB_INT("REG_EBRACK",          REG_EBRACK),
    LUAB_INT("REG_EPAREN",          REG_EPAREN),
    LUAB_INT("REG_EBRACE",          REG_EBRACE),
    LUAB_INT("REG_BADBR",           REG_BADBR),
    LUAB_INT("REG_ERANGE",          REG_ERANGE),
    LUAB_INT("REG_ESPACE",          REG_ESPACE),
    LUAB_INT("REG_BADRPT",          REG_BADRPT),
    LUAB_INT("REG_EMPTY",           REG_EMPTY),
    LUAB_INT("REG_ASSERT",          REG_ASSERT),
    LUAB_INT("REG_INVARG",          REG_INVARG),
    LUAB_INT("REG_ILLSEQ",          REG_ILLSEQ),
    LUAB_INT("REG_ATOI",            REG_ATOI),
    LUAB_INT("REG_ITOA",            REG_ITOA),
    LUAB_INT("REG_NOTBOL",          REG_NOTBOL),
    LUAB_INT("REG_NOTEOL",          REG_NOTEOL),
    LUAB_INT("REG_STARTEND",        REG_STARTEND),
    LUAB_INT("REG_TRACE",           REG_TRACE),
    LUAB_INT("REG_LARGE",           REG_LARGE),
    LUAB_INT("REG_BACKR",           REG_BACKR),
    LUAB_FUNC("regex_create",       luab_regex_create),
    LUAB_FUNC("regmatch_create",    luab_regmatch_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_regex_lib = {
    .m_cookie   = LUAB_REGEX_LIB_ID,
    .m_name     = LUAB_REGEX_LIB_KEY,
    .m_vec      = luab_regex_vec,
};
