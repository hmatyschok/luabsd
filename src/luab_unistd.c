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

#include <sys/limits.h>
#include <sys/param.h>

#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

#define LUABSD_UNISTD_LIB_ID    1593623310
#define LUABSD_UNISTD_LIB_KEY   "unistd"

/*
 * Interface against (subset of) functions of exec(3) family.
 */

extern char **environ;

/***
 * @function execv(3) - execute a file
 *
 * @param path      Identifies the new process image file by its path.
 * @param argv      Argument vector, instance of LUA_TTABLE:
 *
 *                      { "arg0" , "arg1" , ..., argN }.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.execv(path, argv)
 */
static int
luab_execv(lua_State *L)
{
    const char *path;
    const char **argv;
    int status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    argv = luab_checkargv(L, 2);

    status = execv(path, __DECONST(char **, argv));

    free(argv);

    return luab_pusherr(L, status);
}

/***
 * @function execve(2) - execute a file
 *
 * @param path      Identifies the new process image file by its path.
 * @param argv      Argument vector, instance of LUA_TTABLE:
 *
 *                      { "arg0" , "arg1" , ..., argN }.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.execve(path, argv)
 */
static int
luab_execve(lua_State *L)
{
    const char *path;
    const char **argv;
    int status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    argv = luab_checkargv(L, 2);

    status = execve(path, __DECONST(char **, argv), environ);

    free(argv);

    return luab_pusherr(L, status);
}

/***
 * @function execvp(3) - execute a file
 *
 * @param path      Identifies the new process image file by its path.
 * @param argv      Argument vector, instance of LUA_TTABLE:
 *
 *                      { "arg0" , "arg1" , ..., argN }.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.execvp(path, argv)
 */
static int
luab_execvp(lua_State *L)
{
    const char *file;
    const char **argv;
    int status;

    (void)luab_checkmaxargs(L, 2);

    file = luab_checklstring(L, 1, MAXPATHLEN);
    argv = luab_checkargv(L, 2);

    status = execvp(file, __DECONST(char **, argv));

    free(argv);

    return luab_pusherr(L, status);
}

#if __POSIX_VISIBLE >= 200809
/***
 * @function fexecve(2) - execute a file
 *
 * @param fd        Identifies the new process image file by open file
 *                  descriptor.
 * @param argv      Argument vector, instance of LUA_TTABLE:
 *
 *                      { "arg0" , "arg1" , ..., argN }.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.fexecve(fd, argv)
 */
static int
luab_fexecve(lua_State *L)
{
    int fd;
    const char **argv;
    int status;

    (void)luab_checkmaxargs(L, 2);

    fd = luab_checkinteger(L, 1, INT_MAX);
    argv = luab_checkargv(L, 2);

    status = fexecve(fd, __DECONST(char **, argv), environ);

    free(argv);

    return luab_pusherr(L, status);
}
#endif

/*
 * Interface against alarm(3).
 */

static lua_State *saved_L;
static lua_Hook h;

static int h_msk;
static int h_cnt;

static void
h_callback(lua_State *L, lua_Debug *arg __unused)
{
    L = saved_L;

    lua_sethook(L, h, h_msk, h_cnt);
    lua_getfield(L, LUA_REGISTRYINDEX, "l_callback");

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

    lua_sethook(saved_L, h_callback, l_msk, 1);
}

/***
 * @function alarm(3) - set signal timer alarm
 *
 * @param seconds       For timeout specified number of seconds.
 * @param callback      Callout routine.
 * @param group         Group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (sec [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage sec [, msg ] = bsd.unistd.alarm(seconds, callback)
 */
static int
luab_alarm(lua_State *L)
{
    int narg = luab_checkmaxargs(L, 2);
    u_int seconds, status;

    if ((seconds = luab_checkinteger(L, 1, INT_MAX)) > 0) {

        if (lua_type(L, narg) != LUA_TFUNCTION)
            return luaL_error(L, "Missing callout handler.");

        lua_settop(L, narg);
        lua_setfield(L, LUA_REGISTRYINDEX, "l_callback");

        saved_L = L;

        if (signal(SIGALRM, h_signal) == SIG_ERR)
            return luab_pusherr(L, -1);
    }
    status = alarm(seconds);

    return luab_pusherr(L, status);
}

/*
 * Interface against components or service primitives on unistd.h.
 */

/***
 * @function access(2) - check availability of a file
 *
 * @param path          Identifies the file by name.
 * @param mode          See the File Access Permission section of intro(2).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.access(path, mode)
 */
static int
luab_access(lua_State *L)
{
    const char *path;
    int mode, status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    mode = luab_checkinteger(L, 2, INT_MAX);
    status = access(path, mode);

    return luab_pusherr(L, status);
}

/***
 * @function chdir(2) - change current working directory
 *
 * @param path          Points to the pathname of the directory.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.chdir(path)
 */
static int
luab_chdir(lua_State *L)
{
    const char *path;
    int status;

    (void)luab_checkmaxargs(L, 1);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    status = chdir(path);

    return luab_pusherr(L, status);
}

/***
 * @function chown(2) - change owner and group of a file
 *
 * @param path          Identifies the file by name.
 * @param owner         User ID.
 * @param group         Group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.chown(path, owner, group)
 */
static int
luab_chown(lua_State *L)
{
    const char *path;
    uid_t owner;
    gid_t group;
    int status;

    (void)luab_checkmaxargs(L, 3);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    owner = luab_checkinteger(L, 2, INT_MAX);
    group = luab_checkinteger(L, 3, INT_MAX);

    status = chown(path, owner, group);

    return luab_pusherr(L, status);
}

/***
 * @function close(2) - delete a descriptor
 *
 * @param fd            Open file descriptor.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.close(fd)
 */
static int
luab_close(lua_State *L)
{
    int fd, status;

    (void)luab_checkmaxargs(L, 1);

    fd = luab_checkinteger(L, 1, INT_MAX);
    status = close(fd);

    return luab_pusherr(L, status);
}

/***
 * @function closefrom(2) - delete open file descriptors
 *
 * @param lowfd         Any file descriptor greater than or equal from the
 *                      per-process file descriptor table.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.closefrom(lowfd)
 */
static int
luab_closefrom(lua_State *L)
{
    int lowfd;

    (void)luab_checkmaxargs(L, 1);

    lowfd = luab_checkinteger(L, 1, INT_MAX);
    closefrom(lowfd);

    return 0;
}

/***
 * @function dup(2) - duplicate an existing file descriptor
 *
 * @param oldd          Small non-negative integer index in the per-process
 *                      descriptor table.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.dup(oldd)
 */
static int
luab_dup(lua_State *L)
{
    int oldd, fd;

    (void)luab_checkmaxargs(L, 1);

    oldd = luab_checkinteger(L, 1, INT_MAX);
    fd = dup(oldd);

    return luab_pusherr(L, fd);
}

/***
 * @function dup2(2) - duplicate an existing file descriptor
 *
 * @param oldd          Small non-negative integer index in the per-process
 *                      descriptor table.
 * @param newd          The value for the new descriptor.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.dup2(oldd, newd)
 */
static int
luab_dup2(lua_State *L)
{
    int oldd, newd, fd;

    (void)luab_checkmaxargs(L, 2);

    oldd = luab_checkinteger(L, 1, INT_MAX);
    newd = luab_checkinteger(L, 2, INT_MAX);

    fd = dup2(oldd, newd);

    return luab_pusherr(L, fd);
}

/***
 * @function fork(2) - create a new process
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (pid [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage pid [, msg ] = bsd.unistd.fork()
 */
