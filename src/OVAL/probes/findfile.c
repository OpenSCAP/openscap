/**
 * @file   findfile.h
 * @author "Peter Vrabec" <pvrabec@redhat.com>
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
 *      Peter Vrabec <pvrabec@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <regex.h>
#include <assert.h>
#include <alloc.h>

#include "findfile.h"
#include "probe-entcmp.h"

typedef struct {
	SEXP_t *spath;
	SEXP_t *sfilename;
	bool filename_is_nil;
	char *direction;
	char *follow;
	fsdev_t *dev_list;
	int (*cb) (const char *pathname, const char *filename, void *arg);
} setting_t;

typedef struct {
	int pathc;
	char **pathv;
	int offs;
} rglob_t;

static int find_files_recursion(const char *path, setting_t * setting, int depth, void *arg);
static int recurse_direction(const char *file, char *direction);
static int recurse_follow(struct stat *st, char *follow);
static int noRegex(char *token);
static int rglob(const char *pattern, rglob_t * result);
static void find_paths_recursion(const char *path, regex_t * re, rglob_t * result);
static void rglobfree(rglob_t * result);

/* ************************************* PUBLIC ***********************************  */

/*  Collect files and paths according to defined behavior. This function can be used in 
 *  these probes: file_object, textfilecontent54
 *  
 *
 *  @spath  The path to a file on the machine, not including the filename
 *  @sfilename The name of a file
 *  @behaviors Specify find_files settings
 *  @cb Callback that is called for each hit
 */
int find_files(SEXP_t * spath, SEXP_t * sfilename, SEXP_t * behaviors,
	       int (*cb) (const char *, const char *, void *), void *arg)
{
	char *path = NULL, *tmp = NULL;
	setting_t *setting = NULL;
	int i, rc, max_depth;
	rglob_t rglobbuf;
	int finds = 0;
	SEXP_t *stmp;

	SEXP_t *r0, *r1;
	uint32_t op;

	assert(sfilename);
	assert(spath);
	assert(behaviors);

	path = SEXP_string_cstr(r1 = probe_ent_getval(spath));

	tmp = SEXP_string_cstr(r0 = probe_ent_getattrval(behaviors, "max_depth"));
	SEXP_free(r0);

	max_depth = atoi(tmp);
	free(tmp);

	setting = oscap_calloc(1, sizeof(setting_t));
	setting->spath = spath;
	setting->direction = SEXP_string_cstr(r0 = probe_ent_getattrval(behaviors, "recurse_direction"));
	SEXP_free(r0);

	setting->follow = SEXP_string_cstr(r0 = probe_ent_getattrval(behaviors, "recurse"));
	SEXP_free(r0);

	setting->cb = cb;

	/* Init list of local devices */
	setting->dev_list = NULL;
	stmp = probe_ent_getattrval(behaviors, "recurse_file_system");

	if (stmp && !SEXP_strncmp(stmp, "local", 6)) {
		if ((setting->dev_list = fsdev_init(NULL, 0)) == NULL) {
			SEXP_free(stmp);
			goto error;
		}
	}

	SEXP_free(stmp);

	/* Filename */
	setting->sfilename = sfilename;
	r0 = probe_ent_getval(sfilename);
	setting->filename_is_nil = (SEXP_string_length(r0) == 0) ? true : false;
	SEXP_free(r0);

	assert(strlen(path) > 0);

	/* Is there a '/' at the end of the path? */
	i = strlen(path) - 1;

	if (path[i] == '/' && i > 0)
		path[i] = '\0';

	/* Evaluate path(s) */
	r0 = probe_ent_getattrval(spath, "operation");
	op = SEXP_number_getu_32(r0);
	SEXP_free(r0);

	if (op == OVAL_OPERATION_PATTERN_MATCH) {
		rglobbuf.offs = 10;
		rc = rglob(path, &rglobbuf);

		if (!rc && rglobbuf.pathc > 0) {
			finds = 0;

			for (i = 0; i < rglobbuf.pathc; i++) {
				r0 = SEXP_string_newf("%s", rglobbuf.pathv[i]);
				if (probe_entobj_cmp(spath, r0) == OVAL_RESULT_TRUE) {
					rc = find_files_recursion(rglobbuf.pathv[i], setting, max_depth, arg);

					if (rc >= 0)
						finds += rc;
					else {
						SEXP_free(r0);
						rglobfree(&rglobbuf);
						finds = rc;
						goto error;
					}
				}
				SEXP_free(r0);
			}

			rglobfree(&rglobbuf);
		}
	} else {
		if (probe_entobj_cmp(spath, r1) == OVAL_RESULT_TRUE) {
			rc = find_files_recursion(path, setting, max_depth, arg);

			if (rc >= 0)
				finds += rc;
			else {
				finds = rc;
				goto error;
			}
		}
	}

 error:
	SEXP_free(r1);
	free(path);

	free(setting->follow);
	free(setting->direction);
	fsdev_free(setting->dev_list);
	free(setting);

	return finds;
}

