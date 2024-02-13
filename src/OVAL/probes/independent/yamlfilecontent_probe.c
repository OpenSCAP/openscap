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
#include <yaml.h>
#include <yaml-path.h>

#include "yamlfilecontent_probe.h"
#include "sexp-manip.h"
#include "debug_priv.h"
#include "oval_fts.h"
#include "list.h"
#include "probe/probe.h"
#include "common/oscap_pcre.h"

#define OSCAP_YAML_STRING_TAG "tag:yaml.org,2002:str"
#define OSCAP_YAML_BOOL_TAG "tag:yaml.org,2002:bool"
#define OSCAP_YAML_FLOAT_TAG "tag:yaml.org,2002:float"
#define OSCAP_YAML_INT_TAG "tag:yaml.org,2002:int"
#define OSCAP_YAML_NULL_TAG "tag:yaml.org,2002:null"

#define OVECCOUNT 30 /* should be a multiple of 3 */

int yamlfilecontent_probe_offline_mode_supported()
{
	return PROBE_OFFLINE_OWN;
}

static bool match_regex(const char *pattern, const char *value)
{
	char *errptr;
	int erroroffset;
	oscap_pcre_t *re = oscap_pcre_compile(pattern, 0, &errptr, &erroroffset);
	if (re == NULL) {
		dE("oscap_pcre_compile failed on pattern '%s': %s at %d", pattern,
			errptr, erroroffset);
		oscap_pcre_err_free(errptr);
		return false;
	}
	int ovector[OVECCOUNT];
	int rc = oscap_pcre_exec(re, value, strlen(value), 0, 0, ovector, OVECCOUNT);
	oscap_pcre_free(re);
	if (rc > 0) {
		return true;
	}
	return false;
}

