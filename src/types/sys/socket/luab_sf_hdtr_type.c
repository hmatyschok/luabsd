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

#include <err.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#if __BSD_VISIBLE
extern luab_module_t luab_sf_hdtr_type;

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

typedef enum luab_xiovec {
    LUAB_XIOVEC_HDR,
    LUAB_XIOVEC_TRL,
    LUAB_XIOVEC_MAX
} luab_xiovec_t;

typedef int    (*luab_xiovec_init_fn)(struct sf_hdtr *, luab_table_t *);

typedef struct luab_xiovec_vec {
    luab_xiovec_t       xiv_idx;
    luab_type_t         xiv_type;
    luab_id_t           xiv_cookie;
    luab_xiovec_init_fn xiv_init;
} luab_xiovec_vec_t;


typedef struct luab_sf_hdtr {
    luab_udata_t    ud_softc;
    struct sf_hdtr  ud_hdtr;
    luab_table_t    *ud_cache[LUAB_XIOVEC_MAX];
} luab_sf_hdtr_t;

#define LUAB_XIOVEC_VEC_SENTINEL                    \
    {                                               \
        .xiv_idx   = LUAB_XIOVEC_MAX,               \
        .xiv_type = 0,                              \
        .xiv_cookie = 0,                            \
        .xiv_init   = NULL,                         \
    }

#define luab_xdomain(idx) \
    (sf_hdtr_domain[((idx) % LUAB_XIOVEC_MAX)])

/*
 * Subr.
 */

static int
sf_hdtr_hdr(struct sf_hdtr *hdtr, luab_table_t *tbl)
{
    if (tbl != NULL) {
        hdtr->headers = (struct iovec *)(tbl->tbl_vec);
        hdtr->hdr_cnt = tbl->tbl_card;
    } else {
        hdtr->headers = NULL;
        hdtr->hdr_cnt = 0;
    }
    return (hdtr->hdr_cnt);
}

static int
sf_hdtr_trl(struct sf_hdtr *hdtr, luab_table_t *tbl)
{
    if (tbl != NULL) {
        hdtr->trailers = (struct iovec *)(tbl->tbl_vec);
        hdtr->trl_cnt = tbl->tbl_card;
    } else {
        hdtr->trailers = NULL;
        hdtr->trl_cnt = 0;
    }
    return (hdtr->trl_cnt);
}

static luab_xiovec_vec_t sf_hdtr_domain[] = {
    {
        .xiv_idx    = LUAB_XIOVEC_HDR,
        .xiv_type   = LUAB_IOVEC_IDX,
        .xiv_cookie = LUAB_IOVEC_TYPE_ID,
        .xiv_init   = sf_hdtr_hdr,
    },{
        .xiv_idx    = LUAB_XIOVEC_TRL,
        .xiv_type   = LUAB_IOVEC_IDX,
        .xiv_cookie = LUAB_IOVEC_TYPE_ID,
        .xiv_init   = sf_hdtr_trl,
    },
    LUAB_XIOVEC_VEC_SENTINEL
};

static int
sf_hdtr_checkxiovec(lua_State *L, int narg, luab_sf_hdtr_t *self, luab_xiovec_t n)
{
    luab_xiovec_vec_t *xiv;
    luab_module_t *m;
    luab_table_t *tbl;
    int card = 0;

    if (self != NULL) {
        xiv = &(luab_xdomain(n % LUAB_XIOVEC_MAX));
        m = luab_core_checkmodule(xiv->xiv_type, xiv->xiv_cookie, __func__);

        if ((tbl = luab_table_checkxdata(L, narg, m)) != NULL) {

            if (self->ud_cache[xiv->xiv_idx] != NULL)
                luab_table_free(self->ud_cache[xiv->xiv_idx]);

            if (tbl->tbl_card == 0) {
                luab_table_free(tbl);
                tbl = NULL;
            }
            self->ud_cache[xiv->xiv_idx] = tbl;
        }
        card = (*xiv->xiv_init)(&self->ud_hdtr, tbl);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);

    return (card);
}

