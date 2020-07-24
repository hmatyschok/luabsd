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

/*
 * Interface against db(3).
 */

#if __BSD_VISIBLE
#define LUABSD_DB_TYPE_ID    1593623398
#define LUABSD_DB_TYPE   "DB*"

typedef struct luab_db {
    DB  *db;
} luab_db_t;

#define luab_newdb(L, arg) \
    ((luab_db_t *)luab_newuserdata(L, &db_type, (arg)))
#define luab_to_db(L, narg) \
    luab_todata((L), (narg), &db_type, luab_db_t *)

static const char *
db_fname(lua_State *L, int narg)
{
    if (lua_type(L, narg) == LUA_TNIL)
        return NULL;

    return luaL_checklstring(L, narg, NULL);
}

static int
db_newbuf(lua_State *L, int narg, DBT *dst)
{
    const char *s;
    size_t len;

    s = luaL_checklstring(L, narg, &len);

    if ((dst->data = calloc(1, len + 1)) == NULL)
        return -1;

    (void)memmove(dst->data, s, len);

    dst->size = len;

    return 0;
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

    if ((status = db_isclosed(self)) == 0) {
        if ((status = (self->db->close)(self->db)) == 0)
            self->db = NULL;
    }
    return luab_pusherr(L, status);
}

static int
db_del(lua_State *L)
{
    luab_db_t *self;
    DBT k;
    u_int flags;
    int status;

    (void)luab_checkmaxargs(L, 3);

    self = luab_to_db(L, 1);
    flags = luab_checkinteger(L, 3, INT_MAX);

    if ((status = db_isclosed(self)) == 0) {
        if ((status = db_newbuf(L, 2, &k)) == 0) {
            status = (self->db->del)(self->db, &k, flags);
            free(k.data);
        }
    }
    return luab_pusherr(L, status);
}

static int
db_get(lua_State *L)
{
    luab_db_t *self;
    DBT k, v;
    u_int flags;
    int status;

    (void)luab_checkmaxargs(L, 3);

    self = luab_to_db(L, 1);
    flags = luab_checkinteger(L, 3, INT_MAX);

    if ((status = db_isclosed(self)) != 0)
        goto bad;

    if ((status = db_newbuf(L, 2, &k)) != 0)
        goto bad;

    if ((status = (self->db->get)(self->db, &k, &v, flags)) != 0)
        goto bad1;

    lua_pushinteger(L, status);
    lua_pushlstring(L, v.data, v.size);

    free(k.data);

    return 2;
bad1:
    free(k.data);
bad:
    return luab_pusherr(L, status);
}

static int
db_put(lua_State *L)
{
    luab_db_t *self;
    DBT k, v;
    u_int flags;
    int status;

    (void)luab_checkmaxargs(L, 4);

    self = luab_to_db(L, 1);
    flags = luab_checkinteger(L, 4, INT_MAX);

    if ((status = db_isclosed(self)) != 0)
        goto out;

    if ((status = db_newbuf(L, 2, &k)) != 0)
        goto out;

    if ((status = db_newbuf(L, 3, &v)) != 0)
        goto out1;

    status = (self->db->put)(self->db, &k, &v, flags);

    free(v.data);
out1:
    free(k.data);
out:
    return luab_pusherr(L, status);
}

static int
db_seq(lua_State *L)
{
    luab_db_t *self;
    DBT k, v;
    u_int flags;
    int status;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_db(L, 1);
    flags = luab_checkinteger(L, 2, INT_MAX);

    if ((status = db_isclosed(self)) != 0)
        goto bad;

    if ((status = (self->db->seq)(self->db, &k, &v, flags)) != 0)
        goto bad;

    lua_pushinteger(L, status);
    lua_pushlstring(L, k.data, k.size);
    lua_pushlstring(L, v.data, v.size);

    return 3;
bad:
    return luab_pusherr(L, status);
}

static int
db_sync(lua_State *L)
{
    luab_db_t *self;
    u_int flags;
    int status;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_db(L, 1);
    flags = luab_checkinteger(L, 2, INT_MAX);

    if ((status = db_isclosed(self)) == 0)
        status = (self->db->sync)(self->db, flags);

    return luab_pusherr(L, status);
}

static int
db_fd(lua_State *L)
{
    luab_db_t *self;
    int fd = -1, status;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_db(L, 1);

    if ((status = db_isclosed(self)) == 0)
        fd = (self->db->fd)(self->db);

    return luab_pusherr(L, fd);
}

static int
db_flock(lua_State *L)
{
    luab_db_t *self;
    int op, fd, status;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_db(L, 1);
    op = luab_checkinteger(L, 2, INT_MAX);

    if ((status = db_isclosed(self)) == 0) {
        if ((fd = (self->db->fd)(self->db)) >= 0)
            status = flock(fd, op);
    }
    return luab_pusherr(L, status);
}

static int
db_gc(lua_State *L)
{
    luab_db_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_db(L, 1);

    if (db_isclosed(self) != 0)
        db_close(L);

    return 0;
}

static int
db_tostring(lua_State *L)
{
    luab_db_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_db(L, 1);

    if (db_isclosed(self) != 0)
        lua_pushliteral(L, "db (closed)");
    else
        lua_pushfstring(L, "db (%p)", self->db);

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
    LUABSD_FUNC("flock",   db_flock),
    LUABSD_FUNC("__gc",   db_gc),
    LUABSD_FUNC("__tostring", db_tostring),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t db_type = {
    .cookie = LUABSD_DB_TYPE_ID,
    .name = LUABSD_DB_TYPE,
    .vec = db_methods,
    .sz = sizeof(luab_db_t),
};


static int
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
#endif

#define LUABSD_DB_LIB_ID    1593623310
#define LUABSD_DB_LIB_KEY    "db"

static luab_table_t luab_db_vec[] = {   /* db.h */
    LUABSD_INT("RET_ERROR", RET_ERROR),
    LUABSD_INT("RET_SUCCESS",   RET_SUCCESS),
    LUABSD_INT("RET_SPECIAL",   RET_SPECIAL),
    LUABSD_INT("DB_LOCK",    DB_LOCK),
    LUABSD_INT("DB_SHMEM",   DB_SHMEM),
    LUABSD_INT("DB_TXN", DB_TXN),
    LUABSD_INT("R_CURSOR",   R_CURSOR),
    LUABSD_INT("__R_UNUSED", __R_UNUSED),
    LUABSD_INT("R_FIRST",    R_FIRST),
    LUABSD_INT("R_IAFTER",   R_IAFTER),
    LUABSD_INT("R_IBEFORE",  R_IBEFORE),
    LUABSD_INT("R_LAST", R_LAST),
    LUABSD_INT("R_NEXT", R_NEXT),
    LUABSD_INT("R_NOOVERWRITE",  R_NOOVERWRITE),
    LUABSD_INT("R_PREV", R_PREV),
    LUABSD_INT("R_SETCURSOR",    R_SETCURSOR),
    LUABSD_INT("R_RECNOSYNC",    R_RECNOSYNC),
    LUABSD_INT("DB_BTREE",   DB_BTREE),
    LUABSD_INT("DB_HASH",    DB_HASH),
    LUABSD_INT("DB_RECNO",   DB_RECNO),
#if __BSD_VISIBLE
    LUABSD_FUNC("dbopen", luab_dbopen),
#endif
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_db_lib = {
    .cookie = LUABSD_DB_LIB_ID,
    .name = LUABSD_DB_LIB_KEY,
    .vec = luab_db_vec,
};
