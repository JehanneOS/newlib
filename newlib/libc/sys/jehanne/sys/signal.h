/*
 * This file is part of Jehanne.
 *
 * Copyright (C) 2017 Giacomo Tesio <giacomo@tesio.it>
 *
 * Jehanne is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * Jehanne is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Jehanne.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Based upon newlib's sys/signal.h and
 * 	http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/signal.h.html
 */
#ifndef _SYS_SIGNAL_H
#define _SYS_SIGNAL_H
#ifdef __cplusplus
extern "C" {
#endif

#include "_ansi.h"
#include <sys/cdefs.h>
#include <sys/features.h>
#include <sys/types.h>
#include <sys/_timespec.h>
#include <sys/_pthreadtypes.h>

#define _LIBPOSIX_H
#include <posix.h>	/* get libposix defines from posix.h */
#undef _LIBPOSIX_H

#define	_SIGSET_T_DECLARED
typedef	PosixSignalMask	sigset_t;

/* sigev_notify values
   NOTE: P1003.1c/D10, p. 34 adds SIGEV_THREAD.  */

#define SIGEV_NONE   1  /* No asynchronous notification shall be delivered */
                        /*   when the event of interest occurs. */
#define SIGEV_SIGNAL 2  /* A queued signal, with an application defined */
                        /*  value, shall be delivered when the event of */
                        /*  interest occurs. */
#define SIGEV_THREAD 3  /* A notification function shall be called to */
                        /*   perform notification. */

/* Signal Actions, P1003.1b-1993, p. 64 */
/* si_code values, p. 66 */

#define SEGV_MAPERR	PosixSIFaultMapError
#define SEGV_ACCERR	PosixSIFaultAccessError

#define CLD_EXITED	PosixSIChildExited	/* On SIGCHLD */
#define CLD_KILLED	PosixSIChildKilled
#define CLD_DUMPED	PosixSIChildKilled
#define CLD_TRAPPED	PosixSIChildTrapped
#define CLD_STOPPED	PosixSIChildStopped
#define CLD_CONTINUED	PosixSIChildContinued

#define SI_USER    PosixSIUser		/* Sent by a user. kill(), abort(), etc */
#define SI_QUEUE   PosixSIQueue		/* Sent by sigqueue() */
#define SI_TIMER   PosixSITimer		/* Sent by expiration of a timer_settime() timer */
#define SI_ASYNCIO PosixSIAsyncIO	/* Indicates completion of asycnhronous IO */
#define SI_MESGQ   PosixSIMsgQueued	/* Indicates arrival of a message at an empty queue */

#define siginfo_t PosixSignalInfo

/*  3.3.8 Synchronously Accept a Signal, P1003.1b-1993, p. 76 */

#define SA_NOCLDSTOP	PosixSAFNoChildrenStop
#define SA_RESETHAND	PosixSAFResetHandler
#define SA_RESTART	PosixSAFRestart
#define SA_SIGINFO	PosixSAFSigInfo
#define SA_NOCLDWAIT	PosixSAFNoChildrenWait
#define	SA_NODEFER	PosixSAFNoDefer

#if __BSD_VISIBLE || __XSI_VISIBLE >= 4 || __POSIX_VISIBLE >= 200809
#define SA_ONSTACK	PosixSAFOnStack	/* Signal delivery will be on a separate stack. */
#endif


#if __BSD_VISIBLE || __XSI_VISIBLE >= 4 || __POSIX_VISIBLE >= 200809
/*
 * Minimum and default signal stack constants. Allow for target overrides
 * from <sys/features.h>.
 */
#ifndef	MINSIGSTKSZ
#define	MINSIGSTKSZ	2048
#endif
#ifndef	SIGSTKSZ
#define	SIGSTKSZ	8192
#endif

/*
 * Possible values for ss_flags in stack_t below.
 */
#define	SS_ONSTACK	PosixSAFOnStack
#define	SS_DISABLE	PosixSAFDisable

#endif

/*
 * Structure used in sigaltstack call.
 */
typedef struct sigaltstack {
	void*	ss_sp;    /* Stack base or pointer.  */
	int	ss_flags; /* Flags.  */
	size_t	ss_size;  /* Stack size.  */
} stack_t;

#if __POSIX_VISIBLE
#define SIG_SETMASK	PosixSPMSetMask
#define SIG_BLOCK	PosixSPMBlock
#define SIG_UNBLOCK	PosixSPMUnblock

int sigprocmask (int how, const sigset_t *set, sigset_t *oset);
#endif

#if __POSIX_VISIBLE >= 199506
int pthread_sigmask (int how, const sigset_t *set, sigset_t *oset);
#endif

#if __POSIX_VISIBLE
int kill (pid_t, int);
#endif

#if __BSD_VISIBLE || __XSI_VISIBLE >= 4
int killpg (pid_t, int);
#endif

#if __POSIX_VISIBLE
int sigaction (int, const struct sigaction *, struct sigaction *);
int sigaddset (sigset_t *, const int);
int sigdelset (sigset_t *, const int);
int sigismember (const sigset_t *, int);
int sigfillset (sigset_t *);
int sigemptyset (sigset_t *);
int sigpending (sigset_t *);
int sigsuspend (const sigset_t *);
int sigwait (const sigset_t *set, int *sig);
#endif /* __POSIX_VISIBLE */

#if __XSI_VISIBLE
int sigpause (int);
#endif

#if __BSD_VISIBLE || __XSI_VISIBLE >= 4 || __POSIX_VISIBLE >= 200809
int sigaltstack (const stack_t *, stack_t *);
#endif

#if __POSIX_VISIBLE >= 199506
int pthread_kill (pthread_t thread, int sig);
#endif

#if __POSIX_VISIBLE >= 199309

/*  3.3.8 Synchronously Accept a Signal, P1003.1b-1993, p. 76
    NOTE: P1003.1c/D10, p. 39 adds sigwait().  */

int sigwaitinfo (const sigset_t *set, siginfo_t *info);
int sigtimedwait (const sigset_t *set, siginfo_t *info, const struct timespec *timeout);
int sigqueue (pid_t pid, int signo, const union sigval value);

#endif /* __POSIX_VISIBLE >= 199309 */

#define	SIGHUP		PosixSIGHUP	/* hangup */
#define	SIGINT		PosixSIGINT	/* interrupt */
#define	SIGQUIT		PosixSIGQUIT	/* quit */
#define	SIGILL		PosixSIGILL	/* illegal instruction (not reset when caught) */
#define	SIGTRAP		PosixSIGTRAP	/* trace trap (not reset when caught) */
#define	SIGIOT		PosixSIGIOT	/* IOT instruction */
#define	SIGABRT		PosixSIGABRT	/* used by abort, replace SIGIOT in the future */
#define	SIGEMT		PosixSIGEMT	/* EMT instruction */
#define	SIGFPE		PosixSIGFPE	/* floating point exception */
#define	SIGKILL		PosixSIGKILL	/* kill (cannot be caught or ignored) */
#define	SIGBUS		PosixSIGBUS	/* bus error */
#define	SIGSEGV		PosixSIGSEGV	/* segmentation violation */
#define	SIGSYS		PosixSIGSYS	/* bad argument to system call */
#define	SIGPIPE		PosixSIGPIPE	/* write on a pipe with no one to read it */
#define	SIGALRM		PosixSIGALRM	/* alarm clock */
#define	SIGTERM		PosixSIGTERM	/* software termination signal from kill */
#define	SIGURG		PosixSIGURG	/* urgent condition on IO channel */
#define	SIGSTOP		PosixSIGSTOP	/* sendable stop signal not from tty */
#define	SIGTSTP		PosixSIGTSTP	/* stop signal from tty */
#define	SIGCONT		PosixSIGCONT	/* continue a stopped process */
#define	SIGCHLD		PosixSIGCHLD	/* to parent on child stop or exit */
#define	SIGCLD		PosixSIGCLD	/* System V name for SIGCHLD */
#define	SIGTTIN		PosixSIGTTIN	/* to readers pgrp upon background tty read */
#define	SIGTTOU		PosixSIGTTOU	/* like TTIN for output if (tp->t_local&LTOSTOP) */
#define	SIGIO		PosixSIGIO	/* input/output possible signal */
#define	SIGPOLL		PosixSIGIO	/* System V name for SIGIO */
#define	SIGXCPU		PosixSIGXCPU	/* exceeded CPU time limit */
#define	SIGXFSZ		PosixSIGXFSZ	/* exceeded file size limit */
#define	SIGVTALRM	PosixSIGVTALRM	/* virtual time alarm */
#define	SIGPROF		PosixSIGPROF	/* profiling time alarm */
#define	SIGWINCH	PosixSIGWINCH	/* window changed */
#define	SIGLOST 	PosixSIGLOST 	/* resource lost (eg, record-lock lost) */
#define	SIGUSR1 	PosixSIGUSR1 	/* user defined signal 1 */
#define	SIGUSR2 	PosixSIGUSR2 	/* user defined signal 2 */

/* Real-Time Signals Range, P1003.1b-1993, p. 61
   NOTE: By P1003.1b-1993, this should be at least RTSIG_MAX
         (which is a minimum of 8) signals.
 */
#define SIGRTMIN	PosixSIGRTMIN
#define SIGRTMAX	PosixSIGRTMAX

#define NSIG		64		/* signal 0 implied */

#ifdef __cplusplus
}
#endif

#if defined(__CYGWIN__)
#if __XSI_VISIBLE >= 4 || __POSIX_VISIBLE >= 200809
#include <sys/ucontext.h>
#endif
#endif

#ifndef _SIGNAL_H_
/* Some applications take advantage of the fact that <sys/signal.h>
 * and <signal.h> are equivalent in glibc.  Allow for that here.  */
#include <signal.h>
#endif
#endif /* _SYS_SIGNAL_H */
