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

const lua_Integer luab_char_bit = LUAB_CHAR_BIT;

const lua_Integer luab_schar_max = LUAB_SCHAR_MAX;
const lua_Integer luab_schar_min = LUAB_SCHAR_MIN;
const lua_Integer luab_uchar_max = LUAB_UCHAR_MAX;

const lua_Integer luab_char_max = LUAB_CHAR_MAX;
const lua_Integer luab_char_min = CHAR_MIN;

const lua_Integer luab_ushrt_max = LUAB_USHRT_MAX;

const lua_Integer luab_shrt_max = LUAB_SHRT_MAX;
const lua_Integer luab_shrt_min = LUAB_SHRT_MIN;

const lua_Integer luab_uint_max = LUAB_UINT_MAX;
const lua_Integer luab_int_max = LUAB_INT_MAX;
const lua_Integer luab_int_min = LUAB_INT_MIN;

const lua_Integer luab_ulong_max = LUAB_ULONG_MAX;
const lua_Integer luab_long_max = LUAB_LONG_MAX;
const lua_Integer luab_long_min = LUAB_LONG_MIN;

const lua_Integer luab_ullong_max = LUAB_ULLONG_MAX;
const lua_Integer luab_llong_max = LUAB_LLONG_MAX;
const lua_Integer luab_llong_min = LUAB_LLONG_MIN;

const lua_Integer luab_ssize_max = LUAB_SSIZE_MAX;

const lua_Integer luab_size_t_max = LUAB_SIZE_T_MAX;
const lua_Integer luab_off_max = LUAB_OFF_MAX;
const lua_Integer luab_off_min = LUAB_OFF_MIN;

const lua_Integer luab_gid_max = LUAB_GID_MAX;
const lua_Integer luab_uid_max = LUAB_UID_MAX;
const lua_Integer luab_uquad_max = LUAB_UQUAD_MAX;
const lua_Integer luab_quad_max = LUAB_QUAD_MAX;
const lua_Integer luab_quad_min = LUAB_QUAD_MIN;

const lua_Integer luab_long_bit = LUAB_LONG_BIT;
const lua_Integer luab_word_bit = LUAB_WORD_BIT;

/*
 * XXX
 *  Constraints shall set by sysconf(3), if any.
 */

size_t luab_env_buf_max;
size_t luab_env_path_max;   /* XXX */
size_t luab_env_logname_max;

size_t luab_env_arg_max;
size_t luab_env_child_max;
size_t luab_env_clk_tck;
size_t luab_env_ngroups_max;
size_t luab_env_open_max;
size_t luab_env_job_control;
size_t luab_env_saved_ids;
size_t luab_env_version;
size_t luab_env_bc_base_max;
size_t luab_env_bc_dim_max;
size_t luab_env_bc_scale_max;
size_t luab_env_bc_string_max;
size_t luab_env_coll_weights_max;
size_t luab_env_expr_nest_max;
size_t luab_env_line_max;
size_t luab_env_re_dup_max;
size_t luab_env_2_version;
size_t luab_env_2_c_bind;
size_t luab_env_2_c_dev;
size_t luab_env_2_char_term;
size_t luab_env_2_fort_dev;
size_t luab_env_2_fort_run;
size_t luab_env_2_localedef;
size_t luab_env_2_sw_dev;
size_t luab_env_2_upe;
size_t luab_env_stream_max;
size_t luab_env_tzname_max;

size_t luab_env_asynchronous_io;
size_t luab_env_mapped_files;
size_t luab_env_memlock;
size_t luab_env_memlock_range;
size_t luab_env_memory_protection;
size_t luab_env_message_passing;
size_t luab_env_prioritized_io;
size_t luab_env_priority_scheduling;
size_t luab_env_realtime_signals;
size_t luab_env_semaphores;
size_t luab_env_fsync;
size_t luab_env_shared_memory_objects;
size_t luab_env_synchronized_io;
size_t luab_env_timers;
size_t luab_env_aio_listio_max;
size_t luab_env_aio_max;
size_t luab_env_aio_prio_delta_max;
size_t luab_env_delaytimer_max;
size_t luab_env_mq_open_max;
size_t luab_env_pagesize;
size_t luab_env_rtsig_max;
size_t luab_env_sem_nsems_max;
size_t luab_env_sem_value_max;
size_t luab_env_sigqueue_max;
size_t luab_env_timer_max;

size_t luab_env_2_pbs;
size_t luab_env_2_pbs_accounting;
size_t luab_env_2_pbs_checkpoint;
size_t luab_env_2_pbs_locate;
size_t luab_env_2_pbs_message;
size_t luab_env_2_pbs_track;
size_t luab_env_advisory_info;
size_t luab_env_barriers;
size_t luab_env_clock_selection;
size_t luab_env_cputime;
size_t luab_env_file_locking;
size_t luab_env_getgr_r_size_max;
size_t luab_env_getpw_r_size_max;
size_t luab_env_host_name_max;
size_t luab_env_login_name_max;
size_t luab_env_monotonic_clock;
size_t luab_env_mq_prio_max;
size_t luab_env_reader_writer_locks;
size_t luab_env_regexp;
size_t luab_env_shell;
size_t luab_env_spawn;
size_t luab_env_spin_locks;
size_t luab_env_sporadic_server;
size_t luab_env_thread_attr_stackaddr;
size_t luab_env_thread_attr_stacksize;
size_t luab_env_thread_cputime;
size_t luab_env_thread_destructor_iterations;
size_t luab_env_thread_keys_max;
size_t luab_env_thread_prio_inherit;
size_t luab_env_thread_prio_protect;
size_t luab_env_thread_priority_scheduling;
size_t luab_env_thread_process_shared;
size_t luab_env_thread_safe_functions;
size_t luab_env_thread_sporadic_server;
size_t luab_env_thread_stack_min;
size_t luab_env_thread_threads_max;
size_t luab_env_timeouts;
size_t luab_env_threads;
size_t luab_env_trace;
size_t luab_env_trace_event_filter;
size_t luab_env_trace_inherit;
size_t luab_env_trace_log;
size_t luab_env_tty_name_max;
size_t luab_env_typed_memory_objects;
size_t luab_env_v6_ilp32_off32;
size_t luab_env_v6_ilp32_offbig;
size_t luab_env_v6_lp64_off64;
size_t luab_env_v6_lpbig_offbig;
size_t luab_env_ipv6;
size_t luab_env_raw_sockets;
size_t luab_env_symloop_max;

size_t luab_env_atexit_max;
size_t luab_env_iov_max;
size_t luab_env_page_size;
size_t luab_env_xopen_crypt;
size_t luab_env_xopen_enh_i18n;
size_t luab_env_xopen_legacy;
size_t luab_env_xopen_realtime;
size_t luab_env_xopen_realtime_threads;
size_t luab_env_xopen_shm;
size_t luab_env_xopen_streams;
size_t luab_env_xopen_unix;
size_t luab_env_xopen_version;
size_t luab_env_xopen_xcu_version;

size_t luab_env_nprocessors_conf;
size_t luab_env_nprocessors_onln;
size_t luab_env_cpuset_size;

size_t luab_env_phys_pages;
size_t luab_env_phys_pages;
