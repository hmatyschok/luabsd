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
 
#include <sys/un.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <netinet/in.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_sockaddr.h"

extern luab_module_t luab_sockaddr_type;

/*
 * Interface against
 *
 *  struct sockaddr {
 *      unsigned char   sa_len;
 *      sa_family_t sa_family;
 *      char        sa_data[14];
 *  };
 *
 * maps to
 *
 *  struct sockaddr_storage {
 *      unsigned char   ss_len;
 *      sa_family_t ss_family;
 *      char        __ss_pad1[_SS_PAD1SIZE];
 *      __int64_t   __ss_align;
 *      char        __ss_pad2[_SS_PAD2SIZE];
 *  };
 *
 * for so called "auto-boxing".
 *
 * XXX
 *  We shall refactor this due to implementing AF_XXX specific table operations.
 */

typedef struct luab_sockaddr {
    luab_udata_t            ud_softc;
    struct sockaddr_storage ud_sa;
} luab_sockaddr_t;

#define luab_new_sockaddr(L, arg) \
    ((luab_sockaddr_t *)luab_newudata(L, &luab_sockaddr_type, (arg)))
#define luab_to_sockaddr(L, narg) \
    ((struct sockaddr *)luab_checksockaddr((L), (narg)))

/*
 * Subr.
 */

static void *
luab_checksockaddr(lua_State *L, int narg)
{
    luab_iovec_t *iov;
    void *v;

    if ((iov = luab_isiovec(L, narg)) != NULL) {
        if (iov->iov.iov_base == NULL)
            luaL_argerror(L, narg, "Invalid argument.");

        if ((iov->iov.iov_len < LUAB_SOCK_MINADDRLEN) ||
            (iov->iov.iov_len > LUAB_SOCK_MAXADDRLEN))
            luaL_argerror(L, narg, "Invalid argument.");

        v = iov->iov.iov_base;
    } else
        v = luab_toudata(L, narg, &luab_sockaddr_type);

    return (v);
}

/*
 * Generator functions.
 */

static void
sockaddr_to_table(lua_State *L, void *arg)
{
    struct sockaddr *sa;
    size_t len;

    sa = (struct sockaddr *)arg;

    lua_newtable(L);

    luab_setinteger(L, -2, "sa_len",    sa->sa_len);
    luab_setinteger(L, -2, "sa_family", sa->sa_family);

    len = sa->sa_len - sizeof(u_char) - sizeof(sa_family_t);
    luab_setldata(L, -2, "sa_data",     sa->sa_data, len);

    lua_pushvalue(L, -1);
}

static void
sockaddr_dl_to_table(lua_State *L, void *arg)
{
    struct sockaddr_dl *sdl;
    size_t len;

    sdl = (struct sockaddr_dl *)arg;

    lua_newtable(L);

    luab_setinteger(L, -2, "sdl_len",       sdl->sdl_len);
    luab_setinteger(L, -2, "sdl_family",    sdl->sdl_family);

    luab_setinteger(L, -2, "sdl_index",     sdl->sdl_index);
    luab_setinteger(L, -2, "sdl_type",      sdl->sdl_type);
    luab_setinteger(L, -2, "sdl_nlen",      sdl->sdl_nlen);
    luab_setinteger(L, -2, "sdl_alen",      sdl->sdl_alen);
    luab_setinteger(L, -2, "sdl_slen",      sdl->sdl_slen);

    len = sdl->sdl_nlen + sdl->sdl_alen + sdl->sdl_slen;
    luab_setldata(L, -2, "sdl_data", sdl->sdl_data, len);

    lua_pushvalue(L, -1);
}

