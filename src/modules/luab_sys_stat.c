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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#define LUAB_SYS_STAT_LIB_ID    1593623310
#define LUAB_SYS_STAT_LIB_KEY    "stat"

extern luab_module_t luab_sys_stat_lib;


/*
 * Service primitives.
 */

#if __BSD_VISIBLE
/***
 * chflags(2) - set file flags
 *
 * @function chflags
 *
 * @param path              File pointed to by path.
 * @param flags             File flags
 *
 *                             #1 bsd.sys.stat.SF_{APPEND,ARCHIVED,IMMUTABLE,
 *                                  NOUNLINK,SNAPSHOT}
 *
 *                             #2 bsd.sys.stat.UF_{APPEND,ARCHIVE,HIDDEN,
 *                                  IMMUTABLE,NODUMP,NOUNLINK,OFFLINE,
 *                                  OPAQUE,READONLY,REPARSE,SPARSE,SYSTEM}
 *
 *                          are formatted by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.chflags(path, flags)
 */
static int
luab_chflags(lua_State *L)
{
    luab_module_t *m;
    const char *path;
    u_long flags;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(ULONG, TYPE, __func__);

    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    flags = (u_long)luab_checkxinteger(L, 2, m, luab_env_ulong_max);

    status = chflags(path, flags);

    return (luab_pushxinteger(L, status));
}

/***
 * chflagsat(2) - set file flags
 *
 * @function chflagsat
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
 *                               was passed by call of chflagsat(2).
 *
 * @param path              File named by path.
 * @param flags             File flags
 *
 *                             #1 bsd.sys.stat.SF_{APPEND,ARCHIVED,IMMUTABLE,
 *                                  NOUNLINK,SNAPSHOT}
 *
 *                             #2 bsd.sys.stat.UF_{APPEND,ARCHIVE,HIDDEN,
 *                                  IMMUTABLE,NODUMP,NOUNLINK,OFFLINE,
 *                                  OPAQUE,READONLY,REPARSE,SPARSE,SYSTEM}
 *
 *                          are formatted by inclusive or.
 * @param atflags           Passed values are formatted by inclusive or:
 *
 *                              bsd.fcntl.AT_SYMLINK_NOFOLLOW.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.chflagsat(fd, path, flags, atflags)
 */
