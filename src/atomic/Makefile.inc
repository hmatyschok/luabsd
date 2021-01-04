#
# XXX
#   We shall implement a so called directory
#   structure for components, e. g. over
#
#       t : { T }
#
#   by
#
#       T = { atomic, composite }
#
#   its type, realm and/or domain, etc.
#

.PATH: ${LUAB_SRCTOP}/atomic

# integer types
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
SRCS+=  luab_time_type.c
SRCS+=  luab_clock_type.c
SRCS+=  luab_locale_type.c

SRCS+=  luab_vm_offset_type.c

# <arpa/inet.h>
SRCS+=  luab_in_port_type.c

# <sys/stat.h>
SRCS+=  luab_blkcnt_type.c
SRCS+=  luab_dev_type.c
SRSC+=  luab_ino_type.c
SRCS+=  luab_nlink_type.c

# POSIX sized integrals, <sys/stdint.h>
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

# <ctype.h>
SRCS+=  luab_ct_rune_type.c

# <grp.h>
SRCS+=  luab_gid_type.c

# <fcntl.h>
SRCS+=  luab_mode_type.c

# <langinfo.h>
SRCS+=  luab_nl_item_type.c

# <stdio.h>
SRCS+=  luab_fpos_type.c
SRCS+=  luab_rsize_type.c

# <unistd.h>
SRCS+=  luab_pid_type.c
SRCS+=  luab_useconds_type.c
