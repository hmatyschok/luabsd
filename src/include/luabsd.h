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

#ifndef _LUABSD_H_
#define _LUABSD_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/queue.h>

#include <stdlib.h>
#include <string.h>

/*
 * Definitions for API method table.
 */

__BEGIN_DECLS
typedef union luab_type {   /* XXX namespace */
    char        un_char;
    short       un_short;
    int         un_int;
    long        un_long;
    uint8_t     un_uint8;
    uint16_t    un_uint16;
    uint32_t    un_uint32;
    uint64_t    un_uint64;
    u_int       un_u_int;
    u_long      un_u_long;
    socklen_t   un_socklen;
    lua_Integer un_intx;
    lua_Number  un_numx;
    lua_CFunction   un_fn;
    const char  *un_cp;
    wchar_t     un_wc;
} luab_type_u;

typedef void    (*luab_table_fn)(lua_State *, luab_type_u *);
typedef void    (*luab_link_fn)(lua_State *, void *, void *);

typedef struct luab_table {
    luab_table_fn   init;
    const char    *key;
    luab_type_u   val;
    luab_link_fn    link;
} luab_table_t;

#define LUABSD_REG(fn, k, v) \
    { .init = fn, .key = k, v }
#define LUABSD_INT(k, v) \
    LUABSD_REG(luab_initinteger, k, .val.un_int = v)
#define LUABSD_FUNC(k, v) \
    LUABSD_REG(luab_initcfunction, k, .val.un_fn = v)
#define LUABSD_STR(k, v) \
    LUABSD_REG(luab_initstring, k, .val.un_cp = v)

static __inline void
luab_initinteger(lua_State *L, luab_type_u *un)
{
    lua_pushinteger(L, un->un_int);
}

static __inline void
luab_initcfunction(lua_State *L, luab_type_u *un)
{
    lua_pushcfunction(L, un->un_fn);
}

static __inline void
luab_initstring(lua_State *L, luab_type_u *un)
{
    lua_pushstring(L, un->un_cp);
}

/*
 * Definitions for module/udata descriptor.
 */

typedef void *  (*luab_ctor_fn)(lua_State *, void *);
typedef void  (*luab_init_fn)(void *, void *);
typedef void *  (*luab_get_fn)(lua_State *, int);

typedef struct luab_module {
    uint32_t    cookie;        /*  date -u +'%s' */
    size_t  sz;
    const char  *name;
    luab_table_t    *vec;
    luab_ctor_fn    create;
    luab_init_fn    init;
    luab_get_fn    get;
} luab_module_t;

typedef void    (*luab_module_fn)(lua_State *, int, luab_module_t *);

typedef struct luab_modulevec {
    luab_module_t   *mv_mod;
    luab_module_fn  mv_init;
    int             mv_idx;
} luab_modulevec_t;

/*
 * Interface Control Information (ICI) for (LUA_TUSERDATA(XXX)).
 */

typedef struct luab_udata {
    LIST_ENTRY(luab_udata)     ud_next;
    LIST_HEAD(, luab_udata)    ud_list;
    luab_module_t   *ud_m;
    time_t          ud_ts;
} luab_udata_t;

/*
 * Dfinitions for (LUA_TUSERDATA(IOVEC)).
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

extern luab_module_t iovec_type;

/*
 * Internal API for manipulating iovec{}s.
 */

int luab_iov_clear(struct iovec *);
int luab_iov_free(struct iovec *);

int luab_iov_alloc(struct iovec *, size_t);
int luab_iov_realloc(struct iovec *, size_t);

int luab_iov_copyin(struct iovec *, const void *, ssize_t);
int luab_iov_copyout(struct iovec *, void *, ssize_t);

ssize_t luab_iov_readv(struct iovec *, int, size_t);
ssize_t luab_iov_writev(struct iovec *, int, size_t);
#if __BSD_VISIBLE
ssize_t luab_iov_preadv(struct iovec *, int, size_t, off_t);
ssize_t luab_iov_pwritev(struct iovec *, int, size_t, off_t);
#endif

