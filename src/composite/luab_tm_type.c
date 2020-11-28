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

#include <time.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_tm_type;

/*
 * Interface against
 *
 *  struct tm {
 *      int tm_sec;
 *      int tm_min;
 *      int tm_hour;
 *      int tm_mday;
 *      int tm_mon;
 *      int tm_year;
 *      int tm_wday;
 *      int tm_yday;
 *      int tm_isdst;
 *      long    tm_gmtoff;
 *      char    *tm_zone;
 *  };
 */

typedef struct luab_tm {
    luab_udata_t    ud_softc;
    struct tm       ud_tm;
} luab_tm_t;

#define luab_new_tm(L, arg) \
    ((luab_tm_t *)luab_newudata(L, &luab_tm_type, (arg)))
#define luab_to_tm(L, narg) \
    (luab_toldata((L), (narg), &luab_tm_type, \
        struct tm *, sizeof(struct tm)))

/*
 * Subr.
 */

static void
tm_fillxtable(lua_State *L, int narg, void *arg)
{
    struct tm *tm;

    if ((tm = (struct tm *)arg) != NULL) {

        luab_setinteger(L, narg, "tm_sec",        tm->tm_sec);
        luab_setinteger(L, narg, "tm_min",        tm->tm_min);
        luab_setinteger(L, narg, "tm_hour",       tm->tm_hour);
        luab_setinteger(L, narg, "tm_mday",       tm->tm_mday);
        luab_setinteger(L, narg, "tm_mon",        tm->tm_mon);
        luab_setinteger(L, narg, "tm_year",       tm->tm_year);
        luab_setinteger(L, narg, "tm_wday",       tm->tm_wday);
        luab_setinteger(L, narg, "tm_yday",       tm->tm_yday);
        luab_setinteger(L, narg, "tm_isdst",      tm->tm_isdst);
        luab_setinteger(L, narg, "tm_gmtoff",     tm->tm_gmtoff);

        if (tm->tm_zone != NULL)    /* XXX */
            luab_setstring(L, narg, "tm_zone",    tm->tm_zone);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(TM)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              tm_sec      = (LUA_TNUMBER),
 *              tm_min      = (LUA_TNUMBER),
 *              tm_hour     = (LUA_TNUMBER),
 *              tm_mday     = (LUA_TNUMBER),
 *              tm_mon      = (LUA_TNUMBER),
 *              tm_year     = (LUA_TNUMBER),
 *              tm_wday     = (LUA_TNUMBER),
 *              tm_yday     = (LUA_TNUMBER),
 *              tm_isdst    = (LUA_TNUMBER),
 *              tm_gmtoff   = (LUA_TNUMBER),
 *              tm_zone     = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t = tm:get_table()
 */
static int
TM_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = tm_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, &luab_tm_type);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate tm{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = tm:dump()
 */
static int
TM_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_tm_type, sizeof(struct tm)));
}

/*
 * Access functions.
 */

/***
 * Set value for seconds after the minute [0-60].
 *
 * @function set_tm_sec
 *
 * @param data              Seconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:set_tm_sec(data)
 */
