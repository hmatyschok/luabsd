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

#define luab_new_sockproto(L, arg) \
    ((luab_sockproto_t *)luab_newudata(L, &luab_sockproto_type, (arg)))
#define luab_to_sockproto(L, narg) \
    (luab_toldata((L), (narg), &luab_sockproto_type, \
        struct sockproto *, luab_sockproto_type.m_sz))

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
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = sockproto_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, &luab_sockproto_type);
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
    return (luab_core_dump(L, 1, &luab_sockproto_type, luab_sockproto_type.m_sz));
}

/*
 * Access functions.
 */

/***
 * Set protocol family.
 *
 * @function set_sp_family
 *
 * @param data              Specifies protocol domain(9) by name, (LUA_TSTRING).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = sockproto:set_sp_family(data)
 */
static int
SOCKPROTO_set_sp_family(lua_State *L)
{
    struct sockproto *sp;
    u_short x;

    (void)luab_core_checkmaxargs(L, 2);

    sp = luab_udata(L, 1, &luab_sockproto_type, struct sockproto *);
    x = (u_short)luab_checkinteger(L, 2, luab_env_shrt_max);
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
 * @usage data [, err, msg ] = sockproto:get_sp_family()
 */
static int
SOCKPROTO_get_sp_family(lua_State *L)
{
    struct sockproto *sp;
    u_short x;

    (void)luab_core_checkmaxargs(L, 1);

    sp = luab_udata(L, 1, &luab_sockproto_type, struct sockproto *);
    x = sp->sp_family;

    return (luab_pushxinteger(L, x));
}

/***
 * Set protocol.
 *
 * @function set_sp_protocol
 *
 * @param data              Specifies accept filter string, (LUA_TSTRING).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = sockproto:set_sp_protocol(data)
 */
static int
SOCKPROTO_set_sp_protocol(lua_State *L)
{
    struct sockproto *sp;
    u_short x;

    (void)luab_core_checkmaxargs(L, 2);

    sp = luab_udata(L, 1, &luab_sockproto_type, struct sockproto *);
    x = (u_short)luab_checkinteger(L, 2, luab_env_shrt_max);
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
 * @usage data [, err, msg ] = sockproto:get_sp_protocol()
 */
static int
SOCKPROTO_get_sp_protocol(lua_State *L)
{
    struct sockproto *sp;
    u_short x;

    (void)luab_core_checkmaxargs(L, 1);

    sp = luab_udata(L, 1, &luab_sockproto_type, struct sockproto *);
    x = sp->sp_protocol;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods
 */

static int
SOCKPROTO_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_sockproto_type));
}

static int
SOCKPROTO_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_sockproto_type));
}

static int
SOCKPROTO_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_sockproto_type));
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
    return (luab_new_sockproto(L, arg));
}

static void
sockproto_init(void *ud, void *arg)
{
    luab_udata_init(&luab_sockproto_type, ud, arg);
}

static void *
sockproto_udata(lua_State *L, int narg)
{
    return (luab_to_sockproto(L, narg));
}

static luab_table_t *
sockproto_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct sockproto *x, *y;
    size_t m, n;

    if ((tbl = luab_table_newvectornil(L, narg, &luab_sockproto_type)) != NULL) {

        if (((x = (struct sockproto *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_sockproto_type, struct sockproto *);
                        (void)memmove(&(x[m]), y, luab_sockproto_type.m_sz);
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
    struct sockproto *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (struct sockproto *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, new);

            for (m = 0, n = (tbl->tbl_card - 1), k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_sockproto_type, k, &(x[m]));

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
    return (luab_table_create(&luab_sockproto_type, vec, card));
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
