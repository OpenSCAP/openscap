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
#if (defined(__SVR4) && defined(__sun)) || defined(_AIX)
#include "fts_sun.h"
#else
#include <fts.h>
#endif
#include <pcre.h>
#include "fsdev.h"

#define ENT_GET_AREF(ent, dst, attr_name, mandatory)			\
	do {								\
		if (((dst) = probe_ent_getattrval(ent, attr_name)) == NULL) { \
			if (mandatory) {				\
				_F("Attribute `%s' is missing!\n", attr_name); \
				return (NULL);				\
			}						\
		}							\
	} while(0)

#define ENT_GET_STRVAL(ent, dst, dstlen, zerolen_exp)			\
	do {							\
		SEXP_t *___r;					\
								\
		if ((___r = probe_ent_getval(ent)) == NULL) {	\
			dW("entity has no value!");		\
			return (NULL);				\
		} else {					\
			if (!SEXP_stringp(___r)) {		\
				_F("invalid type\n");		\
				SEXP_free(___r);		\
				return (NULL);			\
			}					\
			if (SEXP_string_length(___r) == 0) {	\
				SEXP_free(___r);		\
				zerolen_exp;			\
			} else {				\
				SEXP_string_cstr_r(___r, dst, dstlen); \
				SEXP_free(___r);		\
			}					\
		}						\
	} while (0)

typedef struct {
	/* oval_fts_read_match_path() state */
	FTS *ofts_match_path_fts;
	FTSENT *ofts_match_path_fts_ent;
	/* oval_fts_read_recurse_path() state */
	FTS *ofts_recurse_path_fts;
	int ofts_recurse_path_fts_opts;
	int ofts_recurse_path_curdepth;
	char *ofts_recurse_path_pthcpy;
	char *ofts_recurse_path_curpth;
	dev_t ofts_recurse_path_devid;

	pcre       *ofts_path_regex;
	pcre_extra *ofts_path_regex_extra;
	uint32_t ofts_path_op;

	SEXP_t *ofts_spath;
	SEXP_t *ofts_sfilename;
	SEXP_t *ofts_sfilepath;
	SEXP_t *result;

	int max_depth;
	int direction;
	int recurse;
	int filesystem;

	fsdev_t *localdevs;
	const char *prefix;
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
	unsigned int fts_info;
} OVAL_FTSENT;

/*
 * OVAL FTS public API
 */
OVAL_FTS *oval_fts_open(const char *prefix, SEXP_t *path, SEXP_t *filename, SEXP_t *filepath, SEXP_t *behaviors, SEXP_t* result);
OVAL_FTSENT *oval_fts_read(OVAL_FTS *ofts);
int          oval_fts_close(OVAL_FTS *ofts);

void oval_ftsent_free(OVAL_FTSENT *ofts_ent);

#endif /* OVAL_FTS_H */
