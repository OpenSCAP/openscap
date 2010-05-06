/**
 * @file oval_varModel.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */
#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "oval_string_map_impl.h"
#include "oval_system_characteristics_impl.h"
#include "oval_results_impl.h"
#include <string.h>
#include <time.h>
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"

typedef struct _oval_variable_model_frame {
	char *id;
	char *comment;
	struct oval_collection *values;
	oval_datatype_t datatype;
} _oval_variable_model_frame_t;

static _oval_variable_model_frame_t *_oval_variable_model_frame_new(char *id, const char *comm, oval_datatype_t datatype);
static int _oval_variable_model_parse_tag(xmlTextReader * reader, struct oval_parser_context *context,
					  struct oval_variable_model *model);
static void _oval_variable_model_frame_free(_oval_variable_model_frame_t * frame);
static int _oval_variable_model_parse_variable_values(xmlTextReader * reader, struct oval_parser_context *context,
						      _oval_variable_model_frame_t * frame);
static int _oval_variable_model_parse_variable(xmlTextReader * reader, struct oval_parser_context *context,
					       struct oval_variable_model *model);
static int _oval_variable_model_parse_variables(xmlTextReader * reader, struct oval_parser_context *context,
						struct oval_variable_model *model);
static int _oval_variable_model_parse(struct oval_variable_model *model, xmlTextReader * reader, void *user_param);
static int _oval_generator_parse_tag(xmlTextReader * reader, struct oval_parser_context *context, char *label);
static _oval_variable_model_frame_t *_oval_variable_model_frame_new(char *id, const char *comm, oval_datatype_t datatype)
{
	_oval_variable_model_frame_t *frame = (_oval_variable_model_frame_t *) oscap_alloc(sizeof(_oval_variable_model_frame_t));
	if (frame == NULL)
		return NULL;
	frame->id = oscap_strdup(id);
	frame->comment = oscap_strdup(comm);
	frame->datatype = datatype;
	frame->values = oval_collection_new();
	return frame;
}

typedef struct oval_variable_model {
	struct oval_string_map *varmap;
	bool is_locked;
} oval_variable_model_t;
bool oval_variable_model_is_valid(struct oval_variable_model *variable_model)
{
	return true;		//TODO
}

bool oval_variable_model_is_locked(struct oval_variable_model * variable_model)
{
	__attribute__nonnull__(variable_model);
	return variable_model->is_locked;
}

void oval_variable_model_lock(struct oval_variable_model *variable_model)
{
	__attribute__nonnull__(variable_model);
	variable_model->is_locked = true;
} static void _oval_variable_model_frame_free(_oval_variable_model_frame_t * frame)
{
	if (frame) {
		if (frame->id)
			oscap_free(frame->id);
		if (frame->comment)
			oscap_free(frame->comment);
		oval_collection_free_items(frame->values, oscap_free);
		frame->id = NULL;
		frame->comment = NULL;
		frame->values = NULL;
		frame->datatype = 0;
		oscap_free(frame);
	}
}

struct oval_variable_model *oval_variable_model_new()
{
	oval_variable_model_t *model = (oval_variable_model_t *) oscap_alloc(sizeof(oval_variable_model_t));
	if (model == NULL)
		return NULL;
	model->varmap = oval_string_map_new();
	model->is_locked = false;
	return model;
}

struct oval_variable_model *oval_variable_model_clone(struct oval_variable_model *old_model)
{
	struct oval_variable_model *new_model = oval_variable_model_new();
	struct oval_string_iterator *old_varids = oval_variable_model_get_variable_ids(old_model);
	while (oval_string_iterator_has_more(old_varids)) {
		char *varid = oval_string_iterator_next(old_varids);
		oval_datatype_t datatype = oval_variable_model_get_datatype(old_model, varid);
		struct oval_string_iterator *values = oval_variable_model_get_values(old_model, varid);
		const char *comm = oval_variable_model_get_comment(old_model, varid);
		while (oval_string_iterator_has_more(values)) {
			char *value = oval_string_iterator_next(values);
			oval_variable_model_add(new_model, varid, comm, datatype, value);
		} oval_string_iterator_free(values);
	} oval_string_iterator_free(old_varids);
	return new_model;
}

