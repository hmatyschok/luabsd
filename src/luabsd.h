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

__BEGIN_DECLS

typedef union {
    lua_Integer x_i;
    lua_CFunction   x_f;
} luab_un_t;

typedef void    (*luab_tab_fn_t)(lua_State *L, luab_un_t *x);

typedef struct {
    luab_tab_fn_t   init;
    const char    *key;
    luab_un_t   val;
} luab_table_t;

#define LUABSD_REG(fn, k, v) \
    { .init = fn, .key = k, v }
#define LUABSD_INT(k, v) \
    LUABSD_REG(luab_pushinteger, k, .val.x_i = v)
#define LUABSD_FUNC(k, v) \
    LUABSD_REG(luab_pushcfunction, k, .val.x_f = v)

typedef void (*luab_mod_fn_t)(void *ud, void *arg);

typedef struct {
    uint32_t  cookie;        /*  date -u +'%s' */
    const char  *name;
    luab_table_t    *vec;
    luab_mod_fn_t    init;
    size_t  sz;
} luab_module_t;

#define luab_todata(L, narg, id, t) \
    ((t)luab_checkudata((L), (narg), (id)))

#if __BSD_VISIBLE
extern luab_module_t db_type;
#endif
extern luab_module_t flock_type;
extern luab_module_t timespec_type;
extern luab_module_t stat_type;

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

static __inline void luab_pushcfunction(lua_State *, luab_un_t *);
static __inline void luab_pushinteger(lua_State *, luab_un_t *);

const char **    luab_checkargv(lua_State *, int);
int *   luab_checkintvector(lua_State *, int, size_t);
const char *    luab_checklstring(lua_State *, int, size_t);
int luab_checkmaxargs(lua_State *, int);
void *  luab_checkudata(lua_State *, int, luab_module_t *);
void *  luab_newuserdata(lua_State *, luab_module_t *, void *arg);

int luab_pusherr(lua_State *, int);
int luab_pushnil(lua_State *);

static __inline lua_Integer
luab_checkinteger(lua_State *L, int narg, lua_Integer b_msk)
{
    return ((luaL_checkinteger(L, narg)) & (b_msk));
}

static __inline void
luab_pushinteger(lua_State *L, luab_un_t *u)
{
    lua_pushinteger(L, u->x_i);
}

static __inline void
luab_pushcfunction(lua_State *L, luab_un_t *u)
{
    lua_pushcfunction(L, u->x_f);
}
__END_DECLS

#endif /* _LUABSD_H_ */
