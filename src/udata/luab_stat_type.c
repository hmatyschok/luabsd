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
#include "luab_types.h"

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

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(STAT)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              st_dev          = (LUA_TNUMBER),
 *              st_ino          = (LUA_TNUMBER),
 *              st_nlink        = (LUA_TNUMBER),
 *              st_mode         = (LUA_TNUMBER),
 *              st_uid          = (LUA_TNUMBER),
 *              st_gid          = (LUA_TNUMBER),
 *              st_rdev         = (LUA_TNUMBER),
 *              st_atim_ext     = (LUA_TNUMBER),
 *              st_atim         = (LUA_TUSERDATA(STAT)),
 *              st_mtim_ext     = (LUA_TNUMBER),
 *              st_mtim         = (LUA_TUSERDATA(STAT)),
 *              st_ctim_ext     = (LUA_TNUMBER),
 *              st_ctim         = (LUA_TUSERDATA(STAT)),
 *              st_btim_ext     = (LUA_TNUMBER),
 *              st_birthtim     = (LUA_TUSERDATA(STAT)),
 *              st_size         = (LUA_TNUMBER),
 *              st_blocks       = (LUA_TNUMBER),
 *              st_blksize      = (LUA_TNUMBER),
 *              st_flags        = (LUA_TNUMBER),
 *              st_gen          = (LUA_TNUMBER),
 *          }
 *
 * @usage t = stat:get()
 */
static int
STAT_get(lua_State *L)
{
    struct stat *st;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);

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
    luab_setudata(L, -2, luab_mx(TIMESPEC), "st_atim", &st->st_atim);

#ifdef  __STAT_TIME_T_EXT
    luab_setinteger(L, -2, "st_mtim_ext", st->st_mtim_ext);
#endif
    luab_setudata(L, -2, luab_mx(TIMESPEC), "st_mtim", &st->st_mtim);

#ifdef  __STAT_TIME_T_EXT
    luab_setinteger(L, -2, "st_ctim_ext", st->st_ctim_ext);
#endif
    luab_setudata(L, -2, luab_mx(TIMESPEC), "st_ctim", &st->st_ctim);

#ifdef  __STAT_TIME_T_EXT
    luab_setinteger(L, -2, "st_btim_ext", st->st_btim_ext);
#endif
    luab_setudata(L, -2, luab_mx(TIMESPEC), "st_birthtim", &st->st_birthtim);

    luab_setinteger(L, -2, "st_size", st->st_size);
    luab_setinteger(L, -2, "st_blocks", st->st_blocks);
    luab_setinteger(L, -2, "st_blksize", st->st_blksize);
    luab_setinteger(L, -2, "st_flags", st->st_flags);
    luab_setinteger(L, -2, "st_gen", st->st_gen);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate stat{} into (LUA_TUSERDATA(IOVEC)).
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
STAT_dump(lua_State *L)
{
    return (luab_dump(L, 1, &stat_type, sizeof(struct stat)));
}

/*
 * Accessor.
 */

