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

#include <net/if.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#define LUAB_NET_IF_LIB_ID    1596485465
#define LUAB_NET_IF_LIB_KEY    "net"

extern luab_module_t luab_net_if_lib;

/*
 * Service primitives.
 */

/***
 * if_indextoname(3) - fetch name from interface by its corrosponding index.
 *
 * @function if_indextoname
 *
 * @param ifindex           Index, see ifnet(9) for further details.
 * @param ifname            Empty buffer, (LUA_TUSERDATA(IOVEC)), but
 *                          set up with length by bsd.net.IFNAMSIZ.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.net.if_indextoname(ifindex)
 */
static int
luab_if_indextoname(lua_State *L)
{
    luab_module_t *m0, *m1;
    u_int ifindex;
    luab_iovec_t *buf;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(UINT, TYPE, __func__);
    m1 = luab_xmod(IOVEC, TYPE, __func__);

    ifindex = (u_int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    buf = luab_udata(L, 2, m1, luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (IFNAMSIZ <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if (if_indextoname(ifindex, bp) != NULL) {
            buf->iov.iov_len = strnlen(bp, IFNAMSIZ);
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
 * if_nameindex(3) - build an array maps to set over if_nameindex{}.
 *
 * @function if_nameindex
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.net.if_nameindex(ifni)
 */
static int
luab_if_nameindex(lua_State *L)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct if_nameindex *vec;
    struct if_nameindex *ifni;
    size_t card;

    (void)luab_core_checkmaxargs(L, 0);

    m = luab_xmod(IF_NAMEINDEX, TYPE, __func__);

    if ((vec = if_nameindex()) != NULL) {

        ifni = vec;
        do {
            card = ifni->if_index;
            ifni++;
        } while (ifni->if_name != NULL);

        if ((tbl = (*m->m_alloc_tbl)(vec, card)) != NULL)
            luab_table_pushxdata(L, -2, m, tbl, 1, 1);

        if_freenameindex(vec);
    } else
        errno = (errno != 0) ? errno : ERANGE;

    return (luab_table_pusherr(L, errno, 1));
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
 * @usage index [, err, msg ] = bsd.net.if_nametoindex(ifname)
 */
static int
luab_if_nametoindex(lua_State *L)
{
    const char *ifname;
    u_int index;

    (void)luab_core_checkmaxargs(L, 1);

    ifname = luab_checklstring(L, 1, luab_env_ifname_max, NULL);
    index = if_nametoindex(ifname);

    return (luab_pushxinteger(L, index));
}

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(IF_NAMEINDEX)).
 *
 * @function create_if_nameindex
 *
 * @param arg           Instance of (LUA_TUSERDATA(IF_NAMEINDEX)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage if_nameindex [, err, msg ] = bsd.net.create_if_nameindex([ arg ])
 */
static int
luab_type_create_if_nameindex(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IF_NAMEINDEX, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/*
 * Interface against <net/if.h>.
 */

static luab_module_table_t luab_net_if_vec[] = {
    LUAB_INT("IF_NAMESIZE",           IF_NAMESIZE),
#if __BSD_VISIBLE
    LUAB_INT("IFNAMSIZ",              IFNAMSIZ),
    LUAB_INT("IF_MAXUNIT",            IF_MAXUNIT),
    LUAB_INT("IFF_UP",                IFF_UP),
    LUAB_INT("IFF_BROADCAST",         IFF_BROADCAST),
    LUAB_INT("IFF_DEBUG",             IFF_DEBUG),
    LUAB_INT("IFF_LOOPBACK",          IFF_LOOPBACK),
    LUAB_INT("IFF_POINTOPOINT",       IFF_POINTOPOINT),
    LUAB_INT("IFF_DRV_RUNNING",       IFF_DRV_RUNNING),
    LUAB_INT("IFF_NOARP",             IFF_NOARP),
    LUAB_INT("IFF_PROMISC",           IFF_PROMISC),
    LUAB_INT("IFF_ALLMULTI",          IFF_ALLMULTI),
    LUAB_INT("IFF_DRV_OACTIVE",       IFF_DRV_OACTIVE),
    LUAB_INT("IFF_SIMPLEX",           IFF_SIMPLEX),
    LUAB_INT("IFF_LINK0",             IFF_LINK0),
    LUAB_INT("IFF_LINK1",             IFF_LINK1),
    LUAB_INT("IFF_LINK2",             IFF_LINK2),
    LUAB_INT("IFF_ALTPHYS",           IFF_ALTPHYS),
    LUAB_INT("IFF_MULTICAST",         IFF_MULTICAST),
    LUAB_INT("IFF_CANTCONFIG",        IFF_CANTCONFIG),
    LUAB_INT("IFF_PPROMISC",          IFF_PPROMISC),
    LUAB_INT("IFF_MONITOR",           IFF_MONITOR),
    LUAB_INT("IFF_STATICARP",         IFF_STATICARP),
    LUAB_INT("IFF_DYING",             IFF_DYING),
    LUAB_INT("IFF_RENAMING",          IFF_RENAMING),
    LUAB_INT("IFF_NOGROUP",           IFF_NOGROUP),
    LUAB_INT("IFF_CANTCHANGE",        IFF_CANTCHANGE),
    LUAB_INT("LINK_STATE_UNKNOWN",    LINK_STATE_UNKNOWN),
    LUAB_INT("LINK_STATE_DOWN",       LINK_STATE_DOWN),
    LUAB_INT("LINK_STATE_UP",         LINK_STATE_UP),
    LUAB_INT("IFCAP_RXCSUM",          IFCAP_RXCSUM),
    LUAB_INT("IFCAP_TXCSUM",          IFCAP_TXCSUM),
    LUAB_INT("IFCAP_NETCONS",         IFCAP_NETCONS),
    LUAB_INT("IFCAP_VLAN_MTU",        IFCAP_VLAN_MTU),
    LUAB_INT("IFCAP_VLAN_HWTAGGING",  IFCAP_VLAN_HWTAGGING),
    LUAB_INT("IFCAP_JUMBO_MTU",       IFCAP_JUMBO_MTU),
    LUAB_INT("IFCAP_POLLING",         IFCAP_POLLING),
    LUAB_INT("IFCAP_VLAN_HWCSUM",     IFCAP_VLAN_HWCSUM),
    LUAB_INT("IFCAP_TSO4",            IFCAP_TSO4),
    LUAB_INT("IFCAP_TSO6",            IFCAP_TSO6),
    LUAB_INT("IFCAP_LRO",             IFCAP_LRO),
    LUAB_INT("IFCAP_WOL_UCAST",       IFCAP_WOL_UCAST),
    LUAB_INT("IFCAP_WOL_MCAST",       IFCAP_WOL_MCAST),
    LUAB_INT("IFCAP_WOL_MAGIC",       IFCAP_WOL_MAGIC),
    LUAB_INT("IFCAP_TOE4",            IFCAP_TOE4),
    LUAB_INT("IFCAP_TOE6",            IFCAP_TOE6),
    LUAB_INT("IFCAP_VLAN_HWFILTER",   IFCAP_VLAN_HWFILTER),
    LUAB_INT("IFCAP_VLAN_HWTSO",      IFCAP_VLAN_HWTSO),
    LUAB_INT("IFCAP_LINKSTATE",       IFCAP_LINKSTATE),
    LUAB_INT("IFCAP_NETMAP",          IFCAP_NETMAP),
    LUAB_INT("IFCAP_RXCSUM_IPV6",     IFCAP_RXCSUM_IPV6),
    LUAB_INT("IFCAP_TXCSUM_IPV6",     IFCAP_TXCSUM_IPV6),
    LUAB_INT("IFCAP_HWSTATS",         IFCAP_HWSTATS),
    LUAB_INT("IFCAP_TXRTLMT",         IFCAP_TXRTLMT),
    LUAB_INT("IFCAP_HWRXTSTMP",       IFCAP_HWRXTSTMP),
    LUAB_INT("IFCAP_HWCSUM_IPV6",     IFCAP_HWCSUM_IPV6),
    LUAB_INT("IFCAP_HWCSUM",          IFCAP_HWCSUM),
    LUAB_INT("IFCAP_TSO",             IFCAP_TSO),
    LUAB_INT("IFCAP_WOL",             IFCAP_WOL),
    LUAB_INT("IFCAP_TOE",             IFCAP_TOE),
    LUAB_INT("IFCAP_CANTCHANGE",      IFCAP_CANTCHANGE),
    LUAB_INT("IFQ_MAXLEN",            IFQ_MAXLEN),
    LUAB_INT("IFNET_SLOWHZ",          IFNET_SLOWHZ),
    LUAB_INT("IFAN_ARRIVAL",          IFAN_ARRIVAL),
    LUAB_INT("IFAN_DEPARTURE",        IFAN_DEPARTURE),
    LUAB_INT("IFSTATMAX",             IFSTATMAX),
    LUAB_STR("IFG_ALL",               IFG_ALL),
    LUAB_STR("IFG_EGRESS",            IFG_EGRESS),
    LUAB_INT("RSS_FUNC_NONE",         RSS_FUNC_NONE),
    LUAB_INT("RSS_FUNC_PRIVATE",      RSS_FUNC_PRIVATE),
    LUAB_INT("RSS_FUNC_TOEPLITZ",     RSS_FUNC_TOEPLITZ),
    LUAB_INT("RSS_TYPE_IPV4",         RSS_TYPE_IPV4),
    LUAB_INT("RSS_TYPE_TCP_IPV4",     RSS_TYPE_TCP_IPV4),
    LUAB_INT("RSS_TYPE_IPV6",         RSS_TYPE_IPV6),
    LUAB_INT("RSS_TYPE_IPV6_EX",      RSS_TYPE_IPV6_EX),
    LUAB_INT("RSS_TYPE_TCP_IPV6",     RSS_TYPE_TCP_IPV6),
    LUAB_INT("RSS_TYPE_TCP_IPV6_EX",  RSS_TYPE_TCP_IPV6_EX),
    LUAB_INT("RSS_TYPE_UDP_IPV4",     RSS_TYPE_UDP_IPV4),
    LUAB_INT("RSS_TYPE_UDP_IPV6",     RSS_TYPE_UDP_IPV6),
    LUAB_INT("RSS_TYPE_UDP_IPV6_EX",  RSS_TYPE_UDP_IPV6_EX),
    LUAB_INT("RSS_KEYLEN",            RSS_KEYLEN),
    LUAB_INT("IFNET_PCP_NONE",        IFNET_PCP_NONE),
#endif /* __BSD_VISIBLE */
    LUAB_FUNC("if_indextoname",       luab_if_indextoname),
    LUAB_FUNC("if_nameindex",         luab_if_nameindex),
    LUAB_FUNC("if_nametoindex",       luab_if_nametoindex),
    LUAB_FUNC("create_if_nameindex",  luab_type_create_if_nameindex),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_net_if_lib = {
    .m_id       = LUAB_NET_IF_LIB_ID,
    .m_name     = LUAB_NET_IF_LIB_KEY,
    .m_vec      = luab_net_if_vec,
};
