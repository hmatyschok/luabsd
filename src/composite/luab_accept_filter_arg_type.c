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

#ifdef __BSD_VISIBLE
extern luab_module_t luab_accept_filter_arg_type;

/*
 * Interface against
 *
 *  struct accept_filter_arg {
 *      char    af_name[16];
 *      char    af_arg[256-16];
 *  };
 */

typedef struct luab_accept_filter_arg {
    luab_udata_t                ud_softc;
    struct accept_filter_arg    ud_af;
} luab_accept_filter_arg_t;

#define LUAB_AF_NAMEMAXLEN  16
#define LUAB_AF_ARGMAXLEN   240

#define luab_new_accept_filter_arg(L, arg) \
    ((luab_accept_filter_arg_t *)luab_newudata(L, &luab_accept_filter_arg_type, (arg)))
#define luab_to_accept_filter_arg(L, narg) \
    (luab_toldata((L), (narg), &luab_accept_filter_arg_type, \
        struct accept_filter_arg *, luab_accept_filter_arg_type.m_sz))

/*
 * Subr.
 */

static void
accept_filter_arg_fillxtable(lua_State *L, int narg, void *arg)
{
    struct accept_filter_arg *af;

    if ((af = (struct accept_filter_arg *)arg) != NULL) {

        luab_setldata(L, narg, "af_name", af->af_name, strlen(af->af_name));
        luab_setldata(L, narg, "af_arg",  af->af_arg, strlen(af->af_arg));
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(ACCEPT_FILTER_ARG)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              af_name    = (LUA_T{NIL,STRING}),
 *              af_arg     = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ] = accept_filter_arg:get_table()
 */
static int
ACCEPT_FILTER_ARG_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = accept_filter_arg_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, &luab_accept_filter_arg_type);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate accept_filter_arg{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = accept_filter_arg:dump()
 */
static int
ACCEPT_FILTER_ARG_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_accept_filter_arg_type, luab_accept_filter_arg_type.m_sz));
}

/*
 * Access functions.
 */

/***
 * Copy protocol domain(9) name into data region.
 *
 * @function set_af_name
 *
 * @param data              Specifies protocol domain(9) by name, (LUA_TSTRING).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = accept_filter_arg:set_af_name(data)
 */
static int
ACCEPT_FILTER_ARG_set_af_name(lua_State *L)
{
    struct accept_filter_arg *af;
    const char *dp;
    size_t len;

    (void)luab_core_checkmaxargs(L, 2);

    af = luab_udata(L, 1, &luab_accept_filter_arg_type, struct accept_filter_arg *);
    dp = luab_checklstring(L, 2, LUAB_AF_NAMEMAXLEN, NULL);
    len = strlen(dp);
    (void)memmove(af->af_name, dp, len);
    return (luab_pushldata(L, af->af_name, len));
}

/***
 * Copy name of protocol domain(9) from data region.
 *
 * @function get_af_name
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = accept_filter_arg:get_af_name()
 */
static int
ACCEPT_FILTER_ARG_get_af_name(lua_State *L)
{
    struct accept_filter_arg *af;
    caddr_t dp;
    size_t len;

    (void)luab_core_checkmaxargs(L, 1);

    af = luab_udata(L, 1, &luab_accept_filter_arg_type, struct accept_filter_arg *);
    dp = af->af_name;
    len = strlen(dp);

    return (luab_pushldata(L, dp, len));
}

/***
 * Copy accept filter string into data region.
 *
 * @function set_af_arg
 *
 * @param data              Specifies accept filter string, (LUA_TSTRING).
 *
 * @return (LUA_T{NIL,NUMBER} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = accept_filter_arg:set_af_name(data)
 */
static int
ACCEPT_FILTER_ARG_set_af_arg(lua_State *L)
{
    struct accept_filter_arg *af;
    const char *dp;
    size_t len;

    (void)luab_core_checkmaxargs(L, 2);

    af = luab_udata(L, 1, &luab_accept_filter_arg_type, struct accept_filter_arg *);
    dp = luab_checklstring(L, 2, LUAB_AF_ARGMAXLEN, NULL);
    len = strlen(dp);
    (void)memmove(af->af_arg, dp, len);
    return (luab_pushldata(L, af->af_arg, len));
}

