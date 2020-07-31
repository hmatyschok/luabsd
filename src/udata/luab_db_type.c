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

#include <sys/file.h>
#include <sys/param.h>

#include <db.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t dbt_type;
extern luab_module_t db_type;

/*
 * Interface against
 *
 *  typedef struct __db {
 *      DBTYPE type;
 *      int (*close)(struct __db *);
 *      int (*del)(const struct __db *, const DBT *, unsigned int);
 *      int (*get)(const struct __db *, const DBT *, DBT *, unsigned int);
 *      int (*put)(const struct __db *, DBT *, const DBT *, unsigned int);
 *      int (*seq)(const struct __db *, DBT *, DBT *, unsigned int);
 *      int (*sync)(const struct __db *, unsigned int);
 *      void *internal;
 *      int (*fd)(const struct __db *);
 *  } DB;
 */

#define LUABSD_DB_TYPE_ID    1596201370
#define LUABSD_DB_TYPE   "DB*"

typedef struct luab_db {
    DB  *db;
} luab_db_t;

#define luab_newdb(L, arg) \
    ((luab_db_t *)luab_newuserdata(L, &db_type, (arg)))
#define luab_to_db(L, narg) \
    luab_todata((L), (narg), &db_type, luab_db_t *)

int luab_dbopen(lua_State *);

static const char *
db_fname(lua_State *L, int narg)
{
    if (lua_type(L, narg) == LUA_TNIL)
        return NULL;

    return luaL_checklstring(L, narg, NULL);
}

static int
db_isclosed(luab_db_t *self)
{
    int status;

    if (self->db == NULL) {
        errno = EBADF;
        status = -1;
    } else
        status = 0;

    return status;
}

static int
db_close(lua_State *L)
{
    luab_db_t *self;
    int status;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_db(L, 1);

    if (db_isclosed(self) == 0) {
        if ((status = (self->db->close)(self->db)) == 0)
            self->db = NULL;
    } else {
        errno = EBADF;
        status = -1;
    }
    return luab_pusherr(L, status);
}

static int
db_del(lua_State *L)
{
    DB *db;
    DBT *k;
    u_int flags;
    int status;

    (void)luab_checkmaxargs(L, 3);

    if ((db = (DB *)(*db_type.get)(L, 1)) != NULL) {
        k = (DBT *)(*dbt_type.get)(L, 2);
        flags = luab_checkinteger(L, 3, INT_MAX);

        status = (db->del)(db, k, flags);
    } else {
        errno = EBADF;
        status = -1;
    }
    return luab_pusherr(L, status);
}

static int
db_get(lua_State *L)
{
    DB *db;
    DBT *k, *v;
    u_int flags;
    int status;

    (void)luab_checkmaxargs(L, 4);

    if ((db = (DB *)(*db_type.get)(L, 1)) != NULL) {
        k = (DBT *)(*dbt_type.get)(L, 2);
        v = (DBT *)(*dbt_type.get)(L, 3);
        flags = luab_checkinteger(L, 4, INT_MAX);

        status = (db->get)(db, k, v, flags);
    } else {
        errno = EBADF;
        status = -1;
    }
    return luab_pusherr(L, status);
}

static int
db_put(lua_State *L)
{
    DB *db;
    DBT *k, *v;
    u_int flags;
    int status;

    (void)luab_checkmaxargs(L, 4);

    if ((db = (DB *)(*db_type.get)(L, 1)) != NULL) {
        k = (DBT *)(*dbt_type.get)(L, 2);
        v = (DBT *)(*dbt_type.get)(L, 3);
        flags = luab_checkinteger(L, 4, INT_MAX);

        status = (db->put)(db, k, v, flags);
    } else {
        errno = EBADF;
        status = -1;
    }
    return luab_pusherr(L, status);
}

static int
db_seq(lua_State *L)
{
    DB *db;
    DBT *k, *v;
    u_int flags;
    int status;

    (void)luab_checkmaxargs(L, 4);

    if ((db = (DB *)(*db_type.get)(L, 1)) != NULL) {
        k = (DBT *)(*dbt_type.get)(L, 2);
        v = (DBT *)(*dbt_type.get)(L, 3);
        flags = luab_checkinteger(L, 4, INT_MAX);

        status = (db->seq)(db, k, v, flags);
    } else {
        errno = EBADF;
        status = -1;
    }
    return luab_pusherr(L, status);
}

static int
db_sync(lua_State *L)
{
    DB *db;
    u_int flags;
    int status;

    (void)luab_checkmaxargs(L, 2);

    if ((db = (DB *)(*db_type.get)(L, 1)) != NULL) {
        flags = luab_checkinteger(L, 2, INT_MAX);

        status = (db->sync)(db, flags);
    } else {
        errno = EBADF;
        status = -1;
    }
    return luab_pusherr(L, status);
}

static int
db_fd(lua_State *L)
{
    DB *db;
    int fd;

    (void)luab_checkmaxargs(L, 1);

    if ((db = (DB *)(*db_type.get)(L, 1)) != NULL) {

        fd = (db->fd)(db);
    } else {
        errno = EBADF;
        fd = -1;
    }
    return luab_pusherr(L, fd);
}

static int
db_gc(lua_State *L)
{
    luab_db_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_db(L, 1);

    if (db_isclosed(self) == 0) {
        (void)(*self->db->close)(self->db);
        self->db = NULL;
    }
    return 0;
}

static int
db_tostring(lua_State *L)
{
    luab_db_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_db(L, 1);

    if (db_isclosed(self) == 0)
        lua_pushfstring(L, "db (%p)", self->db);
    else
        lua_pushliteral(L, "db (closed)");

    return 1;
}

static luab_table_t db_methods[] = {
    LUABSD_FUNC("close",  db_close),
    LUABSD_FUNC("del",    db_del),
    LUABSD_FUNC("get",    db_get),
    LUABSD_FUNC("fd", db_fd),
    LUABSD_FUNC("put",    db_put),
    LUABSD_FUNC("seq",    db_seq),
    LUABSD_FUNC("sync",   db_sync),
    LUABSD_FUNC("__gc",   db_gc),
    LUABSD_FUNC("__tostring", db_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
db_udata(lua_State *L, int narg)
{
    luab_db_t *self = luab_to_db(L, narg);

    return self->db;
}

luab_module_t db_type = {
    .cookie = LUABSD_DB_TYPE_ID,
    .name = LUABSD_DB_TYPE,
    .vec = db_methods,
    .get = db_udata,
    .sz = sizeof(luab_db_t),
};

int
luab_dbopen(lua_State *L)
{
    const char *fname;
    int flags, mode, type;
    luab_db_t *self;
    int status;

    (void)luab_checkmaxargs(L, 4);

    fname = db_fname(L, 1);
    flags = luab_checkinteger(L, 2, INT_MAX);
    mode = luab_checkinteger(L, 3, INT_MAX);
    type = luab_checkinteger(L, 4, INT_MAX);

    if ((self = luab_newdb(L, NULL)) != NULL) {
        if ((self->db = dbopen(fname, flags, mode, type, NULL)) == NULL) {
            lua_pop(L, 1);
            status = luab_pushnil(L);
        } else
            status = 1;
    } else
        status = luab_pushnil(L);

    return status;
}
