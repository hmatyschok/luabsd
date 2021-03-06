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

#include <regex.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_regmatch_type;

/*
 * Interface against
 *
 *  typedef struct {
 *      regoff_t rm_so;
 *      regoff_t rm_eo;
 *  } regmatch_t;
 */

typedef struct luab_regmatch {
    luab_udata_t    ud_softc;
    regmatch_t      ud_rm;
} luab_regmatch_t;

/*
 * Subr.
 */

static void
regmatch_fillxtable(lua_State *L, int narg, void *arg)
{
    regmatch_t *rm;

    if ((rm = (regmatch_t *)arg) != NULL) {

        luab_setinteger(L, narg, "rm_so",      rm->rm_so);
        luab_setinteger(L, narg, "rm_eo",       rm->rm_eo);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(REGMATCH)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              rm_so   = (LUA_TNUMBER),
 *              rm_eo   = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = regmatch:get_table()
 */
static int
REGMATCH_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(REGMATCH, TYPE, __func__);

    xtp.xtp_fill = regmatch_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate regmatch{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = regmatch:dump()
 */
static int
REGMATCH_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(REGMATCH, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get start of match.
 *
 * @function rm_so
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = regmatch:rm_so()
 */
static int
REGMATCH_rm_so(lua_State *L)
{
    luab_module_t *m;
    regmatch_t *rm;
    regoff_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(REGMATCH, TYPE, __func__);
    rm = luab_udata(L, 1, m, regmatch_t *);
    x = rm->rm_so;

    return (luab_pushxinteger(L, x));
}

/***
 * Get get end of match.
 *
 * @function rm_eo
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = regmatch:rm_eo()
 */
static int
REGMATCH_rm_eo(lua_State *L)
{
    luab_module_t *m;
    regmatch_t *rm;
    regoff_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(REGMATCH, TYPE, __func__);
    rm = luab_udata(L, 1, m, regmatch_t *);
    x = rm->rm_eo;

    return (luab_pushxinteger(L, x));
}

/*
 * Meta-methods.
 */

static int
REGMATCH_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(REGMATCH, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
REGMATCH_len(lua_State *L)
{
    luab_module_t *m;
    m =  luab_xmod(REGMATCH, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
REGMATCH_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(REGMATCH, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t regmatch_methods[] = {
    LUAB_FUNC("rm_so",          REGMATCH_rm_so),
    LUAB_FUNC("rm_eo",          REGMATCH_rm_eo),
    LUAB_FUNC("get_table",      REGMATCH_get_table),
    LUAB_FUNC("dump",           REGMATCH_dump),
    LUAB_FUNC("__gc",           REGMATCH_gc),
    LUAB_FUNC("__len",          REGMATCH_len),
    LUAB_FUNC("__tostring",     REGMATCH_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
regmatch_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(REGMATCH, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
regmatch_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(REGMATCH, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
regmatch_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(REGMATCH, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
regmatch_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    regmatch_t *x, *y;
    size_t i, j;

    m = luab_xmod(REGMATCH, TYPE, __func__);

    if ((tbl = luab_table_newvector(L, narg, m)) != NULL) {

        if (((x = (regmatch_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, regmatch_t *);
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
regmatch_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    regmatch_t *x;
    size_t i, j, k;

    m = luab_xmod(REGMATCH, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (regmatch_t *)tbl->tbl_vec) != NULL) &&
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
regmatch_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(REGMATCH, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_regmatch_type = {
    .m_id           = LUAB_REGMATCH_TYPE_ID,
    .m_name         = LUAB_REGMATCH_TYPE,
    .m_vec          = regmatch_methods,
    .m_create       = regmatch_create,
    .m_init         = regmatch_init,
    .m_get          = regmatch_udata,
    .m_get_tbl      = regmatch_checktable,
    .m_set_tbl      = regmatch_pushtable,
    .m_alloc_tbl    = regmatch_alloctable,
    .m_len          = sizeof(luab_regmatch_t),
    .m_sz           = sizeof(regmatch_t),
};
