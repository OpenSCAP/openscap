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

#include <errno.h>
#include <yaml.h>
#include <yaml-path.h>

#include "yamlfilecontent_probe.h"
#include "sexp-manip.h"
#include "debug_priv.h"
#include "oval_fts.h"


static int yaml_path_query(const char *filepath, const char *yaml_path_cstr, unsigned char *output_buffer, size_t output_buffer_size, probe_ctx *ctx)
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
		fclose(yaml_file);
		return -1;
	};

	yaml_parser_t parser;
	yaml_parser_initialize(&parser);
	yaml_parser_set_input_file(&parser, yaml_file);

	yaml_emitter_t emitter;
	yaml_emitter_initialize(&emitter);
	size_t size_written;
	yaml_emitter_set_output_string(&emitter,
		output_buffer, output_buffer_size, &size_written);
	yaml_emitter_set_width(&emitter, -1);

	yaml_event_t event;
	bool done = false;

	bool sequence = false;
	do {
		if (yaml_parser_parse(&parser, &event)) {
			done = (event.type == YAML_STREAM_END_EVENT);
			if (yaml_path_filter_event(yaml_path, &parser, &event,
					YAML_PATH_FILTER_RETURN_ALL)) {

				if (sequence) {
					if (event.type == YAML_SEQUENCE_END_EVENT) {
						sequence = false;
					} else if (event.type != YAML_SCALAR_EVENT) {
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
					if (event.type == YAML_SEQUENCE_START_EVENT) {
						sequence = true;
					}
					if (event.type == YAML_MAPPING_START_EVENT) {
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

				if (!yaml_emitter_emit(&emitter, &event)) {
					SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
						"YAML emitter error: yaml_emitter_emit returned 0: %s",
						emitter.problem);
					probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
					SEXP_free(msg);
					probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
					ret = -1;
					goto cleanup;
				}
			} else {
				yaml_event_delete(&event);
			}
		} else {
			SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
				"YAML parser error: yaml_parse_parse returned 0: %s",
				parser.problem);
			probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
			SEXP_free(msg);
			probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
			ret = -1;
			goto cleanup;
		}
	} while (!done);

	/* string output_buffer contains '\n' at the end */
	output_buffer[size_written - 1] = '\0';

cleanup:
	yaml_parser_delete(&parser);
	yaml_emitter_delete(&emitter);

	yaml_path_destroy(yaml_path);
	fclose(yaml_file);

	return ret;
}

static int process_yaml_file(const char *path, const char *filename, const char *yamlpath, probe_ctx *ctx)
{
	int ret = 0;
	char *filepath = oscap_path_join(path, filename);

	size_t output_buffer_size = 1024;
	unsigned char *output_buffer = calloc(output_buffer_size, sizeof(unsigned char));

	if (yaml_path_query(filepath, yamlpath, output_buffer, output_buffer_size, ctx)) {
		ret = -1;
		goto cleanup;
	}

	/* TODO: type conversion of output_buffer data */

	SEXP_t *item = probe_item_create(
		OVAL_INDEPENDENT_YAML_FILE_CONTENT,
		NULL,
		"filepath", OVAL_DATATYPE_STRING, filepath,
		"path", OVAL_DATATYPE_STRING, path,
		"filename", OVAL_DATATYPE_STRING, filename,
		"yamlpath", OVAL_DATATYPE_STRING, yamlpath,
		"value_of", OVAL_DATATYPE_STRING, output_buffer,
		/*
		"windows_view",
		*/
		NULL
	);
	probe_item_collect(ctx, item);

cleanup:
	free(output_buffer);
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
	OVAL_FTS *ofts = oval_fts_open_prefixed(
		NULL, path_ent, filename_ent, filepath_ent, behaviors_ent,
		probe_ctx_getresult(ctx));
	if (ofts != NULL) {
		OVAL_FTSENT *ofts_ent;
		while ((ofts_ent = oval_fts_read(ofts)) != NULL) {
			if (ofts_ent->fts_info == FTS_F
			    || ofts_ent->fts_info == FTS_SL) {
				process_yaml_file(
					ofts_ent->path, ofts_ent->file, yamlpath_str, ctx);
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
