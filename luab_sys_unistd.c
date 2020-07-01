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

luab_table_t luab_sys_unistd_lib[] = {   /* sys/unistd.h */
    LUABSD_INT("_POSIX_ADVISORY_INFO",  _POSIX_ADVISORY_INFO),
    LUABSD_INT("_POSIX_ASYNCHRONOUS_IO",    _POSIX_ASYNCHRONOUS_IO),
    LUABSD_INT("_POSIX_CHOWN_RESTRICTED",   _POSIX_CHOWN_RESTRICTED),
    LUABSD_INT("_POSIX_CLOCK_SELECTION",    _POSIX_CLOCK_SELECTION),
    LUABSD_INT("_POSIX_CPUTIME",    _POSIX_CPUTIME),
    LUABSD_INT("_POSIX_FSYNC",  _POSIX_FSYNC),
    LUABSD_INT("_POSIX_IPV6",   _POSIX_IPV6),
    LUABSD_INT("_POSIX_JOB_CONTROL",    _POSIX_JOB_CONTROL),
    LUABSD_INT("_POSIX_MAPPED_FILES",   _POSIX_MAPPED_FILES),
    LUABSD_INT("_POSIX_MEMLOCK",    _POSIX_MEMLOCK),
    LUABSD_INT("_POSIX_MEMLOCK_RANGE",  _POSIX_MEMLOCK_RANGE),
    LUABSD_INT("_POSIX_MEMORY_PROTECTION",  _POSIX_MEMORY_PROTECTION),
    LUABSD_INT("_POSIX_MESSAGE_PASSING",    _POSIX_MESSAGE_PASSING),
    LUABSD_INT("_POSIX_MONOTONIC_CLOCK",    _POSIX_MONOTONIC_CLOCK),
    LUABSD_INT("_POSIX_NO_TRUNC",   _POSIX_NO_TRUNC),
    LUABSD_INT("_POSIX_PRIORITIZED_IO", _POSIX_PRIORITIZED_IO),
    LUABSD_INT("_POSIX_PRIORITY_SCHEDULING",    _POSIX_PRIORITY_SCHEDULING),
    LUABSD_INT("_POSIX_RAW_SOCKETS",    _POSIX_RAW_SOCKETS),
    LUABSD_INT("_POSIX_REALTIME_SIGNALS",   _POSIX_REALTIME_SIGNALS),
    LUABSD_INT("_POSIX_SEMAPHORES", _POSIX_SEMAPHORES),
    LUABSD_INT("_POSIX_SHARED_MEMORY_OBJECTS",  _POSIX_SHARED_MEMORY_OBJECTS),
    LUABSD_INT("_POSIX_SPORADIC_SERVER",    _POSIX_SPORADIC_SERVER),
    LUABSD_INT("_POSIX_SYNCHRONIZED_IO",    _POSIX_SYNCHRONIZED_IO),
    LUABSD_INT("_POSIX_TIMEOUTS",   _POSIX_TIMEOUTS),
    LUABSD_INT("_POSIX_TIMERS", _POSIX_TIMERS),
    LUABSD_INT("_POSIX_TYPED_MEMORY_OBJECTS",   _POSIX_TYPED_MEMORY_OBJECTS),
    LUABSD_INT("_POSIX_VDISABLE",   _POSIX_VDISABLE),
    LUABSD_INT("_XOPEN_SHM",    _XOPEN_SHM),
    LUABSD_INT("_XOPEN_STREAMS",    _XOPEN_STREAMS),
#ifdef	_NOT_AVAILABLE
    LUABSD_INT("_POSIX_SAVED_IDS",  _POSIX_SAVED_IDS),
#endif
    LUABSD_INT("_POSIX_VERSION",    _POSIX_VERSION),
    LUABSD_INT("F_OK",  F_OK),
    LUABSD_INT("X_OK",  X_OK),
    LUABSD_INT("W_OK",  W_OK),
    LUABSD_INT("R_OK",  R_OK),
    LUABSD_INT("SEEK_SET",  SEEK_SET),
    LUABSD_INT("SEEK_CUR",  SEEK_CUR),
    LUABSD_INT("SEEK_END",  SEEK_END),
    LUABSD_INT("SEEK_DATA", SEEK_DATA),
    LUABSD_INT("SEEK_HOLE", SEEK_HOLE),
    LUABSD_INT("L_SET", L_SET),
    LUABSD_INT("L_INCR",    L_INCR),
    LUABSD_INT("L_XTND",    L_XTND),
    LUABSD_INT("_PC_LINK_MAX",  _PC_LINK_MAX),
    LUABSD_INT("_PC_MAX_CANON", _PC_MAX_CANON),
    LUABSD_INT("_PC_MAX_INPUT", _PC_MAX_INPUT),
    LUABSD_INT("_PC_NAME_MAX",  _PC_NAME_MAX),
    LUABSD_INT("_PC_PATH_MAX",  _PC_PATH_MAX),
    LUABSD_INT("_PC_PIPE_BUF",  _PC_PIPE_BUF),
    LUABSD_INT("_PC_CHOWN_RESTRICTED",  _PC_CHOWN_RESTRICTED),
    LUABSD_INT("_PC_NO_TRUNC",  _PC_NO_TRUNC),
    LUABSD_INT("_PC_VDISABLE",  _PC_VDISABLE),
    LUABSD_INT("_PC_ASYNC_IO",  _PC_ASYNC_IO),
    LUABSD_INT("_PC_PRIO_IO",   _PC_PRIO_IO),
    LUABSD_INT("_PC_SYNC_IO",   _PC_SYNC_IO),
    LUABSD_INT("_PC_ALLOC_SIZE_MIN",    _PC_ALLOC_SIZE_MIN),
    LUABSD_INT("_PC_FILESIZEBITS",  _PC_FILESIZEBITS),
    LUABSD_INT("_PC_REC_INCR_XFER_SIZE",    _PC_REC_INCR_XFER_SIZE),
    LUABSD_INT("_PC_REC_MAX_XFER_SIZE", _PC_REC_MAX_XFER_SIZE),
    LUABSD_INT("_PC_REC_MIN_XFER_SIZE", _PC_REC_MIN_XFER_SIZE),
    LUABSD_INT("_PC_REC_XFER_ALIGN",    _PC_REC_XFER_ALIGN),
    LUABSD_INT("_PC_SYMLINK_MAX",   _PC_SYMLINK_MAX),
    LUABSD_INT("_PC_ACL_EXTENDED",  _PC_ACL_EXTENDED),
    LUABSD_INT("_PC_ACL_PATH_MAX",  _PC_ACL_PATH_MAX),
    LUABSD_INT("_PC_CAP_PRESENT",   _PC_CAP_PRESENT),
    LUABSD_INT("_PC_INF_PRESENT",   _PC_INF_PRESENT),
    LUABSD_INT("_PC_MAC_PRESENT",   _PC_MAC_PRESENT),
    LUABSD_INT("_PC_ACL_NFS4",  _PC_ACL_NFS4),
    LUABSD_INT("_PC_MIN_HOLE_SIZE", _PC_MIN_HOLE_SIZE),
    LUABSD_INT("RFNAMEG",   RFNAMEG),
    LUABSD_INT("RFENVG",    RFENVG),
    LUABSD_INT("RFFDG", RFFDG),
    LUABSD_INT("RFNOTEG",   RFNOTEG),
    LUABSD_INT("RFPROC",    RFPROC),
    LUABSD_INT("RFMEM", RFMEM),
    LUABSD_INT("RFNOWAIT",  RFNOWAIT),
    LUABSD_INT("RFCNAMEG",  RFCNAMEG),
    LUABSD_INT("RFCENVG",   RFCENVG),
    LUABSD_INT("RFCFDG",    RFCFDG),
    LUABSD_INT("RFTHREAD",  RFTHREAD),
    LUABSD_INT("RFSIGSHARE",    RFSIGSHARE),
    LUABSD_INT("RFLINUXTHPN",   RFLINUXTHPN),
    LUABSD_INT("RFSTOPPED", RFSTOPPED),
    LUABSD_INT("RFHIGHPID", RFHIGHPID),
    LUABSD_INT("RFTSIGZMB", RFTSIGZMB),
    LUABSD_INT("RFTSIGSHIFT",   RFTSIGSHIFT),
    LUABSD_INT("RFTSIGMASK",    RFTSIGMASK),
    LUABSD_INT("RFPROCDESC",    RFPROCDESC),
    LUABSD_INT("RFPPWAIT",  RFPPWAIT),
    LUABSD_INT("RFFLAGS",   RFFLAGS),
    LUABSD_INT("RFKERNELONLY",  RFKERNELONLY),
    LUABSD_INT(NULL, 0)
};
