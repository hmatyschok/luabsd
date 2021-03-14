/*-
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

#ifndef _LUAB_ENV_H_
#define _LUAB_ENV_H_

/*
 * Various usefull constants.
 */

#define LUAB_ERROR                              (-1)
#define LUAB_SUCCESS                            (0)

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
#define LUAB_ULLONG_MAX                         ULONG_MAX
#define LUAB_LLONG_MAX                          LONG_MAX
#define LUAB_LLONG_MIN                          LONG_MIN
#endif

#if __POSIX_VISIBLE || __XSI_VISIBLE
#define LUAB_SSIZE_MAX                          SSIZE_MAX
#else
#define LUAB_SSIZE_MAX                          (0)
#endif

#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
#define LUAB_SIZE_T_MAX                         SIZE_T_MAX
#define LUAB_OFF_MAX                            OFF_MAX
#define LUAB_OFF_MIN                            OFF_MIN
#else
#define LUAB_SIZE_T_MAX                         (0)
#define LUAB_OFF_MAX                            (0)
#define LUAB_OFF_MIN                            (0)
#endif

#if __BSD_VISIBLE
#define LUAB_GID_MAX                            GID_MAX
#define LUAB_UID_MAX                            UID_MAX
#define LUAB_UQUAD_MAX                          UQUAD_MAX
#define LUAB_QUAD_MAX                           QUAD_MAX
#define LUAB_QUAD_MIN                           QUAD_MIN
#else
#define LUAB_GID_MAX                            (0)
#define LUAB_UID_MAX                            (0)
#define LUAB_UQUAD_MAX                          (0)
#define LUAB_QUAD_MAX                           (0)
#define LUAB_QUAD_MIN                           (0)
#endif

#if __XSI_VISIBLE || __POSIX_VISIBLE >= 200809
#define LUAB_LONG_BIT                           LONG_BIT
#define LUAB_WORD_BIT                           WORD_BIT
#else
#define LUAB_LONG_BIT                           (0)
#define LUAB_WORD_BIT                           (0)
#endif

#if __POSIX_VISIBLE
#define LUAB_MQ_PRIO_MAX                        MQ_PRIO_MAX
#else
#define LUAB_MQ_PRIO_MAX                        (0)
#endif

#define LUAB_SIGSTKSZ                           SIGSTKSZ

extern const lua_Integer luab_env_error;
extern const lua_Integer luab_env_success;

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

/* <luab_sigset_type> */
extern const lua_Integer luab_env_sig_words;
extern const lua_Integer luab_env_sig_maxsig;

/* <luab_sys_signal_lib> */
#if __BSD_VISIBLE
extern const lua_Integer luab_env_sys_nsig;
#endif
#if __XSI_VISIBLE
extern const lua_Integer luab_env_sigstksz;
#endif

#define LUAB_SC_NOTSUPP                         (-1)
#define LUAB_SCX_DFLT                           (0)

typedef struct luab_sysconf_vec {
    int             scv_key;
    u_long          scv_dflt;
    u_long          *scv_val;
} luab_sysconf_vec_t;

#define LUAB_SC_VEC_SENTINEL \
    { .scv_key = LUAB_SC_NOTSUPP, .scv_dflt = 0, .scv_val = NULL, }

#define LUAB_BUFFERSIZE                         LUAL_BUFFERSIZE
#define LUAB_MAXPATHLEN                         MAXPATHLEN
#define LUAB_SPECNAMELEN                        SPECNAMELEN
#define LUAB_IF_NAMESIZE                        IF_NAMESIZE
#define LUAB_NAME_MAX                           NAME_MAX
#if __XSI_VISIBLE
#define LUAB_PASSWD_MAX                         PASS_MAX
#else
#define LUAB_PASSWD_MAX                         128
#endif
#define LUAB_TS_MIN                             26

extern u_long luab_env_buf_max;
extern u_long luab_env_path_max;
extern u_long luab_env_logname_max;
extern u_long luab_env_specname_max;
extern u_long luab_env_ifname_max;
extern u_long luab_env_name_max;
extern u_long luab_env_passwd_max;
extern u_long luab_env_ts_min;

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

/*
 * Subset of atomic data types.
 */

#define LUAB_CHAR_TYPE_ID                       1605964271
#define LUAB_CHAR_TYPE                          "CHAR*"

#define LUAB_DOUBLE_TYPE_ID                     1605894356
#define LUAB_DOUBLE_TYPE                        "DOUBLE*"

