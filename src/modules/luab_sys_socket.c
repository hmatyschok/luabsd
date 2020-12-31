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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#define LUAB_SYS_SOCKET_LIB_ID    1594740107
#define LUAB_SYS_SOCKET_LIB_KEY   "socket"

extern luab_module_t luab_sys_socket_lib;

/*
 * Subr.
 */

static luab_table_t *
luab_table_checkmmsghdr(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct mmsghdr *x;
    struct msghdr *msg;
    size_t i, j;

    m = luab_xmod(MSGHDR, TYPE, __func__);
    j = luab_checktable(L, narg);

    if ((tbl = luab_table_alloc(j, sizeof(struct mmsghdr), 0)) != NULL) {
        luab_table_init(L, 0);

        x = (struct mmsghdr *)(tbl->tbl_vec);

        for (i = 0, j = tbl->tbl_card; i < j; i++) {

            if (lua_next(L, narg) != 0) {
                /*
                 * (k,v) := (-2,-1) -> (LUA_TNUMBER,LUA_TSTRING)
                 */
                if ((lua_isnumber(L, -2) != 0) &&
                    (lua_isuserdata(L, -1) != 0)) {
                    msg = luab_udata(L, -1, m, struct msghdr *);
                    (void)memmove(&(x[i].msg_hdr), msg, sizeof(struct msghdr));
                } else
                    luab_core_err(EX_DATAERR, __func__, EINVAL);
            } else {
                errno = ENOENT;
                break;
            }
            lua_pop(L, 1);
        }
    }
    return (tbl);
}

/*
 * Tuple (name,level,optval,optlen,x) for {g,s}etsockopt(2).
 */

typedef struct luab_sockopt {
    luab_xarg_t sopt_pci;
    int         sopt_sock;
    int         sopt_level;
    int         sopt_name;
    void        *sopt_val;
    socklen_t   sopt_len;
    void        *sopt_x;
} luab_sockopt_t;

