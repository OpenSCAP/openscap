/**
 * @file oval_varModel.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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

typedef struct _oval_variable_model_frame {
	char                   *id;
	char                   *comment;
	struct oval_collection *values;
	oval_datatype_t         datatype;
} _oval_variable_model_frame_t;

_oval_variable_model_frame_t *_oval_variable_model_frame_new
	(char *id, char *comment, oval_datatype_t datatype)
{
	_oval_variable_model_frame_t *frame = (_oval_variable_model_frame_t *)malloc(sizeof(_oval_variable_model_frame_t));
	frame->id       = strdup(id);
	frame->comment  = strdup(comment);
	frame->datatype = datatype;
	frame->values   = oval_collection_new();
	return frame;
}

void _oval_variable_model_frame_free(_oval_variable_model_frame_t *frame)
{
	if(frame){
		if(frame->id)free(frame->id);
		if(frame->comment)free(frame->comment);
		oval_collection_free_items(frame->values, free);
		frame->id       = NULL;
		frame->comment  = NULL;
		frame->values   = NULL;
		frame->datatype = 0;
		free(frame);
	}
}


typedef struct oval_variable_model {
	struct oval_string_map *varmap;
} oval_variable_model_t;

struct oval_variable_model *oval_variable_model_new()
{
	oval_variable_model_t *model = (oval_variable_model_t *)malloc(sizeof(oval_variable_model_t));
	model->varmap = oval_string_map_new();
	return model;
}

void oval_variable_model_free(struct oval_variable_model *model)
{
	oval_string_map_free(model->varmap, (oscap_destruct_func)_oval_variable_model_frame_free);
	model->varmap = NULL;
	free(model);
}

#define NAMESPACE_VARIABLES "http://oval.mitre.org/XMLSchema/oval-variables-5"
#define NAMESPACE_COMMON    "http://oval.mitre.org/XMLSchema/oval-common-5"

int _oval_generator_parse_tag
	(xmlTextReader *reader,
	 struct oval_parser_context *context, char *label)
{
	char *tagname   = (char*) xmlTextReaderLocalName(reader);
	char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
	int  return_code;
	char message[200]; *message = 0;
	bool is_common_ns =strcmp(NAMESPACE_COMMON, namespace)==0;
	if(is_common_ns && strcmp("product_name", tagname)==0){
		char *value = xmlTextReaderValue(reader);
		sprintf(message,
				"%s:    product name: %s",label, value);
		oval_parser_log_info(context, message);
		free(value);
		return_code = 1;
	}else if(is_common_ns && strcmp("product_version", tagname)==0){
		char *value = xmlTextReaderValue(reader);
		sprintf(message,
				"%s: product version: %s",label, value);
		oval_parser_log_info(context, message);
		free(value);
		return_code = 1;
	}else if(is_common_ns && strcmp("schema_version", tagname)==0){
		char *value = xmlTextReaderValue(reader);
		sprintf(message,
				"%s:  schema version: %s",label, value);
		oval_parser_log_info(context, message);
		free(value);
		return_code = 1;
	}else if(is_common_ns && strcmp("timestamp", tagname)==0){
		char *value = xmlTextReaderValue(reader);
		sprintf(message,
				"%s:      time stamp: %s",label, value);
		oval_parser_log_info(context, message);
		free(value);
		return_code = 1;
	}else{
		sprintf(message,
				"UNPROCESSED TAG <%s:%s>",
				namespace, tagname);
		oval_parser_log_warn(context, message);
		oval_parser_skip_tag(reader,context);
		return_code = 0;
	}
	free(tagname);
	free(namespace);
	return return_code;
}

int _oval_variable_model_parse_variable_values
	(xmlTextReader *reader,
	 struct oval_parser_context *context,
	 _oval_variable_model_frame_t *frame)
{
	char *tagname   = (char*) xmlTextReaderLocalName(reader);
	char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
	int  return_code;
	bool is_variable_ns =strcmp(NAMESPACE_VARIABLES, namespace)==0;
	if(is_variable_ns && strcmp("value", tagname)==0){
		char *value = (char*)xmlTextReaderValue(reader);
		oval_collection_add(frame->values, strdup(value));
		free(value);
	}else{
		char message[200]; *message = 0;
		sprintf(message,
				"UNPROCESSED TAG <%s:%s>",
				namespace, tagname);
		oval_parser_log_warn(context, message);
		oval_parser_skip_tag(reader,context);
		return_code = 0;
	}
	free(tagname);
	free(namespace);
	return return_code;
}
int _oval_variable_model_parse_variable
	(xmlTextReader *reader,
	 struct oval_parser_context *context,
	 struct oval_variable_model *model)
{
	char *id      = xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	char *comment = xmlTextReaderGetAttribute(reader, BAD_CAST "comment");
	oval_datatype_t datatype = oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
	_oval_variable_model_frame_t *frame = oval_string_map_get_value(model->varmap, id);
	int return_code = 1;
	if(frame){
		if(frame->datatype!=datatype){
			char message[200]; *message = 0;
			sprintf(message,
					"Unmatched variable datatypes %s:%s",
					oval_datatype_get_text(frame->datatype), oval_datatype_get_text(datatype));
			oval_parser_log_warn(context, message);
			oval_parser_skip_tag(reader,context);
			return_code = 0;
		}
		frame = NULL;
	}else{
		frame = _oval_variable_model_frame_new(id, comment, datatype);
		oval_string_map_put(model->varmap, id, frame);
	}
	return_code = oval_parser_parse_tag(reader, context, (oval_xml_tag_parser)_oval_variable_model_parse_variable_values, frame);
	free(id);
	free(comment);
	return return_code;
}

int _oval_variable_model_parse_variables
	(xmlTextReader *reader,
	 struct oval_parser_context *context,
	 struct oval_variable_model *model)
{
	char *tagname   = (char*) xmlTextReaderLocalName(reader);
	char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
	int  return_code;
	bool is_variable_ns =strcmp(NAMESPACE_VARIABLES, namespace)==0;
	if(is_variable_ns && strcmp("variable", tagname)==0){
		return_code = _oval_variable_model_parse_variable(reader, context, model);
	}else{
		char message[200]; *message = 0;
		sprintf(message,
				"UNPROCESSED TAG <%s:%s>",
				namespace, tagname);
		oval_parser_log_warn(context, message);
		oval_parser_skip_tag(reader,context);
		return_code = 0;
	}
	free(tagname);
	free(namespace);
	return return_code;
}

int _oval_variable_model_parse_tag
	(xmlTextReader *reader,
	 struct oval_parser_context *context,
	 struct oval_variable_model *model)
{
	char *tagname   = (char*) xmlTextReaderLocalName(reader);
	char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
	int  return_code;
	bool is_variable_ns =strcmp(NAMESPACE_VARIABLES, namespace)==0;
	if(is_variable_ns && strcmp("generator", tagname)==0){
		return_code = oval_parser_parse_tag(reader, context, (oval_xml_tag_parser)_oval_generator_parse_tag, "oval_variables");
	}else if(is_variable_ns && strcmp("variables", tagname)==0){
		return_code = oval_parser_parse_tag(reader, context, (oval_xml_tag_parser)_oval_variable_model_parse_variables, model);
	}else{
		char message[200]; *message = 0;
		sprintf(message,
				"UNPROCESSED TAG <%s:%s>",
				namespace, tagname);
		oval_parser_log_warn(context, message);
		oval_parser_skip_tag(reader,context);
		return_code = 0;
	}
	free(tagname);
	free(namespace);
	return return_code;
}


void _oval_variable_model_parse
	(struct oval_variable_model *model,
	 xmlTextReader *reader,
	 oval_xml_error_handler error_handler, void *user_param)
{
	struct oval_parser_context context;
	context.error_handler       = error_handler;
	context.reader              = reader;
	context.user_data           = user_param;
	xmlTextReaderSetErrorHandler(reader, &libxml_error_handler, &context);
	char *tagname   = (char*) xmlTextReaderLocalName(reader);
	char *namespace = (char*) xmlTextReaderNamespaceUri(reader);
	bool is_variables =
		(strcmp(NAMESPACE_VARIABLES, namespace)==0) &&
		(strcmp("oval_variables", tagname)== 0);
	if(is_variables){
		oval_parser_parse_tag(reader,&context,(oval_xml_tag_parser)_oval_variable_model_parse_tag, model);
	}else{
		char message[200]; *message = 0;
		sprintf(message,
				"UNPROCESSED TAG <%s:%s>",
				namespace, tagname);
		oval_parser_log_warn(&context, message);
		oval_parser_skip_tag(reader,&context);
	}
	free(tagname);
	free(namespace);
}

void oval_variable_model_import
	(struct oval_variable_model *model,
	 struct oval_import_source *source,
	 oval_xml_error_handler error_handler, void *user_param)
{
	xmlDoc *doc = xmlParseFile
		(source->import_source_filename);
	xmlTextReader *reader = xmlNewTextReaderFilename
		(source->import_source_filename);

	xmlTextReaderRead(reader);
	_oval_variable_model_parse
		(model, reader, error_handler, user_param);

	xmlFreeTextReader(reader);
	xmlFreeDoc(doc);
}

void oval_variable_model_export
	(struct oval_variable_model *model,
	 struct oval_export_target *target)
{
	//TODO: implement oval_variable_model_export
}

struct oval_string_iterator *oval_variable_model_get_variable_ids
	(struct oval_variable_model *model)
{
	return (struct oval_string_iterator *)oval_string_map_keys(model->varmap);
}

oval_datatype_t oval_variable_model_get_datatype
	(struct oval_variable_model *model, char *varid)
{
	_oval_variable_model_frame_t *frame = oval_string_map_get_value(model->varmap, varid);
	return (frame)?frame->datatype:OVAL_DATATYPE_UNKNOWN;
}

const char *oval_variable_model_get_comment
	(struct oval_variable_model *model, char *varid)
{
	_oval_variable_model_frame_t *frame = oval_string_map_get_value(model->varmap, varid);
	return (frame)?frame->comment:NULL;
}

struct oval_string_iterator *oval_variable_model_get_values
(struct oval_variable_model *model, char *varid)
{
	_oval_variable_model_frame_t *frame = oval_string_map_get_value(model->varmap, varid);
	return (frame)?(struct oval_string_iterator *)oval_collection_iterator(frame->values):NULL;
}
