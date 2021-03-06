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

#include <net/if.h>

#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_table.h"
#include "luab_modules.h"

const lua_Integer luab_env_error = LUAB_ERROR;
const lua_Integer luab_env_success = LUAB_SUCCESS;

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
 * Constraints are initialized by sysconf(3), if any.
 */

u_long luab_env_buf_max;
u_long luab_env_path_max;       /* XXX */
u_long luab_env_logname_max;
u_long luab_env_specname_max;
u_long luab_env_ifname_max;
u_long luab_env_name_max;
u_long luab_env_passwd_max;
u_long luab_env_ts_min;

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

/*
 * Vector table - constraints, etc.
 */

luab_sysconf_vec_t luab_env_param[] = {
    {                                               /* <core> */
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = LUAB_BUFFERSIZE,
        .scv_val = &luab_env_buf_max,
    },{
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = LUAB_MAXPATHLEN,
        .scv_val = &luab_env_path_max,
    },{
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = LUAB_BUFFERSIZE,
        .scv_val = &luab_env_logname_max,
    },{
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = LUAB_MAXPATHLEN,
        .scv_val = &luab_env_path_max,
    },{
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = LUAB_SPECNAMELEN,
        .scv_val = &luab_env_specname_max,
    },{
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = IF_NAMESIZE,
        .scv_val = &luab_env_ifname_max,
    },{
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = LUAB_NAME_MAX,
        .scv_val = &luab_env_name_max,
    },{
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = LUAB_PASSWD_MAX,
        .scv_val = &luab_env_passwd_max,
    },{
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = LUAB_PASSWD_MAX,
        .scv_val = &luab_env_ifname_max,
    },{
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = LUAB_TS_MIN,
        .scv_val = &luab_env_ts_min,
    },{                                             /* <unistd.h> */
        .scv_key = LUAB_SC_ARG_MAX,
        .scv_dflt = ARG_MAX,
        .scv_val = &luab_env_arg_max,
    },{
        .scv_key = LUAB_SC_CHILD_MAX,
        .scv_dflt = CHILD_MAX,
        .scv_val = &luab_env_child_max,
    },{
        .scv_key = LUAB_SC_CLK_TCK,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_clk_tck,
    },{
        .scv_key = LUAB_SC_NGROUPS_MAX,
        .scv_dflt = NGROUPS_MAX,
        .scv_val = &luab_env_ngroups_max,
    },{
        .scv_key = LUAB_SC_OPEN_MAX,
        .scv_dflt = OPEN_MAX,
        .scv_val = &luab_env_open_max,
    },{
        .scv_key = LUAB_SC_JOB_CONTROL,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_job_control,
    },{
        .scv_key = LUAB_SC_SAVED_IDS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_saved_ids,
    },{
        .scv_key = LUAB_SC_VERSION,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_version,
    },{
        .scv_key = LUAB_SC_BC_BASE_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_bc_base_max,
    },{
        .scv_key = LUAB_SC_BC_DIM_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_bc_dim_max,
    },{
        .scv_key = LUAB_SC_BC_SCALE_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_bc_scale_max,
    },{
        .scv_key = LUAB_SC_BC_STRING_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_bc_string_max,
    },{
        .scv_key = LUAB_SC_COLL_WEIGHTS_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_coll_weights_max,
    },{
        .scv_key = LUAB_SC_EXPR_NEST_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_expr_nest_max,
    },{
        .scv_key = LUAB_SC_LINE_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_line_max,
    },{
        .scv_key = LUAB_SC_RE_DUP_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_re_dup_max,
    },{
        .scv_key = LUAB_SC_2_VERSION,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_version,
    },{
        .scv_key = LUAB_SC_2_C_BIND,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_c_bind,
    },{
        .scv_key = LUAB_SC_2_C_DEV,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_c_dev,
    },{
        .scv_key = LUAB_SC_2_CHAR_TERM,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_char_term,
    },{
        .scv_key = LUAB_SC_2_FORT_DEV,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_fort_dev,
    },{
        .scv_key = LUAB_SC_2_FORT_RUN,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_fort_run,
    },{
        .scv_key = LUAB_SC_2_LOCALEDEF,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_localedef,
    },{
        .scv_key = LUAB_SC_2_SW_DEV,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_sw_dev,
    },{
        .scv_key = LUAB_SC_2_UPE,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_upe,
    },{
        .scv_key = LUAB_SC_STREAM_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_stream_max,
    },{
        .scv_key = LUAB_SC_TZNAME_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_tzname_max,
    },{
        .scv_key = LUAB_SC_ASYNCHRONOUS_IO,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_asynchronous_io,
    },{
        .scv_key = LUAB_SC_MAPPED_FILES,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_mapped_files,
    },{
        .scv_key = LUAB_SC_MEMLOCK,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_memlock,
    },{
        .scv_key = LUAB_SC_MEMLOCK_RANGE,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_memlock_range,
    },{
        .scv_key = LUAB_SC_MEMORY_PROTECTION,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_memory_protection,
    },{
        .scv_key = LUAB_SC_MESSAGE_PASSING,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_message_passing,
    },{
        .scv_key = LUAB_SC_PRIORITIZED_IO,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_prioritized_io,
    },{
        .scv_key = LUAB_SC_PRIORITY_SCHEDULING,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_priority_scheduling,
    },{
        .scv_key = LUAB_SC_REALTIME_SIGNALS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_realtime_signals,
    },{
        .scv_key = LUAB_SC_SEMAPHORES,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_semaphores,
    },{
        .scv_key = LUAB_SC_FSYNC,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_fsync,
    },{
        .scv_key = LUAB_SC_SHARED_MEMORY_OBJECTS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_shared_memory_objects,
    },{
        .scv_key = LUAB_SC_SYNCHRONIZED_IO,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_synchronized_io,
    },{
        .scv_key = LUAB_SC_TIMERS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_timers,
    },{
        .scv_key = LUAB_SC_AIO_LISTIO_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_aio_listio_max,
    },{
        .scv_key = LUAB_SC_AIO_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_aio_max,
    },{
        .scv_key = LUAB_SC_AIO_PRIO_DELTA_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_aio_prio_delta_max,
    },{
        .scv_key = LUAB_SC_DELAYTIMER_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_delaytimer_max,
    },{
        .scv_key = LUAB_SC_MQ_OPEN_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_mq_open_max,
    },{
        .scv_key = LUAB_SC_PAGESIZE,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_pagesize,
    },{
        .scv_key = LUAB_SC_RTSIG_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_rtsig_max,
    },{
        .scv_key = LUAB_SC_SEM_NSEMS_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_sem_nsems_max,
    },{
        .scv_key = LUAB_SC_SEM_VALUE_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_sem_value_max,
    },{
        .scv_key = LUAB_SC_SIGQUEUE_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_sigqueue_max,
    },{
        .scv_key = LUAB_SC_TIMER_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_timer_max,
    },{
        .scv_key = LUAB_SC_2_PBS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_pbs,
    },{
        .scv_key = LUAB_SC_2_PBS_ACCOUNTING,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_pbs_accounting,
    },{
        .scv_key = LUAB_SC_2_PBS_CHECKPOINT,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_pbs_checkpoint,
    },{
        .scv_key = LUAB_SC_2_PBS_LOCATE,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_pbs_locate,
    },{
        .scv_key = LUAB_SC_2_PBS_MESSAGE,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_pbs_message,
    },{
        .scv_key = LUAB_SC_2_PBS_TRACK,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_2_pbs_track,
    },{
        .scv_key = LUAB_SC_ADVISORY_INFO,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_advisory_info,
    },{
        .scv_key = LUAB_SC_BARRIERS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_barriers,
    },{
        .scv_key = LUAB_SC_CLOCK_SELECTION,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_clock_selection,
    },{
        .scv_key = LUAB_SC_CPUTIME,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_cputime,
    },{
        .scv_key = LUAB_SC_FILE_LOCKING,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_file_locking,
    },{
        .scv_key = LUAB_SC_GETGR_R_SIZE_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_getgr_r_size_max,
    },{
        .scv_key = LUAB_SC_GETPW_R_SIZE_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_getpw_r_size_max,
    },{
        .scv_key = LUAB_SC_HOST_NAME_MAX,
        .scv_dflt = MAXHOSTNAMELEN,
        .scv_val = &luab_env_host_name_max,
    },{
        .scv_key = LUAB_SC_LOGIN_NAME_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_login_name_max,
    },{
        .scv_key = LUAB_SC_MONOTONIC_CLOCK,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_monotonic_clock,
    },{
        .scv_key = LUAB_SC_MQ_PRIO_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_mq_prio_max,
    },{
        .scv_key = LUAB_SC_READER_WRITER_LOCKS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_reader_writer_locks,
    },{
        .scv_key = LUAB_SC_REGEXP,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_regexp,
    },{
        .scv_key = LUAB_SC_SHELL,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_shell,
    },{
        .scv_key = LUAB_SC_SPAWN,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_spawn,
    },{
        .scv_key = LUAB_SC_SPIN_LOCKS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_spin_locks,
    },{
        .scv_key = LUAB_SC_SPORADIC_SERVER,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_sporadic_server,
    },{
        .scv_key = LUAB_SC_THREAD_ATTR_STACKADDR,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_thread_attr_stackaddr,
    },{
        .scv_key = LUAB_SC_THREAD_ATTR_STACKSIZE,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_thread_attr_stacksize,
    },{
        .scv_key = LUAB_SC_THREAD_CPUTIME,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_thread_cputime,
    },{
        .scv_key = LUAB_SC_THREAD_DESTRUCTOR_ITERATIONS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_thread_destructor_iterations,
    },{
        .scv_key = LUAB_SC_THREAD_KEYS_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_thread_keys_max,
    },{
        .scv_key = LUAB_SC_THREAD_PRIO_INHERIT,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_thread_prio_inherit,
    },{
        .scv_key = LUAB_SC_THREAD_PRIO_PROTECT,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_thread_prio_protect,
    },{
        .scv_key = LUAB_SC_THREAD_PRIORITY_SCHEDULING,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_thread_priority_scheduling,
    },{
        .scv_key = LUAB_SC_THREAD_PROCESS_SHARED,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_thread_process_shared,
    },{
        .scv_key = LUAB_SC_THREAD_SAFE_FUNCTIONS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_thread_safe_functions,
    },{
        .scv_key = LUAB_SC_THREAD_SPORADIC_SERVER,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_thread_sporadic_server,
    },{
        .scv_key = LUAB_SC_THREAD_STACK_MIN,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_thread_stack_min,
    },{
        .scv_key = LUAB_SC_THREAD_THREADS_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_thread_threads_max,
    },{
        .scv_key = LUAB_SC_TIMEOUTS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_timeouts,
    },{
        .scv_key = LUAB_SC_THREADS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_threads,
    },{
        .scv_key = LUAB_SC_TRACE,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_trace,
    },{
        .scv_key = LUAB_SC_TRACE_EVENT_FILTER,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_trace_event_filter,
    },{
        .scv_key = LUAB_SC_TRACE_INHERIT,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_trace_inherit,
    },{
        .scv_key = LUAB_SC_TRACE_LOG,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_trace_log,
    },{
        .scv_key = LUAB_SC_TTY_NAME_MAX,
        .scv_dflt = LUAB_SPECNAMELEN,
        .scv_val = &luab_env_tty_name_max,
    },{
        .scv_key = LUAB_SC_TYPED_MEMORY_OBJECTS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_typed_memory_objects,
    },{
        .scv_key = LUAB_SC_V6_ILP32_OFF32,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_v6_ilp32_off32,
    },{
        .scv_key = LUAB_SC_V6_ILP32_OFFBIG,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_v6_ilp32_offbig,
    },{
        .scv_key = LUAB_SC_V6_LP64_OFF64,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_v6_lp64_off64,
    },{
        .scv_key = LUAB_SC_V6_LPBIG_OFFBIG,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_v6_lpbig_offbig,
    },{
        .scv_key = LUAB_SC_IPV6,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_ipv6,
    },{
        .scv_key = LUAB_SC_RAW_SOCKETS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_raw_sockets,
    },{
        .scv_key = LUAB_SC_SYMLOOP_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_symloop_max,
    },{
        .scv_key = LUAB_SC_ATEXIT_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_atexit_max,
    },{
        .scv_key = LUAB_SC_IOV_MAX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_iov_max,
    },{
        .scv_key = LUAB_SC_PAGE_SIZE,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_page_size,
    },{
        .scv_key = LUAB_SC_XOPEN_CRYPT,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_xopen_crypt,
    },{
        .scv_key = LUAB_SC_XOPEN_ENH_I18N,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_xopen_enh_i18n,
    },{
        .scv_key = LUAB_SC_XOPEN_LEGACY,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_xopen_legacy,
    },{
        .scv_key = LUAB_SC_XOPEN_REALTIME,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_xopen_realtime,
    },{
        .scv_key = LUAB_SC_XOPEN_REALTIME_THREADS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_xopen_realtime_threads,
    },{
        .scv_key = LUAB_SC_XOPEN_SHM,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_xopen_shm,
    },{
        .scv_key = LUAB_SC_XOPEN_STREAMS,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_xopen_streams,
    },{
        .scv_key = LUAB_SC_XOPEN_UNIX,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_xopen_unix,
    },{
        .scv_key = LUAB_SC_XOPEN_VERSION,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_xopen_version,
    },{
        .scv_key = LUAB_SC_XOPEN_XCU_VERSION,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_xopen_xcu_version,
    },{
        .scv_key = LUAB_SC_NPROCESSORS_CONF,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_nprocessors_conf,
    },{
        .scv_key = LUAB_SC_NPROCESSORS_ONLN,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_nprocessors_onln,
    },{
        .scv_key = LUAB_SC_CPUSET_SIZE,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_cpuset_size,
    },{
        .scv_key = LUAB_SC_PHYS_PAGES,
        .scv_dflt = LUAB_SCX_DFLT,
        .scv_val = &luab_env_phys_pages,
    },
    LUAB_SC_VEC_SENTINEL
};

