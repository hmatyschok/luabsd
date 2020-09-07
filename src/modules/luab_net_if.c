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

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t if_nameindex_type;
extern luab_module_t sockaddr_type;

extern int luab_if_nameindex_create(lua_State *);

#define LUABSD_NET_IF_LIB_ID    1596485465
#define LUABSD_NET_IF_LIB_KEY    "net"

extern luab_module_t luab_net_if_lib;

/***
 * if_indextoname(3) - fetch name from interface by its corrosponding index.
 *
 * @function if_indextoname
 *
 * @param ifindex           Index, see ifntet(9).
 * @param ifname            Empty buffer, LUA_TUSERDATA(luab_iovec_t), but
 *                          set up with length by bsd.net.IFNAMSIZ.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.net.if_indextoname(ifindex)
 */
static int
luab_if_indextoname(lua_State *L)
{
    u_int ifindex;
    luab_iovec_t *buf;
    caddr_t ifname;
    int status;

    (void)luab_checkmaxargs(L, 2);

    ifindex = (u_int)luab_checkinteger(L, 1, INT_MAX);
    buf = luab_udata(L, 2, iovec_type, luab_iovec_t *);

    if ((buf->iov_flags & IOV_LOCK) == 0) {
        buf->iov_flags |= IOV_LOCK;

        if (((ifname = buf->iov.iov_base) != NULL) &&
            (buf->iov_max_len >= IFNAMSIZ) &&
            (buf->iov_flags & IOV_BUFF)) {

            if (if_indextoname(ifindex, ifname) != NULL) {
                buf->iov.iov_len = strnlen(ifname, IFNAMSIZ);
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
 * if_nameindex(3) - build an array maps to set over if_nameindex{}.
 *
 * @function if_nameindex
 *
 * @param ifni              Empty Table, LUA_TTABLE, but filled by
 *                          LUA_TUSERDATA(luab_if_nameindex_t), if
 *                          call was performed successfully.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.net.if_nameindex(ifni)
 */
static int
luab_if_nameindex(lua_State *L)
{
    struct if_nameindex *vec;
    struct if_nameindex *ifni;
    int status;

    (void)luab_checkmaxargs(L, 1);
    (void)luab_checkltable(L, 1, 0);

    if ((vec = if_nameindex()) != NULL) {
        lua_pushnil(L);

        for (ifni = vec; ifni->if_name != NULL; ifni++)
            luab_rawsetudata(L, 1, &if_nameindex_type, ifni->if_index, ifni);

        lua_pop(L, 0);

        free(vec);
        status = 0;
    } else
        status = -1;

    return (luab_pusherr(L, status));

}

/***
 * if_nametoindex(3) - map interface name to its corrosponding index
 *
 * @function if_nametoindex
 *
 * @param ifname            Interface name.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (index [, nil, nil]) on success or
 *          (0, (errno, strerror(errno)))
 *
 * @usage index [, err, msg ] = bsd.net.if_nametoindex(ifname)
 */
static int
luab_if_nametoindex(lua_State *L)
{
    const char *ifname;
    u_int index;

    (void)luab_checkmaxargs(L, 1);

    ifname = luab_checklstring(L, 1, IFNAMSIZ);
    index = if_nametoindex(ifname);

    return (luab_pusherr(L, index));
}

/*
 * Interface against <net/if.h>.
 */

static luab_table_t luab_net_if_vec[] = {
    LUABSD_INT("IF_NAMESIZE",   IF_NAMESIZE),
#if __BSD_VISIBLE
    LUABSD_INT("IFNAMSIZ",  IFNAMSIZ),
    LUABSD_INT("IF_MAXUNIT",    IF_MAXUNIT),
    LUABSD_INT("IFF_UP",    IFF_UP),
    LUABSD_INT("IFF_BROADCAST", IFF_BROADCAST),
    LUABSD_INT("IFF_DEBUG", IFF_DEBUG),
    LUABSD_INT("IFF_LOOPBACK",  IFF_LOOPBACK),
    LUABSD_INT("IFF_POINTOPOINT",   IFF_POINTOPOINT),
    LUABSD_INT("IFF_DRV_RUNNING",   IFF_DRV_RUNNING),
    LUABSD_INT("IFF_NOARP", IFF_NOARP),
    LUABSD_INT("IFF_PROMISC",   IFF_PROMISC),
    LUABSD_INT("IFF_ALLMULTI",  IFF_ALLMULTI),
    LUABSD_INT("IFF_DRV_OACTIVE",   IFF_DRV_OACTIVE),
    LUABSD_INT("IFF_SIMPLEX",   IFF_SIMPLEX),
    LUABSD_INT("IFF_LINK0", IFF_LINK0),
    LUABSD_INT("IFF_LINK1", IFF_LINK1),
    LUABSD_INT("IFF_LINK2", IFF_LINK2),
    LUABSD_INT("IFF_ALTPHYS",   IFF_ALTPHYS),
    LUABSD_INT("IFF_MULTICAST", IFF_MULTICAST),
    LUABSD_INT("IFF_CANTCONFIG",    IFF_CANTCONFIG),
    LUABSD_INT("IFF_PPROMISC",  IFF_PPROMISC),
    LUABSD_INT("IFF_MONITOR",   IFF_MONITOR),
    LUABSD_INT("IFF_STATICARP", IFF_STATICARP),
    LUABSD_INT("IFF_DYING", IFF_DYING),
    LUABSD_INT("IFF_RENAMING",  IFF_RENAMING),
    LUABSD_INT("IFF_NOGROUP",   IFF_NOGROUP),
    LUABSD_INT("IFF_CANTCHANGE",    IFF_CANTCHANGE),
    LUABSD_INT("LINK_STATE_UNKNOWN",    LINK_STATE_UNKNOWN),
    LUABSD_INT("LINK_STATE_DOWN",   LINK_STATE_DOWN),
    LUABSD_INT("LINK_STATE_UP", LINK_STATE_UP),
    LUABSD_INT("IFCAP_RXCSUM",  IFCAP_RXCSUM),
    LUABSD_INT("IFCAP_TXCSUM",  IFCAP_TXCSUM),
    LUABSD_INT("IFCAP_NETCONS", IFCAP_NETCONS),
    LUABSD_INT("IFCAP_VLAN_MTU",    IFCAP_VLAN_MTU),
    LUABSD_INT("IFCAP_VLAN_HWTAGGING",  IFCAP_VLAN_HWTAGGING),
    LUABSD_INT("IFCAP_JUMBO_MTU",   IFCAP_JUMBO_MTU),
    LUABSD_INT("IFCAP_POLLING", IFCAP_POLLING),
    LUABSD_INT("IFCAP_VLAN_HWCSUM", IFCAP_VLAN_HWCSUM),
    LUABSD_INT("IFCAP_TSO4",    IFCAP_TSO4),
    LUABSD_INT("IFCAP_TSO6",    IFCAP_TSO6),
    LUABSD_INT("IFCAP_LRO", IFCAP_LRO),
    LUABSD_INT("IFCAP_WOL_UCAST",   IFCAP_WOL_UCAST),
    LUABSD_INT("IFCAP_WOL_MCAST",   IFCAP_WOL_MCAST),
    LUABSD_INT("IFCAP_WOL_MAGIC",   IFCAP_WOL_MAGIC),
    LUABSD_INT("IFCAP_TOE4",    IFCAP_TOE4),
    LUABSD_INT("IFCAP_TOE6",    IFCAP_TOE6),
    LUABSD_INT("IFCAP_VLAN_HWFILTER",   IFCAP_VLAN_HWFILTER),
    LUABSD_INT("IFCAP_VLAN_HWTSO",  IFCAP_VLAN_HWTSO),
    LUABSD_INT("IFCAP_LINKSTATE",   IFCAP_LINKSTATE),
    LUABSD_INT("IFCAP_NETMAP",  IFCAP_NETMAP),
    LUABSD_INT("IFCAP_RXCSUM_IPV6", IFCAP_RXCSUM_IPV6),
    LUABSD_INT("IFCAP_TXCSUM_IPV6", IFCAP_TXCSUM_IPV6),
    LUABSD_INT("IFCAP_HWSTATS", IFCAP_HWSTATS),
    LUABSD_INT("IFCAP_TXRTLMT", IFCAP_TXRTLMT),
    LUABSD_INT("IFCAP_HWRXTSTMP",   IFCAP_HWRXTSTMP),
    LUABSD_INT("IFCAP_HWCSUM_IPV6", IFCAP_HWCSUM_IPV6),
    LUABSD_INT("IFCAP_HWCSUM",  IFCAP_HWCSUM),
    LUABSD_INT("IFCAP_TSO", IFCAP_TSO),
    LUABSD_INT("IFCAP_WOL", IFCAP_WOL),
    LUABSD_INT("IFCAP_TOE", IFCAP_TOE),
    LUABSD_INT("IFCAP_CANTCHANGE",  IFCAP_CANTCHANGE),
    LUABSD_INT("IFQ_MAXLEN",    IFQ_MAXLEN),
    LUABSD_INT("IFNET_SLOWHZ",  IFNET_SLOWHZ),
    LUABSD_INT("IFAN_ARRIVAL",  IFAN_ARRIVAL),
    LUABSD_INT("IFAN_DEPARTURE",    IFAN_DEPARTURE),
    LUABSD_INT("IFSTATMAX", IFSTATMAX),
    LUABSD_STR("IFG_ALL",   IFG_ALL),
    LUABSD_STR("IFG_EGRESS",    IFG_EGRESS),
    LUABSD_INT("RSS_FUNC_NONE", RSS_FUNC_NONE),
    LUABSD_INT("RSS_FUNC_PRIVATE",  RSS_FUNC_PRIVATE),
    LUABSD_INT("RSS_FUNC_TOEPLITZ", RSS_FUNC_TOEPLITZ),
    LUABSD_INT("RSS_TYPE_IPV4", RSS_TYPE_IPV4),
    LUABSD_INT("RSS_TYPE_TCP_IPV4", RSS_TYPE_TCP_IPV4),
    LUABSD_INT("RSS_TYPE_IPV6", RSS_TYPE_IPV6),
    LUABSD_INT("RSS_TYPE_IPV6_EX",  RSS_TYPE_IPV6_EX),
    LUABSD_INT("RSS_TYPE_TCP_IPV6", RSS_TYPE_TCP_IPV6),
    LUABSD_INT("RSS_TYPE_TCP_IPV6_EX",  RSS_TYPE_TCP_IPV6_EX),
    LUABSD_INT("RSS_TYPE_UDP_IPV4", RSS_TYPE_UDP_IPV4),
    LUABSD_INT("RSS_TYPE_UDP_IPV6", RSS_TYPE_UDP_IPV6),
    LUABSD_INT("RSS_TYPE_UDP_IPV6_EX",  RSS_TYPE_UDP_IPV6_EX),
    LUABSD_INT("RSS_KEYLEN",    RSS_KEYLEN),
    LUABSD_INT("IFNET_PCP_NONE",    IFNET_PCP_NONE),
#endif /* __BSD_VISIBLE */
    LUABSD_FUNC("if_indextoname",   luab_if_indextoname),
    LUABSD_FUNC("if_nameindex", luab_if_nameindex),
    LUABSD_FUNC("if_nametoindex",   luab_if_nametoindex),
    LUABSD_FUNC("if_nameindex_create",    luab_if_nameindex_create),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_net_if_lib = {
    .cookie = LUABSD_NET_IF_LIB_ID,
    .name = LUABSD_NET_IF_LIB_KEY,
    .vec = luab_net_if_vec,
};
