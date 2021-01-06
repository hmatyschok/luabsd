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

#include <uuid.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_uuid_type;

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

/*
 * Subr.
 */

static void
uuid_fillxtable(lua_State *L, int narg, void *arg)
{
    struct uuid *uuid;

    if ((uuid = (struct uuid *)arg) != NULL) {

        luab_setinteger(L, narg, "time_low",      uuid->time_low);
        luab_setinteger(L, narg, "time_mid",      uuid->time_mid);
        luab_setinteger(L, narg, "time_hi_and_version",
            uuid->time_hi_and_version);
        luab_setinteger(L, narg, "clock_seq_hi_and_reserved",
            uuid->clock_seq_hi_and_reserved);
        luab_setinteger(L, narg, "clock_seq_low", uuid->clock_seq_low);
        luab_setldata(L, narg, "node",            uuid->node, _UUID_NODE_LEN);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(UUID)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
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
 * @usage t [, err, msg ] = uuid:get_table()
 */
static int
UUID_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UUID, TYPE, __func__);

    xtp.xtp_fill = uuid_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate uuid{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = uuid:dump()
 */
static int
UUID_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(UUID, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions.
 */

/***
 * Set value for low field of the timestamp.
 *
 * @function set_time_low
 *
 * @param arg               Octets maps to 0-3.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = uuid:set_time_low(arg)
 */
static int
UUID_set_time_low(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct uuid *uuid;
    uint32_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(UUID, TYPE, __func__);
    m1 = luab_xmod(UINT32, TYPE, __func__);

    uuid = luab_udata(L, 1, m0, struct uuid *);
    x = (uint32_t)luab_checkxinteger(L, 2, m1, luab_env_int_max);

    uuid->time_low = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for low field of the timestamp.
 *
 * @function get_time_low
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = uuid:get_time_low()
 */
static int
UUID_get_time_low(lua_State *L)
{
    luab_module_t *m;
    struct uuid *uuid;
    uint32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UUID, TYPE, __func__);
    uuid = luab_udata(L, 1, m, struct uuid *);
    x = uuid->time_low;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for the middle field of the timestamp.
 *
 * @function set_time_mid
 *
 * @param arg               Octets maps to 4-5.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = uuid:set_time_mid(arg)
 */
static int
UUID_set_time_mid(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct uuid *uuid;
    uint16_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(UUID, TYPE, __func__);
    m1 = luab_xmod(UINT16, TYPE, __func__);

    uuid = luab_udata(L, 1, m0, struct uuid *);
    x = (uint16_t)luab_checkxinteger(L, 2, m1, luab_env_ushrt_max);

    uuid->time_mid = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for the middle field of the timestamp.
 *
 * @function get_time_mid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = uuid:get_time_mid()
 */
static int
UUID_get_time_mid(lua_State *L)
{
    luab_module_t *m;
    struct uuid *uuid;
    uint16_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UUID, TYPE, __func__);
    uuid = luab_udata(L, 1, m, struct uuid *);
    x = uuid->time_mid;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for the high field of the timespamp multiplexed
 * with the version number.
 *
 * @function set_time_hi_and_version
 *
 * @param arg               Octets maps to 6-7.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = uuid:set_time_hi_and_version(arg)
 */
static int
UUID_set_time_hi_and_version(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct uuid *uuid;
    uint16_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(UUID, TYPE, __func__);
    m1 = luab_xmod(UINT16, TYPE, __func__);

    uuid = luab_udata(L, 1, m0, struct uuid *);
    x = (uint16_t)luab_checkxinteger(L, 2, m1, luab_env_ushrt_max);

    uuid->time_hi_and_version = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for the high field of the timestamp multiplexed
 * with the version number.
 *
 * @function get_time_hi_and_version
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = uuid:get_time_hi_and_version()
 */
static int
UUID_get_time_hi_and_version(lua_State *L)
{
    luab_module_t *m;
    struct uuid *uuid;
    uint16_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UUID, TYPE, __func__);
    uuid = luab_udata(L, 1, m, struct uuid *);
    x = uuid->time_hi_and_version;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for the high field of the clock sequence multiplexed
 * with the variant.
 *
 * @function set_clock_seq_hi_and_reserved
 *
 * @param arg               Octet maps to 8.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = uuid:set_clock_seq_hi_and_reserved(arg)
 */
static int
UUID_set_clock_seq_hi_and_reserved(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct uuid *uuid;
    uint8_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(UUID, TYPE, __func__);
    m1 = luab_xmod(UINT8, TYPE, __func__);

    uuid = luab_udata(L, 1, m0, struct uuid *);
    x = (uint8_t)luab_checkxinteger(L, 2, m1, luab_env_uchar_max);

    uuid->clock_seq_hi_and_reserved = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for the high field of the of the clock sequence multiplexed
 * with the variant.
 *
 * @function get_clock_seq_hi_and_reserved
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = uuid:get_clock_seq_hi_and_reserved()
 */
static int
UUID_get_clock_seq_hi_and_reserved(lua_State *L)
{
    luab_module_t *m;
    struct uuid *uuid;
    uint8_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UUID, TYPE, __func__);
    uuid = luab_udata(L, 1, m, struct uuid *);
    x = uuid->clock_seq_hi_and_reserved;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for the low field of the clock sequence.
 *
 * @function set_clock_seq_low
 *
 * @param arg               Octet maps to 9.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = uuid:set_clock_seq_low(arg)
 */
static int
UUID_set_clock_seq_low(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct uuid *uuid;
    uint8_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(UUID, TYPE, __func__);
    m1 = luab_xmod(UINT8, TYPE, __func__);

    uuid = luab_udata(L, 1, m0, struct uuid *);
    x = (uint8_t)luab_checkxinteger(L, 2, m1, luab_env_uchar_max);

    uuid->clock_seq_low = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for the low field of the clock sequence.
 *
 * @function get_clock_seq_low
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = uuid:get_clock_seq_low()
 */
static int
UUID_get_clock_seq_low(lua_State *L)
{
    luab_module_t *m;
    struct uuid *uuid;
    uint8_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UUID, TYPE, __func__);
    uuid = luab_udata(L, 1, m, struct uuid *);
    x = uuid->clock_seq_low;

    return (luab_pushxinteger(L, x));
}

/***
 * Set spatially unique node identifier.
 *
 * @function set_node
 *
 * @param arg               Octets maps to 10-15, unsigned 48 bit integer.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage str [, err, msg ] = uuid:set_node(arg)
 */
static int
UUID_set_node(lua_State *L)
{
    luab_module_t *m;
    struct uuid *uuid;
    const void *dp;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(UUID, TYPE, __func__);
    uuid = luab_udata(L, 1, m, struct uuid *);
    dp = luab_iovec_checklstring(L, 2, _UUID_NODE_LEN);

    (void)memmove(uuid->node, dp, _UUID_NODE_LEN);  /* XXX */

    return (luab_pushldata(L, uuid->node, _UUID_NODE_LEN));
}

/***
 * Get spatially unique node identifier.
 *
 * @function get_node
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage str [, err, msg ] = uuid:get_node()
 */
static int
UUID_get_node(lua_State *L)
{
    luab_module_t *m;
    struct uuid *uuid;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(UUID, TYPE, __func__);
    uuid = luab_udata(L, 1, m, struct uuid *);
    dp = uuid->node;

    return (luab_pushldata(L, dp, _UUID_NODE_LEN));
}

/*
 * Metamethods.
 */

static int
UUID_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(UUID, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
UUID_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(UUID, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
UUID_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(UUID, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t uuid_methods[] = {
    LUAB_FUNC("set_time_low",   UUID_set_time_low),
    LUAB_FUNC("set_time_mid",   UUID_set_time_mid),
    LUAB_FUNC("set_time_hi_and_version",
        UUID_set_time_hi_and_version),
    LUAB_FUNC("set_clock_seq_hi_and_reserved",
        UUID_set_clock_seq_hi_and_reserved),
    LUAB_FUNC("set_clock_seq_low",
        UUID_set_clock_seq_low),
    LUAB_FUNC("set_node",       UUID_set_node),
    LUAB_FUNC("get_table",      UUID_get_table),
    LUAB_FUNC("get_time_low",   UUID_get_time_low),
    LUAB_FUNC("get_time_mid",   UUID_get_time_mid),
    LUAB_FUNC("get_time_hi_and_version",
        UUID_get_time_hi_and_version),
    LUAB_FUNC("get_clock_seq_hi_and_reserved",
        UUID_get_clock_seq_hi_and_reserved),
    LUAB_FUNC("get_clock_seq_low",
        UUID_get_clock_seq_low),
    LUAB_FUNC("get_node",       UUID_get_node),
    LUAB_FUNC("dump",           UUID_dump),
    LUAB_FUNC("__gc",           UUID_gc),
    LUAB_FUNC("__len",          UUID_len),
    LUAB_FUNC("__tostring",     UUID_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
_uuid_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(UUID, TYPE, __func__);
    return (luab_newudata(L, m, arg));
}

static void
uuid_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(UUID, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
uuid_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(UUID, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
uuid_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct uuid *x, *y;
    size_t i, j;

    m = luab_xmod(UUID, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct uuid *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct uuid *);
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
uuid_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct uuid *x;
    size_t i, j, k;

    m = luab_xmod(UUID, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct uuid *)tbl->tbl_vec) != NULL) &&
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
uuid_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(UUID, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_uuid_type = {
    .m_id           = LUAB_UUID_TYPE_ID,
    .m_name         = LUAB_UUID_TYPE,
    .m_vec          = uuid_methods,
    .m_create       = _uuid_create,
    .m_init         = uuid_init,
    .m_get          = uuid_udata,
    .m_get_tbl      = uuid_checktable,
    .m_set_tbl      = uuid_pushtable,
    .m_alloc_tbl    = uuid_alloctable,
    .m_len          = sizeof(luab_uuid_t),
    .m_sz           = sizeof(struct uuid),
};
