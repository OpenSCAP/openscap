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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <assume.h>
#include <pcre.h>
#include "fsdev.h"
#include "_probe-api.h"
#include "probe-entcmp.h"
#include "alloc.h"
#include "debug_priv.h"
#include "oval_fts.h"

static OVAL_FTS *OVAL_FTS_new(char **fts_paths, uint16_t fts_paths_count, int fts_options)
{
	OVAL_FTS *ofts;

	ofts = oscap_talloc(OVAL_FTS);
	ofts->ofts_fts = fts_open((char * const *)fts_paths, fts_options, NULL);

	if (ofts->ofts_fts == NULL) {
		dE("fts_open(%p, %d, NULL) failed: errno=%d\n", fts_paths, fts_options, errno);
		oscap_free(ofts);
		return (NULL);
	}

	ofts->ofts_st_path       = fts_paths;
	ofts->ofts_st_path_count = fts_paths_count;
	ofts->ofts_st_path_index = 0;

	ofts->ofts_path      = NULL;
	ofts->ofts_filename  = NULL;
	ofts->ofts_behaviors = NULL;

	ofts->ofts_path_regex       = NULL;
	ofts->ofts_path_regex_extra = NULL;

	ofts->max_depth  = -1;
	ofts->direction  = -1;
	ofts->filesystem = -1;
	ofts->localdevs  = NULL;

	ofts->ofts_nilfilename = false;

	return (ofts);
}

static void OVAL_FTS_free(OVAL_FTS *ofts)
{
	if (ofts->ofts_fts != NULL)
		fts_close(ofts->ofts_fts);

	if (ofts->ofts_st_path != NULL)
		dW("ofts_st_path != NULL (%p)\n", ofts->ofts_st_path);

	oscap_free(ofts);
	return;
}

static OVAL_FTSENT *OVAL_FTSENT_new(OVAL_FTS *ofts, FTSENT *fts_ent)
{
	OVAL_FTSENT *ofts_ent;

	ofts_ent = oscap_talloc(OVAL_FTSENT);

	ofts_ent->filepath     = strdup(fts_ent->fts_path);
	ofts_ent->filepath_len = fts_ent->fts_pathlen;

	if (!ofts->ofts_nilfilename) {
		ofts_ent->file     = ofts_ent->filepath + (fts_ent->fts_pathlen - fts_ent->fts_namelen);
		ofts_ent->file_len = fts_ent->fts_namelen;

		assume_d((fts_ent->fts_pathlen - fts_ent->fts_namelen > 0), NULL);

		ofts_ent->path_len = fts_ent->fts_pathlen - fts_ent->fts_namelen;
		ofts_ent->path     = oscap_alloc(sizeof(char) * (ofts_ent->path_len + 1));

		strncpy(ofts_ent->path, ofts_ent->filepath, ofts_ent->path_len);
		ofts_ent->path[ofts_ent->path_len - 1] = '\0';
	} else {
		ofts_ent->path_len = fts_ent->fts_pathlen;
		ofts_ent->path     = strdup(fts_ent->fts_path);

		ofts_ent->file     = "";
		ofts_ent->file_len = 0;
	}

	dI("\n"
	   "New OVAL_FTSENT:\n"
	   "\tfilepath: %s\n"
	   "\t    file: %s\n"
	   "\t    path: %s\n", ofts_ent->filepath, ofts_ent->file, ofts_ent->path);

	ofts_ent->nilfile = ofts->ofts_nilfilename;

	return (ofts_ent);
}

static void OVAL_FTSENT_free(OVAL_FTSENT *ofts_ent)
{
	oscap_free(ofts_ent->filepath);
	oscap_free(ofts_ent->path);
	oscap_free(ofts_ent);
	return;
}

static bool OVAL_FTS_localp(OVAL_FTS *ofts, const char *path, void *id)
{
	if (id != NULL)
		return (fsdev_search(ofts->localdevs, id) == 1 ? true : false);
	else if (path != NULL)
		return (fsdev_path(ofts->localdevs, path) == 1 ? true : false);
	else
		return (false);
}

