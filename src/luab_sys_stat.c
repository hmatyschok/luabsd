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
#include <sys/param.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

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

#define LUABSD_STAT_TYPE_ID    1594028586
#define LUABSD_STAT_TYPE    "STAT*"

typedef struct {
    struct stat    stat;
} luab_stat_t;

#define luab_newstat(L, arg) \
    ((luab_stat_t *)luab_newuserdata(L, &stat_type, (arg)))
#define luab_tostat(L, narg) \
    (luab_todata((L), (narg), &stat_type, luab_stat_t *))

/* inode's device */
static int
Stat_set_st_dev(lua_State *L)
{
    luab_stat_t *self;
    dev_t st_dev;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_dev = luab_checkinteger(L, 2, LONG_MAX);

    self->stat.st_dev = st_dev;

    return 0;
}

static int
Stat_get_st_dev(lua_State *L)
{
    luab_stat_t *self;
    dev_t st_dev;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_dev = self->stat.st_dev;

    lua_pushinteger(L, st_dev);

    return 1;
}

/* inode's number */
static int
Stat_set_st_ino(lua_State *L)
{
    luab_stat_t *self;
    ino_t st_ino;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_ino = luab_checkinteger(L, 2, LONG_MAX);

    self->stat.st_ino = st_ino;

    return 0;
}

static int
Stat_get_st_ino(lua_State *L)
{
    luab_stat_t *self;
    ino_t st_ino;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_ino = self->stat.st_ino;

    lua_pushinteger(L, st_ino);

    return 1;
}

/* number of hard links */
static int
Stat_set_st_nlink(lua_State *L)
{
    luab_stat_t *self;
    nlink_t st_nlink;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_nlink = luab_checkinteger(L, 2, LONG_MAX);

    self->stat.st_nlink = st_nlink;

    return 0;
}

static int
Stat_get_st_nlink(lua_State *L)
{
    luab_stat_t *self;
    nlink_t st_nlink;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_nlink = self->stat.st_nlink;

    lua_pushinteger(L, st_nlink);

    return 1;
}

/* inode protection mode */
static int
Stat_set_st_mode(lua_State *L)
{
    luab_stat_t *self;
    u_int st_mode;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_mode = luab_checkinteger(L, 2, USHRT_MAX);

    self->stat.st_mode = (mode_t)st_mode;

    return 0;
}

static int
Stat_get_st_mode(lua_State *L)
{
    luab_stat_t *self;
    u_int st_mode;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_mode = self->stat.st_mode;

    lua_pushinteger(L, st_mode);

    return 1;
}

/* user ID of the file's owner */
static int
Stat_set_st_uid(lua_State *L)
{
    luab_stat_t *self;
    uid_t st_uid;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_uid = luab_checkinteger(L, 2, INT_MAX);

    self->stat.st_uid = st_uid;

    return 0;
}

static int
Stat_get_st_uid(lua_State *L)
{
    luab_stat_t *self;
    uid_t st_uid;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_uid = self->stat.st_uid;

    lua_pushinteger(L, st_uid);

    return 1;
}

/* user ID of the file's owner */
static int
Stat_set_st_gid(lua_State *L)
{
    luab_stat_t *self;
    gid_t st_gid;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_gid = luab_checkinteger(L, 2, INT_MAX);

    self->stat.st_gid = st_gid;

    return 0;
}

static int
Stat_get_st_gid(lua_State *L)
{
    luab_stat_t *self;
    gid_t st_gid;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_gid = self->stat.st_gid;

    lua_pushinteger(L, st_gid);

    return 1;
}

/* device type */
static int
Stat_set_st_rdev(lua_State *L)
{
    luab_stat_t *self;
    dev_t st_rdev;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_rdev = luab_checkinteger(L, 2, INT_MAX);

    self->stat.st_rdev = st_rdev;

    return 0;
}

static int
Stat_get_st_rdev(lua_State *L)
{
    luab_stat_t *self;
    dev_t st_rdev;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_rdev = self->stat.st_rdev;

    lua_pushinteger(L, st_rdev);

    return 1;
}

