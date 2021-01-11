
.PATH: ${LUAB_SRCTOP}/types/core

# integer types
SRCS+=  luab_uchar_type.c
SRCS+=  luab_ushrt_type.c
SRCS+=  luab_uint_type.c
SRCS+=  luab_ulong_type.c

SRCS+=  luab_char_type.c
SRCS+=  luab_short_type.c
SRCS+=  luab_int_type.c
SRCS+=  luab_long_type.c

# floating point number types
SRCS+=  luab_double_type.c
SRCS+=  luab_float_type.c

# standard types
SRCS+=  luab_off_type.c
SRCS+=  luab_size_type.c
SRCS+=  luab_socklen_type.c
SRCS+=  luab_ssize_type.c
SRCS+=  luab_uid_type.c

SRCS+=  luab_wchar_type.c
SRCS+=  luab_vm_offset_type.c

SRCS+=  luab_lual_integer_type.c
SRCS+=  luab_lual_number_type.c

# composite data types
SRCS+=  luab_integer_type.c