static void
sockaddr_in_to_table(lua_State *L, void *arg)
{
    struct sockaddr_in *sin;
    struct in_addr sin_addr;

    sin = (struct sockaddr_in *)arg;

    lua_newtable(L);

    luab_setinteger(L, -2, "sin_len",       sin->sin_len);
    luab_setinteger(L, -2, "sin_family",    sin->sin_family);
    luab_setinteger(L, -2, "sin_port",      ntohs(sin->sin_port));

    (void)memmove(&sin_addr, &sin->sin_addr, sizeof(sin_addr));
    sin_addr.s_addr = ntohl(sin_addr.s_addr);
    luab_setudata(L, -2, luab_xmod(IN_ADDR, TYPE, __func__), "sin_addr", &sin_addr);

    lua_pushvalue(L, -1);
}

static void
sockaddr_in6_to_table(lua_State *L, void *arg)
{
    struct sockaddr_in6 *sin6;
    struct in6_addr sin6_addr;

    sin6 = (struct sockaddr_in6 *)arg;

    lua_newtable(L);

    luab_setinteger(L, -2, "sin6_len",      sin6->sin6_len);
    luab_setinteger(L, -2, "sin6_family",   sin6->sin6_family);
    luab_setinteger(L, -2, "sin6_port",     ntohs(sin6->sin6_port));
    luab_setinteger(L, -2, "sin6_flowinfo", ntohl(sin6->sin6_flowinfo));

    (void)memmove(&sin6_addr, &sin6->sin6_addr, sizeof(sin6_addr));
    luab_setudata(L, -2, luab_xmod(IN6_ADDR, TYPE, __func__), "sin6_addr", &sin6_addr);

    luab_setinteger(L, -2, "sin6_scope_id", ntohl(sin6->sin6_scope_id));

    lua_pushvalue(L, -1);
}

static void
sockaddr_un_to_table(lua_State *L, void *arg)
{
    struct sockaddr_un *sun;

    sun = (struct sockaddr_un *)arg;

    lua_newtable(L);

    luab_setinteger(L, -2, "sun_len",       sun->sun_len);
    luab_setinteger(L, -2, "sun_family",    sun->sun_family);
    luab_setstring(L, -2, "sun_path",       sun->sun_path);

    lua_pushvalue(L, -1);
}

/***
 * Generator function - translate (LUA_TUSERDATA(SOCKADDR)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *      AF_XXX:
 *
 *          t = {
 *              sa_len          = (LUA_TNUMBER),
 *              sa_family       = (LUA_TNUMBER),
 *              sa_data         = (TUA_TSTRING),
 *          }
 *
 *      AF_INET:
 *
 *          t = {
 *              sin_len         = (LUA_TNUMBER),
 *              sin_family      = (LUA_TNUMBER),
 *              sin_port        = (TUA_TNUMBER),
 *              sin_addr        = (LUA_TUSERDATA(IN_ADDR)),
 *          }
 *
 *      AF_INET6:
 *
 *          t = {
 *              sin6_len        = (LUA_TNUMBER),
 *              sin6_family     = (LUA_TNUMBER),
 *              sin6_port       = (TUA_TNUMBER),
 *              sin6_flowinfo   = (TUA_TNUMBER),
 *              sin6_addr       = (LUA_TUSERDATA(IN6_ADDR)),
 *              sin6_scope_id   = (LUA_TNUMBER),
 *          }
 *
 *      AF_LINK:
 *
 *          t = {
 *              sdl_len         = (LUA_TNUMBER),
 *              sdl_family      = (LUA_TNUMBER),
 *              sdl_index       = (TUA_TNUMBER),
 *              sdl_type        = (TUA_TNUMBER),
 *              sdl_nlen        = (TUA_TNUMBER),
 *              sdl_alen        = (TUA_TNUMBER),
 *              sdl_slen        = (TUA_TNUMBER),
 *              sdl_data        = (LUA_TSTRING),
 *          }
 *
 *      AF_UNIX:
 *
 *          t = {
 *              sun_len         = (LUA_TNUMBER),
 *              sun_family      = (LUA_TNUMBER),
 *              sun_path        = (TUA_TSTRING),
 *          }
 *
 * @usage t = sockaddr:get()
 */
