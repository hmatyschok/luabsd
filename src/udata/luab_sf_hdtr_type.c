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

enum sf_hdtr_cache {
    SF_HDTR_CH_HDR,
    SF_HDTR_CH_TRL,
    SF_HDTR_CH_MAX
};

typedef struct luab_sf_hdtr {
    luab_udata_t    ud_softc;
    struct sf_hdtr  ud_hdtr;
    luab_table_t    *ud_cache[SF_HDTR_CH_MAX];
} luab_sf_hdtr_t;

#define luab_new_sf_hdtr(L, arg) \
    ((luab_sf_hdtr_t *)luab_newudata(L, &sf_hdtr_type, (arg)))
#define luab_to_sf_hdtr(L, narg) \
    (luab_todata((L), (narg), &sf_hdtr_type, luab_sf_hdtr_t *))

#define LUAB_SF_HDTR_TYPE_ID    1602287699
#define LUAB_SF_HDTR_TYPE    "SF_HDTR*"

/*
 * Subr.
 */

static int
sf_hdtr_cnt(luab_sf_hdtr_t *ud, enum sf_hdtr_cache idx)
{
    luab_table_t *tbl;
    int card = 0;

    if (ud != NULL) {

        if ((tbl = (ud->ud_cache[idx % SF_HDTR_CH_MAX])) != NULL)
            card = ((tbl->tbl_card - 1) & INT_MAX);
        else
            errno = ENOENT;
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);

    return (card);
}

static luab_table_t *
sf_hdtr_iovec(luab_sf_hdtr_t *ud, enum sf_hdtr_cache n, luab_table_t *tbl)
{
    struct sf_hdtr *hdtr;
    enum sf_hdtr_cache idx;
    luab_table_t *cur;

    if (ud != NULL) {
        hdtr = &(ud->ud_hdtr);
        idx = (n % SF_HDTR_CH_MAX);

        if ((cur = ud->ud_cache[idx]) != NULL) {

            if (tbl != NULL) {
                luab_table_free(cur);
add_tbl:
                ud->ud_cache[idx] = tbl;
                cur = ud->ud_cache[idx];
            }

            if (idx == SF_HDTR_CH_HDR) {
                hdtr->headers = (struct iovec *)(cur->tbl_vec);
                hdtr->hdr_cnt = sf_hdtr_cnt(ud, idx);
            } else {
                hdtr->trailers = (struct iovec *)(cur->tbl_vec);
                hdtr->trl_cnt = sf_hdtr_cnt(ud, idx);
            }
            return (cur);
        }

        if (tbl != NULL)
            goto add_tbl;

        if (idx == SF_HDTR_CH_HDR) {
            hdtr->headers = NULL;
            hdtr->hdr_cnt = 0;
        } else {
            hdtr->trailers = NULL;
            hdtr->trl_cnt = 0;
        }
        errno = ENOENT;
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);

    return (NULL);
}

static int
sf_hdtr_checkiovec(lua_State *L, int narg, luab_sf_hdtr_t *ud,
    enum sf_hdtr_cache idx)
{
    luab_table_t *cur;
    int card = 0;

    if (ud != NULL) {

        if ((cur = luab_table_checkiovec(L, narg)) != NULL)
            (void)sf_hdtr_iovec(ud, idx, cur);

        card = sf_hdtr_cnt(ud, idx);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);

    return (card);
}

static int
sf_hdtr_pushiovec(lua_State *L, int narg, const char *k, luab_sf_hdtr_t *ud,
    enum sf_hdtr_cache idx)
{
    luab_table_t *cur;
    int up_call, status;

    if ((cur = sf_hdtr_iovec(ud, idx, NULL)) != NULL) {
        luab_table_pushiovec(L, narg, cur, 1, 0);

        /* set field k and/or push on top of Lua stack */
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
        up_call = ENOENT;

    if ((errno = up_call) != 0)
        status = luab_pushnil(L);
    else
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
    luab_sf_hdtr_t *ud;
    struct sf_hdtr *hdtr;

    (void)luab_core_checkmaxargs(L, 1);

    ud = luab_to_sf_hdtr(L, 1);
    hdtr = &(ud->ud_hdtr);

    lua_newtable(L);

    luab_setinteger(L, -2, "hdr_cnt", hdtr->hdr_cnt);
    luab_setinteger(L, -2, "trl_cnt", hdtr->trl_cnt);

    if (hdtr->headers != NULL)
        (void)sf_hdtr_pushiovec(L, -2, "headers", ud, SF_HDTR_CH_HDR);

    if (hdtr->trailers != NULL)
        (void)sf_hdtr_pushiovec(L, -2, "trailers", ud, SF_HDTR_CH_TRL);

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
    struct sf_hdtr *hdtr;
    int card;

    (void)luab_core_checkmaxargs(L, 1);

    hdtr = luab_udata(L, 1, &sf_hdtr_type, struct sf_hdtr *);
    card = hdtr->hdr_cnt;

    return (luab_pusherr(L, card));
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
    struct sf_hdtr *hdtr;
    int card;

    (void)luab_core_checkmaxargs(L, 1);

    hdtr = luab_udata(L, 1, &sf_hdtr_type, struct sf_hdtr *);
    card = hdtr->trl_cnt;

    return (luab_pusherr(L, card));
}

/***
 * Set headers by an array of iovec{}s
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
 * @usage card [, err, msg ] = sf_hdtr:set_headers(data)
 */
static int
SF_HDTR_set_headers(lua_State *L)
{
    luab_sf_hdtr_t *ud;
    int card;

    (void)luab_core_checkmaxargs(L, 2);

    ud = luab_to_sf_hdtr(L, 1);
    card = sf_hdtr_checkiovec(L, 2, ud, SF_HDTR_CH_HDR);
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
    luab_sf_hdtr_t *ud;

    (void)luab_core_checkmaxargs(L, 1);

    ud = luab_to_sf_hdtr(L, 1);
    return (sf_hdtr_pushiovec(L, -2, NULL, ud, SF_HDTR_CH_HDR));
}

/***
 * Set trailers by an array of iovec{}s
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
    luab_sf_hdtr_t *ud;
    int card;

    (void)luab_core_checkmaxargs(L, 2);

    ud = luab_to_sf_hdtr(L, 1);
    card = sf_hdtr_checkiovec(L, 2, ud, SF_HDTR_CH_TRL);
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
    luab_sf_hdtr_t *ud;

    (void)luab_core_checkmaxargs(L, 1);

    ud = luab_to_sf_hdtr(L, 1);
    return (sf_hdtr_pushiovec(L, -2, NULL, ud, SF_HDTR_CH_TRL));
}

/*
 * Metamethods
 */

static int
SF_HDTR_gc(lua_State *L)
{
    luab_sf_hdtr_t *ud;
    enum sf_hdtr_cache n;

    (void)luab_core_checkmaxargs(L, 1);

    ud = luab_to_sf_hdtr(L, 1);

    for (n = SF_HDTR_CH_HDR; n < SF_HDTR_CH_MAX; n++)
        luab_table_free(ud->ud_cache[n]);

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
    luab_sf_hdtr_t *ud;

    ud = luab_to_sf_hdtr(L, narg);
    return (&(ud->ud_hdtr));
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