#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include "_ansi.h"
#include <sys/cdefs.h>
#include <sys/signal.h>

_BEGIN_STD_C

typedef int	sig_atomic_t;		/* Atomic entity type (ANSI) */
#if __BSD_VISIBLE
typedef PosixSigHandler sig_t;		/* BSD naming */
#endif
#if __GNU_VISIBLE
typedef PosixSigHandler sighandler_t;	/* glibc naming */
#endif

#define SIG_DFL ((PosixSigHandler)0)	/* Default action */
#define SIG_IGN ((PosixSigHandler)1)	/* Ignore action */
#define SIG_ERR ((PosixSigHandler)-1)	/* Error return */

struct _reent;

PosixSigHandler sysv_signal(int signo, PosixSigHandler handler);
PosixSigHandler bsd_signal(int signo, PosixSigHandler handler);

#define ssignal(signo, handler)	bsd_signal(signo, handler)

#ifdef _XOPEN_SOURCE
# define signal(signo, handler)	sysv_signal(signo, handler)
#else
# define signal(signo, handler)	bsd_signal(signo, handler)
#endif

int raise(int signo);
void psignal(int signo, const char *s);

_END_STD_C

#endif /* _SIGNAL_H_ */