/*
 * Common subr. for initializiation, those are
 * called during runtime of package.loadlib().
 */

void
luab_env_populate(lua_State *L, int narg, luab_module_t *m)
{
    luab_module_table_t *tok;

    if ((tok = m->m_vec) != NULL) {

        do {
            if (tok->mt_init != NULL) {
                (void)(*tok->mt_init)(L, &tok->mt_val);
                lua_setfield(L, narg, tok->mt_key);
            } else
                errno = ENOENT;

            tok++;
        } while (tok->mt_key != NULL);

        lua_pop(L, 0);
    } else
        luab_core_err(EX_DATAERR, __func__, ENXIO);
}

void
luab_env_newtable(lua_State *L, int narg, luab_module_t *m)
{
    if (m != NULL) {
        luab_table_init(L, 1);
        luab_env_populate(L, narg, m);
        lua_setfield(L, narg, m->m_name);
    } else
        luab_core_err(EX_DATAERR, __func__, ENOEXEC);
}

void
luab_env_newmetatable(lua_State *L, int narg, luab_module_t *m)
{
    if (m != NULL) {
        luaL_newmetatable(L, m->m_name);

        if (narg < 0)
            lua_pushvalue(L, narg + 1);
        else
            lua_pushvalue(L, narg - 1);

        lua_setfield(L, narg, "__index");
        luab_env_populate(L, narg, m);

        lua_pop(L, 1);
    } else
        luab_core_err(EX_DATAERR, __func__, ENOEXEC);
}

