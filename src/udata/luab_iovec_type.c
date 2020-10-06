/*
 * Copyright (c) 2020 Henning Matyschok <hmatyschok@outlook.com>
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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_types.h"

extern luab_module_t iovec_type;

/*
 * Interface against
 *
 *  struct iovec {
 *      void    *iov_base;
 *      size_t   iov_len;
 *  };
 *
 * maps to
 *
 *  typedef struct luab_iovec {
 *      luab_udata_t    ud_softc;
 *      struct iovec    iov;
 *      size_t  iov_max_len;
 *      u_int   iov_flags;
 *  } luab_iovec_t;
 */

#define luab_newiovec(L, arg) \
    ((luab_iovec_t *)luab_newudata(L, &iovec_type, (arg)))
#define luab_to_iovec(L, narg) \
    (luab_todata((L), (narg), &iovec_type, luab_iovec_t *))

#define LUABSD_IOVEC_TYPE_ID    1594559731
#define LUABSD_IOVEC_TYPE   "IOVEC*"

/*
 * Generator functions.
 */

/***
 * Copy data from attributes of (LUA_TUSERDATA(IOVEC)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              iov_base    = (LUA_TSTRING) or (LUA_TNIL),
 *              iov_len     = (LUA_TNUMBER),
 *              iov_max_len = (LUA_TNUMBER),
 *              iov_flags   = (LUA_TNUMBER),
 *          }
 *
 * @usage t = iovec:get()
 */
static int
IOVEC_get(lua_State *L)
{
    luab_iovec_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);

    lua_newtable(L);

    luab_setldata(L, -2, "iov_base",    self->iov.iov_base, self->iov.iov_len);
    luab_setinteger(L, -2, "iov_len",       self->iov.iov_len);
    luab_setinteger(L, -2, "iov_max_len",   self->iov_max_len);
    luab_setinteger(L, -2, "iov_flags",     self->iov_flags);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function, creates deep copy as instance of (LUA_TUSERDATA(IOVEC)).
 *
 * @function clone
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = iovec:clone()
 */
static int
IOVEC_clone(lua_State *L)
{
    luab_iovec_t *self;
    int status;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        status = luab_iovec_pushudata(L, self->iov.iov_base,
            self->iov.iov_len, self->iov_max_len);

        self->iov_flags &= ~IOV_LOCK;
    } else {
        errno = EBUSY;
        status = luab_pushnil(L);
    }
    return (status);
}

/*
 * Accessor, immutable properties.
 */

/***
 * Get capacity.
 *
 * @function max_len
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (nbytes [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage nbytes [, err, msg ] = iovec:max_len()
 */
static int
IOVEC_max_len(lua_State *L)
{
    luab_iovec_t *self;
    ssize_t nbytes;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        nbytes = self->iov_max_len;

        self->iov_flags &= ~IOV_LOCK;
    } else {
        errno = EBUSY;
        nbytes = -1;
    }
    return (luab_pusherr(L, nbytes));
}

/*
 * Accessor.
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
 *          (nbytes [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage nbytes [, err, msg ] = iovec:set_len(nbytes)
 */
static int
IOVEC_set_len(lua_State *L)
{
    luab_iovec_t *self;
    size_t nbytes;
    struct iovec *iov;
    ssize_t len;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_iovec(L, 1);
    nbytes = (size_t)luab_checkinteger(L, 2,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    iov = &(self->iov);

    if ((nbytes <= self->iov_max_len) &&
        (nbytes > 1)) {

        if ((self->iov_flags & IOV_LOCK) == 0) {
            self->iov_flags |= IOV_LOCK;

            iov->iov_len = nbytes;
            len = nbytes;

            self->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            len = -1;
        }
    } else {
        errno = ERANGE;
        len = -1;
    }
    return (luab_pusherr(L, len));
}

/***
 * Get length of stored data.
 *
 * @function get_len
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (nbytes [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage nbytes [, err, msg ] = iovec:get_len()
 */
static int
IOVEC_get_len(lua_State *L)
{
    luab_iovec_t *self;
    struct iovec *iov;
    int status;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);
    iov = &(self->iov);

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        status = luab_iov_pushlen(L, iov);

        self->iov_flags &= ~IOV_LOCK;
    } else {
        errno = EBUSY;
        status = luab_pusherr(L, -1);
    }
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
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = iovec:clear()
 */
