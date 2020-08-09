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

#include <string.h>     /* XXX */

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
typedef void *  (*luab_udata_fn)(lua_State *, int);

typedef struct luab_module {
    u_int32_t  cookie;        /*  date -u +'%s' */
    size_t  sz;
    const char  *name;
    luab_table_t    *vec;
    luab_ctor_fn    ctor;
    luab_init_fn    init;
    luab_udata_fn    get;
} luab_module_t;

typedef struct luab_buf {
    size_t  buf_len;
    caddr_t buf_data;
} luab_buf_t;

typedef struct luab_iovec_param {
    luab_buf_t  iop_buf;    /* allocated memory, iov_base */
    luab_buf_t  iop_data;   /* data */
    u_int   iop_flags;
} luab_iovec_param_t;

typedef struct luab_iovec {
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

int luab_pusherr(lua_State *, lua_Integer);
int luab_pushnil(lua_State *);
int luab_pushstring(lua_State *, const char *);
void    luab_pushtimesvector(lua_State *, int, size_t, void *);

const char **    luab_checkargv(lua_State *, int);
int *   luab_checkintvector(lua_State *, int, size_t);
const char *    luab_checklstring(lua_State *, int, size_t);
int luab_checkmaxargs(lua_State *, int);
struct timespec *   luab_checktimesvector(lua_State *, int, size_t);

void *  luab_newuserdata(lua_State *, luab_module_t *, void *);
void *  luab_newvector(lua_State *, int, size_t, size_t);

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

/*
 * Accessor for table manipulation.
 */

static __inline void
luab_rawsetinteger(lua_State *L, int narg, lua_Integer k, lua_Integer v)
{
    lua_pushinteger(L, v);
    lua_rawseti(L, narg, k);
}

static __inline void
luab_rawsetudata(lua_State *L, int narg, luab_module_t *m, lua_Integer k, void *v)
{
    if ((*m->ctor)(L, v) != NULL)
        lua_rawseti(L, narg, k);
}

static __inline void
luab_setbuff(lua_State *L, int narg, const char *k, void *v, size_t len)
{
    luab_iovec_param_t iop;
    luab_iovec_t *buf;

    if (len > 0) {
        (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

        iop.iop_buf.buf_len = len + sizeof(uint32_t);
        iop.iop_data.buf_len = len;
        iop.iop_data.buf_data = v;

        if ((buf = (*iovec_type.ctor)(L, &iop)) != NULL)
            lua_setfield(L, narg, k);
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
luab_setudata(lua_State *L, int narg, luab_module_t *m, const char *k, void *v)
{
    if ((*m->ctor)(L, v) != NULL)
        lua_setfield(L, narg, k);
}

/*
 * Accessor for evaluation of function arguments.
 */

static __inline lua_Integer
luab_checkinteger(lua_State *L, int narg, lua_Integer b_msk)
{
    return ((luaL_checkinteger(L, narg)) & (b_msk));
}

static __inline void
luab_checktable(lua_State *L, int narg)
{
    if (lua_istable(L, narg) == 0)
        luaL_argerror(L, narg, "Table expected");
}

static __inline size_t
luab_checkltable(lua_State *L, int narg, size_t len)
{
    size_t n;

    luab_checktable(L, narg);

    if ((n = lua_rawlen(L, narg)) != len)
        luaL_argerror(L, narg, "Size mismatch");

    return (n);
}

static __inline lua_Integer
luab_tointeger(lua_State *L, int narg, lua_Integer b_msk)
{
    return ((lua_tointeger(L, narg)) & (b_msk));
}

/*
 * Accessor for evaluation of LUA_TUSERDATA.
 */

#define luab_isdata(L, narg, id, t) \
    ((t)luaL_testudata((L), (narg), (id)))
#define luab_isiovec(L, narg) \
    (luab_isdata((L), (narg), iovec_type.name, luab_iovec_t *))
#define luab_todata(L, narg, id, t) \
    ((t)luab_checkudata((L), (narg), (id)))
#define luab_toldata(L, narg, id, t, len) \
    ((t)luab_checkludata((L), (narg), (id), (len)))

static __inline void *
luab_checkudata(lua_State *L, int narg, luab_module_t *m)
{
    return (luaL_checkudata(L, narg, m->name));
}

static __inline void *
luab_checkludata(lua_State *L, int narg, luab_module_t *m, size_t len)
{
    luab_iovec_t *buf;

    if ((buf = luab_isiovec(L, narg)) != NULL) {
        if (buf->iov.iov_base == NULL)
            luaL_argerror(L, narg, "Invalid argument.");

        if (buf->iov.iov_len != len)
            luaL_argerror(L, narg, "Invalid argument.");

        return (buf->iov.iov_base);
    }
    return (luab_checkudata(L, narg, m));
}

static __inline void *
luab_checkudataisnil(lua_State *L, int narg, luab_module_t *m)
{
    if (lua_isnil(L, narg) != 0)
        return (NULL);

    return ((*m->get)(L, narg));
}
__END_DECLS

#endif /* _LUABSD_H_ */
