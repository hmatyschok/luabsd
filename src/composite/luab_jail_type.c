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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS JAILERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/jail.h>

#include <netinet/in.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"
#include "luab_table.h"

extern luab_module_t luab_jail_type;

/*
 * Interface against
 *
 *  struct jail {
 *      uint32_t    version;
 *      char        *path;
 *      char        *hostname;
 *      char        *jailname;
 *      uint32_t    ip4s;
 *      uint32_t    ip6s;
 *      struct in_addr  *ip4;
 *      struct in6_addr *ip6;
 *  };
 *
 * JAIL_API_VERSION:    2
 */

typedef enum luab_xaddr {
    LUAB_XADDR_IP4,
    LUAB_XADDR_IP6,
    LUAB_XADDR_MAX
} luab_xaddr_t;

typedef int    (*luab_xaddr_init_fn)(struct jail *, luab_table_t *);

typedef struct luab_xaddr_vec {
    luab_xaddr_t        xav_idx;
    luab_type_t         xav_type;
    luab_id_t           xav_cookie;
    luab_xaddr_init_fn  xav_init;
} luab_xaddr_vec_t;

typedef struct luab_jail {
    luab_udata_t    ud_softc;
    struct jail     ud_jail;
    luab_table_t    *ud_cache[LUAB_XADDR_MAX];
} luab_jail_t;

#define luab_new_jail(L, arg) \
    ((luab_jail_t *)luab_newudata(L, &luab_jail_type, (arg)))
#define luab_to_jail(L, narg) \
    (luab_todata((L), (narg), &luab_jail_type, luab_jail_t *))

#define LUAB_XADDR_VEC_SENTINEL                     \
    {                                               \
        .xav_idx   = LUAB_XADDR_MAX,                \
        .xav_type = 0,                              \
        .xav_cookie = 0,                            \
        .xav_init   = NULL,                         \
    }

#define luab_xdomain(idx) \
    (jail_domain[((idx) % LUAB_XADDR_MAX)])

/*
 * Subr.
 */

static int
jail_ip4_addr(struct jail *jp, luab_table_t *tbl)
{
    if (tbl != NULL) {
        jp->ip4 = (struct in_addr *)(tbl->tbl_vec);
        jp->ip4s = (tbl->tbl_card - 1);
    } else {
        jp->ip4 = NULL;
        jp->ip4s = 0;
    }
    return (jp->ip4s);
}

static int
jail_ip6_addr(struct jail *jp, luab_table_t *tbl)
{
    if (tbl != NULL) {
        jp->ip6 = (struct in6_addr *)(tbl->tbl_vec);
        jp->ip6s = (tbl->tbl_card - 1);
    } else {
        jp->ip6 = NULL;
        jp->ip6s = 0;
    }
    return (jp->ip6s);
}

static luab_xaddr_vec_t jail_domain[] = {
    {
        .xav_idx    = LUAB_XADDR_IP4,
        .xav_type   = LUAB_IN_ADDR_IDX,
        .xav_cookie = LUAB_IN_ADDR_TYPE_ID,
        .xav_init   = jail_ip4_addr,
    },{
        .xav_idx    = LUAB_XADDR_IP6,
        .xav_type   = LUAB_IN6_ADDR_IDX,
        .xav_cookie = LUAB_IN6_ADDR_TYPE_ID,
        .xav_init   = jail_ip6_addr,
    },
    LUAB_XADDR_VEC_SENTINEL
};

static int
jail_checkxaddrtable(lua_State *L, int narg, luab_jail_t *self, luab_xaddr_t n)
{
    luab_xaddr_vec_t *xav;
    luab_module_t *m;
    luab_table_t *tbl;
    int card = 0;

    if (self != NULL) {
        xav = &(luab_xdomain(n % LUAB_XADDR_MAX));
        m = luab_core_checkmodule(xav->xav_type, xav->xav_cookie, __func__);

        if ((tbl = luab_table_checkxdata(L, narg, m)) != NULL) {

            if (self->ud_cache[xav->xav_idx] != NULL)
                luab_table_free(self->ud_cache[xav->xav_idx]);

            if (tbl->tbl_card == 0) {
                luab_table_free(tbl);
                tbl = NULL;
            }
            self->ud_cache[xav->xav_idx] = tbl;
        }
        card = (*xav->xav_init)(&self->ud_jail, tbl);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);

    return (card);
}

