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

#include <pthread.h>
#include <signal.h>

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

static LIST_HEAD(, luab_thread) luab_core_threads =
        LIST_HEAD_INITIALIZER(luab_core_threads);

LUAMOD_API int  luaopen_bsd(lua_State *);

/*
 * Primitives for threading operations.
 */

void
luab_core_closethread(luab_thread_t *thr, int narg)
{
    if (thr != NULL) {

        if (thr->thr_child != NULL)
            lua_pop(thr->thr_child, narg);

        LIST_REMOVE(thr, thr_next);
        luab_core_free(thr, sizeof(*thr));
    } else
        luab_core_err(EX_DATAERR, __func__, ENOENT);
}

luab_thread_t *
luab_core_newthread(lua_State *L, int narg, const char *fname)
{
    luab_thread_t *thr;

    if ((thr = luab_core_alloc(1, sizeof(luab_thread_t))) != NULL) {
        thr->thr_mtx = PTHREAD_MUTEX_INITIALIZER;
        thr->thr_cv = PTHREAD_COND_INITIALIZER;

        if ((thr->thr_parent = L) != NULL) {

            if ((thr->thr_child = lua_newthread(L)) != NULL) {
                lua_pop(thr->thr_parent, narg);

                if (fname != NULL)
                    (void)snprintf(thr->thr_fname, luab_env_name_max, "%s", fname);

                LIST_INSERT_HEAD(&luab_core_threads, thr, thr_next);
                return (thr);
            } else
                luab_core_err(EX_UNAVAILABLE, __func__, ENOMEM);
        } else
            luab_core_err(EX_UNAVAILABLE, __func__, ENXIO);
    } else
        luab_core_err(EX_UNAVAILABLE, __func__, errno);

    return (NULL);
}

void *
luab_core_pcall(void *arg)
{
    luab_thread_t *thr;

    if ((thr = (luab_thread_t *)arg) != NULL) {

        if (thr->thr_child != NULL) {
            lua_getfield(thr->thr_child, LUA_REGISTRYINDEX, thr->thr_fname);

            if (lua_pcall(thr->thr_child, 0, 0, 0) != 0)
                luab_core_err(EX_DATAERR, thr->thr_fname, ENXIO);

            luab_core_closethread(thr, 1);
        } else
            luab_core_err(EX_DATAERR, __func__, ENXIO);
    } else
        luab_core_err(EX_OSERR, __func__, ENOENT);

    return (NULL);
}

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


    /* initialize threading pool */
    LIST_INIT(&luab_core_threads);


    return (1);
}
