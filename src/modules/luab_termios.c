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

/***
 * Generator function, creates an instance of (LUA_TUSERDATA(SPEED)).
 *
 * @function create_speed
 *
 * @param arg               Specifies initial value by an instance of
 *
 *                              (LUA_T{NIL,NUMBER,USERDATA(SPEED)).
 *
 * @return (LUA_T{NIL,USERDATA} [, LUA_T{NIL,NUMBER}, LUA_T{NIL,STRING} ])
 *
 * @usage speed [, err, msg ] = bsd.termios.create_speed(arg)
 */
static int
luab_type_create_speed(lua_State *L)
{
    luab_module_t *m;
    speed_t x;

    (void)luab_core_checkmaxargs(L, 1);

    m = luab_xmod(SPEED, TYPE, __func__);
    x = (speed_t)luab_checkxinteger(L, 1, m, luab_env_uint_max);
    return (luab_pushxdata(L, m, &x));
}

/*
 * Interface against <termios.h>.
 */

static luab_module_table_t luab_termios_vec[] = { /* termios.h */
    LUAB_INT("VEOF",                    VEOF),
    LUAB_INT("VEOL",                    VEOL),
#if __BSD_VISIBLE
    LUAB_INT("VEOL2",                   VEOL2),
#endif
    LUAB_INT("VERASE",                  VERASE),
#if __BSD_VISIBLE
    LUAB_INT("VWERASE",                 VWERASE),
#endif
    LUAB_INT("VKILL",                   VKILL),
#if __BSD_VISIBLE
    LUAB_INT("VREPRINT",                VREPRINT),
    LUAB_INT("VERASE2",                 VERASE2),
#endif
    LUAB_INT("VINTR",                   VINTR),
    LUAB_INT("VQUIT",                   VQUIT),
    LUAB_INT("VSUSP",                   VSUSP),
#if __BSD_VISIBLE
    LUAB_INT("VDSUSP",                  VDSUSP),
#endif
    LUAB_INT("VSTART",                  VSTART),
    LUAB_INT("VSTOP",                   VSTOP),
#if __BSD_VISIBLE
    LUAB_INT("VLNEXT",                  VLNEXT),
    LUAB_INT("VDISCARD",                VDISCARD),
#endif
    LUAB_INT("VMIN",                    VMIN),
    LUAB_INT("VTIME",                   VTIME),
#if __BSD_VISIBLE
    LUAB_INT("VSTATUS",                 VSTATUS),
#endif
    LUAB_INT("NCCS",                    NCCS),
    LUAB_INT("_POSIX_VDISABLE",         _POSIX_VDISABLE),
    LUAB_INT("IGNBRK",                  IGNBRK),
    LUAB_INT("BRKINT",                  BRKINT),
    LUAB_INT("IGNPAR",                  IGNPAR),
    LUAB_INT("PARMRK",                  PARMRK),
    LUAB_INT("INPCK",                   INPCK),
    LUAB_INT("ISTRIP",                  ISTRIP),
    LUAB_INT("INLCR",                   INLCR),
    LUAB_INT("IGNCR",                   IGNCR),
    LUAB_INT("ICRNL",                   ICRNL),
    LUAB_INT("IXON",                    IXON),
    LUAB_INT("IXOFF",                   IXOFF),
#if __XSI_VISIBLE || __POSIX_VISIBLE >= 200809
    LUAB_INT("IXANY",                   IXANY),
#endif
#if __BSD_VISIBLE
    LUAB_INT("IMAXBEL",                 IMAXBEL),
#endif
    LUAB_INT("OPOST",                   OPOST),
#if __XSI_VISIBLE
    LUAB_INT("ONLCR",                   ONLCR),
#endif
#if __BSD_VISIBLE
    LUAB_INT("TABDLY",                  TABDLY),
    LUAB_INT("TAB0",                    TAB0),
    LUAB_INT("TAB3",                    TAB3),
    LUAB_INT("ONOEOT",                  ONOEOT),
#endif
#if __XSI_VISIBLE
    LUAB_INT("OCRNL",                   OCRNL),
    LUAB_INT("ONOCR",                   ONOCR),
    LUAB_INT("ONLRET",                  ONLRET),
#endif
#if __BSD_VISIBLE
    LUAB_INT("CIGNORE",                 CIGNORE),
#endif
    LUAB_INT("CSIZE",                   CSIZE),
    LUAB_INT("CS5",                     CS5),
    LUAB_INT("CS6",                     CS6),
    LUAB_INT("CS7",                     CS7),
    LUAB_INT("CS8",                     CS8),
    LUAB_INT("CSTOPB",                  CSTOPB),
    LUAB_INT("CREAD",                   CREAD),
    LUAB_INT("PARENB",                  PARENB),
    LUAB_INT("PARODD",                  PARODD),
    LUAB_INT("HUPCL",                   HUPCL),
    LUAB_INT("CLOCAL",                  CLOCAL),
#if __BSD_VISIBLE
    LUAB_INT("CCTS_OFLOW",              CCTS_OFLOW),
    LUAB_INT("CRTSCTS",                 CRTSCTS),
    LUAB_INT("CRTS_IFLOW",              CRTS_IFLOW),
    LUAB_INT("CDTR_IFLOW",              CDTR_IFLOW),
    LUAB_INT("CDSR_OFLOW",              CDSR_OFLOW),
    LUAB_INT("CCAR_OFLOW",              CCAR_OFLOW),
#endif
#if __BSD_VISIBLE
    LUAB_INT("ECHOKE",                  ECHOKE),
#endif
    LUAB_INT("ECHOE",                   ECHOE),
    LUAB_INT("ECHOK",                   ECHOK),
    LUAB_INT("ECHO",                    ECHO),
    LUAB_INT("ECHONL",                  ECHONL),
#if __BSD_VISIBLE
    LUAB_INT("ECHOPRT",                 ECHOPRT),
    LUAB_INT("ECHOCTL",                 ECHOCTL),
#endif
    LUAB_INT("ISIG",                    ISIG),
    LUAB_INT("ICANON",                  ICANON),
#if __BSD_VISIBLE
    LUAB_INT("ALTWERASE",               ALTWERASE),
#endif
    LUAB_INT("IEXTEN",                  IEXTEN),
    LUAB_INT("EXTPROC",                 EXTPROC),
    LUAB_INT("TOSTOP",                  TOSTOP),
#if __BSD_VISIBLE
    LUAB_INT("FLUSHO",                  FLUSHO),
    LUAB_INT("NOKERNINFO",              NOKERNINFO),
    LUAB_INT("PENDIN",                  PENDIN),
#endif
    LUAB_INT("NOFLSH",                  NOFLSH),
    LUAB_INT("B0",                      B0),
    LUAB_INT("B50",                     B50),
    LUAB_INT("B75",                     B75),
    LUAB_INT("B110",                    B110),
    LUAB_INT("B134",                    B134),
    LUAB_INT("B150",                    B150),
    LUAB_INT("B200",                    B200),
    LUAB_INT("B300",                    B300),
    LUAB_INT("B600",                    B600),
    LUAB_INT("B1200",                   B1200),
    LUAB_INT("B1800",                   B1800),
    LUAB_INT("B2400",                   B2400),
    LUAB_INT("B4800",                   B4800),
    LUAB_INT("B9600",                   B9600),
    LUAB_INT("B19200",                  B19200),
    LUAB_INT("B38400",                  B38400),
#if __BSD_VISIBLE
    LUAB_INT("B7200",                   B7200),
    LUAB_INT("B14400",                  B14400),
    LUAB_INT("B28800",                  B28800),
    LUAB_INT("B57600",                  B57600),
    LUAB_INT("B76800",                  B76800),
    LUAB_INT("B115200",                 B115200),
    LUAB_INT("B230400",                 B230400),
    LUAB_INT("B460800",                 B460800),
    LUAB_INT("B921600",                 B921600),
    LUAB_INT("EXTA",                    EXTA),
    LUAB_INT("EXTB",                    EXTB),
    LUAB_INT("OXTABS",                  OXTABS),
    LUAB_INT("MDMBUF",                  MDMBUF),
#endif
    LUAB_INT("TCSANOW",                 TCSANOW),
    LUAB_INT("TCSADRAIN",               TCSADRAIN),
    LUAB_INT("TCSAFLUSH",               TCSAFLUSH),
#if __BSD_VISIBLE
    LUAB_INT("TCSASOFT",                TCSASOFT),
#endif
    LUAB_INT("TCIFLUSH",                TCIFLUSH),
    LUAB_INT("TCOFLUSH",                TCOFLUSH),
    LUAB_INT("TCIOFLUSH",               TCIOFLUSH),
    LUAB_INT("TCOOFF",                  TCOOFF),
    LUAB_INT("TCOON",                   TCOON),
    LUAB_INT("TCIOFF",                  TCIOFF),
    LUAB_INT("TCION",                   TCION),
    LUAB_FUNC("create_speed",           luab_type_create_speed),
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_termios_lib = {
    .m_id       = LUAB_TERMIOS_LIB_ID,
    .m_name     = LUAB_TERMIOS_LIB_KEY,
    .m_vec      = luab_termios_vec,
};
