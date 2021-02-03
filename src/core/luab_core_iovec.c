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

#include <sys/uio.h>

#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

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

static void
luab_iovec_init(lua_State *L, int narg, struct iovec *iov)
{
    luab_iovec_t *buf;
    struct iovec *src;

    if (((buf = luab_isiovec(L, narg)) != NULL) &&
        ((buf->iov_flags & IOV_LOCK) == 0)) {
        buf->iov_flags |= IOV_LOCK;

        src = &(buf->iov);

        if ((luab_iov_alloc(iov, src->iov_len)) == 0)
            (void)luab_iov_copyin(iov, src->iov_base, src->iov_len);
                /* XXX */
        buf->iov_flags &= ~IOV_LOCK;
    }
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
            status = luab_env_success;
        } else {
            errno = ERANGE;
            status = luab_env_error;
        }
    } else {
        errno = EINVAL;
        status = luab_env_error;
    }
    return (status);
}

int
luab_iov_alloc(struct iovec *iov, size_t len)
{
    int status;

    if (iov != NULL && len > 1) {

        if ((iov->iov_base = luab_core_alloc(len, sizeof(char))) != NULL) {
            iov->iov_len = len;
            status = luab_env_success;
        } else {
            iov->iov_len = 0;
            status = luab_env_error;
        }
    } else {
        errno = EINVAL;
        status = luab_env_error;
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
            status = luab_env_success;
        } else
            status = luab_env_error;
    } else {
        errno = EINVAL;
        status = luab_env_error;
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

            status = luab_env_success;
        } else {
            errno = ERANGE;
            status = luab_env_error;
        }
    } else {
        errno = EINVAL;
        status = luab_env_error;
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

            status = luab_env_success;
        } else {
            errno = ERANGE;
            status = luab_env_error;
        }
    } else {
        errno = EINVAL;
        status = luab_env_error;
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
        status = luab_env_success;
    } else {
        errno = EINVAL;
        status = luab_env_error;
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

    if (iov != NULL) {
        if (iov->iov_base != NULL) {

            if (n <= iov->iov_len)
                count = readv(fd, iov, n);
            else {
                errno = ERANGE;
                count = luab_env_error;
            }
        } else {
            errno = ERANGE;
            count = luab_env_error;
        }
    } else {
        errno = EINVAL;
        count = luab_env_error;
    }
    return (count);
}

ssize_t
luab_iov_writev(struct iovec *iov, int fd, size_t n)
{
    ssize_t count;

    if (iov != NULL) {
        if (iov->iov_base != NULL) {

            if (n <= iov->iov_len)
                count = writev(fd, iov, n);
            else {
                errno = ERANGE;
                count = luab_env_error;
            }
        } else {
            errno = ERANGE;
            count = luab_env_error;
        }
    } else {
        errno = EINVAL;
        count = luab_env_error;
    }
    return (count);
}

#if __BSD_VISIBLE
ssize_t
luab_iov_preadv(struct iovec *iov, int fd, size_t n, off_t off)
{
    ssize_t count;

    if (iov != NULL) {
        if (iov->iov_base != NULL) {

            if (n <= iov->iov_len && (size_t)off < n)
                count = preadv(fd, iov, n, off);
            else {
                errno = ERANGE;
                count = luab_env_error;
            }
        } else {
            errno = ERANGE;
            count = luab_env_error;
        }
    } else {
        errno = EINVAL;
        count = luab_env_error;
    }
    return (count);
}