static int
luab_fork(lua_State *L)
{
    pid_t pid;

    (void)luab_checkmaxargs(L, 0);
    pid = fork();

    return luab_pusherr(L, pid);
}

/***
 * @function fpathconf(2) - get configurable pathname variables
 *
 * @param fd            Open file descriptor.
 * @param name          Specifies the system variable over
 *
 *                          bsd.sys.unistd._PC_*
 *
 *                      to be queried.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (value [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage value [, msg ] = bsd.unistd.fpathconf(fd, name)
 */
static int
luab_fpathconf(lua_State *L)
{
    int fd, name, status;

    (void)luab_checkmaxargs(L, 2);

    fd = luab_checkinteger(L, 1, INT_MAX);
    name = luab_checkinteger(L, 2, INT_MAX);

    status = fpathconf(fd, name);

    return luab_pusherr(L, status);
}

/***
 * @function getcwd(3) - get working directory pathname
 *
 * @return (LUA_T{NIL,STRING} [, LUA_TSTRING])      (path [, nil]) on success or
 *                                                  (nil, (strerror(errno)))
 * @usage path [, msg ] = bsd.unistd.getcwd()
 */
static int
luab_getcwd(lua_State *L)
{
    char *buf;

    (void)luab_checkmaxargs(L, 0);

    if ((buf = getcwd(NULL, MAXPATHLEN)) == NULL)
        return luab_pushnil(L);

    lua_pushlstring(L, buf, strlen(buf));
    free(buf);

    return 1;
}

/***
 * @function getegid(2) - get (effective)vgroup process identification
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (egid [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 * @usage egid [, msg ] = bsd.unistd.getegid()
 */
static int
luab_getegid(lua_State *L)
{
    gid_t egid;

    (void)luab_checkmaxargs(L, 0);
    egid = getegid();

    return luab_pusherr(L, egid);
}

/***
 * @function geteuid(2) - get (effective) user identification
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (euid [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 * @usage euid [, msg ] = bsd.unistd.geteuid()
 */
static int
luab_geteuid(lua_State *L)
{
    uid_t euid;

    (void)luab_checkmaxargs(L, 0);
    euid = geteuid();

    return luab_pusherr(L, euid);;
}

/***
 * @function getgid(2) - get group process identification
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (gid [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 * @usage gid [, msg ] = bsd.unistd.getgid()
 */
static int
luab_getgid(lua_State *L)
{
    gid_t gid;

    (void)luab_checkmaxargs(L, 0);
    gid = getgid();

    return luab_pusherr(L, gid);;
}

/***
 * @function getgroups(2) - get group access list
 *
 * @param gidsetlen     Number of entries that may be placed on
 *                      instance of returned LUA_TTABLE:
 *
 *                          gitset = { "gid0" , "gid1" , ..., gidN },
 *
 *                      iff (if and only if)
 *
 *                          gidsetlen > 0,
 *
 *                      on success.
 *
 * @return (LUA_TNUMBER, LUA_T{NIL,STRING,TABLE})   (len, nil) or
 *                                                  (len, gidset) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage len, gidset = bsd.unistd.getgroups(gidsetlen)
 */
static int
luab_getgroups(lua_State *L)
{
    int gidsetlen;
    gid_t *gidset;
    int ngroups, i, j;

    (void)luab_checkmaxargs(L, 1);

    if ((gidsetlen = luab_checkinteger(L, 1, INT_MAX)) == 0)
        gidset = NULL;
    else {
        if ((gidset = alloca(gidsetlen * sizeof(gid_t))) == NULL)
            return luab_pusherr(L, -1);
    }

    if ((ngroups = getgroups(gidsetlen, gidset)) < 0)
        return luab_pusherr(L, ngroups);

    lua_pushinteger(L, ngroups);

    if (gidsetlen == 0)
        return 1;

    lua_newtable(L);

    for (i = 0, j = 1; i < gidsetlen; i++, j++) {
        lua_pushinteger(L, gidset[i]);
        lua_rawseti(L, -2, j);
    }
    lua_pushvalue(L, -1);

    return 2;
}

/***
 * @function getlogin(2) - get login name
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,STRING} ]) (name [, nil]) on success or
 *                                                    (nil, (strerror(errno)))
 * @usage name [, msg ] = bsd.unistd.getlogin()
 */
static int
luab_getlogin(lua_State *L)
{
    char *name;

    (void)luab_checkmaxargs(L, 0);

    if ((name = getlogin()) == NULL)
        lua_pushnil(L);
    else
        lua_pushlstring(L, name, strlen(name));

    return 1;
}

/***
 * @function getpgrp(2) - get process group
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (pgrp [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 * @usage pgrp [, msg ] = bsd.unistd.getpgrp()
 */
static int
luab_getpgrp(lua_State *L)
{
    pid_t pgrp;

    (void)luab_checkmaxargs(L, 0);
    pgrp = getpgrp();

    return luab_pusherr(L, pgrp);
}

/***
 * @function getpid(2) - get calling process identification
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (pid [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 * @usage pid [, msg ] = bsd.unistd.getpid()
 */
static int
luab_getpid(lua_State *L)
{
    pid_t pid;

    (void)luab_checkmaxargs(L, 0);
    pid = getpid();

    return luab_pusherr(L, pid);
}

/***
 * @function getppid(2) - get parent process identification
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (ppid [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 * @usage ppid [, msg ] = bsd.unistd.getppid()
 */
static int
luab_getppid(lua_State *L)
{
    pid_t ppid;

    (void)luab_checkmaxargs(L, 0);
    ppid = getppid();

    return luab_pusherr(L, ppid);
}

/***
 * @function getuid(2) - get user identification
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (uid [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 * @usage uid [, msg ] = bsd.unistd.getuid()
 */
static int
luab_getuid(lua_State *L)
{
    uid_t uid;

    (void)luab_checkmaxargs(L, 0);

    uid = getuid();

    return luab_pusherr(L, uid);
}

/***
 * @function isatty(3) - determine, if valid terminal type device.
 *
 * @param fd            Open file descriptor.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (1 [, nil]) or
 *                                                  (1, (strerror(errno)))
 *                                                      on success or
 *                                                  (0, (strerror(errno)))
 * @usage err [, msg ] = bsd.unistd.isatty(fd)
 */
static int
luab_isatty(lua_State *L)
{
    int fd, status;

    (void)luab_checkmaxargs(L, 1);

    fd = luab_checkinteger(L, 1, INT_MAX);
    status = isatty(fd);

    return luab_pusherr(L, status);
}

/***
 * @function link(2) - make a hard file link
 *
 * @param name1         Path or file name of underlying object.
 * @param name2         Path or file name where it points to name1.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.link(name1, name2)
 */
static int
luab_link(lua_State *L)
{
    const char *name1, *name2;
    int status;

    (void)luab_checkmaxargs(L, 2);

    name1 = luab_checklstring(L, 1, MAXPATHLEN);
    name2 = luab_checklstring(L, 2, MAXPATHLEN);

    status = link(name1, name2);

    return luab_pusherr(L, status);
}

#ifndef _LSEEK_DECLARED
#define _LSEEK_DECLARED
/***
 * @function lseek(2) - reposition read/write file offset
 *
 * @param filedes       Open file descriptor.
 * @param offset        Offset according to the directive whence.
 * @param whence        Specifies the directive over
 *
 *                          bsd.sys.unistd.SEEK_*
 *
 *                      for repositioning.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (loc [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage loc [, msg ] = bsd.unistd.lseek(filedes, offset, whence)
 */