static int
luab_chflagsat(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd;
    const char *path;
    u_long flags;
    int atflag;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(ULONG, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    path = luab_checklstring(L, 2, luab_env_path_max, NULL);
    flags = (u_long)luab_checkxinteger(L, 3, m1, luab_env_ulong_max);
    atflag = (int)luab_checkxinteger(L, 4, m0, luab_env_int_max);

    status = chflagsat(fd, path, flags, atflag);

    return (luab_pushxinteger(L, status));
}
#endif

/***
 * chmod(2) - change mode of file
 *
 * @function chmod
 *
 * @param path              File named by path.
 * @param mode              Permission bit masks
 *
 *                              bsd.sys.stat.S_{
 *                                  IRWXU,          -- RWX mask for owner
 *                                  IRUSR,          -- R for owner
 *                                  IWUSR,          -- W for owner
 *                                  IXUSR,          -- X for owner
 *                                  IRWXG,          -- RWX mask for group
 *                                  IRGRP,          -- R for group
 *                                  IWGRP,          -- W for group
 *                                  IXGRP,          -- X for group
 *                                  IRWXO,          -- RWX mask for other
 *                                  IROTH,          -- R for other
 *                                  IWOTH,          -- W for other
 *                                  IXOTH,          -- X for other
 *                                  ISUID,          -- set user id on
 *                                  ISGID,          -- set group id on
 *                                  ISVTX           -- sticky bit
 *                              }
 *
 *                          are formatted by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.chmod(path, mode)
 */
static int
luab_chmod(lua_State *L)
{
    luab_module_t *m;
    const char *path;
    mode_t mode;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(MODE, TYPE, __func__);

    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    mode = (mode_t)luab_checkxinteger(L, 2, m, ALLPERMS);

    status = chmod(path, mode);

    return (luab_pushxinteger(L, status));
}

#if __BSD_VISIBLE
/***
 * fchflags(2) - set file flags
 *
 * @function fchflags
 *
 * @param fd                Filedescriptor.
 * @param flags             File flags
 *
 *                             #1 bsd.sys.stat.SF_{APPEND,ARCHIVED,IMMUTABLE,
 *                                  NOUNLINK,SNAPSHOT}
 *
 *                             #2 bsd.sys.stat.UF_{APPEND,ARCHIVE,HIDDEN,
 *                                  IMMUTABLE,NODUMP,NOUNLINK,OFFLINE,
 *                                  OPAQUE,READONLY,REPARSE,SPARSE,SYSTEM}
 *
 *                          are formatted by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.fchflags(path, flags)
 */
static int
luab_fchflags(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd;
    u_long flags;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(ULONG, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    flags = (u_long)luab_checkxinteger(L, 2, m1, luab_env_ulong_max);

    status = fchflags(fd, flags);

    return (luab_pushxinteger(L, status));
}
#endif
#if __POSIX_VISIBLE >= 200112
/***
 * fchmod(2) - change mode of file
 *
 * @function fchmod
 *
 * @param fd                File named by path.
 * @param mode              Permission bit masks
 *
 *                              bsd.sys.stat.S_{
 *                                  IRWXU,          -- RWX mask for owner
 *                                  IRUSR,          -- R for owner
 *                                  IWUSR,          -- W for owner
 *                                  IXUSR,          -- X for owner
 *                                  IRWXG,          -- RWX mask for group
 *                                  IRGRP,          -- R for group
 *                                  IWGRP,          -- W for group
 *                                  IXGRP,          -- X for group
 *                                  IRWXO,          -- RWX mask for other
 *                                  IROTH,          -- R for other
 *                                  IWOTH,          -- W for other
 *                                  IXOTH,          -- X for other
 *                                  ISUID,          -- set user id on
 *                                  ISGID,          -- set group id on
 *                                  ISVTX           -- sticky bit
 *                              }
 *
 *                          are formatted by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.fchmod(fd, mode)
 */
static int
luab_fchmod(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd;
    mode_t mode;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(MODE, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    mode = (mode_t)luab_checkxinteger(L, 2, m1, ALLPERMS);

    status = fchmod(fd, mode);

    return (luab_pushxinteger(L, status));
}
#endif
#if __POSIX_VISIBLE >= 200809
/***
 * fchmodat(2) - change mode of file
 *
 * @function fchmodat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes newly created directory.
 *
 *                            #2 The newly created directory is relative to the
 *                               directory associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of fchmodat(2).
 *
 * @param mode              Permission bit masks
 *
 *                              bsd.sys.stat.S_{
 *                                  IRWXU,          -- RWX mask for owner
 *                                  IRUSR,          -- R for owner
 *                                  IWUSR,          -- W for owner
 *                                  IXUSR,          -- X for owner
 *                                  IRWXG,          -- RWX mask for group
 *                                  IRGRP,          -- R for group
 *                                  IWGRP,          -- W for group
 *                                  IXGRP,          -- X for group
 *                                  IRWXO,          -- RWX mask for other
 *                                  IROTH,          -- R for other
 *                                  IWOTH,          -- W for other
 *                                  IXOTH,          -- X for other
 *                                  ISUID,          -- set user id on
 *                                  ISGID,          -- set group id on
 *                                  ISVTX           -- sticky bit
 *                              }
 *
 *                          are formatted by inclusive or.
 *
 * @param flag              Passed values are formatted by inclusive or:
 *
 *                              bsd.fcntl.AT_SYMLINK_NOFOLLOW.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.fchmodat(fd, path, mode, flag)
 */
static int
luab_fchmodat(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd;
    const char *path;
    mode_t mode;
    int flag;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(MODE, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    path = luab_checklstring(L, 2, luab_env_path_max, NULL);
    mode = (mode_t)luab_checkxinteger(L, 3, m1, ALLPERMS);
    flag = (int)luab_checkxinteger(L, 4, m0, luab_env_int_max);

    status = fchmodat(fd, path, mode, flag);

    return (luab_pushxinteger(L, status));
}

/***
 * futimens(2) - set file access and modifications times
 *
 * @function futimens
 *
 * @param fd                Filedescriptor.
 * @param times             An instance of (LUA_TTABLE) carrying at least
 *
 *                              {
 *                                  (LUA_TUSERDATA(TIMESPEC)),
 *                                  (LUA_TUSERDATA(TIMESPEC))
 *                              }
 *
 *                          two instances of (LUA_TUSERDATA(TIMESPEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.futimens(fd, times)
 */
static int
luab_futimens(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd;
    luab_table_t *tbl;
    struct timespec *times;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(TIMESPEC, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);

    if (lua_isnil(L, 2) != 0)
        tbl = luab_table_checklxdata(L, 2, m1, 2);
    else
        tbl = NULL;

    if (tbl != NULL)
        times = (struct timespec *)(tbl->tbl_vec);
    else
        times = NULL;

    status = futimens(fd, times);

    if (times != NULL)
        luab_table_pushxdata(L, 2, m1, tbl, 0, 1);

    return (luab_pushxinteger(L, status));
}

/***
 * utimensat(2) - set file access and modifications times
 *
 * @function utimensat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes referred file.
 *
 *                            #2 By path named file is relative to the directory
 *                               associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of utimensat(2).
 *
 * @param times             An instance of (LUA_TTABLE) carrying at least
 *
 *                              {
 *                                  (LUA_TUSERDATA(TIMESPEC)),
 *                                  (LUA_TUSERDATA(TIMESPEC))
 *                              }
 *
 *                          two instances of (LUA_TUSERDATA(TIMESPEC)).
 *
 * @param flag              Passed values are formatted by inclusive or:
 *
 *                              bsd.fcntl.AT_SYMLINK_NOFOLLOW.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.utimensat(fd, path, times, flag)
 */
static int
luab_utimensat(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd;
    const char *path;
    luab_table_t *tbl;
    struct timespec *times;
    int flag, status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(TIMESPEC, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    path = luab_checklstring(L, 2, luab_env_path_max, NULL);

    if (lua_isnil(L, 3) != 0)
        tbl = luab_table_checklxdata(L, 2, m1, 2);
    else
        tbl = NULL;

    if (tbl != NULL)
        times = (struct timespec *)(tbl->tbl_vec);
    else
        times = NULL;

    flag = (int)luab_checkxinteger(L, 4, m0, luab_env_int_max);

    status = utimensat(fd, path, times, flag);

    if (times != NULL)
        luab_table_pushxdata(L, 2, m1, tbl, 0, 1);

    return (luab_pushxinteger(L, status));
}

#endif /* __POSIX_VISIBLE >= 200809 */

/***
 * fstat(2) - get file status
 *
 * @function fstat
 *
 * @param fd                Filedescriptor referres used file for examination.
 * @param sb                Result argument, instance of (LUA_TUSERDATA(STAT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.fstat(fd, sb)
 */
static int
luab_fstat(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd;
    struct stat *sb;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(STAT, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    sb = luab_udata(L, 2, m1, struct stat *);

    status = fstat(fd, sb);

    return (luab_pushxinteger(L, status));
}

#if __BSD_VISIBLE
/***
 * lchflags(2) - set file flags
 *
 * @function lchflags
 *
 * @param path              File named by path.
 * @param flags             File flags
 *
 *                             #1 bsd.sys.stat.SF_{APPEND,ARCHIVED,IMMUTABLE,
 *                                  NOUNLINK,SNAPSHOT}
 *
 *                             #2 bsd.sys.stat.UF_{APPEND,ARCHIVE,HIDDEN,
 *                                  IMMUTABLE,NODUMP,NOUNLINK,OFFLINE,
 *                                  OPAQUE,READONLY,REPARSE,SPARSE,SYSTEM}
 *
 *                          are formatted by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.lchflags(path, flags)
 */
static int
luab_lchflags(lua_State *L)
{
    luab_module_t *m;
    const char *path;
    u_long flags;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(ULONG, TYPE, __func__);

    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    flags = (u_long)luab_checkxinteger(L, 2, m, luab_env_ulong_max);

    status = lchflags(path, flags);

    return (luab_pushxinteger(L, status));
}

/***
 * lchmod(2) - change mode of file
 *
 * @function lchmod
 *
 * @param path              File named by path.
 * @param mode              Permission bit masks
 *
 *                              bsd.sys.stat.S_{
 *                                  IRWXU,          -- RWX mask for owner
 *                                  IRUSR,          -- R for owner
 *                                  IWUSR,          -- W for owner
 *                                  IXUSR,          -- X for owner
 *                                  IRWXG,          -- RWX mask for group
 *                                  IRGRP,          -- R for group
 *                                  IWGRP,          -- W for group
 *                                  IXGRP,          -- X for group
 *                                  IRWXO,          -- RWX mask for other
 *                                  IROTH,          -- R for other
 *                                  IWOTH,          -- W for other
 *                                  IXOTH,          -- X for other
 *                                  ISUID,          -- set user id on
 *                                  ISGID,          -- set group id on
 *                                  ISVTX           -- sticky bit
 *                              }
 *
 *                          are formatted by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.lchmod(path, mode)
 */
static int
luab_lchmod(lua_State *L)
{
    luab_module_t *m;
    const char *path;
    mode_t mode;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(MODE, TYPE, __func__);

    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    mode = (mode_t)luab_checkxinteger(L, 2, m, ALLPERMS);

    status = lchmod(path, mode);

    return (luab_pushxinteger(L, status));
}
#endif
#if __POSIX_VISIBLE >= 200112
/***
 * lstat(2) - get file status
 *
 * @function lstat
 *
 * @param path              File pointed to by path.
 * @param sb                Result argument, instance of (LUA_TUSERDATA(STAT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.lstat(path, sb)
 */
static int
luab_lstat(lua_State *L)
{
    luab_module_t *m;
    const char *path;
    struct stat *sb;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(STAT, TYPE, __func__);
    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    sb = luab_udata(L, 2, m, struct stat *);

    status = lstat(path, sb);

    return (luab_pushxinteger(L, status));
}
#endif

/***
 * mkdir(2) - make a directory file
 *
 * @function mkdir
 *
 * @param path              The directory path.
 * @param mode              Specifies access permissions, restricted
 *                          by the umask(2) of calling process.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.mkdir(path, mode)
 */
static int
luab_mkdir(lua_State *L)
{
    luab_module_t *m;
    const char *path;
    mode_t mode;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(MODE, TYPE, __func__);

    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    mode = (mode_t)luab_checkxinteger(L, 2, m, ALLPERMS);

    status = mkdir(path, mode);

    return (luab_pushxinteger(L, status));
}

/***
 * mkfifo(2) - make a fifo file
 *
 * @function mkfifo
 *
 * @param path              Specifies the FIFO (with name path).
 * @param mode              Specifies access permissions of newly created
 *                          FIFO.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.mkfifo(path, mode)
 */
static int
luab_mkfifo(lua_State *L)
{
    luab_module_t *m;
    const char *path;
    mode_t mode;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(MODE, TYPE, __func__);

    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    mode = (mode_t)luab_checkxinteger(L, 2, m, ALLPERMS);

    status = mkfifo(path, mode);

    return (luab_pushxinteger(L, status));
}

#if !defined(_MKNOD_DECLARED) && __XSI_VISIBLE
/***
 * mknod(2) - make a special file node
 *
 * @function mknodat
 *
 * @param path              Specifies the system file node (path).
 * @param mode              Specifies access permissions of newly created
 *                          system file node
 * @param dev               Specification, denotes particular device.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.mknodat(fd, path, mode)
 */
static int
luab_mknod(lua_State *L)
{
    luab_module_t *m0, *m1;
    const char *path;
    mode_t mode;
    dev_t dev;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(MODE, TYPE, __func__);
    m1 = luab_xmod(DEV, TYPE, __func__);

    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    mode = (mode_t)luab_checkxinteger(L, 2, m0, ALLPERMS);
    dev = (dev_t)luab_checkxinteger(L, 3, m1, luab_env_ulong_max);

    status = mknod(path, mode, dev);

    return (luab_pushxinteger(L, status));
}
#define _MKNOD_DECLARED
#endif

/***
 * stat(2) - get file status
 *
 * @function stat
 *
 * @param path              File pointed to by path.
 * @param sb                Result argument, instance of (LUA_TUSERDATA(STAT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.stat(path, sb)
 */
static int
luab_stat(lua_State *L)
{
    luab_module_t *m;
    const char *path;
    struct stat *sb;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(STAT, TYPE, __func__);

    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    sb = luab_udata(L, 2, m, struct stat *);

    status = stat(path, sb);

    return (luab_pushxinteger(L, status));
}

/***
 * umask(2) - set file creation mode mask
 *
 * @function umask
 *
 * @param numask            Mode mask.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.umask(umask)
 */
static int
luab_umask(lua_State *L)
{
    luab_module_t *m;
    mode_t numask;
    mode_t oumask;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(MODE, TYPE, __func__);

    numask = (mode_t)luab_checkxinteger(L, 1, m, ALLPERMS);
    oumask = umask(numask);

    return (luab_pushxinteger(L, oumask));
}

#if __POSIX_VISIBLE >= 200809
/***
 * fstatat(2) - get file status
 *
 * @function fstatat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Referres used file for examination.
 *
 *                            #2 The focussed file is relative to the directory
 *                               associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of fstatat(2).
 *
 * @param path              By path named file.
 * @param sb                Result argument, instance of (LUA_TUSERDATA(STAT)).
 * @param flag              Passed values are formatted by inclusive or:
 *
 *                              bsd.fcntl.AT_SYMLINK_NOFOLLOW.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.fstatat(fd, path, sb, flag)
 */
static int
luab_fstatat(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd;
    const char *path;
    struct stat *sb;
    int flag;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(STAT, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    path = luab_checklstring(L, 2, luab_env_path_max, NULL);
    sb = luab_udata(L, 3, m1, struct stat *);
    flag = (int)luab_checkxinteger(L, 4, m0, luab_env_int_max);

    status = fstatat(fd, path, sb, flag);

    return (luab_pushxinteger(L, status));
}

/***
 * mkdirat(2) - make a directory file
 *
 * @function mkdirat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes newly created directory.
 *
 *                            #2 The newly created directory is relative to the
 *                               directory associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of mkdirat(2).
 *
 * @param path              Specifies the directory (path).
 * @param mode              Specifies access permissions of newly created
 *                          directory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.mkdirat(fd, path, mode)
 */
static int
luab_mkdirat(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd;
    const char *path;
    mode_t mode;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(MODE, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    path = luab_checklstring(L, 2, luab_env_path_max, NULL);
    mode = (mode_t)luab_checkxinteger(L, 3, m1, ALLPERMS);

    status = mkdirat(fd, path, mode);

    return (luab_pushxinteger(L, status));
}

/***
 * mkfifoat(2) - make a fifo file
 *
 * @function mkfifoat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes newly created FIFO.
 *
 *                            #2 The newly created FIFO is relative to the
 *                               directory associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of mkfifoat(2).
 *
 * @param path              Specifies the FIFO (with name path).
 * @param mode              Specifies access permissions of newly created
 *                          FIFO.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.mkfifoat(fd, path, mode)
 */
static int
luab_mkfifoat(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd;
    const char *path;
    mode_t mode;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(MODE, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    path = luab_checklstring(L, 2, luab_env_path_max, NULL);
    mode = (mode_t)luab_checkxinteger(L, 3, m1, ALLPERMS);

    status = mkfifoat(fd, path, mode);

    return (luab_pushxinteger(L, status));
}

/***
 * mknodat(2) - make a special file node
 *
 * @function mknodat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes newly created node.
 *
 *                            #2 The newly created node is relative to the
 *                               directory associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of mknodat(2).
 *
 * @param path              Specifies the system file node (path).
 * @param mode              Specifies access permissions of newly created
 *                          system file node.
 * @param dev               Specification, denotes particular device.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.stat.mknodat(fd, path, mode)
 */
static int
luab_mknodat(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    int fd;
    const char *path;
    mode_t mode;
    dev_t dev;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(MODE, TYPE, __func__);
    m2 = luab_xmod(DEV, TYPE, __func__);

    fd = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    path = luab_checklstring(L, 2, luab_env_path_max, NULL);
    mode = (mode_t)luab_checkxinteger(L, 3, m1, ALLPERMS);
    dev = (dev_t)luab_checkxinteger(L, 4, m2, luab_env_ulong_max);

    status = mknodat(fd, path, mode, dev);

    return (luab_pushxinteger(L, status));
}
#endif /* __POSIX_VISIBLE >= 200809 */

/*
 * Generator functions.
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(BLKCNT)).
 *
 * @function blkcnt_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(BLKCNT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage blkcnt [, err, msg ] = bsd.sys.stat.blkcnt_create(arg)
 */
static int
luab_blkcnt_create(lua_State *L)
{
    luab_module_t *m;
    blkcnt_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(BLKCNT, TYPE, __func__);
    x = (blkcnt_t)luab_checkxinteger(L, 1, m, luab_env_ulong_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(DEV)).
 *
 * @function dev_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(DEV)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage dev [, err, msg ] = bsd.sys.stat.dev_create(arg)
 */
static int
luab_dev_create(lua_State *L)
{
    luab_module_t *m;
    dev_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DEV, TYPE, __func__);
    x = (dev_t)luab_checkxinteger(L, 1, m, luab_env_ulong_max);

    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(FFLAGS)).
 *
 * @function fflags_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(FFLAGS)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage fflags [, err, msg ] = bsd.sys.stat.fflags_create(arg)
 */
static int
luab_fflags_create(lua_State *L)
{
    luab_module_t *m;
    fflags_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FFLAGS, TYPE, __func__);
    x = (fflags_t)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(INO)).
 *
 * @function ino_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(INO)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ino [, err, msg ] = bsd.sys.stat.ino_create(arg)
 */
static int
luab_ino_create(lua_State *L)
{
    luab_module_t *m;
    ino_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INO, TYPE, __func__);
    x = (ino_t)luab_checkxinteger(L, 1, m, luab_env_ulong_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(NLINK)).
 *
 * @function nlink_create
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(NLINK)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage nlink [, err, msg ] = bsd.sys.stat.nlink_create(arg)
 */
static int
luab_nlink_create(lua_State *L)
{
    luab_module_t *m;
    nlink_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(NLINK, TYPE, __func__);
    x = (nlink_t)luab_checkxinteger(L, 1, m, luab_env_ulong_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(STAT)).
 *
 * @function stat_create
 *
 * @param arg           Instance of (LUA_TUSERDATA(STAT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage hook [, err, msg ] = bsd.sys.stat.stat_create([ data ])
 */
static int
luab_stat_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/*
 * Interface against <sys/stat.h>.
 */

static luab_module_table_t luab_sys_stat_vec[] = {
    LUAB_INT("S_ISUID",             S_ISUID),
    LUAB_INT("S_ISGID",             S_ISGID),
#if __BSD_VISIBLE
    LUAB_INT("S_ISTXT",             S_ISTXT),
#endif
    LUAB_INT("S_IRWXU",             S_IRWXU),
    LUAB_INT("S_IRUSR",             S_IRUSR),
    LUAB_INT("S_IWUSR",             S_IWUSR),
    LUAB_INT("S_IXUSR",             S_IXUSR),
#if __BSD_VISIBLE
    LUAB_INT("S_IREAD",             S_IREAD),
    LUAB_INT("S_IWRITE",            S_IWRITE),
    LUAB_INT("S_IEXEC",             S_IEXEC),
#endif
    LUAB_INT("S_IRWXG",             S_IRWXG),
    LUAB_INT("S_IRGRP",             S_IRGRP),
    LUAB_INT("S_IWGRP",             S_IWGRP),
    LUAB_INT("S_IXGRP",             S_IXGRP),
    LUAB_INT("S_IRWXO",             S_IRWXO),
    LUAB_INT("S_IROTH",             S_IROTH),
    LUAB_INT("S_IWOTH",             S_IWOTH),
    LUAB_INT("S_IXOTH",             S_IXOTH),
#if __XSI_VISIBLE
    LUAB_INT("S_IFMT",              S_IFMT),
    LUAB_INT("S_IFIFO",             S_IFIFO),
    LUAB_INT("S_IFCHR",             S_IFCHR),
    LUAB_INT("S_IFDIR",             S_IFDIR),
    LUAB_INT("S_IFBLK",             S_IFBLK),
    LUAB_INT("S_IFREG",             S_IFREG),
    LUAB_INT("S_IFLNK",             S_IFLNK),
    LUAB_INT("S_IFSOCK",            S_IFSOCK),
    LUAB_INT("S_ISVTX",             S_ISVTX),
#endif
#if __BSD_VISIBLE
    LUAB_INT("S_IFWHT",             S_IFWHT),
    LUAB_INT("ACCESSPERMS",         ACCESSPERMS),
    LUAB_INT("ALLPERMS",            ALLPERMS),
    LUAB_INT("DEFFILEMODE",         DEFFILEMODE),
    LUAB_INT("UF_SETTABLE",         UF_SETTABLE),
    LUAB_INT("UF_NODUMP",           UF_NODUMP),
    LUAB_INT("UF_IMMUTABLE",        UF_IMMUTABLE),
    LUAB_INT("UF_APPEND",           UF_APPEND),
    LUAB_INT("UF_OPAQUE",           UF_OPAQUE),
    LUAB_INT("UF_NOUNLINK",         UF_NOUNLINK),
#if 0
    LUAB_INT("UF_COMPRESSED",       UF_COMPRESSED),
    LUAB_INT("UF_TRACKED",          UF_TRACKED),
#endif
    LUAB_INT("SF_SETTABLE",         SF_SETTABLE),
    LUAB_INT("SF_ARCHIVED",         SF_ARCHIVED),
    LUAB_INT("SF_IMMUTABLE",        SF_IMMUTABLE),
    LUAB_INT("SF_APPEND",           SF_APPEND),
    LUAB_INT("SF_NOUNLINK",         SF_NOUNLINK),
    LUAB_INT("SF_SNAPSHOT",         SF_SNAPSHOT),
#endif /* __BSD_VISIBLE */
#if __POSIX_VISIBLE >= 200809
    LUAB_INT("UTIME_NOW",           UTIME_NOW),
    LUAB_INT("UTIME_OMIT",          UTIME_OMIT),
#endif
#if __BSD_VISIBLE
    LUAB_FUNC("chflags",            luab_chflags),
    LUAB_FUNC("chflagsat",          luab_chflagsat),
#endif
    LUAB_FUNC("chmod",              luab_chmod),
#if __BSD_VISIBLE
    LUAB_FUNC("fchflags",           luab_fchflags),
#endif
#if __POSIX_VISIBLE >= 200112
    LUAB_FUNC("fchmod",             luab_fchmod),
#endif
#if __POSIX_VISIBLE >= 200809
    LUAB_FUNC("fstatat",            luab_fstatat),
    LUAB_FUNC("fchmodat",           luab_fchmodat),
    LUAB_FUNC("futimens",           luab_futimens),
    LUAB_FUNC("utimensat",          luab_utimensat),
#endif
    LUAB_FUNC("fstat",              luab_fstat),
#if __BSD_VISIBLE
    LUAB_FUNC("lchflags",           luab_lchflags),
    LUAB_FUNC("lchmod",             luab_lchmod),
#endif
#if __POSIX_VISIBLE >= 200112
    LUAB_FUNC("lstat",              luab_lstat),
#endif
    LUAB_FUNC("mkdir",              luab_mkdir),
    LUAB_FUNC("mkdirat",            luab_mkdirat),
    LUAB_FUNC("mkfifo",             luab_mkfifo),
    LUAB_FUNC("mkfifoat",           luab_mkfifoat),
#if !defined(_MKNOD_DECLARED) && __XSI_VISIBLE
    LUAB_FUNC("mknod",              luab_mknod),
#define _MKNOD_DECLARED
#endif
    LUAB_FUNC("stat",               luab_stat),
    LUAB_FUNC("umask",              luab_umask),
#if __XSI_VISIBLE >= 700
    LUAB_FUNC("mknodat",            luab_mknodat),
#endif
    LUAB_FUNC("blkcnt_create",      luab_blkcnt_create),
    LUAB_FUNC("dev_create",         luab_dev_create),
    LUAB_FUNC("fflags_create",      luab_fflags_create),
    LUAB_FUNC("ino_create",         luab_ino_create),
    LUAB_FUNC("nlink_create",       luab_nlink_create),
    LUAB_FUNC("stat_create",        luab_stat_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_stat_lib = {
    .m_id       = LUAB_SYS_STAT_LIB_ID,
    .m_name     = LUAB_SYS_STAT_LIB_KEY,
    .m_vec      = luab_sys_stat_vec,
};
