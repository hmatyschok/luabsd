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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <uuid.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern int luab_uuid_create(lua_State *);

#define LUABSD_UUID_LIB_ID    1593623310
#define LUABSD_UUID_LIB_KEY "uuid"

extern luab_module_t luab_uuid_lib;

/*
 * Interface against <uuid.h>.
 */

static luab_table_t luab_uuid_vec[] = {
    LUABSD_INT("uuid_s_ok", uuid_s_ok),
    LUABSD_INT("uuid_s_bad_version",    uuid_s_bad_version),
    LUABSD_INT("uuid_s_invalid_string_uuid",    uuid_s_invalid_string_uuid),
    LUABSD_INT("uuid_s_no_memory",  uuid_s_no_memory),
    LUABSD_FUNC("uuid_create",  luab_uuid_create),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_uuid_lib = {
    .cookie = LUABSD_UUID_LIB_ID,
    .name = LUABSD_UUID_LIB_KEY,
    .vec = luab_uuid_vec,
};
