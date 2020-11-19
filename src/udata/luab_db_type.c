/*
 * Copyright (c) 2020 Henning Matyschok
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

#include <db.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#if __BSD_VISIBLE
extern luab_module_t luab_db_type;

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

#define LUAB_DB_TYPE_ID    1596201370
#define LUAB_DB_TYPE   "DB*"

typedef struct luab_db {
    luab_udata_t    ud_softc;
    DB              *ud_db;
} luab_db_t;

#define luab_new_db(L, arg) \
    ((luab_db_t *)luab_newudata(L, &luab_db_type, (arg)))
#define luab_to_db(L, narg) \
    (luab_todata((L), (narg), &luab_db_type, luab_db_t *))

/*
 * Subr.
 */

static int
db_close(DB *db)
{
    int status;

    if (db != NULL)
        status = (*db->close)(db);
    else {
        errno = EBADF;
        status = -1;
    }
    return (status);
}

/*
 * Generator functions.
 */

/***
 * Generator function - returns (LUA_TNIL).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = cmsgcred:dump()
 */
static int
DB_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
} 

/*
 * Database access methods.
 */

/***
 * Close the db(3).
 *
 * @function close
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = db:close()
 */
static int
DB_close(lua_State *L)
{
    luab_db_t *self;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_db(L, 1);

    if ((status = db_close(self->ud_db)) == 0)
        self->ud_db = NULL;

    return (luab_pushxinteger(L, status));
}

/***
 * Remove key/data pairs from the db(3).
 *
 * @function del
 *
 * @param key               Instance of (LUA_TUSERDATA(DBT)).
 * @param flags             May be set
 *
 *                              bsd.db.R_CURSOR or 0
 *
 *                          as possible value.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = db:del(key, flags)
 */
static int
DB_del(lua_State *L)
{
    DB *db;
    DBT *k;
    u_int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    if ((db = luab_udata(L, 1, &luab_db_type, DB *)) != NULL) {
        k = luab_udata(L, 2, luab_xm(DBT), DBT *);
        flags = (u_int)luab_checkinteger(L, 3, luab_env_int_max);

        status = (*db->del)(db, k, flags);
    } else
        status = -1;

    return (luab_pushxinteger(L, status));
}

/***
 * Return a file descriptor from underlying db(3).
 *
 * @function fd
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = db:fd()
 */
static int
DB_fd(lua_State *L)
{
    DB *db;
    int fd;

    (void)luab_core_checkmaxargs(L, 1);

    if ((db = luab_udata(L, 1, &luab_db_type, DB *)) != NULL)
        fd = (*db->fd)(db);
    else
        fd = -1;

    return (luab_pushxinteger(L, fd));
}

/***
 * Keyed retrieval from from the db(3).
 *
 * @function get
 *
 * @param key               Instance of (LUA_TUSERDATA(DBT)).
 * @param data              Instance of (LUA_TUSERDATA(DBT)).
 * @param flags             Set to 0.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = db:get(key, data, flags)
 */
static int
DB_get(lua_State *L)
{
    DB *db;
    DBT *k, *v;
    u_int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    if ((db = luab_udata(L, 1, &luab_db_type, DB *)) != NULL) {
        k = luab_udata(L, 2, luab_xm(DBT), DBT *);
        v = luab_udata(L, 3, luab_xm(DBT), DBT *);
        flags = (u_int)luab_checkinteger(L, 4, luab_env_int_max);

        status = (*db->get)(db, k, v, flags);
    } else
        status = -1;

    return (luab_pushxinteger(L, status));
}

/***
 * Store key/data pairs in the db(3).
 *
 * @function put
 *
 * @param key               Instance of (LUA_TUSERDATA(DBT)).
 * @param data              Instance of (LUA_TUSERDATA(DBT)).
 * @param flags             May be set from
 *
 *                              bsd.db.R_{CURSOR,I{AFTER,BEFORE},
 *                                  NOOVERWRITE,SETCURSOR}
 *
 *                          as possible value.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = db:put(key, data, flags)
 */
