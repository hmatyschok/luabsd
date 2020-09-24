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

#if __BSD_VISIBLE
#include "luab_types.h"
#endif

#define LUABSD_DB_LIB_ID    1593623310
#define LUABSD_DB_LIB_KEY    "db"

extern luab_module_t luab_db_lib;

/*
 * Service primitives.
 */

#if __BSD_VISIBLE
/***
 * dbopen(3) - database access methods
 *
 * @function dbopen
 *
 * @param file                      Name by (LUA_TSTRING) or (LUA_TNIL)
 *                                  creates an in-memory db(3) file.
 * @param flags                     Same as specified for open(2).
 * @param mode                      Same as specified for open(2).
 * @param type                      Specifies DBTYPE as defined in <db.h>.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (db [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = bsd.db.dbopen(file, flags, mode, type)
 */
static int
luab_dbopen(lua_State *L)
{
    const char *file;
    int flags, mode, type;
    DB *db;

    (void)luab_checkmaxargs(L, 4);

    file = luab_islstring(L, 1, MAXPATHLEN);
    flags = luab_checkinteger(L, 2, INT_MAX);
    mode = luab_checkinteger(L, 3, INT_MAX);
    type = luab_checkinteger(L, 4, INT_MAX);

    db = dbopen(file, flags, mode, type, NULL);

    return (luab_pushudata(L, luab_mx(DB), db));
}

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(DBT)).
 *
 * @function dbt_create
 *
 * @param data          (LUA_T{NIL,USERDATA(IOVEC)}), optional.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (dbt [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage dbt [, err, msg ] = bsd.db.dbt_create([ data ])
 */
static int
luab_dbt_create(lua_State *L)
{
    return (luab_create(L, 1, luab_mx(DBT), luab_mx(IOVEC)));
}
#endif /* __BSD_VISIBLE */

/*
 * Interface against <db.h>.
 */

static luab_table_t luab_db_vec[] = {
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
    LUABSD_FUNC("dbt_create",    luab_dbt_create),
#endif
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_db_lib = {
    .cookie = LUABSD_DB_LIB_ID,
    .name = LUABSD_DB_LIB_KEY,
    .vec = luab_db_vec,
};
