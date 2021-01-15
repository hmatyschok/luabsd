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

#if __POSIX_VISIBLE || __XSI_VISIBLE
extern luab_module_t luab_sigaction_type;

/*
 * Interface against
 *
 *  struct sigaction {
 *      union {
 *          void    (*__sa_handler)(int);
 *          void    (*__sa_sigaction)(int, struct __siginfo *, void *);
 *      } __sigaction_u;
 *      int sa_flags;
 *      sigset_t sa_mask;
 *  };
 *
 * signal vector "template" used in sigaction call.
 */

typedef struct luab_sigaction {
    luab_udata_t        ud_softc;
    struct sigaction    ud_sa;
} luab_sigaction_t;

/*
 * Subr.
 */

static void
sigaction_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_module_t *m;
    struct sigaction *sa;

    m = luab_xmod(SIGSET, TYPE, __func__);

    if ((sa = (struct sigaction *)arg) != NULL) {
        luab_setfstring(L, narg, "sa_handler", "(%p)",    sa->sa_handler);
#if __XSI_VISIBLE
        luab_setfstring(L, narg, "sa_sigaction", "(%p)",  sa->sa_sigaction);
#endif
        luab_setinteger(L, narg, "sa_flags",              sa->sa_flags);
        luab_setxdata(L, narg, m, "sa_mask",              &sa->sa_mask);
   } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SIGACTION)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              sa_handler      = (LUA_T{NIL,STRING}),
 *              sa_sigaction    = (LUA_T{NIL,STRING}),
 *              sa_flags        = (LUA_TNUMBER),
 *              sa_mask         = (LUA_T{NIL,USERDATA(SIGSET)}),
 *          }
 *
 * @usage t [, err, msg ] = sigaction:get_table()
 */
static int
SIGACTION_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGACTION, TYPE, __func__);

    xtp.xtp_fill = sigaction_fillxtable;
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
 * @usage iovec [, err, msg ] = sigaction:dump()
 */
static int
SIGACTION_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions, immtuable properties.
 */

/***
 * Get base address for sa_handler.
 *
 * @function sa_handler
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigaction:sa_handler()
 */
static int
SIGACTION_sa_handler(lua_State *L)
{
    luab_module_t *m;
    struct sigaction *sa;
    void (*x)(int);

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGACTION, TYPE, __func__);

    sa = luab_todata(L, 1, m, struct sigaction *);
    x = sa->sa_handler;

    return (luab_pushfstring(L, "(%p)", x));
}

#if __XSI_VISIBLE
/***
 * Get base address for sa_handler.
 *
 * @function sa_sigaction
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigaction:sa_sigaction()
 */
static int
SIGACTION_sa_sigaction(lua_State *L)
{
    luab_module_t *m;
    struct sigaction *sa;
    void (*x)(int, struct __siginfo *, void *);

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGACTION, TYPE, __func__);

    sa = luab_todata(L, 1, m, struct sigaction *);
    x = sa->sa_sigaction;

    return (luab_pushfstring(L, "(%p)", x));
}
#endif /* __XSI_VISIBLE */

/*
 * Access functions
 */

/***
 * Set signal options.
 *
 * @function set_sa_flags
 *
 * @param arg               Flags, instance of (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigevent:set_sa_flags(arg)
 */
