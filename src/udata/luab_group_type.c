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

#include <grp.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

/*
 * XXX
 *  Extensions on luab_table(3) API are neccessary.
 */

extern luab_module_t group_type;

/*
 * Interface against
 *
 *  struct group {
 *      char    *gr_name;
 *      char    *gr_passwd;
 *      gid_t   gr_gid;
 *      char    **gr_mem;
 *  };
 */

typedef struct luab_group {
    luab_udata_t    ud_softc;
    struct group    ud_grp;
} luab_group_t;

#define luab_new_group(L, arg) \
    ((luab_group_t *)luab_newudata(L, &group_type, (arg)))
#define luab_to_group(L, narg) \
    (luab_toldata((L), (narg), &group_type, \
        struct group *, sizeof(struct group)))

#define LUAB_GROUP_TYPE_ID    1604324396
#define LUAB_GROUP_TYPE    "GROUP*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(GROUP)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              gr_name     = (LUA_TSTRING),
 *              gr_passwd    = (LUA_TSTRING),
 *              gr_gid      = (LUA_TNUMBER),
 *              gr_mem      = (LUA_TTABLE),
 *          }
 *
 * @usage t = group:get()
 */
static int
GROUP_get(lua_State *L)
{
    struct group *grp;

    (void)luab_core_checkmaxargs(L, 1);

    grp = luab_udata(L, 1, &group_type, struct group *);

    lua_newtable(L);
    luab_setstring(L, -2, "gr_name",    grp->gr_name);
    luab_setstring(L, -2, "gr_passwd",   grp->gr_passwd);
    luab_setinteger(L, -2, "gr_gid",    grp->gr_gid);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate group{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = group:dump()
 */
static int
GROUP_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &group_type, sizeof(struct group)));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get group name.
 *
 * @function gr_name
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = group:gr_name()
 */
static int
GROUP_gr_name(lua_State *L)
{
    struct group *grp;
    char *data;

    (void)luab_core_checkmaxargs(L, 2);

    grp = luab_udata(L, 1, &group_type, struct group *);
    data = grp->gr_name;

    return (luab_pushstring(L, data));
}

/***
 * Get group password.
 *
 * @function gr_passwd
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = group:gr_passwd()
 */
static int
GROUP_gr_passwd(lua_State *L)
{
    struct group *grp;
    char *data;

    (void)luab_core_checkmaxargs(L, 2);

    grp = luab_udata(L, 1, &group_type, struct group *);
    data = grp->gr_passwd;

    return (luab_pushstring(L, data));
}

/***
 * Get group ID.
 *
 * @function gr_gid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = group:gr_gid()
 */
static int
GROUP_gr_gid(lua_State *L)
{
    struct group *grp;
    gid_t data;

    (void)luab_core_checkmaxargs(L, 2);

    grp = luab_udata(L, 1, &group_type, struct group *);
    data = grp->gr_gid;

    return (luab_pusherr(L, data));
}

#if 0
/***
 * Get group members.
 *
 * @function gr_mem
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = group:gr_mem()
 */
static int
GROUP_gr_mem(lua_State *L)
{
    struct group *grp;
    luab_table_t *tbl;

    (void)luab_core_checkmaxargs(L, 2);

    grp = luab_udata(L, 1, &group_type, struct group *);

    return (luab_pusherr(L, data));
}
#endif

/*
 * Meta-methods.
 */

static int
GROUP_gc(lua_State *L)
{
    struct group *grp;
    
    (void)luab_core_checkmaxargs(L, 1);

    grp = luab_udata(L, 1, &group_type, struct group *);

    luab_core_free(grp->gr_name, 0);
    luab_core_free(grp->gr_passwd, 0);
    luab_core_free(grp->gr_mem, 0);

    return (luab_core_gc(L, 1, &group_type));
}

static int
GROUP_len(lua_State *L)
{
    return (luab_core_len(L, 2, &group_type));
}

static int
GROUP_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &group_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t group_methods[] = {
    LUAB_FUNC("gr_name",        GROUP_gr_name),
    LUAB_FUNC("gr_passwd",      GROUP_gr_passwd),
    LUAB_FUNC("gr_gid",         GROUP_gr_gid),
#if 0
    LUAB_FUNC("gr_mem",         GROUP_gr_mem),
#endif
    LUAB_FUNC("get",            GROUP_get),
    LUAB_FUNC("dump",           GROUP_dump),
    LUAB_FUNC("__gc",           GROUP_gc),
    LUAB_FUNC("__len",          GROUP_len),
    LUAB_FUNC("__tostring",     GROUP_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
group_create(lua_State *L, void *arg)
{
    return (luab_new_group(L, arg));
}

static void
group_init(void *ud, void *arg)
{
    luab_udata_init(&group_type, ud, arg);
}

static void *
group_udata(lua_State *L, int narg)
{
    return (luab_to_group(L, narg));
}

luab_module_t group_type = {
    .m_cookie   = LUAB_GROUP_TYPE_ID,
    .m_name     = LUAB_GROUP_TYPE,
    .m_vec      = group_methods,
    .m_create   = group_create,
    .m_init     = group_init,
    .m_get      = group_udata,
    .m_sz       = sizeof(luab_group_t),
};
