/*
 * Copyright (c) 2020 Henning Matyschok
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

#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_modules.h"
#include "luab_udata.h"

LUAMOD_API int  luaopen_bsd(lua_State *);

/*
 * Common subr. for initializiation, those are
 * called during runtime of package.loadlib().
 */

static void
luab_core_populate(lua_State *L, int narg, luab_module_t *m)
{
    luab_module_table_t *tok;

    for (tok = m->m_vec; tok->mt_key != NULL; tok++) {
        (void)(*tok->mt_init)(L, &tok->mt_val);
        lua_setfield(L, narg, tok->mt_key);
    }
    lua_pop(L, 0);
}

static void
luab_core_newtable(lua_State *L, int narg, luab_module_t *m)
{
    lua_newtable(L);
    luab_core_populate(L, narg, m);
    lua_setfield(L, narg, m->m_name);
}

static void
luab_core_newmetatable(lua_State *L, int narg, luab_module_t *m)
{
    luaL_newmetatable(L, m->m_name);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luab_core_populate(L, narg, m);

    lua_pop(L, 1);
}

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
        .mv_mod = &luab_sys_mount_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_stat_lib,
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
    },{
        .mv_mod = &luab_sys_reboot_lib,
        .mv_init = luab_core_newtable,
    },{
        .mv_mod = &luab_sys_socket_lib,
        .mv_init = luab_core_newtable,
    },
    LUAB_MOD_VEC_SENTINEL
};

