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
    luab_udata_t    ud_softc;
    struct flock    ud_flock;
} luab_flock_t;

#define luab_new_flock(L, arg) \
    ((luab_flock_t *)luab_newuserdata(L, &flock_type, (arg)))
#define luab_to_flock(L, narg) \
    (luab_toldata((L), (narg), &flock_type, \
        struct flock *, sizeof(struct flock)))

#define LUABSD_FLOCK_TYPE_ID    1593623399
#define LUABSD_FLOCK_TYPE    "FLOCK*"

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(FLOCK)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              l_start     = (LUA_TNUMBER),
 *              l_len       = (LUA_TNUMBER),
 *              l_pid       = (LUA_TNUMBER),
 *              l_type      = (LUA_TNUMBER),
 *              l_whence    = (LUA_TNUMBER),
 *              l_sysid     = (LUA_TNUMBER),
 *          }
 *
 * @usage t = flock:get()
 */
static int
FLOCK_get(lua_State *L)
{
    struct flock *l;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, &flock_type, struct flock *);

    lua_newtable(L);

    luab_setinteger(L, -2, "l_start",   l->l_start);
    luab_setinteger(L, -2, "l_len",     l->l_len);
    luab_setinteger(L, -2, "l_pid",     l->l_pid);
    luab_setinteger(L, -2, "l_type",    l->l_type);
    luab_setinteger(L, -2, "l_whence",  l->l_whence);
    luab_setinteger(L, -2, "l_sysid",   l->l_sysid);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Generator function - translate flock{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (iovec [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage iovec [, err, msg ] = flock:dump()
 */
static int
FLOCK_dump(lua_State *L)
{
    return (luab_dump(L, 1, &flock_type, sizeof(struct flock)));
}

/*
 * Accessor.
 */

/* starting offset - negative l_start, if l_whence = SEEK_{CUR,END} */
static int
FLOCK_set_l_start(lua_State *L)
{
    struct flock *l;
    off_t data;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, &flock_type, struct flock *);
    data = (off_t)luab_checkinteger(L, 2, ULONG_MAX);

    l->l_start = data;

    return (luab_pusherr(L, data));
}

static int
FLOCK_get_l_start(lua_State *L)
{
    struct flock *l;
    off_t data;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, &flock_type, struct flock *);
    data = l->l_start;

    return (luab_pusherr(L, data));
}

/* len = 0 means until end of file */
static int
FLOCK_set_l_len(lua_State *L)
{
    struct flock *l;
    off_t data;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, &flock_type, struct flock *);
    data = (off_t)luab_checkinteger(L, 2, LONG_MAX);

    l->l_len = data;

    return (luab_pusherr(L, data));
}

static int
FLOCK_get_l_len(lua_State *L)
{
    struct flock *l;
    off_t data;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, &flock_type, struct flock *);
    data = l->l_len;

    return (luab_pusherr(L, data));
}

/* lock owner */
static int
FLOCK_set_l_pid(lua_State *L)
{
    struct flock *l;
    pid_t data;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, &flock_type, struct flock *);
    data = (pid_t)luab_checkinteger(L, 2, INT_MAX);

    l->l_pid = data;

    return (luab_pusherr(L, data));
}

static int
FLOCK_get_l_pid(lua_State *L)
{
    struct flock *l;
    pid_t data;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, &flock_type, struct flock *);
    data = l->l_pid;

    return (luab_pusherr(L, data));
}

/* lock type: read/write, etc. */
static int
FLOCK_set_l_type(lua_State *L)
{
    struct flock *l;
    short data;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, &flock_type, struct flock *);
    data = (short)luab_checkinteger(L, 2, SHRT_MAX);

    l->l_type = data;

    return (luab_pusherr(L, data));
}

static int
FLOCK_get_l_type(lua_State *L)
{
    struct flock *l;
    int data;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, &flock_type, struct flock *);
    data = l->l_type;

    return (luab_pusherr(L, data));
}

/* type of l_start */
static int
FLOCK_set_l_whence(lua_State *L)
{
    struct flock *l;
    short data;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, &flock_type, struct flock *);
    data = (short)luab_checkinteger(L, 2, SHRT_MAX);

    l->l_whence = data;

    return (luab_pusherr(L, data));
}

static int
FLOCK_get_l_whence(lua_State *L)
{
    struct flock *l;
    int data;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, &flock_type, struct flock *);
    data = l->l_whence;

    return (luab_pusherr(L, data));
}

/* remote system id or zero for local */
static int
FLOCK_set_l_sysid(lua_State *L)
{
    struct flock *l;
    int data;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, &flock_type, struct flock *);
    data = (int)luab_checkinteger(L, 2, INT_MAX);

    l->l_sysid = data;

    return (luab_pusherr(L, data));
}

static int
FLOCK_get_l_sysid(lua_State *L)
{
    struct flock *l;
    int data;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, &flock_type, struct flock *);
    data = l->l_sysid;

    return (luab_pusherr(L, data));
}

/*
 * Meta-methods.
 */

static int
FLOCK_gc(lua_State *L)
{
    return (luab_gc(L, 1, &flock_type));
}

static int
FLOCK_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &flock_type));
}

/*
 * Internal interface.
 */
 
static luab_table_t flock_methods[] = {
    LUABSD_FUNC("set_l_start",  FLOCK_set_l_start),
    LUABSD_FUNC("set_l_len",    FLOCK_set_l_len),
    LUABSD_FUNC("set_l_pid",    FLOCK_set_l_pid),
    LUABSD_FUNC("set_l_type",   FLOCK_set_l_type),
    LUABSD_FUNC("set_l_whence", FLOCK_set_l_whence),
    LUABSD_FUNC("set_l_sysid",  FLOCK_set_l_sysid),
    LUABSD_FUNC("get",          FLOCK_get),
    LUABSD_FUNC("get_l_start",  FLOCK_get_l_start),
    LUABSD_FUNC("get_l_len",    FLOCK_get_l_len),
    LUABSD_FUNC("get_l_pid",    FLOCK_get_l_pid),
    LUABSD_FUNC("get_l_type",   FLOCK_get_l_type),
    LUABSD_FUNC("get_l_whence", FLOCK_get_l_whence),
    LUABSD_FUNC("get_l_sysid",  FLOCK_get_l_sysid),
    LUABSD_FUNC("dump",         FLOCK_dump),
    LUABSD_FUNC("__gc",         FLOCK_gc),
    LUABSD_FUNC("__tostring",   FLOCK_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
flock_create(lua_State *L, void *arg)
{
    return (luab_new_flock(L, arg));
}

static void
flock_init(void *ud, void *arg)
{
    luab_flock_t *self;

    if (((self = (luab_flock_t *)ud) != NULL) && (arg != NULL))
        (void)memmove(&self->ud_flock, arg, sizeof(self->ud_flock));
}

static void *
flock_udata(lua_State *L, int narg)
{
    return (luab_to_flock(L, narg));
}

luab_module_t flock_type = {
    .cookie = LUABSD_FLOCK_TYPE_ID,
    .name = LUABSD_FLOCK_TYPE,
    .vec = flock_methods,
    .create = flock_create,
    .init = flock_init,
    .get = flock_udata,
    .sz = sizeof(luab_flock_t),
};
