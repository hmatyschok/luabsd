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

extern luab_module_t luab_pthread_once_type;

/*
 * Interface against
 *
 *  struct pthread_once {
 *      int     state;
 *      pthread_mutex_t mutex;
 *  };
 *
 */

typedef struct luab_pthread_once {
    luab_udata_t        ud_softc;
    pthread_once_t      ud_x;
} luab_pthread_once_t;

/*
 * Subr.
 */

static void
pthread_once_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_module_t *m;
    pthread_once_t *x;

    m = luab_xmod(PTHREAD_MUTEX, TYPE, __func__);

    if ((x = (pthread_once_t *)arg) != NULL) {
        luab_setinteger(L, narg, "state", x->state);
        luab_setxdata(L, narg, m, "mutex", x->mutex);

   } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(PTHREAD_ONCE)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ] = pthread_once:get_table()
 */
static int
PTHREAD_ONCE_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PTHREAD_ONCE, TYPE, __func__);

    xtp.xtp_fill = pthread_once_fillxtable;
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
 * @usage iovec [, err, msg ] = pthread_once:dump()
 */
static int
PTHREAD_ONCE_dump(lua_State *L)
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
 * @param arg               Reffered object, (LUA_T{NIL,USERDATA(PTHREAD_ONCE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = pthread_once:set_value(arg)
 */
static int
PTHREAD_ONCE_set_value(lua_State *L)
{
    luab_module_t *m;
    luab_pthread_once_t *self;
    pthread_once_t *x;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(PTHREAD_ONCE, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_pthread_once_t *);

    if ((x = luab_udataisnil(L, 2, m, pthread_once_t *)) != NULL)
        (void)memmove(&self->ud_x, x, m->m_sz);
    else
        (void)memset_s(&self->ud_x, m->m_sz, 0, m->m_sz);

    return (luab_pushxdata(L, m, x));
}

/***
 * Get reference value.
 *
 * @function get_value
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = pthread_once:get_value()
 */
static int
PTHREAD_ONCE_get_value(lua_State *L)
{
    luab_module_t *m;
    pthread_once_t *x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PTHREAD_ONCE, TYPE, __func__);
    x = luab_udata(L, 1, m, pthread_once_t *);

    return (luab_pushxdata(L, m, x));
}

/*
 * Metamethods.
 */

static int
PTHREAD_ONCE_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_ONCE, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
PTHREAD_ONCE_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_ONCE, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
PTHREAD_ONCE_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_ONCE, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t pthread_once_methods[] = {
    LUAB_FUNC("set_value",      PTHREAD_ONCE_set_value),
    LUAB_FUNC("get_table",      PTHREAD_ONCE_get_table),
    LUAB_FUNC("get_value",      PTHREAD_ONCE_get_value),
    LUAB_FUNC("dump",           PTHREAD_ONCE_dump),
    LUAB_FUNC("__gc",           PTHREAD_ONCE_gc),
    LUAB_FUNC("__len",          PTHREAD_ONCE_len),
    LUAB_FUNC("__tostring",     PTHREAD_ONCE_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
pthread_once_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_ONCE, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
pthread_once_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_ONCE, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
pthread_once_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_pthread_once_t *self;

    m = luab_xmod(PTHREAD_ONCE, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_pthread_once_t *);
    return ((void *)&self->ud_x);
}

static luab_table_t *
pthread_once_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    pthread_once_t *x, *y;
    size_t i, j;

    m = luab_xmod(PTHREAD_ONCE, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (pthread_once_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, pthread_once_t *);
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
pthread_once_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    pthread_once_t *x;
    size_t i, j, k;

    m = luab_xmod(PTHREAD_ONCE, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (pthread_once_t *)tbl->tbl_vec) != NULL) &&
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
        errno = EINVAL;
}

static luab_table_t *
pthread_once_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(PTHREAD_ONCE, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_pthread_once_type = {
    .m_id           = LUAB_PTHREAD_ONCE_TYPE_ID,
    .m_name         = LUAB_PTHREAD_ONCE_TYPE,
    .m_vec          = pthread_once_methods,
    .m_create       = pthread_once_create,
    .m_init         = pthread_once_init,
    .m_get          = pthread_once_udata,
    .m_get_tbl      = pthread_once_checktable,
    .m_set_tbl      = pthread_once_pushtable,
    .m_alloc_tbl    = pthread_once_alloctable,
    .m_len          = sizeof(luab_pthread_once_t),
    .m_sz           = sizeof(pthread_once_t),
};
