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
#include <sys/types.h>

#ifndef _SYS_DIRENT_H
#define _SYS_DIRENT_H

#define MAXNAMLEN 127		/* sizeof(struct dirent.d_name)-1 */

#define _LIBPOSIX_H
#include <posix.h>		/* get dirent structure from posix.h */
#undef _LIBPOSIX_H

typedef struct {
	int dd_fd;		/* Directory file. */
	int dd_loc;		/* Position in buffer. */
	int dd_seek;
	char *dd_buf;		/* Pointer to buffer. */
	int dd_len;		/* Buffer size. */
	int dd_size;		/* Data size in buffer. */
} DIR;

#define d_fileno	d_ino	/* Cheap backwards compatibility. */

#ifdef __USE_LARGEFILE64
#define dirent64		dirent
#endif

#ifndef _DIRENT_H_
/* Newlib's dirent.h already defines these */

DIR *opendir(const char *);
struct dirent *readdir(DIR *);
int readdir_r(DIR *__restrict, struct dirent *__restrict,
              struct dirent **__restrict);
void rewinddir(DIR *);
int dirfd(DIR *);
DIR *fdopendir(int);
int closedir(DIR *);

#endif

#ifndef _POSIX_SOURCE
long telldir (DIR *);
void seekdir (DIR *, off_t loc);

int scandir (const char *__dir,
             struct dirent ***__namelist,
             int (*select) (const struct dirent *),
             int (*compar) (const struct dirent **, const struct dirent **));

int alphasort (const struct dirent **__a, const struct dirent **__b);
#endif /* _POSIX_SOURCE */

#endif
