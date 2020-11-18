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

const lua_Integer luab_char_bit = CHAR_BIT;

const lua_Integer luab_schar_max = SCHAR_MAX;
const lua_Integer luab_schar_min = SCHAR_MIN;
const lua_Integer luab_uchar_max = UCHAR_MAX;

const lua_Integer luab_char_max = CHAR_MAX;
const lua_Integer luab_char_min = CHAR_MIN;

const lua_Integer luab_ushrt_max = USHRT_MAX;

const lua_Integer luab_shrt_max = SHRT_MAX;
const lua_Integer luab_shrt_min = SHRT_MIN;

const lua_Integer luab_uint_max = UINT_MAX;
const lua_Integer luab_int_max = INT_MAX;
const lua_Integer luab_int_min = INT_MIN;

const lua_Integer luab_ulong_max = ULONG_MAX;
const lua_Integer luab_long_max = LONG_MAX;
const lua_Integer luab_long_min = LONG_MIN;

#ifdef __LONG_LONG_SUPPORTED
const lua_Integer luab_ullong_max = ULLONG_MAX;
const lua_Integer luab_llong_max = LLONG_MAX;
const lua_Integer luab_llong_min = LLONG_MIN;
#else
const lua_Integer luab_ullong_max = 0;
const lua_Integer luab_llong_max = 0;
const lua_Integer luab_llong_min = 0;
#endif

#if __POSIX_VISIBLE || __XSI_VISIBLE
const lua_Integer luab_ssize_max = SSIZE_MAX;
#else
const lua_Integer luab_ssize_max = 0;
#endif

#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
const lua_Integer luab_size_t_max = SIZE_T_MAX;
const lua_Integer luab_off_max = OFF_MAX;
const lua_Integer luab_off_min = OFF_MIN;
#else
const lua_Integer luab_size_t_max = 0;
const lua_Integer luab_off_max = 0;
const lua_Integer luab_off_min = 0;
#endif

#if __BSD_VISIBLE
const lua_Integer luab_gid_max = GID_MAX;
const lua_Integer luab_uid_max = UID_MAX;
const lua_Integer luab_uquad_max = UQUAD_MAX;
const lua_Integer luab_quad_max = QUAD_MAX;
const lua_Integer luab_quad_min = QUAD_MIN;
#else
const lua_Integer luab_gid_max = 0;
const lua_Integer luab_uid_max = 0;
const lua_Integer luab_uquad_max = 0;
const lua_Integer luab_quad_max = 0;
const lua_Integer luab_quad_min = 0;
#endif

#if __XSI_VISIBLE || __POSIX_VISIBLE >= 200809
const lua_Integer luab_long_bit = LONG_BIT;
const lua_Integer luab_word_bit = WORD_BIT;
#else
const lua_Integer luab_long_bit = 0;
const lua_Integer luab_word_bit = 0;
#endif

/*
 * XXX
 *  Constraints shall set by sysconf(3), if any.
 */

size_t luab_param_buf_max;
size_t luab_param_path_max;   /* XXX */
size_t luab_param_logname_max;

size_t luab_param_arg_max;
size_t luab_param_child_max;
size_t luab_param_clk_tck;
size_t luab_param_ngroups_max;
size_t luab_param_open_max;
size_t luab_param_job_control;
size_t luab_param_saved_ids;
size_t luab_param_version;
size_t luab_param_bc_base_max;
size_t luab_param_bc_dim_max;
size_t luab_param_bc_scale_max;
size_t luab_param_bc_string_max;
size_t luab_param_coll_weights_max;
size_t luab_param_expr_nest_max;
size_t luab_param_line_max;
size_t luab_param_re_dup_max;
size_t luab_param_2_version;
size_t luab_param_2_c_bind;
size_t luab_param_2_c_dev;
size_t luab_param_2_char_term;
size_t luab_param_2_fort_dev;
size_t luab_param_2_fort_run;
size_t luab_param_2_localedef;
size_t luab_param_2_sw_dev;
size_t luab_param_2_upe;
size_t luab_param_stream_max;
size_t luab_param_tzname_max;

