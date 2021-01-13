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

#if __POSIX_VISIBLE >= 199309 || __XSI_VISIBLE
extern luab_module_t luab_siginfo_type;

/*
 * Interface against
 *
 *  typedef struct __siginfo {
 *      int si_signo;
 *      int si_code;
 *      __pid_t si_pid;
 *      __uid_t si_uid;
 *      int si_status;
 *      void    *si_addr;
 *      union sigval si_value;
 *      union   {
 *          struct {
 *              int _trapno;
 *          } _fault;
 *          struct {
 *              int _timerid;
 *              int _overrun;
 *          } _timer;
 *          struct {
 *              int _mqd;
 *          } _mesgq;
 *          struct {
 *              long    _band;
 *          } _poll;
 *          struct {
 *              long    __spare1__;
 *              int __spare2__[7];
 *          } __spare__;
 *      } _reason;
 *  } siginfo_t;
 *
 *  #define si_trapno   _reason._fault._trapno
 *  #define si_timerid  _reason._timer._timerid
 *  #define si_overrun  _reason._timer._overrun
 *  #define si_mqd      _reason._mesgq._mqd
 *  #define si_band     _reason._poll._band
 */

typedef struct luab_siginfo {
    luab_udata_t    ud_softc;
    siginfo_t       ud_si;
} luab_siginfo_t;

/*
 * Subr.
 */