static SEXP_t *yaml_scalar_event_to_sexp(yaml_event_t *event)
{
	char *tag = (char *) event->data.scalar.tag;
	char *value = (char *) event->data.scalar.value;

	/* Nodes lacking an explicit tag are given a non-specific tag:
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
	if (question || !strcmp(tag, OSCAP_YAML_NULL_TAG)) {
		if (match_regex("^(null|Null|NULL|~|)$", value)) {
			// TODO: Return real NULL when record's field will support nil="true"
			return SEXP_string_new("(null)", strlen("(null)"));
		} else if (!question) {
			return NULL;
		}
	}

	return SEXP_string_new(value, strlen(value));
}

static char *escape_key(char *key)
{
	if (key == NULL)
		return NULL;

	size_t cap_letters = 0;
	size_t key_len = strlen(key);
	for (size_t i = 0; i < key_len; i++)
		if ((key[i] >= 'A' && key[i] <= 'Z') || key[i] == '^')
			cap_letters++;

	if (cap_letters == 0)
		return key;

	char *new_key = realloc(key, key_len + 1 + cap_letters);
	if (new_key == NULL)
		return key;
	new_key[key_len + cap_letters] = '\0';

	for (ssize_t i = key_len; i >= 0; i--) {
		if ((new_key[i] >= 'A' && new_key[i] <= 'Z') || new_key[i] == '^') {
			if (new_key[i] != '^')
				new_key[i] += 32;
			memmove(new_key + i + cap_letters, new_key + i, key_len - i);
			new_key[i + cap_letters - 1] = '^';
			cap_letters--;
			key_len = i;
		}
	}

	return new_key;
}

#define result_error(fmt, args...)                                       \
do {                                                                     \
	SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR, fmt, args); \
	probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);                   \
	SEXP_free(msg);                                                      \
	probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);   \
	ret = -1;                                                            \
} while (0)

static int yaml_path_query(yaml_parser_t *parser, const char *yaml_path_cstr, struct oscap_list *values, probe_ctx *ctx)
{
	int ret = 0;

	yaml_path_t *yaml_path = yaml_path_create();
	if (yaml_path_parse(yaml_path, (char *) yaml_path_cstr)) {
		result_error("Invalid YAML path '%s': %s", yaml_path_cstr, yaml_path_error_get(yaml_path)->message);
		yaml_path_destroy(yaml_path);
		return ret;
	};

	yaml_event_t event;
	yaml_event_type_t event_type;
	bool sequence = false;
	bool mapping = false;
	bool fake_mapping = false;
	int index = 0;
	char *key = strdup("#");

	struct oscap_htable *record = NULL;

	do {
		if (!yaml_parser_parse(parser, &event)) {
			result_error("YAML parser error: %s", parser->problem);
			goto cleanup;
		}

		event_type = event.type;

		if (yaml_path_filter_event(yaml_path, parser, &event) == YAML_PATH_FILTER_RESULT_OUT) {
			goto next;
		}

		if (sequence) {
			if (event_type == YAML_SEQUENCE_END_EVENT) {
				if (fake_mapping) {
					fake_mapping = false;
					if (record && record->itemcount > 0) {
						oscap_list_add(values, record);
					} else {
						// Do not collect empty records
						oscap_htable_free0(record);
					}
					record = NULL;
				} else {
					sequence = false;
				}
			} else if (event_type == YAML_SEQUENCE_START_EVENT) {
				if (mapping || fake_mapping) {
					result_error("YAML path '%s' points to a multi-dimensional structure (a map or a sequence containing other sequences)", yaml_path_cstr);
					goto cleanup;
				} else {
					fake_mapping = true;
					record = oscap_htable_new();
				}
			}
		} else {
			if (event_type == YAML_SEQUENCE_START_EVENT) {
				sequence = true;
				if (mapping)
					index++;
			}
		}

		if (mapping) {
			if (event_type == YAML_MAPPING_END_EVENT) {
				mapping = false;
				if (record && record->itemcount > 0) {
					oscap_list_add(values, record);
				} else {
					// Do not collect empty records
					oscap_htable_free0(record);
				}
				record = NULL;
			} else if (event_type == YAML_MAPPING_START_EVENT) {
				result_error("YAML path '%s' points to a multi-dimensional structure (map containing another map)", yaml_path_cstr);
				goto cleanup;
			}
		} else {
			if (event_type == YAML_MAPPING_START_EVENT) {
				if (record) {
					result_error("YAML path '%s' points to an invalid structure (map containing another map)", yaml_path_cstr);
					goto cleanup;
				}
				if (fake_mapping) {
					result_error("YAML path '%s' points to a multi-dimensional structure (two-dimensional sequence containing a map)", yaml_path_cstr);
					goto cleanup;
				}
				mapping = true;
				sequence = false;
				index = 0;
				record = oscap_htable_new();
			}
		}

		if (event_type == YAML_SCALAR_EVENT) {
			if (mapping) {
				if (!sequence) {
					if (index++ % 2 == 0) {
						free(key);
						key = escape_key(strdup((const char *) event.data.scalar.value));
						goto next;
					}
				}
			}

			SEXP_t *sexp = yaml_scalar_event_to_sexp(&event);
			if (sexp == NULL) {
				result_error("Can't convert '%s %s' to SEXP", event.data.scalar.tag, event.data.scalar.value);
				goto cleanup;
			}

			if (!record)
				record = oscap_htable_new();
			struct oscap_list *field = oscap_htable_get(record, key);
			if (!field) {
				field = oscap_list_new();
				oscap_htable_add(record, key, field);
			}

			oscap_list_add(field, sexp);
		}
next:
		yaml_event_delete(&event);
	} while (event_type != YAML_STREAM_END_EVENT);

cleanup:
	if (event.type != 0)
		yaml_event_delete(&event);
	if (record)
		oscap_list_add(values, record);
	free(key);
	yaml_path_destroy(yaml_path);

	return ret;
}

static void record_free(struct oscap_list *items)
{
	oscap_list_free(items, (oscap_destruct_func) SEXP_free);
}

static void values_free(struct oscap_htable *record)
{
	oscap_htable_free(record, (oscap_destruct_func) record_free);
}

static int process_yaml(yaml_parser_t *parser, const char *yamlpath, SEXP_t *item, probe_ctx *ctx)
{
	int ret = 0;

	if (SEXP_typeof(item) != SEXP_TYPE_LIST)
		return -1;

	struct oscap_list *values = oscap_list_new();

	if (yaml_path_query(parser, yamlpath, values, ctx)) {
		ret = -1;
		goto cleanup;
	}

	struct oscap_iterator *values_it = oscap_iterator_new(values);
	if (oscap_iterator_has_more(values_it)) {
		while (oscap_iterator_has_more(values_it)) {
			SEXP_t *result_ent = probe_ent_creat1("value", NULL, NULL);
			probe_ent_setdatatype(result_ent, OVAL_DATATYPE_RECORD);
			struct oscap_htable *record = oscap_iterator_next(values_it);
			struct oscap_htable_iterator *record_it = oscap_htable_iterator_new(record);
			while(oscap_htable_iterator_has_more(record_it)) {
				const struct oscap_htable_item *record_item = oscap_htable_iterator_next(record_it);
				struct oscap_iterator *item_value_it = oscap_iterator_new(record_item->value);
				SEXP_t se_tmp_mem;
				SEXP_t *key = SEXP_string_new_r(&se_tmp_mem, record_item->key, strlen(record_item->key));
				while(oscap_iterator_has_more(item_value_it)) {
					SEXP_t *value_sexp = oscap_iterator_next(item_value_it);
					SEXP_t *field = probe_ent_creat1("field", NULL, value_sexp);
					probe_item_attr_add(field, "name", key);
					SEXP_list_add(result_ent, field);
					SEXP_free(field);
				}
				oscap_iterator_free(item_value_it);
				SEXP_free_r(&se_tmp_mem);
			}
			oscap_htable_iterator_free(record_it);
			SEXP_list_add(item, result_ent);
			SEXP_free(result_ent);
		}
		probe_item_collect(ctx, item);
	}
	oscap_iterator_free(values_it);

cleanup:
	oscap_list_free(values, (oscap_destruct_func) values_free);

	return ret;
}

static int process_yaml_file(const char *prefix, const char *path, const char *filename, const char *yamlpath, probe_ctx *ctx)
{
	int ret = 0;

	char *filepath = oscap_path_join(path, filename);
	if (probe_path_is_blocked(filepath, ctx->blocked_paths)) {
		free(filepath);
		return 0;
	}

	yaml_parser_t parser;
	yaml_parser_initialize(&parser);

	char *filepath_with_prefix = oscap_path_join(prefix, filepath);

	FILE *yaml_file = fopen(filepath_with_prefix, "r");
	if (yaml_file == NULL) {
		result_error("Unable to open file '%s': %s", filepath_with_prefix, strerror(errno));
		goto cleanup;
	}

	yaml_parser_set_input_file(&parser, yaml_file);

	SEXP_t *item = probe_item_create(
		OVAL_INDEPENDENT_YAML_FILE_CONTENT,
		NULL,
		"filepath", OVAL_DATATYPE_STRING, filepath,
		"path", OVAL_DATATYPE_STRING, path,
		"filename", OVAL_DATATYPE_STRING, filename,
		"yamlpath", OVAL_DATATYPE_STRING, yamlpath,
		// TODO: Implement "windows_view",
		NULL
	);

	if (process_yaml(&parser, yamlpath, item, ctx)) {
		SEXP_free(item);
		ret = -1;
		goto cleanup;
	}

cleanup:
	if (yaml_file != NULL)
		fclose(yaml_file);
	yaml_parser_delete(&parser);
	free(filepath_with_prefix);
	free(filepath);

	return ret;
}

static int process_yaml_content(const char *content, const char *yamlpath, probe_ctx *ctx)
{
	int ret = 0;

	yaml_parser_t parser;
	yaml_parser_initialize(&parser);
	yaml_parser_set_input_string(&parser, (unsigned char *) content, strlen(content));

	SEXP_t *item = probe_item_create(
		OVAL_INDEPENDENT_YAML_FILE_CONTENT,
		NULL,
		"content", OVAL_DATATYPE_STRING, content,
		"yamlpath", OVAL_DATATYPE_STRING, yamlpath,
		NULL
	);

	if (process_yaml(&parser, yamlpath, item, ctx)) {
		SEXP_free(item);
		ret = -1;
		goto cleanup;
	}

cleanup:
	yaml_parser_delete(&parser);

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
	SEXP_t *content_ent = probe_obj_getent(probe_in, "content", 1);
	SEXP_t *content_val = probe_ent_getval(content_ent);
	char *content_str = SEXP_string_cstr(content_val);

	if (content_str != NULL) {
		process_yaml_content(content_str, yamlpath_str, ctx);
	} else {
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
	}

	free(content_str);
	SEXP_free(content_val);
	SEXP_free(content_ent);
	free(yamlpath_str);
	SEXP_free(yamlpath_val);
	SEXP_free(yamlpath_ent);
	SEXP_free(filename_ent);
	SEXP_free(path_ent);
	SEXP_free(filepath_ent);
	SEXP_free(behaviors_ent);
	return 0;
}
