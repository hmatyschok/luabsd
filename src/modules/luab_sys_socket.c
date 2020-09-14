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

#include <sys/socket.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

extern luab_module_t hook_type;
extern luab_module_t linger_type;
extern luab_module_t msghdr_type;
extern luab_module_t sockaddr_type;
extern luab_module_t timespec_type;

extern int luab_msghdr_create(lua_State *);

#define LUABSD_SYS_SOCKET_LIB_ID    1594740107
#define LUABSD_SYS_SOCKET_LIB_KEY   "socket"

extern luab_module_t luab_sys_socket_lib;

/*
 * Subr.
 *
 * Evaluates if set contains instances of (LUA_TTABLE(LUA_TUSERDATA(MSGHDR)))
 * and translate its elements or items this into an array of mmsghdr{} items.
 */
static struct mmsghdr *
luab_checkmsgvec(lua_State *L, int narg)
{
    struct mmsghdr *vec;
    struct msghdr *msg;
    int k;

    vec = luab_newvector(L, narg, sizeof(struct mmsghdr));

    lua_pushnil(L);

    for (k = 0; lua_next(L, narg) != 0; k++) {

        if ((lua_isnumber(L, -2) != 0) &&
            (lua_isnumber(L, -1) != 0)) {
            msg = luab_udata(L, -1, msghdr_type, struct msghdr *);
            (void)memmove(&(vec[k].msg_hdr), msg, sizeof(struct msghdr));
        } else {
            free(vec);
            luaL_argerror(L, narg, "Invalid argument");
        }
        lua_pop(L, 1);
    }
    return (vec);
}

/*
 * Service primitives.
 */

/***
 * accept(2) - accept a connection on a socket(9)
 *
 * @function accept
 *
 * @param s                 Socket bound to an adress by bind(2).
 * @param addr              Optional result argument maps to address of its
 *                          peer, otherwise it will be set to nil.
 * @param addrlen           Optional value-result argument, otherwise it will
 *                          be set to nil.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (as [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage as [, err, msg ] = bsd.sys.socket.accept(s, addr, addrlen)
 */
static int
luab_accept(lua_State *L)
{
    int s;
    struct sockaddr *addr;
    luab_type_u *hook;
    socklen_t *addrlen;
    int as;

    (void)luab_checkmaxargs(L, 3);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    addr = luab_udataisnil(L, 2, sockaddr_type, struct sockaddr *);
    hook = luab_udataisnil(L, 3, hook_type, luab_type_u *);

    if (hook != NULL)
        addrlen = &(hook->un_socklen);
    else
        addrlen = NULL;

    as = accept(s, addr, addrlen);

    return (luab_pusherr(L, as));
}

/***
 * bind(2) - assign a local protocol address to a socket(9)
 *
 * @function bind
 *
 * @param s                 By socket(2) instantiated socket(9).
 * @param addr              Local protocol address.
 * @param addrlen           Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.socket.bind(s, addr, addrlen)
 */
static int
luab_bind(lua_State *L)
{
    int s;
    struct sockaddr *addr;
    socklen_t addrlen;
    int status;

    (void)luab_checkmaxargs(L, 3);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    addr = luab_udata(L, 2, sockaddr_type, struct sockaddr *);
    addrlen = (socklen_t)luab_checkinteger(L, 3, INT_MAX);

    status = bind(s, addr, addrlen);

    return (luab_pusherr(L, status));
}

/***
 * connect(2) - initiate a connection on a socket(9)
 *
 * @function connect
 *
 * @param s                 By socket(2) instantiated socket(9).
 * @param name              Protocol address of its peer.
 * @param namelen           Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.socket.connect(s, name, namelen)
 */
static int
luab_connect(lua_State *L)
{
    int s;
    struct sockaddr *name;
    socklen_t namelen;
    int status;

    (void)luab_checkmaxargs(L, 3);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    name = luab_udata(L, 2, sockaddr_type, struct sockaddr *);
    namelen = (socklen_t)luab_checkinteger(L, 3, INT_MAX);

    status = connect(s, name, namelen);

    return (luab_pusherr(L, status));
}

/***
 * socket(2) - create an endpoint for communication
 *
 * @function socket
 *
 * @param domain            Specifies communication domain(9), where Inter
 *                          Process Communication (IPC) takes place.
 * @param type              Specifies semantics of communication for IPC.
 * @param protocol          Specifies used for IPC by socket(9) utilized
 *                          particular Protocol.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (s [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage s [, err, msg ] = bsd.sys.socket.socket(domain, type, protocol)
 */
static int
luab_socket(lua_State *L)
{
    int domain;
    int type;
    int protocol;
    int s;

    (void)luab_checkmaxargs(L, 3);

    domain = (int)luab_checkinteger(L, 1, INT_MAX);
    type = (int)luab_checkinteger(L, 2, INT_MAX);
    protocol = (int)luab_checkinteger(L, 3, INT_MAX);

    s = socket(domain, type, protocol);

    return (luab_pusherr(L, s));
}

