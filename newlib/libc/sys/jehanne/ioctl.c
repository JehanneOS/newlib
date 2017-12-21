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
#include <envvars.h>
#include <posix.h>
#include <reent.h>
#include <sys/ioctl.h>
#include <errno.h>

void
tty_getsize(struct winsize *size)
{
	char *var;

	var = jehanne_getenv("LINES");
	if(var != nil){
		size->ws_row = (unsigned short)atoi(var);
		free(var);
	} else {
		size->ws_row = 0;
	}
	var = jehanne_getenv("COLS");
	if(var != nil){
		size->ws_col = (unsigned short)atoi(var);
		free(var);
	} else {
		size->ws_row = 0;
	}
	var = jehanne_getenv("XPIXELS");
	if(var != nil){
		size->ws_xpixel = (unsigned short)atoi(var);
		free(var);
	} else {
		size->ws_row = 0;
	}
	var = jehanne_getenv("YPIXELS");
	if(var != nil){
		size->ws_ypixel = (unsigned short)atoi(var);
		free(var);
	} else {
		size->ws_row = 0;
	}
}
int
tty_setsize(struct winsize *size)
{
	int fd, i, s, w, h;
	char buf[64];
	struct winsize old;

	if(size->ws_xpixel && size->ws_ypixel){
PixelSized:
		fd = open("/dev/wctl", OWRITE);
		if(fd < 0)
			return 0;
		i = snprint(buf, sizeof(buf), "resize -dx %su -dy %su", size->ws_xpixel, size->ws_ypixel);
		write(fd, buf, i);
		close(fd);
		return 1;
	}
	if(size->ws_row && size->ws_col){
		tty_getsize(&old);
		s = old.ws_xpixel / old.ws_col;
		size->ws_xpixel = (s + 1)*size->ws_col;
		s = old.ws_ypixel / old.ws_row;
		size->ws_ypixel = (s + 1)*size->ws_row;
		goto PixelSized;
	}
	return 0;
}
int
ioctl(int fd, int request, ...)
{
	va_list ap;
	void *parg;

	int *errnop = &_REENT->_errno;
	if(!POSIX_isatty(errnop, fd))
		return -1;

	va_start(ap, request);
	switch(request){
	case TIOCGWINSZ:
		parg = va_arg(ap, void*);
		tty_getsize(parg);
		break;
	case TIOCSWINSZ:
		parg = va_arg(ap, void*);
		if(!tty_setsize(parg))
			*errnop = EPERM;
		break;
	default:
		break;
	}
	va_end(ap);
	// blatant lie
	return 0;
}