static int
SOCKADDR_get(lua_State *L)
{
    struct sockaddr *sa;

    (void)luab_core_checkmaxargs(L, 1);

    sa = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr *);

    /*
     * XXX replacement by protosw-table.
     */
    switch (sa->sa_family) {
    case AF_UNIX:
        sockaddr_un_to_table(L, sa);
        break;
    case AF_INET:
        sockaddr_in_to_table(L, sa);
        break;
    case AF_INET6:
        sockaddr_in6_to_table(L, sa);
        break;
    case AF_LINK:
        sockaddr_dl_to_table(L, sa);
        break;
    default:
        sockaddr_to_table(L, sa);
        break;
    }
    return (1);
}

/***
 * Generator function - translate sockaddr{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = sockaddr:dump()
 */
static int
SOCKADDR_dump(lua_State *L)
{
    struct sockaddr *sa;

    (void)luab_core_checkmaxargs(L, 1);

    sa = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr *);

    return (luab_core_dump(L, 1, &luab_sockaddr_type, sa->sa_len));
}

/*
 * Access functions for immutable properties.
 */

/***
 * Get value for length.
 *
 * @function sa_len
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage len [, err, msg ] = sockaddr:sa_len()
 */
static int
SOCKADDR_sa_len(lua_State *L)
{
    struct sockaddr *sa;
    uint8_t x;

    (void)luab_core_checkmaxargs(L, 2);

    sa = luab_udata(L, 2, &luab_sockaddr_type, struct sockaddr *);
    x = sa->sa_len;

    return (luab_pushxinteger(L, x));
}

/***
 * Get ID for protocol domain(9).
 *
 * @function sa_family
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage domain [, err, msg ] = sockaddr:sa_family()
 */
static int
SOCKADDR_sa_family(lua_State *L)
{
    struct sockaddr *sa;
    sa_family_t x;

    (void)luab_core_checkmaxargs(L, 1);

    sa = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr *);
    x = sa->sa_family;

    return (luab_pushxinteger(L, x));
}

/*
 * Access functions Link-Level sockaddr., AF_LINK domain(9).
 *
 *  struct sockaddr_dl {
 *      u_char  sdl_len;
 *      u_char  sdl_family;
 *      u_short sdl_index;
 *      u_char  sdl_type;
 *      u_char  sdl_nlen;
 *      u_char  sdl_alen;
 *      u_char  sdl_slen;
 *      char    sdl_data[46];
 *  };
 */

/***
 * Set interface index (Interface Layer, Link Layer).
 *
 * @function set_sdl_index
 *
 * @param index             Specifies interface index, see ifnet(9).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, msg] = sockaddr:set_sdl_index(index)
 */
