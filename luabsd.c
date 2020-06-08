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
#include <sys/stat.h>
#include <sys/types.h>

#include <db.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>
#include <uuid.h>

#define LUABSD_DB   "DB*"

typedef struct {
    const char    *key;
    int value;
} tok_t;

typedef struct {
    const char    *field;
    tok_t   entries[];
} table_t;

typedef struct {
    DB  *db;
} db_softc_t;

#define todb(L, narg)	((db_softc_t *)luaL_checkudata(L, narg, LUABSD_DB))

LUAMOD_API int  luaopen_bsd(lua_State *);

static table_t db_open_flags = {
    .field = "db_o_flags",
    .entries = {
        { "DB_LOCK",    DB_LOCK },
        { "DB_SHMEM",   DB_SHMEM },
        { "DB_TXN", DB_TXN },
        { NULL, 0 }
    }
};

static table_t db_routine_flags = {
    .field = "db_r_flags",
    .entries = {
        { "R_CURSOR",   R_CURSOR },
        { "__R_UNUSED", __R_UNUSED },
        { "R_FIRST",    R_FIRST },
        { "R_IAFTER",   R_IAFTER },
        { "R_IBEFORE",  R_IBEFORE },
        { "R_LAST", R_LAST },
        { "R_NEXT", R_NEXT },
        { "R_NOOVERWRITE",  R_NOOVERWRITE },
        { "R_PREV", R_PREV },
        { "R_SETCURSOR",    R_SETCURSOR },
        { "R_RECNOSYNC",    R_RECNOSYNC },
        { NULL, 0 }
    }
};

static table_t db_type = {
    .field = "db_type",
    .entries = {
        { "DB_BTREE",   DB_BTREE },
        { "DB_HASH",    DB_HASH },
        { "DB_RECNO",   DB_RECNO },
        { NULL, 0 }
    }
};

static table_t f_lock_operation = {
    .field = "flock_ops",
    .entries = {
        { "LOCK_SH",   LOCK_SH },
        { "LOCK_EX",   LOCK_EX },
        { "LOCK_NB",   LOCK_NB },
        { "LOCK_UN",   LOCK_UN },
        { NULL, 0 }
    }
};

static table_t f_open_flags = {
    .field = "f_o_flags",
    .entries = {
        { "O_RDONLY",   O_RDONLY },
        { "O_WRONLY",   O_WRONLY },
        { "O_RDWR", O_RDWR },
        { "O_EXEC", O_EXEC },
        { "O_NONBLOCK", O_NONBLOCK },
        { "O_APPEND",   O_APPEND },
        { "O_CREAT",    O_CREAT },
        { "O_TRUNC",    O_TRUNC },
        { "O_EXCL", O_EXCL },
        { "O_SHLOCK",   O_SHLOCK },
        { "O_EXLOCK",   O_EXLOCK },
        { "O_DIRECT",   O_DIRECT },
        { "O_FSYNC",    O_FSYNC },
        { "O_SYNC", O_SYNC },
        { "O_NOFOLLOW", O_NOFOLLOW },
        { "O_NOCTTY",   O_NOCTTY },
        { "O_TTY_INIT", O_TTY_INIT },
        { "O_DIRECTORY", O_DIRECTORY },
        { "O_CLOEXEC",  O_CLOEXEC },
        { "O_VERIFY",   O_VERIFY },
        { NULL, 0 }
    }
};

static table_t f_status_flags = {
    .field = "f_s_flags",
    .entries = {
        { "S_ISUID",    S_ISUID },
        { "S_ISGID",    S_ISGID },
        { "S_ISTXT",    S_ISTXT },
        { "S_IRWXU",    S_IRWXU },
        { "S_IRUSR",    S_IRUSR },
        { "S_IWUSR",    S_IWUSR },
        { "S_IXUSR",    S_IXUSR },
        { "S_IREAD",    S_IREAD },
        { "S_IWRITE",   S_IWRITE },
        { "S_IEXEC",    S_IEXEC },
        { "S_IRWXG",    S_IRWXG },
        { "S_IRGRP",    S_IRGRP },
        { "S_IWGRP",    S_IWGRP },
        { "S_IXGRP",    S_IXGRP },
        { "S_IRWXO",    S_IRWXO },
        { "S_IROTH",    S_IROTH },
        { "S_IWOTH",    S_IWOTH },
        { "S_IXOTH",    S_IXOTH },
        { "S_IFMT", S_IFMT },
        { "S_IFIFO",    S_IFIFO },
        { "S_IFCHR",    S_IFCHR },
        { "S_IFDIR",    S_IFDIR },
        { "S_IFBLK",    S_IFBLK },
        { "S_IFREG",    S_IFREG },
        { "S_IFLNK",    S_IFLNK },
        { "S_IFSOCK",   S_IFSOCK },
        { "S_ISVTX",    S_ISVTX },
        { "S_IFWHT",    S_IFWHT },
        { NULL, 0 }
    }
};

