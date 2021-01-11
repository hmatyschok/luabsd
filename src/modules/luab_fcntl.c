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

#include <sys/stat.h>

#include <fcntl.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_FCNTL_LIB_ID    1593623310
#define LUAB_FCNTL_LIB_KEY    "fcntl"

extern luab_module_t luab_fcntl_lib;

/*
 * Service primitives.
 */

/***
 * open(2) - open or create for reading, writing or executing
 *
 * @function open
 *
 * @param path              The specified file name.
 * @param flags             Values are constructed from
 *
 *                              bsd.fcntl.O_*
 *
 *                          by bitwise-inclusive OR.
 * @param mode              Values are constructed from
 *
 *                              bsd.sys.stat.S_*
 *
 *                          by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage fd [, err, msg ] = bsd.fcntl.open(path, flags [, mode ])
 */
static int
luab_open(lua_State *L)
{
    luab_module_t *m0, *m1;
    int narg;
    const char *path;
    int flags;
    mode_t mode;
    int fd;

    narg = luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(MODE, TYPE, __func__);

    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    flags = (int)luab_checkxinteger(L, 2, m0, luab_env_int_max);
    mode = 0;

    if ((narg == 3) &&
        ((flags & O_CREAT) != 0))
        mode = (mode_t)luab_checkxinteger(L, narg, m1, ALLPERMS);

    fd = open(path, flags, mode);

    return (luab_pushxinteger(L, fd));
}

/***
 * creat(2) - create a new file
 *
 * @function creat
 *
 * @param path              The specified file name.
 * @param mode              Values are constructed from
 *
 *                              bsd.sys.stat.S_*
 *
 *                          by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage fd [, err, msg ] = bsd.fcntl.creat(path, mode)
 */
static int
luab_creat(lua_State *L)
{
    luab_module_t *m;
    const char *path;
    mode_t mode;
    int fd;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(MODE, TYPE, __func__);
    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    mode = (mode_t)luab_checkxinteger(L, 2, m, ALLPERMS);

    fd = creat(path, mode);

    return (luab_pushxinteger(L, fd));
}

/***
 * fcntl(2) - file control
 *
 * @function fcntl
 *
 * @param fd                Open file descriptor.
 * @param cmd               Command operated on fd over
 *
 *                              bsd.fcntl.F_*,
 *
 *                          as described in fcntl(2).
 * @param arg               Optional, depends on applied cmd.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage value [, err, msg ] = bsd.fcntl.fcntl(fd, cmd [, arg ])
 */
