/*
 * Copyright 2020 Red Hat Inc., Durham, North Carolina.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      Jan Černý <jcerny@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <errno.h>
#include <pcre.h>
#include <yaml.h>
#include <yaml-path.h>

#include "yamlfilecontent_probe.h"
#include "sexp-manip.h"
#include "debug_priv.h"
#include "oval_fts.h"
#include "list.h"
#include "probe/probe.h"

#define OSCAP_YAML_STRING_TAG "tag:yaml.org,2002:str"
#define OSCAP_YAML_BOOL_TAG "tag:yaml.org,2002:bool"
#define OSCAP_YAML_FLOAT_TAG "tag:yaml.org,2002:float"
#define OSCAP_YAML_INT_TAG "tag:yaml.org,2002:int"

#define OVECCOUNT 30 /* should be a multiple of 3 */

int yamlfilecontent_probe_offline_mode_supported()
{
	return PROBE_OFFLINE_OWN;
}

static bool match_regex(const char *pattern, const char *value)
{
	const char *errptr;
	int erroroffset;
	pcre *re = pcre_compile(pattern, 0, &errptr, &erroroffset, NULL);
	if (re == NULL) {
		dE("pcre_compile failed on pattern '%s': %s at %d", pattern,
			errptr, erroroffset);
		return false;
	}
	int ovector[OVECCOUNT];
	int rc = pcre_exec(re, NULL, value, strlen(value), 0, 0, ovector, OVECCOUNT);
	if (rc > 0) {
		return true;
	}
	return false;
}

static SEXP_t *yaml_scalar_event_to_sexp(yaml_event_t *event)
{
	char *tag = (char *) event->data.scalar.tag;
	char *value = (char *) event->data.scalar.value;

	/* nodes lacking an explicit tag are given a non-specific tag:
	 * “!” for non-plain scalars, and “?” for all other nodes
	 */
	if (tag == NULL) {
		if (event->data.scalar.style != YAML_PLAIN_SCALAR_STYLE) {
			tag = "!";
		} else {
			tag = "?";
		}
	}

	/* Nodes with "!" tag can be sequences, maps or strings, but we process
	 * only scalars in this functions, so they can only be strings. */
	if (!strcmp(tag, "!")) {
		tag = OSCAP_YAML_STRING_TAG;
	}

	bool question = !strcmp(tag, "?");

	/* Regular expressions based on https://yaml.org/spec/1.2/spec.html#id2804923 */

	if (question || !strcmp(tag, OSCAP_YAML_BOOL_TAG)) {
		if (match_regex("^(true|True|TRUE)$", value)) {
			return SEXP_number_newb(true);
		} else if (match_regex("^(false|False|FALSE)$", value)) {
			return SEXP_number_newb(false);
		} else if (!question) {
			return NULL;
		}
	}
	if (question || !strcmp(tag, OSCAP_YAML_INT_TAG)) {
		if (match_regex("^[+-]?[0-9]+$", value)) {
			int int_value = strtol(value, NULL, 10);
			return SEXP_number_newi(int_value);
		} else if (match_regex("^0o[0-7]+$", value)) {
			/* strtol doesn't understand 0o as octal prefix, it wants 0 */
			int int_value = strtol(value + 2, NULL, 8);
			return SEXP_number_newi(int_value);
		} else if (match_regex("^0x[0-9a-fA-F]+$", value)) {
			int int_value = strtol(value, NULL, 16);
			return SEXP_number_newi(int_value);
		} else if (!question) {
			return NULL;
		}
	}
	if (question || !strcmp(tag, OSCAP_YAML_FLOAT_TAG)) {
		if (match_regex("^[-+]?(\\.[0-9]+|[0-9]+(\\.[0-9]*)?)([eE][-+]?[0-9]+)?$", value)) {
			double double_value = strtod(value, NULL);
			return SEXP_number_newf(double_value);
		} else if (match_regex("^[-+]?(\\.inf|\\.Inf|\\.INF)$", value)) {
			double double_value = INFINITY;
			if (value[0] == '-') {
				double_value = -INFINITY;
			}
			return SEXP_number_newf(double_value);
		} else if (match_regex("^(\\.nan|\\.NaN|\\.NAN)$", value)) {
			double double_value = NAN;
			return SEXP_number_newf(double_value);
		} else if (!question) {
			return NULL;
		}
	}

	return SEXP_string_new(value, strlen(value));
}

