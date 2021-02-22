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

#include <sys/uio.h>

#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_iovec_type;

/*
 * Interface against
 *
 *  struct iovec {
 *      void    *iov_base;
 *      size_t   iov_len;
 *  };
 *
 * by
 *
 *  typedef struct luab_iovec {
 *      luab_udata_t    ud_softc;
 *      struct iovec    iov;
 *      size_t  iov_max_len;
 *      u_int   iov_flags;
 *  } luab_iovec_t;
 */

#define luab_to_iovec(L, narg) \
    (luab_todata((L), (narg), m, luab_iovec_t *))

/*
 * Subr.
 */

static void
iovec_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_iovec_t *self;

    if ((self = (luab_iovec_t *)arg) != NULL) {

        luab_setldata(L, narg, "iov_base",    self->iov.iov_base, self->iov.iov_len);
        luab_setinteger(L, narg, "iov_len",       self->iov.iov_len);
        luab_setinteger(L, narg, "iov_max_len",   self->iov_max_len);
        luab_setinteger(L, narg, "iov_flags",     self->iov_flags);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Copy data from attributes of (LUA_TUSERDATA(IOVEC)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              iov_base    = (LUA_T{NIL,STRING}),
 *              iov_len     = (LUA_TNUMBER),
 *              iov_max_len = (LUA_TNUMBER),
 *              iov_flags   = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = iovec:get_table()
 */
static int
IOVEC_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IOVEC, TYPE, __func__);

    xtp.xtp_fill = iovec_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function, creates deep copy as instance of (LUA_TUSERDATA(IOVEC)).
 *
 * @function clone
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = iovec:clone()
 */
static int
IOVEC_clone(lua_State *L)
{
    luab_module_t *m;
    luab_iovec_t *self;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IOVEC, TYPE, __func__);
    self = luab_udata(L, 1, m, luab_iovec_t *);

    luab_thread_mtx_lock(L, __func__);;
    status = luab_iovec_pushxdata(L, self->iov.iov_base,
        self->iov.iov_len, self->iov_max_len);
    luab_thread_mtx_unlock(L, __func__);;

    return (status);
}

/***
 * Generator function - returns (LUA_TNIL).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = cmsgcred:dump()
 */
static int
IOVEC_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get capacity.
 *
 * @function max_len
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage nbytes [, err, msg ] = iovec:max_len()
 */
static int
IOVEC_max_len(lua_State *L)
{
    luab_module_t *m;
    luab_iovec_t *self;
    ssize_t nbytes;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IOVEC, TYPE, __func__);
    self = luab_udata(L, 1, m, luab_iovec_t *);

    luab_thread_mtx_lock(L, __func__);;
    nbytes = self->iov_max_len;
    luab_thread_mtx_unlock(L, __func__);;

    return (luab_pushxinteger(L, nbytes));
}

/*
 * Access functions.
 */

/***
 * Set length of data region, see {p}readv(2) for further details.
 *
 * @function set_len
 *
 * @param nbytes            Amount of rx'd data in bytes.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage nbytes [, err, msg ] = iovec:set_len(nbytes)
 */
static int
IOVEC_set_len(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *self;
    size_t nbytes;
    struct iovec *iov;
    ssize_t len;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);

    self = luab_udata(L, 1, m0, luab_iovec_t *);
    nbytes = (size_t)luab_checklxinteger(L, 2, m1, 0);
    iov = &(self->iov);

    if ((nbytes <= self->iov_max_len) &&
        (nbytes > 1)) {

        luab_thread_mtx_lock(L, __func__);;

        iov->iov_len = nbytes;
        len = nbytes;

        luab_thread_mtx_unlock(L, __func__);;
    } else {
        errno = ERANGE;
        len = luab_env_error;
    }
    return (luab_pushxinteger(L, len));
}

/***
 * Get length of stored data.
 *
 * @function get_len
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage nbytes [, err, msg ] = iovec:get_len()
 */
static int
IOVEC_get_len(lua_State *L)
{
    luab_module_t *m;
    luab_iovec_t *self;
    struct iovec *iov;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IOVEC, TYPE, __func__);
    self = luab_udata(L, 1, m, luab_iovec_t *);
    iov = &(self->iov);

    luab_thread_mtx_lock(L, __func__);;
    status = luab_iov_pushlen(L, iov);
    luab_thread_mtx_unlock(L, __func__);;
    return (status);
}

/*
 * Storage-methods.
 */

/***
 * Zero-out.
 *
 * @function clear
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = iovec:clear()
 */