/* time of last access */
#ifdef  __STAT_TIME_T_EXT
static int
Stat_set_st_atim_ext(lua_State *L)
{
    luab_stat_t *self;
    __int32_t st_atim_ext;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_atim_ext = luab_checkinteger(L, 2, INT_MAX);

    self->stat.st_atim_ext = st_atim_ext;

    return 0;
}

static int
Stat_get_st_atim_ext(lua_State *L)
{
    luab_stat_t *self;
    __int32_t st_atim_ext;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_atim_ext = self->stat.st_atim_ext;

    lua_pushinteger(L, st_atim_ext);

    return 1;
}
#endif

static int
Stat_set_st_atim(lua_State *L)
{
    luab_stat_t *self;
    void *ud;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->stat.st_atim, ud, timespec_type.sz);

    return 0;
}

static int
Stat_get_st_atim(lua_State *L)
{
    luab_stat_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);

    (void)luab_newuserdata(L, &timespec_type, &self->stat.st_atim);

    return 1;
}

/* time of last data modification */
#ifdef  __STAT_TIME_T_EXT
static int
Stat_set_st_mtim_ext(lua_State *L)
{
    luab_stat_t *self;
    __int32_t st_mtim_ext;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_mtim_ext = luab_checkinteger(L, 2, INT_MAX);

    self->stat.st_mtim_ext = st_mtim_ext;

    return 0;
}

static int
Stat_get_st_mtim_ext(lua_State *L)
{
    luab_stat_t *self;
    __int32_t st_mtim_ext;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_mtim_ext = self->stat.st_mtim_ext;

    lua_pushinteger(L, st_mtim_ext);

    return 1;
}
#endif

static int
Stat_set_st_mtim(lua_State *L)
{
    luab_stat_t *self;
    void *ud;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->stat.st_mtim, ud, timespec_type.sz);

    return 0;
}

static int
Stat_get_st_mtim(lua_State *L)
{
    luab_stat_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);

    (void)luab_newuserdata(L, &timespec_type, &self->stat.st_mtim);

    return 1;
}

/* time of last file status change */
#ifdef  __STAT_TIME_T_EXT
static int
Stat_set_st_ctim_ext(lua_State *L)
{
    luab_stat_t *self;
    __int32_t st_ctim_ext;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_ctim_ext = luab_checkinteger(L, 2, INT_MAX);

    self->stat.st_ctim_ext = st_ctim_ext;

    return 0;
}

static int
Stat_get_st_ctim_ext(lua_State *L)
{
    luab_stat_t *self;
    __int32_t st_ctim_ext;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_ctim_ext = self->stat.st_ctim_ext;

    lua_pushinteger(L, st_ctim_ext);

    return 1;
}
#endif

static int
Stat_set_st_ctim(lua_State *L)
{
    luab_stat_t *self;
    void *ud;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->stat.st_ctim, ud, timespec_type.sz);

    return 0;
}

static int
Stat_get_st_ctim(lua_State *L)
{
    luab_stat_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);

    (void)luab_newuserdata(L, &timespec_type, &self->stat.st_ctim);

    return 1;
}

/* time of file creation */
#ifdef  __STAT_TIME_T_EXT
static int
Stat_set_st_btim_ext(lua_State *L)
{
    luab_stat_t *self;
    __int32_t st_btim_ext;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_btim_ext = luab_checkinteger(L, 2, INT_MAX);

    self->stat.st_btim_ext = st_btim_ext;

    return 0;
}

static int
Stat_get_st_btim_ext(lua_State *L)
{
    luab_stat_t *self;
    __int32_t st_btim_ext;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_btim_ext = self->stat.st_btim_ext;

    lua_pushinteger(L, st_btim_ext);

    return 1;
}
#endif

static int
Stat_set_st_birthtim(lua_State *L)
{
    luab_stat_t *self;
    void *ud;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->stat.st_birthtim, ud, timespec_type.sz);

    return 0;
}

static int
Stat_get_st_birthtim(lua_State *L)
{
    luab_stat_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);

    (void)luab_newuserdata(L, &timespec_type, &self->stat.st_birthtim);

    return 1;
}

