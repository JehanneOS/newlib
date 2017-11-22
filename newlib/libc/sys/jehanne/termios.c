/*
 * This file is part of Jehanne.
 *
 * Copyright (C) 2017 Giacomo Tesio <giacomo@tesio.it>
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, version 3 of the License.
 *
 * Jehanne is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Jehanne.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <u.h>
#include <libc.h>
#include <posix.h>
#include <reent.h>
#include <termios.h>

int
tcgetpgrp(int fd)
{
	int *errnop = &_REENT->_errno;
	return POSIX_tcgetpgrp(errnop, fd);
}

int
tcsetpgrp(int fd, int pgrp)
{
	int *errnop = &_REENT->_errno;
	return POSIX_tcsetpgrp(errnop, fd, pgrp);
}

int
tcflush(int fd, int queue_selector)
{
	// blatant lie
	return 0;
}

#define	CESC	'\\'
#define	CINTR	0177	/* DEL */
#define	CQUIT	034	/* FS, cntl | */
#define	CERASE	010	/* BS */
#define	CKILL	025 /* cntl u */
#define	CEOF	04	/* cntl d */
#define	CSTART	021	/* cntl q */
#define	CSTOP	023	/* cntl s */
#define	CSWTCH	032	/* cntl z */
#define CEOL	000
#define	CNSWTCH	0

int
tcgetattr(int fd, struct termios *t)
{
	int *errnop = &_REENT->_errno;
	if(POSIX_isatty(errnop, fd)) {
		t->c_iflag = ISTRIP|ICRNL|IXON|IXOFF;
		t->c_oflag = OPOST|TAB3|ONLCR;
		t->c_cflag = B9600;
		t->c_lflag = ISIG|ICANON|ECHO|ECHOE|ECHOK;
		t->c_cc[VINTR] = CINTR;
		t->c_cc[VQUIT] = CQUIT;
		t->c_cc[VERASE] = CERASE;
		t->c_cc[VKILL] = CKILL;
		t->c_cc[VEOF] = CEOF;
		t->c_cc[VEOL] = CEOL;
		t->c_cc[VSTART] = CSTART;
		t->c_cc[VSTOP] = CSTOP;
		return 0;
	}
	return -1;
}

int
tcsetattr(int fd, int optactions, const struct termios *t)
{
	int *errnop = &_REENT->_errno;
	if(POSIX_isatty(errnop, fd))
		return 0;
	return -1;
}
