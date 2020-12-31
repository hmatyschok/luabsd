/*
 * Copyright (c) 2020 Henning Matyschok
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

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <uuid.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_modules.h"
#include "luab_udata.h"

/*
 * XXX
 *  upcalls..
 */

#define LUAB_CORE_LIB_ID    1595987973
#define LUAB_CORE_LIB_KEY   "core"

/*
 * Generic service primitives, subset of <core>.
 */

void
luab_core_free(void *v, size_t sz)
{
    if (v != NULL) {
        if (sz > 0)
            (void)memset_s(v, sz, 0, sz);

        free(v);
    } else
        errno = ENOENT;
}

void
luab_core_freestr(caddr_t dp)
{
    size_t n;

    if (dp != NULL) {
        n = strnlen(dp, luab_env_buf_max);
        luab_core_free(dp, n);
    } else
        errno = ENOENT;
}

void *
luab_core_alloc(size_t n, size_t sz)
{
    size_t nbytes, len;
    caddr_t dp;

    if ((nbytes = (n * sz)) > 0) {
        len = nbytes + sz;

        if ((dp = malloc(len)) != NULL)
            (void)memset_s(dp, len, 0, len);
    } else {
        errno = ERANGE;
        dp = NULL;
    }
    return (dp);
}

void
luab_core_errx(int eval, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    verrx(eval, fmt, ap);
    va_end(ap);
}

void
luab_core_err(int eval, const char *fname, int up_call)
{
    err(eval, "%s: %s", fname, strerror(up_call));
}

void
luab_core_warn(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vwarn(fmt, ap);
    va_end(ap);
}

void
luab_core_argerror(lua_State *L, int narg, void *v, size_t n, size_t sz, int up_call)
{
    size_t len;

    if ((len = n * sz) != 0)
        luab_core_free(v, len);

    luaL_argerror(L, narg, strerror(up_call));
}

int
luab_core_checkmaxargs(lua_State *L, int nmax)
{
    int narg;

    if ((narg = lua_gettop(L)) > nmax)
        luaL_error(L, "#%d args, but #%d expected", narg, nmax);

    return (narg);
}

/*
 * XXX
 *  Generic service primitives, we should move those to <luab_core_udata>
 */

int
luab_core_create(lua_State *L, int narg, luab_module_t *m0, luab_module_t *m1)
{
    luab_module_t *m;
    caddr_t arg;

    if ((m = (m1 != NULL) ? m1 : m0) != NULL) {
        if (luab_core_checkmaxargs(L, narg) == 0)
            arg = NULL;
        else
            arg = luab_udata(L, narg, m, caddr_t);
    } else
        arg = NULL;

    return (luab_pushxdata(L, m0, arg));
}

int
luab_core_dump(lua_State *L, int narg, luab_module_t *m, size_t len)
{
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, narg);

    if (m != NULL && m->m_get != NULL)
        dp = (caddr_t)(*m->m_get)(L, narg);
    else
        dp = NULL;

    return (luab_iovec_pushxdata(L, dp, len, len));
}

int
luab_core_gc(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *self, *ud, *ud_tmp;

    (void)luab_core_checkmaxargs(L, narg);

    self = luab_todata(L, narg, m, luab_udata_t *);

    LIST_FOREACH_SAFE(ud, &self->ud_list, ud_next, ud_tmp)
        luab_udata_remove(ud);

    if (self->ud_xhd != NULL)
        luab_udata_remove(self);

    (void)memset_s(self, m->m_len, 0, m->m_len);

    return (0);
}

int
luab_core_len(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *ud;
    ssize_t len;

    (void)luab_core_checkmaxargs(L, narg);

    if ((ud = luab_todata(L, narg, m, luab_udata_t *)) != NULL)
        len = m->m_sz;
    else {
        errno = ERANGE;
        len = luab_env_error;
    }
    return (luab_pushxinteger(L, len));
}

int
luab_core_tostring(lua_State *L, int narg, luab_module_t *m)
{
    luab_udata_t *ud;
    int status;

    (void)luab_core_checkmaxargs(L, narg);

    if ((ud = luab_todata(L, narg, m, luab_udata_t *)) != NULL)
        status = luab_pushfstring(L, "%s (%p,%d)", m->m_name, ud, ud->ud_ts);
    else
        status = luab_pushfstring(L, "%s (%p,%d)", "nil", NULL, 0);   /* XXX */

    return (status);
}

/*
 * Interface of <core>.
 */

/***
 * Interface against uuidgen(2), derived from implementation of uuidgen(1).
 *
 * @function uuid
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (uuid [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage uuid [, err, msg ] = bsd.core.uuid()
 */
static int
luab_uuid(lua_State *L)
{
    uuid_t uuid;
    caddr_t buf;
    int status;

    (void)luab_core_checkmaxargs(L, 0);

    if ((status = uuidgen(&uuid, 1)) != 0)
        status = luab_pushnil(L);
    else {
        uuid_to_string(&uuid, &buf, &status);

        if (status == (int)uuid_s_ok) {
            status = luab_pushldata(L, buf, strlen(buf));
            free(buf);
        } else {
            errno = ENOMEM;
            status = luab_pushnil(L);
        }
    }
    return (status);
}

/* composite data types */
/***
 * Generator function - create an instance of (LUA_TUSERDATA(INTEGER)).
 *
 * @function integer_create
 *
 * @param integer           Instance of (LUA_TUSERDATA(INTEGER)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage integer [, err, msg ] = bsd.core.integer_create([ integer ])
 */
static int
luab_integer_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(INTEGER, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

static luab_module_table_t luab_core_vec[] = {
    LUAB_FUNC("uuid",               luab_uuid),

    /* composite data types */
    LUAB_FUNC("integer_create",     luab_integer_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_core_lib = {
    .m_id       = LUAB_CORE_LIB_ID,
    .m_name     = LUAB_CORE_LIB_KEY,
    .m_vec      = luab_core_vec,
};
