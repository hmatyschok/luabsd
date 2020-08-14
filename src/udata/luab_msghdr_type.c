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

#include <sys/param.h>
#include <sys/types.h>
#include <sys/mbuf.h>

#include <errno.h>
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t sockaddr_type;
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
 */

#define MH_NAME     0
#define MH_CONTROL  1
#define MH_MAX_BUF  2

/*
 * By reffered *msg_{name,control} data maps to mh_buf[MH_{NAME,CONTROL}] avoids
 * race-cond. with gc.
 */

typedef struct luab_msghdr {
    struct msghdr   mh_msg;
    luab_buf_t      mh_buf[MH_MAX_BUF+1];
} luab_msghdr_t;

#define luab_new_msghdr(L, arg) \
    ((luab_msghdr_t *)luab_newuserdata(L, &msghdr_type, (arg)))
#define luab_to_msghdr(L, narg) \
    (luab_todata((L), (narg), &msghdr_type, luab_msghdr_t *))

#define LUABSD_MSGHDR_TYPE_ID    1597320239
#define LUABSD_MSGHDR_TYPE    "MSGHDR*"

int luab_StructMsgHdr(lua_State *);

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

static int
msghdr_init_iov(lua_State *L, int narg, struct iovec *iov, int idx)
{
    luab_iovec_t *buf;
    struct iovec *src;
    struct iovec *dst;
    int status;

    /* XXX well, race-cond. with gc, on case of IOV_BUFF resolved. */

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
                    dst->iov_len = buf->iov_max_len; /* XXX */

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
            luab_rawsetbuff(L, narg, j, iov->iov_base, iov->iov_len);

        lua_pop(L, 0);
        status = 0;
    } else {
        errno = ENOENT;
        status = -1;
    }
    return (status);
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
MsgHdr_msg_iovlen(lua_State *L)
{
    struct msghdr *msg;
    int msg_iovlen;

    (void)luab_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, msghdr_type, struct msghdr *);
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
MsgHdr_msg_flags(lua_State *L)
{
    struct msghdr *msg;
    int msg_flags;

    (void)luab_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, msghdr_type, struct msghdr *);
    msg_flags = msg->msg_flags;

    return (luab_pusherr(L, msg_flags));
}

/*
 * Accessor.
 */

/***
 * Set optional address.
 *
 * @function set_msg_name
 *
 * @param name              Instance of LUA_TUSERDATA(luab_iovec_t) or nil.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = msghdr:set_msg_name(name)
 */
