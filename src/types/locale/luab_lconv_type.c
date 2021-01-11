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

extern luab_module_t luab_lconv_type;

/*
 * Interface against
 *
 *  struct lconv {
 *      char	*decimal_point;
 *      char	*thousands_sep;
 *      char	*grouping;
 *      char	*int_curr_symbol;
 *      char	*currency_symbol;
 *      char	*mon_decimal_point;
 *      char	*mon_thousands_sep;
 *      char	*mon_grouping;
 *      char	*positive_sign;
 *      char	*negative_sign;
 *      char	int_frac_digits;
 *      char	frac_digits;
 *      char	p_cs_precedes;
 *      char	p_sep_by_space;
 *      char	n_cs_precedes;
 *      char	n_sep_by_space;
 *      char	p_sign_posn;
 *      char	n_sign_posn;
 *      char	int_p_cs_precedes;
 *      char	int_n_cs_precedes;
 *      char	int_p_sep_by_space;
 *      char	int_n_sep_by_space;
 *      char	int_p_sign_posn;
 *      char	int_n_sign_posn;
 *  };
 */

typedef struct luab_lconv {
    luab_udata_t    ud_softc;
    struct lconv    ud_lc;
} luab_lconv_t;

/*
 * Subr.
 */

static void
lconv_fillxtable(lua_State *L, int narg, void *arg)
{
    struct lconv *lc;

    if ((lc = (struct lconv *)arg) != NULL) {
        luab_setstring(L, narg, "decimal_point",        lc->thousands_sep);
        luab_setstring(L, narg, "thousands_sep",        lc->thousands_sep);
        luab_setstring(L, narg, "grouping",             lc->grouping);
        luab_setstring(L, narg, "int_curr_symbol",      lc->int_curr_symbol);
        luab_setstring(L, narg, "currency_symbol",      lc->currency_symbol);
        luab_setstring(L, narg, "mon_decimal_point",    lc->mon_decimal_point);
        luab_setstring(L, narg, "mon_thousands_sep",    lc->mon_thousands_sep);
        luab_setstring(L, narg, "mon_grouping",         lc->mon_grouping);
        luab_setstring(L, narg, "positive_sign",        lc->positive_sign);
        luab_setstring(L, narg, "negative_sign",        lc->negative_sign);
        luab_setinteger(L, narg, "int_frac_digits",     lc->int_frac_digits);
        luab_setinteger(L, narg, "frac_digits",         lc->frac_digits);
        luab_setinteger(L, narg, "p_cs_precedes",       lc->p_cs_precedes);
        luab_setinteger(L, narg, "p_sep_by_space",      lc->p_sep_by_space);
        luab_setinteger(L, narg, "n_cs_precedes",       lc->n_cs_precedes);
        luab_setinteger(L, narg, "n_sep_by_space",      lc->n_sep_by_space);
        luab_setinteger(L, narg, "p_sign_posn",         lc->p_sign_posn);
        luab_setinteger(L, narg, "n_sign_posn",         lc->n_sign_posn);
        luab_setinteger(L, narg, "int_p_cs_precedes",   lc->int_p_cs_precedes);
        luab_setinteger(L, narg, "int_n_cs_precedes",   lc->int_n_cs_precedes);
        luab_setinteger(L, narg, "int_p_sep_by_space",  lc->int_p_sep_by_space);
        luab_setinteger(L, narg, "int_n_sep_by_space",  lc->int_n_sep_by_space);
        luab_setinteger(L, narg, "int_p_sign_posn",     lc->int_p_sign_posn);
        luab_setinteger(L, narg, "int_n_sign_posn",     lc->int_n_sign_posn);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(LCONV)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              decimal_point       = (LUA_T{NIL,STRING}),
 *              thousands_sep       = (LUA_T{NIL,STRING}),
 *              grouping            = (LUA_T{NIL,STRING}),
 *              int_curr_symbol     = (LUA_T{NIL,STRING}),
 *              currency_symbol     = (LUA_T{NIL,STRING}),
 *              mon_decimal_point   = (LUA_T{NIL,STRING}),
 *              mon_thousands_sep   = (LUA_T{NIL,STRING}),
 *              mon_grouping        = (LUA_T{NIL,STRING}),
 *              positive_sign       = (LUA_T{NIL,STRING}),
 *              negative_sign       = (LUA_T{NIL,STRING}),
 *              int_frac_digits     = (LUA_TNUMBER),
 *              frac_digits         = (LUA_TNUMBER),
 *              p_cs_precedes       = (LUA_TNUMBER),
 *              p_sep_by_space      = (LUA_TNUMBER),
 *              n_cs_precedes       = (LUA_TNUMBER),
 *              n_sep_by_space      = (LUA_TNUMBER),
 *              p_sign_posn         = (LUA_TNUMBER),
 *              n_sign_posn         = (LUA_TNUMBER),
 *              int_p_cs_precedes   = (LUA_TNUMBER),
 *              int_n_cs_precedes   = (LUA_TNUMBER),
 *              int_p_sep_by_space  = (LUA_TNUMBER),
 *              int_n_sep_by_space  = (LUA_TNUMBER),
 *              int_p_sign_posn     = (LUA_TNUMBER),
 *              int_n_sign_posn     = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = lconv:get_table()
 */
static int
LCONV_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);

    xtp.xtp_fill = lconv_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate lconv{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = lconv:dump()
 */
static int
LCONV_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LCONV, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get decimal point character.
 *
 * @function decimal_point
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:decimal_point()
 */
static int
LCONV_decimal_point(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    dp = lc->decimal_point;

    return (luab_pushstring(L, dp));
}

/***
 * Get thousands separator.
 *
 * @function thousands_sep
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:thousands_sep()
 */
static int
LCONV_thousands_sep(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    dp = lc->thousands_sep;

    return (luab_pushstring(L, dp));
}

/***
 * Get grouping.
 *
 * @function grouping
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:grouping()
 */
static int
LCONV_grouping(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    dp = lc->grouping;

    return (luab_pushstring(L, dp));
}

/***
 * Get currency symbol from ISO 4217 and its seperator.
 *
 * @function int_curr_symbol
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:int_curr_symbol()
 */
static int
LCONV_int_curr_symbol(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    dp = lc->int_curr_symbol;

    return (luab_pushstring(L, dp));
}

/***
 * Get local currency symbol.
 *
 * @function currency_symbol
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:currency_symbol()
 */
static int
LCONV_currency_symbol(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    dp = lc->currency_symbol;

    return (luab_pushstring(L, dp));
}

/***
 * Get decimal point character.
 *
 * @function mon_decimal_point
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:mon_decimal_point()
 */
static int
LCONV_mon_decimal_point(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    dp = lc->mon_decimal_point;

    return (luab_pushstring(L, dp));
}

/***
 * Get thousands separator.
 *
 * @function mon_thousands_sep
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:mon_thousands_sep()
 */
static int
LCONV_mon_thousands_sep(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    dp = lc->mon_thousands_sep;

    return (luab_pushstring(L, dp));
}

/***
 * Get separator.
 *
 * @function mon_grouping
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:mon_grouping()
 */
static int
LCONV_mon_grouping(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    dp = lc->mon_grouping;

    return (luab_pushstring(L, dp));
}

/***
 * Get sign for positive values.
 *
 * @function positive_sign
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:positive_sign()
 */
static int
LCONV_positive_sign(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    dp = lc->positive_sign;

    return (luab_pushstring(L, dp));
}

/***
 * Get sign for negative values.
 *
 * @function negative_sign
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:negative_sign()
 */
static int
LCONV_negative_sign(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    dp = lc->negative_sign;

    return (luab_pushstring(L, dp));
}

/***
 * Get international fractional digits.
 *
 * @function int_frac_digits
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:int_frac_digits()
 */
static int
LCONV_int_frac_digits(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    char x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    x = lc->int_frac_digits;

    return (luab_pushxinteger(L, x));
}

/***
 * Get local fractional digits.
 *
 * @function frac_digits
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:frac_digits()
 */
static int
LCONV_frac_digits(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    char x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    x = lc->frac_digits;

    return (luab_pushxinteger(L, x));
}

/***
 * Get status, if currency symbol precedes positive value.
 *
 * @function p_cs_precedes
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:p_cs_precedes()
 */
static int
LCONV_p_cs_precedes(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    char x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    x = lc->p_cs_precedes;

    return (luab_pushxinteger(L, x));
}

/***
 * Get status, if space separates currency_symbol from positive value.
 *
 * @function p_sep_by_space
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:p_sep_by_space()
 */
static int
LCONV_p_sep_by_space(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    char x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    x = lc->p_sep_by_space;

    return (luab_pushxinteger(L, x));
}

/***
 * Get status, if currency_symbol precedes a negative value.
 *
 * @function n_cs_precedes
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:n_cs_precedes()
 */
static int
LCONV_n_cs_precedes(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    char x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    x = lc->n_cs_precedes;

    return (luab_pushxinteger(L, x));
}

/***
 * Get status, if a space separates currency_symbol from negative value.
 *
 * @function n_sep_by_space
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:n_sep_by_space()
 */
static int
LCONV_n_sep_by_space(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    char x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    x = lc->n_sep_by_space;

    return (luab_pushxinteger(L, x));
}

/***
 * Get positive sign position.
 *
 * @function p_sign_posn
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:p_sign_posn()
 */
static int
LCONV_p_sign_posn(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    char x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    x = lc->p_sign_posn;

    return (luab_pushxinteger(L, x));
}

/***
 * Get negative sign position.
 *
 * @function n_sign_posn
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = lconv:n_sign_posn()
 */
static int
LCONV_n_sign_posn(lua_State *L)
{
    luab_module_t *m;
    struct lconv *lc;
    char x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(LCONV, TYPE, __func__);
    lc = luab_udata(L, 1, m, struct lconv *);
    x = lc->n_sign_posn;

    return (luab_pushxinteger(L, x));
}

/*
 * Meta-methods.
 */

static int
LCONV_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LCONV, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
LCONV_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LCONV, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
LCONV_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LCONV, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t lconv_methods[] = {
    LUAB_FUNC("n_sign_posn",        LCONV_decimal_point),
    LUAB_FUNC("thousands_sep",      LCONV_thousands_sep),
    LUAB_FUNC("grouping",           LCONV_grouping),
    LUAB_FUNC("int_curr_symbol",    LCONV_int_curr_symbol),
    LUAB_FUNC("currency_symbol",    LCONV_currency_symbol),
    LUAB_FUNC("mon_decimal_point",  LCONV_mon_decimal_point),
    LUAB_FUNC("mon_thousands_sep",  LCONV_mon_thousands_sep),
    LUAB_FUNC("mon_grouping",       LCONV_mon_grouping),
    LUAB_FUNC("positive_sign",      LCONV_positive_sign),
    LUAB_FUNC("negative_sign",      LCONV_negative_sign),
    LUAB_FUNC("int_frac_digits",    LCONV_int_frac_digits),
    LUAB_FUNC("frac_digits",        LCONV_frac_digits),
    LUAB_FUNC("p_cs_precedes",      LCONV_p_cs_precedes),
    LUAB_FUNC("p_sep_by_space",     LCONV_p_sep_by_space),
    LUAB_FUNC("n_cs_precedes",      LCONV_n_cs_precedes),
    LUAB_FUNC("n_sep_by_space",     LCONV_n_sep_by_space),
    LUAB_FUNC("p_sign_posn",        LCONV_p_sign_posn),
    LUAB_FUNC("n_sign_posn",        LCONV_n_sign_posn),
    LUAB_FUNC("get_table",          LCONV_get_table),
    LUAB_FUNC("dump",               LCONV_dump),
    LUAB_FUNC("__gc",               LCONV_gc),
    LUAB_FUNC("__len",              LCONV_len),
    LUAB_FUNC("__tostring",         LCONV_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
lconv_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(LCONV, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
lconv_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(LCONV, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
lconv_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(LCONV, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
lconv_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct lconv *x, *y;
    size_t i, j;

    m = luab_xmod(LCONV, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct lconv *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct lconv *);
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
lconv_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct lconv *x;
    size_t i, j, k;

    m = luab_xmod(LCONV, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct lconv *)tbl->tbl_vec) != NULL) &&
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
lconv_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(LCONV, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_lconv_type = {
    .m_id           = LUAB_LCONV_TYPE_ID,
    .m_name         = LUAB_LCONV_TYPE,
    .m_vec          = lconv_methods,
    .m_create       = lconv_create,
    .m_init         = lconv_init,
    .m_get          = lconv_udata,
    .m_get_tbl      = lconv_checktable,
    .m_set_tbl      = lconv_pushtable,
    .m_alloc_tbl    = lconv_alloctable,
    .m_len          = sizeof(luab_lconv_t),
    .m_sz           = sizeof(struct lconv),
};
