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
#include <net/if.h>
#include <net/if_dl.h>

#include <errno.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t in_addr_type;
extern luab_module_t in6_addr_type;
extern luab_module_t sockaddr_type;

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
 */

typedef struct luab_sockaddr {
    struct sockaddr_storage sockaddr;
} luab_sockaddr_t;

#define luab_new_sockaddr(L, arg) \
    ((luab_sockaddr_t *)luab_newuserdata(L, &sockaddr_type, (arg)))
#define luab_to_sockaddr(L, narg) \
    (luab_todata((L), (narg), &sockaddr_type, luab_sockaddr_t *))

#define LUABSD_SOCKADDR_TYPE_ID    1595755513
#define LUABSD_SOCKADDR_TYPE   "SOCKADDR*"

int luab_StructSockAddr(lua_State *);
int luab_StructSockAddrDL(lua_State *);
int luab_StructSockAddrIn(lua_State *);

/*
 * Subr.
 */

static int
sockaddr_pci(struct sockaddr *sa, sa_family_t af, uint8_t len)
{
    (void)memset_s(sa, len, 0, len);

    sa->sa_len = len;
    sa->sa_family = af;

    return 1;
}

static void
sockaddr_to_table(lua_State *L, void *arg)
{
    struct sockaddr *sa;
    size_t len;

    sa = (struct sockaddr *)arg;

    lua_newtable(L);

    luab_setinteger(L, -2, "sa_len", sa->sa_len);
    luab_setinteger(L, -2, "sa_family", sa->sa_family);

    len = sa->sa_len - sizeof(u_char) - sizeof(sa_family_t);
    luab_setbuff(L, -2, "sa_data", sa->sa_data, len);

    lua_pushvalue(L, -1);
}

static void
sockaddr_dl_to_table(lua_State *L, void *arg)
{
    struct sockaddr_dl *sdl;
    size_t len;

    sdl = (struct sockaddr_dl *)arg;

    lua_newtable(L);

    luab_setinteger(L, -2, "sdl_len", sdl->sdl_len);
    luab_setinteger(L, -2, "sdl_family", sdl->sdl_family);

    luab_setinteger(L, -2, "sdl_index", sdl->sdl_index);
    luab_setinteger(L, -2, "sdl_type", sdl->sdl_type);
    luab_setinteger(L, -2, "sdl_nlen", sdl->sdl_nlen);
    luab_setinteger(L, -2, "sdl_alen", sdl->sdl_alen);
    luab_setinteger(L, -2, "sdl_slen", sdl->sdl_slen);

    len = sdl->sdl_nlen + sdl->sdl_alen + sdl->sdl_slen;
    luab_setbuff(L, -2, "sdl_data", sdl->sdl_data, len);

    lua_pushvalue(L, -1);
}

static void
sockaddr_in_to_table(lua_State *L, void *arg)
{
    struct sockaddr_in *sin;
    struct in_addr sin_addr;

    sin = (struct sockaddr_in *)arg;

    lua_newtable(L);

    luab_setinteger(L, -2, "sin_len", sin->sin_len);
    luab_setinteger(L, -2, "sin_family", sin->sin_family);
    luab_setinteger(L, -2, "sin_port", ntohs(sin->sin_port));

    (void)memmove(&sin_addr, &sin->sin_addr, sizeof(sin_addr));
    sin_addr.s_addr = ntohl(sin_addr.s_addr);
    luab_setudata(L, -2, &in_addr_type, "sin_addr", &sin_addr);

    lua_pushvalue(L, -1);
}

/***
 * Get value for length.
 *
 * @function sa_len
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (len [, nil, nil]) on success or
 *          (len, (errno, strerror(errno)))
 *
 * @usage len [, err, msg ] = sockaddr:sa_len()
 */
static int
SockAddr_sa_len(lua_State *L)
{
    struct sockaddr *sa;
    uint8_t sa_len;

    (void)luab_checkmaxargs(L, 1);

    sa = (struct sockaddr *)(*sockaddr_type.get)(L, 1);
    sa_len = sa->sa_len;

    return luab_pusherr(L, sa_len);
}

/***
 * Get ID for protocol domain(9).
 *
 * @function sa_family
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (af [, nil, nil]) on success or
 *          (af, (errno, strerror(errno)))
 *
 * @usage af [, err, msg ] = sockaddr:sa_family()
 */
static int
SockAddr_sa_family(lua_State *L)
{
    struct sockaddr *sa;
    sa_family_t sa_family;

    (void)luab_checkmaxargs(L, 1);

    sa = (struct sockaddr *)(*sockaddr_type.get)(L, 1);
    sa_family = sa->sa_family;

    return luab_pusherr(L, sa_family);
}

