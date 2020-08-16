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

extern luab_module_t hook_type;
extern luab_module_t iovec_type;
extern luab_module_t in_addr_type;
extern luab_module_t in6_addr_type;
extern luab_module_t sockaddr_type;

extern int luab_StructInAddr(lua_State *);
extern int luab_StructIn6Addr(lua_State *);
extern int luab_StructSockAddrIn(lua_State *);

#define LUABSD_ARPA_INET_LIB_ID    1595780686
#define LUABSD_ARPA_INET_LIB_KEY   "inet"

extern luab_module_t luab_arpa_inet_lib;

/*
 * Fetch LUA_TUSERDATA(luab_module_t) by AF_XXX.
 *
 * XXX Well, I'll should refactor and externalize this function - the
 * XXX switch-statement shall replaced by a so called "protosw-table",
 * XXX but not yet at this stage.
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
    return ((*type->get)(L, narg));
}

/***
 * inet_addr(3) - Internet address manipulation routines
 *
 * @function inet_addr
 *
 * @param cp                    String denotes IPv4 address.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (in_addr [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage in_addr [, err, msg] = bsd.arpa.inet.inet_addr(cp)
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

    return (status);
}

/***
 * inet_ntoa(3) - Internet address manipulation routines
 *
 * @function inet_ntoa
 *
 * @param in                    Instance of LUA_TUSERDATA(luab_in_addr_t).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (cp [, nil, nil]) on success or
 *          (msg, (errno, strerror(errno)))
 *
 * @usage cp [, err, msg] = bsd.arpa.inet.inet_ntoa(in)
 */
