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

typedef unsigned short uid_t;
typedef unsigned short gid_t;

int
chown(const char *pathname, uid_t owner, gid_t group)
{
	int *errnop = &_REENT->_errno;
	return POSIX_chown(errnop, pathname, owner, group);
}

int
fchownat(int fd, const char *path, uid_t owner, gid_t group, int flag)
{
	int *errnop = &_REENT->_errno;
	return POSIX_fchownat(errnop, fd, path, owner, group, flag);
}


int
lchown(const char *path, uid_t owner, gid_t group)
{
	int *errnop = &_REENT->_errno;
	return POSIX_lchown(errnop, path, owner, group);
}