ssize_t
luab_iov_pwritev(struct iovec *iov, int fd, size_t n, off_t off)
{
    ssize_t count;

    if (iov != NULL) {
        if (iov->iov_base != NULL) {

            if (n <= iov->iov_len && (size_t)off < n)
                count = pwritev(fd, iov, n, off);
            else {
                errno = ERANGE;
                count = luab_env_error;
            }
        } else {
            errno = ERANGE;
            count = luab_env_error;
        }
    } else {
        errno = EINVAL;
        count = luab_env_error;
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
        len = luab_env_error;
    }
    return (luab_pushxinteger(L, len));
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
    return (luab_iovec_pushxdata(L, dp, len, len));
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

/*
 * Generic service primitives.
 */

void
luab_iovec_freetable(luab_table_t *tbl)
{
    struct iovec *x;
    size_t m, n;

    if (tbl != NULL) {

        if (((x = tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0) &&
            (tbl->tbl_sz == sizeof(struct iovec))) {

            for (m = 0, n = (tbl->tbl_card -1); m < n; m++)
                (void)luab_iov_free(&(x[m]));

            errno = ENOENT;
        }
        luab_table_free(tbl);
    } else
        errno = ERANGE;
}

/*
 * Message primitive for instantiation of (LUA_TUSERDATA(IOVEC)) are initialized
 * by luab_iovec_param_init(3):
 *
 *  #1: The parameter len specifies size of data region v in bytes.
 *
 *  #2: The parameter max_len specifies size of data region iov_base, but the
 *      size in bytes is constrained by the value of (luab_env_buf_max).
 *
 *  #3: If constraints are viaolated, NULL will be returned and call of
 *
 *       (a) luab_pushxdata(3),
 *
 *       (b) luab_rawsetxdata(3) or
 *
 *       (c) luab_setxdata(3)
 *
 *     fails implecitely.
 */

luab_module_t *
luab_iovec_param_init(luab_iovec_param_t *iop, void *v, size_t len, size_t max_len)
{
    luab_module_t *m;

    m = luab_xmod(IOVEC, TYPE, __func__);

    if ((iop != NULL) &&
        (len <= max_len) &&
        (max_len <= luab_env_buf_max)) {
        (void)memset_s(iop, sizeof(*iop), 0, sizeof(*iop));

        iop->iop_iov.iov_len = max_len;

        if (v != NULL && len > 1) {
            iop->iop_data.iov_len = (len == max_len) ? max_len : len;
            iop->iop_data.iov_base = v;
        }
    } else
        m = NULL;

    return (m);
}

/*
 * Access functions, [stack -> C].
 */

luab_iovec_t *
luab_isiovec(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(IOVEC, TYPE, __func__);
    return (luab_isdata(L, narg, m, luab_iovec_t *));
}

caddr_t
luab_iovec_toldata(lua_State *L, int narg, size_t len)
{
    luab_module_t *m;
    luab_iovec_t *buf;
    caddr_t bp;

    m = luab_xmod(IOVEC, TYPE, __func__);
    buf = luab_udata(L, narg, m, luab_iovec_t *);

    if ((buf->iov_max_len <= luab_env_buf_max) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
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
        dp = luab_islstring(L, narg, len);

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
luab_iovec_pushxdata(lua_State *L, void *v, size_t len, size_t max_len)
{
    luab_iovec_param_t mpi;
    luab_module_t *m;

    m = luab_iovec_param_init(&mpi, v, len, max_len);
    return (luab_pushxdata(L, m, &mpi));
}

/*
 * Table operations.
 */

void
luab_iovec_rawsetldata(lua_State *L, int narg, lua_Integer k, void *v, size_t len)
{
    luab_iovec_param_t mpi;
    luab_module_t *m;

    m = luab_iovec_param_init(&mpi, v, len, len);
    luab_rawsetxdata(L, narg, m, k, &mpi);
}

void
luab_iovec_setldata(lua_State *L, int narg, const char *k, void *v, size_t len)
{
    luab_iovec_param_t mpi;
    luab_module_t *m;

    m = luab_iovec_param_init(&mpi, v, len, len);
    luab_setxdata(L, narg, m, k, &mpi);
}

luab_table_t *
luab_iovec_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct iovec *x;
    size_t i, j;

    m = luab_xmod(IOVEC, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct iovec *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        luab_iovec_init(L, -1, &(x[i]));
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        } else
            errno = ERANGE;
    }
    return (tbl);
}

void
luab_iovec_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct iovec *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (struct iovec *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, new);

            for (m = 0, n = tbl->tbl_card, k = 1; m < n; m++, k++)
                luab_iov_rawsetxdata(L, narg, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_iovec_freetable(tbl);
    } else
        errno = ERANGE;
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

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            (len <= buf->iov_max_len) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

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
            errno = ERANGE;
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

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            (len <= buf->iov_max_len) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

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
            errno = ERANGE;
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

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

            if ((buf->iov_flags & IOV_LOCK) == 0) {
                buf->iov_flags |= IOV_LOCK;

                len = (n == NULL) ? buf->iov_max_len : *n;

                if (((bp = buf->iov.iov_base) != NULL) &&
                    (len <= buf->iov_max_len)) {

                    if ((count = read(fd, bp, len)) > 0)
                        buf->iov.iov_len = count;
                } else {
                    errno = ERANGE;
                    count = -1;
                }
                buf->iov_flags &= ~IOV_LOCK;
            } else {
                errno = EBUSY;
                count = -1;
            }
        } else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
}

int
luab_iovec_readv(lua_State *L, int fd, luab_iovec_t *buf, size_t n)
{
    struct iovec *iov;
    ssize_t count;

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

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
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
}

int
luab_iovec_write(lua_State *L, int fd, luab_iovec_t *buf, size_t *n)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

            if ((buf->iov_flags & IOV_LOCK) == 0) {
                buf->iov_flags |= IOV_LOCK;

                len = (n == NULL) ? buf->iov.iov_len : *n;

                if (((bp = buf->iov.iov_base) != NULL) &&
                    (len <= buf->iov_max_len))
                    count = write(fd, bp, len);
                else {
                    errno = ERANGE;
                    count = -1;
                }
                buf->iov_flags &= ~IOV_LOCK;
            } else {
                errno = EBUSY;
                count = -1;
            }
        } else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
}

int
luab_iovec_writev(lua_State *L, int fd, luab_iovec_t *buf, size_t n)
{
    struct iovec *iov;
    ssize_t count;

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

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
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
}

