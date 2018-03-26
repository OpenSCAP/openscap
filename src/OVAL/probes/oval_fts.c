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
#include <sys/mntent.h>
#include <libzonecfg.h>
#include <sys/avl.h>
#elif defined(_AIX)
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

	free(ofts);
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
	const size_t shift = ofts->prefix ? strlen(ofts->prefix) : 0;
	if (ofts->ofts_sfilename || ofts->ofts_sfilepath) {
		ofts_ent->path_len = pathlen_from_ftse(fts_ent->fts_pathlen, fts_ent->fts_namelen) - shift;
		if (ofts_ent->path_len > 0) {
			ofts_ent->path = malloc(ofts_ent->path_len + 1);
			strncpy(ofts_ent->path + shift, fts_ent->fts_path, ofts_ent->path_len);
			ofts_ent->path[ofts_ent->path_len] = '\0';
		} else {
			ofts_ent->path_len = 1;
			ofts_ent->path = strdup("/");
		}

		ofts_ent->file_len = fts_ent->fts_namelen;
		ofts_ent->file = strdup(fts_ent->fts_name);
	} else {
		ofts_ent->path_len = fts_ent->fts_pathlen - shift;
		if (ofts_ent->path_len > 0) {
			ofts_ent->path = strdup(fts_ent->fts_path + shift);
		} else {
			ofts_ent->path_len = 1;
			ofts_ent->path = strdup("/");
		}

		ofts_ent->file_len = -1;
		ofts_ent->file = NULL;
	}

#if defined(OSCAP_FTS_DEBUG)
	dD("New OVAL_FTSENT: file: '%s', path: '%s'.", ofts_ent->file, ofts_ent->path);
#endif
	return (ofts_ent);
}

static void OVAL_FTSENT_free(OVAL_FTSENT *ofts_ent)
{
	free(ofts_ent->path);
	free(ofts_ent->file);
	free(ofts_ent);
	return;
}

#if defined(__SVR4) && defined(__sun)
#ifndef MNTTYPE_SMB
#define MNTTYPE_SMB	"smb"
#endif
#ifndef MNTTYPE_SMBFS
#define MNTTYPE_SMBFS	"smbfs"
#endif
#ifndef MNTTYPE_PROC
#define MNTTYPE_PROC	"proc"
#endif

typedef struct zone_path {
	avl_node_t avl_link_next;
	char zpath[MAXPATHLEN];
} zone_path_t;
static avl_tree_t avl_tree_list;


static bool valid_remote_fs(char *fstype)
{
	if (strcmp(fstype, MNTTYPE_NFS) == 0 ||
	    strcmp(fstype, MNTTYPE_SMBFS) == 0 ||
	    strcmp(fstype, MNTTYPE_SMB) == 0)
		return (true);
	return (false);
}

static bool valid_local_fs(char *fstype)
{
	if (strcmp(fstype, MNTTYPE_SWAP) == 0 ||
	    strcmp(fstype, MNTTYPE_MNTFS) == 0 ||
	    strcmp(fstype, MNTTYPE_CTFS) == 0 ||
	    strcmp(fstype, MNTTYPE_OBJFS) == 0 ||
	    strcmp(fstype, MNTTYPE_SHAREFS) == 0 ||
	    strcmp(fstype, MNTTYPE_PROC) == 0 ||
	    strcmp(fstype, MNTTYPE_LOFS) == 0 ||
	    strcmp(fstype, MNTTYPE_AUTOFS) == 0)
		return (false);
	return (true);
}

/* function to compare two avl nodes in the avl tree */
static int compare_zoneroot(const void *entry1, const void *entry2)
{
	zone_path_t *t1, *t2;
	int comp;

	t1 = (zone_path_t *)entry1;
	t2 = (zone_path_t *)entry2;
	if ((comp = strcmp(t1->zpath, t2->zpath)) == 0) {
		return (0);
	}
	return (comp > 0 ? 1 : -1);
}

