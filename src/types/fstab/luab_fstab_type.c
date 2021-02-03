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
#undef FSTAB    /* XXX */
#include <string.h>

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
 *      char    *fs_spec;
 *      char    *fs_file;
 *      char    *fs_vfstype;
 *      char    *fs_mntops;
 *      char    *fs_type;
 *      int fs_freq;
 *      int fs_passno;
 *  };
 *
 */

typedef struct luab_fstab {
    luab_udata_t    ud_softc;
    struct fstab    ud_fs;
} luab_fstab_t;

/*
 * Subr.
 */

static void
fstab_fillxtable(lua_State *L, int narg, void *arg)
{
    struct fstab *fs;

    if ((fs = (struct fstab *)arg) != NULL) {

        luab_setstring(L, narg, "fs_spec",        fs->fs_spec);
        luab_setstring(L, narg, "fs_file",        fs->fs_file);
        luab_setstring(L, narg, "fs_vfstype",     fs->fs_vfstype);
        luab_setstring(L, narg, "fs_mntops",      fs->fs_mntops);
        luab_setstring(L, narg, "fs_type",        fs->fs_type);
        luab_setinteger(L, narg, "fs_freq",       fs->fs_freq);
        luab_setinteger(L, narg, "fs_passno",     fs->fs_passno);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(FSTAB)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              fs_spec     = (LUA_T{NIL,STRING}),
 *              fs_file     = (LUA_T{NIL,STRING}),
 *              fs_vfstype  = (LUA_T{NIL,STRING}),
 *              fs_mntops   = (LUA_T{NIL,STRING}),
 *              fs_type     = (LUA_T{NIL,STRING}),
 *              fs_freq     = (LUA_TNUMBER),
 *              fs_passno   = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = fstab:get_table()
 */
static int
FSTAB_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FSTAB, TYPE, __func__);

    xtp.xtp_fill = fstab_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
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
    luab_module_t *m;
    m = luab_xmod(FSTAB, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
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
 * @usage x [, err, msg ] = fstab:fs_spec()
 */
static int
FSTAB_fs_spec(lua_State *L)
{
    luab_module_t *m;
    struct fstab *fs;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FSTAB, TYPE, __func__);
    fs = luab_udata(L, 1, m, struct fstab *);
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
 * @usage x [, err, msg ] = fstab:fs_file()
 */
static int
FSTAB_fs_file(lua_State *L)
{
    luab_module_t *m;
    struct fstab *fs;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FSTAB, TYPE, __func__);
    fs = luab_udata(L, 1, m, struct fstab *);
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
 * @usage x [, err, msg ] = fstab:fs_vfstype()
 */
static int
FSTAB_fs_vfstype(lua_State *L)
{
    luab_module_t *m;
    struct fstab *fs;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FSTAB, TYPE, __func__);
    fs = luab_udata(L, 1, m, struct fstab *);
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
 * @usage x [, err, msg ] = fstab:fs_mntops()
 */
static int
FSTAB_fs_mntops(lua_State *L)
{
    luab_module_t *m;
    struct fstab *fs;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FSTAB, TYPE, __func__);
    fs = luab_udata(L, 1, m, struct fstab *);
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
 * @usage x [, err, msg ] = fstab:fs_type()
 */
static int
FSTAB_fs_type(lua_State *L)
{
    luab_module_t *m;
    struct fstab *fs;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FSTAB, TYPE, __func__);
    fs = luab_udata(L, 1, m, struct fstab *);
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
 * @usage x [, err, msg ] = fstab:fs_freq()
 */
static int
FSTAB_fs_freq(lua_State *L)
{
    luab_module_t *m;
    struct fstab *fs;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FSTAB, TYPE, __func__);
    fs = luab_udata(L, 1, m, struct fstab *);
    x = fs->fs_freq;

    return (luab_pushxinteger(L, x));
}

/***
 * Get pass number on parallel fsck(8).
 *
 * @function fs_passno
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = fstab:fs_passno()
 */
static int
FSTAB_fs_passno(lua_State *L)
{
    luab_module_t *m;
    struct fstab *fs;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FSTAB, TYPE, __func__);
    fs = luab_udata(L, 1, m, struct fstab *);
    x = fs->fs_passno;

    return (luab_pushxinteger(L, x));
}

/*
 * Meta-methods.
 */

static int
FSTAB_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FSTAB, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
FSTAB_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FSTAB, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
FSTAB_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FSTAB, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
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
    LUAB_FUNC("get_table",      FSTAB_get_table),
    LUAB_FUNC("dump",           FSTAB_dump),
    LUAB_FUNC("__gc",           FSTAB_gc),
    LUAB_FUNC("__len",          FSTAB_len),
    LUAB_FUNC("__tostring",     FSTAB_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
fstab_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(FSTAB, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
fstab_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(FSTAB, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
fstab_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(FSTAB, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
fstab_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct fstab *x, *y;
    size_t i, j;

    m = luab_xmod(FSTAB, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct fstab *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct fstab *);
                        (void)memmove(&(x[i]), y, m->m_sz);
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        } else
            errno = ERANGE;
    }
    return (tbl);
}

static void
fstab_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct fstab *x;
    size_t i, j, k;

    m = luab_xmod(FSTAB, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct fstab *)tbl->tbl_vec) != NULL) &&
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
fstab_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(FSTAB, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_fstab_type = {
    .m_id           = LUAB_FSTAB_TYPE_ID,
    .m_name         = LUAB_FSTAB_TYPE,
    .m_vec          = fstab_methods,
    .m_create       = fstab_create,
    .m_init         = fstab_init,
    .m_get          = fstab_udata,
    .m_get_tbl      = fstab_checktable,
    .m_set_tbl      = fstab_pushtable,
    .m_alloc_tbl    = fstab_alloctable,
    .m_len          = sizeof(luab_fstab_t),
    .m_sz           = sizeof(struct fstab),
};
