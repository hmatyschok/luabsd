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

#include <sys/file.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

#define LUAB_SYS_FILE_LIB_ID    1593623310
#define LUAB_SYS_FILE_LIB_KEY    "file"

extern luab_module_t luab_sys_file_lib;

/*
 * Service primitives.
 */

/*
 * Interface against <sys/file.h>.
 */

static luab_module_table_t luab_sys_file_vec[] = {
    LUAB_INT("DTYPE_NONE",        DTYPE_NONE),
    LUAB_INT("DTYPE_VNODE",       DTYPE_VNODE),
    LUAB_INT("DTYPE_SOCKET",      DTYPE_SOCKET),
    LUAB_INT("DTYPE_PIPE",        DTYPE_PIPE),
    LUAB_INT("DTYPE_FIFO",        DTYPE_FIFO),
    LUAB_INT("DTYPE_KQUEUE",      DTYPE_KQUEUE),
    LUAB_INT("DTYPE_CRYPTO",      DTYPE_CRYPTO),
    LUAB_INT("DTYPE_MQUEUE",      DTYPE_MQUEUE),
    LUAB_INT("DTYPE_SHM",         DTYPE_SHM),
    LUAB_INT("DTYPE_SEM",         DTYPE_SEM),
    LUAB_INT("DTYPE_PTS",         DTYPE_PTS),
    LUAB_INT("DTYPE_DEV",         DTYPE_DEV),
    LUAB_INT("DTYPE_PROCDESC",    DTYPE_PROCDESC),
    LUAB_INT("DTYPE_LINUXEFD",    DTYPE_LINUXEFD),
    LUAB_INT("DTYPE_LINUXTFD",    DTYPE_LINUXTFD),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_file_lib = {
    .m_id       = LUAB_SYS_FILE_LIB_ID,
    .m_name     = LUAB_SYS_FILE_LIB_KEY,
    .m_vec      = luab_sys_file_vec,
};
