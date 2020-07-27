/*
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

#include <fcntl.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t flock_type;

/*
 * Interface against
 *
 *  struct flock {
 *      off_t   l_start;
 *      off_t   l_len;
 *      pid_t   l_pid;
 *      short   l_type;
 *      short   l_whence;
 *      int     l_sysid;
 *  };
 */

typedef struct luab_flock {
    struct flock    flock;
} luab_flock_t;

#define luab_newflock(L, arg) \
    ((luab_flock_t *)luab_newuserdata(L, &flock_type, (arg)))
#define luab_to_flock(L, narg) \
    (luab_todata((L), (narg), &flock_type, luab_flock_t *))

#define LUABSD_FLOCK_TYPE_ID    1593623399
#define LUABSD_FLOCK_TYPE    "FLOCK*"

int luab_StructFlock(lua_State *);

/* starting offset - negative l_start, if l_whence = SEEK_{CUR,END} */
static int
Flock_set_l_start(lua_State *L)
{
    struct flock *l;
    off_t l_start;

    luab_checkmaxargs(L, 2);

    l = (struct flock *)(*flock_type.get)(L, 1);
    l_start = luab_checkinteger(L, 2, ULONG_MAX);

    l->l_start = l_start;

    return 0;
}

static int
Flock_get_l_start(lua_State *L)
{
    struct flock *l;
    off_t l_start;

    luab_checkmaxargs(L, 1);

    l = (struct flock *)(*flock_type.get)(L, 1);
    l_start = l->l_start;

    lua_pushinteger(L, l_start);

    return 1;
}

/* len = 0 means until end of file */
static int
Flock_set_l_len(lua_State *L)
{
    struct flock *l;
    off_t l_len;

    luab_checkmaxargs(L, 2);

    l = (struct flock *)(*flock_type.get)(L, 1);
    l_len = luab_checkinteger(L, 2, LONG_MAX);

    l->l_len = l_len;

    return 0;
}

static int
Flock_get_l_len(lua_State *L)
{
    struct flock *l;
    off_t l_len;

    luab_checkmaxargs(L, 1);

    l = (struct flock *)(*flock_type.get)(L, 1);
    l_len = l->l_len;

    lua_pushinteger(L, l_len);

    return 1;
}

/* lock owner */
static int
Flock_set_l_pid(lua_State *L)
{
    struct flock *l;
    pid_t l_pid;

    luab_checkmaxargs(L, 2);

    l = (struct flock *)(*flock_type.get)(L, 1);
    l_pid = luab_checkinteger(L, 2, INT_MAX);

    l->l_pid = l_pid;

    return 0;
}

static int
Flock_get_l_pid(lua_State *L)
{
    struct flock *l;
    pid_t l_pid;

    luab_checkmaxargs(L, 1);

    l = (struct flock *)(*flock_type.get)(L, 1);
    l_pid = l->l_pid;

    lua_pushinteger(L, l_pid);

    return 1;
}

/* lock type: read/write, etc. */
static int
Flock_set_l_type(lua_State *L)
{
    struct flock *l;
    int l_type;

    luab_checkmaxargs(L, 2);

    l = (struct flock *)(*flock_type.get)(L, 1);
    l_type = luab_checkinteger(L, 2, SHRT_MAX);

    l->l_type = (short)l_type;

    return 0;
}

static int
Flock_get_l_type(lua_State *L)
{
    struct flock *l;
    int l_type;

    luab_checkmaxargs(L, 1);

    l = (struct flock *)(*flock_type.get)(L, 1);
    l_type = l->l_type;

    lua_pushinteger(L, l_type);

    return 1;
}

/* type of l_start */
static int
Flock_set_l_whence(lua_State *L)
{
    struct flock *l;
    int l_whence;

    luab_checkmaxargs(L, 2);

    l = (struct flock *)(*flock_type.get)(L, 1);
    l_whence = luab_checkinteger(L, 2, SHRT_MAX);

    l->l_whence = (short)l_whence;

    return 0;
}

