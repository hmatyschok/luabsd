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

extern luab_module_t luab_fhandle_type;

/*
 * Interface against
 *
 *  struct fhandle {
 *      fsid_t  fh_fsid;
 *      struct  fid fh_fid;
 *  };
 *  typedef struct fhandle  fhandle_t;
 */

typedef struct luab_fhandle {
    luab_udata_t    ud_softc;
    fhandle_t       ud_fh;
} luab_fhandle_t;

#define luab_new_fhandle(L, arg) \
    ((luab_fhandle_t *)luab_newudata(L, &luab_fhandle_type, (arg)))
#define luab_to_fhandle(L, narg) \
    (luab_toldata((L), (narg), &luab_fhandle_type, \
        fhandle_t *, sizeof(fhandle_t)))

#define LUAB_FHANDLE_TYPE_ID    1604445207
#define LUAB_FHANDLE_TYPE    "FHANDLE*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(FHANDLE)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              fh_fsid     = (LUA_TUSERDATA(FSID)),
 *              fh_fid      = (LUA_TUSERDATA(FID)),
 *          }
 *
 * @usage t = fhandle:get()
 */
static int
FHANDLE_get(lua_State *L)
{
    fhandle_t *fh;

    (void)luab_core_checkmaxargs(L, 1);

    fh = luab_udata(L, 1, &luab_fhandle_type, fhandle_t *);

    lua_newtable(L);
    luab_setudata(L, -2, luab_xtype(FSID), "fh_fsid",  &(fh->fh_fsid));
    luab_setudata(L, -2, luab_xtype(FID), "fh_fid",    &(fh->fh_fid));
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate fhandle{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = fhandle:dump()
 */
static int
FHANDLE_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_fhandle_type, sizeof(fhandle_t)));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get filesystem ID of mount point.
 *
 * @function fh_fsid
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = fhandle:fh_fsid()
 */
static int
FHANDLE_fh_fsid(lua_State *L)
{
    fhandle_t *fh;
    fsid_t *fsid;

    (void)luab_core_checkmaxargs(L, 1);

    fh = luab_udata(L, 1, &luab_fhandle_type, fhandle_t *);
    fsid = &(fh->fh_fsid);

    return (luab_pushudata(L, luab_xtype(FSID), fsid));
}

/***
 * Get filesystem specific ID.
 *
 * @function fh_fid
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = fhandle:fh_fid()
 */
static int
FHANDLE_fh_fid(lua_State *L)
{
    fhandle_t *fh;
    struct fid *fid;

    (void)luab_core_checkmaxargs(L, 1);

    fh = luab_udata(L, 1, &luab_fhandle_type, fhandle_t *);
    fid = &(fh->fh_fid);

    return (luab_pushudata(L, luab_xtype(FID), fid));
}

/*
 * Meta-methods.
 */

static int
FHANDLE_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_fhandle_type));
}

static int
FHANDLE_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_fhandle_type));
}

static int
FHANDLE_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_fhandle_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t fhandle_methods[] = {
    LUAB_FUNC("fh_fsid",        FHANDLE_fh_fsid),
    LUAB_FUNC("fh_fid",         FHANDLE_fh_fid),
    LUAB_FUNC("get",            FHANDLE_get),
    LUAB_FUNC("dump",           FHANDLE_dump),
    LUAB_FUNC("__gc",           FHANDLE_gc),
    LUAB_FUNC("__len",          FHANDLE_len),
    LUAB_FUNC("__tostring",     FHANDLE_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
fhandle_create(lua_State *L, void *arg)
{
    return (luab_new_fhandle(L, arg));
}

static void
fhandle_init(void *ud, void *arg)
{
    luab_udata_init(&luab_fhandle_type, ud, arg);
}

static void *
fhandle_udata(lua_State *L, int narg)
{
    return (luab_to_fhandle(L, narg));
}

luab_module_t luab_fhandle_type = {
    .m_cookie   = LUAB_FHANDLE_TYPE_ID,
    .m_name     = LUAB_FHANDLE_TYPE,
    .m_vec      = fhandle_methods,
    .m_create   = fhandle_create,
    .m_init     = fhandle_init,
    .m_get      = fhandle_udata,
    .m_sz       = sizeof(luab_fhandle_t),
};