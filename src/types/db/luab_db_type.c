/*
 * Copyright (c) 2020, 2021 Henning Matyschok
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

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

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

typedef struct luab_db {
    luab_udata_t    ud_softc;
    DB              *ud_db;
} luab_db_t;

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
        status = luab_env_error;
    }
    return (status);
}

static void
db_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_db_t *self;
    DB *db;

    if ((self = (luab_db_t *)arg) != NULL) {

        if ((db = self->ud_db) != NULL) {
            luab_setinteger(L, narg, "type",                    db->type);
            luab_setfstring(L, narg, "close", "(function:%p)",  db->close);
            luab_setfstring(L, narg, "del", "(function:%p)",    db->del);
            luab_setfstring(L, narg, "get", "(function:%p)",    db->get);
            luab_setfstring(L, narg, "put", "(function:%p)",    db->put);
            luab_setfstring(L, narg, "seq", "(function:%p)",    db->seq);
            luab_setfstring(L, narg, "sync", "(function:%p)",   db->sync);
            luab_setfstring(L, narg, "internal", "(ptr:%p)",    db->internal);
            luab_setfstring(L, narg, "fd", "(function:%p)",     db->fd);
        }
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(DB)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              type        = (LUA_TNUMBER),
 *              close       = (LUA_T{NIL,STRING}),
 *              del         = (LUA_T{NIL,STRING}),
 *              get         = (LUA_T{NIL,STRING}),
 *              put         = (LUA_T{NIL,STRING}),
 *              seq         = (LUA_T{NIL,STRING}),
 *              sync        = (LUA_T{NIL,STRING}),
 *              internal    = (LUA_T{NIL,STRING}),
 *              fd          = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ] = jail:get_table()
 */
static int
DB_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DB, TYPE, __func__);

    xtp.xtp_fill = db_fillxtable;
    xtp.xtp_arg = luab_todata(L, 1, m, void *);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

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
    luab_module_t *m;
    luab_db_t *self;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DB, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_db_t *);

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
    luab_module_t *m0, *m1, *m2;
    DB *db;
    DBT *k;
    u_int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(DB, TYPE, __func__);
    m1 = luab_xmod(DBT, TYPE, __func__);
    m2 = luab_xmod(UINT, TYPE, __func__);

    if ((db = luab_udata(L, 1, m0, DB *)) != NULL) {
        k = luab_udata(L, 2, m1, DBT *);
        flags = (u_int)luab_checkxinteger(L, 3, m2, luab_env_int_max);

        status = (*db->del)(db, k, flags);
    } else
        status = luab_env_error;

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
    luab_module_t *m;
    DB *db;
    int fd;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DB, TYPE, __func__);

    if ((db = luab_udata(L, 1, m, DB *)) != NULL)
        fd = (*db->fd)(db);
    else
        fd = luab_env_error;

    return (luab_pushxinteger(L, fd));
}

/***
 * Keyed retrieval from from the db(3).
 *
 * @function get
 *
 * @param key               Instance of (LUA_TUSERDATA(DBT)).
 * @param arg               Instance of (LUA_TUSERDATA(DBT)).
 * @param flags             Set to 0.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = db:get(key, data, flags)
 */
static int
DB_get(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    DB *db;
    DBT *k, *v;
    u_int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(DB, TYPE, __func__);
    m1 = luab_xmod(DBT, TYPE, __func__);
    m2 = luab_xmod(UINT, TYPE, __func__);

    if ((db = luab_udata(L, 1, m0, DB *)) != NULL) {
        k = luab_udata(L, 2, m1, DBT *);
        v = luab_udata(L, 3, m1, DBT *);
        flags = (u_int)luab_checkxinteger(L, 4, m2, luab_env_int_max);

        status = (*db->get)(db, k, v, flags);
    } else
        status = luab_env_error;

    return (luab_pushxinteger(L, status));
}

