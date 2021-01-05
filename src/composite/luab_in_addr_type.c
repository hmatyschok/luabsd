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

#include <arpa/inet.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_in_addr_type;

/*
 * Interface against
 *
 *  struct in_addr {
 *      in_addr_t s_addr;
 *  };
 */

typedef struct luab_in_addr {
    luab_udata_t    ud_softc;
    struct in_addr  ud_ia;
} luab_in_addr_t;

/*
 * Subr.
 */

static void
in_addr_fillxtable(lua_State *L, int narg, void *arg)
{
    struct in_addr *ia;

    if ((ia = (struct in_addr *)arg) != NULL) {

        luab_setinteger(L, narg, "s_addr", ia->s_addr);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(IN_ADDR)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              s_addr  = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = in_addr:get_table()
 */
static int
IN_ADDR_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IN_ADDR, TYPE, __func__);

    xtp.xtp_fill = in_addr_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate in_addr{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = in_addr:dump()
 */
static int
IN_ADDR_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IN_ADDR, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions.
 */

/***
 * Set IPv4 address.
 *
 * @function set_s_addr
 *
 * @param id                IPv4 address.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage id [, err, msg ] = in_addr:set_s_addr(id)
 */
static int
IN_ADDR_set_s_addr(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct in_addr *ia;
    in_addr_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IN_ADDR, TYPE, __func__);
    m1 = luab_xmod(UINT32, TYPE, __func__);

    ia = luab_udata(L, 1, m0, struct in_addr *);
    x = (in_addr_t)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    ia->s_addr = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get IPv4 address.
 *
 * @function get_s_addr
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage id [, err, msg ] = in_addr:get_s_addr()
 */
static int
IN_ADDR_get_s_addr(lua_State *L)
{
    luab_module_t *m;
    struct in_addr *ia;
    in_addr_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IN_ADDR, TYPE, __func__);
    ia = luab_udata(L, 1, m, struct in_addr *);
    x = ia->s_addr;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
IN_ADDR_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IN_ADDR, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
IN_ADDR_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IN_ADDR, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
IN_ADDR_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IN_ADDR, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t in_addr_methods[] = {
    LUAB_FUNC("set_s_addr",     IN_ADDR_set_s_addr),
    LUAB_FUNC("get_table",      IN_ADDR_get_table),
    LUAB_FUNC("get_s_addr",     IN_ADDR_get_s_addr),
    LUAB_FUNC("dump",           IN_ADDR_dump),
    LUAB_FUNC("__gc",           IN_ADDR_gc),
    LUAB_FUNC("__len",          IN_ADDR_len),
    LUAB_FUNC("__tostring",     IN_ADDR_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
in_addr_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(IN_ADDR, TYPE, __func__);
    return (luab_newudata(L, m, arg));
}

static void
in_addr_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(IN_ADDR, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
in_addr_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(IN_ADDR, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
in_addr_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct in_addr *x, *y;
    size_t i, j;

    m = luab_xmod(IN_ADDR, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct in_addr *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct in_addr *);
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
in_addr_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct in_addr *x;
    size_t i, j, k;

    m = luab_xmod(IN_ADDR, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct in_addr *)tbl->tbl_vec) != NULL) &&
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
in_addr_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(IN_ADDR, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_in_addr_type = {
    .m_id           = LUAB_IN_ADDR_TYPE_ID,
    .m_name         = LUAB_IN_ADDR_TYPE,
    .m_vec          = in_addr_methods,
    .m_create       = in_addr_create,
    .m_init         = in_addr_init,
    .m_get          = in_addr_udata,
    .m_get_tbl      = in_addr_checktable,
    .m_set_tbl      = in_addr_pushtable,
    .m_alloc_tbl    = in_addr_alloctable,
    .m_len          = sizeof(luab_in_addr_t),
    .m_sz           = sizeof(struct in_addr),
};
