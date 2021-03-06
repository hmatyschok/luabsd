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

#include <pwd.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_passwd_type;

/*
 * Interface against
 *
 *  struct passwd {
 *      char    *pw_name;
 *      char    *pw_passwd;
 *      uid_t   pw_uid;
 *      gid_t   pw_gid;
 *      time_t  pw_change;
 *      char    *pw_class;
 *      char    *pw_gecos;
 *      char    *pw_dir;
 *      char    *pw_shell;
 *      time_t  pw_expire;
 *      int pw_fields;
 *  };
 */

typedef struct luab_passwd {
    luab_udata_t    ud_softc;
    struct passwd   ud_pwd;
} luab_passwd_t;

/*
 * Subr.
 */

static void
passwd_fillxtable(lua_State *L, int narg, void *arg)
{
    struct passwd *pwd;

    if ((pwd = (struct passwd *)arg) != NULL) {

        luab_setstring(L, narg, "pw_name",    pwd->pw_name);
        luab_setstring(L, narg, "pw_passwd",  pwd->pw_passwd);
        luab_setinteger(L, narg, "pw_uid",    pwd->pw_uid);
        luab_setinteger(L, narg, "pw_gid",    pwd->pw_gid);
        luab_setinteger(L, narg, "pw_change", pwd->pw_change);
        luab_setstring(L, narg, "pw_class",   pwd->pw_class);
        luab_setstring(L, narg, "pw_gecos",   pwd->pw_gecos);
        luab_setstring(L, narg, "pw_dir",     pwd->pw_dir);
        luab_setstring(L, narg, "pw_shell",   pwd->pw_shell);
        luab_setinteger(L, narg, "pw_expire", pwd->pw_expire);
        luab_setinteger(L, narg, "pw_fields", pwd->pw_fields);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(PASSWD)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              pw_name     = (LUA_T{NIL,STRING}),
 *              pw_passwd   = (LUA_T{NIL,STRING}),
 *              pw_uid      = (LUA_TNUMBER),
 *              pw_gid      = (LUA_TNUMBER),
 *              pw_change   = (LUA_TNUMBER),
 *              pw_class    = (LUA_T{NIL,STRING}),
 *              pw_gecos    = (LUA_T{NIL,STRING}),
 *              pw_dir      = (LUA_T{NIL,STRING}),
 *              pw_shell    = (LUA_T{NIL,STRING}),
 *              pw_expire   = (LUA_TNUMBER),
 *              pw_fields   = (LUA_TNUMBER),
 *          }
 *
 * @usage t = passwd:get_table()
 */
static int
PASSWD_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);

    xtp.xtp_fill = passwd_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate passwd{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = passwd:dump()
 */
static int
PASSWD_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PASSWD, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get user name.
 *
 * @function pw_name
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = passwd:pw_name()
 */
static int
PASSWD_pw_name(lua_State *L)
{
    luab_module_t *m;
    struct passwd *pwd;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);
    pwd = luab_udata(L, 1, m, struct passwd *);
    dp = pwd->pw_name;

    return (luab_pushstring(L, dp));
}

/***
 * Get encrypted password.
 *
 * @function pw_passwd
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = passwd:pw_passwd()
 */
static int
PASSWD_pw_passwd(lua_State *L)
{
    luab_module_t *m;
    struct passwd *pwd;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);
    pwd = luab_udata(L, 1, m, struct passwd *);
    dp = pwd->pw_passwd;

    return (luab_pushstring(L, dp));
}

/***
 * Get user ID.
 *
 * @function pw_uid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = passwd:pw_uid()
 */
static int
PASSWD_pw_uid(lua_State *L)
{
    luab_module_t *m;
    struct passwd *pwd;
    uid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);
    pwd = luab_udata(L, 1, m, struct passwd *);
    x = pwd->pw_uid;

    return (luab_pushxinteger(L, x));
}

/***
 * Get group ID.
 *
 * @function pw_gid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = passwd:pw_gid()
 */
static int
PASSWD_pw_gid(lua_State *L)
{
    luab_module_t *m;
    struct passwd *pwd;
    gid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);
    pwd = luab_udata(L, 1, m, struct passwd *);
    x = pwd->pw_gid;

    return (luab_pushxinteger(L, x));
}

/***
 * Get password change time.
 *
 * @function pw_change
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = passwd:pw_change()
 */
static int
PASSWD_pw_change(lua_State *L)
{
    luab_module_t *m;
    struct passwd *pwd;
    time_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);
    pwd = luab_udata(L, 1, m, struct passwd *);
    x = pwd->pw_change;

    return (luab_pushxinteger(L, x));
}

/***
 * Get user access class.
 *
 * @function pw_class
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = passwd:pw_class()
 */
static int
PASSWD_pw_class(lua_State *L)
{
    luab_module_t *m;
    struct passwd *pwd;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);
    pwd = luab_udata(L, 1, m, struct passwd *);
    dp = pwd->pw_class;

    return (luab_pushstring(L, dp));
}

