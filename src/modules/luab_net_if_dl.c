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

#include <sys/types.h>
#include <sys/socket.h>

#include <net/if.h>
#include <net/if_dl.h>

#include <errno.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t sockaddr_type;

#define LUABSD_NET_IF_DL_LIB_ID    1596382827
#define LUABSD_NET_IF_DL_LIB_KEY    "if_dl"

extern luab_module_t luab_net_if_dl_lib;

/***
 * link_addr(3) - interpret character strings representing link level addresses.
 *
 * @function link_addr
 *
 * @param addr              Character string denotes address.
 * @param sdl               Storage for link level address.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (0, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.net.if_dl.link_addr(addr, sdl)
 */
static int
luab_link_addr(lua_State *L)
{
    const char *addr;
    struct sockaddr_dl *sdl;

    (void)luab_checkmaxargs(L, 2);

    addr = luab_checklstring(L, 1, LUAB_SDL_DATA_MAX_LEN); /* XXX */
    sdl = luab_udata(L, 2, sockaddr_type, struct sockaddr_dl *);

    link_addr(addr, sdl);

    return (luab_pusherr(L, 0));
}

/***
 * link_ntoa(3) - interpret link level address as ASCII string
 *
 * @function link_ntoa
 *
 * @param sdl               Instance of (LUA_TUSERDATA(SOCKADDR)),
 *                          link level address.
 * @param buf               Instance of (LUA_TUSERDATA(IOVEC)),
 *                          ASCII string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.net.if_dl.link_ntoa(sdl, buf)
 */
static int
luab_link_ntoa(lua_State *L)
{
    struct sockaddr_dl *sdl;
    luab_iovec_t *buf;
    caddr_t src, dst;
    size_t len;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sdl = luab_udata(L, 1, sockaddr_type, struct sockaddr_dl *);
    buf = luab_udata(L, 2, iovec_type, luab_iovec_t *);

    if ((buf->iov_flags & IOV_LOCK) == 0) {
        buf->iov_flags |= IOV_LOCK;

        if (((dst = buf->iov.iov_base) != NULL) &&
            (LUAB_SDL_DATA_MAX_LEN <= buf->iov_max_len) &&
            (buf->iov_flags & IOV_BUFF)) {

            if ((src = link_ntoa(sdl)) != NULL) {   /* XXX static buffer */
                len = strnlen(src, LUAB_SDL_DATA_MAX_LEN);
                (void)memmove(dst, src, len);
                buf->iov.iov_len = len;
                status = 0;
            } else {
                errno = EINVAL;
                status = -1;
            }
        } else {
            errno = ENXIO;
            status = -1;
        }
        buf->iov_flags &= ~IOV_LOCK;
    } else {
        errno = EBUSY;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(SOCKADDR)).
 *
 * @function sockaddr_dl_create
 *
 * @param data          Instance of (LUA_TUSERDATA(SOCKADDR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (sockaddr [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage sockaddr [, err, msg ] = bsd.net.if_dl.sockaddr_dl_create([ data ])
 */
static int
luab_sockaddr_dl_create(lua_State *L)
{
    struct sockaddr_dl sdl;
    struct sockaddr *data;

    (void)luab_checkmaxargs(L, 0);

    data = (struct sockaddr *)&sdl;
    luab_sockaddr_pci(data, AF_LINK, sizeof(sdl));

    return (luab_pushudata(L, &sockaddr_type, data));
}

/*
 * Interface against <net/if_dl.h>.
 */

static luab_table_t luab_net_if_dl_vec[] = {
    LUABSD_FUNC("link_addr",    luab_link_addr),
    LUABSD_FUNC("link_ntoa",    luab_link_ntoa),
    LUABSD_FUNC("sockaddr_dl_create", luab_sockaddr_dl_create),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_net_if_dl_lib = {
    .cookie = LUABSD_NET_IF_DL_LIB_ID,
    .name = LUABSD_NET_IF_DL_LIB_KEY,
    .vec = luab_net_if_dl_vec,
};
