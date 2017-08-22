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

/* we cannot include u.h and libc.h due to name collisions */

typedef long			off_t;
typedef long			ptrdiff_t;
typedef unsigned long		uint64_t;
typedef unsigned long		uintptr_t;
typedef unsigned int		uint32_t;
typedef long			int64_t;
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef __SIZE_TYPE__		size_t;

typedef
struct Tm
{
	int	sec;
	int	min;
	int	hour;
	int	mday;
	int	mon;
	int	year;
	int	wday;
	int	yday;
	char	zone[4];
	int	tzoff;
} Tm;

typedef
struct Qid
{
	uint64_t	path;
	uint32_t	vers;
	uint8_t	type;
} Qid;

typedef
struct Dir {
	/* jehanne_system-modified data */
	uint16_t	type;	/* server type */
	uint32_t	dev;	/* server subtype */
	/* file data */
	Qid	qid;	/* unique id from server */
	uint32_t	mode;	/* permissions */
	uint32_t	atime;	/* last jehanne_read jehanne_time */
	uint32_t	mtime;	/* last jehanne_write jehanne_time */
	int64_t	length;	/* file length */
	char	*name;	/* last element of path */
	char	*uid;	/* owner name */
	char	*gid;	/* group name */
	char	*muid;	/* last modifier name */
} Dir;

#define OSTAT	0x00		/* open for stat/wstat */
#define OREAD	0x01		/* open for read */
#define OWRITE	0x02		/* write */
#define ORDWR	(OREAD|OWRITE)	/* read and write */
#define OEXEC	0x04		/* execute, == read but check execute permission */
#define OCEXEC	0x08		/* or'ed in, close on exec */
#define ORCLOSE	0x10		/* or'ed in, remove on close */
#define OTRUNC	0x0100		/* or'ed in (except for exec), truncate file first */

#define DMDIR		0x80000000	/* mode bit for directories */
#define DMAPPEND	0x40000000	/* mode bit for append only files */
#define DMEXCL		0x20000000	/* mode bit for exclusive use files */
#define DMMOUNT		0x10000000	/* mode bit for mounted channel */
#define DMAUTH		0x08000000	/* mode bit for authentication file */
#define DMTMP		0x04000000	/* mode bit for non-backed-up files */
#define DMREAD		0x4		/* mode bit for read permission */
#define DMWRITE		0x2		/* mode bit for write permission */
#define DMEXEC		0x1		/* mode bit for execute permission */
s
extern	void	jehanne_sysfatal(const char*, ...);

#include <posix.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/signal.h>
#define __CYGWIN__	/* needed for O_ACCMODE */
#include <fcntl.h>
#undef __CYGWIN__

extern void __application_newlib_init(void) __attribute__((weak));

static PosixError
__stat_reader(struct stat *s, const Dir *d)
{
	s->st_dev = d->dev;
	s->st_ino = d->qid.path;
	s->st_mode = d->mode;
	s->st_nlink = 1;
	s->st_uid = 1;
	s->st_gid = 1;
	s->st_atime = d->atime;
	s->st_mtime = d->mtime;
	s->st_ctime = d->mtime;
	s->st_size = d->length;
	return 0;
}

static PosixError
stat_reader(void *statp, const Dir *dir)
{
	return __stat_reader((struct stat *)statp, dir);
}