int load_zones_path_list()
{
	FILE *cookie;
	char *name;
	zone_state_t state_num;
	zone_path_t *temp = NULL;
	avl_index_t where;
	char rpath[MAXPATHLEN];

	cookie = setzoneent();
	if (getzoneid() != GLOBAL_ZONEID)
		return (0);
	avl_create(&avl_tree_list, compare_zoneroot,
	    sizeof(zone_path_t), offsetof(zone_path_t, avl_link_next));
	while ((name = getzoneent(cookie)) != NULL) {
		if (strcmp(name, "global") == 0)
			continue;
		if (zone_get_state(name, &state_num) != Z_OK) {
			dE("Could not get zone state for %s", name);
			continue;
		} else if (state_num > ZONE_STATE_CONFIGURED) {
			temp = malloc(sizeof(zone_path_t));
			if (temp == NULL) {
				dE("Memory alloc failed");
				return(1);
			}
			if (zone_get_zonepath(name, rpath,
			    sizeof(rpath)) != Z_OK) {
				dE("Could not get zone path for %s",
				    name);
				continue;
			}
			if (realpath(rpath, temp->zpath) != NULL)
				avl_add(&avl_tree_list, temp);
		}
	}
	endzoneent(cookie);
	return (0);
}

static void free_zones_path_list()
{
	zone_path_t *temp;
	void* cookie = NULL;

	while ((temp = avl_destroy_nodes(&avl_tree_list, &cookie)) != NULL) {
		free(temp);
	}
	avl_destroy(&avl_tree_list);
}

static bool valid_local_zone(const char *path)
{
	zone_path_t temp;
	avl_index_t where;

	strlcpy(temp.zpath, path, sizeof(temp.zpath));
	if (avl_find(&avl_tree_list, &temp, &where) != NULL)
		return (true);

	return (false);
}


#endif

