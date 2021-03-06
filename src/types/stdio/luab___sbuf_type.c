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

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab___sbuf_type;

/*
 * Interface against
 *
 *  struct __sbuf {
 *      unsigned char *_base;
 *      int _size;
 *  };
 *
 */

typedef struct luab___sbuf {
    luab_udata_t        ud_softc;
    struct __sbuf       ud_sb;
} luab___sbuf_type_t;

/*
 * Subr.
 */

static void
sbuf_fillxtable(lua_State *L, int narg, void *arg)
{
    struct __sbuf *sb;

    if ((sb = (struct __sbuf *)arg) != NULL) {

        luab_setldata(L, narg, "_base",      sb->_base, sb->_size);
        luab_setinteger(L, narg, "_size",    sb->_size);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(__SBUF)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              _base       = (LUA_T{NIL,STRING}),
 *              _size       = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = __sbuf:get_table()
 */
static int
SBUF_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(__SBUF, TYPE, __func__);

    xtp.xtp_fill = sbuf_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate __sbuf{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = __sbuf:dump()
 */
static int
SBUF_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(__SBUF, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get contents of data region _base.
 *
 * @function _base
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = __sbuf:_base()
 */
static int
SBUF_base(lua_State *L)
{
    luab_module_t *m;
    struct __sbuf *sb;
    caddr_t dp;
    size_t sz;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(__SBUF, TYPE, __func__);

    sb = luab_udata(L, 1, m, struct __sbuf *);
    dp = sb->_base;
    sz = sb->_size;

    return (luab_pushldata(L, dp, sz));
}

/***
 * Get size of data region.
 *
 * @function _size
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = __sbuf:_size()
 */
static int
SBUF_size(lua_State *L)
{
    luab_module_t *m;
    struct __sbuf *sb;
    size_t sz;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(__SBUF, TYPE, __func__);
    sb = luab_udata(L, 1, m, struct __sbuf *);
    sz = sb->_size;

    return (luab_pushxinteger(L, sz));
}

/*
 * Meta-methods.
 */

static int
SBUF_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(__SBUF, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
SBUF_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(__SBUF, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
SBUF_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(__SBUF, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t sbuf_methods[] = {
    LUAB_FUNC("_base",          SBUF_base),
    LUAB_FUNC("_size",          SBUF_size),
    LUAB_FUNC("get_table",      SBUF_get_table),
    LUAB_FUNC("dump",           SBUF_dump),
    LUAB_FUNC("__gc",           SBUF_gc),
    LUAB_FUNC("__len",          SBUF_len),
    LUAB_FUNC("__tostring",     SBUF_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
sbuf_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(__SBUF, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
sbuf_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(__SBUF, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
sbuf_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(__SBUF, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
sbuf_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct __sbuf *x, *y;
    size_t i, j;

    m = luab_xmod(__SBUF, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct __sbuf *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct __sbuf *);
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
sbuf_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct __sbuf *x;
    size_t i, j, k;

    m = luab_xmod(__SBUF, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct __sbuf *)tbl->tbl_vec) != NULL) &&
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
sbuf_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(__SBUF, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab___sbuf_type = {
    .m_id           = LUAB___SBUF_TYPE_ID,
    .m_name         = LUAB___SBUF_TYPE,
    .m_vec          = sbuf_methods,
    .m_create       = sbuf_create,
    .m_init         = sbuf_init,
    .m_get          = sbuf_udata,
    .m_get_tbl      = sbuf_checktable,
    .m_set_tbl      = sbuf_pushtable,
    .m_alloc_tbl    = sbuf_alloctable,
    .m_len          = sizeof(luab___sbuf_type_t),
    .m_sz           = sizeof(struct __sbuf),
};
