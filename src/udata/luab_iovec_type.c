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
 * Service primitives, accessor.
 */

static int
IOVEC_clear(lua_State *L)
{
    luab_iovec_t *self;
    caddr_t buf;
    size_t len;
    int status;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        if (((buf = self->iov.iov_base) != NULL) &&
            ((len = self->iov_max_len) > 0) &&
            (self->iov_flags & IOV_BUFF)) {
            (void)memset_s(buf, len, 0, len);
            len = self->iov.iov_len;
            self->iov.iov_len = 0;
            status = len;
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

static int
IOVEC_copy_in(lua_State *L)
{
    luab_iovec_t *self;
    const char *src;
    caddr_t dst;
    size_t len;
    int status;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_iovec(L, 1);
    src = luab_iovec_checklxarg(L, 2, self->iov_max_len);
    len = self->iov_max_len;

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        if (((dst = self->iov.iov_base) != NULL) &&
            (self->iov_flags & IOV_BUFF)) {
            (void)memmove(dst, src, len);
            self->iov.iov_len = len;
            status = len;
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

static int
IOVEC_copy_out(lua_State *L)
{
    luab_iovec_t *self;
    caddr_t buf;
    size_t len;
    int status;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);

    if ((self->iov_flags & IOV_LOCK) == 0) {
        self->iov_flags |= IOV_LOCK;

        if (((buf = self->iov.iov_base) != NULL) &&
            ((len = self->iov.iov_len) > 0))
            status = luab_pushldata(L, buf, len);
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

static int
IOVEC_len(lua_State *L)
{
    luab_iovec_t *self;
    size_t len;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);
    len = self->iov.iov_len;

    return (luab_pusherr(L, len));
}

static int
IOVEC_max_len(lua_State *L)
{
    luab_iovec_t *self;
    size_t len;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);
    len = self->iov_max_len;

    return (luab_pusherr(L, len));
}

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
            (self->iov_flags & IOV_BUFF) && (len > 0)) {

            if ((dst = realloc(src, len)) != NULL) {
                self->iov.iov_base = dst;

                if (len <= self->iov.iov_len)
                    self->iov.iov_len = len;

                self->iov_max_len = len;
                status = len;
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

static int
IOVEC_gc(lua_State *L)
{
    luab_iovec_t *self;
    caddr_t buf;
    size_t len;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_iovec(L, 1);

    if ((self->iov.iov_base != NULL) &&
        (self->iov_flags & IOV_BUFF)) {
        buf = self->iov.iov_base;
        len = self->iov_max_len;

        (void)memset_s(buf, len, 0, len);

        free(buf);

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

static luab_table_t iovec_methods[] = {
    LUABSD_FUNC("clear",    IOVEC_clear),
    LUABSD_FUNC("clone",    IOVEC_clone),
    LUABSD_FUNC("copy_in",  IOVEC_copy_in),
    LUABSD_FUNC("copy_out",  IOVEC_copy_out),
    LUABSD_FUNC("len",  IOVEC_len),
    LUABSD_FUNC("max_len",  IOVEC_max_len),
    LUABSD_FUNC("resize",   IOVEC_resize),
    LUABSD_FUNC("__gc", IOVEC_gc),
    LUABSD_FUNC("__tostring",   IOVEC_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
iovec_create(lua_State *L, void *arg)
{
    luab_iovec_param_t *iop;
    luab_iovec_t *self;
    size_t len;

    if ((iop = (luab_iovec_param_t *)arg) != NULL) {
        if ((len = iop->iop_buf.buf_len) > 0) {

            if (luab_buf_alloc(&iop->iop_buf, len) != 0)
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

int
luab_iovec_create(lua_State *L)
{
    luab_iovec_param_t iop;
    int status;

    /* XXX well, this will be refactored, but not yet */
    (void)luab_checkmaxargs(L, 1);

    (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

    iop.iop_buf.buf_len = (size_t)luab_checkinteger(L, 1,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if (iovec_create(L, &iop) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
