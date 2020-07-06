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

__BEGIN_DECLS

typedef union {
    lua_Integer x_i;
    lua_CFunction   x_f;
} luab_un_t;

typedef void    (*luab_fn_t)(lua_State *L, luab_un_t *x);

typedef struct {
    luab_fn_t   func;
    const char    *key;
    luab_un_t   val;
} luab_table_t;

#define LUABSD_REG(f, k, v) \
    { .func = f, .key = k, v }
#define LUABSD_INT(k, v) \
    LUABSD_REG(luab_pushinteger, k, .val.x_i = v)
#define LUABSD_FUNC(k, v) \
    LUABSD_REG(luab_pushcfunction, k, .val.x_f = v)

typedef struct {
    unsigned long   cookie;        /*  date -u +'%s' */
    const char  *name;
    luab_table_t    *vec;
} luab_module_t;

#define luab_todata(L, narg, id, t) \
    ((t)luaL_checkudata((L), (narg), (id)))

extern luab_module_t db_type;
extern luab_module_t flock_type;

extern luab_module_t luab_sys_file_lib;
extern luab_module_t luab_sys_stat_lib;
extern luab_module_t luab_sys_time_lib;
extern luab_module_t luab_sys_unistd_lib;

extern luab_module_t luab_db_lib;
extern luab_module_t luab_fcntl_lib;
extern luab_module_t luab_stdlib_lib;
extern luab_module_t luab_unistd_lib;
extern luab_module_t luab_uuid_lib;

static __inline lua_Integer luab_checkinteger(lua_State *, int, lua_Integer);
static __inline int luab_checkmaxargs(lua_State *, int);

static _inline void luab_pushcfunction(lua_State *, luab_un_t *);
static _inline void luab_pushinteger(lua_State *, luab_un_t *);


const char **    luab_checkargv(lua_State *, int);
int *   luab_checkintvector(lua_State *, int, size_t);
const char *    luab_checklstring(lua_State *, int, size_t);

static __inline lua_Integer
luab_checkinteger(lua_State *L, int narg, lua_Integer b_msk)
{
    return ((luaL_checkinteger(L, narg)) & (b_msk));
}

static __inline int
luab_checkmaxargs(lua_State *L, int nmax)
{
    int narg;

    if ((narg = lua_gettop(L)) > nmax)
        luaL_error(L, "#%d args, but #%d expected", narg, nmax);

    return narg;
}

int luab_pusherr(lua_State *, int);
int luab_pushnil(lua_State *);

static _inline void    /* XXX inline */
luab_pushinteger(lua_State *L, luab_un_t *u)
{
    lua_pushinteger(L, u->x_i);
}

static _inline
luab_pushcfunction(lua_State *L, luab_un_t *u)
{
    lua_pushcfunction(L, u->x_f);
}
__END_DECLS

#endif /* _LUABSD_H_ */