static int yaml_path_query(const char *filepath, const char *yaml_path_cstr, struct oscap_list *values, probe_ctx *ctx)
{
	int ret = 0;
	FILE *yaml_file = fopen(filepath, "r");
	if (yaml_file == NULL) {
		SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
			"Unable to open file '%s': %s", filepath, strerror(errno));
		probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
		SEXP_free(msg);
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
		return -1;
	}

	yaml_path_t *yaml_path = yaml_path_create();
	if (yaml_path_parse(yaml_path, (char *) yaml_path_cstr)) {
		SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
			"Invalid YAML path '%s' (%s)\n", yaml_path_cstr,
			yaml_path_error_get(yaml_path)->message);
		probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
		SEXP_free(msg);
		probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
		yaml_path_destroy(yaml_path);
		fclose(yaml_file);
		return -1;
	};

	yaml_parser_t parser;
	yaml_parser_initialize(&parser);
	yaml_parser_set_input_file(&parser, yaml_file);

	yaml_event_t event;
	yaml_event_type_t event_type;
	bool sequence = false;

	do {
		if (!yaml_parser_parse(&parser, &event)) {
			SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
				"YAML parser error: yaml_parse_parse returned 0: %s",
				parser.problem);
			probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
			SEXP_free(msg);
			probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
			ret = -1;
			goto cleanup;
		}

		event_type = event.type;
		if (!yaml_path_filter_event(yaml_path, &parser, &event,
				YAML_PATH_FILTER_RETURN_ALL)) {
			goto next;
		}
		if (sequence) {
			if (event_type == YAML_SEQUENCE_END_EVENT) {
				sequence = false;
			} else if (event_type != YAML_SCALAR_EVENT) {
				SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
					"YAML path '%s' contains non-scalar in a sequence.",
					yaml_path_cstr);
				probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
				SEXP_free(msg);
				probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
				ret = -1;
				goto cleanup;
			}
		} else {
			if (event_type == YAML_SEQUENCE_START_EVENT) {
				sequence = true;
			}
			if (event_type == YAML_MAPPING_START_EVENT) {
				SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
					"YAML path '%s' matches a mapping.",
					yaml_path_cstr);
				probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
				SEXP_free(msg);
				probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
				ret = -1;
				goto cleanup;
			}
		}
		if (event_type == YAML_SCALAR_EVENT) {
			SEXP_t *sexp = yaml_scalar_event_to_sexp(&event);
			if (sexp == NULL) {
				SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
					"Can't convert '%s %s' to SEXP", event.data.scalar.tag,
					event.data.scalar.value);
				probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
				SEXP_free(msg);
				probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
				ret = -1;
				goto cleanup;
			}
			oscap_list_add(values, sexp);
		}
next:
		yaml_event_delete(&event);
	} while (event_type != YAML_STREAM_END_EVENT);

cleanup:
	yaml_parser_delete(&parser);
	yaml_path_destroy(yaml_path);
	fclose(yaml_file);

	return ret;
}

static int process_yaml_file(const char *prefix, const char *path, const char *filename, const char *yamlpath, probe_ctx *ctx)
{
	int ret = 0;
	char *filepath = oscap_path_join(path, filename);
	struct oscap_list *values = oscap_list_new();
	char *filepath_with_prefix = oscap_path_join(prefix, filepath);

	if (yaml_path_query(filepath_with_prefix, yamlpath, values, ctx)) {
		ret = -1;
		goto cleanup;
	}

	struct oscap_iterator *values_it = oscap_iterator_new(values);
	if (oscap_iterator_has_more(values_it)) {
		SEXP_t *item = probe_item_create(
			OVAL_INDEPENDENT_YAML_FILE_CONTENT,
			NULL,
			"filepath", OVAL_DATATYPE_STRING, filepath,
			"path", OVAL_DATATYPE_STRING, path,
			"filename", OVAL_DATATYPE_STRING, filename,
			"yamlpath", OVAL_DATATYPE_STRING, yamlpath,
			/*
			"windows_view",
			*/
			NULL
		);
		while (oscap_iterator_has_more(values_it)) {
			SEXP_t *value_sexp = oscap_iterator_next(values_it);
			probe_item_ent_add(item, "value_of", NULL, value_sexp);
		}
		probe_item_collect(ctx, item);
	}
	oscap_iterator_free(values_it);

cleanup:
	oscap_list_free(values, (oscap_destruct_func) SEXP_free);
	free(filepath_with_prefix);
	free(filepath);
	return ret;
}

int yamlfilecontent_probe_main(probe_ctx *ctx, void *arg)
{
	SEXP_t *probe_in = probe_ctx_getobject(ctx);
	SEXP_t *behaviors_ent = probe_obj_getent(probe_in, "behaviors", 1);
	SEXP_t *filepath_ent = probe_obj_getent(probe_in, "filepath", 1);
	SEXP_t *path_ent = probe_obj_getent(probe_in, "path", 1);
	SEXP_t *filename_ent = probe_obj_getent(probe_in, "filename", 1);
	SEXP_t *yamlpath_ent = probe_obj_getent(probe_in, "yamlpath", 1);
	SEXP_t *yamlpath_val = probe_ent_getval(yamlpath_ent);
	char *yamlpath_str = SEXP_string_cstr(yamlpath_val);

	probe_filebehaviors_canonicalize(&behaviors_ent);
	const char *prefix = getenv("OSCAP_PROBE_ROOT");
	OVAL_FTS *ofts = oval_fts_open_prefixed(
		prefix, path_ent, filename_ent, filepath_ent, behaviors_ent,
		probe_ctx_getresult(ctx));
	if (ofts != NULL) {
		OVAL_FTSENT *ofts_ent;
		while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
			if (ofts_ent->fts_info == FTS_F
			    || ofts_ent->fts_info == FTS_SL) {
				process_yaml_file(prefix, ofts_ent->path, ofts_ent->file,
					yamlpath_str, ctx);
			}
			oval_ftsent_free(ofts_ent);
		}
		oval_fts_close(ofts);
	}

	free(yamlpath_str);
	SEXP_free(yamlpath_val);
	SEXP_free(yamlpath_ent);
	SEXP_free(filename_ent);
	SEXP_free(path_ent);
	SEXP_free(filepath_ent);
	SEXP_free(behaviors_ent);
	return 0;
}
