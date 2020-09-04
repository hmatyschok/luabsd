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

#include <sys/stat.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t timespec_type;
extern luab_module_t stat_type;

/*
 * Interface against
 *
 *  struct stat {
 *      dev_t     st_dev;
 *      ino_t     st_ino;
 *      nlink_t   st_nlink;
 *      mode_t    st_mode;
 *      __int16_t st_padding0;
 *      uid_t     st_uid;
 *      gid_t     st_gid;
 *      __int32_t st_padding1;
 *      dev_t     st_rdev;
 *  #ifdef  __STAT_TIME_T_EXT
 *       __int32_t st_atim_ext;
 *  #endif
 *      struct  timespec st_atim;
 *  #ifdef  __STAT_TIME_T_EXT
 *      __int32_t st_mtim_ext;
 *  #endif
 *      struct  timespec st_mtim;
 *  #ifdef  __STAT_TIME_T_EXT
 *      __int32_t st_ctim_ext;
 *  #endif
 *      struct  timespec st_ctim;
 *  #ifdef  __STAT_TIME_T_EXT
 *      __int32_t st_btim_ext;
 *  #endif
 *      struct  timespec st_birthtim;
 *      off_t     st_size;
 *      blkcnt_t st_blocks;
 *      blksize_t st_blksize;
 *      fflags_t  st_flags;
 *      __uint64_t st_gen;
 *      __uint64_t st_spare[10];
 *  };
 */

typedef struct luab_stat {
    luab_udata_t    ud_softc;
    struct stat    stat;
} luab_stat_t;

#define luab_new_stat(L, arg) \
    ((luab_stat_t *)luab_newuserdata(L, &stat_type, (arg)))
#define luab_to_stat(L, narg) \
    (luab_toldata((L), (narg), &stat_type, \
        struct stat *, sizeof(struct stat)))

#define LUABSD_STAT_TYPE_ID    1594028586
#define LUABSD_STAT_TYPE    "STAT*"

int luab_StructStat(lua_State *);

/* inode's device */
static int
Stat_set_st_dev(lua_State *L)
{
    struct stat *st;
    dev_t st_dev;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_dev = (dev_t)luab_checkinteger(L, 2, LONG_MAX);

    st->st_dev = st_dev;

    return (luab_pusherr(L, st_dev));
}

static int
Stat_get_st_dev(lua_State *L)
{
    struct stat *st;
    dev_t st_dev;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_dev = st->st_dev;

    return (luab_pusherr(L, st_dev));
}

/* inode's number */
static int
Stat_set_st_ino(lua_State *L)
{
    struct stat *st;
    ino_t st_ino;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_ino = (ino_t)luab_checkinteger(L, 2, LONG_MAX);

    st->st_ino = st_ino;

    return (luab_pusherr(L, st_ino));
}

static int
Stat_get_st_ino(lua_State *L)
{
    struct stat *st;
    ino_t st_ino;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_ino = st->st_ino;

    return (luab_pusherr(L, st_ino));
}

/* number of hard links */
static int
Stat_set_st_nlink(lua_State *L)
{
    struct stat *st;
    nlink_t st_nlink;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_nlink = (nlink_t)luab_checkinteger(L, 2, LONG_MAX);

    st->st_nlink = st_nlink;

    return (luab_pusherr(L, st_nlink));
}

static int
Stat_get_st_nlink(lua_State *L)
{
    struct stat *st;
    nlink_t st_nlink;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_nlink = st->st_nlink;

    return (luab_pusherr(L, st_nlink));
}

/* inode protection mode */
static int
Stat_set_st_mode(lua_State *L)
{
    struct stat *st;
    u_int st_mode;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_mode = luab_checkinteger(L, 2, USHRT_MAX);

    st->st_mode = (mode_t)st_mode;

    return (luab_pusherr(L, st_mode));
}

static int
Stat_get_st_mode(lua_State *L)
{
    struct stat *st;
    u_int st_mode;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_mode = (u_int)st->st_mode;

    return (luab_pusherr(L, st_mode));
}

