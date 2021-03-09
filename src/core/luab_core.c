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

#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_table.h"
#include "luab_modules.h"

static const char *luab_copyright =
    " Copyright (c) 2020, 2021 Henning Matyschok\n"
    " All rights reserved.\n"
    "\n";

LUAMOD_API int  luaopen_bsd(lua_State *);

static void
luab_core_initparam(lua_State *L __unused, luab_sysconf_vec_t *vec)
{
    luab_sysconf_vec_t *tok;

    if ((tok = vec) != NULL) {

        do {
            long scx;

            if (tok->scv_val != NULL) {

                if ((scx = sysconf(tok->scv_key)) < 0)
                    *(tok->scv_val) = tok->scv_dflt;
                else
                    *(tok->scv_val) = (u_long)scx;
            } else
                errno = ENOENT;

            tok++;
        } while (tok->scv_val != NULL);
    } else
        luab_core_err(EX_DATAERR, __func__, ENXIO);
}

static void
luab_core_initlib(lua_State *L, int narg, luab_libdata_t *vec)
{
    luab_libdata_t *tok;

    if ((tok = vec) != NULL) {
        luab_table_init(L, 1);

        do {

            if (tok->lib_vec != NULL)
                luab_env_registerlib(L, narg, tok->lib_vec, tok->lib_name);
            else
                errno = ENOENT;

            tok++;
        } while (tok->lib_vec != NULL);

        if (narg < 0)
            lua_pushvalue(L, narg + 1);
        else
            lua_pushvalue(L, narg - 1);
    } else
        luab_core_err(EX_DATAERR, __func__, ENXIO);

    /* register complex data-types. */
    luab_env_registertype(L, -2, luab_env_type_vec);
}

/*
 * Main entry point for loadlib(3).
 */

LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    (void)printf("%s", luab_copyright);

    /* initialize constraints */
    luab_core_initparam(L, luab_env_param);

    /* register modules */
    luab_core_initlib(L, -2, luab_env_libdata_vec);

    /* initialize threading pool */
    luab_thread_initpool(L);

    return (1);
}
