
.PATH: ${LUAB_SRCTOP}/atomic

# integer types
SRCS+=  luab_ushrt_type.c
SRCS+=  luab_uint_type.c

SRCS+=  luab_char_type.c
SRCS+=  luab_short_type.c
SRCS+=  luab_int_type.c
SRCS+=  luab_long_type.c

# floating point number types
SRCS+=  luab_double_type.c
SRCS+=  luab_float_type.c

# POSIX sized integrals
SRCS+=  luab_int8_type.c
SRCS+=  luab_int16_type.c

# standard types
SRCS+=  luab_fpos_type.c
SRCS+=  luab_gid_type.c
SRCS+=  luab_off_type.c
SRCS+=  luab_size_type.c
SRCS+=  luab_socklen_type.c
SRCS+=  luab_ssize_type.c
SRCS+=  luab_uid_type.c

SRCS+=  luab_wchar_type.c