#define LUAB_FLOAT_TYPE_ID                      1605908147
#define LUAB_FLOAT_TYPE                         "FLOAT*"

#define LUAB_FPOS_TYPE_ID                       1605960373
#define LUAB_FPOS_TYPE                          "FPOS*"

#define LUAB_GID_TYPE_ID                        1605908910
#define LUAB_GID_TYPE                           "GID*"

#define LUAB_INT16_TYPE_ID                      1606002046
#define LUAB_INT16_TYPE                         "INT16*"

#define LUAB_INT32_TYPE_ID                      1606004102
#define LUAB_INT32_TYPE                         "INT32*"

#define LUAB_INT64_TYPE_ID                      1606004507
#define LUAB_INT64_TYPE                         "INT64*"

#define LUAB_INT8_TYPE_ID                       1606001349
#define LUAB_INT8_TYPE                          "INT8*"

#define LUAB_INT_TYPE_ID                        1605877856
#define LUAB_INT_TYPE                           "INT*"

#define LUAB_LONG_TYPE_ID                       1605965914
#define LUAB_LONG_TYPE                          "LONG*"

#define LUAB_OFF_TYPE_ID                        1605914553
#define LUAB_OFF_TYPE                           "OFF*"

#define LUAB_SHORT_TYPE_ID                      1605997500
#define LUAB_SHORT_TYPE                         "SHORT*"

#define LUAB_SIZE_TYPE_ID                       1605967268
#define LUAB_SIZE_TYPE                          "SIZE*"

#define LUAB_SOCKLEN_TYPE_ID                    1605960373
#define LUAB_SOCKLEN_TYPE                       "SOCKLEN*"

#define LUAB_SSIZE_TYPE_ID                      1605991182
#define LUAB_SSIZE_TYPE                         "SSIZE*"

#define LUAB_UID_TYPE_ID                        1605913920
#define LUAB_UID_TYPE                           "UID*"

#define LUAB_UINT_TYPE_ID                       1605965347
#define LUAB_UINT_TYPE                          "UINT*"

#define LUAB_USHRT_TYPE_ID                      1605911568
#define LUAB_USHRT_TYPE                         "USHRT*"

#define LUAB_WCHAR_TYPE_ID                      1605963183
#define LUAB_WCHAR_TYPE                         "WCHAR*"

#define LUAB_TIME_TYPE_ID                       1607255289
#define LUAB_TIME_TYPE                          "TIME*"

#define LUAB_CLOCK_TYPE_ID                      1607649423
#define LUAB_CLOCK_TYPE                         "CLOCK*"

#define LUAB_INTPTR_TYPE_ID                     1608485473
#define LUAB_INTPTR_TYPE                        "INTPTR*"

#define LUAB_UINTPTR_TYPE_ID                    1608557891
#define LUAB_UINTPTR_TYPE                       "UINTPTR*"

#define LUAB_INTMAX_TYPE_ID                     1608559100
#define LUAB_INTMAX_TYPE                        "INTMAX*"

#define LUAB_UINTMAX_TYPE_ID                    1608559801
#define LUAB_UINTMAX_TYPE                       "UINTMAX*"

#define LUAB_UINT8_TYPE_ID                      1608568551
#define LUAB_UINT8_TYPE                         "UINT8*"

#define LUAB_UINT16_TYPE_ID                     1608570564
#define LUAB_UINT16_TYPE                        "UINT16*"

#define LUAB_UINT32_TYPE_ID                     1608590394
#define LUAB_UINT32_TYPE                        "UINT32*"

#define LUAB_UINT64_TYPE_ID                     1608592367
#define LUAB_UINT64_TYPE                        "UINT64*"

#define LUAB_NL_ITEM_TYPE_ID                    1608651977
#define LUAB_NL_ITEM_TYPE                       "NL_ITEM*"

#define LUAB_CT_RUNE_TYPE_ID                    1608692148
#define LUAB_CT_RUNE_TYPE                       "CT_RUNE*"

#define LUAB_IN_PORT_TYPE_ID                    1609278999
#define LUAB_IN_PORT_TYPE                       "IN_PORT*"

#define LUAB_MODE_TYPE_ID                       1609284959
#define LUAB_MODE_TYPE                          "MODE*"

#define LUAB_RSIZE_TYPE_ID                      1609334778
#define LUAB_RSIZE_TYPE                         "RSIZE*"

#define LUAB_DEV_TYPE_ID                        1609369104
#define LUAB_DEV_TYPE                           "DEV*"