static int
luab_lseek(lua_State *L)
{
    int filedes;
    off_t offset;
    int whence;
    off_t location;

    (void)luab_checkmaxargs(L, 3);

    filedes = luab_checkinteger(L, 1, INT_MAX);
    offset = luab_checkinteger(L, 2, ULONG_MAX);
    whence = luab_checkinteger(L, 3, INT_MAX);

    location = lseek(fildes, offset, whence);

    return luab_pusherr(L, location);
}
#endif

/***
 * @function pathconf(2) - get configurable pathname variables
 *
 * @param path          Name of file or directory.
 * @param name          Specifies the system variable over
 *
 *                          bsd.sys.unistd._PC_*
 *
 *                      to be queried.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (value [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage value [, msg ] = bsd.unistd.pathconf(path, name)
 */
static int
luab_pathconf(lua_State *L)
{
    const char *path;
    int name;
    long status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    name = luab_checkinteger(L, 2, INT_MAX);

    status = pathconf(path, name);

    return luab_pusherr(L, status);
}

/***
 * @function pause(3) - stop until signal
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (-1 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage value [, msg ] = bsd.unistd.pause()
 */
static int
luab_pause(lua_State *L)
{
    int status;

    (void)luab_checkmaxargs(L, 0);

    status = pause();

    return luab_pusherr(L, status);
}

/***
 * @function pipe(2) - create descriptor pair for interprocess communication
 *
 * @param filedes       Pair of file descriptors as instance of LUA_TTABLE:
 *
 *                          { filedes1, filedes2 }.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.pipe(filedes)
 */
static int
luab_pipe(lua_State *L)
{
    int *fildes;
    int status;

    (void)luab_checkmaxargs(L, 1);

    fildes = luab_checkintvector(L, 1, 2);
    status = pipe(fildes);

    free(fildes);

    return luab_pusherr(L, status);
}

/***
 * @function pipe2(2) - create descriptor pair for interprocess communication
 *
 * @param filedes       Pair of file descriptors as instance of LUA_TTABLE:
 *
 *                          { filedes1, filedes2 }.
 *
 * @param flags         The values are constructed from
 *
 *                          bsd.fcntl.O_{CLOEXEC,NONBLOCK}
 *
 *                      by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.pipe2(filedes, flags)
 */
static int
luab_pipe2(lua_State *L)
{
    int *fildes;
    int flags;
    int status;

    (void)luab_checkmaxargs(L, 2);

    fildes = luab_checkintvector(L, 1, 2);
    flags = luab_checkinteger(L, 2, INT_MAX);

    status = pipe2(fildes, flags);

    free(fildes);

    return luab_pusherr(L, status);
}

/***
 * @function setgid(2) - set group id
 *
 * @param gid           Group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.setgid(gid)
 */
static int
luab_setgid(lua_State *L)
{
    gid_t gid;
    int status;

    (void)luab_checkmaxargs(L, 1);
    gid = luab_checkinteger(L, 1, INT_MAX);
    status = setgid(gid);

    return luab_pusherr(L, status);
}

/***
 * @function setpgid(2) - set process group
 *
 * @param pid           Process identifier.
 * @param pgrp          Process group for specefied process.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.setpgid(pgrp)
 */
static int
luab_setpgid(lua_State *L)
{
    pid_t pid, pgrp;
    int status;

    (void)luab_checkmaxargs(L, 2);

    pid = luab_checkinteger(L, 1, INT_MAX);
    pgrp = luab_checkinteger(L, 2, INT_MAX);

    status = setpgid(pid, pgrp);

    return luab_pusherr(L, status);
}

/***
 * @function setpgrp(2) - set process group
 *
 * @param pid           Process identifier.
 * @param pgrp          Process group for specefied process.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.setpgrp(pgrp)
 */
static int
luab_setpgrp(lua_State *L)
{
    pid_t pid, pgrp;
    int status;

    (void)luab_checkmaxargs(L, 2);

    pid = luab_checkinteger(L, 1, INT_MAX);
    pgrp = luab_checkinteger(L, 2, INT_MAX);

    status = setpgrp(pid, pgrp);

    return luab_pusherr(L, status);
}

/***
 * @function setsid(2) - create session and set process group ID
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (sid [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage sid [, msg ] = bsd.unistd.setsid()
 */
static int
luab_setsid(lua_State *L)
{
    pid_t sid;

    (void)luab_checkmaxargs(L, 0);

    sid = setsid();

    return luab_pusherr(L, sid);
}

/***
 * @function setuid(2) - set user and group ID
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (uid [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage uid [, msg ] = bsd.unistd.setuid()
 */
static int
luab_setuid(lua_State *L)
{
    uid_t uid;
    int status;

    (void)luab_checkmaxargs(L, 1);

    uid = luab_checkinteger(L, 1, INT_MAX);
    status = setuid(uid);

    return luab_pusherr(L, status);
}

/***
 * @function ttyname(3) - get name from associated terminal (tty) from file
 *                          descriptor
 *
 * @param fd            File descriptor refers to a valid terminal device.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_TSTRING])      (name [, nil]) on success or
 *                                                  (nil, (strerror(errno)))
 *
 * @usage name [, msg ] = bsd.unistd.ttyname()
 */
static int
luab_ttyname(lua_State *L)
{
    int fd;
    char *buf;

    (void)luab_checkmaxargs(L, 1);

    fd = luab_checkinteger(L, 1, INT_MAX);

    if ((buf = ttyname(fd)) == NULL)
        return luab_pushnil(L);

    lua_pushlstring(L, buf, strlen(buf));

    free(buf);

    return 1;
}

/***
 * @function ttyname_r(3) - get name from associated terminal (tty) from file
 *                          descriptor
 *
 * @param fd            File descriptor refers to a valid terminal device.
 *
 * @return (LUA_TNUMBER, LUA_TSTRING)               (0, name) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage name [, msg ] = bsd.unistd.ttyname()
 */
static int
luab_ttyname_r(lua_State *L)
{
    int fd, status;
    size_t len;
    char *buf;

    (void)luab_checkmaxargs(L, 1);

    fd = luab_checkinteger(L, 1, INT_MAX);
    status = -1;

    if ((len = sysconf(_SC_TTY_NAME_MAX)) < 0)
        return luab_pusherr(L, status);

    if ((buf = alloca(len)) == NULL)
        return luab_pusherr(L, status);

    if ((status = ttyname_r(fd, buf, len)) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);
    lua_pushlstring(L, buf, strlen(buf));

    return 2;
}

/***
 * @function unlink(2) - remove a directory entry
 *
 * @param path          The file to be removed.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.unlink(path)
 */
static int
luab_unlink(lua_State *L)
{
    const char *path;
    int status;

    (void)luab_checkmaxargs(L, 1);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    status = unlink(path);

    return luab_pusherr(L, status);
}

/***
 * @function rmdir(2) - remove a directory file
 *
 * @param path          The file to be removed.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.rmdir(path)
 */
static int
luab_rmdir(lua_State *L)
{
    const char *path;
    int status;

    (void)luab_checkmaxargs(L, 1);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    status = rmdir(path);

    return luab_pusherr(L, status);
}

/* ISO/IEC 9945-1: 1996 */
#if __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE
#ifndef _FTRUNCATE_DECLARED
#define _FTRUNCATE_DECLARED
/***
 * @function ftruncate(2) - truncate/extend a file to a specific length
 *
 * @param fd            File descriptor.
 * @param length        If the file was larger than this size, the extra data is
 *                      lost. If the file was smaller than this size, it will be
 *                      extended as if by writing bytes with the value zero.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.ftruncate(path)
 */
