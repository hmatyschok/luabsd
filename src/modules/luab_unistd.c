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
/*
 * The implementation of the interface against alarm(3) is derived from:
 *
 * lalarm.c
 * an alarm library for Lua based on signal
 * Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
 * 28 Jul 2018 12:47:52
 * This code is hereby placed in the public domain and also under the MIT license
 */

#include <arpa/inet.h>

#include <pwd.h>
#include <unistd.h>
#include <signal.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#define LUAB_UNISTD_LIB_ID    1593623310
#define LUAB_UNISTD_LIB_KEY   "unistd"

extern luab_module_t luab_unistd_lib;

extern char **environ;

#define LUAB_SET_LEN        6   /* libc/gen/setmode.c */
#define LUAB_SET_LEN_INCR   4
#define LUAB_SETMAXLEN      LUAB_SET_LEN

/*
 * Subr.
 */

static lua_State *saved_L;
static lua_Hook h;

static int h_msk;
static int h_cnt;

static void
h_callout(lua_State *L, lua_Debug *arg __unused)
{
    L = saved_L;

    lua_sethook(L, h, h_msk, h_cnt);
    lua_getfield(L, LUA_REGISTRYINDEX, "l_callout");

    if (lua_pcall(L, 0, 0, 0) != 0)
        lua_error(L);
}

static void
h_signal(int arg __unused)
{
    int l_msk = (LUA_MASKCALL|LUA_MASKRET|LUA_MASKCOUNT);

    h = lua_gethook(saved_L);
    h_msk = lua_gethookmask(saved_L);
    h_cnt = lua_gethookcount(saved_L);

    lua_sethook(saved_L, h_callout, l_msk, 1);
}

/***
 * alarm(3) - set signal timer alarm
 *
 * @function alarm
 *
 * @param seconds           For timeout specified number of seconds.
 * @param callout           Callout routine implements an event.
 * @param group             Group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sec [, err, msg ] = bsd.unistd.alarm(seconds, callout)
 */
static int
luab_alarm(lua_State *L)    /* XXX */
{
    int narg;
    u_int seconds;
    u_int status;

    narg = luab_core_checkmaxargs(L, 2);

    if ((seconds = (u_int)luab_checkinteger(L, 1, INT_MAX)) > 0) {

        if (lua_type(L, narg) != LUA_TFUNCTION)
            return luaL_error(L, "Missing callout handler.");

        lua_settop(L, narg);
        lua_setfield(L, LUA_REGISTRYINDEX, "l_callout");

        saved_L = L;    /* XXX race condition */

        if (signal(SIGALRM, h_signal) == SIG_ERR)
            return luab_pusherr(L, -1);
    }
    status = alarm(seconds);

    return (luab_pusherr(L, status));
}

/*
 * Service primitives.
 */

/***
 * access(2) - check availability of a file
 *
 * @function access
 *
 * @param path              Identifies the file by name.
 * @param mode              See the File Access Permission section of intro(2).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.access(path, mode)
 */
static int
luab_access(lua_State *L)
{
    const char *path;
    int mode, status;

    (void)luab_core_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    mode = (int)luab_checkinteger(L, 2, INT_MAX);
    status = access(path, mode);

    return (luab_pusherr(L, status));
}

/***
 * @function chdir(2) - change current working directory
 *
 * @param path              Points to the pathname of the directory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.chdir(path)
 */
static int
luab_chdir(lua_State *L)
{
    const char *path;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    status = chdir(path);

    return (luab_pusherr(L, status));
}

/***
 * chown(2) - change owner and group of a file
 *
 * @function chown
 *
 * @param path              Identifies the file by name.
 * @param owner             User ID.
 * @param group             Group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.chown(path, owner, group)
 */
static int
luab_chown(lua_State *L)
{
    const char *path;
    uid_t owner;
    gid_t group;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    owner = (uid_t)luab_checkinteger(L, 2, INT_MAX);
    group = (gid_t)luab_checkinteger(L, 3, INT_MAX);

    status = chown(path, owner, group);

    return (luab_pusherr(L, status));
}

/***
 * close(2) - delete a descriptor
 *
 * @function close
 *
 * @param fd                Open file descriptor.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.close(fd)
 */
static int
luab_close(lua_State *L)
{
    int fd, status;

    (void)luab_core_checkmaxargs(L, 1);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    status = close(fd);

    return (luab_pusherr(L, status));
}

/***
 * closefrom(2) - delete open file descriptors
 *
 * @function closefrom
 *
 * @param lowfd             Any file descriptor greater than or equal from the
 *                          per-process file descriptor table.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.closefrom(lowfd)
 */
static int
luab_closefrom(lua_State *L)
{
    int lowfd;

    (void)luab_core_checkmaxargs(L, 1);

    lowfd = (int)luab_checkinteger(L, 1, INT_MAX);
    closefrom(lowfd);

    return (luab_pusherr(L, 0));
}

/***
 * dup(2) - duplicate an existing file descriptor
 *
 * @function dup
 *
 * @param oldd              Small non-negative integer index in the per-process
 *                          descriptor table.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.dup(oldd)
 */
static int
luab_dup(lua_State *L)
{
    int oldd, fd;

    (void)luab_core_checkmaxargs(L, 1);

    oldd = (int)luab_checkinteger(L, 1, INT_MAX);
    fd = dup(oldd);

    return (luab_pusherr(L, fd));
}

/***
 * dup2(2) - duplicate an existing file descriptor
 *
 * @function dup2
 *
 * @param oldd              Small non-negative integer index in the per-process
 *                          descriptor table.
 * @param newd              The value for the new descriptor.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.dup2(oldd, newd)
 */
static int
luab_dup2(lua_State *L)
{
    int oldd, newd, fd;

    (void)luab_core_checkmaxargs(L, 2);

    oldd = (int)luab_checkinteger(L, 1, INT_MAX);
    newd = (int)luab_checkinteger(L, 2, INT_MAX);

    fd = dup2(oldd, newd);

    return (luab_pusherr(L, fd));
}

/*
 * XXX
 *  execl(2)
 *  execle(2)
 */

/***
 * execv(3) - execute a file
 *
 * @function execv
 *
 * @param path              Identifies the new process image file by its path.
 * @param argv              Argument vector:
 *
 *                              { "arg0" , "arg1" , ..., "argN" },
 *
 *                          instance of LUA_TTABLE.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.execv(path, argv)
 */
