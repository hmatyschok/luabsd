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

#include <ttyent.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_TTYENT_LIB_ID    1604753354
#define LUAB_TTYENT_LIB_KEY    "ttyent"

extern luab_module_t luab_ttyent_lib;

/*
 * Service primitives.
 */

/***
 * getttyent(3) - ttys(5) file routines
 *
 * @function getttyent
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ttyent.getttyent()
 */
static int
luab_getttyent(lua_State *L)
{
    luab_module_t *m;
    struct ttyent *typ;

    (void)luab_core_checkmaxargs(L, 0);

    m = luab_xmod(TTYENT, TYPE, __func__);

    if ((typ = getttyent()) == NULL)
        m = NULL;

     return (luab_pushxdata(L, m, typ));
}

/***
 * getttynam(3) - ttys(5) file routines
 *
 * @function getttynam
 *
 * @param name              Specifies name of character-special file.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ttyent.getttynam(name)
 */
static int
luab_getttynam(lua_State *L)
{
    luab_module_t *m;
    const char *name;
    struct ttyent *typ;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TTYENT, TYPE, __func__);
    name = luab_checklstring(L, 1, luab_env_tty_name_max, NULL);

    if ((typ = getttynam(name)) == NULL)
        m = NULL;

    return (luab_pushxdata(L, m, typ));
}

/***
 * setttyent(3) - ttys(5) file routines
 *
 * @function setttyent
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ttyent.setttyent()
 */
static int
luab_setttyent(lua_State *L)
{
    int status;

    (void)luab_core_checkmaxargs(L, 0);

    status = setttyent();
    return (luab_pushxinteger(L, status));
}

/***
 * endttyent(3) - ttys(5) file routines
 *
 * @function endttyent
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ttyent.endttyent()
 */
static int
luab_endttyent(lua_State *L)
{
    int status;

    (void)luab_core_checkmaxargs(L, 0);

    status = endttyent();
    return (luab_pushxinteger(L, status));
}

/***
 * isdialuptty(3) - ttys(5) file routines
 *
 * @function isdialuptty
 *
 * @param name              Specifies name of character-special file.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ttyent.isdialuptty(name)
 */
static int
luab_isdialuptty(lua_State *L)
{
    const char *name;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    name = luab_checklstring(L, 1, luab_env_tty_name_max, NULL);
    status = isdialuptty(name);
    return (luab_pushxinteger(L, status));
}

/***
 * isnettty(3) - ttys(5) file routines
 *
 * @function isnettty
 *
 * @param name              Specifies name of character-special file.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.ttyent.isnettty(name)
 */
static int
luab_isnettty(lua_State *L)
{
    const char *name;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    name = luab_checklstring(L, 1, luab_env_tty_name_max, NULL);
    status = isnettty(name);
    return (luab_pushxinteger(L, status));
}

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(TTYENT)).
 *
 * @function ttyent_create
 *
 * @param ttyent          Instance of (LUA_TUSERDATA(TTYENT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ttyent [, err, msg ] = bsd.ttyent.ttyent_create([ ttyent ])
 */
static int
luab_ttyent_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TTYENT, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/*
 * Interface against <ttyent.h>.
 */

static luab_module_table_t luab_ttyent_vec[] = { /* ttyent.h */
    LUAB_STR("_PATH_TTYS",              _PATH_TTYS),
    LUAB_STR("_TTYS_OFF",               _TTYS_OFF),
    LUAB_STR("_TTYS_ON",                _TTYS_ON),
    LUAB_STR("_TTYS_ONIFCONSOLE",       _TTYS_ONIFCONSOLE),
    LUAB_STR("_TTYS_ONIFEXISTS",        _TTYS_ONIFEXISTS),
    LUAB_STR("_TTYS_SECURE",            _TTYS_SECURE),
    LUAB_STR("_TTYS_INSECURE",          _TTYS_INSECURE),
    LUAB_STR("_TTYS_WINDOW",            _TTYS_WINDOW),
    LUAB_STR("_TTYS_GROUP",             _TTYS_GROUP),
    LUAB_STR("_TTYS_NOGROUP",           _TTYS_NOGROUP),
    LUAB_STR("_TTYS_DIALUP",            _TTYS_DIALUP),
    LUAB_STR("_TTYS_NETWORK",           _TTYS_NETWORK),
    LUAB_INT("TTY_ON",                  TTY_ON),
    LUAB_INT("TTY_SECURE",              TTY_SECURE),
    LUAB_INT("TTY_DIALUP",              TTY_DIALUP),
    LUAB_INT("TTY_NETWORK",             TTY_NETWORK),
    LUAB_INT("TTY_IFEXISTS",            TTY_IFEXISTS),
    LUAB_INT("TTY_IFCONSOLE",           TTY_IFCONSOLE),
    LUAB_FUNC("getttyent",              luab_getttyent),
    LUAB_FUNC("getttynam",              luab_getttynam),
    LUAB_FUNC("settyend",               luab_setttyent),
    LUAB_FUNC("endttyent",              luab_endttyent),
    LUAB_FUNC("isdialuptty",            luab_isdialuptty),
    LUAB_FUNC("isnettty",               luab_isnettty),
    LUAB_FUNC("ttyent_create",          luab_ttyent_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_ttyent_lib = {
    .m_id       = LUAB_TTYENT_LIB_ID,
    .m_name     = LUAB_TTYENT_LIB_KEY,
    .m_vec      = luab_ttyent_vec,
};
