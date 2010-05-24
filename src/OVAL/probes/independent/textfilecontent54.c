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
 *      "Tomas Heinrich" <theinric@redhat.com>
 */


/*
 * textfilecontent54 probe:
 *
 *  textfilecontent54_object
 *    textfilecontentbehaviors behaviors
 *    string path
 *    string filename
 *    string pattern
 *    int instance
 *
 *  textfilecontent_item
 *    attrs
 *      id
 *      status_enum status
 *    string path
 *    string filename
 *    string pattern
 *    int instance
 *    string line (depr)
 *    string text
 *    [0..*] anytype subexpression
 */

#include <config.h>
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
#include <probe-entcmp.h>
#include <findfile.h>
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
        SEXP_t *r0, *r1, *r2, *r3, *r4;

	item = probe_item_creat ("textfilecontent_item", NULL,
                                 /* entities */
                                 "path", NULL,
                                 r0 = SEXP_string_newf("%s", path),
                                 "filename", NULL,
                                 r1 = SEXP_string_newf("%s", filename),
                                 "pattern", NULL,
                                 r2 = SEXP_string_newf("%s", pattern),
                                 "instance", NULL,
                                 r3 = SEXP_number_newi_32(instance),
                                 "text", NULL,
                                 r4 = SEXP_string_newf("%s", substrs[0]),
                                 NULL);
        SEXP_vfree (r0, r1, r2, r3, r4, NULL);

	for (i = 1; i < substr_cnt; ++i) {
                probe_item_ent_add (item, "subexpression", NULL, r0 = SEXP_string_new (substrs[i], strlen (substrs[i])));
                SEXP_free (r0);
	}

	return item;
}

struct pfdata {
	char *pattern;
	SEXP_t *filename_ent;
	SEXP_t *instance_ent;
	SEXP_t *item_list;
};

static int process_file(const char *path, const char *file, void *arg)
{
	struct pfdata *pfd = (struct pfdata *) arg;
	int ret = 0, path_len, file_len;
	char *whole_path = NULL;
	FILE *fp = NULL;
	int  cur_inst = 0;
	char line[4096];
	SEXP_t *next_inst = NULL;

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
	if (file == NULL)
		goto cleanup;

	path_len   = strlen(path);
	file_len   = strlen(file);
	whole_path = oscap_alloc(path_len + file_len + 2);

	memcpy(whole_path, path, path_len);

	if (whole_path[path_len - 1] != FILE_SEPARATOR) {
		whole_path[path_len] = FILE_SEPARATOR;
		++path_len;
	}

	memcpy(whole_path + path_len, file, file_len + 1);

	fp = fopen(whole_path, "rb");
	if (fp == NULL) {
		ret = -2;
		goto cleanup;
	}

	while (fgets(line, sizeof(line), fp) != NULL) {
		char **substrs;
		int substr_cnt, want_instance;

		next_inst = SEXP_number_newi_32(cur_inst + 1);

		if (probe_entobj_cmp(pfd->instance_ent, next_inst) == OVAL_RESULT_TRUE)
			want_instance = 1;
		else
			want_instance = 0;

                SEXP_free(next_inst);
		substr_cnt = get_substrings(line, re, want_instance, &substrs);

		if (substr_cnt > 0) {
			++cur_inst;

			if (want_instance) {
				int k;
                                SEXP_t *item;

                                item = create_item(path, file, pfd->pattern,
                                                   cur_inst, substrs, substr_cnt);
                                SEXP_list_add(pfd->item_list, item);
                                SEXP_free(item);

				for (k = 0; k < substr_cnt; ++k)
					free(substrs[k]);
				free(substrs);
			}
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

SEXP_t *probe_main(SEXP_t *probe_in, int *err, void *arg)
{
	SEXP_t *path_ent, *file_ent, *inst_ent, *bh_ent, *patt_ent;
        SEXP_t *r0, *r1;
	char *pattern;
	int fcnt;
	struct pfdata pfd;

        (void)arg;

	if (probe_in == NULL) {
		*err = PROBE_EINVAL;
		return NULL;
	}

        *err = PROBE_ENOELM;

        if ((path_ent = probe_obj_getent(probe_in, "path",     1)) == NULL)
                return(NULL);
        if ((file_ent = probe_obj_getent(probe_in, "filename", 1)) == NULL) {
                SEXP_free(path_ent);
                return(NULL);
        }
        if ((inst_ent = probe_obj_getent(probe_in, "instance", 1)) == NULL) {
                SEXP_free(path_ent);
                SEXP_free(file_ent);
                return(NULL);
        }
        if ((patt_ent = probe_obj_getent(probe_in, "pattern",  1)) == NULL) {
                SEXP_free(path_ent);
                SEXP_free(file_ent);
                SEXP_free(inst_ent);
                return(NULL);
        } else {
                SEXP_t *ent_val;

                ent_val = probe_ent_getval(patt_ent);
                pattern = SEXP_string_cstr(ent_val);
                assume_d(pattern != NULL, -1);
                SEXP_free(patt_ent);
                SEXP_free(ent_val);
        }

	/* canonicalize behaviors */
	bh_ent = probe_obj_getent(probe_in, "behaviors", 1);

	if (bh_ent == NULL) {
		SEXP_t *bh_new, *bh_attr;

                bh_attr = probe_attr_creat("max_depth",         r0 = SEXP_string_newf ("1"),
                                           "recurse_direction", r1 = SEXP_string_newf ("none"),
                                           NULL);
		bh_new  = probe_ent_creat("behaviors", bh_attr, NULL /* val */, NULL /* end */);
		bh_ent  = SEXP_list_first(bh_new);

                SEXP_vfree(bh_new, bh_attr, r0, r1, NULL);
	} else {
		if (!probe_ent_attrexists (bh_ent, "max_depth")) {
			probe_ent_attr_add (bh_ent,"max_depth", r0 = SEXP_string_newf ("1"));
                        SEXP_free (r0);
                }
		if (!probe_ent_attrexists (bh_ent, "recurse_direction")) {
			probe_ent_attr_add (bh_ent,"recurse_direction", r0 = SEXP_string_newf ("none"));
                        SEXP_free (r0);
                }
	}

	pfd.pattern      = pattern;
	pfd.filename_ent = file_ent;
	pfd.instance_ent = inst_ent;
	pfd.item_list    = SEXP_list_new(NULL);

	fcnt = find_files(path_ent, file_ent, bh_ent, process_file, (void *) &pfd);

	if (fcnt < 0) {
		SEXP_t *item;
		item = probe_item_creat("textfilecontent_item", NULL,
                                        "path", NULL,
                                        r0 = probe_ent_getval(path_ent),
                                        NULL);
                probe_item_setstatus(item, OVAL_STATUS_ERROR);
		SEXP_list_add(pfd.item_list, item);
                SEXP_vfree(r0, item, NULL);
	}

        SEXP_free(file_ent);
        SEXP_free(path_ent);
        SEXP_free(inst_ent);
        SEXP_free(bh_ent);
        oscap_free(pattern);

	*err = 0;
	return pfd.item_list;
}

