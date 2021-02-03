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

#include <sys/param.h>
#include <sys/jail.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#define LUAB_SYS_JAIL_LIB_ID    1606009660
#define LUAB_SYS_JAIL_LIB_KEY   "jail"

extern luab_module_t luab_sys_jail_lib;

/*
 * Service primitives.
 */

/***
 * jail(2) - create and manage system jails
 *
 * @function jail
 *
 * @param jail              Specifies the jail about to be setup, by
 *                          an instance of (LUA_TUSERDATA(JAIL)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.jail.jail(jail)
 */
static int
luab_jail(lua_State *L)
{
    luab_module_t *m;
    struct jail *jp;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(JAIL, TYPE, __func__);
    jp = luab_udata(L, 1, m, struct jail *);
    status = jail(jp);

    return (luab_pushxinteger(L, status));
}

/***
 * jail_set(2) - create and manage system jails
 *
 * @function jail_set
 *
 * @param iov               Specifies parametrical data over a set of
 *
 *                              (name,value)
 *
 *                          pairs by an instance of (LUA_TABLE)
 *
 *                              { iov0, iov1, ... , iovN }
 *
 *                          over (LUA_TUSERDATA(IOVEC)).
 * @param niov              Specifies the cardinality.
 * @param flags             The flags argument are constructed with values over
 *
 *                              bsd.sys.jail.JAIL_{
 *                                  CREATE,
 *                                  UPDATE,
 *                                  ATTACH,
 *                                  DYING
 *                              }
 *
 *                          by combining those with inclusive-OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg] = bsd.sys.jail.jail_set(iov, niov, flags)
 */
static int
luab_jail_set(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    luab_table_t *tbl;
    struct iovec *iov;
    u_int niov;
    int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(UINT, TYPE, __func__);
    m2 = luab_xmod(INT, TYPE, __func__);

    tbl = luab_table_checkxdata(L, 1, m0);
    niov = luab_checkxinteger(L, 2, m1, luab_env_int_max);
    flags = luab_checkxinteger(L, 3, m2, luab_env_int_max);

    if (tbl != NULL) {
        iov = (struct iovec *)(tbl->tbl_vec);

        if (tbl->tbl_card == niov && niov > 0) {
            status = jail_set(iov, niov, flags);
            luab_table_pushxdata(L, 1, m0, tbl, 0, 1);
        } else {
            luab_table_free(tbl);
            errno = ERANGE;
            status = luab_env_error;
        }
    } else
        status = luab_env_error;

    return (luab_pushxinteger(L, status));
}

/***
 * jail_get(2) - create and manage system jails
 *
 * @function jail_get
 *
 * @param iov               Specifies parametrical data over a set of
 *
 *                              (name,value)
 *
 *                          pairs by an instance of (LUA_TABLE)
 *
 *                              { iov0, iov1, ... , iovN }
 *
 *                          over (LUA_TUSERDATA(IOVEC)).
 * @param niov              Specifies the cardinality.
 * @param flags             The flags argument are constructed with values over
 *
 *                              bsd.sys.jail.JAIL_{
 *                                  CREATE,
 *                                  UPDATE,
 *                                  ATTACH,
 *                                  DYING
 *                              }
 *
 *                          by combining those with inclusive-OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg] = bsd.sys.jail.jail_get(iov, niov, flags)
 */
static int
luab_jail_get(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    luab_table_t *tbl;
    struct iovec *iov;
    u_int niov;
    int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(UINT, TYPE, __func__);
    m2 = luab_xmod(INT, TYPE, __func__);

    tbl = luab_table_checkxdata(L, 1, m0);
    niov = luab_checkxinteger(L, 2, m1, luab_env_int_max);
    flags = luab_checkxinteger(L, 3, m2, luab_env_int_max);

    if (tbl != NULL) {
        iov = (struct iovec *)(tbl->tbl_vec);

        if (tbl->tbl_card == niov && niov > 0) {
            status = jail_get(iov, niov, flags);
            luab_table_pushxdata(L, 1, m0, tbl, 0, 1);
        } else {
            luab_table_free(tbl);
            errno = ERANGE;
            status = luab_env_error;
        }
    } else
        status = luab_env_error;

    return (luab_pushxinteger(L, status));
}

/***
 * jail_attach(2) - create and manage system jails
 *
 * @function jail_attach
 *
 * @param jail              Specifies jail ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg] = bsd.sys.jail.jail_attach(jls)
 */
static int
luab_jail_attach(lua_State *L)
{
    luab_module_t *m;
    int jls, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);

    jls = luab_checkxinteger(L, 1, m, luab_env_int_max);
    status = jail_attach(jls);

    return (luab_pushxinteger(L, status));
}

/***
 * jail_remove(2) - create and manage system jails
 *
 * @function jail_remove
 *
 * @param jail              Specifies jail ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg] = bsd.sys.jail.jail_remove(jls)
 */
static int
luab_jail_remove(lua_State *L)
{
    luab_module_t *m;
    int jls, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);

    jls = luab_checkxinteger(L, 1, m, luab_env_int_max);
    status = jail_remove(jls);

    return (luab_pushxinteger(L, status));
}

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(JAIL)).
 *
 * @function create_jail
 *
 * @param jail              Instance of (LUA_TUSERDATA(JAIL)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage jail [, err, msg ] = bsd.sys.jail.create_jail()
 */
static int
luab_type_create_jail(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(JAIL, TYPE, __func__);
    return (luab_core_create(L, 0, m, NULL));
}

/*
 * Interface against <sys/jail.h>
 */

static luab_module_table_t luab_sys_jail_vec[] = {
    LUAB_INT("JAIL_API_VERSION",        JAIL_API_VERSION),
    LUAB_INT("XPRISON_VERSION",         XPRISON_VERSION),
    LUAB_INT("PRISON_STATE_INVALID",    PRISON_STATE_INVALID),
    LUAB_INT("PRISON_STATE_ALIVE",      PRISON_STATE_ALIVE),
    LUAB_INT("PRISON_STATE_DYING",      PRISON_STATE_DYING),
    LUAB_INT("JAIL_CREATE",             JAIL_CREATE),
    LUAB_INT("JAIL_UPDATE",             JAIL_UPDATE),
    LUAB_INT("JAIL_ATTACH",             JAIL_ATTACH),
    LUAB_INT("JAIL_DYING",              JAIL_DYING),
    LUAB_INT("JAIL_SET_MASK",           JAIL_SET_MASK),
    LUAB_INT("JAIL_GET_MASK",           JAIL_GET_MASK),
    LUAB_INT("JAIL_SYS_DISABLE",        JAIL_SYS_DISABLE),
    LUAB_INT("JAIL_SYS_NEW",            JAIL_SYS_NEW),
    LUAB_INT("JAIL_SYS_INHERIT",        JAIL_SYS_INHERIT),
    LUAB_FUNC("jail",                   luab_jail),
    LUAB_FUNC("jail_set",               luab_jail_set),
    LUAB_FUNC("jail_get",               luab_jail_get),
    LUAB_FUNC("jail_attach",            luab_jail_attach),
    LUAB_FUNC("jail_remove",            luab_jail_remove),
    LUAB_FUNC("create_jail",            luab_type_create_jail),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_jail_lib = {
    .m_id       = LUAB_SYS_JAIL_LIB_ID,
    .m_name     = LUAB_SYS_JAIL_LIB_KEY,
    .m_vec      = luab_sys_jail_vec,
};
