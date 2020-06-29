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

#include <fcntl.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

luab_table_t luab_fcntl[] = {    /* fcntl.h */
    LUABSD_INT("O_RDONLY",   O_RDONLY),
    LUABSD_INT("O_WRONLY",   O_WRONLY),
    LUABSD_INT("O_RDWR", O_RDWR),
    LUABSD_INT("O_EXEC", O_EXEC),
    LUABSD_INT("O_NONBLOCK", O_NONBLOCK),
    LUABSD_INT("O_APPEND",   O_APPEND),
    LUABSD_INT("O_CREAT",    O_CREAT),
    LUABSD_INT("O_TRUNC",    O_TRUNC),
    LUABSD_INT("O_EXCL", O_EXCL),
    LUABSD_INT("O_SHLOCK",   O_SHLOCK),
    LUABSD_INT("O_EXLOCK",   O_EXLOCK),
    LUABSD_INT("O_DIRECT",   O_DIRECT),
    LUABSD_INT("O_FSYNC",    O_FSYNC),
    LUABSD_INT("O_SYNC", O_SYNC),
    LUABSD_INT("O_NOFOLLOW", O_NOFOLLOW),
    LUABSD_INT("O_NOCTTY",   O_NOCTTY),
    LUABSD_INT("O_TTY_INIT", O_TTY_INIT),
    LUABSD_INT("O_DIRECTORY", O_DIRECTORY),
    LUABSD_INT("O_CLOEXEC",  O_CLOEXEC),
    LUABSD_INT("O_VERIFY",   O_VERIFY),
    LUABSD_INT(NULL, 0)
};
