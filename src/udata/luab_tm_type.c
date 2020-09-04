/*
 * Copyright (c) 2020 Henning Matyschok <hmatyschok@outlook.com>
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

#include <string.h>
#include <time.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t tm_type;

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
    struct tm    tm;
} luab_tm_t;

#define luab_new_tm(L, arg) \
    ((luab_tm_t *)luab_newuserdata(L, &tm_type, (arg)))
#define luab_to_tm(L, narg) \
    (luab_toldata((L), (narg), &tm_type, \
        struct tm *, sizeof(struct tm)))

#define LUABSD_TM_TYPE_ID    1594168426
#define LUABSD_TM_TYPE    "TM*"

int luab_StructTM(lua_State *);

/***
 * Set value for seconds after the minute [0-60].
 *
 * @function set_tm_sec
 *
 * @param attr              Seconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:set_tm_sec(attr)
 */
static int
TM_set_tm_sec(lua_State *L)
{
    struct tm *tm;
    int tm_sec;

    (void)luab_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_sec = (int)luab_checkinteger(L, 2, INT_MAX);

    tm->tm_sec = tm_sec;

    return (luab_pusherr(L, tm_sec));
}

/***
 * Get value for seconds after the minute [0-60].
 *
 * @function get_tm_sec
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:get_tm_sec()
 */
static int
TM_get_tm_sec(lua_State *L)
{
    struct tm *tm;
    int tm_sec;

    (void)luab_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_sec = tm->tm_sec;

    return (luab_pusherr(L, tm_sec));
}

/***
 * Set value for minutes after the hour [0-59].
 *
 * @function set_tm_min
 *
 * @param attr              Minutes.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, min ] = tm:set_tm_min(attr)
 */
static int
TM_set_tm_min(lua_State *L)
{
    struct tm *tm;
    int tm_min;

    (void)luab_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_min = (int)luab_checkinteger(L, 2, INT_MAX);

    tm->tm_min = tm_min;

    return (luab_pusherr(L, tm_min));
}

/***
 * Get value for minutes after the hour [0-59].
 *
 * @function get_tm_min
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, min ] = tm:get_tm_min()
 */
static int
TM_get_tm_min(lua_State *L)
{
    struct tm *tm;
    int tm_min;

    (void)luab_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_min = tm->tm_min;

    return (luab_pusherr(L, tm_min));
}

/***
 * Set value for hours since midnight [0-23].
 *
 * @function set_tm_hour
 *
 * @param attr              Hour.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:set_tm_hour(attr)
 */
static int
TM_set_tm_hour(lua_State *L)
{
    struct tm *tm;
    int tm_hour;

    (void)luab_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_hour = (int)luab_checkinteger(L, 2, INT_MAX);

    tm->tm_hour = tm_hour;

    return (luab_pusherr(L, tm_hour));
}

/***
 * Get value for hours since midnight [0-23].
 *
 * @function get_tm_hour
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:get_tm_hour()
 */
static int
TM_get_tm_hour(lua_State *L)
{
    struct tm *tm;
    int tm_hour;

    (void)luab_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_hour = tm->tm_hour;

    return (luab_pusherr(L, tm_hour));
}

/***
 * Set value for day of the month [1-31].
 *
 * @function set_tm_mday
 *
 * @param attr              Day.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:set_tm_mday(attr)
 */
static int
TM_set_tm_mday(lua_State *L)
{
    struct tm *tm;
    int tm_mday;

    (void)luab_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_mday = (int)luab_checkinteger(L, 2, INT_MAX);

    tm->tm_mday = tm_mday;

    return (luab_pusherr(L, tm_mday));
}

/***
 * Get value for day of the month [1-31].
 *
 * @function get_tm_mday
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:get_tm_mday()
 */
static int
TM_get_tm_mday(lua_State *L)
{
    struct tm *tm;
    int tm_mday;

    (void)luab_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_mday = tm->tm_mday;

    return (luab_pusherr(L, tm_mday));
}

/***
 * Set value for months since January [0-11].
 *
 * @function set_tm_mon
 *
 * @param attr              Month.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:set_tm_mon(attr)
 */
static int
TM_set_tm_mon(lua_State *L)
{
    struct tm *tm;
    int tm_mon;

    (void)luab_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_mon = (int)luab_checkinteger(L, 2, INT_MAX);

    tm->tm_mon = tm_mon;

    return (luab_pusherr(L, tm_mon));
}

/***
 * Get value for months since January [0-11].
 *
 * @function get_tm_mon
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:get_tm_mon()
 */
static int
TM_get_tm_mon(lua_State *L)
{
    struct tm *tm;
    int tm_mon;

    (void)luab_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_mon = tm->tm_mon;

    return (luab_pusherr(L, tm_mon));
}

