/*
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

#include <uuid.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_UUID_LIB_ID    1593623310
#define LUAB_UUID_LIB_KEY "uuid"

extern luab_module_t luab_uuid_lib;

/*
 * Service primitives.
 *
 *      (-1 [, errno, strerror(errno)]), if (uuid1,uuid2)
 *      (0 [, nil, nil]), if (uuid1,uuid2) and (uuid2,uuid1)
 *      (1, [, errno, strerror(errno)]), if (uuid2,uuid1)
 *
 */

#define LUAB_UUID_STR_LEN   36

/***
 * uuid_compare(3) - compare UUIDs for equality.
 *
 * @function uuid_compare
 *
 * @param uuid1             UUID, instance of (LUA_USERDATA(UUID)).
 * @param uuid2             UUID, instance of (LUA_USERDATA(UUID)).
 * @param status            Result argument, values from
 *
 *                              bsd.uuid.uuid_s_{
 *                                  ok,
 *                                  bad_version,
 *                                  invalid_string_uuid,
 *                                  no_memory
 *                              }
 *
 *                          by (LUA_TUSERDATA(UINT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.uuid.uuid_compare(uuid1, uuid2, status)
 */
static int
luab_uuid_compare(lua_State *L)
{
    luab_module_t *m0, *m1;
    uuid_t *uuid1, *uuid2;
    uint32_t *status;
    int32_t ret;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(UUID, TYPE, __func__);
    m1 = luab_xmod(UINT, TYPE, __func__);

    uuid1 = luab_udata(L, 1, m0, uuid_t *);
    uuid2 = luab_udata(L, 2, m0, uuid_t *);
    status = luab_udata(L, 3, m1, uint32_t *);

    ret = uuid_compare(uuid1, uuid2, status);

    return (luab_pushxinteger(L, ret));
}

/***
 * uuid_equal(3) - compare UUIDs for equality.
 *
 * @function uuid_equal
 *
 * @param uuid1             UUID, instance of (LUA_USERDATA(UUID)).
 * @param uuid2             UUID, instance of (LUA_USERDATA(UUID)).
 * @param status            Result argument, values from
 *
 *                              bsd.uuid.uuid_s_{
 *                                  ok,
 *                                  bad_version,
 *                                  invalid_string_uuid,
 *                                  no_memory
 *                              }
 *
 *                          by (LUA_TUSERDATA(UINT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.uuid.uuid_compare(uuid1, uuid2, status)
 */
static int
luab_uuid_equal(lua_State *L)
{
    luab_module_t *m0, *m1;
    uuid_t *uuid1, *uuid2;
    uint32_t *status;
    int32_t ret;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(UUID, TYPE, __func__);
    m1 = luab_xmod(UINT, TYPE, __func__);

    uuid1 = luab_udata(L, 1, m0, uuid_t *);
    uuid2 = luab_udata(L, 2, m0, uuid_t *);
    status = luab_udata(L, 3, m1, uint32_t *);

    ret = uuid_equal(uuid1, uuid2, status);

    return (luab_pushxinteger(L, ret));
}

/***
 * uuid_from_string(3) - compare UUIDs for equality.
 *
 * @function uuid_from_string
 *
 * @param str               UUID encoded by (LUA_TSTRING).
 * @param uuid              UUID, instance of (LUA_USERDATA(UUID)).
 * @param status            Result argument, values from
 *
 *                              bsd.uuid.uuid_s_{
 *                                  ok,
 *                                  bad_version,
 *                                  invalid_string_uuid,
 *                                  no_memory
 *                              }
 *
 *                          by (LUA_TUSERDATA(UINT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.uuid.uuid_from_string([ status ])
 */
