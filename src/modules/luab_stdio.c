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

#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_STDIO_LIB_ID    1605194991
#define LUAB_STDIO_LIB_KEY    "stdio"

extern luab_module_t luab_stdio_lib;

/*
 * Service primitives.
 */

/***
 * clearerr(3) - check and reset stream status
 *
 * @function clearerr
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.clearerr(stream)
 */
static int
luab_clearerr(lua_State *L)
{
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL) {
        clearerr(stream);
        status = 0;
    } else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * fclose(3) - close a stream
 *
 * @function fclose
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fclose(stream)
 */
static int
luab_fclose(lua_State *L)
{
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = fclose(stream);
    else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * feof(3) - check and reset stream status
 *
 * @function feof
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.feof(stream)
 */
static int
luab_feof(lua_State *L)
{
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = feof(stream);
    else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * ferror(3) - check and reset stream status
 *
 * @function ferror
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.ferror(stream)
 */
static int
luab_ferror(lua_State *L)
{
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = ferror(stream);
    else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * fflush(3) - flush a stream
 *
 * @function fflush
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fflush(stream)
 */
static int
luab_fflush(lua_State *L)
{
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = fflush(stream);
    else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pusherr(L, status));
}













#if __POSIX_VISIBLE
/***
 * fileno(3) - check and reset stream status
 *
 * @function fileno
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fileno(stream)
 */
static int
luab_fileno(lua_State *L)
{
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = fileno(stream);
    else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pusherr(L, status));
}
#endif /* __POSIX_VISIBLE */














#if __BSD_VISIBLE
/***
 * clearerr_unlocked(3) - check and reset stream status
 *
 * @function clearerr_unlocked
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.clearerr_unlocked(stream)
 */
static int
luab_clearerr_unlocked(lua_State *L)
{
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL) {
        clearerr_unlocked(stream);
        status = 0;
    } else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * feof_unlocked(3) - check and reset stream status
 *
 * @function feof_unlocked
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.feof_unlocked(stream)
 */
static int
luab_feof_unlocked(lua_State *L)
{
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = feof_unlocked(stream);
    else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * ferror_unlocked(3) - check and reset stream status
 *
 * @function ferror_unlocked
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.ferror_unlocked(stream)
 */
static int
luab_ferror_unlocked(lua_State *L)
{
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = ferror_unlocked(stream);
    else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * fileno_unlocked(3) - check and reset stream status
 *
 * @function fileno_unlocked
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fileno_unlocked(stream)
 */
static int
luab_fileno_unlocked(lua_State *L)
{
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = fileno_unlocked(stream);
    else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pusherr(L, status));
}
#endif /* __BSD_VISIBLE */





















#if __BSD_VISIBLE
/***
 * fcloseall(3) - check and reset stream status
 *
 * @function fcloseall
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fcloseall()
 */
static int
luab_fcloseall(lua_State *L)
{
    (void)luab_core_checkmaxargs(L, 0);

    fcloseall();

    return (luab_pusherr(L, 0));
}

/***
 * fdclose(3) - close a stream
 *
 * @function fdclose
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 * @param fdp               File descriptor, optional result argument, either an
 *                          instance of (LUA_TUSERDATA(PRIMITIVE)) or (LUA_TNIL).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fdclose(stream, fdp)
 */
static int
luab_fdclose(lua_State *L)
{
    FILE *stream;
    luab_primitive_u *xp;
    int *fdp, status;

    (void)luab_core_checkmaxargs(L, 2);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);
    xp = luab_udataisnil(L, 2, luab_mx(PRIMITIVE), luab_primitive_u *);

    if (xp != NULL)
        fdp = &(xp->un_int);
    else
        fdp = NULL;

    if (stream != NULL)
        status = fdclose(stream, fdp);
    else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pusherr(L, status));
}

/***
 * fpurge(3) - flush a stream
 *
 * @function fpurge
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fpurge(stream)
 */
static int
luab_fpurge(lua_State *L)
{
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = fpurge(stream);
    else {
        errno = ENOENT;
        status = -1;
    }
    return (luab_pusherr(L, status));
}
#endif /* __BSD_VISIBLE */









/*
 * Generator functions.
 */

/***
 * Generator function - create an instance of (LUA_TUSERDATA(__SBUF)).
 *
 * @function sbuf_create
 *
 * @param __sbuf          Instance of (LUA_TUSERDATA(__SBUUF)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage __sbuf [, err, msg ] = bsd.stdio.sbuf_create([ __sbuf ])
 */