static int
sf_hdtr_pushxiovec(lua_State *L, int narg, const char *k, luab_sf_hdtr_t *self,
    luab_xiovec_t n)
{
    luab_xiovec_vec_t *xiv;
    luab_module_t *m;
    luab_table_t *tbl;

    xiv = &(luab_xdomain(n % LUAB_XIOVEC_MAX));
    m = luab_core_checkmodule(xiv->xiv_type, xiv->xiv_cookie, __func__);

    if ((tbl = self->ud_cache[xiv->xiv_idx]) != NULL) {
        luab_table_pushxdata(L, narg, m, tbl, 1, 0);

        /*
         * Set field k and/or push on top of Lua stack.
         *
         * XXX DRY
         */
        if (k != NULL)
            lua_setfield(L, narg, k);
        else {
            if (narg < 0)
                lua_pushvalue(L, narg + 1);
            else
                lua_pushvalue(L, narg);
        }
        errno = 0;
    } else
        errno = ERANGE;

    return (luab_table_pusherr(L, errno, 1));
}

static void
sf_hdtr_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_sf_hdtr_t *self;
    struct sf_hdtr *hdtr;

    if ((self = (luab_sf_hdtr_t *)arg) != NULL) {
        hdtr = &(self->ud_hdtr);

        luab_setinteger(L, narg, "hdr_cnt", hdtr->hdr_cnt);
        luab_setinteger(L, narg, "trl_cnt", hdtr->trl_cnt);

        if (hdtr->headers != NULL)
            (void)sf_hdtr_pushxiovec(L, narg, "headers", self, LUAB_XIOVEC_HDR);

        if (hdtr->trailers != NULL)
            (void)sf_hdtr_pushxiovec(L, narg, "trailers", self, LUAB_XIOVEC_TRL);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SF_HDTR)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              headers     = (LUA_TTABLE) -> (LUA_TUSERDATA(IOVEC),
 *              hdr_cnt     = (LUA_TNUMBER),
 *              trailers    = (LUA_TTABLE) -> (LUA_TUSERDATA(IOVEC)
 *              trl_cnt     = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = sf_hdtr:get_table()
 */
static int
SF_HDTR_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SF_HDTR, TYPE, __func__);

    xtp.xtp_fill = sf_hdtr_fillxtable;
    xtp.xtp_arg = (void *)luab_todata(L, 1, m, luab_sf_hdtr_t *);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
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
    luab_module_t *m;
    struct sf_hdtr *hdtr;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SF_HDTR, TYPE, __func__);
    hdtr = luab_udata(L, 1, m, struct sf_hdtr *);
    x = hdtr->hdr_cnt;

    return (luab_pushxinteger(L, x));
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
    luab_module_t *m;
    struct sf_hdtr *hdtr;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SF_HDTR, TYPE, __func__);
    hdtr = luab_udata(L, 1, m, struct sf_hdtr *);
    x = hdtr->trl_cnt;

    return (luab_pushxinteger(L, x));
}

/***
 * Set headers by an array of iovec{}s
 *
 * @function set_headers
 *
 * @param arg               Instance of (LUA_TTABLE).
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
 * @usage card [, err, msg ] = sf_hdtr:set_headers(arg)
 */
static int
SF_HDTR_set_headers(lua_State *L)
{
    luab_module_t *m;
    luab_sf_hdtr_t *self;
    int card;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(SF_HDTR, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_sf_hdtr_t *);
    card = sf_hdtr_checkxiovec(L, 2, self, LUAB_XIOVEC_HDR);
    return (luab_pushxinteger(L, card));
}

/***
 * Get headers by (LUA_TTABLE).
 *
 * @function get_headers
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sf_hdtr:get_headers()
 */
static int
SF_HDTR_get_headers(lua_State *L)
{
    luab_module_t *m;
    luab_sf_hdtr_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SF_HDTR, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_sf_hdtr_t *);
    return (sf_hdtr_pushxiovec(L, -2, NULL, self, LUAB_XIOVEC_HDR));
}

