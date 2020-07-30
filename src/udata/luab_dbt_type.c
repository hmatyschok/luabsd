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

#include <db.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t iovec_type;
extern luab_module_t dbt_type;

/*
 * Interface against
 *
 *  typedef struct {
 *      void    *data;
 *      size_t   size;
 *  } DBT;
 */

typedef struct luab_dbt {
    DBT dbt;
} luab_dbt_t;

#define luab_newdbt(L, arg) \
    ((luab_dbt_t *)luab_newuserdata(L, &dbt_type, (arg)))
#define luab_to_dbt(L, narg) \
    (luab_todata((L), (narg), &dbt_type, luab_dbt_t *))

#define LUABSD_DBT_TYPE_ID    1596025036
#define LUABSD_DBT_TYPE   "DBT*"

int luab_StructDBT(lua_State *);

static int
DBT_set_data(lua_State *L)
{
    DBT *dbt;
    luab_iovec_t *buf;
    int status;

    (void)luab_checkmaxargs(L, 2);

    dbt = (DBT *)(*dbt_type.get)(L, 1);
    buf = (luab_iovec_t *)(*iovec_type.get)(L, 2);

    if (dbt != NULL) {
        if (((dbt->data = buf->iov.iov_base) != NULL) &&
            ((dbt->size = buf->iov.iov_len) > 0) &&
            ((buf->iov_flags & IOV_BUFF)))
            status = 0;
        else {
            errno = EINVAL;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return luab_pusherr(L, status);
}

static int
DBT_get_data(lua_State *L)
{
    DBT *dbt;
    luab_iovec_t *buf;
    caddr_t src, dst;
    size_t len;
    int status;

    (void)luab_checkmaxargs(L, 2);

    dbt = (DBT *)(*dbt_type.get)(L, 1);
    buf = (luab_iovec_t *)(*iovec_type.get)(L, 2);

    if ((dbt != NULL) &&
        ((src = dbt->data) != NULL) &&
        ((len = dbt->size) > 0)) {

        if (((dst = buf->iov.iov_base) != NULL) &&
            (len <= buf->iov_max_len) &&
            (buf->iov_flags & IOV_BUFF)) {
            (void)memmove(dst, src, len);
            buf->iov.iov_len = len;
            status = 0;
        } else {
            errno = EINVAL;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return luab_pusherr(L, status);
}

static int
DBT_get(lua_State *L)
{
    DBT *dbt;

    luab_checkmaxargs(L, 1);

    dbt = (DBT *)(*dbt_type.get)(L, 1);

    lua_newtable(L);

    luab_setbuff(L, -2, "data", dbt->data, dbt->size);
    luab_setinteger(L, -2, "size", dbt->size);

    lua_pushvalue(L, -1);

    return 1;
}

static int
DBT_gc(lua_State *L)
{
    luab_dbt_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_dbt(L, 1);

    if (self->dbt.data != NULL) {
        self->dbt.data = NULL;
        self->dbt.size = 0;
    }
    return 0;
}

static int
DBT_tostring(lua_State *L)
{
    luab_dbt_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_dbt(L, 1);
    lua_pushfstring(L, "DBT (%p)", self);

    return 1;
}

static luab_table_t dbt_methods[] = {
    LUABSD_FUNC("set_data",  DBT_set_data),
    LUABSD_FUNC("get",  DBT_get),
    LUABSD_FUNC("get_data",  DBT_get_data),
    LUABSD_FUNC("__gc", DBT_gc),
    LUABSD_FUNC("__tostring",   DBT_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
dbt_udata(lua_State *L, int narg)
{
    luab_dbt_t *self = luab_to_dbt(L, narg);

    return &self->dbt;
}

luab_module_t dbt_type = {
    .cookie = LUABSD_DBT_TYPE_ID,
    .name = LUABSD_DBT_TYPE,
    .vec = dbt_methods,
    .get = dbt_udata,
    .sz = sizeof(luab_dbt_t),
};

int
luab_StructDBT(lua_State *L)
{
    luab_dbt_t *self;
    int status;

    (void)luab_checkmaxargs(L, 1);

    if ((self = luab_newdbt(L, NULL)) != NULL) {
        self->dbt.data = NULL;
        self->dbt.size = 0;
        status = 1;
    } else
        status = luab_pushnil(L);

    return status;
}