/***
 * Translate (generic) socket address into LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = sockaddr:get()
 */
static int
SockAddr_get(lua_State *L)
{
    struct sockaddr *sa;

    (void)luab_checkmaxargs(L, 1);

    sa = (struct sockaddr *)(*sockaddr_type.get)(L, 1);

    /*
     * SockAddr This switch-statement should be replaced by protosw-table.
     */
    switch (sa->sa_family) {
    case AF_INET:
        sockaddr_in_to_table(L, sa);
        break;
    case AF_LINK:
        sockaddr_dl_to_table(L, sa);
        break;
    default:
        sockaddr_to_table(L, sa);
        break;
    }
    return 1;
}

/*
 * Interface against Link-Level sockaddr., AF_LINK domain(9).
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
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, msg] = sockaddr:set_sdl_index(index)
 */
static int
SockAddr_set_sdl_index(lua_State *L)
{
    struct sockaddr_dl *sdl;
    u_short sdl_index;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sdl = (struct sockaddr_dl *)(*sockaddr_type.get)(L, 1);
    sdl_index = (u_short)luab_checkinteger(L, 2, SHRT_MAX);

    if (sdl->sdl_family == AF_LINK) {
        sdl->sdl_index = sdl_index;
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return luab_pusherr(L, status);
}

/***
 * Get interface index (Interface Layer, Link Layer).
 *
 * @function get_sdl_index
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (index [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage index [, err, msg ] = sockaddr:get_sdl_index()
 */
static int
SockAddr_get_sdl_index(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int sdl_index;

    (void)luab_checkmaxargs(L, 1);

    sdl = (struct sockaddr_dl *)(*sockaddr_type.get)(L, 1);

    if (sdl->sdl_family == AF_LINK)
        sdl_index = sdl->sdl_index;
    else {
        errno = EPERM;
        sdl_index = -1;
    }
    return luab_pusherr(L, sdl_index);
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
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, msg] = sockaddr:set_sdl_type(type)
 */
static int
SockAddr_set_sdl_type(lua_State *L)
{
    struct sockaddr_dl *sdl;
    u_char sdl_type;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sdl = (struct sockaddr_dl *)(*sockaddr_type.get)(L, 1);
    sdl_type = (u_char)luab_checkinteger(L, 2, CHAR_MAX);

    if (sdl->sdl_family == AF_LINK) {
        sdl->sdl_type = sdl_type;
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return luab_pusherr(L, status);
}

/***
 * Get interface type ID (Interface Layer, Link Layer).
 *
 * @function get_sdl_type
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (type [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage type [, err, msg ] = sockaddr:get_sdl_type()
 */
static int
SockAddr_get_sdl_type(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int sdl_type;

    (void)luab_checkmaxargs(L, 1);

    sdl = (struct sockaddr_dl *)(*sockaddr_type.get)(L, 1);

    if (sdl->sdl_family == AF_LINK)
        sdl_type = sdl->sdl_type;
    else {
        errno = EPERM;
        sdl_type = -1;
    }
    return luab_pusherr(L, sdl_type);
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
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, msg] = sockaddr:set_sdl_nlen(len)
 */
static int
SockAddr_set_sdl_nlen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    u_char sdl_nlen;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sdl = (struct sockaddr_dl *)(*sockaddr_type.get)(L, 1);
    sdl_nlen = (u_char)luab_checkinteger(L, 2, CHAR_MAX);

    if (sdl->sdl_family == AF_LINK) {
        sdl->sdl_nlen = sdl_nlen % IFNAMSIZ;
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return luab_pusherr(L, status);
}

/***
 * Get interface name length (Interface Layer, Link Layer) over AF_LINK domain(9).
 *
 * @function get_sdl_nlen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (len [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage len [, err, msg ] = sockaddr:get_sdl_nlen()
 */
static int
SockAddr_get_sdl_nlen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int sdl_nlen;

    (void)luab_checkmaxargs(L, 1);

    sdl = (struct sockaddr_dl *)(*sockaddr_type.get)(L, 1);

    if (sdl->sdl_family == AF_LINK)
        sdl_nlen = sdl->sdl_nlen;
    else {
        errno = EPERM;
        sdl_nlen = -1;
    }
    return luab_pusherr(L, sdl_nlen);
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
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, msg] = sockaddr:set_sdl_alen(alen)
 */
static int
SockAddr_set_sdl_alen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    u_char sdl_alen;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sdl = (struct sockaddr_dl *)(*sockaddr_type.get)(L, 1);
    sdl_alen = (u_char)luab_checkinteger(L, 2, CHAR_MAX);

    if (sdl->sdl_family == AF_LINK) {    /* XXX constraint depends on IFT_XXX */
        sdl->sdl_alen = sdl_alen % SDL_ADDR_MAX_LEN;
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return luab_pusherr(L, status);
}

/***
 * Get link level adress length.
 *
 * @function get_sdl_alen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (len [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage len [, err, msg ] = sockaddr:get_sdl_alen()
 */
static int
SockAddr_get_sdl_alen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int sdl_alen;

    (void)luab_checkmaxargs(L, 1);

    sdl = (struct sockaddr_dl *)(*sockaddr_type.get)(L, 1);

    if (sdl->sdl_family == AF_LINK)
        sdl_alen = sdl->sdl_alen;
    else {
        errno = EPERM;
        sdl_alen = -1;
    }
    return luab_pusherr(L, sdl_alen);
}

