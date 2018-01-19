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
#ifndef __PROBE_CHROOT__
#define __PROBE_CHROOT__

#include "common/util.h"


struct probe_chroot {
	char* scan_path;
	int root_fd;
};

/**
 * Init structure
 * @param ch should be allocated
 * @param scan_path will be duplicated, can be NULL
 * No chroot will be executed then
 */
void probe_chroot_init(struct probe_chroot *ch, const char *scan_path);

/**
 * Enter to chroot environment of target
 */
int probe_chroot_enter(const struct probe_chroot *ch);

/**
 * Leave chroot environment - restore original root
 */
int probe_chroot_leave(const struct probe_chroot *ch);

/**
 * Check if scan path is set
 * @return true if scan path != NULL
 */
bool probe_chroot_is_set(const struct probe_chroot *ch);

/**
 * Return path to scan
 * @return can return NULL
 */
const char *probe_chroot_get_path(const struct probe_chroot *ch);

/**
 * Free structures in probe_chroot
 * @param probe_chroot Memory of probe_chroot is not free'ed by this function
 */
void probe_chroot_free(struct probe_chroot *ch);


#endif
