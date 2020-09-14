
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
 * Generic service primitives for handling iovec{}s.
 * 
 *   #1 bp refers iov->iov_base.
 * 
 *   #2 dp or v refers external data.
 */

int
luab_buf_clear(struct iovec *iov)
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
luab_buf_alloc(struct iovec *iov, size_t len)
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
luab_buf_realloc(struct iovec *iov, size_t len)
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
luab_buf_copyin(struct iovec *iov, const void *v, size_t len)
{
    caddr_t bp;
    int status;

    if (iov != NULL && v != NULL) {

        if (((bp = iov->iov_base) != NULL) &&
            (len <= iov->iov_len)) {
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
luab_buf_copyout(struct iovec *iov, void *v, size_t len)
{
    caddr_t bp;
    int status;

    if (iov != NULL && v != NULL) {

        if (((bp = iov->iov_base) != NULL) &&
            (len <= iov->iov_len)) {
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
luab_buf_free(struct iovec *iov)
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