static int
Flock_get_l_whence(lua_State *L)
{
    struct flock *l;
    int l_whence;

    luab_checkmaxargs(L, 1);

    l = (struct flock *)(*flock_type.get)(L, 1);
    l_whence = l->l_whence;

    lua_pushinteger(L, l_whence);

    return 1;
}

/* remote system id or zero for local */
static int
Flock_set_l_sysid(lua_State *L)
{
    struct flock *l;
    int l_sysid;

    luab_checkmaxargs(L, 2);

    l = (struct flock *)(*flock_type.get)(L, 1);
    l_sysid = luab_checkinteger(L, 2, INT_MAX);

    l->l_sysid = l_sysid;

    return 0;
}

static int
Flock_get_l_sysid(lua_State *L)
{
    struct flock *l;
    int l_sysid;

    luab_checkmaxargs(L, 1);

    l = (struct flock *)(*flock_type.get)(L, 1);
    l_sysid = l->l_sysid;

    lua_pushinteger(L, l_sysid);

    return 1;
}

/*
 * Maps attributes on flock{} to an instance of LUA_TTABLE.
 */
static int
Flock_get(lua_State *L)
{
    struct flock *l;

    luab_checkmaxargs(L, 1);

    l = (struct flock *)(*flock_type.get)(L, 1);

    lua_newtable(L);

    luab_setinteger(L, -2, "l_start", l->l_start);
    luab_setinteger(L, -2, "l_len", l->l_len);
    luab_setinteger(L, -2, "l_pid", l->l_pid);
    luab_setinteger(L, -2, "l_type", l->l_type);
    luab_setinteger(L, -2, "l_whence", l->l_whence);
    luab_setinteger(L, -2, "l_sysid", l->l_sysid);

    lua_pushvalue(L, -1);

    return 1;
}

static int
Flock_gc(lua_State *L)
{
    luab_flock_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_flock(L, 1);

    (void)memset_s(self, flock_type.sz, 0, flock_type.sz);

    return 0;
}

static int
Flock_tostring(lua_State *L)
{
    luab_flock_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_flock(L, 1);
    lua_pushfstring(L, "flock (%p)", self);

    return 1;
}

static luab_table_t flock_methods[] = {
    LUABSD_FUNC("set_l_start",  Flock_set_l_start),
    LUABSD_FUNC("set_l_len",    Flock_set_l_len),
    LUABSD_FUNC("set_l_pid",    Flock_set_l_pid),
    LUABSD_FUNC("set_l_type",   Flock_set_l_type),
    LUABSD_FUNC("set_l_whence", Flock_set_l_whence),
    LUABSD_FUNC("set_l_sysid",  Flock_set_l_sysid),
    LUABSD_FUNC("get",  Flock_get),
    LUABSD_FUNC("get_l_start",  Flock_get_l_start),
    LUABSD_FUNC("get_l_len",    Flock_get_l_len),
    LUABSD_FUNC("get_l_pid",    Flock_get_l_pid),
    LUABSD_FUNC("get_l_type",   Flock_get_l_type),
    LUABSD_FUNC("get_l_whence", Flock_get_l_whence),
    LUABSD_FUNC("get_l_sysid",  Flock_get_l_sysid),
    LUABSD_FUNC("__gc", Flock_gc),
    LUABSD_FUNC("__tostring",   Flock_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void
flock_init(void *ud, void *arg)
{                                           /* XXX */
    luab_flock_t *self = (luab_flock_t *)ud;

    (void)memmove(&self->flock, arg, sizeof(self->flock));
}

static void *
flock_udata(lua_State *L, int narg)
{
    luab_flock_t *self = luab_to_flock(L, narg);

    return (&self->flock);
}

luab_module_t flock_type = {
    .cookie = LUABSD_FLOCK_TYPE_ID,
    .name = LUABSD_FLOCK_TYPE,
    .vec = flock_methods,
    .init = flock_init,
    .get = flock_udata,
    .sz = sizeof(luab_flock_t),
};

int
luab_StructFlock(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 1);
    struct flock *flock;
    int status;

    if (narg == 0)
        flock = NULL;
    else
        flock = flock_udata(L, narg);

    if (luab_newflock(L, flock) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}
