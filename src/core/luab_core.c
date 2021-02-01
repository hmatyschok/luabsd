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
#include "luab_udata.h"

extern luab_sysconf_vec_t luab_env_param[];

extern luab_module_vec_t luab_env_typevec[];
extern luab_module_vec_t luab_env_arpa_vec[];
extern luab_module_vec_t luab_env_net_vec[];
extern luab_module_vec_t luab_env_sys_vec[];
extern luab_module_vec_t luab_env_xlocale_vec[];
extern luab_module_vec_t luab_env_core_vec[];
extern luab_module_vec_t luab_env_vec[];

static const char *copyright =
    " Copyright (c) 2020, 2021 Henning Matyschok\n"
    " All rights reserved.\n"
    "\n"
    "  The implementation of the interface against\n"
    "\n"
    "    #1: alarm(3),\n"
    "    #2: pthread_create(3),\n"
    "    #3: setitimer(2)\n"
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
 * Initializer.
 */

static void
luab_core_initparam(luab_sysconf_vec_t *vec)
{
    luab_sysconf_vec_t *tok;
    long scx;

    if ((tok = vec) != NULL) {

        do {
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
luab_core_registerlib(lua_State *L, int narg, luab_module_vec_t *vec, const char *name)
{
    luab_env_initmodule(L, narg, vec, name, 1);
}

static void
luab_core_registertype(lua_State *L, int narg, luab_module_vec_t *vec)
{
    luab_env_initmodule(L, narg, vec, NULL, 0);
}

/*
 * Main entry point for loadlib(3).
 */

LUAMOD_API int
luaopen_bsd(lua_State *L)
{
    (void)printf("%s", copyright);

    /* initialize constraints */
    luab_core_initparam(luab_env_param);

    /*
     * register modules
     *
     * XXX
     *  We shall externalize this.
     */
    lua_newtable(L);

    luab_core_registerlib(L, -2, luab_env_arpa_vec,     "arpa");
    luab_core_registerlib(L, -2, luab_env_core_vec,     "core");
    luab_core_registerlib(L, -2, luab_env_net_vec,      "net");
    luab_core_registerlib(L, -2, luab_env_sys_vec,      "sys");
    luab_core_registerlib(L, -2, luab_env_xlocale_vec,  "xlocale");
    luab_core_registerlib(L, -2, luab_env_vec,          NULL);

    lua_pushvalue(L, -1);

    /* register complex data-types. */
    luab_core_registertype(L, -2, luab_env_typevec);

    return (1);
}

/*
 * Service primitives.
 */

lua_State *
luab_core_newstate(void)
{
    lua_State *L;

    if ((L = luaL_newstate()) != NULL) {

        luaL_openlibs(L);
        luaL_requiref(L, "bsd", luaopen_bsd, 1);
        lua_pop(L, 1);
    } else
        luab_core_err(EX_UNAVAILABLE, __func__, ENOMEM);

    return (L);
}
