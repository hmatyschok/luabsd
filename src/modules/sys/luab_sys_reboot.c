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

#include <sys/reboot.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

#define LUAB_SYS_REBOOT_LIB_ID    1600295164
#define LUAB_SYS_REBOOT_LIB_KEY   "reboot"

extern luab_module_t luab_sys_reboot_lib;

/*
 * Interface against <sys/reboot.h>
 */

static luab_module_table_t luab_sys_reboot_vec[] = {
    LUAB_INT("RB_AUTOBOOT",         RB_AUTOBOOT),
    LUAB_INT("RB_ASKNAME",          RB_ASKNAME),
    LUAB_INT("RB_SINGLE",           RB_SINGLE),
    LUAB_INT("RB_NOSYNC",           RB_NOSYNC),
    LUAB_INT("RB_HALT",             RB_HALT),
    LUAB_INT("RB_INITNAME",         RB_INITNAME),
    LUAB_INT("RB_DFLTROOT",         RB_DFLTROOT),
    LUAB_INT("RB_KDB",              RB_KDB),
    LUAB_INT("RB_RDONLY",           RB_RDONLY),
    LUAB_INT("RB_DUMP",             RB_DUMP),
    LUAB_INT("RB_MINIROOT",         RB_MINIROOT),
    LUAB_INT("RB_VERBOSE",          RB_VERBOSE),
    LUAB_INT("RB_SERIAL",           RB_SERIAL),
    LUAB_INT("RB_CDROM",            RB_CDROM),
    LUAB_INT("RB_POWEROFF",         RB_POWEROFF),
    LUAB_INT("RB_GDB",              RB_GDB),
    LUAB_INT("RB_MUTE",             RB_MUTE),
    LUAB_INT("RB_SELFTEST",         RB_SELFTEST),
    LUAB_INT("RB_RESERVED1",        RB_RESERVED1),
    LUAB_INT("RB_RESERVED2",        RB_RESERVED2),
    LUAB_INT("RB_PAUSE",            RB_PAUSE),
    LUAB_INT("RB_REROOT",           RB_REROOT),
    LUAB_INT("RB_POWERCYCLE",       RB_POWERCYCLE),
    LUAB_INT("RB_PROBE",            RB_PROBE),
    LUAB_INT("RB_MULTIPLE",         RB_MULTIPLE),
    LUAB_INT("RB_BOOTINFO",         RB_BOOTINFO),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_reboot_lib = {
    .m_id       = LUAB_SYS_REBOOT_LIB_ID,
    .m_name     = LUAB_SYS_REBOOT_LIB_KEY,
    .m_vec      = luab_sys_reboot_vec,
};
