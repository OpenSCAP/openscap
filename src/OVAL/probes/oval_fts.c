/*
 * Copyright 2010-2011 Red Hat Inc., Durham, North Carolina.
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
 *      "Tomas Heinrich" <theinric@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <assume.h>
#include <pcre.h>
#include <libgen.h>

#include "fsdev.h"
#include "_probe-api.h"
#include "probe/entcmp.h"
#include "alloc.h"
#include "debug_priv.h"
#include "oval_fts.h"
#if defined(__SVR4) && defined(__sun)
#include "fts_sun.h"
#else
#include <fts.h>
#endif

#undef OSCAP_FTS_DEBUG

static OVAL_FTS *OVAL_FTS_new()
{
	OVAL_FTS *ofts;

	ofts = oscap_talloc(OVAL_FTS);
	memset(ofts, 0, sizeof(*ofts));

	ofts->max_depth  = -1;
	ofts->direction  = -1;
	ofts->filesystem = -1;

	return (ofts);
}

static void OVAL_FTS_free(OVAL_FTS *ofts)
{
	if (ofts->ofts_match_path_fts != NULL)
		fts_close(ofts->ofts_match_path_fts);
	if (ofts->ofts_recurse_path_fts != NULL)
		fts_close(ofts->ofts_recurse_path_fts);

	oscap_free(ofts);
	return;
}

static int pathlen_from_ftse(int fts_pathlen, int fts_namelen)
{
	int pathlen;

	if (fts_pathlen > fts_namelen) {
		pathlen = fts_pathlen - fts_namelen;
		if (pathlen > 1)
			pathlen--; /* strip last slash */
	} else {
		pathlen = fts_pathlen;
	}

	return pathlen;
}

static OVAL_FTSENT *OVAL_FTSENT_new(OVAL_FTS *ofts, FTSENT *fts_ent)
{
	OVAL_FTSENT *ofts_ent;

	ofts_ent = oscap_talloc(OVAL_FTSENT);

	ofts_ent->fts_info = fts_ent->fts_info;
	if (ofts->ofts_sfilename || ofts->ofts_sfilepath) {
		ofts_ent->path_len = pathlen_from_ftse(fts_ent->fts_pathlen, fts_ent->fts_namelen);
		ofts_ent->path = oscap_alloc(ofts_ent->path_len + 1);
		strncpy(ofts_ent->path, fts_ent->fts_path, ofts_ent->path_len);
		ofts_ent->path[ofts_ent->path_len] = '\0';

		ofts_ent->file_len = fts_ent->fts_namelen;
		ofts_ent->file = strdup(fts_ent->fts_name);
	} else {
		ofts_ent->path_len = fts_ent->fts_pathlen;
		ofts_ent->path = strdup(fts_ent->fts_path);

		ofts_ent->file_len = -1;
		ofts_ent->file = NULL;
	}

#if defined(OSCAP_FTS_DEBUG)
	dI("\n"
	   "New OVAL_FTSENT:\n"
	   "\t    file: '%s'.\n"
	   "\t    path: '%s'.\n", ofts_ent->file, ofts_ent->path);
#endif
	return (ofts_ent);
}

