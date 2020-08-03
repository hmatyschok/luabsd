
.PATH:	${LUAB_SRCTOP}/modules

SRCS+=  luab_arpa_inet.c

SRCS+=	luab_net_if.c
SRCS+=	luab_net_if_dl.c

SRCS+=  luab_sys_file.c
SRCS+=  luab_sys_stat.c
SRCS+=  luab_sys_time.c
SRCS+=	luab_sys_uio.c
SRCS+=  luab_sys_unistd.c
SRCS+=	luab_sys_socket.c

SRCS+=  luab_db.c
SRCS+=  luab_fcntl.c
SRCS+=  luab_stdlib.c
SRCS+=  luab_time.c
SRCS+=  luab_unistd.c
SRCS+=  luab_uuid.c

LIBADD+= crypt

