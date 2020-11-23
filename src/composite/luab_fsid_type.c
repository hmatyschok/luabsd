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

extern luab_module_t luab_fsid_type;

/*
 * Interface against
 *
 *  typedef struct fsid { int32_t val[2]; } fsid_t;
 */

typedef struct luab_fsid {
    luab_udata_t    ud_softc;
    fsid_t          ud_fsid;
} luab_fsid_t;

#define luab_new_fsid(L, arg) \
    ((luab_fsid_t *)luab_newudata(L, &luab_fsid_type, (arg)))
#define luab_to_fsid(L, narg) \
    (luab_toldata((L), (narg), &luab_fsid_type, \
        fsid_t *, sizeof(fsid_t)))

/*
 * Subr.
 */

static int
fsid_pushtable_internal(lua_State *L, int narg, const char *k, int32_t *vec)
{
    int up_call, status;
    size_t m, n;

    if (vec != NULL) {
        luab_table_init(L, 1);

        for (m = 0, n = 1; m < 2; m++, n++)
            luab_rawsetinteger(L, narg, n, vec[m]);

        /* set field k and/or push on top of Lua stack */
        if (k != NULL)
            lua_setfield(L, narg, k);
        else {
            if (narg < 0)
                lua_pushvalue(L, narg + 1);
            else
                lua_pushvalue(L, narg);
        }
        up_call = 0;
    } else
        up_call = ENOENT;

    if ((errno = up_call) != 0)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(FSID)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              val     = (LUA_TTABLE),
 *          }
 *
 * @usage t = fsid:get()
 */
static int
FSID_get(lua_State *L)
{
    fsid_t *fsid;

    (void)luab_core_checkmaxargs(L, 1);

    fsid = luab_udata(L, 1, &luab_fsid_type, fsid_t *);

    lua_newtable(L);
    (void)fsid_pushtable_internal(L, -2, "val", fsid->val);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate fsid{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = fsid:dump()
 */
static int
FSID_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_fsid_type, sizeof(struct fsid)));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get file system ID by (LUA_TTABLE).
 *
 * @function val
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = fsid:val()
 */
static int
FSID_val(lua_State *L)
{
    fsid_t *fsid;
    int32_t *dp;

    (void)luab_core_checkmaxargs(L, 1);

    fsid = luab_udata(L, 1, &luab_fsid_type, fsid_t *);
    dp = fsid->val;

    return (fsid_pushtable_internal(L, -2, NULL, dp));
}

/*
 * Meta-methods.
 */

static int
FSID_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_fsid_type));
}

static int
FSID_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_fsid_type));
}

static int
FSID_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_fsid_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t fsid_methods[] = {
    LUAB_FUNC("val",            FSID_val),
    LUAB_FUNC("get",            FSID_get),
    LUAB_FUNC("dump",           FSID_dump),
    LUAB_FUNC("__gc",           FSID_gc),
    LUAB_FUNC("__len",          FSID_len),
    LUAB_FUNC("__tostring",     FSID_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
fsid_create(lua_State *L, void *arg)
{
    return (luab_new_fsid(L, arg));
}

static void
fsid_init(void *ud, void *arg)
{
    luab_udata_init(&luab_fsid_type, ud, arg);
}

static void *
fsid_udata(lua_State *L, int narg)
{
    return (luab_to_fsid(L, narg));
}

static luab_table_t *
fsid_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    fsid_t *x, *y;
    size_t m, n, sz;

    sz = sizeof(fsid_t);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (fsid_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_fsid_type, fsid_t *);
                        (void)memmove(&(x[m]), y, sz);
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        }
    }
    return (tbl);
}

static void
fsid_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    fsid_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (fsid_t *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_fsid_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

luab_module_t luab_fsid_type = {
    .m_cookie   = LUAB_FSID_TYPE_ID,
    .m_name     = LUAB_FSID_TYPE,
    .m_vec      = fsid_methods,
    .m_create   = fsid_create,
    .m_init     = fsid_init,
    .m_get      = fsid_udata,
    .m_get_tbl  = fsid_checktable,
    .m_set_tbl  = fsid_pushtable,
    .m_sz       = sizeof(luab_fsid_t),
};
