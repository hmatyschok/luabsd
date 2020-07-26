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

/*
 * Interface against
 *
 *  struct crypt_data {
 *      int initialized;
 *      char    __buf[256];
 *  };
 */

typedef struct luab_crypt_data {
    struct crypt_data    crypt_data;
} luab_crypt_data_t;

#define CRYPT_DATA_MAX  256

#define luab_new_crypt_data(L, arg) \
    ((luab_crypt_data_t *)luab_newuserdata(L, &crypt_data_type, (arg)))
#define luab_to_crypt_data(L, narg) \
    (luab_todata((L), (narg), &crypt_data_type, luab_crypt_data_t *))

#define LUABSD_CRYPT_DATA_TYPE_ID    1595491033
#define LUABSD_CRYPT_DATA_TYPE    "CRYPTDATA*"

int luab_StructCryptData(lua_State *);

/***
 * Set param denotes initializiation.
 *
 * @function set_initialized
 *
 * @param arg           Integer.
 *
 * @usage crypt_data:set_initialized(arg)
 */
static int
CryptData_set_initialized(lua_State *L)
{
    luab_crypt_data_t *self;
    int initialized;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_crypt_data(L, 1);
    initialized = luab_checkinteger(L, 2, INT_MAX);

    self->crypt_data.initialized = initialized;

    return 0;
}

/***
 * Get param denotes initializiation.
 *
 * @function get_initialized
 *
 * @return (LUA_TNUMBER)
 *
 * @usage zone = crypt_data:get_initialized()
 */
static int
CryptData_get_initialized(lua_State *L)
{
    luab_crypt_data_t *self;
    int initialized;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_crypt_data(L, 1);
    initialized = self->crypt_data.initialized;

    lua_pushinteger(L, initialized);

    return 1;
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
    luab_crypt_data_t *self;
    const char *buf;
    size_t len;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_crypt_data(L, 1);
    buf = luab_checklstring(L, 2, CRYPT_DATA_MAX);

    len = strlen(buf);

    (void)memmove(self->crypt_data.__buf, buf, len);

    return 0;
}

/***
 * Copyout.
 *
 * @function get_buf
 *
 * @return (LUA_TSTRING)
 *
 * @usage data = crypt_data:get_buf()
 */
static int
CryptData_get_buf(lua_State *L)
{
    luab_crypt_data_t *self;
    luaL_Buffer b;
    caddr_t dst, src;
    size_t len;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_crypt_data(L, 1);
    src = self->crypt_data.__buf;
    len = strnlen(src, CRYPT_DATA_MAX);

    luaL_buffinit(L, &b);

    dst = luaL_prepbuffsize(&b, len);

    (void)memmove(dst, src, len);

    luaL_addsize(&b, len);
    luaL_pushresult(&b);

    return 1;
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
    luab_crypt_data_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_crypt_data(L, 1);

    lua_newtable(L);

    luab_setinteger(L, -2, "initialized", self->crypt_data.initialized);
    luab_setstring(L, -2, "buf", self->crypt_data.__buf);   /* XXX */

    lua_pushvalue(L, -1);

    return 1;
}

static int
CryptData_gc(lua_State *L)
{
    luab_crypt_data_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_crypt_data(L, 1);

    (void)memset_s(self, crypt_data_type.sz, 0, crypt_data_type.sz);

    return 0;
}

static int
CryptData_tostring(lua_State *L)
{
    luab_crypt_data_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_crypt_data(L, 1);

    lua_pushfstring(L, "crypt_data (%p)", self);

    return 1;
}

static luab_table_t crypt_data_methods[] = {
    LUABSD_FUNC("set_initialized",   CryptData_set_initialized),
    LUABSD_FUNC("set_buf",   CryptData_set_buf),
    LUABSD_FUNC("get",  CryptData_get),
    LUABSD_FUNC("get_initialized",   CryptData_get_initialized),
    LUABSD_FUNC("get_buf",   CryptData_get_buf),
    LUABSD_FUNC("__gc", CryptData_gc),
    LUABSD_FUNC("__tostring",   CryptData_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void
crypt_data_init(void *ud, void *arg)
{
    luab_crypt_data_t *self = (luab_crypt_data_t *)ud;

    (void)memmove(&self->crypt_data, arg, sizeof(self->crypt_data));
}

static void *
crypt_data_udata(lua_State *L, int narg)
{
    luab_crypt_data_t *self = luab_to_crypt_data(L, narg);

    return (&self->crypt_data);
}

luab_module_t crypt_data_type = {
    .cookie = LUABSD_CRYPT_DATA_TYPE_ID,
    .name = LUABSD_CRYPT_DATA_TYPE,
    .vec = crypt_data_methods,
    .init = crypt_data_init,
    .get = crypt_data_udata,
    .sz = sizeof(luab_crypt_data_t),
};

/***
 * Ctor.
 *
 * @function StructCryptData
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage tv = bsd.sys.time.StructCryptData([ crypt_data ])
 */
int
luab_StructCryptData(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 1);
    struct crypt_data *crypt_data;
    int status;

    if (narg == 0)
        crypt_data = NULL;
    else
        crypt_data = crypt_data_udata(L, narg);

    if (luab_new_crypt_data(L, crypt_data) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}
