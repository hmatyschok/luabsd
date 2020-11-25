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
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY FILEECT,
 * INFILEECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
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

extern luab_module_t luab_sfile_type;

/*
 * Interface against
 *
 *  struct __sFILE {
 *      unsigned char *_p;
 *      int _r;
 *      int _w;
 *      short   _flags;
 *      short   _file;
 *      struct  __sbuf _bf;
 *      int _lbfsize;
 *
 *      void    *_cookie;
 *      int (* _Nullable _close)(void *);
 *      int (* _Nullable _read)(void *, char *, int);
 *      fpos_t  (* _Nullable _seek)(void *, fpos_t, int);
 *      int (* _Nullable _write)(void *, const char *, int);
 *
 *      struct  __sbuf _ub;
 *      unsigned char   *_up;
 *      int _ur;
 *
 *      unsigned char _ubuf[3];
 *      unsigned char _nbuf[1];
 *
 *      struct  __sbuf _lb;
 *
 *      int _blksize;
 *      fpos_t  _offset;
 *
 *      struct pthread_mutex *_fl_mutex;
 *      struct pthread *_fl_owner;
 *      int _fl_count;
 *      int _orientation;
 *      __mbstate_t _mbstate;
 *      int _flags2;
 *  };
 *  #ifndef _STDSFILE_DECLARED
 *  #define _STDSFILE_DECLARED
 *  typedef struct __sFILE FILE;
 *  #endif
 *
 */

typedef struct luab_sfile {
    luab_udata_t    ud_softc;
    FILE            *ud_fp;
} luab_sfile_t;

#define luab_new_sfile(L, arg) \
    ((luab_sfile_t *)luab_newudata(L, &luab_sfile_type, (arg)))
#define luab_to_file(L, narg) \
    (luab_todata((L), (narg), &luab_sfile_type, luab_sfile_t *))

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(FILE)) into (LUA_TTABLE).
 *
 * @function get
 *
 * @return (LUA_TTABLE)
 *
 *          t = {
 *              _p          = (LUA_TSTRING),
 *              _r          = (LUA_TNUMBER),
 *              _w          = (LUA_TNUMBER),
 *              _flags      = (LUA_TNUMBER),
 *              _file       = (LUA_TNUMBER),
 *              _bf         = (LUA_TUSERDATA(__SBUF)),
 *              _cookie     = (LUA_TSTRING),
 *          }
 *
 * @usage t = file:get()
 */
static int
SFILE_get(lua_State *L)
{
    FILE *fp;

    (void)luab_core_checkmaxargs(L, 1);

    fp = luab_udata(L, 1, &luab_sfile_type, FILE *);

    if (fp != NULL) {   /* XXX exception handling */
        lua_newtable(L);
        luab_setfstring(L, -2, "_p", "(%s)",            fp->_p);
        luab_setinteger(L, -2, "_r",                    fp->_r);
        luab_setinteger(L, -2, "_w",                    fp->_w);
        luab_setinteger(L, -2, "_flags",                fp->_flags);
        luab_setinteger(L, -2, "_file",                 fp->_file);
        luab_setudata(L, -2, luab_xmod(__SBUF, TYPE, __func__), "_bf",    &(fp->_bf));
        luab_setfstring(L, -2, "_cookie", "(%p)",       fp->_cookie);
        lua_pushvalue(L, -1);
    }
    return (1);
}

/***
 * Generator function - translate file{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = file:dump()
 */
static int
SFILE_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get base address of current buffer position.
 *
 * @function _p
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = file:_p()
 */
static int
SFILE_p(lua_State *L)
{
    FILE *fp;
    void *v;

    (void)luab_core_checkmaxargs(L, 1);

    fp = luab_udata(L, 1, &luab_sfile_type, FILE *);

    if (fp != NULL)
        v = fp->_p;
    else {
        errno = ENOENT;
        v = NULL;
    }
    return (luab_pushfstring(L, "(%p)", v));
}

/***
 * Get read space left for getc(3).
 *
 * @function _r
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = file:_r()
 */