static int
luab_ftruncate(lua_State *L)
{
    int fd;
    off_t length;
    int status;

    luab_checkmaxargs(L, 2);

    fd = luab_checkinteger(L, 1, MAXPATHLEN);
    length = luab_checkinteger(L, 2, LONG_MAX);

    status = ftruncate(fd, length);

    return luab_pusherr(L, status);
}
#endif
#endif

/* 1003.1-2001 */
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
/***
 * @function fchown(2) - change owner and group of a file
 *
 * @param fd            File by file descriptor.
 * @param owner         User ID.
 * @param group         Group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.fchown(path)
 */
static int
luab_fchown(lua_State *L)
{
    int fd;
    uid_t owner;
    gid_t group;
    int status;

    (void)luab_checkmaxargs(L, 3);

    fd = luab_checkinteger(L, 1, INT_MAX);
    owner = luab_checkinteger(L, 2, INT_MAX);
    group = luab_checkinteger(L, 3, INT_MAX);

    status = fchown(fd, owner, group);

    return luab_pusherr(L, status);
}
#endif /* __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE */

#if __POSIX_VISIBLE >= 200112
/***
 * @function gethostname(3) - get name of current host
 *
 * @return (LUA_TNUMBER, LUA_TSTRING)               (0, hostname) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err, hostname = bsd.unistd.gethostname()
 */
static int
luab_gethostname(lua_State *L)
{
    char buf[MAXHOSTNAMELEN];
    int status;

    (void)luab_checkmaxargs(L, 0);

    if ((status = gethostname(buf, sizeof(buf))) != 0)
        return luab_pusherr(L, status);

    lua_pushinteger(L, status);
    lua_pushlstring(L, buf, strlen(buf));

    return 2;
}

/***
 * @function setegid(2) - set effective group ID
 *
 * @param egid          Effective group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.setegid(egid)
 */
static int
luab_setegid(lua_State *L)
{
    gid_t egid;
    int status;

    (void)luab_checkmaxargs(L, 1);

    egid = luab_checkinteger(L, 1, INT_MAX);
    status = setegid(egid);

    return luab_pusherr(L, status);
}

/***
 * @function seteuid(2) - set effective user ID
 *
 * @param euid          Effective user ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.seteuid(euid)
 */
static int
luab_seteuid(lua_State *L)
{
    uid_t euid;
    int status;

    (void)luab_checkmaxargs(L, 1);

    euid = luab_checkinteger(L, 1, INT_MAX);
    status = seteuid(euid);

    return luab_pusherr(L, status);
}
#endif /* __POSIX_VISIBLE >= 200112 */

/* 1003.1-2008 */
#if __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE
/***
 * @function getsid(2) - get process session
 *
 * @param pid           Process ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (sid [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage sid [, msg ] = bsd.unistd.getsid(pid)
 */
static int
luab_getsid(lua_State *L)
{
    pid_t pid, sid;

    (void)luab_checkmaxargs(L, 1);

    pid = luab_checkinteger(L, 1, INT_MAX);
    sid = getsid(pid);

    return luab_pusherr(L, sid);
}

/***
 * @function fchdir(2) - change current working directory
 *
 * @param fd            The directory referenced by this file descriptor.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage sid [, msg ] = bsd.unistd.fchdir(fd)
 */
static int
luab_fchdir(lua_State *L)
{
    int fd, status;

    (void)luab_checkmaxargs(L, 1);

    fd = luab_checkinteger(L, 1, INT_MAX);
    status = fchdir(fd);

    return luab_pusherr(L, status);
}

/***
 * @function getpgid(2) - get current process group by pid
 *
 * @param pid           Process ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.getpgid(pid)
 */
static int
luab_getpgid(lua_State *L)
{
    pid_t pid, pgrp;

    (void)luab_checkmaxargs(L, 1);

    pid = luab_checkinteger(L, 1, INT_MAX);
    pgrp = getpgid(pid);

    return luab_pusherr(L, pgrp);
}

/***
 * @function lchown(2) - change owner and group of a file
 *
 * @param path          Specifies the file by its name.
 * @param owner         Specifies the owner by User ID.
 * @param group         Group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.lchown(path, owner, group)
 */
static int
luab_lchown(lua_State *L)
{
    const char *path;
    uid_t owner;
    gid_t group;
    int status;

    (void)luab_checkmaxargs(L, 3);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    owner = luab_checkinteger(L, 2, INT_MAX);
    group = luab_checkinteger(L, 3, INT_MAX);

    status = lchown(path, owner, group);

    return luab_pusherr(L, status);
}

#ifndef _TRUNCATE_DECLARED
#define _TRUNCATE_DECLARED
/***
 * @function truncate(2) - truncate / extend a file to a specific length
 *
 * @param path          Identifies the file by name.
 * @param length        If the file was larger than this size, the extra data is
 *                      lost. If the file was smaller than this size, it will be
 *                      extended as if by writing bytes with the value zero.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.truncate(path, length)
 */
static int
luab_truncate(lua_State *L)
{
    const char *path;
    off_t length;
    int status;

    luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    length = luab+checkinteger(L, 2, LONG_MAX);

    status = truncate(path, length);

    return luab_pusherr(L, status);
}
#endif
#endif /* __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE */

#if __POSIX_VISIBLE >= 200809
static int
luab_faccessat(lua_State *L)
{
    int fd;
    const char *path;
    int mode;
    int flag;
    int status;

    (void)luab_checkmaxargs(L, 4);

    fd = luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    mode = luab_checkinteger(L, 3, INT_MAX);
    flag = luab_checkinteger(L, 4, INT_MAX);

    status = faccessat(fd, path, mode, flag);

    return luab_pusherr(L, status);
}

/***
 * @function fchownat(2) - change owner and group of a file
 *
 * @param fd            File descriptor associated with working directory from
 *                      object pointed by path.
 * @param path          Name or path of referred file.
 * @param owner         Specifies user ID.
 * @param group         Specifies group ID.
 * @param flag          The values are constructed over
 *
 *                          bsd.fcntl.AT_SYMLINK_NOFOLLOW
 *
 *                      by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.fchownat(fd, path, owner, group, flag)
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

    (void)luab_checkmaxargs(L, 5);

    fd = luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    owner = luab_checkinteger(L, 3, INT_MAX);
    group = luab_checkinteger(L, 4, INT_MAX);
    flag = luab_checkinteger(L, 5, INT_MAX);

    status = fchownat(fd, path, owner, group, flag);

    return luab_pusherr(L, status);
}

/***
 * @function linkat(2) - make a hard file link
 *
 * @param fd1           File descriptor associated with working directory from
 *                      object pointed by name1 or associated with the object
 *                      pointed by name1.
 * @param name1         By name2 referred underlying object.
 * @param fd2           File descriptor associated with working directory from
 *                      object pointed by name2 or associated with the object
 *                      pointed by name2.
 * @param name2         Name of file link referring object identified by name1.
 * @param flag          The values are constructed over
 *
 *                          bsd.fcntl.AT_SYMLINK_NOFOLLOW
 *
 *                      by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.linkat(fd1, name1, fd2, name2, flag)
 */
static int
luab_linkat(lua_State *L)
{
    int fd1;
    const char *name1;
    int fd2;
    const char *name2;
    int flag, status;

    (void)luab_checkmaxargs(L, 5);

    fd1 = luab_checkinteger(L, 1, INT_MAX);
    name1 = luab_checklstring(L, 2, MAXPATHLEN);
    fd2 = luab_checkinteger(L, 3, INT_MAX);
    name2 = luab_checklstring(L, 4, MAXPATHLEN);
    flag = luab_checkinteger(L, 5, INT_MAX);

    status = linkat(fd1, name1, fd2, name2, flag);

    return luab_pusherr(L, status);
}

