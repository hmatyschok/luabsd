
.PATH: ${LUAB_SRCTOP}/types/sys/signal

# reference data types
SRCS+=  luab_sig_type.c
SRCS+=  luab_sigval_type.c
SRCS+=  luab_sigset_type.c

# composite data types
SRCS+=  luab_sigevent_type.c
SRCS+=  luab_siginfo_type.c
SRCS+=  luab_sigaction_type.c
SRCS+=  luab_stack_type.c
SRCS+=  luab_sigstack_type.c
SRCS+=  luab_sigvec_type.c
