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

#include <dirent.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#define LUAB_DIRENT_LIB_ID    1604795103
#define LUAB_DIRENT_LIB_KEY    "dirent"

/*
 * XXX
 *
 *  int
 *   scandir(const char *dirname, struct dirent ***namelist,
 *       int (*select)(const struct dirent *),
 *       int (*compar)(const struct dirent **, const struct dirent **));
 *
 *   int
 *   scandir_b(const char *dirname, struct dirent ***namelist,
 *       int (*select(rp(const struct dirent *),
 *       int (^compar)(const struct dirent **, const struct dirent **));
 *
 *   int
 *   alphasort(const struct dirent **d1, const struct dirent **d2);
 *
 * Implementation depends on specific luab_type(3) implements autoboxing
 * for (LUA_TCFUNCTION) objects or set of callbacks over vector table.
 */

extern luab_module_t luab_dirent_lib;

/*
 * Subr.
 */

static luab_table_t *
luab_table_checkdirent(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct dirent *x, *dp;
    size_t m, n, sz;

    sz = sizeof(struct dirent);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (struct dirent *)(tbl->tbl_vec)) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        dp = luab_udata(L, -1, luab_mx(DIRENT), struct dirent *);
                        (void)memmove(&(x[m]), dp, sz);
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        }
    }
    return (tbl);
}

static void
luab_table_pushdirent(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct dirent *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, luab_mx(DIRENT), k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = EINVAL;
}

/*
 * Service primitives.
 */

#if __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE >= 700
/***
 * dirfd(3) - directory operations
 *
 * @function dirfd
 *
 * @param dirp              Specifies named directory stream.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.dirent.dirfd(dirp)
 */
static int
luab_dirfd(lua_State *L)
{
    DIR *dirp;
    int fd;

    (void)luab_core_checkmaxargs(L, 1);

    dirp = luab_udata(L, 1, luab_mx(DIR), DIR *);
    fd = dirfd(dirp);

    return (luab_pusherr(L, fd));
}
#endif /* __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE >= 700 */

#if __BSD_VISIBLE
/***
 * fdclosedir(3) - directory operations
 *
 * @function fdclosedir
 *
 * @param name              Specifies directory stream by its name.
 * @param flags             Values from
 *
 *                              bsd.dirent.{__}DTF_{
 *                                  NODUP,
 *                                  REWIND,
 *                                  READALL,
 *                                  SKIPREAD
 *                              }
 *
 *                          are constructed by inclusive-OR.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage dir [, err, msg ] = bsd.dirent.opendir2(dirp, flags)
 */
static int
luab_opendir2(lua_State *L)
{
    const char *name;
    int flags;
    DIR *dirp;
    luab_module_t *m;

    (void)luab_core_checkmaxargs(L, 2);

    name = luab_checklstring(L, 1, MAXPATHLEN);
    flags = (int)luab_checkinteger(L, 2, INT_MAX);

    if ((dirp = __opendir2(name, flags)) != NULL)
        m = luab_mx(DIR);
    else
        m = NULL;

    return (luab_pushudata(L, m, dirp));
}

/***
 * getdents(2) - get directory entries in a filsytem independent format
 *
 * @function getdents
 *
 * @param fd                Specifies directory stream by file descriptor.
 * @param buf               Temporary storage, instance of (LUA_TABLE)
 *
 *                              buf = {
 *                                  dirent0,
 *                                  dirent1,
 *                                   ...
 *                                  direntN
 *                              }
 *
 *                          over (LUA_TUSERDATA(DIRENT)).
 *
 * @param nbytes            Reflects the cardinality of (LUA_TTABLE).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.dirent.getdents(fd, buf, nbytes)
 */
