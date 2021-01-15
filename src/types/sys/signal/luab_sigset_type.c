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

const lua_Integer luab_env_sig_words = _SIG_WORDS;
const lua_Integer luab_env_sig_maxsig = _SIG_MAXSIG;

#if __POSIX_VISIBLE || __XSI_VISIBLE
extern luab_module_t luab_sigset_type;

/*
 * Interface against
 *
 *  sigset_t
 *
 */

typedef struct luab_sigset {
    luab_udata_t    ud_softc;
    sigset_t         ud_sdu;
} luab_sigset_t;

/*
 * Subr.
 */

static void
sigset_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_module_t *m;
    luab_sigset_t *self;
    uint32_t *x;

    m = luab_xmod(UINT32, TYPE, __func__);

    if ((self = (luab_sigset_t *)arg) != NULL) {
        x = (uint32_t *)(self->ud_sdu.__bits);
        luab_table_setxvector(L, narg, m, "bits", x, luab_env_sig_words, 1, 1);
   } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SIGSET)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              bits = (LUA_T{NIL,TABLE}),
 *          }
 *
 * @usage t [, err, msg ] = sigset:get_table()
 */
static int
SIGSET_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGSET, TYPE, __func__);

    xtp.xtp_fill = sigset_fillxtable;
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
 * @usage iovec [, err, msg ] = sigset:dump()
 */
static int
SIGSET_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions, immtuable properties.
 */

/***
 * Get (LUA_TTABLE) over (sigset_t).
 *
 * @function bits
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigset:bits()
 */
static int
SIGSET_bits(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_sigset_t *self;
    uint32_t *x;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(SIGSET, TYPE, __func__);
    m1 = luab_xmod(UINT32, TYPE, __func__);

    self = luab_todata(L, 1, m0, luab_sigset_t *);
    x = (uint32_t *)(self->ud_sdu.__bits);

    return (luab_table_pushxvector(L, -2, m1, x, luab_env_sig_words, 1, 1));
}

/*
 * Metamethods.
 */

static int
SIGSET_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGSET, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
SIGSET_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGSET, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
SIGSET_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGSET, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t sigset_methods[] = {
    LUAB_FUNC("bits",           SIGSET_bits),
    LUAB_FUNC("get_table",      SIGSET_get_table),
    LUAB_FUNC("dump",           SIGSET_dump),
    LUAB_FUNC("__gc",           SIGSET_gc),
    LUAB_FUNC("__len",          SIGSET_len),
    LUAB_FUNC("__tostring",     SIGSET_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
sigset_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SIGSET, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
sigset_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SIGSET, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
sigset_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_sigset_t *self;

    m = luab_xmod(SIGSET, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_sigset_t *);
    return ((void *)&(self->ud_sdu));
}

static luab_table_t *
sigset_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    sigset_t *x, *y;
    size_t i, j;

    m = luab_xmod(SIGSET, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (sigset_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, sigset_t *);
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
sigset_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    sigset_t *x;
    size_t i, j, k;

    m = luab_xmod(SIGSET, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (sigset_t *)tbl->tbl_vec) != NULL) &&
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
sigset_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(SIGSET, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_sigset_type = {
    .m_id           = LUAB_SIGSET_TYPE_ID,
    .m_name         = LUAB_SIGSET_TYPE,
    .m_vec          = sigset_methods,
    .m_create       = sigset_create,
    .m_init         = sigset_init,
    .m_get          = sigset_udata,
    .m_get_tbl      = sigset_checktable,
    .m_set_tbl      = sigset_pushtable,
    .m_alloc_tbl    = sigset_alloctable,
    .m_len          = sizeof(luab_sigset_t),
    .m_sz           = sizeof(sigset_t),
};
#endif /* __POSIX_VISIBLE || __XSI_VISIBLE */
