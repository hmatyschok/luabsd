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

extern luab_module_t luab_stat_type;

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
    struct stat     ud_st;
} luab_stat_t;

/*
 * Subr.
 */

static void
stat_fillxtable(lua_State *L, int narg, void *arg)
{
    struct stat *st;

    if ((st = (struct stat *)arg) != NULL) {

        luab_setinteger(L, narg, "st_dev",                        st->st_dev);
        luab_setinteger(L, narg, "st_ino",                        st->st_ino);
        luab_setinteger(L, narg, "st_nlink",                      st->st_nlink);
        luab_setinteger(L, narg, "st_mode",                       st->st_mode);
        luab_setinteger(L, narg, "st_uid",                        st->st_uid);
        luab_setinteger(L, narg, "st_gid",                        st->st_gid);
        luab_setinteger(L, narg, "st_rdev",                       st->st_rdev);

#ifdef  __STAT_TIME_T_EXT
        luab_setinteger(L, narg, "st_atim_ext",                   st->st_atim_ext);
#endif
        luab_setxdata(L, -2, luab_xmod(TIMESPEC, TYPE, __func__), "st_atim",      &st->st_atim);

#ifdef  __STAT_TIME_T_EXT
        luab_setinteger(L, narg, "st_mtim_ext",                   st->st_mtim_ext);
#endif
        luab_setxdata(L, -2, luab_xmod(TIMESPEC, TYPE, __func__), "st_mtim",      &st->st_mtim);

#ifdef  __STAT_TIME_T_EXT
        luab_setinteger(L, narg, "st_ctim_ext",                   st->st_ctim_ext);
#endif
        luab_setxdata(L, -2, luab_xmod(TIMESPEC, TYPE, __func__), "st_ctim",      &st->st_ctim);

#ifdef  __STAT_TIME_T_EXT
        luab_setinteger(L, narg, "st_btim_ext",                   st->st_btim_ext);
#endif
        luab_setxdata(L, -2, luab_xmod(TIMESPEC, TYPE, __func__), "st_birthtim",  &st->st_birthtim);

        luab_setinteger(L, narg, "st_size",                       st->st_size);
        luab_setinteger(L, narg, "st_blocks",                     st->st_blocks);
        luab_setinteger(L, narg, "st_blksize",                    st->st_blksize);
        luab_setinteger(L, narg, "st_flags",                      st->st_flags);
        luab_setinteger(L, narg, "st_gen",                        st->st_gen);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(STAT)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
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
 * @usage t [, err, msg ] = stat:get_table()
 */
static int
STAT_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STAT, TYPE, __func__);

    xtp.xtp_fill = stat_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate stat{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = stat:dump()
 */
static int
STAT_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions.
 */

/* inode's device */
static int
STAT_set_st_dev(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct stat *st;
    dev_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(STAT, TYPE, __func__);
    m1 = luab_xmod(DEV, TYPE, __func__);

    st = luab_udata(L, 1, m0, struct stat *);
    x = (dev_t)luab_checkxinteger(L, 2, m1, luab_env_long_max);

    st->st_dev = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_dev(lua_State *L)
{
    luab_module_t *m;
    struct stat *st;
    dev_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STAT, TYPE, __func__);
    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_dev;

    return (luab_pushxinteger(L, x));
}

/* inode's number */
static int
STAT_set_st_ino(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct stat *st;
    ino_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(STAT, TYPE, __func__);
    m1 = luab_xmod(INO, TYPE, __func__);

    st = luab_udata(L, 1, m0, struct stat *);
    x = (ino_t)luab_checkxinteger(L, 2, m1, luab_env_ulong_max);

    st->st_ino = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_ino(lua_State *L)
{
    luab_module_t *m;
    struct stat *st;
    ino_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STAT, TYPE, __func__);
    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_ino;

    return (luab_pushxinteger(L, x));
}

/* number of hard links */
static int
STAT_set_st_nlink(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct stat *st;
    nlink_t x;

    (void)luab_core_checkmaxargs(L, 2);

    
    
    st = luab_udata(L, 1, m, struct stat *);
    x = (nlink_t)luab_checkxinteger(L, 2, luab_env_long_max);

    st->st_nlink = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_nlink(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    nlink_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STAT, TYPE, __func__);
    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_nlink;

    return (luab_pushxinteger(L, x));
}

/* inode protection mode */
static int
STAT_set_st_mode(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    mode_t x;

    (void)luab_core_checkmaxargs(L, 2);

    
    st = luab_udata(L, 1, m, struct stat *);
    x = (mode_t)luab_checkxinteger(L, 2, luab_env_ushrt_max);

    st->st_mode = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_mode(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    mode_t x;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_mode;

    return (luab_pushxinteger(L, x));
}

/* user ID of the file's owner */
static int
STAT_set_st_uid(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    uid_t x;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    x = (uid_t)luab_checkxinteger(L, 2, luab_env_int_max);

    st->st_uid = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_uid(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    uid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_uid;

    return (luab_pushxinteger(L, x));
}

/* user ID of the file's owner */
static int
STAT_set_st_gid(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    gid_t x;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    x = (gid_t)luab_checkxinteger(L, 2, luab_env_int_max);

    st->st_gid = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_gid(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    gid_t x;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_gid;

    return (luab_pushxinteger(L, x));
}

/* device type */
static int
STAT_set_st_rdev(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    dev_t x;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    x = (dev_t)luab_checkxinteger(L, 2, luab_env_int_max);

    st->st_rdev = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_rdev(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    dev_t x;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_rdev;

    return (luab_pushxinteger(L, x));
}

/* time of last access */
#ifdef  __STAT_TIME_T_EXT
static int
STAT_set_st_atim_ext(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    __int32_t x;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    x = (__int32_t)luab_checkxinteger(L, 2, luab_env_int_max);

    st->st_atim_ext = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_atim_ext(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    __int32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_atim_ext;

    return (luab_pushxinteger(L, x));
}
#endif

static int
STAT_set_st_atim(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    struct timespec *tv;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    tv = luab_udata(L, 2, luab_xmod(TIMESPEC, TYPE, __func__), struct timespec *);

    (void)memmove(&st->st_atim, tv, sizeof(*tv));

    return (luab_pushxdata(L, luab_xmod(TIMESPEC, TYPE, __func__), tv));
}

static int
STAT_get_st_atim(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    struct timespec *tv;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    tv = &st->st_atim;

    return (luab_pushxdata(L, luab_xmod(TIMESPEC, TYPE, __func__), tv));
}

/* time of last data modification */
#ifdef  __STAT_TIME_T_EXT
static int
STAT_set_st_mtim_ext(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    __int32_t x;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    x = (__int32_t)luab_checkxinteger(L, 2, luab_env_int_max);

    st->st_mtim_ext = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_mtim_ext(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    __int32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_mtim_ext;

    return (luab_pushxinteger(L, x));
}
#endif

static int
STAT_set_st_mtim(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    struct timespec *tv;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    tv = luab_udata(L, 2, luab_xmod(TIMESPEC, TYPE, __func__), struct timespec *);

    (void)memmove(&st->st_mtim, tv, sizeof(*tv));

    return (luab_pushxdata(L, luab_xmod(TIMESPEC, TYPE, __func__), tv));
}

static int
STAT_get_st_mtim(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    struct timespec *tv;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    tv = &st->st_mtim;

    return (luab_pushxdata(L, luab_xmod(TIMESPEC, TYPE, __func__), tv));
}

/* time of last file status change */
#ifdef  __STAT_TIME_T_EXT
static int
STAT_set_st_ctim_ext(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    __int32_t x;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    x = (__int32_t)luab_checkxinteger(L, 2, luab_env_int_max);

    st->st_ctim_ext = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_ctim_ext(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    __int32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_ctim_ext;

    return (luab_pushxinteger(L, x));
}
#endif

static int
STAT_set_st_ctim(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    struct timespec *tv;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    tv = luab_udata(L, 2, luab_xmod(TIMESPEC, TYPE, __func__), struct timespec *);

    (void)memmove(&st->st_ctim, tv, sizeof(*tv));

    return (luab_pushxdata(L, luab_xmod(TIMESPEC, TYPE, __func__), tv));
}

static int
STAT_get_st_ctim(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    struct timespec *tv;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    tv = &st->st_ctim;

    return (luab_pushxdata(L, luab_xmod(TIMESPEC, TYPE, __func__), tv));
}

/* time of file creation */
#ifdef  __STAT_TIME_T_EXT
static int
STAT_set_st_btim_ext(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    __int32_t x;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    x = (__int32_t)luab_checkxinteger(L, 2, luab_env_int_max);

    st->st_btim_ext = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_btim_ext(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    __int32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_btim_ext;

    return (luab_pushxinteger(L, x));
}
#endif

static int
STAT_set_st_birthtim(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    struct timespec *tv;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    tv = luab_udata(L, 2, luab_xmod(TIMESPEC, TYPE, __func__), struct timespec *);

    (void)memmove(&st->st_birthtim, tv, sizeof(*tv));

    return (luab_pushxdata(L, luab_xmod(TIMESPEC, TYPE, __func__), tv));
}

static int
STAT_get_st_birthtim(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    struct timespec *tv;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    tv = &st->st_birthtim;

    return (luab_pushxdata(L, luab_xmod(TIMESPEC, TYPE, __func__), tv));
}

/* file size, in bytes */
static int
STAT_set_st_size(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    off_t x;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    x = (off_t)luab_checkxinteger(L, 2, luab_env_long_max);

    st->st_size = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_size(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    off_t x;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_size;

    return (luab_pushxinteger(L, x));
}

/* blocks allocated for file */
static int
STAT_set_st_blocks(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    blkcnt_t x;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    x = (blkcnt_t)luab_checkxinteger(L, 2, luab_env_long_max);

    st->st_blocks = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_blocks(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    blkcnt_t x;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_blocks;

    return (luab_pushxinteger(L, x));
}

/* optimal blocksize for I/O */
static int
STAT_set_st_blksize(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    blksize_t x;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    x = (blksize_t)luab_checkxinteger(L, 2, luab_env_int_max);

    st->st_blksize = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_blksize(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    blksize_t x;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_blksize;

    return (luab_pushxinteger(L, x));
}

/* user defined flags for file */
static int
STAT_set_st_flags(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    fflags_t x;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    x = (fflags_t)luab_checkxinteger(L, 2, luab_env_uint_max);

    st->st_flags = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_flags(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    fflags_t x;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_flags;

    return (luab_pushxinteger(L, x));
}

/* file generation number */
static int
STAT_set_st_gen(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    __uint64_t x;

    (void)luab_core_checkmaxargs(L, 2);

    st = luab_udata(L, 1, m, struct stat *);
    x = (__uint64_t)luab_checkxinteger(L, 2, luab_env_long_max);

    st->st_gen = x;

    return (luab_pushxinteger(L, x));
}

static int
STAT_get_st_gen(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *st;
    __uint64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    st = luab_udata(L, 1, m, struct stat *);
    x = st->st_gen;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
STAT_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
STAT_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
STAT_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t stat_methods[] = {
    LUAB_FUNC("set_st_dev",         STAT_set_st_dev),
    LUAB_FUNC("set_st_ino",         STAT_set_st_ino),
    LUAB_FUNC("set_st_nlink",       STAT_set_st_nlink),
    LUAB_FUNC("set_st_mode",        STAT_set_st_mode),
    LUAB_FUNC("set_st_uid",         STAT_set_st_uid),
    LUAB_FUNC("set_st_gid",         STAT_set_st_gid),
    LUAB_FUNC("set_st_rdev",        STAT_set_st_rdev),
#ifdef  __STAT_TIME_T_EXT
    LUAB_FUNC("set_st_atim_ext",    STAT_set_st_atim_ext),
#endif
    LUAB_FUNC("set_st_atim",        STAT_set_st_atim),
#ifdef  __STAT_TIME_T_EXT
    LUAB_FUNC("set_st_mtim_ext",    STAT_set_st_mtim_ext),
#endif
    LUAB_FUNC("set_st_mtim",        STAT_set_st_mtim),
#ifdef  __STAT_TIME_T_EXT
    LUAB_FUNC("set_st_ctim_ext",    STAT_set_st_ctim_ext),
#endif
    LUAB_FUNC("set_st_ctim",        STAT_set_st_ctim),
#ifdef  __STAT_TIME_T_EXT
    LUAB_FUNC("set_st_btim_ext",    STAT_set_st_btim_ext),
#endif
    LUAB_FUNC("set_st_birthtim",    STAT_set_st_birthtim),
    LUAB_FUNC("set_st_size",        STAT_set_st_size),
    LUAB_FUNC("set_st_blocks",      STAT_set_st_blocks),
    LUAB_FUNC("set_st_blocks",      STAT_set_st_blksize),
    LUAB_FUNC("set_st_flags",       STAT_set_st_flags),
    LUAB_FUNC("set_st_gen",         STAT_set_st_gen),
    LUAB_FUNC("get_table",          STAT_get_table),
    LUAB_FUNC("get_st_dev",         STAT_get_st_dev),
    LUAB_FUNC("get_st_ino",         STAT_get_st_ino),
    LUAB_FUNC("get_st_nlink",       STAT_get_st_nlink),
    LUAB_FUNC("get_st_mode",        STAT_get_st_mode),
    LUAB_FUNC("get_st_uid",         STAT_get_st_uid),
    LUAB_FUNC("get_st_gid",         STAT_get_st_gid),
    LUAB_FUNC("get_st_rdev",        STAT_get_st_rdev),
#ifdef  __STAT_TIME_T_EXT
    LUAB_FUNC("get_st_atim_ext",    STAT_get_st_atim_ext),
#endif
    LUAB_FUNC("get_st_atim",        STAT_get_st_atim),
#ifdef  __STAT_TIME_T_EXT
    LUAB_FUNC("get_st_mtim_ext",    STAT_get_st_mtim_ext),
#endif
    LUAB_FUNC("get_st_mtim",        STAT_get_st_mtim),
#ifdef  __STAT_TIME_T_EXT
    LUAB_FUNC("get_st_ctim_ext",    STAT_get_st_ctim_ext),
#endif
    LUAB_FUNC("get_st_ctim",        STAT_get_st_ctim),
#ifdef  __STAT_TIME_T_EXT
    LUAB_FUNC("get_st_btim_ext",    STAT_get_st_btim_ext),
#endif
    LUAB_FUNC("get_st_birthtim",    STAT_get_st_birthtim),
    LUAB_FUNC("get_st_size",        STAT_get_st_size),
    LUAB_FUNC("get_st_blocks",      STAT_get_st_blocks),
    LUAB_FUNC("get_st_blocks",      STAT_get_st_blksize),
    LUAB_FUNC("get_st_flags",       STAT_get_st_flags),
    LUAB_FUNC("get_st_gen",         STAT_get_st_gen),
    LUAB_FUNC("dump",               STAT_dump),
    LUAB_FUNC("__gc",               STAT_gc),
    LUAB_FUNC("__len",              STAT_len),
    LUAB_FUNC("__tostring",         STAT_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
stat_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    return (luab_newudata(L, m, arg));
}

static void
stat_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
stat_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
stat_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    luab_table_t *tbl;
    struct stat *x, *y;
    size_t i, j;

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct stat *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct stat *);
                        (void)memmove(&(x[i]), y, m->m_sz);
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        }
    }
    return (tbl);
}

static void
stat_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    struct stat *x;
    size_t i, j, k;

    if (tbl != NULL) {

        if (((x = (struct stat *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, new);

            for (i = 0, j = tbl->tbl_card, k = 1; i < j; i++, k++)
                luab_rawsetxdata(L, narg, m, k, &(x[i]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

static luab_table_t *
stat_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(STAT, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_stat_type = {
    .m_id           = LUAB_STAT_TYPE_ID,
    .m_name         = LUAB_STAT_TYPE,
    .m_vec          = stat_methods,
    .m_create       = stat_create,
    .m_init         = stat_init,
    .m_get          = stat_udata,
    .m_get_tbl      = stat_checktable,
    .m_set_tbl      = stat_pushtable,
    .m_alloc_tbl    = stat_alloctable,
    .m_len          = sizeof(luab_stat_t),
    .m_sz           = sizeof(struct sockproto),
};
