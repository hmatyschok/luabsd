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

#include <cpio.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_CPIO_LIB_ID    1608690443
#define LUAB_CPIO_LIB_KEY    "cpio"

extern luab_module_t luab_cpio_lib;

/*
 * Interface against <cpio.h>.
 */

static luab_module_table_t luab_cpio_vec[] = {
    LUAB_INT("C_ISSOCK",            C_ISSOCK),
    LUAB_INT("C_ISLNK",             C_ISLNK),
    LUAB_INT("C_ISCTG",             C_ISCTG),
    LUAB_INT("C_ISREG",             C_ISREG),
    LUAB_INT("C_ISBLK",             C_ISBLK),
    LUAB_INT("C_ISDIR",             C_ISDIR),
    LUAB_INT("C_ISCHR",             C_ISCHR),
    LUAB_INT("C_ISFIFO",            C_ISFIFO),
    LUAB_INT("C_ISUID",             C_ISUID),
    LUAB_INT("C_ISGID",             C_ISGID),
    LUAB_INT("C_ISVTX",             C_ISVTX),
    LUAB_INT("C_IRUSR",             C_IRUSR),
    LUAB_INT("C_IWUSR",             C_IWUSR),
    LUAB_INT("C_IXUSR",             C_IXUSR),
    LUAB_INT("C_IRGRP",             C_IRGRP),
    LUAB_INT("C_IWGRP",             C_IWGRP),
    LUAB_INT("C_IXGRP",             C_IXGRP),
    LUAB_INT("C_IROTH",             C_IROTH),
    LUAB_INT("C_IWOTH",             C_IWOTH),
    LUAB_INT("C_IXOTH",             C_IXOTH),
    LUAB_STR("MAGIC",               MAGIC),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_cpio_lib = {
    .m_id       = LUAB_CPIO_LIB_ID,
    .m_name     = LUAB_CPIO_LIB_KEY,
    .m_vec      = luab_cpio_vec,
};