/***
 * @function unlinkat(2) - remove directory entry
 *
 * @param fd            File descriptor associated with working directory from
 *                      object pointed by name1 or associated with the object
 *                      pointed by name1.
 * @param path          Link identified by path.
 * @param flag          Values are constructed over
 *
 *                          bsd.fcntl.AT_SYMLINK_NOFOLLOW
 *
 *                      by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.unlinkat(fd, path, flag)
 */
static int
luab_unlinkat(lua_State *L)
{
    int fd;
    const char *path;
    int flag, status;

    (void)luab_checkmaxargs(L, 3);

    fd = luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    flag = luab_checkinteger(L, 3, INT_MAX);

    status = unlinkat(fd, path, flag);

    return luab_pusherr(L, status);
}
#endif /* __POSIX_VISIBLE >= 200809 */

#if (__XSI_VISIBLE && __XSI_VISIBLE <= 600) || __BSD_VISIBLE
/***
 * @function getwd(3) - get working directory pathname
 *
 * @return (LUA_T{NIL,STRING} [, LUA_TSTRING])      (path [, nil]) on success or
 *                                                  (nil, (strerror(errno)))
 * @usage path [, msg ] = bsd.unistd.getwd()
 */
static int
luab_getwd(lua_State *L)
{
    char *buf;

    (void)luab_checkmaxargs(L, 0);

    if ((buf = getwd(NULL)) == NULL)
        return luab_pushnil(L);

    lua_pushlstring(L, buf, strlen(buf));
    free(buf);

    return 1;
}
#endif /* (__XSI_VISIBLE && __XSI_VISIBLE <= 600) || __BSD_VISIBLE */
#if __BSD_VISIBLE
/***
 * @function eaccess(2) - check availability of a file
 *
 * @param path          Identifies the file by name.
 * @param mode          See the File Access Permission section of intro(2).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.eaccess(path, mode)
 */
static int
luab_eaccess(lua_State *L)
{
    const char *path;
    int mode, status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    mode = luab_checkinteger(L, 2, INT_MAX);
    status = eaccess(path, mode);

    return luab_pusherr(L, status);
}

/***
 * @function lpathconf(2) - get configurable pathname variables
 *
 * @param path          Name of file or directory.
 * @param name          Specifies the system variable over
 *
 *                          bsd.sys.unistd._PC_*
 *
 *                      to be queried.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (value [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage value [, msg ] = bsd.unistd.lpathconf(path, name)
 */
static int
luab_lpathconf(lua_State *L)
{
    const char *path;
    int name;
    long status;

    (void)luab_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    name = luab_checkinteger(L, 2, INT_MAX);

    status = lpathconf(path, name);

    return luab_pusherr(L, status);
}

/***
 * @function sethostname(3) - set name of current host
 *
 * @param name          Hostname.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.sethostname(name)
 */
static int
luab_sethostname(lua_State *L)
{
    const char *name;
    int status;

    (void)luab_checkmaxargs(L, 1);
    name = luab_checklstring(L, 1, MAXHOSTNAMELEN);
    status = sethostname(name, strlen(name));

    return luab_pusherr(L, status);
}

/***
 * @function setlogin(2) - set login name
 *
 * @param name          Login name.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,STRING} ])     (0 [, nil]) on success or
 *                                                  (-1, (strerror(errno)))
 *
 * @usage err [, msg ] = bsd.unistd.setlogin(name)
 */
static int
luab_setlogin(lua_State *L)
{
    const char *name;
    int status;

    (void)luab_checkmaxargs(L, 1);
    name = luab_checklstring(L, 1, MAXLOGNAME);
    status = setlogin(name);

    return luab_pusherr(L, status);
}
#endif /* __BSD_VISIBLE */