#define LUAB_PID_TYPE_ID                        1609404242
#define LUAB_PID_TYPE                           "PID*"

#define LUAB_USECONDS_TYPE_ID                   1609414979
#define LUAB_USECONDS_TYPE                      "USECONDS*"

#define LUAB_ULONG_TYPE_ID                      1609419610
#define LUAB_ULONG_TYPE                         "ULONG*"

#define LUAB_VM_OFFSET_TYPE_ID                  1609421996
#define LUAB_VM_OFFSET_TYPE                     "VM_OFFSET*"

#define LUAB_INO_TYPE_ID                        1609721091
#define LUAB_INO_TYPE                           "INO*"

#define LUAB_NLINK_TYPE_ID                      1609760120
#define LUAB_NLINK_TYPE                         "NLINK*"

#define LUAB_BLKCNT_TYPE_ID                     1609763312
#define LUAB_BLKCNT_TYPE                        "BLKCNT*"

#define LUAB_FFLAGS_TYPE_ID                     1609765760
#define LUAB_FFLAGS_TYPE                        "FFLAGS*"

#define LUAB_LUAL_INTEGER_TYPE_ID               1609859233
#define LUAB_LUAL_INTEGER_TYPE                  "LUAL_INTEGER*"

#define LUAB_LUAL_NUMBER_TYPE_ID                1609876996
#define LUAB_LUAL_NUMBER_TYPE                   "LUAL_NUMBER*"

#define LUAB_UCHAR_TYPE_ID                      1609944412
#define LUAB_UCHAR_TYPE                         "UCHAR*"

#define LUAB_KEY_TYPE_ID                        1613618084
#define LUAB_KEY_TYPE                           "KEY*"

#define LUAB_SPEED_TYPE_ID                      1615600985
#define LUAB_SPEED_TYPE                         "SPEED*"

#define LUAB_TCFLAG_TYPE_ID                     1615636502
#define LUAB_TCFLAG_TYPE                        "TCFLAG*"

#if __POSIX_VISIBLE >= 199309
#define LUAB_CLOCKID_TYPE_ID                    1610364586
#define LUAB_CLOCKID_TYPE                       "CLOCKID*"
#endif

/*
 * Subset of reference data types.
 */

#define LUAB_LOCALE_TYPE_ID                     1608306297
#define LUAB_LOCALE_TYPE                        "LOCALE*"

#if __POSIX_VISIBLE >= 199309
#define LUAB_TIMER_TYPE_ID                      1610374001
#define LUAB_TIMER_TYPE                         "TIMER*"
#endif

#if __POSIX_VISIBLE || __XSI_VISIBLE
#define LUAB_SIGSET_TYPE_ID                     1610664236
#define LUAB_SIGSET_TYPE                        "SIGSET*"

#if __POSIX_VISIBLE >= 199309 || __XSI_VISIBLE >= 500
#define LUAB_SIGVAL_TYPE_ID                     1610402059
#define LUAB_SIGVAL_TYPE                        "SIGVAL*"
#endif
#endif

#define LUAB_CADDR_TYPE_ID                      1610728624
#define LUAB_CADDR_TYPE                         "CADDR*"

#define LUAB_PTHREAD_TYPE_ID                    1610773919
#define LUAB_PTHREAD_TYPE                       "PTHREAD*"

#define LUAB_PTHREAD_ATTR_TYPE_ID               1610823607
#define LUAB_PTHREAD_ATTR_TYPE                  "PTHREAD_ATTR*"

#define LUAB_PTHREAD_MUTEX_TYPE_ID              1610827312
#define LUAB_PTHREAD_MUTEX_TYPE                 "PTHREAD_MUTEX*"

#define LUAB_PTHREAD_MUTEXATTR_TYPE_ID          1610831823
#define LUAB_PTHREAD_MUTEXATTR_TYPE             "PTHREAD_MUTEXATTR*"

#define LUAB_PTHREAD_COND_TYPE_ID               1610829923
#define LUAB_PTHREAD_COND_TYPE                  "PTHREAD_COND*"

#define LUAB_PTHREAD_CONDATTR_TYPE_ID           1610832422
#define LUAB_PTHREAD_CONDATTR_TYPE              "PTHREAD_CONDATTR*"

#define LUAB_PTHREAD_KEY_TYPE_ID                1610833224
#define LUAB_PTHREAD_KEY_TYPE                   "PTHREAD_KEY*"