#if __BSD_VISIBLE
/***
 * accept4(2) - accept a connection on a socket(9)
 *
 * @function accept
 *
 * @param s                 Socket bound to an adress by bind(2).
 * @param addr              Optional result argument maps to address of its
 *                          peer, otherwise it will be set to nil.
 * @param addrlen           Optional value-result argument, otherwise it will
 *                          be set to nil.
 * @param flags             See accept4(2) for further details.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (as [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage as [, err, msg ] = bsd.sys.socket.accept4(s, addr, addrlen, flags)
 */
static int
luab_accept4(lua_State *L)
{
    int s;
    struct sockaddr *addr;
    luab_type_u *hook;
    socklen_t *addrlen;
    int flags, as;

    (void)luab_checkmaxargs(L, 4);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    addr = luab_udataisnil(L, 2, sockaddr_type, struct sockaddr *);
    hook = luab_udataisnil(L, 3, hook_type, luab_type_u *);
    flags = (int)luab_checkinteger(L, 4, INT_MAX);

    if (hook != NULL)
        addrlen = &(hook->un_socklen);
    else
        addrlen = NULL;

    as = accept4(s, addr, addrlen, flags);

    return (luab_pusherr(L, as));
}

/***
 * bindat(2) - assign a local protocol address to a socket(9)
 *
 * @function bindat
 *
 * @param fd                Specifies behaviour like call of bind(2), is
 *
 *                              bsd.fcntl.AT_FDCWD
 *
 *                          is used as argument.
 * @param s                 By socket(2) instantiated socket(9).
 * @param addr              Local protocol address.
 * @param addrlen           Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.socket.bindat(fd, s, addr, addrlen)
 */
static int
luab_bindat(lua_State *L)
{
    int fd, s;
    struct sockaddr *addr;
    socklen_t addrlen;
    int status;

    (void)luab_checkmaxargs(L, 4);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    s = (int)luab_checkinteger(L, 2, INT_MAX);
    addr = luab_udata(L, 3, sockaddr_type, struct sockaddr *);
    addrlen = (socklen_t)luab_checkinteger(L, 4, INT_MAX);

    status = bindat(fd, s, addr, addrlen);

    return (luab_pusherr(L, status));
}

/***
 * connectat(2) - initiate a connectation on a socket(9)
 *
 * @function connectat
 *
 * @param fd                Specifies behaviour like call of bind(2), is
 *
 *                              bsd.fcntl.AT_FDCWD
 *
 *                          is used as argument.
 * @param s                 By socket(2) instantiated socket(9).
 * @param name              Protocol address of its peer.
 * @param namelen           Self-explanatory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.socket.connectat(fd s, name, namelen)
 */
static int
luab_connectat(lua_State *L)
{
    int fd, s;
    struct sockaddr *name;
    socklen_t namelen;
    int status;

    (void)luab_checkmaxargs(L, 4);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    s = (int)luab_checkinteger(L, 2, INT_MAX);
    name = luab_udata(L, 3, sockaddr_type, struct sockaddr *);
    namelen = (socklen_t)luab_checkinteger(L, 4, INT_MAX);

    status = connectat(fd, s, name, namelen);

    return (luab_pusherr(L, status));
}
#endif  /* __BSD_VISIBLE */

/***
 * getpeername(2) - get peer name
 *
 * @function getpeername
 *
 * @param s                 Socket bound to an adress by bind(2).
 * @param name              Result argument, (LUA_TUSERDATA(SOCKADDR)).
 * @param namelen           Value-result argument, (LUA_TUSERDATA(HOOK)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage as [, err, msg ] = bsd.sys.socket.getpeername(s, name, namelen)
 */
static int
luab_getpeername(lua_State *L)
{
    int s;
    struct sockaddr *name;
    luab_type_u *hook;
    socklen_t *namelen;
    int status;

    (void)luab_checkmaxargs(L, 3);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    name = luab_udata(L, 2, sockaddr_type, struct sockaddr *);
    hook = luab_udata(L, 3, hook_type, luab_type_u *);
    namelen = &(hook->un_socklen);

    status = getpeername(s, name, namelen);

    return (luab_pusherr(L, status));
}

/***
 * getsockname(2) - get socket(9) name
 *
 * @function getsockname
 *
 * @param s                 Socket bound to an adress by bind(2).
 * @param name              Result argument, (LUA_TUSERDATA(SOCKADDR)).
 * @param namelen           Value-result argument, (LUA_TUSERDATA(HOOK)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage as [, err, msg ] = bsd.sys.socket.getsockname(s, name, namelen)
 */
static int
luab_getsockname(lua_State *L)
{
    int s;
    struct sockaddr *name;
    luab_type_u *hook;
    socklen_t *namelen;
    int status;

    (void)luab_checkmaxargs(L, 3);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    name = luab_udata(L, 2, sockaddr_type, struct sockaddr *);
    hook = luab_udata(L, 3, hook_type, luab_type_u *);
    namelen = &(hook->un_socklen);

    status = getsockname(s, name, namelen);

    return (luab_pusherr(L, status));
}

/***
 * listen(2) - listen for connections on a socket(9)
 *
 * @function listen
 *
 * @param s                 Specifies socket(9).
 * @param backlog           Specifies backlog.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (0 [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage ret [, err, msg ] = bsd.sys.listen.listen(s, backlog)
 */