static bool OVAL_FTS_localp(OVAL_FTS *ofts, const char *path, void *id)
{
#if defined(__SVR4) && defined(__sun)
	if (id != NULL && (*(char*)id) != '\0') {
		/* if not a valid local fs skip */
		if (valid_local_fs((char*)id)) {
			/* if recurse is local , skip remote fs
			   and non-global zones */
			if (ofts->filesystem == OVAL_RECURSE_FS_LOCAL) {
				return (!(valid_remote_fs((char*)id) ||
				    valid_local_zone(path)));
			}
			return (true);
		}
		return (false);
	} else if (path != NULL) {
		/* id was not set, because fts_read failed to stat the node */
		struct stat sb;
		if ((stat(path, &sb) == 0) && (valid_local_fs(sb.st_fstype))) {
			/* if recurse is local , skip remote fs
			   and non-global zones */
			if (ofts->filesystem == OVAL_RECURSE_FS_LOCAL) {
				return (!(valid_remote_fs(sb.st_fstype) ||
				    valid_local_zone(path)));
			}
			return (true);
		}
		return (false);
	} else {
		return (false);
	}
#else
	if (id != NULL)
		return (fsdev_search(ofts->localdevs, id) == 1 ? true : false);
	else if (path != NULL)
		return (fsdev_path(ofts->localdevs, path) == 1 ? true : false);
	else
		return (false);
#endif
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

static char *extract_fixed_path_prefix(char *path)
{
	char *s;

	if (path[0] == '^')
		path++;

	s = __regex_locate(path);
	if (*s != '\0')
		for (s--; s > (path + 1) && *s != '/'; s--);
	if (s > (path + 1)) {
		s = __string_unescape(path, (size_t) (s - path));
		if (s != NULL) {
			if (s[0] == '/')
				return s;
			free(s);
		}
	}

	return strdup("/");
}

static int badpartial_check_slash(const char *pattern)
{
	pcre *regex;
	const char *errptr = NULL;
	int errofs = 0, fb, ret;

	regex = pcre_compile(pattern + 1 /* skip '^' */, 0, &errptr, &errofs, NULL);
	if (regex == NULL) {
		dE("Failed to validate the pattern: pcre_compile(): "
		   "error: '%s', error offset: %d, pattern: '%s'.\n",
		   errofs, errptr, pattern);
		return -1;
	}
	ret = pcre_fullinfo(regex, NULL, PCRE_INFO_FIRSTBYTE, &fb);
	pcre_free(regex);
	regex = NULL;
	if (ret != 0) {
		dE("Failed to validate the pattern: pcre_fullinfo(): "
		   "return code: %d, pattern: '%s'.\n", ret, pattern);
		return -1;
	}
	if (fb != '/') {
		dE("Failed to validate the pattern: pcre_fullinfo(): "
		   "first byte: %d '%c', pattern: '%s' - the first "
		   "byte should be a '/'.\n", fb, fb, pattern);
		return -2;
	}

	return 0;
}

#define TEST_PATH1 "/"
#define TEST_PATH2 "x"

static int badpartial_transform_pattern(char *pattern, pcre **regex_out)
{
	/*
	  PCREPARTIAL(3)
	  http://pcre.org/pcre.txt
	  Last updated: 21 January 2012

	  For releases of PCRE prior to 8.00, because of the way
	  certain internal optimizations were implemented in the
	  pcre_exec() function, the PCRE_PARTIAL option (predecessor
	  of PCRE_PARTIAL_SOFT) could not be used with all patterns.

	  Items that were formerly restricted were repeated single
	  characters and repeated metasequences. If PCRE_PARTIAL was
	  set for a pattern that did not conform to the restrictions,
	  pcre_exec() returned the error code PCRE_ERROR_BADPARTIAL
	  (-13).
	*/

	int ret, brkt_lvl = 0, errofs = 0;
	const char *rchars = "\\[]()*+{"; /* probably incomplete */
	const char *test_path1 = TEST_PATH1;
	const char *errptr = NULL;
	char *s, *brkt_mark;
	bool bracketed = false, found_regex = false;
	pcre *regex;

	/* The processing bellow builds upon the assumption that
	   the pattern has been validated by pcre_compile() */
	for (s = brkt_mark = pattern; (s = strpbrk(s, rchars)) != NULL; s++) {
		switch (*s) {
		case '\\':
			s++;
			break;
		case '[':
			if (!bracketed) {
				bracketed = true;
				if (s[1] == ']')
					s++;
			}
			break;
		case ']':
			bracketed = false;
			break;
		case '(':
			if (!bracketed) {
				if (brkt_lvl++ == 0)
					brkt_mark = s;
			}
			break;
		case ')':
			if (!bracketed)
				brkt_lvl--;
			break;
		default:
			if (!bracketed)
				found_regex = true;
			break;
		}
		if (found_regex)
			break;
	}

	if (s == NULL) {
		dW("Nonfatal failure: can't transform the pattern for partial "
		   "match optimization: none of the suspected culprits found, "
		   "pattern: '%s'.", pattern);
		return -1;
	}

	if (brkt_lvl > 0)
		*brkt_mark = '\0';
	else
		*s = '\0';

	regex = pcre_compile(pattern, 0, &errptr, &errofs, NULL);
	if (regex == NULL) {
		dW("Nonfatal failure: can't transform the pattern for partial "
		   "match optimization, error: '%s', error offset: %d, "
		   "pattern: '%s'.", errptr, errofs, pattern);
		return -1;
	}

	ret = pcre_exec(regex, NULL, test_path1, strlen(test_path1), 0,
		PCRE_PARTIAL, NULL, 0);
	if (ret != PCRE_ERROR_PARTIAL && ret < 0) {
		pcre_free(regex);
		dW("Nonfatal failure: can't transform the pattern for partial "
		   "match optimization, pcre_exec() return code: %d, pattern: "
		   "'%s'.", ret, pattern);
		return -1;
	}

	if (regex_out != NULL)
		*regex_out = regex;

	return 0;
}

/* Verify that the path is usable and try to craft a regex to speed up
   the filesystem traversal. If the path to match is ill-designed, an
   ugly heuristic is employed to obtain something meaningfull. */
static int process_pattern_match(const char *path, pcre **regex_out)
{
	int ret, errofs = 0;
	char *pattern;
	const char *test_path1 = TEST_PATH1;
	//const char *test_path2 = TEST_PATH2;
	const char *errptr = NULL;
	pcre *regex;

	if (path[0] != '^') {
		/* Matching has to have a fixed starting point and thus
		   every pattern has to start with a caret. */
		size_t plen;

		plen = strlen(path) + 1;
		pattern = malloc(plen + 1);
		pattern[0] = '^';
		memcpy(pattern + 1, path, plen);
		dI("The pattern '%s' doesn't contain a leading caret - added. "
		   "All paths with the 'pattern match' operation must begin "
		   "with a caret.", path);
	} else {
		pattern = strdup(path);
	}

	regex = pcre_compile(pattern, 0, &errptr, &errofs, NULL);
	if (regex == NULL) {
		dE("Failed to validate the pattern: pcre_compile(): "
		   "error offset: %d, error: '%s', pattern: '%s'.\n",
		   errofs, errptr, pattern);
		free(pattern);
		return -1;
	}
	ret = pcre_exec(regex, NULL, test_path1, strlen(test_path1), 0,
		PCRE_PARTIAL, NULL, 0);

	switch (ret) {
	case PCRE_ERROR_PARTIAL:
		/* The pattern has matched a prefix of the test path
		   and probably begins with a slash. Make sure that it
		   doesn't match an arbitrary prefix. */

		/* todo:
		   Convince folks that they should really fix their
		   OVAL definitions that use ".*" as 'path' and then
		   uncomment this.

		dI("pcre_exec() returned PCRE_ERROR_PARTIAL for pattern '%s' "
		   "and test path '%s'.\n", pattern, test_path1);
		ret = pcre_exec(regex, NULL, test_path2, strlen(test_path2),
			0, PCRE_PARTIAL, NULL, 0);
		if (ret == PCRE_ERROR_PARTIAL || ret >= 0) {
			dE("Failed to validate the pattern: test path '%s' "
			   "matched by pattern '%s' - the pattern is too "
			   "general, i.e. inefficient. This could take a "
			   "lifetime to complete.\n", test_path2, pattern);
			pcre_free(regex);
			free(pattern);
			return -2;
		}
		*/
		break;
	case PCRE_ERROR_BADPARTIAL:
		dI("pcre_exec() returned PCRE_ERROR_BADPARTIAL for pattern "
		   "'%s' and a test path '%s'. Falling back to "
		   "pcre_fullinfo().\n", pattern, test_path1);
		pcre_free(regex);
		regex = NULL;

		/* Fallback to first byte check to determin if
		   the pattern begins with a slash. */
		ret = badpartial_check_slash((const char *) pattern);
		if (ret != 0) {
			free(pattern);
			return ret;
		}
		/* The pattern contains features that this version of
		   PCRE can't handle for partial matching. At least
		   try to find the longest well-bracketed prefix that
		   can be handled. */
		badpartial_transform_pattern(pattern, &regex);
		break;
	case PCRE_ERROR_NOMATCH:
		/* The pattern doesn't contain a leading slash (or
		   some part of this code is broken). Apologise to the
		   user and fail. */
		dE("Failed to validate the pattern: pcre_exec() returned "
		   "PCRE_ERROR_NOMATCH for pattern '%s' and a test path '%s'. "
		   "This indicates the pattern doesn't match a leading '/'.\n",
		   pattern, test_path1);
		pcre_free(regex);
		free(pattern);
		return -2;
	default:
		if (ret >= 0) {
			/* The pattern actually matches the test
			   path. Iteresting… Make sure that it doesn't
			   match an arbitrary prefix. */

			/* todo:
			   Convince folks that they should really fix
			   their OVAL definitions that use ".*" as
			   'path' and then uncomment this.

			ret = pcre_exec(regex, NULL, test_path2, strlen(test_path2),
					0, PCRE_PARTIAL, NULL, 0);
			if (ret == PCRE_ERROR_PARTIAL || ret >= 0) {
				dE("Failed to validate the pattern: test path '%s' "
				   "matched by pattern '%s' - the pattern is too "
				   "general, i.e. inefficient. This could take a "
				   "lifetime to complete.\n", test_path2, pattern);
				pcre_free(regex);
				free(pattern);
				return -2;
			}
			*/
			break;
		}
		/* Some other error. */
		dE("Failed to validate the pattern: pcre_exec() return "
		   "code: %d, pattern '%s', test path '%s'.\n", ret,
		   pattern, test_path1);
		pcre_free(regex);
		free(pattern);
		return -1;
	}

	if (regex == NULL) {
		dI("Disabling partial match optimization.");
	} else {
		dI("Enabling partial match optimization using "
		   "pattern: '%s'.", pattern);
		if (regex_out != NULL)
			*regex_out = regex;
	}

	free(pattern);

	return 0;
}


#undef TEST_PATH1
#undef TEST_PATH2

OVAL_FTS *oval_fts_open(const char *prefix, SEXP_t *path, SEXP_t *filename, SEXP_t *filepath, SEXP_t *behaviors, SEXP_t* result)
{
	OVAL_FTS *ofts;

	char cstr_path[PATH_MAX+1];
	char cstr_file[PATH_MAX+1];
	char cstr_buff[32];
	const char *paths[2] = { NULL, NULL };

	SEXP_t *r0;

	int mtc_fts_options = FTS_PHYSICAL | FTS_COMFOLLOW | FTS_NOCHDIR;
	int rec_fts_options = FTS_PHYSICAL | FTS_COMFOLLOW | FTS_NOCHDIR;
	int max_depth   = -1;
	int direction   = -1;
	int recurse     = -1;
	int filesystem  = -1;

	uint32_t path_op;
	bool nilfilename = false;
	pcre *regex = NULL;
	struct stat st;

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
	dI("path_op: %u, '%s'.", path_op, oval_operation_get_text(path_op));
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
		dD("path: '%s', filename: '%s', filename: %d.", cstr_path, nilfilename ? "" : cstr_file, nilfilename);
#endif
	} else { /* filepath != NULL */
		PROBE_ENT_STRVAL(filepath, cstr_path, sizeof cstr_path, return NULL;, return NULL;);
	}

	/* max_depth */
	PROBE_ENT_AREF(behaviors, r0, "max_depth", return NULL;);
	SEXP_string_cstr_r(r0, cstr_buff, sizeof cstr_buff - 1);
	max_depth = strtol(cstr_buff, NULL, 10);
	if (errno == EINVAL || errno == ERANGE) {
		dE("Invalid value of the `%s' attribute: %s", "recurse_direction", cstr_buff);
		SEXP_free(r0);
		return (NULL);
	}
#if defined(OSCAP_FTS_DEBUG)
	dI("bh.max_depth: %s => max_depth: %d", cstr_buff, max_depth);
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
		dE("Invalid direction: %s", cstr_buff);
		SEXP_free(r0);
		return (NULL);
	}
