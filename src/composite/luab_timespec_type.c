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

extern luab_module_t luab_timespec_type;

/*
 * Interface against
 *
 *  struct timespec {
 *      time_t  tv_sec;
 *      long    tv_nsec;
 *  };
 */

typedef struct luab_timespec {
    luab_udata_t        ud_softc;
    struct timespec     ud_tv;
} luab_timespec_t;

/*
 * Subr.
 */

static void
timespec_fillxtable(lua_State *L, int narg, void *arg)
{
    struct timespec *tv;

    if ((tv = (struct timespec *)arg) != NULL) {

        luab_setinteger(L, narg, "tv_sec",    tv->tv_sec);
        luab_setinteger(L, narg, "tv_nsec",   tv->tv_nsec);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(TIMESPEC)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              tv_sec  = (LUA_TNUMBER),
 *              tv_nsec = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = timespec:get_table()
 */
static int
TIMESPEC_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIMESPEC, TYPE, __func__);

    xtp.xtp_fill = timespec_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate timespec{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = timespec:dump()
 */
static int
TIMESPEC_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TIMESPEC, TYPE, __func__);
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
 * @usage x [, err, msg ] = timespec:set_tv_sec(arg)
 */
static int
TIMESPEC_set_tv_sec(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct timespec *tv;
    time_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(TIMESPEC, TYPE, __func__);
    m1 = luab_xmod(TIME, TYPE, __func__);

    tv = luab_udata(L, 1, m0, struct timespec *);
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
 * @usage x [, err, msg ] = timespec:get_tv_sec()
 */
static int
TIMESPEC_get_tv_sec(lua_State *L)
{
    luab_module_t *m;
    struct timespec *tv;
    time_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIMESPEC, TYPE, __func__);
    tv = luab_udata(L, 1, m, struct timespec *);
    x = tv->tv_sec;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for tv_nsec.
 *
 * @function set_tv_nsec
 *
 * @param arg               Specifies value in nanoneconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = timespec:set_tv_nsec(arg)
 */
static int
TIMESPEC_set_tv_nsec(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct timespec *tv;
    long x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(TIMESPEC, TYPE, __func__);
    m1 = luab_xmod(LONG, TYPE, __func__);

    tv = luab_udata(L, 1, m0, struct timespec *);
    x = (long)luab_checkxinteger(L, 2, m1, luab_env_long_max);

    tv->tv_nsec = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for tv_nsec.
 *
 * @function get_tv_nsec
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = timespec:get_tv_nsec()
 */
static int
TIMESPEC_get_tv_nsec(lua_State *L)
{
    luab_module_t *m;
    struct timespec *tv;
    long x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIMESPEC, TYPE, __func__);
    tv = luab_udata(L, 1, m, struct timespec *);
    x = tv->tv_nsec;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
TIMESPEC_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TIMESPEC, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
TIMESPEC_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TIMESPEC, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
TIMESPEC_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TIMESPEC, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t timespec_methods[] = {
    LUAB_FUNC("set_tv_sec",     TIMESPEC_set_tv_sec),
    LUAB_FUNC("set_tv_nsec",    TIMESPEC_set_tv_nsec),
    LUAB_FUNC("get_table",      TIMESPEC_get_table),
    LUAB_FUNC("get_tv_sec",     TIMESPEC_get_tv_sec),
    LUAB_FUNC("get_tv_nsec",    TIMESPEC_get_tv_nsec),
    LUAB_FUNC("dump",           TIMESPEC_dump),
    LUAB_FUNC("__gc",           TIMESPEC_gc),
    LUAB_FUNC("__len",          TIMESPEC_len),
    LUAB_FUNC("__tostring",     TIMESPEC_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
timespec_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(TIMESPEC, TYPE, __func__);
    return (luab_newudata(L, m, arg));
}

static void
timespec_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(TIMESPEC, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
timespec_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(TIMESPEC, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
timespec_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct timespec *x, *y;
    size_t i, j;

    m = luab_xmod(TIMESPEC, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct timespec *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct timespec *);
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
timespec_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct timespec *x;
    size_t i, j, k;

    m = luab_xmod(TIMESPEC, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct timespec *)tbl->tbl_vec) != NULL) &&
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
timespec_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(TIMESPEC, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_timespec_type = {
    .m_id           = LUAB_TIMESPEC_TYPE_ID,
    .m_name         = LUAB_TIMESPEC_TYPE,
    .m_vec          = timespec_methods,
    .m_create       = timespec_create,
    .m_init         = timespec_init,
    .m_get          = timespec_udata,
    .m_get_tbl      = timespec_checktable,
    .m_set_tbl      = timespec_pushtable,
    .m_alloc_tbl    = timespec_alloctable,
    .m_len          = sizeof(luab_timespec_t),
    .m_sz           = sizeof(struct timespec),
};
