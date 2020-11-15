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

#define LUAB_REGEX_LIB_ID    1604366574
#define LUAB_REGEX_LIB_KEY    "regex"

extern luab_module_t luab_regex_lib;

/*
 * Subr.
 */

static luab_table_t *
luab_table_checkregmatch(lua_State *L, int narg)
{
    luab_table_t *tbl;
    regmatch_t *x, *rm;
    size_t m, n, sz;

    sz = sizeof(regmatch_t);

    if ((tbl = luab_newvector(L, narg, sz)) != NULL) {

        if (((x = (regmatch_t *)(tbl->tbl_vec)) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        rm = luab_udata(L, -1, luab_mx(REGMATCH), regmatch_t *);
                        (void)memmove(&(x[m]), rm, sz);
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
luab_table_pushregmatch(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    regmatch_t *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, luab_mx(REGMATCH), k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

/*
 * Service primitives.
 */

/***
 * regcomp(3) - regular-expression library
 *
 * @function regcomp
 *
 * @param preg              Storage for compiled regular-expression,
 *                          instance of (LUA_TUSERDATA(REGEX)).
 * @param pattern           Specifies the regular-expression.
 * @param cflags            Values are constructed over
 *
 *                              bsd.regex.REG_{
 *                                  EXTENDED,
 *                                  BASIC,
 *                                  NOSPEC,
 *                                  ICASE,
 *                                  NOSUB,
 *                                  NEWLINE,
 *                                  PEND
 *                              }
 *
 *                          by inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.regex.regcomp(preg, pattern, cflags)
 */
static int
luab_regcomp(lua_State *L)
{
    regex_t *preg;
    const char *pattern;
    int cflags, status;

    (void)luab_core_checkmaxargs(L, 3);

    preg = luab_udata(L, 1, luab_mx(REGEX), regex_t *);
    pattern = luab_checklstring(L, 2, luab_buf_nmax);
    cflags = (int)luab_checkinteger(L, 3, INT_MAX);

    status = regcomp(preg, pattern, cflags);

    return (luab_pusherr(L, status));
}

/***
 * regexec(3) - regular-expression library
 *
 * @function regexec
 *
 * @param preg              Storage for compiled regular-expression,
 *                          instance of (LUA_TUSERDATA(REGEX)).
 * @param string            Specifies string matches against regular-expression.
 * @param nmatch            Specifies maximum number of records over pmatch.
 * @param pmatch            Specifies by nmatch constrained array
 *
 *                              {
 *                                  regmatch0,
 *                                  regmatch1,
 *                                      ...
 *                                  regmatchN
 *                              }
 *
 *                          over (LUA_TUSERDATA(REGMATCH)).
 *
 * @param eflags            Values are constructed over
 *
 *                              bsd.regex.REG_{
 *                                  NOTBOL,
 *                                  NOTEOL,
 *                                  STARTEND
 *                              }
 *
 *                          by inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.regex.regexec(preg, string, nmatch, pmatch, eflags)
 */
static int
luab_regexec(lua_State *L)
{
    regex_t *preg;
    const char *string;
    size_t nmatch;
    luab_table_t *tbl;
    int eflags;
    regmatch_t *pmatch;
    int status;

    (void)luab_core_checkmaxargs(L, 5);

    preg = luab_udata(L, 1, luab_mx(REGEX), regex_t *);
    string = luab_checklstring(L, 2, luab_buf_nmax);
    nmatch = (size_t)luab_checklinteger(L, 3);
    tbl = luab_table_checkregmatch(L, 4);
    eflags = (int)luab_checkinteger(L, 5, INT_MAX);

    if (tbl != NULL) {
        pmatch = (regmatch_t *)(tbl->tbl_vec);
        status = regexec(preg, string, nmatch, pmatch, eflags);
        luab_table_pushregmatch(L, 4, tbl, 0, 1);
    } else
        status = REG_ESPACE;

    return (luab_pusherr(L, status));
}

/***
 * regerror(3) - regular-expression library
 *
 * @function regerror
 *
 * @param errcode           Specifies error code from
 *
 *                              bsd.regex.REG_{
 *                                  NOMATCH,
 *                                  BADPAT,
 *                                  ECOLLATE,
 *                                  ECTYPE,
 *                                  EESCAPE,
 *                                  ESUBREG,
 *                                  EBRACK,
 *                                  EPAREN,
 *                                  EBRACE,
 *                                  BADBR,
 *                                  ERANGE,
 *                                  ESPACE,
 *                                  BADRPT,
 *                                  EMPTY,
 *                                  ASSERT,
 *                                  INVARG,
 *                                  ILLSEQ
 *                              }.
 *
 * @param preg              Storage for compiled regular-expression,
 *                          instance of (LUA_TUSERDATA(REGEX)).
 * @param errbuf            Result argument, storage for error message,
 *                          instance of (LUA_TUSERDATA(IOVEC)).
 * @param errbuf_size       Constraint.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [ err, msg ] = bsd.regex.regerror(errcode, preg, errbuf, errbuf_size)
 */
static int
luab_regerror(lua_State *L)
{
    int errcode;
    regex_t *preg;
    luab_iovec_t *buf;
    size_t errbuf_size;
    caddr_t bp;
    size_t len;

    (void)luab_core_checkmaxargs(L, 4);

    errcode = (int)luab_checkinteger(L, 1, INT_MAX);
    preg = luab_udata(L, 2, luab_mx(REGEX), regex_t *);
    buf = luab_udata(L, 3, luab_mx(IOVEC), luab_iovec_t *);
    errbuf_size = (size_t)luab_checklinteger(L, 4);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_buf_nmax) &&
        (errbuf_size <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((len = regerror(errcode, preg, bp, errbuf_size)) != 0)
                buf->iov.iov_len = len;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            len = 0;
        }
    } else {
        errno = ERANGE;
        len = 0;
    }
    return (luab_pusherr(L, len));
}

/***
 * regfree(3) - regular-expression library
 *
 * @function regfree
 *
 * @param preg              Storage for compiled regular-expression,
 *                          instance of (LUA_TUSERDATA(REGEX)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [ err, msg ] = bsd.regex.regfree(preg)
 */
static int
luab_regfree(lua_State *L)
{
    regex_t *preg;

    (void)luab_core_checkmaxargs(L, 1);

    preg = luab_udata(L, 1, luab_mx(REGEX), regex_t *);
    regfree(preg);

    return (luab_pusherr(L, 0));
}

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(REGEX)).
 *
 * @function regex_create
 *
 * @param regex             Instance of (LUA_TUSERDATA(REGEX)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage regex [, err, msg ] = bsd.regex.regex_create([ regex ])
 */
static int
luab_regex_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(REGEX), NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(REGMATCH)).
 *
 * @function regmatch_create
 *
 * @param regmatch          Instance of (LUA_TUSERDATA(REGMATCH)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage regmatch [, err, msg ] = bsd.regex.regmatch_create([ regmatch ])
 */
static int
luab_regmatch_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(REGMATCH), NULL));
}

