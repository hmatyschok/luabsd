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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS IPC_PERMERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/ipc.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_ipc_perm_type;

/*
 * Interface against
 *
 *  struct ipc_perm {
 *      uid_t       cuid;
 *      gid_t       cgid;
 *      uid_t       uid;
 *      gid_t       gid;
 *      mode_t      mode;
 *      unsigned short  seq;
 *      key_t       key;
 *  };
 */

typedef struct luab_ipc_perm {
    luab_udata_t        ud_softc;
    struct ipc_perm     ud_perm;
} luab_ipc_perm_t;


/*
 * Subr.
 */

static void
ipc_perm_fillxtable(lua_State *L, int narg, void *arg)
{
    struct ipc_perm *perm;

    if ((perm = (struct ipc_perm *)arg) != NULL) {
        luab_setinteger(L, narg, "cuid",    perm->cuid);
        luab_setinteger(L, narg, "cgid",    perm->cgid);
        luab_setinteger(L, narg, "uid",     perm->uid);
        luab_setinteger(L, narg, "gid",     perm->gid);
        luab_setinteger(L, narg, "mode",    perm->mode);
        luab_setinteger(L, narg, "mode",    perm->seq);
        luab_setinteger(L, narg, "mode",    perm->key);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(IPC_PERM)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              cuid    = (LUA_TNUMBER),
 *              cgid    = (LUA_TNUMBER),
 *              uid     = (LUA_TNUMBER),
 *              gid     = (LUA_TNUMBER),
 *              mode    = (LUA_TNUMBER),
 *              seq     = (LUA_TNUMBER),
 *              key     = (LUA_TNUMBER),
 *          }
 *
 * @usage t [, err, msg ] = ipc_perm:get_table()
 */
static int
IPC_PERM_get_table(lua_State *L)
{
    luab_module_t *m;
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IPC_PERM, TYPE, __func__);

    xtp.xtp_fill = ipc_perm_fillxtable;
    xtp.xtp_arg = luab_xdata(L, 1, m);
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - translate ipc_perm{} into (LUA_TUSERDATA(IOVEC))
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = ipc_perm:dump()
 */
static int
IPC_PERM_dump(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IPC_PERM, TYPE, __func__);
    return (luab_core_dump(L, 1, m, m->m_sz));
}

/*
 * Access functions.
 */

/***
 * Set creator user ID.
 *
 * @function set_cuid
 *
 * @param arg               Specifies creator user ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:set_cuid(arg)
 */
static int
IPC_PERM_set_cuid(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct ipc_perm *perm;
    uid_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IPC_PERM, TYPE, __func__);
    m1 = luab_xmod(UID, TYPE, __func__);

    perm = luab_udata(L, 1, m0, struct ipc_perm *);
    x = luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    perm->cuid = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get creator user ID.
 *
 * @function get_cuid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:get_cuid()
 */
static int
IPC_PERM_get_cuid(lua_State *L)
{
    luab_module_t *m;
    struct ipc_perm *perm;
    uid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IPC_PERM, TYPE, __func__);

    perm = luab_udata(L, 1, m, struct ipc_perm *);
    x = perm->cuid;

    return (luab_pushxinteger(L, x));
}

/***
 * Set creator group ID.
 *
 * @function set_cgid
 *
 * @param arg               Specifies creator group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:set_cgid(arg)
 */
static int
IPC_PERM_set_cgid(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct ipc_perm *perm;
    gid_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IPC_PERM, TYPE, __func__);
    m1 = luab_xmod(GID, TYPE, __func__);

    perm = luab_udata(L, 1, m0, struct ipc_perm *);
    x = luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    perm->cgid = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get creator group ID.
 *
 * @function get_cgid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:get_cgid()
 */
static int
IPC_PERM_get_cgid(lua_State *L)
{
    luab_module_t *m;
    struct ipc_perm *perm;
    gid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IPC_PERM, TYPE, __func__);

    perm = luab_udata(L, 1, m, struct ipc_perm *);
    x = perm->cgid;

    return (luab_pushxinteger(L, x));
}

/***
 * Set user ID.
 *
 * @function set_uid
 *
 * @param arg               Specifies user ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:set_uid(arg)
 */
static int
IPC_PERM_set_uid(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct ipc_perm *perm;
    uid_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IPC_PERM, TYPE, __func__);
    m1 = luab_xmod(UID, TYPE, __func__);

    perm = luab_udata(L, 1, m0, struct ipc_perm *);
    x = luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    perm->uid = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get user ID.
 *
 * @function get_uid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:get_uid()
 */