void oval_variable_model_free(struct oval_variable_model *model)
{
	if (model) {
		oval_string_map_free(model->varmap, (oscap_destruct_func) _oval_variable_model_frame_free);
		model->varmap = NULL;
		oscap_free(model);
	}
}

void oval_variable_model_add(struct oval_variable_model *model, char *varid, const char *comm,
			     oval_datatype_t datatype, char *value)
{
	struct _oval_variable_model_frame *frame =
	    (struct _oval_variable_model_frame *)oval_string_map_get_value(model->varmap, varid);
	if (frame == NULL) {
		frame = _oval_variable_model_frame_new(varid, comm, datatype);
	}
	value = oscap_strdup(value);
	oval_collection_add(frame->values, value);
}

#define NAMESPACE_VARIABLES "http://oval.mitre.org/XMLSchema/oval-variables-5"
#define NAMESPACE_COMMON    "http://oval.mitre.org/XMLSchema/oval-common-5"
static int _oval_generator_parse_tag(xmlTextReader * reader, struct oval_parser_context *context, char *label)
{
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int return_code;
	bool is_common_ns = strcmp(NAMESPACE_COMMON, namespace) == 0;
	if (is_common_ns && strcmp("product_name", tagname) == 0) {
		return_code = xmlTextReaderRead(reader);
		char *value = (char *)xmlTextReaderValue(reader);
		oscap_dprintf("INFO: %s:    product name: %s", label, value);
		oscap_free(value);
	} else if (is_common_ns && strcmp("product_version", tagname) == 0) {
		return_code = xmlTextReaderRead(reader);
		char *value = (char *)xmlTextReaderValue(reader);
		oscap_dprintf("INFO: %s: product version: %s", label, value);
		oscap_free(value);
	} else if (is_common_ns && strcmp("schema_version", tagname) == 0) {
		return_code = xmlTextReaderRead(reader);
		char *value = (char *)xmlTextReaderValue(reader);
		oscap_dprintf("INFO: %s:  schema version: %s", label, value);
		oscap_free(value);
	} else if (is_common_ns && strcmp("timestamp", tagname) == 0) {
		return_code = xmlTextReaderRead(reader);
		char *value = (char *)xmlTextReaderValue(reader);
		oscap_dprintf("INFO: %s:      time stamp: %s", label, value);
		oscap_free(value);
	} else {
		oscap_dprintf("WARNING: UNPROCESSED TAG <%s:%s>", namespace, tagname);
		oval_parser_skip_tag(reader, context);
		return_code = 0;
	}
	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}

static int _oval_variable_model_parse_variable_values
    (xmlTextReader * reader, struct oval_parser_context *context, _oval_variable_model_frame_t * frame) {
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int return_code;
	bool is_variable_ns = strcmp(NAMESPACE_VARIABLES, namespace) == 0;
	if (is_variable_ns && strcmp("value", tagname) == 0) {
		return_code = xmlTextReaderRead(reader);
		char *value = (char *)xmlTextReaderValue(reader);
		oval_collection_add(frame->values, strdup(value));
		oscap_free(value);
	} else {
		oscap_dprintf("WARNING: UNPROCESSED TAG <%s:%s>", namespace, tagname);
		oval_parser_skip_tag(reader, context);
		return_code = 0;
	}
	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}

static int _oval_variable_model_parse_variable
    (xmlTextReader * reader, struct oval_parser_context *context, struct oval_variable_model *model) {
	char *id = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	char *comm = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "comment");
	oval_datatype_t datatype = oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
	_oval_variable_model_frame_t *frame = oval_string_map_get_value(model->varmap, id);
	int return_code = 1;
	if (frame) {
		if (frame->datatype != datatype) {
			oscap_dprintf("WARNING: Unmatched variable datatypes %s:%s",
				      oval_datatype_get_text(frame->datatype), oval_datatype_get_text(datatype));
			oval_parser_skip_tag(reader, context);
			return_code = 0;
		}
		frame = NULL;
	} else {
		frame = _oval_variable_model_frame_new(id, comm, datatype);
		oval_string_map_put(model->varmap, id, frame);
	}
	return_code =
	    oval_parser_parse_tag(reader, context, (oval_xml_tag_parser) _oval_variable_model_parse_variable_values, frame);
	oscap_free(id);
	oscap_free(comm);
	return return_code;
}

