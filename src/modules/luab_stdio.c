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
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = fgetc(stream);
    else {
        errno = ENOENT;
        status = -1;
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
 *                          an instance of (LUA_TUSERDATA(PRIMITIVE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fgetpos(stream, pos)
 */
static int
luab_fgetpos(lua_State *L)
{
    FILE *stream;
    luab_primitive_u *xp;
    fpos_t *pos;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);
    xp = luab_udata(L, 2, luab_mx(PRIMITIVE), luab_primitive_u *);
    pos = &(xp->un_fpos);

    if (stream != NULL)
        status = fgetpos(stream, pos);
    else {
        errno = ENOENT;
        status = -1;
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
    luab_iovec_t *buf;
    size_t size;
    FILE *stream;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    size = (size_t)luab_checkinteger(L, 2, INT_MAX);
    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_buf_nmax) &&
        (size <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if (stream != NULL) {

                if (fgets(bp, size, stream) != NULL) {
                    buf->iov.iov_len = size;
                    status = 0;
                } else
                    status = -1;

            } else {
                errno = ENOENT;
                status = -1;
            }
            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = 0;
        }
    } else {
        errno = ERANGE;
        status = 0;
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
    const char *path;
    const char *mode;
    FILE *stream;
    luab_module_t *m;

    (void)luab_core_checkmaxargs(L, 2);

    path = luab_checklstring(L, 1, MAXPATHLEN);
    mode = luab_checklstring(L, 2, LUAB_STDIO_MODE_MAXLEN);

    if ((stream = fopen(path, mode)) != NULL)
        m = luab_mx(SFILE);
    else
        m = NULL;

    return (luab_pushudata(L, m, stream));
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
    int c;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    c = luab_checkinteger(L, 1, UCHAR_MAX);
    stream = luab_udata(L, 2, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = fputc(c, stream);
    else {
        errno = ENOENT;
        status = -1;
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
    const char *str;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    str = luab_checklstring(L, 1, luab_buf_nmax);
    stream = luab_udata(L, 2, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = fputs(str, stream);
    else {
        errno = ENOENT;
        status = -1;
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
    const char *path;
    const char *mode;
    FILE *stream, *ret;
    luab_module_t *m;

    (void)luab_core_checkmaxargs(L, 2);

    path = luab_checklstringisnil(L, 1, MAXPATHLEN);
    mode = luab_checklstring(L, 2, LUAB_STDIO_MODE_MAXLEN);
    stream = luab_udata(L, 3, luab_mx(SFILE), FILE *);

    if (stream != NULL) {

        if ((ret = freopen(path, mode, stream)) != NULL)
            m = luab_mx(SFILE);
        else
            m = NULL;

    } else {
        errno = ENOENT;
        ret = NULL;
        m = NULL;
    }
    return (luab_pushudata(L, m, ret));
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
    FILE *stream;
    long offset;
    int whence;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);
    offset = (long)luab_checkinteger(L, 2, LONG_MAX);
    whence = (int)luab_checkinteger(L, 3, INT_MAX);

    if (stream != NULL)
        status = fseek(stream, offset, whence);
    else {
        errno = ENOENT;
        status = -1;
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
 *                          by an instance of (LUA_TUSERDATA(PRIMITIVE)).
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage ret [, err, msg ] = bsd.stdio.fsetpos(stream, pos)
 */
static int
luab_fsetpos(lua_State *L)
{
    FILE *stream;
    luab_primitive_u *xp;
    fpos_t *pos;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);
    xp = luab_udata(L, 2, luab_mx(PRIMITIVE), luab_primitive_u *);
    pos = &(xp->un_fpos);

    if (stream != NULL)
        status = fsetpos(stream, pos);
    else {
        errno = ENOENT;
        status = -1;
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
    FILE *stream;
    long status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = ftell(stream);
    else {
        errno = ENOENT;
        status = -1;
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
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = getc(stream);
    else {
        errno = ENOENT;
        status = -1;
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
    luab_iovec_t *buf;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_buf_nmax) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if (gets(bp) != NULL) {
                buf->iov.iov_len = strnlen(bp, luab_buf_nmax);
                status = 0;
            } else
                status = -1;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ERANGE;
        status = -1;
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
    luab_iovec_t *buf;
    rsize_t size;
    caddr_t bp;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    size = (rsize_t)luab_checklinteger(L, 2);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_buf_nmax) &&
        (size <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if (gets_s(bp, size) != NULL) {
                buf->iov.iov_len = size;
                status = 0;
            } else
                status = -1;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            status = -1;
        }
    } else {
        errno = ERANGE;
        status = -1;
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
    int c;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    c = luab_checkinteger(L, 1, UCHAR_MAX);
    stream = luab_udata(L, 2, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = putc(c, stream);
    else {
        errno = ENOENT;
        status = -1;
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
    int c, status;

    (void)luab_core_checkmaxargs(L, 2);

    c = luab_checkinteger(L, 1, UCHAR_MAX);
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

    str = luab_checklstring(L, 1, luab_buf_nmax);
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

    path = luab_checklstring(L, 1, MAXPATHLEN);
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
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL) {
        rewind(stream);
        status = 0;
    } else {
        errno = ENOENT;
        status = -1;
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
    int fildes;
    const char *mode;
    FILE *stream;
    luab_module_t *m;

    (void)luab_core_checkmaxargs(L, 2);

    fildes = (int)luab_checkinteger(L, 1, INT_MAX);
    mode = luab_checklstring(L, 2, LUAB_STDIO_MODE_MAXLEN);

    if ((stream = fdopen(fildes, mode)) != NULL)
        m = luab_mx(SFILE);
    else
        m = NULL;

    return (luab_pushudata(L, m, stream));
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
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = getc_unlocked(stream);
    else {
        errno = ENOENT;
        status = -1;
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
    int c;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    c = luab_checkinteger(L, 1, UCHAR_MAX);
    stream = luab_udata(L, 2, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = putc_unlocked(c, stream);
    else {
        errno = ENOENT;
        status = -1;
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

    c = luab_checkinteger(L, 1, UCHAR_MAX);
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
    FILE *stream;
    long offset;
    int whence;
    int status;

    (void)luab_core_checkmaxargs(L, 3);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);
    offset = (long)luab_checkinteger(L, 2, LONG_MAX);
    whence = (int)luab_checkinteger(L, 3, INT_MAX);

    if (stream != NULL)
        status = fseeko(stream, offset, whence);
    else {
        errno = ENOENT;
        status = -1;
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
    FILE *stream;
    __off_t status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = ftello(stream);
    else {
        errno = ENOENT;
        status = -1;
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
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 1);

    stream = luab_udata(L, 1, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = getw(stream);
    else {
        errno = ENOENT;
        status = -1;
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
    int w;
    FILE *stream;
    int status;

    (void)luab_core_checkmaxargs(L, 2);

    w = luab_checkinteger(L, 1, UINT_MAX);
    stream = luab_udata(L, 2, luab_mx(SFILE), FILE *);

    if (stream != NULL)
        status = putw(w, stream);
    else {
        errno = ENOENT;
        status = -1;
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
    luab_iovec_t *buf;
    size_t size;
    const char *mode;
    caddr_t bp;
    FILE *stream;
    luab_module_t *m;

    (void)luab_core_checkmaxargs(L, 2);

    buf = luab_udata(L, 1, luab_mx(IOVEC), luab_iovec_t *);
    size = (size_t)luab_checklinteger(L, 2);
    mode = luab_checklstring(L, 3, LUAB_STDIO_MODE_MAXLEN);

    if (((bp = buf->iov.iov_base) != NULL) &&
        (buf->iov_max_len <= luab_buf_nmax) &&
        (size <= buf->iov_max_len) &&
        (buf->iov_flags & IOV_BUFF)) {

        if ((buf->iov_flags & IOV_LOCK) == 0) {
            buf->iov_flags |= IOV_LOCK;

            if ((stream = fmemopen(bp, size, mode)) != NULL) {
                buf->iov.iov_len = size;
                m = luab_mx(SFILE);
            } else
                m = NULL;

            buf->iov_flags &= ~IOV_LOCK;
        } else {
            errno = EBUSY;
            stream = NULL;
            m = NULL;
        }
    } else {
        errno = ERANGE;
        stream = NULL;
        m = NULL;
    }
    return (luab_pushudata(L, m, stream));
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

    return (luab_pushxinteger(L, 0));
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
    return (luab_pushxinteger(L, status));
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
    LUAB_FUNC("sbuf_create",            luab_sbuf_create),
    LUAB_FUNC("sfile_create",           luab_sfile_create),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_stdio_lib = {
    .m_cookie   = LUAB_STDIO_LIB_ID,
    .m_name     = LUAB_STDIO_LIB_KEY,
    .m_vec      = luab_stdio_vec,
};