/***
 * Copy accept filter string from data region.
 *
 * @function get_af_name
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = accept_filter_arg:get_af_name()
 */
static int
ACCEPT_FILTER_ARG_get_af_arg(lua_State *L)
{
    struct accept_filter_arg *af;
    caddr_t dp;
    size_t len;

    (void)luab_core_checkmaxargs(L, 1);

    af = luab_udata(L, 1, &luab_accept_filter_arg_type, struct accept_filter_arg *);
    dp = af->af_arg;
    len = strlen(dp);

    return (luab_pushldata(L, dp, len));
}

/*
 * Metamethods
 */

static int
ACCEPT_FILTER_ARG_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_accept_filter_arg_type));
}

static int
ACCEPT_FILTER_ARG_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_accept_filter_arg_type));
}

static int
ACCEPT_FILTER_ARG_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_accept_filter_arg_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t accept_filter_arg_methods[] = {
    LUAB_FUNC("set_af_name",    ACCEPT_FILTER_ARG_set_af_name),
    LUAB_FUNC("set_af_arg",     ACCEPT_FILTER_ARG_set_af_arg),
    LUAB_FUNC("get_table",      ACCEPT_FILTER_ARG_get_table),
    LUAB_FUNC("get_af_name",    ACCEPT_FILTER_ARG_get_af_name),
    LUAB_FUNC("get_af_arg",     ACCEPT_FILTER_ARG_get_af_arg),
    LUAB_FUNC("dump",           ACCEPT_FILTER_ARG_dump),
    LUAB_FUNC("__gc",           ACCEPT_FILTER_ARG_gc),
    LUAB_FUNC("__len",          ACCEPT_FILTER_ARG_len),
    LUAB_FUNC("__tostring",     ACCEPT_FILTER_ARG_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
accept_filter_arg_create(lua_State *L, void *arg)
{
    return (luab_new_accept_filter_arg(L, arg));
}

static void
accept_filter_arg_init(void *ud, void *arg)
{
    luab_udata_init(&luab_accept_filter_arg_type, ud, arg);
}

static void *
accept_filter_arg_udata(lua_State *L, int narg)
{
    return (luab_to_accept_filter_arg(L, narg));
}

static luab_table_t *
accept_filter_arg_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct accept_filter_arg *x, *y;
    size_t m, n;

    if ((tbl = luab_table_newvectornil(L, narg, &luab_accept_filter_arg_type)) != NULL) {

        if (((x = (struct accept_filter_arg *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_accept_filter_arg_type, struct accept_filter_arg *);
                        (void)memmove(&(x[m]), y, luab_accept_filter_arg_type.m_sz);
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
accept_filter_arg_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct accept_filter_arg *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (struct accept_filter_arg *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_accept_filter_arg_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

static luab_table_t *
accept_filter_arg_alloctable(void *vec, size_t card)
{
    return (luab_table_create(&luab_accept_filter_arg_type, vec, card));
}

luab_module_t luab_accept_filter_arg_type = {
    .m_id           = LUAB_ACCEPT_FILTER_ARG_TYPE_ID,
    .m_name         = LUAB_ACCEPT_FILTER_ARG_TYPE,
    .m_vec          = accept_filter_arg_methods,
    .m_create       = accept_filter_arg_create,
    .m_init         = accept_filter_arg_init,
    .m_get          = accept_filter_arg_udata,
    .m_get_tbl      = accept_filter_arg_checktable,
    .m_set_tbl      = accept_filter_arg_pushtable,
    .m_alloc_tbl    = accept_filter_arg_alloctable,
    .m_len          = sizeof(luab_accept_filter_arg_t),
    .m_sz           = sizeof(struct accept_filter_arg),
};
#endif /* __BSD_VISIBLE */
