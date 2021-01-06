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

#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#if __BSD_VISIBLE
extern luab_module_t luab_sockproto_type;

/*
 * Interface against
 *
 *  struct sockproto {
 *      unsigned short  sp_family;
 *      unsigned short  sp_protocol;
 *  };
 */

typedef struct luab_sockproto {
    luab_udata_t        ud_softc;
    struct sockproto    ud_sp;
} luab_sockproto_t;

/*
 * Subr.
 */

static void
sockproto_fillxtable(lua_State *L, int narg, void *arg)
{
    struct sockproto *sp;

    if ((sp = (struct sockproto *)arg) != NULL) {

        luab_setinteger(L, narg, "sp_family",     sp->sp_family);
        luab_setinteger(L, narg, "sp_protocol",   sp->sp_protocol);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SOCKPROTO)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              sp_family   = (LUA_T{NIL,STRING}),
 *              sp_protocol = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ]= sockproto:get_table()
 */
static int
SOCKPROTO_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SOCKPROTO, TYPE, __func__);

    xtp.xtp_fill = sockproto_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate sockproto{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = sockproto:dump()
 */
static int
SOCKPROTO_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SOCKPROTO, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions.
 */

/***
 * Set protocol family.
 *
 * @function set_sp_family
 *
 * @param arg               Specifies protocol domain(9) by name, (LUA_TSTRING).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sockproto:set_sp_family(arg)
 */
static int
SOCKPROTO_set_sp_family(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct sockproto *sp;
    u_short x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SOCKPROTO, TYPE, __func__);
    m1 = luab_xmod(USHRT, TYPE, __func__);

    sp = luab_udata(L, 1, m0, struct sockproto *);
    x = (u_short)luab_checkxinteger(L, 2, m1, luab_env_ushrt_max);
    sp->sp_family = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get protocol family.
 *
 * @function get_sp_family
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sockproto:get_sp_family()
 */
static int
SOCKPROTO_get_sp_family(lua_State *L)
{
    luab_module_t *m;
    struct sockproto *sp;
    u_short x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SOCKPROTO, TYPE, __func__);
    sp = luab_udata(L, 1, m, struct sockproto *);
    x = sp->sp_family;

    return (luab_pushxinteger(L, x));
}

/***
 * Set protocol.
 *
 * @function set_sp_protocol
 *
 * @param arg               Specifies accept filter string, (LUA_TSTRING).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sockproto:set_sp_protocol(arg)
 */
static int
SOCKPROTO_set_sp_protocol(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct sockproto *sp;
    u_short x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SOCKPROTO, TYPE, __func__);
    m1 = luab_xmod(USHRT, TYPE, __func__);

    sp = luab_udata(L, 1, m0, struct sockproto *);
    x = (u_short)luab_checkxinteger(L, 2, m1, luab_env_ushrt_max);
    sp->sp_protocol = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get protocol.
 *
 * @function get_sp_protocol
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sockproto:get_sp_protocol()
 */
static int
SOCKPROTO_get_sp_protocol(lua_State *L)
{
    luab_module_t *m;
    struct sockproto *sp;
    u_short x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SOCKPROTO, TYPE, __func__);
    sp = luab_udata(L, 1, m, struct sockproto *);
    x = sp->sp_protocol;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods
 */

static int
SOCKPROTO_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SOCKPROTO, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
SOCKPROTO_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SOCKPROTO, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
SOCKPROTO_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SOCKPROTO, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t sockproto_methods[] = {
    LUAB_FUNC("set_sp_family",      SOCKPROTO_set_sp_family),
    LUAB_FUNC("set_sp_protocol",    SOCKPROTO_set_sp_protocol),
    LUAB_FUNC("get_table",          SOCKPROTO_get_table),
    LUAB_FUNC("get_sp_family",      SOCKPROTO_get_sp_family),
    LUAB_FUNC("get_sp_protocol",    SOCKPROTO_get_sp_protocol),
    LUAB_FUNC("dump",               SOCKPROTO_dump),
    LUAB_FUNC("__gc",               SOCKPROTO_gc),
    LUAB_FUNC("__len",              SOCKPROTO_len),
    LUAB_FUNC("__tostring",         SOCKPROTO_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
sockproto_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SOCKPROTO, TYPE, __func__);
    return (luab_newudata(L, m, arg));
}

static void
sockproto_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SOCKPROTO, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
sockproto_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(SOCKPROTO, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
sockproto_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct sockproto *x, *y;
    size_t i, j;

    m = luab_xmod(SOCKPROTO, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct sockproto *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct sockproto *);
                        (void)memmove(&(x[i]), y, m->m_sz);
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
sockproto_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct sockproto *x;
    size_t i, j, k;

    m = luab_xmod(SOCKPROTO, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct sockproto *)tbl->tbl_vec) != NULL) &&
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
sockproto_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(SOCKPROTO, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_sockproto_type = {
    .m_id           = LUAB_SOCKPROTO_TYPE_ID,
    .m_name         = LUAB_SOCKPROTO_TYPE,
    .m_vec          = sockproto_methods,
    .m_create       = sockproto_create,
    .m_init         = sockproto_init,
    .m_get          = sockproto_udata,
    .m_get_tbl      = sockproto_checktable,
    .m_set_tbl      = sockproto_pushtable,
    .m_alloc_tbl    = sockproto_alloctable,
    .m_len          = sizeof(luab_sockproto_t),
    .m_sz           = sizeof(struct sockproto),
};
#endif /* __BSD_VISBLE */