static int
luab_inet_ntoa(lua_State *L)
{
    struct in_addr *ia;
    const char *cp;

    (void)luab_checkmaxargs(L, 1);

    ia = luab_udata(L, 1, in_addr_type, struct in_addr *);

    cp = inet_ntoa(*ia);

    return (luab_pushstring(L, cp));
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
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_ntop(af, src, dst, size)
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

    af = (int)luab_checkinteger(L, 1, INT_MAX);
    src = luab_checkxaddr(L, 2, af, &size);
    buf = luab_udata(L, 3, iovec_type, luab_iovec_t *);
    size = (size_t)luab_checkinteger(L, 4,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if ((buf->iov_flags & IOV_LOCK) == 0) {
        buf->iov_flags |= IOV_LOCK;

        if (((dst = buf->iov.iov_base) != NULL) &&
            (size <= buf->iov_max_len) &&
            (buf->iov_flags & IOV_BUFF)) {

            if (inet_ntop(af, src, dst, size) != NULL) {
                buf->iov.iov_len = size;
                status = 0;
            } else
                status = -1;
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
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_pton(af, src, dst)
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

    af = (int)luab_checkinteger(L, 1, INT_MAX);
    buf = luab_udata(L, 2, iovec_type, luab_iovec_t *);
    dst = luab_checkxaddr(L, 3, af, &size);

    if ((buf->iov_flags & IOV_LOCK) == 0) {
        buf->iov_flags |= IOV_LOCK;

        if (((src = buf->iov.iov_base) != NULL) &&
            (size <= buf->iov_max_len) &&
            (buf->iov.iov_len <= size) &&
            (buf->iov_flags & IOV_BUFF))
            status = inet_pton(af, src, dst);
        else {
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

#if __BSD_VISIBLE
/***
 * inet_aton(3) - Internet address manipulation routines
 *
 * @function inet_aton
 *
 * @param cp                    Character String to be interpreted as address.
 * @param pin                   Instance of LUA_TUSERDATA(luab_in_addr_t).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (1 [, nil, nil]) on success or
 *          (0, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_aton(cp, pin)
 */
static int
luab_inet_aton(lua_State *L)
{
    const char *cp;
    struct in_addr *pin;
    int status;

    (void)luab_checkmaxargs(L, 2);

    cp = luab_checklstring(L, 1, INET_ADDRSTRLEN);
    pin = luab_udata(L, 2, in_addr_type, struct in_addr *);

    status = inet_aton(cp, pin);

    return (luab_pusherr(L, status));
}

/***
 * inet_lnaof(3) - Internet address manipulation routines
 *
 * @function inet_lnaof
 *
 * @param in                    Instance of LUA_TUSERDATA(luab_in_addr_t),
 *                              denotes internet address.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (in_addr [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage lna [, err, msg] = bsd.arpa.inet.inet_lnaof(in)
 */
static int
luab_inet_lnaof(lua_State *L)
{
    struct in_addr *ia;
    struct in_addr lna;
    int status;

    (void)luab_checkmaxargs(L, 1);

    ia = luab_udata(L, 1, in_addr_type, struct in_addr *);

    lna.s_addr = inet_lnaof(*ia);

    if (luab_newuserdata(L, &in_addr_type, &lna) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

/***
 * inet_makeaddr(3) - Internet address manipulation routines
 *
 * @function inet_makeaddr
 *
 * @param net                   Instance of LUA_TUSERDATA(luab_in_addr_t),
 *                              denotes internet network number.
 * @param lna                   Instance of LUA_TUSERDATA(luab_in_addr_t),
 *                              denotes local network address.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (in_addr [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage in_addr [, err, msg] = bsd.arpa.inet.inet_makeaddr(net, lna)
 */
static int
luab_inet_makeaddr(lua_State *L)
{
    struct in_addr *net;
    struct in_addr *lna;
    struct in_addr ia;
    int status;

    (void)luab_checkmaxargs(L, 2);

    net = luab_udata(L, 1, in_addr_type, struct in_addr *);
    lna = luab_udata(L, 2, in_addr_type, struct in_addr *);

    ia = inet_makeaddr(net->s_addr, lna->s_addr);

    if (luab_newuserdata(L, &in_addr_type, &ia) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

/***
 * inet_neta(3) - format an luab_in_addr_t network number into presentation format
 *
 * @function inet_neta
 *
 * @param src                   Instance of LUA_TUSERDATA(luab_in_addr_t),
 *                              denotes internet network number.
 * @param dst                   Buffer, instance of LUA_TUSERDATA(luab_iovec_t).
 * @param size                  Minimum size of character string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (1 [, nil, nil]) on success or
 *          (0, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_neta(src, dst, size)
 */
static int
luab_inet_neta(lua_State *L)
{
    struct in_addr *src;
    luab_iovec_t *buf;
    size_t size;
    caddr_t dst;
    int status;

    (void)luab_checkmaxargs(L, 3);

    src = luab_udata(L, 1, in_addr_type, struct in_addr *);
    buf = luab_udata(L, 2, iovec_type, luab_iovec_t *);
    size = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if ((buf->iov_flags & IOV_LOCK) == 0) {
        buf->iov_flags |= IOV_LOCK;

        if (((dst = buf->iov.iov_base) != NULL) &&
            (size <= buf->iov_max_len) &&
            (buf->iov_flags & IOV_BUFF)) {

            if (inet_neta(src->s_addr, dst, size) != NULL) {
                buf->iov.iov_len = size;
                status = 0;
            } else
                status = -1;
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
 * inet_netof(3) - Internet address manipulation routines
 *
 * @function inet_netof
 *
 * @param in                    Instance of LUA_TUSERDATA(luab_in_addr_t).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (in_addr [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage net [, err, msg] = bsd.arpa.inet.inet_netof(in)
 */
static int
luab_inet_netof(lua_State *L)
{
    struct in_addr *ia;
    struct in_addr net;
    int status;

    (void)luab_checkmaxargs(L, 1);

    ia = luab_udata(L, 1, in_addr_type, struct in_addr *);

    net.s_addr = inet_netof(*ia);

    if (luab_newuserdata(L, &in_addr_type, &net) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

/***
 * inet_network(3) - Internet address manipulation routines
 *
 * @function inet_network
 *
 * @param cp                    String denotes IPv4 address.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (in_addr [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage in_addr [, err, msg ] = bsd.arpa.inet.inet_network(cp)
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

    return (status);
}

/***
 * inet_net_ntop(3) - Internet address manipulation routines
 *
 * @function inet_net_ntop
 *
 * @param af                    Specifies address fromat over protocol domain(9).
 * @param src                   Instance of LUA_TUSERDATA(luab_in{6}_addr_t)
 *                              for binary representation of character string
 *                              denotes OSI-L3 address.
 * @param bits                  Cardinality of bitvector subset of OSI-L3
 *                              address masks network portion from address.
 * @param dst                   Instance of LUA_TUSERDATA(luab_iovec_t) for
 *                              character String to be interpreted as address.
 * @param size                  Specifies constraint, size of character string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_net_ntop(af, src, bits, dst, size)
 */
static int
luab_inet_net_ntop(lua_State *L)
{
    int af;
    void *src;
    int bits;
    luab_iovec_t *buf;
    caddr_t dst;
    size_t size;
    int status;

    (void)luab_checkmaxargs(L, 5);

    af = (int)luab_checkinteger(L, 1, INT_MAX);
    src = luab_checkxaddr(L, 2, af, &size);
    bits = (int)luab_checkinteger(L, 3, UINT_MAX);
    buf = luab_udata(L, 4, iovec_type, luab_iovec_t *);
    size = (size_t)luab_checkinteger(L, 5,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if ((buf->iov_flags & IOV_LOCK) == 0) {
        buf->iov_flags |= IOV_LOCK;

        if (((dst = buf->iov.iov_base) != NULL) &&
            (size <= buf->iov_max_len) &&
            (buf->iov_flags & IOV_BUFF)) {

            if (inet_net_ntop(af, src, bits, dst, size) != NULL) {
                buf->iov.iov_len = size;
                status = 0;
            } else
                status = -1;
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
 * inet_net_pton(3) - Internet address manipulation routines
 *
 * @function inet_net_pton
 *
 * @param af                    Specifies address fromat over protocol domain(9).
 * @param src                   Instance of LUA_TUSERDATA(luab_iovec_t) for
 *                              character String to be interpreted as address.
 * @param dst                   Instance of LUA_TUSERDATA(luab_in{6}_addr_t)
 *                              for binary representation of character string
 *                              denotes OSI-L3 address.
 * @param size                  Specifies constraint, size of character string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_net_pton(af, src, dst, size)
 */
static int
luab_inet_net_pton(lua_State *L)
{
    int af;
    luab_iovec_t *buf;
    void *dst;
    size_t size;
    caddr_t src;
    int status;

    (void)luab_checkmaxargs(L, 4);

    af = (int)luab_checkinteger(L, 1, INT_MAX);
    buf = luab_udata(L, 2, iovec_type, luab_iovec_t *);
    dst = luab_checkxaddr(L, 3, af, &size);
    size = (size_t)luab_checkinteger(L, 4,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if ((buf->iov_flags & IOV_LOCK) == 0) {
        buf->iov_flags |= IOV_LOCK;

        if (((src = buf->iov.iov_base) != NULL) &&
            (size <= buf->iov_max_len) &&
            (buf->iov.iov_len <= size) &&
            (buf->iov_flags & IOV_BUFF))
            status = inet_net_pton(af, src, dst, size);
        else {
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
 * inet_ntoa_r(3) - Internet address manipulation routines
 *
 * @function inet_ntoa_r
 *
 * @param in                    Instance of LUA_TUSERDATA(luab_in_addr_t).
 * @param buf                   Instance of LUA_TUSERDATA(luab_iovec_t) for
 *                              character String to be interpreted as address.
 * @param size                  Length of string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (1 [, nil, nil]) on success or
 *          (0, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_ntoa_r(in, buf, size)
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

    in = luab_udata(L, 1, in_addr_type, struct in_addr *);
    buf = luab_udata(L, 2, iovec_type, luab_iovec_t *);
    size = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if ((buf->iov_flags & IOV_LOCK) == 0) {
        buf->iov_flags |= IOV_LOCK;

        if (((caddr = buf->iov.iov_base) != NULL) &&
            (size <= buf->iov_max_len) &&
            (buf->iov_flags & IOV_BUFF)) {

            if (inet_ntoa_r(*in, caddr, size) != NULL) {
                buf->iov.iov_len = size;
                status = 0;
            } else
                status = -1;
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
 * inet_cidr_ntop(3) - Internet address manipulation routines
 *
 * @function inet_cidr_ntop
 *
 * @param af                    Specifies address fromat over protocol domain(9).
 * @param src                   Instance of LUA_TUSERDATA(luab_in{6}_addr_t)
 *                              for binary representation of character string
 *                              denotes OSI-L3 address.
 * @param bits                  Cardinality of bitvector subset of OSI-L3
 *                              address masks network portion from address.
 * @param dst                   Instance of LUA_TUSERDATA(luab_iovec_t) for
 *                              character String to be interpreted as address.
 * @param size                  Specifies constraint, size of character string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_cidr_ntop(af, src, bits, dst, size)
 */
static int
luab_inet_cidr_ntop(lua_State *L)
{
    int af;
    void *src;
    int bits;
    luab_iovec_t *buf;
    caddr_t dst;
    size_t size;
    int status;

    (void)luab_checkmaxargs(L, 5);

    af = (int)luab_checkinteger(L, 1, INT_MAX);
    src = luab_checkxaddr(L, 2, af, &size);
    bits = (int)luab_checkinteger(L, 3, UINT_MAX);
    buf = luab_udata(L, 4, iovec_type, luab_iovec_t *);
    size = (size_t)luab_checkinteger(L, 5,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if ((buf->iov_flags & IOV_LOCK) == 0) {
        buf->iov_flags |= IOV_LOCK;

        if (((dst = buf->iov.iov_base) != NULL) &&
            (size <= buf->iov_max_len) &&
            (buf->iov_flags & IOV_BUFF)) {

            if (inet_cidr_ntop(af, src, bits, dst, size) != NULL) {
                buf->iov.iov_len = size;
                status = 0;
            } else
                status = -1;
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
 * inet_cidr_pton(3) - Internet address manipulation routines
 *
 * @function inet_cidr_pton
 *
 * @param af                    Specifies address fromat over protocol domain(9).
 * @param src                   Instance of LUA_TUSERDATA(luab_iovec_t) for
 *                              character String to be interpreted as address.
 * @param dst                   Instance of LUA_TUSERDATA(luab_in{6}_addr_t)
 *                              for binary representation of character string
 *                              denotes OSI-L3 address.
 * @param bits                  Cardinality of bitvector subset of OSI-L3
 *                              address masks network portion from address.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_cidr_pton(af, src, dst, bits)
 */
static int
luab_inet_cidr_pton(lua_State *L)
{
    int af;
    luab_iovec_t *buf;
    void *dst;
    luab_type_u *un;
    int *bits;
    caddr_t src;
    size_t size;
    int status;

    (void)luab_checkmaxargs(L, 4);

    af = (int)luab_checkinteger(L, 1, INT_MAX);
    buf = luab_udata(L, 2, iovec_type, luab_iovec_t *);
    dst = luab_checkxaddr(L, 3, af, &size);
    un = luab_udata(L, 4, hook_type, luab_type_u *);
    bits = &(un->un_int);

    if ((buf->iov_flags & IOV_LOCK) == 0) {
        buf->iov_flags |= IOV_LOCK;

        if (((src = buf->iov.iov_base) != NULL) &&
            (size <= buf->iov_max_len) &&
            (buf->iov.iov_len <= size) &&
            (buf->iov_flags & IOV_BUFF))
            status = inet_cidr_pton(af, src, dst, bits);
        else {
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
#endif /* __BSD_VISIBLE */

/***
 * Ctor for sockaddr_in{}.
 *
 * @function StructSockAddrIn
 *
 * @param port              Specifies port ID, see /etc/services.
 * @param addr              Specifies ip(4) address by instance
 *                          of LUA_TUSERDATA(luab_in_addr_t).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sockaddr [, err, msg ] = bsd.arpa.inet.StructSockAddrIn([ port [, addr ]])
 */
int
luab_StructSockAddrIn(lua_State *L)
{
    struct sockaddr_in sin;
    struct sockaddr *sa;
    struct in_addr *addr;
    int status;

    sa = (struct sockaddr *)&sin;
    sockaddr_pci(sa, AF_INET, sizeof(sin));

    switch (luab_checkmaxargs(L, 2)) {     /* FALLTHROUGH */
    case 2:
        addr = luab_udata(L, 2, in_addr_type, struct in_addr *);
        (void)memmove(&sin.sin_addr, addr, sizeof(sin.sin_addr));
    case 1:
        sin.sin_port = (in_port_t)luab_checkinteger(L, 1, SHRT_MAX);
    default:
        sin.sin_addr.s_addr = htonl(sin.sin_addr.s_addr);
        sin.sin_port = htons(sin.sin_port);
        break;
    }

    if (sockaddr_create(L, sa) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}

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
    LUABSD_FUNC("inet_lnaof",   luab_inet_lnaof),
    LUABSD_FUNC("inet_makeaddr",    luab_inet_makeaddr),
    LUABSD_FUNC("inet_neta",    luab_inet_neta),
    LUABSD_FUNC("inet_netof",   luab_inet_netof),
    LUABSD_FUNC("inet_network", luab_inet_network),
    LUABSD_FUNC("inet_net_ntop",    luab_inet_net_ntop),
    LUABSD_FUNC("inet_net_pton",    luab_inet_net_pton),
    LUABSD_FUNC("inet_ntoa_r",  luab_inet_ntoa_r),
    LUABSD_FUNC("inet_cidr_ntop",   luab_inet_cidr_ntop),
    LUABSD_FUNC("inet_cidr_pton",   luab_inet_cidr_pton),
#if 0
    LUABSD_FUNC("inet_nsap_addr",   luab_inet_nsap_addr),
    LUABSD_FUNC("inet_nsap_ntoa",   luab_inet_nsap_ntoa),
#endif
#endif /* __BSD_VISIBLE */
    LUABSD_FUNC("StructInAddr", luab_StructInAddr),
    LUABSD_FUNC("StructIn6Addr", luab_StructIn6Addr),

    /* XXX wrong, because maps to <netinet/in.h> */
    LUABSD_FUNC("StructSockAddrIn", luab_StructSockAddrIn),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_arpa_inet_lib = {
    .cookie = LUABSD_ARPA_INET_LIB_ID,
    .name = LUABSD_ARPA_INET_LIB_KEY,
    .vec = luab_arpa_inet_vec,
};
