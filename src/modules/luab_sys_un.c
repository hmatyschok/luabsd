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

#include <sys/un.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t sockaddr_type;

extern int luab_StructSockAddrUn(lua_State *);

#define LUABSD_SYS_UN_LIB_ID    1597545462
#define LUABSD_SYS_UN_LIB_KEY    "un"

extern luab_module_t luab_sys_un_lib;

/*
 * Interface against <sys/un.h>.
 */

static luab_table_t luab_sys_un_vec[] = {
#if __BSD_VISIBLE
    LUABSD_INT("LOCAL_PEERCRED",    LOCAL_PEERCRED),
    LUABSD_INT("LOCAL_CREDS",   LOCAL_CREDS),
    LUABSD_INT("LOCAL_CONNWAIT",    LOCAL_CONNWAIT),
    LUABSD_INT("LOCAL_VENDOR",  LOCAL_VENDOR),
#endif
    LUABSD_FUNC("StructSockAddrUn", luab_StructSockAddrUn),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_sys_un_lib = {
    .cookie = LUABSD_SYS_UN_LIB_ID,
    .name = LUABSD_SYS_UN_LIB_KEY,
    .vec = luab_sys_un_vec,
};
