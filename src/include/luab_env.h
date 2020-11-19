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

#ifndef _LUAB_ENV_H_
#define _LUAB_ENV_H_

#define LUAB_CHAR_BIT                           CHAR_BIT;

#define LUAB_SCHAR_MAX                          SCHAR_MAX
#define LUAB_SCHAR_MIN                          SCHAR_MIN
#define LUAB_UCHAR_MAX                          UCHAR_MAX

#define LUAB_CHAR_MAX                           CHAR_MAX
#define LUAB_CHAR_MIN                           CHAR_MIN

#define LUAB_USHRT_MAX                          USHRT_MAX
#define LUAB_SHRT_MAX                           SHRT_MAX
#define LUAB_SHRT_MIN                           SHRT_MIN

#define LUAB_UINT_MAX                           UINT_MAX
#define LUAB_INT_MAX                            INT_MAX
#define LUAB_INT_MIN                            INT_MIN

#define LUAB_ULONG_MAX                          ULONG_MAX
#define LUAB_LONG_MAX                           LONG_MAX
#define LUAB_LONG_MIN                           LONG_MIN

#ifdef __LONG_LONG_SUPPORTED
#define LUAB_ULLONG_MAX                         ULLONG_MAX
#define LUAB_LLONG_MAX                          LLONG_MAX
#define LUAB_LLONG_MIN                          LLONG_MIN
#else
#define LUAB_ULLONG_MAX                         0
#define LUAB_LLONG_MAX                          0
#define LUAB_LLONG_MIN                          0
#endif

#if __POSIX_VISIBLE || __XSI_VISIBLE
#define LUAB_SSIZE_MAX                          SSIZE_MAX
#else
#define LUAB_SSIZE_MAX                          0
#endif

#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
#define LUAB_SIZE_T_MAX                         SIZE_T_MAX
#define LUAB_OFF_MAX                            OFF_MAX
#define LUAB_OFF_MIN                            OFF_MIN
#else
#define LUAB_SIZE_T_MAX                         0
#define LUAB_OFF_MAX                            0
#define LUAB_OFF_MIN                            0
#endif

#if __BSD_VISIBLE
#define LUAB_GID_MAX                            GID_MAX
#define LUAB_UID_MAX                            UID_MAX
#define LUAB_UQUAD_MAX                          UQUAD_MAX
#define LUAB_QUAD_MAX                           QUAD_MAX
#define LUAB_QUAD_MIN                           QUAD_MIN
#else
#define LUAB_GID_MAX                            0
#define LUAB_UID_MAX                            0
#define LUAB_UQUAD_MAX                          0
#define LUAB_QUAD_MAX                           0
#define LUAB_QUAD_MIN                           0
#endif

#if __XSI_VISIBLE || __POSIX_VISIBLE >= 200809
#define LUAB_LONG_BIT                           LONG_BIT
#define LUAB_WORD_BIT                           WORD_BIT
#else
#define LUAB_LONG_BIT                           0
#define LUAB_WORD_BIT                           0
#endif

#if __POSIX_VISIBLE
#define	LUAB_MQ_PRIO_MAX                        MQ_PRIO_MAX
#else
#define	LUAB_MQ_PRIO_MAX                        0
#endif

extern const lua_Integer luab_env_char_bit;

extern const lua_Integer luab_env_schar_max;
extern const lua_Integer luab_env_schar_min;
extern const lua_Integer luab_env_uchar_max;
extern const lua_Integer luab_env_char_max;
extern const lua_Integer luab_env_char_min;

extern const lua_Integer luab_env_ushrt_max;
extern const lua_Integer luab_env_shrt_max;
extern const lua_Integer luab_env_shrt_min;

extern const lua_Integer luab_env_uint_max;
extern const lua_Integer luab_env_int_max;
extern const lua_Integer luab_env_int_min;

extern const lua_Integer luab_env_ulong_max;
extern const lua_Integer luab_env_long_max;
extern const lua_Integer luab_env_long_min;