static int
IOVEC_clear(lua_State *L)
{
    luab_module_t *m;
    luab_iovec_t *self;
    struct iovec *iov;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IOVEC, TYPE, __func__);
    self = luab_udata(L, 1, m, luab_iovec_t *);
    iov = &(self->iov);

    luab_thread_mtx_lock(L, __func__);;

    if (self->iov_flags & IOV_BUFF)
        status = luab_iov_clear(iov);
    else {
        errno = ERANGE;
        status = luab_env_error;
    }
    luab_thread_mtx_unlock(L, __func__);;
    return (luab_pushxinteger(L, status));
}

/***
 * Write data into buffer.
 *
 * @function copy_in
 *
 * @param arg               Either (LUA_TSTRING) or (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = iovec:copy_in(arg)
 */
static int
IOVEC_copy_in(lua_State *L)
{
    luab_module_t *m;
    luab_iovec_t *self;
    const char *dp;
    size_t len;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(IOVEC, TYPE, __func__);
    self = luab_udata(L, 1, m, luab_iovec_t *);
    len = self->iov_max_len;

    dp = luab_iovec_checklstring(L, 2, len);
    status = luab_iovec_copyin(self, dp, len);
    return (luab_pushxinteger(L, status));
}

/***
 * Read data from buffer.
 *
 * @function copy_out
 *
 * @return (LUA_TSTRING [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = iovec:copy_out()
 */
static int
IOVEC_copy_out(lua_State *L)
{
    luab_module_t *m;
    luab_iovec_t *self;
    struct iovec *iov;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IOVEC, TYPE, __func__);
    self = luab_udata(L, 1, m, luab_iovec_t *);
    iov = &(self->iov);

    luab_thread_mtx_lock(L, __func__);;
    status = luab_iov_pushdata(L, iov);
    luab_thread_mtx_unlock(L, __func__);;
    return (status);
}

/***
 * Reallocate buffer maps to iov_base.
 *
 * @function resize
 *
 * @param len               Size by (LUA_TNUMBER).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = iovec:resize(len)
 */
static int
IOVEC_resize(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *self;
    struct iovec *iov;
    size_t len;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);

    self = luab_udata(L, 1, m0, luab_iovec_t *);
    iov = &(self->iov);

    len = (size_t)luab_checklxinteger(L, 2, m1, 0);

    luab_thread_mtx_lock(L, __func__);;

    if (self->iov_flags & IOV_BUFF) {

        if ((status = luab_iov_realloc(iov, len)) == 0) {

            if (len < self->iov_max_len)
                self->iov_max_len = len;
        }
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    luab_thread_mtx_unlock(L, __func__);;

    return (luab_pushxinteger(L, status));
}

/*
 * File I/O.
 */

/***
 * Read input from file into instance of (LUA_TUSERDATA(IOVEC)).
 *
 * @function read
 *
 * @param fd                Open file descriptor.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = iovec:read(fd)
 */
static int
IOVEC_read(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *self;
    int fd;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    self = luab_udata(L, 1, m0, luab_iovec_t *);
    fd = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    return (luab_iovec_read(L, fd, self, NULL));
}

/***
 * Write output from instance of (LUA_TUSERDAT(iovec)).
 *
 * @function write
 *
 * @param fd                Open file descriptor.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = iovec:write(fd)
 */
static int
IOVEC_write(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *self;
    int fd;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    self = luab_udata(L, 1, m0, luab_iovec_t *);
    fd = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    return (luab_iovec_write(L, fd, self, NULL));
}

/*
 * Socket I/O.
 */

/***
 * Receive message(s) from a socket(9).
 *
 * @function recv
 *
 * @param s                 Open socket(9).
 * @param flags             Flags argument over
 *
 *                              bsd.sys.socket.MSG_{OOB,PEEK,WAITALL,
 *                                  DONTWAIT,CMSG_CLOEXEC}
 *
 *                          may combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = iovec:recv(s, flags)
 */
static int
IOVEC_recv(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *self;
    int s, flags;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    self = luab_udata(L, 1, m0, luab_iovec_t *);
    s = (int)luab_checkxinteger(L, 2, m1, luab_env_int_max);
    flags = (int)luab_checkxinteger(L, 3, m1, luab_env_int_max);

    return (luab_iovec_recv(L, s, self, NULL, flags));
}

/***
 * Send message(s) from a socket(9).
 *
 * @function send
 *
 * @param s                 Open socket(9).
 * @param flags             Flags argument over
 *
 *                              bsd.sys.socket.MSG_{OOB,PEEK,WAITALL,
 *                                  DONTWAIT,CMSG_CLOEXEC}
 *
 *                          may combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = iovec:send(s, flags)
 */