/***
 * Get link level selector length.
 *
 * @function sdl_slen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (slen [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage slen = sockaddr:sdl_slen()
 */
static int
SockAddr_sdl_slen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int sdl_slen;

    (void)luab_checkmaxargs(L, 1);

    sdl = (struct sockaddr_dl *)(*sockaddr_type.get)(L, 1);

    if (sdl->sdl_family == AF_LINK)
        sdl_slen = sdl->sdl_slen;
    else {
        errno = EPERM;
        sdl_slen = -1;
    }
    return luab_pusherr(L, sdl_slen);
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
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, msg] = sockaddr:set_sin_port(port)
 */
static int
SockAddr_set_sin_port(lua_State *L)
{
    struct sockaddr_in *sin;
    in_port_t sin_port;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sin = (struct sockaddr_in *)(*sockaddr_type.get)(L, 1);
    sin_port = (in_port_t)luab_checkinteger(L, 2, SHRT_MAX);

    if (sin->sin_family == AF_INET) {
        sin->sin_port = htons(sin_port);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return luab_pusherr(L, status);
}

/***
 * Get ID for SAP at OSI-L4 (Socket Layer, Transport Layer) over ip(4) domain(9).
 *
 * @function get_sin_port
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (port [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage port [, err, msg ] = sockaddr:get_sin_port()
 */
static int
SockAddr_get_sin_port(lua_State *L)
{
    struct sockaddr_in *sin;
    int sin_port;

    (void)luab_checkmaxargs(L, 1);

    sin = (struct sockaddr_in *)(*sockaddr_type.get)(L, 1);

    if (sin->sin_family == AF_INET)
        sin_port = ntohs(sin->sin_port);
    else {
        errno = EPERM;
        sin_port = -1;
    }
    return luab_pusherr(L, sin_port);
}

/***
 * Set ID for SAP at OSI-L3 (Protocol Layer, Network Layer) over ip(4) domain(9).
 *
 * @function set_sin_addr
 *
 * @param addr              Specifies ip(4) address by instance
 *                          of LUA_TUSERDATA(luab_in_addr_t).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = sockaddr:set_sin_addr(addr)
 */
static int
SockAddr_set_sin_addr(lua_State *L)
{
    struct sockaddr_in *sin;
    struct in_addr *sin_addr;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sin = (struct sockaddr_in *)(*sockaddr_type.get)(L, 1);
    sin_addr = (struct in_addr *)(*in_addr_type.get)(L, 2);

    if (sin->sin_family == AF_INET) {
        (void)memmove(&sin->sin_addr, sin_addr, sizeof(*sin_addr));
        sin->sin_addr.s_addr = htonl(sin->sin_addr.s_addr);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return luab_pusherr(L, status);
}

/***
 * Get ID for SAP at OSI-L3 (Protocol Layer, Network Layer) over ip(4) domain(9).
 *
 * @function get_sin_addr
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (addr [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage addr [, err, msg ] = sockaddr:get_sin_addr()
 */
static int
SockAddr_get_sin_addr(lua_State *L)
{
    struct sockaddr_in *sin;
    struct in_addr sin_addr;
    int status;

    (void)luab_checkmaxargs(L, 1);

    sin = (struct sockaddr_in *)(*sockaddr_type.get)(L, 1);

    if (sin->sin_family == AF_INET) {
        (void)memmove(&sin_addr, &sin->sin_addr, sizeof(sin_addr));
        sin_addr.s_addr = ntohl(sin_addr.s_addr);

        if ((*in_addr_type.ctor)(L, &sin_addr) == NULL)
            status = luab_pushnil(L);
        else
            status = 1;
    } else {
        errno = EPERM;
        status = luab_pushnil(L);
    }
    return status;
}

static int
SockAddr_gc(lua_State *L)
{
    luab_sockaddr_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_sockaddr(L, 1);

    (void)memset_s(self, sockaddr_type.sz, 0, sockaddr_type.sz);

    return 0;
}

static int
SockAddr_tostring(lua_State *L)
{
    luab_sockaddr_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_sockaddr(L, 1);
    lua_pushfstring(L, "sockaddr (%p)", self);

    return 1;
}

static luab_table_t sockaddr_methods[] = {
    LUABSD_FUNC("sa_len",   SockAddr_sa_len),
    LUABSD_FUNC("sa_family",  SockAddr_sa_family),
    LUABSD_FUNC("sdl_slen", SockAddr_sdl_slen),
    LUABSD_FUNC("set_sdl_index",    SockAddr_set_sdl_index),
    LUABSD_FUNC("set_sdl_type",    SockAddr_set_sdl_type),
    LUABSD_FUNC("set_sdl_nlen",    SockAddr_set_sdl_nlen),
    LUABSD_FUNC("set_sdl_alen",    SockAddr_set_sdl_alen),
    LUABSD_FUNC("set_sin_port", SockAddr_set_sin_port),
    LUABSD_FUNC("set_sin_addr", SockAddr_set_sin_addr),
    LUABSD_FUNC("get",  SockAddr_get),
    LUABSD_FUNC("get_sdl_index",    SockAddr_get_sdl_index),
    LUABSD_FUNC("get_sdl_type",    SockAddr_get_sdl_type),
    LUABSD_FUNC("get_sdl_nlen",    SockAddr_get_sdl_nlen),
    LUABSD_FUNC("get_sdl_alen",    SockAddr_get_sdl_alen),
    LUABSD_FUNC("get_sin_port", SockAddr_get_sin_port),
    LUABSD_FUNC("get_sin_addr", SockAddr_get_sin_addr),
    LUABSD_FUNC("__gc", SockAddr_gc),
    LUABSD_FUNC("__tostring",   SockAddr_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
sockaddr_create(lua_State *L, void *arg)
{
    return luab_new_sockaddr(L, arg);
}

static void
sockaddr_init(void *ud, void *arg)
{
    luab_sockaddr_t *self = (luab_sockaddr_t *)ud;
    struct sockaddr *sa = (struct sockaddr *)arg;

    (void)memmove(&self->sockaddr, sa, sa->sa_len);
}

static void *
sockaddr_udata(lua_State *L, int narg)
{
    luab_sockaddr_t *self = luab_to_sockaddr(L, narg);

    return (&self->sockaddr);
}

luab_module_t sockaddr_type = {
    .cookie = LUABSD_SOCKADDR_TYPE_ID,
    .name = LUABSD_SOCKADDR_TYPE,
    .vec = sockaddr_methods,
    .ctor = sockaddr_create,
    .init = sockaddr_init,
    .get = sockaddr_udata,
    .sz = sizeof(luab_sockaddr_t),
};

/***
 * Generic ctor.
 *
 * @function StructSockAddr
 *
 * @param sockaddr          Template, LUA_TUSERDATA(luab_sockaddr_t).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sockaddr_in [, err, msg ]= bsd.sys.socket.StructSockAddr(sockaddr)
 */
int
luab_StructSockAddr(lua_State *L)
{
    struct sockaddr *sa;
    int status;

    (void)luab_checkmaxargs(L, 1);

    sa = (struct sockaddr *)sockaddr_udata(L, 1);

    if (sockaddr_create(L, sa) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}

/***
 * Ctor for sockaddr_dl{}.
 *
 * @function StructSockAddrDL
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sockaddr_dl [, err, msg ]= bsd.net.if_dl.StructSockAddrDL()
 */
int
luab_StructSockAddrDL(lua_State *L)
{
    struct sockaddr_dl sdl;
    struct sockaddr *sa;
    int status;

    (void)luab_checkmaxargs(L, 0);

    sa = (struct sockaddr *)&sdl;
    sockaddr_pci(sa, AF_LINK, sizeof(sdl));

    if (sockaddr_create(L, sa) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}

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
 * @usage sockaddr_in [, err, msg ]= bsd.arpa.inet.StructSockAddrIn(port, addr)
 */
int
luab_StructSockAddrIn(lua_State *L)
{
    struct sockaddr_in sin;
    struct sockaddr *sa;
    in_port_t port;
    struct in_addr *addr;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sa = (struct sockaddr *)&sin;
    sockaddr_pci(sa, AF_INET, sizeof(sin));

    port = (in_port_t)luab_checkinteger(L, 1, SHRT_MAX);
    addr = (struct in_addr *)(*in_addr_type.get)(L, 2);

    sin.sin_port = htons(port);

    (void)memmove(&sin.sin_addr, addr, sizeof(sin.sin_addr));

    sin.sin_addr.s_addr = htonl(sin.sin_addr.s_addr);

    if (sockaddr_create(L, sa) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}
