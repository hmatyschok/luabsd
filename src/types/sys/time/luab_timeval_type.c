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

extern luab_module_t luab_timeval_type;

/*
 * Interface against
 *
 *  struct timeval {
 *      time_t      tv_sec;
 *      suseconds_t tv_usec;
 *  };
 */

typedef struct luab_timeval {
    luab_udata_t        ud_softc;
    struct timeval      ud_tv;
} luab_timeval_t;

/*
 * Subr.
 */

static void
timeval_fillxtable(lua_State *L, int narg, void *arg)
{
    struct timeval *tv;

    if ((tv = (struct timeval *)arg) != NULL) {

        luab_setinteger(L, narg, "tv_sec",    tv->tv_sec);
        luab_setinteger(L, narg, "tv_usec",   tv->tv_usec);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(TIMEVAL)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              tv_sec  = (LUA_TNUMBER),
 *              tv_usec = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = timeval:get_table()
 */
static int
TIMEVAL_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIMEVAL, TYPE, __func__);

    xtp.xtp_fill = timeval_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate timeval{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = timeval:dump()
 */
static int
TIMEVAL_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TIMEVAL, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions.
 */

/***
 * Set value for tv_sec.
 *
 * @function set_tv_sec
 *
 * @param arg               Specifies value in seconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = timeval:set_tv_sec(arg)
 */
static int
TIMEVAL_set_tv_sec(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct timeval *tv;
    time_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(TIMEVAL, TYPE, __func__);
    m1 = luab_xmod(TIME, TYPE, __func__);

    tv = luab_udata(L, 1, m0, struct timeval *);
    x = (time_t)luab_checkxinteger(L, 2, m1, luab_env_int_max);

    tv->tv_sec = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for tv_sec.
 *
 * @function get_tv_sec
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = timeval:get_tv_sec()
 */
static int
TIMEVAL_get_tv_sec(lua_State *L)
{
    luab_module_t *m;
    struct timeval *tv;
    time_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIMEVAL, TYPE, __func__);
    tv = luab_udata(L, 1, m, struct timeval *);
    x = tv->tv_sec;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for tv_usec.
 *
 * @function set_tv_usec
 *
 * @param arg               Specifies value in nanoneconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = timeval:set_tv_usec(arg)
 */
static int
TIMEVAL_set_tv_usec(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct timeval *tv;
    long x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(TIMEVAL, TYPE, __func__);
    m1 = luab_xmod(LONG, TYPE, __func__);

    tv = luab_udata(L, 1, m0, struct timeval *);
    x = (long)luab_checkxinteger(L, 2, m1, luab_env_long_max);

    tv->tv_usec = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for tv_usec.
 *
 * @function get_tv_usec
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = timeval:get_tv_usec()
 */
static int
TIMEVAL_get_tv_usec(lua_State *L)
{
    luab_module_t *m;
    struct timeval *tv;
    long x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIMEVAL, TYPE, __func__);
    tv = luab_udata(L, 1, m, struct timeval *);
    x = tv->tv_usec;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
TIMEVAL_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TIMEVAL, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
TIMEVAL_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TIMEVAL, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
TIMEVAL_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TIMEVAL, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t timeval_methods[] = {
    LUAB_FUNC("set_tv_sec",     TIMEVAL_set_tv_sec),
    LUAB_FUNC("set_tv_usec",    TIMEVAL_set_tv_usec),
    LUAB_FUNC("get_table",      TIMEVAL_get_table),
    LUAB_FUNC("get_tv_sec",     TIMEVAL_get_tv_sec),
    LUAB_FUNC("get_tv_usec",    TIMEVAL_get_tv_usec),
    LUAB_FUNC("dump",           TIMEVAL_dump),
    LUAB_FUNC("__gc",           TIMEVAL_gc),
    LUAB_FUNC("__len",          TIMEVAL_len),
    LUAB_FUNC("__tostring",     TIMEVAL_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
timeval_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(TIMEVAL, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
timeval_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(TIMEVAL, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
timeval_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(TIMEVAL, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
timeval_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(TIMEVAL, TYPE, __func__);
    luab_table_t *tbl;
    struct timeval *x, *y;
    size_t i, j;

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct timeval *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct timeval *);
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
timeval_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    m = luab_xmod(TIMEVAL, TYPE, __func__);
    struct timeval *x;
    size_t i, j, k;

    if (tbl != NULL) {

        if (((x = (struct timeval *)tbl->tbl_vec) != NULL) &&
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
timeval_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(TIMEVAL, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_timeval_type = {
    .m_id           = LUAB_TIMEVAL_TYPE_ID,
    .m_name         = LUAB_TIMEVAL_TYPE,
    .m_vec          = timeval_methods,
    .m_create       = timeval_create,
    .m_init         = timeval_init,
    .m_get          = timeval_udata,
    .m_get_tbl      = timeval_checktable,
    .m_set_tbl      = timeval_pushtable,
    .m_alloc_tbl    = timeval_alloctable,
    .m_len          = sizeof(luab_timeval_t),
    .m_sz           = sizeof(struct timeval),
};
