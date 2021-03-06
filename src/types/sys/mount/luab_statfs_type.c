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

#include <sys/mount.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_statfs_type;

/*
 * Interface against
 *
 *  struct statfs {
 *      uint32_t f_version;
 *      uint32_t f_type;
 *      uint64_t f_flags;
 *      uint64_t f_bsize;
 *      uint64_t f_iosize;
 *      uint64_t f_blocks;
 *      uint64_t f_bfree;
 *      int64_t  f_bavail;
 *      uint64_t f_files;
 *      int64_t  f_ffree;
 *      uint64_t f_syncwrites;
 *      uint64_t f_asyncwrites;
 *      uint64_t f_syncreads;
 *      uint64_t f_asyncreads;
 *      uint64_t f_spare[10];
 *      uint32_t f_namemax;
 *      uid_t     f_owner;
 *      fsid_t    f_fsid;
 *      char      f_charspare[80];
 *      char      f_fstypename[MFSNAMELEN];
 *      char      f_mntfromname[MNAMELEN];
 *      char      f_mntonname[MNAMELEN];
 *  };
 */

typedef struct luab_statfs {
    luab_udata_t    ud_softc;
    struct statfs   ud_statfs;
} luab_statfs_t;

/*
 * Subr.
 */

static void
statfs_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_module_t *m;
    struct statfs *f;

    m = luab_xmod(FSID, TYPE, __func__);

    if ((f = (struct statfs *)arg) != NULL) {

        luab_setinteger(L, narg, "f_version",     f->f_version);
        luab_setinteger(L, narg, "f_type",        f->f_type);
        luab_setinteger(L, narg, "f_flags",       f->f_flags);
        luab_setinteger(L, narg, "f_bsize",       f->f_bsize);
        luab_setinteger(L, narg, "f_iosize",      f->f_iosize);
        luab_setinteger(L, narg, "f_blocks",      f->f_blocks);
        luab_setinteger(L, narg, "f_bfree",       f->f_bfree);
        luab_setinteger(L, narg, "f_bavail",      f->f_bavail);
        luab_setinteger(L, narg, "f_files",       f->f_files);
        luab_setinteger(L, narg, "f_ffree",       f->f_ffree);
        luab_setinteger(L, narg, "f_syncwrites",  f->f_syncwrites);
        luab_setinteger(L, narg, "f_asyncwrites", f->f_asyncwrites);
        luab_setinteger(L, narg, "f_syncreads",   f->f_syncreads);
        luab_setinteger(L, narg, "f_asyncreads",  f->f_asyncreads);
        luab_setinteger(L, narg, "f_namemax",     f->f_namemax);
        luab_setinteger(L, narg, "f_owner",       f->f_owner);

        luab_setxdata(L, -2, m, "f_fsid", (void *)&(f->f_fsid));

        luab_setldata(L, narg, "f_fstypename",    f->f_fstypename, MFSNAMELEN);
        luab_setldata(L, narg, "f_mntfromname",   f->f_mntfromname, MNAMELEN);
        luab_setldata(L, narg, "f_mntonname",     f->f_mntonname, MNAMELEN);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(STATFS)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              f_version       = (LUA_TNUMBER),
 *              f_type          = (LUA_TNUMBER),
 *              f_flags         = (LUA_TNUMBER),
 *              f_bsize         = (LUA_TNUMBER),
 *              f_iosize        = (LUA_TNUMBER),
 *              f_blocks        = (LUA_TNUMBER),
 *              f_bfree         = (LUA_TNUMBER),
 *              f_bavail        = (LUA_TNUMBER),
 *              f_files         = (LUA_TNUMBER),
 *              f_ffree         = (LUA_TNUMBER),
 *              f_syncwrites    = (LUA_TNUMBER),
 *              f_asyncwrites   = (LUA_TNUMBER),
 *              f_syncreads     = (LUA_TNUMBER),
 *              f_asyncreads    = (LUA_TNUMBER),
 *              f_namemax       = (LUA_TNUMBER),
 *              f_owner         = (LUA_TNUMBER),
 *              f_fsid          = (LUA_TUSERDATA(FSID)),
 *              f_fstypename    = (LUA_T{NIL,STRING}),
 *              f_mntfromname   = (LUA_T{NIL,STRING}),
 *              f_mntonname     = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t = statfs:get_table()
 */
static int
STATFS_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);

    xtp.xtp_fill = statfs_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate statfs{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = statfs:dump()
 */
static int
STATFS_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STATFS, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get structure version number.
 *
 * @function f_version
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_version()
 */
static int
STATFS_f_version(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uint32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_version;

    return (luab_pushxinteger(L, x));
}

/***
 * Get type of filesystem.
 *
 * @function f_type
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_type()
 */
static int
STATFS_f_type(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uint32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_type;

    return (luab_pushxinteger(L, x));
}

/***
 * Get copy of mount exported flags.
 *
 * @function f_flags
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_flags()
 */
static int
STATFS_f_flags(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uint64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_flags;

    return (luab_pushxinteger(L, x));
}

/***
 * Get filesystem fragment size.
 *
 * @function f_bsize
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_bsize()
 */
static int
STATFS_f_bsize(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uint64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_bsize;

    return (luab_pushxinteger(L, x));
}

/***
 * Get optimal transfer block size.
 *
 * @function f_iosize
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_iosize()
 */
static int
STATFS_f_iosize(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uint64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_iosize;

    return (luab_pushxinteger(L, x));
}

/***
 * Get total data blocks in filesystem.
 *
 * @function f_blocks
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_blocks()
 */
static int
STATFS_f_blocks(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uint64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_blocks;

    return (luab_pushxinteger(L, x));
}

/***
 * Get free blocks in filesystem.
 *
 * @function f_bfree
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_blocks()
 */
static int
STATFS_f_bfree(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uint64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_bfree;

    return (luab_pushxinteger(L, x));
}

/***
 * Get free blocks avail to non-superuser.
 *
 * @function f_bavail
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_bavail()
 */
static int
STATFS_f_bavail(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    int64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_bavail;

    return (luab_pushxinteger(L, x));
}

/***
 * Get total file nodes in filesystem.
 *
 * @function f_files
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_files()
 */
static int
STATFS_f_files(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uint64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_files;

    return (luab_pushxinteger(L, x));
}

/***
 * Get free nodes avail to non-superuser.
 *
 * @function f_ffree
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_ffree()
 */
static int
STATFS_f_ffree(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    int64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_ffree;

    return (luab_pushxinteger(L, x));
}

/***
 * Get count of sync writes since mount.
 *
 * @function f_syncwrites
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_syncwrites()
 */
static int
STATFS_f_syncwrites(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uint64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_syncwrites;

    return (luab_pushxinteger(L, x));
}

/***
 * Get count of async writes since mount.
 *
 * @function f_asyncwrites
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_asyncwrites()
 */
static int
STATFS_f_asyncwrites(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uint64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_asyncwrites;

    return (luab_pushxinteger(L, x));
}

/***
 * Get count of sync reads since mount.
 *
 * @function f_syncreads
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_syncreads()
 */
static int
STATFS_f_syncreads(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uint64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_syncreads;

    return (luab_pushxinteger(L, x));
}

/***
 * Get count of async reads since mount.
 *
 * @function f_asyncreads
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_asyncreads()
 */
static int
STATFS_f_asyncreads(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uint64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_asyncreads;

    return (luab_pushxinteger(L, x));
}

/***
 * Get maximum filename length.
 *
 * @function f_namemax
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_namemax()
 */
static int
STATFS_f_namemax(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uint32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_namemax;

    return (luab_pushxinteger(L, x));
}

/***
 * Get user that mounted the filesystem.
 *
 * @function f_owner
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_owner()
 */
static int
STATFS_f_owner(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    uid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    x = f->f_owner;

    return (luab_pushxinteger(L, x));
}

/***
 * Get filesystem ID.
 *
 * @function f_fsid
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_fsid()
 */
static int
STATFS_f_fsid(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct statfs *f;
    void *v;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(STATFS, TYPE, __func__);
    m1 = luab_xmod(FSID, TYPE, __func__);

    f = luab_udata(L, 1, m0, struct statfs *);
    v = (void *)&(f->f_fsid);

    return (luab_pushxdata(L, m1, v));
}

/***
 * Get filesystem type name.
 *
 * @function f_fstypename
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_fstypename()
 */
static int
STATFS_f_fstypename(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    dp = f->f_fstypename;

    return (luab_pushldata(L, dp, MFSNAMELEN)); /* XXX */
}

/***
 * Get mounted filesystem.
 *
 * @function f_mntfromname
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_mntfromname()
 */
static int
STATFS_f_mntfromname(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    dp = f->f_mntfromname;

    return (luab_pushldata(L, dp, MNAMELEN));   /* XXX */
}

/***
 * Get directory on which mounted.
 *
 * @function f_mntonname
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = statfs:f_mntonname()
 */
static int
STATFS_f_mntonname(lua_State *L)
{
    luab_module_t *m;
    struct statfs *f;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STATFS, TYPE, __func__);
    f = luab_udata(L, 1, m, struct statfs *);
    dp = f->f_mntonname;

    return (luab_pushldata(L, dp, MNAMELEN));
}