#define LUAB_PTHREAD_RWLOCK_TYPE_ID             1610842866
#define LUAB_PTHREAD_RWLOCK_TYPE                "PTHREAD_RWLOCK*"

#define LUAB_PTHREAD_RWLOCKATTR_TYPE_ID         1610843988
#define LUAB_PTHREAD_RWLOCKATTR_TYPE            "PTHREAD_RWLOCKATTR*"

#define LUAB_PTHREAD_BARRIER_TYPE_ID            1610844627
#define LUAB_PTHREAD_BARRIER_TYPE               "PTHREAD_BARRIER*"

#define LUAB_PTHREAD_BARRIERATTR_TYPE_ID        1610845328
#define LUAB_PTHREAD_BARRIERATTR_TYPE           "PTHREAD_BARRIERATTR*"

#define LUAB_PTHREAD_SPINLOCK_TYPE_ID           1610846141
#define LUAB_PTHREAD_SPINLOCK_TYPE              "PTHREAD_SPINLOCK*"

#define LUAB_PTHREAD_ADDR_TYPE_ID               1610848721
#define LUAB_PTHREAD_ADDR_TYPE                  "PTHREAD_ADDR*"

#define LUAB_STRING_TYPE_ID                     1611685185
#define LUAB_STRING_TYPE                        "STRING*"

#define LUAB_SIG_TYPE_ID                        1612398710
#define LUAB_SIG_TYPE                           "SIG*"

/*
 * Subset of composite data types.
 */

#define LUAB___SBUF_TYPE_ID                     1605302940
#define LUAB___SBUF_TYPE                        "__SBUF*"

#define LUAB_ACCEPT_FILTER_ARG_TYPE_ID          1601330449
#define LUAB_ACCEPT_FILTER_ARG_TYPE             "ACCEPT_FILTER_ARG*"

#define LUAB_BINTIME_TYPE_ID                    1594161740
#define LUAB_BINTIME_TYPE                       "BINTIME*"

#define LUAB_CAP_RBUF_TYPE_ID                   1601143331
#define LUAB_CAP_RBUF_TYPE                      "CAP_RBUF*"

#define LUAB_CLOCKINFO_TYPE_ID                  1594164272
#define LUAB_CLOCKINFO_TYPE                     "CLOCKINFO*"

#define LUAB_CMSGCRED_TYPE_ID                   1601557495
#define LUAB_CMSGCRED_TYPE                      "CMSGCRED*"

#define LUAB_CRYPT_DATA_TYPE_ID                 1595491033
#define LUAB_CRYPT_DATA_TYPE                    "CRYPT_DATA*"

#define LUAB_DB_TYPE_ID                         1596201370
#define LUAB_DB_TYPE                            "DB*"

#define LUAB_DBT_TYPE_ID                        1596025036
#define LUAB_DBT_TYPE                           "DBT*"

#define LUAB_DIR_TYPE_ID                        1604794619
#define LUAB_DIR_TYPE                           "DIR*"

#define LUAB_DIRENT_TYPE_ID                     1604795865
#define LUAB_DIRENT_TYPE                        "DIRENT*"

#define LUAB_DIV_TYPE_ID                        1600388163
#define LUAB_DIV_TYPE                           "DIV*"

#define LUAB_FHANDLE_TYPE_ID                    1604445207
#define LUAB_FHANDLE_TYPE                       "FHANDLE*"

#define LUAB_FID_TYPE_ID                        1604423994
#define LUAB_FID_TYPE                           "FID*"

#define LUAB_FLOCK_TYPE_ID                      1593623399
#define LUAB_FLOCK_TYPE                         "FLOCK*"

#define LUAB_FSID_TYPE_ID                       1604426384
#define LUAB_FSID_TYPE                          "FSID*"

#define LUAB_FSTAB_TYPE_ID                      1604964419
#define LUAB_FSTAB_TYPE                         "FSTAB*"

#define LUAB_GROUP_TYPE_ID                      1604324396
#define LUAB_GROUP_TYPE                         "GROUP*"

#define LUAB_IF_NAMEINDEX_TYPE_ID               1596840702
#define LUAB_IF_NAMEINDEX_TYPE                  "IF_NAMEINDEX*"

#define LUAB_IN6_ADDR_TYPE_ID                   1595890830
#define LUAB_IN6_ADDR_TYPE                      "IN6_ADDR*"

#define LUAB_IN_ADDR_TYPE_ID                    1595799233
#define LUAB_IN_ADDR_TYPE                       "IN_ADDR*"

