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
    struct timezone    timezone;
} luab_timezone_t;

#define luab_new_timezone(L, arg) \
    ((luab_timezone_t *)luab_newuserdata(L, &timezone_type, (arg)))
#define luab_to_timezone(L, narg) \
    (luab_toldata((L), (narg), &timezone_type, \
        luab_timezone_t *, sizeof(struct timezone)))

#define LUABSD_TIMEZONE_TYPE_ID    1594159943
#define LUABSD_TIMEZONE_TYPE    "TIMEZONE*"

int luab_StructTimeZone(lua_State *);

/***
 * Set value for minutes west of Greenwich.
 *
 * @function set_tz_minuteswest
 *
 * @param zone          Specifies value in minutes.
 *
 * @usage timezone:set_tz_minuteswest(min)
 */
static int
TimeZone_set_tz_minuteswest(lua_State *L)
{
    struct timezone *tz;
    int tz_minuteswest;

    (void)luab_checkmaxargs(L, 2);

    tz = luab_udata(L, 1, timezone_type, struct timezone *);
    tz_minuteswest = (int)luab_checkinteger(L, 2, INT_MAX);

    tz->tz_minuteswest = tz_minuteswest;

    return (0);
}

/***
 * Get value for minutes west of Greenwich.
 *
 * @function get_tz_minuteswest
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (zone [, nil, nil]) on success or
 *          (zone, (errno, strerror(errno)))
 *
 * @usage zone [, err, msg ] = timezone:get_tz_minuteswest()
 */
static int
TimeZone_get_tz_minuteswest(lua_State *L)
{
    struct timezone *tz;
    int tz_minuteswest;

    (void)luab_checkmaxargs(L, 1);

    tz = luab_udata(L, 1, timezone_type, struct timezone *);
    tz_minuteswest = tz->tz_minuteswest;

    return (luab_pusherr(L, tz_minuteswest));
}

/***
 * Set value for type of dst correction.
 *
 * @function set_tz_dsttime
 *
 * @param dst           Specifies dst correction.
 *
 * @usage timezone:set_tz_dsttime(dst)
 */
static int
TimeZone_set_tz_dsttime(lua_State *L)
{
    struct timezone *tz;
    int tz_dsttime;

    (void)luab_checkmaxargs(L, 2);

    tz = luab_udata(L, 1, timezone_type, struct timezone *);
    tz_dsttime = (int)luab_checkinteger(L, 2, INT_MAX);

    tz->tz_dsttime = tz_dsttime;

    return (0);
}

/***
 * Get value for type of dst correction.
 *
 * @function get_tz_dsttime
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (dst [, nil, nil]) on success or
 *          (dst, (errno, strerror(errno)))
 *
 * @usage dst [, err, msg ] = timezone:get_tz_dsttime()
 */
static int
TimeZone_get_tz_dsttime(lua_State *L)
{
    struct timezone *tz;
    int tz_dsttime;

    (void)luab_checkmaxargs(L, 1);

    tz = luab_udata(L, 1, timezone_type, struct timezone *);
    tz_dsttime = tz->tz_dsttime;

    return (luab_pusherr(L, tz_dsttime));
}

/***
 * Translate timezone{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = timezone:get()
 */
static int
TimeZone_get(lua_State *L)
{
    struct timezone *tz;

    (void)luab_checkmaxargs(L, 1);

    tz = luab_udata(L, 1, timezone_type, struct timezone *);

    lua_newtable(L);

    luab_setinteger(L, -2, "tz_minuteswest", tz->tz_minuteswest);
    luab_setinteger(L, -2, "tz_dsttime", tz->tz_dsttime);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Copy timezone{} into LUA_TUSERDATA(luab_iovec_t).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = timezone:dump()
 */
static int
TimeZone_dump(lua_State *L)
{
    luab_iovec_param_t iop;
    size_t len, max_len;
    caddr_t data;
    int status;

    (void)luab_checkmaxargs(L, 1);

    (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

    data = luab_udata(L, 1, timezone_type, caddr_t);

    len = sizeof(struct timezone);
    max_len = len + sizeof(uint32_t);

    iop.iop_buf.buf_len = max_len;
    iop.iop_data.buf_data = data;
    iop.iop_data.buf_len = len;

    if ((*iovec_type.ctor)(L, &iop) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

static int
TimeZone_gc(lua_State *L)
{
    luab_timezone_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_timezone(L, 1);

    (void)memset_s(self, timezone_type.sz, 0, timezone_type.sz);

    return (0);
}

static int
TimeZone_tostring(lua_State *L)
{
    luab_timezone_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_timezone(L, 1);
    lua_pushfstring(L, "timezone (%p)", self);

    return (1);
}

static luab_table_t timezone_methods[] = {
    LUABSD_FUNC("set_tz_minuteswest",   TimeZone_set_tz_minuteswest),
    LUABSD_FUNC("set_tz_dsttime",   TimeZone_set_tz_dsttime),
    LUABSD_FUNC("get",  TimeZone_get),
    LUABSD_FUNC("get_tz_minuteswest",   TimeZone_get_tz_minuteswest),
    LUABSD_FUNC("get_tz_dsttime",   TimeZone_get_tz_dsttime),
    LUABSD_FUNC("dump", TimeZone_dump),
    LUABSD_FUNC("__gc", TimeZone_gc),
    LUABSD_FUNC("__tostring",   TimeZone_tostring),
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
    luab_timezone_t *self = luab_to_timezone(L, narg);

    return (&self->timezone);
}

luab_module_t timezone_type = {
    .cookie = LUABSD_TIMEZONE_TYPE_ID,
    .name = LUABSD_TIMEZONE_TYPE,
    .vec = timezone_methods,
    .ctor = timezone_create,
    .init = timezone_init,
    .get = timezone_udata,
    .sz = sizeof(luab_timezone_t),
};

/***
 * Ctor.
 *
 * @function StructTimeZone
 *
 * @param timezone              Instance of LUA_TUSERDATE(luab_timezone_t).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage timezone [, err, msg ] = bsd.sys.time.StructTimeZone([ timezone ])
 */
int
luab_StructTimeZone(lua_State *L)
{
    struct timezone *timezone;
    int narg, status;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        timezone = NULL;
    else
        timezone = timezone_udata(L, narg);

    if (timezone_create(L, timezone) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
