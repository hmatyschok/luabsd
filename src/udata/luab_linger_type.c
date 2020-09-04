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

extern luab_module_t linger_type;

/*
 * Interface against
 *
 *  struct linger {
 *      int	l_onoff;
 *      int	l_linger;
 *  };
 */

typedef struct luab_linger {
    luab_udata_t    ud_softc;
    struct linger    linger;
} luab_linger_t;

#define luab_new_linger(L, arg) \
    ((luab_linger_t *)luab_newuserdata(L, &linger_type, (arg)))
#define luab_to_linger(L, narg) \
    (luab_toldata((L), (narg), &linger_type, \
        luab_linger_t *, sizeof(struct linger)))

#define LUABSD_LINGER_TYPE_ID    1597012436
#define LUABSD_LINGER_TYPE    "LINGER*"

int luab_StructLinger(lua_State *);

/***
 * Set value for option on / off.
 *
 * @function set_l_onoff
 *
 * @param val               Option.
 *
 * @usage linger:set_l_onoff(val)
 */
static int
Linger_set_l_onoff(lua_State *L)
{
    struct linger *l;
    int l_onoff;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, linger_type, struct linger *);
    l_onoff = (int)luab_checkinteger(L, 2, INT_MAX);

    l->l_onoff = l_onoff;

    return (0);
}

/***
 * Get value for option on / off.
 *
 * @function get_l_onoff
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (val [, nil, nil]) on success or
 *          (val, (errno, strerror(errno)))
 *
 * @usage val [, err, msg ] = linger:get_l_onoff()
 */
static int
Linger_get_l_onoff(lua_State *L)
{
    struct linger *l;
    int l_onoff;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, linger_type, struct linger *);
    l_onoff = l->l_onoff;

    return (luab_pusherr(L, l_onoff));
}

/***
 * Set value for for system time.
 *
 * @function set_l_linger
 *
 * @param val          Specifies l_linger.
 *
 * @usage linger:set_l_linger(val)
 */
static int
Linger_set_l_linger(lua_State *L)
{
    struct linger *l;
    int l_linger;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, linger_type, struct linger *);
    l_linger = (int)luab_checkinteger(L, 2, INT_MAX);

    l->l_linger = l_linger;

    return (0);
}

/***
 * Get value for for system time.
 *
 * @function get_l_linger
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (val [, nil, nil]) on success or
 *          (val, (errno, strerror(errno)))
 *
 * @usage val [, err, msg ] = linger:get_l_linger()
 */
static int
Linger_get_l_linger(lua_State *L)
{
    struct linger *l;
    int l_linger;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, linger_type, struct linger *);
    l_linger = l->l_linger;

    return (luab_pusherr(L, l_linger));
}

/***
 * Translate linger{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = linger:get()
 */
static int
Linger_get(lua_State *L)
{
    struct linger *l;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, linger_type, struct linger *);

    lua_newtable(L);

    luab_setinteger(L, -2, "l_onoff", l->l_onoff);
    luab_setinteger(L, -2, "l_linger", l->l_linger);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Copy linger{} into LUA_TUSERDATA(luab_iovec_t).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = linger:dump()
 */
static int
Linger_dump(lua_State *L)
{
    luab_iovec_param_t iop;
    size_t len, max_len;
    caddr_t data;
    int status;

    (void)luab_checkmaxargs(L, 1);

    (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

    data = luab_udata(L, 1, linger_type, caddr_t);

    len = sizeof(struct linger);
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
Linger_gc(lua_State *L)
{
    luab_linger_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_linger(L, 1);

    (void)memset_s(self, linger_type.sz, 0, linger_type.sz);

    return (0);
}

static int
Linger_tostring(lua_State *L)
{
    luab_linger_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_linger(L, 1);
    lua_pushfstring(L, "linger (%p)", self);

    return (1);
}

static luab_table_t linger_methods[] = {
    LUABSD_FUNC("set_l_onoff",  Linger_set_l_onoff),
    LUABSD_FUNC("set_l_linger", Linger_set_l_linger),
    LUABSD_FUNC("get",  Linger_get),
    LUABSD_FUNC("get_l_onoff",  Linger_get_l_onoff),
    LUABSD_FUNC("get_l_linger", Linger_get_l_linger),
    LUABSD_FUNC("dump", Linger_dump),
    LUABSD_FUNC("__gc", Linger_gc),
    LUABSD_FUNC("__tostring",   Linger_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
linger_create(lua_State *L, void *arg)
{
    return (luab_new_linger(L, arg));
}

static void
linger_init(void *ud, void *arg)
{
    luab_linger_t *self;

    if (((self = (luab_linger_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->linger, arg, sizeof(self->linger));
}

static void *
linger_udata(lua_State *L, int narg)
{
    luab_linger_t *self = luab_to_linger(L, narg);

    return (&self->linger);
}

luab_module_t linger_type = {
    .cookie = LUABSD_LINGER_TYPE_ID,
    .name = LUABSD_LINGER_TYPE,
    .vec = linger_methods,
    .ctor = linger_create,
    .init = linger_init,
    .get = linger_udata,
    .sz = sizeof(luab_linger_t),
};

/***
 * Ctor.
 *
 * @function StructLinger
 *
 * @param               Instance of LUA_TUSERDATA(luab_linger_t), optional.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage linger = bsd.sys.socket.StructLinger([ linger ])
 */
int
luab_StructLinger(lua_State *L)
{
    struct linger *linger;
    int narg, status;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        linger = NULL;
    else
        linger = linger_udata(L, narg);

    if (linger_create(L, linger) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