OVAL_FTS *oval_fts_open(SEXP_t *path, SEXP_t *filename, SEXP_t *filepath, SEXP_t *behaviors)
{
	OVAL_FTS *ofts;

	char cstr_path[PATH_MAX+1];
	char cstr_file[PATH_MAX+1];
	char cstr_buff[32];

	SEXP_t *r0;

	int fts_options = FTS_LOGICAL;
	int max_depth;
	int direction;
	int recurse;
	int filesystem;

	uint32_t path_op;
	bool nilfilename = false;

#define ENT_GET_AREF(ent, dst, attr_name, mandatory)			\
	do {								\
		if (((dst) = probe_ent_getattrval(ent, attr_name)) == NULL) { \
			if (mandatory) {				\
				dE("Attribute `%s' is missing!\n", attr_name); \
				return (NULL);				\
			}						\
		}							\
	} while(0)

	/* path & filename / filepath */
	if (filepath == NULL) {
		assume_d(path     != NULL, NULL);
		assume_d(filename != NULL, NULL);

		ENT_GET_AREF(path, r0, "operation", false);

		if (r0 != NULL) {
			path_op = SEXP_number_getu(r0);
			SEXP_free(r0);

			switch (path_op) {
			case OVAL_OPERATION_EQUALS:
			case OVAL_OPERATION_PATTERN_MATCH:
				/* XXX: add more operations */
				break;
			default:
				dE("Invalid operation: %u\n", path_op);
			}
		} else
			path_op = OVAL_OPERATION_EQUALS;

		dI("path_op: %u\n", path_op);

#define ENT_GET_STRVAL(ent, dst, dstlen, novalue_exp)			\
		do {							\
			SEXP_t *___r;					\
									\
			if ((___r = probe_ent_getval(ent)) == NULL) {	\
				dW("entity has no value!\n");		\
				novalue_exp;				\
			} else {					\
				if (!SEXP_stringp(___r)) {		\
					dE("invalid type\n");		\
					SEXP_free(___r);		\
					return (NULL);			\
				}					\
									\
				SEXP_string_cstr_r(___r, dst, dstlen);	\
				SEXP_free(___r);			\
			}						\
		} while (0)

		ENT_GET_STRVAL(path,     cstr_path, sizeof cstr_path - 1, return NULL);
		ENT_GET_STRVAL(filename, cstr_file, sizeof cstr_file - 1, nilfilename = true);

		dI("\n"
		   "    path: %s\n",
		   "filename: %s\n", cstr_path, cstr_file);
	} else {
		assume_d(filepath != NULL, NULL);
		dE("filepath entity is not currently supported.\n");
		return (NULL);
	}

	assume_d(behaviors != NULL, NULL);

	/* max_depth */
	ENT_GET_AREF(behaviors, r0, "max_depth", true);
	SEXP_string_cstr_r(r0, cstr_buff, sizeof cstr_buff - 1);

	max_depth = strtol(cstr_buff, NULL, 10);

	if (errno == EINVAL || errno == ERANGE) {
		dE("Invalid value of the `%s' attribute: %s\n", "recurse_direction", cstr_buff);
		SEXP_free(r0);
		return (NULL);
	}

	dI("\n"
	   "bh.max_depth: %s\n"
	   "=> max_depth: %d\n", cstr_buff, max_depth);
	SEXP_free(r0);

	/* recurse_direction */
	ENT_GET_AREF(behaviors, r0, "recurse_direction", true);
	SEXP_string_cstr_r(r0, cstr_buff, sizeof cstr_buff - 1);

	if (strcmp(cstr_buff, "none") == 0)
		direction = OVAL_RECURSE_DIRECTION_NONE;
	else if (strcmp(cstr_buff, "down") == 0)
		direction = OVAL_RECURSE_DIRECTION_DOWN;
	else if (strcmp(cstr_buff, "up") == 0)
		direction = OVAL_RECURSE_DIRECTION_UP;
	else {
		dE("Invalid direction: %s\n", cstr_buff);
		SEXP_free(r0);
		return (NULL);
	}

	dI("\n"
	   "bh.direction: %s\n"
	   "=> direction: %d\n", cstr_buff, direction);
	SEXP_free(r0);

	/* recurse */
	ENT_GET_AREF(behaviors, r0, "recurse", false);

	if (r0 != NULL) {
		SEXP_string_cstr_r(r0, cstr_buff, sizeof cstr_buff - 1);

		if (strcmp(cstr_buff, "symlinks and directories") == 0)
			recurse = OVAL_RECURSE_SYMLINKS_AND_DIRS;
		else if (strcmp(cstr_buff, "files and directories") == 0)
			recurse = OVAL_RECURSE_FILES_AND_DIRS;
		else if (strcmp(cstr_buff, "symlinks") == 0)
			recurse = OVAL_RECURSE_SYMLINKS;
		else if (strcmp(cstr_buff, "directories") == 0)
			recurse = OVAL_RECURSE_DIRS;
		else {
			dE("Invalid recurse: %s\n", cstr_buff);
			SEXP_free(r0);
			return (NULL);
		}
	} else
		recurse = OVAL_RECURSE_SYMLINKS_AND_DIRS;

	dI("\n"
	   "bh.recurse: %s\n"
	   "=> recurse: %d\n", cstr_buff, recurse);
	SEXP_free(r0);

	/* recurse_file_system */
	ENT_GET_AREF(behaviors, r0, "recurse_file_system", false);

	if (r0 != NULL) {
		SEXP_string_cstr_r(r0, cstr_buff, sizeof cstr_buff - 1);

		if (strcmp(cstr_buff, "local") == 0)
			filesystem = OVAL_RECURSE_FS_LOCAL;
		else if (strcmp(cstr_buff, "all") == 0)
			filesystem = OVAL_RECURSE_FS_ALL;
		else if (strcmp(cstr_buff, "defined") == 0) {
			filesystem   = OVAL_RECURSE_FS_DEFINED;
			fts_options |= FTS_XDEV;
		} else {
			dE("Invalid recurse filesystem: %s\n", cstr_buff);
			SEXP_free(r0);
			return (NULL);
		}
	} else
		filesystem = OVAL_RECURSE_FS_ALL;

	dI("\n"
	   "bh.filesystem: %s\n",
	   "=> filesystem: %d\n", cstr_buff, filesystem);
	SEXP_free(r0);

	if (path_op == OVAL_OPERATION_PATTERN_MATCH) {
		pcre  *regex;

		regex = pcre_compile(cstr_path, 0, NULL, NULL, NULL);

		if (regex == NULL) {
			dE("pcre_compile(%s) failed\n", cstr_path);
			return (NULL);
		} else {
			char **paths = oscap_alloc(sizeof(char *) * 2);

			/* analyze */
			/* guess starting points */
			/* fts open */

			paths[0] = strdup("/");
			paths[1] = NULL;

			ofts = OVAL_FTS_new (paths, 1, fts_options);

			ofts->ofts_st_path = paths; /* NULL-terminated array of starting paths */
			ofts->ofts_st_path_count = 1;
			ofts->ofts_st_path_index = 0;

			ofts->ofts_path_regex = regex;
			ofts->ofts_path_regex_extra = NULL;
		}
	} else {
		char **paths = oscap_alloc(sizeof(char *) * 2);

		paths[0] = strdup(cstr_path);
		paths[1] = NULL;

		ofts = OVAL_FTS_new (paths, 1, fts_options);

		ofts->ofts_st_path = paths; /* NULL-terminated array of starting paths */
		ofts->ofts_st_path_count = 1;
		ofts->ofts_st_path_index = 0;
	}

	if (filesystem == OVAL_RECURSE_FS_LOCAL) {
		ofts->localdevs = fsdev_init(NULL, 0);

		if (ofts->localdevs == NULL) {
			dE("fsdev_init failed\n");
			return (NULL);
		}
	}

	ofts->ofts_path      = SEXP_ref(path); /* path entity */
	ofts->ofts_filename  = SEXP_ref(filename); /* filename entity */
	ofts->ofts_behaviors = SEXP_ref(behaviors); /* behaviors entity */

	ofts->max_depth  = max_depth;
	ofts->direction  = direction;
	ofts->recurse    = recurse;
	ofts->filesystem = filesystem;

	ofts->ofts_nilfilename = nilfilename;

	return (ofts);
}

