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

__BEGIN_DECLS
typedef union luab_type {
    char        un_char;
    short       un_short;
    int         un_int;
    long        un_long;
    uint8_t     un_uint8;
    uint16_t    un_uint16;
    uint32_t    un_uint32;
    uint64_t    un_uint64;
    socklen_t   un_socklen;
    lua_Integer un_intx;
    lua_Number  un_numx;
    lua_CFunction   un_fn;
    const char  *un_cp;
} luab_type_u;

typedef void    (*luab_table_fn)(lua_State *, luab_type_u *);

typedef struct luab_table {
    luab_table_fn   init;
    const char    *key;
    luab_type_u   val;
} luab_table_t;

#define LUABSD_REG(fn, k, v) \
    { .init = fn, .key = k, v }
#define LUABSD_INT(k, v) \
    LUABSD_REG(luab_initinteger, k, .val.un_int = v)
#define LUABSD_FUNC(k, v) \
    LUABSD_REG(luab_initcfunction, k, .val.un_fn = v)
#define LUABSD_STR(k, v) \
    LUABSD_REG(luab_initstring, k, .val.un_cp = v)

typedef void *  (*luab_ctor_fn)(lua_State *, void *);
typedef void  (*luab_init_fn)(void *, void *);
typedef void  (*luab_fini_fn)(void *);
typedef void *  (*luab_get_fn)(lua_State *, int);

typedef struct luab_module {
    u_int32_t  cookie;        /*  date -u +'%s' */
    ssize_t  sz;
    const char  *name;
    luab_table_t    *vec;
    luab_ctor_fn    create;
    luab_init_fn    init;
    luab_fini_fn    fini;
    luab_get_fn    get;
} luab_module_t;

typedef struct luab_udata {
    TAILQ_ENTRY(luab_udata)     ud_list;
    TAILQ_HEAD(, luab_udata)    ud_hooks;
    luab_module_t   *ud_m;
} luab_udata_t;

typedef struct luab_buf {
    size_t      buf_len;
    caddr_t     buf_data;
    u_int       buf_flags;
} luab_buf_t;

typedef struct luab_iovec_param {
    luab_buf_t  iop_buf;    /* addr. of allocated memory region, iov_base */
    luab_buf_t  iop_data;   /* supplied data */
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
#else
#define IOV_LOCK    0x0001
#define IOV_PROXY   0x0002
#define IOV_BUFF    0x0004
#endif

extern luab_module_t iovec_type;

#define SDL_DATA_MAX_LEN    46     /* XXX */
#define SDL_ADDR_MAX_LEN    (SDL_DATA_MAX_LEN - IFNAMSIZ)

int luab_buf_clear(luab_buf_t *);
int luab_buf_alloc(luab_buf_t *, size_t);
int luab_buf_copy_in(luab_buf_t *, caddr_t, size_t);
int luab_buf_copy_out(luab_buf_t *, caddr_t, size_t);
int luab_buf_free(luab_buf_t *);

/*
 * Operations on stack.
 */

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

int luab_pusherr(lua_State *, lua_Integer);
int luab_pushnil(lua_State *);
int luab_pushstring(lua_State *, const char *);
int luab_pushldata(lua_State *, void *, size_t);

void *  luab_newuserdata(lua_State *, luab_module_t *, void *);
int luab_pushudata(lua_State *, luab_module_t *, void *);
int luab_pushiovec(lua_State *, void *, size_t, size_t);

/*
 * Operations on LUA_TTABLE.
 */

void    luab_rawsetinteger(lua_State *, int, lua_Integer, lua_Integer );
void    luab_rawsetstring(lua_State *, int, lua_Integer, const char *);
void    luab_rawsetldata(lua_State *, int, lua_Integer, void *, size_t);

void    luab_rawsetudata(lua_State *, int, luab_module_t *, lua_Integer, void *);
void    luab_rawsetiovec(lua_State *, int, lua_Integer, void *, size_t);

void    luab_setcfunction(lua_State *, int, const char *, lua_CFunction);
void    luab_setinteger(lua_State *, int, const char *, lua_Integer);
void    luab_setstring(lua_State *, int, const char *, const char *);
void    luab_setldata(lua_State *, int, const char *, void *, size_t);

void    luab_setudata(lua_State *, int, luab_module_t *, const char *, void *);
void    luab_setiovec(lua_State *, int, const char *, void *, size_t);

/*
 * Each kind of luab_check{l}xxx(3) accessor evaluates, if n-th arg exists,
 * otherwise lua_error will be thrown. Finally luab_{is,to}{l}xxx(3) does
 * the same thing without throwing an error.
 */

const char *    luab_islstring(lua_State *, int, size_t);
const char *    luab_checklstring(lua_State *, int, size_t);

lua_Integer luab_tointeger(lua_State *, int, lua_Integer);
lua_Integer luab_checkinteger(lua_State *, int, lua_Integer);

/*
 * Accessor evaluates argv.
 */

int luab_checkmaxargs(lua_State *, int);
int luab_checktable(lua_State *, int);

size_t  luab_checkltable(lua_State *, int, size_t);
const char **    luab_checkargv(lua_State *, int);

void *  luab_newvector(lua_State *, int, size_t);
void *  luab_newlvector(lua_State *, int, size_t, size_t);
int *   luab_checklintvector(lua_State *, int, size_t);

/*
 * Accessor evaluates LUA_TUSERDATA.
 */

#define luab_isdata(L, narg, m, t) \
    ((t)luaL_testudata((L), (narg), (m)))
#define luab_isiovec(L, narg) \
    (luab_isdata((L), (narg), iovec_type.name, luab_iovec_t *))
#define luab_todata(L, narg, m, t) \
    ((t)luab_checkudata((L), (narg), (m)))
#define luab_toldata(L, narg, m, t, len) \
    ((t)luab_checkludata((L), (narg), (m), (len)))
#define luab_udata(L, narg, m, t) \
    ((t)(*(m).get)((L), (narg)))
#define luab_udataisnil(L, narg, m, t) \
    ((t)(luab_checkudataisnil((L), (narg), &(m))))

void *  luab_checkudata(lua_State *, int, luab_module_t *);
void *  luab_toudata(lua_State *, int, luab_module_t *);
void *  luab_checkludata(lua_State *, int, luab_module_t *, size_t);
void *  luab_checkudataisnil(lua_State *, int, luab_module_t *);

const char *    luab_iovec_islxarg(lua_State *, int, size_t);
const char *    luab_iovec_checklxarg(lua_State *, int, size_t);

int luab_iovec_read(lua_State *, int, luab_iovec_t *, size_t *);
int luab_iovec_write(lua_State *, int, luab_iovec_t *, size_t *);
/* 1003.1-2001 */
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
int luab_iovec_readlink(lua_State *, const char *, luab_iovec_t *, size_t *);
#endif
#if __POSIX_VISIBLE >= 200809
int luab_iovec_readlinkat(lua_State *, int, const char *,
    luab_iovec_t *, size_t *);
#endif

int luab_iovec_recv(lua_State *, int, luab_iovec_t *, size_t *, int);
int luab_iovec_send(lua_State *, int, luab_iovec_t *, size_t *, int);

/*
 * Service primitives subset of <core>.
 */

int luab_dump(lua_State *, int, luab_module_t *, size_t);
int luab_gc(lua_State *, int, luab_module_t *);
int luab_tostring(lua_State *, int, luab_module_t *);
__END_DECLS

#endif /* _LUABSD_H_ */