static PosixError
open_translator(int flag, int mode, long *omode, long *cperm)
{
	*cperm = 0;

	if(omode != nil){
		*omode = 0;
		switch(flag & O_ACCMODE){
		case O_RDONLY:
			*omode |= OREAD;
			break;
		case O_WRONLY:
			*omode |= OWRITE;
			break;
		case O_RDWR:
			*omode |= ORDWR;
			break;
		}

		if(flag & O_EXEC)
			*omode = OEXEC;
		else if(flag & O_SEARCH){
			/* POSIX_open will fail if the file is
			 * not a directory
			 */
			*omode |= (OEXEC|DMDIR);
		}

		if(flag & O_TRUNC)
			*omode |= OTRUNC;
		if(flag & O_CLOEXEC)
			*omode |= OCEXEC;
	}

	/* translate permissions */
	if(mode & S_IRUSR)
		*cperm |= OREAD << 6;
	if(mode & S_IWUSR)
		*cperm |= OWRITE << 6;
	if(mode & S_IXUSR)
		*cperm |= OEXEC << 6;
	if(mode & S_IRGRP)
		*cperm |= OREAD << 3;
	if(mode & S_IWGRP)
		*cperm |= OWRITE << 3;
	if(mode & S_IXGRP)
		*cperm |= OEXEC << 3;
	if(mode & S_IROTH)
		*cperm |= OREAD;
	if(mode & S_IWOTH)
		*cperm |= OWRITE;
	if(mode & S_IXOTH)
		*cperm |= OEXEC;

	if(flag & O_CREAT){
		if(flag & O_DIRECTORY){
			/* let's copy NetBSD's behaviour
			 * see hhttps://stackoverflow.com/questions/45818628/
			 */
			return PosixEINVAL;
		}
		if(flag & O_EXCL)
			*cperm |= DMEXCL;
	} else {
		if(flag & O_APPEND)
			return PosixENOTSUP;
		if(flag & O_EXCL)
			return PosixENOTSUP;
		if(omode != nil){
			if(flag & O_DIRECTORY){
				/* POSIX_open will fail if the file is
				 * not a directory
				 */
				*omode |= DMDIR;
			}
		}
	}

	return 0;
}

#define ADDRESS(func) ((long)func)
static PosixError
default_error_translator(char* error, uintptr_t caller)
{
	jehanne_fprint(2, "newlib: %s\n", error);
	if(caller == ADDRESS(POSIX_open))
		return PosixEIO;
	return PosixEINVAL;
}

static PosixSignalAction
signal_trampoline(int signal)
{
	switch(__sigtramp(signal)){
	case -1: // unknown signal or uninitialized signals
		jehanne_sysfatal("newlib: error handling signal %d or unknown signal", signal);
	case 0: // catched
		return SignalCatched;
	case 3: // ignored
		return SignalIgnored;
	case 1: // default
		return SignalDefault;
	case 2: // error
		return SignalError;
	default:
		jehanne_sysfatal("newlib: unexpected PosixSignalAction from signal trampoline: %d", signal);
	}
}

