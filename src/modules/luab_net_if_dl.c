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
#include <net/if_dl.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t sockaddr_type;

extern int luab_StructSockAddrDL(lua_State *);

#define LUABSD_IF_DL_LIB_ID    1596382827
#define LUABSD_IF_DL_LIB_KEY    "if_dl"

extern luab_module_t luab_net_if_dl_lib;

/*
 * Interface against <net/if_dl.h>.
 */

static luab_table_t luab_if_dl_vec[] = {   /* if_dl.h */
#if 0
    LUABSD_FUNC("link_addr",    luab_link_addr),
    LUABSD_FUNC("link_ntoa",    luab_link_ntoa),
#endif
    LUABSD_FUNC("StructSockAddrDL", luab_StructSockAddrDL),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_net_if_dl_lib = {
    .cookie = LUABSD_IF_DL_LIB_ID,
    .name = LUABSD_IF_DL_LIB_KEY,
    .vec = luab_if_dl_vec,
};