static int
luab_listen(lua_State *L)
{
    int s, backlog;
    int status;

    (void)luab_checkmaxargs(L, 2);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    backlog = (int)luab_checkinteger(L, 2, INT_MAX);

    status = listen(s, backlog);

    return (luab_pusherr(L, status));
}

/***
 * recv(2) - receive message(s) from a socket(9)
 *
 * @function recv
 *
 * @param s                 Open socket(9).
 * @param buf               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param len               Assumed number of bytes to be recv'd.
 * @param flags             Flags argument over
 *
 *                              bsd.sys.socket.MSG_{OOB,PEEK,WAITALL,
 *                                  DONTWAIT,CMSG_CLOEXEC}
 *
 *                          may combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (count [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage count [, err, msg ] = bsd.sys.socket.recv(s, buf, len, flags)
 */
static int
luab_recv(lua_State *L)
{
    int s;
    luab_iovec_t *buf;
    size_t len;
    int flags;

    (void)luab_checkmaxargs(L, 3);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    buf = luab_udata(L, 2, iovec_type, luab_iovec_t *);
    len = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    flags = (int)luab_checkinteger(L, 4, INT_MAX);

    return (luab_iovec_recv(L, s, buf, &len, flags));
}

/***
 * recvfrom(2) - receive message(s) from a socket(9)
 *
 * @function recvfrom
 *
 * @param s                 Open socket(9).
 * @param buf               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param len               Assumed number of bytes to be recvfrom'd.
 * @param flags             Flags argument over
 *
 *                              bsd.sys.socket.MSG_{OOB,PEEK,WAITALL,
 *                                  DONTWAIT,CMSG_CLOEXEC}
 *
 *                          may combined by inclusive or.
 * @param from              Result argument, (LUA_TUSERDATA(SOCKADDR)).
 * @param fromlen           Value-result argument, (LUA_TUSERDATA(HOOK)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (count [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage count [, err, msg ] = bsd.sys.socket.recvfrom(s, buf, len, flags)
 */
static int
luab_recvfrom(lua_State *L)
{
    int s;
    luab_iovec_t *buf;
    size_t len;
    int flags;
    struct sockaddr *from;
    luab_type_u *hook;
    socklen_t *fromlen;

    (void)luab_checkmaxargs(L, 6);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    buf = luab_udata(L, 2, iovec_type, luab_iovec_t *);
    len = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    flags = (int)luab_checkinteger(L, 4, INT_MAX);
    from = luab_udataisnil(L, 5, sockaddr_type, struct sockaddr *);
    hook = luab_udata(L, 6, hook_type, luab_type_u *);
    fromlen = &(hook->un_socklen);

    return (luab_iovec_recvfrom(L, s, buf, &len, flags, from, fromlen));
}

/***
 * recvmsg(2) - receive message(s) from a socket(9)
 *
 * @function recvmsg
 *
 * @param s                 File drscriptor denotes by socket(2) opened socket(9).
 * @param msg               Instance of LUA_TUSERDATA(MSGHDR).
 * @param flags             Flags argument over
 *
 *                              bsd.sys.socket.MSG_{OOB,PEEK,WAITALL,
 *                                  DONTWAIT,CMSG_CLOEXEC}
 *
 *                          may combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (count [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage count [, err, msg ] = bsd.sys.socket.recvmsg(s, msg, flags)
 */
static int
luab_recvmsg(lua_State *L)
{
    int s;
    struct msghdr *msg;
    int flags;
    ssize_t count;

    (void)luab_checkmaxargs(L, 3);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    msg = luab_udata(L, 2, msghdr_type, struct msghdr *);
    flags = (int)luab_checkinteger(L, 3, INT_MAX);

    if ((msg->msg_iov != NULL) &&
        (msg->msg_iovlen > 0))
        count = recvmsg(s, msg, flags);
    else {
        errno = ENXIO;
        count = -1;
    }
    return (luab_pusherr(L, count));
}

#if __BSD_VISIBLE
/***
 * recvmmsg(2) - receive multiple message(s) at a call from a socket(9)
 *
 * @function recvmmsg
 *
 * @param s                 File drscriptor denotes by socket(2) opened socket(9).
 * @param msgvec            Instance of LUA_TTABLE(LUA_TUSERDATA(MSGHDR)).
 * @param vlen              Constraint for #n received messages.
 * @param flags             Flags argument over
 *
 *                              bsd.sys.socket.MSG_{OOB,PEEK,WAITALL,
 *                                  DONTWAIT,CMSG_CLOEXEC,WAITFORONE}
 *
 *                          may combined by inclusive or.
 * @param timeout           Specifies timeout, if !nil.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (count [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage count [, err, msg ] = bsd.sys.socket.recvmmsg(s, msgvec, vlen, flags, timeout)
 */
static int
luab_recvmmsg(lua_State *L)
{
    int s;                              /* XXX */
    struct mmsghdr *msgvec;
    size_t vlen;
    int flags;
    struct timespec *timeout;
    ssize_t count;

    (void)luab_checkmaxargs(L, 5);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    msgvec = luab_checkmsgvec(L, 2);
    vlen = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    flags = (int)luab_checkinteger(L, 4, INT_MAX);
    timeout = luab_udataisnil(L, 5, timespec_type, struct timespec *);

    count = recvmmsg(s, msgvec, vlen, flags, timeout);

    free(msgvec);

    return (luab_pusherr(L, count));
}
#endif

