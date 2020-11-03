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

#define LUAB_SYS_MOUNT_LIB_ID    1604415113
#define LUAB_SYS_MOUNT_LIB_KEY   "mount"

extern luab_module_t luab_sys_mount_lib;

/*
 * Service primitives.
 */

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
    LUAB_FUNC("fsid_create",            luab_fsid_create),
    LUAB_FUNC("fid_create",             luab_fid_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_mount_lib = {
    .m_cookie   = LUAB_SYS_MOUNT_LIB_ID,
    .m_name     = LUAB_SYS_MOUNT_LIB_KEY,
    .m_vec      = luab_sys_mount_vec,
};
