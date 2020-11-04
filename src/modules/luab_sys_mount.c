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

#include <sys/mount.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_SYS_MOUNT_LIB_ID    1604415113
#define LUAB_SYS_MOUNT_LIB_KEY   "mount"

extern luab_module_t luab_sys_mount_lib;

/*
 * Service primitives.
 */

/***
 * fhlink(2) - make a hard file link
 *
 * @function fhlink
 *
 * @param fhp               Identifies the file object.
 * @param to                Specifies directory entry.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.mount.fhlink(fhp, to)
 */
static int
luab_fhlink(lua_State *L)
{
    fhandle_t *fhp;
    const char *to;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    fhp = luab_udata(L, 1, luab_mx(FHANDLE), fhandle_t *);
    to = luab_checklstring(L, 2, MAXPATHLEN);

    status = fhlink(fhp, to);

    return (luab_pusherr(L, status));
}

/***
 * fhlinkat(2) - make a hard file link
 *
 * @function fhlinkat
 *
 * @param fhp               Identifies the file object.
 * @param tofd              Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes referenced file object.
 *
 *                            #2 By path named object is relative to the
 *                               directory to associated with the file
 *                               descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of fhlinkat(2).
 *
 * @param to                Specifies directory entry.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.mount.fhlinkat(fhp, tofd, to)
 */
static int
luab_fhlinkat(lua_State *L)
{
    fhandle_t *fhp;
    int tofd;
    const char *to;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    fhp = luab_udata(L, 1, luab_mx(FHANDLE), fhandle_t *);
    tofd = (int)luab_checkinteger(L, 2, INT_MAX);
    to = luab_checklstring(L, 3, MAXPATHLEN);

    status = fhlinkat(fhp, tofd, to);

    return (luab_pusherr(L, status));
}

/***
 * fhopen(2) - access file via file handle
 *
 * @function fhopen
 *
 * @param fhp               Identifies the file object.
 * @param flags             Values are constructed from
 *
 *                              bsd.fcntl.O_*
 *
 *                          by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.mount.fhopen(fhp, flags)
 */
static int
luab_fhopen(lua_State *L)
{
    fhandle_t *fhp;
    int flags;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    fhp = luab_udata(L, 1, luab_mx(FHANDLE), fhandle_t *);
    flags = (int)luab_checkinteger(L, 2, INT_MAX);

    status = fhopen(fhp, flags);

    return (luab_pusherr(L, status));
}

/***
 * fhstat(2) - access file via file handle
 *
 * @function fhstat
 *
 * @param fhp               Identifies the file object.
 * @param sb                Result argument, instance of (LUA_TUSERDATA(STAT)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.mount.fhstat(fhp, sb)
 */
static int
luab_fhstat(lua_State *L)
{
    fhandle_t *fhp;
    struct stat *sb;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    fhp = luab_udata(L, 1, luab_mx(FHANDLE), fhandle_t *);
    sb = luab_udata(L, 2, luab_mx(STAT), struct stat *);

    status = fhstat(fhp, sb);

    return (luab_pusherr(L, status));
}

/***
 * fhstatfs(2) - access file via file handle
 *
 * @function fhstatfs
 *
 * @param fhp               Identifies the file object.
 * @param buf               Result argument, instance of (LUA_TUSERDATA(STATFS)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.mount.fhstatfs(fhp, statfs)
 */
static int
luab_fhstatfs(lua_State *L)
{
    fhandle_t *fhp;
    struct statfs *buf;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    fhp = luab_udata(L, 1, luab_mx(FHANDLE), fhandle_t *);
    buf = luab_udata(L, 2, luab_mx(STATFS), struct statfs *);

    status = fhstatfs(fhp, buf);

    return (luab_pusherr(L, status));
}

/***
 * fstatfs(2) - get file system statistics
 *
 * @function fstatfs
 *
 * @param fd                Open file descriptor points to an object within the
 *                          mounted file system.
 * @param buf               Result argument, instance of (LUA_TUSERDATA(STATFS)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.mount.fstatfs(fd, fd)
 */
