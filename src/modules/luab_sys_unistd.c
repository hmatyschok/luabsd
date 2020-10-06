/*
 * Copyright (c) 2020 Henning Matyschok <hmatyschok@outlook.com>
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

#include <sys/unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

#define LUAB_SYS_UNISTD_LIB_ID    1593623310
#define LUAB_SYS_UNISTD_LIB_KEY   "unistd"

extern luab_module_t luab_sys_unistd_lib;

/*
 * Interface against <sys/unistd.h>
 */

static luab_table_t luab_sys_unistd_vec[] = {
    LUAB_INT("_POSIX_ADVISORY_INFO",          _POSIX_ADVISORY_INFO),
    LUAB_INT("_POSIX_ASYNCHRONOUS_IO",        _POSIX_ASYNCHRONOUS_IO),
    LUAB_INT("_POSIX_CHOWN_RESTRICTED",       _POSIX_CHOWN_RESTRICTED),
    LUAB_INT("_POSIX_CLOCK_SELECTION",        _POSIX_CLOCK_SELECTION),
    LUAB_INT("_POSIX_CPUTIME",                _POSIX_CPUTIME),
    LUAB_INT("_POSIX_FSYNC",                  _POSIX_FSYNC),
    LUAB_INT("_POSIX_IPV6",                   _POSIX_IPV6),
    LUAB_INT("_POSIX_JOB_CONTROL",            _POSIX_JOB_CONTROL),
    LUAB_INT("_POSIX_MAPPED_FILES",           _POSIX_MAPPED_FILES),
    LUAB_INT("_POSIX_MEMLOCK",                _POSIX_MEMLOCK),
    LUAB_INT("_POSIX_MEMLOCK_RANGE",          _POSIX_MEMLOCK_RANGE),
    LUAB_INT("_POSIX_MEMORY_PROTECTION",      _POSIX_MEMORY_PROTECTION),
    LUAB_INT("_POSIX_MESSAGE_PASSING",        _POSIX_MESSAGE_PASSING),
    LUAB_INT("_POSIX_MONOTONIC_CLOCK",        _POSIX_MONOTONIC_CLOCK),
    LUAB_INT("_POSIX_NO_TRUNC",               _POSIX_NO_TRUNC),
    LUAB_INT("_POSIX_PRIORITIZED_IO",         _POSIX_PRIORITIZED_IO),
    LUAB_INT("_POSIX_PRIORITY_SCHEDULING",    _POSIX_PRIORITY_SCHEDULING),
    LUAB_INT("_POSIX_RAW_SOCKETS",            _POSIX_RAW_SOCKETS),
    LUAB_INT("_POSIX_REALTIME_SIGNALS",       _POSIX_REALTIME_SIGNALS),
    LUAB_INT("_POSIX_SEMAPHORES",             _POSIX_SEMAPHORES),
    LUAB_INT("_POSIX_SHARED_MEMORY_OBJECTS",  _POSIX_SHARED_MEMORY_OBJECTS),
    LUAB_INT("_POSIX_SPORADIC_SERVER",        _POSIX_SPORADIC_SERVER),
    LUAB_INT("_POSIX_SYNCHRONIZED_IO",        _POSIX_SYNCHRONIZED_IO),
    LUAB_INT("_POSIX_TIMEOUTS",               _POSIX_TIMEOUTS),
    LUAB_INT("_POSIX_TIMERS",                 _POSIX_TIMERS),
    LUAB_INT("_POSIX_TYPED_MEMORY_OBJECTS",   _POSIX_TYPED_MEMORY_OBJECTS),
    LUAB_INT("_POSIX_VDISABLE",               _POSIX_VDISABLE),
    LUAB_INT("_XOPEN_SHM",                    _XOPEN_SHM),
    LUAB_INT("_XOPEN_STREAMS",                _XOPEN_STREAMS),
#ifdef	_NOT_AVAILABLE
    LUAB_INT("_POSIX_SAVED_IDS",              _POSIX_SAVED_IDS),
#endif
    LUAB_INT("_POSIX_VERSION",                _POSIX_VERSION),
    LUAB_INT("F_OK",                          F_OK),
    LUAB_INT("X_OK",                          X_OK),
    LUAB_INT("W_OK",                          W_OK),
    LUAB_INT("R_OK",                          R_OK),
    LUAB_INT("SEEK_SET",                      SEEK_SET),
    LUAB_INT("SEEK_CUR",                      SEEK_CUR),
    LUAB_INT("SEEK_END",                      SEEK_END),
    LUAB_INT("SEEK_DATA",                     SEEK_DATA),
    LUAB_INT("SEEK_HOLE",                     SEEK_HOLE),
    LUAB_INT("L_SET",                         L_SET),
    LUAB_INT("L_INCR",                        L_INCR),
    LUAB_INT("L_XTND",                        L_XTND),
    LUAB_INT("_PC_LINK_MAX",                  _PC_LINK_MAX),
    LUAB_INT("_PC_MAX_CANON",                 _PC_MAX_CANON),
    LUAB_INT("_PC_MAX_INPUT",                 _PC_MAX_INPUT),
    LUAB_INT("_PC_NAME_MAX",                  _PC_NAME_MAX),
    LUAB_INT("_PC_PATH_MAX",                  _PC_PATH_MAX),
    LUAB_INT("_PC_PIPE_BUF",                  _PC_PIPE_BUF),
    LUAB_INT("_PC_CHOWN_RESTRICTED",          _PC_CHOWN_RESTRICTED),
    LUAB_INT("_PC_NO_TRUNC",                  _PC_NO_TRUNC),
    LUAB_INT("_PC_VDISABLE",                  _PC_VDISABLE),
    LUAB_INT("_PC_ASYNC_IO",                  _PC_ASYNC_IO),
    LUAB_INT("_PC_PRIO_IO",                   _PC_PRIO_IO),
    LUAB_INT("_PC_SYNC_IO",                   _PC_SYNC_IO),
    LUAB_INT("_PC_ALLOC_SIZE_MIN",            _PC_ALLOC_SIZE_MIN),
    LUAB_INT("_PC_FILESIZEBITS",              _PC_FILESIZEBITS),
    LUAB_INT("_PC_REC_INCR_XFER_SIZE",        _PC_REC_INCR_XFER_SIZE),
    LUAB_INT("_PC_REC_MAX_XFER_SIZE",         _PC_REC_MAX_XFER_SIZE),
    LUAB_INT("_PC_REC_MIN_XFER_SIZE",         _PC_REC_MIN_XFER_SIZE),
    LUAB_INT("_PC_REC_XFER_ALIGN",            _PC_REC_XFER_ALIGN),
    LUAB_INT("_PC_SYMLINK_MAX",               _PC_SYMLINK_MAX),
    LUAB_INT("_PC_ACL_EXTENDED",              _PC_ACL_EXTENDED),
    LUAB_INT("_PC_ACL_PATH_MAX",              _PC_ACL_PATH_MAX),
    LUAB_INT("_PC_CAP_PRESENT",               _PC_CAP_PRESENT),
    LUAB_INT("_PC_INF_PRESENT",               _PC_INF_PRESENT),
    LUAB_INT("_PC_MAC_PRESENT",               _PC_MAC_PRESENT),
    LUAB_INT("_PC_ACL_NFS4",                  _PC_ACL_NFS4),
    LUAB_INT("_PC_MIN_HOLE_SIZE",             _PC_MIN_HOLE_SIZE),
    LUAB_INT("RFNAMEG",                       RFNAMEG),
    LUAB_INT("RFENVG",                        RFENVG),
    LUAB_INT("RFFDG",                         RFFDG),
    LUAB_INT("RFNOTEG",                       RFNOTEG),
    LUAB_INT("RFPROC",                        RFPROC),
    LUAB_INT("RFMEM",                         RFMEM),
    LUAB_INT("RFNOWAIT",                      RFNOWAIT),
    LUAB_INT("RFCNAMEG",                      RFCNAMEG),
    LUAB_INT("RFCENVG",                       RFCENVG),
    LUAB_INT("RFCFDG",                        RFCFDG),
    LUAB_INT("RFTHREAD",                      RFTHREAD),
    LUAB_INT("RFSIGSHARE",                    RFSIGSHARE),
    LUAB_INT("RFLINUXTHPN",                   RFLINUXTHPN),
    LUAB_INT("RFSTOPPED",                     RFSTOPPED),
    LUAB_INT("RFHIGHPID",                     RFHIGHPID),
    LUAB_INT("RFTSIGZMB",                     RFTSIGZMB),
    LUAB_INT("RFTSIGSHIFT",                   RFTSIGSHIFT),
    LUAB_INT("RFTSIGMASK",                    RFTSIGMASK),
    LUAB_INT("RFPROCDESC",                    RFPROCDESC),
    LUAB_INT("RFPPWAIT",                      RFPPWAIT),
    LUAB_INT("RFFLAGS",                       RFFLAGS),
    LUAB_INT("RFKERNELONLY",                  RFKERNELONLY),
    LUAB_INT(NULL, 0)
};

luab_module_t luab_sys_unistd_lib = {
    .cookie = LUAB_SYS_UNISTD_LIB_ID,
    .name = LUAB_SYS_UNISTD_LIB_KEY,
    .vec = luab_sys_unistd_vec,
};
