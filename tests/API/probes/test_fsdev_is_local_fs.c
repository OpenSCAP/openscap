/*
 * Copyright 2019 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      "Jan Černý" <jcerny@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>

#if defined(OS_LINUX)
#include <mntent.h>

/* __fsdev_init reads the mount table that mtab names. */
#undef _PATH_MOUNTED
static const char *mtab;
#define _PATH_MOUNTED mtab
#endif

#include "fsdev.h"
#include "fsdev.c"

#if defined(OS_LINUX) || defined(OS_AIX)
#include <mntent.h>

static int test_single_call()
{
	struct mntent ment = {0};
	ment.mnt_type = "autofs";
	int ret = is_local_fs(&ment);
	/* autofs entry is never considered local */
	return (ret == 0);
}

static int test_multiple_calls(const char *fake_mtab)
{
	/*
	 * fake mtab contains only 3 local filesystems:
	 * /, /tmp, and /home
	 */
	FILE *f = setmntent(fake_mtab, "r");
	if (f == NULL) {
		fprintf(stderr, "fake_mtab could not be open\n");
		return 0;
	}
	struct mntent *ment;
	unsigned int locals = 0;
	while ((ment = getmntent(f)) != NULL) {
		if (is_local_fs(ment)) {
			locals++;
		}
	}
	endmntent(f);
	return (locals == 3);
}

#if defined(OS_LINUX)
/*
 * read_table returns the devices that __fsdev_init finds in the mount
 * table at path, in the order of the table, or NULL with errno set.
 */
static fsdev_t *read_table(const char *path)
{
	fsdev_t *lfs = malloc(sizeof(fsdev_t));

	if (lfs == NULL)
		return NULL;
	mtab = path;
	return __fsdev_init(lfs);
}

/*
 * A missing mount table gives the devices of /proc/self/mounts, which proc
 * holds. fake_mtab gives its own: those of /, /tmp and /home that exist
 * here, / first.
 */
static int test_mount_table(const fsdev_t *proc, const char *fake_mtab)
{
	fsdev_t *lfs;
	struct stat st;
	int ok;

	lfs = read_table("test_fsdev_is_local_fs.missing");
	if (lfs == NULL) {
		fprintf(stderr, "missing mount table: %s\n", strerror(errno));
		return 0;
	}
	ok = lfs->cnt == proc->cnt &&
		(lfs->cnt == 0 || memcmp(lfs->ids, proc->ids, sizeof(dev_t) * lfs->cnt) == 0);
	if (!ok)
		fprintf(stderr, "missing mount table: %u devices, /proc/self/mounts gives %u\n",
			(unsigned)lfs->cnt, (unsigned)proc->cnt);
	fsdev_free(lfs);
	if (!ok)
		return 0;

	lfs = read_table(fake_mtab);
	if (lfs == NULL) {
		fprintf(stderr, "%s: %s\n", fake_mtab, strerror(errno));
		return 0;
	}
	ok = lfs->cnt >= 1 && lfs->cnt <= 3 && stat("/", &st) == 0 && lfs->ids[0] == st.st_dev;
	if (!ok)
		fprintf(stderr, "%s: %u devices, want 1 to 3, that of / first\n",
			fake_mtab, (unsigned)lfs->cnt);
	fsdev_free(lfs);
	return ok;
}
#endif

int main(int argc, char *argv[])
{
	if (!test_single_call()) {
		fprintf(stderr, "test_single_call has failed\n");
		return 1;
	}
	if (!test_multiple_calls(argv[1])) {
		fprintf(stderr, "test_multiple_calls has failed\n");
		return 1;
	}
#if defined(OS_LINUX)
	fsdev_t *proc = read_table("/proc/self/mounts");
	if (proc == NULL) {
		fprintf(stderr, "/proc/self/mounts: %s: skipping test_mount_table\n", strerror(errno));
		return 255;
	}
	int ok = test_mount_table(proc, argv[1]);
	fsdev_free(proc);
	if (!ok) {
		fprintf(stderr, "test_mount_table has failed\n");
		return 1;
	}
#endif
	return 0;
}

#else

int main(int argc, char *argv[])
{
	fprintf(stderr, "is_local is only defined and used by Linux and AIX\n");
	return 0;
}

#endif
