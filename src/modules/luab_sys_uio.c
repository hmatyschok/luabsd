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

#include <sys/uio.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_SYS_UIO_LIB_ID    1594559271
#define LUAB_SYS_UIO_LIB_KEY   "uio"

extern luab_module_t luab_sys_uio_lib;

/*
 * Service primitives.
 */

/***
 * readv(2) - read input
 *
 * @function readv
 *
 * @param fd                Open file descriptor, (LUA_TNUMBER).
 * @param iov               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param iovcnt            Number of rx'd bytes, (LUA_TNUMBER).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.sys.uio.readv(fd, iov, iovcnt)
 */
static int
luab_readv(lua_State *L)
{
    int fd;
    luab_iovec_t *buf;
    size_t iovcnt;

    (void)luab_core_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, luab_env_int_max);
    buf = luab_udata(L, 2, luab_xmod(IOVEC, TYPE, __func__), luab_iovec_t *);
    iovcnt = (size_t)luab_checklinteger(L, 3, 0);

    return (luab_iovec_readv(L, fd, buf, iovcnt));
}

/***
 * writev(2) - write output
 *
 * @function writev
 *
 * @param fd                Open file descriptor, (LUA_TNUMBER).
 * @param iov               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param iovcnt            Specifies number of tx'd bytes, (LUA_TNUMBER).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.sys.uio.writev(fd, iov, iovcnt)
 */
static int
luab_writev(lua_State *L)
{
    int fd;
    luab_iovec_t *buf;
    size_t iovcnt;

    (void)luab_core_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, luab_env_int_max);
    buf = luab_udata(L, 2, luab_xmod(IOVEC, TYPE, __func__), luab_iovec_t *);
    iovcnt = (size_t)luab_checklinteger(L, 3, 0);

    return (luab_iovec_writev(L, fd, buf, iovcnt));
}

#if __BSD_VISIBLE
/***
 * preadv(2) - read input
 *
 * @function preadv
 *
 * @param fd                Open file descriptor, (LUA_TNUMBER).
 * @param iov               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param iovcnt            Specifies number of rx'd bytes, (LUA_TNUMBER).
 * @param offset            Specifies start position for input, (LUA_TNUMBER).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.sys.uio.preadv(fd, iov, iovcnt, offset)
 */
static int
luab_preadv(lua_State *L)
{
    int fd;
    luab_iovec_t *buf;
    size_t iovcnt;
    off_t offset;

    (void)luab_core_checkmaxargs(L, 4);

    fd = (int)luab_checkinteger(L, 1, luab_env_int_max);
    buf = luab_udata(L, 2, luab_xmod(IOVEC, TYPE, __func__), luab_iovec_t *);
    iovcnt = (size_t)luab_checklinteger(L, 3, 0);
    offset = (off_t)luab_checkinteger(L, 4, luab_env_long_max);

    return (luab_iovec_preadv(L, fd, buf, iovcnt, offset));
}

/***
 * pwritev(2) - write output
 *
 * @function pwritev
 *
 * @param fd                Open file descriptor, (LUA_TNUMBER).
 * @param iov               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param iovcnt            Specifies number of rx'd bytes, (LUA_TNUMBER).
 * @param offset            Specifies start position for input, (LUA_TNUMBER).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.sys.uio.pwritev(fd, iov, iovcnt, offset)
 */
static int
luab_pwritev(lua_State *L)
{
    int fd;
    luab_iovec_t *buf;
    size_t iovcnt;
    off_t offset;

    (void)luab_core_checkmaxargs(L, 4);

    fd = (int)luab_checkinteger(L, 1, luab_env_int_max);
    buf = luab_udata(L, 2, luab_xmod(IOVEC, TYPE, __func__), luab_iovec_t *);
    iovcnt = (size_t)luab_checklinteger(L, 3, 0);
    offset = (off_t)luab_checkinteger(L, 4, luab_env_long_max);

    return (luab_iovec_preadv(L, fd, buf, iovcnt, offset));
}
#endif /* __BSD_VISIBLE */

/*
 * Generator functions.
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(IOVEC)).
 *
 * @function iovec_create
 *
 * @param max_len           Capacity in bytes, (LUA_TNUMBER).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = bsd.sys.uio.iovec_create(max_len)
 */
static int
luab_iovec_create(lua_State *L)
{
    size_t max_len;

    (void)luab_core_checkmaxargs(L, 2);

    max_len = (size_t)luab_checklinteger(L, 1, 0);

    return (luab_iovec_pushxdata(L, NULL, 0, max_len));
}

/*
 * Interface against <sys/uio.h>.
 */

static luab_module_table_t luab_sys_uio_vec[] = {
#if __BSD_VISIBLE
    LUAB_INT("UIO_READ",      UIO_READ),
    LUAB_INT("UIO_WRITE",     UIO_WRITE),
    LUAB_INT("UIO_USERSPACE", UIO_USERSPACE),
    LUAB_INT("UIO_SYSSPACE",  UIO_SYSSPACE),
    LUAB_INT("UIO_NOCOPY",    UIO_NOCOPY),
#endif /* __BSD_VISIBLE */
    LUAB_FUNC("readv",        luab_readv),
    LUAB_FUNC("writev",       luab_writev),
#if __BSD_VISIBLE
    LUAB_FUNC("preadv",       luab_preadv),
    LUAB_FUNC("pwritev",      luab_pwritev),
#endif
    LUAB_FUNC("iovec_create", luab_iovec_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_uio_lib = {
    .m_cookie   = LUAB_SYS_UIO_LIB_ID,
    .m_name     = LUAB_SYS_UIO_LIB_KEY,
    .m_vec      = luab_sys_uio_vec,
};