OVAL_FTSENT *oval_fts_read(OVAL_FTS *ofts)
{
	SEXP_t *sexp_filename;
	OVAL_FTSENT *ofts_ent = NULL;

	dI("ofts=%p\n", ofts);

	if (ofts != NULL) {
		register FTSENT *fts_ent;

		for (;;) {
			fts_ent = fts_read(ofts->ofts_fts);

			if (fts_ent == NULL)
				return (NULL);

			switch (fts_ent->fts_info) {
			case FTS_DP:
				continue;
			case FTS_DC:
				dW("Filesystem tree cycle detected at %s\n", fts_ent->fts_path);
				continue;
			}

			dI("fts_ent: p=%s, n=%s\n", fts_ent->fts_path, fts_ent->fts_name);

			/*
			 * path match?
			 */
			if (ofts->ofts_path_regex != NULL) {
				int ret, svec[3];

				ret = pcre_exec(ofts->ofts_path_regex, ofts->ofts_path_regex_extra,
						fts_ent->fts_path, fts_ent->fts_pathlen - fts_ent->fts_namelen,
						0, 0, svec, 1);

				if (ret < 1)
					continue; /* next cycle */
			}

			dI("path match\n");

			if (!ofts->ofts_nilfilename) {
				if (fts_ent->fts_info != FTS_D) {
					/*
					 * filename match?
					 * XXX: This is not efficient! Implement probe_entobj_cmp_cstr().
					 */
					sexp_filename = SEXP_string_newf("%s", fts_ent->fts_name);

					if (probe_entobj_cmp(ofts->ofts_filename, sexp_filename) == OVAL_RESULT_TRUE)
						ofts_ent = OVAL_FTSENT_new(ofts, fts_ent);

					SEXP_free(sexp_filename);
				}
			} else
				ofts_ent = OVAL_FTSENT_new(ofts, fts_ent);

			switch (ofts->direction) {
			case OVAL_RECURSE_DIRECTION_NONE:
				if (fts_ent->fts_level != 0) {
					fts_set(ofts->ofts_fts, fts_ent, FTS_SKIP);
					dI("FTS_SKIP: %s\n", fts_ent->fts_path);
				} else
					dI("Not skipping FTS_ROOT: %s\n", fts_ent->fts_path);
				break;
			case OVAL_RECURSE_DIRECTION_DOWN:
				if (fts_ent->fts_level < ofts->max_depth || ofts->max_depth == -1) {
					/*
					 * Check file type & filesystem recursion.
					 *  `defined' is handled by fts (FTS_XDEV)
					 *  `all' => we don't care
					 *  `local' => the only case we need to handle here
					 */
					switch(fts_ent->fts_info) {
					case FTS_D: /* directory */
						if (!(ofts->recurse & OVAL_RECURSE_DIRS))
							goto __skip_file;
						break;
					case FTS_SL: /* symbolic link */
						if (!(ofts->recurse & OVAL_RECURSE_SYMLINKS))
							goto __skip_file;
						break;
					default:
						/*
						 * No need to check filesystem recursion for other
						 * types of files.
						 */
						goto __case_end;
					}

					if (ofts->filesystem == OVAL_RECURSE_FS_LOCAL) {
						switch (fts_ent->fts_info) {
						case FTS_D:
						case FTS_SL:
							/*
							 * Check whether the filesystem mounted at
							 * the symlink/directory destination is a
							 * local one.
							 */
							if (!OVAL_FTS_localp(ofts, fts_ent->fts_path,
									     fts_ent->fts_statp != NULL ?
									     &fts_ent->fts_statp->st_dev : NULL))
							{
								dI("not on local fs: %s\n", fts_ent->fts_path);
								goto __skip_file;
							}
							break;
						case FTS_SLNONE:
						default:
							/* It's a regular file or something we don't care about */
							break;
						}
					}

					fts_set(ofts->ofts_fts, fts_ent, FTS_FOLLOW);
					dI("FTS_FOLLOW: %s\n", fts_ent->fts_path);
				} else {
				__skip_file:
					fts_set(ofts->ofts_fts, fts_ent, FTS_SKIP);
					dI("FTS_SKIP: %s\n", fts_ent->fts_path);
				}
			__case_end:
				break;
			case OVAL_RECURSE_DIRECTION_UP: /* is this really useful? */
				fts_set(ofts->ofts_fts, fts_ent, FTS_SKIP);
				dI("FTS_SKIP: %s\n", fts_ent->fts_path);
				break;
			} /* switch(recurse_direction */

			if (ofts_ent != NULL)
				break;
		} /* for(;;) */
	} /* ofts != NULL */

	return (ofts_ent);
}

void oval_ftsent_free(OVAL_FTSENT *ofts_ent)
{
	OVAL_FTSENT_free(ofts_ent);
}

int oval_fts_close(OVAL_FTS *ofts)
{
	register uint16_t i;

	if (ofts->ofts_st_path_count > 0) {
		for (i = 0; i < ofts->ofts_st_path_count; ++i)
			oscap_free(ofts->ofts_st_path[i]);
		oscap_free(ofts->ofts_st_path);
	}

	ofts->ofts_st_path = NULL;

	if (ofts->ofts_filename != NULL)
		SEXP_free(ofts->ofts_filename);
	if (ofts->ofts_behaviors != NULL)
		SEXP_free(ofts->ofts_behaviors);
	if (ofts->ofts_path != NULL)
		SEXP_free(ofts->ofts_path);

	OVAL_FTS_free(ofts);

	return (0);
}
