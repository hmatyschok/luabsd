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

#include <sys/time.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#ifdef __BSD_VISIBLE
extern luab_module_t luab_bintime_type;

/*
 * Interface against
 *
 *  struct bintime {
 *      time_t  sec;
 *      uint64_t frac;
 *  };
 */

typedef struct luab_bintime {
    luab_udata_t    ud_softc;
    struct bintime  ud_bt;
} luab_bintime_t;

#define luab_new_bintime(L, arg) \
    ((luab_bintime_t *)luab_newudata(L, &luab_bintime_type, (arg)))
#define luab_to_bintime(L, narg) \
    (luab_toldata((L), (narg), &luab_bintime_type, \
        struct bintime *, luab_bintime_type.m_sz))

/*
 * Subr.
 */

static void
bintime_fillxtable(lua_State *L, int narg, void *arg)
{
    struct bintime *bt;

    if ((bt = (struct bintime *)arg) != NULL) {

        luab_setinteger(L, narg, "sec",   bt->sec);
        luab_setinteger(L, narg, "frac",  bt->frac);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(BINTIME)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              sec     = (LUA_TNUMBER),
 *              frac    = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = bintime:get_table()
 */
static int
BINTIME_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = bintime_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, &luab_bintime_type);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate bintime{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = bintime:dump()
 */
static int
BINTIME_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_bintime_type, luab_bintime_type.m_sz));
}

/*
 * Access functions.
 */

/***
 * Set value for sytem time.
 *
 * @function set_sec
 *
 * @param data              Seconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = bintime:set_sec(data)
 */
static int
BINTIME_set_sec(lua_State *L)
{
    struct bintime *bt;
    time_t x;

    (void)luab_core_checkmaxargs(L, 2);

    bt = luab_udata(L, 1, &luab_bintime_type, struct bintime *);
    x = (time_t)luab_checkinteger(L, 2, luab_env_int_max);

    bt->sec = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for system time.
 *
 * @function get_sec
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = bintime:get_sec()
 */
static int
BINTIME_get_sec(lua_State *L)
{
    struct bintime *bt;
    time_t x;

    (void)luab_core_checkmaxargs(L, 1);

    bt = luab_udata(L, 1, &luab_bintime_type, struct bintime *);
    x = bt->sec;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for for system time.
 *
 * @function set_frac
 *
 * @param data              Specifies frac.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = bintime:set_frac(data)
 */
static int
BINTIME_set_frac(lua_State *L)
{
    struct bintime *bt;
    uint64_t x;

    (void)luab_core_checkmaxargs(L, 2);

    bt = luab_udata(L, 1, &luab_bintime_type, struct bintime *);
    x = (uint64_t)luab_checkinteger(L, 2, luab_env_long_max);

    bt->frac = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for for system time.
 *
 * @function get_frac
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = bintime:get_frac()
 */
static int
BINTIME_get_frac(lua_State *L)
{
    struct bintime *bt;
    uint64_t x;

    (void)luab_core_checkmaxargs(L, 1);

    bt = luab_udata(L, 1, &luab_bintime_type, struct bintime *);
    x = bt->frac;

    return (luab_pushxinteger(L, x));
}

/*
 * Meta-methods.
 */

static int
BINTIME_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_bintime_type));
}

static int
BINTIME_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_bintime_type));
}

static int
BINTIME_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_bintime_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t bintime_methods[] = {
    LUAB_FUNC("set_sec",        BINTIME_set_sec),
    LUAB_FUNC("set_frac",       BINTIME_set_frac),
    LUAB_FUNC("get_table",      BINTIME_get_table),
    LUAB_FUNC("get_sec",        BINTIME_get_sec),
    LUAB_FUNC("get_frac",       BINTIME_get_frac),
    LUAB_FUNC("dump",           BINTIME_dump),
    LUAB_FUNC("__gc",           BINTIME_gc),
    LUAB_FUNC("__len",          BINTIME_len),
    LUAB_FUNC("__tostring",     BINTIME_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
bintime_create(lua_State *L, void *arg)
{
    return (luab_new_bintime(L, arg));
}

static void
bintime_init(void *ud, void *arg)
{
    luab_udata_init(&luab_bintime_type, ud, arg);
}

static void *
bintime_udata(lua_State *L, int narg)
{
    return (luab_to_bintime(L, narg));
}

static luab_table_t *
bintime_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct bintime *x, *y;
    size_t m, n;

    if ((tbl = luab_table_newvectornil(L, narg, &luab_bintime_type)) != NULL) {

        if (((x = (struct bintime *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_bintime_type, struct bintime *);
                        (void)memmove(&(x[m]), y, luab_bintime_type.m_sz);
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
bintime_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct bintime *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (struct bintime *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_bintime_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

static luab_table_t *
bintime_alloctable(void *vec, size_t card)
{
    return (luab_table_create(&luab_bintime_type, vec, card));
}

luab_module_t luab_bintime_type = {
    .m_id           = LUAB_BINTIME_TYPE_ID,
    .m_name         = LUAB_BINTIME_TYPE,
    .m_vec          = bintime_methods,
    .m_create       = bintime_create,
    .m_init         = bintime_init,
    .m_get          = bintime_udata,
    .m_get_tbl      = bintime_checktable,
    .m_set_tbl      = bintime_pushtable,
    .m_alloc_tbl    = bintime_alloctable,
    .m_len          = sizeof(luab_bintime_t),
    .m_sz           = sizeof(struct bintime),
};
#endif /* __BSD_VISIBLE */
