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
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t iovec_type;
extern luab_module_t in_addr_type;
extern luab_module_t in6_addr_type;

extern int luab_StructInAddr(lua_State *);
extern int luab_StructIn6Addr(lua_State *);

#define LUABSD_ARPA_INET_LIB_ID    1595780686
#define LUABSD_ARPA_INET_LIB_KEY   "inet"

extern luab_module_t luab_arpa_inet_lib;

/*
 * Fetch LUA_TUSERDATA(luab_module_t) by AF_XXX.
 * 
 * XXX Well, I'll should refactor this function - the switch-statement
 * XXX shall replaced by a so called "protosw-table", but not yet.
 */
static void *
luab_checkxaddr(lua_State *L, int narg, int af, size_t *len)
{
    luab_module_t *type = NULL;

    switch (af) {
    case AF_INET:
        type = &in_addr_type;
        *len = INET_ADDRSTRLEN;
        break;
    case AF_INET6:
        type = &in6_addr_type;
        *len = INET6_ADDRSTRLEN;
        break;
    default:
        luaL_argerror(L, narg, "Invalid argument");
        break;  /* not reached */
    }
    return (*type->get)(L, narg);
}

/***
 * inet_addr(3) - Internet address manipulation routines
 *
 * @function inet_addr
 *
 * @param cp                String denotes IPv4 address.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])   (in_addr [, nil]) on success or
 *                                                  (nil, (strerror(errno)))
 *
 * @usage in_addr [, msg ] = bsd.arpa.inet.inet_addr(cp)
 */
