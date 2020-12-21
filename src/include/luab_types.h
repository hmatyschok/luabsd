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

#ifndef _LUAB_TYPES_H_
#define _LUAB_TYPES_H_

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

typedef uint32_t luab_id_t;

typedef void *  (*luab_create_fn)(lua_State *, void *);
typedef void  (*luab_init_fn)(void *, void *);
typedef void *  (*luab_get_fn)(lua_State *, int);

typedef struct luab_table {
    luab_id_t   tbl_id;
    void        *tbl_vec;
    size_t      tbl_card;
    size_t      tbl_sz;
} luab_table_t;

typedef luab_table_t *   (*luab_alloc_tbl_fn)(void *, size_t);
typedef luab_table_t *   (*luab_get_tbl_fn)(lua_State *, int);
typedef void     (*luab_set_tbl_fn)(lua_State *, int, luab_table_t *, int, int);

typedef struct luab_module {
    luab_id_t           m_id;        /*  date -u +'%s' */
    size_t              m_len;
    size_t              m_sz;
    const char          *m_name;
    luab_module_table_t *m_vec;
    luab_create_fn      m_create;
    luab_init_fn        m_init;
    luab_get_fn         m_get;
    luab_get_tbl_fn     m_get_tbl;
    luab_set_tbl_fn     m_set_tbl;
    luab_alloc_tbl_fn   m_alloc_tbl;
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
    LUAB_CHAR_IDX,
    LUAB_SHORT_IDX,
    LUAB_INT_IDX,
    LUAB_LONG_IDX,

    /* floating point number types */
    LUAB_DOUBLE_IDX,
    LUAB_FLOAT_IDX,

    /* POSIX sized integrals */
    LUAB_INT8_IDX,
    LUAB_INT16_IDX,
    LUAB_INT32_IDX,
    LUAB_INT64_IDX,

    /* standard types */
    LUAB_FPOS_IDX,
    LUAB_GID_IDX,
    LUAB_OFF_IDX,
    LUAB_SIZE_IDX,
    LUAB_SOCKLEN_IDX,
    LUAB_SSIZE_IDX,
    LUAB_UID_IDX,
    LUAB_WCHAR_IDX,
    LUAB_TIME_IDX,
    LUAB_CLOCK_IDX,

    LUAB_LOCALE_IDX,

    /* <sys/stdint.h> */
    LUAB_INTPTR_IDX,
    LUAB_UINTPTR_IDX,
    LUAB_INTMAX_IDX,

    /* composite data types */
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
    LUAB_JAIL_IDX,
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
#define luab_xcookie(name, type) \
    ((LUAB_##name##_##type##_ID))
#define luab_xmv(idx) \
    (luab_typevec[((idx) % (LUAB_TYPE_SENTINEL))])

typedef struct luab_module_vec {
    luab_module_t       *mv_mod;
    luab_module_fn      mv_init;
    luab_type_t         mv_idx;
} luab_module_vec_t;

#define LUAB_MOD_VEC_SENTINEL \
    { .mv_mod = NULL, .mv_init = NULL, .mv_idx = LUAB_TYPE_SENTINEL, }

extern luab_module_vec_t luab_typevec[];

/*
 * Set of atomic data types.
 */

#define LUAB_CHAR_TYPE_ID                   1605964271
#define LUAB_CHAR_TYPE                      "CHAR*"

#define LUAB_DOUBLE_TYPE_ID                 1605894356
#define LUAB_DOUBLE_TYPE                    "DOUBLE*"

#define LUAB_FLOAT_TYPE_ID                  1605908147
#define LUAB_FLOAT_TYPE                     "FLOAT*"

#define LUAB_FPOS_TYPE_ID                   1605960373
#define LUAB_FPOS_TYPE                      "FPOS*"

#define LUAB_GID_TYPE_ID                    1605908910
#define LUAB_GID_TYPE                       "GID*"

#define LUAB_INT16_TYPE_ID                  1606002046
#define LUAB_INT16_TYPE                     "INT16*"

#define LUAB_INT32_TYPE_ID                  1606004102
#define LUAB_INT32_TYPE                     "INT32*"

#define LUAB_INT64_TYPE_ID                  1606004507
#define LUAB_INT64_TYPE                     "INT64*"

#define LUAB_INT8_TYPE_ID                   1606001349
#define LUAB_INT8_TYPE                      "INT8*"

#define LUAB_INT_TYPE_ID                    1605877856
#define LUAB_INT_TYPE                       "INT*"

#define LUAB_LONG_TYPE_ID                   1605965914
#define LUAB_LONG_TYPE                      "LONG*"

#define LUAB_OFF_TYPE_ID                    1605914553
#define LUAB_OFF_TYPE                       "OFF*"

#define LUAB_SHORT_TYPE_ID                  1605997500
#define LUAB_SHORT_TYPE                     "SHORT*"

#define LUAB_SIZE_TYPE_ID                   1605967268
#define LUAB_SIZE_TYPE                      "SIZE*"

#define LUAB_SOCKLEN_TYPE_ID                1605960373
#define LUAB_SOCKLEN_TYPE                   "SOCKLEN*"

#define LUAB_SSIZE_TYPE_ID                  1605991182
#define LUAB_SSIZE_TYPE                     "SSIZE*"

#define LUAB_UID_TYPE_ID                    1605913920
#define LUAB_UID_TYPE                       "UID*"

#define LUAB_UINT_TYPE_ID                   1605965347
#define LUAB_UINT_TYPE                      "UINT*"

#define LUAB_USHRT_TYPE_ID                  1605911568
#define LUAB_USHRT_TYPE                     "USHRT*"

#define LUAB_WCHAR_TYPE_ID                  1605963183
#define LUAB_WCHAR_TYPE                     "WCHAR*"

#define LUAB_TIME_TYPE_ID                   1607255289
#define LUAB_TIME_TYPE                      "TIME*"

#define LUAB_CLOCK_TYPE_ID                  1607649423
#define LUAB_CLOCK_TYPE                     "CLOCK*"

#define LUAB_LOCALE_TYPE_ID                 1608306297
#define LUAB_LOCALE_TYPE                    "LOCALE*"

#define LUAB_INTPTR_TYPE_ID                 1608485473
#define LUAB_INTPTR_TYPE                    "INTPTR*"

#define LUAB_UINTPTR_TYPE_ID                1608557891
#define LUAB_UINTPTR_TYPE                   "UINTPTR*"

#define LUAB_INTMAX_TYPE_ID                 1608559100
#define LUAB_INTMAX_TYPE                    "INTMAX*"

/*
 * Set of composite data types.
 */

#define LUAB___SBUF_TYPE_ID                 1605302940
#define LUAB___SBUF_TYPE                    "__SBUF*"

#define LUAB_ACCEPT_FILTER_ARG_TYPE_ID      1601330449
#define LUAB_ACCEPT_FILTER_ARG_TYPE         "ACCEPT_FILTER_ARG*"

#define LUAB_BINTIME_TYPE_ID                1594161740
#define LUAB_BINTIME_TYPE                   "BINTIME*"

#define LUAB_CAP_RBUF_TYPE_ID               1601143331
#define LUAB_CAP_RBUF_TYPE                  "CAP_RBUF*"

#define LUAB_CLOCKINFO_TYPE_ID              1594164272
#define LUAB_CLOCKINFO_TYPE                 "CLOCKINFO*"

#define LUAB_CMSGCRED_TYPE_ID               1601557495
#define LUAB_CMSGCRED_TYPE                  "CMSGCRED*"

#define LUAB_CRYPT_DATA_TYPE_ID             1595491033
#define LUAB_CRYPT_DATA_TYPE                "CRYPT_DATA*"

#define LUAB_DB_TYPE_ID                     1596201370
#define LUAB_DB_TYPE                        "DB*"

#define LUAB_DBT_TYPE_ID                    1596025036
#define LUAB_DBT_TYPE                       "DBT*"

#define LUAB_DIR_TYPE_ID                    1604794619
#define LUAB_DIR_TYPE                       "DIR*"

#define LUAB_DIRENT_TYPE_ID                 1604795865
#define LUAB_DIRENT_TYPE                    "DIRENT*"

#define LUAB_DIV_TYPE_ID                    1600388163
#define LUAB_DIV_TYPE                       "DIV*"

#define LUAB_FHANDLE_TYPE_ID                1604445207
#define LUAB_FHANDLE_TYPE                   "FHANDLE*"

#define LUAB_FID_TYPE_ID                    1604423994
#define LUAB_FID_TYPE                       "FID*"

#define LUAB_FLOCK_TYPE_ID                  1593623399
#define LUAB_FLOCK_TYPE                     "FLOCK*"

#define LUAB_FSID_TYPE_ID                   1604426384
#define LUAB_FSID_TYPE                      "FSID*"

#define LUAB_FSTAB_TYPE_ID                  1604964419
#define LUAB_FSTAB_TYPE                     "FSTAB*"

#define LUAB_GROUP_TYPE_ID                  1604324396
#define LUAB_GROUP_TYPE                     "GROUP*"

#define LUAB_IF_NAMEINDEX_TYPE_ID           1596840702
#define LUAB_IF_NAMEINDEX_TYPE              "IF_NAMEINDEX*"

#define LUAB_IN6_ADDR_TYPE_ID               1595890830
#define LUAB_IN6_ADDR_TYPE                  "IN6_ADDR*"

#define LUAB_IN_ADDR_TYPE_ID                1595799233
#define LUAB_IN_ADDR_TYPE                   "IN_ADDR*"

#define LUAB_INTEGER_TYPE_ID                1595975665
#define LUAB_INTEGER_TYPE                   "INTEGER*"

#define LUAB_IOVEC_TYPE_ID                  1594559731
#define LUAB_IOVEC_TYPE                     "IOVEC*"

#define LUAB_ITIMERVAL_TYPE_ID              1594110231
#define LUAB_ITIMERVAL_TYPE                 "ITIMERVAL*"

#define LUAB_JAIL_TYPE_ID                   1606011567
#define LUAB_JAIL_TYPE                      "JAIL*"

#define LUAB_LDIV_TYPE_ID                   1600394297
#define LUAB_LDIV_TYPE                      "LDIV*"

#define LUAB_LINGER_TYPE_ID                 1597012436
#define LUAB_LINGER_TYPE                    "LINGER*"

#define LUAB_LINK_TYPE_ID                   1601843279
#define LUAB_LINK_TYPE                      "LINK*"

#define LUAB_LLDIV_TYPE_ID                  1600628549
#define LUAB_LLDIV_TYPE                     "LLDIV*"

#define LUAB_MSGHDR_TYPE_ID                 1597320239
#define LUAB_MSGHDR_TYPE                    "MSGHDR*"

#define LUAB_PASSWD_TYPE_ID                 1604262109
#define LUAB_PASSWD_TYPE                    "PASSWD*"

#define LUAB_REGEX_TYPE_ID                  1604367435
#define LUAB_REGEX_TYPE                     "REGEX*"

#define LUAB_REGMATCH_TYPE_ID               1604371586
#define LUAB_REGMATCH_TYPE                  "REGMATCH*"

#define LUAB_SF_HDTR_TYPE_ID                1602287699
#define LUAB_SF_HDTR_TYPE                   "SF_HDTR*"

#define LUAB_SFILE_TYPE_ID                  1605120444
#define LUAB_SFILE_TYPE                     "SFILE*"

#define LUAB_SOCKADDR_TYPE_ID               1595755513
#define LUAB_SOCKADDR_TYPE                  "SOCKADDR*"

#define LUAB_SOCKPROTO_TYPE_ID              1601383780
#define LUAB_SOCKPROTO_TYPE                 "SOCKPROTO*"

#define LUAB_STAT_TYPE_ID                   1594028586
#define LUAB_STAT_TYPE                      "STAT*"

#define LUAB_STATFS_TYPE_ID                 1604429614
#define LUAB_STATFS_TYPE                    "STATFS*"

#define LUAB_TIMESPEC_TYPE_ID               1594034844
#define LUAB_TIMESPEC_TYPE                  "TIMESPEC*"

#define LUAB_TIMEVAL_TYPE_ID                1599788349
#define LUAB_TIMEVAL_TYPE                   "TIMEVAL*"

#define LUAB_TIMEZONE_TYPE_ID               1594159943
#define LUAB_TIMEZONE_TYPE                  "TIMEZONE*"

#define LUAB_TM_TYPE_ID                     1594168426
#define LUAB_TM_TYPE                        "TM*"

#define LUAB_TTYENT_TYPE_ID                 1604759728
#define LUAB_TTYENT_TYPE                    "TTYENT*"

#define LUAB_UUID_TYPE_ID                   1599304529
#define LUAB_UUID_TYPE                      "UUID*"

#define LUAB_XVFSCONF_TYPE_ID               1604736441
#define LUAB_XVFSCONF_TYPE                  "XVFSCONF*"

#endif /* _LUAB_TYPES_H_ */