static int
jail_pushxaddrtable(lua_State *L, int narg, const char *k, luab_jail_t *self,
    luab_xaddr_t n)
{
    luab_xaddr_vec_t *xav;
    luab_module_t *m;
    luab_table_t *tbl;

    xav = &(luab_xdomain((n % LUAB_XADDR_MAX)));
    m = luab_core_checkmodule(xav->xav_type, xav->xav_cookie, __func__);

    if ((tbl = self->ud_cache[xav->xav_idx]) != NULL) {
        luab_table_pushxdata(L, narg, m, tbl, 1, 0);

        /*
         * set field k and/or push on top of Lua stack
         *
         * XXX DRY
         */
        if (k != NULL)
            lua_setfield(L, narg, k);
        else {
            if (narg < 0)
                lua_pushvalue(L, narg + 1);
            else
                lua_pushvalue(L, narg);
        }
        errno = 0;
    } else
        errno = ERANGE;

    return (luab_table_pusherr(L, errno, 1));
}

static void
jail_fillxtable(lua_State *L, int narg, void *arg)
{
    luab_jail_t *self;
    struct jail *jp;

    if ((self = (luab_jail_t *)arg) != NULL) {
        jp = &(self->ud_jail);

        luab_setinteger(L, narg, "version",   jp->version);
        luab_setstring(L, narg, "path",       jp->path);
        luab_setstring(L, narg, "hostname",   jp->hostname);
        luab_setstring(L, narg, "jailname",   jp->jailname);
        luab_setinteger(L, narg, "ip4s",      jp->ip4s);
        luab_setinteger(L, narg, "ip6s",      jp->ip6s);

        if (jp->ip4 != NULL)
            (void)jail_pushxaddrtable(L, narg, "ip4", self, LUAB_XADDR_IP4);

        if (jp->ip6 != NULL)
            (void)jail_pushxaddrtable(L, narg, "ip6", self, LUAB_XADDR_IP6);
    } else
        luab_core_err(EX_DATAERR, __func__, EINVAL);
}

/*
 * Generator functions.
 */

/***
 * Generator function - translate (LUA_TUSERDATA(JAIL)) into (LUA_TTABLE).
 *
 * @function get_table
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 *          t = {
 *              version     = (LUA_TNUMBER),
 *              path        = (LUA_T{NIL,STRING}),
 *              hostname    = (LUA_T{NIL,STRING}),
 *              jailname    = (LUA_T{NIL,STRING}),
 *              ip4s        = (LUA_TNUMBER),
 *              ip6s        = (LUA_TNUMBER),
 *              ip4         = (LUA_T{NIL,TABLE}),
 *              ip6         = (LUA_T{NIL,TABLE}),
 *          }
 *
 * @usage t [, err, msg ] = jail:get_table()
 */
static int
JAIL_get_table(lua_State *L)
{
    luab_xtable_param_t xtp;

    (void)luab_core_checkmaxargs(L, 1);

    xtp.xtp_fill = jail_fillxtable;
    xtp.xtp_arg = (void *)luab_to_jail(L, 1);   /* XXX */
    xtp.xtp_new = 1;
    xtp.xtp_k = NULL;

    return (luab_table_pushxtable(L, -2, &xtp));
}

/***
 * Generator function - returns (LUA_TNIL).
 *
 * @function dump
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage iovec [, err, msg ] = jail:dump()
 */
static int
JAIL_dump(lua_State *L)
{
    return (luab_core_dump(L, 1, NULL, 0));
}

/*
 * Access functions, immutable properties.
 */

/***
 * Get jail(4) API version.
 *
 * @function version
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = jail:get_version()
 */
static int
JAIL_version(lua_State *L)
{
    struct jail *jp;
    uint32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    jp = luab_udata(L, 1, &luab_jail_type, struct jail *);
    x = jp->version;

    return (luab_pushxinteger(L, x));
}

/***
 * Get cardinality of IPv4 adresses from jail(4).
 *
 * @function ip4s
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = jail:ip4s()
 */
static int
JAIL_ip4s(lua_State *L)
{
    struct jail *jp;
    uint32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    jp = luab_udata(L, 1, &luab_jail_type, struct jail *);
    x = jp->ip4s;

    return (luab_pushxinteger(L, x));
}

/***
 * Get cradinality of IPv6 adresses from jail(4).
 *
 * @function ip6s
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = jail:ip6s()
 */
static int
JAIL_ip6s(lua_State *L)
{
    struct jail *jp;
    uint32_t x;

    (void)luab_core_checkmaxargs(L, 1);

    jp = luab_udata(L, 1, &luab_jail_type, struct jail *);
    x = jp->ip6s;

    return (luab_pushxinteger(L, x));
}

/*
 * Access functions.
 */

/***
 * Set jail(4) path.
 *
 * @function set_path
 *
 * @param data              Specifies path.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = jail:set_path(data)
 */
static int
JAIL_set_path(lua_State *L)
{
    struct jail *jp;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 2);

    jp = luab_udata(L, 1, &luab_jail_type, struct jail *);
    dp = luab_checklstringalloc(L, 2, luab_env_path_max);

    luab_core_freestr(jp->path);
    jp->path = dp;

    return (luab_pushstring(L, dp));
}