static int
TM_set_tm_sec(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    tm->tm_sec = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for seconds after the minute [0-60].
 *
 * @function get_tm_sec
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:get_tm_sec()
 */
static int
TM_get_tm_sec(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = tm->tm_sec;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for minutes after the hour [0-59].
 *
 * @function set_tm_min
 *
 * @param data              Minutes.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, min ] = tm:set_tm_min(data)
 */
static int
TM_set_tm_min(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    tm->tm_min = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for minutes after the hour [0-59].
 *
 * @function get_tm_min
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, min ] = tm:get_tm_min()
 */
static int
TM_get_tm_min(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = tm->tm_min;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for hours since midnight [0-23].
 *
 * @function set_tm_hour
 *
 * @param data              Hour.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:set_tm_hour(data)
 */
static int
TM_set_tm_hour(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    tm->tm_hour = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for hours since midnight [0-23].
 *
 * @function get_tm_hour
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:get_tm_hour()
 */
static int
TM_get_tm_hour(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = tm->tm_hour;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for day of the month [1-31].
 *
 * @function set_tm_mday
 *
 * @param data              Day.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:set_tm_mday(data)
 */
static int
TM_set_tm_mday(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    tm->tm_mday = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for day of the month [1-31].
 *
 * @function get_tm_mday
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:get_tm_mday()
 */
static int
TM_get_tm_mday(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = tm->tm_mday;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for months since January [0-11].
 *
 * @function set_tm_mon
 *
 * @param data              Month.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:set_tm_mon(data)
 */
static int
TM_set_tm_mon(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    tm->tm_mon = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for months since January [0-11].
 *
 * @function get_tm_mon
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:get_tm_mon()
 */
static int
TM_get_tm_mon(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = tm->tm_mon;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for years since 1900.
 *
 * @function set_tm_year
 *
 * @param data              Seconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:set_tm_year(data)
 */
static int
TM_set_tm_year(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    tm->tm_year = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for years since 1900.
 *
 * @function get_tm_year
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:get_tm_year()
 */
static int
TM_get_tm_year(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = tm->tm_year;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for days since Sunday [0-6].
 *
 * @function set_tm_wday
 *
 * @param data              Day.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:set_tm_wday(data)
 */
static int
TM_set_tm_wday(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    tm->tm_wday = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for days since Sunday [0-6].
 *
 * @function get_tm_wday
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:get_tm_wday()
 */
static int
TM_get_tm_wday(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = tm->tm_wday;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for days since January 1 [0-365].
 *
 * @function set_tm_yday
 *
 * @param data              Day.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ]= tm:set_tm_yday(data)
 */
static int
TM_set_tm_yday(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    tm->tm_yday = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for days since January 1 [0-365].
 *
 * @function get_tm_yday
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ]= tm:get_tm_yday()
 */
static int
TM_get_tm_yday(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = tm->tm_yday;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for Daylight Savings Time flag.
 *
 * @function set_tm_isdst
 *
 * @param data              Daylight Savings Time.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:get_tm_isdst(data)
 */
static int
TM_set_tm_isdst(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    tm->tm_isdst = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for Daylight Savings Time flag.
 *
 * @function get_tm_isdst
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:get_tm_isdst()
 */
static int
TM_get_tm_isdst(lua_State *L)
{
    struct tm *tm;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = tm->tm_isdst;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for offset from UTC in seconds.
 *
 * @function set_tm_gmtoff
 *
 * @param data              Offset.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:set_tm_gmtoff(data)
 */
static int
TM_set_tm_gmtoff(lua_State *L)
{
    struct tm *tm;
    long x;

    (void)luab_core_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = (long)luab_checkinteger(L, 2, luab_env_long_max);

    tm->tm_gmtoff = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for offset from UTC in seconds.
 *
 * @function get_tm_gmtoff
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = tm:get_tm_gmtoff()
 */
static int
TM_get_tm_gmtoff(lua_State *L)
{
    struct tm *tm;
    long x;

    (void)luab_core_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    x = tm->tm_gmtoff;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for tm abbreviation.
 *
 * @function get_tm_zone
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage zone [, err, msg ] = tm:get_tm_zone()
 */
static int
TM_tm_zone(lua_State *L)
{
    struct tm *tm;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, &luab_tm_type, struct tm *);
    dp = tm->tm_zone;

    return (luab_pushstring(L, dp));
}

/*
 * Metamethods.
 */

static int
TM_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_tm_type));
}

static int
TM_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_tm_type));
}

static int
TM_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_tm_type));
}

static luab_module_table_t tm_methods[] = {
    LUAB_FUNC("tm_zone",            TM_tm_zone),
    LUAB_FUNC("set_tm_sec",         TM_set_tm_sec),
    LUAB_FUNC("set_tm_min",         TM_set_tm_min),
    LUAB_FUNC("set_tm_hour",        TM_set_tm_hour),
    LUAB_FUNC("set_tm_mday",        TM_set_tm_mday),
    LUAB_FUNC("set_tm_mon",         TM_set_tm_mon),
    LUAB_FUNC("set_tm_year",        TM_set_tm_year),
    LUAB_FUNC("set_tm_wday",        TM_set_tm_wday),
    LUAB_FUNC("set_tm_yday",        TM_set_tm_yday),
    LUAB_FUNC("set_tm_isdst",       TM_set_tm_isdst),
    LUAB_FUNC("set_tm_gmtoff",      TM_set_tm_gmtoff),
    LUAB_FUNC("get_table",          TM_get_table),
    LUAB_FUNC("get_tm_sec",         TM_get_tm_sec),
    LUAB_FUNC("get_tm_min",         TM_get_tm_min),
    LUAB_FUNC("get_tm_hour",        TM_get_tm_hour),
    LUAB_FUNC("get_tm_mday",        TM_get_tm_mday),
    LUAB_FUNC("get_tm_mon",         TM_get_tm_mon),
    LUAB_FUNC("get_tm_year",        TM_get_tm_year),
    LUAB_FUNC("get_tm_wday",        TM_get_tm_wday),
    LUAB_FUNC("get_tm_yday",        TM_get_tm_yday),
    LUAB_FUNC("get_tm_isdst",       TM_get_tm_isdst),
    LUAB_FUNC("get_tm_gmtoff",      TM_get_tm_gmtoff),
    LUAB_FUNC("dump",               TM_dump),
    LUAB_FUNC("__gc",               TM_gc),
    LUAB_FUNC("__len",              TM_len),
    LUAB_FUNC("__tostring",         TM_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
tm_create(lua_State *L, void *arg)
{
    return (luab_new_tm(L, arg));
}

static void
tm_init(void *ud, void *arg)
{
    luab_udata_init(&luab_tm_type, ud, arg);
    luab_udata_init(&luab_tm_type, ud, arg);
}

static void *
tm_udata(lua_State *L, int narg)
{
    return (luab_to_tm(L, narg));
}

static luab_table_t *
tm_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct tm *x, *y;
    size_t m, n, sz;

    sz = sizeof(struct tm);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (struct tm *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_tm_type, struct tm *);
                        (void)memmove(&(x[m]), y, sz);
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
tm_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct tm *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (struct tm *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_tm_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

luab_module_t luab_tm_type = {
    .m_cookie   = LUAB_TM_TYPE_ID,
    .m_name     = LUAB_TM_TYPE,
    .m_vec      = tm_methods,
    .m_create   = tm_create,
    .m_init     = tm_init,
    .m_get      = tm_udata,
    .m_get_tbl  = tm_checktable,
    .m_set_tbl  = tm_pushtable,
    .m_sz       = sizeof(luab_tm_t),
};