static void
luab_checkxsockopt(lua_State *L, luab_sockopt_t *sopt)
{
    luab_module_t *m;
    luab_xarg_t *pci;
    luab_primitive_t *xp;

    (void)luab_core_checkmaxargs(L, 5);

    m = luab_xmod(INTEGER, TYPE, __func__);
#if 0
    if (sopt == NULL)
        luab_core_err(EX_DATAERR, __func__, EINVAL);

    (void)memset_s(sizeof(*sopt), 0, sizeof(*sopt));
#endif
    pci = &(sopt->sopt_pci);

    sopt->sopt_sock = (int)luab_checkinteger(L, 1, luab_env_int_max);
    sopt->sopt_level = (int)luab_checkinteger(L, 2, luab_env_int_max);
    sopt->sopt_name = (int)luab_checkinteger(L, 3, luab_env_int_max);

    if ((sopt->sopt_val = luab_toxdata(L, 4, pci)) != NULL) {

        if (pci->xarg_idx == LUAB_INTEGER_IDX) {   /* XXX macro, anyone ??? */
            sopt->sopt_val = &(((luab_primitive_t *)sopt->sopt_val)->un_int);
            pci->xarg_len = sizeof(*(int *)(sopt->sopt_val));
        }
    }

    if ((xp = luab_isudata(L, 5, m)) != NULL)
        sopt->sopt_x = &(xp->un_socklen);
    else {
        sopt->sopt_len = (socklen_t)luab_checkinteger(L, 5, luab_env_int_max);

        if (sopt->sopt_len != pci->xarg_len)
            luab_core_err(EX_DATAERR, __func__, ERANGE);
    }
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
 * @usage as [, err, msg ] = bsd.sys.socket.accept(s, addr, addrlen)
 */
static int
luab_accept(lua_State *L)
{
    luab_module_t *m0, *m1;
    int s;
    struct sockaddr *addr;
    socklen_t *addrlen;
    int as;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(SOCKADDR, TYPE, __func__);
    m1 = luab_xmod(SOCKLEN, TYPE, __func__);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    addr = luab_udataisnil(L, 2, m0, struct sockaddr *);
    addrlen = luab_udataisnil(L, 3, m1, socklen_t *);

    as = accept(s, addr, addrlen);

    return (luab_pushxinteger(L, as));
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
 * @usage ret [, err, msg ] = bsd.sys.socket.bind(s, addr, addrlen)
 */
static int
luab_bind(lua_State *L)
{
    luab_module_t *m;
    int s;
    struct sockaddr *addr;
    socklen_t addrlen;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m = luab_xmod(SOCKADDR, TYPE, __func__);
    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    addr = luab_udata(L, 2, m, struct sockaddr *);
    addrlen = (socklen_t)luab_checkinteger(L, 3, luab_env_int_max);

    status = bind(s, addr, addrlen);

    return (luab_pushxinteger(L, status));
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
 * @usage ret [, err, msg ] = bsd.sys.socket.connect(s, name, namelen)
 */
static int
luab_connect(lua_State *L)
{
    luab_module_t *m;
    int s;
    struct sockaddr *name;
    socklen_t namelen;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m = luab_xmod(SOCKADDR, TYPE, __func__);
    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    name = luab_udata(L, 2, m, struct sockaddr *);
    namelen = (socklen_t)luab_checkinteger(L, 3, luab_env_int_max);

    status = connect(s, name, namelen);

    return (luab_pushxinteger(L, status));
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
 * @usage as [, err, msg ] = bsd.sys.socket.accept4(s, addr, addrlen, flags)
 */
static int
luab_accept4(lua_State *L)
{
    luab_module_t *m0, *m1;
    int s;
    struct sockaddr *addr;
    socklen_t *addrlen;
    int flags, as;

    (void)luab_core_checkmaxargs(L, 4);

    m0 = luab_xmod(SOCKADDR, TYPE, __func__);
    m1 = luab_xmod(SOCKLEN, TYPE, __func__);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    addr = luab_udataisnil(L, 2, m0, struct sockaddr *);
    addrlen = luab_udataisnil(L, 3, m1, socklen_t *);
    flags = (int)luab_checkinteger(L, 4, luab_env_int_max);

    as = accept4(s, addr, addrlen, flags);

    return (luab_pushxinteger(L, as));
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
 * @usage ret [, err, msg ] = bsd.sys.socket.bindat(fd, s, addr, addrlen)
 */
static int
luab_bindat(lua_State *L)
{
    luab_module_t *m;
    int fd, s;
    struct sockaddr *addr;
    socklen_t addrlen;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m = luab_xmod(SOCKADDR, TYPE, __func__);

    fd = (int)luab_checkinteger(L, 1, luab_env_int_max);
    s = (int)luab_checkinteger(L, 2, luab_env_int_max);
    addr = luab_udata(L, 3, m, struct sockaddr *);
    addrlen = (socklen_t)luab_checkinteger(L, 4, luab_env_int_max);

    status = bindat(fd, s, addr, addrlen);

    return (luab_pushxinteger(L, status));
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
 * @usage ret [, err, msg ] = bsd.sys.socket.connectat(fd s, name, namelen)
 */
static int
luab_connectat(lua_State *L)
{
    luab_module_t *m;
    int fd, s;
    struct sockaddr *name;
    socklen_t namelen;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m = luab_xmod(SOCKADDR, TYPE, __func__);
    fd = (int)luab_checkinteger(L, 1, luab_env_int_max);
    s = (int)luab_checkinteger(L, 2, luab_env_int_max);
    name = luab_udata(L, 3, m, struct sockaddr *);
    namelen = (socklen_t)luab_checkinteger(L, 4, luab_env_int_max);

    status = connectat(fd, s, name, namelen);

    return (luab_pushxinteger(L, status));
}
#endif  /* __BSD_VISIBLE */

/***
 * getpeername(2) - get peer name
 *
 * @function getpeername
 *
 * @param s                 Socket bound to an adress by bind(2).
 * @param name              Result argument, (LUA_TUSERDATA(SOCKADDR)).
 * @param namelen           Value-result argument, (LUA_TUSERDATA(SOCKLEN)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage as [, err, msg ] = bsd.sys.socket.getpeername(s, name, namelen)
 */
static int
luab_getpeername(lua_State *L)
{
    luab_module_t *m0, *m1;
    int s;
    struct sockaddr *name;
    socklen_t *namelen;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(SOCKADDR, TYPE, __func__);
    m1 = luab_xmod(SOCKLEN, TYPE, __func__);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    name = luab_udata(L, 2, m0, struct sockaddr *);
    namelen = luab_udata(L, 3, m1, socklen_t *);

    status = getpeername(s, name, namelen);

    return (luab_pushxinteger(L, status));
}

/***
 * getsockname(2) - get socket(9) name
 *
 * @function getsockname
 *
 * @param s                 Socket bound to an adress by bind(2).
 * @param name              Result argument, (LUA_TUSERDATA(SOCKADDR)).
 * @param namelen           Value-result argument, (LUA_TUSERDATA(SOCKLEN)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage as [, err, msg ] = bsd.sys.socket.getsockname(s, name, namelen)
 */
static int
luab_getsockname(lua_State *L)
{
    luab_module_t *m0, *m1;
    int s;
    struct sockaddr *name;
    socklen_t *namelen;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(SOCKADDR, TYPE, __func__);
    m1 = luab_xmod(SOCKLEN, TYPE, __func__);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    name = luab_udata(L, 2, m0, struct sockaddr *);
    namelen = luab_udata(L, 3, m1, socklen_t *);

    status = getsockname(s, name, namelen);

    return (luab_pushxinteger(L, status));
}

/***
 * getsockopt(2) - get and set options on a socket
 *
 * @function getsockopt
 *
 * @param s                 File descriptor for open socket(9).
 * @param level             Specifies manipulation of options either on socket(9)
 *                          level by setting its value to SOL_SOCKET or otherwise
 *                          by constant for specific protocol domain(9).
 * @param optname           Specifies uninterpreted options from for
 *                          interpretation of those on the level for
 *                          the specific protocol module.
 * @param optval            Result argument, buffer.
 * @param optlen            Result argument, size of buffer in bytes.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.socket.getsockopt(s, level, optname, optval, optlen)
 */
static int
luab_getsockopt(lua_State *L)
{
    luab_sockopt_t sopt;
    int status;

    luab_checkxsockopt(L, &sopt);

    status = getsockopt(sopt.sopt_sock, sopt.sopt_level,
        sopt.sopt_name, sopt.sopt_val, sopt.sopt_x);

    return (luab_pushxinteger(L, status));
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
 * @usage ret [, err, msg ] = bsd.sys.listen.listen(s, backlog)
 */
static int
luab_listen(lua_State *L)
{
    int s, backlog;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    backlog = (int)luab_checkinteger(L, 2, luab_env_int_max);

    status = listen(s, backlog);

    return (luab_pushxinteger(L, status));
}

/***
 * recv(2) - receive message(s) from a socket(9)
 *
 * @function recv
 *
 * @param s                 Open socket(9).
 * @param buf               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param len               Assumed number of bytes to be rx'd.
 * @param flags             Flags argument, values from
 *
 *                              bsd.sys.socket.MSG_{
 *                                  OOB,
 *                                  PEEK,
 *                                  WAITALL,
 *                                  DONTWAIT,
 *                                  CMSG_CLOEXEC
 *                              }
 *
 *                          may combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.sys.socket.recv(s, buf, len, flags)
 */
static int
luab_recv(lua_State *L)
{
    luab_module_t *m;
    int s;
    luab_iovec_t *buf;
    size_t len;
    int flags;

    (void)luab_core_checkmaxargs(L, 3);

    m = luab_xmod(IOVEC, TYPE, __func__);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    buf = luab_udata(L, 2, m, luab_iovec_t *);
    len = (size_t)luab_checklinteger(L, 3, 0);
    flags = (int)luab_checkinteger(L, 4, luab_env_int_max);

    return (luab_iovec_recv(L, s, buf, &len, flags));
}

/***
 * recvfrom(2) - receive message(s) from a socket(9)
 *
 * @function recvfrom
 *
 * @param s                 Open socket(9).
 * @param buf               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param len               Assumed number of bytes to be rx'd.
 * @param flags             Flags argument, values from
 *
 *                              bsd.sys.socket.MSG_{
 *                                  OOB,
 *                                  PEEK,
 *                                  WAITALL,
 *                                  DONTWAIT,
 *                                  CMSG_CLOEXEC
 *                              }
 *
 *                          may combined by inclusive or.
 * @param from              Result argument, (LUA_TUSERDATA(SOCKADDR)).
 * @param fromlen           Value-result argument, (LUA_TUSERDATA(SOCKLEN)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.sys.socket.recvfrom(s, buf, len, flags, from, fromlen)
 */
static int
luab_recvfrom(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    int s;
    luab_iovec_t *buf;
    size_t len;
    int flags;
    struct sockaddr *from;
    socklen_t *fromlen;

    (void)luab_core_checkmaxargs(L, 6);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(SOCKADDR, TYPE, __func__);
    m2 = luab_xmod(SOCKLEN, TYPE, __func__);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    buf = luab_udata(L, 2, m0, luab_iovec_t *);
    len = (size_t)luab_checklinteger(L, 3, 0);
    flags = (int)luab_checkinteger(L, 4, luab_env_int_max);
    from = luab_udataisnil(L, 5, m1, struct sockaddr *);
    fromlen = luab_udata(L, 6, m2, socklen_t *);

    return (luab_iovec_recvfrom(L, s, buf, &len, flags, from, fromlen));
}

/***
 * recvmsg(2) - receive message(s) from a socket(9)
 *
 * @function recvmsg
 *
 * @param s                 File drscriptor denotes by socket(2) opened socket(9).
 * @param msg               Instance of LUA_TUSERDATA(MSGHDR).
 * @param flags             Flags argument, values from
 *
 *                              bsd.sys.socket.MSG_{
 *                                  OOB,
 *                                  PEEK,
 *                                  WAITALL,
 *                                  DONTWAIT,
 *                                  CMSG_CLOEXEC
 *                              }
 *
 *                          may combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.sys.socket.recvmsg(s, msg, flags)
 */
static int
luab_recvmsg(lua_State *L)
{
    luab_module_t *m;
    int s;
    struct msghdr *msg;
    int flags;
    ssize_t count;

    (void)luab_core_checkmaxargs(L, 3);

    m = luab_xmod(MSGHDR, TYPE, __func__);
    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    msg = luab_udata(L, 2, m, struct msghdr *);
    flags = (int)luab_checkinteger(L, 3, luab_env_int_max);

    if ((msg->msg_iov != NULL) &&
        (msg->msg_iovlen > 0))
        count = recvmsg(s, msg, flags);
    else {
        errno = ERANGE;
        count = luab_env_error;
    }
    return (luab_pushxinteger(L, count));
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
 * @param flags             Flags argument, values from
 *
 *                              bsd.sys.socket.MSG_{
 *                                  OOB,
 *                                  PEEK,
 *                                  WAITALL,
 *                                  DONTWAIT,
 *                                  CMSG_CLOEXEC,
 *                                  WAITFORONE
 *                              }
 *
 *                          may combined by inclusive or.
 * @param timeout           Specifies timeout, if !nil.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.sys.socket.recvmmsg(s, msgvec, vlen, flags, timeout)
 */
static int
luab_recvmmsg(lua_State *L)
{
    luab_module_t *m;
    int s;                              /* XXX */
    luab_table_t *tbl;
    size_t vlen;
    int flags;
    struct timespec *timeout;
    struct mmsghdr *msgvec;
    ssize_t count;

    (void)luab_core_checkmaxargs(L, 5);

    m = luab_xmod(TIMESPEC, TYPE, __func__);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    tbl = luab_table_checkmmsghdr(L, 2);
    vlen = (size_t)luab_checklinteger(L, 3, 0);
    flags = (int)luab_checkinteger(L, 4, luab_env_int_max);
    timeout = luab_udataisnil(L, 5, m, struct timespec *);

    if (tbl != NULL) {
        msgvec = (struct mmsghdr *)(tbl->tbl_vec);
        count = recvmmsg(s, msgvec, vlen, flags, timeout);
        luab_table_free(tbl);
    } else
        count = luab_env_error;

    return (luab_pushxinteger(L, count));
}
#endif

/***
 * send(2) - send message(s) from a socket(9)
 *
 * @function send
 *
 * @param s                 Open socket(9).
 * @param msg               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param len               Assumed number of bytes to be rx'd.
 * @param flags             Flags argument, values from
 *
 *                              bsd.sys.socket.MSG_{
 *                                  OOB,
 *                                  DONTROUTE,
 *                                  EOR,
 *                                  DONTWAIT,
 *                                  EOF,
 *                                  NOSIGNAL
 *                              }
 *
 *                          may combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.sys.socket.send(s, msg, len, flags)
 */
static int
luab_send(lua_State *L)
{
    luab_module_t *m;
    int s;
    luab_iovec_t *msg;
    size_t len;
    int flags;

    (void)luab_core_checkmaxargs(L, 3);

    m = luab_xmod(IOVEC, TYPE, __func__);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    msg = luab_udata(L, 2, m, luab_iovec_t *);
    len = (size_t)luab_checklinteger(L, 3, 0);
    flags = (int)luab_checkinteger(L, 4, luab_env_int_max);

    return (luab_iovec_send(L, s, msg, &len, flags));
}

/***
 * sendto(2) - send message(s) from a socket(9)
 *
 * @function sendto
 *
 * @param s                 Open socket(9).
 * @param buf               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param len               Assumed number of bytes to be tx'd.
 * @param flags             Flags argument, values from
 *
 *                              bsd.sys.socket.MSG_{
 *                                  OOB,
 *                                  DONTROUTE,
 *                                  EOR,
 *                                  DONTWAIT,
 *                                  EOF,
 *                                  NOSIGNAL
 *                              }
 *
 *                          may combined by inclusive or.
 * @param to                Result argument, (LUA_TUSERDATA(SOCKADDR)).
 * @param tolen             Size of address.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.sys.socket.sendto(s, buf, len, flags, to, tolen)
 */
static int
luab_sendto(lua_State *L)
{
    luab_module_t *m0, *m1;
    int s;
    luab_iovec_t *buf;
    size_t len;
    int flags;
    struct sockaddr *to;
    socklen_t tolen;

    (void)luab_core_checkmaxargs(L, 6);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(SOCKADDR, TYPE, __func__);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    buf = luab_udata(L, 2, m0, luab_iovec_t *);
    len = (size_t)luab_checklinteger(L, 3, 0);
    flags = (int)luab_checkinteger(L, 4, luab_env_int_max);
    to = luab_udataisnil(L, 5, m1, struct sockaddr *);
    tolen = (socklen_t)luab_checkinteger(L, 4, luab_env_int_max);

    return (luab_iovec_sendto(L, s, buf, &len, flags, to, tolen));
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
 *                              bsd.sys.socket.MSG_{
 *                                  OOB,
 *                                  DONTROUTE,
 *                                  EOR,
 *                                  DONTWAIT,
 *                                  EOF,
 *                                  NOSIGNAL
 *                              }
 *
 *                          may combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.sys.socket.sendmsg(s, msg, flags)
 */
static int
luab_sendmsg(lua_State *L)
{
    luab_module_t *m;
    int s;
    struct msghdr *msg;
    int flags;
    ssize_t count;

    (void)luab_core_checkmaxargs(L, 3);

    m = luab_xmod(MSGHDR, TYPE, __func__);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    msg = luab_udata(L, 2, m, struct msghdr *);
    flags = (int)luab_checkinteger(L, 3, luab_env_int_max);

    if ((msg->msg_iov != NULL) &&
        (msg->msg_iovlen > 0))
        count = sendmsg(s, msg, flags);
    else {
        errno = ERANGE;
        count = luab_env_error;
    }
    return (luab_pushxinteger(L, count));
}

#if __BSD_VISIBLE
/***
 * sendfile(2) - send a file to a socket
 *
 * @function sendfile
 *
 * @param fd                Specifies either regular file or shared memory object.
 * @param s                 File descriptor for open socket(9).
 * @param offset            Specifies where transmission of contents starts.
 * @param nbytes            Specifies how many bytes will be transmitted.
 * @param hdtr              Specifies optional HDR/TRL, by an instance
 *                          of (LUA_TUSERDATA(SF_HDTR)).
 * @param sbytes            Result argument, how many bytes are sent, instance
 *                          of (LUA_TUSERDATA(OFF)).
 * @param flags             Flags argument over
 *
 *                              bsd.sys.socket.SF_{
 *                                  NODISKIO,
 *                                  NOCACHE,
 *                                  SYNC,
 *                                  USER_READAHEAD
 *                              }
 *
 *                          may combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.socket.sendfile(fd, s, offset, nbytes, hdtr, sbytes, flags)
 */
static int
luab_sendfile(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fd, s;
    off_t offset;
    size_t nbytes;
    struct sf_hdtr *hdtr;
    off_t *sbytes;
    int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 7);

    m0 = luab_xmod(SF_HDTR, TYPE, __func__);
    m1 = luab_xmod(OFF, TYPE, __func__);

    fd = luab_checkinteger(L, 1, luab_env_int_max);
    s = luab_checkinteger(L, 2, luab_env_int_max);
    offset = luab_checkinteger(L, 3, luab_env_long_max);
    nbytes = (size_t)luab_checklinteger(L, 4, 0);
    hdtr = luab_udataisnil(L, 5, m0, struct sf_hdtr *);
    sbytes = luab_udataisnil(L, 6, m1, off_t *);

    flags = luab_checkinteger(L, 7, luab_env_int_max);

    status = sendfile(fd, s, offset, nbytes, hdtr, sbytes, flags);

    return (luab_pushxinteger(L, status));
}

/***
 * sendmmsg(2) - send multiple message(s) at a call from a socket(9)
 *
 * @function sendmmsg
 *
 * @param s                 File descriptor for open socket(9).
 * @param msgvec            Instance of LUA_TTABLE(LUA_TUSERDATA(MSGHDR)).
 * @param vlen              Constraint for transmission of #n messages.
 * @param flags             Flags argument over
 *
 *                              bsd.sys.socket.MSG_{
 *                                  OOB,
 *                                  DONTROUTE,
 *                                  EOR,
 *                                  DONTWAIT,
 *                                  EOF,
 *                                  NOSIGNAL
 *                              }
 *
 *                          may combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.sys.socket.sendmmsg(s, msgvec, vlen, flags)
 */
static int
luab_sendmmsg(lua_State *L)
{
    int s;
    luab_table_t *tbl;
    size_t vlen;
    int flags;
    struct mmsghdr *msgvec;
    ssize_t count;

    (void)luab_core_checkmaxargs(L, 4);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    tbl = luab_table_checkmmsghdr(L, 2);
    vlen = (size_t)luab_checklinteger(L, 3, 0);
    flags = (int)luab_checkinteger(L, 4, luab_env_int_max);

    if (tbl != NULL) {
        msgvec = (struct mmsghdr *)(tbl->tbl_vec);
        count = sendmmsg(s, msgvec, vlen, flags);
        luab_table_free(tbl);
    } else
        count = luab_env_error;

    return (luab_pushxinteger(L, count));
}

/***
 * setfib(2) - set the default FIB (routing table) for the calling process
 *
 * @function setfib
 *
 * @param fib               Specifies associated forward information base.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.socket.setfib(fib)
 */
static int
luab_setfib(lua_State *L)
{
    int fib;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    fib = (int)luab_checkinteger(L, 1, luab_env_int_max);
    status = setfib(fib);

    return (luab_pushxinteger(L, status));
}
#endif

/***
 * setsockopt(2) - get and set options on a socket
 *
 * @function setsockopt
 *
 * @param s                 File descriptor for open socket(9).
 * @param level             Specifies manipulation of options either on socket(9)
 *                          level by setting its value to SOL_SOCKET or otherwise
 *                          by constant for specific protocol domain(9).
 * @param optname           Specifies uninterpreted options from for
 *                          interpretation of those on the level for
 *                          the specific protocol module.
 * @param optval            Specifies supplied data for interpretation.
 * @param optlen            Specifies length of supplied data in bytes.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.socket.setsockopt(s, level, optname, optval, optlen)
 */
static int
luab_setsockopt(lua_State *L)
{
    luab_sockopt_t sopt;
    int status;

    luab_checkxsockopt(L, &sopt);

    status = setsockopt(sopt.sopt_sock, sopt.sopt_level,
        sopt.sopt_name, sopt.sopt_val, sopt.sopt_len);

    return (luab_pushxinteger(L, status));
}

/***
 * shutdown(2) - disables sends and/or receives on a socket
 *
 * @function shutdown
 *
 * @param s                 Open socket(9).
 * @param how               Specifies type of shutdown by values from:
 *
 *                              bsd.sys.socket.SHUT_{
 *                                  RD,
 *                                  WR,
 *                                  RDWR
 *                              }
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.socket.shutdown(s, how)
 */
static int
luab_shutdown(lua_State *L)
{
    int s, how;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    how = (int)luab_checkinteger(L, 2, luab_env_int_max);
    status = shutdown(s, how);

    return (luab_pushxinteger(L, status));
}

/***
 * sockatmark(2) - determine whether the read pointer is at OOB mark
 *
 * @function sockatmark
 *
 * @param s                 Open socket(9).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.socket.shutdown(s, how)
 */
static int
luab_sockatmark(lua_State *L)
{
    int s, status;

    (void)luab_core_checkmaxargs(L, 1);

    s = (int)luab_checkinteger(L, 1, luab_env_int_max);
    status = sockatmark(s);

    return (luab_pushxinteger(L, status));
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
 * @usage s [, err, msg ] = bsd.sys.socket.socket(domain, type, protocol)
 */
static int
luab_socket(lua_State *L)
{
    int domain;
    int type;
    int protocol;
    int s;

    (void)luab_core_checkmaxargs(L, 3);

    domain = (int)luab_checkinteger(L, 1, luab_env_int_max);
    type = (int)luab_checkinteger(L, 2, luab_env_int_max);
    protocol = (int)luab_checkinteger(L, 3, luab_env_int_max);

    s = socket(domain, type, protocol);

    return (luab_pushxinteger(L, s));
}

/***
 * socketpair(2) - create a pair of connected sockets
 *
 * @function socketpair
 *
 * @param domain            Specifies communication domain(9), where Inter
 *                          Process Communication (IPC) takes place.
 * @param type              Specifies semantics of communication for IPC.
 * @param protocol          Specifies used for IPC by socket(9) utilized
 *                          particular Protocol.
 * @param sv                Result argument, empty (LUA_TTABLE).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage s [, err, msg ] = bsd.sys.socket.socketpair(domain, type, protocol)
 */
static int
luab_socketpair(lua_State *L)
{
    luab_module_t *m;
    int domain;
    int type;
    int protocol;
    luab_table_t *tbl;
    int *socks;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    m = luab_xmod(INT, TYPE, __func__);

    domain = (int)luab_checkinteger(L, 1, luab_env_int_max);
    type = (int)luab_checkinteger(L, 2, luab_env_int_max);
    protocol = (int)luab_checkinteger(L, 3, luab_env_int_max);

    (void)luab_checkltable(L, 4, 0);

    if ((tbl = luab_table_alloc(2, m->m_sz, m->m_id)) != NULL)
        socks = (int *)(tbl->tbl_vec);
    else
        socks = NULL;

    if ((status = socketpair(domain, type, protocol, socks)) == 0)
        luab_table_pushxdata(L, 4, m, tbl, 0, 1);

    return (luab_pushxinteger(L, status));
}

/*
 * Generator functions.
 */

/***
 * Generator function.
 *
 * @function linger_create
 *
 * @param data          Instance of (LUA_TUSERDATA(LINGER)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage linger [, err, msg ] = bsd.sys.socket.linger_create([ data ])
 */
static int
luab_linger_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(LINGER, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
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
 * @usage sockaddr [, err, msg ] = bsd.sys.socket.sockaddr_create([ data ])
 */
static int
luab_sockaddr_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SOCKADDR, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(MSGHDR)).
 *
 * @function msghdr_create
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage msghdr [, err, msg ] = bsd.sys.socket.msghdr_create()
 */
static int
luab_msghdr_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(MSGHDR, TYPE, __func__);
    return (luab_core_create(L, 0, m, NULL));
}

#if __BSD_VISIBLE
/***
 * Generator function.
 *
 * @function accept_filter_arg_create
 *
 * @param data          Instance of (LUA_TUSERDATA(ACCEPT_FILTER_ARG)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage accept_filter_arg [, err, msg ] = bsd.sys.socket.accept_filter_arg_create([ data ])
 */
static int
luab_accept_filter_arg_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(ACCEPT_FILTER_ARG, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(CMSGCRED)).
 *
 * @function cmsgcred_create
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage cmsgcred [, err, msg ] = bsd.sys.socket.cmsgcred_create()
 */
static int
luab_cmsgcred_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(CMSGCRED, TYPE, __func__);
    return (luab_core_create(L, 0, m, NULL));
}

/***
 * Generator function.
 *
 * @function sockproto_create
 *
 * @param data          Instance of (LUA_TUSERDATA(SOCKPROTO)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sockproto [, err, msg ] = bsd.sys.socket.sockproto_create([ data ])
 */
static int
luab_sockproto_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SOCKPROTO, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function.
 *
 * @function sf_hdtr_create
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sf_hdtr [, err, msg ] = bsd.sys.socket.sf_hdtr_create()
 */
static int
luab_sf_hdtr_create(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SF_HDTR, TYPE, __func__);
    return (luab_core_create(L, 0, m, NULL));
}
#endif

/*
 * Interface against <sys/socket.h>.
 */

static luab_module_table_t luab_sys_socket_vec[] = {
#if __BSD_VISIBLE
    LUAB_INT("SOCK_MAXADDRLEN",             SOCK_MAXADDRLEN),
#endif
    LUAB_INT("SOCK_STREAM",                 SOCK_STREAM),
    LUAB_INT("SOCK_DGRAM",                  SOCK_DGRAM),
    LUAB_INT("SOCK_RAW",                    SOCK_RAW),
#if __BSD_VISIBLE
    LUAB_INT("SOCK_RDM",                    SOCK_RDM),
#endif
    LUAB_INT("SOCK_SEQPACKET",              SOCK_SEQPACKET),
#if __BSD_VISIBLE
    LUAB_INT("SOCK_CLOEXEC",                SOCK_CLOEXEC),
    LUAB_INT("SOCK_NONBLOCK",               SOCK_NONBLOCK),
#endif
    LUAB_INT("SO_DEBUG",                    SO_DEBUG),
    LUAB_INT("SO_ACCEPTCONN",               SO_ACCEPTCONN),
    LUAB_INT("SO_REUSEADDR",                SO_REUSEADDR),
    LUAB_INT("SO_KEEPALIVE",                SO_KEEPALIVE),
    LUAB_INT("SO_DONTROUTE",                SO_DONTROUTE),
    LUAB_INT("SO_BROADCAST",                SO_BROADCAST),
#if __BSD_VISIBLE
    LUAB_INT("SO_USELOOPBACK",              SO_USELOOPBACK),
#endif
    LUAB_INT("SO_LINGER",                   SO_LINGER),
    LUAB_INT("SO_OOBINLINE",                SO_OOBINLINE),
#if __BSD_VISIBLE
    LUAB_INT("SO_REUSEPORT",                SO_REUSEPORT),
    LUAB_INT("SO_TIMESTAMP",                SO_TIMESTAMP),
    LUAB_INT("SO_NOSIGPIPE",                SO_NOSIGPIPE),
    LUAB_INT("SO_ACCEPTFILTER",             SO_ACCEPTFILTER),
    LUAB_INT("SO_BINTIME",                  SO_BINTIME),
#endif
    LUAB_INT("SO_NO_OFFLOAD",               SO_NO_OFFLOAD),
    LUAB_INT("SO_NO_DDP",                   SO_NO_DDP),
    LUAB_INT("SO_REUSEPORT_LB",             SO_REUSEPORT_LB),
    LUAB_INT("SO_SNDBUF",                   SO_SNDBUF),
    LUAB_INT("SO_RCVBUF",                   SO_RCVBUF),
    LUAB_INT("SO_SNDLOWAT",                 SO_SNDLOWAT),
    LUAB_INT("SO_RCVLOWAT",                 SO_RCVLOWAT),
    LUAB_INT("SO_SNDTIMEO",                 SO_SNDTIMEO),
    LUAB_INT("SO_RCVTIMEO",                 SO_RCVTIMEO),
    LUAB_INT("SO_ERROR",                    SO_ERROR),
    LUAB_INT("SO_TYPE",                     SO_TYPE),
#if __BSD_VISIBLE
    LUAB_INT("SO_LABEL",                    SO_LABEL),
    LUAB_INT("SO_PEERLABEL",                SO_PEERLABEL),
    LUAB_INT("SO_LISTENQLIMIT",             SO_LISTENQLIMIT),
    LUAB_INT("SO_LISTENQLEN",               SO_LISTENQLEN),
    LUAB_INT("SO_LISTENINCQLEN",            SO_LISTENINCQLEN),
    LUAB_INT("SO_SETFIB",                   SO_SETFIB),
    LUAB_INT("SO_USER_COOKIE",              SO_USER_COOKIE),
    LUAB_INT("SO_PROTOCOL",                 SO_PROTOCOL),
    LUAB_INT("SO_PROTOTYPE",                SO_PROTOTYPE),
    LUAB_INT("SO_TS_CLOCK",                 SO_TS_CLOCK),
    LUAB_INT("SO_MAX_PACING_RATE",          SO_MAX_PACING_RATE),
    LUAB_INT("SO_DOMAIN",                   SO_DOMAIN),
    LUAB_INT("SO_TS_REALTIME_MICRO",        SO_TS_REALTIME_MICRO),
    LUAB_INT("SO_TS_BINTIME",               SO_TS_BINTIME),
    LUAB_INT("SO_TS_REALTIME",              SO_TS_REALTIME),
    LUAB_INT("SO_TS_MONOTONIC",             SO_TS_MONOTONIC),
    LUAB_INT("SO_TS_DEFAULT",               SO_TS_DEFAULT),
    LUAB_INT("SO_TS_CLOCK_MAX",             SO_TS_CLOCK_MAX),
    LUAB_INT("SO_VENDOR",                   SO_VENDOR),
#endif
    LUAB_INT("SOL_SOCKET",                  SOL_SOCKET),
    LUAB_INT("AF_UNSPEC",                   AF_UNSPEC),
#if __BSD_VISIBLE
    LUAB_INT("AF_LOCAL",                    AF_LOCAL),
#endif
    LUAB_INT("AF_UNIX",                     AF_UNIX),
    LUAB_INT("AF_INET",                     AF_INET),
#if __BSD_VISIBLE
    LUAB_INT("AF_IMPLINK",                  AF_IMPLINK),
    LUAB_INT("AF_PUP",                      AF_PUP),
    LUAB_INT("AF_CHAOS",                    AF_CHAOS),
    LUAB_INT("AF_NETBIOS",                  AF_NETBIOS),
    LUAB_INT("AF_ISO",                      AF_ISO),
    LUAB_INT("AF_OSI",                      AF_OSI),
    LUAB_INT("AF_ECMA",                     AF_ECMA),
    LUAB_INT("AF_DATAKIT",                  AF_DATAKIT),
    LUAB_INT("AF_CCITT",                    AF_CCITT),
    LUAB_INT("AF_SNA",                      AF_SNA),
    LUAB_INT("AF_DECnet",                   AF_DECnet),
    LUAB_INT("AF_DLI",                      AF_DLI),
    LUAB_INT("AF_LAT",                      AF_LAT),
    LUAB_INT("AF_HYLINK",                   AF_HYLINK),
    LUAB_INT("AF_APPLETALK",                AF_APPLETALK),
    LUAB_INT("AF_ROUTE",                    AF_ROUTE),
    LUAB_INT("AF_LINK",                     AF_LINK),
    LUAB_INT("pseudo_AF_XTP",               pseudo_AF_XTP),
    LUAB_INT("AF_COIP",                     AF_COIP),
    LUAB_INT("AF_CNT",                      AF_CNT),
    LUAB_INT("pseudo_AF_RTIP",              pseudo_AF_RTIP),
    LUAB_INT("AF_IPX",                      AF_IPX),
    LUAB_INT("AF_SIP",                      AF_SIP),
    LUAB_INT("pseudo_AF_PIP",               pseudo_AF_PIP),
    LUAB_INT("AF_ISDN",                     AF_ISDN),
    LUAB_INT("AF_E164",                     AF_E164),
    LUAB_INT("pseudo_AF_KEY",               pseudo_AF_KEY),
#endif
    LUAB_INT("AF_INET6",                    AF_INET6),
#if __BSD_VISIBLE
    LUAB_INT("AF_NATM",                     AF_NATM),
    LUAB_INT("AF_ATM",                      AF_ATM),
    LUAB_INT("pseudo_AF_HDRCMPLT",          pseudo_AF_HDRCMPLT),
    LUAB_INT("AF_NETGRAPH",                 AF_NETGRAPH),
    LUAB_INT("AF_SLOW",                     AF_SLOW),
    LUAB_INT("AF_SCLUSTER",                 AF_SCLUSTER),
    LUAB_INT("AF_ARP",                      AF_ARP),
    LUAB_INT("AF_BLUETOOTH",                AF_BLUETOOTH),
    LUAB_INT("AF_IEEE80211",                AF_IEEE80211),
    LUAB_INT("AF_INET_SDP",                 AF_INET_SDP),
    LUAB_INT("AF_INET6_SDP",                AF_INET6_SDP),
    LUAB_INT("AF_MAX",                      AF_MAX),
    LUAB_INT("AF_VENDOR00",                 AF_VENDOR00),
    LUAB_INT("AF_VENDOR01",                 AF_VENDOR01),
    LUAB_INT("AF_VENDOR02",                 AF_VENDOR02),
    LUAB_INT("AF_VENDOR03",                 AF_VENDOR03),
    LUAB_INT("AF_VENDOR04",                 AF_VENDOR04),
    LUAB_INT("AF_VENDOR05",                 AF_VENDOR05),
    LUAB_INT("AF_VENDOR06",                 AF_VENDOR06),
    LUAB_INT("AF_VENDOR07",                 AF_VENDOR07),
    LUAB_INT("AF_VENDOR08",                 AF_VENDOR08),
    LUAB_INT("AF_VENDOR09",                 AF_VENDOR09),
    LUAB_INT("AF_VENDOR10",                 AF_VENDOR10),
    LUAB_INT("AF_VENDOR11",                 AF_VENDOR11),
    LUAB_INT("AF_VENDOR12",                 AF_VENDOR12),
    LUAB_INT("AF_VENDOR13",                 AF_VENDOR13),
    LUAB_INT("AF_VENDOR14",                 AF_VENDOR14),
    LUAB_INT("AF_VENDOR15",                 AF_VENDOR15),
    LUAB_INT("AF_VENDOR16",                 AF_VENDOR16),
    LUAB_INT("AF_VENDOR17",                 AF_VENDOR17),
    LUAB_INT("AF_VENDOR18",                 AF_VENDOR18),
    LUAB_INT("AF_VENDOR19",                 AF_VENDOR19),
    LUAB_INT("AF_VENDOR20",                 AF_VENDOR20),
    LUAB_INT("AF_VENDOR21",                 AF_VENDOR21),
    LUAB_INT("AF_VENDOR22",                 AF_VENDOR22),
    LUAB_INT("AF_VENDOR23",                 AF_VENDOR23),
    LUAB_INT("AF_VENDOR24",                 AF_VENDOR24),
    LUAB_INT("AF_VENDOR25",                 AF_VENDOR25),
    LUAB_INT("AF_VENDOR26",                 AF_VENDOR26),
    LUAB_INT("AF_VENDOR27",                 AF_VENDOR27),
    LUAB_INT("AF_VENDOR28",                 AF_VENDOR28),
    LUAB_INT("AF_VENDOR29",                 AF_VENDOR29),
    LUAB_INT("AF_VENDOR30",                 AF_VENDOR30),
    LUAB_INT("AF_VENDOR31",                 AF_VENDOR31),
    LUAB_INT("AF_VENDOR32",                 AF_VENDOR32),
    LUAB_INT("AF_VENDOR33",                 AF_VENDOR33),
    LUAB_INT("AF_VENDOR34",                 AF_VENDOR34),
    LUAB_INT("AF_VENDOR35",                 AF_VENDOR35),
    LUAB_INT("AF_VENDOR36",                 AF_VENDOR36),
    LUAB_INT("AF_VENDOR37",                 AF_VENDOR37),
    LUAB_INT("AF_VENDOR38",                 AF_VENDOR38),
    LUAB_INT("AF_VENDOR39",                 AF_VENDOR39),
    LUAB_INT("AF_VENDOR40",                 AF_VENDOR40),
    LUAB_INT("AF_VENDOR41",                 AF_VENDOR41),
    LUAB_INT("AF_VENDOR42",                 AF_VENDOR42),
    LUAB_INT("AF_VENDOR43",                 AF_VENDOR43),
    LUAB_INT("AF_VENDOR44",                 AF_VENDOR44),
    LUAB_INT("AF_VENDOR45",                 AF_VENDOR45),
    LUAB_INT("AF_VENDOR46",                 AF_VENDOR46),
    LUAB_INT("AF_VENDOR47",                 AF_VENDOR47),
    LUAB_INT("SOCK_MAXADDRLEN",             SOCK_MAXADDRLEN),
    LUAB_INT("PF_UNSPEC",                   PF_UNSPEC),
    LUAB_INT("PF_LOCAL",                    PF_LOCAL),
    LUAB_INT("PF_UNIX",                     PF_UNIX),
    LUAB_INT("PF_INET",                     PF_INET),
    LUAB_INT("PF_IMPLINK",                  PF_IMPLINK),
    LUAB_INT("PF_PUP",                      PF_PUP),
    LUAB_INT("PF_CHAOS",                    PF_CHAOS),
    LUAB_INT("PF_NETBIOS",                  PF_NETBIOS),
    LUAB_INT("PF_ISO",                      PF_ISO),
    LUAB_INT("PF_OSI",                      PF_OSI),
    LUAB_INT("PF_ECMA",                     PF_ECMA),
    LUAB_INT("PF_DATAKIT",                  PF_DATAKIT),
    LUAB_INT("PF_CCITT",                    PF_CCITT),
    LUAB_INT("PF_SNA",                      PF_SNA),
    LUAB_INT("PF_DECnet",                   PF_DECnet),
    LUAB_INT("PF_DLI",                      PF_DLI),
    LUAB_INT("PF_LAT",                      PF_LAT),
    LUAB_INT("PF_HYLINK",                   PF_HYLINK),
    LUAB_INT("PF_APPLETALK",                PF_APPLETALK),
    LUAB_INT("PF_ROUTE",                    PF_ROUTE),
    LUAB_INT("PF_LINK",                     PF_LINK),
    LUAB_INT("PF_XTP",                      PF_XTP),
    LUAB_INT("PF_COIP",                     PF_COIP),
    LUAB_INT("PF_CNT",                      PF_CNT),
    LUAB_INT("PF_SIP",                      PF_SIP),
    LUAB_INT("PF_IPX",                      PF_IPX),
    LUAB_INT("PF_RTIP",                     PF_RTIP),
    LUAB_INT("PF_PIP",                      PF_PIP),
    LUAB_INT("PF_ISDN",                     PF_ISDN),
    LUAB_INT("PF_KEY",                      PF_KEY),
    LUAB_INT("PF_INET6",                    PF_INET6),
    LUAB_INT("PF_NATM",                     PF_NATM),
    LUAB_INT("PF_ATM",                      PF_ATM),
    LUAB_INT("PF_NETGRAPH",                 PF_NETGRAPH),
    LUAB_INT("PF_SLOW",                     PF_SLOW),
    LUAB_INT("PF_SCLUSTER",                 PF_SCLUSTER),
    LUAB_INT("PF_ARP",                      PF_ARP),
    LUAB_INT("PF_BLUETOOTH",                PF_BLUETOOTH),
    LUAB_INT("PF_IEEE80211",                PF_IEEE80211),
    LUAB_INT("PF_INET_SDP",                 PF_INET_SDP),
    LUAB_INT("PF_INET6_SDP",                PF_INET6_SDP),
    LUAB_INT("PF_MAX",                      PF_MAX),
    LUAB_INT("NET_RT_DUMP",                 NET_RT_DUMP),
    LUAB_INT("NET_RT_FLAGS",                NET_RT_FLAGS),
    LUAB_INT("NET_RT_IFLIST",               NET_RT_IFLIST),
    LUAB_INT("NET_RT_IFMALIST",             NET_RT_IFMALIST),
    LUAB_INT("NET_RT_IFLISTL",              NET_RT_IFLISTL),
#endif
    LUAB_INT("SOMAXCONN",                   SOMAXCONN),
    LUAB_INT("MSG_OOB",                     MSG_OOB),
    LUAB_INT("MSG_PEEK",                    MSG_PEEK),
    LUAB_INT("MSG_DONTROUTE",               MSG_DONTROUTE),
    LUAB_INT("MSG_EOR",                     MSG_EOR),
    LUAB_INT("MSG_TRUNC",                   MSG_TRUNC),
    LUAB_INT("MSG_CTRUNC",                  MSG_CTRUNC),
    LUAB_INT("MSG_WAITALL",                 MSG_WAITALL),
#if __BSD_VISIBLE
    LUAB_INT("MSG_DONTWAIT",                MSG_DONTWAIT),
    LUAB_INT("MSG_EOF",                     MSG_EOF),
    LUAB_INT("MSG_NOTIFICATION",            MSG_NOTIFICATION),
    LUAB_INT("MSG_NBIO",                    MSG_NBIO),
    LUAB_INT("MSG_COMPAT",                  MSG_COMPAT),
#endif
#if __POSIX_VISIBLE >= 200809
    LUAB_INT("MSG_NOSIGNAL",                MSG_NOSIGNAL),
#endif
#if __BSD_VISIBLE
    LUAB_INT("MSG_CMSG_CLOEXEC",            MSG_CMSG_CLOEXEC),
    LUAB_INT("MSG_WAITFORONE",              MSG_WAITFORONE),
    LUAB_INT("CMGROUP_MAX",                 CMGROUP_MAX),
#endif
    LUAB_INT("SCM_RIGHTS",                  SCM_RIGHTS),
#if __BSD_VISIBLE
    LUAB_INT("SCM_TIMESTAMP",               SCM_TIMESTAMP),
    LUAB_INT("SCM_CREDS",                   SCM_CREDS),
    LUAB_INT("SCM_BINTIME",                 SCM_BINTIME),
    LUAB_INT("SCM_REALTIME",                SCM_REALTIME),
    LUAB_INT("SCM_MONOTONIC",               SCM_MONOTONIC),
    LUAB_INT("SCM_TIME_INFO",               SCM_TIME_INFO),
    LUAB_INT("ST_INFO_HW",                  ST_INFO_HW),
    LUAB_INT("ST_INFO_HW_HPREC",            ST_INFO_HW_HPREC),
#endif
    LUAB_INT("SHUT_RD",                     SHUT_RD),
    LUAB_INT("SHUT_WR",                     SHUT_WR),
    LUAB_INT("SHUT_RDWR",                   SHUT_RDWR),
#if __BSD_VISIBLE
    LUAB_INT("PRU_FLUSH_RD",                PRU_FLUSH_RD),
    LUAB_INT("PRU_FLUSH_WR",                PRU_FLUSH_WR),
    LUAB_INT("PRU_FLUSH_RDWR",              PRU_FLUSH_RDWR),
    LUAB_INT("SF_NODISKIO",                 SF_NODISKIO),
    LUAB_INT("SF_MNOWAIT",                  SF_MNOWAIT),
    LUAB_INT("SF_SYNC",                     SF_SYNC),
    LUAB_INT("SF_USER_READAHEAD",           SF_USER_READAHEAD),
    LUAB_INT("SF_NOCACHE",                  SF_NOCACHE),
#endif
    LUAB_FUNC("accept",                     luab_accept),
    LUAB_FUNC("bind",                       luab_bind),
    LUAB_FUNC("connect",                    luab_connect),
#if __BSD_VISIBLE
    LUAB_FUNC("accept4",                    luab_accept4),
    LUAB_FUNC("bindat",                     luab_bindat),
    LUAB_FUNC("connectat",                  luab_connectat),
#endif
    LUAB_FUNC("getpeername",                luab_getpeername),
    LUAB_FUNC("getsockname",                luab_getsockname),
    LUAB_FUNC("getsockopt",                 luab_getsockopt),
    LUAB_FUNC("listen",                     luab_listen),
    LUAB_FUNC("recv",                       luab_recv),
    LUAB_FUNC("recvfrom",                   luab_recvfrom),
    LUAB_FUNC("recvmsg",                    luab_recvmsg),
#if __BSD_VISIBLE
    LUAB_FUNC("recvmmesg",                  luab_recvmmsg),
#endif
    LUAB_FUNC("send",                       luab_send),
    LUAB_FUNC("sendto",                     luab_sendto),
    LUAB_FUNC("sendmsg",                    luab_sendmsg),
#if __BSD_VISIBLE
    LUAB_FUNC("sendfile",                   luab_sendfile),
    LUAB_FUNC("sendmmesg",                  luab_sendmmsg),
    LUAB_FUNC("setfib",                     luab_setfib),
#endif
    LUAB_FUNC("setsockopt",                 luab_setsockopt),
    LUAB_FUNC("shutdown",                   luab_shutdown),
    LUAB_FUNC("sockatmark",                 luab_sockatmark),
    LUAB_FUNC("socket",                     luab_socket),
    LUAB_FUNC("socketpair",                 luab_socketpair),
    /* generator functions */
    LUAB_FUNC("linger_create",              luab_linger_create),
    LUAB_FUNC("sockaddr_create",            luab_sockaddr_create),
    LUAB_FUNC("msghdr_create",              luab_msghdr_create),
#if __BSD_VISIBLE
    LUAB_FUNC("accept_filter_arg_create",   luab_accept_filter_arg_create),
    LUAB_FUNC("cmsgcred_create",            luab_cmsgcred_create),
    LUAB_FUNC("sockproto_create",           luab_sockproto_create),
    LUAB_FUNC("sf_hdtr_create",             luab_sf_hdtr_create),
#endif
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_socket_lib = {
    .m_id       = LUAB_SYS_SOCKET_LIB_ID,
    .m_name     = LUAB_SYS_SOCKET_LIB_KEY,
    .m_vec      = luab_sys_socket_vec,
};