static int
SOCKADDR_set_sdl_index(lua_State *L)
{
    struct sockaddr_dl *sdl;
    u_short x;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    sdl = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_dl *);
    x = (u_short)luab_checkinteger(L, 2, luab_env_shrt_max);

    if (sdl->sdl_family == AF_LINK) {
        sdl->sdl_index = x;
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * Get interface index (Interface Layer, Link Layer).
 *
 * @function get_sdl_index
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage index [, err, msg ] = sockaddr:get_sdl_index()
 */
static int
SOCKADDR_get_sdl_index(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    sdl = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_dl *);

    if (sdl->sdl_family == AF_LINK)
        x = (int)sdl->sdl_index;
    else {
        errno = EPERM;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
}

/***
 * Set interface type ID (Interface Layer, Link Layer).
 *
 * @function set_sdl_type
 *
 * @param type              Specifies interface type, see net/if_types or RFC1573.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, msg] = sockaddr:set_sdl_type(type)
 */
static int
SOCKADDR_set_sdl_type(lua_State *L)
{
    struct sockaddr_dl *sdl;
    u_char x;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    sdl = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_dl *);
    x = (u_char)luab_checkinteger(L, 2, luab_env_char_max);

    if (sdl->sdl_family == AF_LINK) {
        sdl->sdl_type = x;
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * Get interface type ID (Interface Layer, Link Layer).
 *
 * @function get_sdl_type
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage type [, err, msg ] = sockaddr:get_sdl_type()
 */
static int
SOCKADDR_get_sdl_type(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    sdl = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_dl *);

    if (sdl->sdl_family == AF_LINK)
        x = (int)sdl->sdl_type;
    else {
        errno = EPERM;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
}

/***
 * Set interface name length (Interface Layer, Link Layer).
 *
 * @function set_sdl_nlen
 *
 * @param len               Specifies interface name length.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, msg] = sockaddr:set_sdl_nlen(len)
 */
static int
SOCKADDR_set_sdl_nlen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    u_char x;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    sdl = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_dl *);
    x = (u_char)luab_checkinteger(L, 2, luab_env_char_max);

    if (sdl->sdl_family == AF_LINK) {
        sdl->sdl_nlen = (x % luab_env_ifname_max);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * Get interface name length (Interface Layer, Link Layer) over AF_LINK domain(9).
 *
 * @function get_sdl_nlen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage len [, err, msg ] = sockaddr:get_sdl_nlen()
 */
static int
SOCKADDR_get_sdl_nlen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    sdl = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_dl *);

    if (sdl->sdl_family == AF_LINK)
        x = (int)sdl->sdl_nlen;
    else {
        errno = EPERM;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
}

/***
 * Set link level adress length.
 *
 * @function set_sdl_alen
 *
 * @param len               Specifies link level adress length.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, msg] = sockaddr:set_sdl_alen(alen)
 */
static int
SOCKADDR_set_sdl_alen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    u_char x;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    sdl = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_dl *);
    x = (u_char)luab_checkinteger(L, 2, luab_env_char_max);

    if (sdl->sdl_family == AF_LINK) {    /* XXX constraint depends on IFT_XXX */
        sdl->sdl_alen = (x % LUAB_SDL_MAXADDRLEN);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * Get link level adress length.
 *
 * @function get_sdl_alen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage len [, err, msg ] = sockaddr:get_sdl_alen()
 */
static int
SOCKADDR_get_sdl_alen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    sdl = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_dl *);

    if (sdl->sdl_family == AF_LINK)
        x = (int)sdl->sdl_alen;
    else {
        errno = EPERM;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
}

/***
 * Get link level selector length.
 *
 * @function sdl_slen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage slen = sockaddr:sdl_slen()
 */
static int
SOCKADDR_sdl_slen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    sdl = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_dl *);

    if (sdl->sdl_family == AF_LINK)
        x = (int)sdl->sdl_slen;
    else {
        errno = EPERM;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
}

/*
 * Socket address, internet style.
 *
 *  struct sockaddr_in {
 *      uint8_t sin_len;
 *      sa_family_t sin_family;
 *      in_port_t   sin_port;
 *      struct  in_addr sin_addr;
 *      char    sin_zero[8];
 *  };
 *
 * Conversion between network / host byteorder takes place implicitely.
 */

/***
 * Set ID for SAP at OSI-L4 (Socket Layer, Transport Layer) over ip(4) domain(9).
 *
 * @function set_sin_port
 *
 * @param port              Specifies port ID, see /etc/services.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, msg] = sockaddr:set_sin_port(port)
 */
