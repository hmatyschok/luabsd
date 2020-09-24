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

#include <sys/reboot.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

#define LUABSD_SYS_REBOOT_LIB_ID    1600295164
#define LUABSD_SYS_REBOOT_LIB_KEY   "reboot"

extern luab_module_t luab_sys_reboot_lib;

/*
 * Interface against <sys/reboot.h>
 */

static luab_table_t luab_sys_reboot_vec[] = {
	LUABSD_INT("RB_AUTOBOOT",	RB_AUTOBOOT),
	LUABSD_INT("RB_ASKNAME",	RB_ASKNAME),
	LUABSD_INT("RB_SINGLE",	    RB_SINGLE),
	LUABSD_INT("RB_NOSYNC",	    RB_NOSYNC),
	LUABSD_INT("RB_HALT",	    RB_HALT),
	LUABSD_INT("RB_INITNAME",	RB_INITNAME),
	LUABSD_INT("RB_DFLTROOT",	RB_DFLTROOT),
	LUABSD_INT("RB_KDB",	    RB_KDB),
	LUABSD_INT("RB_RDONLY",	    RB_RDONLY),
	LUABSD_INT("RB_DUMP",	    RB_DUMP),
	LUABSD_INT("RB_MINIROOT",	RB_MINIROOT),
	LUABSD_INT("RB_VERBOSE",	RB_VERBOSE),
	LUABSD_INT("RB_SERIAL",	    RB_SERIAL),
	LUABSD_INT("RB_CDROM",	    RB_CDROM),
	LUABSD_INT("RB_POWEROFF",	RB_POWEROFF),
	LUABSD_INT("RB_GDB",	    RB_GDB),
	LUABSD_INT("RB_MUTE",	    RB_MUTE),
	LUABSD_INT("RB_SELFTEST",	RB_SELFTEST),
	LUABSD_INT("RB_RESERVED1",	RB_RESERVED1),
	LUABSD_INT("RB_RESERVED2",	RB_RESERVED2),
	LUABSD_INT("RB_PAUSE",	    RB_PAUSE),
	LUABSD_INT("RB_REROOT", 	RB_REROOT),
	LUABSD_INT("RB_POWERCYCLE", RB_POWERCYCLE),
	LUABSD_INT("RB_PROBE",	    RB_PROBE),
	LUABSD_INT("RB_MULTIPLE",	RB_MULTIPLE),
	LUABSD_INT("RB_BOOTINFO",	RB_BOOTINFO),
	LUABSD_INT(NULL, 0)
};

luab_module_t luab_sys_reboot_lib = {
    .cookie = LUABSD_SYS_REBOOT_LIB_ID,
    .name = LUABSD_SYS_REBOOT_LIB_KEY,
    .vec = luab_sys_reboot_vec,
};