static int
IPC_PERM_get_uid(lua_State *L)
{
    luab_module_t *m;
    struct ipc_perm *perm;
    uid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IPC_PERM, TYPE, __func__);

    perm = luab_udata(L, 1, m, struct ipc_perm *);
    x = perm->uid;

    return (luab_pushxinteger(L, x));
}

/***
 * Set group ID.
 *
 * @function set_gid
 *
 * @param arg               Specifies group ID.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:set_gid(arg)
 */
static int
IPC_PERM_set_gid(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct ipc_perm *perm;
    gid_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IPC_PERM, TYPE, __func__);
    m1 = luab_xmod(GID, TYPE, __func__);

    perm = luab_udata(L, 1, m0, struct ipc_perm *);
    x = luab_checkxinteger(L, 2, m1, luab_env_uint_max);

    perm->gid = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get group ID.
 *
 * @function get_gid
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:get_gid()
 */
static int
IPC_PERM_get_gid(lua_State *L)
{
    luab_module_t *m;
    struct ipc_perm *perm;
    gid_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IPC_PERM, TYPE, __func__);

    perm = luab_udata(L, 1, m, struct ipc_perm *);
    x = perm->gid;

    return (luab_pushxinteger(L, x));
}

/***
 * Set r/w permission.
 *
 * @function set_mode
 *
 * @param arg               Specifies r/w permission.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:set_mode(arg)
 */
static int
IPC_PERM_set_mode(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct ipc_perm *perm;
    mode_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IPC_PERM, TYPE, __func__);
    m1 = luab_xmod(MODE, TYPE, __func__);

    perm = luab_udata(L, 1, m0, struct ipc_perm *);
    x = luab_checkxinteger(L, 2, m1, luab_env_ushrt_max);

    perm->mode = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get r/w permission.
 *
 * @function get_mode
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:get_mode()
 */
static int
IPC_PERM_get_mode(lua_State *L)
{
    luab_module_t *m;
    struct ipc_perm *perm;
    mode_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IPC_PERM, TYPE, __func__);

    perm = luab_udata(L, 1, m, struct ipc_perm *);
    x = perm->mode;

    return (luab_pushxinteger(L, x));
}

/***
 * Set sequence number to generate unique ipcid.
 *
 * @function set_seq
 *
 * @param arg               Specifies sequence number.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:set_seq(arg)
 */
static int
IPC_PERM_set_seq(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct ipc_perm *perm;
    u_short x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IPC_PERM, TYPE, __func__);
    m1 = luab_xmod(USHRT, TYPE, __func__);

    perm = luab_udata(L, 1, m0, struct ipc_perm *);
    x = luab_checkxinteger(L, 2, m1, luab_env_ushrt_max);

    perm->seq = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get sequence number to generate unique ipcid.
 *
 * @function get_seq
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:get_seq()
 */
static int
IPC_PERM_get_seq(lua_State *L)
{
    luab_module_t *m;
    struct ipc_perm *perm;
    u_short x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IPC_PERM, TYPE, __func__);

    perm = luab_udata(L, 1, m, struct ipc_perm *);
    x = perm->seq;

    return (luab_pushxinteger(L, x));
}

/***
 * Set user specified msg/sem/shm key.
 *
 * @function set_key
 *
 * @param arg               Specifies sequence number.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:set_key(arg)
 */
static int
IPC_PERM_set_key(lua_State *L)
{
    luab_module_t *m0, *m1;
    struct ipc_perm *perm;
    key_t x;

    (void)luab_core_checkmaxargs(L, 2);

    m0 = luab_xmod(IPC_PERM, TYPE, __func__);
    m1 = luab_xmod(KEY, TYPE, __func__);

    perm = luab_udata(L, 1, m0, struct ipc_perm *);
    x = luab_checkxinteger(L, 2, m1, luab_env_ushrt_max);

    perm->seq = x;

    return (luab_pushxinteger(L, x));
}

/***
 * Get user specified msg/sem/shm key.
 *
 * @function get_key
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage x [, err, msg ] = ipc_perm:get_key()
 */
static int
IPC_PERM_get_key(lua_State *L)
{
    luab_module_t *m;
    struct ipc_perm *perm;
    key_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(IPC_PERM, TYPE, __func__);

    perm = luab_udata(L, 1, m, struct ipc_perm *);
    x = perm->seq;

    return (luab_pushxinteger(L, x));
}

