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
#include <u.h>
#include <libc.h>
#include <posix.h>
#include <reent.h>

typedef unsigned long clock_t;
typedef unsigned long useconds_t;

extern void initialize_newlib(void);
static void newlib(int argc, char *argv[]) __attribute__((noreturn));

void
__libc_init(int argc, char *argv[])
{
	/* we use a static function so that the call stack clarify
	 * which __libc_init is being used during debug
	 */
	newlib(argc, argv);
}

static void
newlib(int argc, char *argv[])
{
	struct _reent private_reent = _REENT_INIT (private_reent);

	/* initialize the private reentrancy structure */
	_impure_ptr = &private_reent;

	/* initialize signal infrastructure */
	_init_signal_r(_REENT);

	libposix_init(argc, argv, initialize_newlib);
}

void
_exit(int code)
{
	POSIX_exit(code);
}

int
_close_r(struct _reent *r, int file)
{
	int *errnop = &r->_errno;
	return POSIX_close(errnop, file);
}

int
_execve_r(struct _reent *r, const char * name, char *const * argv, char *const * env)
{
	int *errnop = &r->_errno;
	return POSIX_execve(errnop, name, argv, env);
}

int
_execve(const char * name, char *const * argv, char *const * env)
{
	int *errnop = &_REENT->_errno;
	return POSIX_execve(errnop, name, argv, env);
}

int
_fork_r(struct _reent *r)
{
	int *errnop = &r->_errno;
	return POSIX_fork(errnop);
}

int
_fstat_r(struct _reent *r, int file, struct stat *st)
{
	int *errnop = &r->_errno;
	return POSIX_fstat(errnop, file, st);
}

int
_getpid_r(struct _reent *r)
{
	int *errnop = &r->_errno;
	return POSIX_getpid(errnop);
}

int
getppid(void)
{
	int *errnop = &_REENT->_errno;
	return POSIX_getppid(errnop);
}

int
_isatty_r(struct _reent *r, int file)
{
	int *errnop = &r->_errno;
	return POSIX_isatty(errnop, file);
}

int
_kill_r(struct _reent *r, int pid, int sig)
{
	int *errnop = &r->_errno;
	return POSIX_kill(errnop, pid, sig);
}

int
_link_r(struct _reent *r, const char *old, const char *new)
{
	int *errnop = &r->_errno;
	return POSIX_link(errnop, old, new);
}

off_t
_lseek_r(struct _reent *r, int fd, off_t pos, int whence)
{
	int *errnop = &r->_errno;
	return POSIX_lseek(errnop, fd, pos, whence);
}

int
_open_r(struct _reent *r, const char *name, int flags, int mode)
{
	int *errnop = &r->_errno;
	return POSIX_open(errnop, name, flags, mode);
}

long
_read_r(struct _reent *r, int fd, void *buf, size_t len)
{
	int *errnop = &r->_errno;
	return POSIX_read(errnop, fd, buf, len);
}

void *
_sbrk_r(struct _reent *r, ptrdiff_t incr)
{
	int *errnop = &r->_errno;
	return POSIX_sbrk(errnop, incr);
}

int
_stat_r(struct _reent *r, const char *file, struct stat *pstat)
{
	int *errnop = &r->_errno;
	return POSIX_stat(errnop, file, pstat);
}

int
lstat(const char *file, struct stat *pstat)
{
	int *errnop = &_REENT->_errno;
	return POSIX_lstat(errnop, file, pstat);
}

clock_t
_times_r(struct _reent *r, struct tms *buf)
{
	int *errnop = &r->_errno;
	return POSIX_times(errnop, buf);
}

int
_unlink_r(struct _reent *r, const char *name)
{
	int *errnop = &r->_errno;
	return POSIX_unlink(errnop, name);
}

int
_wait_r(struct _reent *r, int *status)
{
	int *errnop = &r->_errno;
	return POSIX_wait(errnop, status);
}

pid_t
waitpid (pid_t reqpid, int *status, int options)
{
	int *errnop = &_REENT->_errno;
	return POSIX_waitpid(errnop, reqpid, status, options);
}

long
_write_r(struct _reent *r, int fd, const void *buf, size_t len)
{
	int *errnop = &r->_errno;
	return POSIX_write(errnop, fd, buf, len);
}

int
_gettimeofday_r(struct _reent *r, struct timeval *p, void *z)
{
	int *errnop = &r->_errno;
	return POSIX_gettimeofday(errnop, p, z);
}

unsigned int
sleep(unsigned int seconds)
{
	return POSIX_sleep(seconds);
}

int
usleep(useconds_t usec)
{
	int *errnop = &_REENT->_errno;
	return POSIX_usleep(errnop, usec);
}

int
pause(void)
{
	int *errnop = &_REENT->_errno;
	return POSIX_pause(errnop);
}

int
pipe(int fildes[2])
{
	int *errnop = &_REENT->_errno;
	return POSIX_pipe(errnop, fildes);
}

char*
getcwd(char *buf, size_t size)
{
	int *errnop = &_REENT->_errno;
	return POSIX_getcwd(errnop, buf, size);
}

char*
getlogin(void)
{
	int *errnop = &_REENT->_errno;
	return POSIX_getlogin(errnop);
}

int
getlogin_r(char *name, size_t namesize)
{
	int *errnop = &_REENT->_errno;
	return POSIX_getlogin_r(errnop, name, namesize);
}

char*
getpass(const char *prompt)
{
	int *errnop = &_REENT->_errno;
	return POSIX_getpass(errnop, prompt);
}

int
mkdir(const char *path, int mode)
{
	int *errnop = &_REENT->_errno;
	return POSIX_mkdir(errnop, path, mode);
}

int
chdir(const char *path)
{
	int *errnop = &_REENT->_errno;
	return POSIX_chdir(errnop, path);
}

int
fchdir(int fd)
{
	int *errnop = &_REENT->_errno;
	return POSIX_fchdir(errnop, fd);
}

int
getdents (int fd, void *buf, int buf_size)
{
	int *errnop = &_REENT->_errno;
	return libposix_getdents(errnop, fd, buf, buf_size);
}

int
access(const char *path, int amode)
{
	int *errnop = &_REENT->_errno;
	return POSIX_access(errnop, path, amode);
}

int
dup(int fildes)
{
	int *errnop = &_REENT->_errno;
	return POSIX_dup(errnop, fildes);
}

int
dup2(int fildes, int fildes2)
{
	int *errnop = &_REENT->_errno;
	return POSIX_dup2(errnop, fildes, fildes2);
}

int
readlink(const char *path, char *buf, int bufsize)
{
	int *errnop = &_REENT->_errno;
	return POSIX_readlink(errnop, path, buf, bufsize);
}

int
readlinkat(int fd, const char *path, char *buf, int bufsize)
{
	int *errnop = &_REENT->_errno;
	return POSIX_readlinkat(errnop, fd, path, buf, bufsize);
}

int
umask(int mask)
{
	int *errnop = &_REENT->_errno;
	return POSIX_umask(errnop, mask);
}

#undef pread
ssize_t
pread(int fd, void *buf, size_t count, off_t offset)
{
	int *errnop = &_REENT->_errno;
	return POSIX_pread(errnop, fd, buf, count, offset);
}
#undef pwrite
ssize_t
pwrite(int fd, const void *buf, size_t count, off_t offset)
{
	int *errnop = &_REENT->_errno;
	return POSIX_pwrite(errnop, fd, buf, count, offset);
}