static int
DB_put(lua_State *L)
{
    DB *db;
    DBT *k, *v;
    u_int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    if ((db = luab_udata(L, 1, &luab_db_type, DB *)) != NULL) {
        k = luab_udata(L, 2, luab_xm(DBT), DBT *);
        v = luab_udata(L, 3, luab_xm(DBT), DBT *);
        flags = (u_int)luab_checkinteger(L, 4, luab_env_int_max);

        status = (*db->put)(db, k, v, flags);
    } else
        status = -1;

    return (luab_pushxinteger(L, status));
}

/***
 * Sequential key/data pair retrieval from the db(3).
 *
 * @function seq
 *
 * @param key               Instance of (LUA_TUSERDATA(DBT)).
 * @param data              Instance of (LUA_TUSERDATA(DBT)).
 * @param flags             May be set from
 *
 *                              bsd.db.R_{CURSOR,FIRST,LAST,NEXT,PREV}
 *
 *                          as possible value.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = db:seq(key, data, flags)
 */
static int
DB_seq(lua_State *L)
{
    DB *db;
    DBT *k, *v;
    u_int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    if ((db = luab_udata(L, 1, &luab_db_type, DB *)) != NULL) {
        k = luab_udata(L, 2, luab_xm(DBT), DBT *);
        v = luab_udata(L, 3, luab_xm(DBT), DBT *);
        flags = (u_int)luab_checkinteger(L, 4, luab_env_int_max);

        status = (*db->seq)(db, k, v, flags);
    } else
        status = -1;

    return (luab_pushxinteger(L, status));
}

/***
 * Flush any cached information to storage device.
 *
 * @function sync
 *
 * @param flags             May be set
 *
 *                              bsd.db.R_RECNOSYNC or 0
 *
 *                          as possible value.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = db:sync(flags)
 */
static int
DB_sync(lua_State *L)
{
    DB *db;
    u_int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    if ((db = luab_udata(L, 1, &luab_db_type, DB *)) != NULL) {
        flags = (u_int)luab_checkinteger(L, 2, luab_env_int_max);

        status = (*db->sync)(db, flags);
    } else
        status = -1;

    return (luab_pushxinteger(L, status));
}

/*
 * Metamethods.
 */

static int
DB_gc(lua_State *L)
{
    luab_db_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_db(L, 1);

    if (db_close(self->ud_db) == 0)
        self->ud_db = NULL;

    return (0);
}

static int
DB_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_db_type));
}

static int
DB_tostring(lua_State *L)
{
    luab_db_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_db(L, 1);

    if (self->ud_db != NULL)
        lua_pushfstring(L, "db (%p)", self->ud_db);
    else
        lua_pushliteral(L, "db (closed)");

    return (1);
}

/*
 * Internal interface.
 */

static luab_module_table_t db_methods[] = {
    LUAB_FUNC("close",          DB_close),
    LUAB_FUNC("del",            DB_del),
    LUAB_FUNC("get",            DB_get),
    LUAB_FUNC("fd",             DB_fd),
    LUAB_FUNC("put",            DB_put),
    LUAB_FUNC("seq",            DB_seq),
    LUAB_FUNC("sync",           DB_sync),
    LUAB_FUNC("dump",           DB_dump),
    LUAB_FUNC("__gc",           DB_gc),
    LUAB_FUNC("__len",          DB_len),
    LUAB_FUNC("__tostring",     DB_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
db_create(lua_State *L, void *arg)
{
    luab_db_t *self;
    DB* db;

    if ((db = (DB *)arg) != NULL) {

        if ((self = luab_new_db(L, db)) == NULL)
            (void)(*db->close)(db);
    } else
        self = NULL;

    return (self);
}

static void
db_init(void *ud, void *arg)
{
    luab_db_t *self;

    if ((self = (luab_db_t *)ud) != NULL)
        self->ud_db = (DB *)arg;
}

static void *
db_udata(lua_State *L, int narg)
{
    luab_db_t *self = luab_to_db(L, narg);

    if (self->ud_db == NULL)
        errno = EBADF;

    return (self->ud_db);
}

luab_module_t luab_db_type = {
    .m_cookie   = LUAB_DB_TYPE_ID,
    .m_name     = LUAB_DB_TYPE,
    .m_vec      = db_methods,
    .m_create   = db_create,
    .m_init     = db_init,
    .m_get      = db_udata,
    .m_sz       = sizeof(luab_db_t),
};
#endif /* __BSD_VISIBLE */
