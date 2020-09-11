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
#include <sys/un.h>

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

#define SUN_MAX_PATH    103

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
    luab_udata_t        ud_softc;
    struct sockaddr_storage sockaddr;
} luab_sockaddr_t;

#define luab_new_sockaddr(L, arg) \
    ((luab_sockaddr_t *)luab_newuserdata(L, &sockaddr_type, (arg)))
#define luab_to_sockaddr(L, narg) \
    (luab_toudata((L), (narg), &sockaddr_type))

#define LUABSD_SOCKADDR_TYPE_ID    1595755513
#define LUABSD_SOCKADDR_TYPE   "SOCKADDR*"

int luab_sockaddr_dl_create(lua_State *L);
int luab_sockaddr_un_create(lua_State *L);

int luab_sockaddr_in_create(lua_State *L);
int luab_sockaddr_in6_create(lua_State *L);

/*
 * Subr.
 */

static int
sockaddr_pci(struct sockaddr *sa, sa_family_t af, uint8_t len)
{
    (void)memset_s(sa, len, 0, len);

    sa->sa_len = len;
    sa->sa_family = af;

    return (1);
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
    luab_setldata(L, -2, "sa_data", sa->sa_data, len);

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

    luab_setinteger(L, -2, "sin_len", sin->sin_len);
    luab_setinteger(L, -2, "sin_family", sin->sin_family);
    luab_setinteger(L, -2, "sin_port", ntohs(sin->sin_port));

    (void)memmove(&sin_addr, &sin->sin_addr, sizeof(sin_addr));
    sin_addr.s_addr = ntohl(sin_addr.s_addr);
    luab_setudata(L, -2, &in_addr_type, "sin_addr", &sin_addr);

    lua_pushvalue(L, -1);
}

static void
sockaddr_in6_to_table(lua_State *L, void *arg)
{
    struct sockaddr_in6 *sin6;
    struct in6_addr sin6_addr;

    sin6 = (struct sockaddr_in6 *)arg;

    lua_newtable(L);

    luab_setinteger(L, -2, "sin6_len", sin6->sin6_len);
    luab_setinteger(L, -2, "sin6_family", sin6->sin6_family);
    luab_setinteger(L, -2, "sin6_port", ntohs(sin6->sin6_port));
    luab_setinteger(L, -2, "sin6_flowinfo", ntohl(sin6->sin6_flowinfo));

    (void)memmove(&sin6_addr, &sin6->sin6_addr, sizeof(sin6_addr));
    luab_setudata(L, -2, &in6_addr_type, "sin6_addr", &sin6_addr);

    luab_setinteger(L, -2, "sin6_scope_id", ntohl(sin6->sin6_scope_id));

    lua_pushvalue(L, -1);
}

static void
sockaddr_un_to_table(lua_State *L, void *arg)
{
    struct sockaddr_un *sun;

    sun = (struct sockaddr_un *)arg;

    lua_newtable(L);

    luab_setinteger(L, -2, "sun_len", sun->sun_len);
    luab_setinteger(L, -2, "sun_family", sun->sun_family);
    luab_setstring(L, -2, "sun_path", sun->sun_path);

    lua_pushvalue(L, -1);
}