static int _oval_variable_model_parse_variables
    (xmlTextReader * reader, struct oval_parser_context *context, struct oval_variable_model *model) {
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int return_code;
	bool is_variable_ns = strcmp(NAMESPACE_VARIABLES, namespace) == 0;
	if (is_variable_ns && strcmp("variable", tagname) == 0) {
		return_code = _oval_variable_model_parse_variable(reader, context, model);
	} else {
		oscap_dprintf("WARNING: UNPROCESSED TAG <%s:%s>", namespace, tagname);
		oval_parser_skip_tag(reader, context);

		/*oscap_seterr */
		return_code = 0;
	}
	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}

static int _oval_variable_model_parse_tag
    (xmlTextReader * reader, struct oval_parser_context *context, struct oval_variable_model *model) {
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int return_code;
	bool is_variable_ns = strcmp(NAMESPACE_VARIABLES, namespace) == 0;
	if (is_variable_ns && strcmp("generator", tagname) == 0) {
		return_code =
		    oval_parser_parse_tag(reader, context, (oval_xml_tag_parser) _oval_generator_parse_tag, "oval_variables");
	} else if (is_variable_ns && strcmp("variables", tagname) == 0) {
		return_code =
		    oval_parser_parse_tag(reader, context, (oval_xml_tag_parser) _oval_variable_model_parse_variables, model);
	} else {
		oscap_dprintf("WARNING: UNPROCESSED TAG <%s:%s>", namespace, tagname);
		oval_parser_skip_tag(reader, context);
		return_code = 0;
	}
	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}

static int _oval_variable_model_parse(struct oval_variable_model *model, xmlTextReader * reader, void *user_param)
{
	int return_code = 0;
	struct oval_parser_context context;
	context.reader = reader;
	context.user_data = user_param;
	xmlTextReaderSetErrorHandler(reader, &libxml_error_handler, &context);
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	bool is_variables = (strcmp(NAMESPACE_VARIABLES, namespace) == 0) && (strcmp("oval_variables", tagname) == 0);
	if (is_variables) {
		return_code =
		    oval_parser_parse_tag(reader, &context, (oval_xml_tag_parser) _oval_variable_model_parse_tag, model);
	} else {
		oscap_dprintf("WARNING: UNPROCESSED TAG <%s:%s>", namespace, tagname);
		return_code = oval_parser_skip_tag(reader, &context);
	}
	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}

int oval_variable_model_import(struct oval_variable_model *model, struct oscap_import_source *source, void *user_param)
{
	int return_code;
	xmlDoc *doc = xmlParseFile(oscap_import_source_get_name(source));
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}
	xmlTextReader *reader = xmlNewTextReaderFilename(oscap_import_source_get_name(source));
	if (reader == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}
	xmlTextReaderRead(reader);
	return_code = _oval_variable_model_parse(model, reader, user_param);
	xmlFreeTextReader(reader);
	xmlFreeDoc(doc);
	return return_code;
}

int oval_variable_model_export(struct oval_variable_model *model, struct oscap_export_target *target)
{

	//TODO: implement oval_variable_model_export
	return 0;
}

struct oval_string_iterator *oval_variable_model_get_variable_ids(struct oval_variable_model *model)
{
	__attribute__nonnull__(model);
	return (struct oval_string_iterator *)oval_string_map_keys(model->varmap);
} oval_datatype_t oval_variable_model_get_datatype(struct oval_variable_model *model, char *varid)
{

	__attribute__nonnull__(model);
	_oval_variable_model_frame_t *frame = oval_string_map_get_value(model->varmap, varid);
	return (frame) ? frame->datatype : OVAL_DATATYPE_UNKNOWN;
}

const char *oval_variable_model_get_comment(struct oval_variable_model *model, char *varid)
{
	__attribute__nonnull__(model);
	_oval_variable_model_frame_t *frame = oval_string_map_get_value(model->varmap, varid);
	return (frame) ? frame->comment : NULL;
}

struct oval_string_iterator *oval_variable_model_get_values(struct oval_variable_model *model, char *varid)
{
	__attribute__nonnull__(model);
	_oval_variable_model_frame_t *frame = oval_string_map_get_value(model->varmap, varid);
	return (frame) ? (struct oval_string_iterator *)oval_collection_iterator(frame->values) : NULL;
}