/*
 * Metamethods.
 */

static int
IPC_PERM_gc(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IPC_PERM, TYPE, __func__);
    return (luab_core_gc(L, 1, m));
}

static int
IPC_PERM_len(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IPC_PERM, TYPE, __func__);
    return (luab_core_len(L, 2, m));
}

static int
IPC_PERM_tostring(lua_State *L)
{
    luab_module_t *m;
    m = luab_xmod(IPC_PERM, TYPE, __func__);
    return (luab_core_tostring(L, 1, m));
}

/*
 * Internal interface.
 */

static luab_module_table_t ipc_perm_methods[] = {
    LUAB_FUNC("set_cuid",       IPC_PERM_set_cuid),
    LUAB_FUNC("set_cgid",       IPC_PERM_set_cgid),
    LUAB_FUNC("set_uid",        IPC_PERM_set_uid),
    LUAB_FUNC("set_gid",        IPC_PERM_set_gid),
    LUAB_FUNC("set_mode",       IPC_PERM_set_mode),
    LUAB_FUNC("set_seq",        IPC_PERM_set_seq),
    LUAB_FUNC("set_key",        IPC_PERM_set_key),
    LUAB_FUNC("get_table",      IPC_PERM_get_table),
    LUAB_FUNC("get_cuid",       IPC_PERM_get_cuid),
    LUAB_FUNC("get_cgid",       IPC_PERM_get_cgid),
    LUAB_FUNC("get_uid",        IPC_PERM_get_uid),
    LUAB_FUNC("get_gid",        IPC_PERM_get_gid),
    LUAB_FUNC("get_mode",       IPC_PERM_get_mode),
    LUAB_FUNC("get_seq",        IPC_PERM_get_seq),
    LUAB_FUNC("get_key",        IPC_PERM_get_key),
    LUAB_FUNC("dump",           IPC_PERM_dump),
    LUAB_FUNC("__gc",           IPC_PERM_gc),
    LUAB_FUNC("__len",          IPC_PERM_len),
    LUAB_FUNC("__tostring",     IPC_PERM_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
ipc_perm_create(lua_State *L, void *arg __unused)
{
    luab_module_t *m;
    m = luab_xmod(SCHED_PARAM, TYPE, __func__);
    return (luab_newuserdata(L, m, arg));

}

static void
ipc_perm_init(void *ud, void *arg)
{
    luab_module_t *m;
    m = luab_xmod(IPC_PERM, TYPE, __func__);
    luab_udata_init(m, ud, arg);
}

static void *
ipc_perm_udata(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_ipc_perm_t *self;

    m = luab_xmod(IPC_PERM, TYPE, __func__);
    self = luab_checkudata(L, narg, m);
    return ((void *)&(self->ud_perm));
}

static luab_table_t *
ipc_perm_checktable(lua_State *L, int narg)
{
    luab_module_t *m;
    luab_table_t *tbl;
    struct ipc_perm *x, *y;
    size_t i, j;

    m = luab_xmod(IPC_PERM, TYPE, __func__);

    if ((tbl = luab_table_newvectornil(L, narg, m)) != NULL) {

        if (((x = (struct ipc_perm *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 0)) {
            luab_table_init(L, 0);

            for (i = 0, j = tbl->tbl_card; i < j; i++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, m, struct ipc_perm *);
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
ipc_perm_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    luab_module_t *m;
    struct ipc_perm *x;
    size_t i, j, k;

    m = luab_xmod(IPC_PERM, TYPE, __func__);

    if (tbl != NULL) {

        if (((x = (struct ipc_perm *)tbl->tbl_vec) != NULL) &&
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
ipc_perm_alloctable(void *vec, size_t card)
{
    luab_module_t *m;
    m = luab_xmod(IPC_PERM, TYPE, __func__);
    return (luab_table_create(m, vec, card));
}

luab_module_t luab_ipc_perm_type = {
    .m_id           = LUAB_IPC_PERM_TYPE_ID,
    .m_name         = LUAB_IPC_PERM_TYPE,
    .m_vec          = ipc_perm_methods,
    .m_create       = ipc_perm_create,
    .m_init         = ipc_perm_init,
    .m_get          = ipc_perm_udata,
    .m_get_tbl      = ipc_perm_checktable,
    .m_set_tbl      = ipc_perm_pushtable,
    .m_alloc_tbl    = ipc_perm_alloctable,
    .m_len          = sizeof(luab_ipc_perm_t),
    .m_sz           = sizeof(struct ipc_perm),
};