/* 1003.1-2001 */
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
int
luab_iovec_readlink(lua_State *L, const char *path, luab_iovec_t *buf, size_t *n)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if (path != NULL && buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

            if ((buf->iov_flags & IOV_LOCK) == 0) {
                buf->iov_flags |= IOV_LOCK;

                len = (n == NULL) ? buf->iov_max_len : *n;

                if (((bp = buf->iov.iov_base) != NULL) &&
                    (len <= buf->iov_max_len)) {

                    if ((count = readlink(path, bp, len)) > 0)
                        buf->iov.iov_len = count;
                } else {
                    errno = ERANGE;
                    count = -1;
                }
                buf->iov_flags &= ~IOV_LOCK;
            } else {
                errno = EBUSY;
                count = -1;
            }
        } else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
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

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

            if ((buf->iov_flags & IOV_LOCK) == 0) {
                buf->iov_flags |= IOV_LOCK;

                len = (n == NULL) ? buf->iov_max_len : *n;

                if (((bp = buf->iov.iov_base) != NULL) &&
                    (len <= buf->iov_max_len)) {

                if ((count = pread(fd, bp, len, off)) > 0)
                    buf->iov.iov_len = count;
                } else {
                    errno = ERANGE;
                    count = -1;
                }
                buf->iov_flags &= ~IOV_LOCK;
            } else {
                errno = EBUSY;
                count = -1;
            }
        } else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
}

int
luab_iovec_pwrite(lua_State *L, int fd, luab_iovec_t *buf, size_t *n, off_t off)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

            if ((buf->iov_flags & IOV_LOCK) == 0) {
                buf->iov_flags |= IOV_LOCK;

                len = (n == NULL) ? buf->iov.iov_len : *n;

                if (((bp = buf->iov.iov_base) != NULL) &&
                    (len <= buf->iov_max_len))
                    count = pwrite(fd, bp, len, off);
                else {
                    errno = ERANGE;
                    count = -1;
                }
                buf->iov_flags &= ~IOV_LOCK;
            } else {
                errno = EBUSY;
                count = -1;
            }
        } else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
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

    if (path != NULL && buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

            if ((buf->iov_flags & IOV_LOCK) == 0) {
                buf->iov_flags |= IOV_LOCK;

                len = (n == NULL) ? buf->iov_max_len : *n;

                if (((bp = buf->iov.iov_base) != NULL) &&
                    (len <= buf->iov_max_len)) {

                    if ((count = readlinkat(fd, path, bp, len)) > 0)
                        buf->iov.iov_len = count;
                } else {
                    errno = ERANGE;
                    count = -1;
                }
                buf->iov_flags &= ~IOV_LOCK;
            } else {
                errno = EBUSY;
                count = -1;
            }
        } else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
}
#endif /* __POSIX_VISIBLE >= 200809 */

#if __BSD_VISIBLE
int
luab_iovec_preadv(lua_State *L, int fd, luab_iovec_t *buf, size_t n, off_t off)
{
    struct iovec *iov;
    ssize_t count;

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

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
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
}

int
luab_iovec_pwritev(lua_State *L, int fd, luab_iovec_t *buf, size_t n, off_t off)
{
    struct iovec *iov;
    ssize_t count;

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

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
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
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

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

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
                    errno = ERANGE;
                    count = -1;
                }
                buf->iov_flags &= ~IOV_LOCK;
            } else {
                errno = EBUSY;
                count = -1;
            }
        } else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
}

int
luab_iovec_recvfrom(lua_State *L, int s, luab_iovec_t *buf, size_t *n,
    int flags, struct sockaddr *from, socklen_t *fromlen)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

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
                    errno = ERANGE;
                    count = -1;
                }
                buf->iov_flags &= ~IOV_LOCK;
            } else {
                errno = EBUSY;
                count = -1;
            }
        } else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
}

int
luab_iovec_send(lua_State *L, int s, luab_iovec_t *buf, size_t *n, int flags)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

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
                    errno = ERANGE;
                    count = -1;
                }
                buf->iov_flags &= ~IOV_LOCK;
            } else {
                errno = EBUSY;
                count = -1;
            }
        } else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
}

int
luab_iovec_sendto(lua_State *L, int s, luab_iovec_t *buf, size_t *n,
    int flags, struct sockaddr *to, socklen_t tolen)
{
    caddr_t bp;
    size_t len;
    ssize_t count;

    if (buf != NULL) {

        if ((buf->iov_max_len <= luab_env_buf_max) &&
            ((buf->iov_flags & IOV_BUFF) != 0)) {

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
                    errno = ERANGE;
                    count = -1;
                }
                buf->iov_flags &= ~IOV_LOCK;
            } else {
                errno = EBUSY;
                count = -1;
            }
        } else {
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = EINVAL;
        count = -1;
    }
    return (luab_pushxinteger(L, count));
}
