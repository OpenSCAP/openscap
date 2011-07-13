/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
#ifndef OVAL_FTS_H
#define OVAL_FTS_H

#include <sexp.h>
#if defined(__SVR4) && defined(__sun)
#include "fts_sun.h"
#else
#include <fts.h>
#endif
#include <pcre.h>
#include "fsdev.h"

typedef struct {
	FTS    *ofts_fts;

	char   **ofts_st_path;       /**< starting paths - NULL terminated */
	uint16_t ofts_st_path_count; /**< number of starting paths */
	uint16_t ofts_st_path_index; /**< current starting path */

	pcre       *ofts_path_regex;
	pcre_extra *ofts_path_regex_extra;
	uint32_t ofts_path_op;

	SEXP_t *ofts_spath;
	SEXP_t *ofts_sfilename;
	SEXP_t *ofts_sfilepath;

	int max_depth;
	int direction;
	int recurse;
	int filesystem;

	fsdev_t *localdevs;
} OVAL_FTS;

#define OVAL_RECURSE_DIRECTION_NONE 0 /* default */
#define OVAL_RECURSE_DIRECTION_DOWN 1
#define OVAL_RECURSE_DIRECTION_UP   2

#define OVAL_RECURSE_FILES    0x01
#define OVAL_RECURSE_DIRS     0x02
#define OVAL_RECURSE_SYMLINKS 0x04

#define OVAL_RECURSE_SYMLINKS_AND_DIRS (OVAL_RECURSE_SYMLINKS|OVAL_RECURSE_DIRS) /* default */
#define OVAL_RECURSE_FILES_AND_DIRS    (OVAL_RECURSE_FILES|OVAL_RECURSE_SYMLINKS)

#define OVAL_RECURSE_FS_LOCAL   0
#define OVAL_RECURSE_FS_DEFINED 1
#define OVAL_RECURSE_FS_ALL     2 /* default */

typedef struct {
	char *file;
	size_t file_len;
	char *path;
	size_t path_len;
} OVAL_FTSENT;

/*
 * OVAL FTS public API
 */
OVAL_FTS    *oval_fts_open(SEXP_t *path, SEXP_t *filename, SEXP_t *filepath, SEXP_t *behaviors);
OVAL_FTSENT *oval_fts_read(OVAL_FTS *ofts);
int          oval_fts_close(OVAL_FTS *ofts);

void oval_ftsent_free(OVAL_FTSENT *ofts_ent);

#endif /* OVAL_FTS_H */
