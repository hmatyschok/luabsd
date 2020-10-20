/*
 * Copyright (c) 2020 Henning Matyschok
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

#include <sys/socket.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

/*
 * Initializes message primitive for instantiation of (LUA_TUSERDATA(IOVEC)).
 *
 *  #1: The parameter len specifies size of data region v.
 *
 *  #2: The parameter max_len specifies size of data region iov_base, but the
 *      size in bytes is constrained by the value of (LUAL_BUFFERSIZE).
 *
 *  #3: If constraints are viaolated, NULL will be returned and call of
 *
 *       (a) luab_pushudata(3),
 *
 *       (b) luab_rawsetudata(3) or
 *
 *       (c) luab_setudata(3)
 *
 *     fails implecitely.
 */
luab_module_t *
luab_iovec_param_init(luab_iovec_param_t *iop, void *v, size_t len, size_t max_len)
{
    luab_module_t *m;

    if ((iop != NULL) &&
        (len <= max_len) &&
        (max_len <= LUAL_BUFFERSIZE)) {
        (void)memset_s(iop, sizeof(*iop), 0, sizeof(*iop));

        iop->iop_iov.iov_len = max_len;

        if (v != NULL && len > 1) {
            iop->iop_data.iov_len = (len == max_len) ? max_len : len;
            iop->iop_data.iov_base = v;
        }
        m = luab_mx(IOVEC);
    } else
        m = NULL;

    return (m);
}

/*
 * Access functions, [stack -> C].
 */

caddr_t
luab_iovec_toldata(lua_State *L, int narg, size_t len)
{
    luab_iovec_t *buf;
    caddr_t bp;

    buf = luab_udata(L, narg, luab_mx(IOVEC), luab_iovec_t *);

    if ((buf->iov.iov_len <= buf->iov_max_len) &&
        (len <= buf->iov_max_len))
        bp = buf->iov.iov_base;
    else {
        errno = ERANGE;
        bp = NULL;
    }
    return (bp);
}

const char *
luab_iovec_islstring(lua_State *L, int narg, size_t len)
{
    const char *dp;

    if (luab_isiovec(L, narg) != NULL)
        dp = luab_iovec_toldata(L, narg, len);
    else
        dp = luab_tolstring(L, narg, len);

    return (dp);
}

const char *
luab_iovec_checklstring(lua_State *L, int narg, size_t len)
{
    const char *dp;

    if ((dp = luab_iovec_islstring(L, narg, len)) == NULL)
        luab_core_argerror(L, narg, NULL, 0, 0, EINVAL);

    return (dp);
}

/*
 * Operations on stack.
 */

int
luab_iovec_pushudata(lua_State *L, void *v, size_t len, size_t max_len)
{
    luab_iovec_param_t mpi;
    luab_module_t *m;

    m = luab_iovec_param_init(&mpi, v, len, max_len);
    return (luab_pushudata(L, m, &mpi));
}

/*
 * Operations on (LUA_TTABLE).
 */

void
luab_iovec_rawsetldata(lua_State *L, int narg, lua_Integer k, void *v, size_t len)
{
    luab_iovec_param_t mpi;
    luab_module_t *m;

    m = luab_iovec_param_init(&mpi, v, len, len);
    luab_rawsetudata(L, narg, m, k, &mpi);
}

void
luab_iovec_setldata(lua_State *L, int narg, const char *k, void *v, size_t len)
{
    luab_iovec_param_t mpi;
    luab_module_t *m;

    m = luab_iovec_param_init(&mpi, v, len, len);
    luab_setudata(L, narg, m, k, &mpi);
}

/*
 * Generic accessor.
 */