/***
 * send(2) - send message(s) from a socket(9)
 *
 * @function send
 *
 * @param s                 Open socket(9).
 * @param msg               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param len               Assumed number of bytes to be send'd.
 * @param flags             Flags argument over
 *
 *                              bsd.sys.socket.MSG_{OOB,PEEK,WAITALL,
 *                                  DONTWAIT,CMSG_CLOEXEC}
 *
 *                          may combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (count [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage count [, err, msg ] = bsd.sys.socket.send(s, msg, len, flags)
 */
static int
luab_send(lua_State *L)
{
    int s;
    luab_iovec_t *msg;
    size_t len;
    int flags;

    (void)luab_checkmaxargs(L, 3);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    msg = luab_udata(L, 2, iovec_type, luab_iovec_t *);
    len = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    flags = (int)luab_checkinteger(L, 4, INT_MAX);

    return (luab_iovec_send(L, s, msg, &len, flags));
}

/***
 * sendmsg(2) - send message(s) from a socket(9)
 *
 * @function sendmsg
 *
 * @param s                 File drscriptor denotes by socket(2) opened socket(9).
 * @param msg               Instance of LUA_TUSERDATA(MSGHDR).
 * @param flags             Flags argument over
 *
 *                              bsd.sys.socket.MSG_{OOB,DONTROUTE,EOR,
 *                                  DONTWAIT,EOF,NOSIGNAL}
 *
 *                          may combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (count [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage count [, err, msg ] = bsd.sys.socket.sendmsg(s, msg, flags)
 */
static int
luab_sendmsg(lua_State *L)
{
    int s;
    struct msghdr *msg;
    int flags;
    ssize_t count;

    (void)luab_checkmaxargs(L, 3);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    msg = luab_udata(L, 2, msghdr_type, struct msghdr *);
    flags = (int)luab_checkinteger(L, 3, INT_MAX);

    if ((msg->msg_iov != NULL) &&
        (msg->msg_iovlen > 0))
        count = sendmsg(s, msg, flags);
    else {
        errno = ENXIO;
        count = -1;
    }
    return (luab_pusherr(L, count));
}

#if __BSD_VISIBLE
/***
 * sendmmsg(2) - send multiple message(s) at a call from a socket(9)
 *
 * @function sendmmsg
 *
 * @param s                 File drscriptor denotes by socket(2) opened socket(9).
 * @param msgvec            Instance of LUA_TTABLE(LUA_TUSERDATA(MSGHDR)).
 * @param vlen              Constraint for transmission of #n messages.
 * @param flags             Flags argument over
 *
 *                              bsd.sys.socket.MSG_{OOB,DONTROUTE,EOR,
 *                                  DONTWAIT,EOF,NOSIGNAL}
 *
 *                          may combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (count [, nil, nil]) on success or
 *          (-1, (errno, strerror(errno)))
 *
 * @usage count [, err, msg ] = bsd.sys.socket.sendmmsg(s, msgvec, vlen, flags)
 */
static int
luab_sendmmsg(lua_State *L)
{
    int s;
    struct mmsghdr *msgvec;
    size_t vlen;
    int flags;
    ssize_t count;

    (void)luab_checkmaxargs(L, 4);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    msgvec = luab_checkmsgvec(L, 2);
    vlen = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    flags = (int)luab_checkinteger(L, 4, INT_MAX);

    count = sendmmsg(s, msgvec, vlen, flags);

    free(msgvec);

    return (luab_pusherr(L, count));
}
#endif

/*
 * Generator functions.
 */

/***
 * Generator function.
 *
 * @function linger_create
 *
 * @param data          (LUA_T{NIL,USERDATA(linger)}), optional.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (linger [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage linger [, err, msg ] = bsd.sys.socket.linger_create([ data ])
 */
