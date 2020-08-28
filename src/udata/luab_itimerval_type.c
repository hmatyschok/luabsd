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

extern luab_module_t timespec_type;
extern luab_module_t itimerval_type;

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
    luab_udata_t        ud_softc;
    struct itimerval    itimerval;
} luab_itimerval_t;

#define luab_new_itimerval(L, arg) \
    ((luab_itimerval_t *)luab_newuserdata(L, &itimerval_type, (arg)))
#define luab_to_itimerval(L, narg) \
    (luab_toldata((L), (narg), &itimerval_type, \
        luab_itimerval_t *, sizeof(struct itimerval)))

#define LUABSD_ITIMERVAL_TYPE_ID    1594110231
#define LUABSD_ITIMERVAL_TYPE    "ITIMERVAL*"

int luab_StructItimerVal(lua_State *);

/* timer interval */
/***
 * Set timer interval.
 *
 * @function set_it_interval
 *
 * @param timespec                  Instance of LUA_TUSERDATA(luab_timespec_t).
 *
 * @usage itimerval:set_it_interval(timespec)
 */
static int
ItimerVal_set_it_interval(lua_State *L)
{
    struct itimerval *it;
    struct timespec *tv;

    (void)luab_checkmaxargs(L, 2);

    it = luab_udata(L, 1, itimerval_type, struct itimerval *);
    tv = luab_udata(L, 2, timespec_type, struct timespec *);

    (void)memmove(&it->it_interval, tv, sizeof(*tv));

    return (0);
}

/***
 * Get timer interval.
 *
 * @function get_it_interval
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage timespec [, err, msg ] = itimerval:get_it_interval()
 */
static int
ItimerVal_get_it_interval(lua_State *L)
{
    struct itimerval *it;
    int status;

    (void)luab_checkmaxargs(L, 1);

    it = luab_udata(L, 1, itimerval_type, struct itimerval *);

    if ((*timespec_type.ctor)(L, &it->it_interval) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

/* current value */
/***
 * Set current value.
 *
 * @function set_it_value
 *
 * @param timespec                  Instance of LUA_TUSERDATA(luab_timespec_t).
 *
 * @usage itimerval:set_it_value(timespec)
 */
static int
ItimerVal_set_it_value(lua_State *L)
{
    struct itimerval *it;
    struct timespec *tv;

    (void)luab_checkmaxargs(L, 2);

    it = luab_udata(L, 1, itimerval_type, struct itimerval *);
    tv = luab_udata(L, 2, timespec_type, struct timespec *);

    (void)memmove(&it->it_value, tv, sizeof(*tv));

    return (0);
}

/***
 * Get timer value.
 *
 * @function get_it_value
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage timespec [, err, msg ] = itimerval:get_it_value()
 */
static int
ItimerVal_get_it_value(lua_State *L)
{
    struct itimerval *it;
    int status;

    (void)luab_checkmaxargs(L, 1);

    it = luab_udata(L, 1, itimerval_type, struct itimerval *);

    if ((*timespec_type.ctor)(L, &it->it_value) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
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
    struct itimerval *it;

    (void)luab_checkmaxargs(L, 1);

    it = luab_udata(L, 1, itimerval_type, struct itimerval *);

    lua_newtable(L);

    luab_setudata(L, -2, &timespec_type, "it_interval", &it->it_interval);
    luab_setudata(L, -2, &timespec_type, "it_value", &it->it_value);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Copy itimerval{} into LUA_TUSERDATA(luab_iovec_t).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = itimerval:dump()
 */
static int
ItimerVal_dump(lua_State *L)
{
    luab_iovec_param_t iop;
    size_t len, max_len;
    caddr_t data;
    int status;

    (void)luab_checkmaxargs(L, 1);

    (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

    data = luab_udata(L, 1, itimerval_type, caddr_t);

    len = sizeof(struct itimerval);
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
ItimerVal_gc(lua_State *L)
{
    luab_itimerval_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_itimerval(L, 1);

    (void)memset_s(self, itimerval_type.sz, 0, itimerval_type.sz);

    return (0);
}

static int
ItimerVal_tostring(lua_State *L)
{
    luab_itimerval_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_itimerval(L, 1);
    lua_pushfstring(L, "itimerval (%p)", self);

    return (1);
}

static luab_table_t itimerval_methods[] = {
    LUABSD_FUNC("set_it_interval",  ItimerVal_set_it_interval),
    LUABSD_FUNC("set_it_value", ItimerVal_set_it_value),
    LUABSD_FUNC("get",  ItimerVal_get),
    LUABSD_FUNC("get_it_interval",  ItimerVal_get_it_interval),
    LUABSD_FUNC("get_it_value", ItimerVal_get_it_value),
    LUABSD_FUNC("dump", ItimerVal_dump),
    LUABSD_FUNC("__gc", ItimerVal_gc),
    LUABSD_FUNC("__tostring",   ItimerVal_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
itimerval_create(lua_State *L, void *arg)
{
    return (luab_new_itimerval(L, arg));
}

static void
itimerval_init(void *ud, void *arg)
{
    luab_itimerval_t *self;

    if (((self = (luab_itimerval_t *)ud) != NULL) && (arg != NULL))
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
    .ctor = itimerval_create,
    .init = itimerval_init,
    .get = itimerval_udata,
    .sz = sizeof(luab_itimerval_t),
};

/***
 * Ctor.
 *
 * @function StructItimerVal
 *
 * @param itimerval                 Instance of LUA_TUSERDATA(luab_itimerval_t).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage itimerval [, err, msg ] = bsd.sys.time.StructItimerVal([ itimerval ])
 */
int
luab_StructItimerVal(lua_State *L)
{
    struct itimerval *itimerval;
    int narg, status;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        itimerval = NULL;
    else
        itimerval = itimerval_udata(L, narg);

    if (itimerval_create(L, itimerval) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