extern const lua_Integer luab_env_ullong_max;
extern const lua_Integer luab_env_llong_max;
extern const lua_Integer luab_env_llong_min;

extern const lua_Integer luab_env_ssize_max;

extern const lua_Integer luab_env_u_long_max;
extern const lua_Integer luab_env_off_max;
extern const lua_Integer luab_env_off_min;

extern const lua_Integer luab_env_gid_max;
extern const lua_Integer luab_env_uid_max;
extern const lua_Integer luab_env_uquad_max;
extern const lua_Integer luab_env_quad_max;
extern const lua_Integer luab_env_quad_min;

extern const lua_Integer luab_env_long_bit;
extern const lua_Integer luab_env_word_bit;

#define LUAB_SC_NOTSUPP                         (-1)

typedef struct luab_sysconf_vec {
    int             scv_key;
    u_long          scv_dflt;
    u_long          *scv_val;
} luab_sysconf_vec_t;

#define LUAB_SC_VEC_SENTINEL \
    { .scv_key = LUAB_SC_NOTSUPP, .scv_dflt = 0, .scv_val = NULL, }

#define LUAB_SC_ARG_MAX                         _SC_ARG_MAX
#define LUAB_SC_CHILD_MAX                       _SC_CHILD_MAX
#define LUAB_SC_CLK_TCK                         _SC_CLK_TCK
#define LUAB_SC_NGROUPS_MAX                     _SC_NGROUPS_MAX
#define LUAB_SC_OPEN_MAX                        _SC_OPEN_MAX
#define LUAB_SC_JOB_CONTROL                     _SC_JOB_CONTROL
#define LUAB_SC_SAVED_IDS                       _SC_SAVED_IDS
#define LUAB_SC_VERSION                         _SC_VERSION
#define LUAB_SC_BC_BASE_MAX                     _SC_BC_BASE_MAX
#define LUAB_SC_BC_DIM_MAX                      _SC_BC_DIM_MAX
#define LUAB_SC_BC_SCALE_MAX                    _SC_BC_SCALE_MAX
#define LUAB_SC_BC_STRING_MAX                   _SC_BC_STRING_MAX
#define LUAB_SC_COLL_WEIGHTS_MAX                _SC_COLL_WEIGHTS_MAX
#define LUAB_SC_EXPR_NEST_MAX                   _SC_EXPR_NEST_MAX
#define LUAB_SC_LINE_MAX                        _SC_LINE_MAX
#define LUAB_SC_RE_DUP_MAX                      _SC_RE_DUP_MAX
#define LUAB_SC_2_VERSION                       _SC_2_VERSION
#define LUAB_SC_2_C_BIND                        _SC_2_C_BIND
#define LUAB_SC_2_C_DEV                         _SC_2_C_DEV
#define LUAB_SC_2_CHAR_TERM                     _SC_2_CHAR_TERM
#define LUAB_SC_2_FORT_DEV                      _SC_2_FORT_DEV
#define LUAB_SC_2_FORT_RUN                      _SC_2_FORT_RUN
#define LUAB_SC_2_LOCALEDEF                     _SC_2_LOCALEDEF
#define LUAB_SC_2_SW_DEV                        _SC_2_SW_DEV
#define LUAB_SC_2_UPE                           _SC_2_UPE
#define LUAB_SC_STREAM_MAX                      _SC_STREAM_MAX
#define LUAB_SC_TZNAME_MAX                      _SC_TZNAME_MAX