/* file size, in bytes */
static int
Stat_set_st_size(lua_State *L)
{
    luab_stat_t *self;
    off_t st_size;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_size = luab_checkinteger(L, 2, LONG_MAX);

    self->stat.st_size = st_size;

    return 0;
}

static int
Stat_get_st_size(lua_State *L)
{
    luab_stat_t *self;
    off_t st_size;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_size = self->stat.st_size;

    lua_pushinteger(L, st_size);

    return 1;
}

/* blocks allocated for file */
static int
Stat_set_st_blocks(lua_State *L)
{
    luab_stat_t *self;
    blkcnt_t st_blocks;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_blocks = luab_checkinteger(L, 2, LONG_MAX);

    self->stat.st_blocks = st_blocks;

    return 0;
}

static int
Stat_get_st_blocks(lua_State *L)
{
    luab_stat_t *self;
    blkcnt_t st_blocks;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_blocks = self->stat.st_blocks;

    lua_pushinteger(L, st_blocks);

    return 1;
}

/* optimal blocksize for I/O */
static int
Stat_set_st_blksize(lua_State *L)
{
    luab_stat_t *self;
    blksize_t st_blksize;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_blksize = luab_checkinteger(L, 2, INT_MAX);

    self->stat.st_blksize = st_blksize;

    return 0;
}

static int
Stat_get_st_blksize(lua_State *L)
{
    luab_stat_t *self;
    blksize_t st_blksize;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_blksize = self->stat.st_blksize;

    lua_pushinteger(L, st_blksize);

    return 1;
}

/* user defined flags for file */
static int
Stat_set_st_flags(lua_State *L)
{
    luab_stat_t *self;
    fflags_t st_flags;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_flags = luab_checkinteger(L, 2, UINT_MAX);

    self->stat.st_flags = st_flags;

    return 0;
}

static int
Stat_get_st_flags(lua_State *L)
{
    luab_stat_t *self;
    fflags_t st_flags;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_flags = self->stat.st_flags;

    lua_pushinteger(L, st_flags);

    return 1;
}

/* file generation number */
static int
Stat_set_st_gen(lua_State *L)
{
    luab_stat_t *self;
    __uint64_t st_gen;

    luab_checkmaxargs(L, 2);

    self = luab_tostat(L, 1);
    st_gen = luab_checkinteger(L, 2, LONG_MAX);

    self->stat.st_gen = st_gen;

    return 0;
}

static int
Stat_get_st_gen(lua_State *L)
{
    luab_stat_t *self;
    __uint64_t st_gen;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);
    st_gen = self->stat.st_gen;

    lua_pushinteger(L, st_gen);

    return 1;
}

/*
 * Maps attributes on stat{} to an instance of LUA_TTABLE.
 */
static int
Stat_get(lua_State *L)
{
    luab_stat_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_tostat(L, 1);

    lua_newtable(L);

    luab_setinteger(L, -2, "st_dev", self->stat.st_dev);
    luab_setinteger(L, -2, "st_ino", self->stat.st_ino);
    luab_setinteger(L, -2, "st_nlink", self->stat.st_nlink);
    luab_setinteger(L, -2, "st_mode", self->stat.st_mode);
    luab_setinteger(L, -2, "st_uid", self->stat.st_uid);
    luab_setinteger(L, -2, "st_gid", self->stat.st_gid);
    luab_setinteger(L, -2, "st_rdev", self->stat.st_rdev);

#ifdef  __STAT_TIME_T_EXT
    luab_setinteger(L, -2, "st_atim_ext", self->stat.st_atim_ext);
#endif
    luab_setudata(L, -2, &timespec_type, "st_atim", &self->stat.st_atim);

#ifdef  __STAT_TIME_T_EXT
    luab_setinteger(L, -2, "st_mtim_ext", self->stat.st_mtim_ext);
#endif
    luab_setudata(L, -2, &timespec_type, "st_mtim", &self->stat.st_mtim);

#ifdef  __STAT_TIME_T_EXT
    luab_setinteger(L, -2, "st_ctim_ext", self->stat.st_ctim_ext);
#endif
    luab_setudata(L, -2, &timespec_type, "st_ctim", &self->stat.st_ctim);

#ifdef  __STAT_TIME_T_EXT
    luab_setinteger(L, -2, "st_btim_ext", self->stat.st_btim_ext);
#endif
    luab_setudata(L, -2, &timespec_type, "st_birthtim", &self->stat.st_birthtim);

    luab_setinteger(L, -2, "st_size", self->stat.st_size);
    luab_setinteger(L, -2, "st_blocks", self->stat.st_blocks);
    luab_setinteger(L, -2, "st_blksize", self->stat.st_blksize);
    luab_setinteger(L, -2, "st_flags", self->stat.st_flags);
    luab_setinteger(L, -2, "st_gen", self->stat.st_gen);

    lua_pushvalue(L, -1);

    return 1;
}

