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

#include <sys/time.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_linger_type;

/*
 * Interface against
 *
 *  struct linger {
 *      int l_onoff;
 *      int l_linger;
 *  };
 */

typedef struct luab_linger {
    luab_udata_t    ud_softc;
    struct linger   ud_l;
} luab_linger_t;

/*
 * Subr.
 */

static void
linger_fillxtable(lua_State *L, int narg, void *arg)
{
    struct linger *l;

    if ((l = (struct linger *)arg) != NULL) {

        luab_setinteger(L, narg, "l_onoff",   l->l_onoff);
        luab_setinteger(L, narg, "l_linger",  l->l_linger);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(LINGER)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              l_onoff     = (LUA_TNUMBER),
 *              l_linger    = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = linger:gettable()
 */
static int
LINGER_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LINGER, TYPE, __func__);

    xtp.xtp_fill = linger_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate linger{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = linger:dump()
 */
static int
LINGER_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LINGER, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Service primitives.
 */

/***
 * Set value for option on / off.
 *
 * @function set_l_onoff
 *
 * @param arg               Option.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = linger:set_l_onoff(arg)
 */
static int
LINGER_set_l_onoff(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct linger *l;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(LINGER, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    l = luab_udata(L, 1, m0, struct linger *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_int_max);

    l->l_onoff = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for option on / off.
 *
 * @function get_l_onoff
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = linger:get_l_onoff()
 */
static int
LINGER_get_l_onoff(lua_State *L)
{
    luab_module_t *m;
    struct linger *l;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LINGER, TYPE, __func__);
    l = luab_udata(L, 1, m, struct linger *);
    x = l->l_onoff;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for linger.
 *
 * @function set_l_linger
 *
 * @param arg               Specifies l_linger.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = linger:set_l_linger(arg)
 */
static int
LINGER_set_l_linger(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct linger *l;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(LINGER, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    l = luab_udata(L, 1, m0, struct linger *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_int_max);

    l->l_linger = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for linger.
 *
 * @function get_l_linger
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = linger:get_l_linger()
 */
static int
LINGER_get_l_linger(lua_State *L)
{
    luab_module_t *m;
    struct linger *l;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LINGER, TYPE, __func__);
    l = luab_udata(L, 1, m, struct linger *);
    x = l->l_linger;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
LINGER_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LINGER, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
LINGER_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LINGER, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
LINGER_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LINGER, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t linger_methods[] = {
    LUAB_FUNC("set_l_onoff",    LINGER_set_l_onoff),
    LUAB_FUNC("set_l_linger",   LINGER_set_l_linger),
    LUAB_FUNC("get_table",      LINGER_get_table),
    LUAB_FUNC("get_l_onoff",    LINGER_get_l_onoff),
    LUAB_FUNC("get_l_linger",   LINGER_get_l_linger),
    LUAB_FUNC("dump",           LINGER_dump),
    LUAB_FUNC("__gc",           LINGER_gc),
    LUAB_FUNC("__len",          LINGER_len),
    LUAB_FUNC("__tostring",     LINGER_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
linger_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(LINGER, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
linger_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(LINGER, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
linger_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(LINGER, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
linger_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct linger *x, *y;
    size_t i, j;

    m = luab_xmod(LINGER, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct linger *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct linger *);
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
linger_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct linger *x;
    size_t i, j, k;

    m = luab_xmod(LINGER, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct linger *)tbl->tbl_vec) != NULL) &&
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
linger_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(LINGER, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_linger_type = {
    .m_id           = LUAB_LINGER_TYPE_ID,
    .m_name         = LUAB_LINGER_TYPE,
    .m_vec          = linger_methods,
    .m_create       = linger_create,
    .m_init         = linger_init,
    .m_get          = linger_udata,
    .m_get_tbl      = linger_checktable,
    .m_set_tbl      = linger_pushtable,
    .m_alloc_tbl    = linger_alloctable,
    .m_len          = sizeof(luab_linger_t),
    .m_sz           = sizeof(struct linger),
};
