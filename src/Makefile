
PACKAGE=lib${LIB}
LIB=    luabsd
SHLIB_MAJOR= 0

SRCS=   luab_core.c

SRCS+=  luab_sys_file.c
SRCS+=  luab_sys_stat.c
SRCS+=  luab_sys_time.c
SRCS+=  luab_sys_unistd.c

SRCS+=  luab_db.c
SRCS+=  luab_fcntl.c
SRCS+=  luab_stdlib.c
SRCS+=  luab_time.c
SRCS+=  luab_unistd.c
SRCS+=  luab_uuid.c

INCS=   luabsd.h
MAN=

CFLAGS+= -I${.CURDIR} -I/usr/local/include/lua52

.include <bsd.lib.mk>
