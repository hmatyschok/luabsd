
PACKAGE=lib${LIB}
LIB=	luabsd
SHLIB_MAJOR= 0

SRCS=	luabsd.c
INCS=
MAN=

CFLAGS+= -I${.CURDIR} -I/usr/local/include/lua52

.include <bsd.lib.mk>