static int
Stat_tostring(lua_State *L)
{
    luab_stat_t *self = luab_tostat(L, 1);
    lua_pushfstring(L, "stat (%p)", self);

    return 1;
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
    LUABSD_FUNC("__tostring",   Stat_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void
stat_init(void *ud, void *arg)
{                                           /* XXX */
    luab_stat_t *self = (luab_stat_t *)ud;

    (void)memmove(&self->stat, arg, sizeof(self->stat));
}

static void *
stat_udata(lua_State *L, int narg)
{
    luab_stat_t *self = luab_tostat(L, narg);

    return (&self->stat);
}

luab_module_t stat_type = {
    .cookie = LUABSD_STAT_TYPE_ID,
    .name = LUABSD_STAT_TYPE,
    .vec = stat_methods,
    .init = stat_init,
    .get = stat_udata,
    .sz = sizeof(luab_stat_t),
};

static int
luab_StructStat(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 1);
    struct stat *stat;

    if (narg == 0)
        stat = NULL;
    else
        stat = stat_udata(L, narg);

    (void)luab_newstat(L, stat);

    return 1;
}

/*
 * Components or service primitives over sys/stat.h.
 */

#define LUABSD_SYS_STAT_LIB_ID    1593623310
#define LUABSD_SYS_STAT_LIB_KEY    "stat"

#if __BSD_VISIBLE
static int
luab_chflags(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    u_long flags = luab_checkinteger(L, 2, ULONG_MAX);
    int status;

    luab_checkmaxargs(L, 2);
    status = chflags(path, flags);

    return luab_pusherr(L, status);
}

static int
luab_chflagsat(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    const char *path = luab_checklstring(L, 2, MAXPATHLEN);
    u_long flags = luab_checkinteger(L, 3, ULONG_MAX);
    int atflag = luab_checkinteger(L, 4, INT_MAX);
    int status;

    luab_checkmaxargs(L, 4);
    status = chflagsat(fd, path, flags, atflag);

    return luab_pusherr(L, status);
}
#endif

static int
luab_chmod(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 2, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 2);
    status = chmod(path, mode);

    return luab_pusherr(L, status);
}

#if __BSD_VISIBLE
static int
luab_fchflags(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    u_long flags = luab_checkinteger(L, 2, ULONG_MAX);
    int status;

    status = fchflags(fd, flags);

    return luab_pusherr(L, status);
}
#endif

#if __POSIX_VISIBLE >= 200112
static int
luab_fchmod(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    mode_t mode = luab_checkinteger(L, 2, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 2);
    status = fchmod(fd, mode);

    return luab_pusherr(L, status);
}
#endif

#if __POSIX_VISIBLE >= 200809
static int
luab_fchmodat(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    const char *path = luab_checklstring(L, 2, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 3, ALLPERMS);
    int flag = luab_checkinteger(L, 4, INT_MAX);
    int status;

    luab_checkmaxargs(L, 4);
    status = fchmodat(fd, path, mode, flag);

    return luab_pusherr(L, status);
}
#endif /* __POSIX_VISIBLE >= 200809 */