static int
luab_uuid_from_string(lua_State *L)
{
    luab_module_t *m0, *m1;
    const char *str;
    uuid_t *uuid;
    uint32_t *status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(UUID, TYPE, __func__);
    m1 = luab_xmod(UINT, TYPE, __func__);

    str = luab_checklstring(L, 1, LUAB_UUID_STR_LEN, NULL);
    uuid = luab_udata(L, 2, m0, uuid_t *);
    status = luab_udata(L, 3, m1, uint32_t *);

    uuid_from_string(str, uuid, status);

    return (luab_pushxinteger(L, *status));
}

/***
 * uuid_hash(3) - generates a hash value for the specific uuid.
 *
 * @function uuid_hash
 *
 * @param uuid              UUID, instance of (LUA_USERDATA(UUID)).
 * @param status            Result argument, values from
 *
 *                              bsd.uuid.uuid_s_{
 *                                  ok,
 *                                  bad_version,
 *                                  invalid_string_uuid,
 *                                  no_memory
 *                              }
 *
 *                          by (LUA_TUSERDATA(UINT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage hash [, err, msg ] = bsd.uuid.uuid_hash(uuid, status)
 */
static int
luab_uuid_hash(lua_State *L)
{
    luab_module_t *m0, *m1;
    uuid_t *uuid;
    uint32_t *status;
    uint16_t hash;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(UUID, TYPE, __func__);
    m1 = luab_xmod(UINT, TYPE, __func__);

    uuid = luab_udata(L, 1, m0, uuid_t *);
    status = luab_udata(L, 2, m1, uint32_t *);

    hash = uuid_hash(uuid, status);

    return (luab_pushxinteger(L, hash));
}

/***
 * uuid_enc_le(3) - encodes binary representation into an octet stream
 *
 * @function uuid_enc_le
 *
 * @param buf               Storage for an octet stream, (LUA_USERDATA(IOVEC)).
 * @param uuid              UUID, instance of (LUA_USERDATA(UUID)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.uuid.uuid_enc_le(buf, uuid)
 */
