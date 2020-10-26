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

#include <err.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#if notyet
#if __BSD_VISIBLE
extern luab_module_t sf_hdtr_type;

/*
 * Interface against
 *
 *  struct sf_hdtr {
 *      struct iovec *headers;
 *      int hdr_cnt;
 *      struct iovec *trailers;
 *      int trl_cnt;
 *  };
 *
 */

enum sf_hdtr_tbl {
    SF_HDTR_HDR,
    SF_HDTR_TRL,
    SF_HDTR_MAX
};

typedef struct luab_sf_hdtr {
    luab_udata_t    ud_softc;
    luab_table_t    *ud_vec[SF_HDTR_MAX];
} luab_sf_hdtr_t;

#define luab_new_sf_hdtr(L, arg) \
    ((luab_sf_hdtr_t *)luab_newudata(L, &sf_hdtr_type, (arg)))
#define luab_to_sf_hdtr(L, narg) \
    ((luab_table_t *)luab_toudata((L), (narg), &sf_hdtr_type))

#define LUAB_SF_HDTR_TYPE_ID    1602287699
#define LUAB_SF_HDTR_TYPE    "SF_HDTR*"

/*
 * Subr.
 */

static int
sf_hdtr_cnt(luab_table_t *vec, enum sf_hdtr_tbl idx)
{
    luab_table_t *tbl;

    if ((tbl = (vec + idx)) )
}

static struct iovec *
sf_hdtr_checkiovec(lua_State *L, int narg, struct iovec *vec, size_t *n)
{
    struct iovec *iov;
    size_t card;

    if (n == NULL)
        luab_core_err(EX_NOINPUT, __func__, EINVAL);

    luab_table_iovec_free(vec);

    iov = luab_table_checkiovec(L, narg, &card);
    *n = card + 1;

    return (iov);
}

static int
sf_hdtr_pushiovec(lua_State *L, int narg, struct iovec *vec, const char *k)
{
    struct iovec *iov;
    int up_call, status;

    luab_core_warn("%s", __func__);

    if ((iov = vec) != NULL) {
        luab_table_pushiovec(L, narg, iov, 1, 0);

        /* set field k or push on top of Lua stack */
        if (k != NULL)
            lua_setfield(L, narg, k);
        else {
            if (narg < 0)
                lua_pushvalue(L, narg + 1);
            else
                lua_pushvalue(L, narg);
        }
        up_call = 0;
    } else
        up_call = ERANGE;

    if (up_call != 0) {
        errno = up_call;
        status = luab_pushnil(L);
    } else
        status = 1;

    return (status);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SF_HDTR)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              headers     = (LUA_TTABLE) -> (LUA_TUSERDATA(IOVEC),
 *              hdr_cnt     = (LUA_TNUMBER),
 *              trailers    = (LUA_TTABLE) -> (LUA_TUSERDATA(IOVEC)
 *              trl_cnt     = (LUA_TNUMBER),
 *          }
 *
 * @usage t = sf_hdtr:get()
 */
static int
SF_HDTR_get(lua_State *L)
{
    luab_table_t *tbl;

    (void)luab_core_checkmaxargs(L, 1);

    tbl = luab_udata(L, 1, &sf_hdtr_type, luab_table_t *);

    lua_newtable(L);

    luab_setinteger(L, -2, "hdr_cnt", tbl]->hdr_cnt);
    luab_setinteger(L, -2, "trl_cnt", hdtr->trl_cnt);

    if (hdtr->headers != NULL)
        (void)sf_hdtr_pushiovec(L, -2, hdtr->headers, "headers");

    if (hdtr->headers != NULL)
        (void)sf_hdtr_pushiovec(L, -2, hdtr->trailers, "trailers");

    lua_pushvalue(L, -1);

    return (1);
}

/*
 * Access functions, immutables.
 */

/***
 * Get number header iovec{}s.
 *
 * @function hdr_cnt
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage card [, err, msg ] = sf_hdtr:hdr_cnt()
 */
static int
SF_HDTR_hdr_cnt(lua_State *L)
{
    luab_table_t *tbl;
    int data;

    (void)luab_core_checkmaxargs(L, 1);

    tbl = luab_udata(L, 1, &sf_hdtr_type, luab_table_t *);
    data = hdtr->hdr_cnt;

    return (luab_pusherr(L, data));
}

/***
 * Get number of trailer iovec{}s.
 *
 * @function trl_cnt
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage card [, err, msg ] = sf_hdtr:trl_cnt()
 */
static int
SF_HDTR_trl_cnt(lua_State *L)
{
    luab_table_t *tbl;
    int data;

    (void)luab_core_checkmaxargs(L, 1);

    tbl = luab_udata(L, 1, &sf_hdtr_type, luab_table_t *);
    data = hdtr->trl_cnt;

    return (luab_pusherr(L, data));
}

