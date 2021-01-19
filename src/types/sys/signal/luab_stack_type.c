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
extern luab_module_t luab_stack_type;

const lua_Integer luab_env_sigstksz = SIGSTKSZ;

/*
 * Interface against
 *
 *  struct __stack_t {
 *      void    *ss_sp;
 *      __size_t ss_size;
 *      int ss_flags;
 * };
 * typedef  struct __stack_t stack_t;
 *
 */

typedef struct luab_stack {
    luab_udata_t    ud_softc;
    stack_t         ud_stk;
} luab_stack_t;

/*
 * Subr.
 */

static void
stack_fillxtable(lua_State *L, int narg, void *arg)
{
    stack_t *stk;

    if ((stk = (stack_t *)arg) != NULL) {
        luab_setfstring(L, narg, "ss_sp", "(%p)",   stk->ss_sp);
        luab_setinteger(L, narg, "ss_size",         stk->ss_size);
        luab_setinteger(L, narg, "ss_flags",        stk->ss_flags);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(STACK)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              ss_sp       = (LUA_T{NIL,STRING}),
 *              ss_size     = (LUA_TNUMBER),
 *              ss_flags    = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = stack:get_table()
 */
static int
STACK_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STACK, TYPE, __func__);

    xtp.xtp_fill = stack_fillxtable;
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
 * @usage iovec [, err, msg ] = stack:dump()
 */
static int
STACK_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get signal stack base over (stack_t).
 *
 * @function ss_sp
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = stack:ss_sp()
 */
static int
STACK_ss_sp(lua_State *L)
{
    luab_module_t *m;
    stack_t *stk;
    void *dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STACK, TYPE, __func__);
    stk = luab_udata(L, 1, m, stack_t *);
    dp = stk->ss_sp;

    return (luab_pushfstring(L, dp));
}

/*
 * Access functions.
 */

/***
 * Set signal stack size over (stack_t).
 *
 * @function set_ss_size
 *
 * @param arg               Size, (LUA_T{NUMBER,USERDATA(SIZE)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = stack:set_ss_size()
 */
static int
STACK_set_ss_size(lua_State *L)
{
    luab_module_t *m0, *m1;
    stack_t *stk;
    size_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(STACK, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);

    stk = luab_udata(L, 1, m0, stack_t *);
    x = (size_t)luab_checklxinteger(L, 2, m1, 0);

    if (x >= luab_env_sigstksz) {

        if (stk->ss_sp != NULL)
            (void)luab_core_free(stk->ss_sp, stk->ss_size);

        if ((stk->ss_sp = luab_core_alloc(1, x)) != NULL)
            stk->ss_size = x;
        else
            stk->ss_size = 0;
    } else
        errno = ERANGE;

    return (luab_pushxinteger(L, x));
}

/***
 * Get signal stack size over (stack_t).
 *
 * @function get_ss_size
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = stack:get_ss_size()
 */
static int
STACK_get_ss_size(lua_State *L)
{
    luab_module_t *m;
    stack_t *stk;
    size_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STACK, TYPE, __func__);
    stk = luab_udata(L, 1, m, stack_t *);
    x = stk->ss_size;

    return (luab_pushxinteger(L, x));
}

/***
 * Set flags over (stack_t).
 *
 * @function set_ss_flags
 *
 * @param arg               Flags argument are constructed with values over
 *
 *                              SS_{
 *                                  DISABLE,
 *                                  ONSTACK
 *                              }
 *
 *                          by inclusive OR from (LUA_T{NUMBER,USERDATA(INT)}).
 *
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = stack:set_ss_flags(arg)
 */
static int
STACK_set_ss_flags(lua_State *L)
{
    luab_module_t *m0, *m1;
    stack_t *stk;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(STACK, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    stk = luab_udata(L, 1, m0, stack_t *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);
    stk->ss_flags = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get flags over (stack_t).
 *
 * @function get_ss_flags
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = stack:get_ss_flags()
 */
static int
STACK_get_ss_flags(lua_State *L)
{
    luab_module_t *m;
    stack_t *stk;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STACK, TYPE, __func__);
    stk = luab_udata(L, 1, m, stack_t *);
    x = stk->ss_flags;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
STACK_gc(lua_State *L)
{
    luab_module_t *m;
    stack_t *stk;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STACK, TYPE, __func__);
    stk = luab_udata(L, 1, m, stack_t *);
    (void)luab_core_free(stk->ss_sp, stk->ss_size);
    return (luab_core_gc(L, 1, m));
}

static int
STACK_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STACK, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
STACK_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STACK, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t stack_methods[] = {
    LUAB_FUNC("ss_sp",              STACK_ss_sp),
    LUAB_FUNC("set_ss_size",        STACK_set_ss_size),
    LUAB_FUNC("set_ss_flags",       STACK_set_ss_flags),
    LUAB_FUNC("get_table",          STACK_get_table),
    LUAB_FUNC("set_ss_size",        STACK_get_ss_size),
    LUAB_FUNC("get_ss_flags",       STACK_get_ss_flags),
    LUAB_FUNC("dump",               STACK_dump),
    LUAB_FUNC("__gc",               STACK_gc),
    LUAB_FUNC("__len",              STACK_len),
    LUAB_FUNC("__tostring",         STACK_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
stack_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(STACK, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
stack_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(STACK, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
stack_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_stack_t *self;

    m = luab_xmod(STACK, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_stack_t *);
    return ((void *)&(self->ud_stk));
}

static luab_table_t *
stack_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    stack_t *x, *y;
    size_t i, j;

    m = luab_xmod(STACK, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (stack_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, stack_t *);
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
stack_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    stack_t *x;
    size_t i, j, k;

    m = luab_xmod(STACK, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (stack_t *)tbl->tbl_vec) != NULL) &&
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
stack_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(STACK, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_stack_type = {
    .m_id           = LUAB_STACK_TYPE_ID,
    .m_name         = LUAB_STACK_TYPE,
    .m_vec          = stack_methods,
    .m_create       = stack_create,
    .m_init         = stack_init,
    .m_get          = stack_udata,
    .m_get_tbl      = stack_checktable,
    .m_set_tbl      = stack_pushtable,
    .m_alloc_tbl    = stack_alloctable,
    .m_len          = sizeof(luab_stack_t),
    .m_sz           = sizeof(stack_t),
};
#endif /* __XSI_VISIBLE */
