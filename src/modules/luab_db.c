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
#if __BSD_VISIBLE
#include "luab_udata.h"

#define LUAB_DB_LIB_ID    1593623310
#define LUAB_DB_LIB_KEY    "db"

extern luab_module_t luab_db_lib;

/*
 * Service primitives.
 */

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
 * @usage iovec [, err, msg ] = bsd.db.dbopen(file, flags, mode, type)
 */
static int
luab_dbopen(lua_State *L)
{
    const char *file;
    int flags, mode, type;
    DB *db;

    (void)luab_core_checkmaxargs(L, 4);

    file = luab_islstring(L, 1, luab_env_path_max);
    flags = luab_checkinteger(L, 2, luab_int_max);
    mode = luab_checkinteger(L, 3, luab_int_max);
    type = luab_checkinteger(L, 4, luab_int_max);

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
 * @usage dbt [, err, msg ] = bsd.db.dbt_create([ data ])
 */
static int
luab_dbt_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(DBT), luab_mx(IOVEC)));
}
#endif /* __BSD_VISIBLE */

/*
 * Interface against <db.h>.
 */

static luab_module_table_t luab_db_vec[] = {
    LUAB_INT("RET_ERROR",     RET_ERROR),
    LUAB_INT("RET_SUCCESS",   RET_SUCCESS),
    LUAB_INT("RET_SPECIAL",   RET_SPECIAL),
    LUAB_INT("DB_LOCK",       DB_LOCK),
    LUAB_INT("DB_SHMEM",      DB_SHMEM),
    LUAB_INT("DB_TXN",        DB_TXN),
    LUAB_INT("R_CURSOR",      R_CURSOR),
    LUAB_INT("__R_UNUSED",    __R_UNUSED),
    LUAB_INT("R_FIRST",       R_FIRST),
    LUAB_INT("R_IAFTER",      R_IAFTER),
    LUAB_INT("R_IBEFORE",     R_IBEFORE),
    LUAB_INT("R_LAST",        R_LAST),
    LUAB_INT("R_NEXT",        R_NEXT),
    LUAB_INT("R_NOOVERWRITE", R_NOOVERWRITE),
    LUAB_INT("R_PREV",        R_PREV),
    LUAB_INT("R_SETCURSOR",   R_SETCURSOR),
    LUAB_INT("R_RECNOSYNC",   R_RECNOSYNC),
    LUAB_INT("DB_BTREE",      DB_BTREE),
    LUAB_INT("DB_HASH",       DB_HASH),
    LUAB_INT("DB_RECNO",      DB_RECNO),
#if __BSD_VISIBLE
    LUAB_FUNC("dbopen",       luab_dbopen),
    LUAB_FUNC("dbt_create",   luab_dbt_create),
#endif
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_db_lib = {
    .m_cookie   = LUAB_DB_LIB_ID,
    .m_name     = LUAB_DB_LIB_KEY,
    .m_vec      = luab_db_vec,
};
