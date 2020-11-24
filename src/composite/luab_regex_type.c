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

#include <regex.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_regex_type;

/*
 * Interface against
 *
 *  typedef struct {
 *      int re_magic;
 *      size_t re_nsub;
 *      const char *re_endp;
 *      struct re_guts *re_g;
 *  } regex_t;
 */

typedef struct luab_regex {
    luab_udata_t    ud_softc;
    regex_t         ud_re;
} luab_regex_t;

#define luab_new_regex(L, arg) \
    ((luab_regex_t *)luab_newudata(L, &luab_regex_type, (arg)))
#define luab_to_regex(L, narg) \
    (luab_toldata((L), (narg), &luab_regex_type, \
        regex_t *, sizeof(regex_t)))

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(REGEX)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              re_magic    = (LUA_TNUMBER),
 *              re_nsub     = (LUA_TNUMBER),
 *              re_endp     = (LUA_TSTRING),
 *              re_g        = (LUA_TSTRING),
 *          }
 *
 * @usage t = regex:get()
 */
static int
REGEX_get(lua_State *L)
{
    regex_t *re;

    (void)luab_core_checkmaxargs(L, 1);

    re = luab_udata(L, 1, &luab_regex_type, regex_t *);

    lua_newtable(L);

    luab_setinteger(L, -2, "re_magic",      re->re_magic);
    luab_setinteger(L, -2, "re_nsub",       re->re_nsub);

    if (re->re_endp != NULL)
        luab_setfstring(L, -2, "re_endp", "(%p)", re->re_endp);

    if (re->re_g != NULL)
        luab_setfstring(L, -2, "re_g", "(%p)", re->re_g);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate regex{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = regex:dump()
 */
static int
REGEX_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, &luab_regex_type, sizeof(regex_t)));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get version.
 *
 * @function re_magic
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = regex:re_magic()
 */
static int
REGEX_re_magic(lua_State *L)
{
    regex_t *re;
    int data;

    (void)luab_core_checkmaxargs(L, 1);

    re = luab_udata(L, 1, &luab_regex_type, regex_t *);
    data = re->re_magic;

    return (luab_pushxinteger(L, data));
}

/***
 * Get number of parenthesized subexpressions.
 *
 * @function re_nsub
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = regex:re_nsub()
 */
static int
REGEX_re_nsub(lua_State *L)
{
    regex_t *re;
    size_t data;

    (void)luab_core_checkmaxargs(L, 1);

    re = luab_udata(L, 1, &luab_regex_type, regex_t *);
    data = re->re_nsub;

    return (luab_pushxinteger(L, data));
}

/***
 * Get base-address of end pointer for REG_PEND.
 *
 * @function re_endp
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = regex:re_endp()
 */
static int
REGEX_re_endp(lua_State *L)
{
    regex_t *re;
    const char *dp;

    (void)luab_core_checkmaxargs(L, 1);

    re = luab_udata(L, 1, &luab_regex_type, regex_t *);
    dp = re->re_endp;

    return (luab_pushfstring(L, "(%p)", dp));
}

/***
 * Get base-address of internal data region.
 *
 * @function re_g
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = regex:re_g()
 */
static int
REGEX_re_g(lua_State *L)
{
    regex_t *re;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    re = luab_udata(L, 1, &luab_regex_type, regex_t *);
    dp = (caddr_t)(re->re_g);

    return (luab_pushfstring(L, "(%p)", dp));
}

/*
 * Meta-methods.
 */

static int
REGEX_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_regex_type));
}

static int
REGEX_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_regex_type));
}

static int
REGEX_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_regex_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t regex_methods[] = {
    LUAB_FUNC("re_magic",       REGEX_re_magic),
    LUAB_FUNC("re_nsub",        REGEX_re_nsub),
    LUAB_FUNC("re_endp",        REGEX_re_endp),
    LUAB_FUNC("re_g",           REGEX_re_g),
    LUAB_FUNC("get",            REGEX_get),
    LUAB_FUNC("dump",           REGEX_dump),
    LUAB_FUNC("__gc",           REGEX_gc),
    LUAB_FUNC("__len",          REGEX_len),
    LUAB_FUNC("__tostring",     REGEX_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
regex_create(lua_State *L, void *arg)
{
    return (luab_new_regex(L, arg));
}

static void
regex_init(void *ud, void *arg)
{
    luab_udata_init(&luab_regex_type, ud, arg);
}

static void *
regex_udata(lua_State *L, int narg)
{
    return (luab_to_regex(L, narg));
}

static luab_table_t *
regex_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    regex_t *x, *y;
    size_t m, n, sz;

    sz = sizeof(regex_t);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (regex_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_regex_type, regex_t *);
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
regex_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    regex_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (regex_t *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_regex_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

luab_module_t luab_regex_type = {
    .m_cookie   = LUAB_REGEX_TYPE_ID,
    .m_name     = LUAB_REGEX_TYPE,
    .m_vec      = regex_methods,
    .m_create   = regex_create,
    .m_init     = regex_init,
    .m_get      = regex_udata,
    .m_get_tbl  = regex_checktable,
    .m_set_tbl  = regex_pushtable,
    .m_sz       = sizeof(luab_regex_t),
};