/***
 * Set headers by array of iovec{}s
 *
 * @function set_headers
 *
 * @param data              Instance of (LUA_TTABLE).
 *
 *                              iov{0,N} = (LUA_TUSERDATA(IOVEC))
 *
 *                              data = {
 *                                  iov0, iov1, ... , iovN
 *                              }
 *
 *                          If argument specifies an empty (LUA_TTABLE)
 *                          or thw (LUA_TTABLE) is not empty, then by
 *                          array bound data region will be released.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = sf_hdtr:set_headers(data)
 */
static int
SF_HDTR_set_headers(lua_State *L)
{
    luab_table_t *tbl;
    struct iovec *vec;
    size_t card;

    (void)luab_core_checkmaxargs(L, 2);

    tbl = luab_udata(L, 1, &sf_hdtr_type, luab_table_t *);
    vec = sf_hdtr_checkiovec(L, 2, hdtr->headers, &card);

    hdtr->headers = vec;
    hdtr->hdr_cnt = (card & INT_MAX);

    return (luab_pusherr(L, card));
}

/***
 * Get headers by (LUA_TTABLE).
 *
 * @function get_headers
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = sf_hdtr:get_headers()
 */
static int
SF_HDTR_get_headers(lua_State *L)
{
    luab_table_t *tbl;
    struct iovec *vec;

    (void)luab_core_checkmaxargs(L, 1);

    tbl = luab_udata(L, 1, &sf_hdtr_type, luab_table_t *);
    vec = hdtr->headers;

    return (sf_hdtr_pushiovec(L, -2, vec, NULL));
}

/***
 * Set trailers by array of iovec{}s
 *
 * @function set_trailers
 *
 * @param data              Instance of (LUA_TTABLE).
 *
 *                              iov{0,N} = (LUA_TUSERDATA(IOVEC))
 *
 *                              data = {
 *                                  iov0, iov1, ... , iovN
 *                              }
 *
 *                          If argument specifies an empty (LUA_TTABLE)
 *                          or thw (LUA_TTABLE) is not empty, then by
 *                          array bound data region will be released.
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = sf_hdtr:set_trailers(data)
 */
static int
SF_HDTR_set_trailers(lua_State *L)
{
    luab_table_t *tbl;
    struct iovec *vec;
    size_t card;

    (void)luab_core_checkmaxargs(L, 2);

    tbl = luab_udata(L, 1, &sf_hdtr_type, luab_table_t *);
    vec = sf_hdtr_checkiovec(L, 2, hdtr->trailers, &card);

    hdtr->trailers = vec;
    hdtr->trl_cnt = (card & INT_MAX);

    return (luab_pusherr(L, card));
}

/***
 * Get trailers by (LUA_TTABLE).
 *
 * @function get_trailers
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = sf_hdtr:get_trailers()
 */
static int
SF_HDTR_get_trailers(lua_State *L)
{
    luab_table_t *tbl;
    struct iovec *vec;

    (void)luab_core_checkmaxargs(L, 1);

    tbl = luab_udata(L, 1, &sf_hdtr_type, luab_table_t *);
    vec = hdtr->trailers;

    return (sf_hdtr_pushiovec(L, -2, vec, NULL));
}

/*
 * Metamethods
 */

static int
SF_HDTR_gc(lua_State *L)
{
    return (luab_core_gc(L, 1, &sf_hdtr_type));
}

static int
SF_HDTR_len(lua_State *L)
{
    return (luab_core_len(L, 2, &sf_hdtr_type));
}

static int
SF_HDTR_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &sf_hdtr_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t sf_hdtr_methods[] = {
    LUAB_FUNC("hdr_cnt",        SF_HDTR_hdr_cnt),
    LUAB_FUNC("trl_cnt",        SF_HDTR_trl_cnt),
    LUAB_FUNC("set_headers",    SF_HDTR_set_headers),
    LUAB_FUNC("set_trailers",   SF_HDTR_set_trailers),
    LUAB_FUNC("get",            SF_HDTR_get),
    LUAB_FUNC("get_headers",    SF_HDTR_get_headers),
    LUAB_FUNC("get_trailers",   SF_HDTR_get_trailers),
    LUAB_FUNC("__gc",           SF_HDTR_gc),
    LUAB_FUNC("__len",          SF_HDTR_len),
    LUAB_FUNC("__tostring",     SF_HDTR_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
sf_hdtr_create(lua_State *L, void *arg)
{
    return (luab_new_sf_hdtr(L, arg));
}

static void
sf_hdtr_init(void *ud, void *arg)
{
    luab_udata_init(&sf_hdtr_type, ud, arg);
}

static void *
sf_hdtr_udata(lua_State *L, int narg)
{
    return (luab_to_sf_hdtr(L, narg));
}

luab_module_t sf_hdtr_type = {
    .m_cookie   = LUAB_SF_HDTR_TYPE_ID,
    .m_name     = LUAB_SF_HDTR_TYPE,
    .m_vec      = sf_hdtr_methods,
    .m_create   = sf_hdtr_create,
    .m_init     = sf_hdtr_init,
    .m_get      = sf_hdtr_udata,
    .m_sz       = sizeof(luab_sf_hdtr_t),
};
#endif /* __BSD_VISBLE */
#endif /* notyet */
