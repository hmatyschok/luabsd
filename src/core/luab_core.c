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
 * Vector table - constraints, etc.
 */
 
static luab_sysconf_vec_t luab_param[] = {
    {                                               /* <core> */
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = LUAL_BUFFERSIZE,
        .scv_val = &luab_env_buf_max,
    },{
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = MAXPATHLEN,
        .scv_val = &luab_env_path_max,
    },{
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = LUAL_BUFFERSIZE,
        .scv_val = &luab_env_logname_max,
    },{
        .scv_key = LUAB_SC_NOTSUPP,
        .scv_dflt = MAXPATHLEN,
        .scv_val = &luab_env_path_max,
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
        .scv_dflt = 0,
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
        .scv_dflt = 0,
        .scv_val = &luab_env_job_control,
    },{
        .scv_key = LUAB_SC_SAVED_IDS,
        .scv_dflt = 0,
        .scv_val = &luab_env_saved_ids,
    },{
        .scv_key = LUAB_SC_VERSION,
        .scv_dflt = 0,
        .scv_val = &luab_env_version,
    },{
        .scv_key = LUAB_SC_BC_BASE_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_bc_base_max,
    },{
        .scv_key = LUAB_SC_BC_DIM_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_bc_dim_max,
    },{
        .scv_key = LUAB_SC_BC_SCALE_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_bc_scale_max,
    },{
        .scv_key = LUAB_SC_BC_STRING_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_bc_string_max,
    },{
        .scv_key = LUAB_SC_COLL_WEIGHTS_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_coll_weights_max,
    },{
        .scv_key = LUAB_SC_EXPR_NEST_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_expr_nest_max,
    },{
        .scv_key = LUAB_SC_LINE_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_line_max,
    },{
        .scv_key = LUAB_SC_RE_DUP_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_re_dup_max,
    },{
        .scv_key = LUAB_SC_2_VERSION,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_version,
    },{
        .scv_key = LUAB_SC_2_C_BIND,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_c_bind,
    },{
        .scv_key = LUAB_SC_2_C_DEV,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_c_dev,
    },{
        .scv_key = LUAB_SC_2_CHAR_TERM,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_char_term,
    },{
        .scv_key = LUAB_SC_2_FORT_DEV,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_fort_dev,
    },{
        .scv_key = LUAB_SC_2_FORT_RUN,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_fort_run,
    },{
        .scv_key = LUAB_SC_2_LOCALEDEF,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_localedef,
    },{
        .scv_key = LUAB_SC_2_SW_DEV,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_sw_dev,
    },{
        .scv_key = LUAB_SC_2_UPE,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_upe,
    },{
        .scv_key = LUAB_SC_STREAM_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_stream_max,
    },{
        .scv_key = LUAB_SC_TZNAME_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_tzname_max,
    },{
        .scv_key = LUAB_SC_ASYNCHRONOUS_IO,
        .scv_dflt = 0,
        .scv_val = &luab_env_asynchronous_io,
    },{
        .scv_key = LUAB_SC_MAPPED_FILES,
        .scv_dflt = 0,
        .scv_val = &luab_env_mapped_files,
    },{
        .scv_key = LUAB_SC_MEMLOCK,
        .scv_dflt = 0,
        .scv_val = &luab_env_memlock,
    },{
        .scv_key = LUAB_SC_MEMLOCK_RANGE,
        .scv_dflt = 0,
        .scv_val = &luab_env_memlock_range,
    },{
        .scv_key = LUAB_SC_MEMORY_PROTECTION,
        .scv_dflt = 0,
        .scv_val = &luab_env_memory_protection,
    },{
        .scv_key = LUAB_SC_MESSAGE_PASSING,
        .scv_dflt = 0,
        .scv_val = &luab_env_message_passing,
    },{
        .scv_key = LUAB_SC_PRIORITIZED_IO,
        .scv_dflt = 0,
        .scv_val = &luab_env_prioritized_io,
    },{
        .scv_key = LUAB_SC_PRIORITY_SCHEDULING,
        .scv_dflt = 0,
        .scv_val = &luab_env_priority_scheduling,
    },{
        .scv_key = LUAB_SC_REALTIME_SIGNALS,
        .scv_dflt = 0,
        .scv_val = &luab_env_realtime_signals,
    },{
        .scv_key = LUAB_SC_SEMAPHORES,
        .scv_dflt = 0,
        .scv_val = &luab_env_semaphores,
    },{
        .scv_key = LUAB_SC_FSYNC,
        .scv_dflt = 0,
        .scv_val = &luab_env_fsync,
    },{
        .scv_key = LUAB_SC_SHARED_MEMORY_OBJECTS,
        .scv_dflt = 0,
        .scv_val = &luab_env_shared_memory_objects,
    },{
        .scv_key = LUAB_SC_SYNCHRONIZED_IO,
        .scv_dflt = 0,
        .scv_val = &luab_env_synchronized_io,
    },{
        .scv_key = LUAB_SC_TIMERS,
        .scv_dflt = 0,
        .scv_val = &luab_env_timers,
    },{
        .scv_key = LUAB_SC_AIO_LISTIO_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_aio_listio_max,
    },{
        .scv_key = LUAB_SC_AIO_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_aio_max,
    },{
        .scv_key = LUAB_SC_AIO_PRIO_DELTA_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_aio_prio_delta_max,
    },{
        .scv_key = LUAB_SC_DELAYTIMER_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_delaytimer_max,
    },{
        .scv_key = LUAB_SC_MQ_OPEN_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_mq_open_max,
    },{
        .scv_key = LUAB_SC_PAGESIZE,
        .scv_dflt = 0,
        .scv_val = &luab_env_pagesize,
    },{
        .scv_key = LUAB_SC_RTSIG_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_rtsig_max,
    },{
        .scv_key = LUAB_SC_SEM_NSEMS_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_sem_nsems_max,
    },{
        .scv_key = LUAB_SC_SEM_VALUE_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_sem_value_max,
    },{
        .scv_key = LUAB_SC_SIGQUEUE_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_sigqueue_max,
    },{
        .scv_key = LUAB_SC_TIMER_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_timer_max,
    },{
        .scv_key = LUAB_SC_2_PBS,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_pbs,
    },{
        .scv_key = LUAB_SC_2_PBS_ACCOUNTING,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_pbs_accounting,
    },{
        .scv_key = LUAB_SC_2_PBS_CHECKPOINT,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_pbs_checkpoint,
    },{
        .scv_key = LUAB_SC_2_PBS_LOCATE,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_pbs_locate,
    },{
        .scv_key = LUAB_SC_2_PBS_MESSAGE,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_pbs_message,
    },{
        .scv_key = LUAB_SC_2_PBS_TRACK,
        .scv_dflt = 0,
        .scv_val = &luab_env_2_pbs_track,
    },{
        .scv_key = LUAB_SC_ADVISORY_INFO,
        .scv_dflt = 0,
        .scv_val = &luab_env_advisory_info,
    },{
        .scv_key = LUAB_SC_BARRIERS,
        .scv_dflt = 0,
        .scv_val = &luab_env_barriers,
    },{
        .scv_key = LUAB_SC_CLOCK_SELECTION,
        .scv_dflt = 0,
        .scv_val = &luab_env_clock_selection,
    },{
        .scv_key = LUAB_SC_CPUTIME,
        .scv_dflt = 0,
        .scv_val = &luab_env_cputime,
    },{
        .scv_key = LUAB_SC_FILE_LOCKING,
        .scv_dflt = 0,
        .scv_val = &luab_env_file_locking,
    },{
        .scv_key = LUAB_SC_GETGR_R_SIZE_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_getgr_r_size_max,
    },{
        .scv_key = LUAB_SC_GETPW_R_SIZE_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_getpw_r_size_max,
    },{
        .scv_key = LUAB_SC_HOST_NAME_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_host_name_max,
    },{
        .scv_key = LUAB_SC_LOGIN_NAME_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_login_name_max,
    },{
        .scv_key = LUAB_SC_MONOTONIC_CLOCK,
        .scv_dflt = 0,
        .scv_val = &luab_env_monotonic_clock,
    },{
        .scv_key = LUAB_SC_MQ_PRIO_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_mq_prio_max,
    },{
        .scv_key = LUAB_SC_READER_WRITER_LOCKS,
        .scv_dflt = 0,
        .scv_val = &luab_env_reader_writer_locks,
    },{
        .scv_key = LUAB_SC_REGEXP,
        .scv_dflt = 0,
        .scv_val = &luab_env_regexp,
    },{
        .scv_key = LUAB_SC_SHELL,
        .scv_dflt = 0,
        .scv_val = &luab_env_shell,
    },{
        .scv_key = LUAB_SC_SPAWN,
        .scv_dflt = 0,
        .scv_val = &luab_env_spawn,
    },{
        .scv_key = LUAB_SC_SPIN_LOCKS,
        .scv_dflt = 0,
        .scv_val = &luab_env_spin_locks,
    },{
        .scv_key = LUAB_SC_SPORADIC_SERVER,
        .scv_dflt = 0,
        .scv_val = &luab_env_sporadic_server,
    },{
        .scv_key = LUAB_SC_THREAD_ATTR_STACKADDR,
        .scv_dflt = 0,
        .scv_val = &luab_env_thread_attr_stackaddr,
    },{
        .scv_key = LUAB_SC_THREAD_ATTR_STACKSIZE,
        .scv_dflt = 0,
        .scv_val = &luab_env_thread_attr_stacksize,
    },{
        .scv_key = LUAB_SC_THREAD_CPUTIME,
        .scv_dflt = 0,
        .scv_val = &luab_env_thread_cputime,
    },{
        .scv_key = LUAB_SC_THREAD_DESTRUCTOR_ITERATIONS,
        .scv_dflt = 0,
        .scv_val = &luab_env_thread_destructor_iterations,
    },{
        .scv_key = LUAB_SC_THREAD_KEYS_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_thread_keys_max,
    },{
        .scv_key = LUAB_SC_THREAD_PRIO_INHERIT,
        .scv_dflt = 0,
        .scv_val = &luab_env_thread_prio_inherit,
    },{
        .scv_key = LUAB_SC_THREAD_PRIO_PROTECT,
        .scv_dflt = 0,
        .scv_val = &luab_env_thread_prio_protect,
    },{
        .scv_key = LUAB_SC_THREAD_PRIORITY_SCHEDULING,
        .scv_dflt = 0,
        .scv_val = &luab_env_thread_priority_scheduling,
    },{
        .scv_key = LUAB_SC_THREAD_PROCESS_SHARED,
        .scv_dflt = 0,
        .scv_val = &luab_env_thread_process_shared,
    },{
        .scv_key = LUAB_SC_THREAD_SAFE_FUNCTIONS,
        .scv_dflt = 0,
        .scv_val = &luab_env_thread_safe_functions,
    },{
        .scv_key = LUAB_SC_THREAD_SPORADIC_SERVER,
        .scv_dflt = 0,
        .scv_val = &luab_env_thread_sporadic_server,
    },{
        .scv_key = LUAB_SC_THREAD_STACK_MIN,
        .scv_dflt = 0,
        .scv_val = &luab_env_thread_stack_min,
    },{
        .scv_key = LUAB_SC_THREAD_THREADS_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_thread_threads_max,
    },{
        .scv_key = LUAB_SC_TIMEOUTS,
        .scv_dflt = 0,
        .scv_val = &luab_env_timeouts,
    },{
        .scv_key = LUAB_SC_THREADS,
        .scv_dflt = 0,
        .scv_val = &luab_env_threads,
    },{
        .scv_key = LUAB_SC_TRACE,
        .scv_dflt = 0,
        .scv_val = &luab_env_trace,
    },{
        .scv_key = LUAB_SC_TRACE_EVENT_FILTER,
        .scv_dflt = 0,
        .scv_val = &luab_env_trace_event_filter,
    },{
        .scv_key = LUAB_SC_TRACE_INHERIT,
        .scv_dflt = 0,
        .scv_val = &luab_env_trace_inherit,
    },{
        .scv_key = LUAB_SC_TRACE_LOG,
        .scv_dflt = 0,
        .scv_val = &luab_env_trace_log,
    },{
        .scv_key = LUAB_SC_TTY_NAME_MAX,
        .scv_dflt = SPECNAMELEN,
        .scv_val = &luab_env_tty_name_max,
    },{
        .scv_key = LUAB_SC_TYPED_MEMORY_OBJECTS,
        .scv_dflt = 0,
        .scv_val = &luab_env_typed_memory_objects,
    },{
        .scv_key = LUAB_SC_V6_ILP32_OFF32,
        .scv_dflt = 0,
        .scv_val = &luab_env_v6_ilp32_off32,
    },{
        .scv_key = LUAB_SC_V6_ILP32_OFFBIG,
        .scv_dflt = 0,
        .scv_val = &luab_env_v6_ilp32_offbig,
    },{
        .scv_key = LUAB_SC_V6_LP64_OFF64,
        .scv_dflt = 0,
        .scv_val = &luab_env_v6_lp64_off64,
    },{
        .scv_key = LUAB_SC_V6_LPBIG_OFFBIG,
        .scv_dflt = 0,
        .scv_val = &luab_env_v6_lpbig_offbig,
    },{
        .scv_key = LUAB_SC_IPV6,
        .scv_dflt = 0,
        .scv_val = &luab_env_ipv6,
    },{
        .scv_key = LUAB_SC_RAW_SOCKETS,
        .scv_dflt = 0,
        .scv_val = &luab_env_raw_sockets,
    },{
        .scv_key = LUAB_SC_SYMLOOP_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_symloop_max,
    },{
        .scv_key = LUAB_SC_ATEXIT_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_atexit_max,
    },{
        .scv_key = LUAB_SC_IOV_MAX,
        .scv_dflt = 0,
        .scv_val = &luab_env_iov_max,
    },{
        .scv_key = LUAB_SC_PAGE_SIZE,
        .scv_dflt = 0,
        .scv_val = &luab_env_page_size,
    },{
        .scv_key = LUAB_SC_XOPEN_CRYPT,
        .scv_dflt = 0,
        .scv_val = &luab_env_xopen_crypt,
    },{
        .scv_key = LUAB_SC_XOPEN_ENH_I18N,
        .scv_dflt = 0,
        .scv_val = &luab_env_xopen_enh_i18n,
    },{
        .scv_key = LUAB_SC_XOPEN_LEGACY,
        .scv_dflt = 0,
        .scv_val = &luab_env_xopen_legacy,
    },{
        .scv_key = LUAB_SC_XOPEN_REALTIME,
        .scv_dflt = 0,
        .scv_val = &luab_env_xopen_realtime,
    },{
        .scv_key = LUAB_SC_XOPEN_REALTIME_THREADS,
        .scv_dflt = 0,
        .scv_val = &luab_env_xopen_realtime_threads,
    },{
        .scv_key = LUAB_SC_XOPEN_SHM,
        .scv_dflt = 0,
        .scv_val = &luab_env_xopen_shm,
    },{
        .scv_key = LUAB_SC_XOPEN_STREAMS,
        .scv_dflt = 0,
        .scv_val = &luab_env_xopen_streams,
    },{
        .scv_key = LUAB_SC_XOPEN_UNIX,
        .scv_dflt = 0,
        .scv_val = &luab_env_xopen_unix,
    },{
        .scv_key = LUAB_SC_XOPEN_VERSION,
        .scv_dflt = 0,
        .scv_val = &luab_env_xopen_version,
    },{
        .scv_key = LUAB_SC_XOPEN_XCU_VERSION,
        .scv_dflt = 0,
        .scv_val = &luab_env_xopen_xcu_version,
    },{
        .scv_key = LUAB_SC_NPROCESSORS_CONF,
        .scv_dflt = 0,
        .scv_val = &luab_env_nprocessors_conf,
    },{
        .scv_key = LUAB_SC_NPROCESSORS_ONLN,
        .scv_dflt = 0,
        .scv_val = &luab_env_nprocessors_onln,
    },{
        .scv_key = LUAB_SC_CPUSET_SIZE,
        .scv_dflt = 0,
        .scv_val = &luab_env_cpuset_size,
    },{
        .scv_key = LUAB_SC_PHYS_PAGES,
        .scv_dflt = 0,
        .scv_val = &luab_env_phys_pages,
    },
    LUAB_SC_VEC_SENTINEL
};

