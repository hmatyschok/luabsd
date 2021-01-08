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

/*
 * Subr.
 */

static void
xvfsconf_fillxtable(lua_State *L, int narg, void *arg)
{
    struct xvfsconf *vfc;

    if ((vfc = (struct xvfsconf *)arg) != NULL) {

        luab_setfstring(L, narg, "vfc_vfsops", "(%p)",    vfc->vfc_vfsops);
        luab_setstring(L, narg, "vfc_name",               vfc->vfc_name);
        luab_setinteger(L, narg, "vfc_typenum",           vfc->vfc_typenum);
        luab_setinteger(L, narg, "vfc_refcount",          vfc->vfc_refcount);
        luab_setinteger(L, narg, "vfc_flags",             vfc->vfc_flags);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(XVFSCONF)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              vfc_vfsops      = (LUA_T{NIL,STRING}),
 *              vfc_name        = (LUA_T{NIL,STRING}),
 *              vfc_typenum     = (LUA_TNUMBER),
 *              vfc_refcount    = (LUA_TNUMBER),
 *              vfc_flags       = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = xvfsconf:get_table()
 */
static int
XVFSCONF_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(XVFSCONF, TYPE, __func__);

    xtp.xtp_fill = xvfsconf_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
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
    luab_module_t *m;
    m = luab_xmod(XVFSCONF, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
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
 * @usage x [, err, msg ] = xvfsconf:vfc_vfsops()
 */
static int
XVFSCONF_vfc_vfsops(lua_State *L)
{
    luab_module_t *m;
    struct xvfsconf *vfc;
    void *v;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(XVFSCONF, TYPE, __func__);
    vfc = luab_udata(L, 1, m, struct xvfsconf *);
    v = (void *)(vfc->vfc_vfsops);

    return (luab_pushfstring(L, "(%s)", v));
}

/***
 * Get filesystem type name.
 *
 * @function vfc_name
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = xvfsconf:vfc_name()
 */
static int
XVFSCONF_vfc_name(lua_State *L)
{
    luab_module_t *m;
    struct xvfsconf *vfc;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(XVFSCONF, TYPE, __func__);
    vfc = luab_udata(L, 1, m, struct xvfsconf *);
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
 * @usage x [, err, msg ] = xvfsconf:vfc_typenum()
 */
static int
XVFSCONF_vfc_typenum(lua_State *L)
{
    luab_module_t *m;
    struct xvfsconf *vfc;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(XVFSCONF, TYPE, __func__);
    vfc = luab_udata(L, 1, m, struct xvfsconf *);
    x = vfc->vfc_typenum;

    return (luab_pushxinteger(L, x));
}

/***
 * Get get the number mounted of this type.
 *
 * @function vfc_refcount
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = xvfsconf:vfc_refcount()
 */
static int
XVFSCONF_vfc_refcount(lua_State *L)
{
    luab_module_t *m;
    struct xvfsconf *vfc;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(XVFSCONF, TYPE, __func__);
    vfc = luab_udata(L, 1, m, struct xvfsconf *);
    x = vfc->vfc_refcount;

    return (luab_pushxinteger(L, x));
}

/***
 * Get get the value of permament flags.
 *
 * @function vfc_flags
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = xvfsconf:vfc_flags()
 */
static int
XVFSCONF_vfc_flags(lua_State *L)
{
    luab_module_t *m;
    struct xvfsconf *vfc;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(XVFSCONF, TYPE, __func__);
    vfc = luab_udata(L, 1, m, struct xvfsconf *);
    x = vfc->vfc_flags;

    return (luab_pushxinteger(L, x));
}

/*
 * Meta-methods.
 */

static int
XVFSCONF_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(XVFSCONF, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
XVFSCONF_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(XVFSCONF, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
XVFSCONF_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(XVFSCONF, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
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
    LUAB_FUNC("get_table",      XVFSCONF_get_table),
    LUAB_FUNC("dump",           XVFSCONF_dump),
    LUAB_FUNC("__gc",           XVFSCONF_gc),
    LUAB_FUNC("__len",          XVFSCONF_len),
    LUAB_FUNC("__tostring",     XVFSCONF_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
xvfsconf_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(XVFSCONF, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
xvfsconf_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(XVFSCONF, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
xvfsconf_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(XVFSCONF, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
xvfsconf_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct xvfsconf *x, *y;
    size_t i, j;

    m = luab_xmod(XVFSCONF, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct xvfsconf *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct xvfsconf *);
                        (void)memmove(&(x[i]), y, m->m_sz);
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
xvfsconf_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct xvfsconf *x;
    size_t i, j, k;

    m = luab_xmod(XVFSCONF, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct xvfsconf *)tbl->tbl_vec) != NULL) &&
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
xvfsconf_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(XVFSCONF, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_xvfsconf_type = {
    .m_id           = LUAB_XVFSCONF_TYPE_ID,
    .m_name         = LUAB_XVFSCONF_TYPE,
    .m_vec          = xvfsconf_methods,
    .m_create       = xvfsconf_create,
    .m_init         = xvfsconf_init,
    .m_get          = xvfsconf_udata,
    .m_get_tbl      = xvfsconf_checktable,
    .m_set_tbl      = xvfsconf_pushtable,
    .m_alloc_tbl    = xvfsconf_alloctable,
    .m_len          = sizeof(luab_xvfsconf_t),
    .m_sz           = sizeof(struct xvfsconf),
};
