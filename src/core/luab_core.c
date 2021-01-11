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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_modules.h"
#include "luab_udata.h"

LUAMOD_API int  luaopen_bsd(lua_State *);

/*
 * Vector-table set, provides meta-information for Lua bindings.
 */

/* Interface against <arpa/xxx.h>. */
static luab_module_vec_t luab_arpa_vec[] = {
    {
        .mv_mod = &luab_arpa_inet_lib,
        .mv_init = luab_core_newtable,
    },
    LUAB_MOD_VEC_SENTINEL
};

/* Interface against <net/xxx.h>. */
static luab_module_vec_t luab_net_vec[] = {
    {
        .mv_mod = &luab_net_if_dl_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_net_if_lib,
        .mv_init = luab_core_populate,
    },
    LUAB_MOD_VEC_SENTINEL
};

/* Interface against <sys/xxx.h>. */
static luab_module_vec_t luab_sys_vec[] = {
    {
        .mv_mod = &luab_sys_dirent_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_file_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_jail_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_mount_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_reboot_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_signal_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_socket_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_stat_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_stdint_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_time_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_uio_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_un_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_unistd_lib,
        .mv_init = luab_core_newtable,
    },
    LUAB_MOD_VEC_SENTINEL
};

/* Interface against <xlocale/xxx.h>. */
static luab_module_vec_t luab_xlocale_vec[] = {
    {
        .mv_mod = &luab_xlocale_inttypes_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_xlocale_langinfo_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_xlocale_locale_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_xlocale_time_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_xlocale_lib,
        .mv_init = luab_core_populate,
    },
    LUAB_MOD_VEC_SENTINEL
};

/* Interface against <core/xxx> */
static luab_module_vec_t luab_core_vec[] = {
    {
        .mv_mod = &luab_core_atomic_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_core_lib,
        .mv_init = luab_core_populate,
    },
    LUAB_MOD_VEC_SENTINEL
};

/* Interface against <xxx.h> */
static luab_module_vec_t luab_vec[] = {
    {
        .mv_mod = &luab_cpio_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_ctype_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_db_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_dirent_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_fcntl_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_fstab_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_grp_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_langinfo_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_locale_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_pwd_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_regex_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_stdio_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_stdlib_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_time_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_ttyent_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_unistd_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_uuid_lib,
        .mv_init = luab_core_newtable,
    },
    LUAB_MOD_VEC_SENTINEL
};

/*
 * Reflects and maps interface against API over </include/>.
 */

static void
luab_core_registerlib(lua_State *L, int narg, luab_module_vec_t *vec, const char *name)
{
    luab_core_initmodule(L, narg, vec, name, 1);
}

static void
luab_core_registertype(lua_State *L, int narg, luab_module_vec_t *vec)
{
    luab_core_initmodule(L, narg, vec, NULL, 0);
}

static const char *copyright =
    " Copyright (c) 2020, 2021 Henning Matyschok\n"
    " All rights reserved.\n"
    "\n"
    "  The implementation of the interface against alarm(3) and setitimer(2)\n"
    "  is derived from:\n"
    "\n"
    "   lalarm.c\n"
    "   an alarm library for Lua based on signal(3)\n"
    "   Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>\n"
    "   28 Jul 2018 12:47:52\n\n"
    "\n";

/*
 * Initializes Lua-module.
 */
LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    (void)printf("%s", copyright);

    /* initialize constraints */
    luab_core_initenv();

    /*
     * register modules
     *
     * XXX
     *  We shall externalize this.
     */
    lua_newtable(L);

    luab_core_registerlib(L, -2, luab_arpa_vec,     "arpa");
    luab_core_registerlib(L, -2, luab_core_vec,     "core");
    luab_core_registerlib(L, -2, luab_net_vec,      "net");
    luab_core_registerlib(L, -2, luab_sys_vec,      "sys");
    luab_core_registerlib(L, -2, luab_xlocale_vec,  "xlocale");
    luab_core_registerlib(L, -2, luab_vec,          NULL);

    lua_pushvalue(L, -1);

    /* register complex data-types. */
    luab_core_registertype(L, -2, luab_typevec);

    return (1);
}