static int
luab_inet_addr(lua_State *L)
{
    const char *cp;
    struct in_addr ia;
    int status;

    (void)luab_checkmaxargs(L, 1);

    cp = luab_checklstring(L, 1, INET_ADDRSTRLEN);

    ia.s_addr = inet_addr(cp);

    if (luab_newuserdata(L, &in_addr_type, &ia) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}

/***
 * inet_ntoa(3) - Internet address manipulation routines
 *
 * @function inet_ntoa
 *
 * @param in                Instance of LUA_TUSERDATA(luab_in_addr_t).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_TSTRING ])     (cp [, nil]) on success or
 *                                                  (msg, (strerror(errno)))
 *
 * @usage cp [, msg ] = bsd.arpa.inet.inet_ntoa(in)
 */
static int
luab_inet_ntoa(lua_State *L)
{
    struct in_addr *ia;
    const char *cp;

    (void)luab_checkmaxargs(L, 1);

    ia = (struct in_addr *)(*in_addr_type.get)(L, 1);

    cp = inet_ntoa(*ia);

    return luab_pushstring(L, cp);
}

#if __BSD_VISIBLE
/***
 * inet_aton(3) - Internet address manipulation routines
 *
 * @function inet_aton
 *
 * @param cp                    Character String to be interpreted as address.
 * @param pin                   Instance of LUA_TUSERDATA(luab_in_addr_t).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (1 [, nil]) on success or
 *                                                  (0, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.arpa.inet.inet_aton(cp, pin)
 */
static int
luab_inet_aton(lua_State *L)
{
    const char *cp;
    struct in_addr *pin;
    int status;

    (void)luab_checkmaxargs(L, 2);

    cp = luab_checklstring(L, 1, INET_ADDRSTRLEN);
    pin = (struct in_addr *)(*in_addr_type.get)(L, 2);

    status = inet_aton(cp, pin);

    return luab_pusherr(L, status);
}

/***
 * inet_network(3) - Internet address manipulation routines
 *
 * @function inet_network
 *
 * @param cp                String denotes IPv4 address.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])   (in_addr [, nil]) on success or
 *                                                  (nil, (strerror(errno)))
 *
 * @usage in_addr [, msg ] = bsd.arpa.inet.inet_network(cp)
 */
static int
luab_inet_network(lua_State *L)
{
    const char *cp;
    struct in_addr ia;
    int status;

    (void)luab_checkmaxargs(L, 1);

    cp = luab_checklstring(L, 1, INET_ADDRSTRLEN);

    ia.s_addr = inet_network(cp);

    if (luab_newuserdata(L, &in_addr_type, &ia) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}

/***
 * inet_ntoa_r(3) - Internet address manipulation routines
 *
 * @function inet_ntoa_r
 *
 * @param in                    Instance of LUA_TUSERDATA(luab_in_addr_t).
 * @param buf                   Instance of LUA_TUSERDATA(luab_iovec_t) for
 *                              character String to be interpreted as address.
 * @param size                  Length of string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (1 [, nil]) on success or
 *                                                  (0, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.arpa.inet.inet_ntoa_r(cp, pin)
 */
static int
luab_inet_ntoa_r(lua_State *L)
{
    struct in_addr *in;
    luab_iovec_t *buf;
    socklen_t size;
    caddr_t caddr;
    int status;

    (void)luab_checkmaxargs(L, 3);

    in = (struct in_addr *)(*in_addr_type.get)(L, 1);
    buf = (luab_iovec_t *)(*iovec_type.get)(L, 2);
    size = luab_checkinteger(L, 3, INT_MAX);

    if (((caddr = buf->iov.iov_base) != NULL) &&
        (size <= buf->iov_max_len)) {
        if (inet_ntoa_r(*in, caddr, size) != NULL) {
            buf->iov.iov_len = size;
            status = 0;
        } else
            status = -1;
    } else {
        errno = ENXIO;
        status = -1;
    }
    return luab_pusherr(L, status);
}

/***
 * inet_pton(3) - Internet address manipulation routines
 *
 * @function inet_pton
 *
 * @param af                    Specifies address fromat over protocol domain(9).
 * @param src                   Instance of LUA_TUSERDATA(luab_iovec_t) for
 *                              character String to be interpreted as address.
 * @param dst                   Instance of LUA_TUSERDATA(luab_in{6}_addr_t)
 *                              for binary representation of character string
 *                              denotes OSI-L3 address.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.arpa.inet.pton(af, src, dst)
 */
static int
luab_inet_pton(lua_State *L)
{
    int af;
    luab_iovec_t *buf;
    void *dst;
    caddr_t src;
    size_t size;
    int status;

    (void)luab_checkmaxargs(L, 3);

    af = luab_checkinteger(L, 1, INT_MAX);
    buf = (luab_iovec_t *)(*iovec_type.get)(L, 2);
    dst = luab_checkxaddr(L, 3, af, &size);

    if (((src = buf->iov.iov_base) != NULL) &&
        (size <= buf->iov_max_len) &&
        (buf->iov.iov_len <= size))
        status = inet_pton(af, src, dst);
    else {
        errno = ENXIO;
        status = -1;
    }
    return luab_pusherr(L, status);
}

/***
 * inet_ntop(3) - Internet address manipulation routines
 *
 * @function inet_ntop
 *
 * @param af                    Specifies address fromat over protocol domain(9).
 * @param src                   Instance of LUA_TUSERDATA(luab_in{6}_addr_t)
 *                              for binary representation of character string
 *                              denotes OSI-L3 address.
 * @param dst                   Instance of LUA_TUSERDATA(luab_iovec_t) for
 *                              character String to be interpreted as address.
 * @param size                  Specifies constraint, size of character string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.arpa.inet.ntop(af, src, dst)
 */
static int
luab_inet_ntop(lua_State *L)
{
    int af;
    void *src;
    luab_iovec_t *buf;
    caddr_t dst;
    size_t size;
    int status;

    (void)luab_checkmaxargs(L, 4);

    af = luab_checkinteger(L, 1, INT_MAX);
    src = luab_checkxaddr(L, 2, af, &size);
    buf = (luab_iovec_t *)(*iovec_type.get)(L, 3);
    size = luab_checkinteger(L, 4, INT_MAX);

    if (((dst = buf->iov.iov_base) != NULL) &&
        (size <= buf->iov_max_len)) {
        if (inet_ntop(af, src, dst, size) != NULL) {
            buf->iov.iov_len = size;
            status = 0;
        } else
            status = -1;
    } else {
        errno = ENXIO;
        status = -1;
    }
    return luab_pusherr(L, status);
}
#endif /* __BSD_VISIBLE */

/*
 * Interface against <arpa/inet.h>.
 */

static luab_table_t luab_arpa_inet_vec[] = {
    LUABSD_INT("INET_ADDRSTRLEN",   INET_ADDRSTRLEN),
    LUABSD_INT("INET6_ADDRSTRLEN",  INET6_ADDRSTRLEN),
    LUABSD_FUNC("inet_addr",    luab_inet_addr),
    LUABSD_FUNC("inet_ntoa",    luab_inet_ntoa),
    LUABSD_FUNC("inet_ntop",    luab_inet_ntop),
    LUABSD_FUNC("inet_pton",    luab_inet_pton),
#if __BSD_VISIBLE
    LUABSD_FUNC("inet_aton",    luab_inet_aton),
#if 0
    LUABSD_FUNC("inet_lnaof",   luab_inet_lnaof),
    LUABSD_FUNC("inet_makeaddr",    luab_inet_makeaddr),
    LUABSD_FUNC("inet_neta",    luab_inet_neta),
    LUABSD_FUNC("inet_netof",   luab_inet_netof),
#endif
    LUABSD_FUNC("inet_network", luab_inet_network),
#if 0
    LUABSD_FUNC("inet_net_ntop",    luab_inet_net_ntop),
    LUABSD_FUNC("inet_net_pton",    luab_inet_net_pton),
#endif
    LUABSD_FUNC("inet_ntoa_r",  luab_inet_ntoa_r),
#if 0
    LUABSD_FUNC("inet_cidr_ntop",   luab_inet_cidr_ntop),
    LUABSD_FUNC("inet_cidr_pton",   luab_inet_cidr_pton),
    LUABSD_FUNC("inet_nsap_addr",   luab_inet_nsap_addr),
    LUABSD_FUNC("inet_nsap_ntoa",   luab_inet_nsap_ntoa),
#endif
#endif /* __BSD_VISIBLE */
    LUABSD_FUNC("StructInAddr", luab_StructInAddr),
    LUABSD_FUNC("StructIn6Addr", luab_StructIn6Addr),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_arpa_inet_lib = {
    .cookie = LUABSD_ARPA_INET_LIB_ID,
    .name = LUABSD_ARPA_INET_LIB_KEY,
    .vec = luab_arpa_inet_vec,
};
