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

/*
 * Subr.
 */

static int
luab_table_pushfsid(lua_State *L, int narg, const char *k, int32_t *vec)
{
    size_t i, j;

    if (vec != NULL) {
        luab_table_init(L, 1);

        /*
         * XXX
         *  Constraints.
         */

        for (i = 0, j = 1; i < 2; i++, j++)
            luab_rawsetinteger(L, narg, j, vec[i]);

        /*
         * Set field k and/or push on top of Lua stack.
         *
         * XXX DRY
         */
        if (k != NULL)
            lua_setfield(L, narg, k);
        else {
            if (narg < 0)
                lua_pushvalue(L, narg + 1);
            else
                lua_pushvalue(L, narg);
        }
    } else
        errno = ERANGE;

    return (luab_table_pusherr(L, errno, 1));
}

static void
fsid_fillxtable(lua_State *L, int narg, void *arg)
{
    struct fsid *fsid;

    if ((fsid = (struct fsid *)arg) != NULL) {

        (void)luab_table_pushfsid(L, narg, "val", fsid->val);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(FSID)) into (LUA_TTABLE).
 *
 * @function get_table
 *
  * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              val     = (LUA_T{NIL,TABLE}),
 *          }
 *
 * @usage t [, err, msg ] = fsid:get_table()
 */
static int
FSID_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FSID, TYPE, __func__);

    xtp.xtp_fill = fsid_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
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
    luab_module_t *m;
    m = luab_xmod(FSID, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
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
 * @usage x [, err, msg ] = fsid:val()
 */
static int
FSID_val(lua_State *L)
{
    luab_module_t *m;
    fsid_t *fsid;
    int32_t *dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FSID, TYPE, __func__);
    fsid = luab_udata(L, 1, m, fsid_t *);
    dp = fsid->val;

    return (luab_table_pushfsid(L, -2, NULL, dp));
}

/*
 * Meta-methods.
 */

static int
FSID_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FSID, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
FSID_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FSID, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
FSID_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FSID, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t fsid_methods[] = {
    LUAB_FUNC("val",            FSID_val),
    LUAB_FUNC("get_table",      FSID_get_table),
    LUAB_FUNC("dump",           FSID_dump),
    LUAB_FUNC("__gc",           FSID_gc),
    LUAB_FUNC("__len",          FSID_len),
    LUAB_FUNC("__tostring",     FSID_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
fsid_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(FSID, TYPE, __func__);
    return (luab_newudata(L, m, arg));
}

static void
fsid_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(FSID, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
fsid_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(FSID, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
fsid_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    fsid_t *x, *y;
    size_t i, j;

    m = luab_xmod(FSID, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (fsid_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, fsid_t *);
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
fsid_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    fsid_t *x;
    size_t i, j, k;

    m = luab_xmod(FSID, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (fsid_t *)tbl->tbl_vec) != NULL) &&
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
fsid_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(FSID, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_fsid_type = {
    .m_id           = LUAB_FSID_TYPE_ID,
    .m_name         = LUAB_FSID_TYPE,
    .m_vec          = fsid_methods,
    .m_create       = fsid_create,
    .m_init         = fsid_init,
    .m_get          = fsid_udata,
    .m_get_tbl      = fsid_checktable,
    .m_set_tbl      = fsid_pushtable,
    .m_alloc_tbl    = fsid_alloctable,
    .m_len          = sizeof(luab_fsid_t),
    .m_sz           = sizeof(fsid_t),
};
