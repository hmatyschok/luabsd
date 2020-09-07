
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

/*-
 *  COPYRIGHT (C) 1986 Gary S. Brown.  You may use this program, or
 *  code or tables extracted from it, as desired without restriction.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

/*
 * Common methods on generic buffer.
 */

int
luab_buf_clear(luab_buf_t *buf)
{
    int status;

    if (buf != NULL && buf->buf_data != NULL) {
        if ((buf->buf_flags & IOV_LOCK) == 0) {
            buf->buf_flags |= IOV_LOCK;

            (void)memset_s(buf->buf_data, buf->buf_len, 0, buf->buf_len);

            status = 0;

            buf->buf_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (status);
}

int
luab_buf_alloc(luab_buf_t *buf, size_t len)
{
    int status;
    caddr_t bp;

    if (buf != NULL && len > 0) {
        if (buf->buf_data != NULL)
            bp = realloc(buf->buf_data, len);
        else
            bp = malloc(len);

        if (bp != NULL) {
            (void)memset_s(bp, len, 0, len);

            buf->buf_data = bp;
            buf->buf_len = len;

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
luab_buf_copy_in(luab_buf_t *buf, caddr_t data, size_t len)
{
    int status;

    if (buf != NULL && data != NULL && len > 0) {
        if ((buf->buf_flags & IOV_LOCK) == 0) {
            buf->buf_flags |= IOV_LOCK;

            if ((buf->buf_data != NULL) &&
                (len <= buf->buf_len)) {
                (void)memmove(buf->buf_data, data, len);
#if 0
                luab_buf_crc32(buf);
#endif
                status = 0;
            } else {
                errno = EINVAL;
                status = -1;
            }
            buf->buf_flags &= ~IOV_LOCK;
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

int
luab_buf_copy_out(luab_buf_t *buf, caddr_t data, size_t len)
{
    int status;

    if (buf != NULL && data != NULL && len > 0) {
        if ((buf->buf_flags & IOV_LOCK) == 0) {
            buf->buf_flags |= IOV_LOCK;

            if ((buf->buf_data != NULL) &&
                (len < buf->buf_len)) {
                (void)memmove(data, buf->buf_data, len);

                status = 0;
            } else {
                errno = EINVAL;
                status = -1;
            }
            buf->buf_flags &= ~IOV_LOCK;
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

int
luab_buf_free(luab_buf_t *buf)
{
    int status;

    if (buf != NULL && buf->buf_data != NULL) {
        if ((buf->buf_flags & IOV_LOCK) == 0) {
            buf->buf_flags |= IOV_LOCK;

            free(buf->buf_data);
            buf->buf_data = NULL;
            buf->buf_len = 0;

            status = 0;

            buf->buf_flags &= ~IOV_LOCK;
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
