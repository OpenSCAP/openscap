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

#include <stdio.h>
#include <string.h>
#include <pcre.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <seap.h>
#include <probe.h>
#include <findfile.h>

#define FILE_SEPARATOR '/'

static int get_substrings(char *str, pcre *re, int want_substrs, char ***substrings) {
	int i, ret, rc;
	int ovector[60], ovector_len = sizeof (ovector) / sizeof (ovector[0]);

	// todo: max match count check

	for (i = 0; i < ovector_len; ++i)
		ovector[i] = -1;

	rc = pcre_exec(re, NULL, str, strlen(str), 0, 0,
		       ovector, ovector_len);

	if (rc < -1) {
		ret = -1;
	} else if (rc == -1) {
		/* no match */
		ret = 0;
	} else if(!want_substrs) {
		/* just report successful match */
		ret = 1;
	} else {
		char **substrs;

		ret = 0;
		if (rc == 0) {
			/* vector too small */
			rc = ovector_len / 3;
		}

		substrs = malloc(rc * sizeof (char *));
		for (i = 0; i < rc; ++i) {
			int len;
			char *buf;

			if (ovector[2 * i] == -1)
				continue;
			len = ovector[2 * i + 1] - ovector[2 * i];
			buf = malloc(len + 1);
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
	}

	return ret;
}

static SEXP_t *create_item(const char *path, const char *filename, char *pattern,
			   int instance, char **substrs, int substr_cnt)
{
	int i;
	SEXP_t *attrs, *item;

	attrs = SEXP_OVALattr_create("id", SEXP_number_newd(-1), // todo: id
				     "status", SEXP_number_newd(OVAL_STATUS_EXISTS),
				     NULL);
	item = SEXP_OVALobj_create("textfilecontent_item",
				   attrs,
				   "path", NULL,
				   SEXP_string_newf(path),
				   "filename", NULL,
				   SEXP_string_newf(filename),
				   "pattern", NULL,
				   SEXP_string_newf(pattern),
				   "instance", NULL,
				   SEXP_number_newd(instance),
				   "text", NULL,
				   SEXP_string_newf(substrs[0]),
				   NULL);
	SEXP_OVALobj_setelmstatus(item, "path", 1, OVAL_STATUS_EXISTS);
	SEXP_OVALobj_setelmstatus(item, "filename", 1, OVAL_STATUS_EXISTS);

	for (i = 1; i < substr_cnt; ++i) {
		SEXP_OVALobj_elm_add(item, "subexpression", NULL, SEXP_string_newf(substrs[i]));
		SEXP_OVALobj_setelmstatus(item, "subexpression", i, OVAL_STATUS_EXISTS);
	}

	return item;
}

static int report_missing(SEXP_t *elm)
{
	oval_operation_enum op;

	op = SEXP_number_getd(SEXP_OVALelm_getattrval(elm, "operation"));
	if (op == OPERATION_EQUALS)
		return 1;
	else
		return 0;
}

struct pfdata {
	char *pattern;
	SEXP_t *filename_elm;
	SEXP_t *instance_elm;
	SEXP_t *item_list;
};

static int process_file(const char *path, const char *filename, void *arg)
{
	struct pfdata *pfd = (struct pfdata *) arg;
	int erroffset = -1, ret = 0, path_len, filename_len;
	char *whole_path = NULL;
	const char *error;
	pcre *re = NULL;
	FILE *fp = NULL;

	re = pcre_compile(pfd->pattern, PCRE_UTF8, &error, &erroffset, NULL);
	if (re == NULL) {
		return -1;
	}

	if (filename == NULL) {
		SEXP_t *attrs, *item;

		if (report_missing(pfd->filename_elm)) {
			attrs = SEXP_OVALattr_create("id", SEXP_number_newd(-1), // todo: id
						     "status", SEXP_number_newd(OVAL_STATUS_DOESNOTEXIST),
						     NULL);
			item = SEXP_OVALobj_create("textfilecontent_item",
						   attrs,
						   "path", NULL,
						   SEXP_string_newf(path),
						   "filename", NULL,
						   SEXP_string_newf(filename),
						   NULL);
			SEXP_OVALobj_setelmstatus(item, "path", 1, OVAL_STATUS_EXISTS);
			SEXP_OVALobj_setelmstatus(item, "filename", 1, OVAL_STATUS_DOESNOTEXIST);
		} else {
			attrs = SEXP_OVALattr_create("id", SEXP_number_newd(-1), // todo: id
						     "status", SEXP_number_newd(OVAL_STATUS_EXISTS),
						     NULL);
			item = SEXP_OVALobj_create("textfilecontent_item",
						   attrs,
						   "path", NULL,
						   SEXP_string_newf(path),
						   NULL);
			SEXP_OVALobj_setelmstatus(item, "path", 1, OVAL_STATUS_EXISTS);
		}
		SEXP_list_add(pfd->item_list, item);

		goto cleanup;
	}

	path_len = strlen(path);
	filename_len = strlen(filename);
	whole_path = malloc(path_len + filename_len + 2);
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
	SEXP_t *next_inst = NULL;

	while (fgets(line, sizeof(line), fp) != NULL) {
		char **substrs;
		int substr_cnt, want_instance;

		if (next_inst != NULL)
			SEXP_free(next_inst);
		next_inst = SEXP_number_newd(cur_inst + 1);
		if (SEXP_OVALentobj_cmp(pfd->instance_elm, next_inst) == OVAL_RESULT_TRUE)
			want_instance = 1;
		else
			want_instance = 0;

		substr_cnt = get_substrings(line, re, want_instance, &substrs);
		if (substr_cnt > 0) {
			++cur_inst;

			if (want_instance) {
				int k;

				SEXP_list_add(pfd->item_list,
					      create_item(path, filename, pfd->pattern,
							  cur_inst, substrs, substr_cnt));
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
	if (re != NULL)
		pcre_free(re);

	return ret;
}

SEXP_t *probe_main(SEXP_t *probe_in, int *err)
{
	SEXP_t *path_elm, *filename_elm, *instance_elm, *behaviors_elm;
	char *pattern;

	if (probe_in == NULL) {
		*err = PROBE_EINVAL;
		return NULL;
	}

	/* parse request */
	if ( (behaviors_elm = SEXP_OVALobj_getelm(probe_in, "behaviors", 1)) == NULL ||
	     (path_elm = SEXP_OVALobj_getelm(probe_in, "path", 1)) == NULL ||
	     (filename_elm = SEXP_OVALobj_getelm(probe_in, "filename", 1)) == NULL ||
	     (pattern = SEXP_string_cstr(SEXP_OVALobj_getelmval(probe_in, "pattern", 1, 1))) == NULL ||
	     (instance_elm = SEXP_OVALobj_getelm(probe_in, "instance", 1)) == NULL) {
		*err = PROBE_ENOELM;
		return NULL;
	}

	int fcnt;
	struct pfdata pfd;

	pfd.pattern = pattern;
	pfd.filename_elm = filename_elm;
	pfd.instance_elm = instance_elm;
	pfd.item_list = SEXP_list_new();

	fcnt = find_files(path_elm, filename_elm, behaviors_elm,
			  process_file, (void *) &pfd);
	if (fcnt == 0) {
		if (report_missing(pfd.filename_elm)) {
			SEXP_t *item, *attrs;
			attrs = SEXP_OVALattr_create("id", SEXP_number_newd(-1), // todo: id
						     "status", SEXP_number_newd(OVAL_STATUS_DOESNOTEXIST),
						     NULL);
			item = SEXP_OVALobj_create("textfilecontent_item",
						   attrs,
						   "path", NULL,
						   SEXP_OVALelm_getval(path_elm, 1),
						   NULL);
			SEXP_OVALobj_setelmstatus(item, "path", 1, OVAL_STATUS_DOESNOTEXIST);
			SEXP_list_add(pfd.item_list, item);
		}
	} else if (fcnt < 0) {
		SEXP_t *item, *attrs;
		attrs = SEXP_OVALattr_create("id", SEXP_number_newd(-1), // todo: id
					     "status", SEXP_number_newd(OVAL_STATUS_ERROR),
					     NULL);
		item = SEXP_OVALobj_create("textfilecontent_item",
					   attrs,
					   "path", NULL,
					   SEXP_OVALelm_getval(path_elm, 1),
					   NULL);
		SEXP_list_add(pfd.item_list, item);
	}

	*err = 0;
	return pfd.item_list;
}
