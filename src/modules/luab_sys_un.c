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

#include <sys/un.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_sockaddr.h"

#define LUAB_SYS_UN_LIB_ID    1597545462
#define LUAB_SYS_UN_LIB_KEY    "un"

extern luab_module_t luab_sys_un_lib;

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(SOCKADDR)).
 *
 * @function sockaddr_un_create
 *
 * @param path              Specifies path or filename.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sockaddr [, err, msg ] = bsd.sys.socket.sockaddr_un_create([ path ])
 */
static int
luab_sockaddr_un_create(lua_State *L)
{
    luab_module_t *m;
    struct sockaddr_un sun;
    struct sockaddr *data;
    const char *sun_path;

    m = luab_xmod(SOCKADDR, TYPE, __func__);
    data = (struct sockaddr *)&sun;
    luab_sockaddr_pci(data, AF_UNIX, sizeof(sun));

    switch (luab_core_checkmaxargs(L, 1)) {     /* FALLTHROUGH */
    case 1:
        sun_path = luab_checklstring(L, 1, LUAB_SUN_MAXPATHLEN, NULL);
        (void)memmove(sun.sun_path, sun_path, strlen(sun_path));
    default:
        break;
    }
    return (luab_pushxdata(L, m, data));
}

/*
 * Interface against <sys/un.h>.
 */

static luab_module_table_t luab_sys_un_vec[] = {
#if __BSD_VISIBLE
    LUAB_INT("LOCAL_PEERCRED",        LOCAL_PEERCRED),
    LUAB_INT("LOCAL_CREDS",           LOCAL_CREDS),
    LUAB_INT("LOCAL_CONNWAIT",        LOCAL_CONNWAIT),
    LUAB_INT("LOCAL_VENDOR",          LOCAL_VENDOR),
#endif
    LUAB_FUNC("sockaddr_un_create",   luab_sockaddr_un_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_un_lib = {
    .m_id       = LUAB_SYS_UN_LIB_ID,
    .m_name     = LUAB_SYS_UN_LIB_KEY,
    .m_vec      = luab_sys_un_vec,
};
