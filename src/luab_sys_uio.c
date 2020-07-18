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
 *  } luab_iovec_t;
 */

#define LUABSD_IOVEC_TYPE_ID    1594559731
#define LUABSD_IOVEC_TYPE   "IOVEC*"

#define luab_newiovec(L, arg) \
    ((luab_iovec_t *)luab_newuserdata(L, &iovec_type, (arg)))
#define luab_toiovec(L, narg) \
    (luab_todata((L), (narg), &iovec_type, luab_iovec_t *))

static int
IOVec_clear(lua_State *L)
{
    luab_iovec_t *self;
    caddr_t buf;
    size_t len;
    int status;

    luab_checkmaxargs(L, 1);

    self = luab_toiovec(L, 1);

    if ((buf = self->iov.iov_base) != NULL) {
        if ((len = self->iov_max_len) > 0) {
            (void)memset_s(buf, len, 0, len);

            len = self->iov.iov_len;
            self->iov.iov_len = 0;
            status = len;
        } else {
            errno = ENOENT;
            status = errno;
        }
    } else {
        errno = ENXIO;
        status = errno;
    }
    return luab_pusherr(L, status);
}

static int
IOVec_copyin(lua_State *L)
{
    luab_iovec_t *self;
    const char *src;
    caddr_t dst;
    size_t len;
    int status;

    luab_checkmaxargs(L, 2);

    self = luab_toiovec(L, 1);
    src = luab_checklstring(L, 2, self->iov_max_len);

    if ((dst = self->iov.iov_base) != NULL) {
        len = strlen(src);
        (void)memmove(dst, src, len);
        self->iov.iov_len = len;
        status = len;
    } else {
        errno = ENXIO;
        status = errno;
    }
    return luab_pusherr(L, status);
}

static int
IOVec_copyout(lua_State *L)
{
    luab_iovec_t *self;
    luaL_Buffer b;
    caddr_t src, dst;
    size_t len;
    int status;

    luab_checkmaxargs(L, 1);

    self = luab_toiovec(L, 1);

    if ((src = self->iov.iov_base) != NULL) {
        if ((len = self->iov.iov_len) > 0) {
            luaL_buffinit(L, &b);

            dst = luaL_prepbuffsize(&b, len);

            (void)memmove(dst, src, len);

            luaL_addsize(&b, len);
            luaL_pushresult(&b);

            status = 1;
        } else {
            errno = ENOENT;
            status = luab_pushnil(L);
        }
    } else {
        errno = ENXIO;
        status = luab_pushnil(L);
    }
    return status;
}

static int
IOVec_len(lua_State *L)
{
    luab_iovec_t *self;
    size_t len;

    luab_checkmaxargs(L, 1);

    self = luab_toiovec(L, 2);
    len = self->iov.iov_len;

    return luab_pusherr(L, len);
}

static int
IOVec_resize(lua_State *L)
{
    luab_iovec_t *self;
    size_t len;
    caddr_t src, dst;
    int status;

    luab_checkmaxargs(L, 2);

    self = luab_toiovec(L, 1);
    len = luab_checkinteger(L, 2,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if (len > 0) {
        if ((src = self->iov.iov_base) != NULL) {
            if ((dst = realloc(src, len)) != NULL) {
                self->iov.iov_base = dst;

                if (len <= self->iov.iov_len)
                    self->iov.iov_len = len;

                self->iov_max_len = len;
                status = len;
                errno = 0;
            } else
                status = errno;
        } else {
            errno = ENXIO;
            status = errno;
        }
    } else {
        errno = EINVAL;
        status = errno;
    }
    return luab_pusherr(L, status);
}

static int
IOVec_gc(lua_State *L)
{
    luab_iovec_t *self = luab_toiovec(L, 1);
    caddr_t buf;
    size_t len;

    if (self->iov.iov_base != NULL) {
        buf = self->iov.iov_base;
        len = self->iov_max_len;

        (void)memset_s(buf, len, 0, len);

        free(buf);

        self->iov.iov_base = NULL;
        self->iov.iov_len = 0;

        self->iov_max_len = 0;
    }
    return 0;
}

static int
IOVec_tostring(lua_State *L)
{
    luab_iovec_t *self = luab_toiovec(L, 1);
    lua_pushfstring(L, "IOVec (%p)", self);

    return 1;
}

static luab_table_t iovec_methods[] = {
    LUABSD_FUNC("clear",    IOVec_clear),
    LUABSD_FUNC("copyin",  IOVec_copyin),
    LUABSD_FUNC("copyout",  IOVec_copyout),
    LUABSD_FUNC("len",  IOVec_len),
    LUABSD_FUNC("resize",   IOVec_resize),
    LUABSD_FUNC("__gc", IOVec_gc),
    LUABSD_FUNC("__tostring",   IOVec_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
iovec_udata(lua_State *L, int narg)
{
    return luab_toiovec(L, narg);
}

luab_module_t iovec_type = {
    .cookie = LUABSD_IOVEC_TYPE_ID,
    .name = LUABSD_IOVEC_TYPE,
    .vec = iovec_methods,
    .get = iovec_udata,
    .sz = sizeof(luab_iovec_t),
};

static int
luab_StructIOVec(lua_State *L)
{
    luab_iovec_t *self;
    size_t len;
    caddr_t buf;
    int status;

    (void)luab_checkmaxargs(L, 1);

    len = luab_checkinteger(L, 1,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if (len > 0) {
        if ((buf = calloc(1, len)) != NULL) {
            if ((self = luab_newiovec(L, NULL)) != NULL) {
                self->iov.iov_base = buf;
                self->iov.iov_len = 0;
                self->iov_max_len = len;
                status = 1;
            } else {
                status = luab_pushnil(L);
                free(buf);
            }
        } else
            status = luab_pushnil(L);
    } else {
        errno = EINVAL;
        status = luab_pushnil(L);
    }
    return status;
}

/*
 * Interface against <sys/uio.h>.
 */

#define LUABSD_SYS_UIO_LIB_ID    1594559271
#define LUABSD_SYS_UIO_LIB_KEY   "uio"

static luab_table_t luab_sys_uio_vec[] = {   /* sys/uio.h */
#if __BSD_VISIBLE
    LUABSD_INT("UIO_READ",  UIO_READ),
    LUABSD_INT("UIO_WRITE", UIO_WRITE),
    LUABSD_INT("UIO_USERSPACE", UIO_USERSPACE),
    LUABSD_INT("UIO_SYSSPACE",  UIO_SYSSPACE),
    LUABSD_INT("UIO_NOCOPY",    UIO_NOCOPY),
#endif /* __BSD_VISIBLE */
    LUABSD_FUNC("StructIOVec",  luab_StructIOVec),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_sys_uio_lib = {
    .cookie = LUABSD_SYS_UIO_LIB_ID,
    .name = LUABSD_SYS_UIO_LIB_KEY,
    .vec = luab_sys_uio_vec,
};
