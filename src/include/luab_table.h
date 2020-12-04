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

typedef void    (*luab_xtable_fn)(lua_State *, int, void *);

typedef struct luab_xtable_param {
    luab_xtable_fn      xtp_fill;       /* initializer */
    void                *xtp_arg;       /* data set */
    int                 xtp_new;
    const char          *xtp_k;         /* field */
} luab_xtable_param_t;

/*
 * Service primitives.
 */

size_t   luab_checktable(lua_State *, int);
size_t   luab_checktableisnil(lua_State *, int);

size_t   luab_checkltable(lua_State *, int, size_t);
size_t   luab_checkltableisnil(lua_State *, int, size_t);

void     luab_table_init(lua_State *, int);
void     luab_table_free(luab_table_t *);

/*
 * Error handler.
 */

void     luab_table_argerror(lua_State *, int, luab_table_t *, int);

/*
 * Generator functions.
 */

luab_table_t     *luab_table_allocnil(size_t, size_t, luab_id_t);
luab_table_t     *luab_table_alloc(size_t, size_t, luab_id_t);

luab_table_t     *luab_table_create(luab_module_t *, void *, size_t);

/*
 * Access functions, [stack -> C]
 */

luab_table_t     *luab_table_newvector(lua_State *, int, luab_module_t *);
luab_table_t     *luab_table_newvectornil(lua_State *, int, luab_module_t *);

luab_table_t     *luab_table_newlvector(lua_State *, int, luab_module_t *, size_t);
luab_table_t     *luab_table_newlvectornil(lua_State *, int, luab_module_t *, size_t);

luab_table_t     *luab_table_checkargv(lua_State *, int);
luab_table_t     *luab_table_toxargp(lua_State *, int);
luab_table_t     *luab_table_tolxargp(lua_State *, int, size_t);

luab_table_t     *luab_table_checkxdata(lua_State *, int, luab_module_t *);
luab_table_t     *luab_table_checklxdata(lua_State *, int, luab_module_t *, size_t);

/*
 * Access functions, [C -> stack].
 */

int  luab_table_pusherr(lua_State *, int, int);
void     luab_table_pushxdata(lua_State *, int, luab_module_t *,
    luab_table_t *, int, int);
int  luab_table_pushxtable(lua_State *, int, luab_xtable_param_t *);
int  luab_table_pushxvector(lua_State *, int, luab_module_t *, void *,
    size_t, int, int);
int
luab_table_setxvector(lua_State *L, int narg, luab_module_t *m,
    const char *k, void *vec, size_t card, int new, int clr);
#endif /* _LUAB_TABLE_H_ */
