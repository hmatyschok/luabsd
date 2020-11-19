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

#define luab_new_dirent(L, arg) \
    ((luab_dirent_t *)luab_newudata(L, &luab_dirent_type, (arg)))
#define luab_to_dirent(L, narg) \
    (luab_toldata((L), (narg), &luab_dirent_type, \
        struct dirent *, sizeof(struct dirent)))

#define LUAB_DIRENT_TYPE_ID    1604795865
#define LUAB_DIRENT_TYPE    "DIRENT*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(DIRENT)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              d_fileno    = (LUA_TNUMBER),
 *              d_off       = (LUA_TNUMBER),
 *              d_reclen    = (LUA_TNUMBER),
 *              d_type      = (LUA_TNUMBER),
 *              d_namlen    = (LUA_TNUMBER),
 *              d_name      = (LUA_TSTRING),
 *          }
 *
 * @usage t = dirent:get()
 */
static int
DIRENT_get(lua_State *L)
{
    struct dirent *dp;

    (void)luab_core_checkmaxargs(L, 1);

    dp = luab_udata(L, 1, &luab_dirent_type, struct dirent *);

    lua_newtable(L);
    luab_setinteger(L, -2, "d_fileno",  dp->d_fileno);
    luab_setinteger(L, -2, "d_off",     dp->d_off);
    luab_setinteger(L, -2, "d_reclen",  dp->d_reclen);
    luab_setinteger(L, -2, "d_type",    dp->d_type);
    luab_setinteger(L, -2, "d_namlen",  dp->d_namlen);
    luab_setldata(L, -2, "d_name",      dp->d_name, dp->d_namlen);
    lua_pushvalue(L, -1);

    return (1);
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
    return (luab_core_dump(L, 1, &luab_dirent_type, sizeof(void *)));
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
 * @usage data [, err, msg ] = dirent:d_fileno()
 */
static int
DIRENT_d_fileno(lua_State *L)
{
    struct dirent *dp;
    ino_t data;

    (void)luab_core_checkmaxargs(L, 1);

    dp = luab_udata(L, 1, &luab_dirent_type, struct dirent *);
    data = dp->d_fileno;

    return (luab_pushxinteger(L, data));
}

/***
 * Get directory offset of entry.
 *
 * @function d_off
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = dirent:d_off()
 */
static int
DIRENT_d_off(lua_State *L)
{
    struct dirent *dp;
    off_t data;

    (void)luab_core_checkmaxargs(L, 1);

    dp = luab_udata(L, 1, &luab_dirent_type, struct dirent *);
    data = dp->d_off;

    return (luab_pushxinteger(L, data));
}

/***
 * Get length of this record.
 *
 * @function d_reclen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = dirent:d_reclen()
 */
static int
DIRENT_d_reclen(lua_State *L)
{
    struct dirent *dp;
    uint16_t data;

    (void)luab_core_checkmaxargs(L, 1);

    dp = luab_udata(L, 1, &luab_dirent_type, struct dirent *);
    data = dp->d_reclen;

    return (luab_pushxinteger(L, data));
}

/***
 * Get file type.
 *
 * @function d_type
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = dirent:d_type()
 */
static int
DIRENT_d_type(lua_State *L)
{
    struct dirent *dp;
    uint8_t data;

    (void)luab_core_checkmaxargs(L, 1);

    dp = luab_udata(L, 1, &luab_dirent_type, struct dirent *);
    data = dp->d_type;

    return (luab_pushxinteger(L, data));
}

/***
 * Get length of string in d_name.
 *
 * @function d_namlen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = dirent:d_namlen()
 */
static int
DIRENT_d_namlen(lua_State *L)
{
    struct dirent *dp;
    uint16_t data;

    (void)luab_core_checkmaxargs(L, 1);

    dp = luab_udata(L, 1, &luab_dirent_type, struct dirent *);
    data = dp->d_namlen;

    return (luab_pushxinteger(L, data));
}

/***
 * Get directory name.
 *
 * @function d_name
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = dirent:d_name()
 */
static int
DIRENT_d_name(lua_State *L)
{
    struct dirent *dp;
    uint16_t len;
    caddr_t bp;

    (void)luab_core_checkmaxargs(L, 1);

    dp = luab_udata(L, 1, &luab_dirent_type, struct dirent *);
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
    return (luab_core_gc(L, 1, &luab_dirent_type));
}

static int
DIRENT_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_dirent_type));
}

static int
DIRENT_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_dirent_type));
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
    LUAB_FUNC("get",            DIRENT_get),
    LUAB_FUNC("dump",           DIRENT_dump),
    LUAB_FUNC("__gc",           DIRENT_gc),
    LUAB_FUNC("__len",          DIRENT_len),
    LUAB_FUNC("__tostring",     DIRENT_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
dirent_create(lua_State *L, void *arg)
{
    return (luab_new_dirent(L, arg));
}

static void
dirent_init(void *ud, void *arg)
{
    luab_udata_init(&luab_dirent_type, ud, arg);
}

static void *
dirent_udata(lua_State *L, int narg)
{
    return (luab_to_dirent(L, narg));
}

static luab_table_t *
dirent_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct dirent *x, *y;
    size_t m, n, sz;

    sz = sizeof(struct dirent);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (struct dirent *)(tbl->tbl_vec)) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_dirent_type, struct dirent *);
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
dirent_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct dirent *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_dirent_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

luab_module_t luab_dirent_type = {
    .m_cookie   = LUAB_DIRENT_TYPE_ID,
    .m_name     = LUAB_DIRENT_TYPE,
    .m_vec      = dirent_methods,
    .m_create   = dirent_create,
    .m_init     = dirent_init,
    .m_get      = dirent_udata,
    .m_get_tbl  = dirent_checktable,
    .m_set_tbl  = dirent_pushtable,
    .m_sz       = sizeof(luab_dirent_t),
};