static void
newtable(lua_State *L, table_t *descr)
{
    tok_t *param;

    lua_newtable(L);

    for (param = descr->entries; param->key; param++) {
        lua_pushinteger(L, param->value);
        lua_setfield(L, -2, param->key);
    }
    lua_setfield(L, 1, descr->field);
}

/*
 * arc4random(3)
 */
 
static int
bsd_arc4random(lua_State *L)
{
    uint32_t n = arc4random();
    
    lua_pushinteger(L, n);
    
    return 1;    
} 

static int
bsd_arc4random_uniform(lua_State *L)
{
    uint32_t ub = luaL_checkinteger(L, 1);
    uint32_t n = arc4random_uniform(ub);

    lua_pushinteger(L, n);
    
    return 1;
}

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

    memmove(dst->data, s, len);

    dst->size = len;

    return 0;
}

static int
db_isclosed(db_softc_t *sc)
{
    int status;

    if (sc->db == NULL) {
        errno = EBADF;
        status = -1;
    } else
        status = 0;

    return status;
}

static int
db_pusherror(lua_State *L, int status)
{
    int saved_errno = errno;

    lua_pushstring(L, strerror(saved_errno));
    lua_pushinteger(L, status);

    return 2;
}

static int
db_close(lua_State *L)
{
    db_softc_t *sc = todb(L, 1);
    int status;

    if ((status = db_isclosed(sc)) != 0)
        return db_pusherror(L, status);

    if ((status = (sc->db->close)(sc->db)) != 0)
        return db_pusherror(L, status);

    lua_pushinteger(L, status);

    sc->db = NULL;

    return 1;
}

static int
db_del(lua_State *L)
{
    db_softc_t *sc = todb(L, 1);
    DBT k;
    u_int flags;
    int status;

    if ((status = db_isclosed(sc)) != 0)
        return db_pusherror(L, status);

    if ((status = db_newbuf(L, 2, &k)) != 0)
        return db_pusherror(L, status);

    flags = luaL_checkinteger(L, 3);

    if ((status = (sc->db->del)(sc->db, &k, flags)) != 0)
        return db_pusherror(L, status);

    lua_pushinteger(L, status);

    free(k.data);

    return 1;
}

static int
db_get(lua_State *L)
{
    db_softc_t *sc = todb(L, 1);
    DBT k, v;
    u_int flags;
    int status;

    if ((status = db_isclosed(sc)) != 0)
        return db_pusherror(L, status);

    if ((status = db_newbuf(L, 2, &k)) != 0)
        return db_pusherror(L, status);

    flags = luaL_checkinteger(L, 3);

    if ((status = (sc->db->get)(sc->db, &k, &v, flags)) != 0)
        return db_pusherror(L, status);

    lua_pushlstring(L, v.data, v.size);
    lua_pushinteger(L, status);

    free(k.data);

    return 2;
}

static int
db_put(lua_State *L)
{
    db_softc_t *sc = todb(L, 1);
    DBT k, v;
    u_int flags;
    int status;

    if ((status = db_isclosed(sc)) != 0)
        return db_pusherror(L, status);

    if ((status = db_newbuf(L, 2, &k)) != 0)
        return db_pusherror(L, status);

    if ((status = db_newbuf(L, 3, &v)) != 0) {
        free(k.data);
        return db_pusherror(L, status);
    }
    flags = luaL_checkinteger(L, 4);

    if ((status = (sc->db->put)(sc->db, &k, &v, flags)) != 0)
        return db_pusherror(L, status);

    lua_pushinteger(L, status);

    free(k.data);
    free(v.data);

    return 1;
}

