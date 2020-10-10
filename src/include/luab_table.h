/*-
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

#ifndef _LUAB_TABLE_H_
#define _LUAB_TABLE_H_

/*
 * Service primitives.
 */

#define luab_table_xlen(vec, type) \
    ((sizeof(vec)) / (sizeof(type)))

void     luab_table_populate(lua_State *, int);

size_t   luab_checktable(lua_State *, int);
size_t   luab_checktableisnil(lua_State *, int);
size_t   luab_checkltable(lua_State *, int, size_t);
size_t   luab_checkltableisnil(lua_State *, int, size_t);

/*
 * Generator functions.
 */

void     *luab_alloctable(lua_State *, int, size_t, size_t);

void     *luab_newvector(lua_State *, int, size_t *, size_t);
void     *luab_newvectornil(lua_State *, int, size_t *, size_t);
void     *luab_newlvector(lua_State *, int, size_t, size_t);

/*
 * Access functions, [stack -> C].
 */

const char   **luab_table_checkargv(lua_State *, int);
double   *luab_table_checkdouble(lua_State *, int, size_t *);
const void   **luab_table_tolxargp(lua_State *, int, size_t);
u_short  *luab_table_checklu_short(lua_State *, int, size_t);
int  *luab_table_checklint(lua_State *, int, size_t);
gid_t    *luab_table_checklgid(lua_State *, int, size_t);

/* C structures */
struct iovec     *luab_table_checkiovec(lua_State *, int, size_t *);
struct iovec     *luab_table_checkliovec(lua_State *, int, size_t);
struct timespec  *luab_table_checkltimespec(lua_State *, int, size_t);

/*
 * Access functions, [C -> stack].
 */

void     luab_table_pushdouble(lua_State *, int, void *, int);
void     luab_table_pushint(lua_State *, int, void *, int);
void     luab_table_pushldouble(lua_State *, int, void *, size_t, int);
void     luab_table_pushlgid(lua_State *, int, void *, size_t, int);

/* C structures */
void     luab_table_pushliovec(lua_State *, int, void *, size_t, int);
void     luab_table_pushltimespec(lua_State *, int, void *, size_t, int);

#endif /* _LUAB_TABLE_H_ */