/* Interface against <xxx.h> or <core>. */
static luab_module_vec_t luab_core_vec[] = {
    {
        .mv_mod = &luab_core_lib,
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

/* Bindings against complex data types. */
luab_module_vec_t luab_typevec[] = {
#if LUAB_DEBUG
    {
        .mv_mod = &luab_link_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_LINK_IDX,
    },
#endif /* LUAB_DEBUG */
    {
        .mv_mod = &luab_clockinfo_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_CLOCKINFO_IDX,
    },{
        .mv_mod = &luab_div_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_DIV_IDX,
    },{
        .mv_mod = &luab_flock_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_FLOCK_IDX,
    },{
        .mv_mod = &luab_primitive_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_PRIMITIVE_IDX,
    },{
        .mv_mod = &luab_if_nameindex_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_IF_NAMEINDEX_IDX,
    },{
        .mv_mod = &luab_in_addr_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_IN_ADDR_IDX,
    },{
        .mv_mod = &luab_in6_addr_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_IN6_ADDR_IDX,
    },{
        .mv_mod = &luab_itimerval_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_ITIMERVAL_IDX,
    },{
        .mv_mod = &luab_ldiv_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_LDIV_IDX,
    },{
        .mv_mod = &luab_lldiv_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_LLDIV_IDX,
    },{
        .mv_mod = &luab_linger_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_LINGER_IDX,
    },{
        .mv_mod = &luab_msghdr_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_MSGHDR_IDX,
    },{
        .mv_mod = &luab_sockaddr_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_SOCKADDR_IDX,
    },{
        .mv_mod = &luab_stat_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_STAT_IDX,
    },{
        .mv_mod = &luab_timespec_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_TIMESPEC_IDX,
    },{
        .mv_mod = &luab_timeval_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_TIMEVAL_IDX,
    },{
        .mv_mod = &luab_timezone_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_TIMEZONE_IDX,
    },{
        .mv_mod = &luab_tm_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_TM_IDX,
    },{
        .mv_mod = &luab_uuid_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_UUID_IDX,
    },{
        .mv_mod = &luab_iovec_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_IOVEC_IDX,
    },{
        .mv_mod = &luab_passwd_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_PASSWD_IDX,
    },{
        .mv_mod = &luab_group_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_GROUP_IDX,
    },{
        .mv_mod = &luab_regex_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_REGEX_IDX,
    },{
        .mv_mod = &luab_regmatch_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_REGMATCH_IDX,
    },{
        .mv_mod = &luab_fsid_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_FSID_IDX,
    },{
        .mv_mod = &luab_fid_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_FID_IDX,
    },{
        .mv_mod = &luab_statfs_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_STATFS_IDX,
    },{
        .mv_mod = &luab_fhandle_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_FHANDLE_IDX,
    },{
        .mv_mod = &luab_xvfsconf_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_XVFSCONF_IDX,
    },{
        .mv_mod = &luab_ttyent_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_TTYENT_IDX,
    },{
        .mv_mod = &luab_dir_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_DIR_IDX,
    },{
        .mv_mod = &luab_dirent_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_DIRENT_IDX,
    },{
        .mv_mod = &luab_fstab_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_FSTAB_IDX,
    },{
        .mv_mod = &luab___sbuf_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB___SBUF_IDX,
    },{
        .mv_mod = &luab_sfile_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_SFILE_IDX,
    },
#if __BSD_VISIBLE
    {
        .mv_mod = &luab_dbt_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_DBT_IDX,
    },{
        .mv_mod = &luab_db_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_DB_IDX,
    },{
        .mv_mod = &luab_bintime_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_BINTIME_IDX,
    },{
        .mv_mod = &luab_crypt_data_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_CRYPT_DATA_IDX,
    },{
        .mv_mod = &luab_cap_rbuf_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_CAP_RBUF_IDX,
    },{
        .mv_mod = &luab_accept_filter_arg_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_ACCEPT_FILTER_ARG_IDX,
    },{
        .mv_mod = &luab_sockproto_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_SOCKPROTO_IDX,
    },{
        .mv_mod = &luab_cmsgcred_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_CMSGCRED_IDX,
    },{
        .mv_mod = &luab_sf_hdtr_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_SF_HDTR_IDX,
    },
#endif  /* __BSD_VISIBLE */
    LUAB_MOD_VEC_SENTINEL
};

/*
 * Reflects and maps interface against API over </include/>.
 */

static void
luab_core_initmodule(lua_State *L, int narg, luab_module_vec_t *vec,
    const char *name, int new)
{
    luab_module_vec_t *mv;

    if (name != NULL && new != 0)
        lua_newtable(L);

    for (mv = vec; mv->mv_mod != NULL; mv++)
        (*mv->mv_init)(L, narg, mv->mv_mod);

    if (name != NULL && new != 0)
        lua_setfield(L, narg, name);
}

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
    " Copyright (c) 2020 Henning Matyschok\n"
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

LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    (void)printf("%s", copyright);

    lua_newtable(L);

    luab_core_registerlib(L, -2, luab_arpa_vec,  "arpa");
    luab_core_registerlib(L, -2, luab_net_vec,   "net");
    luab_core_registerlib(L, -2, luab_sys_vec,   "sys");
    luab_core_registerlib(L, -2, luab_core_vec,  NULL);

    lua_pushvalue(L, -1);

    /* register complex data-types. */
    luab_core_registertype(L, -2, luab_typevec);

    /* setup constraints */
    luab_buf_max = LUAL_BUFFERSIZE;
    luab_path_max = MAXPATHLEN;

    luab_comlen_max = MAXCOMLEN;
    luab_interp_max = MAXINTERP;
    luab_logname_max = MAXLOGNAME;
    luab_uprc_max = MAXUPRC;

    luab_nofile = NOFILE;
    luab_nogroup = NOGROUP;

    luab_hostname_max = MAXHOSTNAMELEN;
    luab_specname_max = SPECNAMELEN;

    luab_ncargs = luab_core_sysconf(_SC_ARG_MAX, NCARGS);
    luab_ngroups = luab_core_sysconf(_SC_NGROUPS_MAX, NGROUPS);
    
    luab_tty_max = luab_core_sysconf(_SC_TTY_NAME_MAX, SPECNAMELEN);

    return (1);
}
