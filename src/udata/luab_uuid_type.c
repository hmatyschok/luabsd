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

#include <uuid.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_types.h"

extern luab_module_t uuid_type;

/*
 * Interface against
 *
 *  struct uuid {
 *      uint32_t    time_low;
 *      uint16_t    time_mid;
 *      uint16_t    time_hi_and_version;
 *      uint8_t     clock_seq_hi_and_reserved;
 *      uint8_t     clock_seq_low;
 *      uint8_t     node[_UUID_NODE_LEN];
 *  };
 */

typedef struct luab_uuid {
    luab_udata_t    ud_softc;
    struct uuid     ud_uuid;
} luab_uuid_t;

#define luab_new_uuid(L, arg) \
    ((luab_uuid_t *)luab_newudata(L, &uuid_type, (arg)))
#define luab_to_uuid(L, narg) \
    (luab_toldata((L), (narg), &uuid_type, \
        struct uuid *, sizeof(struct uuid)))

#define LUAB_UUID_TYPE_ID    1599304529
#define LUAB_UUID_TYPE    "UUID*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(UUID)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              time_low                    = (LUA_TNUMBER),
 *              time_mid                    = (LUA_TNUMBER),
 *              time_hi_and_version         = (LUA_TNUMBER),
 *              clock_seq_hi_and_reserved   = (LUA_TNUMBER),
 *              clock_low                   = (LUA_TNUMBER),
 *              node                        = (LUA_TUSERDATA(IOVEC)),
 *          }
 *
 * @usage t = uuid:get()
 */
