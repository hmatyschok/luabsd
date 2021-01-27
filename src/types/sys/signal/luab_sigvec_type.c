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

#if  __XSI_VISIBLE
extern luab_module_t luab_sigvec_type;

/*
 * Interface against
 *
 *  struct sigvec {
 *      __sighandler_t *sv_handler;
 *      int	sv_mask;
 *      int	sv_flags;
 *  };
 *
 * Signal vector "template" used in sigvec call.
 */

typedef struct luab_sigvec {
    luab_udata_t    ud_softc;
    struct sigvec   ud_sv;
} luab_sigvec_t;

/*
 * Subr.
 */

static void
sigvec_fillxtable(lua_State *L, int narg, void *arg)
{
    struct sigvec *sv;

    if ((sv = (struct sigvec *)arg) != NULL) {
        luab_setfstring(L, narg, "sv_handler", "(%p)",  sv->sv_handler);
        luab_setinteger(L, narg, "sv_mask",             sv->sv_mask);
        luab_setinteger(L, narg, "sv_flags",            sv->sv_flags);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SIGVEC)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              sv_handler  = (LUA_T{NIL,STRING}),
 *              sv_mask     = (LUA_TNUMBER),
 *              sv_flags    = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = sigvec:get_table()
 */
static int
SIGVEC_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGVEC, TYPE, __func__);

    xtp.xtp_fill = sigvec_fillxtable;
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
 * @usage iovec [, err, msg ] = sigvec:dump()
 */
static int
SIGVEC_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get signal sigvec base over (struct sigvec).
 *
 * @function sv_handler
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigvec:sv_handler()
 */
static int
SIGVEC_sv_handler(lua_State *L)
{
    luab_module_t *m;
    struct sigvec *sv;
    __sighandler_t *dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGVEC, TYPE, __func__);
    sv = luab_udata(L, 1, m, struct sigvec *);
    dp = sv->sv_handler;

    return (luab_pushfstring(L, "(%s)", dp));
}

/*
 * Access functions.
 */

/***
 * Set signal sigvec size over (struct sigvec).
 *
 * @function set_sv_mask
 *
 * @param arg               Size, (LUA_T{NUMBER,USERDATA(SIZE)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigvec:set_sv_mask()
 */
static int
SIGVEC_set_sv_mask(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct sigvec *sv;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SIGVEC, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    sv = luab_udata(L, 1, m0, struct sigvec *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);
    sv->sv_mask = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get signal sigvec size over (struct sigvec).
 *
 * @function get_sv_mask
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigvec:get_sv_mask()
 */
static int
SIGVEC_get_sv_mask(lua_State *L)
{
    luab_module_t *m;
    struct sigvec *sv;
    size_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGVEC, TYPE, __func__);
    sv = luab_udata(L, 1, m, struct sigvec *);
    x = sv->sv_mask;

    return (luab_pushxinteger(L, x));
}

/***
 * Set flags over (struct sigvec).
 *
 * @function set_sv_flags
 *
 * @param arg               Flags argument, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigvec:set_sv_flags(arg)
 */
static int
SIGVEC_set_sv_flags(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct sigvec *sv;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SIGVEC, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    sv = luab_udata(L, 1, m0, struct sigvec *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);
    sv->sv_flags = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get flags over (struct sigvec).
 *
 * @function get_sv_flags
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigvec:get_sv_flags()
 */
static int
SIGVEC_get_sv_flags(lua_State *L)
{
    luab_module_t *m;
    struct sigvec *sv;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGVEC, TYPE, __func__);
    sv = luab_udata(L, 1, m, struct sigvec *);
    x = sv->sv_flags;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
SIGVEC_gc(lua_State *L)
{
    luab_module_t *m;
    struct sigvec *sv;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGVEC, TYPE, __func__);
    sv = luab_udata(L, 1, m, struct sigvec *);
    (void)luab_core_free(sv->sv_handler, sv->sv_mask);
    return (luab_core_gc(L, 1, m));
}

static int
SIGVEC_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGVEC, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
SIGVEC_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGVEC, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t sigvec_methods[] = {
    LUAB_FUNC("sv_handler",         SIGVEC_sv_handler),
    LUAB_FUNC("set_sv_mask",        SIGVEC_set_sv_mask),
    LUAB_FUNC("set_sv_flags",       SIGVEC_set_sv_flags),
    LUAB_FUNC("get_table",          SIGVEC_get_table),
    LUAB_FUNC("set_sv_mask",        SIGVEC_get_sv_mask),
    LUAB_FUNC("get_sv_flags",       SIGVEC_get_sv_flags),
    LUAB_FUNC("dump",               SIGVEC_dump),
    LUAB_FUNC("__gc",               SIGVEC_gc),
    LUAB_FUNC("__len",              SIGVEC_len),
    LUAB_FUNC("__tostring",         SIGVEC_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
sigvec_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SIGVEC, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
sigvec_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SIGVEC, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
sigvec_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_sigvec_t *self;

    m = luab_xmod(SIGVEC, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_sigvec_t *);
    return ((void *)&(self->ud_sv));
}

static luab_table_t *
sigvec_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct sigvec *x, *y;
    size_t i, j;

    m = luab_xmod(SIGVEC, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct sigvec *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct sigvec *);
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
sigvec_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct sigvec *x;
    size_t i, j, k;

    m = luab_xmod(SIGVEC, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct sigvec *)tbl->tbl_vec) != NULL) &&
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
sigvec_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(SIGVEC, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_sigvec_type = {
    .m_id           = LUAB_SIGVEC_TYPE_ID,
    .m_name         = LUAB_SIGVEC_TYPE,
    .m_vec          = sigvec_methods,
    .m_create       = sigvec_create,
    .m_init         = sigvec_init,
    .m_get          = sigvec_udata,
    .m_get_tbl      = sigvec_checktable,
    .m_set_tbl      = sigvec_pushtable,
    .m_alloc_tbl    = sigvec_alloctable,
    .m_len          = sizeof(luab_sigvec_t),
    .m_sz           = sizeof(struct sigvec),
};
#endif /* __XSI_VISIBLE */
