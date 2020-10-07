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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_types.h"

#ifdef __BSD_VISIBLE
extern luab_module_t bintime_type;

/*
 * Interface against
 *
 *  struct bintime {
 *      time_t  sec;
 *      uint64_t frac;
 *  };
 */

typedef struct luab_bintime {
    luab_udata_t    ud_softc;
    struct bintime  ud_bt;
} luab_bintime_t;

#define luab_new_bintime(L, arg) \
    ((luab_bintime_t *)luab_newudata(L, &bintime_type, (arg)))
#define luab_to_bintime(L, narg) \
    (luab_toldata((L), (narg), &bintime_type, \
        struct bintime *, sizeof(struct bintime)))

#define LUAB_BINTIME_TYPE_ID    1594161740
#define LUAB_BINTIME_TYPE    "BINTIME*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(BINTIME)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              sec     = (LUA_TNUMBER),
 *              frac    = (LUA_TNUMBER),
 *          }
 *
 * @usage t = bintime:get()
 */
static int
BINTIME_get(lua_State *L)
{
    struct bintime *bt;

    (void)luab_checkmaxargs(L, 1);

    bt = luab_udata(L, 1, &bintime_type, struct bintime *);

    lua_newtable(L);
    luab_setinteger(L, -2, "sec",   bt->sec);
    luab_setinteger(L, -2, "frac",  bt->frac);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate bintime{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = bintime:dump()
 */
static int
BINTIME_dump(lua_State *L)
{
    return (luab_dump(L, 1, &bintime_type, sizeof(struct bintime)));
}

/*
 * Access functions.
 */

/***
 * Set value for sytem time.
 *
 * @function set_sec
 *
 * @param data              Seconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = bintime:set_sec(data)
 */
static int
BINTIME_set_sec(lua_State *L)
{
    struct bintime *bt;
    time_t data;

    (void)luab_checkmaxargs(L, 2);

    bt = luab_udata(L, 1, &bintime_type, struct bintime *);
    data = (time_t)luab_checkinteger(L, 2, INT_MAX);

    bt->sec = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for system time.
 *
 * @function get_sec
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = bintime:get_sec()
 */
static int
BINTIME_get_sec(lua_State *L)
{
    struct bintime *bt;
    time_t data;

    (void)luab_checkmaxargs(L, 1);

    bt = luab_udata(L, 1, &bintime_type, struct bintime *);
    data = bt->sec;

    return (luab_pusherr(L, data));
}

/***
 * Set value for for system time.
 *
 * @function set_frac
 *
 * @param data              Specifies frac.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = bintime:set_frac(data)
 */
static int
BINTIME_set_frac(lua_State *L)
{
    struct bintime *bt;
    uint64_t data;

    (void)luab_checkmaxargs(L, 2);

    bt = luab_udata(L, 1, &bintime_type, struct bintime *);
    data = (uint64_t)luab_checkinteger(L, 2, LONG_MAX);

    bt->frac = data;

    return (luab_pusherr(L, data));
}

/***
 * Get value for for system time.
 *
 * @function get_frac
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = bintime:get_frac()
 */
static int
BINTIME_get_frac(lua_State *L)
{
    struct bintime *bt;
    uint64_t data;

    (void)luab_checkmaxargs(L, 1);

    bt = luab_udata(L, 1, &bintime_type, struct bintime *);
    data = bt->frac;

    return (luab_pusherr(L, data));
}

/*
 * Meta-methods.
 */

static int
BINTIME_gc(lua_State *L)
{
    return (luab_gc(L, 1, &bintime_type));
}

static int
BINTIME_len(lua_State *L)
{
    return (luab_len(L, 2, &bintime_type));
}

static int
BINTIME_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &bintime_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t bintime_methods[] = {
    LUAB_FUNC("set_sec",      BINTIME_set_sec),
    LUAB_FUNC("set_frac",     BINTIME_set_frac),
    LUAB_FUNC("get",          BINTIME_get),
    LUAB_FUNC("get_sec",      BINTIME_get_sec),
    LUAB_FUNC("get_frac",     BINTIME_get_frac),
    LUAB_FUNC("dump",         BINTIME_dump),
    LUAB_FUNC("__gc",         BINTIME_gc),
    LUAB_FUNC("__len",        BINTIME_len),
    LUAB_FUNC("__tostring",   BINTIME_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
bintime_create(lua_State *L, void *arg)
{
    return (luab_new_bintime(L, arg));
}

static void
bintime_init(void *ud, void *arg)
{
    luab_udata_init(&bintime_type, ud, arg);
}

static void *
bintime_udata(lua_State *L, int narg)
{
    return (luab_to_bintime(L, narg));
}

luab_module_t bintime_type = {
    .m_cookie   = LUAB_BINTIME_TYPE_ID,
    .m_name     = LUAB_BINTIME_TYPE,
    .m_vec      = bintime_methods,
    .m_create   = bintime_create,
    .m_init     = bintime_init,
    .m_get      = bintime_udata,
    .m_sz       = sizeof(luab_bintime_t),
};
#endif /* __BSD_VISIBLE */