static int
luab_sbuf_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(__SBUF), NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(SFILE)).
 *
 * @function sfile_create
 *
 * @param sfile          Instance of (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sfile [, err, msg ] = bsd.stdio.sfile_create([ sfile ])
 */
static int
luab_sfile_create(lua_State *L)
{
    return (luab_core_create(L, 1, luab_mx(SFILE), NULL));
}

/*
 * Interface against <stdio.h>.
 */

static luab_module_table_t luab_stdio_vec[] = { /* stdio.h */
    LUAB_INT("__SLBF",                  __SLBF),
    LUAB_INT("__SNBF",                  __SNBF),
    LUAB_INT("__SRD",                   __SRD),
    LUAB_INT("__SWR",                   __SWR),
    LUAB_INT("__SRW",                   __SRW),
    LUAB_INT("__SEOF",                  __SEOF),
    LUAB_INT("__SERR",                  __SERR),
    LUAB_INT("__SMBF",                  __SMBF),
    LUAB_INT("__SAPP",                  __SAPP),
    LUAB_INT("__SSTR",                  __SSTR),
    LUAB_INT("__SOPT",                  __SOPT),
    LUAB_INT("__SNPT",                  __SNPT),
    LUAB_INT("__SOFF",                  __SOFF),
    LUAB_INT("__SMOD",                  __SMOD),
    LUAB_INT("__SALC",                  __SALC),
    LUAB_INT("__SIGN",                  __SIGN),
    LUAB_INT("__S2OAP",                 __S2OAP),
    LUAB_INT("_IOFBF",                  _IOFBF),
    LUAB_INT("_IOLBF",                  _IOLBF),
    LUAB_INT("_IONBF",                  _IONBF),
    LUAB_INT("BUFSIZ",                  BUFSIZ),
    LUAB_INT("EOF",                     EOF),
#ifndef FOPEN_MAX
    LUAB_INT("FOPEN_MAX",               FOPEN_MAX),
#endif
#if __XSI_VISIBLE
    LUAB_STR("P_tmpdir",                P_tmpdir),
#endif
    LUAB_INT("L_tmpnam",                L_tmpnam),
    LUAB_INT("TMP_MAX",                 TMP_MAX),
#ifndef SEEK_SET
    LUAB_INT("SEEK_SET",                SEEK_SET),
#endif
#ifndef SEEK_CUR
    LUAB_INT("SEEK_CUR",                SEEK_CUR),
#endif
#ifndef SEEK_END
    LUAB_INT("SEEK_END",                SEEK_END),
#endif
#if __BSD_VISIBLE || (__POSIX_VISIBLE && __POSIX_VISIBLE <= 199506)
    LUAB_INT("L_cuserid",               L_cuserid),
#endif
#if __POSIX_VISIBLE
    LUAB_INT("L_ctermid",               L_ctermid),
#endif
    LUAB_INT("FILENAME_MAX",            FILENAME_MAX),
    LUAB_FUNC("clearerr",               luab_clearerr),
    LUAB_FUNC("fclose",                 luab_fclose),
    LUAB_FUNC("feof",                   luab_feof),
    LUAB_FUNC("ferror",                 luab_ferror),
    LUAB_FUNC("fflush",                 luab_fflush),





#if __POSIX_VISIBLE
    LUAB_FUNC("fileno",                 luab_fileno),
#endif /* __POSIX_VISIBLE */






#if __BSD_VISIBLE
    LUAB_FUNC("clearerr_unlocked",      luab_clearerr_unlocked),
    LUAB_FUNC("feof_unlocked",          luab_feof_unlocked),
    LUAB_FUNC("ferror_unlocked",        luab_ferror_unlocked),
    LUAB_FUNC("fileno_unlocked",        luab_fileno_unlocked),
#endif /* __BSD_VISIBLE */














#if __BSD_VISIBLE
    LUAB_FUNC("fcloseall",              luab_fcloseall),
    LUAB_FUNC("fdclose",                luab_fdclose),


    LUAB_FUNC("fpurge",                 luab_fpurge),

#endif /* __BSD_VISIBLE */








    LUAB_FUNC("sbuf_create",            luab_sbuf_create),
    LUAB_FUNC("sfile_create",           luab_sfile_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_stdio_lib = {
    .m_cookie   = LUAB_STDIO_LIB_ID,
    .m_name     = LUAB_STDIO_LIB_KEY,
    .m_vec      = luab_stdio_vec,
};