static int
luab_fstatfs(lua_State *L)
{
    int fd;
    struct statfs *buf;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    buf = luab_udata(L, 2, luab_mx(STATFS), struct statfs *);

    status = fstatfs(fd, buf);

    return (luab_pusherr(L, status));
}

/***
 * getfh(2) - get file handle
 *
 * @function getfh
 *
 * @param path              Specifies the path name for any file residing
 *                          within the mounted file system.
 * @param fhp               Result argument, instance of (LUA_TUSERDATA(FHANDLE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.mount.getfh(path, fhp)
 */
static int
luab_getfh(lua_State *L)
{
    const char *path;
    fhandle_t *fhp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    fhp = luab_udata(L, 2, luab_mx(FHANDLE), fhandle_t *);

    status = getfh(path, fhp);

    return (luab_pusherr(L, status));
}

/***
 * getfhat(2) - get file handle
 *
 * @function getfhat
 *
 * @param fd                Filedescriptor, three cases are considered here:
 *
 *                            #1 Denotes referenced file object.
 *
 *                            #2 By path named object is relative to the
 *                               directory to associated with the file
 *                               descriptor.
 *
 *                            #3 The current working directory is used, when
 *
 *                                  bsd.fcntl.AT_FDCWD
 *
 *                               was passed by call of fhlinkat(2).
 *
 * @param path              Specifies the path name for any file residing
 *                          within the mounted file system.
 * @param fhp               Result argument, instance of (LUA_TUSERDATA(FHANDLE)).
 * @param flags             Values from
 *
 *                                  bsd.fcntl.AT_{
 *                                      SYMLINK_FOLLOW,
 *                                      BENEATH
 *                                  }
 *
 *                          are constructed by bitwise-inclusive OR.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.mount.getfhat(fd, path, fhp, flag)
 */
static int
luab_getfhat(lua_State *L)
{
    int fd;
    const char *path;
    fhandle_t *fhp;
    int flag, status;

    (void)luab_core_checkmaxargs(L, 4);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    path = luab_checklstring(L, 2, MAXPATHLEN);
    fhp = luab_udata(L, 3, luab_mx(FHANDLE), fhandle_t *);
    flag = (int)luab_checkinteger(L, 4, INT_MAX);

    status = getfhat(fd, (void *)(intptr_t)path, fhp, flag);

    return (luab_pusherr(L, status));
}


/***
 * lgetfh(2) - get file handle
 *
 * @function lgetfh
 *
 * @param path              Specifies the path name for any file residing within
 *                          the mounted file system. But if by path specified
 *                          object denotes a symbolic link, then information
 *                          about this link are described by returned file handle.
 * @param fhp               Result argument, instance of (LUA_TUSERDATA(FHANDLE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.mount.lgetfh(path, fhp)
 */
static int
luab_lgetfh(lua_State *L)
{
    const char *path;
    fhandle_t *fhp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    fhp = luab_udata(L, 2, luab_mx(FHANDLE), fhandle_t *);

    status = lgetfh(path, fhp);

    return (luab_pusherr(L, status));
}

/***
 * statfs(2) - get file system statistics
 *
 * @function statfs
 *
 * @param path              Specifies the path name for any file residing
 *                          within the mounted file system.
 * @param buf               Result argument, instance of (LUA_TUSERDATA(STATFS)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.sys.mount.statfs(path, buf)
 */
static int
luab_statfs(lua_State *L)
{
    const char *path;
    struct statfs *buf;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    buf = luab_udata(L, 2, luab_mx(STATFS), struct statfs *);

    status = statfs(path, buf);

    return (luab_pusherr(L, status));
}


