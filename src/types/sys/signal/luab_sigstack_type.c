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

#if __XSI_VISIBLE
extern luab_module_t luab_sigstack_type;

/*
 * Interface against
 *
 *  struct sigstack {
 *      void    *ss_sp;
 *      int ss_onstack;
 *  };
 */

typedef struct luab_sigstack {
    luab_udata_t        ud_softc;
    struct sigstack     ud_stk;
} luab_sigstack_t;

/*
 * Subr.
 */

static void
sigstack_fillxtable(lua_State *L, int narg, void *arg)
{
    struct sigstack *stk;

    if ((stk = (struct sigstack *)arg) != NULL) {
        luab_setfstring(L, narg, "ss_sp", "(%p)",   stk->ss_sp);
        luab_setinteger(L, narg, "ss_onstack",      stk->ss_onstack);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SIGSTACK)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              ss_sp       = (LUA_T{NIL,STRING}),
 *              ss_onstack  = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = sigstack:get_table()
 */
static int
SIGSTACK_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGSTACK, TYPE, __func__);

    xtp.xtp_fill = sigstack_fillxtable;
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
 * @usage iovec [, err, msg ] = sigstack:dump()
 */
static int
SIGSTACK_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get signal sigstack base over (struct sigstack).
 *
 * @function ss_sp
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigstack:ss_sp()
 */
static int
SIGSTACK_ss_sp(lua_State *L)
{
    luab_module_t *m;
    struct sigstack *stk;
    void *dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGSTACK, TYPE, __func__);
    stk = luab_udata(L, 1, m, struct sigstack *);
    dp = stk->ss_sp;

    return (luab_pushfstring(L, dp));
}

/*
 * Access functions.
 */

/***
 * Set signal sigstack size over (struct sigstack).
 *
 * @function set_ss_onstack
 *
 * @param arg               Size, (LUA_T{NUMBER,USERDATA(int)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigstack:set_ss_onstack()
 */
static int
SIGSTACK_set_ss_onstack(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct sigstack *stk;
    size_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(SIGSTACK, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    stk = luab_udata(L, 1, m0, struct sigstack *);
    x = (int)luab_checklxinteger(L, 2, m1, 0);

    stk->ss_onstack = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get signal sigstack size over (struct sigstack).
 *
 * @function get_ss_onstack
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigstack:get_ss_onstack()
 */
static int
SIGSTACK_get_ss_onstack(lua_State *L)
{
    luab_module_t *m;
    struct sigstack *stk;
    size_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGSTACK, TYPE, __func__);
    stk = luab_udata(L, 1, m, struct sigstack *);
    x = stk->ss_onstack;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
SIGSTACK_gc(lua_State *L)
{
    luab_module_t *m;
    struct sigstack *stk;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGSTACK, TYPE, __func__);
    stk = luab_udata(L, 1, m, struct sigstack *);
    (void)luab_core_free(stk->ss_sp, luab_env_sigstksz);
    return (luab_core_gc(L, 1, m));
}

static int
SIGSTACK_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGSTACK, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
SIGSTACK_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGSTACK, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t sigstack_methods[] = {
    LUAB_FUNC("ss_sp",              SIGSTACK_ss_sp),
    LUAB_FUNC("set_ss_onstack",     SIGSTACK_set_ss_onstack),
    LUAB_FUNC("get_table",          SIGSTACK_get_table),
    LUAB_FUNC("set_ss_onstack",     SIGSTACK_get_ss_onstack),
    LUAB_FUNC("dump",               SIGSTACK_dump),
    LUAB_FUNC("__gc",               SIGSTACK_gc),
    LUAB_FUNC("__len",              SIGSTACK_len),
    LUAB_FUNC("__tostring",         SIGSTACK_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
sigstack_create(lua_State *L, void *arg __unused)
{
    luab_module_t *m;
    void *dp;

    m = luab_xmod(SIGSTACK, TYPE, __func__);

    if ((dp = luab_core_alloc(1, luab_env_sigstksz)) == NULL)
        m = NULL;

    return (luab_newuserdata(L, m, dp));
}

static void
sigstack_init(void *ud, void *arg)
{
    luab_sigstack_t *self;

    if ((self = (luab_sigstack_t *)ud) != NULL)
        self->ud_stk.ss_sp = arg;
}

static void *
sigstack_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_sigstack_t *self;

    m = luab_xmod(SIGSTACK, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_sigstack_t *);
    return ((void *)&(self->ud_stk));
}

static luab_table_t *
sigstack_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct sigstack *x, *y;
    size_t i, j;

    m = luab_xmod(SIGSTACK, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct sigstack *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct sigstack *);
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
sigstack_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct sigstack *x;
    size_t i, j, k;

    m = luab_xmod(SIGSTACK, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct sigstack *)tbl->tbl_vec) != NULL) &&
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
sigstack_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(SIGSTACK, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_sigstack_type = {
    .m_id           = LUAB_SIGSTACK_TYPE_ID,
    .m_name         = LUAB_SIGSTACK_TYPE,
    .m_vec          = sigstack_methods,
    .m_create       = sigstack_create,
    .m_init         = sigstack_init,
    .m_get          = sigstack_udata,
    .m_get_tbl      = sigstack_checktable,
    .m_set_tbl      = sigstack_pushtable,
    .m_alloc_tbl    = sigstack_alloctable,
    .m_len          = sizeof(luab_sigstack_t),
    .m_sz           = sizeof(struct sigstack),
};
#endif /* __XSI_VISIBLE */
