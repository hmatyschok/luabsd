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
    luab_setudata(L, -2, luab_xmod(FSID, TYPE, __func__), "fh_fsid",  &(fh->fh_fsid));
    luab_setudata(L, -2, luab_xmod(FID, TYPE, __func__), "fh_fid",    &(fh->fh_fid));
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

    return (luab_pushudata(L, luab_xmod(FSID, TYPE, __func__), fsid));
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

    return (luab_pushudata(L, luab_xmod(FID, TYPE, __func__), fid));
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

static luab_table_t *
fhandle_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    fhandle_t *x, *y;
    size_t m, n, sz;

    sz = sizeof(fhandle_t);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (fhandle_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_fhandle_type, fhandle_t *);
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
fhandle_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    fhandle_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (fhandle_t *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_fhandle_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

luab_module_t luab_fhandle_type = {
    .m_cookie   = LUAB_FHANDLE_TYPE_ID,
    .m_name     = LUAB_FHANDLE_TYPE,
    .m_vec      = fhandle_methods,
    .m_create   = fhandle_create,
    .m_init     = fhandle_init,
    .m_get      = fhandle_udata,
    .m_get_tbl  = fhandle_checktable,
    .m_set_tbl  = fhandle_pushtable,
    .m_sz       = sizeof(luab_fhandle_t),
};