static int
SOCKADDR_set_sin_port(lua_State *L)
{
    struct sockaddr_in *sin;
    in_port_t x;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    sin = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_in *);
    x = (in_port_t)luab_checkinteger(L, 2, luab_env_shrt_max);

    if (sin->sin_family == AF_INET) {
        sin->sin_port = htons(x);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * Get ID for SAP at OSI-L4 (Socket Layer, Transport Layer) over ip(4) domain(9).
 *
 * @function get_sin_port
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage port [, err, msg ] = sockaddr:get_sin_port()
 */
static int
SOCKADDR_get_sin_port(lua_State *L)
{
    struct sockaddr_in *sin;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    sin = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_in *);

    if (sin->sin_family == AF_INET)
        x = (int)ntohs(sin->sin_port);
    else {
        errno = EPERM;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
}

/***
 * Set ID for SAP at OSI-L3 (Protocol Layer, Network Layer) over ip(4) domain(9).
 *
 * @function set_sin_addr
 *
 * @param addr              Specifies ip(4) address by instance
 *                          of (LUA_TUSERDATA(IN_ADDR)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = sockaddr:set_sin_addr(addr)
 */
static int
SOCKADDR_set_sin_addr(lua_State *L)
{
    struct sockaddr_in *sin;
    struct in_addr *ia;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    sin = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_in *);
    ia = luab_udata(L, 2, luab_xmod(IN_ADDR, TYPE, __func__), struct in_addr *);

    if (sin->sin_family == AF_INET) {
        (void)memmove(&sin->sin_addr, ia, sizeof(struct in_addr));
        sin->sin_addr.s_addr = htonl(sin->sin_addr.s_addr);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * Get ID for SAP at OSI-L3 (Protocol Layer, Network Layer) over ip(4) domain(9).
 *
 * @function get_sin_addr
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage addr [, err, msg ] = sockaddr:get_sin_addr()
 */
static int
SOCKADDR_get_sin_addr(lua_State *L)
{
    struct sockaddr_in *sin;
    struct in_addr ia;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    sin = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_in *);

    if (sin->sin_family == AF_INET) {
        (void)memmove(&ia, &sin->sin_addr, sizeof(struct in_addr));
        ia.s_addr = ntohl(ia.s_addr);
        status = luab_pushudata(L, luab_xmod(IN_ADDR, TYPE, __func__), &ia);
    } else {
        errno = EPERM;
        status = luab_pushnil(L);
    }
    return (status);
}

/*
 * Socket address for inet6(4) domain(9).
 *
 *  struct sockaddr_in6 {
 *      uint8_t     sin6_len;
 *      sa_family_t sin6_family;
 *      in_port_t   sin6_port;
 *      uint32_t    sin6_flowinfo;
 *      struct in6_addr sin6_addr;
 *      uint32_t    sin6_scope_id;
 *  };
 *
 * Implicit conversion between network / host byteorder.
 */

/***
 * Set ID for SAP at OSI-L4 (Socket Layer, Transport Layer).
 *
 * @function set_sin6_port
 *
 * @param port              Specifies port ID, see /etc/services.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, msg] = sockaddr:set_sin6_port(port)
 */
static int
SOCKADDR_set_sin6_port(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    in_port_t x;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    sin6 = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_in6 *);
    x = (in_port_t)luab_checkinteger(L, 2, luab_env_shrt_max);

    if (sin6->sin6_family == AF_INET6) {
        sin6->sin6_port = htons(x);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * Get ID for SAP at OSI-L4 (Socket Layer, Transport Layer).
 *
 * @function get_sin6_port
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage port [, err, msg ] = sockaddr:get_sin6_port()
 */
static int
SOCKADDR_get_sin6_port(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    sin6 = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_in6 *);

    if (sin6->sin6_family == AF_INET6)
        x = (int)ntohs(sin6->sin6_port);
    else {
        errno = EPERM;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
}

/***
 * Set inet6(4) Flow Label.
 *
 * @function set_sin6_flowinfo
 *
 * @param info              Specifies Flow Label, see RFC6437.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, msg] = sockaddr:set_sin6_flowinfo(flowinfo)
 */
static int
SOCKADDR_set_sin6_flowinfo(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    uint32_t x;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    sin6 = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_in6 *);
    x = (uint32_t)luab_checkinteger(L, 2, luab_env_int_max);

    if (sin6->sin6_family == AF_INET6) {
        sin6->sin6_flowinfo = htonl(x);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * Get inet6(4) Flow Label, RFC6437.
 *
 * @function get_sin6_flowinfo
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage flowinfo [, err, msg ] = sockaddr:get_sin6_flowinfo()
 */
static int
SOCKADDR_get_sin6_flowinfo(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    long x;

    (void)luab_core_checkmaxargs(L, 1);

    sin6 = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_in6 *);

    if (sin6->sin6_family == AF_INET6)
        x = ntohl(sin6->sin6_flowinfo);
    else {
        errno = EPERM;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
}

/***
 * Set ID for SAP at OSI-L3 (Protocol Layer, Network Layer).
 *
 * @function set_sin6_addr
 *
 * @param addr              Specifies ip6(4) address by instance
 *                          of LUA_TUSERDATA(luab_in6_addr_t).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = sockaddr:set_sin6_addr(addr)
 */
static int
SOCKADDR_set_sin6_addr(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    struct in6_addr *ia;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    sin6 = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_in6 *);
    ia = luab_udata(L, 2, luab_xmod(IN6_ADDR, TYPE, __func__), struct in6_addr *);

    if (sin6->sin6_family == AF_INET6) {
        (void)memmove(&sin6->sin6_addr, ia, sizeof(struct in6_addr));
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * Get ID for SAP at OSI-L3 (Protocol Layer, Network Layer).
 *
 * @function get_sin6_addr
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage addr [, err, msg ] = sockaddr:get_sin6_addr()
 */
static int
SOCKADDR_get_sin6_addr(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    struct in6_addr ia;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    sin6 = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_in6 *);

    if (sin6->sin6_family == AF_INET6) {
        (void)memmove(&ia, &sin6->sin6_addr, sizeof(struct in6_addr));
        status = luab_pushudata(L, luab_xmod(IN6_ADDR, TYPE, __func__), &ia);
    } else {
        errno = EPERM;
        status = luab_pushnil(L);
    }
    return (status);
}

/***
 * Set inet6(4) scope ID.
 *
 * @function set_sin6_scope_id
 *
 * @param id                Specifies scope ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, msg] = sockaddr:set_sin6_scope_id(id)
 */
static int
SOCKADDR_set_sin6_scope_id(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    uint32_t x;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    sin6 = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_in6 *);
    x = (uint32_t)luab_checkinteger(L, 2, luab_env_int_max);

    if (sin6->sin6_family == AF_INET6) {
        sin6->sin6_scope_id = htonl(x);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * Get inet6(4) zone index.
 *
 * @function get_sin6_scope_id
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage id [, err, msg ] = sockaddr:get_sin6_scope_id()
 */
static int
SOCKADDR_get_sin6_scope_id(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    long x;

    (void)luab_core_checkmaxargs(L, 1);

    sin6 = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_in6 *);

    if (sin6->sin6_family == AF_INET6)
        x = ntohl(sin6->sin6_scope_id);
    else {
        errno = EPERM;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
}

/*
 * Socket address for UNIX IPC domain.
 *
 *  struct sockaddr_un {
 *      unsigned char   sun_len;
 *      sa_family_t sun_family;
 *      char    sun_path[104];
 *  };
 */

/***
 * Set path.
 *
 * @function set_sun_path
 *
 * @param path              Specifies path for socket(9).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = sockaddr:set_sun_path(path)
 */
static int
SOCKADDR_set_sun_path(lua_State *L)
{
    struct sockaddr_un *sun;
    const char *dp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    sun = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_un *);
    dp = luab_checklstring(L, 1, LUAB_SUN_MAXPATHLEN, NULL);

    if (sun->sun_family == AF_UNIX) {
        (void)memmove(sun->sun_path, dp, strlen(dp));
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * Get path.
 *
 * @function get_sun_path
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage path [, err, msg ] = sockaddr:get_sun_path()
 */
static int
SOCKADDR_get_sun_path(lua_State *L)
{
    struct sockaddr_un *sun;
    caddr_t dp;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    sun = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr_un *);

    if (sun->sun_family == AF_UNIX) {
        dp = sun->sun_path;
        status = luab_pushstring(L, dp);
    } else {
        errno = EPERM;
        status = luab_pushnil(L);
    }
    return (status);
}

/*
 * Metamethods.
 */

static int
SOCKADDR_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &luab_sockaddr_type));
}

static int
SOCKADDR_len(lua_State *L)
{
    struct sockaddr *sa;

    (void)luab_core_checkmaxargs(L, 1);

    sa = luab_udata(L, 1, &luab_sockaddr_type, struct sockaddr *);

    return (luab_pushxinteger(L, sa->sa_len));
}

static int
SOCKADDR_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_sockaddr_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t sockaddr_methods[] = {
    LUAB_FUNC("sa_len",               SOCKADDR_sa_len),
    LUAB_FUNC("sa_family",            SOCKADDR_sa_family),
    LUAB_FUNC("sdl_slen",             SOCKADDR_sdl_slen),
    LUAB_FUNC("set_sdl_index",        SOCKADDR_set_sdl_index),
    LUAB_FUNC("set_sdl_type",         SOCKADDR_set_sdl_type),
    LUAB_FUNC("set_sdl_nlen",         SOCKADDR_set_sdl_nlen),
    LUAB_FUNC("set_sdl_alen",         SOCKADDR_set_sdl_alen),
    LUAB_FUNC("set_sin_port",         SOCKADDR_set_sin_port),
    LUAB_FUNC("set_sin_addr",         SOCKADDR_set_sin_addr),
    LUAB_FUNC("set_sin6_port",        SOCKADDR_set_sin6_port),
    LUAB_FUNC("set_sin6_flowinfo",    SOCKADDR_set_sin6_flowinfo),
    LUAB_FUNC("set_sin6_addr",        SOCKADDR_set_sin6_addr),
    LUAB_FUNC("set_sin6_scope_id",    SOCKADDR_set_sin6_scope_id),
    LUAB_FUNC("set_sun_path",         SOCKADDR_set_sun_path),
    LUAB_FUNC("get",                  SOCKADDR_get),
    LUAB_FUNC("get_sdl_index",        SOCKADDR_get_sdl_index),
    LUAB_FUNC("get_sdl_type",         SOCKADDR_get_sdl_type),
    LUAB_FUNC("get_sdl_nlen",         SOCKADDR_get_sdl_nlen),
    LUAB_FUNC("get_sdl_alen",         SOCKADDR_get_sdl_alen),
    LUAB_FUNC("get_sin_port",         SOCKADDR_get_sin_port),
    LUAB_FUNC("get_sin_addr",         SOCKADDR_get_sin_addr),
    LUAB_FUNC("get_sin6_port",        SOCKADDR_get_sin6_port),
    LUAB_FUNC("get_sin6_flowinfo",    SOCKADDR_get_sin6_flowinfo),
    LUAB_FUNC("get_sin6_addr",        SOCKADDR_get_sin6_addr),
    LUAB_FUNC("get_sin6_scope_id",    SOCKADDR_get_sin6_scope_id),
    LUAB_FUNC("get_sun_path",         SOCKADDR_get_sun_path),
    LUAB_FUNC("dump",                 SOCKADDR_dump),
    LUAB_FUNC("__gc",                 SOCKADDR_gc),
    LUAB_FUNC("__len",                SOCKADDR_len),
    LUAB_FUNC("__tostring",           SOCKADDR_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
sockaddr_create(lua_State *L, void *arg)
{
    return (luab_new_sockaddr(L, arg));
}

static void
sockaddr_init(void *ud, void *arg)
{
    luab_sockaddr_t *self;
    struct sockaddr *sa;

    if (((self = (luab_sockaddr_t *)ud) != NULL) &&
        ((sa = (struct sockaddr *)arg) != NULL))
        (void)memmove(&self->ud_sa, sa, sa->sa_len);
}

static void *
sockaddr_udata(lua_State *L, int narg)
{
    return (luab_to_sockaddr(L, narg));
}

luab_module_t luab_sockaddr_type = {
    .m_cookie   = LUAB_SOCKADDR_TYPE_ID,
    .m_name     = LUAB_SOCKADDR_TYPE,
    .m_vec      = sockaddr_methods,
    .m_create   = sockaddr_create,
    .m_init     = sockaddr_init,
    .m_get      = sockaddr_udata,
    .m_sz       = sizeof(luab_sockaddr_t),
};
