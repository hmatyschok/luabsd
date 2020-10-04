/*-
 * Copyright (c) 2020 Henning Matyschok <hmatyschok@outlook.com>
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

#include <errno.h>

#define LUAB_CLOCKINFO_IDX          0
#define LUAB_DIV_IDX                1
#define LUAB_FLOCK_IDX              2
#define LUAB_HOOK_IDX               3
#define LUAB_IF_NAMEINDEX_IDX       4
#define LUAB_IN_ADDR_IDX            5
#define LUAB_IN6_ADDR_IDX           6
#define LUAB_ITIMERVAL_IDX          7
#define LUAB_LDIV_IDX               8
#define LUAB_LLDIV_IDX              9
#define LUAB_LINGER_IDX             10
#define LUAB_MSGHDR_IDX             11
#define LUAB_SOCKADDR_IDX           12
#define LUAB_STAT_IDX               13
#define LUAB_TIMESPEC_IDX           14
#define LUAB_TIMEVAL_IDX            15
#define LUAB_TIMEZONE_IDX           16
#define LUAB_TM_IDX                 17
#define LUAB_UUID_IDX               18
#define LUAB_IOVEC_IDX              19
#if __BSD_VISIBLE
#define LUAB_DBT_IDX                20
#define LUAB_DB_IDX                 21
#define LUAB_BINTIME_IDX            22
#define LUAB_CRYPT_DATA_IDX         23
#define LUAB_CAP_RBUF_IDX           24
#define LUAB_ACCEPT_FILTER_ARG_IDX  25
#define LUAB_SOCKPROTO_IDX          26
#define LUAB_CMSGCRED_IDX           27
#endif /* __BSD_VISIBLE */

/*
 * Selector.
 */

#define luab_idx(name) \
    (LUAB_##name##_IDX)
#define luab_vx(idx) \
    (luab_typevec[(idx)])
#define luab_mx(name) \
    ((luab_vx(luab_idx(name))).mv_mod)

extern luab_modulevec_t luab_typevec[];

/*
 * Accessor, (LUA_TUSERDATA(XXX)), [stack -> C].
 */

typedef struct luab_xarg {
    int         xarg_idx;
    size_t      xarg_len;
} luab_xarg_t;

static __inline size_t
luab_xlen(luab_module_t *m)
{
    return ((m->sz - sizeof(luab_udata_t)));
}

static __inline void *
luab_toxudata(lua_State *L, int narg, luab_xarg_t *pci)
{
    luab_modulevec_t *vec;
    luab_udata_t *ud;

    if (pci != NULL) {

        for (vec = luab_typevec; vec->mv_mod != NULL; vec++) {
            if ((ud = luab_isudata(L, narg, vec->mv_mod)) != NULL)
                break;
        }

        if (ud != NULL) {
            pci->xarg_idx = vec->mv_idx;
            pci->xarg_len = luab_xlen(vec->mv_mod);
        } else {
            pci->xarg_idx = -1;
            pci->xarg_len = 0;
        }
    } else {
        errno = EINVAL;
        ud = NULL;
    }
    return (ud);
}

static __inline void *
luab_toxdata(lua_State *L, int narg, luab_xarg_t *pci)
{
    luab_udata_t *ud;

    if ((ud = luab_toxudata(L, narg, pci)) != NULL)
        return (ud + 1);

    return (NULL);
}
