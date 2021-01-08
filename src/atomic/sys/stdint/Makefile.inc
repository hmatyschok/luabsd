#
# POSIX sized integrals, <sys/stdint.h>
#

.PATH: ${LUAB_SRCTOP}/atomic/sys/stdint

SRCS+=  luab_int8_type.c
SRCS+=  luab_int16_type.c
SRCS+=  luab_int32_type.c
SRCS+=  luab_int64_type.c

SRCS+=  luab_uint8_type.c
SRCS+=  luab_uint16_type.c
SRCS+=  luab_uint32_type.c
SRCS+=  luab_uint64_type.c

SRCS+=  luab_intptr_type.c
SRCS+=  luab_uintptr_type.c
SRCS+=  luab_intmax_type.c
SRCS+=  luab_uintmax_type.c