static void OVAL_FTSENT_free(OVAL_FTSENT *ofts_ent)
{
	oscap_free(ofts_ent->path);
	oscap_free(ofts_ent->file);
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

static char *__regex_locate(char *str)
{
    char *regex_sch = "^*?$.(["; /*<< regex start chars */
    bool  escaped = false;

    while (*str != '\0') {
	if (*str == '\\')
	    escaped = !escaped;
	else if (strchr(regex_sch, *str) != NULL) {
	    if (!escaped)
		return (str);
	    else
		escaped = false;
	}
	++str;
    }

    return (str);
}

static char *__string_unescape(char *str, size_t len)
{
    char *ret_str;
    size_t i, j;

    if (str == NULL || len == 0)
        return NULL;

    ret_str = strndup(str, len);

    if (ret_str == NULL)
        return NULL;

    for (i = j = 0; i < len && j <= i; ++i) {
        if (str[i] == '\\') {
            if (str[i+1] == '\0') {
                free(ret_str);
                return NULL;
            }
            ret_str[j] = str[i+1];
            ++j;
            ++i;
        } else {
            ret_str[j] = str[i];
            ++j;
        }
    }

    ret_str[j] = '\0';

    return ret_str;
}

OVAL_FTS *oval_fts_open(SEXP_t *path, SEXP_t *filename, SEXP_t *filepath, SEXP_t *behaviors)
{
	OVAL_FTS *ofts;

	char cstr_path[PATH_MAX+1];
	char cstr_file[PATH_MAX+1];
	char cstr_buff[32];
	char **paths;

	SEXP_t *r0;

	int mtc_fts_options = FTS_PHYSICAL | FTS_COMFOLLOW | FTS_NOCHDIR;
	int rec_fts_options = FTS_PHYSICAL | FTS_COMFOLLOW | FTS_NOCHDIR;
	int max_depth   = -1;
	int direction   = -1;
	int recurse     = -1;
	int filesystem  = -1;

	uint32_t path_op;
	bool nilfilename = false;

	assume_d((path == NULL && filename == NULL && filepath != NULL)
		 || (path != NULL && filepath == NULL), NULL);
	assume_d(behaviors != NULL, NULL);

	if (path)
		PROBE_ENT_AREF(path, r0, "operation", /**/);
	else
		PROBE_ENT_AREF(filepath, r0, "operation", /**/);

	if (r0 != NULL) {
		path_op = SEXP_number_getu(r0);
		SEXP_free(r0);
	} else {
		path_op = OVAL_OPERATION_EQUALS;
	}
#if defined(OSCAP_FTS_DEBUG)
	dI("path_op: %u, '%s'.\n", path_op, oval_operation_get_text(path_op));
#endif
	if (path) { /* filepath == NULL */
		PROBE_ENT_STRVAL(path, cstr_path, sizeof cstr_path,
				 return NULL;, return NULL;);
		if (probe_ent_getvals(filename, NULL) == 0) {
			nilfilename = true;
		} else {
			PROBE_ENT_STRVAL(filename, cstr_file, sizeof cstr_file,
					 return NULL;, /* noop */;);
		}
#if defined(OSCAP_FTS_DEBUG)
		dI("\n"
		   "        path: '%s'.\n"
		   "    filename: '%s'.\n"
		   "nil filename: %d.\n", cstr_path, nilfilename ? "" : cstr_file, nilfilename);
#endif
		/* max_depth */
		PROBE_ENT_AREF(behaviors, r0, "max_depth", return NULL;);
		SEXP_string_cstr_r(r0, cstr_buff, sizeof cstr_buff - 1);
		max_depth = strtol(cstr_buff, NULL, 10);
		if (errno == EINVAL || errno == ERANGE) {
			dE("Invalid value of the `%s' attribute: %s\n", "recurse_direction", cstr_buff);
			SEXP_free(r0);
			return (NULL);
		}
#if defined(OSCAP_FTS_DEBUG)
		dI("bh.max_depth: %s => max_depth: %d\n", cstr_buff, max_depth);
#endif
		SEXP_free(r0);

		/* recurse_direction */
		PROBE_ENT_AREF(behaviors, r0, "recurse_direction", return NULL;);
		SEXP_string_cstr_r(r0, cstr_buff, sizeof cstr_buff - 1);
		/* todo: use oscap_string_to_enum() */
		if (strcmp(cstr_buff, "none") == 0) {
			direction = OVAL_RECURSE_DIRECTION_NONE;
		} else if (strcmp(cstr_buff, "down") == 0) {
			direction = OVAL_RECURSE_DIRECTION_DOWN;
		} else if (strcmp(cstr_buff, "up") == 0) {
			direction = OVAL_RECURSE_DIRECTION_UP;
		} else {
			dE("Invalid direction: %s\n", cstr_buff);
			SEXP_free(r0);
			return (NULL);
		}
#if defined(OSCAP_FTS_DEBUG)
		dI("bh.direction: %s => direction: %d\n", cstr_buff, direction);
#endif
		SEXP_free(r0);

		/* recurse */
		PROBE_ENT_AREF(behaviors, r0, "recurse", /**/);
		if (r0 != NULL) {
			SEXP_string_cstr_r(r0, cstr_buff, sizeof cstr_buff - 1);
			/* todo: use oscap_string_to_enum() */
			if (strcmp(cstr_buff, "symlinks and directories") == 0) {
				recurse = OVAL_RECURSE_SYMLINKS_AND_DIRS;
			} else if (strcmp(cstr_buff, "files and directories") == 0) {
				recurse = OVAL_RECURSE_FILES_AND_DIRS;
			} else if (strcmp(cstr_buff, "symlinks") == 0) {
				recurse = OVAL_RECURSE_SYMLINKS;
			} else if (strcmp(cstr_buff, "directories") == 0) {
				recurse = OVAL_RECURSE_DIRS;
			} else {
				dE("Invalid recurse: %s\n", cstr_buff);
				SEXP_free(r0);
				return (NULL);
			}
		} else {
			recurse = OVAL_RECURSE_SYMLINKS_AND_DIRS;
		}
#if defined(OSCAP_FTS_DEBUG)
		dI("bh.recurse: %s => recurse: %d\n", cstr_buff, recurse);
#endif
		SEXP_free(r0);

		/* recurse_file_system */
		PROBE_ENT_AREF(behaviors, r0, "recurse_file_system", /**/);

		if (r0 != NULL) {
			SEXP_string_cstr_r(r0, cstr_buff, sizeof cstr_buff - 1);
			/* todo: use oscap_string_to_enum() */
			if (strcmp(cstr_buff, "local") == 0) {
				filesystem = OVAL_RECURSE_FS_LOCAL;
			} else if (strcmp(cstr_buff, "all") == 0) {
				filesystem = OVAL_RECURSE_FS_ALL;
			} else if (strcmp(cstr_buff, "defined") == 0) {
				filesystem = OVAL_RECURSE_FS_DEFINED;
				rec_fts_options |= FTS_XDEV;
			} else {
				dE("Invalid recurse filesystem: %s\n", cstr_buff);
				SEXP_free(r0);
				return (NULL);
			}
		} else {
			filesystem = OVAL_RECURSE_FS_ALL;
		}
#if defined(OSCAP_FTS_DEBUG)
		dI("bh.filesystem: %s => filesystem: %d\n", cstr_buff, filesystem);
#endif
		SEXP_free(r0);
	} else { /* filepath != NULL */
		PROBE_ENT_STRVAL(filepath, cstr_path, sizeof cstr_path, return NULL;, return NULL;);
	}

	paths = oscap_alloc(sizeof(char *) * 2);
	paths[0] = NULL;

	switch (path_op)
	    {
	    case OVAL_OPERATION_EQUALS:
		paths[0] = strdup(cstr_path);
		break;
	    case OVAL_OPERATION_PATTERN_MATCH:
		if (strlen(cstr_path) >= 2) {
                    char *regex_loc;

                    if (cstr_path[0] == '^' &&
                        (regex_loc = __regex_locate(cstr_path + 1)) != NULL)
                    {
                        char *slash_loc = regex_loc - 1;

                        while(slash_loc > cstr_path + 2) {
                            if (*slash_loc == '/') {
                                paths[0] = __string_unescape(cstr_path + 1, (size_t)(slash_loc - cstr_path) - 1);
                                if (paths[0] != NULL) {
                                    if (paths[0][0] != '/') {
                                        free(paths[0]);
                                        paths[0] = NULL;
                                    }
                                    break;
                                }
                            }
                            --slash_loc;
                        }
                    }
                }

		if (paths[0] == NULL)
		    paths[0] = strdup("/");

		break;
	    default:
		paths[0] = strdup("/");
	    }

	dI("fts_open args: path: \"%s\", options: %d.\n", paths[0], mtc_fts_options);

	paths[1] = NULL;
	ofts = OVAL_FTS_new();

	ofts->ofts_match_path_fts = fts_open((char * const *) paths, mtc_fts_options, NULL);
	if (ofts->ofts_match_path_fts == NULL || errno != 0) {
		dE("fts_open() failed, errno: %d \"%s\".\n", errno, strerror(errno));
		OVAL_FTS_free(ofts);
		return (NULL);
	}

	ofts->ofts_recurse_path_fts_opts = rec_fts_options;
	ofts->ofts_st_path = paths; /* NULL-terminated array of starting paths */
	ofts->ofts_st_path_count = 1;
	ofts->ofts_st_path_index = 0;
	ofts->ofts_path_op = path_op;

	/* todo: would this also be useful for other operations? */
	if (path_op == OVAL_OPERATION_PATTERN_MATCH) {
		pcre *regex;
		int errofs = 0;
		const char *errptr = NULL;

		regex = pcre_compile(cstr_path, 0, &errptr, &errofs, NULL);
		if (regex == NULL) {
			dE("pcre_compile() failed: pattern: '%s', err offset: %d, err msg: '%s'.\n",
			   cstr_path, errofs, errptr);
			return (NULL);
		} else {
			int firstbyte = -1, ret, svec[3];

			ofts->ofts_path_regex = regex;
			ofts->ofts_path_regex_extra = pcre_study(regex, 0, &errptr);

			pcre_fullinfo(regex, ofts->ofts_path_regex_extra,
				      PCRE_INFO_FIRSTBYTE, &firstbyte);

			dI("pcre_fullinfo(): firstbyte: %d '%c'.\n", firstbyte, firstbyte);

			ret = pcre_exec(ofts->ofts_path_regex, ofts->ofts_path_regex_extra,
					"/f0o/bar/baz", 12, 0, PCRE_PARTIAL, svec, 1);

			/*
			 * If firstbyte == '/', the path is an absolute path.
			 * If firstbyte == -2, the pattern starts with a '^'.
			 * In both cases, the traversal through every path
			 * continues only as long as the path partialy matches
			 * the pattern.
			 */
			if ((firstbyte != '/' && firstbyte != -2) || ret == PCRE_ERROR_BADPARTIAL) {
				pcre_free(ofts->ofts_path_regex);
				pcre_free(ofts->ofts_path_regex_extra);
				ofts->ofts_path_regex = NULL;
				ofts->ofts_path_regex_extra = NULL;
				dI("Partial-match optimization disabled.\n");
			} else {
				dI("Partial-match optimization enabled.\n");
			}
		}
	}

	if (path) { /* filepath == NULL */
		if (filesystem == OVAL_RECURSE_FS_LOCAL) {
			ofts->localdevs = fsdev_init(NULL, 0);
			if (ofts->localdevs == NULL) {
				dE("fsdev_init() failed.\n");
				return (NULL);
			}
		} else if (filesystem == OVAL_RECURSE_FS_DEFINED) {
			/* store the device id for future comparison */
			FTSENT *fts_ent;

			fts_ent = fts_read(ofts->ofts_match_path_fts);
			if (fts_ent != NULL) {
				ofts->ofts_recurse_path_devid = fts_ent->fts_statp->st_dev;
				fts_set(ofts->ofts_match_path_fts, fts_ent, FTS_AGAIN);
			}
		}

		ofts->ofts_spath = SEXP_ref(path); /* path entity */
		if (!nilfilename)
			ofts->ofts_sfilename = SEXP_ref(filename); /* filename entity */

		ofts->max_depth = max_depth;
		ofts->direction = direction;
		ofts->recurse = recurse;
		ofts->filesystem = filesystem;
	} else { /* filepath != NULL */
		ofts->ofts_sfilepath = SEXP_ref(filepath);
	}

	return (ofts);
}

/* find the first matching path or filepath */
static FTSENT *oval_fts_read_match_path(OVAL_FTS *ofts)
{
	FTSENT *fts_ent = NULL;
	SEXP_t *stmp;
	oval_result_t ores;

	/* iterate until a match is found or all elements have been traversed */
	for (;;) {
		fts_ent = fts_read(ofts->ofts_match_path_fts);
		if (fts_ent == NULL)
			return NULL;

		switch (fts_ent->fts_info) {
		case FTS_DP:
			continue;
		case FTS_DC:
			dW("Filesystem tree cycle detected at '%s'.\n", fts_ent->fts_path);
			fts_set(ofts->ofts_match_path_fts, fts_ent, FTS_SKIP);
			continue;
		}

#if defined(OSCAP_FTS_DEBUG)
		dI("fts_path: '%s' (l=%d).\n"
		   "fts_name: '%s' (l=%d).\n"
		   "fts_info: %u.\n", fts_ent->fts_path, fts_ent->fts_pathlen,
		   fts_ent->fts_name, fts_ent->fts_namelen, fts_ent->fts_info);
#endif

		if (fts_ent->fts_info == FTS_SL) {
#if defined(OSCAP_FTS_DEBUG)
			dI("Only the target of a symlink gets reported, skipping '%s'.\n", fts_ent->fts_path, fts_ent->fts_name);
#endif
			fts_set(ofts->ofts_match_path_fts, fts_ent, FTS_FOLLOW);
			continue;
		}

		/* partial match optimization for OVAL_OPERATION_PATTERN_MATCH operation on path and filepath */
		if (ofts->ofts_path_regex != NULL && fts_ent->fts_info == FTS_D) {
			int ret, svec[3];

			ret = pcre_exec(ofts->ofts_path_regex, ofts->ofts_path_regex_extra,
					fts_ent->fts_path, fts_ent->fts_pathlen, 0, PCRE_PARTIAL,
					svec, sizeof(svec) / sizeof(svec[0]));
			if (ret < 0) {
				switch (ret) {
				case PCRE_ERROR_NOMATCH:
					dI("Partial match optimization: PCRE_ERROR_NOMATCH, skipping.\n");
					fts_set(ofts->ofts_match_path_fts, fts_ent, FTS_SKIP);
					continue;
				case PCRE_ERROR_PARTIAL:
					dI("Partial match optimization: PCRE_ERROR_PARTIAL, continuing.\n");
					continue;
				default:
					dE("pcre_exec() error: %d.\n", ret);
					return NULL;
				}
			}
		}

		/* don't recurse into non-local filesystems */
		if (ofts->filesystem == OVAL_RECURSE_FS_LOCAL
		    && (fts_ent->fts_info == FTS_D || fts_ent->fts_info == FTS_SL)
		    && (!OVAL_FTS_localp(ofts, fts_ent->fts_path,
					 (fts_ent->fts_statp != NULL) ?
					 &fts_ent->fts_statp->st_dev : NULL))) {
			dI("Don't recurse into non-local filesystems, skipping '%s'.\n", fts_ent->fts_path);
			fts_set(ofts->ofts_recurse_path_fts, fts_ent, FTS_SKIP);
			continue;
		}
		/* don't recurse beyond the initial filesystem */
		if (ofts->filesystem == OVAL_RECURSE_FS_DEFINED
		    && (fts_ent->fts_info == FTS_D || fts_ent->fts_info == FTS_SL)
		    && ofts->ofts_recurse_path_devid != fts_ent->fts_statp->st_dev) {
			fts_set(ofts->ofts_recurse_path_fts, fts_ent, FTS_SKIP);
			continue;
		}

		if ((ofts->ofts_sfilepath && fts_ent->fts_info == FTS_D)
		    || (!ofts->ofts_sfilepath && fts_ent->fts_info != FTS_D))
			continue;

		stmp = SEXP_string_newf("%s", fts_ent->fts_path);
		if (ofts->ofts_sfilepath)
			/* try to match filepath */
			ores = probe_entobj_cmp(ofts->ofts_sfilepath, stmp);
		else
			/* try to match path */
			ores = probe_entobj_cmp(ofts->ofts_spath, stmp);
		SEXP_free(stmp);

		if (ores == OVAL_RESULT_TRUE)
			break;
	} /* for (;;) */

	/*
	 * If we know that we are not going to return anything
	 * else, then we can close the path FTS and return NULL
	 * the next time...
	 */
	if (ofts->ofts_path_op   == OVAL_OPERATION_EQUALS &&
	    ofts->direction      == OVAL_RECURSE_DIRECTION_NONE &&
	    ofts->ofts_sfilename == NULL &&
	    ofts->ofts_sfilepath == NULL)
	{
		fts_set(ofts->ofts_match_path_fts, fts_ent, FTS_SKIP);
	}

	return fts_ent;
}

/* find the first matching file or directory */
static FTSENT *oval_fts_read_recurse_path(OVAL_FTS *ofts)
{
	FTSENT *out_fts_ent = NULL;
	/* the condition below is correct because ofts_sfilepath is NULL here */
	bool collect_dirs = (ofts->ofts_sfilename == NULL);

	switch (ofts->direction) {

	case OVAL_RECURSE_DIRECTION_DOWN:
	case OVAL_RECURSE_DIRECTION_NONE:
		if (ofts->direction == OVAL_RECURSE_DIRECTION_NONE
		    && collect_dirs) {
			/* the target is the directory itself */
			out_fts_ent = ofts->ofts_match_path_fts_ent;
			ofts->ofts_match_path_fts_ent = NULL;
			break;
		}

		/* initialize separate fts for recursion */
		if (ofts->ofts_recurse_path_fts == NULL) {
			char * const paths[2] = { ofts->ofts_match_path_fts_ent->fts_path, NULL };

#if defined(OSCAP_FTS_DEBUG)
			dI("fts_open args: path: \"%s\", options: %d.\n",
				paths[0], ofts->ofts_recurse_path_fts_opts);
#endif
			ofts->ofts_recurse_path_fts = fts_open(paths,
				ofts->ofts_recurse_path_fts_opts, NULL);
			if (ofts->ofts_recurse_path_fts == NULL || errno != 0) {
				dE("fts_open() failed, errno: %d \"%s\".\n",
					errno, strerror(errno));
#if !defined(OSCAP_FTS_DEBUG)
				dE("fts_open args: path: \"%s\", options: %d.\n",
					paths[0], ofts->ofts_recurse_path_fts_opts);
#endif
				if (ofts->ofts_recurse_path_fts != NULL) {
					fts_close(ofts->ofts_recurse_path_fts);
					ofts->ofts_recurse_path_fts = NULL;
				}
				return (NULL);
			}
		}

		/* iterate until a match is found or all elements have been traversed */
		while (out_fts_ent == NULL) {
			FTSENT *fts_ent;

			fts_ent = fts_read(ofts->ofts_recurse_path_fts);
			if (fts_ent == NULL) {
				fts_close(ofts->ofts_recurse_path_fts);
				ofts->ofts_recurse_path_fts = NULL;

				return NULL;
			}

			switch (fts_ent->fts_info) {
			case FTS_DP:
				continue;
			case FTS_DC:
				dW("Filesystem tree cycle detected at '%s'.\n", fts_ent->fts_path);
				fts_set(ofts->ofts_recurse_path_fts, fts_ent, FTS_SKIP);
				continue;
			}

#if defined(OSCAP_FTS_DEBUG)
			dI("fts_path: '%s' (l=%d).\n"
			   "fts_name: '%s' (l=%d).\n"
			   "fts_info: %u.\n", fts_ent->fts_path, fts_ent->fts_pathlen,
			   fts_ent->fts_name, fts_ent->fts_namelen, fts_ent->fts_info);
#endif

			/* collect matching target */
			if (collect_dirs) {
				if (fts_ent->fts_info == FTS_D
				    && (ofts->max_depth == -1 || fts_ent->fts_level <= ofts->max_depth))
					out_fts_ent = fts_ent;
			} else {
				if (fts_ent->fts_info != FTS_D) {
					SEXP_t *stmp;

					stmp = SEXP_string_newf("%s", fts_ent->fts_name);
					if (probe_entobj_cmp(ofts->ofts_sfilename, stmp) == OVAL_RESULT_TRUE)
						out_fts_ent = fts_ent;
					SEXP_free(stmp);
				}
			}

			if (fts_ent->fts_level > 0) { /* don't skip fts root */
				/* limit recursion depth */
				if (ofts->direction == OVAL_RECURSE_DIRECTION_NONE
				    || (ofts->max_depth != -1 && fts_ent->fts_level > ofts->max_depth)) {
					fts_set(ofts->ofts_recurse_path_fts, fts_ent, FTS_SKIP);
					continue;
				}

				/* limit recursion only to selected file types */
				switch (fts_ent->fts_info) {
				case FTS_D:
					if (!(ofts->recurse & OVAL_RECURSE_DIRS)) {
						fts_set(ofts->ofts_recurse_path_fts, fts_ent, FTS_SKIP);
						continue;
					}
					break;
				case FTS_SL:
					if (!(ofts->recurse & OVAL_RECURSE_SYMLINKS)) {
						fts_set(ofts->ofts_recurse_path_fts, fts_ent, FTS_SKIP);
						continue;
					}
					fts_set(ofts->ofts_recurse_path_fts, fts_ent, FTS_FOLLOW);
					break;
				default:
					continue;
				}
			}

			/* don't recurse into non-local filesystems */
			if (ofts->filesystem == OVAL_RECURSE_FS_LOCAL
			    && (fts_ent->fts_info == FTS_D || fts_ent->fts_info == FTS_SL)
			    && (!OVAL_FTS_localp(ofts, fts_ent->fts_path,
					(fts_ent->fts_statp != NULL) ?
					&fts_ent->fts_statp->st_dev : NULL))) {
				fts_set(ofts->ofts_recurse_path_fts, fts_ent, FTS_SKIP);
				continue;
			}
			/* don't recurse beyond the initial filesystem */
			if (ofts->filesystem == OVAL_RECURSE_FS_DEFINED
			    && (fts_ent->fts_info == FTS_D || fts_ent->fts_info == FTS_SL)
			    && ofts->ofts_recurse_path_devid != fts_ent->fts_statp->st_dev) {
				fts_set(ofts->ofts_recurse_path_fts, fts_ent, FTS_SKIP);
				continue;
			}
		}

		break;
	case OVAL_RECURSE_DIRECTION_UP:
		if (ofts->ofts_recurse_path_pthcpy == NULL) {
			ofts->ofts_recurse_path_pthcpy = \
			ofts->ofts_recurse_path_curpth = strdup(ofts->ofts_match_path_fts_ent->fts_path);
			ofts->ofts_recurse_path_curdepth = 0;
		}

		while (ofts->max_depth == -1 || ofts->ofts_recurse_path_curdepth <= ofts->max_depth) {
			/* initialize separate fts for recursion */
			if (ofts->ofts_recurse_path_fts == NULL) {
				char * const paths[2] = { ofts->ofts_recurse_path_curpth, NULL };

#if defined(OSCAP_FTS_DEBUG)
				dI("fts_open args: path: \"%s\", options: %d.\n",
					paths[0], ofts->ofts_recurse_path_fts_opts);
#endif
				ofts->ofts_recurse_path_fts = fts_open(paths,
					ofts->ofts_recurse_path_fts_opts, NULL);
				if (ofts->ofts_recurse_path_fts == NULL || errno != 0) {
					dE("fts_open() failed, errno: %d \"%s\".\n",
						errno, strerror(errno));
#if !defined(OSCAP_FTS_DEBUG)
					dE("fts_open args: path: \"%s\", options: %d.\n",
						paths[0], ofts->ofts_recurse_path_fts_opts);
#endif
					if (ofts->ofts_recurse_path_fts != NULL) {
						fts_close(ofts->ofts_recurse_path_fts);
						ofts->ofts_recurse_path_fts = NULL;
					}
					return (NULL);
				}
			}

			/* iterate until a match is found or all elements have been traversed */
			while (out_fts_ent == NULL) {
				FTSENT *fts_ent;

				fts_ent = fts_read(ofts->ofts_recurse_path_fts);
				if (fts_ent == NULL)
					break;

				/*
				   it would be more accurate to obtain the device
				   id here, but for the sake of supporting the
				   comparison also in oval_fts_read_match_path(),
				   the device id is obtained in oval_fts_open()

				if (ofts->ofts_recurse_path_curdepth == 0)
					ofts->ofts_recurse_path_devid = fts_ent->fts_statp->st_dev;
				*/

				if (ofts->filesystem == OVAL_RECURSE_FS_LOCAL
				    && (!OVAL_FTS_localp(ofts, fts_ent->fts_path,
						(fts_ent->fts_statp != NULL) ?
						&fts_ent->fts_statp->st_dev : NULL)))
					break;
				if (ofts->filesystem == OVAL_RECURSE_FS_DEFINED
				    && ofts->ofts_recurse_path_devid != fts_ent->fts_statp->st_dev)
					break;

				/* collect matching target */
				if (collect_dirs) {
					/* only fts root is collected */
					if (fts_ent->fts_level == 0 && fts_ent->fts_info == FTS_D) {
						out_fts_ent = fts_ent;
						fts_set(ofts->ofts_recurse_path_fts, fts_ent, FTS_SKIP);
						break;
					}
				} else {
					if (fts_ent->fts_info != FTS_D) {
						SEXP_t *stmp;

						stmp = SEXP_string_newf("%s", fts_ent->fts_name);
						if (probe_entobj_cmp(ofts->ofts_sfilename, stmp) == OVAL_RESULT_TRUE)
							out_fts_ent = fts_ent;
						SEXP_free(stmp);
					}
				}

				if (fts_ent->fts_info == FTS_SL)
					fts_set(ofts->ofts_recurse_path_fts, fts_ent, FTS_FOLLOW);
				/* limit recursion only to fts root */
				else if (fts_ent->fts_level > 0)
					fts_set(ofts->ofts_recurse_path_fts, fts_ent, FTS_SKIP);
			}

			if (out_fts_ent != NULL)
				break;

			fts_close(ofts->ofts_recurse_path_fts);
			ofts->ofts_recurse_path_fts = NULL;

			if (!strcmp(ofts->ofts_recurse_path_curpth, "/"))
				break;

			ofts->ofts_recurse_path_curpth = dirname(ofts->ofts_recurse_path_curpth);
			ofts->ofts_recurse_path_curdepth++;
		}

		if (out_fts_ent == NULL) {
			oscap_free(ofts->ofts_recurse_path_pthcpy);
			ofts->ofts_recurse_path_pthcpy = NULL;
		}

		break;
	}

	return out_fts_ent;
}

OVAL_FTSENT *oval_fts_read(OVAL_FTS *ofts)
{
	FTSENT *fts_ent;

#if defined(OSCAP_FTS_DEBUG)
	dI("ofts: %p.\n", ofts);
#endif

	if (ofts == NULL)
		return NULL;

	for (;;) {
		if (ofts->ofts_match_path_fts_ent == NULL) {
			ofts->ofts_match_path_fts_ent = oval_fts_read_match_path(ofts);
			if (ofts->ofts_match_path_fts_ent == NULL)
				return NULL;
		}

		if (ofts->ofts_sfilepath) {
			fts_ent = ofts->ofts_match_path_fts_ent;
			ofts->ofts_match_path_fts_ent = NULL;
			break;
		} else {
			fts_ent = oval_fts_read_recurse_path(ofts);
			if (fts_ent != NULL)
				break;

			ofts->ofts_match_path_fts_ent = NULL;

			// todo: is this true when variables are used?
			/* with 'equals', there's only one potential target */
			if (ofts->ofts_path_op == OVAL_OPERATION_EQUALS)
				return (NULL);
		}
	}

	return OVAL_FTSENT_new(ofts, fts_ent);
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

	if (ofts->ofts_recurse_path_pthcpy != NULL)
		oscap_free(ofts->ofts_recurse_path_pthcpy);

	ofts->ofts_st_path = NULL;

	if (ofts->ofts_path_regex)
		pcre_free(ofts->ofts_path_regex);
	if (ofts->ofts_path_regex_extra)
		pcre_free(ofts->ofts_path_regex_extra);

	if (ofts->ofts_spath != NULL)
		SEXP_free(ofts->ofts_spath);
	if (ofts->ofts_sfilename != NULL)
		SEXP_free(ofts->ofts_sfilename);
	if (ofts->ofts_sfilepath != NULL)
		SEXP_free(ofts->ofts_sfilepath);

	fsdev_free(ofts->localdevs);

	OVAL_FTS_free(ofts);

	return (0);
}
