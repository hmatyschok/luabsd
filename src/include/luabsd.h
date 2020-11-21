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

#ifndef _LUABSD_H_
#define _LUABSD_H_

#include <sys/param.h>
#include <sys/mbuf.h>   /* XXX */
#include <sys/socket.h>
#include <sys/queue.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "luab_env.h"

/*
 * Definitions for API method table.
 */

typedef union luab_primitive_u {
    char        un_char;
    short       un_short;
    int         un_int;
    long        un_long;
    int8_t      un_int8;
    int16_t     un_int16;
    int32_t     un_int32;
    int64_t     un_int64;
    uint8_t     un_uint8;
    uint16_t    un_uint16;
    uint32_t    un_uint32;
    uint64_t    un_uint64;
    u_int       un_u_int;
    u_long      un_u_long;
    fpos_t      un_fpos;
    gid_t       un_gid;
    off_t       un_off;
    size_t      un_size;
    uid_t       un_uid;
    socklen_t   un_socklen;
    lua_Integer un_intx;
    lua_Number  un_numx;
    lua_CFunction   un_fn;
    const char  *un_cp;
    wchar_t     un_wc;
} luab_primitive_t;

typedef void    (*luab_module_table_fn)(lua_State *, luab_primitive_t *);

typedef struct luab_module_table {
    luab_module_table_fn    mt_init;
    const char              *mt_key;
    luab_primitive_t        mt_val;
} luab_module_table_t;

#define LUAB_TYPE(fn, k, v) \
    { .mt_init = fn, .mt_key = k, v, }
#define LUAB_INT(k, v) \
    LUAB_TYPE(luab_initinteger, k, .mt_val.un_intx = v)
#define LUAB_FUNC(k, v) \
    LUAB_TYPE(luab_initcfunction, k, .mt_val.un_fn = v)
#define LUAB_STR(k, v) \
    LUAB_TYPE(luab_initstring, k, .mt_val.un_cp = v)
#define LUAB_MOD_TBL_SENTINEL \
    LUAB_TYPE(NULL, NULL, .mt_val.un_fn = NULL)

static __inline void
luab_initinteger(lua_State *L, luab_primitive_t *un)
{
    lua_pushinteger(L, un->un_intx);
}

static __inline void
luab_initcfunction(lua_State *L, luab_primitive_t *un)
{
    lua_pushcfunction(L, un->un_fn);
}

static __inline void
luab_initstring(lua_State *L, luab_primitive_t *un)
{
    lua_pushstring(L, un->un_cp);
}

/*
 * Definitions for luab_module{} descriptor.
 */

typedef void *  (*luab_ctor_fn)(lua_State *, void *);
typedef void  (*luab_init_fn)(void *, void *);
typedef void *  (*luab_get_fn)(lua_State *, int);

typedef struct luab_table {
    uint32_t    tbl_cookie;
    void        *tbl_vec;
    size_t      tbl_card;
    size_t      tbl_sz;
} luab_table_t;

typedef luab_table_t *   (*luab_get_tbl_fn)(lua_State *, int);
typedef void     (*luab_set_tbl_fn)(lua_State *, int, luab_table_t *, int, int);

typedef struct luab_module {
    uint32_t        m_cookie;        /*  date -u +'%s' */
    size_t          m_sz;
    const char      *m_name;
    luab_module_table_t *m_vec;
    luab_ctor_fn    m_create;
    luab_init_fn    m_init;
    luab_get_fn     m_get;
    luab_get_tbl_fn m_get_tbl;
    luab_set_tbl_fn m_set_tbl;
} luab_module_t;

typedef void    (*luab_module_fn)(lua_State *, int, luab_module_t *);

/*
 * Selector.
 */

