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

#include <db.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#if __BSD_VISIBLE
extern luab_module_t luab_dbt_type;

/*
 * Interface against
 *
 *  typedef struct {
 *      void    *data;
 *      size_t   size;
 *  } DBT;
 */

typedef struct luab_dbt {
    luab_udata_t    ud_softc;
    DBT             ud_dbt;
} luab_dbt_t;

#define luab_new_dbt(L, arg) \
    ((luab_dbt_t *)luab_newudata(L, &luab_dbt_type, (arg)))
#define luab_to_dbt(L, narg) \
    ((DBT *)luab_toudata((L), (narg), &luab_dbt_type))

/*
 * Subr.
 */

static void
dbt_fillxtable(lua_State *L, int narg, void *arg)
{
    DBT *dbt;

    if ((dbt = (DBT *)arg) != NULL) {

        luab_iovec_setldata(L, narg, "data",    dbt->data, dbt->size);
        luab_setinteger(L, narg, "size",  dbt->size);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(DBT)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              data    = (LUA_T{NIL,STRING}),
 *              size    = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = dbt:get_table()
 */
static int
DBT_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = dbt_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, &luab_dbt_type);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - returns (LUA_TNIL).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = cmsgcred:dump()
 */
static int
DBT_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions for immutable properties.
 */

static int
DBT_get_size(lua_State *L)
{
    DBT *dbt;
    size_t len;

    (void)luab_core_checkmaxargs(L, 1);

    dbt = luab_udata(L, 1, &luab_dbt_type, DBT *);
    len = dbt->size;

    return (luab_pushxinteger(L, len));
}

/*
 * Access functions.
 */

/***
 * Bind data-source.
 *
 * @function set_data
 *
 * @param buf               Instance of (LUA_TUSERDATA(IOVEC)), holds byte string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = dbt:set_data(buf)
 */
static int
DBT_set_data(lua_State *L)
{
    DBT *dbt;
    luab_iovec_t *buf;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    dbt = luab_udata(L, 1, &luab_dbt_type, DBT *);
    buf = luab_udata(L, 2, luab_xmod(IOVEC, TYPE, __func__), luab_iovec_t *);

    if ((buf->iov_flags & IOV_LOCK) == 0) {
        buf->iov_flags |= IOV_LOCK;

    /* XXX race-condition with lua-gc */

        if (((dbt->data = buf->iov.iov_base) != NULL) &&
            ((dbt->size = buf->iov.iov_len) > 0) &&
            (buf->iov_flags & (IOV_BUFF|IOV_PROXY)))
            status = luab_env_success;
        else {
            errno = EINVAL;
            status = luab_env_error;
        }
        buf->iov_flags &= ~IOV_LOCK;
    } else {
        errno = EBUSY;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

static int
DBT_get_data(lua_State *L)
{
    DBT *dbt;
    luab_iovec_t *buf;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    dbt = luab_udata(L, 1, &luab_dbt_type, DBT *);
    buf = luab_udata(L, 2, luab_xmod(IOVEC, TYPE, __func__), luab_iovec_t *);
    status = luab_iovec_copyin(buf, dbt->data, dbt->size);
    return (luab_pushxinteger(L, status));
}

/*
 * Metamethods.
 */

static int
DBT_gc(lua_State *L)
{
    DBT *dbt;

    (void)luab_core_checkmaxargs(L, 1);

    dbt = luab_to_dbt(L, 1);

    if (dbt->data != NULL) {
        dbt->data = NULL;
        dbt->size = 0;
    }
    return (0);
}

static int
DBT_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_dbt_type));
}

static int
DBT_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_dbt_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t dbt_methods[] = {
    LUAB_FUNC("set_data",       DBT_set_data),
    LUAB_FUNC("get_table",      DBT_get_table),
    LUAB_FUNC("get_data",       DBT_get_data),
    LUAB_FUNC("get_size",       DBT_get_size),
    LUAB_FUNC("dump",           DBT_dump),
    LUAB_FUNC("__gc",           DBT_gc),
    LUAB_FUNC("__len",          DBT_len),
    LUAB_FUNC("__tostring",     DBT_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
dbt_create(lua_State *L, void *arg)
{
    return (luab_new_dbt(L, arg));
}

static void
dbt_init(void *ud, void *arg)
{
    luab_dbt_t *self;
    luab_iovec_t *buf;

    if (((self = (luab_dbt_t *)ud) != NULL) &&
        ((buf = (luab_iovec_t *)arg) != NULL)) {

        if (((buf->iov_flags & IOV_LOCK) == 0) &&
            (buf->iov_flags & (IOV_BUFF|IOV_PROXY)) &&
            (buf->iov.iov_base != NULL) &&
            (buf->iov.iov_len > 0)) {
            buf->iov_flags |= IOV_LOCK;

            self->ud_dbt.data = buf->iov.iov_base;
            self->ud_dbt.size = buf->iov.iov_len;

            buf->iov_flags &= ~IOV_LOCK;
        }
    }
}

static void *
dbt_udata(lua_State *L, int narg)
{
    return (luab_to_dbt(L, narg));
}

static luab_table_t *
dbt_alloctable(void *vec, size_t card)
{
    return (luab_table_create(&luab_dbt_type, vec, card));
}

luab_module_t luab_dbt_type = {
    .m_id           = LUAB_DBT_TYPE_ID,
    .m_name         = LUAB_DBT_TYPE,
    .m_vec          = dbt_methods,
    .m_create       = dbt_create,
    .m_init         = dbt_init,
    .m_get          = dbt_udata,
    .m_alloc_tbl    = dbt_alloctable,
    .m_len          = sizeof(luab_dbt_t),
    .m_sz           = sizeof(DBT),
};
#endif /* __BSD_VISIBLE */
