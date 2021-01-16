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

#include <signal.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#define LUAB_SIGNAL_LIB_ID    1610728167
#define LUAB_SIGNAL_LIB_KEY   "signal"

extern luab_module_t luab_signal_lib;

/*
 * Service primitives
 */

/***
 * raise(3) - send a signal to a current thread
 *
 * @function raise
 *
 * @param sig               Specifies signal, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.raise(sig)
 */
static int
luab_raise(lua_State *L)
{
    luab_module_t *m;
    int sig, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    sig = (int)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    status = raise(sig);
    return (luab_pushinteger(L, status));
}

#if __POSIX_VISIBLE || __XSI_VISIBLE
/***
 * kill(2) - send a signal to a process
 *
 * @function kill
 *
 * @param pid               Specifies process or group of processes, by an
 *                          instance of (LUA_T{NUMBER,USERDATA(PID)}).
 * @param sig               Specifies signal, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.kill(pid, sig)
 */
static int
luab_kill(lua_State *L)
{
    luab_module_t *m0, *m1;
    pid_t pid;
    int sig;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PID, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    pid = (pid_t)luab_checkxinteger(L, 1, m0, luab_env_uint_max);
    sig = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    status = kill(pid, sig);
    return (luab_pushxinteger(L, status));
}

#endif /* __POSIX_VISIBLE || __XSI_VISIBLE */

/*
 * Access functions [C -> stack]
 */

#if __BSD_VISIBLE
/***
 * Accessor - sys_signame.
 *
 * @function sys_signame
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage t [, err, msg ] = bsd.signal.sys_signame()
 */
static int
luab_signal_sys_signame(lua_State *L)
{
    luab_module_t *m;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 0);

    m = luab_xmod(CADDR, TYPE, __func__);
    dp = __DECONST(caddr_t, sys_signame);

    return (luab_table_pushxvector(L, -2, m, dp, luab_env_sys_nsig, 1, 1));
}

/***
 * Accessor - sys_siglist.
 *
 * @function sys_siglist
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage t [, err, msg ] = bsd.signal.sys_siglist()
 */
static int
luab_signal_sys_siglist(lua_State *L)
{
    luab_module_t *m;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 0);

    m = luab_xmod(CADDR, TYPE, __func__);
    dp = __DECONST(caddr_t, sys_siglist);
    return (luab_table_pushxvector(L, -2, m, dp, luab_env_sys_nsig, 1, 1));
}

/***
 * Accessor - sys_nsig.
 *
 * @function sys_nsig
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sys_nsig()
 */
static int
luab_signal_sys_nsig(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);
    return (luab_pushxinteger(L, sys_nsig));
}
#endif /* __BSD_VISIBLE */

/*
 * Interface against <signal.h>
 */

static luab_module_table_t luab_signal_vec[] = {
    LUAB_FUNC("raise",              luab_raise),
#if __POSIX_VISIBLE || __XSI_VISIBLE
    LUAB_FUNC("kill",               luab_kill),
#endif /* __POSIX_VISIBLE || __XSI_VISIBLE */
#if __BSD_VISIBLE
    LUAB_FUNC("sys_signame",        luab_signal_sys_signame),
    LUAB_FUNC("sys_siglist",        luab_signal_sys_siglist),
    LUAB_FUNC("sys_nsig",           luab_signal_sys_nsig),
#endif
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_signal_lib = {
    .m_id       = LUAB_SIGNAL_LIB_ID,
    .m_name     = LUAB_SIGNAL_LIB_KEY,
    .m_vec      = luab_signal_vec,
};