static int
luab_fcntl(lua_State *L)
{
    int narg;
    luab_module_t *m0, *m1;
    int fd, cmd, arg, status;
    struct flock *argp;

    narg = luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(FLOCK, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    cmd = (int)luab_checkxinteger(L, 2, m0, luab_env_int_max);

    arg = 0;
    argp = NULL;

    if (narg == 3) {

        if (lua_type(L, narg) == LUA_TUSERDATA)
            argp = luab_udata(L, narg, m1, struct flock *);
        else
            arg = luab_checkxinteger(L, narg, m0, luab_env_int_max);
    }

    if (argp != NULL)
        status = fcntl(fd, cmd, argp);
    else
        status = fcntl(fd, cmd, arg);

    return (luab_pushxinteger(L, status));
}

#if __BSD_VISIBLE
/***
 * flock(2) - apply or remove an advisory lock on an open file
 *
 * @function flock
 *
 * @param fd                Open file descriptor.
 * @param operation         Specified by
 *
 *                              bsd.sys.file.LOCK_*,
 *
 *                          over <sys/file.h>.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage value [, err, msg ] = bsd.fcntl.flock(fd, operation)
 */
static int
luab_flock(lua_State *L)
{
    luab_module_t *m;
    int fd, operation, status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(INT, TYPE, __func__);
    fd = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    operation = (int)luab_checkxinteger(L, 2, m, luab_env_int_max);
    status = flock(fd, operation);

    return (luab_pushxinteger(L, status));
}
#endif

#if __POSIX_VISIBLE >= 200809
/***
 * openat(2) - apply or remove an advisory lock on an open file
 *
 * @function openat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes referenced file.
 *
 *                            #2 By path named file is relative to the directory
 *                               associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of openat(2).
 *
 * @param path              Specified file name.
 * @param flags             Values are constructed from
 *
 *                              bsd.fcntl.O_*
 *
 *                          by bitwise-inclusive OR.
 * @param mode              Values are constructed from
 *
 *                              bsd.sys.stat.S_*
 *
 *                          by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage value [, err, msg ] = bsd.fcntl.openat(fd, path, flags [, mode ])
 */
static int
luab_openat(lua_State *L)
{
    luab_module_t *m0, *m1;
    int narg, dirfd;
    const char *path;
    int flags;
    mode_t mode;
    int fd;

    narg = luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(MODE, TYPE, __func__);

    dirfd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    path = luab_checklstring(L, 2, luab_env_path_max, NULL);
    flags = (int)luab_checkxinteger(L, 3, m0, luab_env_int_max);
    mode = 0;

    if (narg == 4 && (flags & O_CREAT) != 0)
        mode = luab_checkxinteger(L, narg, m1, ALLPERMS);

    fd = openat(dirfd, path, flags, mode);

    return (luab_pushxinteger(L, fd));
}
#endif

#if __POSIX_VISIBLE >= 200112
/***
 * posix_fadvise(2) - give advise about use of file data
 *
 * @function posix_fadvise
 *
 * @param fd                Open file descriptor.
 * @param offset            Specifies the point where the advice starts covering.
 * @param len               Specifies range for got advise or the file entirely
 *                          by 0.
 *
 * @param advice            Specifies advice parameter by values from:
 *
 *                              bsd.fcntl.POSIX_FADV_{
 *                                  NORMAL,
 *                                  RANDOM,
 *                                  SEQUENTIAL,
 *                                  WILLNEED,
 *                                  DONTNEED,
 *                                  NOREUSE
 *                              }
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage value [, err, msg ] = bsd.fcntl.posix_fadvise(fd, offset, len, advice)
 */
static int
luab_posix_fadvise(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd;
    off_t offset;
    off_t len;
    int advice;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(OFF, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    offset = (off_t)luab_checkxinteger(L, 2, m1, luab_env_int_max);
    len = (off_t)luab_checkxinteger(L, 3, m1, luab_env_int_max);
    advice = (int)luab_checkxinteger(L, 4, m0, luab_env_int_max);

    if ((errno = posix_fadvise(fd, offset, len, advice)) != 0)
        status = luab_env_error;
    else
        status = luab_env_success;

    return (luab_pushxinteger(L, status));
}

/***
 * posix_fallocate(2) - pre-allocate storage for a range in a file
 *
 * @function posix_fallocate
 *
 * @param fd                Open file descriptor.
 * @param offset            Specifies the point where the allocated range starts.
 * @param len               Specifies range for got advise or the file entirely
 *                          by 0.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage value [, err, msg ] = bsd.fcntl.posix_fallocate(fd, offset, len, advice)
 */
static int
luab_posix_fallocate(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd;
    off_t offset;
    off_t len;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(OFF, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    offset = (off_t)luab_checkxinteger(L, 2, m1, luab_env_int_max);
    len = (off_t)luab_checkxinteger(L, 3, m1, luab_env_int_max);

    if ((errno = posix_fallocate(fd, offset, len)) != 0)
        status = luab_env_error;
    else
        status = luab_env_success;

    return (luab_pushxinteger(L, status));
}
#endif

/*
 * Generator functions.
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(MODE)).
 *
 * @function mode_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(MODE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage mode [, err, msg ] = bsd.fcntl.mode_create(arg)
 */
static int
luab_mode_create(lua_State *L)
{
    luab_module_t *m;
    mode_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(MODE, TYPE, __func__);
    x = (mode_t)luab_checkxinteger(L, 1, m, luab_env_ushrt_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(FLOCK)).
 *
 * @function flock_create
 *
 * @param arg           Instance of (LUA_TUSERDATA(FLOCK)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage flock [, err, msg ] = bsd.fcntl.flock_create([ arg ])
 */
static int
luab_flock_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FLOCK, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/*
 * Interface against <fcntl.h>.
 */

static luab_module_table_t luab_fcntl_vec[] = {
    LUAB_INT("O_RDONLY",                O_RDONLY),
    LUAB_INT("O_WRONLY",                O_WRONLY),
    LUAB_INT("O_RDWR",                  O_RDWR),
    LUAB_INT("O_ACCMODE",               O_ACCMODE),
#if __BSD_VISIBLE
    LUAB_INT("FREAD",                   FREAD),
    LUAB_INT("FWRITE",                  FWRITE),
#endif
    LUAB_INT("O_NONBLOCK",              O_NONBLOCK),
    LUAB_INT("O_APPEND",                O_APPEND),
#if __BSD_VISIBLE
    LUAB_INT("O_SHLOCK",                O_SHLOCK),
    LUAB_INT("O_EXLOCK",                O_EXLOCK),
    LUAB_INT("O_ASYNC",                 O_ASYNC),
    LUAB_INT("O_FSYNC",                 O_FSYNC),
#endif
    LUAB_INT("O_SYNC",                  O_SYNC),
#if __POSIX_VISIBLE >= 200809
    LUAB_INT("O_NOFOLLOW",              O_NOFOLLOW),
#endif
    LUAB_INT("O_CREAT",                 O_CREAT),
    LUAB_INT("O_TRUNC",                 O_TRUNC),
    LUAB_INT("O_EXCL",                  O_EXCL),
    LUAB_INT("O_NOCTTY",                O_NOCTTY),
#if __BSD_VISIBLE
    LUAB_INT("O_DIRECT",                O_DIRECT),
#endif
#if __POSIX_VISIBLE >= 200809
    LUAB_INT("O_DIRECTORY",             O_DIRECTORY),
    LUAB_INT("O_EXEC",                  O_EXEC),
#endif
#if __POSIX_VISIBLE >= 200809
    LUAB_INT("O_TTY_INIT",              O_TTY_INIT),
    LUAB_INT("O_CLOEXEC",               O_CLOEXEC),
#endif
#if __BSD_VISIBLE
    LUAB_INT("O_VERIFY",                O_VERIFY),
    LUAB_INT("FAPPEND",                 FAPPEND),
    LUAB_INT("FASYNC",                  FASYNC),
    LUAB_INT("FFSYNC",                  FFSYNC),
    LUAB_INT("FNONBLOCK",               FNONBLOCK),
    LUAB_INT("FNDELAY",                 FNDELAY),
    LUAB_INT("O_NDELAY",                O_NDELAY),
    LUAB_INT("FRDAHEAD",                FRDAHEAD),
#endif
#if __POSIX_VISIBLE >= 200809
    LUAB_INT("AT_FDCWD",                AT_FDCWD),
    LUAB_INT("AT_EACCESS",              AT_EACCESS),
    LUAB_INT("AT_SYMLINK_NOFOLLOW",     AT_SYMLINK_NOFOLLOW),
    LUAB_INT("AT_SYMLINK_FOLLOW",       AT_SYMLINK_FOLLOW),
    LUAB_INT("AT_REMOVEDIR",            AT_REMOVEDIR),
#endif
    LUAB_INT("F_DUPFD",                 F_DUPFD),
    LUAB_INT("F_GETFD",                 F_GETFD),
    LUAB_INT("F_SETFD",                 F_SETFD),
    LUAB_INT("F_GETFL",                 F_GETFL),
    LUAB_INT("F_SETFL",                 F_SETFL),
#if __XSI_VISIBLE || __POSIX_VISIBLE >= 200112
    LUAB_INT("F_GETOWN",                F_GETOWN),
    LUAB_INT("F_SETOWN",                F_SETOWN),
#endif
#if __BSD_VISIBLE
    LUAB_INT("F_OGETLK",                F_OGETLK),
    LUAB_INT("F_OSETLK",                F_OSETLK),
    LUAB_INT("F_OSETLKW",               F_OSETLKW),
    LUAB_INT("F_DUP2FD",                F_DUP2FD),
#endif
    LUAB_INT("F_GETLK",                 F_GETLK),
    LUAB_INT("F_SETLK",                 F_SETLK),
    LUAB_INT("F_SETLKW",                F_SETLKW),
#if __BSD_VISIBLE
    LUAB_INT("F_SETLK_REMOTE",          F_SETLK_REMOTE),
    LUAB_INT("F_READAHEAD",             F_READAHEAD),
    LUAB_INT("F_RDAHEAD",               F_RDAHEAD),
#endif
#if __POSIX_VISIBLE >= 200809
    LUAB_INT("F_DUPFD_CLOEXEC",         F_DUPFD_CLOEXEC),
#endif
#if __BSD_VISIBLE
    LUAB_INT("F_DUP2FD_CLOEXEC",        F_DUP2FD_CLOEXEC),
#endif
    LUAB_INT("FD_CLOEXEC",              FD_CLOEXEC),
    LUAB_INT("F_RDLCK",                 F_RDLCK),
    LUAB_INT("F_UNLCK",                 F_UNLCK),
    LUAB_INT("F_WRLCK",                 F_WRLCK),
#if __BSD_VISIBLE
    LUAB_INT("F_UNLCKSYS",              F_UNLCKSYS),
    LUAB_INT("F_CANCEL",                F_CANCEL),
    LUAB_INT("LOCK_SH",                 LOCK_SH),
    LUAB_INT("LOCK_EX",                 LOCK_EX),
    LUAB_INT("LOCK_NB",                 LOCK_NB),
    LUAB_INT("LOCK_UN",                 LOCK_UN),
#endif
#if __POSIX_VISIBLE >= 200112
    LUAB_INT("POSIX_FADV_NORMAL",       POSIX_FADV_NORMAL),
    LUAB_INT("POSIX_FADV_RANDOM",       POSIX_FADV_RANDOM),
    LUAB_INT("POSIX_FADV_SEQUENTIAL",   POSIX_FADV_SEQUENTIAL),
    LUAB_INT("POSIX_FADV_WILLNEED",     POSIX_FADV_WILLNEED),
    LUAB_INT("POSIX_FADV_DONTNEED",     POSIX_FADV_DONTNEED),
    LUAB_INT("POSIX_FADV_NOREUSE",      POSIX_FADV_NOREUSE),
#endif
    LUAB_FUNC("open",                   luab_open),
    LUAB_FUNC("creat",                  luab_creat),
    LUAB_FUNC("fcntl",                  luab_fcntl),
#if __BSD_VISIBLE
    LUAB_FUNC("flock",                  luab_flock),
#endif
#if __POSIX_VISIBLE >= 200809
    LUAB_FUNC("openat",                 luab_openat),
#endif
#if __POSIX_VISIBLE >= 200112
    LUAB_FUNC("posix_fadvise",          luab_posix_fadvise),
    LUAB_FUNC("posix_fallocate",        luab_posix_fallocate),
#endif
    LUAB_FUNC("mode_create",            luab_mode_create),
    LUAB_FUNC("flock_create",           luab_flock_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_fcntl_lib = {
    .m_id       = LUAB_FCNTL_LIB_ID,
    .m_name     = LUAB_FCNTL_LIB_KEY,
    .m_vec      = luab_fcntl_vec,
};