static int
IOVEC_send(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *self;
    int s, flags;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    self = luab_udata(L, 1, m0, luab_iovec_t *);
    s = (int)luab_checkxinteger(L, 2, m1, luab_env_int_max);
    flags = (int)luab_checkxinteger(L, 3, m1, luab_env_int_max);

    return (luab_iovec_send(L, s, self, NULL, flags));
}

/*
 * Metamethods.
 */

static int
IOVEC_gc(lua_State *L)
{
    luab_module_t *m;
    luab_iovec_t *self;
    caddr_t dp;
    size_t len;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IOVEC, TYPE, __func__);
    self = luab_udata(L, 1, m, luab_iovec_t *);

    if (((dp = self->iov.iov_base) != NULL) &&
        (self->iov_flags & IOV_BUFF)) {
        len = self->iov_max_len;

        (void)memset_s(dp, len, 0, len);

        free(dp);
    } else
        dp = NULL;

    return (luab_core_gc(L, 1, m));
}

static int
IOVEC_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IOVEC, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
IOVEC_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IOVEC, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t iovec_methods[] = {
    LUAB_FUNC("get_table",      IOVEC_get_table),
    LUAB_FUNC("set_len",        IOVEC_set_len),
    LUAB_FUNC("get_len",        IOVEC_get_len),
    LUAB_FUNC("max_len",        IOVEC_max_len),
    LUAB_FUNC("clear",          IOVEC_clear),
    LUAB_FUNC("clone",          IOVEC_clone),
    LUAB_FUNC("copy_in",        IOVEC_copy_in),
    LUAB_FUNC("copy_out",       IOVEC_copy_out),
    LUAB_FUNC("resize",         IOVEC_resize),
    LUAB_FUNC("read",           IOVEC_read),
    LUAB_FUNC("write",          IOVEC_write),
    LUAB_FUNC("recv",           IOVEC_recv),
    LUAB_FUNC("send",           IOVEC_send),
    LUAB_FUNC("dump",           IOVEC_dump),
    LUAB_FUNC("__gc",           IOVEC_gc),
    LUAB_FUNC("__len",          IOVEC_len),
    LUAB_FUNC("__tostring",     IOVEC_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
iovec_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    luab_iovec_param_t *iop;
    luab_iovec_t *self;
    size_t max_len;

    m = luab_xmod(IOVEC, TYPE, __func__);

    if ((iop = (luab_iovec_param_t *)arg) != NULL) {
        if ((max_len = iop->iop_iov.iov_len) > 1) {

            if (luab_iov_alloc(&iop->iop_iov, max_len) != 0)
                iop->iop_flags = IOV_PROXY;
            else
                iop->iop_flags = IOV_BUFF;
        } else
            iop->iop_flags = IOV_PROXY;

        if (iop->iop_flags & IOV_BUFF)
            self = luab_newuserdata(L, m, iop);
        else
            self = NULL;    /* XXX IOV_PROXY, not yet. */
    } else
        self = NULL;

    return (self);
}

static void
iovec_init(void *ud, void *arg)
{
    luab_iovec_t *self;
    luab_iovec_param_t *iop;
    size_t len, max_len;
    caddr_t src, dst;

    if (((self = (luab_iovec_t *)ud) != NULL) &&
        ((iop = (luab_iovec_param_t *)arg) != NULL)) {

        self->iov.iov_base = iop->iop_iov.iov_base;
        self->iov_max_len = iop->iop_iov.iov_len;

        if (((max_len = self->iov_max_len) > 1) &&
            ((dst = self->iov.iov_base) != NULL)) {

            if (((src = iop->iop_data.iov_base) != NULL) &&
                ((len = iop->iop_data.iov_len) <= max_len)) {
                (void)memmove(dst, src, len);
                self->iov.iov_len = len;
            }
        }
        self->iov_flags = iop->iop_flags;
    }
}

static void *
iovec_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(IOVEC, TYPE, __func__);
    return (luab_checkudata(L, narg, m));
}

static luab_table_t *
iovec_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(IOVEC, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_iovec_type = {
    .m_id           = LUAB_IOVEC_TYPE_ID,
    .m_name         = LUAB_IOVEC_TYPE,
    .m_vec          = iovec_methods,
    .m_create       = iovec_create,
    .m_init         = iovec_init,
    .m_get          = iovec_udata,
    .m_get_tbl      = luab_iovec_checktable,
    .m_set_tbl      = luab_iovec_pushtable,
    .m_alloc_tbl    = iovec_alloctable,
    .m_len          = sizeof(luab_iovec_t),
    .m_sz           = sizeof(struct iovec),
};
