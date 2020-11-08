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

#include <ttyent.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

extern luab_module_t luab_ttyent_type;

/*
 * Interface against
 *
 *  struct ttyent {
 *      char	*ty_name;
 *      char	*ty_getty;
 *      char	*ty_type;
 *  #define	TTY_ON		0x01
 *  #define	TTY_SECURE	0x02
 *  #define	TTY_DIALUP	0x04   
 *  #define	TTY_NETWORK	0x08
 *  #define	TTY_IFEXISTS	0x10
 *  #define	TTY_IFCONSOLE	0x20
 *      int	ty_status;
 *      char 	*ty_window;
 *      char	*ty_comment;
 *      char	*ty_group;
 *  };
 *
 */

typedef struct luab_ttyent {
    luab_udata_t    ud_softc;
    struct ttyent   ud_tty;
} luab_ttyent_t;

#define luab_new_ttyent(L, arg) \
    ((luab_ttyent_t *)luab_newudata(L, &luab_ttyent_type, (arg)))
#define luab_to_ttyent(L, narg) \
    (luab_toldata((L), (narg), &luab_ttyent_type, \
        struct ttyent *, sizeof(struct ttyent)))

#define LUAB_TTYENT_TYPE_ID    1604759728
#define LUAB_TTYENT_TYPE    "TTYENT*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(TTYENT)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              ty_name     = (LUA_TSTRING),
 *              ty_getty    = (LUA_TSTRING),
 *              ty_type     = (LUA_TSTRING),
 *              ty_status   = (LUA_TNUMBER),
 *              ty_window   = (LUA_TSTRING),
 *              ty_comment  = (LUA_TSTRING),
 *              ty_group    = (LUA_TSTRING),
 *          }
 *
 * @usage t = ttyent:get()
 */
static int
TTYENT_get(lua_State *L)
{
    struct ttyent *typ;

    (void)luab_core_checkmaxargs(L, 1);

    typ = luab_udata(L, 1, &luab_ttyent_type, struct ttyent *);

    lua_newtable(L);
    luab_setstring(L, -2, "ty_name",        typ->ty_name);
    luab_setstring(L, -2, "ty_getty",       typ->ty_getty);
    luab_setstring(L, -2, "ty_type",        typ->ty_type);
    luab_setinteger(L, -2, "ty_status",     typ->ty_status);
    luab_setstring(L, -2, "ty_window",      typ->ty_window);
    luab_setstring(L, -2, "ty_comment",     typ->ty_comment);
    luab_setstring(L, -2, "ty_group",       typ->ty_group);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate ttyent{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = ttyent:dump()
 */
static int
TTYENT_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_ttyent_type, sizeof(struct ttyent)));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get terminal device name.
 *
 * @function ty_name
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = ttyent:ty_name()
 */
static int
TTYENT_ty_name(lua_State *L)
{
    struct ttyent *typ;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    typ = luab_udata(L, 1, &luab_ttyent_type, struct ttyent *);
    dp = typ->ty_name;

    return (luab_pushstring(L, dp));
}

/***
 * Get command to execute.
 *
 * @function ty_getty
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = ttyent:ty_getty()
 */
static int
TTYENT_ty_getty(lua_State *L)
{
    struct ttyent *typ;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    typ = luab_udata(L, 1, &luab_ttyent_type, struct ttyent *);
    dp = typ->ty_getty;

    return (luab_pushstring(L, dp));
}

/***
 * Get terminal type for termcap.
 *
 * @function ty_type
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = ttyent:ty_type()
 */
static int
TTYENT_ty_type(lua_State *L)
{
    struct ttyent *typ;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    typ = luab_udata(L, 1, &luab_ttyent_type, struct ttyent *);
    dp = typ->ty_type;

    return (luab_pushstring(L, dp));
}

/***
 * Get status flags.
 *
 * @function ty_status
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = ttyent:ty_status()
 */
static int
TTYENT_ty_status(lua_State *L)
{
    struct ttyent *typ;
    int data;

    (void)luab_core_checkmaxargs(L, 1);

    typ = luab_udata(L, 1, &luab_ttyent_type, struct ttyent *);
    data = typ->ty_status;

    return (luab_pusherr(L, data));
}

/***
 * Get command to start windowmanager.
 *
 * @function ty_window
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = ttyent:ty_window()
 */
static int
TTYENT_ty_window(lua_State *L)
{
    struct ttyent *typ;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    typ = luab_udata(L, 1, &luab_ttyent_type, struct ttyent *);
    dp = typ->ty_window;

    return (luab_pushstring(L, dp));
}

/***
 * Get contents of comment field.
 *
 * @function ty_comment
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = ttyent:ty_comment()
 */
static int
TTYENT_ty_comment(lua_State *L)
{
    struct ttyent *typ;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    typ = luab_udata(L, 1, &luab_ttyent_type, struct ttyent *);
    dp = typ->ty_comment;

    return (luab_pushstring(L, dp));
}

/***
 * Get tty group.
 *
 * @function ty_group
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = ttyent:ty_group()
 */
static int
TTYENT_ty_group(lua_State *L)
{
    struct ttyent *typ;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    typ = luab_udata(L, 1, &luab_ttyent_type, struct ttyent *);
    dp = typ->ty_group;

    return (luab_pushstring(L, dp));
}

/*
 * Meta-methods.
 */

static int
TTYENT_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_ttyent_type));
}

static int
TTYENT_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_ttyent_type));
}

static int
TTYENT_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_ttyent_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t ttyent_methods[] = {
    LUAB_FUNC("ty_name",        TTYENT_ty_name),
    LUAB_FUNC("ty_getty",       TTYENT_ty_getty),
    LUAB_FUNC("ty_type",        TTYENT_ty_type),
    LUAB_FUNC("ty_status",      TTYENT_ty_status),
    LUAB_FUNC("ty_window",      TTYENT_ty_window),
    LUAB_FUNC("ty_comment",     TTYENT_ty_comment),
    LUAB_FUNC("ty_group",       TTYENT_ty_group),
    LUAB_FUNC("get",            TTYENT_get),
    LUAB_FUNC("dump",           TTYENT_dump),
    LUAB_FUNC("__gc",           TTYENT_gc),
    LUAB_FUNC("__len",          TTYENT_len),
    LUAB_FUNC("__tostring",     TTYENT_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
ttyent_create(lua_State *L, void *arg)
{
    return (luab_new_ttyent(L, arg));
}

static void
ttyent_init(void *ud, void *arg)
{
    luab_udata_init(&luab_ttyent_type, ud, arg);
}

static void *
ttyent_udata(lua_State *L, int narg)
{
    return (luab_to_ttyent(L, narg));
}

luab_module_t luab_ttyent_type = {
    .m_cookie   = LUAB_TTYENT_TYPE_ID,
    .m_name     = LUAB_TTYENT_TYPE,
    .m_vec      = ttyent_methods,
    .m_create   = ttyent_create,
    .m_init     = ttyent_init,
    .m_get      = ttyent_udata,
    .m_sz       = sizeof(luab_ttyent_t),
};