static int
luab_getdents(lua_State *L)
{
    int fd;
    luab_table_t *tbl;
    size_t nbytes;
    ssize_t count;

    (void)luab_core_checkmaxargs(L, 3);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    tbl = luab_table_checkdirent(L, 2);
    nbytes = (size_t)luab_checklinteger(L, 3);

    if (tbl != NULL && nbytes > 0) {

        if ((tbl->tbl_card - 1) == nbytes) {
            nbytes *= tbl->tbl_sz;

            count = getdents(fd, tbl->tbl_vec, nbytes);
            luab_table_pushdirent(L, 2, tbl, 0, 1);
        } else {
            luab_table_free(tbl);
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = ERANGE;
        count = -1;
    }
    return (luab_pusherr(L, count));
}

/***
 * getdirentries(2) - get directory entries in a filsytem independent format
 *
 * @function getdirentries
 *
 * @param fd                Specifies directory stream by file descriptor.
 * @param buf               Temporary storage, instance of (LUA_TABLE)
 *
 *                              buf = {
 *                                  dirent0,
 *                                  dirent1,
 *                                   ...
 *                                  direntN
 *                              }
 *
 *                          over (LUA_TUSERDATA(DIRENT)).
 *
 * @param nbytes            Reflects the cardinality of (LUA_TTABLE).
 * @param basep             Specifies location for position block read.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.dirent.getdirentries(fd, buf, nbytes, basep)
 */
static int
luab_getdirentries(lua_State *L)
{
    int fd;
    luab_table_t *tbl;
    size_t nbytes;
    luab_primitive_u *xp;
    off_t *basep;
    ssize_t count;

    (void)luab_core_checkmaxargs(L, 4);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);
    tbl = luab_table_checkdirent(L, 2);
    nbytes = (size_t)luab_checklinteger(L, 3);
    xp = luab_udataisnil(L, 4, luab_mx(PRIMITIVE), luab_primitive_u *);

    if (xp != NULL)
        basep = &(xp->un_off);
    else
        basep = NULL;

    if (tbl != NULL && nbytes > 0) {

        if ((tbl->tbl_card - 1) == nbytes) {
            nbytes *= tbl->tbl_sz;

            count = getdirentries(fd, tbl->tbl_vec, nbytes, basep);
            luab_table_pushdirent(L, 2, tbl, 0, 1);
        } else {
            luab_table_free(tbl);
            errno = ERANGE;
            count = -1;
        }
    } else {
        errno = ERANGE;
        count = -1;
    }
    return (luab_pusherr(L, count));
}

/***
 * fdclosedir(3) - directory operations
 *
 * @function fdclosedir
 *
 * @param dirp              Specifies named directory stream.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.dirent.fdclosedir(dirp)
 */
static int
luab_fdclosedir(lua_State *L)
{
    DIR *dirp;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    dirp = luab_udata(L, 1, luab_mx(DIR), DIR *);
    status = fdclosedir(dirp);

    return (luab_pusherr(L, status));
}
#endif /* __BSD_VISIBLE */

/***
 * opendir(3) - directory operations
 *
 * @function opendir
 *
 * @param filename          Specifies directory named by filename.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage dir [, err, msg ] = bsd.dirent.opendir(filename)
 */
static int
luab_opendir(lua_State *L)
{
    const char *filename;
    DIR *dirp;
    luab_module_t *m;

    (void)luab_core_checkmaxargs(L, 1);

    filename = luab_checklstring(L, 1, MAXPATHLEN);

    if ((dirp = opendir(filename)) != NULL)
        m = luab_mx(DIR);
    else
        m = NULL;

    return (luab_pushudata(L, m, dirp));
}

/***
 * fdopendir(3) - directory operations
 *
 * @function fdopendir
 *
 * @param fd                Specifies directory named by file descriptor.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage dir [, err, msg ] = bsd.dirent.fdopendir(fd)
 */
static int
luab_fdopendir(lua_State *L)
{
    int fd;
    DIR *dirp;
    luab_module_t *m;

    (void)luab_core_checkmaxargs(L, 1);

    fd = (int)luab_checkinteger(L, 1, INT_MAX);

    if ((dirp = fdopendir(fd)) != NULL)
        m = luab_mx(DIR);
    else
        m = NULL;

    return (luab_pushudata(L, m, dirp));
}

/***
 * readdir(3) - directory operations
 *
 * @function readdir
 *
 * @param dirp              Specifies named directory stream.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage dirent [, err, msg ] = bsd.dirent.readdir(dirp)
 */
static int
luab_readdir(lua_State *L)
{
    DIR *dirp;
    struct dirent *dp;
    luab_module_t *m;

    (void)luab_core_checkmaxargs(L, 1);

    dirp = luab_udata(L, 1, luab_mx(DIR), DIR *);

    if ((dp = readdir(dirp)) != NULL)
        m = luab_mx(DIRENT);
    else
        m = NULL;

    return (luab_pushudata(L, m, dp));
}

#if __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE >= 500
/***
 * readdir_r(3) - directory operations
 *
 * @function readdir_r
 *
 * @param dirp              Specifies named directory stream.
 * @param entry             Temporary storage, instance of (LUA_TUSERDATA(DIRENT)).
 * @param result            Result argument, instance of (LUA_TUSERDATA(DIREN)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.dirent.readdir_r(dirp)
 */
static int
luab_readdir_r(lua_State *L)
{
    DIR *dirp;
    struct dirent *entry;
    struct dirent *ret;
    struct dirent *result;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    dirp = luab_udata(L, 1, luab_mx(DIR), DIR *);
    entry = luab_udata(L, 2, luab_mx(DIRENT), struct dirent *);
    ret = luab_udata(L, 3, luab_mx(DIRENT), struct dirent *);

    if ((status = readdir_r(dirp, entry, &result)) == 0) {
        if (result != NULL)
            (void)memmove(ret, result, sizeof(struct dirent));
    }
    return (luab_pusherr(L, status));
}
#endif /* __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE >= 500 */

