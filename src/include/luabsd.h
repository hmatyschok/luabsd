/*-
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

#ifndef _LUABSD_H_
#define _LUABSD_H_

#include <sys/param.h>
#include <sys/mbuf.h>   /* XXX */
#include <sys/socket.h>
#include <sys/queue.h>

#include <net/if.h> /* XXX */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h> /* XXX */

#include "luab_env.h"

void     luab_core_free(void *, size_t);
void     luab_core_freestr(caddr_t);

void     *luab_core_alloc(size_t, size_t);
char     *luab_core_allocstring(const char *, size_t *);

void     luab_core_err(int, const char *, int);
void     luab_core_errx(int, const char *, ...);
void     luab_core_warn(const char *, ...);
void     luab_core_argerror(lua_State *, int, void *, size_t, size_t, int);

/*
 * Primitives for threading operations.
 */

typedef struct luab_thread {
    lua_State   *thr_child;
    lua_State   *thr_parent;
    sigset_t    thr_nsigset;
    char        thr_fname[LUAB_NAME_MAX+1];
} luab_thread_t;

void     luab_core_closethread(luab_thread_t *);
luab_thread_t    *luab_core_newthread(lua_State *, const char *);
void     *luab_core_pcall(void *);

/*
 * Primitives for operations over (LUA_TUSERDATA).
 */

int  luab_core_create(lua_State *, int, luab_module_t *, luab_module_t *);
int  luab_core_dump(lua_State *, int, luab_module_t *, size_t);
int  luab_core_gc(lua_State *, int, luab_module_t *);
int  luab_core_len(lua_State *, int, luab_module_t *);
int  luab_core_tostring(lua_State *, int, luab_module_t *);

int  luab_core_checkmaxargs(lua_State *, int);

/*
 * Access functions, n-th arg over argv, [stack -> C].
 */

void     *luab_checknil(lua_State *, int);

lua_Integer  luab_core_Integer_promotion_msk(int);

lua_Integer  luab_tointeger(lua_State *, int, lua_Integer);
lua_Integer  luab_tolinteger(lua_State *, int, int);

lua_Integer  luab_toxinteger(lua_State *, int, luab_module_t *, lua_Integer);
lua_Integer  luab_tolxinteger(lua_State *, int, luab_module_t *, int);

lua_Integer  luab_checkinteger(lua_State *, int, lua_Integer);
lua_Integer  luab_checklinteger(lua_State *, int, int);

lua_Integer  luab_checkxinteger(lua_State *, int, luab_module_t *, lua_Integer);
lua_Integer  luab_checklxinteger(lua_State *, int, luab_module_t *, int);

lua_Number   luab_toxnumber(lua_State *, int, luab_module_t *);
lua_Number   luab_checkxnumber(lua_State *, int, luab_module_t *);

const char   *luab_islstring(lua_State *, int, size_t);
const char   *luab_tolstring(lua_State *, int, size_t);
const char   *luab_checklstring(lua_State *, int, size_t, size_t *);
const char   *luab_checklstringisnil(lua_State *, int, size_t, size_t *);

char     *luab_checklstringalloc(lua_State *, int, size_t, size_t *);
char     *luab_checklxstring(lua_State *, int, size_t, size_t *);

luab_thread_t    *luab_checkfunction(lua_State *, int, const char *);

/*
 * Access functions, [C -> stack].
 */

void     luab_rawsetinteger(lua_State *, int, lua_Integer, lua_Integer);
void     luab_rawsetnumber(lua_State *, int, lua_Integer, lua_Number);
void     luab_rawsetstring(lua_State *, int, lua_Integer, const char *);
void     luab_rawsetfstring(lua_State *, int, lua_Integer, const char *, ...);
void     luab_rawsetldata(lua_State *, int, lua_Integer, void *, size_t);

void     luab_setcfunction(lua_State *, int, const char *, lua_CFunction);
void     luab_setinteger(lua_State *, int, const char *, lua_Integer);
void     luab_setnumber(lua_State *, int, const char *, lua_Number);

void     luab_setstring(lua_State *, int, const char *, const char *);
void     luab_setfstring(lua_State *, int, const char *, const char *, ...);
void     luab_setldata(lua_State *, int, const char *, void *, size_t);

int  luab_pusherr(lua_State *, int, int);

#define luab_pushxinteger(L, x) \
    (luab_pushinteger((L), (lua_Integer)(x)))
#define luab_pushxnumber(L, x, err) \
    (luab_pushnumber((L), (lua_Number)(x), (err)))

int  luab_pushinteger(lua_State *, lua_Integer);
int  luab_pushnumber(lua_State *, lua_Number, int);
int  luab_pushnil(lua_State *);

int  luab_pushstring(lua_State *, const char *);
int  luab_pushfstring(lua_State *, const char *, ...);

int  luab_pushldata(lua_State *, void *, size_t);
#endif /* _LUABSD_H_ */
