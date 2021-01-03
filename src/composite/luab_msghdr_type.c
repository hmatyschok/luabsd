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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#if __BSD_VISIBLE
/*
 * XXX well, the "implementation" of this "feature" is
 *  incomplete and _under_construction_ [sic!], etc.
 */

extern luab_module_t luab_msghdr_type;

/*
 * Interface against
 *
 *  struct msghdr {
 *      void        *msg_name;
 *      socklen_t    msg_namelen;
 *      struct iovec    *msg_iov;
 *      int      msg_iovlen;
 *      void        *msg_control;
 *      socklen_t    msg_controllen;
 *      int      msg_flags;
 *  };
 *
 * and
 *
 *  struct mmsghdr {
 *      struct msghdr   msg_hdr;
 *      ssize_t     msg_len;
 *  };
 */

#define MH_NAME     0
#define MH_CONTROL  1
#define MH_MAX_BUF  2

typedef struct luab_msghdr {
    luab_udata_t    ud_softc;
    struct msghdr   msg_hdr;
    ssize_t         msg_len;
    luab_table_t    *msg_buf;
} luab_msghdr_t;

#define luab_new_msghdr(L, arg) \
    ((luab_msghdr_t *)luab_newudata(L, &luab_msghdr_type, (arg)))
#define luab_to_msghdr(L, narg) \
    (luab_todata((L), (narg), &luab_msghdr_type, luab_msghdr_t *))

/*
 * Subr.
 */

static int
msghdr_pushiovec(lua_State *L, int narg, const char *k, luab_table_t *tbl)
{
    luab_module_t *m;

    m = luab_xmod(IOVEC, TYPE, __func__);

    if (tbl != NULL) {
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
    } else
        errno = ERANGE;

    return (luab_table_pusherr(L, errno, 1));
}

static void
msghdr_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_msghdr_t *self;
    struct msghdr *msg;

    if ((self = (luab_msghdr_t *)arg) != NULL) {
        msg = &(self->msg_hdr);

        luab_setinteger(L, narg, "msg_iovlen", msg->msg_iovlen);
        luab_setinteger(L, narg, "msg_namelen", msg->msg_namelen);

        if (msg->msg_name != NULL)
            luab_setxdata(L, narg, luab_xmod(SOCKADDR, TYPE, __func__), "msg_name", msg->msg_name);

        if (msg->msg_iov != NULL)
            (void)msghdr_pushiovec(L, narg, "msg_iov", self->msg_buf);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(MSGHDR)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              msg_name    = (LUA_TUSERDATA(SOCKADDR)),
 *              msg_namelen = (LUA_TNUMBER),
 *              msg_iov     = (LUA_TTABLE(LUA_TNUMBER,LUA_TUSERDATA(IOVEC)),
 *              msg_iovlen  = (LUA_TNUMBER),
 *          }
 *
 * @usage t = msghdr:get_table()
 */
static int
MSGHDR_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = msghdr_fillxtable;
    xtp.xtp_arg = luab_to_msghdr(L, 1);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - returns (LUA_TNIL).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = msghdr:dump()
 */
static int
MSGHDR_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions, immutable properties.
 */


/***
 * Get size of address.
 *
 * @function msg_namelen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage namelen [, err, msg ] = msghdr:msg_namelen()
 */
static int
MSGHDR_msg_namelen(lua_State *L)
{
    struct msghdr *msg;
    socklen_t namelen;

    (void)luab_core_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, &luab_msghdr_type, struct msghdr *);
    namelen = msg->msg_namelen;

    return (luab_pushxinteger(L, namelen));
}

/***
 * Get # elements in msg_iov.
 *
 * @function msg_iovlen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage size [, err, msg ] = msghdr:msg_iovlen()
 */
static int
MSGHDR_msg_iovlen(lua_State *L)
{
    struct msghdr *msg;
    int iovlen;

    (void)luab_core_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, &luab_msghdr_type, struct msghdr *);
    iovlen = msg->msg_iovlen;

    return (luab_pushxinteger(L, iovlen));
}

