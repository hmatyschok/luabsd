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

#include <sys/ipc.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_SYS_IPC_LIB_ID    1613615797
#define LUAB_SYS_IPC_LIB_KEY   "ipc"

extern luab_module_t luab_sys_ipc_lib;

/*
 * Service primitives.
 */

/***
 * ftok(3) - create IPC identifier from path name
 *
 * @function ftok
 *
 * @param path              Specifies existing file, by (LUA_TSTRING).
 * @param id                Specifies user-selectable ID, by an instance
 *                          of (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.ipc.ftok(path, id)
 */
static int
luab_ftok(lua_State *L)
{
    luab_module_t *m;
    const char *path;
    int id;
    key_t status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(INT, TYPE, __func__);

    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    id = luab_checkxinteger(L, 2, m, luab_env_uint_max);

    status = ftok(path, id);
    return (luab_pushxinteger(L, status));
}

/*
 * Generator functions.
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(KEY)).
 *
 * @function create_key
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(KEY)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage key [, err, msg ] = bsd.sys.key.create_key(arg)
 */
static int
luab_type_create_key(lua_State *L)
{
    luab_module_t *m;
    key_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(KEY, TYPE, __func__);
    x = (key_t)luab_checkxinteger(L, 1, m, luab_env_ulong_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(IPC_PERM)).
 *
 * @function create_ipc_perm
 *
 * @param arg           Instance of (LUA_TUSERDATA(IPC_PERM)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ipc_perm [, err, msg ] = bsd.sys.ipc.create_ipc_perm([ arg ])
 */
static int
luab_type_create_ipc_perm(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IPC_PERM, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}


/*
 * Interface against <sys/ipc.h>.
 */

static luab_module_table_t luab_sys_ipc_vec[] = {
#if __BSD_VISIBLE
    LUAB_INT("IPC_R",               IPC_R),
    LUAB_INT("IPC_W",               IPC_W),
    LUAB_INT("IPC_M",               IPC_M),
#endif /* __BSD_VISIBLE */
    LUAB_INT("IPC_CREAT",           IPC_CREAT),
    LUAB_INT("IPC_EXCL",            IPC_EXCL),
    LUAB_INT("IPC_NOWAIT",          IPC_NOWAIT),
    LUAB_INT("IPC_PRIVATE",         IPC_PRIVATE),
    LUAB_INT("IPC_RMID",            IPC_RMID),
    LUAB_INT("IPC_SET",             IPC_SET),
    LUAB_INT("IPC_STAT",            IPC_STAT),
#if __BSD_VISIBLE
    LUAB_INT("IPC_INFO",            IPC_INFO),
#endif
    /* service primitives */
    LUAB_FUNC("ftok",               luab_ftok),

    /* generator functions */
    LUAB_FUNC("create_key",         luab_type_create_key),
    LUAB_FUNC("create_ipc_perm",    luab_type_create_ipc_perm),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_ipc_lib = {
    .m_id       = LUAB_SYS_IPC_LIB_ID,
    .m_name     = LUAB_SYS_IPC_LIB_KEY,
    .m_vec      = luab_sys_ipc_vec,
};