#if defined(OSCAP_FTS_DEBUG)
	dI("bh.direction: %s => direction: %d", cstr_buff, direction);
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
			dE("Invalid recurse: %s", cstr_buff);
			SEXP_free(r0);
			return (NULL);
		}
	} else {
		recurse = OVAL_RECURSE_SYMLINKS_AND_DIRS;
	}
#if defined(OSCAP_FTS_DEBUG)
	dI("bh.recurse: %s => recurse: %d", cstr_buff, recurse);
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
			dE("Invalid recurse filesystem: %s", cstr_buff);
			SEXP_free(r0);
			return (NULL);
		}
	} else {
		filesystem = OVAL_RECURSE_FS_ALL;
	}
#if defined(OSCAP_FTS_DEBUG)
	dI("bh.filesystem: %s => filesystem: %d", cstr_buff, filesystem);
#endif
	SEXP_free(r0);

	/* todo:
	   Still missing is a propagation of the error to the
	   user. Currently, all the information is provided in the
	   debug log, but the oval_fts api has no way of passing this
	   information to the user.
	*/

	if (path_op == OVAL_OPERATION_EQUALS) {
		paths[0] = strdup(cstr_path);
	} else if (path_op == OVAL_OPERATION_PATTERN_MATCH) {
		if (process_pattern_match(cstr_path, &regex) != 0)
			return NULL;
		paths[0] = extract_fixed_path_prefix(cstr_path);
		dI("Extracted fixed path: '%s'.", paths[0]);
	} else {
		paths[0] = strdup("/");
	}

	if (prefix != NULL) {
		char *path_with_prefix = oscap_sprintf("%s%s", prefix, paths[0]);
		free((void *) paths[0]);
		paths[0] = path_with_prefix;
	}
	dI("Opening file '%s'.", paths[0]);
	/* Fail if the provided path doensn't actually exist. Symlinks
	   without targets are accepted. */
	if (lstat(paths[0], &st) == -1) {
		if (errno) {
			dD("lstat() failed: errno: %d, '%s'.",
			   errno, strerror(errno));
		}
		free((void *) paths[0]);
		return NULL;
	}

	dI("Opening file '%s'.", paths[0]);

	ofts = OVAL_FTS_new();
	ofts->prefix = prefix;

	/* reset errno as fts_open() doesn't do it itself. */
	errno = 0;
	ofts->ofts_match_path_fts = fts_open((char * const *) paths, mtc_fts_options, NULL);
	free((void *) paths[0]);
	/* fts_open() doesn't return NULL for all errors (e.g. nonexistent paths),
	   so check errno to detect it. Far from being perfect. */
	if (ofts->ofts_match_path_fts == NULL || errno != 0) {
		dE("fts_open() failed, errno: %d \"%s\".", errno, strerror(errno));
		OVAL_FTS_free(ofts);
		return (NULL);
	}

	ofts->ofts_recurse_path_fts_opts = rec_fts_options;
	ofts->ofts_path_op = path_op;
	if (regex != NULL) {
		const char *errptr = NULL;

		ofts->ofts_path_regex = regex;
		ofts->ofts_path_regex_extra = pcre_study(regex, 0, &errptr);
	}

	if (filesystem == OVAL_RECURSE_FS_LOCAL) {
#if   defined(__SVR4) && defined(__sun)
		ofts->localdevs = NULL;
#else
		ofts->localdevs = fsdev_init(NULL, 0);
		if (ofts->localdevs == NULL) {
			dE("fsdev_init() failed.");
			/* One dummy read to get rid of an uninitialized
			 * value in the FTS data before calling
			 * fts_close() on it. */
			fts_read(ofts->ofts_match_path_fts);
			oval_fts_close(ofts);
			return (NULL);
		}
#endif
	} else if (filesystem == OVAL_RECURSE_FS_DEFINED) {
		/* store the device id for future comparison */
		FTSENT *fts_ent;

		fts_ent = fts_read(ofts->ofts_match_path_fts);
		if (fts_ent != NULL) {
			ofts->ofts_recurse_path_devid = fts_ent->fts_statp->st_dev;
			fts_set(ofts->ofts_match_path_fts, fts_ent, FTS_AGAIN);
		}
	}

	ofts->recurse = recurse;
	ofts->filesystem = filesystem;

	if (path) { /* filepath == NULL */
		ofts->ofts_spath = SEXP_ref(path); /* path entity */
		if (!nilfilename)
			ofts->ofts_sfilename = SEXP_ref(filename); /* filename entity */

		ofts->max_depth = max_depth;
		ofts->direction = direction;
	} else { /* filepath != NULL */
		ofts->ofts_sfilepath = SEXP_ref(filepath);
	}

