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

#include <dirent.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_dir_type;

/*
 * Interface against
 *
 *  typedef void *  DIR;
 *
 */

typedef struct luab_dir {
    luab_udata_t    ud_softc;
    void            *ud_dirp;
} luab_dir_t;

#define luab_new_dir(L, arg) \
    ((luab_dir_t *)luab_newudata(L, &luab_dir_type, (arg)))
#define luab_to_dir(L, narg) \
    (luab_toldata((L), (narg), &luab_dir_type, void *, sizeof(void *)))

/*
 * Subr.
 */

static void
dir_fillxtable(lua_State *L, int narg, void *arg)
{
    void *dirp;

    if ((dirp = arg) != NULL) {

        luab_setfstring(L, narg, "dirp", "(%p)", dirp);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(DIR)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              dirp     = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ] = dir:get_table()
 */
static int
DIR_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = dir_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, &luab_dir_type);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate dir{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = dir:dump()
 */
static int
DIR_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_dir_type, sizeof(void *)));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get base address of directory.
 *
 * @function dirp
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = dir:dir()
 */
static int
DIR_dirp(lua_State *L)
{
    void *dirp;

    (void)luab_core_checkmaxargs(L, 1);

    dirp = luab_udata(L, 1, &luab_dir_type, void *);

    return (luab_pushfstring(L, "(%s)", dirp));
}

/*
 * Meta-methods.
 */

static int
DIR_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_dir_type));
}

static int
DIR_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_dir_type));
}

static int
DIR_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_dir_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t dir_methods[] = {
    LUAB_FUNC("dirp",           DIR_dirp),
    LUAB_FUNC("get_table",      DIR_get_table),
    LUAB_FUNC("dump",           DIR_dump),
    LUAB_FUNC("__gc",           DIR_gc),
    LUAB_FUNC("__len",          DIR_len),
    LUAB_FUNC("__tostring",     DIR_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
dir_create(lua_State *L, void *arg)
{
    return (luab_new_dir(L, arg));
}

static void
dir_init(void *ud, void *arg)
{
    luab_udata_init(&luab_dir_type, ud, arg);
}

static void *
dir_udata(lua_State *L, int narg)
{
    return (luab_to_dir(L, narg));
}

luab_module_t luab_dir_type = {
    .m_cookie   = LUAB_DIR_TYPE_ID,
    .m_name     = LUAB_DIR_TYPE,
    .m_vec      = dir_methods,
    .m_create   = dir_create,
    .m_init     = dir_init,
    .m_get      = dir_udata,
    .m_sz       = sizeof(luab_dir_t),
};
