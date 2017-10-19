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

#define nil			((void*)0)
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

extern	void	jehanne_sysfatal(const char*, ...);

#include <posix.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/time.h>
#define __CYGWIN__	/* needed for O_ACCMODE */
#include <fcntl.h>
#undef __CYGWIN__
#include <reent.h>

extern void __application_newlib_init(void) __attribute__((weak));

static PosixError
__stat_reader(struct stat *s, const Dir *d)
{
	s->st_dev = d->dev;
	s->st_ino = d->qid.path;
	s->st_mode = d->mode;
	/* we have to map types to UNIX */
	if(d->mode & DMDIR)
		s->st_mode |= _IFDIR;
	else if(strcmp("cons", d->name) == 0 || strstr(d->name, "tty") != nil){
		/* newlib expect consoles to be character devices */
		s->st_mode |= _IFCHR;
	} else
		s->st_mode |= _IFREG;	/* UNIX lack fantasy :-) */
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

	if(omode != NULL){
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

	if(omode == nil || (flag & O_CREAT) != 0){
		/* translate permissions */
		if(mode & S_IRUSR)
			*cperm |= DMREAD << 6;
		if(mode & S_IWUSR)
			*cperm |= DMWRITE << 6;
		if(mode & S_IXUSR)
			*cperm |= DMEXEC << 6;
		if(mode & S_IRGRP)
			*cperm |= DMREAD << 3;
		if(mode & S_IWGRP)
			*cperm |= DMWRITE << 3;
		if(mode & S_IXGRP)
			*cperm |= DMEXEC << 3;
		if(mode & S_IROTH)
			*cperm |= DMREAD;
		if(mode & S_IWOTH)
			*cperm |= DMWRITE;
		if(mode & S_IXOTH)
			*cperm |= DMEXEC;
	}

	if(flag & O_CREAT){
		if(flag & O_DIRECTORY){
			/* let's copy NetBSD's behaviour
			 * see https://stackoverflow.com/questions/45818628/
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
		if(omode != NULL){
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
	PosixError e = libposix_translate_kernel_errors(error);
	if(e != 0)
		return e;
	if(caller == ADDRESS(POSIX_open))
		return PosixEIO;
	if(caller == ADDRESS(POSIX_chdir))
		return PosixEACCES;
	if(caller == ADDRESS(POSIX_chmod))
		return PosixEPERM;
	if(caller == ADDRESS(POSIX_fchmodat))
		return PosixEPERM;
	if(caller == ADDRESS(POSIX_fchdir))
		return PosixEACCES;
	return PosixEINVAL;
}

static PosixError
default_timeval_reader(void *tv, const Tm *time)
{
	struct timeval *t = tv;

	t->tv_sec = jehanne_tm2sec(time);
	t->tv_usec = 0;

	return 0;
}

static PosixError
default_timezone_reader(void *tz, const Tm *time)
{
	struct timezone *t = tz;

	t->tz_minuteswest = time->tzoff;
	t->tz_dsttime = 0;

	return 0;
}

void
on_process_disposition(int status)
{
	extern void __call_exitprocs (int, void*);

	__call_exitprocs(status, NULL);
	if (_REENT->__cleanup)
		(*_REENT->__cleanup) (_REENT);
}

void
initialize_newlib(void)
{
	/* */
	libposix_define_at_fdcwd(AT_FDCWD);
	libposix_set_stat_reader(stat_reader);
	libposix_set_timeval_reader(default_timeval_reader);
	libposix_set_timezone_reader(default_timezone_reader);
	libposix_translate_seek_whence(SEEK_SET, SEEK_CUR, SEEK_END);
	libposix_translate_access_mode(F_OK, R_OK, W_OK, X_OK);
	libposix_translate_open(open_translator);
	libposix_translate_error(default_error_translator, 0);
	libposix_set_wait_options(0, WNOHANG, 0);
	libposix_on_process_disposition(on_process_disposition);

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

/* _fcntl_r is here to access <fcntl.h> */
int
_fcntl_r(struct _reent *r, int fd, int cmd, int arg)
{
/*
extern	int	jehanne_print(const char*, ...);
extern	uintptr_t	jehanne_getcallerpc(void);
jehanne_fprint(2, "_fcntl_r(%d, %d, %d) from %#p\n", fd, cmd, arg, jehanne_getcallerpc());
*/
	int *errnop = &r->_errno;
	PosixFDCmds pcmd;
	int tmp;
	switch(cmd){
	case F_DUPFD:
		pcmd = PosixFDCDupFD;
		break;
	case F_DUPFD_CLOEXEC:
		pcmd = PosixFDCDupFDCloseOnExec;
		break;
	case F_GETFD:
		if(POSIX_fcntl(errnop, fd, PosixFDCGetFD, arg))
			return O_CLOEXEC;
		return 0;
	case F_SETFD:
		pcmd = PosixFDCSetFD;
		break;
	case F_GETFL:
		tmp = POSIX_fcntl(errnop, fd, PosixFDCGetFL, arg);
		if(tmp < 0)
			return -1;
		if((tmp & ORDWR) == ORDWR)
			return O_RDWR;
		if(tmp & OREAD)
			return O_RDONLY;
		if(tmp & OWRITE)
			return O_WRONLY;
		return 0;
	case F_SETFL:
		pcmd = PosixFDCSetFL;
		break;
	default:
		*errnop = EINVAL;
		return -1;
	}
	return POSIX_fcntl(errnop, fd, pcmd, arg);
}