/***
 * Set value for years since 1900.
 *
 * @function set_tm_year
 *
 * @param attr              Seconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:set_tm_year(attr)
 */
static int
TM_set_tm_year(lua_State *L)
{
    struct tm *tm;
    int tm_year;

    (void)luab_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_year = (int)luab_checkinteger(L, 2, INT_MAX);

    tm->tm_year = tm_year;

    return (luab_pusherr(L, tm_year));
}

/***
 * Get value for years since 1900.
 *
 * @function get_tm_year
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:get_tm_year()
 */
static int
TM_get_tm_year(lua_State *L)
{
    struct tm *tm;
    int tm_year;

    (void)luab_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_year = tm->tm_year;

    return (luab_pusherr(L, tm_year));
}

/***
 * Set value for days since Sunday [0-6].
 *
 * @function set_tm_wday
 *
 * @param attr              Day.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:set_tm_wday(attr)
 */
static int
TM_set_tm_wday(lua_State *L)
{
    struct tm *tm;
    int tm_wday;

    (void)luab_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_wday = (int)luab_checkinteger(L, 2, INT_MAX);

    tm->tm_wday = tm_wday;

    return (luab_pusherr(L, tm_wday));
}

/***
 * Get value for days since Sunday [0-6].
 *
 * @function get_tm_wday
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:get_tm_wday()
 */
static int
TM_get_tm_wday(lua_State *L)
{
    struct tm *tm;
    int tm_wday;

    (void)luab_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_wday = tm->tm_wday;

    return (luab_pusherr(L, tm_wday));
}

/***
 * Set value for days since January 1 [0-365].
 *
 * @function set_tm_yday
 *
 * @param attr              Day.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ]= tm:set_tm_yday(attr)
 */
static int
TM_set_tm_yday(lua_State *L)
{
    struct tm *tm;
    int tm_yday;

    (void)luab_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_yday = (int)luab_checkinteger(L, 2, INT_MAX);

    tm->tm_yday = tm_yday;

    return (luab_pusherr(L, tm_yday));
}

/***
 * Get value for days since January 1 [0-365].
 *
 * @function get_tm_yday
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ]= tm:get_tm_yday()
 */
static int
TM_get_tm_yday(lua_State *L)
{
    struct tm *tm;
    int tm_yday;

    (void)luab_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_yday = tm->tm_yday;

    return (luab_pusherr(L, tm_yday));
}

/***
 * Set value for Daylight Savings Time flag.
 *
 * @function set_tm_isdst
 *
 * @param attr              Daylight Savings Time.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:get_tm_isdst(attr)
 */
static int
TM_set_tm_isdst(lua_State *L)
{
    struct tm *tm;
    int tm_isdst;

    (void)luab_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_isdst = (int)luab_checkinteger(L, 2, INT_MAX);

    tm->tm_isdst = tm_isdst;

    return (luab_pusherr(L, tm_isdst));
}

/***
 * Get value for Daylight Savings Time flag.
 *
 * @function get_tm_isdst
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:get_tm_isdst()
 */
static int
TM_get_tm_isdst(lua_State *L)
{
    struct tm *tm;
    int tm_isdst;

    (void)luab_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_isdst = tm->tm_isdst;

    return (luab_pusherr(L, tm_isdst));
}

/***
 * Set value for offset from UTC in seconds.
 *
 * @function set_tm_gmtoff
 *
 * @param attr              Offset.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:set_tm_gmtoff(attr)
 */
static int
TM_set_tm_gmtoff(lua_State *L)
{
    struct tm *tm;
    long tm_gmtoff;

    (void)luab_checkmaxargs(L, 2);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_gmtoff = (long)luab_checkinteger(L, 2, LONG_MAX);

    tm->tm_gmtoff = tm_gmtoff;

    return (luab_pusherr(L, tm_gmtoff));
}

/***
 * Get value for offset from UTC in seconds.
 *
 * @function get_tm_gmtoff
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (attr [, nil, nil]) on success or
 *          (attr, (errno, strerror(errno)))
 *
 * @usage attr [, err, msg ] = tm:get_tm_gmtoff()
 */
static int
TM_get_tm_gmtoff(lua_State *L)
{
    struct tm *tm;
    long tm_gmtoff;

    (void)luab_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_gmtoff = tm->tm_gmtoff;

    return (luab_pusherr(L, tm_gmtoff));
}

/***
 * Get value for tm abbreviation.
 *
 * @function get_tm_zone
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (zone [, nil, nil]) on success or
 *          (zone, (errno, strerror(errno)))
 *
 * @usage zone [, err, msg ] = tm:get_tm_zone()
 */
static int
TM_tm_zone(lua_State *L)
{
    struct tm *tm;
    char *tm_zone;

    (void)luab_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, tm_type, struct tm *);
    tm_zone = tm->tm_zone;

    return (luab_pushstring(L, tm_zone));
}

