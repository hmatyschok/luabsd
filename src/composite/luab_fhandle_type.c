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

/*
 * Subr.
 */

static void
fhandle_fillxtable(lua_State *L, int narg, void *arg)
{
    fhandle_t *fh;

    if ((fh = (fhandle_t *)arg) != NULL) {

        luab_setxdata(L, narg, luab_xmod(FSID, TYPE, __func__), "fh_fsid",  &(fh->fh_fsid));
        luab_setxdata(L, narg, luab_xmod(FID, TYPE, __func__), "fh_fid",    &(fh->fh_fid));
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(FHANDLE)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              fh_fsid     = (LUA_TUSERDATA(FSID)),
 *              fh_fid      = (LUA_TUSERDATA(FID)),
 *          }
 *
 * @usage t [, err, msg ] = fhandle:get_table()
 */
static int
FHANDLE_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FHANDLE, TYPE, __func__);

    xtp.xtp_fill = fhandle_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
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
    luab_module_t *m;
    m = luab_xmod(FHANDLE, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
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
 * @usage x [, err, msg ] = fhandle:fh_fsid()
 */
static int
FHANDLE_fh_fsid(lua_State *L)
{
    luab_module_t *m0, *m1;
    fhandle_t *fh;
    fsid_t *x;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(FHANDLE, TYPE, __func__);
    m1 = luab_xmod(FSID, TYPE, __func__);

    fh = luab_udata(L, 1, m0, fhandle_t *);
    x = &(fh->fh_fsid);

    return (luab_pushxdata(L, m1, x));
}

/***
 * Get filesystem specific ID.
 *
 * @function fh_fid
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = fhandle:fh_fid()
 */
static int
FHANDLE_fh_fid(lua_State *L)
{
    luab_module_t *m0, *m1;
    fhandle_t *fh;
    struct fid *fid;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(FHANDLE, TYPE, __func__);
    m1 = luab_xmod(FID, TYPE, __func__);

    fh = luab_udata(L, 1, m0, fhandle_t *);
    fid = &(fh->fh_fid);

    return (luab_pushxdata(L, m1, fid));
}

/*
 * Meta-methods.
 */

static int
FHANDLE_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FHANDLE, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
FHANDLE_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FHANDLE, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
FHANDLE_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FHANDLE, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t fhandle_methods[] = {
    LUAB_FUNC("fh_fsid",        FHANDLE_fh_fsid),
    LUAB_FUNC("fh_fid",         FHANDLE_fh_fid),
    LUAB_FUNC("get_table",      FHANDLE_get_table),
    LUAB_FUNC("dump",           FHANDLE_dump),
    LUAB_FUNC("__gc",           FHANDLE_gc),
    LUAB_FUNC("__len",          FHANDLE_len),
    LUAB_FUNC("__tostring",     FHANDLE_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
fhandle_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(FHANDLE, TYPE, __func__);
    return (luab_newudata(L, m, arg));
}

static void
fhandle_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(FHANDLE, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
fhandle_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(FHANDLE, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
fhandle_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    fhandle_t *x, *y;
    size_t i, j;

    m = luab_xmod(FHANDLE, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (fhandle_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, fhandle_t *);
                        (void)memmove(&(x[i]), y, m->m_sz);
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        } else
            errno = ERANGE;
    }
    return (tbl);
}

static void
fhandle_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    fhandle_t *x;
    size_t i, j, k;

    m = luab_xmod(FHANDLE, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (fhandle_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, new);

            for (i = 0, j = tbl->tbl_card, k = 1; i < j; i++, k++)
                luab_rawsetxdata(L, narg, m, k, &(x[i]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

static luab_table_t *
fhandle_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(FHANDLE, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_fhandle_type = {
    .m_id           = LUAB_FHANDLE_TYPE_ID,
    .m_name         = LUAB_FHANDLE_TYPE,
    .m_vec          = fhandle_methods,
    .m_create       = fhandle_create,
    .m_init         = fhandle_init,
    .m_get          = fhandle_udata,
    .m_get_tbl      = fhandle_checktable,
    .m_set_tbl      = fhandle_pushtable,
    .m_alloc_tbl    = fhandle_alloctable,
    .m_len          = sizeof(luab_fhandle_t),
    .m_sz           = sizeof(fhandle_t),
};
