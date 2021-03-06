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

#include <netinet/in.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_in6_addr_type;

/*
 * Interface against
 *
 *  struct in6_addr {
 *      union {
 *          uint8_t     __u6_addr8[16];
 *          uint16_t    __u6_addr16[8];
 *          uint32_t    __u6_addr32[4];
 *      } __u6_addr;
 *  };
 *
 * 128-bit IP6 address,
 */

typedef struct luab_in6_addr {
    luab_udata_t    ud_softc;
    struct in6_addr ud_ia;
} luab_in6_addr_t;
#define s6_addr32   __u6_addr.__u6_addr32

/*
 * Subr.
 */

static void
in6_addr_fillxtable(lua_State *L, int narg, void *arg)
{
    struct in6_addr *ia;

    if ((ia = (struct in6_addr *)arg) != NULL) {

        luab_setldata(L, narg, "s6_addr", &ia->s6_addr, sizeof(ia->__u6_addr));
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(IN6_ADDR)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              s6_addr = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = in6_addr:get_table()
 */
static int
IN6_ADDR_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IN6_ADDR, TYPE, __func__);

    xtp.xtp_fill = in6_addr_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate in6_addr{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = in6_addr:dump()
 */
static int
IN6_ADDR_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IN6_ADDR, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions.
 */

/***
 * Copyin IPv6 address.
 *
 * @function set_s6_addr
 *
 * @param arg               LUA_TTABLE(uint32_t) with cardinality of #4.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = in6_addr:set_s6_addr(id)
 */
static int
IN6_ADDR_set_s6_addr(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct in6_addr *ia;
    uint32_t k, v;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IN6_ADDR, TYPE, __func__);
    m1 = luab_xmod(UINT32, TYPE, __func__);

    ia = luab_udata(L, 1, m0, struct in6_addr *);

    (void)luab_checkltable(L, 2, 4);

    luab_table_init(L, 0); /* Traverse through whole table. */

    for (k = 0; lua_next(L, 2) != 0; k++) {         /* XXX DRY */

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            v = (uint32_t)luab_toxinteger(L, -1, m1, luab_env_uint_max);
            ia->s6_addr32[k] = (uint32_t)v;
        } else
            luab_core_argerror(L, 2, NULL, 0, 0, EINVAL);

        lua_pop(L, 1);
    }
    return (luab_pushxinteger(L, luab_env_success));   /* XXX */
}

/***
 * Copyout IPv6 address.
 *
 * @function get_s6_addr
 *
 * @return (LUA_TTABLE) with cardinality of #4 (over uint32_t).
 *
 * @usage t = in6_addr:get_s6_addr()
 */
static int
IN6_ADDR_get_s6_addr(lua_State *L)
{
    luab_module_t *m;
    struct in6_addr *ia;
    int i, j;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IN6_ADDR, TYPE, __func__);
    ia = luab_udata(L, 1, m, struct in6_addr *);

    luab_table_init(L, 1);
                                                /* XXX DRY */
    for (i = 0, j = 1; i < 4; i++, j++)
        luab_rawsetinteger(L, -2, j, ia->s6_addr32[i]);

    lua_pushvalue(L, -1);

    return (luab_table_pusherr(L, errno, 1));
}

/*
 * Metamethods
 */

static int
IN6_ADDR_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IN6_ADDR, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
IN6_ADDR_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IN6_ADDR, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
IN6_ADDR_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IN6_ADDR, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t in6_addr_methods[] = {
    LUAB_FUNC("set_s6_addr",    IN6_ADDR_set_s6_addr),
    LUAB_FUNC("get_table",      IN6_ADDR_get_table),
    LUAB_FUNC("get_s6_addr",    IN6_ADDR_get_s6_addr),
    LUAB_FUNC("dump",           IN6_ADDR_dump),
    LUAB_FUNC("__gc",           IN6_ADDR_gc),
    LUAB_FUNC("__len",          IN6_ADDR_len),
    LUAB_FUNC("__tostring",     IN6_ADDR_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
in6_addr_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(IN6_ADDR, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
in6_addr_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(IN6_ADDR, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
in6_addr_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(IN6_ADDR, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
in6_addr_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct in6_addr *x, *y;
    size_t i, j;

    m = luab_xmod(IN6_ADDR, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct in6_addr *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct in6_addr *);
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
in6_addr_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct in6_addr *x;
    size_t i, j, k;

    m = luab_xmod(IN6_ADDR, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct in6_addr *)tbl->tbl_vec) != NULL) &&
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
in6_addr_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(IN6_ADDR, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_in6_addr_type = {
    .m_id           = LUAB_IN6_ADDR_TYPE_ID,
    .m_name         = LUAB_IN6_ADDR_TYPE,
    .m_vec          = in6_addr_methods,
    .m_create       = in6_addr_create,
    .m_init         = in6_addr_init,
    .m_get          = in6_addr_udata,
    .m_get_tbl      = in6_addr_checktable,
    .m_set_tbl      = in6_addr_pushtable,
    .m_alloc_tbl    = in6_addr_alloctable,
    .m_len          = sizeof(luab_in6_addr_t),
    .m_sz           = sizeof(struct in6_addr),
};
