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

#include <sys/mount.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_xvfsconf_type;

/*
 * Interface against
 *
 *  struct vfsconf {
 *      u_int   vfc_version;
 *      char    vfc_name[MFSNAMELEN];
 *      struct  vfsops *vfc_vfsops;
 *      int vfc_typenum;
 *      int vfc_refcount;
 *      int vfc_flags;
 *      int vfc_prison_flag;
 *      struct  vfsoptdecl *vfc_opts;
 *      TAILQ_ENTRY(vfsconf) vfc_list;
 *  };
 *
 * by
 *
 *  struct xvfsconf {
 *      struct  vfsops *vfc_vfsops;
 *      char    vfc_name[MFSNAMELEN];
 *      int vfc_typenum;
 *      int vfc_refcount;
 *      int vfc_flags;
 *      struct  vfsconf *vfc_next;
 *  };
 */

typedef struct luab_xvfsconf {
    luab_udata_t        ud_softc;
    struct xvfsconf     ud_vfc;
} luab_xvfsconf_t;

#define luab_new_xvfsconf(L, arg) \
    ((luab_xvfsconf_t *)luab_newudata(L, &luab_xvfsconf_type, (arg)))
#define luab_to_xvfsconf(L, narg) \
    (luab_toldata((L), (narg), &luab_xvfsconf_type, \
        struct xvfsconf *, sizeof(struct xvfsconf)))

#define LUAB_XVFSCONF_TYPE_ID    1604736441
#define LUAB_XVFSCONF_TYPE    "XVFSCONF*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(XVFSCONF)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              vfc_vfsops      = (LUA_TSTRING),
 *              vfc_name        = (LUA_TSTRING),
 *              vfc_typenum     = (LUA_TNUMBER),
 *              vfc_refcount    = (LUA_TNUMBER),
 *              vfc_flags       = (LUA_TNUMBER),
 *          }
 *
 * @usage t = xvfsconf:get()
 */
static int
XVFSCONF_get(lua_State *L)
{
    struct xvfsconf *vfc;

    (void)luab_core_checkmaxargs(L, 1);

    vfc = luab_udata(L, 1, &luab_xvfsconf_type, struct xvfsconf *);

    lua_newtable(L);
    luab_setfstring(L, -2, "vfc_vfsops", "(%p)",    vfc->vfc_vfsops);
    luab_setstring(L, -2, "vfc_name",               vfc->vfc_name);
    luab_setinteger(L, -2, "vfc_typenum",           vfc->vfc_typenum);
    luab_setinteger(L, -2, "vfc_refcount",          vfc->vfc_refcount);
    luab_setinteger(L, -2, "vfc_flags",             vfc->vfc_flags);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate xvfsconf{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = xvfsconf:dump()
 */
static int
XVFSCONF_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_xvfsconf_type, sizeof(struct xvfsconf)));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get base address of filesystem operations vector.
 *
 * @function vfc_vfsops
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = xvfsconf:vfc_vfsops()
 */
static int
XVFSCONF_vfc_vfsops(lua_State *L)
{
    struct xvfsconf *vfc;
    void *dp;

    (void)luab_core_checkmaxargs(L, 1);

    vfc = luab_udata(L, 1, &luab_xvfsconf_type, struct xvfsconf *);
    dp = (void *)(vfc->vfc_vfsops);

    return (luab_pushfstring(L, "(%s)", dp));
}

/***
 * Get filesystem type name.
 *
 * @function vfc_name
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = xvfsconf:vfc_name()
 */
static int
XVFSCONF_vfc_name(lua_State *L)
{
    struct xvfsconf *vfc;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    vfc = luab_udata(L, 1, &luab_xvfsconf_type, struct xvfsconf *);
    dp = vfc->vfc_name;

    return (luab_pushstring(L, dp));
}

/***
 * Get historic filsystem type number.
 *
 * @function vfc_typenum
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = xvfsconf:vfc_typenum()
 */
static int
XVFSCONF_vfc_typenum(lua_State *L)
{
    struct xvfsconf *vfc;
    int data;

    (void)luab_core_checkmaxargs(L, 1);

    vfc = luab_udata(L, 1, &luab_xvfsconf_type, struct xvfsconf *);
    data = vfc->vfc_typenum;

    return (luab_pusherr(L, data));
}

/***
 * Get get the number mounted of this type.
 *
 * @function vfc_refcount
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = xvfsconf:vfc_refcount()
 */
static int
XVFSCONF_vfc_refcount(lua_State *L)
{
    struct xvfsconf *vfc;
    int data;

    (void)luab_core_checkmaxargs(L, 1);

    vfc = luab_udata(L, 1, &luab_xvfsconf_type, struct xvfsconf *);
    data = vfc->vfc_refcount;

    return (luab_pusherr(L, data));
}

/***
 * Get get the value of permament flags.
 *
 * @function vfc_flags
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = xvfsconf:vfc_flags()
 */
static int
XVFSCONF_vfc_flags(lua_State *L)
{
    struct xvfsconf *vfc;
    int data;

    (void)luab_core_checkmaxargs(L, 1);

    vfc = luab_udata(L, 1, &luab_xvfsconf_type, struct xvfsconf *);
    data = vfc->vfc_flags;

    return (luab_pusherr(L, data));
}

/*
 * Meta-methods.
 */

static int
XVFSCONF_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_xvfsconf_type));
}

static int
XVFSCONF_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_xvfsconf_type));
}

static int
XVFSCONF_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_xvfsconf_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t xvfsconf_methods[] = {
    LUAB_FUNC("vfc_vfsops",     XVFSCONF_vfc_vfsops),
    LUAB_FUNC("vfc_name",       XVFSCONF_vfc_name),
    LUAB_FUNC("vfc_typenum",    XVFSCONF_vfc_typenum),
    LUAB_FUNC("vfc_refcount",   XVFSCONF_vfc_refcount),
    LUAB_FUNC("vfc_flags",      XVFSCONF_vfc_flags),
    LUAB_FUNC("get",            XVFSCONF_get),
    LUAB_FUNC("dump",           XVFSCONF_dump),
    LUAB_FUNC("__gc",           XVFSCONF_gc),
    LUAB_FUNC("__len",          XVFSCONF_len),
    LUAB_FUNC("__tostring",     XVFSCONF_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
xvfsconf_create(lua_State *L, void *arg)
{
    return (luab_new_xvfsconf(L, arg));
}

static void
xvfsconf_init(void *ud, void *arg)
{
    luab_udata_init(&luab_xvfsconf_type, ud, arg);
}

static void *
xvfsconf_udata(lua_State *L, int narg)
{
    return (luab_to_xvfsconf(L, narg));
}

luab_module_t luab_xvfsconf_type = {
    .m_cookie   = LUAB_XVFSCONF_TYPE_ID,
    .m_name     = LUAB_XVFSCONF_TYPE,
    .m_vec      = xvfsconf_methods,
    .m_create   = xvfsconf_create,
    .m_init     = xvfsconf_init,
    .m_get      = xvfsconf_udata,
    .m_sz       = sizeof(luab_xvfsconf_t),
};