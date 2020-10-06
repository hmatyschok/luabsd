/*
 * Copyright (c) 2020 Henning Matyschok <hmatyschok@outlook.com>
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
#include "luab_types.h"

#if __BSD_VISIBLE
/*
 * XXX well, the implementation if this "feature" is
 *  incomplete and _under_construction_ [sic!], etc.
 */

extern luab_module_t msghdr_type;

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

/*
 * By *msg_{name,control} reffered data maps to msg_buf[MH_{NAME,CONTROL}] avoids
 * possible race-cond. with Lua-gc.
 *
 * XXX msg_buf[MH_{NAME,CONTROL}] or "this" shall replaced by hooks.
 */

typedef struct luab_msghdr {
    luab_udata_t    ud_softc;
    struct msghdr   msg_hdr;
    ssize_t         msg_len;
    struct iovec    msg_buf[MH_MAX_BUF+1];
} luab_msghdr_t;

#define luab_new_msghdr(L, arg) \
    ((luab_msghdr_t *)luab_newudata(L, &msghdr_type, (arg)))
#define luab_to_msghdr(L, narg) \
    (luab_todata((L), (narg), &msghdr_type, luab_msghdr_t *))

#define LUAB_MSGHDR_TYPE_ID    1597320239
#define LUAB_MSGHDR_TYPE    "MSGHDR*"

/*
 * Subr.
 */

static void
msghdr_free_iov(struct msghdr *msg)
{
    struct iovec *iov;
    int i, iovlen;

    if (((iov = msg->msg_iov) != NULL) &&
        ((iovlen = msg->msg_iovlen) > 0)) {

        for (i = 0; i < iovlen; i++) {
            if (iov->iov_base != NULL) {
                free(iov->iov_base);
                iov->iov_base = NULL;
                iov->iov_len = 0;
            }
        }
        free(msg->msg_iov);
        msg->msg_iov = NULL;
        msg->msg_iovlen = 0;
    }
}

/* select n-th luab_iovec{} by #idx and initialize iov{} maps to #idx */
static int
msghdr_init_iov(lua_State *L, int narg, struct iovec *iov, int idx)
{
    luab_iovec_t *buf;
    struct iovec *src;
    struct iovec *dst;
    int status;

    /*
     * XXX race-cond. with gc, as a workaround.
     */

    if (((buf = luab_isiovec(L, narg)) != NULL) &&
        (buf->iov_flags & (IOV_PROXY|IOV_BUFF))) {
        buf->iov_flags |= IOV_LOCK;

        src = &(buf->iov);
        dst = &(iov[idx]);

        if (buf->iov_flags & IOV_BUFF) {

            if ((dst->iov_base = calloc(1, buf->iov_max_len)) != NULL) {

                if (src->iov_len > 0) {
                    (void)memmove(dst->iov_base, src->iov_base, src->iov_len);
                    dst->iov_len = src->iov_len;
                } else
                    dst->iov_len = buf->iov_max_len;

                status = 0;
            } else
                status = -1;

        } else {
            errno = EOPNOTSUPP;
            status = -1;
        }
        buf->iov_flags &= ~IOV_LOCK;
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (status);
}

static int
msghdr_populate_iovec(lua_State *L, int narg, struct msghdr *msg, int new)
{
    struct iovec *iov;
    int i, j, status;

    if (((iov = msg->msg_iov) != NULL) &&
        (msg->msg_iovlen > 0)) {

        if (new != 0)
            lua_newtable(L);
        else
            lua_pushnil(L);

        for (i = 0, j = 1; i < msg->msg_iovlen; i++, j++, iov++)
            luab_iovec_rawsetldata(L, narg, j, iov->iov_base, iov->iov_len);

        lua_pop(L, 0);
        status = 0;
    } else {
        errno = ENOENT;
        status = -1;
    }
    return (status);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(MSGHDR)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              msg_name    = (LUA_TUSERDATA(SOCKADDR)),
 *              msg_namelen = (LUA_TNUMBER),
 *              msg_iov     = (LUA_TTABLE(LUA_TNUMBER,LUA_TUSERDATA(IOVEC)),
 *              msg_iovlen  = (LUA_TNUMBER),
 *          }
 *
 * @usage t = msghdr:get()
 */
static int
MSGHDR_get(lua_State *L)
{
    struct msghdr *msg;

    (void)luab_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, &msghdr_type, struct msghdr *);

    lua_newtable(L);

    if (msg->msg_name != NULL) {
        luab_setudata(L, -2, luab_mx(SOCKADDR), "msg_name", msg->msg_name);
        luab_setinteger(L, -2, "msg_namelen", msg->msg_namelen);
    }

    if (msghdr_populate_iovec(L, -2, msg, 1) == 0) {
        lua_setfield(L, -2, "msg_iov");
        luab_setinteger(L, -2, "msg_iovlen", msg->msg_iovlen);
    }
    lua_pushvalue(L, -1);

    return (1);
}

/*
 * Accessor for immutable properties.
 */

/***
 * Get # elements in msg_iov.
 *
 * @function msg_iovlen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (size [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage size [, err, msg ] = msghdr:msg_iovlen()
 */
static int
MSGHDR_msg_iovlen(lua_State *L)
{
    struct msghdr *msg;
    int msg_iovlen;

    (void)luab_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, &msghdr_type, struct msghdr *);
    msg_iovlen = msg->msg_iovlen;

    return (luab_pusherr(L, msg_iovlen));
}

