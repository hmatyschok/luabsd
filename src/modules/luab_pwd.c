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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

/*
 * XXX
 *
 *      int  pwcache_userdb(int (*)(int), void (*)(void),
 *              struct passwd * (*)(const char *),
 *              struct passwd * (*)(uid_t));
 *
 *  Implementation depends on specific luab_type(3) implements autoboxing
 *  for (LUA_TCFUNCTION) objects or set of callbacks over vector table.
 */

#define LUAB_PWD_LIB_ID    1604247804
#define LUAB_PWD_LIB_KEY    "pwd"

extern luab_module_t luab_pwd_lib;

/*
 * Service primitives.
 */

/***
 * getpwnam(3) - password database operations
 *
 * @function getpwnam
 *
 * @param login             Specifies user name.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage passwd [, err, msg ] = bsd.pwd.getpwnam(login)
 */
static int
luab_getpwnam(lua_State *L)
{
    luab_module_t *m;
    const char *login;
    struct passwd *pwd;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(PASSWD, TYPE, __func__);
    login = luab_checklstring(L, 1, luab_env_logname_max, NULL);

    if ((pwd = getpwnam(login)) == NULL)
        m = NULL;

    return (luab_pushxdata(L, m, pwd));
}

/***
 * getpwuid(3) - password database operations
 *
 * @function getpwuid
 *
 * @param uid               Specifies user ID.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage passwd [, err, msg ] = bsd.pwd.getpwuid(uid)
 */
static int
luab_getpwuid(lua_State *L)
{
    luab_module_t *m0, *m1;
    uid_t uid;
    struct passwd *pwd;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(UID, TYPE, __func__);
    m1 = luab_xmod(PASSWD, TYPE, __func__);

    uid = (uid_t)luab_checkxinteger(L, 1, m0 ,luab_env_int_max);

    if ((pwd = getpwuid(uid)) == NULL)
        m1 = NULL;

    return (luab_pushxdata(L, m1, pwd));
}

#if __XSI_VISIBLE >= 500
/***
 * endpwent(3) - password database operations
 *
 * @function endpwent
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pwd.endpwent()
 */
static int
luab_endpwent(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);

    endpwent();
    return (luab_pushxinteger(L, luab_env_success));
}

/***
 * getpwent(3) - password database operations
 *
 * @function getpwent
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage passwd [, err, msg ] = bsd.pwd.getpwent()
 */
static int
luab_getpwent(lua_State *L)
{
    luab_module_t *m;
    struct passwd *pwd;

    (void)luab_core_checkmaxargs(L, 0);

    m = luab_xmod(PASSWD, TYPE, __func__);

    if ((pwd = getpwent()) == NULL)
        m = NULL;

    return (luab_pushxdata(L, m, pwd));
}

/***
 * setpwent(3) - password database operations
 *
 * @function setpwent
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pwd.setpwent()
 */
static int
luab_setpwent(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);

    setpwent();
    return (luab_pushxinteger(L, luab_env_success));
}
#endif /* __XSI_VISIBLE */

#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE >= 500
/***
 * getpwnam_r(3) - password database operations
 *
 * @function getpwnam_r
 *
 * @param name              Specifies user name.
 * @param pwd               Storage, instance of (LUA_TUSERDATA(PASSWD)).
 * @param buffer            Line buffer, instance of (LUA_TUSERDATA(IOVEC)),
 *                          see passwd(5) for further details.
 * @param bufsize           By _SC_GETPW_R_SIZE_MAX over sysconf(3) determined
 *                          size in bytes to hold at least one line by passwd(5)
 *                          specified format.
 * @param result            Result argument, instance of (LUA_TUSERDATA(PASSWD)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pwd.getpwnam_r(name, pwd, buffer, bufsize, result)
 */