size_t luab_param_asynchronous_io;
size_t luab_param_mapped_files;
size_t luab_param_memlock;
size_t luab_param_memlock_range;
size_t luab_param_memory_protection;
size_t luab_param_message_passing;
size_t luab_param_prioritized_io;
size_t luab_param_priority_scheduling;
size_t luab_param_realtime_signals;
size_t luab_param_semaphores;
size_t luab_param_fsync;
size_t luab_param_shared_memory_objects;
size_t luab_param_synchronized_io;
size_t luab_param_timers;
size_t luab_param_aio_listio_max;
size_t luab_param_aio_max;
size_t luab_param_aio_prio_delta_max;
size_t luab_param_delaytimer_max;
size_t luab_param_mq_open_max;
size_t luab_param_pagesize;
size_t luab_param_rtsig_max;
size_t luab_param_sem_nsems_max;
size_t luab_param_sem_value_max;
size_t luab_param_sigqueue_max;
size_t luab_param_timer_max;

size_t luab_param_2_pbs;
size_t luab_param_2_pbs_accounting;
size_t luab_param_2_pbs_checkpoint;
size_t luab_param_2_pbs_locate;
size_t luab_param_2_pbs_message;
size_t luab_param_2_pbs_track;
size_t luab_param_advisory_info;
size_t luab_param_barriers;
size_t luab_param_clock_selection;
size_t luab_param_cputime;
size_t luab_param_file_locking;
size_t luab_param_getgr_r_size_max;
size_t luab_param_getpw_r_size_max;
size_t luab_param_host_name_max;
size_t luab_param_login_name_max;
size_t luab_param_monotonic_clock;
size_t luab_param_mq_prio_max;
size_t luab_param_reader_writer_locks;
size_t luab_param_regexp;
size_t luab_param_shell;
size_t luab_param_spawn;
size_t luab_param_spin_locks;
size_t luab_param_sporadic_server;
size_t luab_param_thread_attr_stackaddr;
size_t luab_param_thread_attr_stacksize;
size_t luab_param_thread_cputime;
size_t luab_param_thread_destructor_iterations;
size_t luab_param_thread_keys_max;
size_t luab_param_thread_prio_inherit;
size_t luab_param_thread_prio_protect;
size_t luab_param_thread_priority_scheduling;
size_t luab_param_thread_process_shared;
size_t luab_param_thread_safe_functions;
size_t luab_param_thread_sporadic_server;
size_t luab_param_thread_stack_min;
size_t luab_param_thread_threads_max;
size_t luab_param_timeouts;
size_t luab_param_threads;
size_t luab_param_trace;
size_t luab_param_trace_event_filter;
size_t luab_param_trace_inherit;
size_t luab_param_trace_log;
size_t luab_param_tty_name_max;
size_t luab_param_typed_memory_objects;
size_t luab_param_v6_ilp32_off32;
size_t luab_param_v6_ilp32_offbig;
size_t luab_param_v6_lp64_off64;
size_t luab_param_v6_lpbig_offbig;
size_t luab_param_ipv6;
size_t luab_param_raw_sockets;
size_t luab_param_symloop_max;

size_t luab_param_atexit_max;
size_t luab_param_iov_max;
size_t luab_param_page_size;
size_t luab_param_xopen_crypt;
size_t luab_param_xopen_enh_i18n;
size_t luab_param_xopen_legacy;
size_t luab_param_xopen_realtime;
size_t luab_param_xopen_realtime_threads;
size_t luab_param_xopen_shm;
size_t luab_param_xopen_streams;
size_t luab_param_xopen_unix;
size_t luab_param_xopen_version;
size_t luab_param_xopen_xcu_version;

size_t luab_param_nprocessors_conf;
size_t luab_param_nprocessors_onln;
size_t luab_param_cpuset_size;

size_t luab_param_phys_pages;
size_t luab_param_phys_pages;