void
luab_env_initmodule(lua_State *L, int narg, luab_module_vec_t *vec,
    const char *name, int new)
{
    luab_module_vec_t *mv;

    if ((mv = vec) != NULL) {

        if (name != NULL && new != 0)
            luab_table_init(L, 1);

        do {
            if (mv->mv_init != NULL)
                (*mv->mv_init)(L, narg, mv->mv_mod);
            else
                errno = ENOENT;

            mv++;
        } while (mv->mv_mod != NULL);

        if (name != NULL && new != 0)
            lua_setfield(L, narg, name);
    } else
        luab_core_err(EX_DATAERR, __func__, ENXIO);
}

void
luab_env_registerlib(lua_State *L, int narg, luab_module_vec_t *vec, const char *name)
{
    luab_env_initmodule(L, narg, vec, name, 1);
}

void
luab_env_registertype(lua_State *L, int narg, luab_module_vec_t *vec)
{
    luab_env_initmodule(L, narg, vec, NULL, 0);
}

/*
 * Selector over module/type vector-table set.
 */

luab_module_t *
luab_env_checkmodule(luab_type_t idx, uint32_t id, const char *fname)
{
    luab_module_t *m;

    if ((m = luab_xmv(idx).mv_mod) != NULL) {

        if (m->m_id == id)
            return (m);
        else
            luab_core_err(EX_UNAVAILABLE, fname, ENOENT);
    } else
        luab_core_err(EX_UNAVAILABLE, fname, ENOSYS);

    return (NULL);
}
