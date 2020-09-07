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

#include <unistd.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t crypt_data_type;

/*
 * Interface against
 *
 *  struct crypt_data {
 *      int initialized;
 *      char    __buf[256];
 *  };
 */

typedef struct luab_crypt_data {
    luab_udata_t        ud_softc;
    struct crypt_data    crypt_data;
} luab_crypt_data_t;

#define CRYPT_DATA_MAX  256

#define luab_new_crypt_data(L, arg) \
    ((luab_crypt_data_t *)luab_newuserdata(L, &crypt_data_type, (arg)))
#define luab_to_crypt_data(L, narg) \
    (luab_toldata((L), (narg), &crypt_data_type, \
        struct crypt_data *, sizeof(struct crypt_data)))

#define LUABSD_CRYPT_DATA_TYPE_ID    1595491033
#define LUABSD_CRYPT_DATA_TYPE    "CRYPTDATA*"

int luab_crypt_data_create(lua_State *);

/***
 * Set param denotes initializiation.
 *
 * @function set_initialized
 *
 * @param data              Integer.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = crypt_data:set_initialized(data)
 */
static int
CryptData_set_initialized(lua_State *L)
{
    struct crypt_data *cd;
    int data;

    (void)luab_checkmaxargs(L, 2);

    cd = luab_udata(L, 1, crypt_data_type, struct crypt_data *);
    data = (int)luab_checkinteger(L, 2, INT_MAX);

    cd->initialized = data;

    return (luab_pusherr(L, data));
}

/***
 * Get param denotes initializiation.
 *
 * @function get_initialized
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = crypt_data:get_initialized()
 */
static int
CryptData_get_initialized(lua_State *L)
{
    struct crypt_data *cd;
    int data;

    (void)luab_checkmaxargs(L, 1);

    cd = luab_udata(L, 1, crypt_data_type, struct crypt_data *);
    data = cd->initialized;

    return (luab_pusherr(L, data));
}

/***
 * Copyin.
 *
 * @function copyin
 *
 * @param data          Byte string.
 *
 * @usage crypt_data:set_buf(data)
 */
static int
CryptData_set_buf(lua_State *L)
{
    struct crypt_data *cd;
    const char *buf;
    size_t len;

    (void)luab_checkmaxargs(L, 2);

    cd = luab_udata(L, 1, crypt_data_type, struct crypt_data *);
    buf = luab_checklstring(L, 2, CRYPT_DATA_MAX);

    len = strlen(buf);

    (void)memmove(cd->__buf, buf, len);

    return (0);
}

/***
 * Copyout.
 *
 * @function get_buf
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (data, (errno, strerror(errno)))
 *
 * @usage data = crypt_data:get_buf()
 */
static int
CryptData_get_buf(lua_State *L)
{
    struct crypt_data *cd;
    caddr_t buf;
    size_t len;

    (void)luab_checkmaxargs(L, 1);

    cd = luab_udata(L, 1, crypt_data_type, struct crypt_data *);
    buf = cd->__buf;
    len = strnlen(buf, CRYPT_DATA_MAX);

    return (luab_pushldata(L, buf, len));
}

/***
 * Translate crypt_data{} into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = crypt_data:get()
 */
static int
CryptData_get(lua_State *L)
{
    struct crypt_data *cd;

    (void)luab_checkmaxargs(L, 1);

    cd = luab_udata(L, 1, crypt_data_type, struct crypt_data *);

    lua_newtable(L);

    luab_setinteger(L, -2, "initialized", cd->initialized);
    luab_setstring(L, -2, "buf", cd->__buf);   /* XXX */

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Copy crypt_data{} into LUA_TUSERDATA(luab_iovec_t).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = crypt_data:dump()
 */
static int
CryptData_dump(lua_State *L)
{
    return (luab_dump(L, 1, &crypt_data_type, sizeof(struct crypt_data)));
}

/*
 * Meta-methods
 */

static int
CryptData_gc(lua_State *L)
{
    return (luab_gc(L, 1, &crypt_data_type));
}

static int
CryptData_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &crypt_data_type));
}

static luab_table_t crypt_data_methods[] = {
    LUABSD_FUNC("set_initialized",   CryptData_set_initialized),
    LUABSD_FUNC("set_buf",   CryptData_set_buf),
    LUABSD_FUNC("get",  CryptData_get),
    LUABSD_FUNC("get_initialized",   CryptData_get_initialized),
    LUABSD_FUNC("get_buf",   CryptData_get_buf),
    LUABSD_FUNC("dump", CryptData_dump),
    LUABSD_FUNC("__gc", CryptData_gc),
    LUABSD_FUNC("__tostring",   CryptData_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
crypt_data_create(lua_State *L, void *arg)
{
    return (luab_new_crypt_data(L, arg));
}

static void
crypt_data_init(void *ud, void *arg)
{
    luab_crypt_data_t *self = (luab_crypt_data_t *)ud;

    (void)memmove(&self->crypt_data, arg, sizeof(self->crypt_data));
}

static void *
crypt_data_udata(lua_State *L, int narg)
{
    return (luab_to_crypt_data(L, narg));
}

luab_module_t crypt_data_type = {
    .cookie = LUABSD_CRYPT_DATA_TYPE_ID,
    .name = LUABSD_CRYPT_DATA_TYPE,
    .vec = crypt_data_methods,
    .ctor = crypt_data_create,
    .init = crypt_data_init,
    .get = crypt_data_udata,
    .sz = sizeof(luab_crypt_data_t),
};

/***
 * Ctor.
 *
 * @function crypt_data_create
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (crypt_data [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage crypt_data [, err, msg ] = bsd.sys.time.crypt_data_create([ crypt_data ])
 */
int
luab_crypt_data_create(lua_State *L)
{
    struct crypt_data *crypt_data;
    int narg, status;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        crypt_data = NULL;
    else
        crypt_data = crypt_data_udata(L, narg);

    if (crypt_data_create(L, crypt_data) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
