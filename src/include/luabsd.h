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

int  luab_core_checkmaxargs(lua_State *, int);

#include "luab_types.h"

int  luab_core_create(lua_State *, int, luab_module_t *, luab_module_t *);
int  luab_core_dump(lua_State *, int, luab_module_t *, size_t);
int  luab_core_gc(lua_State *, int, luab_module_t *);
int  luab_core_len(lua_State *, int, luab_module_t *);
int  luab_core_tostring(lua_State *, int, luab_module_t *);

#endif /* _LUABSD_H_ */
