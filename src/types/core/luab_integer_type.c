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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_integer_type;

/*
 * Interface over subset against
 *
 *  typedef union luab_primitive_u {
 *      char        un_char;
 *      short       un_short;
 *      int         un_int;
 *      long        un_long;
 *      int8_t      un_int8;
 *      int16_t     un_int16;
 *      int32_t     un_int32;
 *      int64_t     un_int64;
 *      uint8_t     un_uint8;
 *      uint16_t    un_uint16;
 *      uint32_t    un_uint32;
 *      uint64_t    un_uint64;
 *      u_int       un_u_int;
 *      u_long      un_u_long;
 *      fpos_t      un_fpos;
 *      gid_t       un_gid;
 *      off_t       un_off;
 *      size_t      un_size;
 *      uid_t       un_uid;
 *      socklen_t   un_socklen;
 *      lua_Integer un_intx;
 *      lua_Number  un_numx;
 *      lua_CFunction   un_fn;
 *      const char  *un_cp;
 *      wchar_t     un_wc;
 *  } luab_primitive_t;
 *
 */

typedef struct luab_integer {
    luab_udata_t        ud_softc;
    luab_primitive_t     ud_x;
} luab_integer_t;

/*
 * Subr.
 */

static void
in_addr_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_primitive_t *xp;

    if ((xp = (luab_primitive_t *)arg) != NULL) {

        luab_setinteger(L, narg, "x", xp->un_intx);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(INTEGER)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              x   = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = integer:get_table()
 */
static int
INTEGER_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INTEGER, TYPE, __func__);

    xtp.xtp_fill = in_addr_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
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
 * @usage iovec [, err, msg ] = integer:dump()
 */
static int
INTEGER_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set integer.
 *
 * @function set_x
 *
 * @param arg               Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = integer:set_x(arg)
 */
static int
INTEGER_set_x(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_primitive_t *xp;
    lua_Integer x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(INTEGER, TYPE, __func__);
    m1 = luab_xmod(LUAL_INTEGER, TYPE, __func__);

    xp = luab_udata(L, 1, m0, luab_primitive_t *);
    x = luab_checklxinteger(L, 2, m1, 0);

    xp->un_intx = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get integer.
 *
 * @function get_x
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = integer:get_x()
 */
static int
INTEGER_get_x(lua_State *L)
{
    luab_module_t *m;
    luab_primitive_t *xp;
    lua_Integer x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INTEGER, TYPE, __func__);
    xp = luab_udata(L, 1, m, luab_primitive_t *);
    x = xp->un_intx;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
INTEGER_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(INTEGER, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
INTEGER_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(INTEGER, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
INTEGER_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(INTEGER, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t integer_methods[] = {
    LUAB_FUNC("set_x",          INTEGER_set_x),
    LUAB_FUNC("get_table",      INTEGER_get_table),
    LUAB_FUNC("get_x",          INTEGER_get_x),
    LUAB_FUNC("dump",           INTEGER_dump),
    LUAB_FUNC("__gc",           INTEGER_gc),
    LUAB_FUNC("__len",          INTEGER_len),
    LUAB_FUNC("__tostring",     INTEGER_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
integer_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(INTEGER, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
integer_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(INTEGER, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
integer_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(INTEGER, TYPE, __func__);
    return (luab_toudata(L, narg, m));
}

static luab_table_t *
integer_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    luab_primitive_t *x, *y;
    size_t i, j;

    m = luab_xmod(INTEGER, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (luab_primitive_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, luab_primitive_t *);
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
integer_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    luab_primitive_t *x;
    size_t i, j, k;

    m = luab_xmod(INTEGER, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (luab_primitive_t *)tbl->tbl_vec) != NULL) &&
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
integer_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(INTEGER, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_integer_type = {
    .m_id           = LUAB_INTEGER_TYPE_ID,
    .m_name         = LUAB_INTEGER_TYPE,
    .m_vec          = integer_methods,
    .m_create       = integer_create,
    .m_init         = integer_init,
    .m_get          = integer_udata,
    .m_get_tbl      = integer_checktable,
    .m_set_tbl      = integer_pushtable,
    .m_alloc_tbl    = integer_alloctable,
    .m_len          = sizeof(luab_integer_t),
    .m_sz           = sizeof(luab_primitive_t),
};
