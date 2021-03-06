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

static pthread_mutex_t luab_thread_mtx;

static LIST_HEAD(, luab_thread) luab_thread_pool =
        LIST_HEAD_INITIALIZER(luab_thread_pool);

/*
 * Primitives for threading operations.
 */

void
luab_thread_close(luab_thread_t *thr, int narg)
{
    if (thr != NULL) {
        (void)pthread_cancel(thr->thr_id);

        if (thr->thr_child != NULL)
            lua_pop(thr->thr_child, narg);

        LIST_REMOVE(thr, thr_next);
        luab_core_free(thr, sizeof(*thr));
    } else
        luab_core_err(EX_DATAERR, __func__, ENOENT);
}

luab_thread_t *
luab_thread_alloc(lua_State *L, int narg, const char *fname)
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

                LIST_INSERT_HEAD(&luab_thread_pool, thr, thr_next);
                return (thr);
            } else
                luab_core_err(EX_UNAVAILABLE, __func__, ENOMEM);
        } else
            luab_core_err(EX_UNAVAILABLE, __func__, ENXIO);
    } else
        luab_core_err(EX_UNAVAILABLE, __func__, errno);

    return (NULL);
}

/*
 * Callback functions.
 */

void
luab_thread_atfork(void)
{
    (void)raise(SIGUSR1);
}

void
luab_thread_once(void)
{
    (void)raise(SIGUSR1);
}

void
luab_thread_signal(int arg __unused)
{
    (void)raise(SIGUSR2);
}

void *
luab_thread_pcall(void *arg)
{
    luab_thread_t *thr;

    if ((thr = (luab_thread_t *)arg) != NULL) {

        if (thr->thr_child != NULL) {
            lua_getfield(thr->thr_child, LUA_REGISTRYINDEX, thr->thr_fname);

            if (lua_pcall(thr->thr_child, 0, 0, 0) != 0)
                luab_core_err(EX_DATAERR, thr->thr_fname, ENXIO);

            luab_thread_close(thr, 1);
        } else
            luab_core_err(EX_DATAERR, __func__, ENXIO);
    } else
        luab_core_err(EX_OSERR, __func__, ENOENT);

    return (NULL);
}

void *
luab_thread_sigwait(void *arg)
{
    luab_thread_t *thr;
    int sig;

    if ((thr = (luab_thread_t *)arg) != NULL) {

        while (1) {

            if (sigwait(&thr->thr_sigset, &sig) == 0)
                (void)luab_thread_pcall(arg);
            else
                luab_thread_close(thr, 1);
        }
    }
    return (NULL);
}

/*
 * Locking primitives.
 */

void
luab_thread_mtx_lock(lua_State *L __unused, const char *fname)
{
    if (pthread_mutex_lock(&luab_thread_mtx) != 0)
        luab_core_err(EX_OSERR, fname, errno);
}

void
luab_thread_mtx_unlock(lua_State *L __unused, const char *fname)
{
    if (pthread_mutex_unlock(&luab_thread_mtx) != 0)
        luab_core_err(EX_OSERR, fname, errno);
}

/*
 * Main entry point for loadlib(3).
 */

void
luab_thread_initpool(lua_State *L __unused)
{
    pthread_mutexattr_t attr;

    if (pthread_mutexattr_init(&attr) != 0)
        luab_core_err(EX_OSERR, __func__, errno);

    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL) != 0)
        luab_core_err(EX_OSERR, __func__, errno);

    if (pthread_mutex_init(&luab_thread_mtx, &attr) != 0)
        luab_core_err(EX_OSERR, __func__, errno);

    LIST_INIT(&luab_thread_pool);
}
