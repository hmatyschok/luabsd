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

#define luab_new_timespec(L, arg) \
    ((luab_timespec_t *)luab_newudata(L, &luab_timespec_type, (arg)))
#define luab_to_timespec(L, narg) \
    (luab_toldata((L), (narg), &luab_timespec_type, \
        struct timespec *, sizeof(struct timespec)))

#define LUAB_TIMESPEC_TYPE_ID    1594034844
#define LUAB_TIMESPEC_TYPE    "TIMESPEC*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(TIMESPEC)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              tv_sec  = (LUA_TNUMBER),
 *              tv_nsec = (LUA_TNUMBER),
 *          }
 *
 * @usage t = timespec:get()
 */
static int
TIMESPEC_get(lua_State *L)
{
    struct timespec *tv;

    (void)luab_core_checkmaxargs(L, 1);

    tv = luab_udata(L, 1, &luab_timespec_type, struct timespec *);

    lua_newtable(L);
    luab_setinteger(L, -2, "tv_sec",    tv->tv_sec);
    luab_setinteger(L, -2, "tv_nsec",   tv->tv_nsec);
    lua_pushvalue(L, -1);

    return (1);
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
    return (luab_core_dump(L, 1, &luab_timespec_type, sizeof(struct timespec)));
}

/*
 * Access functions.
 */

/***
 * Set value for tv_sec.
 *
 * @function set_tv_sec
 *
 * @param data              Specifies value in seconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = timespec:set_tv_sec(data)
 */
static int
TIMESPEC_set_tv_sec(lua_State *L)
{
    struct timespec *tv;
    time_t data;

    (void)luab_core_checkmaxargs(L, 2);

    tv = luab_udata(L, 1, &luab_timespec_type, struct timespec *);
    data = (time_t)luab_checkinteger(L, 2, luab_env_int_max);

    tv->tv_sec = data;

    return (luab_pushxinteger(L, data));
}

/***
 * Get value for tv_sec.
 *
 * @function get_tv_sec
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = timespec:get_tv_sec()
 */
static int
TIMESPEC_get_tv_sec(lua_State *L)
{
    struct timespec *tv;
    time_t data;

    (void)luab_core_checkmaxargs(L, 1);

    tv = luab_udata(L, 1, &luab_timespec_type, struct timespec *);
    data = tv->tv_sec;

    return (luab_pushxinteger(L, data));
}

/***
 * Set value for tv_nsec.
 *
 * @function set_tv_nsec
 *
 * @param data              Specifies value in nanoneconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = timespec:set_tv_nsec(data)
 */
static int
TIMESPEC_set_tv_nsec(lua_State *L)
{
    struct timespec *tv;
    long data;

    (void)luab_core_checkmaxargs(L, 2);

    tv = luab_udata(L, 1, &luab_timespec_type, struct timespec *);
    data = (long)luab_checkinteger(L, 2, luab_env_long_max);

    tv->tv_nsec = data;

    return (luab_pushxinteger(L, data));
}

/***
 * Get value for tv_nsec.
 *
 * @function get_tv_nsec
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = timespec:get_tv_nsec()
 */
static int
TIMESPEC_get_tv_nsec(lua_State *L)
{
    struct timespec *tv;
    long data;

    (void)luab_core_checkmaxargs(L, 1);

    tv = luab_udata(L, 1, &luab_timespec_type, struct timespec *);
    data = tv->tv_nsec;

    return (luab_pushxinteger(L, data));
}

/*
 * Metamethods.
 */

static int
TIMESPEC_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_timespec_type));
}

static int
TIMESPEC_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_timespec_type));
}

static int
TIMESPEC_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_timespec_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t timespec_methods[] = {
    LUAB_FUNC("set_tv_sec",   TIMESPEC_set_tv_sec),
    LUAB_FUNC("set_tv_nsec",  TIMESPEC_set_tv_nsec),
    LUAB_FUNC("get",          TIMESPEC_get),
    LUAB_FUNC("get_tv_sec",   TIMESPEC_get_tv_sec),
    LUAB_FUNC("get_tv_nsec",  TIMESPEC_get_tv_nsec),
    LUAB_FUNC("dump",         TIMESPEC_dump),
    LUAB_FUNC("__gc",         TIMESPEC_gc),
    LUAB_FUNC("__len",        TIMESPEC_len),
    LUAB_FUNC("__tostring",   TIMESPEC_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
timespec_create(lua_State *L, void *arg)
{
    return (luab_new_timespec(L, arg));
}

static void
timespec_init(void *ud, void *arg)
{
    luab_udata_init(&luab_timespec_type, ud, arg);
}

static void *
timespec_udata(lua_State *L, int narg)
{
    return (luab_to_timespec(L, narg));
}

static luab_table_t *
timespec_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct timespec *x, *y;
    size_t m, n, sz;

    sz = sizeof(struct timespec);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (struct timespec *)(tbl->tbl_vec)) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_timespec_type, struct timespec *);
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
timespec_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct timespec *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_timespec_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

luab_module_t luab_timespec_type = {
    .m_cookie   = LUAB_TIMESPEC_TYPE_ID,
    .m_name     = LUAB_TIMESPEC_TYPE,
    .m_vec      = timespec_methods,
    .m_create   = timespec_create,
    .m_init     = timespec_init,
    .m_get      = timespec_udata,
    .m_get_tbl  = timespec_checktable,
    .m_set_tbl  = timespec_pushtable,
    .m_sz       = sizeof(luab_timespec_t),
};