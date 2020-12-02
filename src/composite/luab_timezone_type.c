/*
 * Copyright (c) 2020 Henning Matyschok
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

extern luab_module_t luab_timezone_type;

/*
 * Interface against
 *
 *  struct timezone {
 *      int tz_minuteswest;
 *      int tz_dsttime;
 *  };
 */

typedef struct luab_timezone {
    luab_udata_t        ud_softc;
    struct timezone     ud_tz;
} luab_timezone_t;

#define luab_new_timezone(L, arg) \
    ((luab_timezone_t *)luab_newudata(L, &luab_timezone_type, (arg)))
#define luab_to_timezone(L, narg) \
    (luab_toldata((L), (narg), &luab_timezone_type, \
        struct timezone *, luab_timezone_type.m_sz))

/*
 * Subr.
 */

static void
timezone_fillxtable(lua_State *L, int narg, void *arg)
{
    struct timezone *tz;

    if ((tz = (struct timezone *)arg) != NULL) {

        luab_setinteger(L, narg, "tz_minuteswest",    tz->tz_minuteswest);
        luab_setinteger(L, narg, "tz_dsttime",        tz->tz_dsttime);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(TIMEZONE)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              tz_minuteswest  = (LUA_TNUMBER),
 *              tz_dsttime      = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = timezone:get_table()
 */
static int
TIMEZONE_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = timezone_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, &luab_timezone_type);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate timezone{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = timezone:dump()
 */
static int
TIMEZONE_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_timezone_type, luab_timezone_type.m_sz));
}

/*
 * Access functions.
 */

/***
 * Set value for minutes west of Greenwich.
 *
 * @function set_tz_minuteswest
 *
 * @param data              Specifies value in minutes.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = timezone:get_tz_minuteswest(data)
 */
static int
TIMEZONE_set_tz_minuteswest(lua_State *L)
{
    struct timezone *tz;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    tz = luab_udata(L, 1, &luab_timezone_type, struct timezone *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    tz->tz_minuteswest = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for minutes west of Greenwich.
 *
 * @function get_tz_minuteswest
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = timezone:get_tz_minuteswest()
 */
static int
TIMEZONE_get_tz_minuteswest(lua_State *L)
{
    struct timezone *tz;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    tz = luab_udata(L, 1, &luab_timezone_type, struct timezone *);
    x = tz->tz_minuteswest;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for type of dst correction.
 *
 * @function set_tz_dsttime
 *
 * @param data              Value.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = timezone:set_tz_dsttime(data)
 */
static int
TIMEZONE_set_tz_dsttime(lua_State *L)
{
    struct timezone *tz;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    tz = luab_udata(L, 1, &luab_timezone_type, struct timezone *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    tz->tz_dsttime = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for type of dst correction.
 *
 * @function get_tz_dsttime
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = timezone:get_tz_dsttime()
 */
static int
TIMEZONE_get_tz_dsttime(lua_State *L)
{
    struct timezone *tz;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    tz = luab_udata(L, 1, &luab_timezone_type, struct timezone *);
    x = tz->tz_dsttime;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
TIMEZONE_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_timezone_type));
}

static int
TIMEZONE_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_timezone_type));
}

static int
TIMEZONE_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_timezone_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t timezone_methods[] = {
    LUAB_FUNC("set_tz_minuteswest",     TIMEZONE_set_tz_minuteswest),
    LUAB_FUNC("set_tz_dsttime",         TIMEZONE_set_tz_dsttime),
    LUAB_FUNC("get_table",              TIMEZONE_get_table),
    LUAB_FUNC("get_tz_minuteswest",     TIMEZONE_get_tz_minuteswest),
    LUAB_FUNC("get_tz_dsttime",         TIMEZONE_get_tz_dsttime),
    LUAB_FUNC("dump",                   TIMEZONE_dump),
    LUAB_FUNC("__gc",                   TIMEZONE_gc),
    LUAB_FUNC("__len",                  TIMEZONE_len),
    LUAB_FUNC("__tostring",             TIMEZONE_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
timezone_create(lua_State *L, void *arg)
{
    return (luab_new_timezone(L, arg));
}

static void
timezone_init(void *ud, void *arg)
{
    luab_udata_init(&luab_timezone_type, ud, arg);
}

static void *
timezone_udata(lua_State *L, int narg)
{
    return (luab_to_timezone(L, narg));
}

static luab_table_t *
timezone_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct timezone *x, *y;
    size_t m, n;

    if ((tbl = luab_table_newvectornil(L, narg, &luab_timezone_type)) != NULL) {

        if (((x = (struct timezone *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_timezone_type, struct timezone *);
                        (void)memmove(&(x[m]), y, luab_timezone_type.m_sz);
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
timezone_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct timezone *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (struct timezone *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) > 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_timezone_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

static luab_table_t *
timezone_alloctable(void *vec, size_t card)
{
    return (luab_table_create(&luab_timezone_type, vec, card));
}

luab_module_t luab_timezone_type = {
    .m_id           = LUAB_TIMEZONE_TYPE_ID,
    .m_name         = LUAB_TIMEZONE_TYPE,
    .m_vec          = timezone_methods,
    .m_create       = timezone_create,
    .m_init         = timezone_init,
    .m_get          = timezone_udata,
    .m_get_tbl      = timezone_checktable,
    .m_set_tbl      = timezone_pushtable,
    .m_alloc_tbl    = timezone_alloctable,
    .m_len          = sizeof(luab_timezone_t),
    .m_sz           = sizeof(struct timezone),
};
