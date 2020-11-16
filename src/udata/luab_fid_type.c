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

extern luab_module_t luab_fid_type;

/*
 * Interface against
 *
 *  struct fid {
 *      u_short     fid_len;
 *      u_short     fid_data0;
 *      char        fid_data[MAXFIDSZ];
 *  };
 */

typedef struct luab_fid {
    luab_udata_t    ud_softc;
    struct fid      ud_fid;
} luab_fid_t;

#define luab_new_fid(L, arg) \
    ((luab_fid_t *)luab_newudata(L, &luab_fid_type, (arg)))
#define luab_to_fid(L, narg) \
    (luab_toldata((L), (narg), &luab_fid_type, \
        struct fid *, sizeof(struct fid)))

#define LUAB_FID_TYPE_ID    1604423994
#define LUAB_FID_TYPE    "FID*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(FID)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              fid_len     = (LUA_TNUMBER),
 *              fid_data0   = (LUA_TNUMBER),
 *              fid_data    = (LUA_TSTRING),
 *          }
 *
 * @usage t = fid:get()
 */
static int
FID_get(lua_State *L)
{
    struct fid *fid;

    (void)luab_core_checkmaxargs(L, 1);

    fid = luab_udata(L, 1, &luab_fid_type, struct fid *);

    lua_newtable(L);

    luab_setinteger(L, -2, "fid_len",   fid->fid_len);
    luab_setinteger(L, -2, "fid_data0", fid->fid_data0);
    luab_setldata(L, -2, "fid_data", fid->fid_data, MAXFIDSZ);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate fid{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = fid:dump()
 */
static int
FID_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_fid_type, sizeof(struct fid)));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get length of data in bytes.
 *
 * @function fid_len
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = fid:fid_len()
 */
static int
FID_fid_len(lua_State *L)
{
    struct fid *fid;
    u_short data;

    (void)luab_core_checkmaxargs(L, 1);

    fid = luab_udata(L, 1, &luab_fid_type, struct fid *);
    data = fid->fid_len;

    return (luab_pushxinteger(L, data));
}

/***
 * Get force longword alignement.
 *
 * @function fid_data0
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = fid:fid_data0()
 */
static int
FID_fid_data0(lua_State *L)
{
    struct fid *fid;
    u_short data;

    (void)luab_core_checkmaxargs(L, 1);

    fid = luab_udata(L, 1, &luab_fid_type, struct fid *);
    data = fid->fid_data0;

    return (luab_pushxinteger(L, data));
}

/***
 * Get variable data.
 *
 * @function fid_data
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = fid:fid_data()
 */
static int
FID_fid_data(lua_State *L)
{
    struct fid *fid;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    fid = luab_udata(L, 1, &luab_fid_type, struct fid *);
    dp = fid->fid_data;

    return (luab_pushldata(L, dp, MAXFIDSZ));
}

/*
 * Meta-methods.
 */

static int
FID_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_fid_type));
}

static int
FID_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_fid_type));
}

static int
FID_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_fid_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t fid_methods[] = {
    LUAB_FUNC("fid_len",        FID_fid_len),
    LUAB_FUNC("fid_data0",      FID_fid_data0),
    LUAB_FUNC("fid_data",       FID_fid_data),
    LUAB_FUNC("get",            FID_get),
    LUAB_FUNC("dump",           FID_dump),
    LUAB_FUNC("__gc",           FID_gc),
    LUAB_FUNC("__len",          FID_len),
    LUAB_FUNC("__tostring",     FID_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
fid_create(lua_State *L, void *arg)
{
    return (luab_new_fid(L, arg));
}

static void
fid_init(void *ud, void *arg)
{
    luab_udata_init(&luab_fid_type, ud, arg);
}

static void *
fid_udata(lua_State *L, int narg)
{
    return (luab_to_fid(L, narg));
}

luab_module_t luab_fid_type = {
    .m_cookie   = LUAB_FID_TYPE_ID,
    .m_name     = LUAB_FID_TYPE,
    .m_vec      = fid_methods,
    .m_create   = fid_create,
    .m_init     = fid_init,
    .m_get      = fid_udata,
    .m_sz       = sizeof(luab_fid_t),
};
