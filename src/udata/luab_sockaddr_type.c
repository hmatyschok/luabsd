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
 * by
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

#define luab_newsockaddr(L, arg) \
    ((luab_sockaddr_t *)luab_newuserdata(L, &sockaddr_type, (arg)))
#define luab_to_sockaddr(L, narg) \
    (luab_todata((L), (narg), &sockaddr_type, luab_sockaddr_t *))

#define LUABSD_SOCKADDR_TYPE_ID    1595755513
#define LUABSD_SOCKADDR_TYPE   "SOCKADDR*"

int luab_StructSockAddr(lua_State *);
int luab_StructSockAddrIn(lua_State *);

/*
 * Subr.
 */
static int
sockaddr_pci(struct sockaddr *sa, uint8_t len, sa_family_t af)
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

    sa = (struct sockaddr *)arg;

    lua_newtable(L);

    luab_setinteger(L, -2, "sa_len", sa->sa_len);
    luab_setinteger(L, -2, "sa_family", sa->sa_family);

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
 * @return (LUA_TNUMBER)
 *
 * @usage sa_len = sockaddr:sa_len()
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
 * @function get_sa_family
 *
 * @return (LUA_TNUMBER)
 *
 * @usage sa_family; = sockaddr:get_sa_family()
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

    /* XXX This switch-satement will be replaced by protosw-table. */
    switch (sa->sa_family) {
    case AF_INET:
        sockaddr_in_to_table(L, sa);
        break;
    default:
        sockaddr_to_table(L, sa);
        break;
    }
    return 1;
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
 * Conversion between byteorder takes place automatically.
 */

/***
 * Set ID for SAP at OSI-L4 (Socket Layer, Transport Layer) over ip(4) domain(9).
 *
 * @function set_sin_port
 *
 * @param sin_port              Specifies port ID, see /etc/services.
 *
 * @usage sockaddr:get_sin_port(sin_port)
 */
static int
SockAddr_set_sin_port(lua_State *L)
{
    struct sockaddr_in *sin;
    in_port_t sin_port;

    (void)luab_checkmaxargs(L, 2);

    sin = (struct sockaddr_in *)(*sockaddr_type.get)(L, 1);
    sin_port = (in_port_t)luab_checkinteger(L, 2, SHRT_MAX);

    sin->sin_port = htons(sin_port);

    return 0;
}

/***
 * Get ID for SAP at OSI-L4 (Socket Layer, Transport Layer) over ip(4) domain(9).
 *
 * @function get_sin_port
 *
 * @return (LUA_TNUMBER)
 *
 * @usage sin_port = sockaddr:get_sin_port()
 */
static int
SockAddr_get_sin_port(lua_State *L)
{
    struct sockaddr_in *sin;
    in_port_t sin_port;

    (void)luab_checkmaxargs(L, 1);

    sin = (struct sockaddr_in *)(*sockaddr_type.get)(L, 1);

    sin_port = ntohs(sin->sin_port);

    return luab_pusherr(L, sin_port);
}

/***
 * Set ID for SAP at OSI-L3 (Protocol Layer, Network Layer) over ip(4) domain(9).
 *
 * @function set_sin_addr
 *
 * @param sin_addr                  Specifies ip(4) address by instance
 *                                  of LUA_TUSERDATA(luab_in_addr_t).
 *
 * @usage sockaddr:set_sin_addr(sin_addr)
 */
static int
SockAddr_set_sin_addr(lua_State *L)
{
    struct sockaddr_in *sin;
    struct in_addr *sin_addr;

    (void)luab_checkmaxargs(L, 2);

    sin = (struct sockaddr_in *)(*sockaddr_type.get)(L, 1);
    sin_addr = (struct in_addr *)(*in_addr_type.get)(L, 2);

    (void)memmove(&sin->sin_addr, &sin_addr, sizeof(*sin_addr));

    sin->sin_addr.s_addr = htonl(sin->sin_addr.s_addr);

    return 0;
}

/***
 * Get ID for SAP at OSI-L3 (Protocol Layer, Network Layer) over ip(4) domain(9).
 *
 * @function get_sin_addr
 *
 * @return (LUA_TUSERDATA)
 *
 * @usage sin_addr = sockaddr:get_sin_addr()
 */
static int
SockAddr_get_sin_addr(lua_State *L)
{
    struct sockaddr_in *sin;
    struct in_addr sin_addr;
    int status;

    (void)luab_checkmaxargs(L, 1);

    sin = (struct sockaddr_in *)(*sockaddr_type.get)(L, 1);

    (void)memmove(&sin_addr, &sin->sin_addr, sizeof(sin_addr));

    sin_addr.s_addr = ntohl(sin_addr.s_addr);

    if (luab_newuserdata(L, &in_addr_type, &sin_addr) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

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
    LUABSD_FUNC("set_sin_port", SockAddr_set_sin_port),
    LUABSD_FUNC("set_sin_addr", SockAddr_set_sin_addr),
    LUABSD_FUNC("get",  SockAddr_get),
    LUABSD_FUNC("get_sin_port", SockAddr_get_sin_port),
    LUABSD_FUNC("get_sin_addr", SockAddr_get_sin_addr),
    LUABSD_FUNC("__gc", SockAddr_gc),
    LUABSD_FUNC("__tostring",   SockAddr_tostring),
    LUABSD_FUNC(NULL, NULL)
};

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
    .init = sockaddr_init,
    .get = sockaddr_udata,
    .sz = sizeof(luab_sockaddr_t),
};


/***
 * Generic ctor.
 *
 * @function StructSockAddrIn
 *
 * @param sockaddr              Template, LUA_TUSERDATA(luab_sockaddr_t).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage sockaddr_in = bsd.sys.socket.StructSockAddr(sockaddr)
 */
int
luab_StructSockAddr(lua_State *L)
{
    struct sockaddr *sa;
    int status;

    (void)luab_checkmaxargs(L, 1);

    sa = (struct sockaddr *)sockaddr_udata(L, 1);

    if (luab_newsockaddr(L, sa) == NULL)
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
 * @param port                  Specifies port ID, see /etc/services.
 * @param addr                   Specifies ip(4) address by instance
 *                                  of LUA_TUSERDATA(luab_in_addr_t).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage sockaddr_in = bsd.arpa.inet.StructSockAddrIn(in_port, in_addr)
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
    sockaddr_pci(sa, sizeof(sin), AF_INET);

    port = (in_port_t)luab_checkinteger(L, 1, SHRT_MAX);
    addr = (struct in_addr *)(*in_addr_type.get)(L, 2);

    sin.sin_port = htons(port);

    (void)memmove(&sin.sin_addr, addr, sizeof(sin.sin_addr));

    sin.sin_addr.s_addr = htonl(sin.sin_addr.s_addr);

    if (luab_newsockaddr(L, sa) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return status;
}
