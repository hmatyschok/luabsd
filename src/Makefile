
PACKAGE=lib${LIB}
SHLIBDIR?=  /lib

.include <src.opts.mk>

LUAB_SRCTOP?= ${.CURDIR}

LIB=    luabsd
SHLIB_MAJOR= 0

.include "${LUAB_SRCTOP}/types/Makefile.inc"
.include "${LUAB_SRCTOP}/modules/Makefile.inc"
.include "${LUAB_SRCTOP}/core/Makefile.inc"

LIBADD+= crypt pthread rt

MAN=

CFLAGS+= -I${LUAB_SRCTOP}/include -I/usr/local/include/lua52

.include <bsd.lib.mk>
