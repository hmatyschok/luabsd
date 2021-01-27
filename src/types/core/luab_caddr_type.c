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

extern luab_module_t luab_caddr_type;

/*
 * Interface against
 *
 *  caddr_t
 *
 */

typedef struct luab_caddr {
    luab_udata_t    ud_softc;
    caddr_t         ud_addr;
} luab_caddr_t;

/*
 * Subr.
 */

static void
caddr_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_caddr_t *self;

    if ((self = (luab_caddr_t *)arg) != NULL) {
        luab_setfstring(L, narg, "addr", "(%p)", self->ud_addr);

   } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(CADDR)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              addr = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ] = caddr:get_table()
 */
static int
CADDR_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CADDR, TYPE, __func__);

    xtp.xtp_fill = caddr_fillxtable;
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
 * @usage iovec [, err, msg ] = caddr:dump()
 */
static int
CADDR_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions.
 */

/***
 * Get core address.
 *
 * @function addr
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = caddr:addr()
 */
static int
CADDR_addr(lua_State *L)
{
    luab_module_t *m;
    luab_caddr_t *self;
    caddr_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CADDR, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_caddr_t *);
    x = self->ud_addr;

    return (luab_pushfstring(L, "(%p)", x));
}

/*
 * Metamethods.
 */

static int
CADDR_gc(lua_State *L)
{
    luab_module_t *m;
    luab_caddr_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(CADDR, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_caddr_t *);

    return (luab_core_gc(L, 1, m));
}

static int
CADDR_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(CADDR, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
CADDR_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(CADDR, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t caddr_methods[] = {
    LUAB_FUNC("addr",           CADDR_addr),
    LUAB_FUNC("get_table",      CADDR_get_table),
    LUAB_FUNC("dump",           CADDR_dump),
    LUAB_FUNC("__gc",           CADDR_gc),
    LUAB_FUNC("__len",          CADDR_len),
    LUAB_FUNC("__tostring",     CADDR_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
caddr_create(lua_State *L, void *arg __unused)
{
    luab_module_t *m;
    m = luab_xmod(CADDR, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
caddr_init(void *ud __unused, void *arg __unused)
{

}

static void *
caddr_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_caddr_t *self;

    m = luab_xmod(CADDR, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_caddr_t *);
    return ((void *)self->ud_addr);
}

luab_module_t luab_caddr_type = {
    .m_id           = LUAB_CADDR_TYPE_ID,
    .m_name         = LUAB_CADDR_TYPE,
    .m_vec          = caddr_methods,
    .m_create       = caddr_create,
    .m_init         = caddr_init,
    .m_get          = caddr_udata,
    .m_len          = sizeof(luab_caddr_t),
    .m_sz           = sizeof(caddr_t),
};