/*
 * Generator functions.
 */

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

    (void)luab_checkmaxargs(L, 1);

    sa = luab_udata(L, 1, sockaddr_type, struct sockaddr *);

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
int
luab_sockaddr_dl_create(lua_State *L)
{
    struct sockaddr_dl sdl;
    struct sockaddr *data;

    (void)luab_checkmaxargs(L, 0);

    data = (struct sockaddr *)&sdl;
    sockaddr_pci(data, AF_LINK, sizeof(sdl));

    return (luab_pushudata(L, &sockaddr_type, data));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(SOCKADDR)).
 *
 * @function sockaddr_in_create
 *
 * @param port              Specifies port ID, see /etc/services.
 * @param addr              Specifies ip(4) address by instance
 *                          of (LUA_TUSERDATA(IN_ADDR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (sockaddr_in [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage sockaddr_in [, err, msg ] = bsd.arpa.inet.sockaddr_in_create([ port [, addr ]])
 */
int
luab_sockaddr_in_create(lua_State *L)
{
    struct sockaddr_in sin;
    struct sockaddr *data;
    struct in_addr *addr;

    data = (struct sockaddr *)&sin;
    sockaddr_pci(data, AF_INET, sizeof(sin));

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
    return (luab_pushudata(L, &sockaddr_type, data));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(SOCKADDR)).
 *
 * @function sockaddr_in6_create
 *
 * @param port              Specifies port ID, see /etc/services.
 * @param info              Specifies Flow Label, see RFC6437,.
 * @param addr              Specifies ip(4) address by instance
 *                          of (LUA_TUSERDATA(IN_ADDR)).
 * @param id                Specifies scope ID.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (sockaddr_in6 [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage sockaddr_in6 [, err, msg ] = bsd.arpa.inet.sockaddr_in6_create([ port [, info [, addr [, id ]]]])
 */
int
luab_sockaddr_in6_create(lua_State *L)
{
    struct sockaddr_in6 sin6;
    struct sockaddr *data;
    struct in6_addr *addr;

    data = (struct sockaddr *)&sin6;
    sockaddr_pci(data, AF_INET6, sizeof(sin6));

    switch (luab_checkmaxargs(L, 4)) {     /* FALLTHROUGH */
    case 4:
        sin6.sin6_scope_id = (uint32_t)luab_checkinteger(L, 4, INT_MAX);
    case 3:
        addr = luab_udata(L, 3, in6_addr_type, struct in6_addr *);
        (void)memmove(&sin6.sin6_addr, addr, sizeof(sin6.sin6_addr));
    case 2:
        sin6.sin6_flowinfo = (uint32_t)luab_checkinteger(L, 2, INT_MAX);
    case 1:
        sin6.sin6_port = (in_port_t)luab_checkinteger(L, 1, SHRT_MAX);
    default:
        sin6.sin6_scope_id = htons(sin6.sin6_scope_id);
        sin6.sin6_flowinfo = htons(sin6.sin6_flowinfo);
        sin6.sin6_port = htons(sin6.sin6_port);
        break;
    }
    return (luab_pushudata(L, &sockaddr_type, data));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(SOCKADDR)).
 *
 * @function sockaddr_un_create
 *
 * @param path              Specifies path or filename.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (sockaddr_un [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage sockaddr_un [, err, msg ] = bsd.sys.socket.sockaddr_un_create([ path ])
 */
int
luab_sockaddr_un_create(lua_State *L)
{
    struct sockaddr_un sun;
    struct sockaddr *data;
    const char *sun_path;

    data = (struct sockaddr *)&sun;
    sockaddr_pci(data, AF_UNIX, sizeof(sun));

    switch (luab_checkmaxargs(L, 1)) {     /* FALLTHROUGH */
    case 1:
        sun_path = luab_checklstring(L, 1, SUN_MAX_PATH);
        (void)memmove(sun.sun_path, sun_path, strlen(sun_path));
    default:
        break;
    }
    return (luab_pushudata(L, &sockaddr_type, data));
}

/*
 * Accessor for immutable properties.
 */

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
SOCKADDR_sa_len(lua_State *L)
{
    struct sockaddr *sa;
    uint8_t sa_len;

    (void)luab_checkmaxargs(L, 1);

    sa = luab_udata(L, 1, sockaddr_type, struct sockaddr *);
    sa_len = sa->sa_len;

    return (luab_pusherr(L, sa_len));
}

/***
 * Get ID for protocol domain(9).
 *
 * @function sa_family
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (domain [, nil, nil]) on success or
 *          (domain, (errno, strerror(errno)))
 *
 * @usage domain [, err, msg ] = sockaddr:sa_family()
 */
static int
SOCKADDR_sa_family(lua_State *L)
{
    struct sockaddr *sa;
    sa_family_t sa_family;

    (void)luab_checkmaxargs(L, 1);

    sa = luab_udata(L, 1, sockaddr_type, struct sockaddr *);
    sa_family = sa->sa_family;

    return (luab_pusherr(L, sa_family));
}

/*
 * Accessor Link-Level sockaddr., AF_LINK domain(9).
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
SOCKADDR_set_sdl_index(lua_State *L)
{
    struct sockaddr_dl *sdl;
    u_short sdl_index;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sdl = luab_udata(L, 1, sockaddr_type, struct sockaddr_dl *);
    sdl_index = (u_short)luab_checkinteger(L, 2, SHRT_MAX);

    if (sdl->sdl_family == AF_LINK) {
        sdl->sdl_index = sdl_index;
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pusherr(L, status));
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
SOCKADDR_get_sdl_index(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int sdl_index;

    (void)luab_checkmaxargs(L, 1);

    sdl = luab_udata(L, 1, sockaddr_type, struct sockaddr_dl *);

    if (sdl->sdl_family == AF_LINK)
        sdl_index = (int)sdl->sdl_index;
    else {
        errno = EPERM;
        sdl_index = -1;
    }
    return (luab_pusherr(L, sdl_index));
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
SOCKADDR_set_sdl_type(lua_State *L)
{
    struct sockaddr_dl *sdl;
    u_char sdl_type;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sdl = luab_udata(L, 1, sockaddr_type, struct sockaddr_dl *);
    sdl_type = (u_char)luab_checkinteger(L, 2, CHAR_MAX);

    if (sdl->sdl_family == AF_LINK) {
        sdl->sdl_type = sdl_type;
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pusherr(L, status));
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
SOCKADDR_get_sdl_type(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int sdl_type;

    (void)luab_checkmaxargs(L, 1);

    sdl = luab_udata(L, 1, sockaddr_type, struct sockaddr_dl *);

    if (sdl->sdl_family == AF_LINK)
        sdl_type = (int)sdl->sdl_type;
    else {
        errno = EPERM;
        sdl_type = -1;
    }
    return (luab_pusherr(L, sdl_type));
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
SOCKADDR_set_sdl_nlen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    u_char sdl_nlen;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sdl = luab_udata(L, 1, sockaddr_type, struct sockaddr_dl *);
    sdl_nlen = (u_char)luab_checkinteger(L, 2, CHAR_MAX);

    if (sdl->sdl_family == AF_LINK) {
        sdl->sdl_nlen = sdl_nlen % IFNAMSIZ;
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pusherr(L, status));
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
SOCKADDR_get_sdl_nlen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int sdl_nlen;

    (void)luab_checkmaxargs(L, 1);

    sdl = luab_udata(L, 1, sockaddr_type, struct sockaddr_dl *);

    if (sdl->sdl_family == AF_LINK)
        sdl_nlen = (int)sdl->sdl_nlen;
    else {
        errno = EPERM;
        sdl_nlen = -1;
    }
    return (luab_pusherr(L, sdl_nlen));
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
SOCKADDR_set_sdl_alen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    u_char sdl_alen;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sdl = luab_udata(L, 1, sockaddr_type, struct sockaddr_dl *);
    sdl_alen = (u_char)luab_checkinteger(L, 2, CHAR_MAX);

    if (sdl->sdl_family == AF_LINK) {    /* XXX constraint depends on IFT_XXX */
        sdl->sdl_alen = sdl_alen % SDL_ADDR_MAX_LEN;
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pusherr(L, status));
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
SOCKADDR_get_sdl_alen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int sdl_alen;

    (void)luab_checkmaxargs(L, 1);

    sdl = luab_udata(L, 1, sockaddr_type, struct sockaddr_dl *);

    if (sdl->sdl_family == AF_LINK)
        sdl_alen = (int)sdl->sdl_alen;
    else {
        errno = EPERM;
        sdl_alen = -1;
    }
    return (luab_pusherr(L, sdl_alen));
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
SOCKADDR_sdl_slen(lua_State *L)
{
    struct sockaddr_dl *sdl;
    int sdl_slen;

    (void)luab_checkmaxargs(L, 1);

    sdl = luab_udata(L, 1, sockaddr_type, struct sockaddr_dl *);

    if (sdl->sdl_family == AF_LINK)
        sdl_slen = (int)sdl->sdl_slen;
    else {
        errno = EPERM;
        sdl_slen = -1;
    }
    return (luab_pusherr(L, sdl_slen));
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
SOCKADDR_set_sin_port(lua_State *L)
{
    struct sockaddr_in *sin;
    in_port_t sin_port;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sin = luab_udata(L, 1, sockaddr_type, struct sockaddr_in *);
    sin_port = (in_port_t)luab_checkinteger(L, 2, SHRT_MAX);

    if (sin->sin_family == AF_INET) {
        sin->sin_port = htons(sin_port);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pusherr(L, status));
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
SOCKADDR_get_sin_port(lua_State *L)
{
    struct sockaddr_in *sin;
    int sin_port;

    (void)luab_checkmaxargs(L, 1);

    sin = luab_udata(L, 1, sockaddr_type, struct sockaddr_in *);

    if (sin->sin_family == AF_INET)
        sin_port = ntohs(sin->sin_port);
    else {
        errno = EPERM;
        sin_port = -1;
    }
    return (luab_pusherr(L, sin_port));
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
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = sockaddr:set_sin_addr(addr)
 */
static int
SOCKADDR_set_sin_addr(lua_State *L)
{
    struct sockaddr_in *sin;
    struct in_addr *sin_addr;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sin = luab_udata(L, 1, sockaddr_type, struct sockaddr_in *);
    sin_addr = luab_udata(L, 2, in_addr_type, struct in_addr *);

    if (sin->sin_family == AF_INET) {
        (void)memmove(&sin->sin_addr, sin_addr, sizeof(*sin_addr));
        sin->sin_addr.s_addr = htonl(sin->sin_addr.s_addr);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pusherr(L, status));
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
SOCKADDR_get_sin_addr(lua_State *L)
{
    struct sockaddr_in *sin;
    struct in_addr sin_addr;
    int status;

    (void)luab_checkmaxargs(L, 1);

    sin = luab_udata(L, 1, sockaddr_type, struct sockaddr_in *);

    if (sin->sin_family == AF_INET) {
        (void)memmove(&sin_addr, &sin->sin_addr, sizeof(sin_addr));
        sin_addr.s_addr = ntohl(sin_addr.s_addr);

        if ((*in_addr_type.create)(L, &sin_addr) == NULL)
            status = luab_pushnil(L);
        else
            status = 1;
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
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, msg] = sockaddr:set_sin6_port(port)
 */
static int
SOCKADDR_set_sin6_port(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    in_port_t sin6_port;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sin6 = luab_udata(L, 1, sockaddr_type, struct sockaddr_in6 *);
    sin6_port = (in_port_t)luab_checkinteger(L, 2, SHRT_MAX);

    if (sin6->sin6_family == AF_INET6) {
        sin6->sin6_port = htons(sin6_port);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * Get ID for SAP at OSI-L4 (Socket Layer, Transport Layer).
 *
 * @function get_sin6_port
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (port [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage port [, err, msg ] = sockaddr:get_sin6_port()
 */
static int
SOCKADDR_get_sin6_port(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    int sin6_port;

    (void)luab_checkmaxargs(L, 1);

    sin6 = luab_udata(L, 1, sockaddr_type, struct sockaddr_in6 *);

    if (sin6->sin6_family == AF_INET6)
        sin6_port = ntohs(sin6->sin6_port);
    else {
        errno = EPERM;
        sin6_port = -1;
    }
    return (luab_pusherr(L, sin6_port));
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
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, msg] = sockaddr:set_sin6_flowinfo(flowinfo)
 */
static int
SOCKADDR_set_sin6_flowinfo(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    uint32_t sin6_flowinfo;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sin6 = luab_udata(L, 1, sockaddr_type, struct sockaddr_in6 *);
    sin6_flowinfo = (uint32_t)luab_checkinteger(L, 2, INT_MAX);

    if (sin6->sin6_family == AF_INET6) {
        sin6->sin6_flowinfo = htonl(sin6_flowinfo);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * Get inet6(4) Flow Label, RFC6437.
 *
 * @function get_sin6_flowinfo
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (flowinfo [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage flowinfo [, err, msg ] = sockaddr:get_sin6_flowinfo()
 */
static int
SOCKADDR_get_sin6_flowinfo(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    long sin6_flowinfo;

    (void)luab_checkmaxargs(L, 1);

    sin6 = luab_udata(L, 1, sockaddr_type, struct sockaddr_in6 *);

    if (sin6->sin6_family == AF_INET6)
        sin6_flowinfo = ntohl(sin6->sin6_flowinfo);
    else {
        errno = EPERM;
        sin6_flowinfo = -1;
    }
    return (luab_pusherr(L, sin6_flowinfo));
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
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = sockaddr:set_sin6_addr(addr)
 */
static int
SOCKADDR_set_sin6_addr(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    struct in6_addr *sin6_addr;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sin6 = luab_udata(L, 1, sockaddr_type, struct sockaddr_in6 *);
    sin6_addr = luab_udata(L, 2, in6_addr_type, struct in6_addr *);

    if (sin6->sin6_family == AF_INET6) {
        (void)memmove(&sin6->sin6_addr, sin6_addr, sizeof(*sin6_addr));
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * Get ID for SAP at OSI-L3 (Protocol Layer, Network Layer).
 *
 * @function get_sin6_addr
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (addr [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage addr [, err, msg ] = sockaddr:get_sin6_addr()
 */
static int
SOCKADDR_get_sin6_addr(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    struct in6_addr sin6_addr;
    int status;

    (void)luab_checkmaxargs(L, 1);

    sin6 = luab_udata(L, 1, sockaddr_type, struct sockaddr_in6 *);

    if (sin6->sin6_family == AF_INET6) {
        (void)memmove(&sin6_addr, &sin6->sin6_addr, sizeof(sin6_addr));

        if ((*in6_addr_type.create)(L, &sin6_addr) == NULL)
            status = luab_pushnil(L);
        else
            status = 1;
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
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, msg] = sockaddr:set_sin6_scope_id(id)
 */
static int
SOCKADDR_set_sin6_scope_id(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    uint32_t sin6_scope_id;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sin6 = luab_udata(L, 1, sockaddr_type, struct sockaddr_in6 *);
    sin6_scope_id = (uint32_t)luab_checkinteger(L, 2, INT_MAX);

    if (sin6->sin6_family == AF_INET6) {
        sin6->sin6_scope_id = htonl(sin6_scope_id);
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * Get inet6(4) zone index.
 *
 * @function get_sin6_scope_id
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (id [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage id [, err, msg ] = sockaddr:get_sin6_scope_id()
 */
static int
SOCKADDR_get_sin6_scope_id(lua_State *L)
{
    struct sockaddr_in6 *sin6;
    long sin6_scope_id;

    (void)luab_checkmaxargs(L, 1);

    sin6 = luab_udata(L, 1, sockaddr_type, struct sockaddr_in6 *);

    if (sin6->sin6_family == AF_INET6)
        sin6_scope_id = ntohl(sin6->sin6_scope_id);
    else {
        errno = EPERM;
        sin6_scope_id = -1;
    }
    return (luab_pusherr(L, sin6_scope_id));
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
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = sockaddr:set_sun_path(path)
 */
static int
SOCKADDR_set_sun_path(lua_State *L)
{
    struct sockaddr_un *sun;
    const char *sun_path;
    int status;

    (void)luab_checkmaxargs(L, 2);

    sun = luab_udata(L, 1, sockaddr_type, struct sockaddr_un *);
    sun_path = luab_checklstring(L, 1, SUN_MAX_PATH);

    if (sun->sun_family == AF_UNIX) {
        (void)memmove(sun->sun_path, sun_path, strlen(sun_path));
        status = 0;
    } else {
        errno = EPERM;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * Get path.
 *
 * @function get_sun_path
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (path [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage path [, err, msg ] = sockaddr:get_sun_path()
 */
static int
SOCKADDR_get_sun_path(lua_State *L)
{
    struct sockaddr_un *sun;
    char *sun_path;
    int status;

    (void)luab_checkmaxargs(L, 1);

    sun = luab_udata(L, 1, sockaddr_type, struct sockaddr_un *);

    if (sun->sun_family == AF_UNIX) {
        sun_path = sun->sun_path;
        status = luab_pushstring(L, sun_path);
    } else {
        errno = EPERM;
        status = luab_pushnil(L);
    }
    return (status);
}

/*
 * Meta-methods.
 */

static int
SOCKADDR_gc(lua_State *L)
{
    return (luab_gc(L, 1, &sockaddr_type));
}

static int
SOCKADDR_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &sockaddr_type));
}

/*
 * Internal interface.
 */

static luab_table_t sockaddr_methods[] = {
    LUABSD_FUNC("sa_len",   SOCKADDR_sa_len),
    LUABSD_FUNC("sa_family",  SOCKADDR_sa_family),
    LUABSD_FUNC("sdl_slen", SOCKADDR_sdl_slen),
    LUABSD_FUNC("set_sdl_index",    SOCKADDR_set_sdl_index),
    LUABSD_FUNC("set_sdl_type",    SOCKADDR_set_sdl_type),
    LUABSD_FUNC("set_sdl_nlen",    SOCKADDR_set_sdl_nlen),
    LUABSD_FUNC("set_sdl_alen",    SOCKADDR_set_sdl_alen),
    LUABSD_FUNC("set_sin_port", SOCKADDR_set_sin_port),
    LUABSD_FUNC("set_sin_addr", SOCKADDR_set_sin_addr),
    LUABSD_FUNC("set_sin6_port",    SOCKADDR_set_sin6_port),
    LUABSD_FUNC("set_sin6_flowinfo",    SOCKADDR_set_sin6_flowinfo),
    LUABSD_FUNC("set_sin6_addr",    SOCKADDR_set_sin6_addr),
    LUABSD_FUNC("set_sin6_scope_id",    SOCKADDR_set_sin6_scope_id),
    LUABSD_FUNC("set_sun_path", SOCKADDR_set_sun_path),
    LUABSD_FUNC("get",  SOCKADDR_get),
    LUABSD_FUNC("get_sdl_index",    SOCKADDR_get_sdl_index),
    LUABSD_FUNC("get_sdl_type",    SOCKADDR_get_sdl_type),
    LUABSD_FUNC("get_sdl_nlen",    SOCKADDR_get_sdl_nlen),
    LUABSD_FUNC("get_sdl_alen",    SOCKADDR_get_sdl_alen),
    LUABSD_FUNC("get_sin_port", SOCKADDR_get_sin_port),
    LUABSD_FUNC("get_sin_addr", SOCKADDR_get_sin_addr),
    LUABSD_FUNC("get_sin6_port",    SOCKADDR_get_sin6_port),
    LUABSD_FUNC("get_sin6_flowinfo",    SOCKADDR_get_sin6_flowinfo),
    LUABSD_FUNC("get_sin6_addr",    SOCKADDR_get_sin6_addr),
    LUABSD_FUNC("get_sin6_scope_id",    SOCKADDR_get_sin6_scope_id),
    LUABSD_FUNC("get_sun_path", SOCKADDR_get_sun_path),
    LUABSD_FUNC("__gc", SOCKADDR_gc),
    LUABSD_FUNC("__tostring",   SOCKADDR_tostring),
    LUABSD_FUNC(NULL, NULL)
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
        (void)memmove(&self->sockaddr, sa, sa->sa_len);
}

static void *
sockaddr_udata(lua_State *L, int narg)
{
    return (luab_to_sockaddr(L, narg));
}

luab_module_t sockaddr_type = {
    .cookie = LUABSD_SOCKADDR_TYPE_ID,
    .name = LUABSD_SOCKADDR_TYPE,
    .vec = sockaddr_methods,
    .create = sockaddr_create,
    .init = sockaddr_init,
    .get = sockaddr_udata,
    .sz = sizeof(luab_sockaddr_t),
};
