/**
 * @file   textfilecontent.c
 * @brief  textfilecontent probe
 * @author "Tomas Heinrich" <theinric@redhat.com>
 *
 * 2010/06/13 dkopecek@redhat.com
 *  This probe is able to process a textfilecontent_object as defined in OVAL 5.4 and 5.5.
 *
 */

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
 *      "Tomas Heinrich" <theinric@redhat.com>
 */

/*
 * textfilecontent probe:
 *
 *  textfilecontent_object
 *    textfilecontentbehaviors behaviors
 *    string path
 *    string filename
 *    string line
 *
 *  textfilecontent_item
 *    attrs
 *      id
 *      status_enum status
 *    string path
 *    string filename
 *    string pattern
 *    int instance
 *    string line
 *    string text
 *    [0..*] anytype subexpression
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#if defined USE_REGEX_PCRE
#include <pcre.h>
#elif defined USE_REGEX_POSIX
#include <regex.h>
#endif

#include <seap.h>
#include <probe-api.h>
#include <probe/entcmp.h>
#include <oval_fts.h>
#include <alloc.h>
#include "common/assume.h"

#define FILE_SEPARATOR '/'

#if defined USE_REGEX_PCRE
static int get_substrings(char *str, pcre *re, int want_substrs, char ***substrings) {
	int i, ret, rc;
	int ovector[60], ovector_len = sizeof (ovector) / sizeof (ovector[0]);

	// todo: max match count check

	for (i = 0; i < ovector_len; ++i)
		ovector[i] = -1;

	rc = pcre_exec(re, NULL, str, strlen(str), 0, 0,
		       ovector, ovector_len);

	if (rc < -1) {
		return -1;
	} else if (rc == -1) {
		/* no match */
		return 0;
	} else if(!want_substrs) {
		/* just report successful match */
		return 1;
	}

	char **substrs;

	ret = 0;
	if (rc == 0) {
		/* vector too small */
		rc = ovector_len / 3;
	}

	substrs = oscap_alloc(rc * sizeof (char *));
	for (i = 0; i < rc; ++i) {
		int len;
		char *buf;

		if (ovector[2 * i] == -1)
			continue;
		len = ovector[2 * i + 1] - ovector[2 * i];
		buf = oscap_alloc(len + 1);
		memcpy(buf, str + ovector[2 * i], len);
		buf[len] = '\0';
		substrs[ret] = buf;
		++ret;
	}
	/*
	  if (ret < rc)
	  substrs = realloc(substrs, ret * sizeof (char *));
	*/
	*substrings = substrs;

	return ret;
}
#elif defined USE_REGEX_POSIX
static int get_substrings(char *str, regex_t *re, int want_substrs, char ***substrings) {
	int i, ret, rc;
	regmatch_t pmatch[40];
	int pmatch_len = sizeof (pmatch) / sizeof (pmatch[0]);

	rc = regexec(re, str, pmatch_len, pmatch, 0);
	if (rc == REG_NOMATCH) {
		/* no match */
		return 0;
	} else if (!want_substrs) {
		/* just report successful match */
		return 1;
	}

	char **substrs;

	ret = 0;
	substrs = oscap_alloc(pmatch_len * sizeof (char *));
	for (i = 0; i < pmatch_len; ++i) {
		int len;
		char *buf;

		if (pmatch[i].rm_so == -1)
			continue;
		len = pmatch[i].rm_eo - pmatch[i].rm_so;
		buf = oscap_alloc(len + 1);
		memcpy(buf, str + pmatch[i].rm_so, len);
		buf[len] = '\0';
		substrs[ret] = buf;
		++ret;
	}

	/*
	  if (ret < pmatch_len)
	  substrs = realloc(substrs, ret * sizeof (char *));
	*/
	*substrings = substrs;

	return ret;
}
#endif

static SEXP_t *create_item(const char *path, const char *filename, char *pattern,
			   int instance, char **substrs, int substr_cnt)
{
	int i;
	SEXP_t *item;
        SEXP_t *r0, *r1, *r2, *r3, *r4, *r5, *r6;

	item = probe_item_creat ("textfilecontent_item", NULL,
                                 /* entities */
                                 "filepath", NULL,
				 r2 = SEXP_string_newf ("%s/%s", path, filename),
                                 "path", NULL,
                                 r0 = SEXP_string_newf("%s", path),
                                 "filename", NULL,
                                 r1 = SEXP_string_newf("%s", filename),
                                 "pattern", NULL,
                                 r3 = SEXP_string_newf("%s", ""),
                                 "instance", NULL,
                                 r4 = SEXP_number_newi_32(instance),
                                 "line", NULL,
                                 r5 = SEXP_string_newf("%s", pattern),
                                 "text", NULL,
                                 r6 = SEXP_string_newf("%s", substrs[0]),
                                 NULL);
	SEXP_free(r2);
        SEXP_vfree (r0, r1, r3, r4, r5, r6, NULL);

	for (i = 1; i < substr_cnt; ++i) {
                probe_item_ent_add (item, "subexpression", NULL, r0 = SEXP_string_new (substrs[i], strlen (substrs[i])));
                SEXP_free (r0);
	}

	return item;
}

struct pfdata {
	char *pattern;
	SEXP_t *filename_ent;
	SEXP_t *cobj;
};

