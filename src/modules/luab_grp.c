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

#include <grp.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

/*
 * XXX
 *
 *      int  pwcache_groupdb(int (*)(int), void (*)(void),
 *              struct group * (*)(const char *),
 *              struct group * (*)(gid_t));
 *
 *  Implementation depends on specific luab_type(3) implements autoboxing
 *  for (LUA_TCFUNCTION) objects or set of callbacks over vector table.
 */

#define LUAB_GRP_LIB_ID    1604324195
#define LUAB_GRP_LIB_KEY    "grp"

extern luab_module_t luab_grp_lib;

/*
 * Service primitives.
 */

#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
/***
 * endgrent(3) - group database operations
 *
 * @function endgrent
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.pwd.endgrent()
 */
static int
luab_endgrent(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);

    endgrent();
    return (luab_pushxinteger(L, luab_env_success));
}

/***
 * getgrent(3) - group database operations
 *
 * @function getgrent
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage passwd [, err, msg ] = bsd.grp.getgrent()
 */
static int
luab_getgrent(lua_State *L)
{
    luab_module_t *m;
    struct group *grp;

    (void)luab_core_checkmaxargs(L, 0);

    m = luab_xmod(GROUP, TYPE, __func__);

    if ((grp = getgrent()) == NULL)
        m = NULL;

    return (luab_pushxdata(L, m, grp));
}
#endif /* __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE */

/***
 * getgrgid(3) - group database operations
 *
 * @function getgrgid
 *
 * @param gid               Specifies group ID.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage group [, err, msg ] = bsd.grp.getgrgid(gid)
 */
static int
luab_getgrgid(lua_State *L)
{
    luab_module_t *m0, *m1;
    gid_t gid;
    struct group *grp;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(GID, TYPE, __func__);
    m1 = luab_xmod(GROUP, TYPE, __func__);

    gid = (gid_t)luab_checkxinteger(L, 1, m0, luab_env_int_max);

    if ((grp = getgrgid(gid)) == NULL)
        m1 = NULL;

    return (luab_pushxdata(L, m1, grp));
}

/***
 * getgrnam(3) - group database operations
 *
 * @function getgrnam
 *
 * @param name              Specifies group name.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage group [, err, msg ] = bsd.grp.getgrnam(name)
 */
static int
luab_getgrnam(lua_State *L)
{
    luab_module_t *m;
    const char *name;
    struct group *grp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(GROUP, TYPE, __func__);
    name = luab_checklstring(L, 1, luab_env_logname_max, NULL);

    if ((grp = getgrnam(name)) == NULL)
        m = NULL;

    return (luab_pushxdata(L, m, grp));
}

#if __BSD_VISIBLE
/***
 * group_from_gid(3) - cache group entries
 *
 * @function group_from_gid
 *
 * @param gid               Specifies group ID.
 * @param nosuer            Specifies if (LUA_TNIL) shall returned, if set
 *                          non-zero, when user name by requested GID does
 *                          not exist.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage str [, err, msg ] = bsd.grp.group_from_gid(gid, nouser)
 */
static int
luab_group_from_gid(lua_State *L)
{
    luab_module_t *m0, *m1;
    gid_t gid;
    int nouser;
    const char *name;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(GID, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    gid = (gid_t)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    nouser = (int)luab_checkxinteger(L, 2, m1, luab_env_int_max);

    name = group_from_gid(gid, nouser);
    return (luab_pushstring(L, name));
}

/***
 * gid_from_group(3) - cache password entries
 *
 * @function gid_from_group
 *
 * @param name              Specifies group name.
 * @param gid               Result argument, with name linked GID,
 *                          instance of (LUA_TUSERDATA(INTEGER)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.grp.gid_from_group(name, gid)
 */
static int
luab_gid_from_group(lua_State *L)
{
    luab_module_t *m;
    const char *name;
    gid_t *gid;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(GID, TYPE, __func__);
    name = luab_checklstring(L, 1, luab_env_logname_max, NULL);
    gid = luab_udata(L, 2, m, gid_t *);

    status = gid_from_group(name, gid);
    return (luab_pushxinteger(L, status));
}
#endif /* __BSD_VISIBLE */

#if __XSI_VISIBLE
/***
 * setgrent(3) - group database operations
 *
 * @function setgrent
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.grp.setgrent()
 */
static int
luab_setgrent(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);

    setgrent();
    return (luab_pushxinteger(L, luab_env_success));
}
#endif