#define LUAB_INTEGER_TYPE_ID                    1595975665
#define LUAB_INTEGER_TYPE                       "INTEGER*"

#define LUAB_IOVEC_TYPE_ID                      1594559731
#define LUAB_IOVEC_TYPE                         "IOVEC*"

#define LUAB_ITIMERVAL_TYPE_ID                  1594110231
#define LUAB_ITIMERVAL_TYPE                     "ITIMERVAL*"

#define LUAB_JAIL_TYPE_ID                       1606011567
#define LUAB_JAIL_TYPE                          "JAIL*"

#define LUAB_LDIV_TYPE_ID                       1600394297
#define LUAB_LDIV_TYPE                          "LDIV*"

#define LUAB_LINGER_TYPE_ID                     1597012436
#define LUAB_LINGER_TYPE                        "LINGER*"

#define LUAB_LINK_TYPE_ID                       1601843279
#define LUAB_LINK_TYPE                          "LINK*"

#define LUAB_LLDIV_TYPE_ID                      1600628549
#define LUAB_LLDIV_TYPE                         "LLDIV*"

#define LUAB_MSGHDR_TYPE_ID                     1597320239
#define LUAB_MSGHDR_TYPE                        "MSGHDR*"

#define LUAB_PASSWD_TYPE_ID                     1604262109
#define LUAB_PASSWD_TYPE                        "PASSWD*"

#define LUAB_REGEX_TYPE_ID                      1604367435
#define LUAB_REGEX_TYPE                         "REGEX*"

#define LUAB_REGMATCH_TYPE_ID                   1604371586
#define LUAB_REGMATCH_TYPE                      "REGMATCH*"

#define LUAB_SF_HDTR_TYPE_ID                    1602287699
#define LUAB_SF_HDTR_TYPE                       "SF_HDTR*"

#define LUAB_SFILE_TYPE_ID                      1605120444
#define LUAB_SFILE_TYPE                         "SFILE*"

#define LUAB_SOCKADDR_TYPE_ID                   1595755513
#define LUAB_SOCKADDR_TYPE                      "SOCKADDR*"

#define LUAB_SOCKPROTO_TYPE_ID                  1601383780
#define LUAB_SOCKPROTO_TYPE                     "SOCKPROTO*"

#define LUAB_STAT_TYPE_ID                       1594028586
#define LUAB_STAT_TYPE                          "STAT*"

#define LUAB_STATFS_TYPE_ID                     1604429614
#define LUAB_STATFS_TYPE                        "STATFS*"

#define LUAB_TIMESPEC_TYPE_ID                   1594034844
#define LUAB_TIMESPEC_TYPE                      "TIMESPEC*"

#define LUAB_TIMEVAL_TYPE_ID                    1599788349
#define LUAB_TIMEVAL_TYPE                       "TIMEVAL*"

#define LUAB_TIMEZONE_TYPE_ID                   1594159943
#define LUAB_TIMEZONE_TYPE                      "TIMEZONE*"

#define LUAB_TM_TYPE_ID                         1594168426
#define LUAB_TM_TYPE                            "TM*"

#define LUAB_TTYENT_TYPE_ID                     1604759728
#define LUAB_TTYENT_TYPE                        "TTYENT*"

#define LUAB_UUID_TYPE_ID                       1599304529
#define LUAB_UUID_TYPE                          "UUID*"

#define LUAB_XVFSCONF_TYPE_ID                   1604736441
#define LUAB_XVFSCONF_TYPE                      "XVFSCONF*"

#define LUAB_LCONV_TYPE_ID                      1610321466
#define LUAB_LCONV_TYPE                         "LCONV*"

#define LUAB_ITIMERSPEC_TYPE_ID                 1610498425
#define LUAB_ITIMERSPEC_TYPE                    "ITIMERSPEC*"

#define LUAB_PTHREAD_ONCE_TYPE_ID               1610834094
#define LUAB_PTHREAD_ONCE_TYPE                  "PTHREAD_ONCE*"

#define LUAB_SCHED_PARAM_TYPE_ID                1611724636
#define LUAB_SCHED_PARAM_TYPE                   "SCHED_PARAM*"

#define LUAB_IPC_PERM_TYPE_ID                   1614475372
#define LUAB_IPC_PERM_TYPE                      "IPC_PERM*"

#if __POSIX_VISIBLE >= 199309
#define LUAB_SIGEVENT_TYPE_ID                   1610459044
#define LUAB_SIGEVENT_TYPE                      "SIGEVENT*"
#endif

