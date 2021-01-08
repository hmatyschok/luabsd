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

.include "${LUAB_SRCTOP}/types/core/Makefile.inc"

.include "${LUAB_SRCTOP}/types/arpa/Makefile.inc"
.include "${LUAB_SRCTOP}/types/net/Makefile.inc"
.include "${LUAB_SRCTOP}/types/sys/Makefile.inc"

.include "${LUAB_SRCTOP}/types/ctype/Makefile.inc"
.include "${LUAB_SRCTOP}/types/db/Makefile.inc"
.include "${LUAB_SRCTOP}/types/dirent/Makefile.inc"
.include "${LUAB_SRCTOP}/types/fcntl/Makefile.inc"
.include "${LUAB_SRCTOP}/types/fstab/Makefile.inc"
.include "${LUAB_SRCTOP}/types/grp/Makefile.inc"
.include "${LUAB_SRCTOP}/types/langinfo/Makefile.inc"
.include "${LUAB_SRCTOP}/types/pwd/Makefile.inc"
.include "${LUAB_SRCTOP}/types/regex/Makefile.inc"
.include "${LUAB_SRCTOP}/types/stdio/Makefile.inc"
.include "${LUAB_SRCTOP}/types/stdlib/Makefile.inc"
.include "${LUAB_SRCTOP}/types/time/Makefile.inc"
.include "${LUAB_SRCTOP}/types/ttyent/Makefile.inc"
.include "${LUAB_SRCTOP}/types/unistd/Makefile.inc"
.include "${LUAB_SRCTOP}/types/uuid/Makefile.inc"
