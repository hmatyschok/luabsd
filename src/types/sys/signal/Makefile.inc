
.PATH: ${LUAB_SRCTOP}/types/sys/signal

# reference data types
SRCS+=  luab_sigval_type.c
SRCS+=  luab_sigset_type.c

# composite data types
SRCS+=  luab_sigevent_type.c
SRCS+=  luab_siginfo_type.c
SRCS+=  luab_sigaction_type.c
