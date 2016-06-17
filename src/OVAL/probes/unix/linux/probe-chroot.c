/*
 * Copyright 2016 Red Hat Inc., Durham, North Carolina.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "probe-chroot.h"
#include <errno.h>
#include "common/debug_priv.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


void probe_chroot_init(struct probe_chroot *ch, const char *scan_path) {
	assert(ch != NULL);

	if (scan_path == NULL) {
		ch->scan_path = NULL;
		ch->root_fd = -1;
	} else {
		// Copy path
		ch->scan_path = oscap_strdup(scan_path);

		// Open descriptor to original root
		// It will be used to unchroot
		ch->root_fd = open("/", O_RDONLY);
		if (ch->root_fd < 0) {
			dE("Unable to open root directory: errno: %d \"%s\"", errno, strerror(errno));
			abort();
		}
	}
}


int probe_chroot_enter(const struct probe_chroot *ch)
{
	assert(ch != NULL);

	// chdir() is not really required for our purposes of chroot()
	// but it could help us with debugging of unsuccessfull chroot
	if (chdir(ch->scan_path) < 0 ) {
		dE("Unable to chdir to '%s', errno: %d \"%s\"", ch->scan_path, errno, strerror(errno));
		return -1;
	}

	if (chroot(ch->scan_path) < 0) {
		dE("Unable to chroot to '%s', errno: %d \"%s\"", ch->scan_path, errno, strerror(errno));
		return -1;
	}

	dD("Probe has just entered to new root: '%s'", ch->scan_path);
	return 0;
}

bool probe_chroot_is_set(const struct probe_chroot *ch)
{
	return (ch->scan_path != NULL);
}

const char *probe_chroot_get_path(const struct probe_chroot *ch)
{
	return ch->scan_path;
}

int probe_chroot_leave(const struct probe_chroot *ch)
{
	assert(ch != NULL);

	if (fchdir(ch->root_fd)) {
		dE("Unable to chdir to host root, errno: %d \"%s\"", errno, strerror(errno));
		return -1;
	}

	if (chroot(".") < 0) {
		dE("Unable to leave chroot environment, errno: %d \"%s\"", errno, strerror(errno));
		return -1;
	}

	dD("Probe has just escaped chroot environment (previous root: '%s')", ch->scan_path);
	return 0;
}


void probe_chroot_free(struct probe_chroot* ch)
{
	assert(ch != NULL);

	if (ch->scan_path == NULL) {
		return;
	}
	close(ch->root_fd);
	ch->root_fd = -1;

	oscap_free(ch->scan_path);
	ch->scan_path = NULL;
}
