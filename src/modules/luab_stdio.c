/*
 * Copyright (c) 2020, 2021 Henning Matyschok
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
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_STDIO_LIB_ID    1605194991
#define LUAB_STDIO_LIB_KEY    "stdio"

#define LUAB_STDIO_MODE_MAXLEN  3

/*
 * XXX
 *
 * #1: Implementation of initializer for
 *
 *      extern FILE *__stdinp;
 *      extern FILE *__stdoutp;
 *      extern FILE *__stderrp;
 *
 *      #define stdin   __stdinp
 *      #define stdout  __stdoutp
 *      #define stderr  __stderrp
 *
 *    is pending.
 *
 * #2: Subset of primitives shall implemented.
 */

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
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL) {
        clearerr(stream);
        status = luab_env_success;
    } else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
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
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = fclose(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
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
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = feof(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
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
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = ferror(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
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
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = fflush(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * fgetc(3) - get next character or word from input stream
 *
 * @function fgetc
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fgetc(stream)
 */
static int
luab_fgetc(lua_State *L)
{
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = fgetc(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * fgetpos(3) - reposition a stream
 *
 * @function fgetpos
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 * @param pos               Result argument, tracks current position by
 *                          an instance of (LUA_TUSERDATA(OFF)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fgetpos(stream, pos)
 */
static int
luab_fgetpos(lua_State *L)
{
    luab_module_t *m0, *m1;
    FILE *stream;
    fpos_t *pos;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SFILE, TYPE, __func__);
    m1 = luab_xmod(FPOS, TYPE, __func__);

    stream = luab_udata(L, 1, m0, FILE *);
    pos = luab_udata(L, 2, m1, fpos_t *);

    if (stream != NULL)
        status = fgetpos(stream, pos);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * fgets(3) - get a line from a stream
 *
 * @function fgets
 *
 * @param str               Buffer, instance of (LUA_TUSERDATA(IOVEC)).
 * @param size              Specifies the amount of data about to read.
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fgets(str, size, stream)
 */
static int
luab_fgets(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    luab_iovec_t *buf;
    size_t size;
    FILE *stream;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);
    m2 = luab_xmod(SFILE, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    size = (size_t)luab_checklxinteger(L, 2, m1, 0);
    stream = luab_udata(L, 1, m2, FILE *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (size <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if (stream != NULL) {

            if (fgets(bp, size, stream) != NULL) {
                buf->iov.iov_len = size;
                status = luab_env_success;
            } else
                status = luab_env_error;

        } else {
            errno = ENOENT;
            status = luab_env_error;
        }
        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_success;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * fopen(3) - stream open functions
 *
 * @function fopen
 *
 * @param path              Specifies file name pointed by its path.
 * @param mode              Specifies mode subset of L(X) over X = {'r','w','x'}.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fopen(path, mode)
 */
static int
luab_fopen(lua_State *L)
{
    luab_module_t *m;
    const char *path;
    const char *mode;
    FILE *stream;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(SFILE, TYPE, __func__);

    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    mode = luab_checklstring(L, 2, LUAB_STDIO_MODE_MAXLEN, NULL);   /* XXX */

    if ((stream = fopen(path, mode)) == NULL)
        m = NULL;

    return (luab_pushxdata(L, m, stream));
}

/***
 * fputc(3) - output a character or word to a stream
 *
 * @function fputc
 *
 * @param c                 Specifies character about to write on stream.
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fputc(c, stream)
 */
static int
luab_fputc(lua_State *L)
{
    luab_module_t *m0, *m1;
    int c;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(UINT8, TYPE, __func__);
    m1 = luab_xmod(SFILE, TYPE, __func__);

    c = (int)luab_checkxinteger(L, 1, m0, luab_env_uchar_max);
    stream = luab_udata(L, 2, m1, FILE *);

    if (stream != NULL)
        status = fputc(c, stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * fputs(3) - output a line to a stream
 *
 * @function fputs
 *
 * @param str               Specifies string about to write on stream.
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fputs(str, stream)
 */
static int
luab_fputs(lua_State *L)
{
    luab_module_t *m;
    const char *str;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(SFILE, TYPE, __func__);

    str = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    stream = luab_udata(L, 2, m, FILE *);

    if (stream != NULL)
        status = fputs(str, stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * freopen(3) - stream open functions
 *
 * @function freopen
 *
 * @param path              Specifies file name pointed by its path.
 * @param mode              Specifies mode subset of L(X) over X = {'r','w','x'}.
 * @param stream
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.freopen(path, mode, stream)
 */
static int
luab_freopen(lua_State *L)
{
    luab_module_t *m;
    const char *path;
    const char *mode;
    FILE *stream, *ret;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(SFILE, TYPE, __func__);

    path = luab_checklstringisnil(L, 1, luab_env_path_max, NULL);
    mode = luab_checklstring(L, 2, LUAB_STDIO_MODE_MAXLEN, NULL);
    stream = luab_udata(L, 3, m, FILE *);

    if (stream != NULL) {

        if ((ret = freopen(path, mode, stream)) == NULL)
            m = NULL;

    } else {
        errno = ENOENT;
        ret = NULL;
        m = NULL;
    }
    return (luab_pushxdata(L, m, ret));
}

/***
 * fseek(3) - reposition a stream
 *
 * @function fseek
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 * @param offset            Specifies the new position in bytes.
 * @param whence            Specifies by values from
 *
 *                              bsd.unistd.SEEK_{
 *                                  SET,
 *                                  CUR,
 *                                  END
 *                              }
 *
 *                          the position indicator.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fseek(stream, offset, whence)
 */
static int
luab_fseek(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    FILE *stream;
    long offset;
    int whence;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(SFILE, TYPE, __func__);
    m1 = luab_xmod(LONG, TYPE, __func__);
    m2 = luab_xmod(INT, TYPE, __func__);

    stream = luab_udata(L, 1, m0, FILE *);
    offset = (long)luab_checkxinteger(L, 2, m1, luab_env_long_max);
    whence = (int)luab_checkxinteger(L, 3, m2, luab_env_int_max);

    if (stream != NULL)
        status = fseek(stream, offset, whence);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * fsetpos(3) - reposition a stream
 *
 * @function fsetpos
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 * @param pos               Value argument, specifies the current position
 *                          by an instance of (LUA_TUSERDATA(OFF)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fsetpos(stream, pos)
 */
static int
luab_fsetpos(lua_State *L)
{
    luab_module_t *m0, *m1;
    FILE *stream;
    fpos_t *pos;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(SFILE, TYPE, __func__);
    m1 = luab_xmod(FPOS, TYPE, __func__);

    stream = luab_udata(L, 1, m0, FILE *);
    pos = luab_udata(L, 2, m1, fpos_t *);

    if (stream != NULL)
        status = fsetpos(stream, pos);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * ftell(3) - reposition a stream
 *
 * @function ftell
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.ftell(stream)
 */
static int
luab_ftell(lua_State *L)
{
    luab_module_t *m;
    FILE *stream;
    long status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = ftell(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * getc(3) - get next character or word from input stream
 *
 * @function getc
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.getc(stream)
 */
static int
luab_getc(lua_State *L)
{
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = getc(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * getchar(3) - get next character or word from input stream
 *
 * @function getchar
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.getchar()
 */
static int
luab_getchar(lua_State *L)
{
    int status;

    (void)luab_core_checkmaxargs(L, 0);

    status = getchar();
    return (luab_pushxinteger(L, status));
}

/***
 * gets(3) - get a line from a stream
 *
 * @function gets
 *
 * @param str               Buffer, instance of (LUA_TUSERDATA(IOVEC)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.gets(str)
 */
static int
luab_gets(lua_State *L)
{
    luab_module_t *m;
    luab_iovec_t *buf;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IOVEC, TYPE, __func__);
    buf = luab_udata(L, 1, m, luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if (gets(bp) != NULL) {
            buf->iov.iov_len = strnlen(bp, luab_env_buf_max);
            status = luab_env_success;
        } else
            status = luab_env_error;

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}
#if __EXT1_VISIBLE
/***
 * gets_s(3) - get a line from a stream
 *
 * @function gets_s
 *
 * @param str               Buffer, instance of (LUA_TUSERDATA(IOVEC)).
 * @param size              Specifies the amount of data about to read.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.gets_s(str, size)
 */
static int
luab_gets_s(lua_State *L)
{
    luab_module_t *m0, *m1;
    luab_iovec_t *buf;
    rsize_t size;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(RSIZE, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    size = (rsize_t)luab_checklxinteger(L, 2, m1, 0);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (size <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if (gets_s(bp, size) != NULL) {
            buf->iov.iov_len = size;
            status = luab_env_success;
        } else
            status = luab_env_error;

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}
#endif /* __EXT1_VISIBLE */

/***
 * putc(3) - output a character or word to a stream
 *
 * @function putc
 *
 * @param c                 Specifies character about to write on stream.
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.putc(c, stream)
 */
static int
luab_putc(lua_State *L)
{
    luab_module_t *m0, *m1;
    int c;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(UINT8, TYPE, __func__);
    m1 = luab_xmod(SFILE, TYPE, __func__);

    c = luab_checkxinteger(L, 1, m0, luab_env_uchar_max);
    stream = luab_udata(L, 2, m1, FILE *);

    if (stream != NULL)
        status = putc(c, stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * putchar(3) - output a character or word to a stream
 *
 * @function putchar
 *
 * @param c                 Specifies character about to write on stream.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.putchar(c)
 */
static int
luab_putchar(lua_State *L)
{
    luab_module_t *m;
    int c, status;

    (void)luab_core_checkmaxargs(L, 2);

    m = luab_xmod(UINT8, TYPE, __func__);
    c = luab_checkxinteger(L, 1, m, luab_env_uchar_max);
    status = putchar(c);

    return (luab_pushxinteger(L, status));
}

/***
 * puts(3) - output a line to a stream
 *
 * @function puts
 *
 * @param str               Specifies string about to write on stream.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.puts(str)
 */
static int
luab_puts(lua_State *L)
{
    const char *str;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    str = luab_checklstring(L, 1, luab_env_buf_max, NULL);
    status = puts(str);

    return (luab_pushxinteger(L, status));
}

/***
 * remove(3) - remove directory entry
 *
 * @function remove
 *
 * @param path              Specifies directory or file by path.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.remove(path)
 */
static int
luab_remove(lua_State *L)
{
    const char *path;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, luab_env_path_max, NULL);
    status = remove(path);

    return (luab_pushxinteger(L, status));
}

/***
 * rewind(3) - reposition a stream
 *
 * @function rewind
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.rewind(stream)
 */
static int
luab_rewind(lua_State *L)
{
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL) {
        rewind(stream);
        status = luab_env_success;
    } else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

#if __POSIX_VISIBLE
/***
 * fdopen(3) - stream open functions
 *
 * @function fdopen
 *
 * @param fildes            Specifies open file descriptor.
 * @param mode              Specifies mode subset of L(X) over X = {'r','w','x'}.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fdopen(path, mode)
 */
static int
luab_fdopen(lua_State *L)
{
    luab_module_t *m0, *m1;
    int fildes;
    const char *mode;
    FILE *stream;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(SFILE, TYPE, __func__);

    fildes = (int)luab_checkxinteger(L, 1, m0, luab_env_int_max);
    mode = luab_checklstring(L, 2, LUAB_STDIO_MODE_MAXLEN, NULL);

    if ((stream = fdopen(fildes, mode)) == NULL)
        m1 = NULL;

    return (luab_pushxdata(L, m1, stream));
}

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
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = fileno(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}
#endif /* __POSIX_VISIBLE */

#if __POSIX_VISIBLE >= 199506
/***
 * getc_unlocked(3) - get next character or word from input stream
 *
 * @function getc_unlocked
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.getc_unlocked(stream)
 */
static int
luab_getc_unlocked(lua_State *L)
{
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = getc_unlocked(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * getchar_unlocked(3) - get next character or word from input stream
 *
 * @function getchar_unlocked
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.getchar_unlocked()
 */
static int
luab_getchar_unlocked(lua_State *L)
{
    int status;

    (void)luab_core_checkmaxargs(L, 0);

    status = getchar_unlocked();
    return (luab_pushxinteger(L, status));
}

/***
 * putc_unlocked(3) - output a character or word to a stream
 *
 * @function putc_unlocked
 *
 * @param c                 Specifies character about to write on stream.
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.putc_unlocked(c, stream)
 */
static int
luab_putc_unlocked(lua_State *L)
{
    luab_module_t *m0, *m1;
    int c;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(UINT8, TYPE, __func__);
    m1 = luab_xmod(SFILE, TYPE, __func__);

    c = luab_checkxinteger(L, 1, m0, luab_env_uchar_max);
    stream = luab_udata(L, 2, m1, FILE *);

    if (stream != NULL)
        status = putc_unlocked(c, stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * putchar_unlocked(3) - output a character or word to a stream
 *
 * @function putchar_unlocked
 *
 * @param c                 Specifies character about to write on stream.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.putchar_unlocked(c)
 */
static int
luab_putchar_unlocked(lua_State *L)
{
    int c, status;

    (void)luab_core_checkmaxargs(L, 2);

    c = luab_checkinteger(L, 1, luab_env_uchar_max);
    status = putchar_unlocked(c);

    return (luab_pushxinteger(L, status));
}
#endif /* __POSIX_VISIBLE >= 199506 */

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
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL) {
        clearerr_unlocked(stream);
        status = luab_env_success;
    } else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
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
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = feof_unlocked(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
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
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = ferror_unlocked(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
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
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = fileno_unlocked(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}
#endif /* __BSD_VISIBLE */

#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE >= 500
/***
 * fseeko(3) - reposition a stream
 *
 * @function fseeko
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 * @param offset            Specifies the new position in bytes.
 * @param whence            Specifies by values from
 *
 *                              bsd.unistd.SEEK_{
 *                                  SET,
 *                                  CUR,
 *                                  END
 *                              }
 *
 *                          the position indicator.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fseeko(stream, offset, whence)
 */
static int
luab_fseeko(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    FILE *stream;
    long offset;
    int whence;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    m0 = luab_xmod(SFILE, TYPE, __func__);
    m1 = luab_xmod(LONG, TYPE, __func__);
    m2 = luab_xmod(INT, TYPE, __func__);

    stream = luab_udata(L, 1, m0, FILE *);
    offset = (long)luab_checkxinteger(L, 2, m1, luab_env_long_max);
    whence = (int)luab_checkxinteger(L, 3, m2, luab_env_int_max);

    if (stream != NULL)
        status = fseeko(stream, offset, whence);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * ftello(3) - reposition a stream
 *
 * @function ftello
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.ftello(stream)
 */
static int
luab_ftello(lua_State *L)
{
    luab_module_t *m;
    FILE *stream;
    __off_t status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = ftello(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}
#endif

#if __BSD_VISIBLE || __XSI_VISIBLE > 0 && __XSI_VISIBLE < 600
/***
 * getw(3) - get next character or word from input stream
 *
 * @function getw
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.getw(stream)
 */
static int
luab_getw(lua_State *L)
{
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = getw(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}

/***
 * putw(3) - output a character or word to a stream
 *
 * @function putw
 *
 * @param w                 Specifies word about to write on stream.
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.putw(w, stream)
 */
static int
luab_putw(lua_State *L)
{
    luab_module_t *m0, *m1;
    int w;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(INT, TYPE, __func__);
    m1 = luab_xmod(SFILE, TYPE, __func__);

    w = luab_checkxinteger(L, 1, m0, luab_env_uint_max);
    stream = luab_udata(L, 2, m1, FILE *);

    if (stream != NULL)
        status = putw(w, stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}
#endif /* BSD or X/Open before issue 6 */

#if __POSIX_VISIBLE >= 200809
/***
 * fmemopen(3) - stream open functions
 *
 * @function fmemopen
 *
 * @param str               Buffer, instance of (LUA_TUSERDATA(IOVEC)).
 * @param size              Constraint, size of mapped data region.
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fmemopen(str, size)
 */
static int
luab_fmemopen(lua_State *L)
{
    luab_module_t *m0, *m1, *m2;
    luab_iovec_t *buf;
    size_t size;
    const char *mode;
    caddr_t bp;
    FILE *stream;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IOVEC, TYPE, __func__);
    m1 = luab_xmod(SIZE, TYPE, __func__);
    m2 = luab_xmod(SFILE, TYPE, __func__);

    buf = luab_udata(L, 1, m0, luab_iovec_t *);
    size = (size_t)luab_checklxinteger(L, 2, m1, 0);
    mode = luab_checklstring(L, 3, LUAB_STDIO_MODE_MAXLEN, NULL);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_env_buf_max) &&
        (size <= buf->iov_max_len) &&
        ((buf->iov_flags & IOV_BUFF) != 0)) {

        luab_thread_mtx_lock(L, __func__);

        if ((stream = fmemopen(bp, size, mode)) != NULL)
            buf->iov.iov_len = size;
        else
            m2 = NULL;

        luab_thread_mtx_unlock(L, __func__);
    } else {
        errno = ERANGE;
        stream = NULL;
        m2 = NULL;
    }
    return (luab_pushxdata(L, m2, stream));
}
#endif /* __POSIX_VISIBLE >= 200809 */

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

    return (luab_pushxinteger(L, luab_env_success));
}

/***
 * fdclose(3) - close a stream
 *
 * @function fdclose
 *
 * @param stream            Open file stream, (LUA_TUSERDATA(SFILE)).
 * @param fdp               File descriptor, optional result argument, either an
 *                          instance of (LUA_TUSERDATA(INT)) or (LUA_TNIL).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fdclose(stream, fdp)
 */
static int
luab_fdclose(lua_State *L)
{
    luab_module_t *m0, *m1;
    FILE *stream;
    int *fdp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SFILE, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    stream = luab_udata(L, 1, m0, FILE *);
    fdp = luab_udataisnil(L, 2, m1, int *);

    if (stream != NULL)
        status = fdclose(stream, fdp);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
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
    luab_module_t *m;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SFILE, TYPE, __func__);
    stream = luab_udata(L, 1, m, FILE *);

    if (stream != NULL)
        status = fpurge(stream);
    else {
        errno = ENOENT;
        status = luab_env_error;
    }
    return (luab_pushxinteger(L, status));
}
#endif /* __BSD_VISIBLE */

/*
 * Generator functions.
 */

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(FPOS)).
 *
 * @function create_fpos
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(FPOS)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage fpos [, err, msg ] = bsd.stdio.create_fpos(arg)
 */
static int
luab_type_create_fpos(lua_State *L)
{
    luab_module_t *m;
    fpos_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FPOS, TYPE, __func__);
    x = (fpos_t)luab_checkxinteger(L, 1, m, luab_env_ulong_max);

    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(RSIZE)).
 *
 * @function create_rsize
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(RSIZE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage rsize [, err, msg ] = bsd.stdio.create_rsize(arg)
 */
static int
luab_type_create_rsize(lua_State *L)
{
    luab_module_t *m;
    rsize_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(RSIZE, TYPE, __func__);
    x = (rsize_t)luab_checklxinteger(L, 1, m, 0);
    return (luab_pushxdata(L, m, &x));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(__SBUF)).
 *
 * @function create_sbuf
 *
 * @param __sbuf          Instance of (LUA_TUSERDATA(__SBUUF)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage __sbuf [, err, msg ] = bsd.stdio.create_sbuf([ __sbuf ])
 */
static int
luab_type_create_sbuf(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(__SBUF, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
}

/***
 * Generator function - create an instance of (LUA_TUSERDATA(SFILE)).
 *
 * @function create_sfile
 *
 * @param sfile          Instance of (LUA_TUSERDATA(SFILE)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sfile [, err, msg ] = bsd.stdio.create_sfile([ sfile ])
 */
static int
luab_type_create_sfile(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SFILE, TYPE, __func__);
    return (luab_core_create(L, 1, m, NULL));
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
    LUAB_FUNC("fgetc",                  luab_fgetc),
    LUAB_FUNC("fgetpos",                luab_fgetpos),
    LUAB_FUNC("fgets",                  luab_fgets),
    LUAB_FUNC("fopen",                  luab_fopen),
    LUAB_FUNC("fputc",                  luab_fputc),
    LUAB_FUNC("fputs",                  luab_fputs),
    LUAB_FUNC("freopen",                luab_freopen),
    LUAB_FUNC("fseek",                  luab_fseek),
    LUAB_FUNC("fsetpos",                luab_fsetpos),
    LUAB_FUNC("ftell",                  luab_ftell),
    LUAB_FUNC("getc",                   luab_getc),
    LUAB_FUNC("getchar",                luab_getchar),
    LUAB_FUNC("gets",                   luab_gets),
#if __EXT1_VISIBLE
    LUAB_FUNC("gets_s",                 luab_gets_s),
#endif
    LUAB_FUNC("putc",                   luab_putc),
    LUAB_FUNC("putchar",                luab_putchar),
    LUAB_FUNC("puts",                   luab_puts),
    LUAB_FUNC("remove",                 luab_remove),
    LUAB_FUNC("rewind",                 luab_rewind),
#if __POSIX_VISIBLE
    LUAB_FUNC("fdopen",                 luab_fdopen),
    LUAB_FUNC("fileno",                 luab_fileno),
#endif /* __POSIX_VISIBLE */
#if __POSIX_VISIBLE >= 199506
    LUAB_FUNC("getc_unlocked",          luab_getc_unlocked),
    LUAB_FUNC("getchar_unlocked",       luab_getchar_unlocked),
    LUAB_FUNC("putc_unlocked",          luab_putc_unlocked),
    LUAB_FUNC("putchar_unlocked",       luab_putchar_unlocked),
#endif /* __POSIX_VISIBLE >= 199506 */
#if __BSD_VISIBLE
    LUAB_FUNC("clearerr_unlocked",      luab_clearerr_unlocked),
    LUAB_FUNC("feof_unlocked",          luab_feof_unlocked),
    LUAB_FUNC("ferror_unlocked",        luab_ferror_unlocked),
    LUAB_FUNC("fileno_unlocked",        luab_fileno_unlocked),
#endif /* __BSD_VISIBLE */
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE >= 500
    LUAB_FUNC("fseeko",                 luab_fseeko),
    LUAB_FUNC("ftello",                 luab_ftello),
#endif
#if __BSD_VISIBLE || __XSI_VISIBLE > 0 && __XSI_VISIBLE < 600
    LUAB_FUNC("getw",                   luab_getw),
    LUAB_FUNC("putw",                   luab_putw),
#endif /* BSD or X/Open before issue 6 */
#if __POSIX_VISIBLE >= 200809
    LUAB_FUNC("fmemopen",               luab_fmemopen),
#endif /* __POSIX_VISIBLE >= 200809 */
#if __BSD_VISIBLE
    LUAB_FUNC("fcloseall",              luab_fcloseall),
    LUAB_FUNC("fdclose",                luab_fdclose),
    LUAB_FUNC("fpurge",                 luab_fpurge),
#endif /* __BSD_VISIBLE */
    LUAB_FUNC("create_fpos",            luab_type_create_fpos),
    LUAB_FUNC("create_rsize",           luab_type_create_rsize),
    LUAB_FUNC("create_sbuf",            luab_type_create_sbuf),
    LUAB_FUNC("create_sfile",           luab_type_create_sfile),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_stdio_lib = {
    .m_id       = LUAB_STDIO_LIB_ID,
    .m_name     = LUAB_STDIO_LIB_KEY,
    .m_vec      = luab_stdio_vec,
};
