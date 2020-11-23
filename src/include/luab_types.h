/*-
 * Copyright (c) 2020 Henning Matyschok
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LUAB_TYPES_H_
#define _LUAB_TYPES_H_

/*
 * Set of atomic data types.
 */

#define LUAB_CHAR_TYPE_ID                   1605964271
#define LUAB_CHAR_TYPE                      "CHAR*"

#define LUAB_DOUBLE_TYPE_ID                 1605894356
#define LUAB_DOUBLE_TYPE                    "DOUBLE*"

#define LUAB_FLOAT_TYPE_ID                  1605908147
#define LUAB_FLOAT_TYPE                     "FLOAT*"

#define LUAB_FPOS_TYPE_ID                   1605960373
#define LUAB_FPOS_TYPE                      "FPOS*"

#define LUAB_GID_TYPE_ID                    1605908910
#define LUAB_GID_TYPE                       "GID*"

#define LUAB_INT16_TYPE_ID                  1606002046
#define LUAB_INT16_TYPE                     "INT16*"

#define LUAB_INT32_TYPE_ID                  1606004102
#define LUAB_INT32_TYPE                     "INT32*"

#define LUAB_INT64_TYPE_ID                  1606004507
#define LUAB_INT64_TYPE                     "INT64*"

#define LUAB_INT8_TYPE_ID                   1606001349
#define LUAB_INT8_TYPE                      "INT8*"

#define LUAB_INT_TYPE_ID                    1605877856
#define LUAB_INT_TYPE                       "INT*"

#define LUAB_LONG_TYPE_ID                   1605965914
#define LUAB_LONG_TYPE                      "LONG*"

#define LUAB_OFF_TYPE_ID                    1605914553
#define LUAB_OFF_TYPE                       "OFF*"

#define LUAB_SHORT_TYPE_ID                  1605997500
#define LUAB_SHORT_TYPE                     "SHORT*"

#define LUAB_SIZE_TYPE_ID                   1605967268
#define LUAB_SIZE_TYPE                      "SIZE*"

#define LUAB_SOCKLEN_TYPE_ID                1605960373
#define LUAB_SOCKLEN_TYPE                   "SOCKLEN*"

#define LUAB_SSIZE_TYPE_ID                  1605991182
#define LUAB_SSIZE_TYPE                     "SSIZE*"

#define LUAB_UID_TYPE_ID                    1605913920
#define LUAB_UID_TYPE                       "UID*"

#define LUAB_UINT_TYPE_ID                   1605965347
#define LUAB_UINT_TYPE                      "UINT*"

#define LUAB_USHRT_TYPE_ID                  1605911568
#define LUAB_USHRT_TYPE                     "USHRT*"

#define LUAB_WCHAR_TYPE_ID                  1605963183
#define LUAB_WCHAR_TYPE                     "WCHAR*"

/*
 * Set of composite data types.
 */

#define LUAB___SBUF_TYPE_ID                 1605302940
#define LUAB___SBUF_TYPE                    "__SBUF*"

#define LUAB_ACCEPT_FILTER_ARG_TYPE_ID      1601330449
#define LUAB_ACCEPT_FILTER_ARG_TYPE         "ACCEPT_FILTER_ARG*"

#define LUAB_BINTIME_TYPE_ID                1594161740
#define LUAB_BINTIME_TYPE                   "BINTIME*"

#define LUAB_CAP_RBUF_TYPE_ID               1601143331
#define LUAB_CAP_RBUF_TYPE                  "CAP_RBUF*"

#define LUAB_CLOCKINFO_TYPE_ID              1594164272
#define LUAB_CLOCKINFO_TYPE                 "CLOCKINFO*"

#define LUAB_CMSGCRED_TYPE_ID               1601557495
#define LUAB_CMSGCRED_TYPE                  "CMSGCRED*"

#define LUAB_CRYPT_DATA_TYPE_ID             1595491033
#define LUAB_CRYPT_DATA_TYPE                "CRYPT_DATA*"

#define LUAB_DB_TYPE_ID                     1596201370
#define LUAB_DB_TYPE                        "DB*"

#define LUAB_DBT_TYPE_ID                    1596025036
#define LUAB_DBT_TYPE                       "DBT*"

#define LUAB_DIR_TYPE_ID                    1604794619
#define LUAB_DIR_TYPE                       "DIR*"

#define LUAB_DIRENT_TYPE_ID                 1604795865
#define LUAB_DIRENT_TYPE                    "DIRENT*"

#define LUAB_DIV_TYPE_ID                    1600388163
#define LUAB_DIV_TYPE                       "DIV*"

#define LUAB_FHANDLE_TYPE_ID                1604445207
#define LUAB_FHANDLE_TYPE                   "FHANDLE*"