static int
MsgHdr_set_msg_name(lua_State *L)
{
    luab_msghdr_t *self;
    struct sockaddr *sa;
    struct msghdr *msg;
    luab_buf_t *buf;
    caddr_t name;
    int status;

    (void)luab_checkmaxargs(L, 2);

    self = luab_to_msghdr(L, 1);
    sa = luab_udataisnil(L, 2, sockaddr_type, struct sockaddr *);

    msg = &(self->mh_msg);
    buf = &(self->mh_buf[MH_NAME]);

    if ((name = (caddr_t)sa) != NULL) {
        if ((status = luab_buf_copy_in(buf, name, sa->sa_len)) == 0)
            msg->msg_name = buf->buf_data;
    } else {
        if ((status = luab_buf_clear(buf)) == 0)
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
MsgHdr_get_msg_name(lua_State *L)
{
    struct msghdr *msg;
    struct sockaddr *dst;
    struct sockaddr *src;
    int status;

    (void)luab_checkmaxargs(L, 2);

    msg = luab_udata(L, 1, msghdr_type, struct msghdr *);
    dst = luab_udata(L, 2, sockaddr_type, struct sockaddr *);

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
MsgHdr_set_msg_namelen(lua_State *L)
{
    struct msghdr *msg;
    socklen_t msg_namelen;

    (void)luab_checkmaxargs(L, 2);

    msg = luab_udata(L, 1, msghdr_type, struct msghdr *);
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
MsgHdr_get_msg_namelen(lua_State *L)
{
    struct msghdr *msg;
    socklen_t msg_namelen;

    (void)luab_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, msghdr_type, struct msghdr *);
    msg_namelen = msg->msg_namelen;

    return (luab_pusherr(L, msg_namelen));
}

/***
 * Translate LUA_TTABLE into an array over iovec{}.
 *
 * @function set_msg_iov
 *
 * @param t                 LUA_TTABLE(LUA_TUSERDATA(luab_iovec_t)).
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
MsgHdr_set_msg_iov(lua_State *L)
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

    msg = luab_udata(L, 1, msghdr_type, struct msghdr *);
    iov = luab_newvector(L, 2, sizeof(struct iovec));

    msghdr_free_iov(msg);
    msg->msg_iov = iov;

    lua_pushnil(L);

    while (lua_next(L, 2) != 0) {

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
 *                          LUA_TTABLE(LUA_TUSERDATA(luab_iovec_t))
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
MsgHdr_get_msg_iov(lua_State *L)
{
    struct msghdr *msg;
    int status;

    (void)luab_checkmaxargs(L, 2);

    msg = luab_udata(L, 1, msghdr_type, struct msghdr *);

    (void)luab_checkltable(L, 2, 0);

    status = msghdr_populate_iovec(L, 2, msg, 0);

    return (luab_pusherr(L, status));
}

/***
 * Fetch this as instance of LUA_TTABLE.
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 * @usage t = msghdr:get()
 */
static int
MsgHdr_get(lua_State *L)
{
    struct msghdr *msg;

    (void)luab_checkmaxargs(L, 1);

    msg = luab_udata(L, 1, msghdr_type, struct msghdr *);

    lua_newtable(L);

    if (msg->msg_name != NULL) {
        luab_setudata(L, -2, &sockaddr_type, "msg_name", msg->msg_name);
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
 * Metamethods.
 */

static int
MsgHdr_gc(lua_State *L)
{
    luab_msghdr_t *self;
    luab_buf_t *buf;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_msghdr(L, 1);
    buf = self->mh_buf;

    while (buf->buf_data != NULL)
        luab_buf_free(buf++);

    (void)memset_s(self, msghdr_type.sz, 0, msghdr_type.sz);

    return (0);
}

static int
MsgHdr_tostring(lua_State *L)
{
    luab_msghdr_t *self;

    (void)luab_checkmaxargs(L, 1);

    self = luab_to_msghdr(L, 1);
    lua_pushfstring(L, "msghdr (%p)", self);

    return (1);
}

static luab_table_t msghdr_methods[] = {
    LUABSD_FUNC("msg_iovlen",  MsgHdr_msg_iovlen),
    LUABSD_FUNC("msg_flags",    MsgHdr_msg_flags),
/*  LUABSD_FUNC("msg_controllen",   MsgHdr_msg_controllen), */
    LUABSD_FUNC("set_msg_name", MsgHdr_set_msg_name),
    LUABSD_FUNC("set_msg_namelen",  MsgHdr_set_msg_namelen),
    LUABSD_FUNC("set_msg_iov",  MsgHdr_set_msg_iov),
/*  LUABSD_FUNC("set_msg_control",  MsgHdr_set_msg_control), */
    LUABSD_FUNC("set_msg_flags",  MsgHdr_set_msg_iov),
    LUABSD_FUNC("get",  MsgHdr_get),
    LUABSD_FUNC("get_msg_name", MsgHdr_get_msg_name),
    LUABSD_FUNC("get_msg_namelen",  MsgHdr_get_msg_namelen),
    LUABSD_FUNC("get_msg_iov",  MsgHdr_get_msg_iov),
/*  LUABSD_FUNC("get_msg_control",  MsgHdr_get_msg_control), */
    LUABSD_FUNC("get_msg_flags",  MsgHdr_get_msg_iov),
    LUABSD_FUNC("__gc", MsgHdr_gc),
    LUABSD_FUNC("__tostring",   MsgHdr_tostring),
    LUABSD_FUNC(NULL, NULL)
};

static void *
msghdr_create(lua_State *L, void *arg __unused)
{
    luab_buf_t buf[MH_MAX_BUF+1];
    luab_msghdr_t *self;
    int i;

    (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));

    for (i = 0; i < MH_MAX_BUF; i++) {
        if (luab_buf_alloc(&buf[i], MHLEN) != 0)
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
    luab_buf_t *src, *dst;

    if (((self = (luab_msghdr_t *)ud) != NULL) &&
        ((src = (luab_buf_t *)arg) != NULL)) {

        dst = self->mh_buf;

        while (src->buf_data != NULL) {
            dst->buf_len = src->buf_len;
            dst->buf_data = src->buf_data;

            dst++; src++;
        }
    }
}

static void *
msghdr_udata(lua_State *L, int narg)
{
    luab_msghdr_t *self = luab_to_msghdr(L, narg);

    return (&self->mh_msg);
}

luab_module_t msghdr_type = {
    .cookie = LUABSD_MSGHDR_TYPE_ID,
    .name = LUABSD_MSGHDR_TYPE,
    .vec = msghdr_methods,
    .ctor = msghdr_create,
    .init = msghdr_init,
    .get = msghdr_udata,
    .sz = sizeof(luab_msghdr_t),
};

/***
 * Ctor.
 *
 * @function StructMsgHdr
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_TSTRING ])
 *
 * @usage msghdr = bsd.sys.socket.StructMsgHdr()
 */
int
luab_StructMsgHdr(lua_State *L)
{
    int status;

    (void)luab_checkmaxargs(L, 0);

    if (msghdr_create(L, NULL) == NULL)
        status = luab_pushnil(L);
    else
        status = 1;

    return (status);
}
