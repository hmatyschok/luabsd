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

#ifndef _LUAB_UDATA_H_
#define _LUAB_UDATA_H_

/*
 * Interface Control Information (ICI).
 */

typedef struct luab_udata {
    LIST_ENTRY(luab_udata)  ud_next;
    LIST_HEAD(, luab_udata) ud_list;
    luab_module_t           *ud_m;
    time_t                  ud_ts;
    void                    **ud_x;
    void                    *ud_xhd;
} luab_udata_t;

static __inline size_t
luab_xlen(luab_module_t *m)
{
    return ((m->m_len - sizeof(luab_udata_t)));
}

/*
 * Protocol Control Information (PCI).
 */

typedef struct luab_xarg {
    luab_type_t xarg_idx;
    size_t      xarg_len;
} luab_xarg_t;

/*
 * Generic service primitives.
 */

void     *luab_newudata(lua_State *, luab_module_t *, void *);
void     luab_udata_init(luab_module_t *, luab_udata_t *, void *);
void     luab_udata_remove(luab_udata_t *);
luab_udata_t     *luab_udata_find(luab_udata_t *, void **);
void     *luab_udata_insert(luab_udata_t *, luab_udata_t *, void **);

/*
 * Access functions, [stack -> C].
 */

#define luab_dptox(dp) ((void *)&(dp))
#define luab_isdata(L, narg, m, t) \
    ((t)luaL_testudata((L), (narg), ((m)->m_name)))

void     *luab_isudata(lua_State *, int, luab_module_t *);

#define luab_todata(L, narg, m, t) \
    ((t)luab_checkudata((L), (narg), (m)))
#define luab_toldata(L, narg, m, t, len) \
    ((t)luab_checkludata((L), (narg), (m), (len)))
#define luab_xdata(L, narg, m) \
    ((*(m)->m_get)((L), (narg)))
#define luab_udata(L, narg, m, t) \
    ((t)luab_xdata((L), (narg), (m)))
#define luab_udataisnil(L, narg, m, t) \
    ((t)(luab_checkudataisnil((L), (narg), (m))))

void     *luab_checkudata(lua_State *, int, luab_module_t *);
void     *luab_toudata(lua_State *, int, luab_module_t *);
void     *luab_checkudataisnil(lua_State *, int, luab_module_t *);
void     *luab_checkludata(lua_State *, int, luab_module_t *, size_t);

void     *luab_checkxdata(lua_State *, int, luab_module_t *, luab_udata_t **);
void     *luab_checkxdataisnil(lua_State *, int, luab_module_t *);

void     *luab_toxudata(lua_State *, int, luab_xarg_t *);
void     *luab_toxdata(lua_State *, int, luab_xarg_t *);

void     *luab_udata_xlink(lua_State *, int, luab_udata_t *, void **);
void     *luab_udata_checkxlink(lua_State *L, int narg, luab_module_t *m,
    luab_udata_t *udx, void **x);

/*
 * Access functions, [C -> stack].
 */

int  luab_pushxdata(lua_State *, luab_module_t *, void *);
void     luab_rawsetxdata(lua_State *, int, luab_module_t *, lua_Integer, void *);
void     luab_setxdata(lua_State *, int, luab_module_t *, const char *, void *);

#include "luab_buf.h"
#include "luab_iovec.h"
#include "luab_db.h"

#endif /* _LUAB_UDATA_H_ */
