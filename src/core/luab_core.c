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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_modules.h"

static const char *luab_core_copyright =
    " Copyright (c) 2020, 2021 Henning Matyschok\n"
    " All rights reserved.\n"
    "\n"
    "  The implementation of the interface against\n"
    "\n"
    "    #1: alarm(3),\n"
    "\n"
    "  is derived from:\n"
    "\n"
    "   lalarm.c\n"
    "   an alarm library for Lua based on signal(3)\n"
    "   Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>\n"
    "   28 Jul 2018 12:47:52\n\n"
    "\n";


LUAMOD_API int  luaopen_bsd(lua_State *);

/*
 * Main entry point for loadlib(3).
 */

LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    (void)printf("%s", luab_core_copyright);

    /* initialize constraints */
    luab_env_initparam(luab_env_param);

    /*
     * register modules
     */
    luab_env_initlib(L, -2, luab_env_libdata_vec);

    /* register complex data-types. */
    luab_env_registertype(L, -2, luab_env_type_vec);

    return (1);
}
