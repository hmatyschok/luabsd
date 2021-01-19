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

/*
 * XXX
 *  int pthread_kill(__pthread_t, int);
 *  int pthread_sigmask(int, const __sigset_t * __restrict,
 *      __sigset_t * __restrict);
 *  int sigaction(int, const struct sigaction * __restrict,
 *      struct sigaction * __restrict);
 */

/***
 * sigaddset(3) - manipulate signal set
 *
 * @function sigaddset
 *
 * @param set               Specifies signal set, by (LUA_TUSERDATA(SIGSET)).
 * @param signo             Specifies signal, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigaddset(set, signo)
 */
static int
luab_sigaddset(lua_State *L)
{
    luab_module_t *m0, *m1;
    sigset_t *set;
    int signo, status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SIGSET, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    set = luab_udata(L, 1, m0, sigset_t *);
    signo = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    status = sigaddset(set, signo);
    return (luab_pushxinteger(L, status));
}

/***
 * sigdelset(3) - manipulate signal set
 *
 * @function sigdelset
 *
 * @param set               Specifies signal set, by (LUA_TUSERDATA(SIGSET)).
 * @param signo             Specifies signal, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sidelset(set, signo)
 */
static int
luab_sigdelset(lua_State *L)
{
    luab_module_t *m0, *m1;
    sigset_t *set;
    int signo, status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SIGSET, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    set = luab_udata(L, 1, m0, sigset_t *);
    signo = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    status = sigdelset(set, signo);
    return (luab_pushxinteger(L, status));
}

/***
 * sigemptyset(3) - manipulate signal set
 *
 * @function sigemptyset
 *
 * @param set               Specifies signal set, by (LUA_TUSERDATA(SIGSET)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigemptyset(set)
 */
static int
luab_sigemptyset(lua_State *L)
{
    luab_module_t *m;
    sigset_t *set;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGSET, TYPE, __func__);
    set = luab_udata(L, 1, m, sigset_t *);

    status = sigemptyset(set);
    return (luab_pushxinteger(L, status));
}

/***
 * sigfillset(3) - manipulate signal set
 *
 * @function sigfillset
 *
 * @param set               Specifies signal set, by (LUA_TUSERDATA(SIGSET)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigfillset(set)
 */
static int
luab_sigfillset(lua_State *L)
{
    luab_module_t *m;
    sigset_t *set;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGSET, TYPE, __func__);
    set = luab_udata(L, 1, m, sigset_t *);

    status = sigfillset(set);
    return (luab_pushxinteger(L, status));
}

/***
 * sigismember(3) - manipulate signal set
 *
 * @function sigismember
 *
 * @param set               Specifies signal set, by (LUA_TUSERDATA(SIGSET)).
 * @param signo             Specifies signal, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigismember(set, signo)
 */
static int
luab_sigismember(lua_State *L)
{
    luab_module_t *m0, *m1;
    sigset_t *set;
    int signo, status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SIGSET, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    set = luab_udata(L, 1, m0, sigset_t *);
    signo = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    status = sigismember(set, signo);
    return (luab_pushxinteger(L, status));
}

/***
 * sigpending(2) - get pending signals
 *
 * @function sigpending
 *
 * @param set               Specifies signal set, by (LUA_TUSERDATA(SIGSET)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigpending(set)
 */
static int
luab_sigpending(lua_State *L)
{
    luab_module_t *m;
    sigset_t *set;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGSET, TYPE, __func__);
    set = luab_udata(L, 1, m, sigset_t *);

    status = sigpending(set);
    return (luab_pushxinteger(L, status));
}

/***
 * sigprocmask(2) - manipulate current signal mask
 *
 * @function sigprocmask
 *
 * @param how               Specifies function by values from:
 *
 *                              SIG_{
 *                                  BLOCK,
 *                                  UNBLOCK,
 *                                  SETMASK
 *                              }
 *
 * @param set               Specifies signal set, by (LUA_TUSERDATA(SIGSET)).
 * @param oset              Specifies previos signal set, (LUA_TUSERDATA(SIGSET)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigprocmask(how, set, oset)
 */
static int
luab_sigprocmask(lua_State *L)
{
    luab_module_t *m0, *m1;
    int how;
    sigset_t *set;
    sigset_t *oset;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(SIGSET, TYPE, __func__);

    how = (int)luab_checkxinteger(L, 1, m0, luab_env_uint_max);
    set = luab_udataisnil(L, 2, m1, sigset_t *);
    oset = luab_udataisnil(L, 3, m1, sigset_t *);

    status = sigprocmask(how, set, oset);
    return (luab_pushxinteger(L, status));
}

/***
 * sigwait(2) - select a set of signals
 *
 * @function sigwait
 *
 * @param set               Specifies signal set, by (LUA_TUSERDATA(SIGSET)).
 * @param sig               Specifies signal, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigwait(set, signo)
 */