/* ********************************* PRIVATE *******************************  */

/*
 *  depth defines how many directories to recurse when a recurse direction is specified
 *  '-1' means no limitation
 *  '0' is equivalent to no recursion
 *  '1' means to step only one directory level up/down
 */
static int find_files_recursion(const char *path, setting_t * setting, int depth, void *arg)
{
	struct stat st;
	struct dirent *pDirent;
	char path_new[PATH_MAX];
	DIR *pDir;
	int rc, tmp;
	size_t path_len;

	pDir = opendir(path);
	if (pDir == NULL) {
		/* FIXME */
		return 0;
	}

	rc = 0;
	strcpy(path_new, path);
	path_len = strlen(path);

	if (setting->filename_is_nil) { /* we are only interested in directories */
		rc++;

		if (setting->cb (path, "", arg) != 0) {
			closedir(pDir);
			return (-1);
		}
	}

	// Make sure new_path always has a '/' at the end
	if (path_len > 1 && path_new[path_len-1] != '/') {
		path_new[path_len] = '/';
		path_len++;
	}

	while ((pDirent = readdir(pDir))) {
		if (PATH_MAX < path_len + pDirent->d_reclen + 1)
			continue;
		strcpy(&path_new[path_len], pDirent->d_name);

		if (lstat(path_new, &st) == -1)
			continue;

		if (depth &&	/* how deep rabbit hole is? */
		    recurse_direction(pDirent->d_name, setting->direction) &&	/* up or down direction? */
		    recurse_follow(&st, setting->follow) &&	/* follow symlinks? */
		    fsdev_search(setting->dev_list, &st.st_dev)) {	/* local filesystem? */
			tmp = find_files_recursion(path_new, setting, depth == -1 ? -1 : depth - 1, arg);
			if (tmp >= 0)
				rc += tmp;
			else {
				rc = tmp;
				break;
			}
		}

		if (!setting->filename_is_nil
		    && !S_ISDIR(st.st_mode)) {
			SEXP_t *sf;

			sf = SEXP_string_newf("%s", pDirent->d_name);
			if (probe_entobj_cmp(setting->sfilename, sf) == OVAL_RESULT_TRUE) {
				rc++;

				if (setting->cb (path, pDirent->d_name, arg) != 0) {
					closedir(pDir);
					SEXP_free(sf);
					return (-1);
				}
			}
			SEXP_free(sf);
		}
	}

	closedir(pDir);
	return rc;
}

/*
 * What to follow during recursion?
 * 'symlinks and directories' --default
 * 'directories'
 * 'symlinks'
 */
static int recurse_follow(struct stat *st, char *follow)
{

	if (!(S_ISDIR(st->st_mode) || S_ISLNK(st->st_mode)))
		return 0;

	/* default */
	if ((!follow || !strncmp(follow, "symlinks and directories", 24)))
		return 1;

	/* deprecated */
	if (!strncmp(follow, "files and directories", 12) && S_ISDIR(st->st_mode))
		return 1;

	/* one or another, not very usual case */
	if (!strncmp(follow, "symlinks", 9) && S_ISLNK(st->st_mode))
		return 1;
	if (!strncmp(follow, "directories", 12) && S_ISDIR(st->st_mode))
		return 1;

	return 0;
}

