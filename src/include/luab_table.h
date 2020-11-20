/*-
 * Copyright (c) 2020 Henning Matyschok
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

#define LUAB_TABLE_XS_FLAG  0x7e

/*
 * Service primitives.
 */

size_t   luab_checktable(lua_State *, int);
size_t   luab_checktableisnil(lua_State *, int);
size_t   luab_checkltable(lua_State *, int, size_t);
size_t   luab_checkltableisnil(lua_State *, int, size_t);

void     luab_table_init(lua_State *, int);
void     luab_table_free(luab_table_t *);

void     luab_table_iovec_free(luab_table_t *);

/*
 * Error handler.
 */

void     luab_table_argerror(lua_State *, int, luab_table_t *, int);

/*
 * Generator functions.
 */

luab_table_t     *luab_table_allocnil(size_t, size_t);
luab_table_t     *luab_table_alloc(lua_State *, int, size_t, size_t);

luab_table_t     *luab_newvector(lua_State *, int, size_t);
luab_table_t     *luab_newvectornil(lua_State *, int, size_t);
luab_table_t     *luab_newlvector(lua_State *, int, size_t, size_t);
luab_table_t     *luab_newlvectornil(lua_State *, int, size_t, size_t);

/*
 * Access functions, [stack -> C]
 */

luab_table_t     *luab_table_checkargv(lua_State *, int);
luab_table_t     *luab_table_toxargp(lua_State *, int);

luab_table_t     *luab_table_checkgid(lua_State *, int);
luab_table_t     *luab_table_checku_short(lua_State *, int);

luab_table_t     *luab_table_tolxargp(lua_State *, int, size_t);

luab_table_t     *luab_table_checklgid(lua_State *, int, size_t);
luab_table_t     *luab_table_checklu_short(lua_State *, int, size_t);

luab_table_t     *luab_table_checkxdata(lua_State *, int, luab_module_t *);
luab_table_t     *luab_table_checklxdata(lua_State *, int, luab_module_t *, size_t);

/*
 * Access functions, [C -> stack].
 */

void     luab_table_pushgid(lua_State *, int, luab_table_t *, int, int);

void     luab_table_pushxdata(lua_State *, int, luab_module_t *,
    luab_table_t *, int, int);

#endif /* _LUAB_TABLE_H_ */