static int
luab_execv(lua_State *L)
{
    const char *path;
    luab_table_t *tbl;
    const char **argv;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);

    if ((tbl = luab_table_checkargv(L, 2)) != NULL)
        argv = (const char **)(tbl->tbl_vec);
    else
        argv = NULL;

    if (tbl != NULL) {
        status = execv(path, __DECONST(char **, argv));
        luab_table_free(tbl);
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * execve(2) - execute a files
 *
 * @function execve
 *
 * @param path              Identifies the new process image file by its path.
 * @param argv              Argument vector:
 *
 *                              { "arg0" , "arg1" , ..., "argN" },
 *
 *                          instance of LUA_TTABLE.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.execve(path, argv)
 */
static int
luab_execve(lua_State *L)
{
    const char *path;
    luab_table_t *tbl;
    const char **argv;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);

    if ((tbl = luab_table_checkargv(L, 2)) != NULL)
        argv = (const char **)(tbl->tbl_vec);
    else
        argv = NULL;

    if (tbl != NULL) {
        status = execve(path, __DECONST(char **, argv), environ);
        luab_table_free(tbl);
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * execvp(3) - execute a file
 *
 * @function execvp
 *
 * @param path              Identifies the new process image file by its path.
 * @param argv              Argument vector:
 *
 *                              { "arg0" , "arg1" , ..., "argN" },
 *
 *                          instance of LUA_TTABLE.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.execvp(path, argv)
 */
static int
luab_execvp(lua_State *L)
{
    const char *file;
    luab_table_t *tbl;
    const char **argv;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    file = luab_checklstring(L, 1, MAXPATHLEN);

    if ((tbl = luab_table_checkargv(L, 2)) != NULL)
        argv = (const char **)(tbl->tbl_vec);
    else
        argv = NULL;

    if (tbl != NULL) {
        status = execvp(file, __DECONST(char **, argv));
        luab_table_free(tbl);
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * fork(2) - create a new process
 *
 * @function fork
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pid [, err, msg ] = bsd.unistd.fork()
 */
static int
luab_fork(lua_State *L)
{
    pid_t pid;

    (void)luab_core_checkmaxargs(L, 0);
    pid = fork();

    return (luab_pusherr(L, pid));
}

/***
 * fpathconf(2) - get configurable pathname variables
 *
 * @function fpathconf
 *
 * @param fd                Open file descriptor.
 * @param name              Specifies the system variable over
 *
 *                              bsd.sys.unistd._PC_*
 *
 *                          to be queried.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage value [, err, msg ] = bsd.unistd.fpathconf(fd, name)
 */
static int
luab_fpathconf(lua_State *L)
{
    int fd, name, status;

    (void)luab_core_checkmaxargs(L, 2);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    name = (int)luab_checkinteger(L, 2, INT_MAX);

    status = fpathconf(fd, name);

    return (luab_pusherr(L, status));
}

/***
 * getcwd(3) - get working directory pathname
 *
 * @function getwd
 *
 * @param buf               Source location, (LUA_TUSERDATA(IOVEC)).
 * @param size              Size in bytes.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING}])
 *
 * @usage path [, err, msg ] = bsd.unistd.getwd(buf, size)
 */
static int
luab_getcwd(lua_State *L)
{
    luab_iovec_t *buf;
    size_t size;
    caddr_t bp;
    caddr_t status;

    (void)luab_core_checkmaxargs(L, 2);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    size = (size_t)luab_checkinteger(L, 2,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if (((bp = buf->iov.iov_base) != NULL) &&
        (size <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = getcwd(bp, size)) != NULL)
                buf->iov.iov_len = size;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = NULL;
        }
    } else {
        errno = ENXIO;
        status = NULL;
    }
    return (luab_pushstring(L, status));
}

/***
 * getegid(2) - get (effective) group process identification
 *
 * @function getegid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage egid [, err, msg ] = bsd.unistd.getegid()
 */
static int
luab_getegid(lua_State *L)
{
    gid_t egid;

    (void)luab_core_checkmaxargs(L, 0);
    egid = getegid();

    return (luab_pusherr(L, egid));
}

/***
 * geteuid(2) - get (effective) user identification
 *
 * @function geteuid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage euid [, err, msg ] = bsd.unistd.geteuid()
 */
static int
luab_geteuid(lua_State *L)
{
    uid_t euid;

    (void)luab_core_checkmaxargs(L, 0);
    euid = geteuid();

    return (luab_pusherr(L, euid));
}

/***
 * getgid(2) - get group process identification
 *
 * @function getgid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage gid [, err, msg ] = bsd.unistd.getgid()
 */
static int
luab_getgid(lua_State *L)
{
    gid_t gid;

    (void)luab_core_checkmaxargs(L, 0);
    gid = getgid();

    return (luab_pusherr(L, gid));
}

/***
 * getgroups(2) - get group access list
 *
 * @function getgroups
 *
 * @param gidsetlen         Number of entries that may be placed on
 *                          gidset, of successfull.
 * @param gidset            Empty instance of
 *
 *                              (LUA_TTABLE(LUA_TNUMBER,LUA_TNUMBER)),
 *
 *                          but still populated
 *
 *                              { "gid0" , "gid1" , ..., "gidN" },
 *
 *                          iff (if and only if)
 *
 *                              gidsetlen > 0.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage len [, err, msg ] = bsd.unistd.getgroups(gidsetlen, gidset)
 */
static int
luab_getgroups(lua_State *L)
{
    int gidsetlen;
    gid_t *gidset;
    int ngroups;
    int i, j;

    (void)luab_core_checkmaxargs(L, 2);

    gidsetlen = (int)luab_checkinteger(L, 1, INT_MAX);

    (void)luab_checktable(L, 2);

    if (gidsetlen != 0) {
        if ((gidset = alloca(gidsetlen * sizeof(gid_t))) == NULL)
            ngroups = -1;
        else
            ngroups = 0;
    } else {
        gidset = NULL;
        ngroups = 0;
    }

    if (ngroups == 0) {
        if ((ngroups = getgroups(gidsetlen, gidset)) > 0) {
            lua_pushnil(L); /* populate table, if any */

            for (i = 0, j = 1; i < gidsetlen; i++, j++)
                luab_rawsetinteger(L, 2, j, gidset[i]);

            lua_pop(L, 0);
        }
    }
    return (luab_pusherr(L, ngroups));
}

/***
 * getlogin(2) - get login name
 *
 * @function getlogin
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage name [, err, msg ] = bsd.unistd.getlogin()
 */
static int
luab_getlogin(lua_State *L)
{
    char *name;

    (void)luab_core_checkmaxargs(L, 0);

    name = getlogin();
    return (luab_pushstring(L, name));
}

/***
 * getpgrp(2) - get process group
 *
 * @function getpgrp
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pgrp [, err, msg ] = bsd.unistd.getpgrp()
 */
static int
luab_getpgrp(lua_State *L)
{
    pid_t pgrp;

    (void)luab_core_checkmaxargs(L, 0);
    pgrp = getpgrp();

    return (luab_pusherr(L, pgrp));
}

/***
 * getpid(2) - get calling process identification
 *
 * @function getpid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pid [, err, msg ] = bsd.unistd.getpid()
 */
static int
luab_getpid(lua_State *L)
{
    pid_t pid;

    (void)luab_core_checkmaxargs(L, 0);
    pid = getpid();

    return (luab_pusherr(L, pid));
}

/***
 * getppid(2) - get parent process identification
 *
 * @function getppid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ppid [, err, msg ] = bsd.unistd.getppid()
 */
static int
luab_getppid(lua_State *L)
{
    pid_t ppid;

    (void)luab_core_checkmaxargs(L, 0);
    ppid = getppid();

    return (luab_pusherr(L, ppid));
}

/***
 * getuid(2) - get user identification
 *
 * @function getuid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uid [, err, msg ] = bsd.unistd.getuid()
 */
static int
luab_getuid(lua_State *L)
{
    uid_t uid;

    (void)luab_core_checkmaxargs(L, 0);

    uid = getuid();

    return (luab_pusherr(L, uid));
}

/***
 * isatty(3) - determine, if valid terminal type device
 *
 * @function isatty
 *
 * @param fd                Open file descriptor.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.isatty(fd)
 */
static int
luab_isatty(lua_State *L)
{
    int fd, status;

    (void)luab_core_checkmaxargs(L, 1);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    status = isatty(fd);

    return (luab_pusherr(L, status));
}

/***
 * link(2) - make a hard file link
 *
 * @function link
 *
 * @param name1             Path or file name of underlying object.
 * @param name2             Path or file name where it points to name1.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.link(name1, name2)
 */
static int
luab_link(lua_State *L)
{
    const char *name1, *name2;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    name1 = luab_checklstring(L, 1, MAXPATHLEN);
    name2 = luab_checklstring(L, 2, MAXPATHLEN);

    status = link(name1, name2);

    return (luab_pusherr(L, status));
}

/***
 * lseek(2) - reposition read/write file offset
 *
 * @function lseek
 *
 * @param filedes           Open file descriptor.
 * @param offset            Offset according to the directive whence.
 * @param whence            Specifies the directive from
 *
 *                              bsd.sys.unistd.SEEK_{SET,CUR,END,HOLE,DATA}
 *
 *                          for repositioning.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage loc [, err, msg ] = bsd.unistd.lseek(filedes, offset, whence)
 */
static int
luab_lseek(lua_State *L)
{
    int filedes;
    off_t offset;
    int whence;
    off_t location;

    (void)luab_core_checkmaxargs(L, 3);

    filedes = (int)luab_checkinteger(L, 1, INT_MAX);
    offset = (off_t)luab_checkinteger(L, 2, ULONG_MAX);
    whence = (int)luab_checkinteger(L, 3, INT_MAX);

    location = lseek(filedes, offset, whence);

    return (luab_pusherr(L, location));
}

/***
 * pathconf(2) - get configurable pathname variables
 *
 * @function pathconf
 *
 * @param path              Name of file or directory.
 * @param name              Specifies the system variable over
 *
 *                              bsd.sys.unistd._PC_*
 *
 *                          to be queried.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage value [, err, msg ] = bsd.unistd.pathconf(path, name)
 */
static int
luab_pathconf(lua_State *L)
{
    const char *path;
    int name;
    long status;

    (void)luab_core_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    name = (int)luab_checkinteger(L, 2, INT_MAX);

    status = pathconf(path, name);

    return (luab_pusherr(L, status));
}
#ifdef notyet
/***
 * pause(3) - stop until signal
 *
 * @function pause
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage value [, err, msg ] = bsd.unistd.pause()
 */
static int
luab_pause(lua_State *L)
{
    int status;

    (void)luab_core_checkmaxargs(L, 0);

    status = pause();

    return (luab_pusherr(L, status));
}
#endif

/***
 * pipe(2) - create descriptor pair for interprocess communication
 *
 * @function pipe
 *
 * @param filedes           Pair of file descriptors
 *
 *                              { filedes1, filedes2 },
 *
 *                          instance of (LUA_TTABLE(LUA_TNUMBER,LUA_TNUMBER)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.pipe(filedes)
 */
static int
luab_pipe(lua_State *L)
{
    luab_table_t *tbl;
    int *fildes;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    if ((tbl = luab_table_checklint(L, 1, 2)) != NULL) {
        fildes = (int *)(tbl->tbl_vec);
        status = pipe(fildes);
        luab_table_free(tbl);
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * read(2) - read input
 *
 * @function read
 *
 * @param fd                Open file descriptor.
 * @param buf               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param nbytes            Assumed number of bytes to be read.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.unistd.read(fd, buf, nbytes)
 */
static int
luab_read(lua_State *L)
{
    int fd;
    luab_iovec_t *buf;
    size_t nbytes;

    (void)luab_core_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    buf = luab_udata(L, 2, luab_mx(IOVEC), luab_iovec_t *);
    nbytes = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    return (luab_iovec_read(L, fd, buf, &nbytes));
}

/***
 * rmdir(2) - remove a directory file
 *
 * @function rmdir
 *
 * @param path              The file to be removed.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.rmdir(path)
 */
static int
luab_rmdir(lua_State *L)
{
    const char *path;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    status = rmdir(path);

    return (luab_pusherr(L, status));
}

/***
 * setgid(2) - set group id
 *
 * @function setgid
 *
 * @param gid               Group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setgid(gid)
 */
static int
luab_setgid(lua_State *L)
{
    gid_t gid;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    gid = (gid_t)luab_checkinteger(L, 1, INT_MAX);
    status = setgid(gid);

    return (luab_pusherr(L, status));
}

/***
 * setpgid(2) - set process group
 *
 * @function setpgid
 *
 * @param pid               Process identifier.
 * @param pgrp              Process group for specefied process.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setpgid(pgrp)
 */
static int
luab_setpgid(lua_State *L)
{
    pid_t pid, pgrp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    pid = (pid_t)luab_checkinteger(L, 1, INT_MAX);
    pgrp = (pid_t)luab_checkinteger(L, 2, INT_MAX);

    status = setpgid(pid, pgrp);

    return (luab_pusherr(L, status));
}

/***
 * setsid(2) - create session and set process group ID
 *
 * @function setsid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sid [, err, msg ] = bsd.unistd.setsid()
 */
static int
luab_setsid(lua_State *L)
{
    pid_t sid;

    (void)luab_core_checkmaxargs(L, 0);

    sid = setsid();

    return (luab_pusherr(L, sid));
}

/***
 * setuid(2) - set user and group ID
 *
 * @function setuid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage uid [, err, msg ] = bsd.unistd.setuid()
 */
static int
luab_setuid(lua_State *L)
{
    uid_t uid;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    uid = (uid_t)luab_checkinteger(L, 1, INT_MAX);
    status = setuid(uid);

    return (luab_pusherr(L, status));
}

/***
 * sysconf(3) - get configuration system variable
 *
 * @function sysconf
 *
 * @param name              Specifies the system variable over
 *
 *                              bsd.unistd._SC_*
 *
 *                          to be queried.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING}])
 *
 * @usage value [, err, msg ] = bsd.unistd.sysconf()
 */
static int
luab_sysconf(lua_State *L)
{
    int name;
    long value;

    (void)luab_core_checkmaxargs(L, 1);

    name = (int)luab_checkinteger(L, 1, INT_MAX);
    value = sysconf(name);

    return (luab_pusherr(L, value));
}

/***
 * tcgetpgrp(3) - set foreground process group ID
 *
 * @function tcgetpgrp
 *
 * @param fd                File descriptor refers to a controlling
 *                          terminal device.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING}])
 *
 * @usage pgrp [, err, msg ] = bsd.unistd.tcgetpgrp()
 */
static int
luab_tcgetpgrp(lua_State *L)
{
    int fd;
    pid_t pgrp;

    (void)luab_core_checkmaxargs(L, 1);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    pgrp = tcgetpgrp(fd);

    return (luab_pusherr(L, pgrp));
}

/***
 * tcsetpgrp(3) - set foreground process group ID
 *
 * @function tcsetpgrp
 *
 * @param fd                File descriptor refers to a controlling terminal
 *                          device.
 * @param pgrp_id           The value must be the same as the process group ID
 *                          of a process in the same session as the calling
 *                          process.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING}])
 *
 * @usage ret [, err, msg ] = bsd.unistd.tcsetpgrp()
 */
static int
luab_tcsetpgrp(lua_State *L)
{
    int fd;
    pid_t pgrp_id;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    pgrp_id = (pid_t)luab_checkinteger(L, 2, INT_MAX);

    status = tcsetpgrp(fd, pgrp_id);

    return (luab_pusherr(L, status));
}

/***
 * ttyname(3) - get name from associated terminal (tty) from file descriptor
 *
 * @function ttyname
 *
 * @param fd                File descriptor refers to a valid terminal device.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING}])
 *
 * @usage name [, err, msg ] = bsd.unistd.ttyname()
 */
static int
luab_ttyname(lua_State *L)
{
    int fd;
    char *buf;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);

    if ((buf = ttyname(fd)) != NULL) {
        status = luab_pushstring(L, buf);
        free(buf);
    } else
        status = luab_pushnil(L);

    return (status);
}

/***
 * ttyname_r(3) - get name from associated terminal (tty) from file descriptor
 *
 * @function ttyname_r
 *
 * @param fd                File descriptor refers to a valid terminal device.
 * @param buf               Result argument, instance of (LUA_TUSERDATA(IOVEC)),
 *                          capable to hold requested user name.
 * @param len               Specifies the length in bytes for requested tty(4)
 *                          name.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.ttyname_r(buf, len)
 */
static int
luab_ttyname_r(lua_State *L)
{
    int fd;
    luab_iovec_t *buf;
    size_t len;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    buf = luab_udata(L, 2, luab_mx(IOVEC), luab_iovec_t *);
    len = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if (((bp = buf->iov.iov_base) != NULL) &&
        (len <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = ttyname_r(fd, bp, len)) == 0)
                buf->iov.iov_len = len;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * unlink(2) - remove a directory entry
 *
 * @function unlink
 *
 * @param path              The file to be removed.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.unlink(path)
 */
static int
luab_unlink(lua_State *L)
{
    const char *path;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    status = unlink(path);

    return (luab_pusherr(L, status));
}

/***
 * write(2) - write output
 *
 * @function write
 *
 * @param fd                Open file descriptor.
 * @param buf               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param nbytes            Number of bytes for write operation.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.unistd.write(fd, buf, nbytes)
 */
static int
luab_write(lua_State *L)
{
    int fd;
    luab_iovec_t *buf;
    size_t nbytes;

    (void)luab_core_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    buf = luab_udata(L, 2, luab_mx(IOVEC), luab_iovec_t *);
    nbytes = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    return (luab_iovec_write(L, fd, buf, &nbytes));
}

/* ISO/IEC 9945-1: 1996 */
#if __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE
/***
 * fsync(2) - synchronize changes to a file
 *
 * @function fsync
 *
 * @param fd                Open file descriptor.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.fsync(fd)
 */
static int
luab_fsync(lua_State *L)
{
    int fd, status;

    (void)luab_core_checkmaxargs(L, 1);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);

    status = fsync(fd);

    return (luab_pusherr(L, status));
}

/***
 * fdatasync(2) - synchronize changes to a file
 *
 * @function fsync
 *
 * @param fd                Open file descriptor.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.fdatasync(fd)
 */
static int
luab_fdatasync(lua_State *L)
{
    int fd, status;

    (void)luab_core_checkmaxargs(L, 1);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);

    status = fdatasync(fd);

    return (luab_pusherr(L, status));
}

/***
 * ftruncate(2) - truncate/extend a file to a specific length
 *
 * @function ftruncate
 *
 * @param fd                File descriptor.
 * @param length            If the file was larger than this size, the extra
 *                          data is lost. If the file was smaller than this
 *                          size, it will be extended as if by writing bytes
 *                          with the value zero.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.ftruncate(path)
 */
static int
luab_ftruncate(lua_State *L)
{
    int fd;
    off_t length;
    int status;

    luab_core_checkmaxargs(L, 2);

    fd = (int)luab_checkinteger(L, 1, MAXPATHLEN);
    length = (off_t)luab_checkinteger(L, 2, LONG_MAX);

    status = ftruncate(fd, length);

    return (luab_pusherr(L, status));
}
#endif /* __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE */

#if __POSIX_VISIBLE >= 199506
/***
 * getlogin_r(2) - get login name
 *
 * @function getlogin_r
 *
 * @param name              Buffer, instance of (LUA_TUSERDATA(IOVEC)), capable
 *                          hold requested user name.
 * @param len               Specifies the length in bytes of requested user name.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.getlogin_r(buf, len)
 */
static int
luab_getlogin_r(lua_State *L)
{
    luab_iovec_t *buf;
    size_t len;
    caddr_t name;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *); /* XXX macro might defined */
    len = (size_t)luab_checkinteger(L, 2, INT_MAX);

    if (((name = buf->iov.iov_base) != NULL) &&
        (len <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = getlogin_r(name, len)) == 0)
                buf->iov.iov_len = len;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}
#endif

/* 1003.1-2001 */
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
/***
 * fchown(2) - change owner and group of a file
 *
 * @function fchown
 *
 * @param fd                Open file descriptor.
 * @param owner             User ID.
 * @param group             Group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.fchown(path)
 */
static int
luab_fchown(lua_State *L)
{
    int fd;
    uid_t owner;
    gid_t group;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    owner = (uid_t)luab_checkinteger(L, 2, INT_MAX);
    group = (gid_t)luab_checkinteger(L, 3, INT_MAX);

    status = fchown(fd, owner, group);

    return (luab_pusherr(L, status));
}

/***
 * readlink(2) - read value of a symbolic link
 *
 * @function readlink
 *
 * @param path              Symbolic link.
 * @param buf               Holds read contents from symbolic link.
 * @param bufsiz            Assumed number of bytes to be read.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.unistd.readlink(path, buf, bufsiz)
 */
static int
luab_readlink(lua_State *L)
{
    const char * path;
    luab_iovec_t *buf;
    size_t bufsiz;

    (void)luab_core_checkmaxargs(L, 3);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    buf = luab_udata(L, 2, luab_mx(IOVEC), luab_iovec_t *);
    bufsiz = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    return (luab_iovec_readlink(L, path, buf, &bufsiz));
}
#endif /* __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE */

#if __POSIX_VISIBLE >= 200112
/***
 * gethostname(3) - get name of current host
 *
 * @function gethostname
 *
 * @param name              Instance of (LUA_TUSERDATA(IOVEC)) capable
 *                          to hold requested hostname.
 * @param namelen           Constraint, specifies length.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.gethostname(name, namelen)
 */
static int
luab_gethostname(lua_State *L)
{
    luab_iovec_t *buf;
    size_t namelen;
    caddr_t dp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    namelen = (size_t)luab_checkinteger(L, 2,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if (((dp = buf->iov.iov_base) != NULL) &&
        (namelen <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = gethostname(dp, namelen)) == 0)
                buf->iov.iov_len = namelen;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * setegid(2) - set effective group ID
 *
 * @function setegid
 *
 * @param egid              Effective group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setegid(egid)
 */
static int
luab_setegid(lua_State *L)
{
    gid_t egid;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    egid = (gid_t)luab_checkinteger(L, 1, INT_MAX);
    status = setegid(egid);

    return (luab_pusherr(L, status));
}

/***
 * seteuid(2) - set effective user ID
 *
 * @function seteuid
 *
 * @param euid              Effective user ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.seteuid(euid)
 */
static int
luab_seteuid(lua_State *L)
{
    uid_t euid;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    euid = (uid_t)luab_checkinteger(L, 1, INT_MAX);
    status = seteuid(euid);

    return (luab_pusherr(L, status));
}
#endif /* __POSIX_VISIBLE >= 200112 */

/* 1003.1-2008 */
#if __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE
/***
 * getsid(2) - get process session ID
 *
 * @function getsid
 *
 * @param pid               Process ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sid [, err, msg ] = bsd.unistd.getsid(pid)
 */
static int
luab_getsid(lua_State *L)
{
    pid_t pid, sid;

    (void)luab_core_checkmaxargs(L, 1);

    pid = (pid_t)luab_checkinteger(L, 1, INT_MAX);
    sid = getsid(pid);

    return (luab_pusherr(L, sid));
}

/***
 * fchdir(2) - change current working directory
 *
 * @function fchdir
 *
 * @param fd                File descriptor of directory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sid [, err, msg ] = bsd.unistd.fchdir(fd)
 */
static int
luab_fchdir(lua_State *L)
{
    int fd, status;

    (void)luab_core_checkmaxargs(L, 1);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    status = fchdir(fd);

    return (luab_pusherr(L, status));
}

/***
 * getpgid(2) - get current process group by pid
 *
 * @function getpgid
 *
 * @param pid               Process ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.getpgid(pid)
 */
static int
luab_getpgid(lua_State *L)
{
    pid_t pid, pgrp;

    (void)luab_core_checkmaxargs(L, 1);

    pid = (pid_t)luab_checkinteger(L, 1, INT_MAX);
    pgrp = getpgid(pid);

    return (luab_pusherr(L, pgrp));
}

/***
 * lchown(2) - change owner and group of a file
 *
 * @function lchown
 *
 * @param path              Specifies the file by its name.
 * @param owner             Specifies the owner by User ID.
 * @param group             Group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.lchown(path, owner, group)
 */
static int
luab_lchown(lua_State *L)
{
    const char *path;
    uid_t owner;
    gid_t group;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    owner = (uid_t)luab_checkinteger(L, 2, INT_MAX);
    group = (gid_t)luab_checkinteger(L, 3, INT_MAX);

    status = lchown(path, owner, group);

    return (luab_pusherr(L, status));
}

/***
 * pread(2) - read input
 *
 * @function pread
 *
 * @param fd                Open file descriptor.
 * @param buf               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param nbytes            Assumed number of bytes to be read.
 * @param offset            Location, where data to be read.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.unistd.pread(fd, buf, nbytes, offset)
 */
static int
luab_pread(lua_State *L)
{
    int fd;                 /* XXX wrong */
    luab_iovec_t *buf;
    size_t nbytes;
    off_t offset;

    (void)luab_core_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    buf = luab_udata(L, 2, luab_mx(IOVEC), luab_iovec_t *);
    nbytes = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    offset = (off_t)luab_checkinteger(L, 4, LONG_MAX);

    return (luab_iovec_pread(L, fd, buf, &nbytes, offset));
}

/***
 * pwrite(2) - write output
 *
 * @function pwrite
 *
 * @param fd                Open file descriptor.
 * @param buf               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param nbytes            Number of bytes for write operation.
 * @param offset            Specifies position for write operation.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.unistd.pwrite(fd, buf, nbytes, offset)
 */
static int
luab_pwrite(lua_State *L)
{
    int fd;
    luab_iovec_t *buf;
    size_t nbytes;
    off_t offset;

    (void)luab_core_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    buf = luab_udata(L, 2, luab_mx(IOVEC), luab_iovec_t *);
    nbytes = (size_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    offset = (off_t)luab_checkinteger(L, 4, LONG_MAX);

    return (luab_iovec_pwrite(L, fd, buf, &nbytes, offset));
}

/***
 * truncate(2) - truncate / extend a file to a specific length
 *
 * @function truncate
 *
 * @param path              Identifies the file by name.
 * @param length            If the file was larger than this size, the extra
 *                          data is lost. If the file was smaller than this
 *                          size, it will be extended as if by writing bytes
 *                          with the value zero.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.truncate(path, length)
 */
static int
luab_truncate(lua_State *L)
{
    const char *path;
    off_t length;
    int status;

    luab_core_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    length = (off_t)luab_checkinteger(L, 2, LONG_MAX);

    status = truncate(path, length);

    return (luab_pusherr(L, status));
}
#endif /* __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE */

#if __POSIX_VISIBLE >= 200809
/***
 * faccessat(2) - check accessibility of a file
 *
 * @function faccessat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes referenced file.
 *
 *                            #2 By path named file is relative to the directory
 *                               associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of chflagsat(2).
 *
 * @param path              Name or path of referred file.
 * @param mode              Specifies mode setting.
 * @param flag              The values are constructed from
 *
 *                              bsd.fcntl.AT_SYMLINK_NOFOLLOW
 *
 *                          by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.faccessat(fd, path, owner, group, flag)
 */
static int
luab_faccessat(lua_State *L)
{
    int fd;
    const char *path;
    int mode;
    int flag;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    mode = (int)luab_checkinteger(L, 3, INT_MAX);
    flag = (int)luab_checkinteger(L, 4, INT_MAX);

    status = faccessat(fd, path, mode, flag);

    return (luab_pusherr(L, status));
}

/***
 * fchownat(2) - change owner and group of a file
 *
 * @function fchownat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes referenced file.
 *
 *                            #2 By path named file is relative to the directory
 *                               associated with the file descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of fchownat(2).
 *
 * @param owner             Specifies user ID.
 * @param group             Specifies group ID.
 * @param flag              The values are constructed from
 *
 *                              bsd.fcntl.AT_SYMLINK_NOFOLLOW
 *
 *                          by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.fchownat(fd, path, owner, group, flag)
 */
static int
luab_fchownat(lua_State *L)
{
    int fd;
    const char *path;
    uid_t owner;
    gid_t group;
    int flag;
    int status;

    (void)luab_core_checkmaxargs(L, 5);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    owner = (uid_t)luab_checkinteger(L, 3, INT_MAX);
    group = (gid_t)luab_checkinteger(L, 4, INT_MAX);
    flag = (int)luab_checkinteger(L, 5, INT_MAX);

    status = fchownat(fd, path, owner, group, flag);

    return (luab_pusherr(L, status));
}

/***
 * fexecve(2) - execute a file
 *
 * @function fexecve
 *
 * @param fd                Identifies the new process image file by open file
 *                          descriptor.
 * @param argv              Argument vector:
 *
 *                              { "arg0" , "arg1" , ..., "argN" },
 *
 *                          instance of (LUA_TTABLE(LUA_TNUMBER,LUA_TSTRING)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.fexecve(fd, argv)
 */
static int
luab_fexecve(lua_State *L)
{
    int fd;
    luab_table_t *tbl;
    const char **argv;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);

    if ((tbl = luab_table_checkargv(L, 2)) != NULL)
        argv = (const char **)(tbl->tbl_vec);
    else
        argv = NULL;

    if (tbl != NULL) {
        status = fexecve(fd, __DECONST(char **, argv), environ);
        luab_table_free(tbl);
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * linkat(2) - make a hard file link
 *
 * @function linkat
 *
 * @param fd1               File descriptor associated with working directory
 *                          from object pointed by name1 or associated with the
 *                          object pointed by name1.
 * @param name1             By name2 referred underlying object.
 * @param fd2               File descriptor associated with working directory
 *                          from object pointed by name2 or associated with the
 *                          object pointed by name2.
 * @param name2             Name of file link referring object identified by
 *                          name1.
 * @param flag              The values are constructed from
 *
 *                              bsd.fcntl.AT_SYMLINK_NOFOLLOW
 *
 *                          by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.linkat(fd1, name1, fd2, name2, flag)
 */
static int
luab_linkat(lua_State *L)
{
    int fd1;
    const char *name1;
    int fd2;
    const char *name2;
    int flag, status;

    (void)luab_core_checkmaxargs(L, 5);

    fd1 = (int)luab_checkinteger(L, 1, INT_MAX);
    name1 = luab_checklstring(L, 2, MAXPATHLEN);
    fd2 = (int)luab_checkinteger(L, 3, INT_MAX);
    name2 = luab_checklstring(L, 4, MAXPATHLEN);
    flag = (int)luab_checkinteger(L, 5, INT_MAX);

    status = linkat(fd1, name1, fd2, name2, flag);

    return (luab_pusherr(L, status));
}

/***
 * readlinkat(2) - read value of a symbolic link
 *
 * @function readlinkat
 *
 * @param fd                Open file descriptor.
 * @param path              Symbolic link.
 * @param buf               Holds read contents from symbolic link.
 * @param bufsize           Assumed number of bytes to be read.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage count [, err, msg ] = bsd.unistd.readlinkat(fd, path, buf, bufsize)
 */
static int
luab_readlinkat(lua_State *L)
{
    int fd;
    const char *path;
    luab_iovec_t *buf;
    size_t bufsize;

    (void)luab_core_checkmaxargs(L, 4);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    buf = luab_udata(L, 3, luab_mx(IOVEC), luab_iovec_t *);
    bufsize = (size_t)luab_checkinteger(L, 4,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    return (luab_iovec_readlinkat(L, fd, path, buf, &bufsize));
}

/***
 * symlinkat(2) - make symbolic link to a file
 *
 * @function symlinkat
 *
 * @param name1             File name from target.
 * @param fd                Denotes either file descriptor associated with
 *                          directory where its relative to the symbolic link
 *                          or specifies by fd accepted parameter
 *
 *                              bsd.unistd.AT_FDCWD.
 *
 * @param name2             Denotes assumed symbolic link maps to file name1.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.symlinkat(name1, fd, name2)
 */
static int
luab_symlinkat(lua_State *L)
{
    const char *name1;
    int fd;
    const char *name2;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    name1 = luab_checklstring(L, 1, MAXPATHLEN);
    fd = (int)luab_checkinteger(L, 2, INT_MAX);
    name2 = luab_checklstring(L, 3, MAXPATHLEN);

    status = symlinkat(name1, fd, name2);

    return (luab_pusherr(L, status));
}

/***
 * unlinkat(2) - remove directory entry
 *
 * @function unlinkat
 *
 * @param fd                File descriptor associated with working directory
 *                          from object pointed by name1 or associated with the
 *                          object pointed by name1.
 * @param path              Link identified by path.
 * @param flag              Values are constructed from
 *
 *                              bsd.fcntl.AT_SYMLINK_NOFOLLOW
 *
 *                          by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.unlinkat(fd, path, flag)
 */
static int
luab_unlinkat(lua_State *L)
{
    int fd;
    const char *path;
    int flag, status;

    (void)luab_core_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    flag = (int)luab_checkinteger(L, 3, INT_MAX);

    status = unlinkat(fd, path, flag);

    return (luab_pusherr(L, status));
}
#endif /* __POSIX_VISIBLE >= 200809 */
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE >= 402
/***
 * symlink(2) - make symbolic link to a file
 *
 * @function symlink
 *
 * @param name1             File name from target.
 * @param name2             Denotes assumed symbolic link maps to file name1.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.symlink(name1, name2)
 */
static int
luab_symlink(lua_State *L)
{
    const char *name1;
    const char *name2;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    name1 = luab_checklstring(L, 1, MAXPATHLEN);
    name2 = luab_checklstring(L, 3, MAXPATHLEN);

    status = symlink(name1, name2);

    return (luab_pusherr(L, status));
}
#endif /* __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE >= 402 */

/* X/Open System Interfaces */
#if __XSI_VISIBLE
/***
 * crypt(3) - Trapdoor encryption
 *
 * @function crypt
 *
 * @param key               Data to hash.
 * @param salt              String specifies salt in
 *
 *                           #1 Extended,
 *
 *                           #2 Modular or
 *
 *                           #3 Traditional
 *
 *                          form.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage value [, err, msg ] = bsd.unistd.crypt(key, salt)
 */
static int
luab_crypt(lua_State *L)
{
    const char *key;
    const char *salt;
    caddr_t value;

    (void)luab_core_checkmaxargs(L, 2);

    key = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    salt = luab_checklstring(L, 2, LUAL_BUFFERSIZE);

    value = crypt(key, salt);

    return (luab_pushstring(L, value));
}

/***
 * gethostid(3) - get unique identifier of current host
 *
 * @function gethostid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage hostid [, err, msg ] = bsd.unistd.gethostid()
 */
static int
luab_gethostid(lua_State *L)
{
    long hostid;

    (void)luab_core_checkmaxargs(L, 0);

    hostid = gethostid();

    return (luab_pusherr(L, hostid));
}

/***
 * lockf(3) - record locking on files
 *
 * @function lockf
 *
 * @param fd                Open file descriptor.
 * @param function          Control value specifies assumed action from
 *
 *                              bsd.unistd.F_{ULOCK,LOCK,TLOCK,TEST}
 *
 *                          to be taken.
 * @param size              Number of contigous bytes to be locked or unlocked.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.lockf(fd, function, size)
 */
static int
luab_lockf(lua_State *L)
{
    int fd;
    int function;
    off_t size;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    function = (int)luab_checkinteger(L, 2, INT_MAX);
    size = (off_t)luab_checkinteger(L, 3, LONG_MAX);

    status = lockf(fd, function, size);

    return (luab_pusherr(L, status));
}

/***
 * nice(3) - set programm scheduling priority
 *
 * @function nice
 *
 * @param incr              Scheduling priority of the process.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.nice(incr)
 */
static int
luab_nice(lua_State *L)
{
    int incr, status;

    (void)luab_core_checkmaxargs(L, 1);

    incr = (int)luab_checkinteger(L, 1, INT_MAX);

    status = nice(incr);

    return (luab_pusherr(L, status));
}

/***
 * setregid(2) - set real and effective group ID
 *
 * @function setregid
 *
 * @param rgid              Real group ID.
 * @param egid              Effective group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setregid(rgid, egid)
 */
static int
luab_setregid(lua_State *L)
{
    gid_t rgid, egid;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    rgid = (gid_t)luab_checkinteger(L, 1, INT_MAX);
    egid = (gid_t)luab_checkinteger(L, 2, INT_MAX);

    status = setregid(rgid, egid);

    return (luab_pusherr(L, status));
}

/***
 * setreuid(2) - set real and effective user ID
 *
 * @function setreuid
 *
 * @param ruid              Real user ID.
 * @param euid              Effective user ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setreuid(ruid, euid)
 */
static int
luab_setreuid(lua_State *L)
{
    uid_t ruid, euid;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    ruid = (uid_t)luab_checkinteger(L, 1, INT_MAX);
    euid = (uid_t)luab_checkinteger(L, 2, INT_MAX);

    status = setreuid(ruid, euid);

    return (luab_pusherr(L , status));
}

/***
 * swab(3) - swap adjacent bytes
 *
 * @function swab
 *
 * @param in_buf            Source location, (LUA_TUSERDATA(IOVEC)).
 * @param out_buf           Destination location, (LUA_TUSERDATA(IOVEC)).
 * @param len               Length.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.swab(in_buf, out_buf, len)
 */
static int
luab_swab(lua_State *L)
{
    luab_iovec_t *in_buf, *out_buf;
    ssize_t len;
    caddr_t src, dst;

    (void)luab_core_checkmaxargs(L, 3);

    in_buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    out_buf = luab_udata(L, 2, luab_mx(IOVEC), luab_iovec_t *);
    len = (ssize_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    int status;

    if ((in_buf->iov.iov_len == out_buf->iov.iov_len) &&
        ((src = in_buf->iov.iov_base) != NULL) &&
        ((dst = out_buf->iov.iov_base) != NULL) &&
        (len <= (ssize_t)out_buf->iov.iov_len) &&
        (in_buf->iov_flags & IOV_BUFF) &&
        (out_buf->iov_flags & IOV_BUFF)) {

        if (((in_buf->iov_flags & IOV_LOCK) == 0) &&
            ((out_buf->iov_flags & IOV_LOCK) == 0)) {
            in_buf->iov_flags |= IOV_LOCK;
            out_buf->iov_flags |= IOV_LOCK;

            swab(src, dst, len);
            status = 0;

            out_buf->iov_flags &= ~IOV_LOCK;
            in_buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * sync(2) - schedule file system updates
 *
 * @function sync
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.sync()
 */
static int
luab_sync(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);

    sync();

    return (luab_pusherr(L, 0));
}
#endif /* __XSI_VISIBLE */

#if (__XSI_VISIBLE && __XSI_VISIBLE <= 500) || __BSD_VISIBLE
/***
 * chroot(2) - change root directory
 *
 * @function chroot
 *
 * @param dirname           The pathname of a directory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.chroot(dirname)
 */
static int
luab_chroot(lua_State *L)
{
    const char *dirname;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    dirname = luab_checklstring(L, 1, MAXPATHLEN);
    status = chroot(dirname);

    return (luab_pusherr(L, status));
}

/***
 * getdtablesize(2) - get filedescriptor limit
 *
 * @function getdtablesize
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.getdtablesize()
 */
static int
luab_getdtablesize(lua_State *L)
{
    int size;

    (void)luab_core_checkmaxargs(L, 0);

    size = getdtablesize();

    return (luab_pusherr(L, size));
}

/***
 * getpagesize(3) - get system page size
 *
 * @function getpagesize
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.getpagesize()
 */
static int
luab_getpagesize(lua_State *L)
{
    int size;

    (void)luab_core_checkmaxargs(L, 0);

    size = getpagesize();

    return (luab_pusherr(L, size));
}

/***
 * getpass(3) - get a password
 *
 * @function getpass
 *
 * @param prompt            String denotes prompt.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage value [, err, msg ] = bsd.unistd.getpass(prompt)
 */
static int
luab_getpass(lua_State *L)
{
    const char *prompt;
    caddr_t value;

    (void)luab_core_checkmaxargs(L, 1);

    prompt = luab_checklstring(L, 1, _PASSWORD_LEN);
    value = getpass(prompt);

    return (luab_pushstring(L, value));
}
#endif /* (__XSI_VISIBLE && __XSI_VISIBLE <= 500) || __BSD_VISIBLE */

#if (__XSI_VISIBLE && __XSI_VISIBLE <= 600) || __BSD_VISIBLE
/***
 * getwd(3) - get working directory pathname
 *
 * @function getwd
 *
 * @param buf               Source location, (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING}])
 *
 * @usage path [, err, msg ] = bsd.unistd.getwd(buf)
 */
static int
luab_getwd(lua_State *L)
{
    luab_iovec_t *buf;
    caddr_t bp, status;

    (void)luab_core_checkmaxargs(L, 1);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (MAXPATHLEN <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = getwd(bp)) != NULL)
                buf->iov.iov_len = strlen(bp);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = ENXIO;
            status = NULL;
        }
    } else {
        errno = EBUSY;
        status = NULL;
    }
    return (luab_pushstring(L, status));
}

/***
 * usleep(3) - suspend process execution for an interval measured in microseconds
 *
 * @function usleep
 *
 * @param microseconds      For suspension specified number of microseconds.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.usleep(microseconds)
 */
static int
luab_usleep(lua_State *L)
{
    useconds_t microseconds;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    microseconds = (useconds_t)luab_checkinteger(L, 1, INT_MAX);
    status = usleep(microseconds);

    return (luab_pusherr(L, status));
}

/***
 * vfork(2) - create a new process without copying the address space
 *
 * @function vfork
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.vfork()
 */
static int
luab_vfork(lua_State *L)
{
    pid_t pid;

    (void)luab_core_checkmaxargs(L, 0);

    pid = vfork();

    return (luab_pusherr(L, pid));
}
#endif /* (__XSI_VISIBLE && __XSI_VISIBLE <= 600) || __BSD_VISIBLE */

#if __BSD_VISIBLE
/***
 * acct(2) - enable or disable process accounting
 *
 * @function acct
 *
 * @param file              Existing pathname.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.acct(file)
 */
static int
luab_acct(lua_State *L)
{
    const char *file;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    file = luab_checklstring(L, 1, MAXPATHLEN);
    status = acct(file);

    return (luab_pusherr(L, status));
}

/***
 * check_utility_compat(3) - determine wether a utility should be compatible
 *
 * @function check_utility_compat
 *
 * @param utility           Existing pathname.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage compat [, err, msg ] = bsd.unistd.check_utility_compat(utility)
 */
static int
luab_check_utility_compat(lua_State *L)
{
    const char *utility;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    utility = luab_checklstring(L, 1, MAXPATHLEN);
    status = check_utility_compat(utility);

    return (luab_pusherr(L, status));
}

/***
 * crypt_get_format(3) - trapdoor encryption
 *
 * @function crypt_get_format
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING}])
 *
 * @usage format [, err, msg ] = bsd.unistd.crypt_get_format()
 */
static int
luab_crypt_get_format(lua_State *L)
{
    const char *format;

    (void)luab_core_checkmaxargs(L, 0);

    format = crypt_get_format();

    return (luab_pushstring(L, format));
}

/***
 * crypt_r(3) - Trapdoor encryption
 *
 * @function crypt_r
 *
 * @param key               Data to hash.
 * @param salt              String specifies salt in
 *
 *                           #1 Extended,
 *
 *                           #2 Modular or
 *
 *                           #3 Traditional
 *
 *                          form.
 * @param data              Instance of (LUA_TUSERDATA(CRYPT_DATA)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage value [, err, msg ] = bsd.unistd.crypt_r(key, salt, data)
 */
static int
luab_crypt_r(lua_State *L)
{
    const char *key;
    const char *salt;
    struct crypt_data *data;
    const char *value;

    (void)luab_core_checkmaxargs(L, 3);

    key = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    salt = luab_checklstring(L, 2, LUAL_BUFFERSIZE);
    data = luab_udata(L, 3, luab_mx(CRYPT_DATA), struct crypt_data *);

    value = crypt_r(key, salt, data);

    return (luab_pushstring(L, value));
}

/***
 * crypt_set_format(3) - trapddor encryption
 *
 * @function crypt_set_format
 *
 * @param string            Specifies encoding format.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage compat [ err, msg ] = bsd.unistd.crypt_set_format(string)
 */
static int
luab_crypt_set_format(lua_State *L)
{
    const char *string;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    string = luab_checklstring(L, 1, MAXPATHLEN);
    status = crypt_set_format(string);

    return (luab_pusherr(L, status));
}

/***
 * dup3(2) - duplicate an existing file descriptor
 *
 * @function dup3
 *
 * @param oldd              Small non-negative integer index in the per-process
 *                          descriptor table.
 * @param newd              Small non-negative integer index in the per-process
 *                          descriptor table.
 * @param flags             Only the close-on-exit
 *
 *                              bsd.fcntl.O_CLOEXEC
 *
 *                          flag is allowed.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.dup3(path, mode)
 */
static int
luab_dup3(lua_State *L)
{
    int oldd;
    int newd;
    int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    oldd = (int)luab_checkinteger(L, 1, INT_MAX);
    newd = (int)luab_checkinteger(L, 2, INT_MAX);
    flags = (int)luab_checkinteger(L, 3, INT_MAX);

    status = dup3(oldd, newd, flags);

    return (luab_pusherr(L, status));
}

/***
 * eaccess(2) - check availability of a file
 *
 * @function eaccess
 *
 * @param path              Identifies the file by name.
 * @param mode              See the File Access Permission section of intro(2).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.eaccess(path, mode)
 */
static int
luab_eaccess(lua_State *L)
{
    const char *path;
    int mode, status;

    (void)luab_core_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    mode = (int)luab_checkinteger(L, 2, INT_MAX);
    status = eaccess(path, mode);

    return (luab_pusherr(L, status));
}

/***
 * endusershell(3) - get valid user shells
 *
 * @function endusershell
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.endusershell()
 */
static int
luab_endusershell(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);

    endusershell();

    return (luab_pusherr(L, 0));
}

/***
 * exect(3) - execute a file
 *
 * @function exect
 *
 * @param path              Identifies the new process image file by its path.
 * @param argv              Argument vector:
 *
 *                              { "arg0" , "arg1" , ..., "argN" },
 *
 *                          instance of (LUA_TTABLE(LUA_TNUMBER,LUA_TSTRING)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.exect(path, argv)
 */
static int
luab_exect(lua_State *L)
{
    const char *path;
    luab_table_t *tbl;
    const char **argv;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);

    if ((tbl = luab_table_checkargv(L, 2)) != NULL)
        argv = (const char **)(tbl->tbl_vec);
    else
        argv = NULL;

    if (tbl != NULL) {
        status = exect(path, __DECONST(char **, argv), environ);
        luab_table_free(tbl);
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * execvP(3) - execute a file
 *
 * @function execvP
 *
 * @param file              Identifies the new process image file by its path.
 * @param search_path       Search path.
 * @param argv              Argument vector:
 *
 *                              { "arg0" , "arg1" , ..., "argN" },
 *
 *                          instance of (LUA_TTABLE(LUA_TNUMBER,LUA_TSTRING)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.execvP(file, search_path, argv)
 */
static int
luab_execvP(lua_State *L)
{
    const char *file;
    const char *search_path;
    luab_table_t *tbl;
    const char **argv;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    file = luab_checklstring(L, 1, MAXPATHLEN);
    search_path = luab_checklstring(L, 2, MAXPATHLEN);

    if ((tbl = luab_table_checkargv(L, 2)) != NULL)
        argv = (const char **)(tbl->tbl_vec);
    else
        argv = NULL;

    if (tbl != NULL) {
        status = execvP(file, search_path, __DECONST(char **, argv));
        luab_table_free(tbl);
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * feature_present(3) - query presence of a kernel feature
 *
 * @function feature_present
 *
 * @param feature               Name of feature to check.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.feature_present(feature)
 */
static int
luab_feature_present(lua_State *L)
{
    const char *feature;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    feature = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    status = feature_present(feature);

    return (luab_pusherr(L, status));
}

/***
 * fflagstostr(3) - query presence of a kernel feature
 *
 * @function fflagstostr
 *
 * @param flags             Flags as described in chflags(1).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage str [, err, msg ] = bsd.unistd.fflagstostr(flags)
 */
static int
luab_fflagstostr(lua_State *L)
{
    u_long flags;
    char *str;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    flags = (u_long)luab_checkinteger(L, 1, LONG_MAX);

    str = fflagstostr(flags);
    status = luab_pushstring(L, str);
    free(str);

    return (status);
}

/***
 * getdomainname(3) - get NIS domainname of current host
 *
 * @function getdomainname
 *
 * @param name              Instance of (LUA_TUSERDATA(IOVEC)).
 * @param namelen           Maximum size of buffer maps to name.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.getdomainname(name, namelen)
 */
static int
luab_getdomainname(lua_State *L)
{
    luab_iovec_t *buf;
    size_t namelen;
    caddr_t name;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    namelen = (size_t)luab_checkinteger(L, 2,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if (((name = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= MAXHOSTNAMELEN) &&
        (namelen <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = getdomainname(name, namelen)) == 0)
                buf->iov.iov_len = namelen;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * getentropy(3) - get entropy
 *
 * @function getentropy
 *
 * @param buf               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param buflen            Maximum capacity for used buffer.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.getentropy(buf, buflen)
 */
static int
luab_getentropy(lua_State *L)
{
    luab_iovec_t *buf;
    size_t buflen;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    buflen = (size_t)luab_checkinteger(L, 2,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if (((bp = buf->iov.iov_base) != NULL) &&
        (MAX_INPUT <= buf->iov_max_len) &&
        (buflen <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = getentropy(bp, buflen)) == 0)
                buf->iov.iov_len = buflen;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * getgrouplist(3) - calculate group access list
 *
 * @function getgrouplist
 *
 * @param name              Group name.
 * @param basegid           Base group ID.
 * @param gidset            Empty instance of
 *
 *                              (LUA_TTABLE(LUA_TNUMBER,LUA_TNUMBER)),
 *
 *                          but still populated by
 *
 *                              { "gid0" , "gid1" , ..., "gidN" },
 *
 *                          if query by (name, basegid) was successfull.
 *
 * @param ngroups           Size, instance of (LUA_TUSERDATA(PRIMITIVE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.getgrouplist(gidsetlen, gidset)
 */
static int
luab_getgrouplist(lua_State *L)
{
    const char *name;
    gid_t basegid;
    luab_table_t *tbl;
    gid_t *gidset;
    luab_primitive_u *xp;
    size_t *ngroups;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    name = luab_checklstring(L, 1, NAME_MAX);
    basegid = (gid_t)luab_checkinteger(L, 2, INT_MAX);

    (void)luab_checkltable(L, 3, 0);  /* only empty tables are accepted */

    xp = luab_udata(L, 4, luab_mx(PRIMITIVE), luab_primitive_u *);
    ngroups = &(xp->un_size);

    if (*ngroups != 0) {

        if ((tbl = luab_table_alloc(L, 3, *ngroups, sizeof(gid_t))) != NULL) {
            gidset = (gid_t *)(tbl->tbl_vec);

            if ((status = getgrouplist(name, basegid, gidset, (int *)ngroups)) == 0)
                luab_table_pushgid(L, 3, tbl, 0, 1);
        } else
            status = -1;
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * getloginclass(2) - get login class
 *
 * @function getloginclass
 *
 * @param name              Instance of (LUA_TUSERDATA(IOVEC)).
 * @param len               Maximum capacity for used buffer.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.getloginclass(name, len)
 */
static int
luab_getloginclass(lua_State *L)
{
    luab_iovec_t *buf;
    size_t len;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    len = (size_t)luab_checkinteger(L, 2,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= MAXLOGNAME) &&
        (len <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = getloginclass(bp, len)) == 0)
                buf->iov.iov_len = len;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * getmode(2) - modify mode bits
 *
 * @function getmode
 *
 * @param set               Instance of (LUA_TUSERDATA(IOVEC)).
 * @param mode              Mode bits.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage mode [, err, msg ] = bsd.unistd.getmode(set, mode)
 */
static int
luab_getmode(lua_State *L)
{
    luab_iovec_t *buf;
    mode_t mode;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    mode = (mode_t)luab_checkinteger(L, 2, SHRT_MAX);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= LUAB_SETMAXLEN) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((status = getmode(bp, LUAB_SETMAXLEN)) == 0)
                buf->iov.iov_len = LUAB_SETMAXLEN;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * getosreldate(2) - get the value of __FreeBSD_version
 *
 * @function getosreldate
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage date [, err, msg ] = bsd.unistd.getosreldate()
 */
static int
luab_getosreldate(lua_State *L)
{
    int date;

    (void)luab_core_checkmaxargs(L, 0);

    date = getosreldate();

    return (luab_pusherr(L, date));
}

/***
 * getpeereid(2) - get the effective credentials of a UNIX-domain peer
 *
 * @function getpeereid
 *
 * @param s                 Open socket(9), unix(4) domain(9).
 * @param euid              Effective user ID, (LUA_TUSERDATA(PRIMITIVE)).
 * @param egid              Effective group ID, (LUA_TUSERDATA(PRIMITIVE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.getpeereid(s, euid, egid)
 */
static int
luab_getpeereid(lua_State *L)
{
    int s;
    luab_primitive_u *h1;
    luab_primitive_u *h2;
    uid_t *euid;
    gid_t *egid;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    s = (int)luab_checkinteger(L, 1, INT_MAX);
    h1 = luab_udata(L, 2, luab_mx(PRIMITIVE), luab_primitive_u *);
    h2 = luab_udata(L, 3, luab_mx(PRIMITIVE), luab_primitive_u *);

    euid = &(h1->un_uint32);
    egid = &(h2->un_uint32);

    status = getpeereid(s, egid, euid);

    return (luab_pusherr(L, status));
}

/***
 * getresgid(2) - get real, effective and saved group ID
 *
 * @function getresgid
 *
 * @param rgid              Real group ID, (LUA_TUSERDATA(PRIMITIVE)).
 * @param egid              Effective group ID, (LUA_TUSERDATA(PRIMITIVE)).
 * @param sgid              Saved group ID, (LUA_TUSERDATA(PRIMITIVE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.getresgid(rgid, egid, sgid)
 */
static int
luab_getresgid(lua_State *L)
{
    luab_primitive_u *h1;
    luab_primitive_u *h2;
    luab_primitive_u *h3;
    gid_t *rgid;
    gid_t *egid;
    gid_t *sgid;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    h1 = luab_udata(L, 1, luab_mx(PRIMITIVE), luab_primitive_u *);
    h2 = luab_udata(L, 2, luab_mx(PRIMITIVE), luab_primitive_u *);
    h3 = luab_udata(L, 3, luab_mx(PRIMITIVE), luab_primitive_u *);

    rgid = &(h1->un_uint32);
    egid = &(h2->un_uint32);
    sgid = &(h3->un_uint32);

    status = getresgid(rgid, egid, sgid);

    return (luab_pusherr(L, status));
}

/***
 * getresuid(2) - get real, effective and saved user ID
 *
 * @function getresuid
 *
 * @param ruid              Real user ID, (LUA_TUSERDATA(PRIMITIVE)).
 * @param euid              Effective user ID, (LUA_TUSERDATA(PRIMITIVE)).
 * @param suid              Saved user ID, (LUA_TUSERDATA(PRIMITIVE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.getresuid(ruid, euid, suid)
 */
static int
luab_getresuid(lua_State *L)
{
    luab_primitive_u *h1;
    luab_primitive_u *h2;
    luab_primitive_u *h3;
    uid_t *ruid;
    uid_t *euid;
    uid_t *suid;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    h1 = luab_udata(L, 1, luab_mx(PRIMITIVE), luab_primitive_u *);
    h2 = luab_udata(L, 2, luab_mx(PRIMITIVE), luab_primitive_u *);
    h3 = luab_udata(L, 3, luab_mx(PRIMITIVE), luab_primitive_u *);

    ruid = &(h1->un_uint32);
    euid = &(h2->un_uint32);
    suid = &(h3->un_uint32);

    status = getresuid(ruid, euid, suid);

    return (luab_pusherr(L, status));
}

/***
 * getusershell(3) - get valid user shells
 *
 * @function getusershell
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage shell [, err, msg ] = bsd.unistd.getusershell()
 */
static int
luab_getusershell(lua_State *L)
{
    char *shell;
    int status;

    (void)luab_core_checkmaxargs(L, 0);

    if ((shell = getusershell()) != NULL)
        status = luab_pushstring(L, shell);
    else
        status = luab_pushnil(L);

    return (status);
}

/***
 * initgroups(3) - initialize group access list
 *
 * @function initgroups
 *
 * @param name              Specifies user name.
 * @param basegid           Specifies user group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.initgroups(name, gid)
 */
static int
luab_initgroups(lua_State *L)
{
    const char *name;
    gid_t basegid;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    name = luab_checklstring(L, 1, MAXLOGNAME);
    basegid = (gid_t)luab_checkinteger(L, 2, INT_MAX);

    status = initgroups(name, basegid);

    return (luab_pusherr(L, status));
}

/***
 * iruserok(3) - routines for returning a stream to e remote command.
 *
 * @function iruserok
 *
 * @param raddr             Remote IPv4 address, (LUA_TUSERDATA(IN_ADDR)).
 * @param superuser         Nonzero, if the local user is the superuser.
 * @param ruser             Specifies the name of the remote user.
 * @param luser             Specifies the name of the local user.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.iruserok(raddr, superuser, ruser, luser)
 */
static int
luab_iruserok(lua_State *L)
{
    struct in_addr *raddr;
    int superuser;
    const char *ruser;
    const char *luser;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    raddr = luab_udata(L, 1, luab_mx(IN_ADDR), struct in_addr *);
    superuser = (int)luab_checkinteger(L, 2, INT_MAX);
    ruser = luab_checklstring(L, 3, MAXLOGNAME);
    luser = luab_checklstring(L, 4, MAXLOGNAME);

    status = iruserok(raddr->s_addr, superuser, ruser, luser);

    return (luab_pusherr(L, status));
}

/***
 * iruserok_sa(3) - routines for returning a stream to e remote command.
 *
 * @function iruserok_sa
 *
 * @param addr              Remote IP address, (LUA_TUSERDATA(IOVEC)).
 * @param addrlen           Specifies the length of the IP address.
 * @param superuser         Nonzero, if the local user is the superuser.
 * @param ruser             Specifies the name of the remote user.
 * @param luser             Specifies the name of the local user.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.iruserok_sa(raddr, rlen, superuser, ruser, luser)
 */
static int
luab_iruserok_sa(lua_State *L)
{
    luab_iovec_t *buf;
    int addrlen;
    int superuser;
    const char *ruser;
    const char *luser;
    caddr_t bp;
    int status;

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    addrlen = (int)luab_checkinteger(L, 2, INT_MAX);
    superuser = (int)luab_checkinteger(L, 3, INT_MAX);
    ruser = luab_checklstring(L, 4, MAXLOGNAME);
    luser = luab_checklstring(L, 5, MAXLOGNAME);

    if (((bp = buf->iov.iov_base) != 0) &&
        ((size_t)addrlen <= buf->iov.iov_len) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            status = iruserok_sa(bp, addrlen, superuser, ruser, luser);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * issetugid(2) - is current process tainted by uid or gid changes
 *
 * @function issetugid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.issetugid()
 */
static int
luab_issetugid(lua_State *L)
{
    int status;

    (void)luab_core_checkmaxargs(L, 0);

    status = issetugid();

    return (luab_pusherr(L, status));
}

/***
 * lpathconf(2) - get configurable pathname variables
 *
 * @function lpathconf
 *
 * @param path              Name of file or directory.
 * @param name              Specifies the system variable over
 *
 *                              bsd.sys.unistd._PC_*
 *
 *                          to be queried.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage value [, err, msg ] = bsd.unistd.lpathconf(path, name)
 */
static int
luab_lpathconf(lua_State *L)
{
    const char *path;
    int name;
    long status;

    (void)luab_core_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    name = (int)luab_checkinteger(L, 2, INT_MAX);

    status = lpathconf(path, name);

    return (luab_pusherr(L, status));
}

/***
 * mkdtemp(3) - make tempory file name (unique)
 *
 * @function mkdtemp
 *
 * @param template          File name template, (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])s
 *
 * @usage path [, err, msg ] = bsd.unistd.mkdtemp(template)
 */
static int
luab_mkdtemp(lua_State *L)
{
    luab_iovec_t *buf;
    caddr_t bp;
    char *dp;

    (void)luab_core_checkmaxargs(L, 1);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != 0) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (buf->iov_max_len <= MAXPATHLEN) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            dp = mkdtemp(bp);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            dp = NULL;
        }
    } else {
        errno = ENXIO;
        dp = NULL;
    }
    return (luab_pushstring(L, dp));
}

/***
 * mkstemp(3) - make tempory file name (unique)
 *
 * @function mkstemp
 *
 * @param template          File name template, (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.mkstemp(template)
 */
static int
luab_mkstemp(lua_State *L)
{
    luab_iovec_t *buf;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != 0) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (buf->iov_max_len <= MAXPATHLEN) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            status = mkstemp(bp);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * mkstemps(3) - make tempory file name (unique)
 *
 * @function mkstemps
 *
 * @param template          File name template, (LUA_TUSERDATA(IOVEC)).
 * @param suffixlen         Specifies the length of the suffix string.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.mkstemps(template, suffixlen)
 */
static int
luab_mkstemps(lua_State *L)
{
    luab_iovec_t *buf;
    int suffixlen;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    suffixlen = (int)luab_checkinteger(L, 2, INT_MAX);

    if (((bp = buf->iov.iov_base) != 0) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (buf->iov_max_len <= MAXPATHLEN) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            status = mkstemps(bp, suffixlen);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * mktemp(3) - make tempory file name (unique)
 *
 * @function mktemp
 *
 * @param template          File name template, (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage path [, err, msg ] = bsd.unistd.mktemp(template)
 */
static int
luab_mktemp(lua_State *L)
{
    luab_iovec_t *buf;
    caddr_t bp;
    char *dp;

    (void)luab_core_checkmaxargs(L, 1);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != 0) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (buf->iov_max_len <= MAXPATHLEN) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            dp = mktemp(bp);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            dp = NULL;
        }
    } else {
        errno = ENXIO;
        dp = NULL;
    }
    return (luab_pushstring(L, dp));
}

/***
 * pipe2(2) - create descriptor pair for interprocess communication
 *
 * @function pipe2
 *
 * @param filedes           Pair of file descriptors:
 *
 *                              { filedes1, filedes2 },
 *
 *                          instance of (LUA_TTABLE(LUA_TNUMBER,LUA_TNUMBER)),
 *
 * @param flags             The values are constructed from
 *
 *                              bsd.fcntl.O_{CLOEXEC,NONBLOCK}
 *
 *                          by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.pipe2(filedes, flags)
 */
static int
luab_pipe2(lua_State *L)
{
    luab_table_t *tbl;
    int *fildes;
    int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    if ((tbl = luab_table_checklint(L, 1, 2)))
        fildes = (int *)(tbl->tbl_vec);
    else
        fildes = NULL;

    flags = (int)luab_checkinteger(L, 2, INT_MAX);

    if (fildes != NULL) {
        status = pipe2(fildes, flags);
        luab_table_free(tbl);
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * profil(2) - controll process profiling
 *
 * @function profil
 *
 * @param smaples           Samples buffer. (LUA_TUSERDATA(IOVEC)).
 * @param size              Capacity of samples buffer.
 * @param offset            Lowest address which the kernel takes
 *                          programm counter samples.
 * @param scale             Specifies the span of the bins.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.profil(samples, size, offset, scale)
 */
static int
luab_profil(lua_State *L)
{
    luab_iovec_t *buf;
    size_t size;
    vm_offset_t offset;
    int scale;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    buf = luab_isiovec(L, 1);
    size = (size_t)luab_checkinteger(L, 2,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    offset = (vm_offset_t)luab_checkinteger(L, 3,
#ifdef  __LP64__
    LONG_MAX
#else
    INT_MAX
#endif
    );
    scale = (int)luab_checkinteger(L, 4, INT_MAX);

    if (buf != NULL) {
        if (((bp = buf->iov.iov_base) != 0) &&
            (size <= buf->iov_max_len) &&
            (buf->iov_flags & IOV_BUFF)) {

            if ((buf->iov_flags & IOV_LOCK) == 0) {
                buf->iov_flags |= IOV_LOCK;

                if ((status = profil(bp, size, offset, scale)) == 0)
                    buf->iov.iov_len = size;

                buf->iov_flags &= ~IOV_LOCK;
            } else {
                errno = EBUSY;
                status = -1;
            }
        } else {
            errno = ENXIO;
            status = -1;
        }
    } else
        status = profil(NULL, size, offset, scale);

    return (luab_pusherr(L, status));
}

/***
 * rcmd(3) - routines for returning a stream to a remote command
 *
 * @function rcmd
 *
 * @param ahost             Name of remote host, (LUA_TUSERDATA(IOVEC)).
 * @param inport            Service Accees Point (SAP), server host, OSI-L4.
 * @param locuser           User ID on local host.
 * @param remuser           User ID on remote host.
 * @param cmd               Command.
 * @param fd2p              Auxiliary channel for control process.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage s [, err, msg ] = bsd.unistd.rcmd(ahost, inport, locuser, remuser, cmd, fd2p)
 */
static int
luab_rcmd(lua_State *L)
{
    luab_iovec_t *buf;
    int inport;
    const char *locuser;
    const char *remuser;
    const char *cmd;
    luab_primitive_u *xp;
    caddr_t bp;
    int *fd2p;
    int s;

    (void)luab_core_checkmaxargs(L, 6);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    inport = luab_checkinteger(L, 2, INT_MAX);
    locuser = luab_checklstring(L, 3, MAXLOGNAME);
    remuser = luab_checklstring(L, 4, MAXLOGNAME);
    cmd = luab_checklstring(L, 5, ARG_MAX);
    xp = luab_udataisnil(L, 6, luab_mx(PRIMITIVE), luab_primitive_u *);

    if (((bp = buf->iov.iov_base) != 0) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (buf->iov_max_len <= MAXPATHLEN) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if (xp != NULL)
                fd2p = &(xp->un_int);
            else
                fd2p = NULL;

            if ((s = rcmd(&bp, inport, locuser, remuser, cmd, fd2p)) > 0)
                buf->iov.iov_len = strnlen(bp, buf->iov_max_len);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            s = -1;
        }
    } else {
        errno = ENXIO;
        s = -1;
    }
    return (luab_pusherr(L, s));
}

/***
 * rcmd_af(3) - routines for returning a stream to a remote command
 *
 * @function rcmd_af
 *
 * @param ahost             Name of remote host, (LUA_TUSERDATA(IOVEC)).
 * @param inport            Service Accees Point (SAP), server host, OSI-L4.
 * @param locuser           User ID on local host.
 * @param remuser           User ID on remote host.
 * @param cmd               Command.
 * @param fd2p              Auxiliary channel for control process.
 * @param af                Address family, AF_XXX.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage s [, err, msg ] = bsd.unistd.rcmd_af(ahost, inport, locuser, remuser, cmd, fd2p, af)
 */
static int
luab_rcmd_af(lua_State *L)
{
    luab_iovec_t *buf;
    int inport;
    const char *locuser;
    const char *remuser;
    const char *cmd;
    luab_primitive_u *xp;
    int af;
    caddr_t bp;
    int *fd2p;
    int s;

    (void)luab_core_checkmaxargs(L, 7);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    inport = luab_checkinteger(L, 2, INT_MAX);
    locuser = luab_checklstring(L, 3, MAXLOGNAME);
    remuser = luab_checklstring(L, 4, MAXLOGNAME);
    cmd = luab_checklstring(L, 5, ARG_MAX);
    xp = luab_udataisnil(L, 6, luab_mx(PRIMITIVE), luab_primitive_u *);
    af = (int)luab_checkinteger(L, 7, INT_MAX);

    if (((bp = buf->iov.iov_base) != 0) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (buf->iov_max_len <= MAXPATHLEN) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if (h != NULL)
                fd2p = &(xp->un_int);
            else
                fd2p = NULL;

            if ((s = rcmd_af(&bp, inport, locuser, remuser, cmd, fd2p, af)) > 0)
                buf->iov.iov_len = strnlen(bp, buf->iov_max_len);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            s = -1;
        }
    } else {
        errno = ENXIO;
        s = -1;
    }
    return (luab_pusherr(L, s));
}

/***
 * rcmdsh(3) - return a stream to a remote command without superuser
 *
 * @function rcmdsh
 *
 * @param ahost             Name of remote host, (LUA_TUSERDATA(IOVEC)).
 * @param inport            Service Accees Point (SAP), server host, OSI-L4.
 * @param locuser           User ID on local host.
 * @param remuser           User ID on remote host.
 * @param cmd               Command.
 * @param rshprog           Shell.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage s [, err, msg ] = bsd.unistd.rcmdsh(ahost, inport, locuser, remuser, cmd, rshcmd)
 */
static int
luab_rcmdsh(lua_State *L)
{
    luab_iovec_t *buf;
    int inport;
    const char *locuser;
    const char *remuser;
    const char *cmd;
    const char *rshprog;
    caddr_t bp;
    int s;

    (void)luab_core_checkmaxargs(L, 6);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    inport = luab_checkinteger(L, 2, INT_MAX);
    locuser = luab_checklstring(L, 3, MAXLOGNAME);
    remuser = luab_checklstring(L, 4, MAXLOGNAME);
    cmd = luab_checklstring(L, 5, ARG_MAX);
    rshprog = luab_islstring(L, 6, ARG_MAX);

    if (((bp = buf->iov.iov_base) != 0) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (buf->iov_max_len <= MAXPATHLEN) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((s = rcmdsh(&bp, inport, locuser, remuser, cmd, rshprog)) > 0)
                buf->iov.iov_len = strnlen(bp, buf->iov_max_len);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            s = -1;
        }
    } else {
        errno = ENXIO;
        s = -1;
    }
    return (luab_pusherr(L, s));
}

/***
 * reboot(2) - reboot system or halt processor
 *
 * @function reboot
 *
 * @param howto             Mask of present options from
 *
 *                              bsd.sys.reboot.RB_*
 *
 *                          are combined by inclusive or.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.reboot(howto)
 */
static int
luab_reboot(lua_State *L)
{
    int howto;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    howto = (int)luab_checkinteger(L, 1, INT_MAX);
    status = reboot(howto);

        /* NOTREACHED */

    return (luab_pusherr(L, status));
}

/***
 * revoke(2) - revoke file access
 *
 * @function revoke
 *
 * @param path              File named by path.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.revoke(path)
 */
static int
luab_revoke(lua_State *L)
{
    const char *path;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    status = revoke(path);

    return (luab_pusherr(L, status));
}

/***
 * rfork(2) - manipulate process resources
 *
 * @function rfork
 *
 * @param flags             Flags argument.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pid [, err, msg ] = bsd.unistd.rfork(flags)
 */
static int
luab_rfork(lua_State *L)
{
    int flags;
    pid_t pid;

    (void)luab_core_checkmaxargs(L, 1);

    flags = luab_checkinteger(L, 1, INT_MAX);
    pid = rfork(flags);

    return (luab_pusherr(L, pid));
}

/***
 * rresvport(3) - routines for returning a stream to a remote command
 *
 * @function rresvport
 *
 * @param port              Port, SAP OSI-L4, (LUA_TUSERDATA(PRIMITIVE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage pid [, err, msg ] = bsd.unistd.rresvport(port)
 */
static int
luab_rresvport(lua_State *L)
{
    luab_primitive_u *xp;
    int *port, s;

    (void)luab_core_checkmaxargs(L, 1);

    xp = luab_udata(L, 1, luab_mx(PRIMITIVE), luab_primitive_u *);
    port = &(xp->un_int);
    s = rresvport(port);

    return (luab_pusherr(L, s));
}

/***
 * rresvport_af(3) - routines for returning a stream to a remote command
 *
 * @function rresvport_af
 *
 * @param port              Port, SAP OSI-L4, (LUA_TUSERDATA(PRIMITIVE)).
 * @param af                Protocol domain(9), OSI-L3.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage s [, err, msg ] = bsd.unistd.rresvport_af(port, af)
 */
static int
luab_rresvport_af(lua_State *L)
{
    luab_primitive_u *xp;
    int af, *port, s;

    (void)luab_core_checkmaxargs(L, 2);

    xp = luab_udata(L, 1, luab_mx(PRIMITIVE), luab_primitive_u *);
    af = (int)luab_checkinteger(L, 2, INT_MAX);

    port = &(xp->un_int);
    s = rresvport_af(port, af);

    return (luab_pusherr(L, s));
}

/***
 * ruserok(3) - routines for returning a stream to a remote command
 *
 * @function ruserok
 *
 * @param rhost             Hostname for gethostbyname(3).
 * @param superuser         Nonzero, if the local user is the superuser.
 * @param ruser             Specifies the name of the remote user.
 * @param luser             Specifies the name of the local user
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.ruserok(rhost, superuser, ruser, luser)
 */
static int
luab_ruserok(lua_State *L)
{
    const char *rhost;
    int superuser;
    const char *ruser;
    const char *luser;
    int status;

    (void)luab_core_checkmaxargs(L, 4);

    rhost = luab_checklstring(L, 1, MAXHOSTNAMELEN);
    superuser = (int)luab_checkinteger(L, 2, INT_MAX);
    ruser = luab_checklstring(L, 3, MAXLOGNAME);
    luser = luab_checklstring(L, 4, MAXLOGNAME);

    status = ruserok(rhost, superuser, ruser, luser);

    return (luab_pusherr(L, status));
}

/***
 * setdomainname(3) - set NIS domainname of current host
 *
 * @function getdomainname
 *
 * @param name              Instance of (LUA_TUSERDATA(IOVEC)).
 * @param namelen           Maximum size of buffer maps to name.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setdomainname(name, namelen)
 */
static int
luab_setdomainname(lua_State *L)
{
    luab_iovec_t *buf;
    int namelen;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    namelen = (int)luab_checkinteger(L, 2, INT_MAX);

    if (((bp = buf->iov.iov_base) != 0) &&
        ((size_t)namelen <= buf->iov.iov_len) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (buf->iov_max_len <= MAXHOSTNAMELEN) &&

        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            status = setdomainname(bp, namelen);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * setgroups(2) - set group access list
 *
 * @function setgroups
 *
 * @param ngroups           Number of entries, #gidset.
 * @param gidset            Instance of LUA_TTABLE(LUA_TNUMBER,LUA_TNUMBER):
 *
 *                              { gid0, gid1, gid2, ... , gidN }
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setgroups(ngroup, gidset)
 */
static int
luab_setgroups(lua_State *L)
{
    int ngroups;
    luab_table_t *tbl;
    gid_t *gidset;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    ngroups = (int)luab_checkinteger(L, 1, INT_MAX);

    if ((tbl = luab_table_checklgid(L, 2, ngroups)) != NULL) {
        gidset = (gid_t *)(tbl->tbl_vec);
        status = setgroups(ngroups, gidset);
        luab_table_free(tbl);
    } else {
        errno = EINVAL;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * sethostname(3) - set name of current host
 *
 * @function sethostname
 *
 * @param name              Hostname.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.sethostname(name)
 */
static int
luab_sethostname(lua_State *L)
{
    const char *name;
    int status;

    (void)luab_core_checkmaxargs(L, 1);
    name = luab_checklstring(L, 1, MAXHOSTNAMELEN);
    status = sethostname(name, strlen(name));

    return (luab_pusherr(L, status));
}

/***
 * setlogin(2) - set login name
 *
 * @function setlogin
 *
 * @param name              Login name.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setlogin(name)
 */
static int
luab_setlogin(lua_State *L)
{
    const char *name;
    int status;

    (void)luab_core_checkmaxargs(L, 1);
    name = luab_checklstring(L, 1, MAXLOGNAME);
    status = setlogin(name);

    return (luab_pusherr(L, status));
}

/***
 * setloginclass(2) - set login class
 *
 * @function Setloginclass
 *
 * @param name              Instance of (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setloginclass(name, len)
 */
static int
luab_setloginclass(lua_State *L)
{
    luab_iovec_t *buf;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (buf->iov_max_len <= MAXLOGNAME) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            status = setloginclass(bp);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ENXIO;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * setmode(2) - modify mode bits
 *
 * @function setmode
 *
 * @param mode_str          Instance of (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage mode [, err, msg ] = bsd.unistd.setmode(mode_str)
 */
static int
luab_setmode(lua_State *L)
{
    luab_iovec_t *buf;
    mode_t mode;
    caddr_t bp, dp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    mode = (mode_t)luab_checkinteger(L, 2, SHRT_MAX);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov.iov_len <= buf->iov_max_len) &&
        (LUAB_SETMAXLEN <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((dp = setmode(bp)) != NULL) {
                status = luab_pushldata(L, dp, LUAB_SETMAXLEN);
                free(dp);
            } else
                status = luab_pushnil(L);

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = luab_pushnil(L);
        }
    } else {
        errno = ENXIO;
        status = luab_pushnil(L);
    }
    return (status);
}

/***
 * setpgrp(2) - set process group
 *
 * @function setpgrp
 *
 * @param pid               Process identifier.
 * @param pgrp              Process group for specefied process.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setpgrp(pgrp)
 */
static int
luab_setpgrp(lua_State *L)
{
    pid_t pid, pgrp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    pid = (pid_t)luab_checkinteger(L, 1, INT_MAX);
    pgrp = (pid_t)luab_checkinteger(L, 2, INT_MAX);

    status = setpgrp(pid, pgrp);

    return (luab_pusherr(L, status));
}

/***
 * setproctitle(3) - set process title
 *
 * @function setproctitle
 *
 * @param title             Process title, by (Lua).string.format()
 *                          pr-formatted instance of (LUA_TSTRING).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setproctitle(title)
 */
static int
luab_setproctitle(lua_State *L)
{
    const char *title;

    (void)luab_core_checkmaxargs(L, 1);

    title = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    setproctitle("%s", title);

    return (luab_pusherr(L, 0));
}

/***
 * setproctitle_fast(3) - set process title
 *
 * @function setproctitle_fast
 *
 * @param title             Process title, by (Lua).string.format()
 *                          pr-formatted instance of (LUA_TSTRING).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setproctitle_fast(title)
 */
static int
luab_setproctitle_fast(lua_State *L)
{
    const char *title;

    (void)luab_core_checkmaxargs(L, 1);

    title = luab_checklstring(L, 1, LUAL_BUFFERSIZE);
    setproctitle_fast("%s", title);

    return (luab_pusherr(L, 0));
}

/***
 * setresgid(2) - set real, effective and saved group ID
 *
 * @function setresgid
 *
 * @param rgid              Real group ID.
 * @param egid              Effective group ID.
 * @param sgid              Saved group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setresgid(rgid, egid, sgid)
 */
static int
luab_setresgid(lua_State *L)
{
    gid_t rgid;
    gid_t egid;
    gid_t sgid;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    rgid = luab_checkinteger(L, 1, INT_MAX);
    egid = luab_checkinteger(L, 2, INT_MAX);
    sgid = luab_checkinteger(L, 3, INT_MAX);

    status = setresgid(rgid, egid, sgid);

    return (luab_pusherr(L, status));
}

/***
 * setresuid(2) - set real, effective and saved user ID
 *
 * @function setresuid
 *
 * @param ruid              Real user ID.
 * @param euid              Effective user ID.
 * @param suid              Saved user ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setresuid(ruid, euid, suid)
 */
static int
luab_setresuid(lua_State *L)
{
    uid_t ruid;
    uid_t euid;
    uid_t suid;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    ruid = luab_checkinteger(L, 1, INT_MAX);
    euid = luab_checkinteger(L, 2, INT_MAX);
    suid = luab_checkinteger(L, 3, INT_MAX);

    status = setresuid(ruid, euid, suid);

    return (luab_pusherr(L, status));
}

/***
 * setrgid(2) - set real group ID
 *
 * @function setrgid
 *
 * @param rgid              Real group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setrgid(rgid)
 */
static int
luab_setrgid(lua_State *L)
{
    gid_t rgid;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    rgid = luab_checkinteger(L, 1, INT_MAX);
    status = setrgid(rgid);

    return (luab_pusherr(L, status));
}

/***
 * setruid(2) - set real user ID
 *
 * @function setruid
 *
 * @param ruid              Real user ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.setruid(ruid)
 */
static int
luab_setruid(lua_State *L)
{
    uid_t ruid;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    ruid = luab_checkinteger(L, 1, INT_MAX);
    status = setruid(ruid);

    return (luab_pusherr(L, status));
}

/***
 * setusershell(3) - get valid user shells
 *
 * @function setusershell
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.getusershell()
 */
static int
luab_setusershell(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);

    setusershell();

    return (luab_pusherr(L, 0));
}

/***
 * swapon(2) - control devices for interleaved paging/swapping
 *
 * @function swapon
 *
 * @param special           Block device name.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.swapon(special)
 */
static int
luab_swapon(lua_State *L)
{
    const char *special;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    special = luab_checklstring(L, 1, MAXPATHLEN);
    status = swapon(special);

    return (luab_pusherr(L, status));
}

/***
 * swapoff(2) - control devices for interleaved paging/swapping
 *
 * @function swapoff
 *
 * @param special           Block device name.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.swapoff(special)
 */
static int
luab_swapoff(lua_State *L)
{
    const char *special;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    special = luab_checklstring(L, 1, MAXPATHLEN);
    status = swapoff(special);

    return (luab_pusherr(L, status));
}

/***
 * undelete(2) - attempt to recover a deleted file
 *
 * @function undelete
 *
 * @param path              File about to be recovered.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.unistd.undelete(path)
 */
static int
luab_undelete(lua_State *L)
{
    const char *path;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    status = undelete(path);

    return (luab_pusherr(L, status));
}
#endif /* __BSD_VISIBLE */

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(CRYPT_DATA)).
 *
 * @function crypt_data_create
 *
 * @param data          Instance of (LUA_TUSERDATA(CRYPT_DATA)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage crypt_data [, err, msg ] = bsd.unistd.crypt_data_create([ data ])
 */
static int
luab_crypt_data_create(lua_State *L)
{
    return (luab_create(L, 1, luab_mx(CRYPT_DATA), NULL));
}

/*
 * Interface against <unistd.h>.
 */

static luab_module_table_t luab_unistd_vec[] = {
    LUAB_INT("STDIN_FILENO",                      STDIN_FILENO),
    LUAB_INT("STDOUT_FILENO",                     STDOUT_FILENO),
    LUAB_INT("STDERR_FILENO",                     STDERR_FILENO),
#if __XSI_VISIBLE || __POSIX_VISIBLE >= 200112
    LUAB_INT("F_ULOCK",                           F_ULOCK),
    LUAB_INT("F_LOCK",                            F_LOCK),
    LUAB_INT("F_TLOCK",                           F_TLOCK),
    LUAB_INT("F_TEST",                            F_TEST),
#endif
    LUAB_INT("_POSIX_BARRIERS",                   _POSIX_BARRIERS),
    LUAB_INT("_POSIX_CPUTIME",                    _POSIX_CPUTIME),
    LUAB_INT("_POSIX_READER_WRITER_LOCKS",        _POSIX_READER_WRITER_LOCKS),
    LUAB_INT("_POSIX_REGEXP",                     _POSIX_REGEXP),
    LUAB_INT("_POSIX_SHELL",                      _POSIX_SHELL),
    LUAB_INT("_POSIX_SPAWN",                      _POSIX_SPAWN),
    LUAB_INT("_POSIX_SPIN_LOCKS",                 _POSIX_SPIN_LOCKS),
    LUAB_INT("_POSIX_THREAD_ATTR_STACKADDR",      _POSIX_THREAD_ATTR_STACKADDR),
    LUAB_INT("_POSIX_THREAD_ATTR_STACKSIZE",      _POSIX_THREAD_ATTR_STACKSIZE),
    LUAB_INT("_POSIX_THREAD_CPUTIME",             _POSIX_THREAD_CPUTIME),
    LUAB_INT("_POSIX_THREAD_PRIO_INHERIT",        _POSIX_THREAD_PRIO_INHERIT),
    LUAB_INT("_POSIX_THREAD_PRIO_PROTECT",        _POSIX_THREAD_PRIO_PROTECT),
    LUAB_INT("_POSIX_THREAD_PRIORITY_SCHEDULING", _POSIX_THREAD_PRIORITY_SCHEDULING),
    LUAB_INT("_POSIX_THREAD_PROCESS_SHARED",      _POSIX_THREAD_PROCESS_SHARED),
    LUAB_INT("_POSIX_THREAD_SAFE_FUNCTIONS",      _POSIX_THREAD_SAFE_FUNCTIONS),
    LUAB_INT("_POSIX_THREAD_SPORADIC_SERVER",     _POSIX_THREAD_SPORADIC_SERVER),
    LUAB_INT("_POSIX_THREADS",                    _POSIX_THREADS),
    LUAB_INT("_POSIX_TRACE",                      _POSIX_TRACE),
    LUAB_INT("_POSIX_TRACE_EVENT_FILTER",         _POSIX_TRACE_EVENT_FILTER),
    LUAB_INT("_POSIX_TRACE_INHERIT",              _POSIX_TRACE_INHERIT),
    LUAB_INT("_POSIX_TRACE_LOG",                  _POSIX_TRACE_LOG),
    LUAB_INT("_POSIX2_C_BIND",                    _POSIX2_C_BIND),
    LUAB_INT("_POSIX2_C_DEV",                     _POSIX2_C_DEV),
    LUAB_INT("_POSIX2_CHAR_TERM",                 _POSIX2_CHAR_TERM),
    LUAB_INT("_POSIX2_FORT_DEV",                  _POSIX2_FORT_DEV),
    LUAB_INT("_POSIX2_FORT_RUN",                  _POSIX2_FORT_RUN),
    LUAB_INT("_POSIX2_LOCALEDEF",                 _POSIX2_LOCALEDEF),
    LUAB_INT("_POSIX2_PBS",                       _POSIX2_PBS),
    LUAB_INT("_POSIX2_PBS_ACCOUNTING",            _POSIX2_PBS_ACCOUNTING),
    LUAB_INT("_POSIX2_PBS_CHECKPOINT",            _POSIX2_PBS_CHECKPOINT),
    LUAB_INT("_POSIX2_PBS_LOCATE",                _POSIX2_PBS_LOCATE),
    LUAB_INT("_POSIX2_PBS_MESSAGE",               _POSIX2_PBS_MESSAGE),
    LUAB_INT("_POSIX2_PBS_TRACK",                 _POSIX2_PBS_TRACK),
    LUAB_INT("_POSIX2_SW_DEV",                    _POSIX2_SW_DEV),
    LUAB_INT("_POSIX2_UPE",                       _POSIX2_UPE),
    LUAB_INT("_V6_ILP32_OFF32",                   _V6_ILP32_OFF32),
    LUAB_INT("_V6_ILP32_OFFBIG",                  _V6_ILP32_OFFBIG),
    LUAB_INT("_V6_LP64_OFF64",                    _V6_LP64_OFF64),
    LUAB_INT("_V6_LPBIG_OFFBIG",                  _V6_LPBIG_OFFBIG),
#if __XSI_VISIBLE
    LUAB_INT("_XOPEN_CRYPT",                      _XOPEN_CRYPT),
    LUAB_INT("_XOPEN_ENH_I18N",                   _XOPEN_ENH_I18N),
    LUAB_INT("_XOPEN_LEGACY",                     _XOPEN_LEGACY),
    LUAB_INT("_XOPEN_REALTIME",                   _XOPEN_REALTIME),
    LUAB_INT("_XOPEN_REALTIME_THREADS",           _XOPEN_REALTIME_THREADS),
    LUAB_INT("_XOPEN_UNIX",                       _XOPEN_UNIX),
#endif
    LUAB_INT("_POSIX2_VERSION",                   _POSIX2_VERSION),
    LUAB_INT("_SC_ARG_MAX",                       _SC_ARG_MAX),
    LUAB_INT("_SC_CHILD_MAX",                     _SC_CHILD_MAX),
    LUAB_INT("_SC_CLK_TCK",                       _SC_CLK_TCK),
    LUAB_INT("_SC_NGROUPS_MAX",                   _SC_NGROUPS_MAX),
    LUAB_INT("_SC_OPEN_MAX",                      _SC_OPEN_MAX),
    LUAB_INT("_SC_JOB_CONTROL",                   _SC_JOB_CONTROL),
    LUAB_INT("_SC_SAVED_IDS",                     _SC_SAVED_IDS),
    LUAB_INT("_SC_VERSION",                       _SC_VERSION),
    LUAB_INT("_SC_BC_BASE_MAX",                   _SC_BC_BASE_MAX),
    LUAB_INT("_SC_BC_DIM_MAX",                    _SC_BC_DIM_MAX),
    LUAB_INT("_SC_BC_SCALE_MAX",                  _SC_BC_SCALE_MAX),
    LUAB_INT("_SC_BC_STRING_MAX",                 _SC_BC_STRING_MAX),
    LUAB_INT("_SC_COLL_WEIGHTS_MAX",              _SC_COLL_WEIGHTS_MAX),
    LUAB_INT("_SC_EXPR_NEST_MAX",                 _SC_EXPR_NEST_MAX),
    LUAB_INT("_SC_LINE_MAX",                      _SC_LINE_MAX),
    LUAB_INT("_SC_RE_DUP_MAX",                    _SC_RE_DUP_MAX),
    LUAB_INT("_SC_2_VERSION",                     _SC_2_VERSION),
    LUAB_INT("_SC_2_C_BIND",                      _SC_2_C_BIND),
    LUAB_INT("_SC_2_C_DEV",                       _SC_2_C_DEV),
    LUAB_INT("_SC_2_CHAR_TERM",                   _SC_2_CHAR_TERM),
    LUAB_INT("_SC_2_FORT_DEV",                    _SC_2_FORT_DEV),
    LUAB_INT("_SC_2_FORT_RUN",                    _SC_2_FORT_RUN),
    LUAB_INT("_SC_2_LOCALEDEF",                   _SC_2_LOCALEDEF),
    LUAB_INT("_SC_2_SW_DEV",                      _SC_2_SW_DEV),
    LUAB_INT("_SC_2_UPE",                         _SC_2_UPE),
    LUAB_INT("_SC_STREAM_MAX",                    _SC_STREAM_MAX),
    LUAB_INT("_SC_TZNAME_MAX",                    _SC_TZNAME_MAX),
#if __POSIX_VISIBLE >= 199309
    LUAB_INT("_SC_ASYNCHRONOUS_IO",               _SC_ASYNCHRONOUS_IO),
    LUAB_INT("_SC_MAPPED_FILES",                  _SC_MAPPED_FILES),
    LUAB_INT("_SC_MEMLOCK",                       _SC_MEMLOCK),
    LUAB_INT("_SC_MEMLOCK_RANGE",                 _SC_MEMLOCK_RANGE),
    LUAB_INT("_SC_MEMORY_PROTECTION",             _SC_MEMORY_PROTECTION),
    LUAB_INT("_SC_MESSAGE_PASSING",               _SC_MESSAGE_PASSING),
    LUAB_INT("_SC_PRIORITIZED_IO",                _SC_PRIORITIZED_IO),
    LUAB_INT("_SC_PRIORITY_SCHEDULING",           _SC_PRIORITY_SCHEDULING),
    LUAB_INT("_SC_REALTIME_SIGNALS",              _SC_REALTIME_SIGNALS),
    LUAB_INT("_SC_SEMAPHORES",                    _SC_SEMAPHORES),
    LUAB_INT("_SC_FSYNC",                         _SC_FSYNC),
    LUAB_INT("_SC_SHARED_MEMORY_OBJECTS",         _SC_SHARED_MEMORY_OBJECTS),
    LUAB_INT("_SC_SYNCHRONIZED_IO",               _SC_SYNCHRONIZED_IO),
    LUAB_INT("_SC_TIMERS",                        _SC_TIMERS),
    LUAB_INT("_SC_AIO_LISTIO_MAX",                _SC_AIO_LISTIO_MAX),
    LUAB_INT("_SC_AIO_MAX",                       _SC_AIO_MAX),
    LUAB_INT("_SC_AIO_PRIO_DELTA_MAX",            _SC_AIO_PRIO_DELTA_MAX),
    LUAB_INT("_SC_DELAYTIMER_MAX",                _SC_DELAYTIMER_MAX),
    LUAB_INT("_SC_MQ_OPEN_MAX",                   _SC_MQ_OPEN_MAX),
    LUAB_INT("_SC_PAGESIZE",                      _SC_PAGESIZE),
    LUAB_INT("_SC_RTSIG_MAX",                     _SC_RTSIG_MAX),
    LUAB_INT("_SC_SEM_NSEMS_MAX",                 _SC_SEM_NSEMS_MAX),
    LUAB_INT("_SC_SEM_VALUE_MAX",                 _SC_SEM_VALUE_MAX),
    LUAB_INT("_SC_SIGQUEUE_MAX",                  _SC_SIGQUEUE_MAX),
    LUAB_INT("_SC_TIMER_MAX",                     _SC_TIMER_MAX),
#endif
#if __POSIX_VISIBLE >= 200112
    LUAB_INT("_SC_2_PBS",                         _SC_2_PBS),
    LUAB_INT("_SC_2_PBS_ACCOUNTING",              _SC_2_PBS_ACCOUNTING),
    LUAB_INT("_SC_2_PBS_CHECKPOINT",              _SC_2_PBS_CHECKPOINT),
    LUAB_INT("_SC_2_PBS_LOCATE",                  _SC_2_PBS_LOCATE),
    LUAB_INT("_SC_2_PBS_MESSAGE",                 _SC_2_PBS_MESSAGE),
    LUAB_INT("_SC_2_PBS_TRACK",                   _SC_2_PBS_TRACK),
    LUAB_INT("_SC_ADVISORY_INFO",                 _SC_ADVISORY_INFO),
    LUAB_INT("_SC_BARRIERS",                      _SC_BARRIERS),
    LUAB_INT("_SC_CLOCK_SELECTION",               _SC_CLOCK_SELECTION),
    LUAB_INT("_SC_CPUTIME",                       _SC_CPUTIME),
    LUAB_INT("_SC_FILE_LOCKING",                  _SC_FILE_LOCKING),
    LUAB_INT("_SC_GETGR_R_SIZE_MAX",              _SC_GETGR_R_SIZE_MAX),
    LUAB_INT("_SC_GETPW_R_SIZE_MAX",              _SC_GETPW_R_SIZE_MAX),
    LUAB_INT("_SC_HOST_NAME_MAX",                 _SC_HOST_NAME_MAX),
    LUAB_INT("_SC_LOGIN_NAME_MAX",                _SC_LOGIN_NAME_MAX),
    LUAB_INT("_SC_MONOTONIC_CLOCK",               _SC_MONOTONIC_CLOCK),
    LUAB_INT("_SC_MQ_PRIO_MAX",                   _SC_MQ_PRIO_MAX),
    LUAB_INT("_SC_READER_WRITER_LOCKS",           _SC_READER_WRITER_LOCKS),
    LUAB_INT("_SC_REGEXP",                        _SC_REGEXP),
    LUAB_INT("_SC_SHELL",                         _SC_SHELL),
    LUAB_INT("_SC_SPAWN",                         _SC_SPAWN),
    LUAB_INT("_SC_SPIN_LOCKS",                    _SC_SPIN_LOCKS),
    LUAB_INT("_SC_SPORADIC_SERVER",               _SC_SPORADIC_SERVER),
    LUAB_INT("_SC_THREAD_ATTR_STACKADDR",         _SC_THREAD_ATTR_STACKADDR),
    LUAB_INT("_SC_THREAD_ATTR_STACKSIZE",         _SC_THREAD_ATTR_STACKSIZE),
    LUAB_INT("_SC_THREAD_CPUTIME",                _SC_THREAD_CPUTIME),
    LUAB_INT("_SC_THREAD_DESTRUCTOR_ITERATIONS",  _SC_THREAD_DESTRUCTOR_ITERATIONS),
    LUAB_INT("_SC_THREAD_KEYS_MAX",               _SC_THREAD_KEYS_MAX),
    LUAB_INT("_SC_THREAD_PRIO_INHERIT",           _SC_THREAD_PRIO_INHERIT),
    LUAB_INT("_SC_THREAD_PRIO_PROTECT",           _SC_THREAD_PRIO_PROTECT),
    LUAB_INT("_SC_THREAD_PRIORITY_SCHEDULING",    _SC_THREAD_PRIORITY_SCHEDULING),
    LUAB_INT("_SC_THREAD_PROCESS_SHARED",         _SC_THREAD_PROCESS_SHARED),
    LUAB_INT("_SC_THREAD_SAFE_FUNCTIONS",         _SC_THREAD_SAFE_FUNCTIONS),
    LUAB_INT("_SC_THREAD_SPORADIC_SERVER",        _SC_THREAD_SPORADIC_SERVER),
    LUAB_INT("_SC_THREAD_STACK_MIN",              _SC_THREAD_STACK_MIN),
    LUAB_INT("_SC_THREAD_THREADS_MAX",            _SC_THREAD_THREADS_MAX),
    LUAB_INT("_SC_TIMEOUTS",                      _SC_TIMEOUTS),
    LUAB_INT("_SC_THREADS",                       _SC_THREADS),
    LUAB_INT("_SC_TRACE",                         _SC_TRACE),
    LUAB_INT("_SC_TRACE_EVENT_FILTER",            _SC_TRACE_EVENT_FILTER),
    LUAB_INT("_SC_TRACE_INHERIT",                 _SC_TRACE_INHERIT),
    LUAB_INT("_SC_TRACE_LOG",                     _SC_TRACE_LOG),
    LUAB_INT("_SC_TTY_NAME_MAX",                  _SC_TTY_NAME_MAX),
    LUAB_INT("_SC_TYPED_MEMORY_OBJECTS",          _SC_TYPED_MEMORY_OBJECTS),
    LUAB_INT("_SC_V6_ILP32_OFF32",                _SC_V6_ILP32_OFF32),
    LUAB_INT("_SC_V6_ILP32_OFFBIG",               _SC_V6_ILP32_OFFBIG),
    LUAB_INT("_SC_V6_LP64_OFF64",                 _SC_V6_LP64_OFF64),
    LUAB_INT("_SC_V6_LPBIG_OFFBIG",               _SC_V6_LPBIG_OFFBIG),
    LUAB_INT("_SC_IPV6",                          _SC_IPV6),
    LUAB_INT("_SC_RAW_SOCKETS",                   _SC_RAW_SOCKETS),
    LUAB_INT("_SC_SYMLOOP_MAX",                   _SC_SYMLOOP_MAX),
#endif
#if __XSI_VISIBLE
    LUAB_INT("_SC_ATEXIT_MAX",                    _SC_ATEXIT_MAX),
    LUAB_INT("_SC_IOV_MAX",                       _SC_IOV_MAX),
    LUAB_INT("_SC_PAGE_SIZE",                     _SC_PAGE_SIZE),
    LUAB_INT("_SC_XOPEN_CRYPT",                   _SC_XOPEN_CRYPT),
    LUAB_INT("_SC_XOPEN_ENH_I18N",                _SC_XOPEN_ENH_I18N),
    LUAB_INT("_SC_XOPEN_LEGACY",                  _SC_XOPEN_LEGACY),
    LUAB_INT("_SC_XOPEN_REALTIME",                _SC_XOPEN_REALTIME),
    LUAB_INT("_SC_XOPEN_REALTIME_THREADS",        _SC_XOPEN_REALTIME_THREADS),
    LUAB_INT("_SC_XOPEN_SHM",                     _SC_XOPEN_SHM),
    LUAB_INT("_SC_XOPEN_STREAMS",                 _SC_XOPEN_STREAMS),
    LUAB_INT("_SC_XOPEN_UNIX",                    _SC_XOPEN_UNIX),
    LUAB_INT("_SC_XOPEN_VERSION",                 _SC_XOPEN_VERSION),
    LUAB_INT("_SC_XOPEN_XCU_VERSION",             _SC_XOPEN_XCU_VERSION),
#endif
#if __BSD_VISIBLE
    LUAB_INT("_SC_NPROCESSORS_CONF",              _SC_NPROCESSORS_CONF),
    LUAB_INT("_SC_NPROCESSORS_ONLN",              _SC_NPROCESSORS_ONLN),
    LUAB_INT("_SC_CPUSET_SIZE",                   _SC_CPUSET_SIZE),
#endif
    LUAB_INT("_SC_PHYS_PAGES",                    _SC_PHYS_PAGES),
#if __POSIX_VISIBLE >= 199209
    LUAB_INT("_CS_PATH",                          _CS_PATH),
#endif
#if __POSIX_VISIBLE >= 200112
    LUAB_INT("_CS_POSIX_V6_ILP32_OFF32_CFLAGS",       _CS_POSIX_V6_ILP32_OFF32_CFLAGS),
    LUAB_INT("_CS_POSIX_V6_ILP32_OFF32_LDFLAGS",      _CS_POSIX_V6_ILP32_OFF32_LDFLAGS),
    LUAB_INT("_CS_POSIX_V6_ILP32_OFF32_LIBS",         _CS_POSIX_V6_ILP32_OFF32_LIBS),
    LUAB_INT("_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS",      _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS),
    LUAB_INT("_CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS",     _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS),
    LUAB_INT("_CS_POSIX_V6_ILP32_OFFBIG_LIBS",        _CS_POSIX_V6_ILP32_OFFBIG_LIBS),
    LUAB_INT("_CS_POSIX_V6_LP64_OFF64_CFLAGS",        _CS_POSIX_V6_LP64_OFF64_CFLAGS),
    LUAB_INT("_CS_POSIX_V6_LP64_OFF64_LDFLAGS",       _CS_POSIX_V6_LP64_OFF64_LDFLAGS),
    LUAB_INT("_CS_POSIX_V6_LP64_OFF64_LIBS",          _CS_POSIX_V6_LP64_OFF64_LIBS),
    LUAB_INT("_CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS",      _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS),
    LUAB_INT("_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS",     _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS),
    LUAB_INT("_CS_POSIX_V6_LPBIG_OFFBIG_LIBS",        _CS_POSIX_V6_LPBIG_OFFBIG_LIBS),
    LUAB_INT("_CS_POSIX_V6_WIDTH_RESTRICTED_ENVS",    _CS_POSIX_V6_WIDTH_RESTRICTED_ENVS),
#endif
/*
 * 1003.1-1990
 */
    LUAB_FUNC("access",               luab_access),
    LUAB_FUNC("alarm",                luab_alarm),
    LUAB_FUNC("chdir",                luab_chdir),
    LUAB_FUNC("chown",                luab_chown),
    LUAB_FUNC("close",                luab_close),
    LUAB_FUNC("closefrom",            luab_closefrom),
    LUAB_FUNC("dup",                  luab_dup),
    LUAB_FUNC("dup2",                 luab_dup2),
    LUAB_FUNC("execv",                luab_execv),
    LUAB_FUNC("execve",               luab_execve),
    LUAB_FUNC("execvp",               luab_execvp),
    LUAB_FUNC("fork",                 luab_fork),
    LUAB_FUNC("fpathconf",            luab_fpathconf),
    LUAB_FUNC("getcwd",               luab_getcwd),
    LUAB_FUNC("getegid",              luab_getegid),
    LUAB_FUNC("geteuid",              luab_geteuid),
    LUAB_FUNC("getgid",               luab_getgid),
    LUAB_FUNC("getgroups",            luab_getgroups),
    LUAB_FUNC("getlogin",             luab_getlogin),
    LUAB_FUNC("getpgrp",              luab_getpgrp),
    LUAB_FUNC("getpid",               luab_getpid),
    LUAB_FUNC("getppid",              luab_getppid),
    LUAB_FUNC("getuid",               luab_getuid),
    LUAB_FUNC("isatty",               luab_isatty),
    LUAB_FUNC("link",                 luab_link),
    LUAB_FUNC("lseek",                luab_lseek),
    LUAB_FUNC("pathconf",             luab_pathconf),
#ifdef notyet
    LUAB_FUNC("pause",                luab_pause),
#endif
    LUAB_FUNC("pipe",                 luab_pipe),
    LUAB_FUNC("read",                 luab_read),
    LUAB_FUNC("rmdir",                luab_rmdir),
    LUAB_FUNC("setgid",               luab_setgid),
    LUAB_FUNC("setpgid",              luab_setpgid),
    LUAB_FUNC("setsid",               luab_setsid),
    LUAB_FUNC("setuid",               luab_setuid),
    LUAB_FUNC("sysconf",              luab_sysconf),
    LUAB_FUNC("tcgetpgrp",            luab_tcgetpgrp),
    LUAB_FUNC("tcsetpgrp",            luab_tcsetpgrp),
    LUAB_FUNC("ttyname",              luab_ttyname),
    LUAB_FUNC("ttyname_r",            luab_ttyname_r),
    LUAB_FUNC("unlink",               luab_unlink),
    LUAB_FUNC("write",                luab_write),
/*
 * 1003.2-1992
 */
#if __POSIX_VISIBLE >= 199209 || __XSI_VISIBLE
/*
    LUAB_FUNC("confstr",  luab_confstr),
 */
#endif
/*
 * ISO/IEC 9945-1: 1996
 */
#if __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE
    LUAB_FUNC("fsync",                luab_fsync),
    LUAB_FUNC("fdatasync",            luab_fdatasync),
    LUAB_FUNC("ftruncate",            luab_ftruncate),
#endif
#if __POSIX_VISIBLE >= 199506
    LUAB_FUNC("getlogin_r",           luab_getlogin_r),
#endif
/*
 * 1003.1-2001
 */
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
    LUAB_FUNC("fchown",               luab_fchown),
    LUAB_FUNC("readlink",             luab_readlink),
#endif
#if __POSIX_VISIBLE >= 200112
    LUAB_FUNC("gethostname",          luab_gethostname),
    LUAB_FUNC("setegid",              luab_setegid),
    LUAB_FUNC("seteuid",              luab_seteuid),
#endif
/*
 * 1003.1-2008
 */
#if __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE
    LUAB_FUNC("getsid",               luab_getsid),
    LUAB_FUNC("fchdir",               luab_fchdir),
    LUAB_FUNC("getpgid",              luab_getpgid),
    LUAB_FUNC("lchown",               luab_lchown),
    LUAB_FUNC("pread",                luab_pread),
    LUAB_FUNC("pwrite",               luab_pwrite),
    LUAB_FUNC("truncate",             luab_truncate),
#endif
#if __POSIX_VISIBLE >= 200809
    LUAB_FUNC("faccessat",            luab_faccessat),
    LUAB_FUNC("fchownat",             luab_fchownat),
    LUAB_FUNC("fexecve",              luab_fexecve),
    LUAB_FUNC("linkat",               luab_linkat),
    LUAB_FUNC("readlinkat",           luab_readlinkat),
    LUAB_FUNC("symlinkat",            luab_symlinkat),
    LUAB_FUNC("unlinkat",             luab_unlinkat),
#endif
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE >= 402
    LUAB_FUNC("symlink",              luab_symlink),
#endif
/*
 * X/Open System Interfaces
 */
#if __XSI_VISIBLE
    LUAB_FUNC("crypt",                luab_crypt),
    LUAB_FUNC("gethostid",            luab_gethostid),
    LUAB_FUNC("lockf",                luab_lockf),
    LUAB_FUNC("nice",                 luab_nice),
    LUAB_FUNC("setregid",             luab_setregid),
    LUAB_FUNC("setreuid",             luab_setreuid),
    LUAB_FUNC("swab",                 luab_swab),
    LUAB_FUNC("sync",                 luab_sync),
#endif

#if (__XSI_VISIBLE && __XSI_VISIBLE <= 500) || __BSD_VISIBLE
    LUAB_FUNC("chroot",               luab_chroot),
    LUAB_FUNC("getdtablesize",        luab_getdtablesize),
    LUAB_FUNC("getpagesize",          luab_getpagesize),
    LUAB_FUNC("getpass",              luab_getpass),
#endif

#if (__XSI_VISIBLE && __XSI_VISIBLE <= 600) || __BSD_VISIBLE
    LUAB_FUNC("getwd",                luab_getwd),
    LUAB_FUNC("usleep",               luab_usleep),
    LUAB_FUNC("vfork",                luab_vfork),
#endif
#if __BSD_VISIBLE
    LUAB_FUNC("acct",                 luab_acct),
    LUAB_FUNC("check_utility_compat", luab_check_utility_compat),
    LUAB_FUNC("crypt_get_format",     luab_crypt_get_format),
    LUAB_FUNC("crypt_r",              luab_crypt_r),
    LUAB_FUNC("crypt_set_format",     luab_crypt_set_format),
    LUAB_FUNC("crypt_dup3",           luab_dup3),
    LUAB_FUNC("eaccess",              luab_eaccess),
    LUAB_FUNC("endusershell",         luab_endusershell),
    LUAB_FUNC("exect",                luab_exect),
    LUAB_FUNC("execvP",               luab_execvP),
    LUAB_FUNC("feature_present",      luab_feature_present),
    LUAB_FUNC("fflagstostr",          luab_fflagstostr),
    LUAB_FUNC("getdomainname",        luab_getdomainname),
    LUAB_FUNC("getentropy",           luab_getentropy),
    LUAB_FUNC("getgrouplist",         luab_getgrouplist),
    LUAB_FUNC("getloginclass",        luab_getloginclass),
    LUAB_FUNC("getmode",              luab_getmode),
    LUAB_FUNC("getosreldate",         luab_getosreldate),
    LUAB_FUNC("getpeereid",           luab_getpeereid),
    LUAB_FUNC("getresgid",            luab_getresgid),
    LUAB_FUNC("getresuid",            luab_getresuid),
    LUAB_FUNC("getusershell",         luab_getusershell),
    LUAB_FUNC("initgroups",           luab_initgroups),
    LUAB_FUNC("iruserok",             luab_iruserok),
    LUAB_FUNC("iruserok_sa",          luab_iruserok_sa),
    LUAB_FUNC("issetugid",            luab_issetugid),
    LUAB_FUNC("lpathconf",            luab_lpathconf),
    LUAB_FUNC("mkdtemp",              luab_mkdtemp),
    LUAB_FUNC("mkstemp",              luab_mkstemp),
    LUAB_FUNC("mkstemps",             luab_mkstemps),
    LUAB_FUNC("mktemp",               luab_mktemp),
    LUAB_FUNC("profil",               luab_profil),
    LUAB_FUNC("pipe2",                luab_pipe2),
    LUAB_FUNC("rcmd",                 luab_rcmd),
    LUAB_FUNC("rcmd_af",              luab_rcmd_af),
    LUAB_FUNC("rcmdsh",               luab_rcmdsh),
    LUAB_FUNC("reboot",               luab_reboot),
    LUAB_FUNC("revoke",               luab_revoke),
    LUAB_FUNC("rfork",                luab_rfork),
    LUAB_FUNC("rresvport",            luab_rresvport),
    LUAB_FUNC("rresvport_af",         luab_rresvport_af),
    LUAB_FUNC("ruserok",              luab_ruserok),
    LUAB_FUNC("setdomainname",        luab_setdomainname),
    LUAB_FUNC("setgroups",            luab_setgroups),
    LUAB_FUNC("sethostname",          luab_sethostname),
    LUAB_FUNC("setlogin",             luab_setlogin),
    LUAB_FUNC("setloginclass",        luab_setloginclass),
    LUAB_FUNC("setmode",              luab_setmode),
    LUAB_FUNC("setpgrp",              luab_setpgrp),
    LUAB_FUNC("setproctitle",         luab_setproctitle),
    LUAB_FUNC("setproctitle_fast",    luab_setproctitle_fast),
    LUAB_FUNC("setresgid",            luab_setresgid),
    LUAB_FUNC("setresuid",            luab_setresuid),
    LUAB_FUNC("setrgid",              luab_setrgid),
    LUAB_FUNC("setruid",              luab_setruid),
    LUAB_FUNC("setusershell",         luab_setusershell),
    LUAB_FUNC("swapon",               luab_swapon),
    LUAB_FUNC("swapoff",              luab_swapoff),
    LUAB_FUNC("undelete",             luab_undelete),
    LUAB_FUNC("crypt_data_create",    luab_crypt_data_create),
#endif /* __BSD_VISIBLE */
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_unistd_lib = {
    .m_cookie   = LUAB_UNISTD_LIB_ID,
    .m_name     = LUAB_UNISTD_LIB_KEY,
    .m_vec      = luab_unistd_vec,
};
