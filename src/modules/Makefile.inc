
.PATH:  ${LUAB_SRCTOP}/modules

.include "arpa/Makefile.inc"
.include "net/Makefile.inc"
.include "sys/Makefile.inc"
.include "xlocale/Makefile.inc"

SRCS+=  luab_cpio.c
SRCS+=  luab_ctype.c
SRCS+=  luab_db.c
SRCS+=  luab_dirent.c
SRCS+=  luab_fcntl.c
SRCS+=  luab_fstab.c
SRCS+=  luab_grp.c
SRCS+=  luab_langinfo.c
SRCS+=  luab_locale.c
SRCS+=  luab_pthread.c
SRCS+=  luab_pwd.c
SRCS+=  luab_regex.c
SRCS+=  luab_signal.c
SRCS+=  luab_stdio.c
SRCS+=  luab_stdlib.c
SRCS+=  luab_termios.c
SRCS+=  luab_time.c
SRCS+=  luab_ttyent.c
SRCS+=  luab_unistd.c
SRCS+=  luab_uuid.c
