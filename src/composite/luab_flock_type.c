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

#include <fcntl.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_flock_type;

/*
 * Interface against
 *
 *  struct flock {
 *      off_t   l_start;
 *      off_t   l_len;
 *      pid_t   l_pid;
 *      short   l_type;
 *      short   l_whence;
 *      int     l_sysid;
 *  };
 */

typedef struct luab_flock {
    luab_udata_t    ud_softc;
    struct flock    ud_l;
} luab_flock_t;

/*
 * Subr.
 */

static void
flock_fillxtable(lua_State *L, int narg, void *arg)
{
    struct flock *l;

    if ((l = (struct flock *)arg) != NULL) {

        luab_setinteger(L, narg, "l_start",   l->l_start);
        luab_setinteger(L, narg, "l_len",     l->l_len);
        luab_setinteger(L, narg, "l_pid",     l->l_pid);
        luab_setinteger(L, narg, "l_type",    l->l_type);
        luab_setinteger(L, narg, "l_whence",  l->l_whence);
        luab_setinteger(L, narg, "l_sysid",   l->l_sysid);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(FLOCK)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              l_start     = (LUA_TNUMBER),
 *              l_len       = (LUA_TNUMBER),
 *              l_pid       = (LUA_TNUMBER),
 *              l_type      = (LUA_TNUMBER),
 *              l_whence    = (LUA_TNUMBER),
 *              l_sysid     = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = flock:get_table()
 */
static int
FLOCK_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FLOCK, TYPE, __func__);

    xtp.xtp_fill = flock_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate flock{} into (LUA_TUSERDATA(IOVEC)).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = flock:dump()
 */
static int
FLOCK_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FLOCK, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions.
 */

/* starting offset - negative l_start, if l_whence = SEEK_{CUR,END} */
static int
FLOCK_set_l_start(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct flock *l;
    off_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(FLOCK, TYPE, __func__);
    m1 = luab_xmod(OFF, TYPE, __func__);

    l = luab_udata(L, 1, m0, struct flock *);
    x = (off_t)luab_checkxinteger(L, 2, m1, luab_env_ulong_max);

    l->l_start = x;

    return (luab_pushxinteger(L, x));
}

static int
FLOCK_get_l_start(lua_State *L)
{
    luab_module_t *m;
    struct flock *l;
    off_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FLOCK, TYPE, __func__);
    l = luab_udata(L, 1, m, struct flock *);
    x = l->l_start;

    return (luab_pushxinteger(L, x));
}

/* len = 0 means until end of file */
static int
FLOCK_set_l_len(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct flock *l;
    off_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(FLOCK, TYPE, __func__);
    m1 = luab_xmod(OFF, TYPE, __func__);

    l = luab_udata(L, 1, m0, struct flock *);
    x = (off_t)luab_checkxinteger(L, 2, m1, luab_env_ulong_max);

    l->l_len = x;

    return (luab_pushxinteger(L, x));
}

static int
FLOCK_get_l_len(lua_State *L)
{
    luab_module_t *m;
    struct flock *l;
    off_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FLOCK, TYPE, __func__);
    l = luab_udata(L, 1, m, struct flock *);
    x = l->l_len;

    return (luab_pushxinteger(L, x));
}

/* lock owner */
static int
FLOCK_set_l_pid(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct flock *l;
    pid_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(FLOCK, TYPE, __func__);
    m1 = luab_xmod(PID, TYPE, __func__);

    l = luab_udata(L, 1, m0, struct flock *);
    x = (pid_t)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    l->l_pid = x;

    return (luab_pushxinteger(L, x));
}

static int
FLOCK_get_l_pid(lua_State *L)
{
    luab_module_t *m;
    struct flock *l;
    pid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FLOCK, TYPE, __func__);
    l = luab_udata(L, 1, m, struct flock *);
    x = l->l_pid;

    return (luab_pushxinteger(L, x));
}

/* lock type: read/write, etc. */
static int
FLOCK_set_l_type(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct flock *l;
    short x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(FLOCK, TYPE, __func__);
    m1 = luab_xmod(SHORT, TYPE, __func__);

    l = luab_udata(L, 1, m0, struct flock *);
    x = (short)luab_checkxinteger(L, 2, m1, luab_env_ushrt_max);

    l->l_type = x;

    return (luab_pushxinteger(L, x));
}

static int
FLOCK_get_l_type(lua_State *L)
{
    luab_module_t *m;
    struct flock *l;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FLOCK, TYPE, __func__);
    l = luab_udata(L, 1, m, struct flock *);
    x = l->l_type;

    return (luab_pushxinteger(L, x));
}

