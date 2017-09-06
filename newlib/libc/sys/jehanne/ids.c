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

typedef int uid_t;
typedef int gid_t;

uid_t
getuid(void)
{
	int *errnop = &_REENT->_errno;
	return POSIX_getuid(errnop);
}

uid_t
geteuid(void)
{
	int *errnop = &_REENT->_errno;
	return POSIX_geteuid(errnop);
}

int
setuid(uid_t uid)
{
	int *errnop = &_REENT->_errno;
	return POSIX_setuid(errnop, uid);
}

int
seteuid(uid_t euid)
{
	int *errnop = &_REENT->_errno;
	return POSIX_seteuid(errnop, euid);
}

int
setreuid(uid_t ruid, uid_t euid)
{
	int *errnop = &_REENT->_errno;
	return POSIX_setreuid(errnop, ruid, euid);
}

gid_t
getgid(void)
{
	int *errnop = &_REENT->_errno;
	return POSIX_getgid(errnop);
}

gid_t
getegid(void)
{
	int *errnop = &_REENT->_errno;
	return POSIX_getegid(errnop);
}

int
setgid(gid_t gid)
{
	int *errnop = &_REENT->_errno;
	return POSIX_setgid(errnop, gid);
}

int
setegid(gid_t egid)
{
	int *errnop = &_REENT->_errno;
	return POSIX_setgid(errnop, egid);
}

int
setregid(gid_t rgid, gid_t egid)
{
	int *errnop = &_REENT->_errno;
	return POSIX_setregid(errnop, rgid, egid);
}

int setpgid(pid_t pid, pid_t pgid)
{
	int *errnop = &_REENT->_errno;
	return POSIX_setpgid(errnop, pid, pgid);
}

pid_t
getsid(pid_t pid)
{
	int *errnop = &_REENT->_errno;
	return POSIX_getsid(errnop, pid);
}

pid_t
getpgrp(void)
{
	int *errnop = &_REENT->_errno;
	return POSIX_getpgrp(errnop);
}

pid_t
setsid(void)
{
	int *errnop = &_REENT->_errno;
	return POSIX_setsid(errnop);
}