#if defined(__SVR4) && defined(__sun)
	if (load_zones_path_list() != 0) {
		dE("Failed to load zones path info. Recursing non-global zones.");
		free_zones_path_list();
	}
#endif

	ofts->result = result;

	return (ofts);
}

static inline int _oval_fts_is_local(OVAL_FTS *ofts, FTSENT *fts_ent) {
# if defined (__SVR4) && defined(__sun)
	/* pseudo filesystems will be skipped */
	/* don't recurse into remote fs if local is specified */
	return ((fts_ent->fts_info == FTS_D || fts_ent->fts_info == FTS_SL)
	    && (!OVAL_FTS_localp(ofts, fts_ent->fts_path,
	    (fts_ent->fts_statp != NULL) ?
	    &fts_ent->fts_statp->st_fstype : NULL)));
#else
	/* don't recurse into non-local filesystems */
	return (ofts->filesystem == OVAL_RECURSE_FS_LOCAL
	    && (fts_ent->fts_info == FTS_D || fts_ent->fts_info == FTS_SL)
	    && (!OVAL_FTS_localp(ofts, fts_ent->fts_path,
				 (fts_ent->fts_statp != NULL) ?
				 &fts_ent->fts_statp->st_dev : NULL)));
#endif
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
			dW("Filesystem tree cycle detected at '%s'.", fts_ent->fts_path);
			fts_set(ofts->ofts_match_path_fts, fts_ent, FTS_SKIP);
			continue;
		}