static int process_file(const char *path, const char *filename, void *arg)
{
	struct pfdata *pfd = (struct pfdata *) arg;
	int ret = 0, path_len, filename_len;
	char *whole_path = NULL;
	FILE *fp = NULL;

// todo: move to probe_main()?
#if defined USE_REGEX_PCRE
	int erroffset = -1;
	pcre *re = NULL;
	const char *error;

	re = pcre_compile(pfd->pattern, PCRE_UTF8, &error, &erroffset, NULL);
	if (re == NULL) {
		return -1;
	}
#elif defined USE_REGEX_POSIX
	regex_t _re, *re = &_re;

	if (regcomp(re, pfd->pattern, REG_EXTENDED | REG_NEWLINE) != 0) {
		return -1;
	}
#endif

	if (filename == NULL)
		goto cleanup;

	path_len = strlen(path);
	filename_len = strlen(filename);
	whole_path = oscap_alloc(path_len + filename_len + 2);
	memcpy(whole_path, path, path_len);
	if (whole_path[path_len - 1] != FILE_SEPARATOR) {
		whole_path[path_len] = FILE_SEPARATOR;
		++path_len;
	}
	memcpy(whole_path + path_len, filename, filename_len + 1);

	fp = fopen(whole_path, "rb");
	if (fp == NULL) {
		ret = -2;
		goto cleanup;
	}

	int cur_inst = 0;
	char line[4096];

	while (fgets(line, sizeof(line), fp) != NULL) {
		char **substrs;
		int substr_cnt;

		substr_cnt = get_substrings(line, re, 1, &substrs);
		if (substr_cnt > 0) {
			int k;
			SEXP_t *item;

			++cur_inst;
			item = create_item(path, filename, pfd->pattern,
					   cur_inst, substrs, substr_cnt);
			probe_cobj_add_item(pfd->cobj, item);
			SEXP_free(item);
			for (k = 0; k < substr_cnt; ++k)
				free(substrs[k]);
			free(substrs);
		}
	}

 cleanup:
	if (fp != NULL)
		fclose(fp);
	if (whole_path != NULL)
		free(whole_path);
#if defined USE_REGEX_PCRE
	if (re != NULL)
		pcre_free(re);
#elif defined USE_REGEX_POSIX
	regfree(re);
#endif

	return ret;
}

int probe_main(SEXP_t *probe_in, SEXP_t *probe_out, void *arg, SEXP_t *filters)
{
	SEXP_t *path_ent, *filename_ent, *line_ent, *behaviors_ent, *filepath_ent;
        SEXP_t *r0, *r1;
	char *pattern;

	OVAL_FTS    *ofts;
	OVAL_FTSENT *ofts_ent;

        (void)arg;
        (void)filters;

	if (probe_in == NULL || probe_out == NULL) {
		return(PROBE_EINVAL);
	}

	path_ent = probe_obj_getent(probe_in, "path",     1);
	filename_ent = probe_obj_getent(probe_in, "filename", 1);
	line_ent = probe_obj_getent(probe_in, "line",  1);
	filepath_ent = probe_obj_getent(probe_in, "filepath",  1);
	behaviors_ent = probe_obj_getent(probe_in, "behaviors", 1);

        if ( ((path_ent == NULL || filename_ent == NULL) && filepath_ent==NULL) ||
	     line_ent==NULL ) {
		SEXP_free (path_ent);
		SEXP_free (filename_ent);
		SEXP_free (line_ent);
		SEXP_free (filepath_ent);
		SEXP_free (behaviors_ent);
		return PROBE_ENOELM;
	}

	/* get pattern from SEXP */
	SEXP_t *ent_val;
	ent_val = probe_ent_getval(line_ent);
	pattern = SEXP_string_cstr(ent_val);
	assume_d(pattern != NULL, -1);
	SEXP_vfree(line_ent, ent_val, NULL);

        /* behaviours are not important if filepath is used */
        if(filepath_ent != NULL && behaviors_ent != NULL) {
                SEXP_free (behaviors_ent);
                behaviors_ent = NULL;
	}

	/* canonicalize behaviors */
	if (behaviors_ent == NULL) {
		SEXP_t * behaviors_new;
		behaviors_new = probe_ent_creat("behaviors",
                                                probe_attr_creat("max_depth", r0 = SEXP_string_newf ("1"),
                                                                 "recurse_direction", r1 = SEXP_string_newf ("none"),
                                                                 NULL),
                                                NULL /* val */,
                                                NULL /* end */);
		behaviors_ent = SEXP_list_first(behaviors_new);
                SEXP_vfree(r0, r1, behaviors_new, NULL);
	}
	else {
		if (!probe_ent_attrexists (behaviors_ent, "max_depth")) {
			probe_ent_attr_add (behaviors_ent,"max_depth", r0 = SEXP_string_newf ("1"));
                        SEXP_free (r0);
                }
		if (!probe_ent_attrexists (behaviors_ent, "recurse_direction")) {
			probe_ent_attr_add (behaviors_ent,"recurse_direction", r0 = SEXP_string_newf ("none"));
                        SEXP_free (r0);
                }
	}

	struct pfdata pfd;

	pfd.pattern = pattern;
	pfd.filename_ent = filename_ent;
	pfd.cobj = probe_out;

	if ((ofts = oval_fts_open(path_ent, filename_ent, filepath_ent, behaviors_ent)) != NULL) {
		while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
			process_file(ofts_ent->path, ofts_ent->file, &pfd);
			oval_ftsent_free(ofts_ent);
		}

		oval_fts_close(ofts);
	}

	SEXP_free(path_ent);
	SEXP_free(filename_ent);
	SEXP_free(behaviors_ent);
	SEXP_free(filepath_ent);
	oscap_free(pattern);

	return 0;
}

