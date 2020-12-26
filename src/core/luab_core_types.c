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
        .mv_mod = &luab_fpos_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_FPOS_IDX,
    },{
        .mv_mod = &luab_gid_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_GID_IDX,
    },{
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
        .mv_mod = &luab_locale_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_LOCALE_IDX,
    },{ /* POSIX sized integrals, <sys/stdint.h> */
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
    },{ /* <ctype.h> */
        .mv_mod = &luab_ct_rune_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_CT_RUNE_IDX,
    },{ /* <langinfo.h> */
        .mv_mod = &luab_nl_item_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_NL_ITEM_IDX,
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
#if LUAB_DEBUG
    {
        .mv_mod = &luab_link_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_LINK_IDX,
    },
#endif /* LUAB_DEBUG */
    LUAB_MOD_VEC_SENTINEL
};

