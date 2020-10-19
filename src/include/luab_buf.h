/*-
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

#ifndef _LUAB_BUF_H_
#define _LUAB_BUF_H_

/*
 * Generic service primitives.
 */

int  luab_iov_clear(struct iovec *);
int  luab_iov_free(struct iovec *);

int  luab_iov_alloc(struct iovec *, size_t);
int  luab_iov_realloc(struct iovec *, size_t);

int  luab_iov_copyin(struct iovec *, const void *, ssize_t);
int  luab_iov_copyout(struct iovec *, void *, ssize_t);

ssize_t  luab_iov_readv(struct iovec *, int, size_t);
ssize_t  luab_iov_writev(struct iovec *, int, size_t);
#if __BSD_VISIBLE
ssize_t  luab_iov_preadv(struct iovec *, int, size_t, off_t);
ssize_t  luab_iov_pwritev(struct iovec *, int, size_t, off_t);
#endif

/*
 * Access functions, [C -> stack].
 */

int  luab_iov_pushlen(lua_State *, struct iovec *);
int  luab_iov_pushdata(lua_State *, struct iovec *);
int  luab_iov_pushxdata(lua_State *, struct iovec *);

void     luab_iov_rawsetdata(lua_State *, int, lua_Integer, struct iovec *);
void     luab_iov_rawsetxdata(lua_State *, int, lua_Integer, struct iovec *);

void     luab_iov_setdata(lua_State *, int, const char *, struct iovec *);
void     luab_iov_setxdata(lua_State *, int, const char *, struct iovec *);
#endif /* _LUAB_BUF_H_ */