/* user ID of the file's owner */
static int
Stat_set_st_uid(lua_State *L)
{
    struct stat *st;
    uid_t st_uid;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_uid = (uid_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_uid = st_uid;

    return (luab_pusherr(L, st_uid));
}

static int
Stat_get_st_uid(lua_State *L)
{
    struct stat *st;
    uid_t st_uid;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_uid = st->st_uid;

    return (luab_pusherr(L, st_uid));
}

/* user ID of the file's owner */
static int
Stat_set_st_gid(lua_State *L)
{
    struct stat *st;
    gid_t st_gid;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_gid = (gid_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_gid = st_gid;

    return (luab_pusherr(L, st_gid));
}

static int
Stat_get_st_gid(lua_State *L)
{
    struct stat *st;
    gid_t st_gid;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_gid = st->st_gid;

    return (luab_pusherr(L, st_gid));
}

/* device type */
static int
Stat_set_st_rdev(lua_State *L)
{
    struct stat *st;
    dev_t st_rdev;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_rdev = (dev_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_rdev = st_rdev;

    return (luab_pusherr(L, st_rdev));
}

static int
Stat_get_st_rdev(lua_State *L)
{
    struct stat *st;
    dev_t st_rdev;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_rdev = st->st_rdev;

    return (luab_pusherr(L, st_rdev));
}

/* time of last access */
#ifdef  __STAT_TIME_T_EXT
static int
Stat_set_st_atim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t st_atim_ext;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_atim_ext = (__int32_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_atim_ext = st_rdev;

    return (luab_pusherr(L, st_rdev));
}

static int
Stat_get_st_atim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t st_atim_ext;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_atim_ext = st->st_atim_ext;

    return (luab_pusherr(L, st_atim_ext));
}
#endif

static int
Stat_set_st_atim(lua_State *L)
{
    struct stat *st;
    struct timespec *tv;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    tv = luab_udata(L, 2, timespec_type, struct timespec *);

    (void)memmove(&st->st_atim, tv, sizeof(*tv));

    return (0);
}

static int
Stat_get_st_atim(lua_State *L)
{
    struct stat *st;
    int status;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);

    if ((*timespec_type.ctor)(L, &st->st_atim) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

/* time of last data modification */
#ifdef  __STAT_TIME_T_EXT
static int
Stat_set_st_mtim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t st_mtim_ext;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_mtim_ext = (__int32_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_mtim_ext = st_mtim_ext;

    return (luab_pusherr(L, st_mtim_ext));
}

static int
Stat_get_st_mtim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t st_mtim_ext;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_mtim_ext = st->st_mtim_ext;

    return (luab_pusherr(L, st_mtim_ext));
}
#endif

static int
Stat_set_st_mtim(lua_State *L)
{
    struct stat *st;
    struct timespec *tv;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    tv = luab_udata(L, 2, timespec_type, struct timespec *);

    (void)memmove(&st->st_mtim, tv, sizeof(*tv));

    return (0);
}

static int
Stat_get_st_mtim(lua_State *L)
{
    struct stat *st;
    int status;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);

    if ((*timespec_type.ctor)(L, &st->st_mtim) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

/* time of last file status change */
#ifdef  __STAT_TIME_T_EXT
static int
Stat_set_st_ctim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t st_ctim_ext;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_ctim_ext = (__int32_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_ctim_ext = st_ctim_ext;

    return (luab_pusherr(L, st_ctim_ext));
}

static int
Stat_get_st_ctim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t st_ctim_ext;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_ctim_ext = st->st_ctim_ext;

    return (luab_pusherr(L, st_ctim_ext));
}
#endif

static int
Stat_set_st_ctim(lua_State *L)
{
    struct stat *st;
    struct timespec *tv;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    tv = luab_udata(L, 2, timespec_type, struct timespec *);

    (void)memmove(&st->st_ctim, tv, sizeof(*tv));

    return (0);
}

static int
Stat_get_st_ctim(lua_State *L)
{
    struct stat *st;
    int status;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);

    if ((*timespec_type.ctor)(L, &st->st_ctim) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

/* time of file creation */
#ifdef  __STAT_TIME_T_EXT
static int
Stat_set_st_btim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t st_btim_ext;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_btim_ext = (__int32_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_btim_ext = st_btim_ext;

    return (luab_pusherr(L, st_btim_ext));
}

static int
Stat_get_st_btim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t st_btim_ext;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_btim_ext = st->st_btim_ext;

    return (luab_pusherr(L, st_btim_ext));
}
#endif

static int
Stat_set_st_birthtim(lua_State *L)
{
    struct stat *st;
    struct timespec *tv;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    tv = luab_udata(L, 2, timespec_type, struct timespec *);

    (void)memmove(&st->st_birthtim, tv, sizeof(*tv));

    return (0);
}

static int
Stat_get_st_birthtim(lua_State *L)
{
    struct stat *st;
    int status;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);

    if ((*timespec_type.ctor)(L, &st->st_birthtim) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

/* file size, in bytes */
static int
Stat_set_st_size(lua_State *L)
{
    struct stat *st;
    off_t st_size;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_size = (off_t)luab_checkinteger(L, 2, LONG_MAX);

    st->st_size = st_size;

    return (luab_pusherr(L, st_size));
}

static int
Stat_get_st_size(lua_State *L)
{
    struct stat *st;
    off_t st_size;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_size = st->st_size;

    return (luab_pusherr(L, st_size));
}

/* blocks allocated for file */
static int
Stat_set_st_blocks(lua_State *L)
{
    struct stat *st;
    blkcnt_t st_blocks;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_blocks = (blkcnt_t)luab_checkinteger(L, 2, LONG_MAX);

    st->st_blocks = st_blocks;

    return (luab_pusherr(L, st_blocks));
}

static int
Stat_get_st_blocks(lua_State *L)
{
    struct stat *st;
    blkcnt_t st_blocks;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_blocks = st->st_blocks;

    return (luab_pusherr(L, st_blocks));
}

/* optimal blocksize for I/O */
static int
Stat_set_st_blksize(lua_State *L)
{
    struct stat *st;
    blksize_t st_blksize;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_blksize = (blksize_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_blksize = st_blksize;

    return (luab_pusherr(L, st_blksize));
}

static int
Stat_get_st_blksize(lua_State *L)
{
    struct stat *st;
    blksize_t st_blksize;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_blksize = st->st_blksize;

    return (luab_pusherr(L, st_blksize));
}

/* user defined flags for file */
static int
Stat_set_st_flags(lua_State *L)
{
    struct stat *st;
    fflags_t st_flags;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_flags = (fflags_t)luab_checkinteger(L, 2, UINT_MAX);

    st->st_flags = st_flags;

    return (luab_pusherr(L, st_flags));
}

static int
Stat_get_st_flags(lua_State *L)
{
    struct stat *st;
    fflags_t st_flags;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_flags = st->st_flags;

    return (luab_pusherr(L, st_flags));
}

/* file generation number */
static int
Stat_set_st_gen(lua_State *L)
{
    struct stat *st;
    __uint64_t st_gen;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_gen = (__uint64_t)luab_checkinteger(L, 2, LONG_MAX);

    st->st_gen = st_gen;

    return (luab_pusherr(L, st_gen));
}

static int
Stat_get_st_gen(lua_State *L)
{
    struct stat *st;
    __uint64_t st_gen;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);
    st_gen = st->st_gen;

    return (luab_pusherr(L, st_gen));
}

/*
 * Maps attributes on stat{} to an instance of LUA_TTABLE.
 */
static int
Stat_get(lua_State *L)
{
    struct stat *st;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, stat_type, struct stat *);

    lua_newtable(L);

    luab_setinteger(L, -2, "st_dev", st->st_dev);
    luab_setinteger(L, -2, "st_ino", st->st_ino);
    luab_setinteger(L, -2, "st_nlink", st->st_nlink);
    luab_setinteger(L, -2, "st_mode", st->st_mode);
    luab_setinteger(L, -2, "st_uid", st->st_uid);
    luab_setinteger(L, -2, "st_gid", st->st_gid);
    luab_setinteger(L, -2, "st_rdev", st->st_rdev);

#ifdef  __STAT_TIME_T_EXT
    luab_setinteger(L, -2, "st_atim_ext", st->st_atim_ext);
#endif
    luab_setudata(L, -2, &timespec_type, "st_atim", &st->st_atim);

#ifdef  __STAT_TIME_T_EXT
    luab_setinteger(L, -2, "st_mtim_ext", st->st_mtim_ext);
#endif
    luab_setudata(L, -2, &timespec_type, "st_mtim", &st->st_mtim);

#ifdef  __STAT_TIME_T_EXT
    luab_setinteger(L, -2, "st_ctim_ext", st->st_ctim_ext);
#endif
    luab_setudata(L, -2, &timespec_type, "st_ctim", &st->st_ctim);

#ifdef  __STAT_TIME_T_EXT
    luab_setinteger(L, -2, "st_btim_ext", st->st_btim_ext);
#endif
    luab_setudata(L, -2, &timespec_type, "st_birthtim", &st->st_birthtim);

    luab_setinteger(L, -2, "st_size", st->st_size);
    luab_setinteger(L, -2, "st_blocks", st->st_blocks);
    luab_setinteger(L, -2, "st_blksize", st->st_blksize);
    luab_setinteger(L, -2, "st_flags", st->st_flags);
    luab_setinteger(L, -2, "st_gen", st->st_gen);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Copy stat{} into LUA_TUSERDATA(luab_iovec_t).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = stat:dump()
 */
static int
Stat_dump(lua_State *L)
{
    luab_iovec_param_t iop;
    size_t len, max_len;
    caddr_t data;
    int status;

    (void)luab_checkmaxargs(L, 1);

    (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

    data = luab_udata(L, 1, stat_type, caddr_t);

    len = sizeof(struct stat);
    max_len = len + sizeof(uint32_t);

    iop.iop_buf.buf_len = max_len;
    iop.iop_data.buf_data = data;
    iop.iop_data.buf_len = len;

    if ((*iovec_type.ctor)(L, &iop) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

static int
Stat_gc(lua_State *L)
{
    return (luab_gc(L, 1, &stat_type));
}

static int
Stat_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &stat_type));
}

static luab_table_t stat_methods[] = {
    LUABSD_FUNC("set_st_dev",   Stat_set_st_dev),
    LUABSD_FUNC("set_st_ino",   Stat_set_st_ino),
    LUABSD_FUNC("set_st_nlink", Stat_set_st_nlink),
    LUABSD_FUNC("set_st_mode",  Stat_set_st_mode),
    LUABSD_FUNC("set_st_uid",   Stat_set_st_uid),
    LUABSD_FUNC("set_st_gid",   Stat_set_st_gid),
    LUABSD_FUNC("set_st_rdev",  Stat_set_st_rdev),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("set_st_atim_ext",  Stat_set_st_atim_ext),
#endif
    LUABSD_FUNC("set_st_atim",  Stat_set_st_atim),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("set_st_mtim_ext",  Stat_set_st_mtim_ext),
#endif
    LUABSD_FUNC("set_st_mtim",  Stat_set_st_mtim),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("set_st_ctim_ext",  Stat_set_st_ctim_ext),
#endif
    LUABSD_FUNC("set_st_ctim",  Stat_set_st_ctim),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("set_st_btim_ext",  Stat_set_st_btim_ext),
#endif
    LUABSD_FUNC("set_st_birthtim",  Stat_set_st_birthtim),
    LUABSD_FUNC("set_st_size",  Stat_set_st_size),
    LUABSD_FUNC("set_st_blocks",    Stat_set_st_blocks),
    LUABSD_FUNC("set_st_blocks",    Stat_set_st_blksize),
    LUABSD_FUNC("set_st_flags", Stat_set_st_flags),
    LUABSD_FUNC("set_st_gen",   Stat_set_st_gen),
    LUABSD_FUNC("get",  Stat_get),
    LUABSD_FUNC("get_st_dev",   Stat_get_st_dev),
    LUABSD_FUNC("get_st_ino",   Stat_get_st_ino),
    LUABSD_FUNC("get_st_nlink", Stat_get_st_nlink),
    LUABSD_FUNC("get_st_mode",  Stat_get_st_mode),
    LUABSD_FUNC("get_st_uid",   Stat_get_st_uid),
    LUABSD_FUNC("get_st_gid",   Stat_get_st_gid),
    LUABSD_FUNC("get_st_rdev",  Stat_get_st_rdev),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("get_st_atim_ext",  Stat_get_st_atim_ext),
#endif
    LUABSD_FUNC("get_st_atim",  Stat_get_st_atim),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("get_st_mtim_ext",  Stat_get_st_mtim_ext),
#endif
    LUABSD_FUNC("get_st_mtim",  Stat_get_st_mtim),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("get_st_ctim_ext",  Stat_get_st_ctim_ext),
#endif
    LUABSD_FUNC("get_st_ctim",  Stat_get_st_ctim),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("get_st_btim_ext",  Stat_get_st_btim_ext),
#endif
    LUABSD_FUNC("get_st_birthtim",  Stat_get_st_birthtim),
    LUABSD_FUNC("get_st_size",  Stat_get_st_size),
    LUABSD_FUNC("get_st_blocks",    Stat_get_st_blocks),
    LUABSD_FUNC("get_st_blocks",    Stat_get_st_blksize),
    LUABSD_FUNC("get_st_flags", Stat_get_st_flags),
    LUABSD_FUNC("get_st_gen",   Stat_get_st_gen),
    LUABSD_FUNC("dump",  Stat_dump),
    LUABSD_FUNC("__gc", Stat_gc),
    LUABSD_FUNC("__tostring",   Stat_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
stat_create(lua_State *L, void *arg)
{
    return (luab_new_stat(L, arg));
}

static void
stat_init(void *ud, void *arg)
{
    luab_stat_t *self;

    if (((self = (luab_stat_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->stat, arg, sizeof(self->stat));
}

static void *
stat_udata(lua_State *L, int narg)
{
    return (luab_to_stat(L, narg));
}

luab_module_t stat_type = {
    .cookie = LUABSD_STAT_TYPE_ID,
    .name = LUABSD_STAT_TYPE,
    .vec = stat_methods,
    .ctor = stat_create,
    .init = stat_init,
    .get = stat_udata,
    .sz = sizeof(luab_stat_t),
};

int
luab_StructStat(lua_State *L)
{
    struct stat *stat;
    int narg, status;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        stat = NULL;
    else
        stat = stat_udata(L, narg);

    if (stat_create(L, stat) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
