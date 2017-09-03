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

int
getrusage(int who, struct rusage *r_usage)
{
	PosixRUsages u;
	int *errnop = &_REENT->_errno;
	switch(who){
	case 0:
		u = PosixRUsageSelf;
		break;
	case 1:
		u = PosixRUsageChildren;
		break;
	default:
		u = PosixRUsageUnknown;
		break;
	}
	return POSIX_getrusage(errnop, u, r_usage);
}
