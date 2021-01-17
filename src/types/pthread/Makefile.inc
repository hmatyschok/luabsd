
.PATH:  ${LUAB_SRCTOP}/types/pthread

# atomic data types
SRCS+=  luab_pthread_key_type.c

# reference data types
SRCS+=  luab_pthread_type.c
SRCS+=  luab_pthread_attr_type.c
SRCS+=  luab_pthread_mutex_type.c
SRCS+=  luab_pthread_mutexattr_type.c
SRCS+=  luab_pthread_cond_type.c
SRCS+=  luab_pthread_condattr_type.c
SRCS+=  luab_pthread_rwlock_type.c
SRCS+=  luab_pthread_rwlockattr_type.c
SRCS+=  luab_pthread_barrier_type.c
SRCS+=  luab_pthread_barrierattr_type.c

# composite data types
SRCS+=  luab_pthread_once_type.c

