
/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#pragma once
#ifndef FSDEV_H
#define FSDEV_H

#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>

typedef struct {
	dev_t *ids;
	uint16_t cnt;
} fsdev_t;

fsdev_t *fsdev_init(const char **fs, size_t fs_cnt);
fsdev_t *fsdev_strinit(const char *fs_names);
void fsdev_free(fsdev_t * lfs);

int fsdev_search(fsdev_t * lfs, void *id);
int fsdev_path(fsdev_t * lfs, const char *path);
int fsdev_fd(fsdev_t * lfs, int fd);

#endif				/* FSDEV_H */