#if __POSIX_VISIBLE >= 199309
#define LUAB_SC_ASYNCHRONOUS_IO                 _SC_ASYNCHRONOUS_IO
#define LUAB_SC_MAPPED_FILES                    _SC_MAPPED_FILES
#define LUAB_SC_MEMLOCK                         _SC_MEMLOCK
#define LUAB_SC_MEMLOCK_RANGE                   _SC_MEMLOCK_RANGE
#define LUAB_SC_MEMORY_PROTECTION               _SC_MEMORY_PROTECTION
#define LUAB_SC_MESSAGE_PASSING                 _SC_MESSAGE_PASSING
#define LUAB_SC_PRIORITIZED_IO                  _SC_PRIORITIZED_IO
#define LUAB_SC_PRIORITY_SCHEDULING             _SC_PRIORITY_SCHEDULING
#define LUAB_SC_REALTIME_SIGNALS                _SC_REALTIME_SIGNALS
#define LUAB_SC_SEMAPHORES                      _SC_SEMAPHORES
#define LUAB_SC_FSYNC                           _SC_FSYNC
#define LUAB_SC_SHARED_MEMORY_OBJECTS           _SC_SHARED_MEMORY_OBJECTS
#define LUAB_SC_SYNCHRONIZED_IO                 _SC_SYNCHRONIZED_IO
#define LUAB_SC_TIMERS                          _SC_TIMERS
#define LUAB_SC_AIO_LISTIO_MAX                  _SC_AIO_LISTIO_MAX
#define LUAB_SC_AIO_MAX                         _SC_AIO_MAX
#define LUAB_SC_AIO_PRIO_DELTA_MAX              _SC_AIO_PRIO_DELTA_MAX
#define LUAB_SC_DELAYTIMER_MAX                  _SC_DELAYTIMER_MAX
#define LUAB_SC_MQ_OPEN_MAX                     _SC_MQ_OPEN_MAX
#define LUAB_SC_PAGESIZE                        _SC_PAGESIZE
#define LUAB_SC_RTSIG_MAX                       _SC_RTSIG_MAX
#define LUAB_SC_SEM_NSEMS_MAX                   _SC_SEM_NSEMS_MAX
#define LUAB_SC_SEM_VALUE_MAX                   _SC_SEM_VALUE_MAX
#define LUAB_SC_SIGQUEUE_MAX                    _SC_SIGQUEUE_MAX
#define LUAB_SC_TIMER_MAX                       _SC_TIMER_MAX
#else
#define LUAB_SC_ASYNCHRONOUS_IO                 LUAB_SC_NOTSUPP
#define LUAB_SC_MAPPED_FILES                    LUAB_SC_NOTSUPP
#define LUAB_SC_MEMLOCK                         LUAB_SC_NOTSUPP
#define LUAB_SC_MEMLOCK_RANGE                   LUAB_SC_NOTSUPP
#define LUAB_SC_MEMORY_PROTECTION               LUAB_SC_NOTSUPP
#define LUAB_SC_MESSAGE_PASSING                 LUAB_SC_NOTSUPP
#define LUAB_SC_PRIORITIZED_IO                  LUAB_SC_NOTSUPP
#define LUAB_SC_PRIORITY_SCHEDULING             LUAB_SC_NOTSUPP
#define LUAB_SC_REALTIME_SIGNALS                LUAB_SC_NOTSUPP
#define LUAB_SC_SEMAPHORES                      LUAB_SC_NOTSUPP
#define LUAB_SC_FSYNC                           LUAB_SC_NOTSUPP
#define LUAB_SC_SHARED_MEMORY_OBJECTS           LUAB_SC_NOTSUPP
#define LUAB_SC_SYNCHRONIZED_IO                 LUAB_SC_NOTSUPP
#define LUAB_SC_TIMERS                          LUAB_SC_NOTSUPP
#define LUAB_SC_AIO_LISTIO_MAX                  LUAB_SC_NOTSUPP
#define LUAB_SC_AIO_MAX                         LUAB_SC_NOTSUPP
#define LUAB_SC_AIO_PRIO_DELTA_MAX              LUAB_SC_NOTSUPP
#define LUAB_SC_DELAYTIMER_MAX                  LUAB_SC_NOTSUPP
#define LUAB_SC_MQ_OPEN_MAX                     LUAB_SC_NOTSUPP
#define LUAB_SC_PAGESIZE                        LUAB_SC_NOTSUPP
#define LUAB_SC_RTSIG_MAX                       LUAB_SC_NOTSUPP
#define LUAB_SC_SEM_NSEMS_MAX                   LUAB_SC_NOTSUPP
#define LUAB_SC_SEM_VALUE_MAX                   LUAB_SC_NOTSUPP
#define LUAB_SC_SIGQUEUE_MAX                    LUAB_SC_NOTSUPP
#define LUAB_SC_TIMER_MAX                       LUAB_SC_NOTSUPP
#endif /* __POSIX_VISIBLE >= 199309 */