static int
luab_getpwnam_r(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    const char *name;
    struct passwd *pwd;
    luab_iovec_t *buf;
    size_t bufsize;
    struct passwd *ret;
    caddr_t bp;
    struct passwd *result;
    int status;

    (void)luab_core_checkmaxargs(L, 5);

    m0 = luab_xmod(PASSWD, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);
    m2 = luab_xmod(SIZE, TYPE, __func__);

    name = luab_checklstring(L, 1, luab_env_logname_max, NULL);
    pwd = luab_udata(L, 2, m0, struct passwd *);
    buf = luab_udata(L, 3, m1, luab_iovec_t *);
    bufsize = (size_t)luab_checklxinteger(L, 4, m2, 0);
    ret = luab_udata(L, 5, m0, struct passwd *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (bufsize <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = getpwnam_r(name, pwd, bp, bufsize, &result)) != 0)
                (void)memmove(ret, result, sizeof(struct passwd));

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = luab_env_success;
        }
    } else {
        errno = ERANGE;
        status = luab_env_success;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * getpwuid_r(3) - password database operations
 *
 * @function getpwuid_r
 *
 * @param uid               Specifies user ID.
 * @param pwd               Storage, instance of (LUA_TUSERDATA(PASSWD)).
 * @param buffer            Line buffer, instance of (LUA_TUSERDATA(IOVEC)),
 *                          see passwd(5) for further details.
 * @param bufsize           By _SC_GETPW_R_SIZE_MAX over sysconf(3) determined
 *                          size in bytes to hold at least one line by passwd(5)
 *                          specified format.
 * @param result            Result argument, instance of (LUA_TUSERDATA(PASSWD)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pwd.getpwuid_r(uid, pwd, buffer, bufsize, result)
 */
static int
luab_getpwuid_r(lua_State *L)
{
    luab_module_t *m0, *m1, *m2, *m3;
    uid_t uid;
    struct passwd *pwd;
    luab_iovec_t *buf;
    size_t bufsize;
    struct passwd *ret;
    caddr_t bp;
    struct passwd *result;
    int status;

    (void)luab_core_checkmaxargs(L, 5);

    m0 = luab_xmod(UID, TYPE, __func__);
    m1 = luab_xmod(PASSWD, TYPE, __func__);
    m2 = luab_xmod(IOVEC, TYPE, __func__);
    m3 = luab_xmod(SIZE, TYPE, __func__);

    uid = (uid_t)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    pwd = luab_udata(L, 2, m1, struct passwd *);
    buf = luab_udata(L, 3, m2, luab_iovec_t *);
    bufsize = (size_t)luab_checklxinteger(L, 4, m3, 0);
    ret = luab_udata(L, 5, m1, struct passwd *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (bufsize <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = getpwuid_r(uid, pwd, bp, bufsize, &result)) != 0)
                (void)memmove(ret, result, sizeof(struct passwd));

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = luab_env_success;
        }
    } else {
        errno = ERANGE;
        status = luab_env_success;
    }
    return (luab_pushxinteger(L, status));
}
#endif /* __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE >= 500 */

#if __BSD_VISIBLE
/***
 * setpassent(3) - password database operations
 *
 * @function setpassent
 *
 * @param stayopen          Specifies, if file descriptors from db(3) stays
 *                          open when its value is set non-zero.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pwd.setpassent(stayopen)
 */
static int
luab_setpassent(lua_State *L)
{
    luab_module_t *m;
    int stayopen;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    stayopen = (int)luab_checkxinteger(L, 1, m, luab_env_int_max);
    status = setpassent(stayopen);

    return (luab_pushxinteger(L, status));
}

/***
 * getpwent_r(3) - password database operations
 *
 * @function getpwent_r
 *
 * @param pwd               Storage, instance of (LUA_TUSERDATA(PASSWD)).
 * @param buffer            Line buffer, instance of (LUA_TUSERDATA(IOVEC)),
 *                          see passwd(5) for further details.
 * @param bufsize           By _SC_GETPW_R_SIZE_MAX over sysconf(3) determined
 *                          size in bytes to hold at least one line by passwd(5)
 *                          specified format.
 * @param result            Result argument, instance of (LUA_TUSERDATA(PASSWD)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pwd.getpwent_r(pwd, buffer, bufsize, result)
 */