/* inode's device */
static int
STAT_set_st_dev(lua_State *L)
{
    struct stat *st;
    dev_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (dev_t)luab_checkinteger(L, 2, LONG_MAX);

    st->st_dev = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_dev(lua_State *L)
{
    struct stat *st;
    dev_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_dev;

    return (luab_pusherr(L, data));
}

/* inode's number */
static int
STAT_set_st_ino(lua_State *L)
{
    struct stat *st;
    ino_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (ino_t)luab_checkinteger(L, 2, LONG_MAX);

    st->st_ino = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_ino(lua_State *L)
{
    struct stat *st;
    ino_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_ino;

    return (luab_pusherr(L, data));
}

/* number of hard links */
static int
STAT_set_st_nlink(lua_State *L)
{
    struct stat *st;
    nlink_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (nlink_t)luab_checkinteger(L, 2, LONG_MAX);

    st->st_nlink = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_nlink(lua_State *L)
{
    struct stat *st;
    nlink_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_nlink;

    return (luab_pusherr(L, data));
}

/* inode protection mode */
static int
STAT_set_st_mode(lua_State *L)
{
    struct stat *st;
    mode_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (mode_t)luab_checkinteger(L, 2, USHRT_MAX);

    st->st_mode = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_mode(lua_State *L)
{
    struct stat *st;
    mode_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_mode;

    return (luab_pusherr(L, data));
}

/* user ID of the file's owner */
static int
STAT_set_st_uid(lua_State *L)
{
    struct stat *st;
    uid_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (uid_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_uid = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_uid(lua_State *L)
{
    struct stat *st;
    uid_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_uid;

    return (luab_pusherr(L, data));
}

/* user ID of the file's owner */
static int
STAT_set_st_gid(lua_State *L)
{
    struct stat *st;
    gid_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (gid_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_gid = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_gid(lua_State *L)
{
    struct stat *st;
    gid_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_gid;

    return (luab_pusherr(L, data));
}

/* device type */
static int
STAT_set_st_rdev(lua_State *L)
{
    struct stat *st;
    dev_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (dev_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_rdev = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_rdev(lua_State *L)
{
    struct stat *st;
    dev_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_rdev;

    return (luab_pusherr(L, data));
}

/* time of last access */
#ifdef  __STAT_TIME_T_EXT
static int
STAT_set_st_atim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (__int32_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_atim_ext = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_atim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_atim_ext;

    return (luab_pusherr(L, data));
}
#endif

static int
STAT_set_st_atim(lua_State *L)
{
    struct stat *st;
    struct timespec *tv;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    tv = luab_udata(L, 2, luab_mx(TIMESPEC), struct timespec *);

    (void)memmove(&st->st_atim, tv, sizeof(*tv));

    return (0);
}

static int
STAT_get_st_atim(lua_State *L)
{
    struct stat *st;
    int status;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);

    if ((*(luab_mx(TIMESPEC))->create)(L, &st->st_atim) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

/* time of last data modification */
#ifdef  __STAT_TIME_T_EXT
static int
STAT_set_st_mtim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (__int32_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_mtim_ext = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_mtim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_mtim_ext;

    return (luab_pusherr(L, data));
}
#endif

static int
STAT_set_st_mtim(lua_State *L)
{
    struct stat *st;
    struct timespec *tv;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    tv = luab_udata(L, 2, luab_mx(TIMESPEC), struct timespec *);

    (void)memmove(&st->st_mtim, tv, sizeof(*tv));

    return (0);
}

static int
STAT_get_st_mtim(lua_State *L)
{
    struct stat *st;
    int status;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);

    if ((*(luab_mx(TIMESPEC))->create)(L, &st->st_mtim) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

/* time of last file status change */
#ifdef  __STAT_TIME_T_EXT
static int
STAT_set_st_ctim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (__int32_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_ctim_ext = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_ctim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_ctim_ext;

    return (luab_pusherr(L, data));
}
#endif

static int
STAT_set_st_ctim(lua_State *L)
{
    struct stat *st;
    struct timespec *tv;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    tv = luab_udata(L, 2, luab_mx(TIMESPEC), struct timespec *);

    (void)memmove(&st->st_ctim, tv, sizeof(*tv));

    return (0);
}

static int
STAT_get_st_ctim(lua_State *L)
{
    struct stat *st;
    int status;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);

    if ((*(luab_mx(TIMESPEC))->create)(L, &st->st_ctim) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

/* time of file creation */
#ifdef  __STAT_TIME_T_EXT
static int
STAT_set_st_btim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (__int32_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_btim_ext = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_btim_ext(lua_State *L)
{
    struct stat *st;
    __int32_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_btim_ext;

    return (luab_pusherr(L, data));
}
#endif

static int
STAT_set_st_birthtim(lua_State *L)
{
    struct stat *st;
    struct timespec *tv;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    tv = luab_udata(L, 2, luab_mx(TIMESPEC), struct timespec *);

    (void)memmove(&st->st_birthtim, tv, sizeof(*tv));

    return (0);
}

static int
STAT_get_st_birthtim(lua_State *L)
{
    struct stat *st;
    int status;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);

    if ((*(luab_mx(TIMESPEC))->create)(L, &st->st_birthtim) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

/* file size, in bytes */
static int
STAT_set_st_size(lua_State *L)
{
    struct stat *st;
    off_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (off_t)luab_checkinteger(L, 2, LONG_MAX);

    st->st_size = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_size(lua_State *L)
{
    struct stat *st;
    off_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_size;

    return (luab_pusherr(L, data));
}

/* blocks allocated for file */
static int
STAT_set_st_blocks(lua_State *L)
{
    struct stat *st;
    blkcnt_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (blkcnt_t)luab_checkinteger(L, 2, LONG_MAX);

    st->st_blocks = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_blocks(lua_State *L)
{
    struct stat *st;
    blkcnt_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_blocks;

    return (luab_pusherr(L, data));
}

/* optimal blocksize for I/O */
static int
STAT_set_st_blksize(lua_State *L)
{
    struct stat *st;
    blksize_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (blksize_t)luab_checkinteger(L, 2, INT_MAX);

    st->st_blksize = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_blksize(lua_State *L)
{
    struct stat *st;
    blksize_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_blksize;

    return (luab_pusherr(L, data));
}

/* user defined flags for file */
static int
STAT_set_st_flags(lua_State *L)
{
    struct stat *st;
    fflags_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (fflags_t)luab_checkinteger(L, 2, UINT_MAX);

    st->st_flags = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_flags(lua_State *L)
{
    struct stat *st;
    fflags_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_flags;

    return (luab_pusherr(L, data));
}

/* file generation number */
static int
STAT_set_st_gen(lua_State *L)
{
    struct stat *st;
    __uint64_t data;

    (void)luab_checkmaxargs(L, 2);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = (__uint64_t)luab_checkinteger(L, 2, LONG_MAX);

    st->st_gen = data;

    return (luab_pusherr(L, data));
}

static int
STAT_get_st_gen(lua_State *L)
{
    struct stat *st;
    __uint64_t data;

    (void)luab_checkmaxargs(L, 1);

    st = luab_udata(L, 1, &stat_type, struct stat *);
    data = st->st_gen;

    return (luab_pusherr(L, data));
}

/*
 * Meta-methods.
 */

static int
STAT_gc(lua_State *L)
{
    return (luab_gc(L, 1, &stat_type));
}

static int
STAT_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &stat_type));
}

/*
 * Internal interface.
 */

static luab_table_t stat_methods[] = {
    LUABSD_FUNC("set_st_dev",   STAT_set_st_dev),
    LUABSD_FUNC("set_st_ino",   STAT_set_st_ino),
    LUABSD_FUNC("set_st_nlink", STAT_set_st_nlink),
    LUABSD_FUNC("set_st_mode",  STAT_set_st_mode),
    LUABSD_FUNC("set_st_uid",   STAT_set_st_uid),
    LUABSD_FUNC("set_st_gid",   STAT_set_st_gid),
    LUABSD_FUNC("set_st_rdev",  STAT_set_st_rdev),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("set_st_atim_ext",  STAT_set_st_atim_ext),
#endif
    LUABSD_FUNC("set_st_atim",  STAT_set_st_atim),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("set_st_mtim_ext",  STAT_set_st_mtim_ext),
#endif
    LUABSD_FUNC("set_st_mtim",  STAT_set_st_mtim),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("set_st_ctim_ext",  STAT_set_st_ctim_ext),
#endif
    LUABSD_FUNC("set_st_ctim",  STAT_set_st_ctim),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("set_st_btim_ext",  STAT_set_st_btim_ext),
#endif
    LUABSD_FUNC("set_st_birthtim",  STAT_set_st_birthtim),
    LUABSD_FUNC("set_st_size",  STAT_set_st_size),
    LUABSD_FUNC("set_st_blocks",    STAT_set_st_blocks),
    LUABSD_FUNC("set_st_blocks",    STAT_set_st_blksize),
    LUABSD_FUNC("set_st_flags", STAT_set_st_flags),
    LUABSD_FUNC("set_st_gen",   STAT_set_st_gen),
    LUABSD_FUNC("get",  STAT_get),
    LUABSD_FUNC("get_st_dev",   STAT_get_st_dev),
    LUABSD_FUNC("get_st_ino",   STAT_get_st_ino),
    LUABSD_FUNC("get_st_nlink", STAT_get_st_nlink),
    LUABSD_FUNC("get_st_mode",  STAT_get_st_mode),
    LUABSD_FUNC("get_st_uid",   STAT_get_st_uid),
    LUABSD_FUNC("get_st_gid",   STAT_get_st_gid),
    LUABSD_FUNC("get_st_rdev",  STAT_get_st_rdev),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("get_st_atim_ext",  STAT_get_st_atim_ext),
#endif
    LUABSD_FUNC("get_st_atim",  STAT_get_st_atim),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("get_st_mtim_ext",  STAT_get_st_mtim_ext),
#endif
    LUABSD_FUNC("get_st_mtim",  STAT_get_st_mtim),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("get_st_ctim_ext",  STAT_get_st_ctim_ext),
#endif
    LUABSD_FUNC("get_st_ctim",  STAT_get_st_ctim),
#ifdef  __STAT_TIME_T_EXT
    LUABSD_FUNC("get_st_btim_ext",  STAT_get_st_btim_ext),
#endif
    LUABSD_FUNC("get_st_birthtim",  STAT_get_st_birthtim),
    LUABSD_FUNC("get_st_size",  STAT_get_st_size),
    LUABSD_FUNC("get_st_blocks",    STAT_get_st_blocks),
    LUABSD_FUNC("get_st_blocks",    STAT_get_st_blksize),
    LUABSD_FUNC("get_st_flags", STAT_get_st_flags),
    LUABSD_FUNC("get_st_gen",   STAT_get_st_gen),
    LUABSD_FUNC("dump",  STAT_dump),
    LUABSD_FUNC("__gc", STAT_gc),
    LUABSD_FUNC("__tostring",   STAT_tostring),
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
    .create = stat_create,
    .init = stat_init,
    .get = stat_udata,
    .sz = sizeof(luab_stat_t),
};