static int
SIGACTION_set_sa_flags(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct sigaction *sa;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SIGACTION, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    sa = luab_udata(L, 1, m0, struct sigaction *);
    x = luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    sa->sa_flags = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get signal options.
 *
 * @function get_sa_flags
 *
 * @return (LUA_TNUNBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigaction:get_sa_flags()
 */
static int
SIGACTION_get_sa_flags(lua_State *L)
{
    luab_module_t *m;
    struct sigaction *sa;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGACTION, TYPE, __func__);
    sa = luab_udata(L, 1, m, struct sigaction *);
    x = sa->sa_flags;

    return (luab_pushxinteger(L, x));
}

/***
 * Set signal set.
 *
 * @function set_sa_mask
 *
 * @param arg               Mask, instance of (LUA_TUSERDATA(SIGSET)).
 *
 * @return (LUA_T{NIL,USERDATA(SIGSET)} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sigmask [, err, msg ] = sigevent:set_sa_mask(arg)
 */
static int
SIGACTION_set_sa_mask(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct sigaction *sa;
    sigset_t *x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SIGACTION, TYPE, __func__);
    m1 = luab_xmod(SIGSET, TYPE, __func__);

    sa = luab_udata(L, 1, m0, struct sigaction *);

    if ((x = luab_udataisnil(L, 2, m1, sigset_t *)) != NULL)
        (void)memmove(&sa->sa_mask, x, m1->m_sz);
    else
        (void)memset_s(&sa->sa_mask, m1->m_sz, 0, m1->m_sz);

    return (luab_pushxdata(L, m1, x));
}

/***
 * Get signal set.
 *
 * @function get_sa_mask
 *
 * @return (LUA_TNUNBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sigmask [, err, msg ] = sigaction:get_sa_mask()
 */
static int
SIGACTION_get_sa_mask(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct sigaction *sa;
    sigset_t *x;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(SIGACTION, TYPE, __func__);
    m1 = luab_xmod(SIGSET, TYPE, __func__);

    sa = luab_udata(L, 1, m0, struct sigaction *);
    x = luab_udata(L, 2, m1, sigset_t *);

    return (luab_pushxdata(L, m1, x));
}

/*
 * Metamethods.
 */

static int
SIGACTION_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGACTION, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
SIGACTION_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGACTION, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
SIGACTION_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGACTION, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t sigaction_methods[] = {
    LUAB_FUNC("sa_handler",     SIGACTION_sa_handler),
#if __XSI_VISIBLE
    LUAB_FUNC("sa_sigaction",   SIGACTION_sa_sigaction),
#endif
    LUAB_FUNC("set_sa_flags",   SIGACTION_set_sa_flags),
    LUAB_FUNC("set_sa_mask",    SIGACTION_set_sa_mask),
    LUAB_FUNC("get_table",      SIGACTION_get_table),
    LUAB_FUNC("get_sa_flags",   SIGACTION_get_sa_flags),
    LUAB_FUNC("get_sa_mask",    SIGACTION_get_sa_mask),
    LUAB_FUNC("dump",           SIGACTION_dump),
    LUAB_FUNC("__gc",           SIGACTION_gc),
    LUAB_FUNC("__len",          SIGACTION_len),
    LUAB_FUNC("__tostring",     SIGACTION_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
sigaction_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SIGACTION, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
sigaction_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SIGACTION, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
sigaction_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_sigaction_t *self;

    m = luab_xmod(SIGACTION, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_sigaction_t *);
    return ((void *)&(self->ud_sa));
}

static luab_table_t *
sigaction_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct sigaction *x, *y;
    size_t i, j;

    m = luab_xmod(SIGACTION, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct sigaction *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct sigaction *);
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
sigaction_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct sigaction *x;
    size_t i, j, k;

    m = luab_xmod(SIGACTION, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct sigaction *)tbl->tbl_vec) != NULL) &&
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
        errno = EINVAL;
}

static luab_table_t *
sigaction_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(SIGACTION, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_sigaction_type = {
    .m_id           = LUAB_SIGACTION_TYPE_ID,
    .m_name         = LUAB_SIGACTION_TYPE,
    .m_vec          = sigaction_methods,
    .m_create       = sigaction_create,
    .m_init         = sigaction_init,
    .m_get          = sigaction_udata,
    .m_get_tbl      = sigaction_checktable,
    .m_set_tbl      = sigaction_pushtable,
    .m_alloc_tbl    = sigaction_alloctable,
    .m_len          = sizeof(luab_sigaction_t),
    .m_sz           = sizeof(struct sigaction),
};
#endif /* __POSIX_VISIBLE || __XSI_VISIBLE */
