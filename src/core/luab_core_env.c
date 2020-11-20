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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

const lua_Integer luab_env_char_bit = LUAB_CHAR_BIT;

const lua_Integer luab_env_schar_max = LUAB_SCHAR_MAX;
const lua_Integer luab_env_schar_min = LUAB_SCHAR_MIN;
const lua_Integer luab_env_uchar_max = LUAB_UCHAR_MAX;

const lua_Integer luab_env_char_max = LUAB_CHAR_MAX;
const lua_Integer luab_env_char_min = CHAR_MIN;

const lua_Integer luab_env_ushrt_max = LUAB_USHRT_MAX;

const lua_Integer luab_env_shrt_max = LUAB_SHRT_MAX;
const lua_Integer luab_env_shrt_min = LUAB_SHRT_MIN;

const lua_Integer luab_env_uint_max = LUAB_UINT_MAX;
const lua_Integer luab_env_int_max = LUAB_INT_MAX;
const lua_Integer luab_env_int_min = LUAB_INT_MIN;

const lua_Integer luab_env_ulong_max = LUAB_ULONG_MAX;
const lua_Integer luab_env_long_max = LUAB_LONG_MAX;
const lua_Integer luab_env_long_min = LUAB_LONG_MIN;

const lua_Integer luab_env_ullong_max = LUAB_ULLONG_MAX;
const lua_Integer luab_env_llong_max = LUAB_LLONG_MAX;
const lua_Integer luab_env_llong_min = LUAB_LLONG_MIN;

const lua_Integer luab_env_ssize_max = LUAB_SSIZE_MAX;

const lua_Integer luab_env_u_long_max = LUAB_SIZE_T_MAX;
const lua_Integer luab_env_off_max = LUAB_OFF_MAX;
const lua_Integer luab_env_off_min = LUAB_OFF_MIN;

const lua_Integer luab_env_gid_max = LUAB_GID_MAX;
const lua_Integer luab_env_uid_max = LUAB_UID_MAX;
const lua_Integer luab_env_uquad_max = LUAB_UQUAD_MAX;
const lua_Integer luab_env_quad_max = LUAB_QUAD_MAX;
const lua_Integer luab_env_quad_min = LUAB_QUAD_MIN;

const lua_Integer luab_env_long_bit = LUAB_LONG_BIT;
const lua_Integer luab_env_word_bit = LUAB_WORD_BIT;

/*
 * XXX
 *  Constraints shall set by sysconf(3), if any.
 */

u_long luab_env_buf_max;
u_long luab_env_path_max;   /* XXX */
u_long luab_env_logname_max;

u_long luab_env_arg_max;
u_long luab_env_child_max;
u_long luab_env_clk_tck;
u_long luab_env_ngroups_max;
u_long luab_env_open_max;
u_long luab_env_job_control;
u_long luab_env_saved_ids;
u_long luab_env_version;
u_long luab_env_bc_base_max;
u_long luab_env_bc_dim_max;
u_long luab_env_bc_scale_max;
u_long luab_env_bc_string_max;
u_long luab_env_coll_weights_max;
u_long luab_env_expr_nest_max;
u_long luab_env_line_max;
u_long luab_env_re_dup_max;
u_long luab_env_2_version;
u_long luab_env_2_c_bind;
u_long luab_env_2_c_dev;
u_long luab_env_2_char_term;
u_long luab_env_2_fort_dev;
u_long luab_env_2_fort_run;
u_long luab_env_2_localedef;
u_long luab_env_2_sw_dev;
u_long luab_env_2_upe;
u_long luab_env_stream_max;
u_long luab_env_tzname_max;

u_long luab_env_asynchronous_io;
u_long luab_env_mapped_files;
u_long luab_env_memlock;
u_long luab_env_memlock_range;
u_long luab_env_memory_protection;
u_long luab_env_message_passing;
u_long luab_env_prioritized_io;
u_long luab_env_priority_scheduling;
u_long luab_env_realtime_signals;
u_long luab_env_semaphores;
u_long luab_env_fsync;
u_long luab_env_shared_memory_objects;
u_long luab_env_synchronized_io;
u_long luab_env_timers;
u_long luab_env_aio_listio_max;
u_long luab_env_aio_max;
u_long luab_env_aio_prio_delta_max;
u_long luab_env_delaytimer_max;
u_long luab_env_mq_open_max;
u_long luab_env_pagesize;
u_long luab_env_rtsig_max;
u_long luab_env_sem_nsems_max;
u_long luab_env_sem_value_max;
u_long luab_env_sigqueue_max;
u_long luab_env_timer_max;

u_long luab_env_2_pbs;
u_long luab_env_2_pbs_accounting;
u_long luab_env_2_pbs_checkpoint;
u_long luab_env_2_pbs_locate;
u_long luab_env_2_pbs_message;
u_long luab_env_2_pbs_track;
u_long luab_env_advisory_info;
u_long luab_env_barriers;
u_long luab_env_clock_selection;
u_long luab_env_cputime;
u_long luab_env_file_locking;
u_long luab_env_getgr_r_size_max;
u_long luab_env_getpw_r_size_max;
u_long luab_env_host_name_max;
u_long luab_env_login_name_max;
u_long luab_env_monotonic_clock;
u_long luab_env_mq_prio_max;
u_long luab_env_reader_writer_locks;
u_long luab_env_regexp;
u_long luab_env_shell;
u_long luab_env_spawn;
u_long luab_env_spin_locks;
u_long luab_env_sporadic_server;
u_long luab_env_thread_attr_stackaddr;
u_long luab_env_thread_attr_stacksize;
u_long luab_env_thread_cputime;
u_long luab_env_thread_destructor_iterations;
u_long luab_env_thread_keys_max;
u_long luab_env_thread_prio_inherit;
u_long luab_env_thread_prio_protect;
u_long luab_env_thread_priority_scheduling;
u_long luab_env_thread_process_shared;
u_long luab_env_thread_safe_functions;
u_long luab_env_thread_sporadic_server;
u_long luab_env_thread_stack_min;
u_long luab_env_thread_threads_max;
u_long luab_env_timeouts;
u_long luab_env_threads;
u_long luab_env_trace;
u_long luab_env_trace_event_filter;
u_long luab_env_trace_inherit;
u_long luab_env_trace_log;
u_long luab_env_tty_name_max;
u_long luab_env_typed_memory_objects;
u_long luab_env_v6_ilp32_off32;
u_long luab_env_v6_ilp32_offbig;
u_long luab_env_v6_lp64_off64;
u_long luab_env_v6_lpbig_offbig;
u_long luab_env_ipv6;
u_long luab_env_raw_sockets;
u_long luab_env_symloop_max;

u_long luab_env_atexit_max;
u_long luab_env_iov_max;
u_long luab_env_page_size;
u_long luab_env_xopen_crypt;
u_long luab_env_xopen_enh_i18n;
u_long luab_env_xopen_legacy;
u_long luab_env_xopen_realtime;
u_long luab_env_xopen_realtime_threads;
u_long luab_env_xopen_shm;
u_long luab_env_xopen_streams;
u_long luab_env_xopen_unix;
u_long luab_env_xopen_version;
u_long luab_env_xopen_xcu_version;

u_long luab_env_nprocessors_conf;
u_long luab_env_nprocessors_onln;
u_long luab_env_cpuset_size;

u_long luab_env_phys_pages;
