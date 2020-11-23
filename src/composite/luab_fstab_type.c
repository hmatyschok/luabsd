/*
 * Copyright (c) 2020 Henning Matyschok
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
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY FSTABECT,
 * INFSTABECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <fstab.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_fstab_type;

/*
 * Interface against
 *
 *  struct fstab {
 *      char	*fs_spec;
 *      char	*fs_file;
 *      char	*fs_vfstype;
 *      char	*fs_mntops;
 *      char	*fs_type;
 *      int	fs_freq;
 *      int	fs_passno;
 *  };
 *
 */

typedef struct luab_fstab {
    luab_udata_t    ud_softc;
    struct fstab    ud_fs;
} luab_fstab_t;

#define luab_new_fstab(L, arg) \
    ((luab_fstab_t *)luab_newudata(L, &luab_fstab_type, (arg)))
#define luab_to_fstab(L, narg) \
    (luab_toldata((L), (narg), &luab_fstab_type, \
        struct fstab *, sizeof(struct fstab)))

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(FSTAB)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              fs_spec     = (LUA_TSTRING),
 *              fs_file     = (LUA_TSTRING),
 *              fs_vfstype  = (LUA_TSTRING),
 *              fs_mntops   = (LUA_TSTRING),
 *              fs_type     = (LUA_TSTRING),
 *              fs_freq     = (LUA_TNUMBER),
 *              fs_passno   = (LUA_TNUMBER),
 *          }
 *
 * @usage t = fstab:get()
 */
static int
FSTAB_get(lua_State *L)
{
    struct fstab *fs;

    (void)luab_core_checkmaxargs(L, 1);

    fs = luab_udata(L, 1, &luab_fstab_type, struct fstab *);

    lua_newtable(L);
    luab_setstring(L, -2, "fs_spec",        fs->fs_spec);
    luab_setstring(L, -2, "fs_file",        fs->fs_file);
    luab_setstring(L, -2, "fs_vfstype",     fs->fs_vfstype);
    luab_setstring(L, -2, "fs_mntops",      fs->fs_mntops);
    luab_setstring(L, -2, "fs_type",        fs->fs_type);
    luab_setinteger(L, -2, "fs_freq",       fs->fs_freq);
    luab_setinteger(L, -2, "fs_passno",     fs->fs_passno);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate fstab{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = fstab:dump()
 */
static int
FSTAB_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_fstab_type, sizeof(struct fstab)));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get get block special device name.
 *
 * @function fs_spec
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = fstab:fs_spec()
 */
static int
FSTAB_fs_spec(lua_State *L)
{
    struct fstab *fs;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    fs = luab_udata(L, 1, &luab_fstab_type, struct fstab *);
    dp = fs->fs_spec;

    return (luab_pushstring(L, dp));
}

/***
 * Get file system path prefix.
 *
 * @function fs_file
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = fstab:fs_file()
 */
static int
FSTAB_fs_file(lua_State *L)
{
    struct fstab *fs;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    fs = luab_udata(L, 1, &luab_fstab_type, struct fstab *);
    dp = fs->fs_file;

    return (luab_pushstring(L, dp));
}

/***
 * Get file system type, e. g. ufs, nfs, etc.
 *
 * @function fs_vfstype
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = fstab:fs_vfstype()
 */
static int
FSTAB_fs_vfstype(lua_State *L)
{
    struct fstab *fs;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    fs = luab_udata(L, 1, &luab_fstab_type, struct fstab *);
    dp = fs->fs_vfstype;

    return (luab_pushstring(L, dp));
}

/***
 * Get mount options.
 *
 * @function fs_mntops
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = fstab:fs_mntops()
 */
static int
FSTAB_fs_mntops(lua_State *L)
{
    struct fstab *fs;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    fs = luab_udata(L, 1, &luab_fstab_type, struct fstab *);
    dp = fs->fs_mntops;

    return (luab_pushstring(L, dp));
}

/***
 * Get type of mount over (fs_mntops).
 *
 * @function fs_type
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = fstab:fs_type()
 */
static int
FSTAB_fs_type(lua_State *L)
{
    struct fstab *fs;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    fs = luab_udata(L, 1, &luab_fstab_type, struct fstab *);
    dp = fs->fs_type;

    return (luab_pushstring(L, dp));
}

/***
 * Get dump frequency in days.
 *
 * @function fs_freq
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = fstab:fs_freq()
 */
static int
FSTAB_fs_freq(lua_State *L)
{
    struct fstab *fs;
    int data;

    (void)luab_core_checkmaxargs(L, 1);

    fs = luab_udata(L, 1, &luab_fstab_type, struct fstab *);
    data = fs->fs_freq;

    return (luab_pushxinteger(L, data));
}

/***
 * Get pass number on parallel fsck(8).
 *
 * @function fs_passno
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = fstab:fs_passno()
 */
static int
FSTAB_fs_passno(lua_State *L)
{
    struct fstab *fs;
    int data;

    (void)luab_core_checkmaxargs(L, 1);

    fs = luab_udata(L, 1, &luab_fstab_type, struct fstab *);
    data = fs->fs_passno;

    return (luab_pushxinteger(L, data));
}

/*
 * Meta-methods.
 */

static int
FSTAB_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_fstab_type));
}

static int
FSTAB_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_fstab_type));
}

static int
FSTAB_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_fstab_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t fstab_methods[] = {
    LUAB_FUNC("fs_spec",        FSTAB_fs_spec),
    LUAB_FUNC("fs_file",        FSTAB_fs_file),
    LUAB_FUNC("fs_vfstype",     FSTAB_fs_vfstype),
    LUAB_FUNC("fs_mntops",      FSTAB_fs_mntops),
    LUAB_FUNC("fs_type",        FSTAB_fs_type),
    LUAB_FUNC("fs_freq",        FSTAB_fs_freq),
    LUAB_FUNC("fs_passno",      FSTAB_fs_passno),
    LUAB_FUNC("get",            FSTAB_get),
    LUAB_FUNC("dump",           FSTAB_dump),
    LUAB_FUNC("__gc",           FSTAB_gc),
    LUAB_FUNC("__len",          FSTAB_len),
    LUAB_FUNC("__tostring",     FSTAB_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
fstab_create(lua_State *L, void *arg)
{
    return (luab_new_fstab(L, arg));
}

static void
fstab_init(void *ud, void *arg)
{
    luab_udata_init(&luab_fstab_type, ud, arg);
}

static void *
fstab_udata(lua_State *L, int narg)
{
    return (luab_to_fstab(L, narg));
}

static luab_table_t *
fstab_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct fstab *x, *y;
    size_t m, n, sz;

    sz = sizeof(struct fstab);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (struct fstab *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_fstab_type, struct fstab *);
                        (void)memmove(&(x[m]), y, sz);
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
fstab_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct fstab *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (struct fstab *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_fstab_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

luab_module_t luab_fstab_type = {
    .m_cookie   = LUAB_FSTAB_TYPE_ID,
    .m_name     = LUAB_FSTAB_TYPE,
    .m_vec      = fstab_methods,
    .m_create   = fstab_create,
    .m_init     = fstab_init,
    .m_get      = fstab_udata,
    .m_get_tbl  = fstab_checktable,
    .m_set_tbl  = fstab_pushtable,
    .m_sz       = sizeof(luab_fstab_t),
};
