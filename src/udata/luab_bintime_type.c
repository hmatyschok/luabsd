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
    struct bintime    bintime;
} luab_bintime_t;

#define luab_new_bintime(L, arg) \
    ((luab_bintime_t *)luab_newuserdata(L, &bintime_type, (arg)))
#define luab_to_bintime(L, narg) \
    (luab_todata((L), (narg), &bintime_type, luab_bintime_t *))

#define LUABSD_BINTIME_TYPE_ID    1594161740
#define LUABSD_BINTIME_TYPE    "BINTIME*"

int luab_StructBinTime(lua_State *);

/***
 * Set value for sytem time.
 *
 * @function set_sec
 *
 * @param sec           Seconds.
 *
 * @usage bintime:set_sec(sec)
 */
static int
BinTime_set_sec(lua_State *L)
{
    struct bintime *bt;
    int sec;

    luab_checkmaxargs(L, 2);

    bt = (struct bintime *)(*bintime_type.get)(L, 1);
    sec = luab_checkinteger(L, 2, INT_MAX);

    bt->sec = sec;

    return 0;
}

/***
 * Get value for system time.
 *
 * @function get_sec
 *
 * @return (LUA_TNUMBER)
 *
 * @usage sec = bintime:get_sec()
 */
static int
BinTime_get_sec(lua_State *L)
{
    struct bintime *bt;
    int sec;

    luab_checkmaxargs(L, 1);

    bt = (struct bintime *)(*bintime_type.get)(L, 1);
    sec = bt->sec;

    return luab_pusherr(L, sec);
}

/***
 * Set value for for system time.
 *
 * @function set_frac
 *
 * @param frac          Specifies frac.
 *
 * @usage bintime:set_frac(frac)
 */
static int
BinTime_set_frac(lua_State *L)
{
    struct bintime *bt;
    uint64_t frac;

    luab_checkmaxargs(L, 2);

    bt = (struct bintime *)(*bintime_type.get)(L, 1);
    frac = luab_checkinteger(L, 2, LONG_MAX);

    bt->frac = frac;

    return 0;
}

/***
 * Get value for for system time.
 *
 * @function get_frac
 *
 * @return (LUA_TNUMBER)
 *
 * @usage frac = bintime:get_frac()
 */
static int
BinTime_get_frac(lua_State *L)
{
    struct bintime *bt;
    uint64_t frac;

    luab_checkmaxargs(L, 1);

    bt = (struct bintime *)(*bintime_type.get)(L, 1);
    frac = bt->frac;

    return luab_pusherr(L, frac);
}

/***
 * Translate bintime{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = bintime:get()
 */
static int
BinTime_get(lua_State *L)
{
    struct bintime *bt;

    luab_checkmaxargs(L, 1);

    bt = (struct bintime *)(*bintime_type.get)(L, 1);

    lua_newtable(L);

    luab_setinteger(L, -2, "sec", bt->sec);
    luab_setinteger(L, -2, "frac", bt->frac);

    lua_pushvalue(L, -1);

    return 1;
}

static int
BinTime_gc(lua_State *L)
{
    luab_bintime_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_bintime(L, 1);

    (void)memset_s(self, bintime_type.sz, 0, bintime_type.sz);

    return 0;
}

static int
BinTime_tostring(lua_State *L)
{
    luab_bintime_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_bintime(L, 1);
    lua_pushfstring(L, "bintime (%p)", self);

    return 1;
}

static luab_table_t bintime_methods[] = {
    LUABSD_FUNC("set_sec",  BinTime_set_sec),
    LUABSD_FUNC("set_frac", BinTime_set_frac),
    LUABSD_FUNC("get",  BinTime_get),
    LUABSD_FUNC("get_sec",  BinTime_get_sec),
    LUABSD_FUNC("get_frac", BinTime_get_frac),
    LUABSD_FUNC("__gc", BinTime_gc),
    LUABSD_FUNC("__tostring",   BinTime_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void
bintime_init(void *ud, void *arg)
{
    luab_bintime_t *self = (luab_bintime_t *)ud;

    (void)memmove(&self->bintime, arg, sizeof(self->bintime));
}

static void *
bintime_udata(lua_State *L, int narg)
{
    luab_bintime_t *self = luab_to_bintime(L, narg);

    return (&self->bintime);
}

luab_module_t bintime_type = {
    .cookie = LUABSD_BINTIME_TYPE_ID,
    .name = LUABSD_BINTIME_TYPE,
    .vec = bintime_methods,
    .init = bintime_init,
    .get = bintime_udata,
    .sz = sizeof(luab_bintime_t),
};

/***
 * Ctor.
 *
 * @function StructBinTime
 *
 * @param               Optional.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage bt = bsd.sys.time.StructBinTime([ bt ])
 */
int
luab_StructBinTime(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 1);
    struct bintime *bintime;
    int status;

    if (narg == 0)
        bintime = NULL;
    else
        bintime = bintime_udata(L, narg);

    if (luab_new_bintime(L, bintime) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}