/***
 * Store key/data pairs in the db(3).
 *
 * @function put
 *
 * @param key               Instance of (LUA_TUSERDATA(DBT)).
 * @param arg               Instance of (LUA_TUSERDATA(DBT)).
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
    luab_module_t *m0, *m1, *m2;
    DB *db;
    DBT *k, *v;
    u_int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(DB, TYPE, __func__);
    m1 = luab_xmod(DBT, TYPE, __func__);
    m2 = luab_xmod(UINT, TYPE, __func__);

    if ((db = luab_udata(L, 1, m0, DB *)) != NULL) {
        k = luab_udata(L, 2, m1, DBT *);
        v = luab_udata(L, 3, m1, DBT *);
        flags = (u_int)luab_checkxinteger(L, 4, m2, luab_env_int_max);

        status = (*db->put)(db, k, v, flags);
    } else
        status = luab_env_error;

    return (luab_pushxinteger(L, status));
}

/***
 * Sequential key/data pair retrieval from the db(3).
 *
 * @function seq
 *
 * @param key               Instance of (LUA_TUSERDATA(DBT)).
 * @param arg               Instance of (LUA_TUSERDATA(DBT)).
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
    luab_module_t *m0, *m1, *m2;
    DB *db;
    DBT *k, *v;
    u_int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(DB, TYPE, __func__);
    m1 = luab_xmod(DBT, TYPE, __func__);
    m2 = luab_xmod(UINT, TYPE, __func__);

    if ((db = luab_udata(L, 1, m0, DB *)) != NULL) {
        k = luab_udata(L, 2, m1, DBT *);
        v = luab_udata(L, 3, m1, DBT *);
        flags = (u_int)luab_checkxinteger(L, 4, m2, luab_env_int_max);

        status = (*db->seq)(db, k, v, flags);
    } else
        status = luab_env_error;

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
    luab_module_t *m0, *m1;
    DB *db;
    u_int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(DB, TYPE, __func__);
    m1 = luab_xmod(UINT, TYPE, __func__);

    if ((db = luab_udata(L, 1, m0, DB *)) != NULL) {
        flags = (u_int)luab_checkxinteger(L, 2, m1, luab_env_int_max);

        status = (*db->sync)(db, flags);
    } else
        status = luab_env_error;

    return (luab_pushxinteger(L, status));
}

/*
 * Metamethods.
 */

static int
DB_gc(lua_State *L)
{
    luab_module_t *m;
    luab_db_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DB, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_db_t *);

    if (db_close(self->ud_db) == 0)
        self->ud_db = NULL;

    return (0);
}

static int
DB_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(DB, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
DB_tostring(lua_State *L)
{
    luab_module_t *m;
    luab_db_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DB, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_db_t *);

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
    LUAB_FUNC("get_table",      DB_get_table),
    LUAB_FUNC("dump",           DB_dump),
    LUAB_FUNC("__gc",           DB_gc),
    LUAB_FUNC("__len",          DB_len),
    LUAB_FUNC("__tostring",     DB_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
db_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    luab_db_param_t *dbp;
    luab_db_t *self;

    m = luab_xmod(DB, TYPE, __func__);

    if ((dbp = (luab_db_param_t *)arg) != NULL) {

        if ((self = luab_newuserdata(L, m, dbp->dbp_db)) == NULL) {

            if (dbp->dbp_db != NULL)
                (void)(*dbp->dbp_db->close)(dbp->dbp_db);
        }
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
    luab_module_t *m;
    luab_db_t *self;

    m = luab_xmod(DB, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_db_t *);

    if (self->ud_db == NULL)
        errno = EBADF;

    return (self->ud_db);
}

static luab_table_t *
db_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    DB *x, *y;
    size_t i, j;

    m = luab_xmod(DB, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (DB *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, DB *);
                        (void)memmove(&(x[i]), y, m->m_sz);
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        } else
            errno = ERANGE;
    }
    return (tbl);
}

static void
db_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    DB *x;
    size_t i, j, k;

    m = luab_xmod(DB, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (DB *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, new);

            for (i = 0, j = tbl->tbl_card, k = 1; i < j; i++, k++)
                luab_rawsetxdata(L, narg, m, k, &(x[i]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

static luab_table_t *
db_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(DB, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_db_type = {
    .m_id           = LUAB_DB_TYPE_ID,
    .m_name         = LUAB_DB_TYPE,
    .m_vec          = db_methods,
    .m_create       = db_create,
    .m_init         = db_init,
    .m_get          = db_udata,
    .m_get_tbl      = db_checktable,
    .m_set_tbl      = db_pushtable,
    .m_alloc_tbl    = db_alloctable,
    .m_len          = sizeof(luab_db_t),
    .m_sz           = sizeof(DB *),
};
#endif /* __BSD_VISIBLE */
