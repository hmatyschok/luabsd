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
#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#if __BSD_VISIBLE
extern luab_module_t luab_crypt_data_type;

/*
 * Interface against
 *
 *  struct crypt_data {
 *      int initialized;
 *      char    __buf[256];
 *  };
 */

typedef struct luab_crypt_data {
    luab_udata_t        ud_softc;
    struct crypt_data   ud_data;
} luab_crypt_data_t;

#define LUAB_CRYPT_DATAMAXLEN  256

/*
 * Subr.
 */

static void
crypt_data_fillxtable(lua_State *L, int narg, void *arg)
{
    struct crypt_data *cd;

    if ((cd = (struct crypt_data *)arg) != NULL) {

        luab_setinteger(L, narg, "initialized", cd->initialized);
        luab_setldata(L, narg, "buf", cd->__buf, LUAB_CRYPT_DATAMAXLEN);   /* XXX */
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(CRYPT_DATA)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              initialized = (LUA_TNUMBER),
 *              buf         = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ] = crypt_data:get_table()
 */
static int
CRYPT_DATA_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CRYPT_DATA, TYPE, __func__);

    xtp.xtp_fill = crypt_data_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate crypt_data{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = crypt_data:dump()
 */
static int
CRYPT_DATA_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(CRYPT_DATA, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions.
 */

/***
 * Set param denotes initializiation.
 *
 * @function set_initialized
 *
 * @param arg               Integer.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = crypt_data:set_initialized(arg)
 */
static int
CRYPT_DATA_set_initialized(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct crypt_data *cd;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(CRYPT_DATA, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    cd = luab_udata(L, 1, m0, struct crypt_data *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    cd->initialized = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get param denotes initializiation.
 *
 * @function get_initialized
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = crypt_data:get_initialized()
 */
static int
CRYPT_DATA_get_initialized(lua_State *L)
{
    luab_module_t *m;
    struct crypt_data *cd;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CRYPT_DATA, TYPE, __func__);
    cd = luab_udata(L, 1, m, struct crypt_data *);
    x = cd->initialized;

    return (luab_pushxinteger(L, x));
}

/***
 * Copyin.
 *
 * @function copyin
 *
 * @param arg               Byte string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = crypt_data:set_buf(arg)
 */
static int
CRYPT_DATA_set_buf(lua_State *L)
{
    luab_module_t *m;
    struct crypt_data *cd;
    const char *dp;
    size_t len;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(CRYPT_DATA, TYPE, __func__);
    cd = luab_udata(L, 1, m, struct crypt_data *);
    dp = luab_checklstring(L, 2, LUAB_CRYPT_DATAMAXLEN, NULL);

    if ((len = strnlen(dp, LUAB_CRYPT_DATAMAXLEN)) > 0)
        (void)memmove(cd->__buf, dp, len);

    return (luab_pushxinteger(L, len));
}

/***
 * Copyout.
 *
 * @function get_buf
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [err, msg ] = crypt_data:get_buf()
 */
static int
CRYPT_DATA_get_buf(lua_State *L)
{
    luab_module_t *m;
    struct crypt_data *cd;
    caddr_t dp;
    size_t len;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CRYPT_DATA, TYPE, __func__);
    cd = luab_udata(L, 1, m, struct crypt_data *);

    dp = cd->__buf;
    len = strnlen(dp, LUAB_CRYPT_DATAMAXLEN);

    return (luab_pushldata(L, dp, len));
}

/*
 * Metamethods
 */

static int
CRYPT_DATA_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(CRYPT_DATA, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
CRYPT_DATA_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(CRYPT_DATA, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
CRYPT_DATA_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(CRYPT_DATA, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t crypt_data_methods[] = {
    LUAB_FUNC("set_initialized",    CRYPT_DATA_set_initialized),
    LUAB_FUNC("set_buf",            CRYPT_DATA_set_buf),
    LUAB_FUNC("get_table",          CRYPT_DATA_get_table),
    LUAB_FUNC("get_initialized",    CRYPT_DATA_get_initialized),
    LUAB_FUNC("get_buf",            CRYPT_DATA_get_buf),
    LUAB_FUNC("dump",               CRYPT_DATA_dump),
    LUAB_FUNC("__gc",               CRYPT_DATA_gc),
    LUAB_FUNC("__len",              CRYPT_DATA_len),
    LUAB_FUNC("__tostring",         CRYPT_DATA_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
crypt_data_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(CRYPT_DATA, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
crypt_data_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(CRYPT_DATA, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
crypt_data_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(CRYPT_DATA, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
crypt_data_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct crypt_data *x, *y;
    size_t i, j;

    m = luab_xmod(CRYPT_DATA, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct crypt_data *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct crypt_data *);
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
crypt_data_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct crypt_data *x;
    size_t i, j, k;

    m = luab_xmod(CRYPT_DATA, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct crypt_data *)tbl->tbl_vec) != NULL) &&
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
crypt_data_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(CRYPT_DATA, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_crypt_data_type = {
    .m_id           = LUAB_CRYPT_DATA_TYPE_ID,
    .m_name         = LUAB_CRYPT_DATA_TYPE,
    .m_vec          = crypt_data_methods,
    .m_create       = crypt_data_create,
    .m_init         = crypt_data_init,
    .m_get          = crypt_data_udata,
    .m_get_tbl      = crypt_data_checktable,
    .m_set_tbl      = crypt_data_pushtable,
    .m_alloc_tbl    = crypt_data_alloctable,
    .m_len          = sizeof(luab_crypt_data_t),
    .m_sz           = sizeof(struct crypt_data),
};
#endif /* __BSD_VISIBLE */