/***
 * rewinddir(3) - directory operations
 *
 * @function rewinddir
 *
 * @param dirp              Specifies named directory stream.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.dirent.rewinddir(dirp)
 */
static int
luab_rewinddir(lua_State *L)
{
    DIR *dirp;

    (void)luab_core_checkmaxargs(L, 1);

    dirp = luab_udata(L, 1, luab_mx(DIR), DIR *);
    rewinddir(dirp);
    return (luab_pusherr(L, 0));
}

#if __XSI_VISIBLE
/***
 * seekdir(3) - directory operations
 *
 * @function seekdir
 *
 * @param dirp              Specifies named directory stream.
 * @param loc               Specifies position on directory stream.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.dirent.seekdir(dirp, loc)
 */
static int
luab_seekdir(lua_State *L)
{
    DIR *dirp;
    long loc;

    (void)luab_core_checkmaxargs(L, 2);

    dirp = luab_udata(L, 1, luab_mx(DIR), DIR *);
    loc = (long)luab_checkinteger(L, 2, LONG_MAX);

    seekdir(dirp, loc);
    return (luab_pusherr(L, 0));
}

/***
 * telldir(3) - directory operations
 *
 * @function telldir
 *
 * @param dirp              Specifies named directory stream.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.dirent.telldir(dirp)
 */
static int
luab_telldir(lua_State *L)
{
    DIR *dirp;
    long tok;

    (void)luab_core_checkmaxargs(L, 1);

    dirp = luab_udata(L, 1, luab_mx(DIR), DIR *);

    tok = telldir(dirp);
    return (luab_pusherr(L, tok));
}
#endif /* __XSI_VISIBLE */

/***
 * closedir(3) - directory operations
 *
 * @function closedir
 *
 * @param dirp              Specifies named directory stream.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.dirent.closedir(dirp)
 */
static int
luab_closedir(lua_State *L)
{
    DIR *dirp;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    dirp = luab_udata(L, 1, luab_mx(DIR), DIR *);
    status = closedir(dirp);
    return (luab_pusherr(L, status));
}

/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(DIR)).
 *
 * @function dir_create
 *
 * @param dir               Instance of (LUA_TUSERDATA(DIR)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage dir [, err, msg ] = bsd.dirent.dir_create([ dir ])
 */
static int
luab_dir_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(DIR), NULL));
}

/*
 * Interface against <dirent.h>.
 */

static luab_module_table_t luab_dirent_vec[] = { /* dirent.h */
#if __BSD_VISIBLE
    LUAB_INT("DIRBLKSIZ",               DIRBLKSIZ),
    LUAB_INT("DTF_HIDEW",               DTF_HIDEW),
    LUAB_INT("DTF_NODUP",               DTF_NODUP),
    LUAB_INT("DTF_REWIND",              DTF_REWIND),
    LUAB_INT("__DTF_READALL",           __DTF_READALL),
    LUAB_INT("__DTF_SKIPREAD",          __DTF_SKIPREAD),
#endif /* __BSD_VISIBLE */
#if __POSIX_VISIBLE >= 200809 || __XSI_VISIBLE >= 700
    LUAB_FUNC("dirfd",                  luab_dirfd),
#endif
#if __BSD_VISIBLE
    LUAB_FUNC("opendir2",               luab_opendir2),
    LUAB_FUNC("fdclosedir",             luab_fdclosedir),
    LUAB_FUNC("getdents",               luab_getdents),
    LUAB_FUNC("getdirentries",          luab_getdirentries),
#endif
    LUAB_FUNC("opendir",                luab_opendir),
    LUAB_FUNC("fdopendir",              luab_fdopendir),
    LUAB_FUNC("readdir",                luab_readdir),
#if __POSIX_VISIBLE >= 199506 || __XSI_VISIBLE >= 500
    LUAB_FUNC("readdir_r",              luab_readdir_r),
#endif
    LUAB_FUNC("rewinddir",              luab_rewinddir),
#if __XSI_VISIBLE
    LUAB_FUNC("seekdir",                luab_seekdir),
    LUAB_FUNC("telldir",                luab_telldir),
#endif
    LUAB_FUNC("closedir",               luab_closedir),
    LUAB_FUNC("dir_create",             luab_dir_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_dirent_lib = {
    .m_cookie   = LUAB_DIRENT_LIB_ID,
    .m_name     = LUAB_DIRENT_LIB_KEY,
    .m_vec      = luab_dirent_vec,
};
