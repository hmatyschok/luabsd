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

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

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
 *      struct iovec    iov;
 *      size_t  iov_max_len;
 *      u_int   iov_flags;
 *  } luab_iovec_t;
 */

#define luab_newiovec(L, arg) \
    ((luab_iovec_t *)luab_newuserdata(L, &iovec_type, (arg)))
#define luab_to_iovec(L, narg) \
    (luab_todata((L), (narg), &iovec_type, luab_iovec_t *))

#define LUABSD_IOVEC_TYPE_ID    1594559731
#define LUABSD_IOVEC_TYPE   "IOVEC*"

int luab_iovec_create(lua_State *);

/*
 * Accessor for immutable properties.
 */

/***
 * Get length of stored data.
 *
 * @function len
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (nbytes [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage nbytes [, err, msg ] = iovec:len()
 */
static int
IOVEC_len(lua_State *L)
{
    luab_iovec_t *self;
    size_t nbytes;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        nbytes = self->iov.iov_len;

        self->iov_flags &= ~IOV_LOCK;
    } else {
        errno = EBUSY;
        nbytes = -1;
    }
    return (luab_pusherr(L, nbytes));
}

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
    size_t nbytes;

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
 * Generator functions.
 */

/***
 * Copy data from attributes of (LUA_TUSERDATA(iovec)) into (LUA_TTABLE).
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

    luab_setldata(L, -2, "iov_base", self->iov.iov_base, self->iov.iov_len);
    luab_setinteger(L, -2, "iov_len", self->iov.iov_len);
    luab_setinteger(L, -2, "iov_max_len", self->iov_max_len);
    luab_setinteger(L, -2, "iov_max_len", self->iov_flags);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function, creates deep copy as instance of (LUA_TUSERDATA(iovec)).
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

        status = luab_pushiovec(L, self->iov.iov_base,
            self->iov.iov_len, self->iov_max_len);

        self->iov_flags &= ~IOV_LOCK;
    } else {
        errno = EBUSY;
        status = luab_pushnil(L);
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
    caddr_t dp;
    size_t len;
    int status;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        if (((dp = self->iov.iov_base) != NULL) &&
            ((len = self->iov_max_len) > 0) &&
            (self->iov_flags & IOV_BUFF)) {
            (void)memset_s(dp, len, 0, len);
            self->iov.iov_len = 0;
            status = 0;
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

/***
 * Write data into buffer.
 *
 * @function copy_in
 *
 * @param data              Either (LUA_TSTRING) or (LUA_TUSERDATA(iovec)).
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
    const char *data;
    caddr_t dp;
    size_t len;
    int status;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_iovec(L, 1);
    data = luab_iovec_checklxarg(L, 2, self->iov_max_len);
    len = self->iov_max_len;

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        if (((dp = self->iov.iov_base) != NULL) &&
            (self->iov_flags & IOV_BUFF)) {
            (void)memmove(dp, data, len);
            self->iov.iov_len = len;
            status = 0;
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
    caddr_t dp;
    size_t len;
    int status;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        if (((dp = self->iov.iov_base) != NULL) &&
            ((len = self->iov.iov_len) > 0))
            status = luab_pushldata(L, dp, len);
        else {
            errno = ENXIO;
            status = luab_pushnil(L);
        }
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
    size_t len;
    caddr_t src, dst;
    int status;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_iovec(L, 1);
    len = (size_t)luab_checkinteger(L, 2,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        if (((src = self->iov.iov_base) != NULL) &&
            (self->iov_flags & IOV_BUFF) && (len > 1)) {

            if ((dst = realloc(src, len)) != NULL) {
                self->iov.iov_base = dst;

                if (len <= self->iov.iov_len)
                    self->iov.iov_len = len;

                self->iov_max_len = len;
                status = 0;
            } else
                status = -1;
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
 * Meta-methods.
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
        dp = self->iov.iov_base;
        len = self->iov_max_len;

        (void)memset_s(dp, len, 0, len);

        free(dp);

        self->iov.iov_base = NULL;
    }
    self->iov.iov_len = 0;
    self->iov_max_len = 0;
    self->iov_flags = 0;

    return (0);
}

static int
IOVEC_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &iovec_type));
}

/*
 * Method-table.
 */

static luab_table_t iovec_methods[] = {
    LUABSD_FUNC("clear",    IOVEC_clear),
    LUABSD_FUNC("clone",    IOVEC_clone),
    LUABSD_FUNC("copy_in",  IOVEC_copy_in),
    LUABSD_FUNC("copy_out",  IOVEC_copy_out),
    LUABSD_FUNC("get",  IOVEC_get),
    LUABSD_FUNC("len",  IOVEC_len),
    LUABSD_FUNC("max_len",  IOVEC_max_len),
    LUABSD_FUNC("resize",   IOVEC_resize),
    LUABSD_FUNC("__gc", IOVEC_gc),
    LUABSD_FUNC("__tostring",   IOVEC_tostring),
    LUABSD_FUNC(NULL, NULL)
};

/*
 * Internal interface.
 */

static void *
iovec_create(lua_State *L, void *arg)
{
    luab_iovec_param_t *iop;
    luab_iovec_t *self;
    size_t max_len;

    if ((iop = (luab_iovec_param_t *)arg) != NULL) {
        if ((max_len = iop->iop_buf.buf_len) > 1) {

            if (luab_buf_alloc(&iop->iop_buf, max_len) != 0)
                iop->iop_flags = IOV_PROXY;
            else
                iop->iop_flags = IOV_BUFF;
        } else
            iop->iop_flags = IOV_PROXY;

        if (iop->iop_flags & IOV_BUFF)
            self = luab_newiovec(L, iop);
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

        self->iov_max_len = iop->iop_buf.buf_len;
        self->iov.iov_base = iop->iop_buf.buf_data;
        self->iov_flags = iop->iop_flags;

        if (((max_len = self->iov_max_len) > 0) &&
            ((dst = self->iov.iov_base) != NULL)) {

            if (((src = iop->iop_data.buf_data) != NULL) &&
                ((len = iop->iop_data.buf_len) <= max_len)) {
                (void)memmove(dst, src, len);
                self->iov.iov_len = len;
            }
        }
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

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(iovec)).
 *
 * @function iovec_create
 *
 * @param max_len           Denodes capacity in bytes.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = bsd.sys.uio.iovec_create(max_len)
 */

int
luab_iovec_create(lua_State *L)
{
    size_t max_len;

    (void)luab_checkmaxargs(L, 1);

    max_len = (size_t)luab_checkinteger(L, 1,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    return (luab_pushiovec(L, NULL, 0, max_len));
}