/*
 * Meta-methods.
 */

static int
STATFS_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STATFS, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
STATFS_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STATFS, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
STATFS_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STATFS, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t statfs_methods[] = {
    LUAB_FUNC("f_version",      STATFS_f_version),
    LUAB_FUNC("f_type",         STATFS_f_type),
    LUAB_FUNC("f_flags",        STATFS_f_flags),
    LUAB_FUNC("f_bsize",        STATFS_f_bsize),
    LUAB_FUNC("f_iosize",       STATFS_f_iosize),
    LUAB_FUNC("f_blocks",       STATFS_f_blocks),
    LUAB_FUNC("f_bfree",        STATFS_f_bfree),
    LUAB_FUNC("f_bavail",       STATFS_f_bavail),
    LUAB_FUNC("f_files",        STATFS_f_files),
    LUAB_FUNC("f_ffree",        STATFS_f_ffree),
    LUAB_FUNC("f_syncwrites",   STATFS_f_syncwrites),
    LUAB_FUNC("f_asyncwrites",  STATFS_f_asyncwrites),
    LUAB_FUNC("f_syncreads",    STATFS_f_syncreads),
    LUAB_FUNC("f_asyncreads",   STATFS_f_asyncreads),
    LUAB_FUNC("f_namemax",      STATFS_f_namemax),
    LUAB_FUNC("f_owner",        STATFS_f_owner),
    LUAB_FUNC("f_fsid",         STATFS_f_fsid),
    LUAB_FUNC("f_fstypename",   STATFS_f_fstypename),
    LUAB_FUNC("f_mntfromname",  STATFS_f_mntfromname),
    LUAB_FUNC("f_mntonname",    STATFS_f_mntonname),
    LUAB_FUNC("get_table",      STATFS_get_table),
    LUAB_FUNC("dump",           STATFS_dump),
    LUAB_FUNC("__gc",           STATFS_gc),
    LUAB_FUNC("__len",          STATFS_len),
    LUAB_FUNC("__tostring",     STATFS_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
statfs_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(STATFS, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
statfs_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(STATFS, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
statfs_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(STATFS, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
statfs_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct statfs *x, *y;
    size_t i, j;

    m = luab_xmod(STATFS, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct statfs *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct statfs *);
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
statfs_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct statfs *x;
    size_t i, j, k;

    m = luab_xmod(STATFS, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct statfs *)tbl->tbl_vec) != NULL) &&
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
statfs_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(STATFS, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_statfs_type = {
    .m_id           = LUAB_STATFS_TYPE_ID,
    .m_name         = LUAB_STATFS_TYPE,
    .m_vec          = statfs_methods,
    .m_create       = statfs_create,
    .m_init         = statfs_init,
    .m_get          = statfs_udata,
    .m_get_tbl      = statfs_checktable,
    .m_set_tbl      = statfs_pushtable,
    .m_alloc_tbl    = statfs_alloctable,
    .m_len          = sizeof(luab_statfs_t),
    .m_sz           = sizeof(struct statfs),
};