static int
luab_linger_create(lua_State *L)
{
    struct linger *data;
    int narg;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        data = NULL;
    else
        data = luab_udata(L, narg, linger_type, struct linger *);

    return (luab_pushudata(L, &linger_type, data));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(SOCKADDR)).
 *
 * @function sockaddr_create
 *
 * @param data          Instance of (LUA_TUSERDATA(SOCKADDR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          (sockaddr [, nil, nil]) on success or
 *          (nil, (errno, strerror(errno)))
 *
 * @usage sockaddr [, err, msg ] = bsd.sys.socket.sockaddr_create([ data ])
 */
static int
luab_sockaddr_create(lua_State *L)
{
    struct sockaddr *data;
    int narg;

    if ((narg = luab_checkmaxargs(L, 1)) == 0)
        data = NULL;
    else
        data = luab_udata(L, narg, sockaddr_type, struct sockaddr *);

    return (luab_pushudata(L, &sockaddr_type, data));
}

/*
 * Interface against <sys/socket.h>.
 */

static luab_table_t luab_sys_socket_vec[] = {
#if __BSD_VISIBLE
    LUABSD_INT("SOCK_MAXADDRLEN",   SOCK_MAXADDRLEN),
#endif
    LUABSD_INT("SOCK_STREAM",   SOCK_STREAM),
    LUABSD_INT("SOCK_DGRAM",    SOCK_DGRAM),
    LUABSD_INT("SOCK_RAW",  SOCK_RAW),
#if __BSD_VISIBLE
    LUABSD_INT("SOCK_RDM",  SOCK_RDM),
#endif
    LUABSD_INT("SOCK_SEQPACKET",    SOCK_SEQPACKET),
#if __BSD_VISIBLE
    LUABSD_INT("SOCK_CLOEXEC",  SOCK_CLOEXEC),
    LUABSD_INT("SOCK_NONBLOCK", SOCK_NONBLOCK),
#endif
    LUABSD_INT("SO_DEBUG",  SO_DEBUG),
    LUABSD_INT("SO_ACCEPTCONN", SO_ACCEPTCONN),
    LUABSD_INT("SO_REUSEADDR",  SO_REUSEADDR),
    LUABSD_INT("SO_KEEPALIVE",  SO_KEEPALIVE),
    LUABSD_INT("SO_DONTROUTE",  SO_DONTROUTE),
    LUABSD_INT("SO_BROADCAST",  SO_BROADCAST),
#if __BSD_VISIBLE
    LUABSD_INT("SO_USELOOPBACK",    SO_USELOOPBACK),
#endif
    LUABSD_INT("SO_LINGER", SO_LINGER),
    LUABSD_INT("SO_OOBINLINE",  SO_OOBINLINE),
#if __BSD_VISIBLE
    LUABSD_INT("SO_REUSEPORT",  SO_REUSEPORT),
    LUABSD_INT("SO_TIMESTAMP",  SO_TIMESTAMP),
    LUABSD_INT("SO_NOSIGPIPE",  SO_NOSIGPIPE),
    LUABSD_INT("SO_ACCEPTFILTER",   SO_ACCEPTFILTER),
    LUABSD_INT("SO_BINTIME",    SO_BINTIME),
#endif
    LUABSD_INT("SO_NO_OFFLOAD", SO_NO_OFFLOAD),
    LUABSD_INT("SO_NO_DDP", SO_NO_DDP),
    LUABSD_INT("SO_REUSEPORT_LB",   SO_REUSEPORT_LB),
    LUABSD_INT("SO_SNDBUF", SO_SNDBUF),
    LUABSD_INT("SO_RCVBUF", SO_RCVBUF),
    LUABSD_INT("SO_SNDLOWAT",   SO_SNDLOWAT),
    LUABSD_INT("SO_RCVLOWAT",   SO_RCVLOWAT),
    LUABSD_INT("SO_SNDTIMEO",   SO_SNDTIMEO),
    LUABSD_INT("SO_RCVTIMEO",   SO_RCVTIMEO),
    LUABSD_INT("SO_ERROR",  SO_ERROR),
    LUABSD_INT("SO_TYPE",   SO_TYPE),
#if __BSD_VISIBLE
    LUABSD_INT("SO_LABEL",  SO_LABEL),
    LUABSD_INT("SO_PEERLABEL",  SO_PEERLABEL),
    LUABSD_INT("SO_LISTENQLIMIT",   SO_LISTENQLIMIT),
    LUABSD_INT("SO_LISTENQLEN", SO_LISTENQLEN),
    LUABSD_INT("SO_LISTENINCQLEN",  SO_LISTENINCQLEN),
    LUABSD_INT("SO_SETFIB", SO_SETFIB),
    LUABSD_INT("SO_USER_COOKIE",    SO_USER_COOKIE),
    LUABSD_INT("SO_PROTOCOL",   SO_PROTOCOL),
    LUABSD_INT("SO_PROTOTYPE",  SO_PROTOTYPE),
    LUABSD_INT("SO_TS_CLOCK",   SO_TS_CLOCK),
    LUABSD_INT("SO_MAX_PACING_RATE",    SO_MAX_PACING_RATE),
    LUABSD_INT("SO_DOMAIN", SO_DOMAIN),
    LUABSD_INT("SO_TS_REALTIME_MICRO",  SO_TS_REALTIME_MICRO),
    LUABSD_INT("SO_TS_BINTIME", SO_TS_BINTIME),
    LUABSD_INT("SO_TS_REALTIME",    SO_TS_REALTIME),
    LUABSD_INT("SO_TS_MONOTONIC",   SO_TS_MONOTONIC),
    LUABSD_INT("SO_TS_DEFAULT", SO_TS_DEFAULT),
    LUABSD_INT("SO_TS_CLOCK_MAX",   SO_TS_CLOCK_MAX),
    LUABSD_INT("SO_VENDOR", SO_VENDOR),
#endif
    LUABSD_INT("SOL_SOCKET",    SOL_SOCKET),
    LUABSD_INT("AF_UNSPEC", AF_UNSPEC),
#if __BSD_VISIBLE
    LUABSD_INT("AF_LOCAL",  AF_LOCAL),
#endif
    LUABSD_INT("AF_UNIX",   AF_UNIX),
    LUABSD_INT("AF_INET",   AF_INET),
#if __BSD_VISIBLE
    LUABSD_INT("AF_IMPLINK",    AF_IMPLINK),
    LUABSD_INT("AF_PUP",    AF_PUP),
    LUABSD_INT("AF_CHAOS",  AF_CHAOS),
    LUABSD_INT("AF_NETBIOS",    AF_NETBIOS),
    LUABSD_INT("AF_ISO",    AF_ISO),
    LUABSD_INT("AF_OSI",    AF_OSI),
    LUABSD_INT("AF_ECMA",   AF_ECMA),
    LUABSD_INT("AF_DATAKIT",    AF_DATAKIT),
    LUABSD_INT("AF_CCITT",  AF_CCITT),
    LUABSD_INT("AF_SNA",    AF_SNA),
    LUABSD_INT("AF_DECnet", AF_DECnet),
    LUABSD_INT("AF_DLI",    AF_DLI),
    LUABSD_INT("AF_LAT",    AF_LAT),
    LUABSD_INT("AF_HYLINK", AF_HYLINK),
    LUABSD_INT("AF_APPLETALK",  AF_APPLETALK),
    LUABSD_INT("AF_ROUTE",  AF_ROUTE),
    LUABSD_INT("AF_LINK",   AF_LINK),
    LUABSD_INT("pseudo_AF_XTP", pseudo_AF_XTP),
    LUABSD_INT("AF_COIP",   AF_COIP),
    LUABSD_INT("AF_CNT",    AF_CNT),
    LUABSD_INT("pseudo_AF_RTIP",    pseudo_AF_RTIP),
    LUABSD_INT("AF_IPX",    AF_IPX),
    LUABSD_INT("AF_SIP",    AF_SIP),
    LUABSD_INT("pseudo_AF_PIP", pseudo_AF_PIP),
    LUABSD_INT("AF_ISDN",   AF_ISDN),
    LUABSD_INT("AF_E164",   AF_E164),
    LUABSD_INT("pseudo_AF_KEY", pseudo_AF_KEY),
#endif
    LUABSD_INT("AF_INET6",  AF_INET6),
#if __BSD_VISIBLE
    LUABSD_INT("AF_NATM",   AF_NATM),
    LUABSD_INT("AF_ATM",    AF_ATM),
    LUABSD_INT("pseudo_AF_HDRCMPLT",    pseudo_AF_HDRCMPLT),
    LUABSD_INT("AF_NETGRAPH",   AF_NETGRAPH),
    LUABSD_INT("AF_SLOW",   AF_SLOW),
    LUABSD_INT("AF_SCLUSTER",   AF_SCLUSTER),
    LUABSD_INT("AF_ARP",    AF_ARP),
    LUABSD_INT("AF_BLUETOOTH",  AF_BLUETOOTH),
    LUABSD_INT("AF_IEEE80211",  AF_IEEE80211),
    LUABSD_INT("AF_INET_SDP",   AF_INET_SDP),
    LUABSD_INT("AF_INET6_SDP",  AF_INET6_SDP),
    LUABSD_INT("AF_MAX",    AF_MAX),
    LUABSD_INT("AF_VENDOR00",   AF_VENDOR00),
    LUABSD_INT("AF_VENDOR01",   AF_VENDOR01),
    LUABSD_INT("AF_VENDOR02",   AF_VENDOR02),
    LUABSD_INT("AF_VENDOR03",   AF_VENDOR03),
    LUABSD_INT("AF_VENDOR04",   AF_VENDOR04),
    LUABSD_INT("AF_VENDOR05",   AF_VENDOR05),
    LUABSD_INT("AF_VENDOR06",   AF_VENDOR06),
    LUABSD_INT("AF_VENDOR07",   AF_VENDOR07),
    LUABSD_INT("AF_VENDOR08",   AF_VENDOR08),
    LUABSD_INT("AF_VENDOR09",   AF_VENDOR09),
    LUABSD_INT("AF_VENDOR10",   AF_VENDOR10),
    LUABSD_INT("AF_VENDOR11",   AF_VENDOR11),
    LUABSD_INT("AF_VENDOR12",   AF_VENDOR12),
    LUABSD_INT("AF_VENDOR13",   AF_VENDOR13),
    LUABSD_INT("AF_VENDOR14",   AF_VENDOR14),
    LUABSD_INT("AF_VENDOR15",   AF_VENDOR15),
    LUABSD_INT("AF_VENDOR16",   AF_VENDOR16),
    LUABSD_INT("AF_VENDOR17",   AF_VENDOR17),
    LUABSD_INT("AF_VENDOR18",   AF_VENDOR18),
    LUABSD_INT("AF_VENDOR19",   AF_VENDOR19),
    LUABSD_INT("AF_VENDOR20",   AF_VENDOR20),
    LUABSD_INT("AF_VENDOR21",   AF_VENDOR21),
    LUABSD_INT("AF_VENDOR22",   AF_VENDOR22),
    LUABSD_INT("AF_VENDOR23",   AF_VENDOR23),
    LUABSD_INT("AF_VENDOR24",   AF_VENDOR24),
    LUABSD_INT("AF_VENDOR25",   AF_VENDOR25),
    LUABSD_INT("AF_VENDOR26",   AF_VENDOR26),
    LUABSD_INT("AF_VENDOR27",   AF_VENDOR27),
    LUABSD_INT("AF_VENDOR28",   AF_VENDOR28),
    LUABSD_INT("AF_VENDOR29",   AF_VENDOR29),
    LUABSD_INT("AF_VENDOR30",   AF_VENDOR30),
    LUABSD_INT("AF_VENDOR31",   AF_VENDOR31),
    LUABSD_INT("AF_VENDOR32",   AF_VENDOR32),
    LUABSD_INT("AF_VENDOR33",   AF_VENDOR33),
    LUABSD_INT("AF_VENDOR34",   AF_VENDOR34),
    LUABSD_INT("AF_VENDOR35",   AF_VENDOR35),
    LUABSD_INT("AF_VENDOR36",   AF_VENDOR36),
    LUABSD_INT("AF_VENDOR37",   AF_VENDOR37),
    LUABSD_INT("AF_VENDOR38",   AF_VENDOR38),
    LUABSD_INT("AF_VENDOR39",   AF_VENDOR39),
    LUABSD_INT("AF_VENDOR40",   AF_VENDOR40),
    LUABSD_INT("AF_VENDOR41",   AF_VENDOR41),
    LUABSD_INT("AF_VENDOR42",   AF_VENDOR42),
    LUABSD_INT("AF_VENDOR43",   AF_VENDOR43),
    LUABSD_INT("AF_VENDOR44",   AF_VENDOR44),
    LUABSD_INT("AF_VENDOR45",   AF_VENDOR45),
    LUABSD_INT("AF_VENDOR46",   AF_VENDOR46),
    LUABSD_INT("AF_VENDOR47",   AF_VENDOR47),
    LUABSD_INT("SOCK_MAXADDRLEN",   SOCK_MAXADDRLEN),
    LUABSD_INT("PF_UNSPEC", PF_UNSPEC),
    LUABSD_INT("PF_LOCAL",  PF_LOCAL),
    LUABSD_INT("PF_UNIX",   PF_UNIX),
    LUABSD_INT("PF_INET",   PF_INET),
    LUABSD_INT("PF_IMPLINK",    PF_IMPLINK),
    LUABSD_INT("PF_PUP",    PF_PUP),
    LUABSD_INT("PF_CHAOS",  PF_CHAOS),
    LUABSD_INT("PF_NETBIOS",    PF_NETBIOS),
    LUABSD_INT("PF_ISO",    PF_ISO),
    LUABSD_INT("PF_OSI",    PF_OSI),
    LUABSD_INT("PF_ECMA",   PF_ECMA),
    LUABSD_INT("PF_DATAKIT",    PF_DATAKIT),
    LUABSD_INT("PF_CCITT",  PF_CCITT),
    LUABSD_INT("PF_SNA",    PF_SNA),
    LUABSD_INT("PF_DECnet", PF_DECnet),
    LUABSD_INT("PF_DLI",    PF_DLI),
    LUABSD_INT("PF_LAT",    PF_LAT),
    LUABSD_INT("PF_HYLINK", PF_HYLINK),
    LUABSD_INT("PF_APPLETALK",  PF_APPLETALK),
    LUABSD_INT("PF_ROUTE",  PF_ROUTE),
    LUABSD_INT("PF_LINK",   PF_LINK),
    LUABSD_INT("PF_XTP",    PF_XTP),
    LUABSD_INT("PF_COIP",   PF_COIP),
    LUABSD_INT("PF_CNT",    PF_CNT),
    LUABSD_INT("PF_SIP",    PF_SIP),
    LUABSD_INT("PF_IPX",    PF_IPX),
    LUABSD_INT("PF_RTIP",   PF_RTIP),
    LUABSD_INT("PF_PIP",    PF_PIP),
    LUABSD_INT("PF_ISDN",   PF_ISDN),
    LUABSD_INT("PF_KEY",    PF_KEY),
    LUABSD_INT("PF_INET6",  PF_INET6),
    LUABSD_INT("PF_NATM",   PF_NATM),
    LUABSD_INT("PF_ATM",    PF_ATM),
    LUABSD_INT("PF_NETGRAPH",   PF_NETGRAPH),
    LUABSD_INT("PF_SLOW",   PF_SLOW),
    LUABSD_INT("PF_SCLUSTER",   PF_SCLUSTER),
    LUABSD_INT("PF_ARP",    PF_ARP),
    LUABSD_INT("PF_BLUETOOTH",  PF_BLUETOOTH),
    LUABSD_INT("PF_IEEE80211",  PF_IEEE80211),
    LUABSD_INT("PF_INET_SDP",   PF_INET_SDP),
    LUABSD_INT("PF_INET6_SDP",  PF_INET6_SDP),
    LUABSD_INT("PF_MAX",    PF_MAX),
    LUABSD_INT("NET_RT_DUMP",   NET_RT_DUMP),
    LUABSD_INT("NET_RT_FLAGS",  NET_RT_FLAGS),
    LUABSD_INT("NET_RT_IFLIST", NET_RT_IFLIST),
    LUABSD_INT("NET_RT_IFMALIST",   NET_RT_IFMALIST),
    LUABSD_INT("NET_RT_IFLISTL",    NET_RT_IFLISTL),
#endif
    LUABSD_INT("SOMAXCONN", SOMAXCONN),
    LUABSD_INT("MSG_OOB",   MSG_OOB),
    LUABSD_INT("MSG_PEEK",  MSG_PEEK),
    LUABSD_INT("MSG_DONTROUTE", MSG_DONTROUTE),
    LUABSD_INT("MSG_EOR",   MSG_EOR),
    LUABSD_INT("MSG_TRUNC", MSG_TRUNC),
    LUABSD_INT("MSG_CTRUNC",    MSG_CTRUNC),
    LUABSD_INT("MSG_WAITALL",   MSG_WAITALL),
#if __BSD_VISIBLE
    LUABSD_INT("MSG_DONTWAIT",  MSG_DONTWAIT),
    LUABSD_INT("MSG_EOF",   MSG_EOF),
    LUABSD_INT("MSG_NOTIFICATION",  MSG_NOTIFICATION),
    LUABSD_INT("MSG_NBIO",  MSG_NBIO),
    LUABSD_INT("MSG_COMPAT",    MSG_COMPAT),
#endif
#if __POSIX_VISIBLE >= 200809
    LUABSD_INT("MSG_NOSIGNAL",  MSG_NOSIGNAL),
#endif
#if __BSD_VISIBLE
    LUABSD_INT("MSG_CMSG_CLOEXEC",  MSG_CMSG_CLOEXEC),
    LUABSD_INT("MSG_WAITFORONE",    MSG_WAITFORONE),
    LUABSD_INT("CMGROUP_MAX",   CMGROUP_MAX),
#endif
    LUABSD_INT("SCM_RIGHTS",    SCM_RIGHTS),
#if __BSD_VISIBLE
    LUABSD_INT("SCM_TIMESTAMP", SCM_TIMESTAMP),
    LUABSD_INT("SCM_CREDS", SCM_CREDS),
    LUABSD_INT("SCM_BINTIME",   SCM_BINTIME),
    LUABSD_INT("SCM_REALTIME",  SCM_REALTIME),
    LUABSD_INT("SCM_MONOTONIC", SCM_MONOTONIC),
    LUABSD_INT("SCM_TIME_INFO", SCM_TIME_INFO),
    LUABSD_INT("ST_INFO_HW",    ST_INFO_HW),
    LUABSD_INT("ST_INFO_HW_HPREC",  ST_INFO_HW_HPREC),
#endif
    LUABSD_INT("SHUT_RD",   SHUT_RD),
    LUABSD_INT("SHUT_WR",   SHUT_WR),
    LUABSD_INT("SHUT_RDWR", SHUT_RDWR),
#if __BSD_VISIBLE
    LUABSD_INT("PRU_FLUSH_RD",  PRU_FLUSH_RD),
    LUABSD_INT("PRU_FLUSH_WR",  PRU_FLUSH_WR),
    LUABSD_INT("PRU_FLUSH_RDWR",    PRU_FLUSH_RDWR),
    LUABSD_INT("SF_NODISKIO",   SF_NODISKIO),
    LUABSD_INT("SF_MNOWAIT",    SF_MNOWAIT),
    LUABSD_INT("SF_SYNC",   SF_SYNC),
    LUABSD_INT("SF_USER_READAHEAD", SF_USER_READAHEAD),
    LUABSD_INT("SF_NOCACHE",    SF_NOCACHE),
#endif
    LUABSD_FUNC("accept",   luab_accept),
    LUABSD_FUNC("bind", luab_bind),
    LUABSD_FUNC("connect",  luab_connect),
    LUABSD_FUNC("socket",   luab_socket),
#if __BSD_VISIBLE
    LUABSD_FUNC("accept4",   luab_accept4),
    LUABSD_FUNC("bindat",   luab_bindat),
    LUABSD_FUNC("connectat",    luab_connectat),
#endif
    LUABSD_FUNC("getpeername",  luab_getpeername),
    LUABSD_FUNC("getsockname",  luab_getsockname),
#if 0
    LUABSD_FUNC("getsockopt",   luab_getsockopt),
#endif
    LUABSD_FUNC("listen",   luab_listen),
    LUABSD_FUNC("recv", luab_recv),
    LUABSD_FUNC("recvfrom", luab_recvfrom),
    LUABSD_FUNC("recvmsg", luab_recvmsg),
#if __BSD_VISIBLE
    LUABSD_FUNC("recvmmesg",    luab_recvmmsg),
#endif
    LUABSD_FUNC("send", luab_send),
    LUABSD_FUNC("sendmsg", luab_sendmsg),
#if __BSD_VISIBLE
    LUABSD_FUNC("sendmmesg",    luab_sendmmsg),
#endif
    LUABSD_FUNC("linger_create", luab_linger_create),
    LUABSD_FUNC("msghdr_create", luab_msghdr_create),
    LUABSD_FUNC("sockaddr_create",   luab_sockaddr_create),
    LUABSD_INT(NULL, 0)
};

luab_module_t luab_sys_socket_lib = {
    .cookie = LUABSD_SYS_SOCKET_LIB_ID,
    .name = LUABSD_SYS_SOCKET_LIB_KEY,
    .vec = luab_sys_socket_vec,
};