#if __POSIX_VISIBLE >= 199309 || __XSI_VISIBLE
#define LUAB__SIGINFO_TYPE_ID                   1610570807
#define LUAB__SIGINFO_TYPE                      "SIGINFO*"
#endif

#if __POSIX_VISIBLE || __XSI_VISIBLE
#define LUAB_SIGACTION_TYPE_ID                  1610718477
#define LUAB_SIGACTION_TYPE                     "SIGACTION*"
#endif

#if __XSI_VISIBLE
#define LUAB_STACK_TYPE_ID                      1611099614
#define LUAB_STACK_TYPE                         "STACK*"

#define LUAB_SIGSTACK_TYPE_ID                   1611098632
#define LUAB_SIGSTACK_TYPE                      "SIGSTACK*"
#endif /* __XSI_VISIBLE */

#if __BSD_VISIBLE
#define LUAB_SIGVEC_TYPE_ID                     1611262452
#define LUAB_SIGVEC_TYPE                        "SIGVEC*"
#endif

/*
 * Definitions for API method table.
 */

typedef union luab_primitive_u {
    char        un_char;
    short       un_short;
    int         un_int;
    long        un_long;
    int8_t      un_int8;
    int16_t     un_int16;
    int32_t     un_int32;
    int64_t     un_int64;
    uint8_t     un_uint8;
    uint16_t    un_uint16;
    uint32_t    un_uint32;
    uint64_t    un_uint64;
    u_int       un_u_int;
    u_long      un_u_long;
    fpos_t      un_fpos;
    gid_t       un_gid;
    off_t       un_off;
    size_t      un_size;
    uid_t       un_uid;
    socklen_t   un_socklen;
    lua_Integer un_intx;
    lua_Number  un_numx;
    lua_CFunction   un_fn;
    const char  *un_cp;
    wchar_t     un_wc;
} luab_primitive_t;

typedef void    (*luab_module_table_fn)(lua_State *, luab_primitive_t *);

typedef struct luab_module_table {
    luab_module_table_fn    mt_init;
    const char              *mt_key;
    luab_primitive_t        mt_val;
} luab_module_table_t;

#define LUAB_TYPE(fn, k, v) \
    { .mt_init = fn, .mt_key = k, v, }
#define LUAB_INT(k, v) \
    LUAB_TYPE(luab_initinteger, k, .mt_val.un_intx = v)
#define LUAB_FUNC(k, v) \
    LUAB_TYPE(luab_initcfunction, k, .mt_val.un_fn = v)
#define LUAB_STR(k, v) \
    LUAB_TYPE(luab_initstring, k, .mt_val.un_cp = v)
#define LUAB_MOD_TBL_SENTINEL \
    LUAB_TYPE(NULL, NULL, .mt_val.un_fn = NULL)

static __inline void
luab_initinteger(lua_State *L, luab_primitive_t *un)
{
    lua_pushinteger(L, un->un_intx);
}

static __inline void
luab_initcfunction(lua_State *L, luab_primitive_t *un)
{
    lua_pushcfunction(L, un->un_fn);
}

static __inline void
luab_initstring(lua_State *L, luab_primitive_t *un)
{
    lua_pushstring(L, un->un_cp);
}

/*
 * Definitions for luab_module{} descriptor.
 */

typedef uint32_t luab_id_t;

typedef void *  (*luab_create_fn)(lua_State *, void *);
typedef void  (*luab_init_fn)(void *, void *);
typedef void *  (*luab_get_fn)(lua_State *, int);

typedef struct luab_table {
    luab_id_t   tbl_id;
    void        *tbl_vec;
    size_t      tbl_card;
    size_t      tbl_sz;
} luab_table_t;

typedef luab_table_t *   (*luab_alloc_tbl_fn)(void *, size_t);
typedef luab_table_t *   (*luab_get_tbl_fn)(lua_State *, int);
typedef void     (*luab_set_tbl_fn)(lua_State *, int, luab_table_t *, int, int);

typedef struct luab_module {
    luab_id_t           m_id;        /*  date -u +'%s' */
    size_t              m_len;
    size_t              m_sz;
    const char          *m_name;
    luab_module_table_t *m_vec;
    luab_create_fn      m_create;
    luab_init_fn        m_init;
    luab_get_fn         m_get;
    luab_get_tbl_fn     m_get_tbl;
    luab_set_tbl_fn     m_set_tbl;
    luab_alloc_tbl_fn   m_alloc_tbl;
} luab_module_t;