static int
luab_fstat(lua_State *L)
{
    int fd;
    luab_stat_t *ud;
    int status;

    (void)luab_checkmaxargs(L, 2);

    fd = luab_checkinteger(L, 1, INT_MAX);
    ud = luab_tostat(L, 2);

    status = fstat(fd, &ud->stat);

    return luab_pusherr(L, status);
}

#if __BSD_VISIBLE
static int
luab_lchflags(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    u_long flags = luab_checkinteger(L, 2, ULONG_MAX);
    int status;

    luab_checkmaxargs(L, 2);
    status = lchflags(path, flags);

    return luab_pusherr(L, status);
}

static int
luab_lchmod(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 2, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 2);
    status = lchmod(path, mode);

    return luab_pusherr(L, status);
}
#endif

static int
luab_mkdir(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 2, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 2);
    status = mkdir(path, mode);

    return luab_pusherr(L, status);
}

static int
luab_mkfifo(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 2, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 2);
    status = mkfifo(path, mode);

    return luab_pusherr(L, status);
}

#if !defined(_MKNOD_DECLARED) && __XSI_VISIBLE
static int
luab_mknod(lua_State *L)
{
    const char *path = luab_checklstring(L, 1, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 2, ALLPERMS);
    dev_t dev = luab_checkinteger(L, 3, ULONG_MAX);
    int status;

    luab_checkmaxargs(L, 4);
    status = mknod(path, mode, dev);

    return luab_pusherr(L, status);
}
#define _MKNOD_DECLARED
#endif

static int
luab_umask(lua_State *L)
{
    mode_t numask = luab_checkinteger(L, 1, ALLPERMS);
    mode_t oumask;

    luab_checkmaxargs(L, 1);
    oumask = umask(numask);

    lua_pushinteger(L, oumask);

    return 1;
}

#if __POSIX_VISIBLE >= 200809
static int
luab_mkdirat(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    const char *path = luab_checklstring(L, 2, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 3, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 3);
    status = mkdirat(fd, path, mode);

    return luab_pusherr(L, status);
}

static int
luab_mkfifoat(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    const char *path = luab_checklstring(L, 2, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 3, ALLPERMS);
    int status;

    luab_checkmaxargs(L, 3);
    status = mkfifoat(fd, path, mode);

    return luab_pusherr(L, status);
}

static int
luab_mknodat(lua_State *L)
{
    int fd = luab_checkinteger(L, 1, INT_MAX);
    const char *path = luab_checklstring(L, 2, MAXPATHLEN);
    mode_t mode = luab_checkinteger(L, 3, ALLPERMS);
    dev_t dev = luab_checkinteger(L, 4, ULONG_MAX);
    int status;

    luab_checkmaxargs(L, 4);
    status = mknodat(fd, path, mode, dev);

    return luab_pusherr(L, status);
}
#endif /* __POSIX_VISIBLE >= 200809 */



