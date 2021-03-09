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

#include <termios.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"
#include "luab_udata.h"

#define LUAB_TERMIOS_LIB_ID    1615327084
#define LUAB_TERMIOS_LIB_KEY    "termios"

extern luab_module_t luab_termios_lib;

/*
 * Service primitives.
 */

/*
 * Generator functions.
 */

/*
 * Interface against <termios.h>.
 */

static luab_module_table_t luab_termios_vec[] = { /* termios.h */
#if __BSD_VISIBLE
    LUAB_INT("OXTABS",				OXTABS),
    LUAB_INT("MDMBUF",				MDMBUF),
#endif
    LUAB_INT("TCSANOW",				TCSANOW),
    LUAB_INT("TCSADRAIN",			TCSADRAIN),
    LUAB_INT("TCSAFLUSH",			TCSAFLUSH),
#if __BSD_VISIBLE
    LUAB_INT("TCSASOFT",			TCSASOFT),
#endif
    LUAB_INT("TCIFLUSH",			TCIFLUSH),
    LUAB_INT("TCOFLUSH",			TCOFLUSH),
    LUAB_INT("TCIOFLUSH",			TCIOFLUSH),
    LUAB_INT("TCOOFF",				TCOOFF),
    LUAB_INT("TCOON",				TCOON),
    LUAB_INT("TCIOFF",				TCIOFF),
    LUAB_INT("TCION",				TCION),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_termios_lib = {
    .m_id       = LUAB_TERMIOS_LIB_ID,
    .m_name     = LUAB_TERMIOS_LIB_KEY,
    .m_vec      = luab_termios_vec,
};