/*
 * Common subr. for initializiation, those are
 * called during runtime of package.loadlib().
 */

static void
luab_core_envinit(luab_sysconf_vec_t *vec)
{
    luab_sysconf_vec_t *tok;
    long scx;

    for (tok = vec; tok->scv_val != NULL; tok++) {

        if ((scx = sysconf(tok->scv_key)) < 0)
            *(tok->scv_val) = tok->scv_dflt;
        else
            *(tok->scv_val) = (u_long)scx;
    }
}

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

/* Bindings against atomic / composite data types. */
luab_module_vec_t luab_typevec[] = {
    {
        .mv_mod = &luab_int_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_INT_IDX,
    },{
        .mv_mod = &luab_ushrt_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_USHRT_IDX,
    },{
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
        .mv_mod = &luab_socklen_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_SOCKLEN_IDX,
    },{
        .mv_mod = &luab_uid_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_UID_IDX,
    },{
        .mv_mod = &luab_double_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_DOUBLE_IDX,
    },{
        .mv_mod = &luab_float_type,
        .mv_init = luab_core_newmetatable,
        .mv_idx = LUAB_FLOAT_IDX,
    },
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

    /* initialize constraints */
    luab_core_envinit(luab_param);

    /* register modules */
    lua_newtable(L);

    luab_core_registerlib(L, -2, luab_arpa_vec,  "arpa");
    luab_core_registerlib(L, -2, luab_net_vec,   "net");
    luab_core_registerlib(L, -2, luab_sys_vec,   "sys");
    luab_core_registerlib(L, -2, luab_core_vec,  NULL);

    lua_pushvalue(L, -1);

    /* register complex data-types. */
    luab_core_registertype(L, -2, luab_typevec);

    return (1);
}
