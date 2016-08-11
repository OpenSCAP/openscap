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
#include <limits.h>
#include <pcre.h>

#include <seap.h>
#include <probe-api.h>
#include <probe/entcmp.h>
#include <probe/probe.h>
#include <probe/option.h>
#include <oval_fts.h>
#include <alloc.h>
#include "common/assume.h"
#include "common/debug_priv.h"

#define FILE_SEPARATOR '/'

oval_schema_version_t over;

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

static SEXP_t *create_item(const char *path, const char *filename, char *pattern,
			   int instance, char **substrs, int substr_cnt)
{
	int i;
	SEXP_t *item;
        SEXP_t *r0;
	SEXP_t *se_instance, *se_filepath;
	char *text;

        if (strlen(path) + strlen(filename) + 1 > PATH_MAX) {
                dE("path+filename too long");
                return (NULL);
        }

	if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.4)) < 0) {
		pattern = text = NULL;
		se_instance = NULL;
	} else {
		text = substrs[0];
		se_instance = SEXP_number_newu_64((int64_t) instance);
	}
	if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.6)) < 0) {
		se_filepath = NULL;
	} else {
		se_filepath = SEXP_string_newf("%s%c%s", path, FILE_SEPARATOR, filename);
	}

        item = probe_item_create(OVAL_INDEPENDENT_TEXT_FILE_CONTENT, NULL,
                                 "filepath", OVAL_DATATYPE_SEXP, se_filepath,
                                 "path",     OVAL_DATATYPE_STRING, path,
                                 "filename", OVAL_DATATYPE_STRING, filename,
                                 "pattern",  OVAL_DATATYPE_STRING, pattern,
                                 "instance", OVAL_DATATYPE_SEXP, se_instance,
                                 "line",     OVAL_DATATYPE_STRING, pattern,
                                 "text",     OVAL_DATATYPE_STRING, substrs[0],
                                 NULL);

	for (i = 1; i < substr_cnt; ++i) {
                probe_item_ent_add (item, "subexpression", NULL, r0 = SEXP_string_new (substrs[i], strlen (substrs[i])));
                SEXP_free (r0);
	}

	return item;
}

struct pfdata {
	char *pattern;
	SEXP_t *filename_ent;
        probe_ctx *ctx;
};

static int process_file(const char *path, const char *filename, void *arg)
{
	struct pfdata *pfd = (struct pfdata *) arg;
	int ret = 0, path_len, filename_len;
	char *whole_path = NULL;
	FILE *fp = NULL;
	struct stat st;

// todo: move to probe_main()?
	int erroffset = -1;
	pcre *re = NULL;
	const char *error;

	re = pcre_compile(pfd->pattern, PCRE_UTF8, &error, &erroffset, NULL);
	if (re == NULL) {
		return -1;
	}

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

	/*
	 * If stat() fails, don't report an error and just skip the file.
	 * This is an expected situation, because the fts_*() functions
	 * are called with the 'FTS_PHYSICAL' option. Normally, stumbling
	 * upon a symlink without a target would cause fts_read() to return
	 * the 'FTS_SLNONE' flag, but the 'FTS_PHYSICAL' option causes it
	 * to return 'FTS_SL' and the presence of a valid target has to
	 * be determined with stat().
	 */
	if (stat(whole_path, &st) == -1)
		goto cleanup;
	if (!S_ISREG(st.st_mode))
		goto cleanup;

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

                        probe_item_collect(pfd->ctx, item);

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
	if (re != NULL)
		pcre_free(re);

	return ret;
}

void *probe_init(void)
{
	probe_setoption(PROBEOPT_OFFLINE_MODE_SUPPORTED, PROBE_OFFLINE_CHROOT);
	return NULL;
}

int probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *path_ent, *filename_ent, *line_ent, *behaviors_ent, *filepath_ent, *probe_in;
	char *pattern;

	OVAL_FTS    *ofts;
	OVAL_FTSENT *ofts_ent;

        (void)arg;

        probe_in = probe_ctx_getobject(ctx);

	over = probe_obj_get_platform_schema_version(probe_in);
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

	probe_filebehaviors_canonicalize(&behaviors_ent);

	struct pfdata pfd;

	pfd.pattern = pattern;
	pfd.filename_ent = filename_ent;
	pfd.ctx = ctx;

	if ((ofts = oval_fts_open(path_ent, filename_ent, filepath_ent, behaviors_ent, probe_ctx_getresult(ctx))) != NULL) {
		while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
			if (ofts_ent->fts_info == FTS_F
			    || ofts_ent->fts_info == FTS_SL) {
				// todo: handle return code
				process_file(ofts_ent->path, ofts_ent->file, &pfd);
			}
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