/***
 * Set trailers by an array of iovec{}s
 *
 * @function set_trailers
 *
 * @param arg               Instance of (LUA_TTABLE).
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
 * @usage x [, err, msg ] = sf_hdtr:set_trailers(arg)
 */
static int
SF_HDTR_set_trailers(lua_State *L)
{
    luab_module_t *m;
    luab_sf_hdtr_t *self;
    int card;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(SF_HDTR, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_sf_hdtr_t *);
    card = sf_hdtr_checkxiovec(L, 2, self, LUAB_XIOVEC_TRL);
    return (luab_pushxinteger(L, card));
}

/***
 * Get trailers by (LUA_TTABLE).
 *
 * @function get_trailers
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sf_hdtr:get_trailers()
 */
static int
SF_HDTR_get_trailers(lua_State *L)
{
    luab_module_t *m;
    luab_sf_hdtr_t *self;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SF_HDTR, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_sf_hdtr_t *);
    return (sf_hdtr_pushxiovec(L, -2, NULL, self, LUAB_XIOVEC_TRL));
}

/*
 * Metamethods
 */

static int
SF_HDTR_gc(lua_State *L)
{
    luab_module_t *m;
    luab_sf_hdtr_t *self;
    luab_xiovec_t n;
    luab_table_t *tbl;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SF_HDTR, TYPE, __func__);
    self = luab_todata(L, 1, m, luab_sf_hdtr_t *);

    n = LUAB_XIOVEC_HDR;

    do {
        tbl = self->ud_cache[n];
        luab_table_free(tbl);
        n++;
    } while (n < LUAB_XIOVEC_MAX);

    return (luab_core_gc(L, 1, m));
}

static int
SF_HDTR_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SF_HDTR, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
SF_HDTR_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SF_HDTR, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t sf_hdtr_methods[] = {
    LUAB_FUNC("hdr_cnt",        SF_HDTR_hdr_cnt),
    LUAB_FUNC("trl_cnt",        SF_HDTR_trl_cnt),
    LUAB_FUNC("set_headers",    SF_HDTR_set_headers),
    LUAB_FUNC("set_trailers",   SF_HDTR_set_trailers),
    LUAB_FUNC("get_table",      SF_HDTR_get_table),
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
    luab_module_t *m;
    m = luab_xmod(SF_HDTR, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
sf_hdtr_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SF_HDTR, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
sf_hdtr_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_sf_hdtr_t *self;

    m = luab_xmod(SF_HDTR, TYPE, __func__);
    self = luab_todata(L, narg, m, luab_sf_hdtr_t *);
    return ((void *)&(self->ud_hdtr));
}

static luab_table_t *
sf_hdtr_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct sf_hdtr *x, *y;
    size_t i, j;

    m = luab_xmod(SF_HDTR, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct sf_hdtr *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct sf_hdtr *);
                        (void)memmove(&(x[i]), y, luab_sf_hdtr_type.m_sz);
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        } else
            errno = ERANGE;
    }
    return (tbl);
}

static void
sf_hdtr_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct sf_hdtr *x;
    size_t i, j, k;

    m = luab_xmod(SF_HDTR, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct sf_hdtr *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, new);

            for (i = 0, j = tbl->tbl_card, k = 1; i < j; i++, k++)
                luab_rawsetxdata(L, narg, m, k, &(x[i]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

static luab_table_t *
sf_hdtr_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(SF_HDTR, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_sf_hdtr_type = {
    .m_id           = LUAB_SF_HDTR_TYPE_ID,
    .m_name         = LUAB_SF_HDTR_TYPE,
    .m_vec          = sf_hdtr_methods,
    .m_create       = sf_hdtr_create,
    .m_init         = sf_hdtr_init,
    .m_get          = sf_hdtr_udata,
    .m_get_tbl      = sf_hdtr_checktable,
    .m_set_tbl      = sf_hdtr_pushtable,
    .m_alloc_tbl    = sf_hdtr_alloctable,
    .m_len          = sizeof(luab_sf_hdtr_t),
    .m_sz           = sizeof(struct sf_hdtr),
};
#endif /* __BSD_VISBLE */