static luab_table_t luab_sys_stat_vec[] = { /* sys/stat.h */
    LUABSD_INT("S_ISUID",    S_ISUID),
    LUABSD_INT("S_ISGID",    S_ISGID),
#if __BSD_VISIBLE
    LUABSD_INT("S_ISTXT",    S_ISTXT),
#endif
    LUABSD_INT("S_IRWXU",    S_IRWXU),
    LUABSD_INT("S_IRUSR",    S_IRUSR),
    LUABSD_INT("S_IWUSR",    S_IWUSR),
    LUABSD_INT("S_IXUSR",    S_IXUSR),
#if __BSD_VISIBLE
    LUABSD_INT("S_IREAD",    S_IREAD),
    LUABSD_INT("S_IWRITE",   S_IWRITE),
    LUABSD_INT("S_IEXEC",    S_IEXEC),
#endif
    LUABSD_INT("S_IRWXG",    S_IRWXG),
    LUABSD_INT("S_IRGRP",    S_IRGRP),
    LUABSD_INT("S_IWGRP",    S_IWGRP),
    LUABSD_INT("S_IXGRP",    S_IXGRP),
    LUABSD_INT("S_IRWXO",    S_IRWXO),
    LUABSD_INT("S_IROTH",    S_IROTH),
    LUABSD_INT("S_IWOTH",    S_IWOTH),
    LUABSD_INT("S_IXOTH",    S_IXOTH),
#if __XSI_VISIBLE
    LUABSD_INT("S_IFMT", S_IFMT),
    LUABSD_INT("S_IFIFO",    S_IFIFO),
    LUABSD_INT("S_IFCHR",    S_IFCHR),
    LUABSD_INT("S_IFDIR",    S_IFDIR),
    LUABSD_INT("S_IFBLK",    S_IFBLK),
    LUABSD_INT("S_IFREG",    S_IFREG),
    LUABSD_INT("S_IFLNK",    S_IFLNK),
    LUABSD_INT("S_IFSOCK",   S_IFSOCK),
    LUABSD_INT("S_ISVTX",    S_ISVTX),
#endif
#if __BSD_VISIBLE
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
    LUABSD_INT("SF_SETTABLE",   SF_SETTABLE),
    LUABSD_INT("SF_ARCHIVED",   SF_ARCHIVED),
    LUABSD_INT("SF_IMMUTABLE",  SF_IMMUTABLE),
    LUABSD_INT("SF_APPEND", SF_APPEND),
    LUABSD_INT("SF_NOUNLINK",   SF_NOUNLINK),
    LUABSD_INT("SF_SNAPSHOT",   SF_SNAPSHOT),
#endif /* __BSD_VISIBLE */

#if __POSIX_VISIBLE >= 200809
    LUABSD_INT("UTIME_NOW", UTIME_NOW),
    LUABSD_INT("UTIME_OMIT",    UTIME_OMIT),
#endif
#if __BSD_VISIBLE
    LUABSD_FUNC("chflags",  luab_chflags),
    LUABSD_FUNC("chflagsat", luab_chflagsat),
#endif
    LUABSD_FUNC("chmod",    luab_chmod),
#if __BSD_VISIBLE
    LUABSD_FUNC("fchflags", luab_fchflags),
#endif
#if __POSIX_VISIBLE >= 200112
    LUABSD_FUNC("fchmod",   luab_fchmod),
#endif
#if __POSIX_VISIBLE >= 200809
    LUABSD_FUNC("fchmodat", luab_fchmodat),
/*
 * XXX
 *
 *  int futimens(int fd, const struct timespec times[2]);
 *  int utimensat(int fd, const char *path, const struct timespec times[2],
 *      int flag);
 *
 *  LUABSD_FUNC("futimens", luab_futimens),
 *  LUABSD_FUNC("utimensat", luab_utimensat),
 */
#endif
    LUABSD_FUNC("fstat",    luab_fstat),
#if __BSD_VISIBLE
    LUABSD_FUNC("lchflags", luab_lchflags),
#endif
#if __POSIX_VISIBLE >= 200112
/*
 * XXX
 *
 *  int lstat(const char * __restrict, struct stat * __restrict);
 */
#endif
    LUABSD_FUNC("lchmod",   luab_lchmod),
    LUABSD_FUNC("mkdir",   luab_mkdir),
    LUABSD_FUNC("mkdirat",   luab_mkdirat),
    LUABSD_FUNC("mkfifo",   luab_mkfifo),
    LUABSD_FUNC("mkfifoat",   luab_mkfifoat),
#if !defined(_MKNOD_DECLARED) && __XSI_VISIBLE
    LUABSD_FUNC("mknod",    luab_mknod),
#define _MKNOD_DECLARED
#endif
/*
 * XXX
 *
 *  int stat(const char * __restrict, struct stat * __restrict);
 */
    LUABSD_FUNC("umask",    luab_umask),
#if __XSI_VISIBLE >= 700
    LUABSD_FUNC("mknodat",    luab_mknodat),
#endif
    LUABSD_FUNC("StructStat",   luab_StructStat),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_sys_stat_lib = {
    .cookie = LUABSD_SYS_STAT_LIB_ID,
    .name = LUABSD_SYS_STAT_LIB_KEY,
    .vec = luab_sys_stat_vec,
};