static int
luab_getpwent_r(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    struct passwd *pwd;
    luab_iovec_t *buf;
    size_t bufsize;
    struct passwd *ret;
    caddr_t bp;
    struct passwd *result;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(PASSWD, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);
    m2 = luab_xmod(SIZE, TYPE, __func__);

    pwd = luab_udata(L, 1, m0, struct passwd *);
    buf = luab_udata(L, 2, m1, luab_iovec_t *);
    bufsize = (size_t)luab_checklxinteger(L, 3, m2, 0);
    ret = luab_udata(L, 4, m0, struct passwd *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (bufsize <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = getpwent_r(pwd, bp, bufsize, &result)) != 0)
                (void)memmove(ret, result, sizeof(struct passwd));

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = luab_env_success;
        }
    } else {
        errno = ERANGE;
        status = luab_env_success;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * user_from_uid(3) - cache password entries
 *
 * @function user_from_uid
 *
 * @param uid               Specifies user ID.
 * @param nosuer            Specifies if (LUA_TNIL) shall returned, if set
 *                          non-zero, when user name by requested UID does
 *                          not exist.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage str [, err, msg ] = bsd.pwd.user_from_uid(uid, nouser)
 */
static int
luab_user_from_uid(lua_State *L)
{
    luab_module_t *m0, *m1;
    uid_t uid;
    int nouser;
    const char *name;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(UID, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    uid = (uid_t)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    nouser = (int)luab_checkxinteger(L, 2, m1, luab_env_int_max);

    name = user_from_uid(uid, nouser);
    return (luab_pushstring(L, name));
}

/***
 * uid_from_user(3) - cache password entries
 *
 * @function uid_from_user
 *
 * @param name              Specifies user name.
 * @param uid               Result argument, with name linked UID,
 *                          instance of (LUA_TUSERDATA(UID)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pwd.uid_from_user(name, uid)
 */
static int
luab_uid_from_user(lua_State *L)
{
    luab_module_t *m;
    const char *name;
    uid_t *uid;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(UID, TYPE, __func__);

    name = luab_checklstring(L, 1, luab_env_logname_max, NULL);
    uid = luab_udata(L, 2, m, uid_t *);

    status = uid_from_user(name, uid);
    return (luab_pushxinteger(L, status));
}
#endif /* __BSD_VISIBLE */

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(PASSWD)).
 *
 * @function create_passwd
 *
 * @param passwd          Instance of (LUA_TUSERDATA(PASSWD)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage passwd [, err, msg ] = bsd.pwd.create_passwd([ passwd ])
 */
static int
luab_type_create_passwd(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(PASSWD, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/*
 * Interface against <pwd.h>.
 */

static luab_module_table_t luab_pwd_vec[] = { /* pwd.h */
    LUAB_STR("_PATH_PWD",               _PATH_PWD),
    LUAB_STR("_PATH_PASSWD",            _PATH_PASSWD),
    LUAB_STR("_PASSWD",                 _PASSWD),
    LUAB_STR("_PATH_MASTERPASSWD",      _PATH_MASTERPASSWD),
    LUAB_STR("_MASTERPASSWD",           _MASTERPASSWD),
    LUAB_STR("_PATH_MP_DB",             _PATH_MP_DB),
    LUAB_STR("_MP_DB",                  _MP_DB),
    LUAB_STR("_PATH_SMP_DB",            _PATH_SMP_DB),
    LUAB_STR("_SMP_DB",                 _SMP_DB),
    LUAB_STR("_PATH_PWD_MKDB",          _PATH_PWD_MKDB),
    LUAB_INT("_PW_VERSION_MASK",        _PW_VERSION_MASK),
    LUAB_INT("_PW_KEYBYNAME",           _PW_KEYBYNAME),
    LUAB_INT("_PW_KEYBYNUM",            _PW_KEYBYNUM),
    LUAB_INT("_PW_KEYBYUID",            _PW_KEYBYUID),
    LUAB_INT("_PW_KEYYPENABLED",        _PW_KEYYPENABLED),
    LUAB_INT("_PW_KEYYPBYNUM",          _PW_KEYYPBYNUM),
    LUAB_STR("_PWD_VERSION_KEY",        _PWD_VERSION_KEY),
    LUAB_INT("_PWD_CURRENT_VERSION",    _PWD_CURRENT_VERSION),
    LUAB_INT("_PASSWORD_EFMT1",         _PASSWORD_EFMT1),
    LUAB_INT("_PASSWORD_LEN",           _PASSWORD_LEN),
    LUAB_INT("_PWF_NAME",               _PWF_NAME),
    LUAB_INT("_PWF_PASSWD",             _PWF_PASSWD),
    LUAB_INT("_PWF_UID",                _PWF_UID),
    LUAB_INT("_PWF_GID",                _PWF_GID),
    LUAB_INT("_PWF_CHANGE",             _PWF_CHANGE),
    LUAB_INT("_PWF_CLASS",              _PWF_CLASS),
    LUAB_INT("_PWF_GECOS",              _PWF_GECOS),
    LUAB_INT("_PWF_DIR",                _PWF_DIR),
    LUAB_INT("_PWF_SHELL",              _PWF_SHELL),
    LUAB_INT("_PWF_EXPIRE",             _PWF_EXPIRE),
    LUAB_INT("_PWF_SOURCE",             _PWF_SOURCE),
    LUAB_INT("_PWF_FILES",              _PWF_FILES),
    LUAB_INT("_PWF_NIS",                _PWF_NIS),
    LUAB_INT("_PWF_HESIOD",             _PWF_HESIOD),
    LUAB_FUNC("getpwnam",               luab_getpwnam),
    LUAB_FUNC("getpwuid",               luab_getpwuid),
#if __XSI_VISIBLE >= 500
    LUAB_FUNC("endpwent",               luab_endpwent),
    LUAB_FUNC("getpwent",               luab_getpwent),
    LUAB_FUNC("setpwent",               luab_setpwent),
#endif
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE >= 500
    LUAB_FUNC("getpwnam_r",             luab_getpwnam_r),
    LUAB_FUNC("getpwuid_r",             luab_getpwuid_r),
#endif
#ifdef __BSD_VISIBLE
    LUAB_FUNC("setpassent",             luab_setpassent),
    LUAB_FUNC("getpwent_r",             luab_getpwent_r),
    LUAB_FUNC("user_from_uid",          luab_user_from_uid),
    LUAB_FUNC("uid_from_user",          luab_uid_from_user),
#endif
    LUAB_FUNC("create_passwd",          luab_type_create_passwd),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_pwd_lib = {
    .m_id       = LUAB_PWD_LIB_ID,
    .m_name     = LUAB_PWD_LIB_KEY,
    .m_vec      = luab_pwd_vec,
};