/*
 * Generic error handler.
 */

static __inline void
luab_argerror(lua_State *L, int narg, void *v, size_t n, size_t sz)
{
    size_t len;

    if ((v != NULL) &&
        ((len = n * sz) != 0)) {
        (void)memset_s(v, len, 0, len);
        free(v);
    }
    luaL_argerror(L, narg, "Invalid argument");
}

/*
 * Generator functions, [Lua -> stack].
 */

void *  luab_newudata(lua_State *, luab_module_t *, void *);

/*
 * Accessor, n-th arg over argv, [stack -> C].
 *
 * luab_check{l}xxx(3) accessor evaluates, if n-th arg exists, otherwise
 * lua_error will be thrown. Finally luab_{is,to}{l}xxx(3) does the same
 * thing without throwing an error, but return NULL, if n-th arg does
 * not exist.
 */

int luab_checkmaxargs(lua_State *, int);

/* Atomic data types. */
static __inline lua_Integer
luab_checkinteger(lua_State *L, int narg, lua_Integer b_msk)
{
    return ((luaL_checkinteger(L, narg)) & (b_msk));
}

static __inline lua_Integer
luab_tointeger(lua_State *L, int narg, lua_Integer b_msk)
{
    return ((lua_tointeger(L, narg)) & (b_msk));
}

static __inline const char *
luab_islstring(lua_State *L, int narg, size_t len)
{
    const char *dp;
    size_t n;

    if ((dp = luaL_tolstring(L, narg, &n)) != NULL) {
        if (n <= len)
            return (dp);
    }
    return (NULL);
}

static __inline const char *
luab_tolstring(lua_State *L, int narg, size_t len)
{
    const char *dp;
    size_t n;

    if ((dp = luaL_tolstring(L, narg, &n)) != NULL) {
        if (n == len)
            return (dp);
    }
    return (NULL);
}

static __inline const char *
luab_checklstring(lua_State *L, int narg, size_t max_len)
{
    const char *dp;
    size_t len;

    dp = luaL_checklstring(L, narg, &len);

    if (len > max_len)    /* XXX err_msg */
        luaL_argerror(L, narg, "Value too large to be stored in data type");

    return (dp);
}

/* (LUA_TUSERDATA(XXX)). */
#define luab_isdata(L, narg, m, t) \
    ((t)luaL_testudata((L), (narg), ((m)->name)))
#define luab_todata(L, narg, m, t) \
    ((t)luab_checkudata((L), (narg), (m)))
#define luab_toldata(L, narg, m, t, len) \
    ((t)luab_checkludata((L), (narg), (m), (len)))
#define luab_udata(L, narg, m, t) \
    ((t)((*(m)->get)((L), (narg))))
#define luab_udataisnil(L, narg, m, t) \
    ((t)(luab_checkudataisnil((L), (narg), (m))))

static __inline void *
luab_checkudata(lua_State *L, int narg, luab_module_t *m)
{
    if (m == NULL)
        luaL_argerror(L, narg, "Invalid argument");

    return (luaL_checkudata(L, narg, m->name));
}

static __inline void *
luab_isudata(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *ud = luab_isdata(L, narg, m, luab_udata_t *);

    if (ud == NULL)
        return (NULL);

    return (ud + 1);
}

static __inline void *
luab_toudata(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *ud = luab_todata(L, narg, m, luab_udata_t *);

    return (ud + 1);
}

static __inline void *
luab_checkudataisnil(lua_State *L, int narg, luab_module_t *m)
{
    if (lua_isnil(L, narg) != 0)
        return (NULL);

    if (m != NULL && m->get != NULL)
        return ((*m->get)(L, narg));

    return (NULL);
}

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
 * Accessor, (LUA_TTABLE), [stack -> C].
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

/* Allocate an generic C-pointer array by its cardinality from given table. */
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
 * Accessor, [C -> stack].
 */

