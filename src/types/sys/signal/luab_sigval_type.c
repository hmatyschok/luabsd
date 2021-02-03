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

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#if __POSIX_VISIBLE >= 199309 || __XSI_VISIBLE >= 500
extern luab_module_t luab_sigval_type;

/*
 * Interface against
 *
 *  union sigval {
 *
 *      int sival_int;
 *      void    *sival_ptr;
 *      int     sigval_int;
 *      void    *sigval_ptr;
 *  };
 *
 */

typedef struct luab_sigval {
    luab_udata_t    ud_softc;
    union sigval    ud_sdu;
} luab_sigval_t;

/*
 * Subr.
 */

static void
sigval_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_sigval_t *self;

    if ((self = (luab_sigval_t *)arg) != NULL) {
        luab_setinteger(L, narg, "sival_int", self->ud_sdu.sival_int);
        luab_setfstring(L, narg, "sival_ptr", "(%p)", self->ud_sdu.sival_ptr);
        luab_setinteger(L, narg, "sigval_int", self->ud_sdu.sigval_int);
        luab_setfstring(L, narg, "sigval_ptr", "(%p)", self->ud_sdu.sigval_ptr);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SIGVAL)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              sival_int   = (LUA_TNUMBER),
 *              sival_ptr   = (LUA_T{NIL,STRING}),
 *              sigval_int  = (LUA_TNUMBER),
 *              sigval_ptr  = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ] = sigval:get_table()
 */
static int
SIGVAL_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGVAL, TYPE, __func__);

    xtp.xtp_fill = sigval_fillxtable;
    xtp.xtp_arg = luab_todata(L, 1, m, void *);
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
 * @usage iovec [, err, msg ] = sigval:dump()
 */
static int
SIGVAL_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Set sival_int over (union sigval).
 *
 * @function set_sival_int
 *
 * @param arg               Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigval:set_sival_int(arg)
 */
static int
SIGVAL_set_sival_int(lua_State *L)
{
    luab_module_t *m;
    luab_sigval_t *self;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(SIGVAL, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_sigval_t *);
    x = (int)luab_checkxinteger(L, 2, m, luab_env_uint_max);
    self->ud_sdu.sival_int = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get sival_int over (union sigval).
 *
 * @function get_sival_int
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigval:get_sival_int()
 */
static int
SIGVAL_get_sival_int(lua_State *L)
{
    luab_module_t *m;
    luab_sigval_t *self;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGVAL, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_sigval_t *);
    x = self->ud_sdu.sigval_int;

    return (luab_pushxinteger(L, x));
}

/***
 * Set sigval_int over (union sigval).
 *
 * @function set_sigval_int
 *
 * @param arg               Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigval:set_sigval_int(arg)
 */
static int
SIGVAL_set_sigval_int(lua_State *L)
{
    luab_module_t *m;
    luab_sigval_t *self;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(SIGVAL, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_sigval_t *);
    x = (int)luab_checkxinteger(L, 2, m, luab_env_uint_max);
    self->ud_sdu.sigval_int = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get sigval_int over (union sigval).
 *
 * @function get_sigval_int
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigval:get_sigval_int()
 */
static int
SIGVAL_get_sigval_int(lua_State *L)
{
    luab_module_t *m;
    luab_sigval_t *self;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGVAL, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_sigval_t *);
    x = self->ud_sdu.sigval_int;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
SIGVAL_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGVAL, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
SIGVAL_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGVAL, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
SIGVAL_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGVAL, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t sigval_methods[] = {
    LUAB_FUNC("set_sival_int",      SIGVAL_set_sival_int),
    LUAB_FUNC("set_sigval_int",     SIGVAL_set_sigval_int),
    LUAB_FUNC("get_table",          SIGVAL_get_table),
    LUAB_FUNC("get_sival_int",      SIGVAL_get_sival_int),
    LUAB_FUNC("get_sigval_int",      SIGVAL_get_sigval_int),
    LUAB_FUNC("dump",               SIGVAL_dump),
    LUAB_FUNC("__gc",               SIGVAL_gc),
    LUAB_FUNC("__len",              SIGVAL_len),
    LUAB_FUNC("__tostring",         SIGVAL_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
sigval_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SIGVAL, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
sigval_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SIGVAL, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
sigval_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_sigval_t *self;

    m = luab_xmod(SIGVAL, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_sigval_t *);
    return ((void *)&(self->ud_sdu));
}

static luab_table_t *
sigval_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    union sigval *x, *y;
    size_t i, j;

    m = luab_xmod(TM, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (union sigval *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, union sigval *);
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
sigval_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    union sigval *x;
    size_t i, j, k;

    m = luab_xmod(TM, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (union sigval *)tbl->tbl_vec) != NULL) &&
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
sigval_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(TM, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_sigval_type = {
    .m_id           = LUAB_SIGVAL_TYPE_ID,
    .m_name         = LUAB_SIGVAL_TYPE,
    .m_vec          = sigval_methods,
    .m_create       = sigval_create,
    .m_init         = sigval_init,
    .m_get          = sigval_udata,
    .m_get_tbl      = sigval_checktable,
    .m_set_tbl      = sigval_pushtable,
    .m_alloc_tbl    = sigval_alloctable,
    .m_len          = sizeof(luab_sigval_t),
    .m_sz           = sizeof(union sigval),
};
#endif /* __POSIX_VISIBLE >= 199309 || __XSI_VISIBLE >= 500 */
