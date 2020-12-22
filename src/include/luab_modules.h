/*-
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

#ifndef _LUAB_MODULES_H_
#define _LUAB_MODULES_H_

/*
 * Set of atomic data types.
 */

#if __BSD_VISIBLE
extern luab_module_t luab_ushrt_type;
extern luab_module_t luab_uint_type;
#endif /* __BSD_VISIBLE */

extern luab_module_t luab_char_type;
extern luab_module_t luab_short_type;
extern luab_module_t luab_int_type;
extern luab_module_t luab_long_type;

extern luab_module_t luab_double_type;
extern luab_module_t luab_float_type;

/* standard types */
extern luab_module_t luab_fpos_type;
extern luab_module_t luab_gid_type;
extern luab_module_t luab_off_type;
extern luab_module_t luab_size_type;
extern luab_module_t luab_socklen_type;
extern luab_module_t luab_ssize_type;
extern luab_module_t luab_uid_type;

extern luab_module_t luab_wchar_type;
extern luab_module_t luab_time_type;
extern luab_module_t luab_clock_type;
extern luab_module_t luab_locale_type;

/* POSIX sized integrals, <sys/stdint.h> */
extern luab_module_t luab_int8_type;
extern luab_module_t luab_int16_type;
extern luab_module_t luab_int32_type;
extern luab_module_t luab_int64_type;

extern luab_module_t luab_uint8_type;
extern luab_module_t luab_uint16_type;
extern luab_module_t luab_uint32_type;
extern luab_module_t luab_uint64_type;

extern luab_module_t luab_intptr_type;
extern luab_module_t luab_uintptr_type;
extern luab_module_t luab_intmax_type;
extern luab_module_t luab_uintmax_type;

/*
 * Set of composite data types.
 */

extern luab_module_t luab_clockinfo_type;
extern luab_module_t luab_div_type;
extern luab_module_t luab_flock_type;
extern luab_module_t luab_integer_type;
extern luab_module_t luab_if_nameindex_type;
extern luab_module_t luab_in_addr_type;
extern luab_module_t luab_in6_addr_type;
extern luab_module_t luab_itimerval_type;
extern luab_module_t luab_ldiv_type;
extern luab_module_t luab_lldiv_type;
extern luab_module_t luab_linger_type;
extern luab_module_t luab_msghdr_type;
extern luab_module_t luab_sockaddr_type;
extern luab_module_t luab_stat_type;
extern luab_module_t luab_timespec_type;
extern luab_module_t luab_timeval_type;
extern luab_module_t luab_timezone_type;
extern luab_module_t luab_tm_type;
extern luab_module_t luab_uuid_type;
extern luab_module_t luab_iovec_type;
extern luab_module_t luab_passwd_type;
extern luab_module_t luab_group_type;
extern luab_module_t luab_regex_type;
extern luab_module_t luab_regmatch_type;
extern luab_module_t luab_fsid_type;
extern luab_module_t luab_fid_type;
extern luab_module_t luab_statfs_type;
extern luab_module_t luab_fhandle_type;
extern luab_module_t luab_xvfsconf_type;
extern luab_module_t luab_ttyent_type;
extern luab_module_t luab_dir_type;
extern luab_module_t luab_dirent_type;
extern luab_module_t luab_fstab_type;
extern luab_module_t luab___sbuf_type;
extern luab_module_t luab_sfile_type;
extern luab_module_t luab_jail_type;

#if __BSD_VISIBLE
extern luab_module_t luab_dbt_type;
extern luab_module_t luab_db_type;
extern luab_module_t luab_bintime_type;
extern luab_module_t luab_crypt_data_type;
extern luab_module_t luab_cap_rbuf_type;
extern luab_module_t luab_accept_filter_arg_type;
extern luab_module_t luab_sockproto_type;
extern luab_module_t luab_cmsgcred_type;
extern luab_module_t luab_sf_hdtr_type;
#endif

/*
 * Set of interfaces.
 */

extern luab_module_t luab_arpa_inet_lib;

extern luab_module_t luab_core_lib;
extern luab_module_t luab_core_atomic_lib;

extern luab_module_t luab_net_if_lib;
extern luab_module_t luab_net_if_dl_lib;

extern luab_module_t luab_sys_dirent_lib;
extern luab_module_t luab_sys_file_lib;
extern luab_module_t luab_sys_jail_lib;
extern luab_module_t luab_sys_mount_lib;
extern luab_module_t luab_sys_stat_lib;
extern luab_module_t luab_sys_time_lib;
extern luab_module_t luab_sys_uio_lib;
extern luab_module_t luab_sys_un_lib;
extern luab_module_t luab_sys_unistd_lib;
extern luab_module_t luab_sys_reboot_lib;
extern luab_module_t luab_sys_socket_lib;
extern luab_module_t luab_sys_stdint_lib;

extern luab_module_t luab_xlocale_inttypes_lib;
extern luab_module_t luab_xlocale_locale_lib;
extern luab_module_t luab_xlocale_time_lib;

extern luab_module_t luab_db_lib;
extern luab_module_t luab_dirent_lib;
extern luab_module_t luab_fcntl_lib;
extern luab_module_t luab_fstab_lib;
extern luab_module_t luab_grp_lib;
extern luab_module_t luab_pwd_lib;
extern luab_module_t luab_regex_lib;
extern luab_module_t luab_stdio_lib;
extern luab_module_t luab_stdlib_lib;
extern luab_module_t luab_time_lib;
extern luab_module_t luab_ttyent_lib;
extern luab_module_t luab_unistd_lib;
extern luab_module_t luab_uuid_lib;

void     luab_core_populate(lua_State *, int, luab_module_t *);
void     luab_core_newtable(lua_State *, int, luab_module_t *);
void     luab_core_newmetatable(lua_State *, int, luab_module_t *);

void     luab_core_initmodule(lua_State *, int, luab_module_vec_t *,
    const char *, int);

#endif /* _LUAB_MODULES_H_ */
