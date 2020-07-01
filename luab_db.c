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
#include <sys/limits.h>

#include <db.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

typedef struct {
    DB  *db;
} luab_db_t;

#define LUABSD_DB   "DB*"
#define luab_todb(L, narg) \
    luab_todata((L), (narg), LUABSD_DB, luab_db_t *)

/*
 * Interface against db(3).
 */

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
    luab_db_t *self = luab_todb(L, 1);
    int status;

    if ((status = db_isclosed(self)) != 0)
        return luab_pusherr(L, status);

    if ((status = (self->db->close)(self->db)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    self->db = NULL;

    return 1;
}

static int
db_del(lua_State *L)
{
    luab_db_t *self = luab_todb(L, 1);
    DBT k;
    u_int flags;
    int status;

    if ((status = db_isclosed(self)) != 0)
        return luab_pusherr(L, status);

    if ((status = db_newbuf(L, 2, &k)) != 0)
        return luab_pusherr(L, status);

    flags = luab_checkinteger(L, 3, UINT_MAX);

    if ((status = (self->db->del)(self->db, &k, flags)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    free(k.data);

    return 1;
}

static int
db_get(lua_State *L)
{
    luab_db_t *self = luab_todb(L, 1);
    DBT k, v;
    u_int flags;
    int status;

    if ((status = db_isclosed(self)) != 0)
        return luab_pusherr(L, status);

    if ((status = db_newbuf(L, 2, &k)) != 0)
        return luab_pusherr(L, status);

    flags = luab_checkinteger(L, 3, UINT_MAX);

    if ((status = (self->db->get)(self->db, &k, &v, flags)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);
    lua_pushlstring(L, v.data, v.size);

    free(k.data);

    return 2;
}

static int
db_put(lua_State *L)
{
    luab_db_t *self = luab_todb(L, 1);
    DBT k, v;
    u_int flags;
    int status;

    if ((status = db_isclosed(self)) != 0)
        return luab_pusherr(L, status);

    if ((status = db_newbuf(L, 2, &k)) != 0)
        return luab_pusherr(L, status);

    if ((status = db_newbuf(L, 3, &v)) != 0) {
        free(k.data);
        return luab_pusherr(L, status);
    }
    flags = luab_checkinteger(L, 4, UINT_MAX);

    if ((status = (self->db->put)(self->db, &k, &v, flags)) != 0) {
        free(k.data);
        free(v.data);
        return luab_pusherr(L, status);
    }
    lua_pushinteger(L, status);

    free(k.data);
    free(v.data);

    return 1;
}

static int
db_seq(lua_State *L)
{
    luab_db_t *self = luab_todb(L, 1);
    DBT k, v;
    u_int flags;
    int status;

    if ((status = db_isclosed(self)) != 0)
        return luab_pusherr(L, status);

    flags = luab_checkinteger(L, 2, UINT_MAX);

    if ((status = (self->db->seq)(self->db, &k, &v, flags)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);
    lua_pushlstring(L, k.data, k.size);
    lua_pushlstring(L, v.data, v.size);

    return 3;
}

static int
db_sync(lua_State *L)
{
    luab_db_t *self = luab_todb(L, 1);
    u_int flags;
    int status;

    if ((status = db_isclosed(self)) != 0)
        return luab_pusherr(L, status);

    flags = luab_checkinteger(L, 2, UINT_MAX);

    if ((status = (self->db->sync)(self->db, flags)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
db_fd(lua_State *L)
{
    luab_db_t *self = luab_todb(L, 1);
    int fd, status;

    if ((status = db_isclosed(self)) != 0)
        return luab_pusherr(L, status);

    if ((fd = (self->db->fd)(self->db)) < 0)
        return luab_pusherr(L, fd);

    lua_pushinteger(L, fd);

    return 1;
}

static int
db_flock(lua_State *L)
{
    luab_db_t *self = luab_todb(L, 1);
    int op = luab_checkinteger(L, 2, INT_MAX);
    int fd, status;

    if ((status = db_isclosed(self)) != 0)
        return luab_pusherr(L, status);

    if ((fd = (self->db->fd)(self->db)) < 0)
        return luab_pusherr(L, fd);

    if ((status = flock(fd, op)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
db_gc(lua_State *L)
{
    luab_db_t *self = luab_todb(L, 1);

    if (db_isclosed(self) != 0)
        db_close(L);

    return 0;
}

static int
db_tostring(lua_State *L)
{
    luab_db_t *self = luab_todb(L, 1);

    if (db_isclosed(self) != 0)
        lua_pushliteral(L, "db (closed)");
    else
        lua_pushfstring(L, "db (%p)", self->db);

    return 1;
}

static luaL_Reg db_methods[] = {
    { "close",  db_close },
    { "del",    db_del },
    { "get",    db_get },
    { "fd", db_fd },
    { "put",    db_put },
    { "seq",    db_seq },
    { "sync",   db_sync },
    { "flock",   db_flock },
    { "__gc",   db_gc },
    { "__tostring", db_tostring },
    { NULL, NULL }
};

luab_type_t db_type = {
    .name = LUABSD_DB,
    .reg = db_methods,
};

static int
luab_dbopen(lua_State *L)
{
    const char *fname = db_fname(L, 1);
    int flags = luab_checkinteger(L, 2, INT_MAX);
    int mode = luab_checkinteger(L, 3, INT_MAX);
    int type = luab_checkinteger(L, 4, INT_MAX);
    luab_db_t *self = (luab_db_t *)lua_newuserdata(L, sizeof(luab_db_t));

    self->db = NULL;
    luaL_setmetatable(L, LUABSD_DB);

    if ((self->db = dbopen(fname, flags, mode, type, NULL)) == NULL)
        lua_pushnil(L);

    return 1;
}

luab_table_t luab_db_lib[] = {   /* db.h */
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
    LUABSD_FUNC("dbopen", luab_dbopen),
    LUABSD_FUNC(NULL, NULL)
};