#if __POSIX_VISIBLE >= 200112
#define LUAB_SC_2_PBS                           _SC_2_PBS
#define LUAB_SC_2_PBS_ACCOUNTING                _SC_2_PBS_ACCOUNTING
#define LUAB_SC_2_PBS_CHECKPOINT                _SC_2_PBS_CHECKPOINT
#define LUAB_SC_2_PBS_LOCATE                    _SC_2_PBS_LOCATE
#define LUAB_SC_2_PBS_MESSAGE                   _SC_2_PBS_MESSAGE
#define LUAB_SC_2_PBS_TRACK                     _SC_2_PBS_TRACK
#define LUAB_SC_ADVISORY_INFO                   _SC_ADVISORY_INFO
#define LUAB_SC_BARRIERS                        _SC_BARRIERS
#define LUAB_SC_CLOCK_SELECTION                 _SC_CLOCK_SELECTION
#define LUAB_SC_CPUTIME                         _SC_CPUTIME
#define LUAB_SC_FILE_LOCKING                    _SC_FILE_LOCKING
#define LUAB_SC_GETGR_R_SIZE_MAX                _SC_GETGR_R_SIZE_MAX
#define LUAB_SC_GETPW_R_SIZE_MAX                _SC_GETPW_R_SIZE_MAX
#define LUAB_SC_HOST_NAME_MAX                   _SC_HOST_NAME_MAX
#define LUAB_SC_LOGIN_NAME_MAX                  _SC_LOGIN_NAME_MAX
#define LUAB_SC_MONOTONIC_CLOCK                 _SC_MONOTONIC_CLOCK
#define LUAB_SC_MQ_PRIO_MAX                     _SC_MQ_PRIO_MAX
#define LUAB_SC_READER_WRITER_LOCKS             _SC_READER_WRITER_LOCKS
#define LUAB_SC_REGEXP                          _SC_REGEXP
#define LUAB_SC_SHELL                           _SC_SHELL
#define LUAB_SC_SPAWN                           _SC_SPAWN
#define LUAB_SC_SPIN_LOCKS                      _SC_SPIN_LOCKS
#define LUAB_SC_SPORADIC_SERVER                 _SC_SPORADIC_SERVER
#define LUAB_SC_THREAD_ATTR_STACKADDR           _SC_THREAD_ATTR_STACKADDR
#define LUAB_SC_THREAD_ATTR_STACKSIZE           _SC_THREAD_ATTR_STACKSIZE
#define LUAB_SC_THREAD_CPUTIME                  _SC_THREAD_CPUTIME
#define LUAB_SC_THREAD_DESTRUCTOR_ITERATIONS    _SC_THREAD_DESTRUCTOR_ITERATIONS
#define LUAB_SC_THREAD_KEYS_MAX                 _SC_THREAD_KEYS_MAX
#define LUAB_SC_THREAD_PRIO_INHERIT             _SC_THREAD_PRIO_INHERIT
#define LUAB_SC_THREAD_PRIO_PROTECT             _SC_THREAD_PRIO_PROTECT
#define LUAB_SC_THREAD_PRIORITY_SCHEDULING      _SC_THREAD_PRIORITY_SCHEDULING
#define LUAB_SC_THREAD_PROCESS_SHARED           _SC_THREAD_PROCESS_SHARED
#define LUAB_SC_THREAD_SAFE_FUNCTIONS           _SC_THREAD_SAFE_FUNCTIONS
#define LUAB_SC_THREAD_SPORADIC_SERVER          _SC_THREAD_SPORADIC_SERVER
#define LUAB_SC_THREAD_STACK_MIN                _SC_THREAD_STACK_MIN
#define LUAB_SC_THREAD_THREADS_MAX              _SC_THREAD_THREADS_MAX
#define LUAB_SC_TIMEOUTS                        _SC_TIMEOUTS
#define LUAB_SC_THREADS                         _SC_THREADS
#define LUAB_SC_TRACE                           _SC_TRACE
#define LUAB_SC_TRACE_EVENT_FILTER              _SC_TRACE_EVENT_FILTER
#define LUAB_SC_TRACE_INHERIT                   _SC_TRACE_INHERIT
#define LUAB_SC_TRACE_LOG                       _SC_TRACE_LOG
#define LUAB_SC_TTY_NAME_MAX                    _SC_TTY_NAME_MAX
#define LUAB_SC_TYPED_MEMORY_OBJECTS            _SC_TYPED_MEMORY_OBJECTS
#define LUAB_SC_V6_ILP32_OFF32                  _SC_V6_ILP32_OFF32
#define LUAB_SC_V6_ILP32_OFFBIG                 _SC_V6_ILP32_OFFBIG
#define LUAB_SC_V6_LP64_OFF64                   _SC_V6_LP64_OFF64
#define LUAB_SC_V6_LPBIG_OFFBIG                 _SC_V6_LPBIG_OFFBIG
#define LUAB_SC_IPV6                            _SC_IPV6
#define LUAB_SC_RAW_SOCKETS                     _SC_RAW_SOCKETS
#define LUAB_SC_SYMLOOP_MAX                     _SC_SYMLOOP_MAX
#else
#define LUAB_SC_2_PBS                           LUAB_SC_NOTSUPP
#define LUAB_SC_2_PBS_ACCOUNTING                LUAB_SC_NOTSUPP
#define LUAB_SC_2_PBS_CHECKPOINT                LUAB_SC_NOTSUPP
#define LUAB_SC_2_PBS_LOCATE                    LUAB_SC_NOTSUPP
#define LUAB_SC_2_PBS_MESSAGE                   LUAB_SC_NOTSUPP
#define LUAB_SC_2_PBS_TRACK                     LUAB_SC_NOTSUPP
#define LUAB_SC_ADVISORY_INFO                   LUAB_SC_NOTSUPP
#define LUAB_SC_BARRIERS                        LUAB_SC_NOTSUPP
#define LUAB_SC_CLOCK_SELECTION                 LUAB_SC_NOTSUPP
#define LUAB_SC_CPUTIME                         LUAB_SC_NOTSUPP
#define LUAB_SC_FILE_LOCKING                    LUAB_SC_NOTSUPP
#define LUAB_SC_GETGR_R_SIZE_MAX                LUAB_SC_NOTSUPP
#define LUAB_SC_GETPW_R_SIZE_MAX                LUAB_SC_NOTSUPP
#define LUAB_SC_HOST_NAME_MAX                   LUAB_SC_NOTSUPP
#define LUAB_SC_LOGIN_NAME_MAX                  LUAB_SC_NOTSUPP
#define LUAB_SC_MONOTONIC_CLOCK                 LUAB_SC_NOTSUPP
#define LUAB_SC_MQ_PRIO_MAX                     LUAB_SC_NOTSUPP
#define LUAB_SC_READER_WRITER_LOCKS             LUAB_SC_NOTSUPP
#define LUAB_SC_REGEXP                          LUAB_SC_NOTSUPP
#define LUAB_SC_SHELL                           LUAB_SC_NOTSUPP
#define LUAB_SC_SPAWN                           LUAB_SC_NOTSUPP
#define LUAB_SC_SPIN_LOCKS                      LUAB_SC_NOTSUPP
#define LUAB_SC_SPORADIC_SERVER                 LUAB_SC_NOTSUPP
#define LUAB_SC_THREAD_ATTR_STACKADDR           LUAB_SC_NOTSUPP
#define LUAB_SC_THREAD_ATTR_STACKSIZE           LUAB_SC_NOTSUPP
#define LUAB_SC_THREAD_CPUTIME                  LUAB_SC_NOTSUPP
#define LUAB_SC_THREAD_DESTRUCTOR_ITERATIONS    LUAB_SC_NOTSUPP
#define LUAB_SC_THREAD_KEYS_MAX                 LUAB_SC_NOTSUPP
#define LUAB_SC_THREAD_PRIO_INHERIT             LUAB_SC_NOTSUPP
#define LUAB_SC_THREAD_PRIO_PROTECT             LUAB_SC_NOTSUPP
#define LUAB_SC_THREAD_PRIORITY_SCHEDULING      LUAB_SC_NOTSUPP
#define LUAB_SC_THREAD_PROCESS_SHARED           LUAB_SC_NOTSUPP
#define LUAB_SC_THREAD_SAFE_FUNCTIONS           LUAB_SC_NOTSUPP
#define LUAB_SC_THREAD_SPORADIC_SERVER          LUAB_SC_NOTSUPP
#define LUAB_SC_THREAD_STACK_MIN                LUAB_SC_NOTSUPP
#define LUAB_SC_THREAD_THREADS_MAX              LUAB_SC_NOTSUPP
#define LUAB_SC_TIMEOUTS                        LUAB_SC_NOTSUPP
#define LUAB_SC_THREADS                         LUAB_SC_NOTSUPP
#define LUAB_SC_TRACE                           LUAB_SC_NOTSUPP
#define LUAB_SC_TRACE_EVENT_FILTER              LUAB_SC_NOTSUPP
#define LUAB_SC_TRACE_INHERIT                   LUAB_SC_NOTSUPP
#define LUAB_SC_TRACE_LOG                       LUAB_SC_NOTSUPP
#define LUAB_SC_TTY_NAME_MAX                    LUAB_SC_NOTSUPP
#define LUAB_SC_TYPED_MEMORY_OBJECTS            LUAB_SC_NOTSUPP
#define LUAB_SC_V6_ILP32_OFF32                  LUAB_SC_NOTSUPP
#define LUAB_SC_V6_ILP32_OFFBIG                 LUAB_SC_NOTSUPP
#define LUAB_SC_V6_LP64_OFF64                   LUAB_SC_NOTSUPP
#define LUAB_SC_V6_LPBIG_OFFBIG                 LUAB_SC_NOTSUPP
#define LUAB_SC_IPV6                            LUAB_SC_NOTSUPP
#define LUAB_SC_RAW_SOCKETS                     LUAB_SC_NOTSUPP
#define LUAB_SC_SYMLOOP_MAX                     LUAB_SC_NOTSUPP
#endif /* __POSIX_VISIBLE >= 200112 */

