/*
 * Copyright (c) 2020, 2021 Henning Matyschok
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

#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_sockaddr.h"

#define LUAB_ARPA_INET_LIB_ID    1595780686
#define LUAB_ARPA_INET_LIB_KEY   "inet"

extern luab_module_t luab_arpa_inet_lib;

/*
 * Subr.
 *
 * f: (AF_XXX) -> (LUA_TUSERDATA(SOCKADDR))
 */
static void *
luab_checkxaddr(lua_State *L, int narg, int af, size_t *len)
{
    luab_module_t *xmod = NULL;

    switch (af) {
    case AF_INET:
        xmod = luab_xmod(IN_ADDR, TYPE, __func__);
        *len = INET_ADDRSTRLEN;
        break;
    case AF_INET6:
        xmod = luab_xmod(IN6_ADDR, TYPE, __func__);
        *len = INET6_ADDRSTRLEN;
        break;
    default:
        break;
    }

    if (xmod != NULL) {

        if (xmod->m_get != NULL)
            return ((*xmod->m_get)(L, narg));
        else
            luab_core_argerror(L, narg, NULL, 0, 0, ENXIO);
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, EAFNOSUPPORT);

    return (NULL);
}

/*
 * Service primitves.
 */

/***
 * inet_addr(3) - Internet address manipulation routines
 *
 * @function inet_addr
 *
 * @param cp                    String represents IPv4 address.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage in_addr [, err, msg] = bsd.arpa.inet.inet_addr(cp)
 */
static int
luab_inet_addr(lua_State *L)
{
    luab_module_t *m;
    const char *cp;
    struct in_addr ia;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IN_ADDR, TYPE, __func__);

    cp = luab_checklstring(L, 1, INET_ADDRSTRLEN, NULL);
    ia.s_addr = inet_addr(cp);

    return (luab_pushxdata(L, m, &ia));
}

/***
 * inet_ntoa(3) - Internet address manipulation routines
 *
 * @function inet_ntoa
 *
 * @param ia                    Instance of (LUA_TUSERDATA(IN_ADDR)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage cp [, err, msg] = bsd.arpa.inet.inet_ntoa(in)
 */
static int
luab_inet_ntoa(lua_State *L)
{
    luab_module_t *m;
    struct in_addr *ia;
    const char *cp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IN_ADDR, TYPE, __func__);

    ia = luab_udata(L, 1, m, struct in_addr *);
    cp = inet_ntoa(*ia);

    return (luab_pushstring(L, cp));
}

/***
 * inet_ntop(3) - Internet address manipulation routines
 *
 * @function inet_ntop
 *
 * @param af                    Specifies address fromat over protocol domain(9).
 * @param src                   Instance of (LUA_TUSERDATA(IN6_ADDR)) holds a
 *                              binary representation of character string
 *                              denotes OSI-L3 address.
 * @param dst                   Instance of (LUA_TUSERDATA(IOVEC)) holds a
 *                              character String to be interpreted as IPv4
 *                              address.
 * @param size                  Specifies constraint, size of character string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_ntop(af, src, dst, size)
 */