/***
 * Get flags on received message.
 *
 * @function msg_flags
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (flags [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage flags [, err, msg ] = msghdr:msg_flags()
 */
static int
MSGHDR_msg_flags(lua_State *L)
{
    struct msghdr *msg;
    int msg_flags;

    (void)luab_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, &msghdr_type, struct msghdr *);
    msg_flags = msg->msg_flags;

    return (luab_pusherr(L, msg_flags));
}

/***
 * Get amount of by {recv,send}mmsg(2) rx'd / tx'd bytes.
 *
 * @function msg_len
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (size [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage flags [, err, msg ] = msghdr:msg_len()
 */
static int
MSGHDR_msg_len(lua_State *L)
{
    struct mmsghdr *msg;
    int msg_len;

    (void)luab_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, &msghdr_type, struct mmsghdr *);
    msg_len = msg->msg_len;

    return (luab_pusherr(L, msg_len));
}

/*
 * Common accessor on mutuable properties.
 */

/***
 * Set optional address.
 *
 * @function set_msg_name
 *
 * @param name              Instance of (LUA_TUSERDATA(IOVEC)) or nil.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = msghdr:set_msg_name(name)
 */
static int
MSGHDR_set_msg_name(lua_State *L)
{
    luab_msghdr_t *self;
    struct sockaddr *sa;
    struct msghdr *msg;
    struct iovec *buf;
    caddr_t name;
    int status;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_msghdr(L, 1);
    sa = luab_udataisnil(L, 2, luab_mx(SOCKADDR), struct sockaddr *);

    msg = &(self->msg_hdr);
    buf = &(self->msg_buf[MH_NAME]);

    if ((name = (caddr_t)sa) != NULL) {
        if ((status = luab_iov_copyin(buf, name, sa->sa_len)) == 0)
            msg->msg_name = buf->iov_base;
    } else {
        if ((status = luab_iov_clear(buf)) == 0)
            msg->msg_name = NULL;
    }
    return (luab_pusherr(L, status));
}

/***
 * Get optional address.
 *
 * @function get_msg_name
 *
 * @param name              Value-result argument, instance of
 *                          LUA_TUSERDATA(luab_sockaddr_t).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = msghdr:get_msg_name(name)
 */
static int
MSGHDR_get_msg_name(lua_State *L)
{
    struct msghdr *msg;
    struct sockaddr *dst;
    struct sockaddr *src;
    int status;

    (void)luab_checkmaxargs(L, 2);

    msg = luab_udata(L, 1, &msghdr_type, struct msghdr *);
    dst = luab_udata(L, 2, luab_mx(SOCKADDR), struct sockaddr *);

    if (((src = msg->msg_name) != NULL) &&
        (src->sa_len == msg->msg_namelen)) {
        (void)memmove(dst, src, src->sa_len);
        status = 0;
    } else {
        errno = EADDRNOTAVAIL;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * Set size of address.
 *
 * @function set_msg_namelen
 *
 * @param size
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = msghdr:set_msg_namelen(size)
 */
static int
MSGHDR_set_msg_namelen(lua_State *L)
{
    struct msghdr *msg;
    socklen_t msg_namelen;

    (void)luab_checkmaxargs(L, 2);

    msg = luab_udata(L, 1, &msghdr_type, struct msghdr *);
    msg_namelen = (socklen_t)luab_checkinteger(L, 2, INT_MAX);

    msg->msg_namelen = msg_namelen;

    return (luab_pusherr(L, 0));
}

/***
 * Get size of address.
 *
 * @function get_msg_namelen
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (size [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage size [, err, msg ] = msghdr:get_msg_namelen()
 */
static int
MSGHDR_get_msg_namelen(lua_State *L)
{
    struct msghdr *msg;
    socklen_t msg_namelen;

    (void)luab_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, &msghdr_type, struct msghdr *);
    msg_namelen = msg->msg_namelen;

    return (luab_pusherr(L, msg_namelen));
}

/***
 * Translate LUA_TTABLE into an array over iovec{}.
 *
 * @function set_msg_iov
 *
 * @param t                 LUA_TTABLE((LUA_TUSERDATA(IOVEC))).
 *
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = msghdr:set_msg_iov(t)
 */
static int
MSGHDR_set_msg_iov(lua_State *L)
{
    struct msghdr *msg;
    struct iovec *iov;
    int status;

    (void)luab_checkmaxargs(L, 2);

    /*
     * Translate a LUA_TTABLE over LUA_TUSERDATA into an array for scattered I/O.
     *
     * Raises error, iff (if and only if) argv[argn] is not LUA_TTABLE. Otherwise,
     * errno will be set and by msg_iov bound ressources are released or free'd.
     */

    msg = luab_udata(L, 1, &msghdr_type, struct msghdr *);
    iov = luab_newvector(L, 2, NULL, sizeof(struct iovec));

    msghdr_free_iov(msg);
    msg->msg_iov = iov;

    lua_pushnil(L);

    while (lua_next(L, 2) != 0) {   /* XXX DRY. */

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isuserdata(L, -1) != 0)) {

            status = msghdr_init_iov(L, -1, iov, msg->msg_iovlen);
        } else {
            errno = EINVAL;
            status = -1;
            break;
        }
        lua_pop(L, 1);

        msg->msg_iovlen++;
    }

    if (status != 0) {
        msghdr_free_iov(msg);

        lua_pop(L, 1);
    }
    return (luab_pusherr(L, status));
}