/* type of l_start */
static int
FLOCK_set_l_whence(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct flock *l;
    short x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(FLOCK, TYPE, __func__);
    m1 = luab_xmod(SHORT, TYPE, __func__);

    l = luab_udata(L, 1, m0, struct flock *);
    x = (short)luab_checkxinteger(L, 2, m1, luab_env_ushrt_max);

    l->l_whence = x;

    return (luab_pushxinteger(L, x));
}

static int
FLOCK_get_l_whence(lua_State *L)
{
    luab_module_t *m;
    struct flock *l;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FLOCK, TYPE, __func__);
    l = luab_udata(L, 1, m, struct flock *);
    x = l->l_whence;

    return (luab_pushxinteger(L, x));
}

/* remote system id or zero for local */
static int
FLOCK_set_l_sysid(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct flock *l;
    int x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(FLOCK, TYPE, __func__);
    m1 = luab_xmod(INT, TYPE, __func__);

    l = luab_udata(L, 1, m0, struct flock *);
    x = (int)luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    l->l_sysid = x;

    return (luab_pushxinteger(L, x));
}

static int
FLOCK_get_l_sysid(lua_State *L)
{
    luab_module_t *m;
    struct flock *l;
    int x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(FLOCK, TYPE, __func__);
    l = luab_udata(L, 1, m, struct flock *);
    x = l->l_sysid;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
FLOCK_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FLOCK, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
FLOCK_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FLOCK, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
FLOCK_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(FLOCK, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t flock_methods[] = {
    LUAB_FUNC("set_l_start",    FLOCK_set_l_start),
    LUAB_FUNC("set_l_len",      FLOCK_set_l_len),
    LUAB_FUNC("set_l_pid",      FLOCK_set_l_pid),
    LUAB_FUNC("set_l_type",     FLOCK_set_l_type),
    LUAB_FUNC("set_l_whence",   FLOCK_set_l_whence),
    LUAB_FUNC("set_l_sysid",    FLOCK_set_l_sysid),
    LUAB_FUNC("get_table",      FLOCK_get_table),
    LUAB_FUNC("get_l_start",    FLOCK_get_l_start),
    LUAB_FUNC("get_l_len",      FLOCK_get_l_len),
    LUAB_FUNC("get_l_pid",      FLOCK_get_l_pid),
    LUAB_FUNC("get_l_type",     FLOCK_get_l_type),
    LUAB_FUNC("get_l_whence",   FLOCK_get_l_whence),
    LUAB_FUNC("get_l_sysid",    FLOCK_get_l_sysid),
    LUAB_FUNC("dump",           FLOCK_dump),
    LUAB_FUNC("__gc",           FLOCK_gc),
    LUAB_FUNC("__len",          FLOCK_len),
    LUAB_FUNC("__tostring",     FLOCK_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
flock_create(lua_State *L, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(FLOCK, TYPE, __func__);
    return (luab_newudata(L, m, arg));
}

static void
flock_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(FLOCK, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
flock_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    m = luab_xmod(FLOCK, TYPE, __func__);
    return (luab_checkludata(L, narg, m, m->m_sz));
}

static luab_table_t *
flock_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct flock *x, *y;
    size_t i, j;

    m = luab_xmod(FLOCK, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct flock *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct flock *);
                        (void)memmove(&(x[i]), y, m->m_sz);
                    } else
                        luab_core_err(EX_DATAERR, __func__, EINVAL);
                } else {
                    errno = ENOENT;
                    break;
                }
                lua_pop(L, 1);
            }
        } else
            errno = ERANGE;
    }
    return (tbl);
}

static void
flock_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct flock *x;
    size_t i, j, k;

    m = luab_xmod(FLOCK, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct flock *)tbl->tbl_vec) != NULL) &&
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
flock_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(FLOCK, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_flock_type = {
    .m_id           = LUAB_FLOCK_TYPE_ID,
    .m_name         = LUAB_FLOCK_TYPE,
    .m_vec          = flock_methods,
    .m_create       = flock_create,
    .m_init         = flock_init,
    .m_get          = flock_udata,
    .m_get_tbl      = flock_checktable,
    .m_set_tbl      = flock_pushtable,
    .m_alloc_tbl    = flock_alloctable,
    .m_len          = sizeof(luab_flock_t),
    .m_sz           = sizeof(struct flock),
};
