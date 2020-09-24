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

#include <sys/param.h>

#include <errno.h>
#include <string.h>
#include <time.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_types.h"

#define LUABSD_TIME_LIB_ID    1594167179
#define LUABSD_TIME_LIB_KEY    "time"

extern luab_module_t luab_time_lib;

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(TM)).
 *
 * @function tm_create
 *
 * @param data          Instance of (LUA_TUSERDATA(TM)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (tm [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage tm [, err, msg ] = bsd.time.tm_create([ data ])
 */
static int
luab_tm_create(lua_State *L)
{
    return (luab_create(L, 1, luab_mx(TM), NULL));
}

/*
 * Interface against <time.h>.
 */

static luab_table_t luab_time_vec[] = { /* time.h */
#if __POSIX_VISIBLE > 0 && __POSIX_VISIBLE < 200112 || __BSD_VISIBLE
    LUABSD_INT("CLK_TCK",   CLK_TCK),
#endif
#if !defined(CLOCK_REALTIME) && __POSIX_VISIBLE >= 200112
    LUABSD_INT("CLOCK_REALTIME",    CLOCK_REALTIME),
#ifdef __BSD_VISIBLE
    LUABSD_INT("CLOCK_VIRTUAL", CLOCK_VIRTUAL),
    LUABSD_INT("CLOCK_PROF",    CLOCK_PROF),
#endif
    LUABSD_INT("CLOCK_MONOTONIC",   CLOCK_MONOTONIC),
    LUABSD_INT("CLOCK_UPTIME",  CLOCK_UPTIME),
    LUABSD_INT("CLOCK_UPTIME_PRECISE",  CLOCK_UPTIME_PRECISE),
    LUABSD_INT("CLOCK_UPTIME_FAST", CLOCK_UPTIME_FAST),
    LUABSD_INT("CLOCK_REALTIME_PRECISE",    CLOCK_REALTIME_PRECISE),
    LUABSD_INT("CLOCK_REALTIME_FAST",   CLOCK_REALTIME_FAST),
    LUABSD_INT("CLOCK_MONOTONIC_PRECISE",   CLOCK_MONOTONIC_PRECISE),
    LUABSD_INT("CLOCK_MONOTONIC_FAST",  CLOCK_MONOTONIC_FAST),
    LUABSD_INT("CLOCK_SECOND",  CLOCK_SECOND),
    LUABSD_INT("CLOCK_THREAD_CPUTIME_ID",   CLOCK_THREAD_CPUTIME_ID),
    LUABSD_INT("CLOCK_PROCESS_CPUTIME_ID",  CLOCK_PROCESS_CPUTIME_ID),
#endif /* !defined(CLOCK_REALTIME) && __POSIX_VISIBLE >= 200112 */
#if !defined(TIMER_ABSTIME) && __POSIX_VISIBLE >= 200112
#if __BSD_VISIBLE
    LUABSD_INT("TIMER_RELTIME", TIMER_RELTIME),
#endif
    LUABSD_INT("TIMER_ABSTIME", TIMER_ABSTIME),
#endif /* !defined(TIMER_ABSTIME) && __POSIX_VISIBLE >= 200112 */
    LUABSD_INT("CLOCKS_PER_SEC",    CLOCKS_PER_SEC),
    LUABSD_FUNC("tm_create", luab_tm_create),
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_time_lib = {
    .cookie = LUABSD_TIME_LIB_ID,
    .name = LUABSD_TIME_LIB_KEY,
    .vec = luab_time_vec,
};