/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(FSID)).
 *
 * @function fsid_create
 *
 * @param fsid              Instance of (LUA_TUSERDATA(FSID)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage fsid [, err, msg ] = bsd.sys.mount.fsid_create([ fsid ])
 */
static int
luab_fsid_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(FSID), NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(FID)).
 *
 * @function fid_create
 *
 * @param fid               Instance of (LUA_TUSERDATA(FID)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage fid [, err, msg ] = bsd.sys.mount.fid_create([ fid ])
 */
static int
luab_fid_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(FID), NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(STATFS)).
 *
 * @function statfs_create
 *
 * @param statfs            Instance of (LUA_TUSERDATA(FID)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage statfs [, err, msg ] = bsd.sys.mount.statfs_create([ statfs ])
 */
static int
luab_statfs_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(STATFS), NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(FHANDLE)).
 *
 * @function fhandle_create
 *
 * @param fhandle           Instance of (LUA_TUSERDATA(FHANDLE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage fhandle [, err, msg ] = bsd.sys.mount.statfs_create([ fhandle ])
 */
static int
luab_fhandle_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(FHANDLE), NULL));
}

/*
 * Interface against <sys/mount.h>
 */

static luab_module_table_t luab_sys_mount_vec[] = {
    LUAB_INT("MAXFIDSZ",                MAXFIDSZ),
    LUAB_INT("MFSNAMELEN",              MFSNAMELEN),
    LUAB_INT("MNAMELEN",                MNAMELEN),
    LUAB_INT("STATFS_VERSION",          STATFS_VERSION),
    LUAB_INT("MNT_RDONLY",              MNT_RDONLY),
    LUAB_INT("MNT_SYNCHRONOUS",         MNT_SYNCHRONOUS),
    LUAB_INT("MNT_NOEXEC",              MNT_NOEXEC),
    LUAB_INT("MNT_NOSUID",              MNT_NOSUID),
    LUAB_INT("MNT_NFS4ACLS",            MNT_NFS4ACLS),
    LUAB_INT("MNT_UNION",               MNT_UNION),
    LUAB_INT("MNT_ASYNC",               MNT_ASYNC),
    LUAB_INT("MNT_SUIDDIR",             MNT_SUIDDIR),
    LUAB_INT("MNT_SOFTDEP",             MNT_SOFTDEP),
    LUAB_INT("MNT_NOSYMFOLLOW",         MNT_NOSYMFOLLOW),
    LUAB_INT("MNT_GJOURNAL",            MNT_GJOURNAL),
    LUAB_INT("MNT_MULTILABEL",          MNT_MULTILABEL),
    LUAB_INT("MNT_ACLS",                MNT_ACLS),
    LUAB_INT("MNT_NOATIME",             MNT_NOATIME),
    LUAB_INT("MNT_NOCLUSTERR",          MNT_NOCLUSTERR),
    LUAB_INT("MNT_NOCLUSTERW",          MNT_NOCLUSTERW),
    LUAB_INT("MNT_SUJ",                 MNT_SUJ),
    LUAB_INT("MNT_AUTOMOUNTED",         MNT_AUTOMOUNTED),
    LUAB_INT("MNT_UNTRUSTED",           MNT_UNTRUSTED),
    LUAB_INT("MNT_EXRDONLY",            MNT_EXRDONLY),
    LUAB_INT("MNT_EXPORTED",            MNT_EXPORTED),
    LUAB_INT("MNT_DEFEXPORTED",         MNT_DEFEXPORTED),
    LUAB_INT("MNT_EXPORTANON",          MNT_EXPORTANON),
    LUAB_INT("MNT_EXKERB",              MNT_EXKERB),
    LUAB_INT("MNT_EXPUBLIC",            MNT_EXPUBLIC),
    LUAB_INT("MNT_LOCAL",               MNT_LOCAL),
    LUAB_INT("MNT_QUOTA",               MNT_QUOTA),
    LUAB_INT("MNT_ROOTFS",              MNT_ROOTFS),
    LUAB_INT("MNT_USER",                MNT_USER),
    LUAB_INT("MNT_IGNORE",              MNT_IGNORE),
    LUAB_INT("MNT_VERIFIED",            MNT_VERIFIED),
    LUAB_INT("MNT_VISFLAGMASK",         MNT_VISFLAGMASK),
    LUAB_INT("MNT_UPDATEMASK",          MNT_UPDATEMASK),
    LUAB_INT("MNT_UPDATE",              MNT_UPDATE),
    LUAB_INT("MNT_DELEXPORT",           MNT_DELEXPORT),
    LUAB_INT("MNT_RELOAD",              MNT_RELOAD),
    LUAB_INT("MNT_FORCE",               MNT_FORCE),
    LUAB_INT("MNT_SNAPSHOT",            MNT_SNAPSHOT),
    LUAB_INT("MNT_NONBUSY",             MNT_NONBUSY),
    LUAB_INT("MNT_BYFSID",              MNT_BYFSID),
    LUAB_INT("MNT_CMDFLAGS",            MNT_CMDFLAGS),
    LUAB_INT("MNTK_UNMOUNTF",           MNTK_UNMOUNTF),
    LUAB_INT("MNTK_ASYNC",              MNTK_ASYNC),
    LUAB_INT("MNTK_SOFTDEP",            MNTK_SOFTDEP),
    LUAB_INT("MNTK_DRAINING",           MNTK_DRAINING),
    LUAB_INT("MNTK_REFEXPIRE",          MNTK_REFEXPIRE),
    LUAB_INT("MNTK_EXTENDED_SHARED",    MNTK_EXTENDED_SHARED),
    LUAB_INT("MNTK_SHARED_WRITES",      MNTK_SHARED_WRITES),
    LUAB_INT("MNTK_NO_IOPF",            MNTK_NO_IOPF),
    LUAB_INT("MNTK_VGONE_UPPER",        MNTK_VGONE_UPPER),
    LUAB_INT("MNTK_VGONE_WAITER",       MNTK_VGONE_WAITER),
    LUAB_INT("MNTK_LOOKUP_EXCL_DOTDOT", MNTK_LOOKUP_EXCL_DOTDOT),
    LUAB_INT("MNTK_MARKER",             MNTK_MARKER),
    LUAB_INT("MNTK_UNMAPPED_BUFS",      MNTK_UNMAPPED_BUFS),
    LUAB_INT("MNTK_USES_BCACHE",        MNTK_USES_BCACHE),
    LUAB_INT("MNTK_TEXT_REFS",          MNTK_TEXT_REFS),
    LUAB_INT("MNTK_NOASYNC",            MNTK_NOASYNC),
    LUAB_INT("MNTK_UNMOUNT",            MNTK_UNMOUNT),
    LUAB_INT("MNTK_MWAIT",              MNTK_MWAIT),
    LUAB_INT("MNTK_SUSPEND",            MNTK_SUSPEND),
    LUAB_INT("MNTK_SUSPEND2",           MNTK_SUSPEND2),
    LUAB_INT("MNTK_SUSPENDED",          MNTK_SUSPENDED),
    LUAB_INT("MNTK_NULL_NOCACHE",       MNTK_NULL_NOCACHE),
    LUAB_INT("MNTK_LOOKUP_SHARED",      MNTK_LOOKUP_SHARED),
    LUAB_INT("MNTK_NOKNOTE",            MNTK_NOKNOTE),
    LUAB_INT("VFS_VFSCONF",             VFS_VFSCONF),
    LUAB_INT("VFS_GENERIC",             VFS_GENERIC),
    LUAB_INT("VFS_MAXTYPENUM",          VFS_MAXTYPENUM),
    LUAB_INT("VFS_CONF",                VFS_CONF),
    LUAB_INT("MNT_WAIT",                MNT_WAIT),
    LUAB_INT("MNT_NOWAIT",              MNT_NOWAIT),
    LUAB_INT("MNT_LAZY",                MNT_LAZY),
    LUAB_INT("MNT_SUSPEND",             MNT_SUSPEND),
    LUAB_INT("MAXSECFLAVORS",           MAXSECFLAVORS),
    LUAB_INT("VFCF_STATIC",             VFCF_STATIC),
    LUAB_INT("VFCF_NETWORK",            VFCF_NETWORK),
    LUAB_INT("VFCF_READONLY",           VFCF_READONLY),
    LUAB_INT("VFCF_SYNTHETIC",          VFCF_SYNTHETIC),
    LUAB_INT("VFCF_LOOPBACK",           VFCF_LOOPBACK),
    LUAB_INT("VFCF_UNICODE",            VFCF_UNICODE),
    LUAB_INT("VFCF_JAIL",               VFCF_JAIL),
    LUAB_INT("VFCF_DELEGADMIN",         VFCF_DELEGADMIN),
    LUAB_INT("VFCF_SBDRY",              VFCF_SBDRY),
    LUAB_INT("VFS_CTL_VERS1",           VFS_CTL_VERS1),
    LUAB_INT("VFS_CTL_QUERY",           VFS_CTL_QUERY),
    LUAB_INT("VFS_CTL_TIMEO",           VFS_CTL_TIMEO),
    LUAB_INT("VFS_CTL_NOLOCKS",         VFS_CTL_NOLOCKS),
    LUAB_INT("VQ_NOTRESP",              VQ_NOTRESP),
    LUAB_INT("VQ_NEEDAUTH",             VQ_NEEDAUTH),
    LUAB_INT("VQ_LOWDISK",              VQ_LOWDISK),
    LUAB_INT("VQ_MOUNT",                VQ_MOUNT),
    LUAB_INT("VQ_UNMOUNT",              VQ_UNMOUNT),
    LUAB_INT("VQ_DEAD",                 VQ_DEAD),
    LUAB_INT("VQ_ASSIST",               VQ_ASSIST),
    LUAB_INT("VQ_NOTRESPLOCK",          VQ_NOTRESPLOCK),
    LUAB_INT("VQ_FLAG0100",             VQ_FLAG0100),
    LUAB_INT("VQ_FLAG0200",             VQ_FLAG0200),
    LUAB_INT("VQ_FLAG0400",             VQ_FLAG0400),
    LUAB_INT("VQ_FLAG0800",             VQ_FLAG0800),
    LUAB_INT("VQ_FLAG1000",             VQ_FLAG1000),
    LUAB_INT("VQ_FLAG2000",             VQ_FLAG2000),
    LUAB_INT("VQ_FLAG4000",             VQ_FLAG4000),
    LUAB_INT("VQ_FLAG8000",             VQ_FLAG8000),
    LUAB_FUNC("fhlink",                 luab_fhlink),
    LUAB_FUNC("fhlinkat",               luab_fhlinkat),
    LUAB_FUNC("fhopen",                 luab_fhopen),

    LUAB_FUNC("fhstat",                 luab_fhstat),
    LUAB_FUNC("fhstatfs",               luab_fhstatfs),
    LUAB_FUNC("fstatfs",                luab_fstatfs),
    LUAB_FUNC("getfh",                  luab_getfh),
    LUAB_FUNC("getfhat",                luab_getfhat),

    LUAB_FUNC("lgetfh",                 luab_lgetfh),

    LUAB_FUNC("statfs",                 luab_statfs),

    LUAB_FUNC("fsid_create",            luab_fsid_create),
    LUAB_FUNC("fid_create",             luab_fid_create),
    LUAB_FUNC("statfs_create",          luab_statfs_create),
    LUAB_FUNC("fhandle_create",         luab_fhandle_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_mount_lib = {
    .m_cookie   = LUAB_SYS_MOUNT_LIB_ID,
    .m_name     = LUAB_SYS_MOUNT_LIB_KEY,
    .m_vec      = luab_sys_mount_vec,
};