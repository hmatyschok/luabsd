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
 * Set of complex data types.
 */

#if LUAB_DEBUG
extern luab_module_t link_type;
#endif
extern luab_module_t clockinfo_type;
extern luab_module_t div_type;
extern luab_module_t flock_type;
extern luab_module_t primitive_type;
extern luab_module_t if_nameindex_type;
extern luab_module_t in_addr_type;
extern luab_module_t in6_addr_type;
extern luab_module_t itimerval_type;
extern luab_module_t ldiv_type;
extern luab_module_t lldiv_type;
extern luab_module_t linger_type;
extern luab_module_t msghdr_type;
extern luab_module_t sockaddr_type;
extern luab_module_t stat_type;
extern luab_module_t timespec_type;
extern luab_module_t timeval_type;
extern luab_module_t timezone_type;
extern luab_module_t tm_type;
extern luab_module_t uuid_type;
extern luab_module_t iovec_type;
extern luab_module_t passwd_type;
extern luab_module_t group_type;
extern luab_module_t regex_type;
extern luab_module_t regmatch_type;
extern luab_module_t fsid_type;
extern luab_module_t fid_type;
extern luab_module_t statfs_type;

#if __BSD_VISIBLE
extern luab_module_t dbt_type;
extern luab_module_t db_type;
extern luab_module_t bintime_type;
extern luab_module_t crypt_data_type;
extern luab_module_t cap_rbuf_type;
extern luab_module_t accept_filter_arg_type;
extern luab_module_t sockproto_type;
extern luab_module_t cmsgcred_type;
extern luab_module_t sf_hdtr_type;
#endif

/*
 * Set of interfaces.
 */

extern luab_module_t luab_arpa_inet_lib;

extern luab_module_t luab_net_if_lib;
extern luab_module_t luab_net_if_dl_lib;

extern luab_module_t luab_sys_file_lib;
extern luab_module_t luab_sys_mount_lib;
extern luab_module_t luab_sys_stat_lib;
extern luab_module_t luab_sys_time_lib;
extern luab_module_t luab_sys_uio_lib;
extern luab_module_t luab_sys_un_lib;
extern luab_module_t luab_sys_unistd_lib;
extern luab_module_t luab_sys_reboot_lib;
extern luab_module_t luab_sys_socket_lib;

extern luab_module_t luab_core_lib;

extern luab_module_t luab_db_lib;
extern luab_module_t luab_fcntl_lib;
extern luab_module_t luab_grp_lib;
extern luab_module_t luab_pwd_lib;
extern luab_module_t luab_regex_lib;
extern luab_module_t luab_stdlib_lib;
extern luab_module_t luab_time_lib;
extern luab_module_t luab_unistd_lib;
extern luab_module_t luab_uuid_lib;
#endif /* _LUAB_MODULES_H_ */
