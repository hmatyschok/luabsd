
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
#include "luab_udata.h"

/*
 * Subr.
 */

static caddr_t
luab_iov_base(struct iovec *iov, size_t *len)
{
    caddr_t dp;

    if (iov != NULL && len != NULL) {

        if ((dp = iov->iov_base) != NULL)
            *len = iov->iov_len;
        else
            *len = 0;
    } else
        dp = NULL;

    return (dp);
}

/*
 * Generic service primitives for handling iovec{}s.
 *
 *   #1 bp refers iov->iov_base.
 *
 *   #2 dp or v refers external data.
 */

int
luab_iov_clear(struct iovec *iov)
{
    caddr_t bp;
    size_t len;
    int status;

    if (iov != NULL) {
        if (((bp = iov->iov_base) != NULL) &&
            ((len = iov->iov_len) > 1)) {
            (void)memset_s(bp, len, 0, len);
            status = 0;
        } else {
            errno = ENXIO;
            status = -1;
        }
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (status);
}

int
luab_iov_alloc(struct iovec *iov, size_t len)
{
    int status;

    if (iov != NULL && len > 1) {

        if ((iov->iov_base = malloc(len)) != NULL) {
            (void)memset_s(iov->iov_base, len, 0, len);
            iov->iov_len = len;
            status = 0;
        } else {
            iov->iov_len = 0;
            status = -1;
        }
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (status);
}

int
luab_iov_realloc(struct iovec *iov, size_t len)
{
    caddr_t bp;
    int status;

    if (iov != NULL && len > 1) {

        if ((bp = realloc(iov->iov_base, len)) != NULL) {
            iov->iov_base = bp;
            iov->iov_len = len;
            status = 0;
        } else
            status = -1;
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (status);
}

int
luab_iov_copyin(struct iovec *iov, const void *v, ssize_t len)
{
    caddr_t bp;
    int status;

    if (iov != NULL && v != NULL && len > 0) {

        if (((bp = iov->iov_base) != NULL) &&
            (len == (ssize_t)iov->iov_len)) {
            (void)memmove(bp, v, len);

            status = 0;
        } else {
            errno = ENXIO;
            status = -1;
        }
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (status);
}

int
luab_iov_copyout(struct iovec *iov, void *v, ssize_t len)
{
    caddr_t bp;
    int status;

    if (iov != NULL && v != NULL && len > 0) {

        if (((bp = iov->iov_base) != NULL) &&
            (len == (ssize_t)iov->iov_len)) {
            (void)memmove(v, bp, len);

            status = 0;
        } else {
            errno = ENXIO;
            status = -1;
        }
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (status);
}

int
luab_iov_free(struct iovec *iov)
{
    int status;

    if (iov != NULL) {
        if (iov->iov_base != NULL) {
            free(iov->iov_base);
            iov->iov_base = NULL;
        }
        iov->iov_len = 0;
        status = 0;
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (status);
}

/*
 * I/O.
 */

ssize_t
luab_iov_readv(struct iovec *iov, int fd, size_t n)
{
    ssize_t count;

    if (iov != NULL && iov->iov_base != NULL) {

        if (n <= iov->iov_len)
            count = readv(fd, iov, n);
        else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (count);
}

ssize_t
luab_iov_writev(struct iovec *iov, int fd, size_t n)
{
    ssize_t count;

    if (iov != NULL && iov->iov_base != NULL) {

        if (n <= iov->iov_len)
            count = writev(fd, iov, n);
        else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (count);
}

#if __BSD_VISIBLE
ssize_t
luab_iov_preadv(struct iovec *iov, int fd, size_t n, off_t off)
{
    ssize_t count;

    if (iov != NULL && iov->iov_base != NULL) {

        if (n <= iov->iov_len && (size_t)off < n)
            count = preadv(fd, iov, n, off);
        else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (count);
}

ssize_t
luab_iov_pwritev(struct iovec *iov, int fd, size_t n, off_t off)
{
    ssize_t count;

    if (iov != NULL && iov->iov_base != NULL) {

        if (n <= iov->iov_len && (size_t)off < n)
            count = pwritev(fd, iov, n, off);
        else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (count);
}
#endif

/*
 * Access functions, [C -> stack].
 */

int
luab_iov_pushlen(lua_State *L, struct iovec *iov)
{
    ssize_t len;

    if (iov != NULL)
        len = iov->iov_len;
    else {
        errno = EINVAL;
        len = -1;
    }
    return (luab_pusherr(L, len));
}

int
luab_iov_pushdata(lua_State *L, struct iovec *iov)
{
    caddr_t dp;
    size_t len;

    dp = luab_iov_base(iov, &len);
    return (luab_pushldata(L, dp, len));
}

int
luab_iov_pushxdata(lua_State *L, struct iovec *iov)
{
    caddr_t dp;
    size_t len;

    dp = luab_iov_base(iov, &len);
    return (luab_iovec_pushudata(L, dp, len, len));
}

void
luab_iov_rawsetdata(lua_State *L, int narg, lua_Integer k, struct iovec *iov)
{
    caddr_t dp;
    size_t len;

    dp = luab_iov_base(iov, &len);
    luab_rawsetldata(L, narg, k, dp, len);
}

void
luab_iov_rawsetxdata(lua_State *L, int narg, lua_Integer k, struct iovec *iov)
{
    caddr_t dp;
    size_t len;
(void)printf("%s: (k:%zu,dp:%p,len:%zu)\n", __func__, k, iov->iov_base, iov->iov_len);
    dp = luab_iov_base(iov, &len);
    luab_iovec_rawsetldata(L, narg, k, dp, len);
}

void
luab_iov_setdata(lua_State *L, int narg, const char *k, struct iovec *iov)
{
    caddr_t dp;
    size_t len;

    dp = luab_iov_base(iov, &len);
    luab_setldata(L, narg, k, dp, len);
}

void
luab_iov_setxdata(lua_State *L, int narg, const char *k, struct iovec *iov)
{
    caddr_t dp;
    size_t len;

    dp = luab_iov_base(iov, &len);
    luab_iovec_setldata(L, narg, k, dp, len);
}