/***
 * Get Honeywell login info.
 *
 * @function pw_gecos
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = passwd:pw_gecos()
 */
static int
PASSWD_pw_gecos(lua_State *L)
{
    luab_module_t *m;
    struct passwd *pwd;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);
    pwd = luab_udata(L, 1, m, struct passwd *);
    dp = pwd->pw_gecos;

    return (luab_pushstring(L, dp));
}

/***
 * Get home directory.
 *
 * @function pw_dir
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = passwd:pw_dir()
 */
static int
PASSWD_pw_dir(lua_State *L)
{
    luab_module_t *m;
    struct passwd *pwd;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);
    pwd = luab_udata(L, 1, m, struct passwd *);
    dp = pwd->pw_dir;

    return (luab_pushstring(L, dp));
}

/***
 * Get default shell.
 *
 * @function pw_shell
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = passwd:pw_shell()
 */
static int
PASSWD_pw_shell(lua_State *L)
{
    luab_module_t *m;
    struct passwd *pwd;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);
    pwd = luab_udata(L, 1, m, struct passwd *);
    dp = pwd->pw_shell;

    return (luab_pushstring(L, dp));
}

/***
 * Get account expiration.
 *
 * @function pw_expire
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = passwd:pw_expire()
 */
static int
PASSWD_pw_expire(lua_State *L)
{
    luab_module_t *m;
    struct passwd *pwd;
    time_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);
    pwd = luab_udata(L, 1, m, struct passwd *);
    x = pwd->pw_expire;

    return (luab_pushxinteger(L, x));
}

/***
 * Get meta-information about filled in fields.
 *
 * @function pw_fields
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = passwd:pw_fields()
 */
static int
PASSWD_pw_fields(lua_State *L)
{
    luab_module_t *m;
    struct passwd *pwd;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);
    pwd = luab_udata(L, 1, m, struct passwd *);
    x = pwd->pw_fields;

    return (luab_pushxinteger(L, x));
}

/*
 * Meta-methods.
 */

static int
PASSWD_gc(lua_State *L)
{
    luab_module_t *m;
    struct passwd *pwd;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);
    pwd = luab_udata(L, 1, m, struct passwd *);

    luab_core_freestr(pwd->pw_name);
    luab_core_freestr(pwd->pw_passwd);
    luab_core_freestr(pwd->pw_class);
    luab_core_freestr(pwd->pw_gecos);
    luab_core_freestr(pwd->pw_dir);
    luab_core_freestr(pwd->pw_shell);

    return (luab_core_gc(L, 1, m));
}

static int
PASSWD_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PASSWD, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
PASSWD_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PASSWD, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t passwd_methods[] = {
    LUAB_FUNC("pw_name",        PASSWD_pw_name),
    LUAB_FUNC("pw_passwd",      PASSWD_pw_passwd),
    LUAB_FUNC("pw_uid",         PASSWD_pw_uid),
    LUAB_FUNC("pw_gid",         PASSWD_pw_gid),
    LUAB_FUNC("pw_change",      PASSWD_pw_change),
    LUAB_FUNC("pw_class",       PASSWD_pw_class),
    LUAB_FUNC("pw_gecos",       PASSWD_pw_gecos),
    LUAB_FUNC("pw_dir",         PASSWD_pw_dir),
    LUAB_FUNC("pw_shell",       PASSWD_pw_shell),
    LUAB_FUNC("pw_expire",      PASSWD_pw_expire),
    LUAB_FUNC("pw_fields",      PASSWD_pw_fields),
    LUAB_FUNC("get_table",      PASSWD_get_table),
    LUAB_FUNC("dump",           PASSWD_dump),
    LUAB_FUNC("__gc",           PASSWD_gc),
    LUAB_FUNC("__len",          PASSWD_len),
    LUAB_FUNC("__tostring",     PASSWD_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
passwd_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(PASSWD, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
passwd_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(PASSWD, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
passwd_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(PASSWD, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
passwd_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct passwd *x, *y;
    size_t i, j;

    m = luab_xmod(PASSWD, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct passwd *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct passwd *);
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
passwd_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct passwd *x;
    size_t i, j, k;

    m = luab_xmod(PASSWD, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct passwd *)tbl->tbl_vec) != NULL) &&
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
passwd_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(PASSWD, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_passwd_type = {
    .m_id           = LUAB_PASSWD_TYPE_ID,
    .m_name         = LUAB_PASSWD_TYPE,
    .m_vec          = passwd_methods,
    .m_create       = passwd_create,
    .m_init         = passwd_init,
    .m_get          = passwd_udata,
    .m_get_tbl      = passwd_checktable,
    .m_set_tbl      = passwd_pushtable,
    .m_alloc_tbl    = passwd_alloctable,
    .m_len          = sizeof(luab_passwd_t),
    .m_sz           = sizeof(struct passwd),
};
