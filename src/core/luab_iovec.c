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

#include <errno.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

/*
 * Generic service primitives on (LUA_TUSERDATA(iovec)). 
 */

const char *
luab_iovec_islxarg(lua_State *L, int narg, size_t len)
{
    luab_iovec_t *iov;
    const char *buf;

    if (((iov = luab_isiovec(L, narg)) != NULL) &&
        (iov->iov_flags & (IOV_BUFF|IOV_PROXY)) &&
        (len <= iov->iov_max_len))
        buf = iov->iov.iov_base;
    else
        buf = luab_islstring(L, narg, len);

    return (buf);
}

const char *
luab_iovec_checklxarg(lua_State *L, int narg, size_t len)
{
    const char *buf;

    if ((buf = luab_iovec_islxarg(L, narg, len)) == NULL)
        luaL_argerror(L, narg, "Invalid argument");

    return (buf);
}

/*
 * Operations on stack.
 */

int
luab_pushiovec(lua_State *L, void *v, size_t len, size_t max_len)
{
    luab_iovec_param_t iop;
    int status;

    if (len > 0 && max_len >= len) {
        (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

        iop.iop_buf.buf_len = max_len;
        iop.iop_data.buf_len = len;
        iop.iop_data.buf_data = v;

        status = luab_pushudata(L, &iovec_type, &iop);
    } else {
        errno = EINVAL;
        status = luab_pushnil(L);
    }
    return (status);
}

/*
 * Operations on LUA_TTABLE.
 */

void
luab_rawsetiovec(lua_State *L, int narg, lua_Integer k, void *v, size_t len)
{
    luab_iovec_param_t iop;

    if (v != NULL && len > 0) {
        (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

        iop.iop_buf.buf_len = len;
        iop.iop_data.buf_len = len;
        iop.iop_data.buf_data = v;

        luab_rawsetudata(L, narg, &iovec_type, k, &iop);
    }
}

void
luab_setiovec(lua_State *L, int narg, const char *k, void *v, size_t len)
{
    luab_iovec_param_t iop;

    if (v != NULL && len > 0) {
        (void)memset_s(&iop, sizeof(iop), 0, sizeof(iop));

        iop.iop_buf.buf_len = len;
        iop.iop_data.buf_len = len;
        iop.iop_data.buf_data = v;

        luab_setudata(L, narg, &iovec_type, k, &iop);
    }
}
