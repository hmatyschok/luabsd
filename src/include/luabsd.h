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

#ifndef _LUAB_H_
#define _LUAB_H_

#include <sys/param.h>
#include <sys/mbuf.h>   /* XXX */
#include <sys/socket.h>
#include <sys/queue.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

/*
 * Definitions for API method table.
 */

typedef union primitive {
    char        un_char;
    short       un_short;
    int         un_int;
    long        un_long;
    int8_t      un_int8;
    int16_t     un_int16;
    int32_t     un_int32;
    int64_t     un_int64;
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
} luab_primitive_u;

typedef void    (*luab_module_table_fn)(lua_State *, luab_primitive_u *);

typedef struct luab_module_table {
    luab_module_table_fn    mt_init;
    const char              *mt_key;
    luab_primitive_u        mt_val;
} luab_module_table_t;

#define LUAB_TYPE(fn, k, v) \
    { .mt_init = fn, .mt_key = k, v, }
#define LUAB_INT(k, v) \
    LUAB_TYPE(luab_initinteger, k, .mt_val.un_int = v)
#define LUAB_FUNC(k, v) \
    LUAB_TYPE(luab_initcfunction, k, .mt_val.un_fn = v)
#define LUAB_STR(k, v) \
    LUAB_TYPE(luab_initstring, k, .mt_val.un_cp = v)
#define LUAB_MOD_TBL_SENTINEL \
    LUAB_TYPE(NULL, NULL, .mt_val.un_fn = NULL)

static __inline void
luab_initinteger(lua_State *L, luab_primitive_u *un)
{
    lua_pushinteger(L, un->un_int);
}

static __inline void
luab_initcfunction(lua_State *L, luab_primitive_u *un)
{
    lua_pushcfunction(L, un->un_fn);
}

static __inline void
luab_initstring(lua_State *L, luab_primitive_u *un)
{
    lua_pushstring(L, un->un_cp);
}

/*
 * Definitions for luab_module{} descriptor.
 */

typedef void *  (*luab_ctor_fn)(lua_State *, void *);
typedef void  (*luab_init_fn)(void *, void *);
typedef void *  (*luab_get_fn)(lua_State *, int);

typedef struct luab_module {
    uint32_t        m_cookie;        /*  date -u +'%s' */
    size_t          m_sz;
    const char      *m_name;
    luab_module_table_t *m_vec;
    luab_ctor_fn    m_create;
    luab_init_fn    m_init;
    luab_get_fn    m_get;
} luab_module_t;

typedef void    (*luab_module_fn)(lua_State *, int, luab_module_t *);

typedef struct luab_module_vec {
    luab_module_t   *mv_mod;
    luab_module_fn  mv_init;
    int             mv_idx;
} luab_module_vec_t;

#define LUAB_MOD_VEC_SENTINEL \
    { .mv_mod = NULL, .mv_init = NULL, .mv_idx = -1, }

/*
 * Access functions, n-th arg over argv, [stack -> C].
 */

lua_Integer  luab_checkinteger(lua_State *, int, lua_Integer);
lua_Integer  luab_tointeger(lua_State *, int, lua_Integer);

const char   *luab_islstring(lua_State *, int, size_t);
const char   *luab_tolstring(lua_State *, int, size_t);
const char   *luab_checklstring(lua_State *, int, size_t);

/*
 * Access functions, [C -> stack].
 */

void     luab_rawsetinteger(lua_State *, int, lua_Integer, lua_Integer);
void     luab_rawsetnumber(lua_State *, int, lua_Integer, lua_Number);
void     luab_rawsetstring(lua_State *, int, lua_Integer, const char *);
void     luab_rawsetfstring(lua_State *, int, lua_Integer, const char *, ...);
void     luab_rawsetldata(lua_State *, int, lua_Integer, void *, size_t);

void     luab_setcfunction(lua_State *, int, const char *, lua_CFunction);
void     luab_setinteger(lua_State *, int, const char *, lua_Integer);
void     luab_setstring(lua_State *, int, const char *, const char *);

void     luab_setfstring(lua_State *, int, const char *, const char *, ...);
void     luab_setldata(lua_State *, int, const char *, void *, size_t);

int  luab_pusherr(lua_State *, lua_Integer);
int  luab_pushnumber(lua_State *, lua_Number);
int  luab_pushnil(lua_State *);
int  luab_pushstring(lua_State *, const char *);
int  luab_pushfstring(lua_State *, const char *, ...);
int  luab_pushldata(lua_State *, void *, size_t);

int  luab_iov_pushlen(lua_State *, struct iovec *);
int  luab_iov_pushdata(lua_State *, struct iovec *);

/*
 * Generic service primitives, subset of <core>.
 */

void     luab_argerror(lua_State *, int, void *, size_t, size_t, int);
int  luab_checkmaxargs(lua_State *, int);

int  luab_create(lua_State *, int, luab_module_t *, luab_module_t *);
int  luab_dump(lua_State *, int, luab_module_t *, size_t);
int  luab_gc(lua_State *, int, luab_module_t *);
int  luab_len(lua_State *, int, luab_module_t *);
int  luab_tostring(lua_State *, int, luab_module_t *);
#endif /* _LUAB_H_ */
