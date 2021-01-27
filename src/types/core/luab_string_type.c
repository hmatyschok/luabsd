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

extern luab_module_t luab_string_type;

/*
 * Interface against
 *
 *  caddr_t
 *
 */

typedef struct luab_string {
    luab_udata_t    ud_softc;
    caddr_t         ud_str;
    size_t          ud_len;
} luab_string_t;

/*
 * Subr.
 */

static void
string_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_string_t *self;

    if ((self = (luab_string_t *)arg) != NULL) {
        luab_setstring(L, narg, "value",    self->ud_str);
        luab_setinteger(L, narg, "length",  self->ud_len);
   } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(STRING)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              value   = (LUA_T{NIL,STRING}),
 *              length  = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = string:get_table()
 */
static int
STRING_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STRING, TYPE, __func__);

    xtp.xtp_fill = string_fillxtable;
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
 * @usage iovec [, err, msg ] = string:dump()
 */
static int
STRING_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get string.
 *
 * @function value
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = string:value()
 */
static int
STRING_value(lua_State *L)
{
    luab_module_t *m;
    luab_string_t *self;
    caddr_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STRING, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_string_t *);
    x = self->ud_str;

    return (luab_pushstring(L, x));
}

/***
 * Get length of string.
 *
 * @function length
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = string:length()
 */
static int
STRING_length(lua_State *L)
{
    luab_module_t *m;
    luab_string_t *self;
    size_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STRING, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_string_t *);
    x = self->ud_len;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
STRING_gc(lua_State *L)
{
    luab_module_t *m;
    luab_string_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(STRING, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_string_t *);

    luab_core_freestr(self->ud_str);

    return (luab_core_gc(L, 1, m));
}

static int
STRING_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STRING, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
STRING_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(STRING, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t string_methods[] = {
    LUAB_FUNC("value",          STRING_value),
    LUAB_FUNC("length",         STRING_length),
    LUAB_FUNC("get_table",      STRING_get_table),
    LUAB_FUNC("dump",           STRING_dump),
    LUAB_FUNC("__gc",           STRING_gc),
    LUAB_FUNC("__len",          STRING_len),
    LUAB_FUNC("__tostring",     STRING_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
string_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(STRING, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
string_init(void *ud, void *arg)
{
    luab_module_t *m;
    luab_string_t *self;

    m = luab_xmod(STRING, TYPE, __func__);

    if ((self = (luab_string_t *)ud) != NULL) {

        if ((self->ud_str = (caddr_t)arg) != NULL)
            self->ud_len = strnlen(self->ud_str, luab_env_buf_max);
        else
            self->ud_len = 0;
    } else
        errno = EINVAL;
}

static void *
string_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_string_t *self;

    m = luab_xmod(STRING, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_string_t *);
    return ((void *)self->ud_str);
}

static luab_table_t *
string_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    caddr_t *x, y;
    size_t i, j;

    m = luab_xmod(STRING, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (caddr_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_type(L, -1) != -1)) {
                        y = luab_checklxstring(L, -1, luab_env_buf_max, NULL);
                        x[i] = (caddr_t)y;
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
string_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    caddr_t *x;
    size_t i, j, k;

    if (tbl != NULL) {

        if (((x = (caddr_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, new);

            for (i = 0, j = tbl->tbl_card, k = 1; i < j; i++, k++)
                luab_rawsetstring(L, narg, k, x[i]);

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

static luab_table_t *
string_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(STRING, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_string_type = {
    .m_id           = LUAB_STRING_TYPE_ID,
    .m_name         = LUAB_STRING_TYPE,
    .m_vec          = string_methods,
    .m_create       = string_create,
    .m_init         = string_init,
    .m_get          = string_udata,
    .m_get_tbl      = string_checktable,
    .m_set_tbl      = string_pushtable,
    .m_alloc_tbl    = string_alloctable,
    .m_len          = sizeof(luab_string_t),
    .m_sz           = sizeof(caddr_t),
};