static __inline void
luab_rawsetinteger(lua_State *L, int narg, lua_Integer k, lua_Integer v)
{
    lua_pushinteger(L, v);
    lua_rawseti(L, narg, k);
}

static __inline void
luab_rawsetnumber(lua_State *L, int narg, lua_Integer k, lua_Number v)
{
    lua_pushnumber(L, v);
    lua_rawseti(L, narg, k);
}

static __inline void
luab_rawsetstring(lua_State *L, int narg, lua_Integer k, const char *v)
{
    lua_pushstring(L, v);
    lua_rawseti(L, narg, k);
}

static __inline void
luab_rawsetldata(lua_State *L, int narg, lua_Integer k, void *v, size_t len)
{
    luaL_Buffer b;
    caddr_t dp;

    if (v != NULL && len > 1) {
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        lua_rawseti(L, narg, k);
    }
}

static __inline void
luab_setcfunction(lua_State *L, int narg, const char* k, lua_CFunction v)
{
    lua_pushcfunction(L, v);
    lua_setfield(L, narg, k);
}

static __inline void
luab_setinteger(lua_State *L, int narg, const char *k, lua_Integer v)
{
    lua_pushinteger(L, v);
    lua_setfield(L, narg, k);
}

static __inline void
luab_setstring(lua_State *L, int narg, const char *k, const char *v)
{
    lua_pushstring(L, v);
    lua_setfield(L, narg, k);
}

static __inline void
luab_setldata(lua_State *L, int narg, const char *k, void *v, size_t len)
{
    luaL_Buffer b;
    caddr_t dp;

    if (v != NULL && len > 1) {
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        lua_setfield(L, narg, k);
    }
}

/*
 * Accessor, [C -> stack].
 */

int luab_pusherr(lua_State *, lua_Integer);
int luab_pushnumber(lua_State *, lua_Number);
int luab_pushnil(lua_State *);
int luab_pushstring(lua_State *, const char *);
int luab_pushfstring(lua_State *, const char *, ...);
int luab_pushldata(lua_State *, void *, size_t);

int luab_iov_pushlen(lua_State *, struct iovec *);
int luab_iov_pushdata(lua_State *, struct iovec *);

int luab_pushudata(lua_State *, luab_module_t *, void *);
int luab_iovec_pushudata(lua_State *, void *, size_t, size_t);

void *  luab_checkludata(lua_State *, int, luab_module_t *, size_t);
void *  luab_addudata(lua_State *, int, luab_module_t *, int);

/* (LUA_TUSERDATA(IOVEC)) */
#define luab_isiovec(L, narg) \
    (luab_isdata((L), (narg), luab_mx(IOVEC), luab_iovec_t *))

const char *    luab_iovec_islxarg(lua_State *, int, size_t);
const char *    luab_iovec_checklxarg(lua_State *, int, size_t);

const char **    luab_checkargv(lua_State *, int);
double *    luab_table_checkdouble(lua_State *, int, size_t *);
const void ** luab_table_tolxargp(lua_State *, int, size_t);
u_short *   luab_table_checklu_short(lua_State *, int, size_t);
int *   luab_table_checklint(lua_State *, int, size_t);
gid_t * luab_table_checklgid(lua_State *, int, size_t);

void    luab_rawsetudata(lua_State *, int, luab_module_t *, lua_Integer, void *);
void    luab_iovec_rawsetldata(lua_State *, int, lua_Integer, void *, size_t);

void    luab_setcfunction(lua_State *, int, const char *, lua_CFunction);
void    luab_setinteger(lua_State *, int, const char *, lua_Integer);
void    luab_setstring(lua_State *, int, const char *, const char *);
void    luab_setldata(lua_State *, int, const char *, void *, size_t);

void    luab_setudata(lua_State *, int, luab_module_t *, const char *, void *);
void    luab_iovec_setldata(lua_State *, int, const char *, void *, size_t);

void    luab_table_pushdouble(lua_State *, int, double *, int);
void    luab_table_pushint(lua_State *, int, int *, int);
void    luab_table_pushldouble(lua_State *, int, double *, size_t, int);
void    luab_table_pushlgidset(lua_State *, int, gid_t *, int, int);

