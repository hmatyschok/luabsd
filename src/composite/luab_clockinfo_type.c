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

extern luab_module_t luab_clockinfo_type;

/*
 * Interface against getkerninfo clock information structure
 *
 *  struct clockinfo {
 *      int hz;
 *      int tick;
 *      int spare;
 *      int stathz;
 *      int profhz;
 *  };
 */

typedef struct luab_clockinfo {
    luab_udata_t        ud_softc;
    struct clockinfo    ud_info;
} luab_clockinfo_t;

#define luab_new_clockinfo(L, arg) \
    ((luab_clockinfo_t *)luab_newudata(L, &luab_clockinfo_type, (arg)))
#define luab_to_clockinfo(L, narg) \
    (luab_toldata((L), (narg), &luab_clockinfo_type, \
        struct clockinfo *, sizeof(struct clockinfo)))

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(CLOCKINFO)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              hz      = (LUA_TNUMBER),
 *              tick    = (LUA_TNUMBER),
 *              spare   = (LUA_TNUMBER),
 *              stathz  = (LUA_TNUMBER),
 *              profhz  = (LUA_TNUMBER),
 *          }
 *
 * @usage t = clockinfo:get()
 */
static int
CLOCKINFO_get(lua_State *L)
{
    struct clockinfo *ci;

    (void)luab_core_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, &luab_clockinfo_type, struct clockinfo *);

    lua_newtable(L);

    luab_setinteger(L, -2, "hz",        ci->hz);
    luab_setinteger(L, -2, "tick",      ci->tick);
    luab_setinteger(L, -2, "stathz",    ci->stathz);
    luab_setinteger(L, -2, "profhz",    ci->stathz);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate clockinfo{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = clockinfo:dump()
 */
static int
CLOCKINFO_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_clockinfo_type, sizeof(struct clockinfo)));
}

/*
 * Access functions.
 */

/***
 * Set clock frequency.
 *
 * @function set_hz
 *
 * @param data              Frequency.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = clockinfo:set_hz(data)
 */
static int
CLOCKINFO_set_hz(lua_State *L)
{
    struct clockinfo *ci;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    ci = luab_udata(L, 1, &luab_clockinfo_type, struct clockinfo *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    ci->hz = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get clock frequency.
 *
 * @function get_hz
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = clockinfo:get_hz()
 */
static int
CLOCKINFO_get_hz(lua_State *L)
{
    struct clockinfo *ci;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, &luab_clockinfo_type, struct clockinfo *);
    x = ci->hz;

    return (luab_pushxinteger(L, x));
}

/***
 * Set micro-seconds per hz tick.
 *
 * @function set_tick
 *
 * @param data              Tick.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = clockinfo:set_tick(data)
 */
static int
CLOCKINFO_set_tick(lua_State *L)
{
    struct clockinfo *ci;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    ci = luab_udata(L, 1, &luab_clockinfo_type, struct clockinfo *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    ci->tick = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get micro-seconds per hz tick.
 *
 * @function get_tick
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = clockinfo:get_tick()
 */
static int
CLOCKINFO_get_tick(lua_State *L)
{
    struct clockinfo *ci;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, &luab_clockinfo_type, struct clockinfo *);
    x = ci->tick;

    return (luab_pushxinteger(L, x));
}

/***
 * Set statistics clock frequency.
 *
 * @function set_stathz
 *
 * @param data              Frequency.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = clockinfo:set_stathz(data)
 */
static int
CLOCKINFO_set_stathz(lua_State *L)
{
    struct clockinfo *ci;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    ci = luab_udata(L, 1, &luab_clockinfo_type, struct clockinfo *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    ci->stathz = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get statistics clock frequency.
 *
 * @function get_stathz
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = clockinfo:get_stathz()
 */
static int
CLOCKINFO_get_stathz(lua_State *L)
{
    struct clockinfo *ci;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, &luab_clockinfo_type, struct clockinfo *);
    x = ci->stathz;

    return (luab_pushxinteger(L, x));
}

/***
 * Set profiling clock frequency.
 *
 * @function set_profhz
 *
 * @param data              Frequency.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = clockinfo:set_profhz(data)
 */
static int
CLOCKINFO_set_profhz(lua_State *L)
{
    struct clockinfo *ci;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    ci = luab_udata(L, 1, &luab_clockinfo_type, struct clockinfo *);
    x = (int)luab_checkinteger(L, 2, luab_env_int_max);

    ci->profhz = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get profiling clock frequency.
 *
 * @function get_profhz
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = clockinfo:get_profhz()
 */
static int
CLOCKINFO_get_profhz(lua_State *L)
{
    struct clockinfo *ci;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    ci = luab_udata(L, 1, &luab_clockinfo_type, struct clockinfo *);
    x = ci->profhz;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
CLOCKINFO_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_clockinfo_type));
}

static int
CLOCKINFO_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_clockinfo_type));
}

static int
CLOCKINFO_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_clockinfo_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t clockinfo_methods[] = {
    LUAB_FUNC("set_hz",         CLOCKINFO_set_hz),
    LUAB_FUNC("set_tickc",      CLOCKINFO_set_tick),
    LUAB_FUNC("set_stathz",     CLOCKINFO_set_stathz),
    LUAB_FUNC("set_profhz",     CLOCKINFO_set_profhz),
    LUAB_FUNC("get",            CLOCKINFO_get),
    LUAB_FUNC("get_hz",         CLOCKINFO_get_hz),
    LUAB_FUNC("get_tick",       CLOCKINFO_get_tick),
    LUAB_FUNC("get_stathz",     CLOCKINFO_get_stathz),
    LUAB_FUNC("get_profhz",     CLOCKINFO_get_profhz),
    LUAB_FUNC("dump",           CLOCKINFO_dump),
    LUAB_FUNC("__gc",           CLOCKINFO_gc),
    LUAB_FUNC("__len",          CLOCKINFO_len),
    LUAB_FUNC("__tostring",     CLOCKINFO_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
clockinfo_create(lua_State *L, void *arg)
{
    return (luab_new_clockinfo(L, arg));
}

static void
clockinfo_init(void *ud, void *arg)
{
    luab_udata_init(&luab_clockinfo_type, ud, arg);
}

static void *
clockinfo_udata(lua_State *L, int narg)
{
    return (luab_to_clockinfo(L, narg));
}

static luab_table_t *
clockinfo_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct clockinfo *x, *y;
    size_t m, n, sz;

    sz = sizeof(struct clockinfo);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (struct clockinfo *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_clockinfo_type, struct clockinfo *);
                        (void)memmove(&(x[m]), y, sz);
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
clockinfo_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct clockinfo *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (struct clockinfo *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_clockinfo_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

luab_module_t luab_clockinfo_type = {
    .m_cookie   = LUAB_CLOCKINFO_TYPE_ID,
    .m_name     = LUAB_CLOCKINFO_TYPE,
    .m_vec      = clockinfo_methods,
    .m_create   = clockinfo_create,
    .m_init     = clockinfo_init,
    .m_get      = clockinfo_udata,
    .m_get_tbl  = clockinfo_checktable,
    .m_set_tbl  = clockinfo_pushtable,
    .m_sz       = sizeof(luab_clockinfo_t),
};