typedef enum luab_type {
#if __BSD_VISIBLE
    LUAB_USHRT_IDX,
    LUAB_UINT_IDX,
#endif /* __BSD_VISIBLE */
    LUAB_CHAR_IDX,                  /* integers */
    LUAB_SHORT_IDX,
    LUAB_INT_IDX,
    LUAB_LONG_IDX,

    /* floating point numbers */
    LUAB_DOUBLE_IDX,
    LUAB_FLOAT_IDX,
                                    /* standard type definitions */
    LUAB_FPOS_IDX,
    LUAB_GID_IDX,
    LUAB_OFF_IDX,
    LUAB_SIZE_IDX,
    LUAB_SOCKLEN_IDX,
    LUAB_SSIZE_IDX,
    LUAB_UID_IDX,
    LUAB_WCHAR_IDX,
                                    /* composite data types */
#if LUAB_DEBUG
    LUAB_LINK_IDX,
#endif /* LUAB_DEBUG */
    LUAB_CLOCKINFO_IDX,
    LUAB_DIV_IDX,
    LUAB_FLOCK_IDX,
    LUAB_INTEGER_IDX,
    LUAB_IF_NAMEINDEX_IDX,
    LUAB_IN_ADDR_IDX,
    LUAB_IN6_ADDR_IDX,
    LUAB_ITIMERVAL_IDX,
    LUAB_LDIV_IDX,
    LUAB_LLDIV_IDX,
    LUAB_LINGER_IDX,
    LUAB_MSGHDR_IDX,
    LUAB_SOCKADDR_IDX,
    LUAB_STAT_IDX,
    LUAB_TIMESPEC_IDX,
    LUAB_TIMEVAL_IDX,
    LUAB_TIMEZONE_IDX,
    LUAB_TM_IDX,
    LUAB_UUID_IDX,
    LUAB_IOVEC_IDX,
    LUAB_PASSWD_IDX,
    LUAB_GROUP_IDX,
    LUAB_REGEX_IDX,
    LUAB_REGMATCH_IDX,
    LUAB_FSID_IDX,
    LUAB_FID_IDX,
    LUAB_STATFS_IDX,
    LUAB_FHANDLE_IDX,
    LUAB_XVFSCONF_IDX,
    LUAB_TTYENT_IDX,
    LUAB_DIR_IDX,
    LUAB_DIRENT_IDX,
    LUAB_FSTAB_IDX,
    LUAB___SBUF_IDX,
    LUAB_SFILE_IDX,
#if __BSD_VISIBLE
    LUAB_DBT_IDX,
    LUAB_DB_IDX,
    LUAB_BINTIME_IDX,
    LUAB_CRYPT_DATA_IDX,
    LUAB_CAP_RBUF_IDX,
    LUAB_ACCEPT_FILTER_ARG_IDX,
    LUAB_SOCKPROTO_IDX,
    LUAB_CMSGCRED_IDX,
    LUAB_SF_HDTR_IDX,
#endif /* __BSD_VISIBLE */
    LUAB_TYPE_SENTINEL
} luab_type_t;

#define luab_idx(name) \
    (LUAB_##name##_IDX)
#define luab_xmv(idx) \
    (luab_typevec[((idx) % (LUAB_TYPE_SENTINEL))])
#define luab_xtype(name) \
    ((luab_xmv(luab_idx(name))).mv_mod)

typedef struct luab_module_vec {
    luab_module_t       *mv_mod;
    luab_module_fn      mv_init;
    luab_type_t         mv_idx;
} luab_module_vec_t;

#define LUAB_MOD_VEC_SENTINEL \
    { .mv_mod = NULL, .mv_init = NULL, .mv_idx = LUAB_TYPE_SENTINEL, }

extern luab_module_vec_t luab_typevec[];

/*
 * Generic service primitives, subset of <core>.
 */

void     luab_core_free(void *, size_t);
void     luab_core_freestr(caddr_t);

void     luab_core_err(int, const char *, int);
void     luab_core_errx(int, const char *, ...);
void     luab_core_warn(const char *, ...);
void     luab_core_argerror(lua_State *, int, void *, size_t, size_t, int);

int  luab_core_checkmaxargs(lua_State *, int);

luab_module_t    *luab_core_checktype(luab_type_t, const char *);

#define luab_core_checkxtype(m, name, fname)                    \
    do {                                                        \
        (m) = luab_core_checktype(luab_idx(name), (fname));   \
    } while (0)

/*
 * Access functions, n-th arg over argv, [stack -> C].
 */

lua_Integer  luab_checkinteger(lua_State *, int, lua_Integer);
lua_Integer  luab_tointeger(lua_State *, int, lua_Integer);
lua_Integer  luab_tolinteger(lua_State *, int, int);

lua_Integer  luab_checklinteger(lua_State *, int, int);

const char   *luab_islstring(lua_State *, int, size_t);
const char   *luab_tolstring(lua_State *, int, size_t);
const char   *luab_checklstring(lua_State *, int, size_t);
const char   *luab_checklstringisnil(lua_State *, int, size_t);

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

#define luab_pushxinteger(L, x) \
    (luab_pusherr((L), (lua_Integer)(x)))
#define luab_pushxnumber(L, x) \
    (luab_pushnumber((L), (lua_Number)(x)))

int  luab_pusherr(lua_State *, lua_Integer);
int  luab_pushnumber(lua_State *, lua_Number);
int  luab_pushnil(lua_State *);

int  luab_pushstring(lua_State *, const char *);
int  luab_pushfstring(lua_State *, const char *, ...);

int  luab_pushldata(lua_State *, void *, size_t);

/*
 * Generic service primitives, interface of <core>.
 */

int  luab_core_create(lua_State *, int, luab_module_t *, luab_module_t *);
int  luab_core_dump(lua_State *, int, luab_module_t *, size_t);
int  luab_core_gc(lua_State *, int, luab_module_t *);
int  luab_core_len(lua_State *, int, luab_module_t *);
int  luab_core_tostring(lua_State *, int, luab_module_t *);
#endif /* _LUABSD_H_ */