#if defined(OSCAP_FTS_DEBUG)
		dI("fts_path: '%s' (l=%d)."
		   "fts_name: '%s' (l=%d).\n"
		   "fts_info: %u.\n", fts_ent->fts_path, fts_ent->fts_pathlen,
		   fts_ent->fts_name, fts_ent->fts_namelen, fts_ent->fts_info);
#endif

		if (fts_ent->fts_info == FTS_SL) {
#if defined(OSCAP_FTS_DEBUG)
			dI("Only the target of a symlink gets reported, skipping '%s'.", fts_ent->fts_path, fts_ent->fts_name);
#endif
			fts_set(ofts->ofts_match_path_fts, fts_ent, FTS_FOLLOW);
			continue;
		}
		if (_oval_fts_is_local(ofts, fts_ent)) {
			dI("Don't recurse into non-local filesystems, skipping '%s'.", fts_ent->fts_path);
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

		/* partial match optimization for OVAL_OPERATION_PATTERN_MATCH operation on path and filepath */
		if (ofts->ofts_path_regex != NULL && fts_ent->fts_info == FTS_D) {
			int ret, svec[3];

			ret = pcre_exec(ofts->ofts_path_regex, ofts->ofts_path_regex_extra,
					fts_ent->fts_path, fts_ent->fts_pathlen, 0, PCRE_PARTIAL,
					svec, sizeof(svec) / sizeof(svec[0]));
			if (ret < 0) {
				switch (ret) {
				case PCRE_ERROR_NOMATCH:
					dD("Partial match optimization: PCRE_ERROR_NOMATCH, skipping.");
					fts_set(ofts->ofts_match_path_fts, fts_ent, FTS_SKIP);
					continue;
				case PCRE_ERROR_PARTIAL:
					dD("Partial match optimization: PCRE_ERROR_PARTIAL, continuing.");
					continue;
				default:
					dE("pcre_exec() error: %d.", ret);
					return NULL;
				}
			}
		}

		if ((ofts->ofts_sfilepath && fts_ent->fts_info == FTS_D)
		    || (!ofts->ofts_sfilepath && fts_ent->fts_info != FTS_D))
			continue;

		const size_t shift = ofts->prefix ? strlen(ofts->prefix) : 0;
		stmp = SEXP_string_newf("%s", fts_ent->fts_path + shift);

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
			dI("fts_open args: path: \"%s\", options: %d.",
				paths[0], ofts->ofts_recurse_path_fts_opts);
#endif
			/* reset errno as fts_open() doesn't do it itself. */
			errno = 0;
			ofts->ofts_recurse_path_fts = fts_open(paths,
				ofts->ofts_recurse_path_fts_opts, NULL);
			/* fts_open() doesn't return NULL for all errors
			   (e.g. nonexistent paths), so check errno to detect it.
			   Far from being perfect. */
			if (ofts->ofts_recurse_path_fts == NULL || errno != 0) {
				dE("fts_open() failed, errno: %d \"%s\".",
					errno, strerror(errno));
#if !defined(OSCAP_FTS_DEBUG)
				dE("fts_open args: path: \"%s\", options: %d.",
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
				dW("Filesystem tree cycle detected at '%s'.", fts_ent->fts_path);
				fts_set(ofts->ofts_recurse_path_fts, fts_ent, FTS_SKIP);
				continue;
			}

#if defined(OSCAP_FTS_DEBUG)
			dI("fts_path: '%s' (l=%d)."
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
					oval_result_t result = probe_entobj_cmp(ofts->ofts_sfilename, stmp);
					switch (result){
						case OVAL_RESULT_TRUE:
							out_fts_ent = fts_ent;
							break;

						case OVAL_RESULT_ERROR:
							probe_cobj_set_flag(ofts->result, SYSCHAR_FLAG_ERROR);
							break;

						default:
							break;
					}

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
			if (_oval_fts_is_local(ofts, fts_ent)) {
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
				dI("fts_open args: path: \"%s\", options: %d.",
					paths[0], ofts->ofts_recurse_path_fts_opts);
#endif
				/* reset errno as fts_open() doesn't do it itself. */
				errno = 0;
				/* fts_open() doesn't return NULL for all errors
				   (e.g. nonexistent paths), so check errno to
				   detect it. Far from being perfect. */
				ofts->ofts_recurse_path_fts = fts_open(paths,
					ofts->ofts_recurse_path_fts_opts, NULL);
				if (ofts->ofts_recurse_path_fts == NULL || errno != 0) {
					dE("fts_open() failed, errno: %d \"%s\".",
						errno, strerror(errno));
#if !defined(OSCAP_FTS_DEBUG)
					dE("fts_open args: path: \"%s\", options: %d.",
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
#if   defined(__SVR4) && defined(__sun)
				if ((!OVAL_FTS_localp(ofts, fts_ent->fts_path,
				    (fts_ent->fts_statp != NULL) ?
				    &fts_ent->fts_statp->st_fstype : NULL)))
				       break;
#else
				if (ofts->filesystem == OVAL_RECURSE_FS_LOCAL
				    && (!OVAL_FTS_localp(ofts, fts_ent->fts_path,
						(fts_ent->fts_statp != NULL) ?
						&fts_ent->fts_statp->st_dev : NULL)))
					break;
#endif
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
			free(ofts->ofts_recurse_path_pthcpy);
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
	dI("ofts: %p.", ofts);
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
	if (ofts->ofts_recurse_path_pthcpy != NULL)
		free(ofts->ofts_recurse_path_pthcpy);

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
#if defined(__SVR4) && defined(__sun)
	free_zones_path_list();
#endif

	return (0);
}