/* dp -> buf */
int
luab_iovec_copyin(luab_iovec_t *buf, const void *dp, size_t len)
{
    struct iovec *iov;
    size_t olen;
    int status;

    if ((buf != NULL) &&
        (len <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            iov = &(buf->iov);
            olen = iov->iov_len;
            iov->iov_len = len;

            if ((status = luab_iov_copyin(iov, dp, len)) != 0)
                iov->iov_len = olen;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (status);
}

/* buf -> dp */
int
luab_iovec_copyout(luab_iovec_t *buf, void *dp, size_t len)
{
    struct iovec *iov;
    int status;

    if ((buf != NULL) &&
        (len <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            iov = &(buf->iov);
            status = luab_iov_copyout(iov, dp, len);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (status);
}

/*
 * Service primitives, file I/O.
 */

int
luab_iovec_read(lua_State *L, int fd, luab_iovec_t *buf, size_t *n)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if ((buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            len = (n == NULL) ? buf->iov_max_len : *n;

            if (((bp = buf->iov.iov_base) != NULL) &&
                (len <= buf->iov_max_len)) {

                if ((count = read(fd, bp, len)) > 0)
                    buf->iov.iov_len = count;
            } else {
                errno = ENXIO;
                count = -1;
            }
            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}

int
luab_iovec_readv(lua_State *L, int fd, luab_iovec_t *buf, size_t n)
{
    struct iovec *iov;
    ssize_t count;

    if ((buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            iov = &(buf->iov);
            count = luab_iov_readv(iov, fd, n);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}

int
luab_iovec_write(lua_State *L, int fd, luab_iovec_t *buf, size_t *n)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if ((buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            len = (n == NULL) ? buf->iov.iov_len : *n;

            if (((bp = buf->iov.iov_base) != NULL) &&
                (len <= buf->iov_max_len))
                count = write(fd, bp, len);
            else {
                errno = ENXIO;
                count = -1;
            }
            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}

int
luab_iovec_writev(lua_State *L, int fd, luab_iovec_t *buf, size_t n)
{
    struct iovec *iov;
    ssize_t count;

    if ((buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            iov = &(buf->iov);
            count = luab_iov_writev(iov, fd, n);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}

/* 1003.1-2001 */
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
int
luab_iovec_readlink(lua_State *L, const char *path, luab_iovec_t *buf, size_t *n)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if ((path != NULL) &&
        (buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            len = (n == NULL) ? buf->iov_max_len : *n;

            if (((bp = buf->iov.iov_base) != NULL) &&
                (len <= buf->iov_max_len)) {

                if ((count = readlink(path, bp, len)) > 0)
                    buf->iov.iov_len = count;
            } else {
                errno = ENXIO;
                count = -1;
            }
            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}
#endif /* __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE */

/* 1003.1-2008 */
#if __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE
int
luab_iovec_pread(lua_State *L, int fd, luab_iovec_t *buf, size_t *n, off_t off)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if ((buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            len = (n == NULL) ? buf->iov_max_len : *n;

            if (((bp = buf->iov.iov_base) != NULL) &&
                (len <= buf->iov_max_len)) {

                if ((count = pread(fd, bp, len, off)) > 0)
                    buf->iov.iov_len = count;
            } else {
                errno = ENXIO;
                count = -1;
            }
            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}

int
luab_iovec_pwrite(lua_State *L, int fd, luab_iovec_t *buf, size_t *n, off_t off)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if ((buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            len = (n == NULL) ? buf->iov.iov_len : *n;

            if (((bp = buf->iov.iov_base) != NULL) &&
                (len <= buf->iov_max_len))
                count = pwrite(fd, bp, len, off);
            else {
                errno = ENXIO;
                count = -1;
            }
            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}
#endif /* __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE */

#if __POSIX_VISIBLE >= 200809
int
luab_iovec_readlinkat(lua_State *L, int fd, const char *path,
    luab_iovec_t *buf, size_t *n)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if ((path != NULL) &&
        (buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            len = (n == NULL) ? buf->iov_max_len : *n;

            if (((bp = buf->iov.iov_base) != NULL) &&
                (len <= buf->iov_max_len)) {

                if ((count = readlinkat(fd, path, bp, len)) > 0)
                    buf->iov.iov_len = count;
            } else {
                errno = ENXIO;
                count = -1;
            }
            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}
#endif /* __POSIX_VISIBLE >= 200809 */

#if __BSD_VISIBLE
int
luab_iovec_preadv(lua_State *L, int fd, luab_iovec_t *buf, size_t n, off_t off)
{
    struct iovec *iov;
    ssize_t count;

    if ((buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            iov = &(buf->iov);
            count = luab_iov_preadv(iov, fd, n, off);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}

int
luab_iovec_pwritev(lua_State *L, int fd, luab_iovec_t *buf, size_t n, off_t off)
{
    struct iovec *iov;
    ssize_t count;

    if ((buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            iov = &(buf->iov);
            count = luab_iov_pwritev(iov, fd, n, off);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}
#endif /* __BSD_VISIBLE */

/*
 * Service primitives, socket I/O.
 */

int
luab_iovec_recv(lua_State *L, int s, luab_iovec_t *buf, size_t *n, int flags)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if ((buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if (n == NULL)
                len = buf->iov_max_len;
            else
                len = *n;

            if (((bp = buf->iov.iov_base) != NULL) &&
                (len <= buf->iov_max_len)) {

                if ((count = recv(s, bp, len, flags)) > 0)
                    buf->iov.iov_len = count;
            } else {
                errno = ENXIO;
                count = -1;
            }
            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}

int
luab_iovec_recvfrom(lua_State *L, int s, luab_iovec_t *buf, size_t *n,
    int flags, struct sockaddr *from, socklen_t *fromlen)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if ((buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if (n == NULL)
                len = buf->iov_max_len;
            else
                len = *n;

            if (((bp = buf->iov.iov_base) != NULL) &&
                (len <= buf->iov_max_len)) {

                if ((count = recvfrom(s, bp, len, flags, from, fromlen)) > 0)
                    buf->iov.iov_len = count;
            } else {
                errno = ENXIO;
                count = -1;
            }
            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}

int
luab_iovec_send(lua_State *L, int s, luab_iovec_t *buf, size_t *n, int flags)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if ((buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if (n == NULL)
                len = buf->iov.iov_len;
            else
                len = *n;

            if (((bp = buf->iov.iov_base) != NULL) &&
                (len <= buf->iov_max_len))
                count = send(s, bp, len, flags);
            else {
                errno = ENXIO;
                count = -1;
            }
            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}

int
luab_iovec_sendto(lua_State *L, int s, luab_iovec_t *buf, size_t *n,
    int flags, struct sockaddr *to, socklen_t tolen)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if ((buf != NULL) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if (n == NULL)
                len = buf->iov.iov_len;
            else
                len = *n;

            if (((bp = buf->iov.iov_base) != NULL) &&
                (len <= buf->iov_max_len))
                count = sendto(s, bp, len, flags, to, tolen);
            else {
                errno = ENXIO;
                count = -1;
            }
            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pusherr(L, count));
}
