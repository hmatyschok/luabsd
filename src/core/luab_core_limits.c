/*
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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

const lua_Integer luab_char_bit = CHAR_BIT;

const lua_Integer luab_schar_max = SCHAR_MAX;
const lua_Integer luab_schar_min = SCHAR_MIN;
const lua_Integer luab_uchar_max = UCHAR_MAX;

const lua_Integer luab_char_max = CHAR_MAX;
const lua_Integer luab_char_min = CHAR_MIN;

const lua_Integer luab_ushrt_max = USHRT_MAX;

const lua_Integer luab_shrt_max = SHRT_MAX;
const lua_Integer luab_shrt_min = SHRT_MIN;

const lua_Integer luab_uint_max = UINT_MAX;
const lua_Integer luab_int_max = INT_MAX;
const lua_Integer luab_int_min = INT_MIN;

const lua_Integer luab_ulong_max = ULONG_MAX;
const lua_Integer luab_long_max = LONG_MAX;
const lua_Integer luab_long_min = LONG_MIN;

#ifdef __LONG_LONG_SUPPORTED
const lua_Integer luab_ullong_max = ULLONG_MAX;
const lua_Integer luab_llong_max = LLONG_MAX;
const lua_Integer luab_llong_min = LLONG_MIN;
#else
const lua_Integer luab_ullong_max = 0;
const lua_Integer luab_llong_max = 0;
const lua_Integer luab_llong_min = 0;
#endif

#if __POSIX_VISIBLE || __XSI_VISIBLE
const lua_Integer luab_ssize_max = SSIZE_MAX;
#else
const lua_Integer luab_ssize_max = 0;
#endif

#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
const lua_Integer luab_size_t_max = SIZE_T_MAX;
const lua_Integer luab_off_max = OFF_MAX;
const lua_Integer luab_off_min = OFF_MIN;
#else
const lua_Integer luab_size_t_max = 0;
const lua_Integer luab_off_max = 0;
const lua_Integer luab_off_min = 0;
#endif

#if __BSD_VISIBLE
const lua_Integer luab_gid_max = GID_MAX;
const lua_Integer luab_uid_max = UID_MAX;
const lua_Integer luab_uquad_max = UQUAD_MAX;
const lua_Integer luab_quad_max = QUAD_MAX;
const lua_Integer luab_quad_min = QUAD_MIN;
#else
const lua_Integer luab_gid_max = 0;
const lua_Integer luab_uid_max = 0;
const lua_Integer luab_uquad_max = 0;
const lua_Integer luab_quad_max = 0;
const lua_Integer luab_quad_min = 0;
#endif

#if __XSI_VISIBLE || __POSIX_VISIBLE >= 200809
const lua_Integer luab_long_bit = LONG_BIT;
const lua_Integer luab_word_bit = WORD_BIT;
#else
const lua_Integer luab_long_bit = 0;
const lua_Integer luab_word_bit = 0;
#endif

#if __POSIX_VISIBLE
const lua_Integer luab_mq_prio_max = MQ_PRIO_MAX;
#else
const lua_Integer luab_mq_prio_max = 0;
#endif

/*
 * XXX
 *  Constraints shall set by sysconf(3), if any.
 */
 
size_t luab_buf_max;
size_t luab_path_max;

size_t luab_comlen_max;
size_t luab_interp_max;
size_t luab_logname_max;
size_t luab_uprc_max;

size_t luab_ncargs;
size_t luab_ngroups;
size_t luab_nofile;
size_t luab_nogroup;

size_t luab_hostname_max;
size_t luab_specname_max;

size_t luab_tty_max;

