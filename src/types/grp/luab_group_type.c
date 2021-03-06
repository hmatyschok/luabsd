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

#include <grp.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_group_type;

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

/*
 * Subr.
 */

static int
luab_table_pushgroup(lua_State *L, int narg, const char *k, caddr_t *vec)
{
    size_t i, j;

    if (vec != NULL) {
        luab_table_init(L, 1);

        for (i = 0, j = 1; vec[i] != NULL; i++, j++)
            luab_rawsetstring(L, narg, j, vec[i]);

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
group_fillxtable(lua_State *L, int narg, void *arg)
{
    struct group *grp;

    if ((grp = (struct group *)arg) != NULL) {

        luab_setstring(L, narg, "gr_name",    grp->gr_name);
        luab_setstring(L, narg, "gr_passwd",   grp->gr_passwd);
        luab_setinteger(L, narg, "gr_gid",    grp->gr_gid);

        if (grp->gr_mem != NULL)
            (void)luab_table_pushgroup(L, narg, "gr_mem", grp->gr_mem);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(GROUP)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              gr_name     = (LUA_T{NIL,STRING}),
 *              gr_passwd    = (LUA_T{NIL,STRING}),
 *              gr_gid      = (LUA_TNUMBER),
 *              gr_mem      = (LUA_T{NIL,TABLE}),
 *          }
 *
 * @usage t [, err, msg ]= group:get_table()
 */
static int
GROUP_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(GROUP, TYPE, __func__);

    xtp.xtp_fill = group_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
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
    luab_module_t *m;
    m = luab_xmod(GROUP, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
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
 * @usage x [, err, msg ] = group:gr_name()
 */
static int
GROUP_gr_name(lua_State *L)
{
    luab_module_t *m;
    struct group *grp;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(GROUP, TYPE, __func__);
    grp = luab_udata(L, 1, m, struct group *);
    dp = grp->gr_name;

    return (luab_pushstring(L, dp));
}

/***
 * Get group password.
 *
 * @function gr_passwd
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = group:gr_passwd()
 */
static int
GROUP_gr_passwd(lua_State *L)
{
    luab_module_t *m;
    struct group *grp;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(GROUP, TYPE, __func__);
    grp = luab_udata(L, 1, m, struct group *);
    dp = grp->gr_passwd;

    return (luab_pushstring(L, dp));
}

/***
 * Get group ID.
 *
 * @function gr_gid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = group:gr_gid()
 */
static int
GROUP_gr_gid(lua_State *L)
{
    luab_module_t *m;
    struct group *grp;
    gid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(GROUP, TYPE, __func__);
    grp = luab_udata(L, 1, m, struct group *);
    x = grp->gr_gid;

    return (luab_pushxinteger(L, x));
}

/***
 * Get group members.
 *
 * @function gr_mem
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = group:gr_mem()
 */
static int
GROUP_gr_mem(lua_State *L)
{
    luab_module_t *m;
    struct group *grp;
    caddr_t *vec;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(GROUP, TYPE, __func__);
    grp = luab_udata(L, 1, m, struct group *);
    vec = grp->gr_mem;

    return (luab_table_pushgroup(L, -2, NULL, vec));
}

/*
 * Meta-methods.
 */

static int
GROUP_gc(lua_State *L)
{
    luab_module_t *m;
    struct group *grp;
    caddr_t *vec;
    size_t n;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(GROUP, TYPE, __func__);
    grp = luab_udata(L, 1, m, struct group *);

    luab_core_freestr(grp->gr_name);
    luab_core_freestr(grp->gr_passwd);

    if ((vec = grp->gr_mem) != NULL) {
        for (n = 0; vec[n] != NULL; n++)    /* XXX */
            luab_core_freestr(vec[n]);

        luab_core_free(vec, 0);
    }
    return (luab_core_gc(L, 1, m));
}

static int
GROUP_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(GROUP, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
GROUP_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(GROUP, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t group_methods[] = {
    LUAB_FUNC("gr_name",        GROUP_gr_name),
    LUAB_FUNC("gr_passwd",      GROUP_gr_passwd),
    LUAB_FUNC("gr_gid",         GROUP_gr_gid),
    LUAB_FUNC("gr_mem",         GROUP_gr_mem),
    LUAB_FUNC("get_table",      GROUP_get_table),
    LUAB_FUNC("dump",           GROUP_dump),
    LUAB_FUNC("__gc",           GROUP_gc),
    LUAB_FUNC("__len",          GROUP_len),
    LUAB_FUNC("__tostring",     GROUP_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
group_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(GROUP, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
group_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(GROUP, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
group_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(GROUP, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
group_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct group *x, *y;
    size_t i, j;

    m = luab_xmod(GROUP, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct group *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct group *);
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
group_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct group *x;
    size_t i, j, k;

    m = luab_xmod(GROUP, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct group *)tbl->tbl_vec) != NULL) &&
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
group_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(GROUP, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_group_type = {
    .m_id           = LUAB_GROUP_TYPE_ID,
    .m_name         = LUAB_GROUP_TYPE,
    .m_vec          = group_methods,
    .m_create       = group_create,
    .m_init         = group_init,
    .m_get          = group_udata,
    .m_get_tbl      = group_checktable,
    .m_set_tbl      = group_pushtable,
    .m_alloc_tbl    = group_alloctable,
    .m_len          = sizeof(luab_group_t),
    .m_sz           = sizeof(struct group),
};
