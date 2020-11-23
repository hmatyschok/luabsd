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
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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

#define luab_new_crypt_data(L, arg) \
    ((luab_crypt_data_t *)luab_newudata(L, &luab_crypt_data_type, (arg)))
#define luab_to_crypt_data(L, narg) \
    (luab_toldata((L), (narg), &luab_crypt_data_type, \
        struct crypt_data *, sizeof(struct crypt_data)))

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(CRYPT_DATA)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              initialized = (LUA_TNUMBER),
 *              buf         = (LUA_TSTRING),
 *          }
 *
 * @usage t = crypt_data:get()
 */
static int
CRYPT_DATA_get(lua_State *L)
{
    struct crypt_data *cd;

    (void)luab_core_checkmaxargs(L, 1);

    cd = luab_udata(L, 1, &luab_crypt_data_type, struct crypt_data *);

    lua_newtable(L);

    luab_setinteger(L, -2, "initialized", cd->initialized);
    luab_setldata(L, -2, "buf", cd->__buf, LUAB_CRYPT_DATAMAXLEN);   /* XXX */

    lua_pushvalue(L, -1);

    return (1);
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
    return (luab_core_dump(L, 1, &luab_crypt_data_type, sizeof(struct crypt_data)));
}

/*
 * Access functions.
 */

/***
 * Set param denotes initializiation.
 *
 * @function set_initialized
 *
 * @param data              Integer.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = crypt_data:set_initialized(data)
 */
static int
CRYPT_DATA_set_initialized(lua_State *L)
{
    struct crypt_data *cd;
    int data;

    (void)luab_core_checkmaxargs(L, 2);

    cd = luab_udata(L, 1, &luab_crypt_data_type, struct crypt_data *);
    data = (int)luab_checkinteger(L, 2, luab_env_int_max);

    cd->initialized = data;

    return (luab_pushxinteger(L, data));
}

/***
 * Get param denotes initializiation.
 *
 * @function get_initialized
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = crypt_data:get_initialized()
 */
static int
CRYPT_DATA_get_initialized(lua_State *L)
{
    struct crypt_data *cd;
    int data;

    (void)luab_core_checkmaxargs(L, 1);

    cd = luab_udata(L, 1, &luab_crypt_data_type, struct crypt_data *);
    data = cd->initialized;

    return (luab_pushxinteger(L, data));
}

/***
 * Copyin.
 *
 * @function copyin
 *
 * @param data              Byte string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = crypt_data:set_buf(data)
 */
static int
CRYPT_DATA_set_buf(lua_State *L)
{
    struct crypt_data *cd;
    const char *buf;
    size_t len;

    (void)luab_core_checkmaxargs(L, 2);

    cd = luab_udata(L, 1, &luab_crypt_data_type, struct crypt_data *);
    buf = luab_checklstring(L, 2, LUAB_CRYPT_DATAMAXLEN, NULL);

    len = strnlen(buf, LUAB_CRYPT_DATAMAXLEN);

    (void)memmove(cd->__buf, buf, len);

    return (luab_pushxinteger(L, 0));
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
    struct crypt_data *cd;
    caddr_t buf;
    size_t len;

    (void)luab_core_checkmaxargs(L, 1);

    cd = luab_udata(L, 1, &luab_crypt_data_type, struct crypt_data *);
    buf = cd->__buf;
    len = strnlen(buf, LUAB_CRYPT_DATAMAXLEN);

    return (luab_pushldata(L, buf, len));
}

/*
 * Metamethods
 */

static int
CRYPT_DATA_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_crypt_data_type));
}

static int
CRYPT_DATA_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_crypt_data_type));
}

static int
CRYPT_DATA_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_crypt_data_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t crypt_data_methods[] = {
    LUAB_FUNC("set_initialized",    CRYPT_DATA_set_initialized),
    LUAB_FUNC("set_buf",            CRYPT_DATA_set_buf),
    LUAB_FUNC("get",                CRYPT_DATA_get),
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
    return (luab_new_crypt_data(L, arg));
}

static void
crypt_data_init(void *ud, void *arg)
{
    luab_udata_init(&luab_crypt_data_type, ud, arg);
}

static void *
crypt_data_udata(lua_State *L, int narg)
{
    return (luab_to_crypt_data(L, narg));
}

static luab_table_t *
crypt_data_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct crypt_data *x, *y;
    size_t m, n, sz;

    sz = sizeof(struct crypt_data);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (struct crypt_data *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_crypt_data_type, struct crypt_data *);
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
crypt_data_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct crypt_data *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (struct crypt_data *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_crypt_data_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

luab_module_t luab_crypt_data_type = {
    .m_cookie   = LUAB_CRYPT_DATA_TYPE_ID,
    .m_name     = LUAB_CRYPT_DATA_TYPE,
    .m_vec      = crypt_data_methods,
    .m_create   = crypt_data_create,
    .m_init     = crypt_data_init,
    .m_get      = crypt_data_udata,
    .m_get_tbl  = crypt_data_checktable,
    .m_set_tbl  = crypt_data_pushtable,
    .m_sz       = sizeof(luab_crypt_data_t),
};
#endif /* __BSD_VISIBLE */