#if __XSI_VISIBLE
#define LUAB_SC_ATEXIT_MAX                      _SC_ATEXIT_MAX
#define LUAB_SC_IOV_MAX                         _SC_IOV_MAX
#define LUAB_SC_PAGE_SIZE                       _SC_PAGE_SIZE
#define LUAB_SC_XOPEN_CRYPT                     _SC_XOPEN_CRYPT
#define LUAB_SC_XOPEN_ENH_I18N                  _SC_XOPEN_ENH_I18N
#define LUAB_SC_XOPEN_LEGACY                    _SC_XOPEN_LEGACY
#define LUAB_SC_XOPEN_REALTIME                  _SC_XOPEN_REALTIME
#define LUAB_SC_XOPEN_REALTIME_THREADS          _SC_XOPEN_REALTIME_THREADS
#define LUAB_SC_XOPEN_SHM                       _SC_XOPEN_SHM
#define LUAB_SC_XOPEN_STREAMS                   _SC_XOPEN_STREAMS
#define LUAB_SC_XOPEN_UNIX                      _SC_XOPEN_UNIX
#define LUAB_SC_XOPEN_VERSION                   _SC_XOPEN_VERSION
#define LUAB_SC_XOPEN_XCU_VERSION               _SC_XOPEN_XCU_VERSION
#else
#define LUAB_SC_ATEXIT_MAX                      LUAB_SC_NOTSUPP
#define LUAB_SC_IOV_MAX                         LUAB_SC_NOTSUPP
#define LUAB_SC_PAGE_SIZE                       LUAB_SC_NOTSUPP
#define LUAB_SC_XOPEN_CRYPT                     LUAB_SC_NOTSUPP
#define LUAB_SC_XOPEN_ENH_I18N                  LUAB_SC_NOTSUPP
#define LUAB_SC_XOPEN_LEGACY                    LUAB_SC_NOTSUPP
#define LUAB_SC_XOPEN_REALTIME                  LUAB_SC_NOTSUPP
#define LUAB_SC_XOPEN_REALTIME_THREADS          LUAB_SC_NOTSUPP
#define LUAB_SC_XOPEN_SHM                       LUAB_SC_NOTSUPP
#define LUAB_SC_XOPEN_STREAMS                   LUAB_SC_NOTSUPP
#define LUAB_SC_XOPEN_UNIX                      LUAB_SC_NOTSUPP
#define LUAB_SC_XOPEN_VERSION                   LUAB_SC_NOTSUPP
#define LUAB_SC_XOPEN_XCU_VERSION               LUAB_SC_NOTSUPP
#endif /* __XSI_VISIBLE */