/***
 * Get flags on received message.
 *
 * @function msg_flags
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage flags [, err, msg ] = msghdr:msg_flags()
 */
static int
MSGHDR_msg_flags(lua_State *L)
{
    struct msghdr *msg;
    int flags;

    (void)luab_core_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, &luab_msghdr_type, struct msghdr *);
    flags = msg->msg_flags;

    return (luab_pushxinteger(L, flags));
}

/***
 * Get amount of by {recv,send}mmsg(2) rx'd / tx'd bytes.
 *
 * @function msg_len
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage len [, err, msg ] = msghdr:msg_len()
 */
static int
MSGHDR_msg_len(lua_State *L)
{
    struct mmsghdr *msg;
    int len;

    (void)luab_core_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, &luab_msghdr_type, struct mmsghdr *);
    len = msg->msg_len;

    return (luab_pushxinteger(L, len));
}

/*
 * Access functions, mutuable properties.
 */

/***
 * Set optional address.
 *
 * @function set_msg_name
 *
 * @param sockaddr          Instance of (LUA_TUSERDATA(SOCKADDR)) or nil.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage namelen [, err, msg ] = msghdr:set_msg_name(sockaddr)
 */
static int
MSGHDR_set_msg_name(lua_State *L)
{
    luab_udata_t *udx;
    struct msghdr *msg;
    struct sockaddr *sa;
    void **dp;
    int namelen;

    (void)luab_core_checkmaxargs(L, 2);

    msg = (struct msghdr *)luab_checkxdata(L, 1, &luab_msghdr_type, &udx);
    dp = luab_dptox(msg->msg_name);

    sa = luab_udata_checkxlink(L, 2, luab_xmod(SOCKADDR, TYPE, __func__), udx, dp);

    if (sa != NULL)
        msg->msg_namelen = sa->sa_len;
    else
        msg->msg_namelen = 0;

    namelen = msg->msg_namelen;

    return (luab_pushxinteger(L, namelen));
}

/***
 * Get optional address.
 *
 * @function get_msg_name
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sockaddr [, err, msg ] = msghdr:get_msg_name()
 */
static int
MSGHDR_get_msg_name(lua_State *L)
{
    struct msghdr *msg;
    struct sockaddr *sa;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, &luab_msghdr_type, struct msghdr *);

    if ((sa = msg->msg_name) != NULL)
        status = luab_pushxdata(L, luab_xmod(SOCKADDR, TYPE, __func__), sa);
    else {
        errno = EADDRNOTAVAIL;
        status = luab_pushnil(L);
    }
    return (status);
}

/***
 * Translate (LUA_TTABLE) into an array over iovec{}.
 *
 * @function set_msg_iov
 *
 * @param t                 Set of iovec{}s, instance of (LUA_TTABLE).
 *
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovlen [, err, msg ] = msghdr:set_msg_iov(t)
 */
static int
MSGHDR_set_msg_iov(lua_State *L)
{
    luab_msghdr_t *ud;
    struct msghdr *msg;
    luab_table_t *iov;
    int iovlen;

    (void)luab_core_checkmaxargs(L, 2);

    ud = luab_to_msghdr(L, 1);
    msg = &(ud->msg_hdr);

    if ((iov = luab_table_checkxdata(L, 2, luab_xmod(IOVEC, TYPE, __func__))) != NULL) {
        luab_iovec_freetable(ud->msg_buf);
        ud->msg_buf = iov;

        msg->msg_iov = iov->tbl_vec;
        msg->msg_iovlen = (iov->tbl_card - 1);

        iovlen = msg->msg_iovlen;
    } else
        iovlen = luab_env_error;

    return (luab_pushxinteger(L, iovlen));
}

/***
 * Translate msg_iov attribute into an instance of (LUA_TTABLE).
 *
 * @function get_msg_iov
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage t [, err, msg ] = msghdr:get_msg_iov()
 */
static int
MSGHDR_get_msg_iov(lua_State *L)
{
    luab_msghdr_t *ud;
    luab_table_t *iov;

    (void)luab_core_checkmaxargs(L, 2);

    ud = luab_to_msghdr(L, 1);
    iov = ud->msg_buf;

    return (msghdr_pushiovec(L, -2, NULL, iov));
}

