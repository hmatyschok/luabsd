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

#ifndef _LUAB_TYPES_H_
#define _LUAB_TYPES_H_

#include "luab_buf.h"

/*
 * Interface Control Information (ICI).
 */

typedef struct luab_udata {
    LIST_ENTRY(luab_udata)  ud_next;
    LIST_HEAD(, luab_udata) ud_list;
    luab_module_t           *ud_m;
    time_t                  ud_ts;
    void                    **ud_x;
    void                    *ud_xhd;
} luab_udata_t;

static __inline size_t
luab_xlen(luab_module_t *m)
{
    return ((m->m_sz - sizeof(luab_udata_t)));
}

/*
 * Protocol Control Information (PCI).
 */

typedef struct luab_xarg {
    luab_type_t xarg_idx;
    size_t      xarg_len;
} luab_xarg_t;

/*
 * Definitions for (LUA_TUSERDATA(IOVEC)).
 */

typedef struct luab_iovec_param {
    struct iovec    iop_iov;    /* maps-to allocated memory region, iov_base */
    struct iovec    iop_data;   /* maps-to supplied data */
    u_int   iop_flags;
} luab_iovec_param_t;

typedef struct luab_iovec {
    luab_udata_t    ud_softc;
    struct iovec    iov;
    size_t  iov_max_len;
    u_int   iov_flags;
} luab_iovec_t;
#if UINT_MAX > 65535
#define IOV_LOCK    0x00000001
#define IOV_PROXY   0x00000002
#define IOV_BUFF    0x00000004
#define IOV_DUMP    0x00000008
#else
#define IOV_LOCK    0x0001
#define IOV_PROXY   0x0002
#define IOV_BUFF    0x0004
#define IOV_DUMP    0x0008
#endif

/*
 * Generic service primitives.
 */

luab_module_t    *luab_iovec_param_init(luab_iovec_param_t *, void *, size_t,
    size_t);

void     *luab_newudata(lua_State *, luab_module_t *, void *);
void     luab_udata_init(luab_module_t *, luab_udata_t *, void *);
void     luab_udata_remove(luab_udata_t *);
luab_udata_t     *luab_udata_find(luab_udata_t *, void **);
void     *luab_udata_insert(luab_udata_t *, luab_udata_t *, void **);

/*
 * Access functions, [stack -> C].
 */
#define luab_dptox(dp) ((void *)&(dp))
#define luab_isdata(L, narg, m, t) \
    ((t)luaL_testudata((L), (narg), ((m)->m_name)))

void     *luab_isudata(lua_State *, int, luab_module_t *);

#define luab_todata(L, narg, m, t) \
    ((t)luab_checkudata((L), (narg), (m)))
#define luab_toldata(L, narg, m, t, len) \
    ((t)luab_checkludata((L), (narg), (m), (len)))
#define luab_udata(L, narg, m, t) \
    ((t)((*(m)->m_get)((L), (narg))))
#define luab_udataisnil(L, narg, m, t) \
    ((t)(luab_checkudataisnil((L), (narg), (m))))

void     *luab_checkudata(lua_State *, int, luab_module_t *);
void     *luab_toudata(lua_State *, int, luab_module_t *);
void     *luab_checkudataisnil(lua_State *, int, luab_module_t *);
void     *luab_checkludata(lua_State *, int, luab_module_t *, size_t);

void     *luab_checkxdata(lua_State *, int, luab_module_t *, luab_udata_t **);
void     *luab_checkxdataisnil(lua_State *, int, luab_module_t *);

void     *luab_toxudata(lua_State *, int, luab_xarg_t *);
void     *luab_toxdata(lua_State *, int, luab_xarg_t *);

void     *luab_udata_xlink(lua_State *, int, luab_udata_t *, void **);
void     *luab_udata_checkxlink(lua_State *L, int narg, luab_module_t *m,
    luab_udata_t *udx, void **x);

#define luab_isiovec(L, narg) \
    (luab_isdata((L), (narg), luab_mx(IOVEC), luab_iovec_t *))

caddr_t  luab_iovec_toldata(lua_State *, int, size_t);
const char   *luab_iovec_islstring(lua_State *, int, size_t);
const char   *luab_iovec_checklstring(lua_State *, int, size_t);

/*
 * Access functions, [C -> stack].
 */

int  luab_pushudata(lua_State *, luab_module_t *, void *);
int  luab_iovec_pushudata(lua_State *, void *, size_t, size_t);

void     luab_rawsetudata(lua_State *, int, luab_module_t *, lua_Integer, void *);
void     luab_iovec_rawsetldata(lua_State *, int, lua_Integer, void *, size_t);

void     luab_setudata(lua_State *, int, luab_module_t *, const char *, void *);
void     luab_iovec_setldata(lua_State *, int, const char *, void *, size_t);

/*
 * Service primitives.
 */

int  luab_iovec_copyin(luab_iovec_t *, const void *, size_t);
int  luab_iovec_copyout(luab_iovec_t *, void *, size_t);

int  luab_iovec_read(lua_State *, int, luab_iovec_t *, size_t *);
int  luab_iovec_readv(lua_State *, int, luab_iovec_t *, size_t);
int  luab_iovec_write(lua_State *, int, luab_iovec_t *, size_t *);
int  luab_iovec_writev(lua_State *, int, luab_iovec_t *, size_t);

/* 1003.1-2001 */
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
int  luab_iovec_readlink(lua_State *, const char *, luab_iovec_t *, size_t *);
#endif
/* 1003.1-2008 */
#if __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE
int  luab_iovec_pread(lua_State *, int, luab_iovec_t *, size_t *, off_t);
int  luab_iovec_pwrite(lua_State *, int, luab_iovec_t *, size_t *, off_t);
#endif
#if __POSIX_VISIBLE >= 200809
int  luab_iovec_readlinkat(lua_State *, int, const char *,
    luab_iovec_t *, size_t *);
#endif
int  luab_iovec_recv(lua_State *, int, luab_iovec_t *, size_t *, int);
int  luab_iovec_recvfrom(lua_State *, int , luab_iovec_t *,
    size_t *, int, struct sockaddr *, socklen_t *);
int  luab_iovec_send(lua_State *, int, luab_iovec_t *, size_t *, int);
int  luab_iovec_sendto(lua_State *, int, luab_iovec_t *, size_t *,
        int, struct sockaddr *, socklen_t);
#if __BSD_VISIBLE
int  luab_iovec_preadv(lua_State *, int, luab_iovec_t *, size_t, off_t);
int  luab_iovec_pwritev(lua_State *, int, luab_iovec_t *, size_t, off_t);
#endif
#endif /* _LUAB_TYPES_H_ */
