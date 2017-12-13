/**
 * @file   fsdev.h
 * @brief  fsdev API public header file
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * @addtogroup PROBEAUXAPI
 * @{
 */
/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
#include "oscap_export.h"

/**
 * Filesystem device structure.
 */
typedef struct {
	dev_t *ids;   /**< Sorted array of device ids   */
	uint16_t cnt; /**< Number of items in the array */
} fsdev_t;

/**
 * Initialize the fsdev_t structure from an array of filesystem
 * names.
 */
OSCAP_API fsdev_t *fsdev_init(const char **fs, size_t fs_cnt);

/**
 * Initialize the fsdev_t structure from a string containing filesystem
 * names.
 */
OSCAP_API fsdev_t *fsdev_strinit(const char *fs_names);

/**
 * Free the fsdev_t structure.
 */
OSCAP_API void fsdev_free(fsdev_t * lfs);

/**
 * Search an id in the fsdev_t structure.
 */
OSCAP_API int fsdev_search(fsdev_t * lfs, void *id);

/**
 * Check whether a path points points to a place on any of the devices
 * in the fsdev_t structure.
 * @param lfs pointer to the fsdev_t structure
 * @param path path
 * @retval 1 if found
 * @retval 0 otherwise
 * @retval -1 error
 */
OSCAP_API int fsdev_path(fsdev_t * lfs, const char *path);

/**
 * Check whether a file descriptor is associated with a file that resides
 * on any of the devices in the fsdev_t structure.
 * @param lfs pointer to the fsdev_t structure
 * @param fd file descriptor
 * @retval 1 if found
 * @retval 0 otherwise
 * @retval -1 error
 */
OSCAP_API int fsdev_fd(fsdev_t * lfs, int fd);

#endif				/* FSDEV_H */
/// @}
