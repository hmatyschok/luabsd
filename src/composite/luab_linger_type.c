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

extern luab_module_t luab_linger_type;

/*
 * Interface against
 *
 *  struct linger {
 *      int l_onoff;
 *      int l_linger;
 *  };
 */

typedef struct luab_linger {
    luab_udata_t    ud_softc;
    struct linger   ud_l;
} luab_linger_t;

#define luab_new_linger(L, arg) \
    ((luab_linger_t *)luab_newudata(L, &luab_linger_type, (arg)))
#define luab_to_linger(L, narg) \
    (luab_toldata((L), (narg), &luab_linger_type, \
        struct linger *, sizeof(struct linger)))

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(LINGER)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              l_onoff     = (LUA_TNUMBER),
 *              l_linger    = (LUA_TNUMBER),
 *          }
 *
 * @usage t = linger:get()
 */
static int
LINGER_get(lua_State *L)
{
    struct linger *l;

    (void)luab_core_checkmaxargs(L, 1);

    l = luab_udata(L, 1, &luab_linger_type, struct linger *);

    lua_newtable(L);
    luab_setinteger(L, -2, "l_onoff",   l->l_onoff);
    luab_setinteger(L, -2, "l_linger",  l->l_linger);
    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate linger{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = linger:dump()
 */
static int
LINGER_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_linger_type, sizeof(struct linger)));
}

/*
 * Service primitives.
 */

/***
 * Set value for option on / off.
 *
 * @function set_l_onoff
 *
 * @param data              Option.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = linger:set_l_onoff(data)
 */
static int
LINGER_set_l_onoff(lua_State *L)
{
    struct linger *l;
    int data;

    (void)luab_core_checkmaxargs(L, 2);

    l = luab_udata(L, 1, &luab_linger_type, struct linger *);
    data = (int)luab_checkinteger(L, 2, luab_env_int_max);

    l->l_onoff = data;

    return (luab_pushxinteger(L, data));
}

/***
 * Get value for option on / off.
 *
 * @function get_l_onoff
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = linger:get_l_onoff()
 */
static int
LINGER_get_l_onoff(lua_State *L)
{
    struct linger *l;
    int data;

    (void)luab_core_checkmaxargs(L, 1);

    l = luab_udata(L, 1, &luab_linger_type, struct linger *);
    data = l->l_onoff;

    return (luab_pushxinteger(L, data));
}

/***
 * Set value for linger.
 *
 * @function set_l_linger
 *
 * @param data              Specifies l_linger.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = linger:set_l_linger(data)
 */
static int
LINGER_set_l_linger(lua_State *L)
{
    struct linger *l;
    int data;

    (void)luab_core_checkmaxargs(L, 2);

    l = luab_udata(L, 1, &luab_linger_type, struct linger *);
    data = (int)luab_checkinteger(L, 2, luab_env_int_max);

    l->l_linger = data;

    return (luab_pushxinteger(L, data));
}

/***
 * Get value for linger.
 *
 * @function get_l_linger
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = linger:get_l_linger()
 */
static int
LINGER_get_l_linger(lua_State *L)
{
    struct linger *l;
    int data;

    (void)luab_core_checkmaxargs(L, 1);

    l = luab_udata(L, 1, &luab_linger_type, struct linger *);
    data = l->l_linger;

    return (luab_pushxinteger(L, data));
}

/*
 * Metamethods.
 */

static int
LINGER_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_linger_type));
}

static int
LINGER_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_linger_type));
}

static int
LINGER_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_linger_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t linger_methods[] = {
    LUAB_FUNC("set_l_onoff",  LINGER_set_l_onoff),
    LUAB_FUNC("set_l_linger", LINGER_set_l_linger),
    LUAB_FUNC("get",          LINGER_get),
    LUAB_FUNC("get_l_onoff",  LINGER_get_l_onoff),
    LUAB_FUNC("get_l_linger", LINGER_get_l_linger),
    LUAB_FUNC("dump",         LINGER_dump),
    LUAB_FUNC("__gc",         LINGER_gc),
    LUAB_FUNC("__len",        LINGER_len),
    LUAB_FUNC("__tostring",   LINGER_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
linger_create(lua_State *L, void *arg)
{
    return (luab_new_linger(L, arg));
}

static void
linger_init(void *ud, void *arg)
{
    luab_udata_init(&luab_linger_type, ud, arg);
}

static void *
linger_udata(lua_State *L, int narg)
{
    return (luab_to_linger(L, narg));
}

static luab_table_t *
linger_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct linger *x, *y;
    size_t m, n, sz;

    sz = sizeof(struct linger);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (struct linger *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_linger_type, struct linger *);
                        (void)memmove(&(x[m]), y, sz);
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
linger_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct linger *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (struct linger *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_linger_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

luab_module_t luab_linger_type = {
    .m_cookie   = LUAB_LINGER_TYPE_ID,
    .m_name     = LUAB_LINGER_TYPE,
    .m_vec      = linger_methods,
    .m_create   = linger_create,
    .m_init     = linger_init,
    .m_get      = linger_udata,
    .m_get_tbl  = linger_checktable,
    .m_set_tbl  = linger_pushtable,
    .m_sz       = sizeof(luab_linger_t),
};
