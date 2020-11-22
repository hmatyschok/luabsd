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

#include <sys/param.h>
#include <sys/jail.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

#define LUAB_SYS_JAIL_LIB_ID    1606009660
#define LUAB_SYS_JAIL_LIB_KEY   "jail"

extern luab_module_t luab_sys_jail_lib;

/*
 * Service primitives.
 */

/*
 * Generator functions.
 */

/*
 * Interface against <sys/jail.h>
 */

static luab_module_table_t luab_sys_jail_vec[] = {
    LUAB_INT("JAIL_API_VERSION",        JAIL_API_VERSION),
    LUAB_INT("XPRISON_VERSION",         XPRISON_VERSION),
    LUAB_INT("PRISON_STATE_INVALID",    PRISON_STATE_INVALID),
    LUAB_INT("PRISON_STATE_ALIVE",      PRISON_STATE_ALIVE),
    LUAB_INT("PRISON_STATE_DYING",      PRISON_STATE_DYING),
    LUAB_INT("JAIL_CREATE",             JAIL_CREATE),
    LUAB_INT("JAIL_UPDATE",             JAIL_UPDATE),
    LUAB_INT("JAIL_ATTACH",             JAIL_ATTACH),
    LUAB_INT("JAIL_DYING",              JAIL_DYING),
    LUAB_INT("JAIL_SET_MASK",           JAIL_SET_MASK),
    LUAB_INT("JAIL_GET_MASK",           JAIL_GET_MASK),
    LUAB_INT("JAIL_SYS_DISABLE",        JAIL_SYS_DISABLE),
    LUAB_INT("JAIL_SYS_NEW",            JAIL_SYS_NEW),
    LUAB_INT("JAIL_SYS_INHERIT",        JAIL_SYS_INHERIT),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_jail_lib = {
    .m_cookie   = LUAB_SYS_JAIL_LIB_ID,
    .m_name     = LUAB_SYS_JAIL_LIB_KEY,
    .m_vec      = luab_sys_jail_vec,
};
