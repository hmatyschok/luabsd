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

#if __POSIX_VISIBLE >= 199309
extern luab_module_t luab_timer_type;

/*
 * Interface against
 *
 *  timer_t
 *
 * by
 *
 *  typedef struct luab_timer {
 *      luab_udata_t    ud_softc;
 *      timer_t         ud_sdu;
 *  } luab_timer_t;
 *
 */

/*
 * Subr.
 */

static void
timer_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_timer_t *self;

    if ((self = (luab_timer_t *)arg) != NULL) {

        luab_setfstring(L, narg, "value", "(%p)", self->ud_sdu);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(TIMER)) timero (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value = (LUA_T{NIL,STRING}),
 *          }
 *
 * @usage t [, err, msg ] = timer:get_table()
 */
static int
TIMER_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIMER, TYPE, __func__);

    xtp.xtp_fill = timer_fillxtable;
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
 * @usage iovec [, err, msg ] = timer:dump()
 */
static int
TIMER_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get addr of timer.
 *
 * @function value
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = timer:value()
 */
static int
TIMER_value(lua_State *L)
{
    luab_module_t *m;
    luab_timer_t *self;
    timer_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIMER, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_timer_t *);
    x = self->ud_sdu;

    return (luab_pushfstring(L, "(%p)", x));
}

/*
 * Metamethods.
 */

static int
TIMER_gc(lua_State *L)
{
    luab_module_t *m;
    luab_timer_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(TIMER, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_timer_t *);

    if (self->ud_sdu != NULL)
        (void)timer_delete(self->ud_sdu);

    return (luab_core_gc(L, 1, m));
}

static int
TIMER_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TIMER, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
TIMER_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(TIMER, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal timererface.
 */

static luab_module_table_t timer_methods[] = {
    LUAB_FUNC("value",          TIMER_value),
    LUAB_FUNC("get_table",      TIMER_get_table),
    LUAB_FUNC("dump",           TIMER_dump),
    LUAB_FUNC("__gc",           TIMER_gc),
    LUAB_FUNC("__len",          TIMER_len),
    LUAB_FUNC("__tostring",     TIMER_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
_timer_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(TIMER, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
timer_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(TIMER, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
timer_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_timer_t *self;

    m = luab_xmod(TIMER, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_timer_t *);
    return ((void *)self);
}

luab_module_t luab_timer_type = {
    .m_id           = LUAB_TIMER_TYPE_ID,
    .m_name         = LUAB_TIMER_TYPE,
    .m_vec          = timer_methods,
    .m_create       = _timer_create,
    .m_init         = timer_init,
    .m_get          = timer_udata,
    .m_len          = sizeof(luab_timer_t),
    .m_sz           = sizeof(timer_t),
};
#endif /* __POSIX_VISIBLE >= 199309 */