/*
 * Generic service primitives, subset of <core>.
 */

int luab_create(lua_State *, int, luab_module_t *, luab_module_t *);
int luab_dump(lua_State *, int, luab_module_t *, size_t);
int luab_gc(lua_State *, int, luab_module_t *);
int luab_len(lua_State *, int, luab_module_t *);
int luab_tostring(lua_State *, int, luab_module_t *);

/*
 * Generic service primitves, complex data types.
 */

static __inline void *
luab_udata_insert(luab_udata_t *self, luab_udata_t *ud)
{
    LIST_INSERT_HEAD(&self->ud_list, ud, ud_next);

    return (ud + 1);
}

static __inline void
luab_udata_remove(luab_udata_t *self)
{
    LIST_REMOVE(self, ud_next);
}

static __inline void
luab_udata_clear(luab_udata_t *self)
{
    luab_udata_t *ud, *ud_temp;

    ud = LIST_FIRST(&self->ud_list);

    while (ud != NULL) {
        ud_temp = LIST_NEXT(ud, ud_next);
        LIST_REMOVE(ud, ud_next);
        ud = ud_temp;
    }
    LIST_INIT(&self->ud_list);
}

/* (LUA_TUSERDATA(IOVEC)) */
int luab_iovec_copyin(luab_iovec_t *, const void *, size_t);
int luab_iovec_copyout(luab_iovec_t *, void *, size_t);

int luab_iovec_read(lua_State *, int, luab_iovec_t *, size_t *);
int luab_iovec_readv(lua_State *, int, luab_iovec_t *, size_t);
int luab_iovec_write(lua_State *, int, luab_iovec_t *, size_t *);
int luab_iovec_writev(lua_State *, int, luab_iovec_t *, size_t);

/* 1003.1-2001 */
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
int luab_iovec_readlink(lua_State *, const char *, luab_iovec_t *, size_t *);
#endif
/* 1003.1-2008 */
#if __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE
int luab_iovec_pread(lua_State *, int, luab_iovec_t *, size_t *, off_t);
int luab_iovec_pwrite(lua_State *, int, luab_iovec_t *, size_t *, off_t);
#endif
#if __POSIX_VISIBLE >= 200809
int luab_iovec_readlinkat(lua_State *, int, const char *,
    luab_iovec_t *, size_t *);
#endif
int luab_iovec_recv(lua_State *, int, luab_iovec_t *, size_t *, int);
int luab_iovec_recvfrom(lua_State *, int , luab_iovec_t *,
    size_t *, int, struct sockaddr *, socklen_t *);
int luab_iovec_send(lua_State *, int, luab_iovec_t *, size_t *, int);
int luab_iovec_sendto(lua_State *, int, luab_iovec_t *, size_t *,
        int, struct sockaddr *, socklen_t);
#if __BSD_VISIBLE
int luab_iovec_preadv(lua_State *, int, luab_iovec_t *, size_t, off_t);
int luab_iovec_pwritev(lua_State *, int, luab_iovec_t *, size_t, off_t);
#endif

/* (LUA_TUSERDATA(SOCKADDR)) */
#define LUAB_SOCK_MAXADDRLEN    SOCK_MAXADDRLEN
#define LUAB_SOCK_MINADDRLEN    2
#define LUAB_SDL_MAXDATALEN    46
#define LUAB_SDL_MAXADDRLEN    (LUAB_SDL_MAXDATALEN - IFNAMSIZ)
#define LUAB_SUN_MAXPATHLEN    103

static __inline void
luab_sockaddr_pci(struct sockaddr *sa, sa_family_t af, uint8_t len)
{
    sa->sa_len = len;
    sa->sa_family = af;
}

/* (LUA_TTABLE) */
#define luab_table_xlen(vec, type) \
    ((sizeof(vec)) / (sizeof(type)))

__END_DECLS
#endif /* _LUABSD_H_ */
