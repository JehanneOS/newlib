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
#ifndef _SYS_TERMIOS_H_
#define _SYS_TERMIOS_H_

/* For more info, see
 * - http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/termios.h.html
 * - http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap11.html
 */

/* input modes */
#define BRKINT	0x001
#define ICRNL	0x002
#define IGNBRK	0x004
#define IGNCR	0x008
#define IGNPAR	0x010
#define INLCR	0x020
#define INPCK	0x040
#define ISTRIP	0x080
#define IXANY	0x100
#define IXOFF	0x200
#define IXON	0x400
#define PARMRK	0x800

/* output modes: ONLCR, TAB3 are an extension to POSIX! */
#define	OPOST	0000001
#define	ONLCR	0000002
#define	OCRNL	0000004
#define	ONOCR	0000010
#define	ONLRET	0000020
#define	OFDEL	0000040
#define	OFILL	0000100
#define	NLDLY	0000200
#define		NL0	0000000
#define		NL1	0000200
#define	CRDLY	0003000
#define		CR0	0000000
#define		CR1	0001000
#define		CR2	0002000
#define		CR3	0003000
#define	TABDLY	0014000
#define		TAB0	0000000
#define		TAB1	0004000
#define		TAB2	0010000
#define		TAB3	0014000
#define	BSDLY	0020000
#define		BS0	0000000
#define		BS1	0020000
#define	VTDLY	0040000
#define		VT0	0000000
#define		VT1	0040000
#define	FFDLY	0100000
#define		FF0	0000000
#define		FF1	0100000

/* control modes */
#define CLOCAL	0x001
#define CREAD	0x002
#define CSIZE	0x01C
#define 	CS5	0x004
#define 	CS6	0x008
#define 	CS7	0x00C
#define 	CS8	0x010
#define CSTOPB	0x020
#define HUPCL	0x040
#define PARENB	0x080
#define PARODD	0x100

/* local modes */
#define ECHO	0x001
#define ECHOE	0x002
#define ECHOK	0x004
#define ECHONL	0x008
#define ICANON	0x010
#define IEXTEN	0x020
#define ISIG	0x040
#define NOFLSH	0x080
#define TOSTOP	0x100

/* control characters */
#define VEOF	0
#define VEOL	1
#define VERASE	2
#define VINTR	3
#define VKILL	4
#define VMIN	5
#define VQUIT	6
#define VSTART	7
#define VSTOP	8
#define VSUSP	9
#define VTIME	10
#define NCCS	11	/* Size of the array c_cc for control characters. */

/* baud rates */
#define B0	0
#define B50	1
#define B75	2
#define B110	3
#define B134	4
#define B150	5
#define B200	6
#define B300	7
#define B600	8
#define B1200	9
#define B1800	10
#define B2400	11
#define B4800	12
#define B9600	13
#define B19200	14
#define B38400	15

/* optional actions for tcsetattr */
#define TCSANOW	  1
#define TCSADRAIN 2
#define TCSAFLUSH 3

/* queue selectors for tcflush */
#define TCIFLUSH  1
#define TCIOFLUSH 2
#define TCOFLUSH  3

/* actions for tcflow */
#define TCOOFF 0
#define TCOON  1
#define TCIOFF 2
#define TCION  3

typedef unsigned long tcflag_t;
typedef unsigned long speed_t;
typedef unsigned char cc_t;
typedef int pid_t;

struct termios {
	tcflag_t	c_iflag;	/* input modes */
	tcflag_t	c_oflag;	/* output modes */
	tcflag_t	c_cflag;	/* control modes */
	tcflag_t	c_lflag;	/* local modes */
	cc_t		c_cc[NCCS];	/* control characters */
};

int tcgetattr(int fd, struct termios *termios_p);
int tcsetattr(int fd, int mode, const struct termios *termios_p);
int tcsendbreak(int fd, int duration);
int tcflush(int fd, int queue_selector);
int tcdrain(int fd);
int tcflow(int fildes, int action);
pid_t tcgetsid(int fd);

speed_t cfgetispeed(const struct termios *);
speed_t cfgetospeed(const struct termios *);
int     cfsetispeed(struct termios *, speed_t);
int     cfsetospeed(struct termios *, speed_t);

#endif