static int
luab_sigwait(lua_State *L)
{
    luab_module_t *m0, *m1;
    sigset_t *set;
    int *sig, status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SIGSET, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    set = luab_udata(L, 1, m0, sigset_t *);
    sig = luab_udata(L, 2, m1, int *);

    status = sigwait(set, sig);
    return (luab_pushxinteger(L, status));
}

#endif /* __POSIX_VISIBLE || __XSI_VISIBLE */

#if __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE >= 600
/***
 * sigqueue(2) - queue a signal to a process (REALTIME)
 *
 * @function sigqueue
 *
 * @param set               Specifies process, by (LUA_TUSERDATA(SIGSET)).
 * @param info              Specifies signal, (LUA_TUSERDATA(SIGINFO)).
 * @param timeout           Specifies value, (LUA_TUSERDATA(TIMESPEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigqueue(pid, signo, sigval)
 */
static int
luab_sigqueue(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    pid_t pid;
    int signo;
    union sigval *value;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(PID, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);
    m2 = luab_xmod(SIGVAL, TYPE, __func__);

    pid = (pid_t)luab_checkxinteger(L, 1, m0, luab_env_uint_max);
    signo = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    if ((value = luab_udata(L, 3, m2, union sigval *)) != NULL)
        status = sigqueue(pid, signo, *value);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * sigtimedwait(2) - wait for queued signals (REALTIME)
 *
 * @function sigtimedwait
 *
 * @param set               Specifies process, by (LUA_TUSERDATA(SIGSET)).
 * @param info              Specifies signal, (LUA_TUSERDATA(SIGINFO)).
 * @param timeout           Specifies value, (LUA_TUSERDATA(TIMESPEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigtimedwait(set, info, timeout)
 */
static int
luab_sigtimedwait(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    sigset_t *set;
    siginfo_t *info;
    struct timespec *timeout;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(SIGSET, TYPE, __func__);
    m1 = luab_xmod(_SIGINFO, TYPE, __func__);
    m2 = luab_xmod(TIMESPEC, TYPE, __func__);

    set = luab_udata(L, 1, m0, sigset_t *);
    info = luab_udata(L, 2, m1, siginfo_t *);
    timeout = luab_udataisnil(L, 3, m2, struct timespec *);

    status = sigtimedwait(set, info, timeout);
    return (luab_pushxinteger(L, status));
}

/***
 * sigwaitinfo(2) - wait for queued signals (REALTIME)
 *
 * @function sigwaitinfo
 *
 * @param set               Specifies process, by (LUA_TUSERDATA(SIGSET)).
 * @param info              Specifies signal, (LUA_TUSERDATA(SIGINFO)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigwaitinfo(set, info)
 */
static int
luab_sigwaitinfo(lua_State *L)
{
    luab_module_t *m0, *m1;
    sigset_t *set;
    siginfo_t *info;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SIGSET, TYPE, __func__);
    m1 = luab_xmod(_SIGINFO, TYPE, __func__);

    set = luab_udata(L, 1, m0, sigset_t *);
    info = luab_udata(L, 2, m1, siginfo_t *);

    status = sigwaitinfo(set, info);
    return (luab_pushxinteger(L, status));
}
#endif /* __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE >= 600 */

#if __XSI_VISIBLE
/***
 * killpg(2) - send a signal to a process group
 *
 * @function killpg
 *
 * @param pgrp              Specifies process group, by an instance
 *                          of (LUA_T{NUMBER,USERDATA(PID)}).
 * @param sig               Specifies signal, by (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.killpg(pgrp, sig)
 */
static int
luab_killpg(lua_State *L)
{
    luab_module_t *m0, *m1;
    pid_t pgrp;
    int sig;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(PID, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    pgrp = (pid_t)luab_checkxinteger(L, 1, m0, luab_env_uint_max);
    sig = (int)luab_checkxinteger(L, 1, m1, luab_env_uint_max);

    status = killpg(pgrp, sig);
    return (luab_pushinteger(L, status));
}

/***
 * sigaltstack(2) - send and/or get signal stack context
 *
 * @function sigaltstack
 *
 * @param ss                Current signal stack, (LUA_TUSERDATA(STACK)).
 * @param oss               Old signal stack, (LUA_TUSERDATA(STACK)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigaltstack(ss, oss)
 */
static int
luab_sigaltstack(lua_State *L)
{
    luab_module_t *m;
    stack_t *ss, *oss;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(STACK, TYPE, __func__);

    ss = luab_udata(L, 1, m, stack_t *);
    oss = luab_udataisnil(L, 2, m, stack_t *);

    status = sigaltstack(ss, oss);
    return (luab_pushxinteger(L, status));
}

/***
 * sighold(2) - legacy interface for signal management
 *
 * @function sighold
 *
 * @param sig               Signal, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sighold(sig)
 */
static int
luab_sighold(lua_State *L)
{
    luab_module_t *m;
    int sig, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    sig = (int)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    status = sighold(sig);
    return (luab_pushxinteger(L, status));
}

/***
 * sigignore(2) - legacy interface for signal management
 *
 * @function sigignore
 *
 * @param sig               Signal, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigignore(sig)
 */
static int
luab_sigignore(lua_State *L)
{
    luab_module_t *m;
    int sig, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    sig = (int)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    status = sigignore(sig);
    return (luab_pushxinteger(L, status));
}

/***
 * sigpause(2) - legacy interface for signal management
 *
 * @function sigpause
 *
 * @param sigmask           Signalmask, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigpause(sigmask)
 */
static int
luab_sigpause(lua_State *L)
{
    luab_module_t *m;
    int sigmask, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    sigmask = (int)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    status = sigpause(sigmask);
    return (luab_pushxinteger(L, status));
}

/***
 * sigrelse(2) - legacy interface for signal management
 *
 * @function sigrelse
 *
 * @param sig               Signal, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.sigrelse(sig)
 */
static int
luab_sigrelse(lua_State *L)
{
    luab_module_t *m;
    int sig, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    sig = (int)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    status = sigrelse(sig);
    return (luab_pushxinteger(L, status));
}

/***
 * xsi_sigpause(2) - legacy interface for signal management
 *
 * @function xsi_sigpause
 *
 * @param sigmask           Signalmask, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.xsi_sigpause(sigmask)
 */
static int
luab_xsi_sigpause(lua_State *L)
{
    luab_module_t *m;
    int sigmask, status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);
    sigmask = (int)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    status = xsi_sigpause(sigmask);
    return (luab_pushxinteger(L, status));
}
#endif /* __XSI_VISIBLE */

#if __XSI_VISIBLE >= 600
/***
 * siginterrupt(3) - allow signal to interrupt system calls
 *
 * @function siginterrupt
 *
 * @param sig               Signal, (LUA_T{NUMBER,USERDATA(INT)}).
 * @param flag              Flags, (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.siginterrupt(sigmask)
 */
static int
luab_siginterrupt(lua_State *L)
{
    luab_module_t *m;
    int sig, flag;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(INT, TYPE, __func__);

    sig = (int)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    flag = (int)luab_checkxinteger(L, 2, m, luab_env_uint_max);

    status = siginterrupt(sig, flag);
    return (luab_pushxinteger(L, status));
}
#endif /* __XSI_VISIBLE >= 600 */

#if __POSIX_VISIBLE >= 200809
/***
 * psignal(3) - system signal messages
 *
 * @function psignal
 *
 * @param sig            Signal, (LUA_T{NUMBER,USERDATA(INT)}).
 * @param s              Specifies message by (LUA_TSTRING).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.signal.psignal(sig, s)
 */
static int
luab_psignal(lua_State *L)
{
    luab_module_t *m;
    int sig;
    const char *s;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(INT, TYPE, __func__);

    sig = luab_checkxinteger(L, 1, m, luab_env_uint_max);
    s = luab_checklstring(L, 2, luab_env_buf_max, NULL);

    psignal(sig, s);
    return (luab_pushxinteger(L, luab_env_success));
}
#endif /* __POSIX_VISIBLE >= 200809 */

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

/*
 * XXX
 *  int pthread_kill(__pthread_t, int);
 *  int pthread_sigmask(int, const __sigset_t * __restrict,
 *      __sigset_t * __restrict);
 *  int sigaction(int, const struct sigaction * __restrict,
 *      struct sigaction * __restrict);
 */

    LUAB_FUNC("sigaddset",          luab_sigaddset),
    LUAB_FUNC("sigdelset",          luab_sigdelset),
    LUAB_FUNC("sigemptyset",        luab_sigemptyset),
    LUAB_FUNC("sigfillset",         luab_sigfillset),
    LUAB_FUNC("sigismember",        luab_sigismember),
    LUAB_FUNC("sigpending",         luab_sigpending),
    LUAB_FUNC("sigprocmask",        luab_sigprocmask),
    LUAB_FUNC("sigwait",            luab_sigwait),
#endif /* __POSIX_VISIBLE || __XSI_VISIBLE */
#if __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE >= 600
    LUAB_FUNC("sigqueue",           luab_sigqueue),
    LUAB_FUNC("sigtimedwait",       luab_sigtimedwait),
    LUAB_FUNC("sigwaitinfo",        luab_sigwaitinfo),
#endif /* __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE >= 600 */
#if __XSI_VISIBLE
    LUAB_FUNC("killpg",             luab_killpg),
    LUAB_FUNC("sigaltstack",        luab_sigaltstack),
    LUAB_FUNC("sighold",            luab_sighold),
    LUAB_FUNC("sigignore",          luab_sigignore),
    LUAB_FUNC("sigpause",           luab_sigpause),
    LUAB_FUNC("sigrelse",           luab_sigrelse),
    LUAB_FUNC("xsi_sigpause",       luab_xsi_sigpause),
#endif /* __XSI_VISIBLE */
#if __XSI_VISIBLE >= 600
    LUAB_FUNC("siginterrupt",       luab_siginterrupt),
#endif
#if __POSIX_VISIBLE >= 200809
    LUAB_FUNC("psignal",            luab_psignal),
#endif
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
