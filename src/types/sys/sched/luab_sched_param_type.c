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

#include <sys/sched.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_sched_param_type;

/*
 * Interface against
 *
 *  struct sched_param {
 *      int     sched_priority;
 *  };
 */

typedef struct luab_sched_param {
    luab_udata_t        ud_softc;
    struct sched_param  ud_sched;
} luab_sched_param_t;

/*
 * Subr.
 */

static void
sched_param_fillxtable(lua_State *L, int narg, void *arg)
{
    struct sched_param *sched;

    if ((sched = (struct sched_param *)arg) != NULL) {
        luab_setinteger(L, narg, "sched_priority",  sched->sched_priority);

    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SCHED_PARAM)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              sched_priority  = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = sched_param:get_table()
 */
static int
SCHED_PARAM_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SCHED_PARAM, TYPE, __func__);

    xtp.xtp_fill = sched_param_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - returns (LUA_TNIL).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = sched_param:dump()
 */
static int
SCHED_PARAM_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set schedeuling priority.
 *
 * @function set_sched_priority
 *
 * @param arg               Size, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sched_param:set_sched_priority()
 */
static int
SCHED_PARAM_set_sched_priority(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct sched_param *sched;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(SCHED_PARAM, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    sched = luab_udata(L, 1, m0, struct sched_param *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    sched->sched_priority = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get schedeuling priority.
 *
 * @function get_sched_priority
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sched_param:get_sched_priority()
 */
static int
SCHED_PARAM_get_sched_priority(lua_State *L)
{
    luab_module_t *m;
    struct sched_param *sched;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SCHED_PARAM, TYPE, __func__);
    sched = luab_udata(L, 1, m, struct sched_param *);
    x = sched->sched_priority;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
SCHED_PARAM_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SCHED_PARAM, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
SCHED_PARAM_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SCHED_PARAM, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
SCHED_PARAM_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SCHED_PARAM, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t sched_param_methods[] = {
    LUAB_FUNC("set_sched_priority",     SCHED_PARAM_set_sched_priority),
    LUAB_FUNC("get_table",              SCHED_PARAM_get_table),
    LUAB_FUNC("set_sched_priority",     SCHED_PARAM_get_sched_priority),
    LUAB_FUNC("dump",                   SCHED_PARAM_dump),
    LUAB_FUNC("__gc",                   SCHED_PARAM_gc),
    LUAB_FUNC("__len",                  SCHED_PARAM_len),
    LUAB_FUNC("__tostring",             SCHED_PARAM_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
sched_param_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SCHED_PARAM, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
sched_param_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SCHED_PARAM, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
sched_param_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_sched_param_t *self;

    m = luab_xmod(SCHED_PARAM, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_sched_param_t *);
    return ((void *)&(self->ud_sched));
}

static luab_table_t *
sched_param_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct sched_param *x, *y;
    size_t i, j;

    m = luab_xmod(SCHED_PARAM, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct sched_param *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct sched_param *);
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
sched_param_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct sched_param *x;
    size_t i, j, k;

    m = luab_xmod(SCHED_PARAM, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct sched_param *)tbl->tbl_vec) != NULL) &&
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
sched_param_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(SCHED_PARAM, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_sched_param_type = {
    .m_id           = LUAB_SCHED_PARAM_TYPE_ID,
    .m_name         = LUAB_SCHED_PARAM_TYPE,
    .m_vec          = sched_param_methods,
    .m_create       = sched_param_create,
    .m_init         = sched_param_init,
    .m_get          = sched_param_udata,
    .m_get_tbl      = sched_param_checktable,
    .m_set_tbl      = sched_param_pushtable,
    .m_alloc_tbl    = sched_param_alloctable,
    .m_len          = sizeof(luab_sched_param_t),
    .m_sz           = sizeof(struct sched_param),
};