static int
luab_inet_ntop(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    int af;
    void *src;
    luab_iovec_t *buf;
    caddr_t dst;
    size_t size;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);
    m2 = luab_xmod(SIZE, TYPE, __func__);

    af = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    src = luab_checkxaddr(L, 2, af, &size);
    buf = luab_udata(L, 3, m1, luab_iovec_t *);
    size = (size_t)luab_checklxinteger(L, 4, m2, 0);

    if (((dst = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (size <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if (inet_ntop(af, src, dst, size) != NULL) {
            buf->iov.iov_len = size;
            status = luab_env_success;
        } else
            status = luab_env_error;

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * inet_pton(3) - Internet address manipulation routines
 *
 * @function inet_pton
 *
 * @param af                    Specifies address fromat over protocol domain(9).
 * @param src                   Instance of (LUA_TUSERDATA(IOVEC)) holds a
 *                              character String to be interpreted as address.
 * @param dst                   Instance of (LUA_TUSERDATA(IN6_ADDR)) holds
 *                              binary representation of character string
 *                              denotes OSI-L3 address.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_pton(af, src, dst)
 */
static int
luab_inet_pton(lua_State *L)
{
    luab_module_t *m0, *m1;
    int af;
    luab_iovec_t *buf;
    void *dst;
    caddr_t src;
    size_t size;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);

    af = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    buf = luab_udata(L, 2, m1, luab_iovec_t *);
    dst = luab_checkxaddr(L, 3, af, &size);

    if (((src = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (size <= buf->iov.iov_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);
        status = inet_pton(af, src, dst);
        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

#if __BSD_VISIBLE
/***
 * inet_aton(3) - Internet address manipulation routines
 *
 * @function inet_aton
 *
 * @param cp                    Character String to be interpreted as address.
 * @param pin                   Instance of (LUA_TUSERDATA(IN_ADDR)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_aton(cp, pin)
 */
static int
luab_inet_aton(lua_State *L)
{
    luab_module_t *m;
    const char *cp;
    struct in_addr *pin;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(IN_ADDR, TYPE, __func__);

    cp = luab_checklstring(L, 1, INET_ADDRSTRLEN, NULL);
    pin = luab_udata(L, 2, m, struct in_addr *);

    status = inet_aton(cp, pin);

    return (luab_pushxinteger(L, status));
}

/***
 * inet_lnaof(3) - Internet address manipulation routines
 *
 * @function inet_lnaof
 *
 * @param in                    Instance of (LUA_TUSERDATA(IN_ADDR)),
 *                              denotes internet address.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage lna [, err, msg] = bsd.arpa.inet.inet_lnaof(in)
 */
static int
luab_inet_lnaof(lua_State *L)
{
    luab_module_t *m;
    struct in_addr *ia;
    struct in_addr lna;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IN_ADDR, TYPE, __func__);

    ia = luab_udata(L, 1, m, struct in_addr *);
    lna.s_addr = inet_lnaof(*ia);

    return (luab_pushxdata(L, m, &lna));
}

/***
 * inet_makeaddr(3) - Internet address manipulation routines
 *
 * @function inet_makeaddr
 *
 * @param net                   Instance of (LUA_TUSERDATA(IN_ADDR)), holds
 *                              an internet network number.
 * @param lna                   Instance of (LUA_TUSERDATA(IN_ADDR)), holds
 *                              a local network address.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage in_addr [, err, msg] = bsd.arpa.inet.inet_makeaddr(net, lna)
 */
static int
luab_inet_makeaddr(lua_State *L)
{
    luab_module_t *m;
    struct in_addr *net;
    struct in_addr *lna;
    struct in_addr ia;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(IN_ADDR, TYPE, __func__);

    net = luab_udata(L, 1, m, struct in_addr *);
    lna = luab_udata(L, 2, m, struct in_addr *);

    ia = inet_makeaddr(net->s_addr, lna->s_addr);

    return (luab_pushxdata(L, m, &ia));
}

/***
 * inet_neta(3) - format an luab_in_addr_t network number into presentation format
 *
 * @function inet_neta
 *
 * @param src                   Instance of (LUA_TUSERDATA(IN_ADDR)), holds
 *                              an internet network number.
 * @param dst                   Buffer, instance of (LUA_TUSERDATA(IOVEC)).
 * @param size                  Minimum size of character string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_neta(src, dst, size)
 */
static int
luab_inet_neta(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct in_addr *src;
    luab_iovec_t *buf;
    size_t size;
    caddr_t dst;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(IN_ADDR, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);

    src = luab_udata(L, 1, m0, struct in_addr *);
    buf = luab_udata(L, 2, m1, luab_iovec_t *);
    size = (size_t)luab_checklinteger(L, 3, 0);

    if (((dst = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (size <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if (inet_neta(src->s_addr, dst, size) != NULL) {
            buf->iov.iov_len = size;
            status = luab_env_success;
        } else
            status = luab_env_error;

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * inet_netof(3) - Internet address manipulation routines
 *
 * @function inet_netof
 *
 * @param ia                    Instance of (LUA_TUSERDATA(IN_ADDR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage net [, err, msg] = bsd.arpa.inet.inet_netof(ia)
 */
static int
luab_inet_netof(lua_State *L)
{
    luab_module_t *m;
    struct in_addr *ia;
    struct in_addr net;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IN_ADDR, TYPE, __func__);

    ia = luab_udata(L, 1, m, struct in_addr *);
    net.s_addr = inet_netof(*ia);

    return (luab_pushxdata(L, m, &net));
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
 * @usage in_addr [, err, msg ] = bsd.arpa.inet.inet_network(cp)
 */
static int
luab_inet_network(lua_State *L)
{
    luab_module_t *m;
    const char *cp;
    struct in_addr ia;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IN_ADDR, TYPE, __func__);

    cp = luab_checklstring(L, 1, INET_ADDRSTRLEN, NULL);
    ia.s_addr = inet_network(cp);

    return (luab_pushxdata(L, m, &ia));
}

/***
 * inet_net_ntop(3) - Internet address manipulation routines
 *
 * @function inet_net_ntop
 *
 * @param af                    Specifies address fromat over protocol domain(9).
 * @param src                   Instance of (LUA_TUSERDATA(IN{6}_ADDR))
 *                              for binary representation of character string
 *                              denotes OSI-L3 address.
 * @param bits                  Cardinality of bitvector subset of OSI-L3
 *                              address masks network portion from address.
 * @param dst                   Instance of (LUA_TUSERDATA(IOVEC)) for
 *                              character String to be interpreted as address.
 * @param size                  Specifies constraint, size of character string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_net_ntop(af, src, bits, dst, size)
 */
static int
luab_inet_net_ntop(lua_State *L)
{
    luab_module_t *m0, *m1;
    int af;
    void *src;
    int bits;
    luab_iovec_t *buf;
    caddr_t dst;
    size_t size;
    int status;

    (void)luab_core_checkmaxargs(L, 5);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);

    af = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    src = luab_checkxaddr(L, 2, af, &size);
    bits = (int)luab_checkxinteger(L, 3, m0, luab_env_int_max);
    buf = luab_udata(L, 4, m1, luab_iovec_t *);
    size = (size_t)luab_checklinteger(L, 5, 0);

    if (((dst = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (size <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if (inet_net_ntop(af, src, bits, dst, size) != NULL) {
            buf->iov.iov_len = size;
            status = luab_env_success;
        } else
            status = luab_env_error;

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * inet_net_pton(3) - Internet address manipulation routines
 *
 * @function inet_net_pton
 *
 * @param af                    Specifies address fromat over protocol domain(9).
 * @param src                   Instance of (LUA_TUSERDATA(IOVEC)) for
 *                              character String to be interpreted as address.
 * @param dst                   Instance of (LUA_TUSERDATA(IN{6}_ADDR))
 *                              for binary representation of character string
 *                              denotes OSI-L3 address.
 * @param size                  Specifies constraint, size of character string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_net_pton(af, src, dst, size)
 */
static int
luab_inet_net_pton(lua_State *L)
{
    luab_module_t *m0, *m1;
    int af;
    luab_iovec_t *buf;
    void *dst;
    size_t size;
    caddr_t src;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);

    af = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    buf = luab_udata(L, 2, m1, luab_iovec_t *);
    dst = luab_checkxaddr(L, 3, af, &size);
    size = (size_t)luab_checklinteger(L, 4, 0);

    if (((src = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (size <= buf->iov.iov_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);
        status = inet_net_pton(af, src, dst, size);
        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * inet_ntoa_r(3) - Internet address manipulation routines
 *
 * @function inet_ntoa_r
 *
 * @param in                    Instance of (LUA_TUSERDATA(IN_ADDR)).
 * @param buf                   Instance of (LUA_TUSERDATA(IOVEC)) holds a
 *                              character String to be interpreted as address.
 * @param size                  Length of string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_ntoa_r(in, buf, size)
 */
static int
luab_inet_ntoa_r(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    struct in_addr *in;
    luab_iovec_t *buf;
    socklen_t size;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(IN_ADDR, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);
    m2 = luab_xmod(SOCKLEN, TYPE, __func__);

    in = luab_udata(L, 1, m0, struct in_addr *);
    buf = luab_udata(L, 2, m1, luab_iovec_t *);
    size = (size_t)luab_checklxinteger(L, 3, m2, 0);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (size <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if (inet_ntoa_r(*in, bp, size) != NULL) {
            buf->iov.iov_len = size;
            status = luab_env_success;
        } else
            status = luab_env_error;

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * inet_cidr_ntop(3) - Internet address manipulation routines
 *
 * @function inet_cidr_ntop
 *
 * @param af                    Specifies address fromat over protocol domain(9).
 * @param src                   Instance of (LUA_TUSERDATA(IN{6}_ADDR))
 *                              for binary representation of character string
 *                              denotes OSI-L3 address.
 * @param bits                  Cardinality of bitvector subset of OSI-L3
 *                              address masks network portion from address.
 * @param dst                   Instance of (LUA_TUSERDATA(IOVEC)) for
 *                              character String to be interpreted as address.
 * @param size                  Specifies constraint, size of character string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_cidr_ntop(af, src, bits, dst, size)
 */
static int
luab_inet_cidr_ntop(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    int af;
    void *src;
    int bits;
    luab_iovec_t *buf;
    caddr_t dst;
    size_t size;
    int status;

    (void)luab_core_checkmaxargs(L, 5);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);
    m2 = luab_xmod(SIZE, TYPE, __func__);

    af = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    src = luab_checkxaddr(L, 2, af, &size);
    bits = (int)luab_checkxinteger(L, 3, m0, luab_env_uint_max);
    buf = luab_udata(L, 4, m1, luab_iovec_t *);
    size = (size_t)luab_checklxinteger(L, 5, m2, 0);

    if (((dst = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (size <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if (inet_cidr_ntop(af, src, bits, dst, size) != NULL) {
            buf->iov.iov_len = size;
            status = luab_env_success;
        } else
            status = luab_env_error;

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * inet_cidr_pton(3) - Internet address manipulation routines
 *
 * @function inet_cidr_pton
 *
 * @param af                    Specifies address fromat over protocol domain(9).
 * @param src                   Instance of (LUA_TUSERDATA(IOVEC)) for
 *                              character String to be interpreted as address.
 * @param dst                   Instance of (LUA_TUSERDATA(IN{6}_ADDR))
 *                              for binary representation of character string
 *                              denotes OSI-L3 address.
 * @param bits                  Cardinality of bitvector subset of OSI-L3
 *                              address masks network portion from address.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ]  = bsd.arpa.inet.inet_cidr_pton(af, src, dst, bits)
 */
static int
luab_inet_cidr_pton(lua_State *L)
{
    luab_module_t *m0, *m1;
    int af;
    luab_iovec_t *buf;
    void *dst;
    int *bits;
    caddr_t src;
    size_t size;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);

    af = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    buf = luab_udata(L, 2, m1, luab_iovec_t *);
    dst = luab_checkxaddr(L, 3, af, &size);
    bits = luab_udata(L, 4, m0, int *);

    if (((src = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (size <= buf->iov.iov_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);
        status = inet_cidr_pton(af, src, dst, bits);
        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}
#endif /* __BSD_VISIBLE */

/*
 * Generator functions.
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(IN_PORT)).
 *
 * @function create_in_port
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(IN_PORT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage in_port [, err, msg ] = bsd.arpa.inet.create_in_port(arg)
 */
static int
luab_type_create_in_port(lua_State *L)
{
    luab_module_t *m;
    in_port_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IN_PORT, TYPE, __func__);
    x = (in_port_t)luab_checkxinteger(L, 1, m, luab_env_ushrt_max);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(IN_ADDR)).
 *
 * @function create_in_addr
 *
 * @param arg           Instance of (LUA_TUSERDATA(IN_ADDR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage in_addr [, err, msg ] = bsd.arpa.inet.create_in_addr([ arg ])
 */
static int
luab_type_create_in_addr(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IN_ADDR, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(IN6_ADDR)).
 *
 * @function create_in6_addr
 *
 * @param arg           Instance of (LUA_TUSERDATA(IN6_ADDR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage in6_addr [, err, msg ] = bsd.arpa.inet.create_in6_addr([ arg ])
 */
static int
luab_type_create_in6_addr(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IN6_ADDR, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(SOCKADDR)).
 *
 * @function create_sockaddr_in
 *
 * @param port              Specifies port ID, see /etc/services.
 * @param addr              Specifies ip(4) address by instance
 *                          of (LUA_TUSERDATA(IN_ADDR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sockaddr [, err, msg ] = bsd.arpa.inet.create_sockaddr_in([ port [, addr ]])
 */
static int
luab_type_create_sockaddr_in(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    struct sockaddr_in sin;
    struct sockaddr *data;
    struct in_addr *addr;

    m0 = luab_xmod(IN_ADDR, TYPE, __func__);
    m1 = luab_xmod(SHORT, TYPE, __func__);
    m2 = luab_xmod(SOCKADDR, TYPE, __func__);

    data = (struct sockaddr *)&sin;
    luab_sockaddr_pci(data, AF_INET, sizeof(sin));

    switch (luab_core_checkmaxargs(L, 2)) {     /* FALLTHROUGH */
    case 2:
        addr = luab_udata(L, 2, m0, struct in_addr *);
        (void)memmove(&sin.sin_addr, addr, sizeof(sin.sin_addr));
    case 1:
        sin.sin_port = (in_port_t)luab_checkxinteger(L, 1, m1, luab_env_shrt_max);
    default:
        sin.sin_addr.s_addr = htonl(sin.sin_addr.s_addr);
        sin.sin_port = htons(sin.sin_port);
        break;
    }
    return (luab_pushxdata(L, m2, data));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(SOCKADDR)).
 *
 * @function create_sockaddr_in6
 *
 * @param port              Specifies port ID, see /etc/services.
 * @param info              Specifies Flow Label, see RFC6437,.
 * @param addr              Specifies ip(4) address by instance
 *                          of (LUA_TUSERDATA(IN_ADDR)).
 * @param id                Specifies scope ID.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sockaddr [, err, msg ] = bsd.arpa.inet.create_sockaddr_in6([ port [, info [, addr [, id ]]]])
 */
static int
luab_type_create_sockaddr_in6(lua_State *L)
{
    luab_module_t *m0, *m1, *m2, *m3;
    struct sockaddr_in6 sin6;
    struct sockaddr *data;
    struct in6_addr *addr;

    m0 = luab_xmod(UINT32, TYPE, __func__);
    m1 = luab_xmod(IN6_ADDR, TYPE, __func__);
    m2 = luab_xmod(IN_PORT, TYPE, __func__);
    m3 = luab_xmod(SOCKADDR, TYPE, __func__);

    data = (struct sockaddr *)&sin6;
    luab_sockaddr_pci(data, AF_INET6, sizeof(sin6));

    switch (luab_core_checkmaxargs(L, 4)) {     /* FALLTHROUGH */
    case 4:
        sin6.sin6_scope_id = (uint32_t)luab_checkxinteger(L, 4, m0, luab_env_int_max);
    case 3:
        addr = luab_udata(L, 3, m1, struct in6_addr *);
        (void)memmove(&sin6.sin6_addr, addr, sizeof(sin6.sin6_addr));
    case 2:
        sin6.sin6_flowinfo = (uint32_t)luab_checkxinteger(L, 2, m1, luab_env_int_max);
    case 1:
        sin6.sin6_port = (in_port_t)luab_checkxinteger(L, 1, m2, luab_env_shrt_max);
    default:
        sin6.sin6_scope_id = htons(sin6.sin6_scope_id);
        sin6.sin6_flowinfo = htons(sin6.sin6_flowinfo);
        sin6.sin6_port = htons(sin6.sin6_port);
        break;
    }
    return (luab_pushxdata(L, m3, data));
}

/*
 * Interface against <arpa/inet.h>.
 */

static luab_module_table_t luab_arpa_inet_vec[] = {
    LUAB_INT("INET_ADDRSTRLEN",         INET_ADDRSTRLEN),
    LUAB_INT("INET6_ADDRSTRLEN",        INET6_ADDRSTRLEN),
    LUAB_FUNC("inet_addr",              luab_inet_addr),
    LUAB_FUNC("inet_ntoa",              luab_inet_ntoa),
    LUAB_FUNC("inet_ntop",              luab_inet_ntop),
    LUAB_FUNC("inet_pton",              luab_inet_pton),
#if __BSD_VISIBLE
    LUAB_FUNC("inet_aton",              luab_inet_aton),
    LUAB_FUNC("inet_lnaof",             luab_inet_lnaof),
    LUAB_FUNC("inet_makeaddr",          luab_inet_makeaddr),
    LUAB_FUNC("inet_neta",              luab_inet_neta),
    LUAB_FUNC("inet_netof",             luab_inet_netof),
    LUAB_FUNC("inet_network",           luab_inet_network),
    LUAB_FUNC("inet_net_ntop",          luab_inet_net_ntop),
    LUAB_FUNC("inet_net_pton",          luab_inet_net_pton),
    LUAB_FUNC("inet_ntoa_r",            luab_inet_ntoa_r),
    LUAB_FUNC("inet_cidr_ntop",         luab_inet_cidr_ntop),
    LUAB_FUNC("inet_cidr_pton",         luab_inet_cidr_pton),
#if 0
    LUAB_FUNC("inet_nsap_addr",         luab_inet_nsap_addr),
    LUAB_FUNC("inet_nsap_ntoa",         luab_inet_nsap_ntoa),
#endif
#endif /* __BSD_VISIBLE */
    LUAB_FUNC("create_in_port",         luab_type_create_in_port),
    LUAB_FUNC("create_in_addr",         luab_type_create_in_addr),
    LUAB_FUNC("create_in6_addr",        luab_type_create_in6_addr),

    /* XXX wrong, because maps to <netinet/in.h> */
    LUAB_FUNC("create_sockaddr_in",     luab_type_create_sockaddr_in),
    LUAB_FUNC("create_sockaddr_in6",    luab_type_create_sockaddr_in6),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_arpa_inet_lib = {
    .m_id       = LUAB_ARPA_INET_LIB_ID,
    .m_name     = LUAB_ARPA_INET_LIB_KEY,
    .m_vec      = luab_arpa_inet_vec,
};