#if __BSD_VISIBLE
#define LUAB_SC_NPROCESSORS_CONF                _SC_NPROCESSORS_CONF
#define LUAB_SC_NPROCESSORS_ONLN                _SC_NPROCESSORS_ONLN
#define LUAB_SC_CPUSET_SIZE                     _SC_CPUSET_SIZE
#else
#define LUAB_SC_NPROCESSORS_CONF                LUAB_SC_NOTSUPP
#define LUAB_SC_NPROCESSORS_ONLN                LUAB_SC_NOTSUPP
#define LUAB_SC_CPUSET_SIZE                     LUAB_SC_NOTSUPP
#endif /* __BSD_VISIBLE */

#define LUAB_SC_PHYS_PAGES                      _SC_PHYS_PAGES
#define LUAB_SC_PHYS_PAGES                      _SC_PHYS_PAGES

extern u_long luab_env_buf_max;
extern u_long luab_env_path_max;
extern u_long luab_env_logname_max;

extern u_long luab_env_arg_max;
extern u_long luab_env_child_max;
extern u_long luab_env_clk_tck;
extern u_long luab_env_ngroups_max;
extern u_long luab_env_open_max;
extern u_long luab_env_job_control;
extern u_long luab_env_saved_ids;
extern u_long luab_env_version;
extern u_long luab_env_bc_base_max;
extern u_long luab_env_bc_dim_max;
extern u_long luab_env_bc_scale_max;
extern u_long luab_env_bc_string_max;
extern u_long luab_env_coll_weights_max;
extern u_long luab_env_expr_nest_max;
extern u_long luab_env_line_max;
extern u_long luab_env_re_dup_max;
extern u_long luab_env_2_version;
extern u_long luab_env_2_c_bind;
extern u_long luab_env_2_c_dev;
extern u_long luab_env_2_char_term;
extern u_long luab_env_2_fort_dev;
extern u_long luab_env_2_fort_run;
extern u_long luab_env_2_localedef;
extern u_long luab_env_2_sw_dev;
extern u_long luab_env_2_upe;
extern u_long luab_env_stream_max;
extern u_long luab_env_tzname_max;