void
initialize_newlib(void)
{
	/* */
	libposix_set_stat_reader(stat_reader);
	libposix_translate_seek_whence(SEEK_SET, SEEK_CUR, SEEK_END);
	libposix_translate_open(open_translator);
	libposix_set_signal_trampoline(signal_trampoline);
	libposix_translate_error(default_error_translator, 0);
	libposix_set_wait_options(0, WNOHANG, 0);

	/* signals */
	libposix_define_signal(PosixSIGABRT, SIGABRT);
	libposix_define_signal(PosixSIGALRM, SIGALRM);
	libposix_define_signal(PosixSIGBUS, SIGBUS);
	libposix_define_signal(PosixSIGCHLD, SIGCHLD);
	libposix_define_signal(PosixSIGCONT, SIGCONT);
	libposix_define_signal(PosixSIGFPE, SIGFPE);
	libposix_define_signal(PosixSIGHUP, SIGHUP);
	libposix_define_signal(PosixSIGILL, SIGILL);
	libposix_define_signal(PosixSIGINT, SIGINT);
	libposix_define_signal(PosixSIGKILL, SIGKILL);
	libposix_define_signal(PosixSIGPIPE, SIGPIPE);
	libposix_define_signal(PosixSIGQUIT, SIGQUIT);
	libposix_define_signal(PosixSIGSEGV, SIGSEGV);
	libposix_define_signal(PosixSIGSTOP, SIGSTOP);
	libposix_define_signal(PosixSIGTERM, SIGTERM);
	libposix_define_signal(PosixSIGTSTP, SIGTSTP);
	libposix_define_signal(PosixSIGTTIN, SIGTTIN);
	libposix_define_signal(PosixSIGTTOU, SIGTTOU);
	libposix_define_signal(PosixSIGUSR1, SIGUSR1);
	libposix_define_signal(PosixSIGUSR2, SIGUSR2);
	libposix_define_signal(PosixSIGPOLL, SIGPOLL);
	libposix_define_signal(PosixSIGPROF, SIGPROF);
	libposix_define_signal(PosixSIGSYS, SIGSYS);
	libposix_define_signal(PosixSIGTRAP, SIGTRAP);
	libposix_define_signal(PosixSIGURG, SIGURG);
	libposix_define_signal(PosixSIGVTALRM, SIGVTALRM);
	libposix_define_signal(PosixSIGXCPU, SIGXCPU);
	libposix_define_signal(PosixSIGXFSZ, SIGXFSZ);
#ifdef SIGIOT
	libposix_define_signal(PosixSIGIOT, SIGIOT);
#endif
#ifdef SIGEMT
	libposix_define_signal(PosixSIGEMT, SIGEMT);
#endif
#ifdef SIGSTKFLT
	libposix_define_signal(PosixSIGSTKFLT, SIGSTKFLT);
#endif
#ifdef SIGIO
	libposix_define_signal(PosixSIGIO, SIGIO);
#endif
#ifdef SIGCLD
	libposix_define_signal(PosixSIGCLD, SIGCLD);
#endif
#ifdef SIGPWR
	libposix_define_signal(PosixSIGPWR, SIGPWR);
#endif
#ifdef SIGINFO
	libposix_define_signal(PosixSIGINFO, SIGINFO);
#endif
#ifdef SIGLOST
	libposix_define_signal(PosixSIGLOST, SIGLOST);
#endif
#ifdef SIGWINCH
	libposix_define_signal(PosixSIGWINCH, SIGWINCH);
#endif
#ifdef SIGUNUSED
	libposix_define_signal(PosixSIGUNUSED, SIGUNUSED);
#endif
#if defined(SIGRTMIN) && defined(SIGRTMAX)
	libposix_define_realtime_signals(SIGRTMIN, SIGRTMAX);
#endif

	/* error numbers */
	libposix_define_errno(PosixE2BIG, E2BIG);
	libposix_define_errno(PosixEACCES, EACCES);
	libposix_define_errno(PosixEADDRINUSE, EADDRINUSE);
	libposix_define_errno(PosixEADDRNOTAVAIL, EADDRNOTAVAIL);
	libposix_define_errno(PosixEAFNOSUPPORT, EAFNOSUPPORT);
	libposix_define_errno(PosixEAGAIN, EAGAIN);
	libposix_define_errno(PosixEALREADY, EALREADY);
	libposix_define_errno(PosixEBADF, EBADF);
	libposix_define_errno(PosixEBADMSG, EBADMSG);
	libposix_define_errno(PosixEBUSY, EBUSY);
	libposix_define_errno(PosixECANCELED, ECANCELED);
	libposix_define_errno(PosixECHILD, ECHILD);
	libposix_define_errno(PosixECONNABORTED, ECONNABORTED);
	libposix_define_errno(PosixECONNREFUSED, ECONNREFUSED);
	libposix_define_errno(PosixECONNRESET, ECONNRESET);
	libposix_define_errno(PosixEDEADLK, EDEADLK);
	libposix_define_errno(PosixEDESTADDRREQ, EDESTADDRREQ);
	libposix_define_errno(PosixEDOM, EDOM);
	libposix_define_errno(PosixEDQUOT, EDQUOT);
	libposix_define_errno(PosixEEXIST, EEXIST);
	libposix_define_errno(PosixEFAULT, EFAULT);
	libposix_define_errno(PosixEFBIG, EFBIG);
	libposix_define_errno(PosixEHOSTUNREACH, EHOSTUNREACH);
	libposix_define_errno(PosixEIDRM, EIDRM);
	libposix_define_errno(PosixEILSEQ, EILSEQ);
	libposix_define_errno(PosixEINPROGRESS, EINPROGRESS);
	libposix_define_errno(PosixEINTR, EINTR);
	libposix_define_errno(PosixEINVAL, EINVAL);
	libposix_define_errno(PosixEIO, EIO);
	libposix_define_errno(PosixEISCONN, EISCONN);
	libposix_define_errno(PosixEISDIR, EISDIR);
	libposix_define_errno(PosixELOOP, ELOOP);
	libposix_define_errno(PosixEMFILE, EMFILE);
	libposix_define_errno(PosixEMLINK, EMLINK);
	libposix_define_errno(PosixEMSGSIZE, EMSGSIZE);
	libposix_define_errno(PosixEMULTIHOP, EMULTIHOP);
	libposix_define_errno(PosixENAMETOOLONG, ENAMETOOLONG);
	libposix_define_errno(PosixENETDOWN, ENETDOWN);
	libposix_define_errno(PosixENETRESET, ENETRESET);
	libposix_define_errno(PosixENETUNREACH, ENETUNREACH);
	libposix_define_errno(PosixENFILE, ENFILE);
	libposix_define_errno(PosixENOBUFS, ENOBUFS);
	libposix_define_errno(PosixENODATA, ENODATA);
	libposix_define_errno(PosixENODEV, ENODEV);
	libposix_define_errno(PosixENOENT, ENOENT);
	libposix_define_errno(PosixENOEXEC, ENOEXEC);
	libposix_define_errno(PosixENOLCK, ENOLCK);
	libposix_define_errno(PosixENOLINK, ENOLINK);
	libposix_define_errno(PosixENOMEM, ENOMEM);
	libposix_define_errno(PosixENOMSG, ENOMSG);
	libposix_define_errno(PosixENOPROTOOPT, ENOPROTOOPT);
	libposix_define_errno(PosixENOSPC, ENOSPC);
	libposix_define_errno(PosixENOSR, ENOSR);
	libposix_define_errno(PosixENOSTR, ENOSTR);
	libposix_define_errno(PosixENOSYS, ENOSYS);
	libposix_define_errno(PosixENOTCONN, ENOTCONN);
	libposix_define_errno(PosixENOTDIR, ENOTDIR);
	libposix_define_errno(PosixENOTEMPTY, ENOTEMPTY);
	libposix_define_errno(PosixENOTRECOVERABLE, ENOTRECOVERABLE);
	libposix_define_errno(PosixENOTSOCK, ENOTSOCK);
	libposix_define_errno(PosixENOTSUP, ENOTSUP);
	libposix_define_errno(PosixENOTTY, ENOTTY);
	libposix_define_errno(PosixENXIO, ENXIO);
	libposix_define_errno(PosixEOPNOTSUPP, EOPNOTSUPP);
	libposix_define_errno(PosixEOVERFLOW, EOVERFLOW);
	libposix_define_errno(PosixEOWNERDEAD, EOWNERDEAD);
	libposix_define_errno(PosixEPERM, EPERM);
	libposix_define_errno(PosixEPIPE, EPIPE);
	libposix_define_errno(PosixEPROTO, EPROTO);
	libposix_define_errno(PosixEPROTONOSUPPORT, EPROTONOSUPPORT);
	libposix_define_errno(PosixEPROTOTYPE, EPROTOTYPE);
	libposix_define_errno(PosixERANGE, ERANGE);
	libposix_define_errno(PosixEROFS, EROFS);
	libposix_define_errno(PosixESPIPE, ESPIPE);
	libposix_define_errno(PosixESRCH, ESRCH);
	libposix_define_errno(PosixESTALE, ESTALE);
	libposix_define_errno(PosixETIME, ETIME);
	libposix_define_errno(PosixETIMEDOUT, ETIMEDOUT);
	libposix_define_errno(PosixETXTBSY, ETXTBSY);
	libposix_define_errno(PosixEWOULDBLOCK, EWOULDBLOCK);
	libposix_define_errno(PosixEXDEV, EXDEV);
	
	/* let the application override defaults */
	if(__application_newlib_init != 0)
		__application_newlib_init();
}
