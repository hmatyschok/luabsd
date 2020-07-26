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
    struct stat    stat;
} luab_stat_t;

#define luab_new_stat(L, arg) \
    ((luab_stat_t *)luab_newuserdata(L, &stat_type, (arg)))
#define luab_to_stat(L, narg) \
    (luab_todata((L), (narg), &stat_type, luab_stat_t *))

#define LUABSD_STAT_TYPE_ID    1594028586
#define LUABSD_STAT_TYPE    "STAT*"

int luab_StructStat(lua_State *);

/* inode's device */
static int
Stat_set_st_dev(lua_State *L)
{
    luab_stat_t *self;
    dev_t st_dev;

    luab_checkmaxargs(L, 2);

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->stat.st_atim, ud, timespec_type.sz);

    return 0;
}

static int
Stat_get_st_atim(lua_State *L)
{
    luab_stat_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_to_stat(L, 1);

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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->stat.st_mtim, ud, timespec_type.sz);

    return 0;
}

static int
Stat_get_st_mtim(lua_State *L)
{
    luab_stat_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_to_stat(L, 1);

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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->stat.st_ctim, ud, timespec_type.sz);

    return 0;
}

static int
Stat_get_st_ctim(lua_State *L)
{
    luab_stat_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_to_stat(L, 1);

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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
    ud = luab_checkudata(L, 2, &timespec_type);

    (void)memmove(&self->stat.st_birthtim, ud, timespec_type.sz);

    return 0;
}

static int
Stat_get_st_birthtim(lua_State *L)
{
    luab_stat_t *self;

    luab_checkmaxargs(L, 1);

    self = luab_to_stat(L, 1);

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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);
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

    self = luab_to_stat(L, 1);

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
Stat_gc(lua_State *L)
{
    luab_stat_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_stat(L, 1);

    (void)memset_s(self, stat_type.sz, 0, stat_type.sz);

    return 0;
}

static int
Stat_tostring(lua_State *L)
{
    luab_stat_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_stat(L, 1);
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
    LUABSD_FUNC("__gc", Stat_gc),
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
    luab_stat_t *self = luab_to_stat(L, narg);

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

int
luab_StructStat(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 1);
    struct stat *stat;
    int status;

    if (narg == 0)
        stat = NULL;
    else
        stat = stat_udata(L, narg);

    if (luab_new_stat(L, stat) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}