extern u_long luab_env_asynchronous_io;
extern u_long luab_env_mapped_files;
extern u_long luab_env_memlock;
extern u_long luab_env_memlock_range;
extern u_long luab_env_memory_protection;
extern u_long luab_env_message_passing;
extern u_long luab_env_prioritized_io;
extern u_long luab_env_priority_scheduling;
extern u_long luab_env_realtime_signals;
extern u_long luab_env_semaphores;
extern u_long luab_env_fsync;
extern u_long luab_env_shared_memory_objects;
extern u_long luab_env_synchronized_io;
extern u_long luab_env_timers;
extern u_long luab_env_aio_listio_max;
extern u_long luab_env_aio_max;
extern u_long luab_env_aio_prio_delta_max;
extern u_long luab_env_delaytimer_max;
extern u_long luab_env_mq_open_max;
extern u_long luab_env_pagesize;
extern u_long luab_env_rtsig_max;
extern u_long luab_env_sem_nsems_max;
extern u_long luab_env_sem_value_max;
extern u_long luab_env_sigqueue_max;
extern u_long luab_env_timer_max;

extern u_long luab_env__posix_visible;
extern u_long luab_env_2_pbs;
extern u_long luab_env_2_pbs_accounting;
extern u_long luab_env_2_pbs_checkpoint;
extern u_long luab_env_2_pbs_locate;
extern u_long luab_env_2_pbs_message;
extern u_long luab_env_2_pbs_track;
extern u_long luab_env_advisory_info;
extern u_long luab_env_barriers;
extern u_long luab_env_clock_selection;
extern u_long luab_env_cputime;
extern u_long luab_env_file_locking;
extern u_long luab_env_getgr_r_size_max;
extern u_long luab_env_getpw_r_size_max;
extern u_long luab_env_host_name_max;
extern u_long luab_env_login_name_max;
extern u_long luab_env_monotonic_clock;
extern u_long luab_env_mq_prio_max;
extern u_long luab_env_reader_writer_locks;
extern u_long luab_env_regexp;
extern u_long luab_env_shell;
extern u_long luab_env_spawn;
extern u_long luab_env_spin_locks;
extern u_long luab_env_sporadic_server;
extern u_long luab_env_thread_attr_stackaddr;
extern u_long luab_env_thread_attr_stacksize;
extern u_long luab_env_thread_cputime;
extern u_long luab_env_thread_destructor_iterations;
extern u_long luab_env_thread_keys_max;
extern u_long luab_env_thread_prio_inherit;
extern u_long luab_env_thread_prio_protect;
extern u_long luab_env_thread_priority_scheduling;
extern u_long luab_env_thread_process_shared;
extern u_long luab_env_thread_safe_functions;
extern u_long luab_env_thread_sporadic_server;
extern u_long luab_env_thread_stack_min;
extern u_long luab_env_thread_threads_max;
extern u_long luab_env_timeouts;
extern u_long luab_env_threads;
extern u_long luab_env_trace;
extern u_long luab_env_trace_event_filter;
extern u_long luab_env_trace_inherit;
extern u_long luab_env_trace_log;
extern u_long luab_env_tty_name_max;
extern u_long luab_env_typed_memory_objects;
extern u_long luab_env_v6_ilp32_off32;
extern u_long luab_env_v6_ilp32_offbig;
extern u_long luab_env_v6_lp64_off64;
extern u_long luab_env_v6_lpbig_offbig;
extern u_long luab_env_ipv6;
extern u_long luab_env_raw_sockets;
extern u_long luab_env_symloop_max;

extern u_long luab_env_atexit_max;
extern u_long luab_env_iov_max;
extern u_long luab_env_page_size;
extern u_long luab_env_xopen_crypt;
extern u_long luab_env_xopen_enh_i18n;
extern u_long luab_env_xopen_legacy;
extern u_long luab_env_xopen_realtime;
extern u_long luab_env_xopen_realtime_threads;
extern u_long luab_env_xopen_shm;
extern u_long luab_env_xopen_streams;
extern u_long luab_env_xopen_unix;
extern u_long luab_env_xopen_version;
extern u_long luab_env_xopen_xcu_version;

extern u_long luab_env_nprocessors_conf;
extern u_long luab_env_nprocessors_onln;
extern u_long luab_env_cpuset_size;

extern u_long luab_env_phys_pages;
extern u_long luab_env_phys_pages;

#endif /* _LUAB_ENV_H_ */
