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

#define luab_new_integer(L, arg) \
    ((luab_integer_t *)luab_newudata(L, &luab_integer_type, (arg)))
#define luab_to_integer(L, narg) \
    ((luab_integer_t *)luab_toudata((L), (narg), &luab_integer_type))

#define LUAB_INTEGER_TYPE_ID    1595975665
#define LUAB_INTEGER_TYPE   "INTEGER*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(INTEGER)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              x   = (LUA_TNUMBER),
 *          }
 *
 * @usage t = integer:get()
 */
static int
INTEGER_get(lua_State *L)
{
    luab_primitive_t *xp;

    (void)luab_core_checkmaxargs(L, 2);

    xp = luab_udata(L, 1, &luab_integer_type, luab_primitive_t *);

    lua_newtable(L);
    luab_setinteger(L, -2, "x", xp->un_intx);
    lua_pushvalue(L, -1);

    return (1);
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
 * @param data              Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = integer:set_x(data)
 */
static int
INTEGER_set_x(lua_State *L)
{
    luab_primitive_t *xp;
    lua_Integer data;

    (void)luab_core_checkmaxargs(L, 2);

    xp = luab_udata(L, 1, &luab_integer_type, luab_primitive_t *);
    data = luab_checklinteger(L, 2, 1);

    xp->un_intx = data;

    return (luab_pushxinteger(L, data));
}

/***
 * Get integer.
 *
 * @function get_x
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = integer:get_x()
 */
static int
INTEGER_get_x(lua_State *L)
{
    luab_primitive_t *xp;
    lua_Integer data;

    (void)luab_core_checkmaxargs(L, 1);

    xp = luab_udata(L, 1, &luab_integer_type, luab_primitive_t *);
    data = xp->un_intx;

    return (luab_pushxinteger(L, data));
}

/*
 * Metamethods.
 */

static int
INTEGER_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_integer_type));
}

static int
INTEGER_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_integer_type));
}

static int
INTEGER_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_integer_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t integer_methods[] = {
    LUAB_FUNC("set_x",          INTEGER_set_x),
    LUAB_FUNC("get",            INTEGER_get),
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
    return (luab_new_integer(L, arg));
}

static void
integer_init(void *ud, void *arg)
{
    luab_udata_init(&luab_integer_type, ud, arg);
}

static void *
integer_udata(lua_State *L, int narg)
{
    return (luab_to_integer(L, narg));
}

static luab_table_t *
integer_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    luab_primitive_t *x, *y;
    size_t m, n, sz;

    sz = sizeof(luab_primitive_t);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (luab_primitive_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_integer_type, luab_primitive_t *);
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
integer_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_primitive_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (luab_primitive_t *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_integer_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

luab_module_t luab_integer_type = {
    .m_cookie   = LUAB_INTEGER_TYPE_ID,
    .m_name     = LUAB_INTEGER_TYPE,
    .m_vec      = integer_methods,
    .m_create   = integer_create,
    .m_init     = integer_init,
    .m_get      = integer_udata,
    .m_get_tbl  = integer_checktable,
    .m_set_tbl  = integer_pushtable,
    .m_sz       = sizeof(luab_integer_t),
};