static int
db_seq(lua_State *L)
{
    db_softc_t *sc = todb(L, 1);
    DBT k, v;
    u_int flags;
    int status;

    if ((status = db_isclosed(sc)) != 0)
        return db_pusherror(L, status);

    flags = luaL_checkinteger(L, 2);

    if ((status = (sc->db->seq)(sc->db, &k, &v, flags)) != 0)
        return db_pusherror(L, status);

    lua_pushlstring(L, k.data, k.size);
    lua_pushlstring(L, v.data, v.size);
    lua_pushinteger(L, status);

    return 3;
}

static int
db_sync(lua_State *L)
{
    db_softc_t *sc = todb(L, 1);
    u_int flags;
    int status;

    if ((status = db_isclosed(sc)) != 0)
        return db_pusherror(L, status);

    flags = luaL_checkinteger(L, 2);

    if ((status = (sc->db->sync)(sc->db, flags)) != 0)
        return db_pusherror(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
db_fd(lua_State *L)
{
    db_softc_t *sc = todb(L, 1);
    int fd, status;

    if ((status = db_isclosed(sc)) != 0)
        return db_pusherror(L, status);

    if ((fd = (sc->db->fd)(sc->db)) < 0)
        return db_pusherror(L, fd);

    lua_pushinteger(L, fd);

    return 1;
}

static int
db_flock(lua_State *L)
{
    db_softc_t *sc = todb(L, 1);
    int op = luaL_checkinteger(L, 2);
    int fd, status;

    if ((status = db_isclosed(sc)) != 0)
        return db_pusherror(L, status);

    if ((fd = (sc->db->fd)(sc->db)) < 0)
        return db_pusherror(L, fd);

    if ((status = flock(fd, op)) < 0)
        return db_pusherror(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
db_gc(lua_State *L)
{
    db_softc_t *sc = todb(L, 1);

    if (db_isclosed(sc) != 0)
        db_close(L);

    return 0;
}

static int
db_tostring(lua_State *L)
{
    db_softc_t *sc = todb(L, 1);

    if (db_isclosed(sc) != 0)
        lua_pushliteral(L, "db (closed)");
    else
        lua_pushfstring(L, "db (%p)", sc->db);

    return 1;
}

static const luaL_Reg dblib[] = {
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

/*
 * Wrapper for dbopen(3).
 */
static int
bsd_dbopen(lua_State *L)
{
    const char *fname = db_fname(L, 1);
    int flags = luaL_checkinteger(L, 2);
    int mode = luaL_checkinteger(L, 3);
    int type = luaL_checkinteger(L, 4);
    db_softc_t *sc = (db_softc_t *)lua_newuserdata(L, sizeof(db_softc_t));

    sc->db = NULL;
    luaL_setmetatable(L, LUABSD_DB);

    if ((sc->db = dbopen(fname, flags, mode, type, NULL)) == NULL)
        return luaL_error(L, "dbopen (%s)", strerror(errno));

    return 1;
}

/*
 * Wrapper for uuidgen(2), derived from implementation of uuidgen(1).
 */
static int
bsd_uuidgen(lua_State *L)
{
    uuid_t uuid;
    char *buf;
    uint32_t status;

    if ((status = uuidgen(&uuid, 1)) != 0)
        return luaL_error(L, "unable to generate a UUID");

    uuid_to_string(&uuid, &buf, &status);

    if (status != uuid_s_ok)
        return luaL_error(L, "cannot stringify a UUID");

    lua_pushlstring(L, buf, strlen(buf));
    free(buf);

    return 1;
}

static const luaL_Reg bsdlib[] = {
    { "arc4random", bsd_arc4random },
    { "bsd_arc4random_uniform", bsd_arc4random_uniform },
    { "dbopen", bsd_dbopen },
    { "uuidgen",    bsd_uuidgen },
    { NULL, NULL }
};

/*
 * Called, when package.loadlib takes place.
 */
LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    luaL_newlib(L, bsdlib);
    
    newtable(L, &db_open_flags);
    newtable(L, &db_routine_flags);
    newtable(L, &db_type);
    
    newtable(L, &f_lock_operation);
    newtable(L, &f_open_flags);
    newtable(L, &f_status_flags);

    luaL_newmetatable(L, LUABSD_DB);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, dblib, 0);
    lua_pop(L, 1);

    return 1;
}
