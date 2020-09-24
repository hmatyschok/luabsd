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

#include <sys/time.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t timezone_type;

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
    struct timezone    timezone;
} luab_timezone_t;

#define luab_new_timezone(L, arg) \
    ((luab_timezone_t *)luab_newuserdata(L, &timezone_type, (arg)))
#define luab_to_timezone(L, narg) \
    (luab_toldata((L), (narg), &timezone_type, \
        struct timezone *, sizeof(struct timezone)))

#define LUABSD_TIMEZONE_TYPE_ID    1594159943
#define LUABSD_TIMEZONE_TYPE    "TIMEZONE*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(TIMEZONE)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              tz_minuteswest  = (LUA_TNUMBER),
 *              tz_dsttime      = (LUA_TNUMBER),
 *          }
 *
 * @usage t = timezone:get()
 */
static int
TIMEZONE_get(lua_State *L)
{
    struct timezone *tz;

    (void)luab_checkmaxargs(L, 1);

    tz = luab_udata(L, 1, &timezone_type, struct timezone *);

    lua_newtable(L);
    luab_setinteger(L, -2, "tz_minuteswest",    tz->tz_minuteswest);
    luab_setinteger(L, -2, "tz_dsttime",        tz->tz_dsttime);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate timezone{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = timezone:dump()
 */
static int
TIMEZONE_dump(lua_State *L)
{
    return (luab_dump(L, 1, &timezone_type, sizeof(struct timezone)));
}

/*
 * Accessor.
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
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = timezone:get_tz_minuteswest(data)
 */
static int
TIMEZONE_set_tz_minuteswest(lua_State *L)
{
    struct timezone *tz;
    int data;

    (void)luab_checkmaxargs(L, 2);

    tz = luab_udata(L, 1, &timezone_type, struct timezone *);
    data = (int)luab_checkinteger(L, 2, INT_MAX);

    tz->tz_minuteswest = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for minutes west of Greenwich.
 *
 * @function get_tz_minuteswest
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = timezone:get_tz_minuteswest()
 */
static int
TIMEZONE_get_tz_minuteswest(lua_State *L)
{
    struct timezone *tz;
    int data;

    (void)luab_checkmaxargs(L, 1);

    tz = luab_udata(L, 1, &timezone_type, struct timezone *);
    data = tz->tz_minuteswest;

    return (luab_pusherr(L, data));
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
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = timezone:set_tz_dsttime(data)
 */
static int
TIMEZONE_set_tz_dsttime(lua_State *L)
{
    struct timezone *tz;
    int data;

    (void)luab_checkmaxargs(L, 2);

    tz = luab_udata(L, 1, &timezone_type, struct timezone *);
    data = (int)luab_checkinteger(L, 2, INT_MAX);

    tz->tz_dsttime = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for type of dst correction.
 *
 * @function get_tz_dsttime
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = timezone:get_tz_dsttime()
 */
static int
TIMEZONE_get_tz_dsttime(lua_State *L)
{
    struct timezone *tz;
    int data;

    (void)luab_checkmaxargs(L, 1);

    tz = luab_udata(L, 1, &timezone_type, struct timezone *);
    data = tz->tz_dsttime;

    return (luab_pusherr(L, data));
}

/*
 * Meta-methods.
 */

static int
TIMEZONE_gc(lua_State *L)
{
    return (luab_gc(L, 1, &timezone_type));
}

static int
TIMEZONE_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &timezone_type));
}

/*
 * Internal interface.
 */

static luab_table_t timezone_methods[] = {
    LUABSD_FUNC("set_tz_minuteswest",   TIMEZONE_set_tz_minuteswest),
    LUABSD_FUNC("set_tz_dsttime",       TIMEZONE_set_tz_dsttime),
    LUABSD_FUNC("get",                  TIMEZONE_get),
    LUABSD_FUNC("get_tz_minuteswest",   TIMEZONE_get_tz_minuteswest),
    LUABSD_FUNC("get_tz_dsttime",       TIMEZONE_get_tz_dsttime),
    LUABSD_FUNC("dump",                 TIMEZONE_dump),
    LUABSD_FUNC("__gc",                 TIMEZONE_gc),
    LUABSD_FUNC("__tostring",           TIMEZONE_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
timezone_create(lua_State *L, void *arg)
{
    return (luab_new_timezone(L, arg));
}

static void
timezone_init(void *ud, void *arg)
{
    luab_timezone_t *self;

    if (((self = (luab_timezone_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->timezone, arg, sizeof(self->timezone));
}

static void *
timezone_udata(lua_State *L, int narg)
{
    return (luab_to_timezone(L, narg));
}

luab_module_t timezone_type = {
    .cookie = LUABSD_TIMEZONE_TYPE_ID,
    .name = LUABSD_TIMEZONE_TYPE,
    .vec = timezone_methods,
    .create = timezone_create,
    .init = timezone_init,
    .get = timezone_udata,
    .sz = sizeof(luab_timezone_t),
};
