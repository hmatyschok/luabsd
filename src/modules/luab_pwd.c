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

#include <pwd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_PWD_LIB_ID    1604247804
#define LUAB_PWD_LIB_KEY    "pwd"

extern luab_module_t luab_pwd_lib;

/*
 * Service primitves.
 */

/*
 * Generator functions.
 */

/*
 * Interface against <pwd.h>.
 */

static luab_module_table_t luab_pwd_vec[] = { /* pwd.h */
    LUAB_STR("_PATH_PWD",               _PATH_PWD),
    LUAB_STR("_PATH_PASSWD",            _PATH_PASSWD),
    LUAB_STR("_PASSWD",                 _PASSWD),
    LUAB_STR("_PATH_MASTERPASSWD",      _PATH_MASTERPASSWD),
    LUAB_STR("_MASTERPASSWD",           _MASTERPASSWD),
    LUAB_STR("_PATH_MP_DB",             _PATH_MP_DB),
    LUAB_STR("_MP_DB",                  _MP_DB),
    LUAB_STR("_PATH_SMP_DB",            _PATH_SMP_DB),
    LUAB_STR("_SMP_DB",                 _SMP_DB),
    LUAB_STR("_PATH_PWD_MKDB",          _PATH_PWD_MKDB),
    LUAB_INT("_PW_VERSION_MASK",        _PW_VERSION_MASK),
    LUAB_INT("_PW_KEYBYNAME",           _PW_KEYBYNAME),
    LUAB_INT("_PW_KEYBYNUM",            _PW_KEYBYNUM),
    LUAB_INT("_PW_KEYBYUID",            _PW_KEYBYUID),
    LUAB_INT("_PW_KEYYPENABLED",        _PW_KEYYPENABLED),
    LUAB_INT("_PW_KEYYPBYNUM",          _PW_KEYYPBYNUM),
    LUAB_STR("_PWD_VERSION_KEY",        _PWD_VERSION_KEY),
    LUAB_INT("_PWD_CURRENT_VERSION",    _PWD_CURRENT_VERSION),
    LUAB_INT("_PASSWORD_EFMT1",         _PASSWORD_EFMT1),
    LUAB_INT("_PASSWORD_LEN",           _PASSWORD_LEN),
    LUAB_INT("_PWF_NAME",               _PWF_NAME),
    LUAB_INT("_PWF_PASSWD",             _PWF_PASSWD),
    LUAB_INT("_PWF_UID",                _PWF_UID),
    LUAB_INT("_PWF_GID",                _PWF_GID),
    LUAB_INT("_PWF_CHANGE",             _PWF_CHANGE),
    LUAB_INT("_PWF_CLASS",              _PWF_CLASS),
    LUAB_INT("_PWF_GECOS",              _PWF_GECOS),
    LUAB_INT("_PWF_DIR",                _PWF_DIR),
    LUAB_INT("_PWF_SHELL",              _PWF_SHELL),
    LUAB_INT("_PWF_EXPIRE",             _PWF_EXPIRE),
    LUAB_INT("_PWF_SOURCE",             _PWF_SOURCE),
    LUAB_INT("_PWF_FILES",              _PWF_FILES),
    LUAB_INT("_PWF_NIS",                _PWF_NIS),
    LUAB_INT("_PWF_HESIOD",             _PWF_HESIOD),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_pwd_lib = {
    .m_cookie   = LUAB_PWD_LIB_ID,
    .m_name     = LUAB_PWD_LIB_KEY,
    .m_vec      = luab_pwd_vec,
};
