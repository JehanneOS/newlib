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

typedef unsigned long sigset_t;

int
sigfillset(sigset_t *set)
{
	int *errnop = &_REENT->_errno;
	return POSIX_sigfillset(errnop, set);
}

int
sigemptyset(sigset_t *set)
{
	int *errnop = &_REENT->_errno;
	return POSIX_sigemptyset(errnop, set);
}

int
sigismember(const sigset_t *set, int signo)
{
	int *errnop = &_REENT->_errno;
	return POSIX_sigismember(errnop, set, signo);
}

int
sigaddset(sigset_t *set, int signo)
{
	int *errnop = &_REENT->_errno;
	return POSIX_sigaddset(errnop, set, signo);
}

int
sigdelset(sigset_t *set, int signo)
{
	int *errnop = &_REENT->_errno;
	return POSIX_sigdelset(errnop, set, signo);
}

int
sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
	int *errnop = &_REENT->_errno;
	return POSIX_sigprocmask(errnop, how, set, oset);
}
