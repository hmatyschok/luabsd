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

#include <pwd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

extern luab_module_t passwd_type;

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

#define luab_new_passwd(L, arg) \
    ((luab_passwd_t *)luab_newudata(L, &passwd_type, (arg)))
#define luab_to_passwd(L, narg) \
    (luab_toldata((L), (narg), &passwd_type, \
        struct passwd *, sizeof(struct passwd)))

#define LUAB_PASSWD_TYPE_ID    1604262109
#define LUAB_PASSWD_TYPE    "PASSWD*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(PASSWD)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              pw_name     = (LUA_TSTRING),
 *              pw_passwd   = (LUA_TSTRING),
 *              pw_uid      = (LUA_TNUMBER),
 *              pw_gid      = (LUA_TNUMBER),
 *              pw_change   = (LUA_TNUMBER),
 *              pw_class    = (LUA_TSTRING),
 *              pw_gecos    = (LUA_TSTRING),
 *              pw_dir      = (LUA_TSTRING),
 *              pw_shell    = (LUA_TSTRING),
 *              pw_expire   = (LUA_TNUMBER),
 *              pw_fields   = (LUA_TNUMBER),
 *          }
 *
 * @usage t = passwd:get()
 */
static int
PASSWD_get(lua_State *L)
{
    struct passwd *pwd;

    (void)luab_core_checkmaxargs(L, 1);

    pwd = luab_udata(L, 1, &passwd_type, struct passwd *);

    lua_newtable(L);
    luab_setstring(L, -2, "pw_name",    pwd->pw_name);
    luab_setstring(L, -2, "pw_passwd",  pwd->pw_passwd);
    luab_setinteger(L, -2, "pw_uid",    pwd->pw_uid);
    luab_setinteger(L, -2, "pw_gid",    pwd->pw_gid);
    luab_setinteger(L, -2, "pw_change", pwd->pw_change);
    luab_setstring(L, -2, "pw_class",   pwd->pw_class);
    luab_setstring(L, -2, "pw_gecos",   pwd->pw_gecos);
    luab_setstring(L, -2, "pw_dir",     pwd->pw_dir);
    luab_setstring(L, -2, "pw_shell",   pwd->pw_shell);
    luab_setinteger(L, -2, "pw_expire", pwd->pw_expire);
    luab_setinteger(L, -2, "pw_fields", pwd->pw_fields);
    lua_pushvalue(L, -1);

    return (1);
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
    return (luab_core_dump(L, 1, &passwd_type, sizeof(struct passwd)));
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
 * @usage data [, err, msg ] = passwd:pw_name()
 */
static int
PASSWD_pw_name(lua_State *L)
{
    struct passwd *pwd;
    char *data;

    (void)luab_core_checkmaxargs(L, 2);

    pwd = luab_udata(L, 1, &passwd_type, struct passwd *);
    data = pwd->pw_name;

    return (luab_pushstring(L, data));
}

/***
 * Get encrypted password.
 *
 * @function pw_passwd
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = passwd:pw_passwd()
 */
static int
PASSWD_pw_passwd(lua_State *L)
{
    struct passwd *pwd;
    char *data;

    (void)luab_core_checkmaxargs(L, 2);

    pwd = luab_udata(L, 1, &passwd_type, struct passwd *);
    data = pwd->pw_passwd;

    return (luab_pushstring(L, data));
}

/***
 * Get user ID.
 *
 * @function pw_uid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = passwd:pw_uid()
 */
static int
PASSWD_pw_uid(lua_State *L)
{
    struct passwd *pwd;
    uid_t data;

    (void)luab_core_checkmaxargs(L, 2);

    pwd = luab_udata(L, 1, &passwd_type, struct passwd *);
    data = pwd->pw_uid;

    return (luab_pusherr(L, data));
}

/***
 * Get group ID.
 *
 * @function pw_gid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = passwd:pw_gid()
 */
static int
PASSWD_pw_gid(lua_State *L)
{
    struct passwd *pwd;
    gid_t data;

    (void)luab_core_checkmaxargs(L, 2);

    pwd = luab_udata(L, 1, &passwd_type, struct passwd *);
    data = pwd->pw_gid;

    return (luab_pusherr(L, data));
}

/***
 * Get password change time.
 *
 * @function pw_change
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = passwd:pw_change()
 */
static int
PASSWD_pw_change(lua_State *L)
{
    struct passwd *pwd;
    time_t data;

    (void)luab_core_checkmaxargs(L, 2);

    pwd = luab_udata(L, 1, &passwd_type, struct passwd *);
    data = pwd->pw_change;

    return (luab_pusherr(L, data));
}

/***
 * Get user access class.
 *
 * @function pw_class
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = passwd:pw_class()
 */
static int
PASSWD_pw_class(lua_State *L)
{
    struct passwd *pwd;
    char *data;

    (void)luab_core_checkmaxargs(L, 2);

    pwd = luab_udata(L, 1, &passwd_type, struct passwd *);
    data = pwd->pw_class;

    return (luab_pushstring(L, data));
}

/***
 * Get Honeywell login info.
 *
 * @function pw_gecos
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = passwd:pw_gecos()
 */
static int
PASSWD_pw_gecos(lua_State *L)
{
    struct passwd *pwd;
    char *data;

    (void)luab_core_checkmaxargs(L, 2);

    pwd = luab_udata(L, 1, &passwd_type, struct passwd *);
    data = pwd->pw_gecos;

    return (luab_pushstring(L, data));
}

/***
 * Get home directory.
 *
 * @function pw_dir
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = passwd:pw_dir()
 */
static int
PASSWD_pw_dir(lua_State *L)
{
    struct passwd *pwd;
    char *data;

    (void)luab_core_checkmaxargs(L, 2);

    pwd = luab_udata(L, 1, &passwd_type, struct passwd *);
    data = pwd->pw_dir;

    return (luab_pushstring(L, data));
}

/***
 * Get default shell.
 *
 * @function pw_shell
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = passwd:pw_shell()
 */
static int
PASSWD_pw_shell(lua_State *L)
{
    struct passwd *pwd;
    char *data;

    (void)luab_core_checkmaxargs(L, 2);

    pwd = luab_udata(L, 1, &passwd_type, struct passwd *);
    data = pwd->pw_shell;

    return (luab_pushstring(L, data));
}

/***
 * Get account expiration.
 *
 * @function pw_expire
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = passwd:pw_expire()
 */
static int
PASSWD_pw_expire(lua_State *L)
{
    struct passwd *pwd;
    time_t data;

    (void)luab_core_checkmaxargs(L, 2);

    pwd = luab_udata(L, 1, &passwd_type, struct passwd *);
    data = pwd->pw_expire;

    return (luab_pusherr(L, data));
}

/***
 * Get meta-information about filled in fields.
 *
 * @function pw_fields
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = passwd:pw_fields()
 */
static int
PASSWD_pw_fields(lua_State *L)
{
    struct passwd *pwd;
    int data;

    (void)luab_core_checkmaxargs(L, 2);

    pwd = luab_udata(L, 1, &passwd_type, struct passwd *);
    data = pwd->pw_fields;

    return (luab_pusherr(L, data));
}

/*
 * Meta-methods.
 */

static int
PASSWD_gc(lua_State *L)
{
    struct passwd *pwd;

    (void)luab_core_checkmaxargs(L, 1);

    pwd = luab_udata(L, 1, &passwd_type, struct passwd *);

    luab_core_freestr(pwd->pw_name);
    luab_core_freestr(pwd->pw_passwd);
    luab_core_freestr(pwd->pw_class);
    luab_core_freestr(pwd->pw_gecos);
    luab_core_freestr(pwd->pw_dir);
    luab_core_freestr(pwd->pw_shell);

    return (luab_core_gc(L, 1, &passwd_type));
}

static int
PASSWD_len(lua_State *L)
{
    return (luab_core_len(L, 2, &passwd_type));
}

static int
PASSWD_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &passwd_type));
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
    LUAB_FUNC("get",            PASSWD_get),
    LUAB_FUNC("dump",           PASSWD_dump),
    LUAB_FUNC("__gc",           PASSWD_gc),
    LUAB_FUNC("__len",          PASSWD_len),
    LUAB_FUNC("__tostring",     PASSWD_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
passwd_create(lua_State *L, void *arg)
{
    return (luab_new_passwd(L, arg));
}

static void
passwd_init(void *ud, void *arg)
{
    luab_udata_init(&passwd_type, ud, arg);
}

static void *
passwd_udata(lua_State *L, int narg)
{
    return (luab_to_passwd(L, narg));
}

luab_module_t passwd_type = {
    .m_cookie   = LUAB_PASSWD_TYPE_ID,
    .m_name     = LUAB_PASSWD_TYPE,
    .m_vec      = passwd_methods,
    .m_create   = passwd_create,
    .m_init     = passwd_init,
    .m_get      = passwd_udata,
    .m_sz       = sizeof(luab_passwd_t),
};