static int
luab_uuid_enc_le(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *buf;
    uuid_t *uuid;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(UUID, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    uuid = luab_udata(L, 2, m1, uuid_t *);

    if ((buf != NULL) &&
        ((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (LUAB_UUID_STR_LEN <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        uuid_enc_le(bp, uuid);
        buf->iov.iov_len = LUAB_UUID_STR_LEN;
        status = luab_env_success;

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * uuid_dec_le(3) - decodes an octet stream into its binary representation
 *
 * @function uuid_dec_le
 *
 * @param buf               Storage for an octet stream, (LUA_USERDATA(IOVEC)).
 * @param uuid              UUID, instance of (LUA_USERDATA(UUID)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.uuid.uuid_dec_le(buf, uuid)
 */
static int
luab_uuid_dec_le(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *buf;
    uuid_t *uuid;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(UUID, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    uuid = luab_udata(L, 2, m1, uuid_t *);

    if ((buf != NULL) &&
        ((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (LUAB_UUID_STR_LEN <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        uuid_dec_le(bp, uuid);
        status = luab_env_success;

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * uuid_enc_be(3) - encodes binary representation into an octet stream
 *
 * @function uuid_enc_be
 *
 * @param buf               Storage for an octet stream, (LUA_USERDATA(IOVEC)).
 * @param uuid              UUID, instance of (LUA_USERDATA(UUID)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.uuid.uuid_enc_be(buf, uuid)
 */
static int
luab_uuid_enc_be(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *buf;
    uuid_t *uuid;
    caddr_t bp;
    int status;
                                        /* XXX DRY */
    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(UUID, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    uuid = luab_udata(L, 2, m1, uuid_t *);

    if ((buf != NULL) &&
        ((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (LUAB_UUID_STR_LEN <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        uuid_enc_be(bp, uuid);
        buf->iov.iov_len = LUAB_UUID_STR_LEN;   /* XXX */
        status = luab_env_success;

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * uuid_dec_be(3) - decodes an octet stream into its binary representation
 *
 * @function uuid_dec_be
 *
 * @param buf               Storage for an octet stream, (LUA_USERDATA(IOVEC)).
 * @param uuid              UUID, instance of (LUA_USERDATA(UUID)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.uuid.uuid_dec_be(buf, uuid)
 */
static int
luab_uuid_dec_be(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *buf;
    uuid_t *uuid;
    caddr_t bp;
    int status;
                                        /* XXX DRY */
    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(UUID, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    uuid = luab_udata(L, 2, m1, uuid_t *);

    if ((buf != NULL) &&
        ((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (LUAB_UUID_STR_LEN <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        uuid_dec_be(bp, uuid);
        status = luab_env_success;

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/*
 * Generator functions.
 */

/***
 * uuid(3) - generator function.
 *
 * @function uuid_create
 *
 * @param status            Result argument, values from
 *
 *                              bsd.uuid.uuid_s_{
 *                                  ok,
 *                                  bad_version,
 *                                  invalid_string_uuid,
 *                                  no_memory
 *                              }
 *
 *                          by (LUA_TUSERDATA(UINT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uuid [, err, msg ] = bsd.uuid.uuid_create(status)
 */
static int
luab_uuid_create(lua_State *L)
{
    luab_module_t *m0, *m1;
    uint32_t *status;
    uuid_t uuid;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(UINT, TYPE, __func__);
    m1 = luab_xmod(UUID, TYPE, __func__);

    status = luab_udata(L, 1, m0, uint32_t *);

    uuid_create(&uuid, status);

    return (luab_pushxdata(L, m1, &uuid));
}

/***
 * uuid(3) - generator function.
 *
 * @function uuid_create_nil
 *
 * @param status            Result argument, values from
 *
 *                              bsd.uuid.uuid_s_{
 *                                  ok,
 *                                  bad_version,
 *                                  invalid_string_uuid,
 *                                  no_memory
 *                              }
 *
 *                          by (LUA_TUSERDATA(UINT)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uuid [, err, msg ] = bsd.uuid.uuid_create_nil(status)
 */
static int
luab_uuid_create_nil(lua_State *L)
{
    luab_module_t *m0, *m1;
    uint32_t *status;
    uuid_t uuid;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(UINT, TYPE, __func__);
    m1 = luab_xmod(UUID, TYPE, __func__);

    status = luab_udata(L, 1, m0, uint32_t *);

    uuid_create_nil(&uuid, status);

    return (luab_pushxdata(L, m1, &uuid));
}

/*
 * Interface against <uuid.h>.
 */

static luab_module_table_t luab_uuid_vec[] = {
    LUAB_INT("uuid_s_ok",                     uuid_s_ok),
    LUAB_INT("uuid_s_bad_version",            uuid_s_bad_version),
    LUAB_INT("uuid_s_invalid_string_uuid",    uuid_s_invalid_string_uuid),
    LUAB_INT("uuid_s_no_memory",              uuid_s_no_memory),
    LUAB_FUNC("uuid_compare",                 luab_uuid_compare),
    LUAB_FUNC("uuid_create",                  luab_uuid_create),
    LUAB_FUNC("uuid_create_nil",              luab_uuid_create_nil),
    LUAB_FUNC("uuid_equal",                   luab_uuid_equal),
    LUAB_FUNC("uuid_from_string",             luab_uuid_from_string),
    LUAB_FUNC("uuid_hash",                    luab_uuid_hash),
    LUAB_FUNC("uuid_enc_le",                  luab_uuid_enc_le),
    LUAB_FUNC("uuid_dec_le",                  luab_uuid_dec_le),
    LUAB_FUNC("uuid_enc_be",                  luab_uuid_enc_be),
    LUAB_FUNC("uuid_dec_be",                  luab_uuid_dec_be),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_uuid_lib = {
    .m_id       = LUAB_UUID_LIB_ID,
    .m_name     = LUAB_UUID_LIB_KEY,
    .m_vec      = luab_uuid_vec,
};