static luab_table_t luab_unistd_vec[] = {   /* unistd.h */
    LUABSD_INT("STDIN_FILENO",  STDIN_FILENO),
    LUABSD_INT("STDOUT_FILENO", STDOUT_FILENO),
    LUABSD_INT("STDERR_FILENO", STDERR_FILENO),
#if __XSI_VISIBLE || __POSIX_VISIBLE >= 200112
    LUABSD_INT("F_ULOCK",   F_ULOCK),
    LUABSD_INT("F_LOCK",    F_LOCK),
    LUABSD_INT("F_TLOCK",   F_TLOCK),
    LUABSD_INT("F_TEST",    F_TEST),
#endif
    LUABSD_INT("_POSIX_BARRIERS",   _POSIX_BARRIERS),
    LUABSD_INT("_POSIX_CPUTIME",    _POSIX_CPUTIME),
    LUABSD_INT("_POSIX_READER_WRITER_LOCKS",    _POSIX_READER_WRITER_LOCKS),
    LUABSD_INT("_POSIX_REGEXP", _POSIX_REGEXP),
    LUABSD_INT("_POSIX_SHELL",  _POSIX_SHELL),
    LUABSD_INT("_POSIX_SPAWN",  _POSIX_SPAWN),
    LUABSD_INT("_POSIX_SPIN_LOCKS", _POSIX_SPIN_LOCKS),
    LUABSD_INT("_POSIX_THREAD_ATTR_STACKADDR",  _POSIX_THREAD_ATTR_STACKADDR),
    LUABSD_INT("_POSIX_THREAD_ATTR_STACKSIZE",  _POSIX_THREAD_ATTR_STACKSIZE),
    LUABSD_INT("_POSIX_THREAD_CPUTIME", _POSIX_THREAD_CPUTIME),
    LUABSD_INT("_POSIX_THREAD_PRIO_INHERIT",    _POSIX_THREAD_PRIO_INHERIT),
    LUABSD_INT("_POSIX_THREAD_PRIO_PROTECT",    _POSIX_THREAD_PRIO_PROTECT),
    LUABSD_INT("_POSIX_THREAD_PRIORITY_SCHEDULING", _POSIX_THREAD_PRIORITY_SCHEDULING),
    LUABSD_INT("_POSIX_THREAD_PROCESS_SHARED",  _POSIX_THREAD_PROCESS_SHARED),
    LUABSD_INT("_POSIX_THREAD_SAFE_FUNCTIONS",  _POSIX_THREAD_SAFE_FUNCTIONS),
    LUABSD_INT("_POSIX_THREAD_SPORADIC_SERVER", _POSIX_THREAD_SPORADIC_SERVER),
    LUABSD_INT("_POSIX_THREADS",    _POSIX_THREADS),
    LUABSD_INT("_POSIX_TRACE",  _POSIX_TRACE),
    LUABSD_INT("_POSIX_TRACE_EVENT_FILTER", _POSIX_TRACE_EVENT_FILTER),
    LUABSD_INT("_POSIX_TRACE_INHERIT",  _POSIX_TRACE_INHERIT),
    LUABSD_INT("_POSIX_TRACE_LOG",  _POSIX_TRACE_LOG),
    LUABSD_INT("_POSIX2_C_BIND",    _POSIX2_C_BIND),
    LUABSD_INT("_POSIX2_C_DEV", _POSIX2_C_DEV),
    LUABSD_INT("_POSIX2_CHAR_TERM", _POSIX2_CHAR_TERM),
    LUABSD_INT("_POSIX2_FORT_DEV",  _POSIX2_FORT_DEV),
    LUABSD_INT("_POSIX2_FORT_RUN",  _POSIX2_FORT_RUN),
    LUABSD_INT("_POSIX2_LOCALEDEF", _POSIX2_LOCALEDEF),
    LUABSD_INT("_POSIX2_PBS",   _POSIX2_PBS),
    LUABSD_INT("_POSIX2_PBS_ACCOUNTING",    _POSIX2_PBS_ACCOUNTING),
    LUABSD_INT("_POSIX2_PBS_CHECKPOINT",    _POSIX2_PBS_CHECKPOINT),
    LUABSD_INT("_POSIX2_PBS_LOCATE",    _POSIX2_PBS_LOCATE),
    LUABSD_INT("_POSIX2_PBS_MESSAGE",   _POSIX2_PBS_MESSAGE),
    LUABSD_INT("_POSIX2_PBS_TRACK", _POSIX2_PBS_TRACK),
    LUABSD_INT("_POSIX2_SW_DEV",    _POSIX2_SW_DEV),
    LUABSD_INT("_POSIX2_UPE",   _POSIX2_UPE),
    LUABSD_INT("_V6_ILP32_OFF32",   _V6_ILP32_OFF32),
    LUABSD_INT("_V6_ILP32_OFFBIG",  _V6_ILP32_OFFBIG),
    LUABSD_INT("_V6_LP64_OFF64",    _V6_LP64_OFF64),
    LUABSD_INT("_V6_LPBIG_OFFBIG",  _V6_LPBIG_OFFBIG),
#if __XSI_VISIBLE
    LUABSD_INT("_XOPEN_CRYPT",  _XOPEN_CRYPT),
    LUABSD_INT("_XOPEN_ENH_I18N",   _XOPEN_ENH_I18N),
    LUABSD_INT("_XOPEN_LEGACY", _XOPEN_LEGACY),
    LUABSD_INT("_XOPEN_REALTIME",   _XOPEN_REALTIME),
    LUABSD_INT("_XOPEN_REALTIME_THREADS",   _XOPEN_REALTIME_THREADS),
    LUABSD_INT("_XOPEN_UNIX",   _XOPEN_UNIX),
#endif
    LUABSD_INT("_POSIX2_VERSION",   _POSIX2_VERSION),
    LUABSD_INT("_SC_ARG_MAX",   _SC_ARG_MAX),
    LUABSD_INT("_SC_CHILD_MAX", _SC_CHILD_MAX),
    LUABSD_INT("_SC_CLK_TCK",   _SC_CLK_TCK),
    LUABSD_INT("_SC_NGROUPS_MAX",   _SC_NGROUPS_MAX),
    LUABSD_INT("_SC_OPEN_MAX",  _SC_OPEN_MAX),
    LUABSD_INT("_SC_JOB_CONTROL",   _SC_JOB_CONTROL),
    LUABSD_INT("_SC_SAVED_IDS", _SC_SAVED_IDS),
    LUABSD_INT("_SC_VERSION",   _SC_VERSION),
    LUABSD_INT("_SC_BC_BASE_MAX",   _SC_BC_BASE_MAX),
    LUABSD_INT("_SC_BC_DIM_MAX",    _SC_BC_DIM_MAX),
    LUABSD_INT("_SC_BC_SCALE_MAX",  _SC_BC_SCALE_MAX),
    LUABSD_INT("_SC_BC_STRING_MAX", _SC_BC_STRING_MAX),
    LUABSD_INT("_SC_COLL_WEIGHTS_MAX",  _SC_COLL_WEIGHTS_MAX),
    LUABSD_INT("_SC_EXPR_NEST_MAX", _SC_EXPR_NEST_MAX),
    LUABSD_INT("_SC_LINE_MAX",  _SC_LINE_MAX),
    LUABSD_INT("_SC_RE_DUP_MAX",    _SC_RE_DUP_MAX),
    LUABSD_INT("_SC_2_VERSION", _SC_2_VERSION),
    LUABSD_INT("_SC_2_C_BIND",  _SC_2_C_BIND),
    LUABSD_INT("_SC_2_C_DEV",   _SC_2_C_DEV),
    LUABSD_INT("_SC_2_CHAR_TERM",   _SC_2_CHAR_TERM),
    LUABSD_INT("_SC_2_FORT_DEV",    _SC_2_FORT_DEV),
    LUABSD_INT("_SC_2_FORT_RUN",    _SC_2_FORT_RUN),
    LUABSD_INT("_SC_2_LOCALEDEF",   _SC_2_LOCALEDEF),
    LUABSD_INT("_SC_2_SW_DEV",  _SC_2_SW_DEV),
    LUABSD_INT("_SC_2_UPE", _SC_2_UPE),
    LUABSD_INT("_SC_STREAM_MAX",    _SC_STREAM_MAX),
    LUABSD_INT("_SC_TZNAME_MAX",    _SC_TZNAME_MAX),
#if __POSIX_VISIBLE >= 199309
    LUABSD_INT("_SC_ASYNCHRONOUS_IO",   _SC_ASYNCHRONOUS_IO),
    LUABSD_INT("_SC_MAPPED_FILES",  _SC_MAPPED_FILES),
    LUABSD_INT("_SC_MEMLOCK",   _SC_MEMLOCK),
    LUABSD_INT("_SC_MEMLOCK_RANGE", _SC_MEMLOCK_RANGE),
    LUABSD_INT("_SC_MEMORY_PROTECTION", _SC_MEMORY_PROTECTION),
    LUABSD_INT("_SC_MESSAGE_PASSING",   _SC_MESSAGE_PASSING),
    LUABSD_INT("_SC_PRIORITIZED_IO",    _SC_PRIORITIZED_IO),
    LUABSD_INT("_SC_PRIORITY_SCHEDULING",   _SC_PRIORITY_SCHEDULING),
    LUABSD_INT("_SC_REALTIME_SIGNALS",  _SC_REALTIME_SIGNALS),
    LUABSD_INT("_SC_SEMAPHORES",    _SC_SEMAPHORES),
    LUABSD_INT("_SC_FSYNC", _SC_FSYNC),
    LUABSD_INT("_SC_SHARED_MEMORY_OBJECTS", _SC_SHARED_MEMORY_OBJECTS),
    LUABSD_INT("_SC_SYNCHRONIZED_IO",   _SC_SYNCHRONIZED_IO),
    LUABSD_INT("_SC_TIMERS",    _SC_TIMERS),
    LUABSD_INT("_SC_AIO_LISTIO_MAX",    _SC_AIO_LISTIO_MAX),
    LUABSD_INT("_SC_AIO_MAX",   _SC_AIO_MAX),
    LUABSD_INT("_SC_AIO_PRIO_DELTA_MAX",    _SC_AIO_PRIO_DELTA_MAX),
    LUABSD_INT("_SC_DELAYTIMER_MAX",    _SC_DELAYTIMER_MAX),
    LUABSD_INT("_SC_MQ_OPEN_MAX",   _SC_MQ_OPEN_MAX),
    LUABSD_INT("_SC_PAGESIZE",  _SC_PAGESIZE),
    LUABSD_INT("_SC_RTSIG_MAX", _SC_RTSIG_MAX),
    LUABSD_INT("_SC_SEM_NSEMS_MAX", _SC_SEM_NSEMS_MAX),
    LUABSD_INT("_SC_SEM_VALUE_MAX", _SC_SEM_VALUE_MAX),
    LUABSD_INT("_SC_SIGQUEUE_MAX",  _SC_SIGQUEUE_MAX),
    LUABSD_INT("_SC_TIMER_MAX", _SC_TIMER_MAX),
#endif
#if __POSIX_VISIBLE >= 200112
    LUABSD_INT("_SC_2_PBS", _SC_2_PBS),
    LUABSD_INT("_SC_2_PBS_ACCOUNTING",  _SC_2_PBS_ACCOUNTING),
    LUABSD_INT("_SC_2_PBS_CHECKPOINT",  _SC_2_PBS_CHECKPOINT),
    LUABSD_INT("_SC_2_PBS_LOCATE",  _SC_2_PBS_LOCATE),
    LUABSD_INT("_SC_2_PBS_MESSAGE", _SC_2_PBS_MESSAGE),
    LUABSD_INT("_SC_2_PBS_TRACK",   _SC_2_PBS_TRACK),
    LUABSD_INT("_SC_ADVISORY_INFO", _SC_ADVISORY_INFO),
    LUABSD_INT("_SC_BARRIERS",  _SC_BARRIERS),
    LUABSD_INT("_SC_CLOCK_SELECTION",   _SC_CLOCK_SELECTION),
    LUABSD_INT("_SC_CPUTIME",   _SC_CPUTIME),
    LUABSD_INT("_SC_FILE_LOCKING",  _SC_FILE_LOCKING),
    LUABSD_INT("_SC_GETGR_R_SIZE_MAX",  _SC_GETGR_R_SIZE_MAX),
    LUABSD_INT("_SC_GETPW_R_SIZE_MAX",  _SC_GETPW_R_SIZE_MAX),
    LUABSD_INT("_SC_HOST_NAME_MAX", _SC_HOST_NAME_MAX),
    LUABSD_INT("_SC_LOGIN_NAME_MAX",    _SC_LOGIN_NAME_MAX),
    LUABSD_INT("_SC_MONOTONIC_CLOCK",   _SC_MONOTONIC_CLOCK),
    LUABSD_INT("_SC_MQ_PRIO_MAX",   _SC_MQ_PRIO_MAX),
    LUABSD_INT("_SC_READER_WRITER_LOCKS",   _SC_READER_WRITER_LOCKS),
    LUABSD_INT("_SC_REGEXP",    _SC_REGEXP),
    LUABSD_INT("_SC_SHELL", _SC_SHELL),
    LUABSD_INT("_SC_SPAWN", _SC_SPAWN),
    LUABSD_INT("_SC_SPIN_LOCKS",    _SC_SPIN_LOCKS),
    LUABSD_INT("_SC_SPORADIC_SERVER",   _SC_SPORADIC_SERVER),
    LUABSD_INT("_SC_THREAD_ATTR_STACKADDR", _SC_THREAD_ATTR_STACKADDR),
    LUABSD_INT("_SC_THREAD_ATTR_STACKSIZE", _SC_THREAD_ATTR_STACKSIZE),
    LUABSD_INT("_SC_THREAD_CPUTIME",    _SC_THREAD_CPUTIME),
    LUABSD_INT("_SC_THREAD_DESTRUCTOR_ITERATIONS",  _SC_THREAD_DESTRUCTOR_ITERATIONS),
    LUABSD_INT("_SC_THREAD_KEYS_MAX",   _SC_THREAD_KEYS_MAX),
    LUABSD_INT("_SC_THREAD_PRIO_INHERIT",   _SC_THREAD_PRIO_INHERIT),
    LUABSD_INT("_SC_THREAD_PRIO_PROTECT",   _SC_THREAD_PRIO_PROTECT),
    LUABSD_INT("_SC_THREAD_PRIORITY_SCHEDULING",    _SC_THREAD_PRIORITY_SCHEDULING),
    LUABSD_INT("_SC_THREAD_PROCESS_SHARED", _SC_THREAD_PROCESS_SHARED),
    LUABSD_INT("_SC_THREAD_SAFE_FUNCTIONS", _SC_THREAD_SAFE_FUNCTIONS),
    LUABSD_INT("_SC_THREAD_SPORADIC_SERVER",    _SC_THREAD_SPORADIC_SERVER),
    LUABSD_INT("_SC_THREAD_STACK_MIN",  _SC_THREAD_STACK_MIN),
    LUABSD_INT("_SC_THREAD_THREADS_MAX",    _SC_THREAD_THREADS_MAX),
    LUABSD_INT("_SC_TIMEOUTS",  _SC_TIMEOUTS),
    LUABSD_INT("_SC_THREADS",   _SC_THREADS),
    LUABSD_INT("_SC_TRACE", _SC_TRACE),
    LUABSD_INT("_SC_TRACE_EVENT_FILTER",    _SC_TRACE_EVENT_FILTER),
    LUABSD_INT("_SC_TRACE_INHERIT", _SC_TRACE_INHERIT),
    LUABSD_INT("_SC_TRACE_LOG", _SC_TRACE_LOG),
    LUABSD_INT("_SC_TTY_NAME_MAX",  _SC_TTY_NAME_MAX),
    LUABSD_INT("_SC_TYPED_MEMORY_OBJECTS",  _SC_TYPED_MEMORY_OBJECTS),
    LUABSD_INT("_SC_V6_ILP32_OFF32",    _SC_V6_ILP32_OFF32),
    LUABSD_INT("_SC_V6_ILP32_OFFBIG",   _SC_V6_ILP32_OFFBIG),
    LUABSD_INT("_SC_V6_LP64_OFF64", _SC_V6_LP64_OFF64),
    LUABSD_INT("_SC_V6_LPBIG_OFFBIG",   _SC_V6_LPBIG_OFFBIG),
    LUABSD_INT("_SC_IPV6",  _SC_IPV6),
    LUABSD_INT("_SC_RAW_SOCKETS",   _SC_RAW_SOCKETS),
    LUABSD_INT("_SC_SYMLOOP_MAX",   _SC_SYMLOOP_MAX),
#endif
#if __XSI_VISIBLE
    LUABSD_INT("_SC_ATEXIT_MAX",    _SC_ATEXIT_MAX),
    LUABSD_INT("_SC_IOV_MAX",   _SC_IOV_MAX),
    LUABSD_INT("_SC_PAGE_SIZE", _SC_PAGE_SIZE),
    LUABSD_INT("_SC_XOPEN_CRYPT",   _SC_XOPEN_CRYPT),
    LUABSD_INT("_SC_XOPEN_ENH_I18N",    _SC_XOPEN_ENH_I18N),
    LUABSD_INT("_SC_XOPEN_LEGACY",  _SC_XOPEN_LEGACY),
    LUABSD_INT("_SC_XOPEN_REALTIME",    _SC_XOPEN_REALTIME),
    LUABSD_INT("_SC_XOPEN_REALTIME_THREADS",    _SC_XOPEN_REALTIME_THREADS),
    LUABSD_INT("_SC_XOPEN_SHM", _SC_XOPEN_SHM),
    LUABSD_INT("_SC_XOPEN_STREAMS", _SC_XOPEN_STREAMS),
    LUABSD_INT("_SC_XOPEN_UNIX",    _SC_XOPEN_UNIX),
    LUABSD_INT("_SC_XOPEN_VERSION", _SC_XOPEN_VERSION),
    LUABSD_INT("_SC_XOPEN_XCU_VERSION", _SC_XOPEN_XCU_VERSION),
#endif
#if __BSD_VISIBLE
    LUABSD_INT("_SC_NPROCESSORS_CONF",  _SC_NPROCESSORS_CONF),
    LUABSD_INT("_SC_NPROCESSORS_ONLN",  _SC_NPROCESSORS_ONLN),
    LUABSD_INT("_SC_CPUSET_SIZE",   _SC_CPUSET_SIZE),
#endif
    LUABSD_INT("_SC_PHYS_PAGES",    _SC_PHYS_PAGES),
#if __POSIX_VISIBLE >= 199209
    LUABSD_INT("_CS_PATH",  _CS_PATH),
#endif
#if __POSIX_VISIBLE >= 200112
    LUABSD_INT("_CS_POSIX_V6_ILP32_OFF32_CFLAGS",   _CS_POSIX_V6_ILP32_OFF32_CFLAGS),
    LUABSD_INT("_CS_POSIX_V6_ILP32_OFF32_LDFLAGS",  _CS_POSIX_V6_ILP32_OFF32_LDFLAGS),
    LUABSD_INT("_CS_POSIX_V6_ILP32_OFF32_LIBS", _CS_POSIX_V6_ILP32_OFF32_LIBS),
    LUABSD_INT("_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS",  _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS),
    LUABSD_INT("_CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS", _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS),
    LUABSD_INT("_CS_POSIX_V6_ILP32_OFFBIG_LIBS",    _CS_POSIX_V6_ILP32_OFFBIG_LIBS),
    LUABSD_INT("_CS_POSIX_V6_LP64_OFF64_CFLAGS",    _CS_POSIX_V6_LP64_OFF64_CFLAGS),
    LUABSD_INT("_CS_POSIX_V6_LP64_OFF64_LDFLAGS",   _CS_POSIX_V6_LP64_OFF64_LDFLAGS),
    LUABSD_INT("_CS_POSIX_V6_LP64_OFF64_LIBS",  _CS_POSIX_V6_LP64_OFF64_LIBS),
    LUABSD_INT("_CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS",  _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS),
    LUABSD_INT("_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS", _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS),
    LUABSD_INT("_CS_POSIX_V6_LPBIG_OFFBIG_LIBS",    _CS_POSIX_V6_LPBIG_OFFBIG_LIBS),
    LUABSD_INT("_CS_POSIX_V6_WIDTH_RESTRICTED_ENVS",    _CS_POSIX_V6_WIDTH_RESTRICTED_ENVS),
#endif
/* 1003.1-1990 */
    LUABSD_FUNC("access",   luab_access),
    LUABSD_FUNC("alarm",    luab_alarm),
    LUABSD_FUNC("chdir",    luab_chdir),
    LUABSD_FUNC("chown",    luab_chown),
    LUABSD_FUNC("close",    luab_close),
    LUABSD_FUNC("closefrom",    luab_closefrom),
    LUABSD_FUNC("dup",    luab_dup),
    LUABSD_FUNC("dup2",    luab_dup2),
    LUABSD_FUNC("execv",   luab_execv),
    LUABSD_FUNC("execve",   luab_execve),
    LUABSD_FUNC("execvp",   luab_execvp),
    LUABSD_FUNC("fork",   luab_fork),
    LUABSD_FUNC("fpathconf",    luab_fpathconf),
    LUABSD_FUNC("getcwd",   luab_getcwd),
    LUABSD_FUNC("getegid",    luab_getegid),
    LUABSD_FUNC("geteuid",    luab_geteuid),
    LUABSD_FUNC("getgid",    luab_getgid),
    LUABSD_FUNC("getgroups",    luab_getgroups),
    LUABSD_FUNC("getlogin",   luab_getlogin),
    LUABSD_FUNC("getpgrp",    luab_getpgrp),
    LUABSD_FUNC("getpid", luab_getpid),
    LUABSD_FUNC("getppid",    luab_getppid),
    LUABSD_FUNC("getuid", luab_getuid),
    LUABSD_FUNC("isatty",   luab_isatty),
    LUABSD_FUNC("link", luab_link),
#ifndef _LSEEK_DECLARED
#define _LSEEK_DECLARED
    LUABSD_FUNC("lseek", luab_lseek),
#endif
    LUABSD_FUNC("pathconf",    luab_pathconf),
    LUABSD_FUNC("pause",    luab_pause),
    LUABSD_FUNC("pipe", luab_pipe),
    LUABSD_FUNC("pipe2", luab_pipe2),
    LUABSD_FUNC("rmdir",    luab_rmdir),
    LUABSD_FUNC("setgid",    luab_setgid),
    LUABSD_FUNC("setpgid",    luab_setpgid),
    LUABSD_FUNC("setpgrp",    luab_setpgrp),
    LUABSD_FUNC("setsid", luab_setsid),
    LUABSD_FUNC("setuid", luab_setuid),
    LUABSD_FUNC("ttyname",  luab_ttyname),
    LUABSD_FUNC("ttyname_r",  luab_ttyname_r),
    LUABSD_FUNC("unlink",   luab_unlink),

/* 1003.2-1992 */
#if __POSIX_VISIBLE >= 199209 || __XSI_VISIBLE
/*
    LUABSD_FUNC("confstr",  luab_confstr),
 */
#ifndef _GETOPT_DECLARED
#define _GETOPT_DECLARED
/*
    LUABSD_FUNC("getopt",   luab_getopt),
 */
#endif /* _GETOPT_DECLARED */
#endif

/* ISO/IEC 9945-1: 1996 */
#if __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE

/*
    LUABSD_FUNC("fsync",    luab_fsync),
    LUABSD_FUNC("fdatasync",    luab_fdatasync),
 */
#ifndef _FTRUNCATE_DECLARED
#define _FTRUNCATE_DECLARED
    LUABSD_FUNC("ftruncate",    luab_ftruncate),
#endif
#endif

#if __POSIX_VISIBLE >= 199506
/*
    LUABSD_FUNC("getlogin_r",   luab_getlogin_r),
 */
#endif
/* 1003.1-2001 */
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
    LUABSD_FUNC("fchown",   luab_fchown),
/*
    LUABSD_FUNC("readlink", luab_readlink),
 */
#endif
#if __POSIX_VISIBLE >= 200112
    LUABSD_FUNC("gethostname",  luab_gethostname),
    LUABSD_FUNC("setegid",    luab_setegid),
    LUABSD_FUNC("seteuid",    luab_seteuid),
#endif
/* 1003.1-2008 */
#if __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE
    LUABSD_FUNC("getsid", luab_getsid),
    LUABSD_FUNC("fchdir",    luab_fchdir),
    LUABSD_FUNC("getpgid",    luab_getpgid),
    LUABSD_FUNC("lchown",   luab_lchown),
/*
    LUABSD_FUNC("pread",    luab_pread),
    LUABSD_FUNC("pwrite",   luab_pwrite),
 */
#ifndef _TRUNCATE_DECLARED
#define _TRUNCATE_DECLARED
    LUABSD_FUNC("truncate", luab_truncate),
#endif
#endif /* __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE */

#if __POSIX_VISIBLE >= 200809
    LUABSD_FUNC("faccessat",   luab_faccessat),
    LUABSD_FUNC("fchownat", luab_fchownat),
    LUABSD_FUNC("fexecve",   luab_fexecve),
    LUABSD_FUNC("linkat", luab_linkat),
    LUABSD_FUNC("unlinkat", luab_unlinkat),
#endif /* __POSIX_VISIBLE >= 200809 */
#if (__XSI_VISIBLE && __XSI_VISIBLE <= 600) || __BSD_VISIBLE
    LUABSD_FUNC("getwd",   luab_getwd),
#endif
#if __BSD_VISIBLE
    LUABSD_FUNC("eaccess",   luab_eaccess),
    LUABSD_FUNC("lpathconf",    luab_lpathconf),
    LUABSD_FUNC("sethostname",  luab_sethostname),
    LUABSD_FUNC("setlogin",   luab_setlogin),
#endif /* __BSD_VISIBLE */
    LUABSD_FUNC(NULL, NULL)
};

luab_module_t luab_unistd_lib = {
    .cookie = LUABSD_UNISTD_LIB_ID,
    .name = LUABSD_UNISTD_LIB_KEY,
    .vec = luab_unistd_vec,
};