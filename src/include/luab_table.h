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

typedef struct luab_table {
    void        *tbl_vec;
    size_t      tbl_card;
    size_t      tbl_sz;
} luab_table_t;

#define LUAB_TABLE_XS_FLAG  0x7e

typedef luab_table_t * (*luab_xtable_fn)(lua_State *, int);

typedef struct luab_xtable {
    luab_xtable_fn  xt_fn;
    size_t          xt_sz;
} luab_xtable_t;

#define LUAB_XTABLE_SENTINEL \
    { .xt_fn = NULL, .xt_fn = 0 }

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
 * Access functions, [stack -> C].
 */

luab_table_t     *luab_table_checkargv(lua_State *, int);
luab_table_t     *luab_table_toxargp(lua_State *, int);

luab_table_t     *luab_table_checkdouble(lua_State *, int);
luab_table_t     *luab_table_checkgid(lua_State *, int);
luab_table_t     *luab_table_checkint(lua_State *, int);
luab_table_t     *luab_table_checku_short(lua_State *, int);
luab_table_t     *luab_table_checkiovec(lua_State *, int);
luab_table_t     *luab_table_checktimespec(lua_State *, int);

luab_table_t     *luab_table_tolxargp(lua_State *, int, size_t);

luab_table_t     *luab_table_checkldouble(lua_State *, int, size_t);
luab_table_t     *luab_table_checklgid(lua_State *, int, size_t);
luab_table_t     *luab_table_checklint(lua_State *, int, size_t);
luab_table_t     *luab_table_checklu_short(lua_State *, int, size_t);
luab_table_t     *luab_table_checkliovec(lua_State *, int, size_t);
luab_table_t     *luab_table_checkltimespec(lua_State *, int, size_t);
#if 0
luab_table_t     *luab_table_checklx(lua_State *, int, size_t, size_t);
#endif

/*
 * Access functions, [C -> stack].
 */

void     luab_table_pushdouble(lua_State *, int, luab_table_t *, int, int);
void     luab_table_pushgid(lua_State *, int, luab_table_t *, int, int);
void     luab_table_pushint(lua_State *, int, luab_table_t *, int, int);

void     luab_table_pushiovec(lua_State *, int, luab_table_t *, int, int);
void     luab_table_pushtimespec(lua_State *, int, luab_table_t *, int, int);

#endif /* _LUAB_TABLE_H_ */