/***
 * Get jail(4) path.
 *
 * @function get_path
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = jail:get_path()
 */
static int
JAIL_get_path(lua_State *L)
{
    struct jail *jp;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    jp = luab_udata(L, 1, &luab_jail_type, struct jail *);
    dp = jp->path;

    return (luab_pushstring(L, dp));
}

/***
 * Set jail(4) hostname.
 *
 * @function set_hostname
 *
 * @param data              Specifies hostname.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = jail:set_hostname(data)
 */
static int
JAIL_set_hostname(lua_State *L)
{
    struct jail *jp;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 2);

    jp = luab_udata(L, 1, &luab_jail_type, struct jail *);
    dp = luab_checklstringalloc(L, 2, luab_env_host_name_max);

    luab_core_freestr(jp->hostname);
    jp->hostname = dp;

    return (luab_pushstring(L, dp));
}

/***
 * Get jail(4) hostname.
 *
 * @function get_hostname
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = jail:get_hostname()
 */
static int
JAIL_get_hostname(lua_State *L)
{
    struct jail *jp;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    jp = luab_udata(L, 1, &luab_jail_type, struct jail *);
    dp = jp->hostname;

    return (luab_pushstring(L, dp));
}

/***
 * Set jail(4) name.
 *
 * @function set_jailname
 *
 * @param data              Specifies jailname.
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = jail:set_jailname(data)
 */
static int
JAIL_set_jailname(lua_State *L)
{
    struct jail *jp;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 2);

    jp = luab_udata(L, 1, &luab_jail_type, struct jail *);
    dp = luab_checklstringalloc(L, 2, luab_env_host_name_max);

    luab_core_freestr(jp->jailname);
    jp->jailname = dp;

    return (luab_pushstring(L, dp));
}

/***
 * Get jail(4) name.
 *
 * @function get_jailname
 *
 * @return (LUA_T{NIL,STRING} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = jail:get_jailname()
 */
static int
JAIL_get_jailname(lua_State *L)
{
    struct jail *jp;
    caddr_t dp;

    (void)luab_core_checkmaxargs(L, 1);

    jp = luab_udata(L, 1, &luab_jail_type, struct jail *);
    dp = jp->jailname;

    return (luab_pushstring(L, dp));
}

/***
 * Set IPv4 adresses.
 *
 * @function set_jailname
 *
 * @param data              Specifies (LUA_TTABLE) capable to hold at least
 *                          one instance of (LUA_TUSERDATA(IN_ADDR)) or more.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = jail:set_ip4(data)
 */
static int
JAIL_set_ip4(lua_State *L)
{
    luab_jail_t *self;
    size_t card;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_jail(L, 1);
    card = jail_checkxaddrtable(L, 2, self, LUAB_XADDR_IP4);
    return (luab_pushxinteger(L, card));
}

/***
 * Get IPv4 adresses.
 *
 * @function get_ip4
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = jail:get_ip4()
 */
static int
JAIL_get_ip4(lua_State *L)
{
    luab_jail_t *self;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_jail(L, 1);

    return (jail_pushxaddrtable(L, -2, NULL, self, LUAB_XADDR_IP4));
}

/***
 * Set IPv6 adresses.
 *
 * @function set_jailname
 *
 * @param data              Specifies (LUA_TTABLE) capable to hold at least
 *                          one instance of (LUA_TUSERDATA(IN_ADDR)) or more.
 *
 * @return (LUA_TNUMBER [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = jail:set_ip6(data)
 */
static int
JAIL_set_ip6(lua_State *L)
{
    luab_jail_t *self;
    size_t card;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_jail(L, 1);
    card = jail_checkxaddrtable(L, 2, self, LUAB_XADDR_IP6);
    return (luab_pushxinteger(L, card));
}

/***
 * Get IPv6 adresses.
 *
 * @function get_ip6
 *
 * @return (LUA_T{NIL,TABLE} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage data [, err, msg ] = jail:get_ip6()
 */
static int
JAIL_get_ip6(lua_State *L)
{
    luab_jail_t *self;

    (void)luab_core_checkmaxargs(L, 2);

    self = luab_to_jail(L, 1);

    return (jail_pushxaddrtable(L, -2, NULL, self, LUAB_XADDR_IP6));
}

/*
 * Metamethods.
 */