typedef void    (*luab_module_fn)(lua_State *, int, luab_module_t *);

/*
 * Selector.
 *
 * XXX
 *  We should externalize set over luab_type{} to <luab_core_env>.
 */

typedef enum luab_type {
#if __BSD_VISIBLE
    LUAB_UCHAR_IDX,
    LUAB_USHRT_IDX,
    LUAB_UINT_IDX,
    LUAB_ULONG_IDX,
#endif /* __BSD_VISIBLE */
    LUAB_CHAR_IDX,
    LUAB_SHORT_IDX,
    LUAB_INT_IDX,
    LUAB_LONG_IDX,

    /* floating point number types */
    LUAB_DOUBLE_IDX,
    LUAB_FLOAT_IDX,

    /* standard types */
    LUAB_OFF_IDX,
    LUAB_SIZE_IDX,
    LUAB_SOCKLEN_IDX,
    LUAB_SSIZE_IDX,
    LUAB_UID_IDX,
    LUAB_WCHAR_IDX,

    LUAB_VM_OFFSET_IDX,

    LUAB_LUAL_INTEGER_IDX,
    LUAB_LUAL_NUMBER_IDX,

    /* <arpa/inet.h> */
    LUAB_IN_PORT_IDX,

    /* <sys/ipc.h> */
    LUAB_KEY_IDX,

    /* <sys/stat.h> */
    LUAB_BLKCNT_IDX,
    LUAB_DEV_IDX,
    LUAB_FFLAGS_IDX,
    LUAB_INO_IDX,
    LUAB_NLINK_IDX,

    /* <sys/stdint.h> */
    LUAB_INT8_IDX,
    LUAB_INT16_IDX,
    LUAB_INT32_IDX,
    LUAB_INT64_IDX,

    LUAB_UINT8_IDX,
    LUAB_UINT16_IDX,
    LUAB_UINT32_IDX,
    LUAB_UINT64_IDX,

    LUAB_INTPTR_IDX,
    LUAB_UINTPTR_IDX,
    LUAB_INTMAX_IDX,
    LUAB_UINTMAX_IDX,

    /* <ctype.h> */
    LUAB_CT_RUNE_IDX,

    /* <fcntl.h> */
    LUAB_MODE_IDX,

    /* <grp.h> */
    LUAB_GID_IDX,

    /* <langinfo.h> */
    LUAB_NL_ITEM_IDX,

    /* <pthread.h> */
    LUAB_PTHREAD_KEY_IDX,

    /* <stdio.h> */
    LUAB_FPOS_IDX,
    LUAB_RSIZE_IDX,

    /* <termios.h> */
    LUAB_SPEED_IDX,
    LUAB_TCFLAG_IDX,

    /* <time.h> */
    LUAB_CLOCK_IDX,
    LUAB_TIME_IDX,
#if __POSIX_VISIBLE >= 199309
    LUAB_CLOCKID_IDX,
#endif

    /* <unistd.h> */
    LUAB_PID_IDX,
    LUAB_USECONDS_IDX,

    /*
     * Reference data types
     */

    /* <luab_core_atomic> */
    LUAB_CADDR_IDX,
    LUAB_STRING_IDX,

    /* <sys/signal.h> */
    LUAB_SIG_IDX,
#if __POSIX_VISIBLE || __XSI_VISIBLE
    LUAB_SIGSET_IDX,
#endif
#if __POSIX_VISIBLE >= 199309 || __XSI_VISIBLE >= 500
    LUAB_SIGVAL_IDX,
#endif
    /* <xlocale/_locale.h> */
    LUAB_LOCALE_IDX,

    /* <pthread.h> */
    LUAB_PTHREAD_IDX,
    LUAB_PTHREAD_ATTR_IDX,
    LUAB_PTHREAD_MUTEX_IDX,
    LUAB_PTHREAD_MUTEXATTR_IDX,
    LUAB_PTHREAD_COND_IDX,
    LUAB_PTHREAD_CONDATTR_IDX,
    LUAB_PTHREAD_RWLOCK_IDX,
    LUAB_PTHREAD_RWLOCKATTR_IDX,
    LUAB_PTHREAD_BARRIER_IDX,
    LUAB_PTHREAD_BARRIERATTR_IDX,
    LUAB_PTHREAD_SPINLOCK_IDX,
    LUAB_PTHREAD_ADDR_IDX,

    /* <time.h> */
#if __POSIX_VISIBLE >= 199309
    LUAB_TIMER_IDX,
#endif

    /*
     * Composite data types
     */

    LUAB_CLOCKINFO_IDX,
    LUAB_DIV_IDX,
    LUAB_FLOCK_IDX,
    LUAB_INTEGER_IDX,
    LUAB_IF_NAMEINDEX_IDX,
    LUAB_IN_ADDR_IDX,
    LUAB_IN6_ADDR_IDX,
    LUAB_ITIMERVAL_IDX,
    LUAB_LDIV_IDX,
    LUAB_LLDIV_IDX,
    LUAB_LINGER_IDX,
    LUAB_MSGHDR_IDX,
    LUAB_SOCKADDR_IDX,
    LUAB_STAT_IDX,
    LUAB_TIMESPEC_IDX,
    LUAB_TIMEVAL_IDX,
    LUAB_TIMEZONE_IDX,
    LUAB_TM_IDX,
    LUAB_UUID_IDX,
    LUAB_IOVEC_IDX,
    LUAB_PASSWD_IDX,
    LUAB_GROUP_IDX,
    LUAB_REGEX_IDX,
    LUAB_REGMATCH_IDX,
    LUAB_FSID_IDX,
    LUAB_FID_IDX,
    LUAB_STATFS_IDX,
    LUAB_FHANDLE_IDX,
    LUAB_XVFSCONF_IDX,
    LUAB_TTYENT_IDX,
    LUAB_DIR_IDX,
    LUAB_DIRENT_IDX,
    LUAB_FSTAB_IDX,
    LUAB___SBUF_IDX,
    LUAB_SFILE_IDX,
    LUAB_JAIL_IDX,
    LUAB_LCONV_IDX,
    LUAB_ITIMERSPEC_IDX,
    LUAB_PTHREAD_ONCE_IDX,
    LUAB_SCHED_PARAM_IDX,
    LUAB_IPC_PERM_IDX,
#if __POSIX_VISIBLE >= 199309
    LUAB_SIGEVENT_IDX,
#endif
#if __POSIX_VISIBLE >= 199309 || __XSI_VISIBLE
    LUAB__SIGINFO_IDX,
#endif
#if __POSIX_VISIBLE || __XSI_VISIBLE
    LUAB_SIGACTION_IDX,
#endif
#if __XSI_VISIBLE
    LUAB_STACK_IDX,
    LUAB_SIGSTACK_IDX,
#endif /* __XSI_VISIBLE */
#if __BSD_VISIBLE
    LUAB_DBT_IDX,
    LUAB_DB_IDX,
    LUAB_BINTIME_IDX,
    LUAB_CRYPT_DATA_IDX,
    LUAB_CAP_RBUF_IDX,
    LUAB_ACCEPT_FILTER_ARG_IDX,
    LUAB_SOCKPROTO_IDX,
    LUAB_CMSGCRED_IDX,
    LUAB_SF_HDTR_IDX,
    LUAB_SIGVEC_IDX,
#endif /* __BSD_VISIBLE */
    LUAB_TYPE_SENTINEL
} luab_type_t;

