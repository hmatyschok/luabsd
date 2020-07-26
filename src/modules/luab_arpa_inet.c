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

#include <arpa/inet.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

#define LUABSD_ARPA_INET_LIB_ID    1595780686
#define LUABSD_ARPA_INET_LIB_KEY   "inet"

extern luab_module_t luab_arpa_inet_lib;

/*
 * Interface against <arpa/inet.h>.
 */

static luab_table_t luab_arpa_inet_vec[] = {   /* arpa/inet.h */
    LUABSD_INT("INET_ADDRSTRLEN",   INET_ADDRSTRLEN),
    LUABSD_INT("INET6_ADDRSTRLEN",  INET6_ADDRSTRLEN),
#if 0
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
    LUABSD_FUNC("inet_nsap_addr",   luab_inet_nsap_addr),
    LUABSD_FUNC("inet_nsap_ntoa",   luab_inet_nsap_ntoa),
#endif /* __BSD_VISIBLE */
#endif
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_arpa_inet_lib = {
    .cookie = LUABSD_ARPA_INET_LIB_ID,
    .name = LUABSD_ARPA_INET_LIB_KEY,
    .vec = luab_arpa_inet_vec,
};