/*
 * Metamethods.
 */

static int
MSGHDR_gc(lua_State *L)
{
    luab_msghdr_t *ud;
    luab_table_t *iov;

    (void)luab_core_checkmaxargs(L, 1);

    ud = luab_to_msghdr(L, 1);
    iov = ud->msg_buf;

    luab_iovec_freetable(iov);

    return (luab_core_gc(L, 1, &luab_msghdr_type));
}

static int
MSGHDR_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_msghdr_type));
}

static int
MSGHDR_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_msghdr_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t msghdr_methods[] = {
    LUAB_FUNC("msg_namelen",        MSGHDR_msg_namelen),
    LUAB_FUNC("msg_iovlen",         MSGHDR_msg_iovlen),
    LUAB_FUNC("msg_flags",          MSGHDR_msg_flags),
    LUAB_FUNC("msg_len",            MSGHDR_msg_len),
/*  LUAB_FUNC("msg_controllen",     MSGHDR_msg_controllen), */
    LUAB_FUNC("set_msg_name",       MSGHDR_set_msg_name),
    LUAB_FUNC("set_msg_iov",        MSGHDR_set_msg_iov),
/*  LUAB_FUNC("set_msg_control",    MSGHDR_set_msg_control), */
    LUAB_FUNC("set_msg_flags",      MSGHDR_set_msg_iov),
    LUAB_FUNC("get_table",          MSGHDR_get_table),
    LUAB_FUNC("get_msg_name",       MSGHDR_get_msg_name),
    LUAB_FUNC("get_msg_iov",        MSGHDR_get_msg_iov),
/*  LUAB_FUNC("get_msg_control",    MSGHDR_get_msg_control), */
    LUAB_FUNC("get_msg_flags",      MSGHDR_get_msg_iov),
    LUAB_FUNC("dump",               MSGHDR_dump),
    LUAB_FUNC("__gc",               MSGHDR_gc),
    LUAB_FUNC("__len",              MSGHDR_len),
    LUAB_FUNC("__tostring",         MSGHDR_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
msghdr_create(lua_State *L, void *arg __unused)
{
    return (luab_new_msghdr(L, arg));
}

static void
msghdr_init(void *ud __unused, void *arg __unused)
{
    errno = EEXIST;
}

static void *
msghdr_udata(lua_State *L, int narg)
{
    luab_msghdr_t *ud = luab_to_msghdr(L, narg);

    return (&(ud->msg_hdr));
}

static luab_table_t *
msghdr_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct msghdr *x, *y;
    size_t i, j;

    if ((tbl = luab_table_newvectornil(L, narg, &luab_msghdr_type)) != NULL) {

        if (((x = (struct msghdr *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_msghdr_type, struct msghdr *);
                        (void)memmove(&(x[i]), y, luab_msghdr_type.m_sz);
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
msghdr_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct msghdr *x;
    size_t i, j, k;

    if (tbl != NULL) {

        if (((x = (struct msghdr *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, new);

            for (i = 0, j = tbl->tbl_card, k = 1; i < j; i++, k++)
                luab_rawsetxdata(L, narg, &luab_msghdr_type, k, &(x[i]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

static luab_table_t *
msghdr_alloctable(void *vec, size_t card)
{
    return (luab_table_create(&luab_msghdr_type, vec, card));
}

luab_module_t luab_msghdr_type = {
    .m_id           = LUAB_MSGHDR_TYPE_ID,
    .m_name         = LUAB_MSGHDR_TYPE,
    .m_vec          = msghdr_methods,
    .m_create       = msghdr_create,
    .m_init         = msghdr_init,
    .m_get          = msghdr_udata,
    .m_get_tbl      = msghdr_checktable,
    .m_set_tbl      = msghdr_pushtable,
    .m_alloc_tbl    = msghdr_alloctable,
    .m_len          = sizeof(luab_msghdr_t),
    .m_sz           = sizeof(struct msghdr),
};
#endif /* __BSD_VISIBLE */
