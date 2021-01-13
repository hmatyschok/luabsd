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

extern luab_module_t luab_itimerspec_type;

/*
 * Interface against
 *
 *  struct itimerspec {
 *      struct timespec  it_interval;
 *      struct timespec  it_value;
 *  };
 *
 */

typedef struct luab_itimerspec {
    luab_udata_t        ud_softc;
    struct itimerspec    ud_it;
} luab_itimerspec_t;

/*
 * Subr.
 */

static void
itimerspec_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_module_t *m;
    struct itimerspec *it;

    m = luab_xmod(TIMESPEC, TYPE, __func__);

    if ((it = (struct itimerspec *)arg) != NULL) {

        luab_setxdata(L, narg, m, "it_interval",   &it->it_interval);
        luab_setxdata(L, narg, m, "it_value",      &it->it_value);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(ITIMERSPEC)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              it_interval = (LUA_TUSERDATA(TIMEVAL)),
 *              it_value    = (LUA_TUSERDATA(TIMEVAL)),
 *          }
 *
 * @usage t [, err, msg ] = itimerspec:get_table()
 */
static int
ITIMERSPEC_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(ITIMERSPEC, TYPE, __func__);

    xtp.xtp_fill = itimerspec_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate itimerspec{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA(TIMEVAL)} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = itimerspec:dump()
 */
static int
ITIMERSPEC_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(ITIMERSPEC, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions.
 */

/***
 * Set timer interval.
 *
 * @function set_it_interval
 *
 * @param timeval          Instance of (LUA_TUSERDATA(TIMEVAL)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = itimerspec:set_it_interval(timeval)
 */
static int
ITIMERSPEC_set_it_interval(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct itimerspec *it;
    struct timespec *tv;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(ITIMERSPEC, TYPE, __func__);
    m1 = luab_xmod(TIMEVAL, TYPE, __func__);

    it = luab_udata(L, 1, m0, struct itimerspec *);
    tv = luab_udata(L, 2, m1, struct timespec *);

    (void)memmove(&it->it_interval, tv, m1->m_sz);

    return (luab_pushxinteger(L, luab_env_success));
}

/***
 * Get timer interval.
 *
 * @function get_it_interval
 *
 * @return (LUA_T{NIL,USERDATA(TIMEVAL)} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage timeval [, err, msg ] = itimerspec:get_it_interval()
 */
static int
ITIMERSPEC_get_it_interval(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct itimerspec *it;
    struct timespec *tv;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(ITIMERSPEC, TYPE, __func__);
    m1 = luab_xmod(TIMEVAL, TYPE, __func__);

    it = luab_udata(L, 1, m0, struct itimerspec *);
    tv = &(it->it_interval);

    return (luab_pushxdata(L, m1, tv));
}

/* current value */
/***
 * Set current value.
 *
 * @function set_it_value
 *
 * @param timeval          Instance of (LUA_TUSERDATA(TIMEVAL)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = itimerspec:set_it_value(timeval)
 */
static int
ITIMERSPEC_set_it_value(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct itimerspec *it;
    struct timespec *tv;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(ITIMERSPEC, TYPE, __func__);
    m1 = luab_xmod(TIMEVAL, TYPE, __func__);

    it = luab_udata(L, 1, m0, struct itimerspec *);
    tv = luab_udata(L, 2, m1, struct timespec *);

    (void)memmove(&it->it_value, tv, m1->m_sz);

    return (luab_pushxinteger(L, luab_env_success));
}

/***
 * Get timer value.
 *
 * @function get_it_value
 *
 * @return (LUA_T{NIL,USERDATA(TIMEVAL)} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage timeval [, err, msg ] = itimerspec:get_it_value()
 */
static int
ITIMERSPEC_get_it_value(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct itimerspec *it;
    struct timespec *tv;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(ITIMERSPEC, TYPE, __func__);
    m1 = luab_xmod(TIMEVAL, TYPE, __func__);

    it = luab_udata(L, 1, m0, struct itimerspec *);
    tv = &(it->it_value);

    return (luab_pushxdata(L, m1, tv));
}

/*
 * Metamethods.
 */

static int
ITIMERSPEC_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(ITIMERSPEC, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
ITIMERSPEC_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(ITIMERSPEC, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
ITIMERSPEC_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(ITIMERSPEC, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t itimerspec_methods[] = {
    LUAB_FUNC("set_it_interval",    ITIMERSPEC_set_it_interval),
    LUAB_FUNC("set_it_value",       ITIMERSPEC_set_it_value),
    LUAB_FUNC("get_table",          ITIMERSPEC_get_table),
    LUAB_FUNC("get_it_interval",    ITIMERSPEC_get_it_interval),
    LUAB_FUNC("get_it_value",       ITIMERSPEC_get_it_value),
    LUAB_FUNC("dump",               ITIMERSPEC_dump),
    LUAB_FUNC("__gc",               ITIMERSPEC_gc),
    LUAB_FUNC("__len",              ITIMERSPEC_len),
    LUAB_FUNC("__tostring",         ITIMERSPEC_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
itimerspec_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(ITIMERSPEC, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
itimerspec_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(ITIMERSPEC, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
itimerspec_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(ITIMERSPEC, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
itimerspec_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct itimerspec *x, *y;
    size_t i, j;

    m = luab_xmod(ITIMERSPEC, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct itimerspec *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct itimerspec *);
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
itimerspec_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct itimerspec *x;
    size_t i, j, k;

    m = luab_xmod(ITIMERSPEC, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct itimerspec *)tbl->tbl_vec) != NULL) &&
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
itimerspec_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(ITIMERSPEC, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_itimerspec_type = {
    .m_id           = LUAB_ITIMERSPEC_TYPE_ID,
    .m_name         = LUAB_ITIMERSPEC_TYPE,
    .m_vec          = itimerspec_methods,
    .m_create       = itimerspec_create,
    .m_init         = itimerspec_init,
    .m_get          = itimerspec_udata,
    .m_get_tbl      = itimerspec_checktable,
    .m_set_tbl      = itimerspec_pushtable,
    .m_alloc_tbl    = itimerspec_alloctable,
    .m_len          = sizeof(luab_itimerspec_t),
    .m_sz           = sizeof(struct itimerspec),
};
