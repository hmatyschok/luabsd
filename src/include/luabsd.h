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
#include <sys/uio.h>

__BEGIN_DECLS

typedef union luab_type {
    lua_Integer type_int;
    lua_CFunction   type_fn;
} luab_type_un;

typedef void    (*luab_table_fn)(lua_State *, luab_type_un *);

typedef struct {
    luab_table_fn   init;
    const char    *key;
    luab_type_un   val;
} luab_table_t;

#define LUABSD_REG(fn, k, v) \
    { .init = fn, .key = k, v }
#define LUABSD_INT(k, v) \
    LUABSD_REG(luab_pushinteger, k, .val.type_int = v)
#define LUABSD_FUNC(k, v) \
    LUABSD_REG(luab_pushcfunction, k, .val.type_fn = v)

typedef void (*luab_init_fn)(void *, void *);
typedef void *  (*luab_udata_fn)(lua_State *, int);
typedef int (*luab_ctor_fn)(lua_State *);

typedef struct luab_module {
    u_int32_t  cookie;        /*  date -u +'%s' */
    size_t  sz;
    const char  *name;
    luab_table_t    *vec;
    luab_init_fn    init;
    luab_udata_fn    get;
    luab_ctor_fn    ctor;
} luab_module_t;

typedef struct luab_iovec {
    struct iovec    iov;
    size_t  iov_max_len;
} luab_iovec_t;

extern luab_module_t luab_iovec_type;

const char **    luab_checkargv(lua_State *, int);
int *   luab_checkintvector(lua_State *, int, size_t);
const char *    luab_checklstring(lua_State *, int, size_t);
int luab_checkmaxargs(lua_State *, int);
struct timespec *   luab_checktimesvector(lua_State *, int, size_t);

void *  luab_newuserdata(lua_State *, luab_module_t *, void *);
void *  luab_newvector(lua_State *, int, size_t, size_t);

int luab_pusherr(lua_State *, lua_Integer);
int luab_pushnil(lua_State *);
int luab_pushstring(lua_State *, const char *);
void    luab_pushtimesvector(lua_State *, int, size_t, void *);

#define luab_todata(L, narg, id, t) \
    ((t)luab_checkudata((L), (narg), (id)))
#define luab_rawsetinteger(L, narg, k, v)                   \
    do {                                                    \
        lua_pushinteger((L), (v));                          \
        lua_rawseti((L), (narg), (k));                      \
    } while (0)
#define luab_setinteger(L, narg, k, v)                      \
    do {                                                    \
        lua_pushinteger((L), (v));                          \
        lua_setfield((L), (narg), (k));                     \
    } while (0)
#define luab_setstring(L, narg, k, v)                       \
    do {                                                    \
        lua_pushstring((L), (v));                           \
        lua_setfield((L), (narg), (k));                     \
    } while (0)
#define luab_setudata(L, narg, id, k, v)                    \
    do {                                                    \
        if (luab_newuserdata((L), (id), (v)) != NULL)       \
            lua_setfield((L), (narg), (k));                 \
    } while (0)

/* Push binary string v by len at field k on instance of LUA_TTABLE */
static __inline void
luab_setbuff(lua_State *L, int narg, const char *k, caddr_t v, size_t len)
{
    luaL_Buffer b;
    caddr_t buf;

    if (k != NULL && v != NULL && len > 0) {
        luaL_buffinit(L, &b);

        buf = luaL_prepbuffsize(&b, len);

        (void)memmove(buf, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        lua_setfield(L, narg, k);
    }
}

static __inline lua_Integer
luab_tointeger(lua_State *L, int narg, lua_Integer b_msk)
{
    return ((lua_tointeger(L, narg)) & (b_msk));
}

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

static __inline void *
luab_checkudata(lua_State *L, int narg, luab_module_t *m)
{
    return (luaL_checkudata(L, narg, m->name));
}

static __inline void *
luab_checkudataisnil(lua_State *L, int narg, luab_module_t *m)
{
    if (lua_isnil(L, narg) != 0)
        return (NULL);

    return (luaL_checkudata(L, narg, m->name));
}

static __inline void
luab_pushinteger(lua_State *L, luab_type_un *un)
{
    lua_pushinteger(L, un->type_int);
}

static __inline void
luab_pushcfunction(lua_State *L, luab_type_un *un)
{
    lua_pushcfunction(L, un->type_fn);
}
__END_DECLS

#endif /* _LUABSD_H_ */