static int
SFILE_r(lua_State *L)
{
    FILE *fp;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    fp = luab_udata(L, 1, &luab_sfile_type, FILE *);

    if (fp != NULL)
        x = fp->_r;
    else {
        errno = ENOENT;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
}

/***
 * Get write space left for putc(3).
 *
 * @function _w
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = file:_w()
 */
static int
SFILE_w(lua_State *L)
{
    FILE *fp;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    fp = luab_udata(L, 1, &luab_sfile_type, FILE *);

    if (fp != NULL)
        x = fp->_w;
    else {
        errno = ENOENT;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
}

/***
 * Get flags.
 *
 * @function _flags
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = file:_flags()
 */
static int
SFILE_flags(lua_State *L)
{
    FILE *fp;
    short x;

    (void)luab_core_checkmaxargs(L, 1);

    fp = luab_udata(L, 1, &luab_sfile_type, FILE *);

    if (fp != NULL)
        x = fp->_flags;
    else {
        errno = ENOENT;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
}

/***
 * Get file descriptor.
 *
 * @function _file
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = file:_file()
 */
static int
SFILE_file(lua_State *L)
{
    FILE *fp;
    short x;

    (void)luab_core_checkmaxargs(L, 1);

    fp = luab_udata(L, 1, &luab_sfile_type, FILE *);

    if (fp != NULL)
        x = fp->_file;
    else {
        errno = ENOENT;
        x = -1;
    }
    return (luab_pushxinteger(L, x));
}

/***
 * Get buffer.
 *
 * @function _bf
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = file:_bf()
 */
static int
SFILE_bf(lua_State *L)
{
    FILE *fp;
    void *v;
    luab_module_t *m;

    (void)luab_core_checkmaxargs(L, 1);

    fp = luab_udata(L, 1, &luab_sfile_type, FILE *);

    if (fp != NULL) {
        v = &(fp->_bf);
        m = luab_xmod(__SBUF, TYPE, __func__);
    } else {
        v = NULL;
        m = NULL;
    }
    return (luab_pushudata(L, luab_xmod(__SBUF, TYPE, __func__), v));
}

/***
 * Get base address of cookie.
 *
 * @function _cookie
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = file:_cookie()
 */
static int
SFILE_cookie(lua_State *L)
{
    FILE *fp;
    void *v;

    (void)luab_core_checkmaxargs(L, 1);

    fp = luab_udata(L, 1, &luab_sfile_type, FILE *);

    if (fp != NULL)
        v = fp->_cookie;
    else {
        errno = ENOENT;
        v = NULL;
    }
    return (luab_pushfstring(L, "(%p)", v));
}

/*
 * Meta-methods.
 */

static int
SFILE_gc(lua_State *L __unused)
{
    luab_sfile_t *self = luab_to_file(L, 1);

    if (self->ud_fp != NULL) {
        (void)fclose(self->ud_fp);
        self->ud_fp = NULL;
    }
    return (0);
}

static int
SFILE_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_sfile_type));
}

static int
SFILE_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_sfile_type));
}

/*
 * Internal interface.
 */

static luab_module_table_t sfile_methods[] = {
    LUAB_FUNC("_p",             SFILE_p),
    LUAB_FUNC("_r",             SFILE_r),
    LUAB_FUNC("_w",             SFILE_w),
    LUAB_FUNC("_flags",         SFILE_flags),
    LUAB_FUNC("_file",          SFILE_file),
    LUAB_FUNC("_bf",            SFILE_bf),
    LUAB_FUNC("_cookie",        SFILE_cookie),
    LUAB_FUNC("get",            SFILE_get),
    LUAB_FUNC("dump",           SFILE_dump),
    LUAB_FUNC("__gc",           SFILE_gc),
    LUAB_FUNC("__len",          SFILE_len),
    LUAB_FUNC("__tostring",     SFILE_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
sfile_create(lua_State *L, void *arg)
{
    return (luab_new_sfile(L, arg));
}

static void
sfile_init(void *ud, void *arg)
{
    luab_udata_init(&luab_sfile_type, ud, arg);
}

static void *
sfile_udata(lua_State *L, int narg)
{
    luab_sfile_t *self = luab_to_file(L, narg);
    return (self->ud_fp);
}

luab_module_t luab_sfile_type = {
    .m_cookie   = LUAB_SFILE_TYPE_ID,
    .m_name     = LUAB_SFILE_TYPE,
    .m_vec      = sfile_methods,
    .m_create   = sfile_create,
    .m_init     = sfile_init,
    .m_get      = sfile_udata,
    .m_sz       = sizeof(luab_sfile_t),
};
