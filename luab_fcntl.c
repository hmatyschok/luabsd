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

#include <sys/limits.h>

#include <errno.h>
#include <fcntl.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

#define LUABSD_FLOCK    "FLOCK*"

typedef struct {
    struct flock    fl;
} luab_flock_t;

#define luab_toflock(L, narg) \
    ((luab_db_t *)luaL_checkudata(L, narg, LUABSD_FLOCK))

static int
luab_posix_fadvise(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    off_t offset = luab_checkinteger(L, 2, UINT_MAX);
    off_t len = luab_checkinteger(L, 3, UINT_MAX);
    int advice = luab_checkinteger(L, 4, INT_MAX);

    if ((errno = posix_fadvise(fd, offset, len, advice)) != 0)
        return luab_pusherr(L, errno);

    lua_pushinteger(L, errno);
    return 1;
}

luab_table_t luab_fcntl[] = {    /* fcntl.h */
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
    LUABSD_INT("FAPPEND",   FAPPEND),
    LUABSD_INT("FASYNC",    FASYNC),
    LUABSD_INT("FFSYNC",    FFSYNC),
    LUABSD_INT("FNONBLOCK", FNONBLOCK),
    LUABSD_INT("FNDELAY",   FNDELAY),
    LUABSD_INT("O_NDELAY",  O_NDELAY),
    LUABSD_INT("FRDAHEAD",  FRDAHEAD),
    LUABSD_INT("AT_FDCWD",  AT_FDCWD),
    LUABSD_INT("AT_EACCESS",    AT_EACCESS),
    LUABSD_INT("AT_SYMLINK_NOFOLLOW",   AT_SYMLINK_NOFOLLOW),
    LUABSD_INT("AT_SYMLINK_FOLLOW", AT_SYMLINK_FOLLOW),
    LUABSD_INT("AT_REMOVEDIR",  AT_REMOVEDIR),
    LUABSD_INT("F_DUPFD",   F_DUPFD),
    LUABSD_INT("F_GETFD",   F_GETFD),
    LUABSD_INT("F_SETFD",   F_SETFD),
    LUABSD_INT("F_GETFL",   F_GETFL),
    LUABSD_INT("F_SETFL",   F_SETFL),
    LUABSD_INT("F_GETOWN",  F_GETOWN),
    LUABSD_INT("F_SETOWN",  F_SETOWN),
    LUABSD_INT("F_OGETLK",  F_OGETLK),
    LUABSD_INT("F_OSETLK",  F_OSETLK),
    LUABSD_INT("F_OSETLKW", F_OSETLKW),
    LUABSD_INT("F_DUP2FD",  F_DUP2FD),
    LUABSD_INT("F_GETLK",   F_GETLK),
    LUABSD_INT("F_SETLK",   F_SETLK),
    LUABSD_INT("F_SETLKW",  F_SETLKW),
    LUABSD_INT("F_SETLK_REMOTE",    F_SETLK_REMOTE),
    LUABSD_INT("F_READAHEAD",   F_READAHEAD),
    LUABSD_INT("F_RDAHEAD", F_RDAHEAD),
    LUABSD_INT("F_DUPFD_CLOEXEC",   F_DUPFD_CLOEXEC),
    LUABSD_INT("F_DUP2FD_CLOEXEC",  F_DUP2FD_CLOEXEC),
    LUABSD_INT("FD_CLOEXEC",    FD_CLOEXEC),
    LUABSD_INT("F_RDLCK",   F_RDLCK),
    LUABSD_INT("F_UNLCK",   F_UNLCK),
    LUABSD_INT("F_WRLCK",   F_WRLCK),
    LUABSD_INT("F_UNLCKSYS",    F_UNLCKSYS),
    LUABSD_INT("F_CANCEL",  F_CANCEL),
    LUABSD_INT("LOCK_SH",   LOCK_SH),
    LUABSD_INT("LOCK_EX",   LOCK_EX),
    LUABSD_INT("LOCK_NB",   LOCK_NB),
    LUABSD_INT("LOCK_UN",   LOCK_UN),
    LUABSD_INT("POSIX_FADV_NORMAL", POSIX_FADV_NORMAL),
    LUABSD_INT("POSIX_FADV_RANDOM", POSIX_FADV_RANDOM),
    LUABSD_INT("POSIX_FADV_SEQUENTIAL", POSIX_FADV_SEQUENTIAL),
    LUABSD_INT("POSIX_FADV_WILLNEED",   POSIX_FADV_WILLNEED),
    LUABSD_INT("POSIX_FADV_DONTNEED",   POSIX_FADV_DONTNEED),
    LUABSD_INT("POSIX_FADV_NOREUSE",    POSIX_FADV_NOREUSE),
    LUABSD_FUNC("posix_fadvise",    luab_posix_fadvise),
    LUABSD_FUNC(NULL, NULL)
};