/*
 * Interface against <regex.h>.
 */

static luab_module_table_t luab_regex_vec[] = { /* regex.h */
    LUAB_INT("REG_BASIC",           REG_BASIC),
    LUAB_INT("REG_EXTENDED",        REG_EXTENDED),
    LUAB_INT("REG_ICASE",           REG_ICASE),
    LUAB_INT("REG_NOSUB",           REG_NOSUB),
    LUAB_INT("REG_NEWLINE",         REG_NEWLINE),
    LUAB_INT("REG_NOSPEC",          REG_NOSPEC),
    LUAB_INT("REG_PEND",            REG_PEND),
    LUAB_INT("REG_DUMP",            REG_DUMP),
    LUAB_INT("REG_ENOSYS",          REG_ENOSYS),
    LUAB_INT("REG_NOMATCH",         REG_NOMATCH),
    LUAB_INT("REG_BADPAT",          REG_BADPAT),
    LUAB_INT("REG_ECOLLATE",        REG_ECOLLATE),
    LUAB_INT("REG_ECTYPE",          REG_ECTYPE),
    LUAB_INT("REG_EESCAPE",         REG_EESCAPE),
    LUAB_INT("REG_ESUBREG",         REG_ESUBREG),
    LUAB_INT("REG_EBRACK",          REG_EBRACK),
    LUAB_INT("REG_EPAREN",          REG_EPAREN),
    LUAB_INT("REG_EBRACE",          REG_EBRACE),
    LUAB_INT("REG_BADBR",           REG_BADBR),
    LUAB_INT("REG_ERANGE",          REG_ERANGE),
    LUAB_INT("REG_ESPACE",          REG_ESPACE),
    LUAB_INT("REG_BADRPT",          REG_BADRPT),
    LUAB_INT("REG_EMPTY",           REG_EMPTY),
    LUAB_INT("REG_ASSERT",          REG_ASSERT),
    LUAB_INT("REG_INVARG",          REG_INVARG),
    LUAB_INT("REG_ILLSEQ",          REG_ILLSEQ),
    LUAB_INT("REG_ATOI",            REG_ATOI),
    LUAB_INT("REG_ITOA",            REG_ITOA),
    LUAB_INT("REG_NOTBOL",          REG_NOTBOL),
    LUAB_INT("REG_NOTEOL",          REG_NOTEOL),
    LUAB_INT("REG_STARTEND",        REG_STARTEND),
    LUAB_INT("REG_TRACE",           REG_TRACE),
    LUAB_INT("REG_LARGE",           REG_LARGE),
    LUAB_INT("REG_BACKR",           REG_BACKR),
    LUAB_FUNC("regcomp",            luab_regcomp),
    LUAB_FUNC("regexec",            luab_regexec),
    LUAB_FUNC("regerror",           luab_regerror),
    LUAB_FUNC("regfree",            luab_regfree),
    LUAB_FUNC("regex_create",       luab_regex_create),
    LUAB_FUNC("regmatch_create",    luab_regmatch_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_regex_lib = {
    .m_cookie   = LUAB_REGEX_LIB_ID,
    .m_name     = LUAB_REGEX_LIB_KEY,
    .m_vec      = luab_regex_vec,
};