static int
IOVEC_clear(lua_State *L)
{
    luab_iovec_t *self;
    struct iovec *iov;
    int status;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);
    iov = &(self->iov);

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        if (self->iov_flags & IOV_BUFF)
            status = luab_iov_clear(iov);
        else {
            errno = ENXIO;
            status = -1;
        }
        self->iov_flags &= ~IOV_LOCK;
    } else {
        errno = EBUSY;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * Write data into buffer.
 *
 * @function copy_in
 *
 * @param data              Either (LUA_TSTRING) or (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = iovec:copy_in(data)
 */
static int
IOVEC_copy_in(lua_State *L)
{
    luab_iovec_t *self;
    const char *dp;
    size_t len;
    int status;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_iovec(L, 1);
    len = self->iov_max_len;

    dp = luab_iovec_checklxarg(L, 2, len);
    status = luab_iovec_copyin(self, dp, len);
    return (luab_pusherr(L, status));
}

/***
 * Read data from buffer.
 *
 * @function copy_out
 *
 * @return (LUA_TSTRING [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (data [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage data [, err, msg ] = iovec:copy_out()
 */
static int
IOVEC_copy_out(lua_State *L)
{
    luab_iovec_t *self;
    struct iovec *iov;
    int status;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);
    iov = &(self->iov);

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        status = luab_iov_pushdata(L, iov);

        self->iov_flags &= ~IOV_LOCK;
    } else {
        errno = EBUSY;
        status = luab_pushnil(L);
    }
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
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = iovec:resize(len)
 */
static int
IOVEC_resize(lua_State *L)
{
    luab_iovec_t *self;
    struct iovec *iov;
    size_t len;
    int status;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_iovec(L, 1);
    iov = &(self->iov);

    len = (size_t)luab_checkinteger(L, 2,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        if (self->iov_flags & IOV_BUFF) {

            if ((status = luab_iov_realloc(iov, len)) == 0) {

                if (len < self->iov_max_len)
                    self->iov_max_len = len;
            }
        } else {
            errno = ENXIO;
            status = -1;
        }
        self->iov_flags &= ~IOV_LOCK;
    } else {
        errno = EBUSY;
        status = -1;
    }
    return (luab_pusherr(L, status));
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
 *          (count [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage count [, err, msg ] = iovec:read(fd)
 */
static int
IOVEC_read(lua_State *L)
{
    luab_iovec_t *self;
    int fd;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_iovec(L, 1);
    fd = (int)luab_checkinteger(L, 2, INT_MAX);

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
 *          (count [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage count [, err, msg ] = iovec:write(fd)
 */
static int
IOVEC_write(lua_State *L)
{
    luab_iovec_t *self;
    int fd;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_iovec(L, 1);
    fd = (int)luab_checkinteger(L, 2, INT_MAX);

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
 *          (count [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage count [, err, msg ] = iovec:recv(s, flags)
 */
static int
IOVEC_recv(lua_State *L)
{
    luab_iovec_t *self;
    int s, flags;

    (void)luab_checkmaxargs(L, 3);

    self = luab_to_iovec(L, 1);
    s = (int)luab_checkinteger(L, 2, INT_MAX);
    flags = (int)luab_checkinteger(L, 3, INT_MAX);

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
 *          (count [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage count [, err, msg ] = iovec:send(s, flags)
 */
static int
IOVEC_send(lua_State *L)
{
    luab_iovec_t *self;
    int s, flags;

    (void)luab_checkmaxargs(L, 3);

    self = luab_to_iovec(L, 1);
    s = (int)luab_checkinteger(L, 2, INT_MAX);
    flags = (int)luab_checkinteger(L, 3, INT_MAX);

    return (luab_iovec_send(L, s, self, NULL, flags));
}

/*
 * Metamethods.
 */

static int
IOVEC_gc(lua_State *L)
{
    luab_iovec_t *self;
    caddr_t dp;
    size_t len;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);

    if ((self->iov.iov_base != NULL) &&
        (self->iov_flags & IOV_BUFF)) {
        dp = self->iov.iov_base;    /* XXX */
        len = self->iov_max_len;

        (void)memset_s(dp, len, 0, len);

        free(dp);
    }
    return (luab_gc(L, 1, &iovec_type));
}

static int
IOVEC_len(lua_State *L)
{
    return (luab_len(L, 2, &iovec_type));
}

static int
IOVEC_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &iovec_type));
}

/*
 * Internal interface.
 */

static luab_table_t iovec_methods[] = {
    LUABSD_FUNC("get",          IOVEC_get),
    LUABSD_FUNC("set_len",      IOVEC_set_len),
    LUABSD_FUNC("get_len",      IOVEC_get_len),
    LUABSD_FUNC("max_len",      IOVEC_max_len),
    LUABSD_FUNC("clear",        IOVEC_clear),
    LUABSD_FUNC("clone",        IOVEC_clone),
    LUABSD_FUNC("copy_in",      IOVEC_copy_in),
    LUABSD_FUNC("copy_out",     IOVEC_copy_out),
    LUABSD_FUNC("resize",       IOVEC_resize),
    LUABSD_FUNC("read",         IOVEC_read),
    LUABSD_FUNC("write",        IOVEC_write),
    LUABSD_FUNC("recv",         IOVEC_recv),
    LUABSD_FUNC("send",         IOVEC_send),
    LUABSD_FUNC("__gc",         IOVEC_gc),
    LUABSD_FUNC("__len",        IOVEC_len),
    LUABSD_FUNC("__tostring",   IOVEC_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
iovec_create(lua_State *L, void *arg)
{
    luab_iovec_param_t *iop;
    luab_iovec_t *self;
    size_t max_len;

    if ((iop = (luab_iovec_param_t *)arg) != NULL) {
        if ((max_len = iop->iop_iov.iov_len) > 1) {

            if (luab_iov_alloc(&iop->iop_iov, max_len) != 0)
                iop->iop_flags = IOV_PROXY;
            else
                iop->iop_flags = IOV_BUFF;
        } else
            iop->iop_flags = IOV_PROXY;

        if (iop->iop_flags & IOV_BUFF)
            self = luab_newiovec(L, iop);
        else
            self = NULL;    /* XXX IOV_PROXY, not yet. */

        (void)memset_s(iop, sizeof(*iop), 0, sizeof(*iop));
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
    return (luab_to_iovec(L, narg));
}

luab_module_t iovec_type = {
    .cookie = LUABSD_IOVEC_TYPE_ID,
    .name = LUABSD_IOVEC_TYPE,
    .vec = iovec_methods,
    .create = iovec_create,
    .init = iovec_init,
    .get = iovec_udata,
    .sz = sizeof(luab_iovec_t),
};