#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
/***
 * getgrgid_r(3) - group database operations
 *
 * @function getgrgid_r
 *
 * @param gid               Specifies group ID.
 * @param grp               Storage, instance of (LUA_TUSERDATA(GROUP)).
 * @param buffer            Line buffer, instance of (LUA_TUSERDATA(IOVEC)),
 *                          see group(5) for further details.
 * @param bufsize           By _SC_GETGR_R_SIZE_MAX over sysconf(3) determined
 *                          size in bytes to hold at least one line by group(5)
 *                          specified format.
 * @param result            Result argument, instance of (LUA_TUSERDATA(GROUP)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.grp.getgrgid_r(gid, grp, buffer, bufsize, result)
 */
static int
luab_getgrgid_r(lua_State *L)
{
    luab_module_t *m0, *m1, *m2, *m3;
    gid_t gid;
    struct group *grp;
    luab_iovec_t *buf;
    size_t bufsize;
    struct group *ret;
    caddr_t bp;
    struct group *result;
    int status;

    (void)luab_core_checkmaxargs(L, 5);

    m0 = luab_xmod(GID, TYPE, __func__);
    m1 = luab_xmod(GROUP, TYPE, __func__);
    m2 = luab_xmod(IOVEC, TYPE, __func__);
    m3 = luab_xmod(SIZE, TYPE, __func__);

    gid = (gid_t)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    grp = luab_udata(L, 2, m1, struct group *);
    buf = luab_udata(L, 3, m2, luab_iovec_t *);
    bufsize = (size_t)luab_checklxinteger(L, 4, m3, 0);
    ret = luab_udata(L, 5, m0, struct group *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (bufsize <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if ((status = getgrgid_r(gid, grp, bp, bufsize, &result)) != 0)
            (void)memmove(ret, result, sizeof(struct group));

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_success;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * getgrnam_r(3) - group database operations
 *
 * @function getgrnam_r
 *
 * @param name              Specifies group name.
 * @param grp               Storage, instance of (LUA_TUSERDATA(GROUP)).
 * @param buffer            Line buffer, instance of (LUA_TUSERDATA(IOVEC)),
 *                          see group(5) for further details.
 * @param bufsize           By _SC_GETGR_R_SIZE_MAX over sysconf(3) determined
 *                          size in bytes to hold at least one line by group(5)
 *                          specified format.
 * @param result            Result argument, instance of (LUA_TUSERDATA(GROUP)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.grp.getgrnam_r(name, grp, buffer, bufsize, result)
 */
static int
luab_getgrnam_r(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    const char *name;
    struct group *grp;
    luab_iovec_t *buf;
    size_t bufsize;
    struct group *ret;
    caddr_t bp;
    struct group *result;
    int status;

    (void)luab_core_checkmaxargs(L, 5);

    m0 = luab_xmod(GROUP, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);
    m2 = luab_xmod(GID, TYPE, __func__);

    name = luab_checklstring(L, 1, luab_env_logname_max, NULL);
    grp = luab_udata(L, 2, m0, struct group *);
    buf = luab_udata(L, 3, m1, luab_iovec_t *);
    bufsize = (size_t)luab_checklxinteger(L, 4, m2, 0);
    ret = luab_udata(L, 5, m0, struct group *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (bufsize <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if ((status = getgrnam_r(name, grp, bp, bufsize, &result)) != 0)
            (void)memmove(ret, result, sizeof(struct group));

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_success;
    }
    return (luab_pushxinteger(L, status));
}
#endif /* __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE */

#if __BSD_VISIBLE
/***
 * getgrent_r(3) - group database operations
 *
 * @function getgrent_r
 *
 * @param grp               Storage, instance of (LUA_TUSERDATA(GROUP)).
 * @param buffer            Line buffer, instance of (LUA_TUSERDATA(IOVEC)),
 *                          see group(5) for further details.
 * @param bufsize           By _SC_GETGR_R_SIZE_MAX over sysconf(3) determined
 *                          size in bytes to hold at least one line by group(5)
 *                          specified format.
 * @param result            Result argument, instance of (LUA_TUSERDATA(GROUP)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.grp.getgrent_r(grp, buffer, bufsize, result)
 */
static int
luab_getgrent_r(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    struct group *grp;
    luab_iovec_t *buf;
    size_t bufsize;
    struct group *ret;
    caddr_t bp;
    struct group *result;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(GROUP, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);
    m2 = luab_xmod(SIZE, TYPE, __func__);

    grp = luab_udata(L, 1, m0, struct group *);
    buf = luab_udata(L, 2, m1, luab_iovec_t *);
    bufsize = (size_t)luab_checklxinteger(L, 3, m2, 0);
    ret = luab_udata(L, 4, m0, struct group *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (bufsize <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if ((status = getgrent_r(grp, bp, bufsize, &result)) != 0)
            (void)memmove(ret, result, sizeof(struct group));

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_success;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * setgroupent(3) - group database operations
 *
 * @function setgroupent
 *
 * @param stayopen          Specifies, if file descriptors from db(3) stays
 *                          open when its value is set non-zero.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.grp.setgroupent(stayopen)
 */
static int
luab_setgroupent(lua_State *L)
{
    luab_module_t *m;
    int stayopen;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    stayopen = luab_checkxinteger(L, 1, m, luab_env_int_max);

    status = setgroupent(stayopen);
    return (luab_pushxinteger(L, status));
}
#endif /* __BSD_VISIBLE */

/*
 * Generator functions.
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(GID)).
 *
 * @function create_gid
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(GID)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage gid [, err, msg ] = bsd.grp.create_gid(arg)
 */
static int
luab_type_create_gid(lua_State *L)
{
    luab_module_t *m;
    gid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(GID, TYPE, __func__);
    x = (gid_t)luab_checkxinteger(L, 1, m, luab_env_int_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(GROUP)).
 *
 * @function create_group
 *
 * @param group             Instance of (LUA_TUSERDATA(GROUP)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage group [, err, msg ] = bsd.grp.create_group([ group ])
 */
static int
luab_type_create_group(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(GROUP, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/*
 * Interface against <grp.h>.
 */

static luab_module_table_t luab_grp_vec[] = { /* grp.h */
    LUAB_STR("_PATH_GROUP",         _PATH_GROUP),
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
    LUAB_FUNC("endgrent",           luab_endgrent),
    LUAB_FUNC("getgrent",           luab_getgrent),
#endif
    LUAB_FUNC("getgrgid",           luab_getgrgid),
    LUAB_FUNC("getgrnam",           luab_getgrnam),
#if __BSD_VISIBLE
    LUAB_FUNC("group_from_gid",     luab_group_from_gid),
    LUAB_FUNC("gid_from_group",     luab_gid_from_group),
#endif
#if __XSI_VISIBLE
    LUAB_FUNC("setgrent",           luab_setgrent),
#endif
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
    LUAB_FUNC("getgrgid_r",         luab_getgrgid_r),
    LUAB_FUNC("getgrnam_r",         luab_getgrnam_r),
#endif
#if __BSD_VISIBLE
    LUAB_FUNC("getgrent_r",         luab_getgrent_r),
    LUAB_FUNC("setgroupent",        luab_setgroupent),
#endif
    LUAB_FUNC("create_gid",         luab_type_create_gid),
    LUAB_FUNC("create_group",       luab_type_create_group),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_grp_lib = {
    .m_id       = LUAB_GRP_LIB_ID,
    .m_name     = LUAB_GRP_LIB_KEY,
    .m_vec      = luab_grp_vec,
};
