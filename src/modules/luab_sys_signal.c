/*
 * Copyright (c) 2020, 2021 Henning Matyschok
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

#include <sys/signal.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luabsd.h"

#define LUAB_SYS_SIGNAL_LIB_ID    1610381740
#define LUAB_SYS_SIGNAL_LIB_KEY   "signal"

extern luab_module_t luab_sys_signal_lib;

/*
 * Interface against <sys/signal.h>
 */

static luab_module_table_t luab_sys_signal_vec[] = {
#if __POSIX_VISIBLE || __XSI_VISIBLE
    LUAB_INT("SIGHUP",                  SIGHUP),
#endif
    LUAB_INT("SIGINT",                  SIGINT),
#if __POSIX_VISIBLE || __XSI_VISIBLE
    LUAB_INT("SIGQUIT",                 SIGQUIT),
#endif
    LUAB_INT("SIGILL",                  SIGILL),
#if __XSI_VISIBLE
    LUAB_INT("SIGTRAP",                 SIGTRAP),
#endif
    LUAB_INT("SIGABRT",                 SIGABRT),
#if __BSD_VISIBLE
    LUAB_INT("SIGIOT",                  SIGIOT),
    LUAB_INT("SIGEMT",                  SIGEMT),
#endif
    LUAB_INT("SIGFPE",                  SIGFPE),
#if __POSIX_VISIBLE || __XSI_VISIBLE
    LUAB_INT("SIGKILL",                 SIGKILL),
#endif
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
    LUAB_INT("SIGBUS",                  SIGBUS),
#endif
    LUAB_INT("SIGSEGV",                 SIGSEGV),
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
    LUAB_INT("SIGSYS",                  SIGSYS),
#endif
#if __POSIX_VISIBLE || __XSI_VISIBLE
    LUAB_INT("SIGPIPE",                 SIGPIPE),
    LUAB_INT("SIGALRM",                 SIGALRM),
#endif
    LUAB_INT("SIGTERM",                 SIGTERM),
#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE
    LUAB_INT("SIGURG",                  SIGURG),
#endif
#if __POSIX_VISIBLE || __XSI_VISIBLE
    LUAB_INT("SIGSTOP",                 SIGSTOP),
    LUAB_INT("SIGTSTP",                 SIGTSTP),
    LUAB_INT("SIGCONT",                 SIGCONT),
    LUAB_INT("SIGCHLD",                 SIGCHLD),
    LUAB_INT("SIGTTIN",                 SIGTTIN),
    LUAB_INT("SIGTTOU",                 SIGTTOU),
#endif
#if __BSD_VISIBLE
    LUAB_INT("SIGIO",                   SIGIO),
#endif
#if __XSI_VISIBLE
    LUAB_INT("SIGXCPU",                 SIGXCPU),
    LUAB_INT("SIGXFSZ",                 SIGXFSZ),
    LUAB_INT("SIGVTALRM",               SIGVTALRM),
    LUAB_INT("SIGPROF",                 SIGPROF),
#endif
#if __BSD_VISIBLE
    LUAB_INT("SIGWINCH",                SIGWINCH),
    LUAB_INT("SIGINFO",                 SIGINFO),
#endif
#if __POSIX_VISIBLE || __XSI_VISIBLE
    LUAB_INT("SIGUSR1",                 SIGUSR1),
    LUAB_INT("SIGUSR2",                 SIGUSR2),
#endif
#if __BSD_VISIBLE
    LUAB_INT("SIGTHR",                  SIGTHR),
    LUAB_INT("SIGLWP",                  SIGLWP),
    LUAB_INT("SIGLIBRT",                SIGLIBRT),
#endif
    LUAB_INT("SIGRTMIN",                SIGRTMIN),
    LUAB_INT("SIGRTMAX",                SIGRTMAX),
#if __POSIX_VISIBLE >= 199309
    LUAB_INT("SIGEV_NONE",              SIGEV_NONE),
    LUAB_INT("SIGEV_SIGNAL",            SIGEV_SIGNAL),
    LUAB_INT("SIGEV_THREAD",            SIGEV_THREAD),
#if __BSD_VISIBLE
    LUAB_INT("SIGEV_KEVENT",            SIGEV_KEVENT),
    LUAB_INT("SIGEV_THREAD_ID",         SIGEV_THREAD_ID),
#endif
#endif  /* __POSIX_VISIBLE >= 199309 */
#if __POSIX_VISIBLE >= 199309 || __XSI_VISIBLE
    LUAB_INT("ILL_ILLOPC",              ILL_ILLOPC),
    LUAB_INT("ILL_ILLOPN",              ILL_ILLOPN),
    LUAB_INT("ILL_ILLADR",              ILL_ILLADR),
    LUAB_INT("ILL_ILLTRP",              ILL_ILLTRP),
    LUAB_INT("ILL_PRVOPC",              ILL_PRVOPC),
    LUAB_INT("ILL_PRVREG",              ILL_PRVREG),
    LUAB_INT("ILL_COPROC",              ILL_COPROC),
    LUAB_INT("ILL_BADSTK",              ILL_BADSTK),
    LUAB_INT("BUS_ADRALN",              BUS_ADRALN),
    LUAB_INT("BUS_ADRERR",              BUS_ADRERR),
    LUAB_INT("BUS_OBJERR",              BUS_OBJERR),
    LUAB_INT("SEGV_MAPERR",             SEGV_MAPERR),
    LUAB_INT("SEGV_ACCERR",             SEGV_ACCERR),
    LUAB_INT("FPE_INTOVF",              FPE_INTOVF),
    LUAB_INT("FPE_INTDIV",              FPE_INTDIV),
    LUAB_INT("FPE_FLTDIV",              FPE_FLTDIV),
    LUAB_INT("FPE_FLTOVF",              FPE_FLTOVF),
    LUAB_INT("FPE_FLTUND",              FPE_FLTUND),
    LUAB_INT("FPE_FLTRES",              FPE_FLTRES),
    LUAB_INT("FPE_FLTINV",              FPE_FLTINV),
    LUAB_INT("FPE_FLTSUB",              FPE_FLTSUB),
    LUAB_INT("TRAP_BRKPT",              TRAP_BRKPT),
    LUAB_INT("TRAP_TRACE",              TRAP_TRACE),
    LUAB_INT("TRAP_DTRACE",             TRAP_DTRACE),
    LUAB_INT("TRAP_CAP",                TRAP_CAP),
    LUAB_INT("CLD_EXITED",              CLD_EXITED),
    LUAB_INT("CLD_KILLED",              CLD_KILLED),
    LUAB_INT("CLD_DUMPED",              CLD_DUMPED),
    LUAB_INT("CLD_TRAPPED",             CLD_TRAPPED),
    LUAB_INT("CLD_STOPPED",             CLD_STOPPED),
    LUAB_INT("CLD_CONTINUED",           CLD_CONTINUED),
    LUAB_INT("POLL_IN",                 POLL_IN),
    LUAB_INT("POLL_OUT",                POLL_OUT),
    LUAB_INT("POLL_MSG",                POLL_MSG),
    LUAB_INT("POLL_ERR",                POLL_ERR),
    LUAB_INT("POLL_PRI",                POLL_PRI),
    LUAB_INT("POLL_HUP",                POLL_HUP),
#endif /* __POSIX_VISIBLE >= 199309 || __XSI_VISIBLE */
#if __POSIX_VISIBLE || __XSI_VISIBLE
    LUAB_INT("SA_NOCLDSTOP",            SA_NOCLDSTOP),
#endif
#if __XSI_VISIBLE
    LUAB_INT("SA_ONSTACK",              SA_ONSTACK),
    LUAB_INT("SA_RESTART",              SA_RESTART),
    LUAB_INT("SA_RESETHAND",            SA_RESETHAND),
    LUAB_INT("SA_NODEFER",              SA_NODEFER),
    LUAB_INT("SA_NOCLDWAIT",            SA_NOCLDWAIT),
    LUAB_INT("SA_SIGINFO",              SA_SIGINFO),
#endif
#if __BSD_VISIBLE
    LUAB_INT("NSIG",                    NSIG),
#endif
#if __POSIX_VISIBLE || __XSI_VISIBLE
    LUAB_INT("SI_NOINFO",               SI_NOINFO),
    LUAB_INT("SI_USER",                 SI_USER),
    LUAB_INT("SI_QUEUE",                SI_QUEUE),
    LUAB_INT("SI_TIMER",                SI_TIMER),
    LUAB_INT("SI_ASYNCIO",              SI_ASYNCIO),
    LUAB_INT("SI_MESGQ",                SI_MESGQ),
    LUAB_INT("SI_KERNEL",               SI_KERNEL),
    LUAB_INT("SI_LWP",                  SI_LWP),
#endif
#if __BSD_VISIBLE
    LUAB_INT("SI_UNDEFINED",            SI_UNDEFINED),
#endif
#if __XSI_VISIBLE
    LUAB_INT("SS_ONSTACK",              SS_ONSTACK),
    LUAB_INT("SS_DISABLE",              SS_DISABLE),
    LUAB_INT("MINSIGSTKSZ",             MINSIGSTKSZ),
    LUAB_INT("SIGSTKSZ",                SIGSTKSZ),
#endif
#if __BSD_VISIBLE
    LUAB_INT("SV_ONSTACK",              SV_ONSTACK),
    LUAB_INT("SV_INTERRUPT",            SV_INTERRUPT),
    LUAB_INT("SV_RESETHAND",            SV_RESETHAND),
    LUAB_INT("SV_NODEFER",              SV_NODEFER),
    LUAB_INT("SV_NOCLDSTOP",            SV_NOCLDSTOP),
    LUAB_INT("SV_SIGINFO",              SV_SIGINFO),
#endif
#if __POSIX_VISIBLE || __XSI_VISIBLE
    LUAB_INT("SIG_BLOCK",               SIG_BLOCK),
    LUAB_INT("SIG_UNBLOCK",             SIG_UNBLOCK),
    LUAB_INT("SIG_SETMASK",             SIG_SETMASK),
#endif
    LUAB_MOD_TBL_SENTINEL
};

luab_module_t luab_sys_signal_lib = {
    .m_id       = LUAB_SYS_SIGNAL_LIB_ID,
    .m_name     = LUAB_SYS_SIGNAL_LIB_KEY,
    .m_vec      = luab_sys_signal_vec,
};