#define luab_xid(name) \
    (LUAB_##name##_IDX)
#define luab_xcookie(name, type) \
    ((LUAB_##name##_##type##_ID))
#define luab_xmv(idx) \
    (luab_env_type_vec[((idx) % (LUAB_TYPE_SENTINEL))])

typedef struct luab_module_vec {
    luab_module_t       *mv_mod;
    luab_module_fn      mv_init;
    luab_type_t         mv_idx;
} luab_module_vec_t;

#define LUAB_MOD_VEC_SENTINEL \
    { .mv_mod = NULL, .mv_init = NULL, .mv_idx = LUAB_TYPE_SENTINEL, }

extern luab_module_vec_t luab_env_type_vec[];

/*
 * Primitives for module-vector operations.
 */

void     luab_env_populate(lua_State *, int, luab_module_t *);
void     luab_env_newtable(lua_State *, int, luab_module_t *);
void     luab_env_newmetatable(lua_State *, int, luab_module_t *);

void     luab_env_initmodule(lua_State *, int, luab_module_vec_t *,
    const char *, int);

luab_module_t    *luab_env_checkmodule(luab_type_t, uint32_t, const char *);

#define luab_xmod(name, type, fname)                                \
    (luab_env_checkmodule(luab_xid(name),                          \
        luab_xcookie(name, type), (fname)))

#endif /* _LUAB_ENV_H_ */
