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

/*
 * Interface Control Information (ICI).
 */

typedef struct luab_udata {
    LIST_ENTRY(luab_udata)  ud_next;
    LIST_HEAD(, luab_udata) ud_list;
    luab_module_t           *ud_m;
    time_t                  ud_ts;
    caddr_t                 *ud_x;
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
    int         xarg_idx;
    size_t      xarg_len;
} luab_xarg_t;

#define LUAB_CLOCKINFO_IDX          0
#define LUAB_DIV_IDX                1
#define LUAB_FLOCK_IDX              2
#define LUAB_PRIMITIVE_IDX           3
#define LUAB_IF_NAMEINDEX_IDX       4
#define LUAB_IN_ADDR_IDX            5
#define LUAB_IN6_ADDR_IDX           6
#define LUAB_ITIMERVAL_IDX          7
#define LUAB_LDIV_IDX               8
#define LUAB_LLDIV_IDX              9
#define LUAB_LINGER_IDX             10
#define LUAB_MSGHDR_IDX             11
#define LUAB_SOCKADDR_IDX           12
#define LUAB_STAT_IDX               13
#define LUAB_TIMESPEC_IDX           14
#define LUAB_TIMEVAL_IDX            15
#define LUAB_TIMEZONE_IDX           16
#define LUAB_TM_IDX                 17
#define LUAB_UUID_IDX               18
#define LUAB_IOVEC_IDX              19
#define LUAB_LINK_IDX               20
#if __BSD_VISIBLE
#define LUAB_DBT_IDX                21
#define LUAB_DB_IDX                 22
#define LUAB_BINTIME_IDX            23
#define LUAB_CRYPT_DATA_IDX         24
#define LUAB_CAP_RBUF_IDX           25
#define LUAB_ACCEPT_FILTER_ARG_IDX  26
#define LUAB_SOCKPROTO_IDX          27
#define LUAB_CMSGCRED_IDX           28
#endif /* __BSD_VISIBLE */

/*
 * Selector.
 */

#define luab_idx(name) \
    (LUAB_##name##_IDX)
#define luab_vx(idx) \
    (luab_typevec[(idx)])
#define luab_mx(name) \
    ((luab_vx(luab_idx(name))).mv_mod)

extern luab_module_vec_t luab_typevec[];

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
 * Generator functions, (LUA_TTABLE).
 */

static __inline void *
luab_alloctable(lua_State *L, int narg, size_t n, size_t sz)
{
    void *vec;

    if (n == 0 && sz == 0)
        luaL_argerror(L, narg, "Invalid argument");

    if ((vec = calloc(n, sz)) == NULL)
        luaL_argerror(L, narg, "Cannot allocate memory");

    return (vec);
}

/*
 * Access functions, (LUA_TTABLE), [stack -> C].
 */

static __inline size_t
luab_checktable(lua_State *L, int narg)
{
    if (lua_istable(L, narg) == 0)
        luaL_argerror(L, narg, "Table expected");

    return (lua_rawlen(L, narg));
}

static __inline size_t
luab_checktableisnil(lua_State *L, int narg)
{
    if (lua_isnil(L, narg) != 0)
        return (0);

    return (luab_checktable(L, narg));
}

static __inline size_t
luab_checkltable(lua_State *L, int narg, size_t len)
{
    size_t n;

    if ((n = luab_checktable(L, narg)) != len)
        luaL_argerror(L, narg, "Size mismatch");

    return (len);
}

static __inline size_t
luab_checkltableisnil(lua_State *L, int narg, size_t len)
{
    if (lua_isnil(L, narg) != 0)
        return (0);

    return (luab_checkltable(L, narg, len));
}

/* Allocate an generic C-pointer array by cardinality of (LUA_TTABLE). */
static __inline void *
luab_newvector(lua_State *L, int narg, size_t *len, size_t sz)
{
    size_t n;

    if ((n = luab_checktable(L, narg)) == 0)
        luaL_argerror(L, narg, "Empty table");

    if (len != NULL)
        *len = n;

    return (luab_alloctable(L, narg, n, sz));
}

/* Same as above, but the cardinality is constrained by value argument len. */
static __inline void *
luab_newlvector(lua_State *L, int narg, size_t len, size_t sz)
{
    size_t n = luab_checkltable(L, narg, len);
    return (luab_alloctable(L, narg, n, sz));
}

/*
 * Generic service primitives.
 */

static __inline void
luab_udata_init(luab_module_t *m, luab_udata_t *ud, void *arg)
{
    if (m != NULL && ud != NULL && arg != NULL)
        (void)memmove(ud + 1, arg, luab_xlen(m));
}

static __inline void
luab_udata_remove(luab_udata_t *ud)
{
    if (ud != NULL) {
        if (ud->ud_x != NULL) {
            *(ud->ud_x) = NULL;
            ud->ud_x = NULL;
            ud->ud_xhd = NULL;
        }
        LIST_REMOVE(ud, ud_next);
    }
}

/*
 * Access functions, [stack -> C].
 */

void     *luab_newudata(lua_State *, luab_module_t *, void *);

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
void     *luab_toxudata(lua_State *, int, luab_xarg_t *);
void     *luab_toxdata(lua_State *, int, luab_xarg_t *);
void     *luab_checkludata(lua_State *, int, luab_module_t *, size_t);

void     *luab_udata_link(lua_State *, int, luab_module_t *, int, void **);

#define luab_isiovec(L, narg) \
    (luab_isdata((L), (narg), luab_mx(IOVEC), luab_iovec_t *))

const char   *luab_iovec_islxarg(lua_State *, int, size_t);
const char   *luab_iovec_checklxarg(lua_State *, int, size_t);

const char   **luab_checkargv(lua_State *, int);
double   *luab_module_table_checkdouble(lua_State *, int, size_t *);
const void   **luab_module_table_tolxargp(lua_State *, int, size_t);
u_short  *luab_module_table_checklu_short(lua_State *, int, size_t);
int  *luab_module_table_checklint(lua_State *, int, size_t);
gid_t    *luab_module_table_checklgid(lua_State *, int, size_t);

/*
 * Access functions, [C -> stack].
 */

int  luab_pushudata(lua_State *, luab_module_t *, void *);
int  luab_iovec_pushudata(lua_State *, void *, size_t, size_t);

void     luab_rawsetudata(lua_State *, int, luab_module_t *, lua_Integer, void *);
void     luab_iovec_rawsetldata(lua_State *, int, lua_Integer, void *, size_t);

void     luab_setudata(lua_State *, int, luab_module_t *, const char *, void *);
void     luab_iovec_setldata(lua_State *, int, const char *, void *, size_t);

void     luab_module_table_pushdouble(lua_State *, int, double *, int);
void     luab_module_table_pushint(lua_State *, int, int *, int);
void     luab_module_table_pushldouble(lua_State *, int, double *, size_t, int);
void     luab_module_table_pushlgidset(lua_State *, int, gid_t *, int, int);

/*
 * Service primitives.
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

/* (LUA_TTABLE) */
#define luab_module_table_xlen(vec, type) \
    ((sizeof(vec)) / (sizeof(type)))

#endif /* _LUAB_TYPES_H_ */