static int
UUID_get(lua_State *L)
{
    struct uuid *uuid;

    (void)luab_checkmaxargs(L, 1);

    uuid = luab_udata(L, 1, &uuid_type, struct uuid *);

    lua_newtable(L);

    luab_setinteger(L, -2, "time_low",      uuid->time_low);
    luab_setinteger(L, -2, "time_mid",      uuid->time_mid);
    luab_setinteger(L, -2, "time_hi_and_version",
        uuid->time_hi_and_version);
    luab_setinteger(L, -2, "clock_seq_hi_and_reserved",
        uuid->clock_seq_hi_and_reserved);
    luab_setinteger(L, -2, "clock_seq_low", uuid->clock_seq_low);
    luab_setldata(L, -2, "node",            uuid->node, _UUID_NODE_LEN);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate uuid{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = uuid:dump()
 */
static int
UUID_dump(lua_State *L)
{
    return (luab_dump(L, 1, &uuid_type, sizeof(struct uuid)));
}

/*
 * Accessor.
 */

/***
 * Set value for low field of the timestamp.
 *
 * @function set_time_low
 *
 * @param data              Octets maps to 0-3.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = uuid:set_time_low(data)
 */
static int
UUID_set_time_low(lua_State *L)
{
    struct uuid *uuid;
    uint32_t data;

    (void)luab_checkmaxargs(L, 2);

    uuid = luab_udata(L, 1, &uuid_type, struct uuid *);
    data = (uint32_t)luab_checkinteger(L, 2, INT_MAX);

    uuid->time_low = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for low field of the timestamp.
 *
 * @function get_time_low
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = uuid:get_time_low()
 */
static int
UUID_get_time_low(lua_State *L)
{
    struct uuid *uuid;
    uint32_t data;

    (void)luab_checkmaxargs(L, 1);

    uuid = luab_udata(L, 1, &uuid_type, struct uuid *);
    data = uuid->time_low;

    return (luab_pusherr(L, data));
}

/***
 * Set value for the middle field of the timestamp.
 *
 * @function set_time_mid
 *
 * @param data              Octets maps to 4-5.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = uuid:set_time_mid(data)
 */
static int
UUID_set_time_mid(lua_State *L)
{
    struct uuid *uuid;
    uint16_t data;

    (void)luab_checkmaxargs(L, 2);

    uuid = luab_udata(L, 1, &uuid_type, struct uuid *);
    data = (uint16_t)luab_checkinteger(L, 2, SHRT_MAX);

    uuid->time_mid = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for the middle field of the timestamp.
 *
 * @function get_time_mid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = uuid:get_time_mid()
 */
static int
UUID_get_time_mid(lua_State *L)
{
    struct uuid *uuid;
    uint16_t data;

    (void)luab_checkmaxargs(L, 1);

    uuid = luab_udata(L, 1, &uuid_type, struct uuid *);
    data = uuid->time_mid;

    return (luab_pusherr(L, data));
}

/***
 * Set value for the high field of the timespamp multiplexed
 * with the version number.
 *
 * @function set_time_hi_and_version
 *
 * @param data              Octets maps to 6-7.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = uuid:set_time_hi_and_version(data)
 */
static int
UUID_set_time_hi_and_version(lua_State *L)
{
    struct uuid *uuid;
    uint16_t data;

    (void)luab_checkmaxargs(L, 2);

    uuid = luab_udata(L, 1, &uuid_type, struct uuid *);
    data = (uint16_t)luab_checkinteger(L, 2, SHRT_MAX);

    uuid->time_hi_and_version = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for the high field of the timestamp multiplexed
 * with the version number.
 *
 * @function get_time_hi_and_version
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = uuid:get_time_hi_and_version()
 */
static int
UUID_get_time_hi_and_version(lua_State *L)
{
    struct uuid *uuid;
    uint16_t data;

    (void)luab_checkmaxargs(L, 1);

    uuid = luab_udata(L, 1, &uuid_type, struct uuid *);
    data = uuid->time_hi_and_version;

    return (luab_pusherr(L, data));
}

/***
 * Set value for the high field of the clock sequence multiplexed
 * with the variant.
 *
 * @function set_clock_seq_hi_and_reserved
 *
 * @param data              Octet maps to 8.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = uuid:set_clock_seq_hi_and_reserved(data)
 */
static int
UUID_set_clock_seq_hi_and_reserved(lua_State *L)
{
    struct uuid *uuid;
    uint8_t data;

    (void)luab_checkmaxargs(L, 2);

    uuid = luab_udata(L, 1, &uuid_type, struct uuid *);
    data = (uint8_t)luab_checkinteger(L, 2, CHAR_MAX);

    uuid->clock_seq_hi_and_reserved = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for the high field of the of the clock sequence multiplexed
 * with the variant.
 *
 * @function get_clock_seq_hi_and_reserved
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = uuid:get_clock_seq_hi_and_reserved()
 */
static int
UUID_get_clock_seq_hi_and_reserved(lua_State *L)
{
    struct uuid *uuid;
    uint8_t data;

    (void)luab_checkmaxargs(L, 1);

    uuid = luab_udata(L, 1, &uuid_type, struct uuid *);
    data = uuid->clock_seq_hi_and_reserved;

    return (luab_pusherr(L, data));
}

/***
 * Set value for the low field of the clock sequence.
 *
 * @function set_clock_seq_low
 *
 * @param data              Octet maps to 9.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = uuid:set_clock_seq_low(data)
 */
static int
UUID_set_clock_seq_low(lua_State *L)
{
    struct uuid *uuid;
    uint8_t data;

    (void)luab_checkmaxargs(L, 2);

    uuid = luab_udata(L, 1, &uuid_type, struct uuid *);
    data = (uint8_t)luab_checkinteger(L, 2, CHAR_MAX);

    uuid->clock_seq_low = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for the low field of the clock sequence.
 *
 * @function get_clock_seq_low
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = uuid:get_clock_seq_low()
 */
static int
UUID_get_clock_seq_low(lua_State *L)
{
    struct uuid *uuid;
    uint8_t data;

    (void)luab_checkmaxargs(L, 1);

    uuid = luab_udata(L, 1, &uuid_type, struct uuid *);
    data = uuid->clock_seq_low;

    return (luab_pusherr(L, data));
}

/***
 * Set spatially unique node identifier.
 *
 * @function set_node
 *
 * @param data              Octets maps to 10-15, unsigned 48 bit integer.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (str [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage str [, err, msg ] = uuid:set_node(data)
 */
static int
UUID_set_node(lua_State *L)
{
    struct uuid *uuid;
    const void *data;

    (void)luab_checkmaxargs(L, 2);

    uuid = luab_udata(L, 1, &uuid_type, struct uuid *);
    data = luab_iovec_checklxarg(L, 2, _UUID_NODE_LEN);

    (void)memmove(uuid->node, data, _UUID_NODE_LEN);

    return (luab_pushldata(L, uuid->node, _UUID_NODE_LEN));
}

/***
 * Get spatially unique node identifier.
 *
 * @function get_node
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (str [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage str [, err, msg ] = uuid:get_node()
 */
static int
UUID_get_node(lua_State *L)
{
    struct uuid *uuid;
    caddr_t data;

    (void)luab_checkmaxargs(L, 1);

    uuid = luab_udata(L, 1, &uuid_type, struct uuid *);
    data = uuid->node;

    return (luab_pushldata(L, data, _UUID_NODE_LEN));
}

/*
 * Metamethods.
 */

static int
UUID_gc(lua_State *L)
{
    return (luab_gc(L, 1, &uuid_type));
}

static int
UUID_len(lua_State *L)
{
    return (luab_len(L, 2, &uuid_type));
}

static int
UUID_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &uuid_type));
}

/*
 * Internal interface.
 */

static luab_table_t uuid_methods[] = {
    LUAB_FUNC("set_time_low", UUID_set_time_low),
    LUAB_FUNC("set_time_mid", UUID_set_time_mid),
    LUAB_FUNC("set_time_hi_and_version",
        UUID_set_time_hi_and_version),
    LUAB_FUNC("set_clock_seq_hi_and_reserved",
        UUID_set_clock_seq_hi_and_reserved),
    LUAB_FUNC("set_clock_seq_low",
        UUID_set_clock_seq_low),
    LUAB_FUNC("set_node",     UUID_set_node),
    LUAB_FUNC("get",          UUID_get),
    LUAB_FUNC("get_time_low", UUID_get_time_low),
    LUAB_FUNC("get_time_mid", UUID_get_time_mid),
    LUAB_FUNC("get_time_hi_and_version",
        UUID_get_time_hi_and_version),
    LUAB_FUNC("get_clock_seq_hi_and_reserved",
        UUID_get_clock_seq_hi_and_reserved),
    LUAB_FUNC("get_clock_seq_low",
        UUID_get_clock_seq_low),
    LUAB_FUNC("get_node",     UUID_get_node),
    LUAB_FUNC("dump",         UUID_dump),
    LUAB_FUNC("__gc",         UUID_gc),
    LUAB_FUNC("__len",        UUID_len),
    LUAB_FUNC("__tostring",   UUID_tostring),
    LUAB_FUNC(NULL, NULL)
};

static void *
_uuid_create(lua_State *L, void *arg)
{
    return (luab_new_uuid(L, arg));
}

static void
uuid_init(void *ud, void *arg)
{
    luab_uuid_t *self;

    if (((self = (luab_uuid_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->ud_uuid, arg, sizeof(self->ud_uuid));
}

static void *
uuid_udata(lua_State *L, int narg)
{
    return (luab_to_uuid(L, narg));
}

luab_module_t uuid_type = {
    .cookie = LUAB_UUID_TYPE_ID,
    .name = LUAB_UUID_TYPE,
    .vec = uuid_methods,
    .create = _uuid_create,
    .init = uuid_init,
    .get = uuid_udata,
    .sz = sizeof(luab_uuid_t),
};
