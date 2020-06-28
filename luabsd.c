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
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <db.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <uuid.h>

#define LUABSD_DB   "DB*"

typedef struct {
    DB  *db;
} db_softc_t;

#define todb(L, narg)   ((db_softc_t *)luaL_checkudata(L, narg, LUABSD_DB))

typedef union {
    lua_Integer x_i;
    lua_CFunction   x_f;
} luab_un_t;

typedef void    (*luab_fn_t)(lua_State *L, luab_un_t *x);

typedef struct {
    luab_fn_t   func;
    const char    *key;
    luab_un_t   val;
} luab_table_t;

LUAMOD_API int  luaopen_bsd(lua_State *);

static void
luab_pushinteger(lua_State *L, luab_un_t *x)
{
    lua_pushinteger(L, x->x_i);
}

static void
luab_pushcfunction(lua_State *L, luab_un_t *x)
{
    lua_pushcfunction(L, x->x_f);
}

#define LUABSD_REG(f, k, v) \
    { .func = f, .key = k, v }
#define LUABSD_INT(k, v) \
    LUABSD_REG(luab_pushinteger, k, .val.x_i = v)
#define LUABSD_FUNC(k, v) \
    LUABSD_REG(luab_pushcfunction, k, .val.x_f = v)

static void
luab_newtable(lua_State *L, luab_table_t *reg, const char *name)
{
    luab_table_t *tok;

    lua_newtable(L);

    for (tok = reg; tok->key; tok++) {
        tok->func(L, &tok->val);
        lua_setfield(L, -2, tok->key);
    }
    lua_setfield(L, -2, name);
}

static int
luab_pusherr(lua_State *L, int status)
{
    int saved_errno = errno;

    lua_pushinteger(L, status);
    lua_pushstring(L, strerror(saved_errno));

    return 2;
}

/*
 * arc4random(3)
 */

static int
luab_arc4random(lua_State *L)
{
    uint32_t n = arc4random();

    lua_pushinteger(L, n);

    return 1;
}

