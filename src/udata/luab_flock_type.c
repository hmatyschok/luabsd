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
    struct flock    flock;
} luab_flock_t;

#define luab_new_flock(L, arg) \
    ((luab_flock_t *)luab_newuserdata(L, &flock_type, (arg)))
#define luab_to_flock(L, narg) \
    (luab_toldata((L), (narg), &flock_type, \
        luab_flock_t *, sizeof(struct flock)))

#define LUABSD_FLOCK_TYPE_ID    1593623399
#define LUABSD_FLOCK_TYPE    "FLOCK*"

int luab_StructFlock(lua_State *);

/* starting offset - negative l_start, if l_whence = SEEK_{CUR,END} */
static int
Flock_set_l_start(lua_State *L)
{
    struct flock *l;
    off_t l_start;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, flock_type, struct flock *);
    l_start = (off_t)luab_checkinteger(L, 2, ULONG_MAX);

    l->l_start = l_start;

    return (luab_pusherr(L, l_start));
}

static int
Flock_get_l_start(lua_State *L)
{
    struct flock *l;
    off_t l_start;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, flock_type, struct flock *);
    l_start = l->l_start;

    return (luab_pusherr(L, l_start));
}

/* len = 0 means until end of file */
static int
Flock_set_l_len(lua_State *L)
{
    struct flock *l;
    off_t l_len;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, flock_type, struct flock *);
    l_len = (off_t)luab_checkinteger(L, 2, LONG_MAX);

    l->l_len = l_len;

    return (luab_pusherr(L, l_len));
}

static int
Flock_get_l_len(lua_State *L)
{
    struct flock *l;
    off_t l_len;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, flock_type, struct flock *);
    l_len = l->l_len;

    return (luab_pusherr(L, l_len));
}

/* lock owner */
static int
Flock_set_l_pid(lua_State *L)
{
    struct flock *l;
    pid_t l_pid;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, flock_type, struct flock *);
    l_pid = (pid_t)luab_checkinteger(L, 2, INT_MAX);

    l->l_pid = l_pid;

    return (luab_pusherr(L, l_pid));
}

static int
Flock_get_l_pid(lua_State *L)
{
    struct flock *l;
    pid_t l_pid;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, flock_type, struct flock *);
    l_pid = l->l_pid;

    return (luab_pusherr(L, l_pid));
}

/* lock type: read/write, etc. */
static int
Flock_set_l_type(lua_State *L)
{
    struct flock *l;
    short l_type;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, flock_type, struct flock *);
    l_type = (short)luab_checkinteger(L, 2, SHRT_MAX);

    l->l_type = l_type;

    return (luab_pusherr(L, l_type));
}

static int
Flock_get_l_type(lua_State *L)
{
    struct flock *l;
    int l_type;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, flock_type, struct flock *);
    l_type = l->l_type;

    return (luab_pusherr(L, l_type));
}

/* type of l_start */
static int
Flock_set_l_whence(lua_State *L)
{
    struct flock *l;
    short l_whence;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, flock_type, struct flock *);
    l_whence = (short)luab_checkinteger(L, 2, SHRT_MAX);

    l->l_whence = l_whence;

    return (luab_pusherr(L, l_whence));
}

static int
Flock_get_l_whence(lua_State *L)
{
    struct flock *l;
    int l_whence;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, flock_type, struct flock *);
    l_whence = l->l_whence;

    return (luab_pusherr(L, l_whence));
}

/* remote system id or zero for local */
static int
Flock_set_l_sysid(lua_State *L)
{
    struct flock *l;
    int l_sysid;

    (void)luab_checkmaxargs(L, 2);

    l = luab_udata(L, 1, flock_type, struct flock *);
    l_sysid = (int)luab_checkinteger(L, 2, INT_MAX);

    l->l_sysid = l_sysid;

    return (luab_pusherr(L, l_sysid));
}

static int
Flock_get_l_sysid(lua_State *L)
{
    struct flock *l;
    int l_sysid;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, flock_type, struct flock *);
    l_sysid = l->l_sysid;

    return (luab_pusherr(L, l_sysid));
}

/*
 * Maps attributes on flock{} to an instance of LUA_TTABLE.
 */
static int
Flock_get(lua_State *L)
{
    struct flock *l;

    (void)luab_checkmaxargs(L, 1);

    l = luab_udata(L, 1, flock_type, struct flock *);

    lua_newtable(L);

    luab_setinteger(L, -2, "l_start", l->l_start);
    luab_setinteger(L, -2, "l_len", l->l_len);
    luab_setinteger(L, -2, "l_pid", l->l_pid);
    luab_setinteger(L, -2, "l_type", l->l_type);
    luab_setinteger(L, -2, "l_whence", l->l_whence);
    luab_setinteger(L, -2, "l_sysid", l->l_sysid);

    lua_pushvalue(L, -1);

    return (1);
}

/***
 * Copy flock{} into LUA_TUSERDATA(luab_iovec_t).
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
Flock_dump(lua_State *L)
{
    luab_iovec_param_t iop;
    size_t len, max_len;
    caddr_t data;
    int status;

    (void)luab_checkmaxargs(L, 1);

    (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

    data = luab_udata(L, 1, flock_type, caddr_t);

    len = sizeof(struct flock);
    max_len = len + sizeof(uint32_t);

    iop.iop_buf.buf_len = max_len;
    iop.iop_data.buf_data = data;
    iop.iop_data.buf_len = len;

    if ((*iovec_type.ctor)(L, &iop) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

static int
Flock_gc(lua_State *L)
{
    return (luab_gc(L, 1, &flock_type));
}

static int
Flock_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &flock_type));
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
    LUABSD_FUNC("dump", Flock_dump),
    LUABSD_FUNC("__gc", Flock_gc),
    LUABSD_FUNC("__tostring",   Flock_tostring),
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
    .ctor = flock_create,
    .init = flock_init,
    .get = flock_udata,
    .sz = sizeof(luab_flock_t),
};

int
luab_StructFlock(lua_State *L)
{
    int narg;
    struct flock *flock;
    int status;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        flock = NULL;
    else
        flock = flock_udata(L, narg);

    if (flock_create(L, flock) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
