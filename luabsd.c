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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdlib.h>
#include <uuid.h>

LUAMOD_API int  luaopen_bsd(lua_State *);

/* 
 * Wrapper for uuidgen(2), derived from implementation of uuidgen(1). 
 */

static int
bsd_uuidgen(lua_State *L)
{
    uuid_t uuid;
    char *buf;
    int status;

    if ((status = uuidgen(&uuid, 1)) != 0)
        return luaL_error(L, "unable to generate a UUID");

    uuid_to_string(&uuid, &buf, &status);

    if (status != uuid_s_ok)
        status = luaL_error(L, "cannot stringify a UUID");
    else {
        lua_pushstring(L, buf);
        status = 1;
    }
    free(buf);
    return status;
}

static const luaL_Reg bsdlib[] = {
    { "uuidgen",    bsd_uuidgen },
    { NULL, NULL }
};

LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    luaL_newlib(L, bsdlib);
    return 1;
}

