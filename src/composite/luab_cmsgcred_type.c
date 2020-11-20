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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#if __BSD_VISIBLE
extern luab_module_t luab_cmsgcred_type;

/*
 * Interface against
 *
 *  struct cmsgcred {
 *      pid_t   cmcred_pid;
 *      uid_t   cmcred_uid;
 *      uid_t   cmcred_euid;
 *      gid_t   cmcred_gid;
 *      short   cmcred_ngroups;
 *      gid_t   cmcred_groups[CMGROUP_MAX];
 *  };
 */

typedef struct luab_cmsgcred {
    luab_udata_t        ud_softc;
    struct cmsgcred     ud_cmcred;
} luab_cmsgcred_t;

#define luab_new_cmsgcred(L, arg) \
    ((luab_cmsgcred_t *)luab_newudata(L, &luab_cmsgcred_type, (arg)))
#define luab_to_cmsgcred(L, narg) \
    (luab_toldata((L), (narg), &luab_cmsgcred_type, \
        struct cmsgcred *, sizeof(struct cmsgcred)))

#define LUAB_CMSGCRED_TYPE_ID    1601557495
#define LUAB_CMSGCRED_TYPE    "CMSGCRED*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(CMSGCRED)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              cmcred_pid      = (LUA_TNUMBER),
 *              cmcred_uid      = (LUA_TNUMBER),
 *              cmcred_euid     = (LUA_TNUMBER),
 *              cmcred_gid      = (LUA_TNUMBER),
 *              cmcred_ngroups  = (LUA_TNUMBER),
 *              cmcred_groups   = (LUA_TSTRING),
 *          }
 *
 * @usage t = cmsgcred:get()
 */
static int
CMSGCRED_get(lua_State *L)
{
    struct cmsgcred *cmcred;

    (void)luab_core_checkmaxargs(L, 1);

    cmcred = luab_udata(L, 1, &luab_cmsgcred_type, struct cmsgcred *);

    lua_newtable(L);
    luab_setinteger(L, -2, "cmcred_pid",        cmcred->cmcred_pid);
    luab_setinteger(L, -2, "cmcred_uid",        cmcred->cmcred_uid);
    luab_setinteger(L, -2, "cmcred_euid",       cmcred->cmcred_euid);
    luab_setinteger(L, -2, "cmcred_gid",        cmcred->cmcred_gid);
    luab_setinteger(L, -2, "cmcred_ngroups",    cmcred->cmcred_ngroups);

        /* XXX (LUA_TTABLE) */
    luab_setldata(L, -2, "cmcred_groups", cmcred->cmcred_groups, CMGROUP_MAX);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate cmsgcred{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = cmsgcred:dump()
 */
static int
CMSGCRED_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_cmsgcred_type, sizeof(struct cmsgcred)));
}

/*
 * Access functions, immutables.
 */

/***
 * Get PID of sending process.
 *
 * @function cmcred_pid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = cmsgcred:cmcred_pid()
 */
static int
CMSGCRED_cmcred_pid(lua_State *L)
{
    struct cmsgcred *cmcred;
    pid_t data;

    (void)luab_core_checkmaxargs(L, 1);

    cmcred = luab_udata(L, 1, &luab_cmsgcred_type, struct cmsgcred *);
    data = cmcred->cmcred_pid;

    return (luab_pushxinteger(L, data));
}

/***
 * Get real UID of sending process.
 *
 * @function cmcred_uid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = cmsgcred:cmcred_uid()
 */
static int
CMSGCRED_cmcred_uid(lua_State *L)
{
    struct cmsgcred *cmcred;
    uid_t data;

    (void)luab_core_checkmaxargs(L, 1);

    cmcred = luab_udata(L, 1, &luab_cmsgcred_type, struct cmsgcred *);
    data = cmcred->cmcred_uid;

    return (luab_pushxinteger(L, data));
}

/***
 * Get effective UID of sending process.
 *
 * @function cmcred_euid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = cmsgcred:cmcred_euid()
 */