/***
 * Map by recvmsg(2) got set of messagaes over iovec{} into table.
 *
 * @function get_msg_iov
 *
 * @param t                 Value-result argument, instance of
 *                          LUA_TTABLE((LUA_TUSERDATA(IOVEC)))
 *                          with cardinality of 0.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = msghdr:get_msg_iov()
 */
static int
MSGHDR_get_msg_iov(lua_State *L)
{
    struct msghdr *msg;
    int status;

    (void)luab_checkmaxargs(L, 2);

    msg = luab_udata(L, 1, &msghdr_type, struct msghdr *);

    (void)luab_checkltable(L, 2, 0);

    status = msghdr_populate_iovec(L, 2, msg, 0);

    return (luab_pusherr(L, status));
}

/*
 * Metamethods.
 */

static int
MSGHDR_gc(lua_State *L)
{
    luab_msghdr_t *self;
    struct iovec *buf;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_msghdr(L, 1);
    buf = self->msg_buf;

    while (buf->iov_base != NULL)
        luab_iov_free(buf++);

    (void)memset_s(self, msghdr_type.sz, 0, msghdr_type.sz);

    return (0);
}

static int
MSGHDR_len(lua_State *L)
{
    return (luab_len(L, 2, &msghdr_type));
}

static int
MSGHDR_tostring(lua_State *L)
{
    return (luab_tostring(L, 1, &msghdr_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t msghdr_methods[] = {
    LUAB_FUNC("msg_iovlen",       MSGHDR_msg_iovlen),
    LUAB_FUNC("msg_flags",        MSGHDR_msg_flags),
    LUAB_FUNC("msg_len",          MSGHDR_msg_len),
/*  LUAB_FUNC("msg_controllen",   MSGHDR_msg_controllen), */
    LUAB_FUNC("set_msg_name",     MSGHDR_set_msg_name),
    LUAB_FUNC("set_msg_namelen",  MSGHDR_set_msg_namelen),
    LUAB_FUNC("set_msg_iov",      MSGHDR_set_msg_iov),
/*  LUAB_FUNC("set_msg_control",  MSGHDR_set_msg_control), */
    LUAB_FUNC("set_msg_flags",    MSGHDR_set_msg_iov),
    LUAB_FUNC("get",              MSGHDR_get),
    LUAB_FUNC("get_msg_name",     MSGHDR_get_msg_name),
    LUAB_FUNC("get_msg_namelen",  MSGHDR_get_msg_namelen),
    LUAB_FUNC("get_msg_iov",      MSGHDR_get_msg_iov),
/*  LUAB_FUNC("get_msg_control",  MSGHDR_get_msg_control), */
    LUAB_FUNC("get_msg_flags",    MSGHDR_get_msg_iov),
    LUAB_FUNC("__gc",             MSGHDR_gc),
    LUAB_FUNC("__len",            MSGHDR_len),
    LUAB_FUNC("__tostring",       MSGHDR_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
msghdr_create(lua_State *L, void *arg __unused)
{
    struct iovec buf[MH_MAX_BUF+1];
    luab_msghdr_t *self;
    int i;

    (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));

    for (i = 0; i < MH_MAX_BUF; i++) {
        if (luab_iov_alloc(&buf[i], MHLEN) != 0)
            break;
    }

    if (i == MH_MAX_BUF) {
        self = luab_new_msghdr(L, buf);
    } else
        self = NULL;

    return (self);
}

static void
msghdr_init(void *ud, void *arg)
{
    luab_msghdr_t *self;
    struct iovec *src, *dst;

    if (((self = (luab_msghdr_t *)ud) != NULL) &&
        ((src = (struct iovec *)arg) != NULL)) {

        dst = self->msg_buf;

        while (src->iov_base != NULL) {
            dst->iov_len = src->iov_len;
            dst->iov_base = src->iov_base;

            dst++; src++;
        }
    }
}

static void *
msghdr_udata(lua_State *L, int narg)
{
    luab_msghdr_t *self = luab_to_msghdr(L, narg);

    return (&self->msg_hdr);
}

luab_module_t msghdr_type = {
    .cookie = LUAB_MSGHDR_TYPE_ID,
    .name = LUAB_MSGHDR_TYPE,
    .vec = msghdr_methods,
    .create = msghdr_create,
    .init = msghdr_init,
    .get = msghdr_udata,
    .sz = sizeof(luab_msghdr_t),
};
#endif /* __BSD_VISIBLE */