static int
JAIL_gc(lua_State *L)
{
    luab_jail_t *self;
    struct jail *jp;
    luab_xaddr_t n;
    luab_table_t *tbl;

    (void)luab_core_checkmaxargs(L, 1);

    self = luab_to_jail(L, 1);
    jp = &(self->ud_jail);

    luab_core_freestr(jp->path);
    luab_core_freestr(jp->hostname);
    luab_core_freestr(jp->jailname);

    n = LUAB_XADDR_IP4;

    do {
        tbl = self->ud_cache[n];
        luab_table_free(tbl);
        n++;
    } while (n < LUAB_XADDR_MAX);

    return (luab_core_gc(L, 1, &luab_jail_type));
}

static int
JAIL_len(lua_State *L)
{
    return (luab_core_len(L, 2, &luab_jail_type));
}

static int
JAIL_tostring(lua_State *L)
{
    return (luab_core_tostring(L, 1, &luab_jail_type));
}

/*
 * Internal jailerface.
 */

static luab_module_table_t jail_methods[] = {
    LUAB_FUNC("version",        JAIL_version),
    LUAB_FUNC("ip4s",           JAIL_ip4s),
    LUAB_FUNC("ip6s",           JAIL_ip6s),
    LUAB_FUNC("set_path",       JAIL_set_path),
    LUAB_FUNC("set_hostname",   JAIL_set_hostname),
    LUAB_FUNC("set_jailname",   JAIL_set_jailname),
    LUAB_FUNC("set_ip4",        JAIL_set_ip4),
    LUAB_FUNC("set_ip6",        JAIL_set_ip6),
    LUAB_FUNC("get_table",      JAIL_get_table),
    LUAB_FUNC("get_path",       JAIL_get_path),
    LUAB_FUNC("get_hostname",   JAIL_get_hostname),
    LUAB_FUNC("get_jailname",   JAIL_get_jailname),
    LUAB_FUNC("get_ip4",        JAIL_get_ip4),
    LUAB_FUNC("get_ip6",        JAIL_get_ip6),
    LUAB_FUNC("dump",           JAIL_dump),
    LUAB_FUNC("__gc",           JAIL_gc),
    LUAB_FUNC("__len",          JAIL_len),
    LUAB_FUNC("__tostring",     JAIL_tostring),
    LUAB_MOD_TBL_SENTINEL
};

static void *
jail_create(lua_State *L, void *arg __unused)
{
    luab_jail_t *self;
    int api;

    if (JAIL_API_VERSION == 2) {
        api = JAIL_API_VERSION;
        self = luab_new_jail(L, &api);
    } else
        self = NULL;

    return (self);
}

static void
jail_init(void *ud, void *arg)
{
    luab_jail_t *self;
    int *xp;

    if (((self = (luab_jail_t *)ud) != NULL) &&
        ((xp = (int *)arg) != NULL))
        self->ud_jail.version = *xp;
}

static void *
jail_udata(lua_State *L, int narg)
{
    luab_jail_t *self;
    self = luab_to_jail(L, narg);
    return ((void *)&(self->ud_jail));
}

static luab_table_t *
jail_checktable(lua_State *L, int narg)
{
    luab_table_t *tbl;
    struct jail *x, *y;
    size_t m, n, sz;

    sz = sizeof(struct jail);

    if ((tbl = luab_newvectornil(L, narg, sz)) != NULL) {

        if (((x = (struct jail *)tbl->tbl_vec) != NULL) &&
            (tbl->tbl_card > 1)) {
            luab_table_init(L, 0);

            for (m = 0, n = (tbl->tbl_card - 1); m < n; m++) {

                if (lua_next(L, narg) != 0) {

                    if ((lua_isnumber(L, -2) != 0) &&
                        (lua_isuserdata(L, -1) != 0)) {
                        y = luab_udata(L, -1, &luab_jail_type, struct jail *);
                        (void)memmove(&(x[m]), y, sz);
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
jail_pushtable(lua_State *L, int narg, luab_table_t *tbl, int new, int clr)
{
    struct jail *x;
    size_t m, n, k;

    if (tbl != NULL) {

        if (((x = (struct jail *)tbl->tbl_vec) != NULL) &&
            ((n = (tbl->tbl_card - 1)) != 0)) {
            luab_table_init(L, new);

            for (m = 0, k = 1; m < n; m++, k++)
                luab_rawsetudata(L, narg, &luab_jail_type, k, &(x[m]));

            errno = ENOENT;
        } else
            errno = ERANGE;

        if (clr != 0)
            luab_table_free(tbl);
    } else
        errno = ERANGE;
}

luab_module_t luab_jail_type = {
    .m_cookie   = LUAB_JAIL_TYPE_ID,
    .m_name     = LUAB_JAIL_TYPE,
    .m_vec      = jail_methods,
    .m_create   = jail_create,
    .m_init     = jail_init,
    .m_get      = jail_udata,
    .m_get_tbl  = jail_checktable,
    .m_set_tbl  = jail_pushtable,
    .m_sz       = sizeof(luab_jail_t),
};