static int
CMSGCRED_cmcred_euid(lua_State *L)
{
    struct cmsgcred *cmcred;
    uid_t data;

    (void)luab_core_checkmaxargs(L, 1);

    cmcred = luab_udata(L, 1, &luab_cmsgcred_type, struct cmsgcred *);
    data = cmcred->cmcred_euid;

    return (luab_pushxinteger(L, data));
}

/***
 * Get real GID of sending process.
 *
 * @function cmcred_gid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = cmsgcred:cmcred_gid()
 */
static int
CMSGCRED_cmcred_gid(lua_State *L)
{
    struct cmsgcred *cmcred;
    gid_t data;

    (void)luab_core_checkmaxargs(L, 1);

    cmcred = luab_udata(L, 1, &luab_cmsgcred_type, struct cmsgcred *);
    data = cmcred->cmcred_gid;

    return (luab_pushxinteger(L, data));
}

/***
 * Get number or groups.
 *
 * @function cmcred_ngroups
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = cmsgcred:cmcred_ngroups()
 */
static int
CMSGCRED_cmcred_ngroups(lua_State *L)
{
    struct cmsgcred *cmcred;
    short data;

    (void)luab_core_checkmaxargs(L, 1);

    cmcred = luab_udata(L, 1, &luab_cmsgcred_type, struct cmsgcred *);
    data = cmcred->cmcred_ngroups;

    return (luab_pushxinteger(L, data));
}

/***
 * Get groups.
 *
 * @function cmcred_groups
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = cmsgcred:cmcred_groups()
 */
static int
CMSGCRED_cmcred_groups(lua_State *L)
{
    struct cmsgcred *cmcred;
    void *data;
    size_t len;

    (void)luab_core_checkmaxargs(L, 1);

    cmcred = luab_udata(L, 1, &luab_cmsgcred_type, struct cmsgcred *);
    data = cmcred->cmcred_groups;
    len = (luab_env_ngroups_max * sizeof(gid_t));
    
        /* XXX (LUA_TTABLE) */

    return (luab_pushldata(L, data, len));
}

/*
 * Metamethods
 */

static int
CMSGCRED_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_cmsgcred_type));
}

static int
CMSGCRED_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_cmsgcred_type));
}

static int
CMSGCRED_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_cmsgcred_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t cmsgcred_methods[] = {
    LUAB_FUNC("get",                    CMSGCRED_get),
    LUAB_FUNC("get_cmcred_pid",         CMSGCRED_cmcred_pid),
    LUAB_FUNC("get_cmcred_uid",         CMSGCRED_cmcred_uid),
    LUAB_FUNC("get_cmcred_euid",        CMSGCRED_cmcred_euid),
    LUAB_FUNC("get_cmcred_gid",         CMSGCRED_cmcred_gid),
    LUAB_FUNC("get_cmcred_ngroups",     CMSGCRED_cmcred_ngroups),
    LUAB_FUNC("get_cmcred_groups",      CMSGCRED_cmcred_groups),
    LUAB_FUNC("dump",                   CMSGCRED_dump),
    LUAB_FUNC("__gc",                   CMSGCRED_gc),
    LUAB_FUNC("__len",                  CMSGCRED_len),
    LUAB_FUNC("__tostring",             CMSGCRED_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
cmsgcred_create(lua_State *L, void *arg)
{
    return (luab_new_cmsgcred(L, arg));
}

static void
cmsgcred_init(void *ud, void *arg)
{
    luab_udata_init(&luab_cmsgcred_type, ud, arg);
}

static void *
cmsgcred_udata(lua_State *L, int narg)
{
    return (luab_to_cmsgcred(L, narg));
}

luab_module_t luab_cmsgcred_type = {
    .m_cookie   = LUAB_CMSGCRED_TYPE_ID,
    .m_name     = LUAB_CMSGCRED_TYPE,
    .m_vec      = cmsgcred_methods,
    .m_create   = cmsgcred_create,
    .m_init     = cmsgcred_init,
    .m_get      = cmsgcred_udata,
    .m_sz       = sizeof(luab_cmsgcred_t),
};
#endif /* __BSD_VISBLE */