/***
 * Translate tm{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = tm:get()
 */
static int
TM_get(lua_State *L)
{
    struct tm *tm;

    (void)luab_checkmaxargs(L, 1);

    tm = luab_udata(L, 1, tm_type, struct tm *);

    lua_newtable(L);

    luab_setinteger(L, -2, "tm_sec", tm->tm_sec);
    luab_setinteger(L, -2, "tm_min", tm->tm_min);
    luab_setinteger(L, -2, "tm_hour", tm->tm_hour);
    luab_setinteger(L, -2, "tm_mday", tm->tm_mday);
    luab_setinteger(L, -2, "tm_mon", tm->tm_mon);
    luab_setinteger(L, -2, "tm_year", tm->tm_year);
    luab_setinteger(L, -2, "tm_wday", tm->tm_wday);
    luab_setinteger(L, -2, "tm_yday", tm->tm_yday);
    luab_setinteger(L, -2, "tm_isdst", tm->tm_isdst);
    luab_setinteger(L, -2, "tm_gmtoff", tm->tm_gmtoff);

    if (tm->tm_zone != NULL)
        luab_setstring(L, -2, "tm_zone", tm->tm_zone);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Copy tm{} into LUA_TUSERDATA(luab_iovec_t).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = tm:dump()
 */
static int
TM_dump(lua_State *L)
{
    return (luab_dump(L, 1, &tm_type, sizeof(struct tm)));
}

static int
TM_gc(lua_State *L)
{
    return (luab_gc(L, 1, &tm_type));
}

static int
TM_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &tm_type));
}

static luab_table_t tm_methods[] = {
    LUABSD_FUNC("tm_zone",  TM_tm_zone),
    LUABSD_FUNC("set_tm_sec",   TM_set_tm_sec),
    LUABSD_FUNC("set_tm_min",   TM_set_tm_min),
    LUABSD_FUNC("set_tm_hour",   TM_set_tm_hour),
    LUABSD_FUNC("set_tm_mday",   TM_set_tm_mday),
    LUABSD_FUNC("set_tm_mon",   TM_set_tm_mon),
    LUABSD_FUNC("set_tm_year",   TM_set_tm_year),
    LUABSD_FUNC("set_tm_wday",   TM_set_tm_wday),
    LUABSD_FUNC("set_tm_yday",   TM_set_tm_yday),
    LUABSD_FUNC("set_tm_isdst",   TM_set_tm_isdst),
    LUABSD_FUNC("set_tm_gmtoff",   TM_set_tm_gmtoff),
    LUABSD_FUNC("get",  TM_get),
    LUABSD_FUNC("get_tm_sec",   TM_get_tm_sec),
    LUABSD_FUNC("get_tm_min",   TM_get_tm_min),
    LUABSD_FUNC("get_tm_hour",   TM_get_tm_hour),
    LUABSD_FUNC("get_tm_mday",   TM_get_tm_mday),
    LUABSD_FUNC("get_tm_mon",   TM_get_tm_mon),
    LUABSD_FUNC("get_tm_year",   TM_get_tm_year),
    LUABSD_FUNC("get_tm_wday",   TM_get_tm_wday),
    LUABSD_FUNC("get_tm_yday",   TM_get_tm_yday),
    LUABSD_FUNC("get_tm_isdst",   TM_get_tm_isdst),
    LUABSD_FUNC("get_tm_gmtoff",   TM_get_tm_gmtoff),
    LUABSD_FUNC("dump", TM_dump),
    LUABSD_FUNC("__gc", TM_gc),
    LUABSD_FUNC("__tostring",   TM_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
tm_create(lua_State *L, void *arg)
{
    return (luab_new_tm(L, arg));
}

static void
tm_init(void *ud, void *arg)
{
    luab_tm_t *self;

    if (((self = (luab_tm_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->tm, arg, sizeof(self->tm));
}

static void *
tm_udata(lua_State *L, int narg)
{
    return (luab_to_tm(L, narg));
}

luab_module_t tm_type = {
    .cookie = LUABSD_TM_TYPE_ID,
    .name = LUABSD_TM_TYPE,
    .vec = tm_methods,
    .ctor = tm_create,
    .init = tm_init,
    .get = tm_udata,
    .sz = sizeof(luab_tm_t),
};

/***
 * Ctor.
 *
 * @function StructTM
 *
 * @param tm            Instance of LUA_TUSERDATA(luab_tm_t).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (tm [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage tm [, err, msg ] = bsd.sys.time.StructTM([ tm ])
 */
int
luab_StructTM(lua_State *L)
{
    struct tm *tm;
    int narg, status;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        tm = NULL;
    else
        tm = tm_udata(L, narg);

    if (tm_create(L, tm) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
