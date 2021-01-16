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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_pthread_mutexattr_type;

/*
 * Interface against
 *
 *  pthread_mutexattr_t
 *
 */

typedef struct luab_pthread_mutexattr {
    luab_udata_t    ud_softc;
    pthread_mutexattr_t         ud_dp;
} luab_pthread_mutexattr_t;

/*
 * Subr.
 */

static void
pthread_mutexattr_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_pthread_mutexattr_t *self;

    if ((self = (luab_pthread_mutexattr_t *)arg) != NULL) {
        luab_setfstring(L, narg, "value", "(%p)", self->ud_dp);

   } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(PTHREAD_MUTEXATTR)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ] = pthread_mutexattr:get_table()
 */
static int
PTHREAD_MUTEXATTR_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PTHREAD_MUTEXATTR, TYPE, __func__);

    xtp.xtp_fill = pthread_mutexattr_fillxtable;
    xtp.xtp_arg = luab_todata(L, 1, m, void *);
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
 * @usage iovec [, err, msg ] = pthread_mutexattr:dump()
 */
static int
PTHREAD_MUTEXATTR_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set reference value.
 *
 * @function set_value
 *
 * @param arg               Reffered object, (LUA_T{NIL,USERDATA(PTHREAD_MUTEXATTR)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = pthread_mutexattr:set_value(arg)
 */
static int
PTHREAD_MUTEXATTR_set_value(lua_State *L)
{
    luab_module_t *m;
    luab_pthread_mutexattr_t *self;
    pthread_mutexattr_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(PTHREAD_MUTEXATTR, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_pthread_mutexattr_t *);
    x = luab_udataisnil(L, 2, m, pthread_mutexattr_t);

    self->ud_dp = x;

    return (luab_pushxdata(L, m, x));
}

/***
 * Get reference value.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = pthread_mutexattr:get_value()
 */
static int
PTHREAD_MUTEXATTR_get_value(lua_State *L)
{
    luab_module_t *m;
    pthread_mutexattr_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PTHREAD_MUTEXATTR, TYPE, __func__);
    x = luab_udata(L, 1, m, pthread_mutexattr_t);

    return (luab_pushxdata(L, m, x));
}

/*
 * Metamethods.
 */

static int
PTHREAD_MUTEXATTR_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_MUTEXATTR, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
PTHREAD_MUTEXATTR_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_MUTEXATTR, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
PTHREAD_MUTEXATTR_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_MUTEXATTR, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t pthread_mutexattr_methods[] = {
    LUAB_FUNC("set_value",      PTHREAD_MUTEXATTR_set_value),
    LUAB_FUNC("get_table",      PTHREAD_MUTEXATTR_get_table),
    LUAB_FUNC("get_value",      PTHREAD_MUTEXATTR_get_value),
    LUAB_FUNC("dump",           PTHREAD_MUTEXATTR_dump),
    LUAB_FUNC("__gc",           PTHREAD_MUTEXATTR_gc),
    LUAB_FUNC("__len",          PTHREAD_MUTEXATTR_len),
    LUAB_FUNC("__tostring",     PTHREAD_MUTEXATTR_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
pthread_mutexattr_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_MUTEXATTR, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
pthread_mutexattr_init(void *ud, void *arg)
{
    luab_pthread_mutexattr_t *self;
    pthread_mutexattr_t x;

    if (((self = (luab_pthread_mutexattr_t *)ud) != NULL) &&
        ((x = (pthread_mutexattr_t)arg) != NULL))
        self->ud_dp = x;
    else
        errno = ENXIO;
}

static void *
pthread_mutexattr_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_pthread_mutexattr_t *self;

    m = luab_xmod(PTHREAD_MUTEXATTR, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_pthread_mutexattr_t *);
    return ((void *)self->ud_dp);
}

static luab_table_t *
pthread_mutexattr_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    pthread_mutexattr_t *x, y;
    size_t i, j;

    m = luab_xmod(PTHREAD_MUTEXATTR, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (pthread_mutexattr_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, pthread_mutexattr_t);
                        x[i] = y;
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
pthread_mutexattr_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    pthread_mutexattr_t *x;
    size_t i, j, k;

    m = luab_xmod(PTHREAD_MUTEXATTR, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (pthread_mutexattr_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, new);

            for (i = 0, j = tbl->tbl_card, k = 1; i < j; i++, k++)
                luab_rawsetxdata(L, narg, m, k, x[i]);

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

static luab_table_t *
pthread_mutexattr_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_MUTEXATTR, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_pthread_mutexattr_type = {
    .m_id           = LUAB_PTHREAD_MUTEXATTR_TYPE_ID,
    .m_name         = LUAB_PTHREAD_MUTEXATTR_TYPE,
    .m_vec          = pthread_mutexattr_methods,
    .m_create       = pthread_mutexattr_create,
    .m_init         = pthread_mutexattr_init,
    .m_get          = pthread_mutexattr_udata,
    .m_get_tbl      = pthread_mutexattr_checktable,
    .m_set_tbl      = pthread_mutexattr_pushtable,
    .m_alloc_tbl    = pthread_mutexattr_alloctable,
    .m_len          = sizeof(luab_pthread_mutexattr_t),
    .m_sz           = sizeof(pthread_mutexattr_t),
};