#define LUAB_FID_TYPE_ID                    1604423994
#define LUAB_FID_TYPE                       "FID*"

#define LUAB_FLOCK_TYPE_ID                  1593623399
#define LUAB_FLOCK_TYPE                     "FLOCK*"

#define LUAB_FSID_TYPE_ID                   1604426384
#define LUAB_FSID_TYPE                      "FSID*"

#define LUAB_FSTAB_TYPE_ID                  1604964419
#define LUAB_FSTAB_TYPE                     "FSTAB*"

#define LUAB_GROUP_TYPE_ID                  1604324396
#define LUAB_GROUP_TYPE                     "GROUP*"

#define LUAB_IF_NAMEINDEX_TYPE_ID           1596840702
#define LUAB_IF_NAMEINDEX_TYPE              "IF_NAMEINDEX*"

#define LUAB_IN6_ADDR_TYPE_ID               1595890830
#define LUAB_IN6_ADDR_TYPE                  "IN6_ADDR*"

#define LUAB_IN_ADDR_TYPE_ID                1595799233
#define LUAB_IN_ADDR_TYPE                   "IN_ADDR*"

#define LUAB_INTEGER_TYPE_ID                1595975665
#define LUAB_INTEGER_TYPE                   "INTEGER*"

#define LUAB_IOVEC_TYPE_ID                  1594559731
#define LUAB_IOVEC_TYPE                     "IOVEC*"

#define LUAB_ITIMERVAL_TYPE_ID              1594110231
#define LUAB_ITIMERVAL_TYPE                 "ITIMERVAL*"

#define LUAB_JAIL_TYPE_ID                   1606011567
#define LUAB_JAIL_TYPE                      "JAIL*"

#define LUAB_LDIV_TYPE_ID                   1600394297
#define LUAB_LDIV_TYPE                      "LDIV*"

#define LUAB_LINGER_TYPE_ID                 1597012436
#define LUAB_LINGER_TYPE                    "LINGER*"

#define LUAB_LINK_TYPE_ID                   1601843279
#define LUAB_LINK_TYPE                      "LINK*"

#define LUAB_LLDIV_TYPE_ID                  1600628549
#define LUAB_LLDIV_TYPE                     "LLDIV*"

#define LUAB_MSGHDR_TYPE_ID                 1597320239
#define LUAB_MSGHDR_TYPE                    "MSGHDR*"

#define LUAB_PASSWD_TYPE_ID                 1604262109
#define LUAB_PASSWD_TYPE                    "PASSWD*"

#define LUAB_REGEX_TYPE_ID                  1604367435
#define LUAB_REGEX_TYPE                     "REGEX*"

#define LUAB_REGMATCH_TYPE_ID               1604371586
#define LUAB_REGMATCH_TYPE                  "REGMATCH*"

#define LUAB_SF_HDTR_TYPE_ID                1602287699
#define LUAB_SF_HDTR_TYPE                   "SF_HDTR*"

#define LUAB_SFILE_TYPE_ID                  1605120444
#define LUAB_SFILE_TYPE                     "SFILE*"

#define LUAB_SOCKADDR_TYPE_ID               1595755513
#define LUAB_SOCKADDR_TYPE                  "SOCKADDR*"

#define LUAB_SOCKPROTO_TYPE_ID              1601383780
#define LUAB_SOCKPROTO_TYPE                 "SOCKPROTO*"

#define LUAB_STAT_TYPE_ID                   1594028586
#define LUAB_STAT_TYPE                      "STAT*"

#define LUAB_STATFS_TYPE_ID                 1604429614
#define LUAB_STATFS_TYPE                    "STATFS*"

#define LUAB_TIMESPEC_TYPE_ID               1594034844
#define LUAB_TIMESPEC_TYPE                  "TIMESPEC*"

#define LUAB_TIMEVAL_TYPE_ID                1599788349
#define LUAB_TIMEVAL_TYPE                   "TIMEVAL*"

#define LUAB_TIMEZONE_TYPE_ID               1594159943
#define LUAB_TIMEZONE_TYPE                  "TIMEZONE*"

#define LUAB_TM_TYPE_ID                     1594168426
#define LUAB_TM_TYPE                        "TM*"

#define LUAB_TTYENT_TYPE_ID                 1604759728
#define LUAB_TTYENT_TYPE                    "TTYENT*"

#define LUAB_UUID_TYPE_ID                   1599304529
#define LUAB_UUID_TYPE                      "UUID*"

#define LUAB_XVFSCONF_TYPE_ID               1604736441
#define LUAB_XVFSCONF_TYPE                  "XVFSCONF*"

#endif /* _LUAB_TYPES_H_ */
