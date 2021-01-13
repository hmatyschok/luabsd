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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

#if __POSIX_VISIBLE >= 199309
extern luab_module_t luab_sigevent_type;

/*
 * Interface against
 *
 *  struct sigevent {
 *      int	sigev_notify;
 *      int	sigev_signo;
 *      union sigval sigev_value;
 *      union {
 *          __lwpid_t	_threadid;
 *          struct {
 *              void (*_function)(union sigval);
 *		        struct pthread_attr **_attribute;
 *          } _sigev_thread;
 *          unsigned short _kevent_flags;
 *          long __spare__[8];
 *      } _sigev_un;
 *  };
 */

typedef struct luab_sigevent {
    luab_udata_t        ud_softc;
    struct sigevent     ud_sigev;
} luab_sigevent_t;

/*
 * Subr.
 */

static void
sigevent_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_module_t *m;
    struct sigevent *sigev;

    m = luab_xmod(SIGVAL, TYPE, __func__);

    if ((sigev = (struct sigevent *)arg) != NULL) {
        luab_setinteger(L, narg, "sigev_notify",    sigev->sigev_notify);
        luab_setinteger(L, narg, "sigev_signo",     sigev->sigev_signo);
        luab_setxdata(L, narg, m, "sigev_value",    &sigev->sigev_value);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(SIGEVENT)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              sigev_notify    = (LUA_TNUMBER),
 *              sigev_signo     = (LUA_TNUMBER),
 *              sigev_value     = (LUA_T{NIL,USERDATA(SIGVAL)}),
 *          }
 *
 * @usage t [, err, msg ] = sigevent:get_table()
 */
static int
SIGEVENT_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGEVENT, TYPE, __func__);

    xtp.xtp_fill = sigevent_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate sigevent{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = sigevent:dump()
 */
static int
SIGEVENT_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGEVENT, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions.
 */

/***
 * Set notification value.
 *
 * @function set_sigev_notify
 *
 * @param arg               Specifies signal notification value.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigevent:set_sigev_notify(arg)
 */
static int
SIGEVENT_set_sigev_notify(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct sigevent *sigev;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SIGEVENT, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    sigev = luab_udata(L, 1, m0, struct sigevent *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_int_max);

    sigev->sigev_notify = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get notification value.
 *
 * @function get_sigev_notify
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = sigevent:get_sigev_notify()
 */
static int
SIGEVENT_get_sigev_notify(lua_State *L)
{
    luab_module_t *m;
    struct sigevent *sigev;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGEVENT, TYPE, __func__);
    sigev = luab_udata(L, 1, m, struct sigevent *);
    x = sigev->sigev_notify;

    return (luab_pushxinteger(L, x));
}

/***
 * Set signal number.
 *
 * @function set_sigev_signo
 *
 * @param arg               Specifies signal number.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, min ] = sigevent:set_sigev_signo(arg)
 */
static int
SIGEVENT_set_sigev_signo(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct sigevent *sigev;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SIGEVENT, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    sigev = luab_udata(L, 1, m0, struct sigevent *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_int_max);

    sigev->sigev_signo = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get signal number.
 *
 * @function get_sigev_signo
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, min ] = sigevent:get_sigev_signo()
 */
static int
SIGEVENT_get_sigev_signo(lua_State *L)
{
    luab_module_t *m;
    struct sigevent *sigev;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SIGEVENT, TYPE, __func__);
    sigev = luab_udata(L, 1, m, struct sigevent *);
    x = sigev->sigev_signo;

    return (luab_pushxinteger(L, x));
}

/***
 * Set signal value.
 *
 * @function set_sigev_value
 *
 * @param arg               Its value, by an instance of (LUA_TUSERDATA(SIGVAL)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sigval [, err, msg ] = sigevent:set_sigev_value(arg)
 */
static int
SIGEVENT_set_sigev_value(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct sigevent *sigev;
    union sigval *x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(SIGEVENT, TYPE, __func__);
    m1 = luab_xmod(SIGVAL, TYPE, __func__);

    sigev = luab_udata(L, 1, m0, struct sigevent *);

    if ((x = luab_udataisnil(L, 2, m1, union sigval *)) != NULL)
        (void)memmove(&sigev->sigev_value, x, m1->m_sz);
    else
        (void)memset_s(&sigev->sigev_value, m1->m_sz, 0, m1->m_sz);

    return (luab_pushxdata(L, m1, x));
}

/***
 * Get signal value.
 *
 * @function get_sigev_value
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage sigval [, err, msg ] = sigevent:get_sigev_value()
 */
static int
SIGEVENT_get_sigev_value(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct sigevent *sigev;
    union sigval *x;

    (void)luab_core_checkmaxargs(L, 1);

    m0 = luab_xmod(SIGEVENT, TYPE, __func__);
    m1 = luab_xmod(SIGVAL, TYPE, __func__);

    sigev = luab_udata(L, 1, m0, struct sigevent *);
    x = &sigev->sigev_value;

    return (luab_pushxdata(L, m1, x));
}

/*
 * Metamethods.
 */

static int
SIGEVENT_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGEVENT, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
SIGEVENT_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGEVENT, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
SIGEVENT_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(SIGEVENT, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

static luab_module_table_t sigevent_methods[] = {
    LUAB_FUNC("set_sigev_notify",       SIGEVENT_set_sigev_notify),
    LUAB_FUNC("set_sigev_signo",        SIGEVENT_set_sigev_signo),
    LUAB_FUNC("set_sigev_value",        SIGEVENT_set_sigev_value),
    LUAB_FUNC("get_table",              SIGEVENT_get_table),
    LUAB_FUNC("get_sigev_notify",       SIGEVENT_get_sigev_notify),
    LUAB_FUNC("get_sigev_signo",        SIGEVENT_get_sigev_signo),
    LUAB_FUNC("get_sigev_value",        SIGEVENT_get_sigev_value),
    LUAB_FUNC("dump",                   SIGEVENT_dump),
    LUAB_FUNC("__gc",                   SIGEVENT_gc),
    LUAB_FUNC("__len",                  SIGEVENT_len),
    LUAB_FUNC("__tostring",             SIGEVENT_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
sigevent_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SIGEVENT, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));
}

static void
sigevent_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(SIGEVENT, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
sigevent_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(SIGEVENT, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
sigevent_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct sigevent *x, *y;
    size_t i, j;

    m = luab_xmod(SIGEVENT, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct sigevent *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct sigevent *);
                        (void)memmove(&(x[i]), y, m->m_sz);
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
sigevent_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct sigevent *x;
    size_t i, j, k;

    m = luab_xmod(SIGEVENT, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct sigevent *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, new);

            for (i = 0, j = tbl->tbl_card, k = 1; i < j; i++, k++)
                luab_rawsetxdata(L, narg, m, k, &(x[i]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

static luab_table_t *
sigevent_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(SIGEVENT, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_sigevent_type = {
    .m_id           = LUAB_SIGEVENT_TYPE_ID,
    .m_name         = LUAB_SIGEVENT_TYPE,
    .m_vec          = sigevent_methods,
    .m_create       = sigevent_create,
    .m_init         = sigevent_init,
    .m_get          = sigevent_udata,
    .m_get_tbl      = sigevent_checktable,
    .m_set_tbl      = sigevent_pushtable,
    .m_alloc_tbl    = sigevent_alloctable,
    .m_len          = sizeof(luab_sigevent_t),
    .m_sz           = sizeof(struct sigevent),
};
#endif /* __POSIX_VISIBLE >= 199309 */
