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

/*
 * Subr.
 */

static void
fid_fillxtable(lua_State *L, int narg, void *arg)
{
    struct fid *fid;

    if ((fid = (struct fid *)arg) != NULL) {

        luab_setinteger(L, narg, "fid_len",   fid->fid_len);
        luab_setinteger(L, narg, "fid_data0", fid->fid_data0);
        luab_setldata(L, narg, "fid_data", fid->fid_data, MAXFIDSZ);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(FID)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              fid_len     = (LUA_TNUMBER),
 *              fid_data0   = (LUA_TNUMBER),
 *              fid_data    = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ]= fid:get_table()
 */
static int
FID_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FID, TYPE, __func__);

    xtp.xtp_fill = fid_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
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
    luab_module_t *m;
    m = luab_xmod(FID, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
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
 * @usage x [, err, msg ] = fid:fid_len()
 */
static int
FID_fid_len(lua_State *L)
{
    luab_module_t *m;
    struct fid *fid;
    u_short x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FID, TYPE, __func__);
    fid = luab_udata(L, 1, m, struct fid *);
    x = fid->fid_len;

    return (luab_pushxinteger(L, x));
}

/***
 * Get force longword alignement.
 *
 * @function fid_data0
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = fid:fid_data0()
 */
static int
FID_fid_data0(lua_State *L)
{
    luab_module_t *m;
    struct fid *fid;
    u_short x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FID, TYPE, __func__);
    fid = luab_udata(L, 1, m, struct fid *);
    x = fid->fid_data0;

    return (luab_pushxinteger(L, x));
}

/***
 * Get variable data.
 *
 * @function fid_data
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = fid:fid_data()
 */
static int
FID_fid_data(lua_State *L)
{
    luab_module_t *m;
    struct fid *fid;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FID, TYPE, __func__);
    fid = luab_udata(L, 1, m, struct fid *);
    dp = fid->fid_data;

    return (luab_pushldata(L, dp, MAXFIDSZ));   /* XXX */
}

/*
 * Meta-methods.
 */

static int
FID_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FID, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
FID_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FID, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
FID_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FID, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t fid_methods[] = {
    LUAB_FUNC("fid_len",        FID_fid_len),
    LUAB_FUNC("fid_data0",      FID_fid_data0),
    LUAB_FUNC("fid_data",       FID_fid_data),
    LUAB_FUNC("get_table",      FID_get_table),
    LUAB_FUNC("dump",           FID_dump),
    LUAB_FUNC("__gc",           FID_gc),
    LUAB_FUNC("__len",          FID_len),
    LUAB_FUNC("__tostring",     FID_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
fid_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(FID, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
fid_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(FID, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
fid_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(FID, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
fid_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct fid *x, *y;
    size_t i, j;

    m = luab_xmod(FID, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct fid *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct fid *);
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
fid_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct fid *x;
    size_t i, j, k;

    m = luab_xmod(FID, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct fid *)tbl->tbl_vec) != NULL) &&
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
fid_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(FID, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_fid_type = {
    .m_id           = LUAB_FID_TYPE_ID,
    .m_name         = LUAB_FID_TYPE,
    .m_vec          = fid_methods,
    .m_create       = fid_create,
    .m_init         = fid_init,
    .m_get          = fid_udata,
    .m_get_tbl      = fid_checktable,
    .m_set_tbl      = fid_pushtable,
    .m_alloc_tbl    = fid_alloctable,
    .m_len          = sizeof(luab_fid_t),
    .m_sz           = sizeof(struct fid),
};
