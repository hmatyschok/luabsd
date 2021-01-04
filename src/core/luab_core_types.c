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

#include <net/if.h> /* XXX */
#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_modules.h"
#include "luab_udata.h"

/*
 * Bindings against atomic / composite data types.
 *
 * XXX
 *
 *  (a) We should split this vector table into
 *      subsets by category and externalize.
 *
 *  (b) Changes on documentation and namespace
 *      are neccessary.
 *
 *  (c) We shall regroup the set by utilizing a so called
 *      directory-like structure for components, e. g. over
 *
 *          t : { T }
 *
 *      by
 *
 *          T = { atomic, composite }
 *
 *      its type, realm and/or domain, etc.
 */

luab_module_vec_t luab_typevec[] = {
#if __BSD_VISIBLE
    {                                       /* integer types */
        .mv_mod = &luab_ushrt_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_USHRT_IDX,
    },{
        .mv_mod = &luab_uint_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_UINT_IDX,
    },{
        .mv_mod = &luab_ulong_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_ULONG_IDX,
    },
#endif /* __BSD_VISIBLE */
    {
        .mv_mod = &luab_char_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_CHAR_IDX,
    },{
        .mv_mod = &luab_short_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_SHORT_IDX,
    },{
        .mv_mod = &luab_int_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_INT_IDX,
    },{
        .mv_mod = &luab_long_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_LONG_IDX,
    },{                                     /* floating point number types */
        .mv_mod = &luab_double_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_DOUBLE_IDX,
    },{
        .mv_mod = &luab_float_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_FLOAT_IDX,
    },{                                     /* standard types */
        .mv_mod = &luab_off_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_OFF_IDX,
    },{
        .mv_mod = &luab_size_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_SIZE_IDX,
    },{
        .mv_mod = &luab_socklen_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_SOCKLEN_IDX,
    },{
        .mv_mod = &luab_ssize_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_SSIZE_IDX,
    },{
        .mv_mod = &luab_uid_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_UID_IDX,
    },{
        .mv_mod = &luab_wchar_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_WCHAR_IDX,
    },{
        .mv_mod = &luab_time_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_TIME_IDX,
    },{
        .mv_mod = &luab_clock_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_CLOCK_IDX,
    },{
        .mv_mod = &luab_vm_offset_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_VM_OFFSET_IDX,
    },{ /* <arpa/inet.h> */
        .mv_mod = &luab_in_port_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_IN_PORT_IDX,
    },{ /* <sys/stat.h> */
        .mv_mod = &luab_blkcnt_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_BLKCNT_IDX,
    },{
        .mv_mod = &luab_dev_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_DEV_IDX,
    },{
        .mv_mod = &luab_fflags_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_FFLAGS_IDX,
    },{
        .mv_mod = &luab_ino_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_INO_IDX,
    },{
        .mv_mod = &luab_nlink_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_NLINK_IDX,
    }{ /* POSIX sized integrals, <sys/stdint.h> */
        .mv_mod = &luab_int8_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_INT8_IDX,
    },{
        .mv_mod = &luab_int16_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_INT16_IDX,
    },{
        .mv_mod = &luab_int32_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_INT32_IDX,
    },{
        .mv_mod = &luab_int64_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_INT64_IDX,
    },{
        .mv_mod = &luab_uint8_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_UINT8_IDX,
    },{
        .mv_mod = &luab_uint16_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_UINT16_IDX,
    },{
        .mv_mod = &luab_uint32_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_UINT32_IDX,
    },{
        .mv_mod = &luab_uint64_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_UINT64_IDX,
    },{
        .mv_mod = &luab_intptr_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_INTPTR_IDX,
    },{
        .mv_mod = &luab_uintptr_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_UINTPTR_IDX,
    },{
        .mv_mod = &luab_intmax_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_INTMAX_IDX,
    },{
        .mv_mod = &luab_uintmax_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_UINTMAX_IDX,
    },{ /* <xlocale/_locale.h> */
        .mv_mod = &luab_locale_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_LOCALE_IDX,
    },{ /* <ctype.h> */
        .mv_mod = &luab_ct_rune_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_CT_RUNE_IDX,
    },{ /* <fcntl.h> */
        .mv_mod = &luab_mode_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_MODE_IDX,
    },{ /* <grp.h> */
        .mv_mod = &luab_gid_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_GID_IDX,
    },{ /* <langinfo.h> */
        .mv_mod = &luab_nl_item_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_NL_ITEM_IDX,
    },{ /* <stdio.h> */
        .mv_mod = &luab_fpos_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_FPOS_IDX,
    },{
        .mv_mod = &luab_rsize_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_RSIZE_IDX,
    },{ /* <unistd.h> */
        .mv_mod = &luab_pid_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_PID_IDX,
    },{
        .mv_mod = &luab_useconds_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_USECONDS_IDX,
    },{                                     /* composite data types */
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
        .mv_mod = &luab_integer_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_INTEGER_IDX,
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
    },{
        .mv_mod = &luab_jail_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_JAIL_IDX,
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
 * Service primitives.
 */

lua_Integer
luab_core_Integer_promotion_msk(int s)
{
    lua_Integer b_msk;

    b_msk = (s != 0) ? (lua_Integer)(
#if defined(__LP64__) || defined(__mips_n64)
    luab_env_ulong_max
#else
    luab_env_uint_max
#endif
    ) : (
#if defined(__LP64__) || defined(__mips_n64)
    luab_env_long_max
#else
    luab_env_int_max
#endif
    );
    return (b_msk);
}

/*
 * Access functions, n-th arg over argv, [stack -> C].
 *
 * luab_check{l}xxx(3) accessor evaluates, if n-th arg exists, otherwise
 * lua_error will be thrown. Finally luab_{is,to}{l}xxx(3) does the same
 * thing without throwing an error, but return NULL, if n-th arg does
 * not exist.
 */

void *
luab_checknil(lua_State *L, int narg)
{
    if (lua_isnil(L, narg) == 0)
        luab_core_argerror(L, narg, NULL, 0, 0, EINVAL);

    return (NULL);
}

lua_Integer
luab_tointeger(lua_State *L, int narg, lua_Integer b_msk)
{
    return ((lua_tointeger(L, narg)) & (b_msk));
}

lua_Integer
luab_tolinteger(lua_State *L, int narg, int s)
{
    lua_Integer b_msk;

    b_msk = luab_core_Integer_promotion_msk(s);
    return (luab_tointeger(L, narg, b_msk));
}

lua_Integer
luab_toxinteger(lua_State *L, int narg, luab_module_t *m, lua_Integer b_msk)
{
    lua_Integer *xp;

    if (m != NULL) {

        if (lua_isnumber(L, narg) != 0)
            return (luab_tointeger(L, narg, b_msk));

        xp = luab_udataisnil(L, 1, m, lua_Integer *);

        if (xp != NULL)
            return (*xp & b_msk);
    } else
        errno = ENOSYS;

    return (0);
}

lua_Integer
luab_tolxinteger(lua_State *L, int narg, luab_module_t *m, int s)
{
    lua_Integer *xp, b_msk;

    if (m != NULL) {

        if (lua_isnumber(L, narg) != 0)
            return (luab_tolinteger(L, narg, s));

        xp = luab_udataisnil(L, 1, m, lua_Integer *);

        if (xp != NULL) {
            b_msk = luab_core_Integer_promotion_msk(s);
            return (*xp & b_msk);
        }
    } else
        errno = ENOSYS;

    return (0);
}

lua_Integer
luab_checkinteger(lua_State *L, int narg, lua_Integer b_msk)
{
    return ((luaL_checkinteger(L, narg)) & (b_msk));
}

lua_Integer
luab_checklinteger(lua_State *L, int narg, int s)
{
    lua_Integer b_msk;

    b_msk = luab_core_Integer_promotion_msk(s);
    return (luab_checkinteger(L, narg, b_msk));
}

lua_Integer
luab_checkxinteger(lua_State *L, int narg, luab_module_t *m, lua_Integer b_msk)
{
    lua_Integer *xp;

    if (m != NULL) {

        if (lua_isnumber(L, narg) != 0)
            return (luab_checkinteger(L, narg, b_msk));

        xp = luab_udataisnil(L, 1, m, lua_Integer *);

        if (xp != NULL)
            return (*xp & b_msk);
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ENOSYS);

    return (0);
}

lua_Integer
luab_checklxinteger(lua_State *L, int narg, luab_module_t *m, int s)
{
    lua_Integer *xp, b_msk;

    if (m != NULL) {

        if (lua_isnumber(L, narg) != 0)
            return (luab_checklinteger(L, narg, s));

        xp = luab_udataisnil(L, 1, m, lua_Integer *);

        if (xp != NULL) {
            b_msk = luab_core_Integer_promotion_msk(s);
            return (*xp & b_msk);
        }
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ENOSYS);

    return (0);
}

lua_Number
luab_toxnumber(lua_State *L, int narg, luab_module_t *m)
{
    lua_Number *xp;

    if (m != NULL) {

        if (lua_isnumber(L, narg) != 0)
            return (lua_tonumber(L, narg));

        xp = luab_udataisnil(L, 1, m, lua_Number *);

        if (xp != NULL)
            return (*xp);
    } else
        errno = ENOSYS;

    return (0.0);
}

lua_Number
luab_checkxnumber(lua_State *L, int narg, luab_module_t *m)
{
    lua_Number *xp;

    if (m != NULL) {

        if (lua_isnumber(L, narg) != 0)
            return (luaL_checknumber(L, narg));

        xp = luab_udataisnil(L, 1, m, lua_Number *);

        if (xp != NULL)
            return (*xp);
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ENOSYS);

    return (0.0);
}

const char *
luab_islstring(lua_State *L, int narg, size_t nmax)
{
    const char *dp;
    size_t n;

    if ((dp = luaL_tolstring(L, narg, &n)) != NULL) {
        if (n <= nmax && nmax <= luab_env_buf_max)
            return (dp);
    }
    return (NULL);
}

const char *
luab_tolstring(lua_State *L, int narg, size_t nmax)
{
    const char *dp;
    size_t n;

    if ((dp = luaL_tolstring(L, narg, &n)) != NULL) {
        if (n == nmax && nmax <= luab_env_buf_max)
            return (dp);
    }
    return (NULL);
}

const char *
luab_checklstring(lua_State *L, int narg, size_t nmax, size_t *np)
{
    const char *dp;
    size_t n;

    dp = luaL_checklstring(L, narg, &n);

    if (n <= nmax && nmax <= luab_env_buf_max) {

        if (np != NULL)
            *np = n;

        return (dp);
    } else
        luab_core_argerror(L, narg, NULL, 0, 0, ERANGE);

    return (NULL);
}

const char *
luab_checklstringisnil(lua_State *L, int narg, size_t nmax, size_t *np)
{
    if (lua_isnil(L, narg) != 0) {

        if (np != NULL)
            *np = 0;

        return (NULL);
    }
    return (luab_checklstring(L, narg, nmax, np));
}

char *
luab_checklstringalloc(lua_State *L, int narg, size_t nmax)
{
    const char *dp;
    size_t len;
    caddr_t bp;

    if ((dp = luab_checklstringisnil(L, narg, nmax, &len)) != NULL) {

        if ((bp = luab_core_alloc(len, sizeof(char))) != NULL)
            (void)memmove(bp, dp, len);
        else
            luab_core_argerror(L, narg, NULL, 0, 0, errno);
    } else
        bp = NULL;

    return (bp);
}

/*
 * Access functions, [C -> stack].
 */

void
luab_rawsetinteger(lua_State *L, int narg, lua_Integer k, lua_Integer v)
{
    lua_pushinteger(L, v);
    lua_rawseti(L, narg, k);
}

void
luab_rawsetnumber(lua_State *L, int narg, lua_Integer k, lua_Number v)
{
    lua_pushnumber(L, v);
    lua_rawseti(L, narg, k);
}

void
luab_rawsetstring(lua_State *L, int narg, lua_Integer k, const char *v)
{
    lua_pushstring(L, v);
    lua_rawseti(L, narg, k);
}

void
luab_rawsetfstring(lua_State *L, int narg, lua_Integer k, const char *fmt, ...)
{
    va_list ap;
    char buf[luab_env_buf_max];

    va_start(ap, fmt);
    (void)vsnprintf(buf, luab_env_buf_max, fmt, ap);
    va_end(ap);

    lua_pushstring(L, buf);
    lua_rawseti(L, narg, k);
}

void
luab_rawsetldata(lua_State *L, int narg, lua_Integer k, void *v, size_t len)
{
    luaL_Buffer b;
    caddr_t dp;

    if ((v != NULL) &&
        (len > 1) &&
        (len < luab_env_buf_max)) {
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        lua_rawseti(L, narg, k);
    }
}

void
luab_setcfunction(lua_State *L, int narg, const char* k, lua_CFunction v)
{
    lua_pushcfunction(L, v);
    lua_setfield(L, narg, k);
}

void
luab_setinteger(lua_State *L, int narg, const char *k, lua_Integer v)
{
    lua_pushinteger(L, v);
    lua_setfield(L, narg, k);
}

void
luab_setnumber(lua_State *L, int narg, const char *k, lua_Number v)
{
    lua_pushnumber(L, v);
    lua_setfield(L, narg, k);
}

void
luab_setstring(lua_State *L, int narg, const char *k, const char *v)
{
    lua_pushstring(L, v);
    lua_setfield(L, narg, k);
}

void
luab_setfstring(lua_State *L, int narg, const char *k, const char *fmt, ...)
{
    va_list ap;
    char buf[luab_env_buf_max];

    va_start(ap, fmt);
    (void)vsnprintf(buf, luab_env_buf_max, fmt, ap);
    va_end(ap);

    lua_pushstring(L, buf);
    lua_setfield(L, narg, k);
}

void
luab_setldata(lua_State *L, int narg, const char *k, void *v, size_t len)
{
    luaL_Buffer b;
    caddr_t dp;

    if ((v != NULL) &&
        (len > 1) &&
        (len < luab_env_buf_max)) {
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        lua_setfield(L, narg, k);
    }
}

int
luab_pusherr(lua_State *L, int up_call, int ret)
{
    caddr_t msg;
    int status;

    if (up_call != 0) {
        lua_pushinteger(L, up_call);
        msg = strerror(up_call);
        lua_pushstring(L, msg);
        status = 2 + ret;
    } else
        status = ret;

    return (status);
}

int
luab_pushinteger(lua_State *L, lua_Integer x)
{
    int up_call;

    up_call = errno;
    lua_pushinteger(L, x);

    return (luab_pusherr(L, up_call, 1));
}

int
luab_pushnumber(lua_State *L, lua_Number x, int err)
{
    int up_call;

    if ((up_call = errno) != 0 && err != 0)
        lua_pushinteger(L, luab_env_error);
    else
        lua_pushnumber(L, x);

    return (luab_pusherr(L, up_call, 1));
}

int
luab_pushnil(lua_State *L)
{
    int up_call;

    up_call = errno;
    lua_pushnil(L);

    return (luab_pusherr(L, up_call, 1));
}

int
luab_pushstring(lua_State *L, const char *dp)
{
    int up_call;
    size_t n;
    int status;

    up_call = errno;

    if (dp != NULL) {
        n = strnlen(dp, luab_env_buf_max);
        lua_pushlstring(L, dp, n);

        status = luab_pusherr(L, up_call, 1);
    } else {
        errno = (up_call != 0) ? up_call : ENOENT;
        status = luab_pushnil(L);
    }
    return (status);
}

int
luab_pushfstring(lua_State *L, const char *fmt, ...)
{
    char buf[luab_env_buf_max];
    va_list ap;

    va_start(ap, fmt);
    (void)vsnprintf(buf, luab_env_buf_max, fmt, ap);
    va_end(ap);

    return (luab_pushstring(L, buf));
}

int
luab_pushldata(lua_State *L, void *v, size_t len)
{
    int up_call;
    luaL_Buffer b;
    caddr_t dp;
    int status;

    up_call = errno;

    if ((v != NULL) &&
        (len > 1) &&
        (len < luab_env_buf_max)) {
        luaL_buffinit(L, &b);
        dp = luaL_prepbuffsize(&b, len);

        (void)memmove(dp, v, len);

        luaL_addsize(&b, len);
        luaL_pushresult(&b);

        status = luab_pusherr(L, up_call, 1);
    } else {
        errno = (up_call != 0) ? up_call : EINVAL;
        status = luab_pushnil(L);
    }
    return (status);
}
