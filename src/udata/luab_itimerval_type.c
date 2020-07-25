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

/*
 * Interface against
 *
 *  struct itimerval {
 *      struct  timeval it_interval;
 *      struct  timeval it_value;
 *  };
 *
 */

typedef struct luab_itimerval {
    struct itimerval    itimerval;
} luab_itimerval_t;

#define luab_new_itimerval(L, arg) \
    ((luab_itimerval_t *)luab_newuserdata(L, &itimerval_type, (arg)))
#define luab_to_itimerval(L, narg) \
    (luab_todata((L), (narg), &itimerval_type, luab_itimerval_t *))

#define LUABSD_ITIMERVAL_TYPE_ID    1594110231
#define LUABSD_ITIMERVAL_TYPE    "ITIMERVAL*"

int luab_StructItimerVal(lua_State *);

/* timer interval */
/***
 * Set timer interval.
 *
 * @function set_it_interval
 *
 * @param tv            Instance of timespec{}.
 *
 * @usage itimerval:set_it_interval(tv)
 */
static int
ItimerVal_set_it_interval(lua_State *L)
{
    luab_itimerval_t *self;
    void *ud;

    luab_checkmaxargs(L, 2);

    self = luab_to_itimerval(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->itimerval.it_interval, ud, timespec_type.sz);

    return 0;
}

/***
 * Get timer interval.
 *
 * @function get_it_interval
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage tv = itimerval:get_it_interval()
 */
static int
ItimerVal_get_it_interval(lua_State *L)
{
    luab_itimerval_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_to_itimerval(L, 1);

    (void)luab_newuserdata(L, &timespec_type, &self->itimerval.it_interval);

    return 1;
}

/* current value */
/***
 * Set current value.
 *
 * @function set_it_value
 *
 * @param tv            Instance of timespec{}.
 *
 * @usage itimerval:set_it_value(tv)
 */
static int
ItimerVal_set_it_value(lua_State *L)
{
    luab_itimerval_t *self;
    void *ud;

    luab_checkmaxargs(L, 2);

    self = luab_to_itimerval(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->itimerval.it_value, ud, timespec_type.sz);

    return 0;
}

/***
 * Get timer value.
 *
 * @function get_it_value
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage tv = itimerval:get_it_value()
 */
static int
ItimerVal_get_it_value(lua_State *L)
{
    luab_itimerval_t *self;
    int status;

    luab_checkmaxargs(L, 1);

    self = luab_to_itimerval(L, 1);

    if (luab_newuserdata(L, &timespec_type, &self->itimerval.it_value) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}

/***
 * Translate itimerval{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = itimerval:get()
 */
static int
ItimerVal_get(lua_State *L)
{
    luab_itimerval_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_to_itimerval(L, 1);

    lua_newtable(L);   /* XXX */

    luab_setudata(L, -2, &timespec_type, "it_interval", &self->itimerval.it_interval);
    luab_setudata(L, -2, &timespec_type, "it_value", &self->itimerval.it_value);

    lua_pushvalue(L, -1);

    return 1;
}

static int
ItimerVal_gc(lua_State *L)
{
    luab_itimerval_t *self;
    luab_module_t *m;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_itimerval(L, 1);
    m = &itimerval_type;

    (void)memset_s(self, m->sz, 0, m->sz);

    return 0;
}

static int
ItimerVal_tostring(lua_State *L)
{
    luab_itimerval_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_itimerval(L, 1);
    lua_pushfstring(L, "itimerval (%p)", self);

    return 1;
}

static luab_table_t itimerval_methods[] = {
    LUABSD_FUNC("set_it_interval",  ItimerVal_set_it_interval),
    LUABSD_FUNC("set_it_value", ItimerVal_set_it_value),
    LUABSD_FUNC("get",  ItimerVal_get),
    LUABSD_FUNC("get_it_interval",  ItimerVal_get_it_interval),
    LUABSD_FUNC("get_it_value", ItimerVal_get_it_value),
    LUABSD_FUNC("__gc", ItimerVal_gc),
    LUABSD_FUNC("__tostring",   ItimerVal_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void
itimerval_init(void *ud, void *arg)
{
    luab_itimerval_t *self = ud;    /* XXX */

    (void)memmove(&self->itimerval, arg, sizeof(self->itimerval));
}

static void *
itimerval_udata(lua_State *L, int narg)
{
    luab_itimerval_t *self = luab_to_itimerval(L, narg);

    return (&self->itimerval);
}

luab_module_t itimerval_type = {
    .cookie = LUABSD_ITIMERVAL_TYPE_ID,
    .name = LUABSD_ITIMERVAL_TYPE,
    .vec = itimerval_methods,
    .init = itimerval_init,
    .get = itimerval_udata,
    .sz = sizeof(luab_itimerval_t),
};

/***
 * Ctor.
 *
 * @function StructItimerVal
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage tv = bsd.sys.time.StructItimerVal()
 */
int
luab_StructItimerVal(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 1);
    struct itimerval *itimerval;
    int status;

    if (narg == 0)
        itimerval = NULL;
    else
        itimerval = itimerval_udata(L, narg);

    if (luab_new_itimerval(L, itimerval) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}
