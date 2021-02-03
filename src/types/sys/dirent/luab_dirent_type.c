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
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRENTECT,
 * INDIRENTECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/dirent.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_dirent_type;

/*
 * Interface against
 *
 *  struct dirent {
 *      ino_t      d_fileno;
 *      off_t      d_off;
 *      __uint16_t d_reclen;
 *      __uint8_t  d_type;
 *      __uint8_t  d_pad0;
 *      __uint16_t d_namlen;
 *      __uint16_t d_pad1;
 *  #if __BSD_VISIBLE
 *  #define MAXNAMLEN   255
 *      char    d_name[MAXNAMLEN + 1];
 *  #else
 *      char    d_name[255 + 1];
 *  #endif
 *  };
 *
 */

typedef struct luab_dirent {
    luab_udata_t    ud_softc;
    struct dirent   ud_d;
} luab_dirent_t;

/*
 * Subr.
 */

static void
dirent_fillxtable(lua_State *L, int narg, void *arg)
{
    struct dirent *dp;

    if ((dp = (struct dirent *)arg) != NULL) {

        luab_setinteger(L, narg, "d_fileno",  dp->d_fileno);
        luab_setinteger(L, narg, "d_off",     dp->d_off);
        luab_setinteger(L, narg, "d_reclen",  dp->d_reclen);
        luab_setinteger(L, narg, "d_type",    dp->d_type);
        luab_setinteger(L, narg, "d_namlen",  dp->d_namlen);
        luab_setldata(L, narg, "d_name",      dp->d_name, dp->d_namlen);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(DIRENT)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              d_fileno    = (LUA_TNUMBER),
 *              d_off       = (LUA_TNUMBER),
 *              d_reclen    = (LUA_TNUMBER),
 *              d_type      = (LUA_TNUMBER),
 *              d_namlen    = (LUA_TNUMBER),
 *              d_name      = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ]= dirent:get_table()
 */
static int
DIRENT_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DIRENT, TYPE, __func__);

    xtp.xtp_fill = dirent_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate dirent{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = dirent:dump()
 */
static int
DIRENT_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(DIRENT, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get file number of entry.
 *
 * @function d_fileno
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = dirent:d_fileno()
 */
static int
DIRENT_d_fileno(lua_State *L)
{
    luab_module_t *m;
    struct dirent *dp;
    ino_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DIRENT, TYPE, __func__);
    dp = luab_udata(L, 1, m, struct dirent *);
    x = dp->d_fileno;

    return (luab_pushxinteger(L, x));
}

/***
 * Get directory offset of entry.
 *
 * @function d_off
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = dirent:d_off()
 */
static int
DIRENT_d_off(lua_State *L)
{
    luab_module_t *m;
    struct dirent *dp;
    off_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DIRENT, TYPE, __func__);
    dp = luab_udata(L, 1, m, struct dirent *);
    x = dp->d_off;

    return (luab_pushxinteger(L, x));
}

/***
 * Get length of this record.
 *
 * @function d_reclen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = dirent:d_reclen()
 */
static int
DIRENT_d_reclen(lua_State *L)
{
    luab_module_t *m;
    struct dirent *dp;
    uint16_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DIRENT, TYPE, __func__);
    dp = luab_udata(L, 1, m, struct dirent *);
    x = dp->d_reclen;

    return (luab_pushxinteger(L, x));
}

/***
 * Get file type.
 *
 * @function d_type
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = dirent:d_type()
 */
static int
DIRENT_d_type(lua_State *L)
{
    luab_module_t *m;
    struct dirent *dp;
    uint8_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DIRENT, TYPE, __func__);
    dp = luab_udata(L, 1, m, struct dirent *);
    x = dp->d_type;

    return (luab_pushxinteger(L, x));
}

/***
 * Get length of string in d_name.
 *
 * @function d_namlen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = dirent:d_namlen()
 */
static int
DIRENT_d_namlen(lua_State *L)
{
    luab_module_t *m;
    struct dirent *dp;
    uint16_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DIRENT, TYPE, __func__);
    dp = luab_udata(L, 1, m, struct dirent *);
    x = dp->d_namlen;

    return (luab_pushxinteger(L, x));
}

/***
 * Get directory name.
 *
 * @function d_name
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = dirent:d_name()
 */
static int
DIRENT_d_name(lua_State *L)
{
    luab_module_t *m;
    struct dirent *dp;
    uint16_t len;
    caddr_t bp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(DIRENT, TYPE, __func__);
    dp = luab_udata(L, 1, m, struct dirent *);
    len = dp->d_namlen;
    bp = dp->d_name;

    return (luab_pushldata(L, bp, len));
}

/*
 * Meta-methods.
 */

static int
DIRENT_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(DIRENT, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
DIRENT_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(DIRENT, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
DIRENT_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(DIRENT, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t dirent_methods[] = {
    LUAB_FUNC("d_fileno",       DIRENT_d_fileno),
    LUAB_FUNC("d_off",          DIRENT_d_off),
    LUAB_FUNC("d_reclen",       DIRENT_d_reclen),
    LUAB_FUNC("d_type",         DIRENT_d_type),
    LUAB_FUNC("d_namlen",       DIRENT_d_namlen),
    LUAB_FUNC("d_name",         DIRENT_d_name),
    LUAB_FUNC("get_table",      DIRENT_get_table),
    LUAB_FUNC("dump",           DIRENT_dump),
    LUAB_FUNC("__gc",           DIRENT_gc),
    LUAB_FUNC("__len",          DIRENT_len),
    LUAB_FUNC("__tostring",     DIRENT_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
dirent_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(DIRENT, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
dirent_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(DIRENT, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
dirent_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(DIRENT, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
dirent_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct dirent *x, *y;
    size_t i, j;

    m = luab_xmod(DIRENT, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct dirent *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct dirent *);
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
dirent_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct dirent *x;
    size_t i, j, k;

    m = luab_xmod(DIRENT, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct dirent *)tbl->tbl_vec) != NULL) &&
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
dirent_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(DIRENT, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_dirent_type = {
    .m_id           = LUAB_DIRENT_TYPE_ID,
    .m_name         = LUAB_DIRENT_TYPE,
    .m_vec          = dirent_methods,
    .m_create       = dirent_create,
    .m_init         = dirent_init,
    .m_get          = dirent_udata,
    .m_get_tbl      = dirent_checktable,
    .m_set_tbl      = dirent_pushtable,
    .m_alloc_tbl    = dirent_alloctable,
    .m_len          = sizeof(luab_dirent_t),
    .m_sz           = sizeof(struct dirent),
};