/*
 * Up or down? 
 * 'none' - default
 * 'up' to parent directories 
 * 'down' into child directories
 */

static int recurse_direction(const char *file, char *direction)
{

	if (!strncmp(direction, "down", 5)) {
		if (strncmp(file, "..", 3) && strncmp(file, ".", 2))
			return 1;
	} else if (!strncmp(direction, "up", 3)) {
		if (!strncmp(file, "..", 3))
			return 1;
	}
	/* default none */
	return 0;
}

/*
 * Similar to glob function, but instead off shell wildcard 
 * use regexps 
 */
static int rglob(const char *pattern, rglob_t * result)
{
	char *tmp_ptr, *tmp, *token, *saveptr = NULL;
	char path[PATH_MAX] = "/";
	int len = 1;
	regex_t re;

	/* check pattern */
	if (!pattern)
		return 1;

	/* get no regexp portion from pattern to path */
	tmp_ptr = tmp = strdup(pattern);
	if (tmp[0] == '^')
		tmp++;

	for (;; tmp = NULL) {
		token = strtok_r(tmp, "/", &saveptr);
		if (token == NULL)
			break;
		if (noRegex(token)) {
			strcat(path, token);
			strcat(path, "/");
		} else
			break;
	}
	free(tmp_ptr);
	/* erase last slash, but not the first one! */
	len = strlen(path);
	if (len > 1)
		path[len - 1] = '\0';

	/* init regex machine */
	if (regcomp(&re, pattern, REG_EXTENDED) != 0)
		return 1;	/* Can't init pattern buffer storage area */

	/* allocate memory for result */
	if (result->offs < 1 || result->offs > 1000)
		result->offs = 10;
	result->pathv = oscap_alloc(sizeof(char **) * result->offs);
	result->pathc = 0;

	/* find paths */
	find_paths_recursion(path, &re, result);

	regfree(&re);
	return 0;

}

static void find_paths_recursion(const char *path, regex_t * re, rglob_t * result)
{
	struct dirent *pDirent;
	struct stat st;
	DIR *pDir;
	char path_new[PATH_MAX];
	size_t path_len;

	pDir = opendir(path);
	if (pDir == NULL)
		return;

	if (regexec(re, path, 0, NULL, 0) == 0) {	/* catch? */
		result->pathv[result->pathc] = strdup(path);
		result->pathc++;
		if (result->pathc == result->offs) {
			result->offs = result->offs * 2;	/* magic constant */
			result->pathv = oscap_realloc(result->pathv, sizeof(char **) * result->offs);
		}
	}

	strcpy(path_new, path);
	path_len = strlen(path);

	// Make sure new_path always has a '/' at the end
	if (path_len > 1 && path_new[path_len-1] != '/') {
		path_new[path_len] = '/';
		path_len++;
	}

	while ((pDirent = readdir(pDir))) {
		if (PATH_MAX < path_len + pDirent->d_reclen + 1)
			continue;
		strcpy(&path_new[path_len], pDirent->d_name);

		if (lstat(path_new, &st) == -1)
			continue;

		if (S_ISDIR(st.st_mode) && strncmp(pDirent->d_name, "..", 3) && strncmp(pDirent->d_name, ".", 2)) {
			find_paths_recursion(path_new, re, result);	/* recursion */
		}
	}
	closedir(pDir);
}

static int noRegex(char *token)
{
	const char regexChars[] = "^$\\.[](){}*+?";
	size_t i, token_len;

	token_len = strlen(token);
	for (i = 0; i < token_len; i++) {
		if (strchr(regexChars, token[i]))
			return 0;
	}
	return 1;
}

static void rglobfree(rglob_t * result)
{
	int i;

	for (i = 0; i < result->pathc; i++) {
		free(result->pathv[i]);
	}
	free(result->pathv);

	result->pathc = 0;
}

/// @}