static int
luab_arc4random_uniform(lua_State *L)
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
db_close(lua_State *L)
{
    db_softc_t *sc = todb(L, 1);
    int status;

    if ((status = db_isclosed(sc)) != 0)
        return luab_pusherr(L, status);

    if ((status = (sc->db->close)(sc->db)) != 0)
        return luab_pusherr(L, status);

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
        return luab_pusherr(L, status);

    if ((status = db_newbuf(L, 2, &k)) != 0)
        return luab_pusherr(L, status);

    flags = luaL_checkinteger(L, 3);

    if ((status = (sc->db->del)(sc->db, &k, flags)) != 0)
        return luab_pusherr(L, status);

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
        return luab_pusherr(L, status);

    if ((status = db_newbuf(L, 2, &k)) != 0)
        return luab_pusherr(L, status);

    flags = luaL_checkinteger(L, 3);

    if ((status = (sc->db->get)(sc->db, &k, &v, flags)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);
    lua_pushlstring(L, v.data, v.size);

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
        return luab_pusherr(L, status);

    if ((status = db_newbuf(L, 2, &k)) != 0)
        return luab_pusherr(L, status);

    if ((status = db_newbuf(L, 3, &v)) != 0) {
        free(k.data);
        return luab_pusherr(L, status);
    }
    flags = luaL_checkinteger(L, 4);

    if ((status = (sc->db->put)(sc->db, &k, &v, flags)) != 0) {
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
    db_softc_t *sc = todb(L, 1);
    DBT k, v;
    u_int flags;
    int status;

    if ((status = db_isclosed(sc)) != 0)
        return luab_pusherr(L, status);

    flags = luaL_checkinteger(L, 2);

    if ((status = (sc->db->seq)(sc->db, &k, &v, flags)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);
    lua_pushlstring(L, k.data, k.size);
    lua_pushlstring(L, v.data, v.size);

    return 3;
}

static int
db_sync(lua_State *L)
{
    db_softc_t *sc = todb(L, 1);
    u_int flags;
    int status;

    if ((status = db_isclosed(sc)) != 0)
        return luab_pusherr(L, status);

    flags = luaL_checkinteger(L, 2);

    if ((status = (sc->db->sync)(sc->db, flags)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
db_fd(lua_State *L)
{
    db_softc_t *sc = todb(L, 1);
    int fd, status;

    if ((status = db_isclosed(sc)) != 0)
        return luab_pusherr(L, status);

    if ((fd = (sc->db->fd)(sc->db)) < 0)
        return luab_pusherr(L, fd);

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
        return luab_pusherr(L, status);

    if ((fd = (sc->db->fd)(sc->db)) < 0)
        return luab_pusherr(L, fd);

    if ((status = flock(fd, op)) != 0)
        return luab_pusherr(L, status);

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

static luaL_Reg luab_db_db[] = {
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

static int
luab_dbopen(lua_State *L)
{
    const char *fname = db_fname(L, 1);
    int flags = luaL_checkinteger(L, 2);
    int mode = luaL_checkinteger(L, 3);
    int type = luaL_checkinteger(L, 4);
    db_softc_t *sc = (db_softc_t *)lua_newuserdata(L, sizeof(db_softc_t));

    sc->db = NULL;
    luaL_setmetatable(L, LUABSD_DB);

    if ((sc->db = dbopen(fname, flags, mode, type, NULL)) == NULL)
        lua_pushnil(L);

    return 1;
}

/*
 * Interface against service primitives on <unistd.h>.
 */
static int
luab_fork(lua_State *L)
{
    pid_t pid;

    if ((pid = fork()) < 0)
        return luab_pusherr(L, pid);

    lua_pushinteger(L, pid);

    return 1;
}

static int
luab_getegid(lua_State *L)
{
    gid_t egid = getegid();

    lua_pushinteger(L, egid);

    return 1;
}

static int
luab_geteuid(lua_State *L)
{
    uid_t euid = geteuid();

    lua_pushinteger(L, euid);

    return 1;
}

static int
luab_getgid(lua_State *L)
{
    gid_t gid = getgid();

    lua_pushinteger(L, gid);

    return 1;
}

static int
luab_getlogin(lua_State *L)
{
    char *p;

    if ((p = getlogin()) == NULL)
        lua_pushnil(L);
    else
        lua_pushstring(L, p);

    return 1;
}

static int
luab_getpid(lua_State *L)
{
    pid_t pid = getpid();

    lua_pushinteger(L, pid);

    return 1;
}

static int
luab_getppid(lua_State *L)
{
    pid_t pid = getppid();

    lua_pushinteger(L, pid);

    return 1;
}

static int
luab_getpgrp(lua_State *L)
{
    pid_t pgrp = getpgrp();

    lua_pushinteger(L, pgrp);

    return 1;
}

static int
luab_getpgid(lua_State *L)
{
    pid_t pid = luaL_checkinteger(L, 1);
    pid_t pgrp;

    if ((pgrp = getpgid(pid)) < 0)
        return luab_pusherr(L, pgrp);

    lua_pushinteger(L, pgrp);

    return 1;
}

static int
luab_getuid(lua_State *L)
{
    uid_t uid = getuid();

    lua_pushinteger(L, uid);

    return 1;
}

static int
luab_getsid(lua_State *L)
{
    pid_t pid = luaL_checkinteger(L, 1);
    pid_t sid;

    if ((sid = getsid(pid)) < 0)
        return luab_pusherr(L, sid);

    lua_pushinteger(L, sid);

    return 1;
}

static int
luab_setegid(lua_State *L)
{
    gid_t egid = luaL_checkinteger(L, 1);
    int status;

    if ((status = setegid(egid)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
luab_seteuid(lua_State *L)
{
    uid_t euid = luaL_checkinteger(L, 1);
    int status;

    if ((status = seteuid(euid)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
luab_setgid(lua_State *L)
{
    gid_t gid = luaL_checkinteger(L, 1);
    int status;

    if ((status = setgid(gid)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
luab_setlogin(lua_State *L)
{
    const char *name = luaL_checklstring(L, 1, &((size_t){MAXLOGNAME}));
    int status;

    if ((status = setlogin(name)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
luab_setpgid(lua_State *L)
{
    pid_t pid = luaL_checkinteger(L, 1);
    pid_t pgrp = luaL_checkinteger(L, 2);
    int status;

    if ((status = setpgid(pid, pgrp)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
luab_setpgrp(lua_State *L)
{
    pid_t pid = luaL_checkinteger(L, 1);
    pid_t pgrp = luaL_checkinteger(L, 2);
    int status;

    if ((status = setpgrp(pid, pgrp)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

static int
luab_setsid(lua_State *L)
{
    pid_t sid;

    if ((sid = setsid()) < 0)
        return luab_pusherr(L, sid);

    lua_pushinteger(L, sid);

    return 1;
}

static int
luab_setuid(lua_State *L)
{
    uid_t uid = luaL_checkinteger(L, 1);
    int status;

    if ((status = setuid(uid)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);

    return 1;
}

/*
 * Interface against uuidgen(2), derived from implementation of uuidgen(1).
 */
static int
luab_uuidgen(lua_State *L)
{
    uuid_t uuid;
    char *buf;
    uint32_t status;

    if ((status = uuidgen(&uuid, 1)) != 0)
        return luab_pusherr(L, status);

    uuid_to_string(&uuid, &buf, &status);

    if (status != uuid_s_ok) {
        errno = ENOMEM;
        return luab_pusherr(L, status);
    }

    lua_pushlstring(L, buf, strlen(buf));
    free(buf);

    return 1;
}

/*
 * Interface against [gs]etitimer(2).
 */
static sigset_t nsigset;
static pthread_t tid;

static lua_State *saved_L;
static lua_Hook h;

static int h_msk;
static int h_cnt;

static void
callback_rtn(lua_State *L, lua_Debug *arg __unused)
{
    L = saved_L;

    lua_sethook(L, h, h_msk, h_cnt);
    lua_getfield(L, LUA_REGISTRYINDEX, "l_callback");

    if (lua_pcall(L, 0, 0, 0) != 0)
        lua_error(L);
}

static void *
signal_rtn(void *arg __unused)
{
    int l_msk = (LUA_MASKCALL|LUA_MASKRET|LUA_MASKCOUNT);
    int sig;

    for (;;) {
        if (sigwait(&nsigset, &sig) != 0)
            goto out;   /* XXX up-call */

        switch (sig) {
        case SIGALRM:
        case SIGVTALRM:
        case SIGPROF:

            h = lua_gethook(saved_L);
            h_msk = lua_gethookmask(saved_L);
            h_cnt = lua_gethookcount(saved_L);

            lua_sethook(saved_L, callback_rtn, l_msk, 1);
            goto out;
        default:
            break;
        }
    }
out:
    pthread_exit(NULL);
}

static int
luab_setitimer(lua_State *L)
{
    int which = luaL_checkinteger(L, 1);
    time_t sec = luaL_checkinteger(L, 2);
    int narg = lua_gettop(L), status;
    struct itimerval itv;

    if (sec <= 0)
        return luaL_error(L, "Invalid value for timeout.");

    if (lua_type(L, narg) != LUA_TFUNCTION)
        return luaL_error(L, "Missing callout handler.");

    lua_settop(L, narg);
    lua_setfield(L, LUA_REGISTRYINDEX, "l_callback");

    saved_L = L;

    if ((status = sigfillset(&nsigset)) != 0)
        return luab_pusherr(L, status);

    if ((status = pthread_sigmask(SIG_BLOCK, &nsigset, NULL)) != 0)
        return luab_pusherr(L, status);

    if ((status = pthread_create(&tid, NULL, signal_rtn, NULL)) != 0)
        return luab_pusherr(L, status);

    bzero(&itv, sizeof(itv));
    itv.it_value.tv_sec = sec;

    if ((status = setitimer(which, &itv, NULL)) != 0) {
        pthread_cancel(tid);
        return luab_pusherr(L, status);
    }
    lua_pushinteger(L, status);

    return 1;
}

static int
luab_getitimer(lua_State *L)
{
    int which = luaL_checkinteger(L, 1);
    struct itimerval itv;
    int status;

    if ((status = getitimer(which, &itv)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, itv.it_value.tv_sec);

    return 1;
}

/*
 * Service primitives.
 */

static luab_table_t luab_sys_file[] = { /* sys/file.h */
    LUABSD_INT("LOCK_SH",   LOCK_SH),
    LUABSD_INT("LOCK_EX",   LOCK_EX),
    LUABSD_INT("LOCK_NB",   LOCK_NB),
    LUABSD_INT("LOCK_UN",   LOCK_UN),
    LUABSD_INT(NULL, 0)
};

static luab_table_t luab_sys_stat[] = { /* sys/stat.h */
    LUABSD_INT("S_ISUID",    S_ISUID),
    LUABSD_INT("S_ISGID",    S_ISGID),
    LUABSD_INT("S_ISTXT",    S_ISTXT),
    LUABSD_INT("S_IRWXU",    S_IRWXU),
    LUABSD_INT("S_IRUSR",    S_IRUSR),
    LUABSD_INT("S_IWUSR",    S_IWUSR),
    LUABSD_INT("S_IXUSR",    S_IXUSR),
    LUABSD_INT("S_IREAD",    S_IREAD),
    LUABSD_INT("S_IWRITE",   S_IWRITE),
    LUABSD_INT("S_IEXEC",    S_IEXEC),
    LUABSD_INT("S_IRWXG",    S_IRWXG),
    LUABSD_INT("S_IRGRP",    S_IRGRP),
    LUABSD_INT("S_IWGRP",    S_IWGRP),
    LUABSD_INT("S_IXGRP",    S_IXGRP),
    LUABSD_INT("S_IRWXO",    S_IRWXO),
    LUABSD_INT("S_IROTH",    S_IROTH),
    LUABSD_INT("S_IWOTH",    S_IWOTH),
    LUABSD_INT("S_IXOTH",    S_IXOTH),
    LUABSD_INT("S_IFMT", S_IFMT),
    LUABSD_INT("S_IFIFO",    S_IFIFO),
    LUABSD_INT("S_IFCHR",    S_IFCHR),
    LUABSD_INT("S_IFDIR",    S_IFDIR),
    LUABSD_INT("S_IFBLK",    S_IFBLK),
    LUABSD_INT("S_IFREG",    S_IFREG),
    LUABSD_INT("S_IFLNK",    S_IFLNK),
    LUABSD_INT("S_IFSOCK",   S_IFSOCK),
    LUABSD_INT("S_ISVTX",    S_ISVTX),
    LUABSD_INT("S_IFWHT",    S_IFWHT),
    LUABSD_INT("ACCESSPERMS",   ACCESSPERMS),
    LUABSD_INT("ALLPERMS",  ALLPERMS),
    LUABSD_INT("DEFFILEMODE",   DEFFILEMODE),
    LUABSD_INT("UF_SETTABLE",   UF_SETTABLE),
    LUABSD_INT("UF_NODUMP", UF_NODUMP),
    LUABSD_INT("UF_IMMUTABLE",  UF_IMMUTABLE),
    LUABSD_INT("UF_APPEND", UF_APPEND),
    LUABSD_INT("UF_OPAQUE", UF_OPAQUE),
    LUABSD_INT("UF_NOUNLINK",   UF_NOUNLINK),
#if 0
    LUABSD_INT("UF_COMPRESSED", UF_COMPRESSED),
    LUABSD_INT("UF_TRACKED",    UF_TRACKED),
#endif
    LUABSD_INT(NULL, 0)
};

static luab_table_t luab_sys_time[] = { /* sys/time.h */
    LUABSD_INT("ITIMER_REAL",    ITIMER_REAL),
    LUABSD_INT("ITIMER_VIRTUAL",    ITIMER_VIRTUAL),
    LUABSD_INT("ITIMER_PROF",    ITIMER_PROF),
    LUABSD_FUNC("getitimer",  luab_getitimer),
    LUABSD_FUNC("setitimer",  luab_setitimer),
    LUABSD_FUNC(NULL, NULL)
};

static luab_table_t luab_db[] = {   /* db.h */
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

static luab_table_t luab_fcntl[] = {    /* fcntl.h */
    LUABSD_INT("O_RDONLY",   O_RDONLY),
    LUABSD_INT("O_WRONLY",   O_WRONLY),
    LUABSD_INT("O_RDWR", O_RDWR),
    LUABSD_INT("O_EXEC", O_EXEC),
    LUABSD_INT("O_NONBLOCK", O_NONBLOCK),
    LUABSD_INT("O_APPEND",   O_APPEND),
    LUABSD_INT("O_CREAT",    O_CREAT),
    LUABSD_INT("O_TRUNC",    O_TRUNC),
    LUABSD_INT("O_EXCL", O_EXCL),
    LUABSD_INT("O_SHLOCK",   O_SHLOCK),
    LUABSD_INT("O_EXLOCK",   O_EXLOCK),
    LUABSD_INT("O_DIRECT",   O_DIRECT),
    LUABSD_INT("O_FSYNC",    O_FSYNC),
    LUABSD_INT("O_SYNC", O_SYNC),
    LUABSD_INT("O_NOFOLLOW", O_NOFOLLOW),
    LUABSD_INT("O_NOCTTY",   O_NOCTTY),
    LUABSD_INT("O_TTY_INIT", O_TTY_INIT),
    LUABSD_INT("O_DIRECTORY", O_DIRECTORY),
    LUABSD_INT("O_CLOEXEC",  O_CLOEXEC),
    LUABSD_INT("O_VERIFY",   O_VERIFY),
    LUABSD_INT(NULL, 0)
};

static luab_table_t luab_stdlib[] = {   /* stdlib.h */
    LUABSD_FUNC("arc4random", luab_arc4random),
    LUABSD_FUNC("arc4random_uniform", luab_arc4random_uniform),
    LUABSD_FUNC(NULL, NULL)
};

static luab_table_t luab_unistd[] = {   /* unistd.h */
    LUABSD_FUNC("fork",   luab_fork),
    LUABSD_FUNC("getegid",    luab_getegid),
    LUABSD_FUNC("geteuid",    luab_geteuid),
    LUABSD_FUNC("getgid",    luab_getgid),
    LUABSD_FUNC("getlogin",   luab_getlogin),
    LUABSD_FUNC("getpid", luab_getpid),
    LUABSD_FUNC("getppid",    luab_getppid),
    LUABSD_FUNC("getpgid",    luab_getpgid),
    LUABSD_FUNC("getpgrp",    luab_getpgrp),
    LUABSD_FUNC("getuid", luab_getuid),
    LUABSD_FUNC("getsid", luab_getsid),
    LUABSD_FUNC("setegid",    luab_setegid),
    LUABSD_FUNC("seteuid",    luab_seteuid),
    LUABSD_FUNC("setgid",    luab_setgid),
    LUABSD_FUNC("setlogin",   luab_setlogin),
    LUABSD_FUNC("setpgid",    luab_setpgid),
    LUABSD_FUNC("setpgrp",    luab_setpgrp),
    LUABSD_FUNC("setsid", luab_setsid),
    LUABSD_FUNC("setuid", luab_setuid),
    LUABSD_FUNC(NULL, NULL)
};

static luab_table_t luab_uuid[] = { /* uuid.h */
    LUABSD_FUNC("uuidgen",    luab_uuidgen),
    LUABSD_FUNC(NULL, NULL)
};

/*
 * Called, when package.loadlib takes place.
 */
LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    lua_newtable(L);

    lua_newtable(L);

    luab_newtable(L, luab_sys_file, "file");
    luab_newtable(L, luab_sys_stat, "stat");
    luab_newtable(L, luab_sys_time, "time");

    lua_setfield(L, -2, "sys");

    luab_newtable(L, luab_db, "db");
    luab_newtable(L, luab_fcntl, "fcntl");
    luab_newtable(L, luab_stdlib, "stdlib");
    luab_newtable(L, luab_unistd, "unistd");
    luab_newtable(L, luab_uuid, "uuid");

    lua_pushvalue(L, -1);

    luaL_newmetatable(L, LUABSD_DB);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, luab_db_db, 0);
    lua_pop(L, 1);

    return 1;
}
