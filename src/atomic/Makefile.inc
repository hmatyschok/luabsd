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

.include "${LUAB_SRCTOP}/atomic/core/Makefile.inc"

.include "${LUAB_SRCTOP}/atomic/arpa/Makefile.inc"
.include "${LUAB_SRCTOP}/atomic/sys/Makefile.inc"

.include "${LUAB_SRCTOP}/atomic/ctype/Makefile.inc"
.include "${LUAB_SRCTOP}/atomic/grp/Makefile.inc"
.include "${LUAB_SRCTOP}/atomic/fcntl/Makefile.inc"
.include "${LUAB_SRCTOP}/atomic/langinfo/Makefile.inc"
.include "${LUAB_SRCTOP}/atomic/stdio/Makefile.inc"
.include "${LUAB_SRCTOP}/atomic/unistd/Makefile.inc"
