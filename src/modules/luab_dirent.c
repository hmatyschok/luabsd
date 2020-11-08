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

#include <dirent.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_DIRENT_LIB_ID    1604795103
#define LUAB_DIRENT_LIB_KEY    "dirent"

extern luab_module_t luab_dirent_lib;

/*
 * Service primitives.
 */

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(DIR)).
 *
 * @function dir_create
 *
 * @param dir               Instance of (LUA_TUSERDATA(DIR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage dir [, err, msg ] = bsd.dirent.dir_create([ dir ])
 */
static int
luab_dir_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(DIR), NULL));
}

/*
 * Interface against <dirent.h>.
 */

static luab_module_table_t luab_dirent_vec[] = { /* dirent.h */
#if __BSD_VISIBLE
    LUAB_INT("DIRBLKSIZ",               DIRBLKSIZ),
    LUAB_INT("DTF_HIDEW",               DTF_HIDEW),
    LUAB_INT("DTF_NODUP",               DTF_NODUP),
    LUAB_INT("DTF_REWIND",              DTF_REWIND),
    LUAB_INT("__DTF_READALL",           __DTF_READALL),
    LUAB_INT("__DTF_SKIPREAD",          __DTF_SKIPREAD),
#endif /* __BSD_VISIBLE */
    LUAB_FUNC("dir_create",             luab_dir_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_dirent_lib = {
    .m_cookie   = LUAB_DIRENT_LIB_ID,
    .m_name     = LUAB_DIRENT_LIB_KEY,
    .m_vec      = luab_dirent_vec,
};
