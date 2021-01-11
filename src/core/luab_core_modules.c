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
#include "luab_modules.h"
#include "luab_udata.h"
#include "luab_table.h"

luab_module_t *
luab_core_checkmodule(luab_type_t idx, uint32_t id, const char *fname)
{
    luab_module_t *m;

    if ((m = luab_xmv(idx).mv_mod) != NULL) {

        if (m->m_id == id)
            return (m);
        else
            luab_core_err(EX_UNAVAILABLE, fname, ENOENT);
    } else
        luab_core_err(EX_UNAVAILABLE, fname, ENOSYS);

    return (NULL);
}

luab_module_t *
luab_core_checkxmodule(luab_type_t idx, uint32_t id, const char *fname,
    luab_xmodule_t *xmp)
{
    luab_module_t *m;

    m = luab_core_checkmodule(idx, id, fname);

    if (xmp != NULL) {
        xmp->xm_idx = idx;
        xmp->xm_id = id;
        xmp->xm_fname = fname;
        xmp->xm_mod = m;
    } else
        luab_core_err(EX_DATAERR, __func__, ENXIO);

    return (m);
}

/*
 * Common subr. for initializiation, those are
 * called during runtime of package.loadlib().
 */

void
luab_core_populate(lua_State *L, int narg, luab_module_t *m)
{
    luab_module_table_t *tok;

    if ((tok = m->m_vec) != NULL) {

        do {
            if (tok->mt_init != NULL) {
                (void)(*tok->mt_init)(L, &tok->mt_val);
                lua_setfield(L, narg, tok->mt_key);
            } else
                errno = ENOENT;

            tok++;
        } while (tok->mt_key != NULL);

        lua_pop(L, 0);
    } else
        luab_core_err(EX_DATAERR, __func__, ENXIO);
}

void
luab_core_newtable(lua_State *L, int narg, luab_module_t *m)
{
    if (m != NULL) {
        luab_table_init(L, 1);
        luab_core_populate(L, narg, m);
        lua_setfield(L, narg, m->m_name);
    } else
        luab_core_err(EX_DATAERR, __func__, ENOEXEC);
}

void
luab_core_newmetatable(lua_State *L, int narg, luab_module_t *m)
{
    if (m != NULL) {
        luaL_newmetatable(L, m->m_name);

        if (narg < 0)
            lua_pushvalue(L, narg + 1);
        else
            lua_pushvalue(L, narg - 1);

        lua_setfield(L, narg, "__index");
        luab_core_populate(L, narg, m);

        lua_pop(L, 1);
    } else
        luab_core_err(EX_DATAERR, __func__, ENOEXEC);
}

/*
 * Reflects and maps interface against API over </include/>.
 */

void
luab_core_initmodule(lua_State *L, int narg, luab_module_vec_t *vec,
    const char *name, int new)
{
    luab_module_vec_t *mv;

    if (name != NULL && new != 0)
        luab_table_init(L, 1);

    mv = vec;

     do {
        if (mv->mv_init != NULL)
            (*mv->mv_init)(L, narg, mv->mv_mod);
        else
            errno = ENOENT;

        mv++;
    } while (mv->mv_mod != NULL);

    if (name != NULL && new != 0)
        lua_setfield(L, narg, name);
}