static void
siginfo_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_module_t *m;
    siginfo_t *si;

    m = luab_xmod(SIGVAL, TYPE, __func__);

    if ((si = (siginfo_t *)arg) != NULL) {
        luab_setinteger(L, narg, "si_signo",        si->si_signo);
        luab_setinteger(L, narg, "si_errno",        si->si_errno);
        luab_setinteger(L, narg, "si_code",         si->si_code);
        luab_setinteger(L, narg, "si_pid",          si->si_pid);
        luab_setinteger(L, narg, "si_uid",          si->si_uid);
        luab_setinteger(L, narg, "si_status",       si->si_status);
        luab_setfstring(L, narg, "si_addr", "(%s)", si->si_addr);
        luab_setxdata(L, narg, m, "si_value",       &si->si_value);
        luab_setinteger(L, narg, "si_trapno",       si->si_trapno);
        luab_setinteger(L, narg, "si_timerid",      si->si_timerid);
        luab_setinteger(L, narg, "si_overrun",      si->si_overrun);
        luab_setinteger(L, narg, "si_mqd",          si->si_mqd);
        luab_setinteger(L, narg, "si_band",         si->si_band);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(_SIGINFO)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              si_signo    = (LUA_TNUMBER),
 *              si_errno    = (LUA_TNUMBER),
 *              si_code     = (LUA_TNUMBER),
 *              si_pid      = (LUA_TNUMBER),
 *              si_uid      = (LUA_TNUMBER),
 *              si_status   = (LUA_TNUMBER),
 *              si_addr     = (LUA_T{NIL,STRING}),
 *              si_value    = (LUA_T{NIL,USERDATA(SIGVAL)),
 *              si_trapno   = (LUA_TNUMBER),
 *              si_timerid  = (LUA_TNUMBER),
 *              si_overrun  = (LUA_TNUMBER),
 *              si_mqd      = (LUA_TNUMBER),
 *              si_band     = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = siginfo:get_table()
 */
static int
SIGINFO_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(_SIGINFO, TYPE, __func__);

    xtp.xtp_fill = siginfo_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate siginfo{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = siginfo:dump()
 */
static int
SIGINFO_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(_SIGINFO, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get address value of faulting instruction.
 *
 * @function si_addr
 *
 * @return (LUA_T{NIL,STRING} [, LUA_TNUMBER, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:si_addr()
 */
static int
SIGINFO_si_addr(lua_State *L)
{
    luab_module_t *m;
    siginfo_t *si;
    void *dp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(_SIGINFO, TYPE, __func__);
    si = luab_udata(L, 1, m, siginfo_t *);
    dp = si->si_addr;

    return (luab_pushfstring(L, "(%s)", dp));
}

/*
 * Access functions.
 */

/***
 * Set signal number.
 *
 * @function set_si_signo
 *
 * @param arg               Instance of (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:set_si_signo(arg)
 */
static int
SIGINFO_set_si_signo(lua_State *L)
{
    luab_module_t *m0, *m1;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(_SIGINFO, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    si = luab_udata(L, 1, m0, siginfo_t *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    si->si_signo = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get signal number.
 *
 * @function get_si_signo
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:get_si_signo()
 */
static int
SIGINFO_get_si_signo(lua_State *L)
{
    luab_module_t *m;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(_SIGINFO, TYPE, __func__);
    si = luab_udata(L, 1, m, siginfo_t *);
    x = si->si_signo;

    return (luab_pushxinteger(L, x));
}

/***
 * Set errno association.
 *
 * @function set_si_errno
 *
 * @param arg               Instance of (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, min ] = siginfo:set_si_errno(arg)
 */
static int
SIGINFO_set_si_errno(lua_State *L)
{
    luab_module_t *m0, *m1;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(_SIGINFO, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    si = luab_udata(L, 1, m0, siginfo_t *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    si->si_errno = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get errno association.
 *
 * @function get_si_errno
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, min ] = siginfo:get_si_errno()
 */
static int
SIGINFO_get_si_errno(lua_State *L)
{
    luab_module_t *m;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(_SIGINFO, TYPE, __func__);
    si = luab_udata(L, 1, m, siginfo_t *);
    x = si->si_errno;

    return (luab_pushxinteger(L, x));
}

/***
 * Set signal code.
 *
 * @function set_si_code
 *
 * @param arg               Instance of (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:set_si_code(arg)
 */
static int
SIGINFO_set_si_code(lua_State *L)
{
    luab_module_t *m0, *m1;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(_SIGINFO, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    si = luab_udata(L, 1, m0, siginfo_t *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    si->si_code = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get signal code.
 *
 * @function get_si_code
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:get_si_code()
 */
static int
SIGINFO_get_si_code(lua_State *L)
{
    luab_module_t *m;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(_SIGINFO, TYPE, __func__);
    si = luab_udata(L, 1, m, siginfo_t *);
    x = si->si_code;

    return (luab_pushxinteger(L, x));
}

/***
 * Set sender's PID.
 *
 * @function set_si_pid
 *
 * @param arg               Instance of (LUA_T{NUMBER,USERDATA(PID)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:set_si_pid(arg)
 */
static int
SIGINFO_set_si_pid(lua_State *L)
{
    luab_module_t *m0, *m1;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(_SIGINFO, TYPE, __func__);
    m1 = luab_xmod(PID, TYPE, __func__);

    si = luab_udata(L, 1, m0, siginfo_t *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    si->si_pid = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get sender's PID.
 *
 * @function get_si_pid
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:get_si_pid()
 */
static int
SIGINFO_get_si_pid(lua_State *L)
{
    luab_module_t *m;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(_SIGINFO, TYPE, __func__);
    si = luab_udata(L, 1, m, siginfo_t *);
    x = si->si_pid;

    return (luab_pushxinteger(L, x));
}

/***
 * Set sender's remote UID.
 *
 * @function set_si_uid
 *
 * @param arg               Instance of (LUA_T{NUMBER,USERDATA(UID)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:set_si_uid(arg)
 */
static int
SIGINFO_set_si_uid(lua_State *L)
{
    luab_module_t *m0, *m1;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(_SIGINFO, TYPE, __func__);
    m1 = luab_xmod(UID, TYPE, __func__);

    si = luab_udata(L, 1, m0, siginfo_t *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    si->si_uid = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get sender's remote UID.
 *
 * @function get_si_uid
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:get_si_uid()
 */
static int
SIGINFO_get_si_uid(lua_State *L)
{
    luab_module_t *m;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(_SIGINFO, TYPE, __func__);
    si = luab_udata(L, 1, m, siginfo_t *);
    x = si->si_uid;

    return (luab_pushxinteger(L, x));
}

/***
 * Set exit status.
 *
 * @function set_si_status
 *
 * @param arg               Instance of (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:set_si_status(arg)
 */
static int
SIGINFO_set_si_status(lua_State *L)
{
    luab_module_t *m0, *m1;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(_SIGINFO, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    si = luab_udata(L, 1, m0, siginfo_t *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    si->si_status = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get exit status.
 *
 * @function get_si_status
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:get_si_status()
 */
static int
SIGINFO_get_si_status(lua_State *L)
{
    luab_module_t *m;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(_SIGINFO, TYPE, __func__);
    si = luab_udata(L, 1, m, siginfo_t *);
    x = si->si_status;

    return (luab_pushxinteger(L, x));
}

/***
 * Set signal value.
 *
 * @function set_si_value
 *
 * @param arg               Its value, by an instance of (LUA_TUSERDATA(SIGVAL)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sigval [, err, msg ] = siginfo:set_si_value(arg)
 */
static int
SIGINFO_set_si_value(lua_State *L)
{
    luab_module_t *m0, *m1;
    siginfo_t *si;
    union sigval *x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(_SIGINFO, TYPE, __func__);
    m1 = luab_xmod(SIGVAL, TYPE, __func__);

    si = luab_udata(L, 1, m0, siginfo_t *);

    if ((x = luab_udataisnil(L, 2, m1, union sigval *)) != NULL)
        (void)memmove(&si->si_value, x, m1->m_sz);
    else
        (void)memset_s(&si->si_value, m1->m_sz, 0, m1->m_sz);

    return (luab_pushxdata(L, m1, x));
}

/***
 * Get signal value.
 *
 * @function get_si_value
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sigval [, err, msg ] = siginfo:get_si_value()
 */
static int
SIGINFO_get_si_value(lua_State *L)
{
    luab_module_t *m0, *m1;
    siginfo_t *si;
    union sigval *x;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(_SIGINFO, TYPE, __func__);
    m1 = luab_xmod(SIGVAL, TYPE, __func__);

    si = luab_udata(L, 1, m0, siginfo_t *);
    x = &si->si_value;

    return (luab_pushxdata(L, m1, x));
}

/***
 * Set machine specific trap code.
 *
 * @function set_si_trapno
 *
 * @param arg               Instance of (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:set_si_trapno(arg)
 */
static int
SIGINFO_set_si_trapno(lua_State *L)
{
    luab_module_t *m0, *m1;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(_SIGINFO, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    si = luab_udata(L, 1, m0, siginfo_t *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    si->si_trapno = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get machine specific trap code.
 *
 * @function get_si_trapno
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:get_si_trapno()
 */
static int
SIGINFO_get_si_trapno(lua_State *L)
{
    luab_module_t *m;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(_SIGINFO, TYPE, __func__);
    si = luab_udata(L, 1, m, siginfo_t *);
    x = si->si_trapno;

    return (luab_pushxinteger(L, x));
}

/***
 * Set timer ID value.
 *
 * @function set_si_timerid
 *
 * @param arg               Instance of (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:set_si_timerid(arg)
 */
static int
SIGINFO_set_si_timerid(lua_State *L)
{
    luab_module_t *m0, *m1;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(_SIGINFO, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    si = luab_udata(L, 1, m0, siginfo_t *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    si->si_timerid = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get timer ID value.
 *
 * @function get_si_timerid
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:get_si_timerid()
 */
static int
SIGINFO_get_si_timerid(lua_State *L)
{
    luab_module_t *m;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(_SIGINFO, TYPE, __func__);
    si = luab_udata(L, 1, m, siginfo_t *);
    x = si->si_timerid;

    return (luab_pushxinteger(L, x));
}

/***
 * Set overrun value.
 *
 * @function set_si_overrun
 *
 * @param arg               Instance of (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:set_si_overrun(arg)
 */
static int
SIGINFO_set_si_overrun(lua_State *L)
{
    luab_module_t *m0, *m1;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(_SIGINFO, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    si = luab_udata(L, 1, m0, siginfo_t *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    si->si_timerid = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get overrun value.
 *
 * @function get_si_overrun
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:get_si_overrun()
 */
static int
SIGINFO_get_si_overrun(lua_State *L)
{
    luab_module_t *m;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(_SIGINFO, TYPE, __func__);
    si = luab_udata(L, 1, m, siginfo_t *);
    x = si->si_overrun;

    return (luab_pushxinteger(L, x));
}

/***
 * Set mqd value.
 *
 * @function set_si_mqd
 *
 * @param arg               Instance of (LUA_T{NUMBER,USERDATA(INT)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:set_si_mqd(arg)
 */
static int
SIGINFO_set_si_mqd(lua_State *L)
{
    luab_module_t *m0, *m1;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(_SIGINFO, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    si = luab_udata(L, 1, m0, siginfo_t *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    si->si_mqd = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get mqd value.
 *
 * @function get_si_mqd
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:get_si_mqd()
 */
static int
SIGINFO_get_si_mqd(lua_State *L)
{
    luab_module_t *m;
    siginfo_t *si;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(_SIGINFO, TYPE, __func__);
    si = luab_udata(L, 1, m, siginfo_t *);
    x = si->si_mqd;

    return (luab_pushxinteger(L, x));
}

/***
 * Set value for band event for SIGPOLL.
 *
 * @function set_si_band
 *
 * @param arg               Instance of (LUA_T{NUMBER,USERDATA(LONG)}).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:set_si_band(arg)
 */
static int
SIGINFO_set_si_band(lua_State *L)
{
    luab_module_t *m0, *m1;
    siginfo_t *si;
    long x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(_SIGINFO, TYPE, __func__);
    m1 = luab_xmod(LONG, TYPE, __func__);

    si = luab_udata(L, 1, m0, siginfo_t *);
    x = (long)luab_checkxinteger(L, 2, m1, luab_env_ulong_max);

    si->si_band = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get value for band event for SIGPOLL.
 *
 * @function get_si_band
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = siginfo:get_si_band()
 */
static int
SIGINFO_get_si_band(lua_State *L)
{
    luab_module_t *m;
    siginfo_t *si;
    long x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(_SIGINFO, TYPE, __func__);
    si = luab_udata(L, 1, m, siginfo_t *);
    x = si->si_band;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
SIGINFO_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(_SIGINFO, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
SIGINFO_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(_SIGINFO, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
SIGINFO_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(_SIGINFO, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

static luab_module_table_t siginfo_methods[] = {
    LUAB_FUNC("si_addr",            SIGINFO_si_addr),
    LUAB_FUNC("set_si_signo",       SIGINFO_set_si_signo),
    LUAB_FUNC("set_si_errno",       SIGINFO_set_si_errno),
    LUAB_FUNC("set_si_code",        SIGINFO_set_si_code),
    LUAB_FUNC("set_si_pid",         SIGINFO_set_si_pid),
    LUAB_FUNC("set_si_uid",         SIGINFO_set_si_uid),
    LUAB_FUNC("set_si_status",      SIGINFO_set_si_status),
    LUAB_FUNC("set_si_value",       SIGINFO_set_si_value),
    LUAB_FUNC("set_si_trapno",      SIGINFO_set_si_trapno),
    LUAB_FUNC("set_si_timerid",     SIGINFO_set_si_timerid),
    LUAB_FUNC("set_si_overrun",     SIGINFO_set_si_overrun),
    LUAB_FUNC("set_si_mqd",         SIGINFO_set_si_mqd),
    LUAB_FUNC("set_si_band",        SIGINFO_set_si_band),
    LUAB_FUNC("get_table",          SIGINFO_get_table),
    LUAB_FUNC("get_si_signo",       SIGINFO_get_si_signo),
    LUAB_FUNC("get_si_errno",       SIGINFO_get_si_errno),
    LUAB_FUNC("get_si_code",        SIGINFO_get_si_code),
    LUAB_FUNC("get_si_pid",         SIGINFO_get_si_pid),
    LUAB_FUNC("get_si_uid",         SIGINFO_get_si_uid),
    LUAB_FUNC("get_si_status",      SIGINFO_get_si_status),
    LUAB_FUNC("get_si_value",       SIGINFO_get_si_value),
    LUAB_FUNC("get_si_trapno",      SIGINFO_get_si_trapno),
    LUAB_FUNC("get_si_timerid",     SIGINFO_get_si_timerid),
    LUAB_FUNC("get_si_overrun",     SIGINFO_get_si_overrun),
    LUAB_FUNC("get_si_mqd",         SIGINFO_get_si_mqd),
    LUAB_FUNC("get_si_band",        SIGINFO_get_si_band),
    LUAB_FUNC("dump",               SIGINFO_dump),
    LUAB_FUNC("__gc",               SIGINFO_gc),
    LUAB_FUNC("__len",              SIGINFO_len),
    LUAB_FUNC("__tostring",         SIGINFO_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
siginfo_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(_SIGINFO, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
siginfo_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(_SIGINFO, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
siginfo_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(_SIGINFO, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
siginfo_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    siginfo_t *x, *y;
    size_t i, j;

    m = luab_xmod(_SIGINFO, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (siginfo_t *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, siginfo_t *);
                        (void)memmove(&(x[i]), y, m->m_sz);
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
siginfo_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    siginfo_t *x;
    size_t i, j, k;

    m = luab_xmod(_SIGINFO, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (siginfo_t *)tbl->tbl_vec) != NULL) &&
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
siginfo_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(_SIGINFO, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_siginfo_type = {
    .m_id           = LUAB__SIGINFO_TYPE_ID,
    .m_name         = LUAB__SIGINFO_TYPE,
    .m_vec          = siginfo_methods,
    .m_create       = siginfo_create,
    .m_init         = siginfo_init,
    .m_get          = siginfo_udata,
    .m_get_tbl      = siginfo_checktable,
    .m_set_tbl      = siginfo_pushtable,
    .m_alloc_tbl    = siginfo_alloctable,
    .m_len          = sizeof(luab_siginfo_t),
    .m_sz           = sizeof(siginfo_t),
};
#endif /* __POSIX_VISIBLE >= 199309 */
