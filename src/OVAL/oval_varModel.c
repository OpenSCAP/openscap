/**
 * @file oval_varModel.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <time.h>

#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "adt/oval_string_map_impl.h"
#include "oval_system_characteristics_impl.h"
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"
#include "common/elements.h"
#include "oscap_source.h"
#include "source/oscap_source_priv.h"

typedef struct _oval_variable_model_frame {
	char *id;
	char *comment;
	struct oval_collection *values;
	oval_datatype_t datatype;
} _oval_variable_model_frame_t;

typedef struct oval_variable_model {
	struct oval_generator *generator;
	struct oval_string_map *varmap;
} oval_variable_model_t;

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

bool oval_variable_model_iterator_has_more(struct oval_variable_model_iterator *itr)
{
	return oval_collection_iterator_has_more((struct oval_iterator *) itr);
}

struct oval_variable_model *oval_variable_model_iterator_next(struct oval_variable_model_iterator *itr)
{
	return (struct oval_variable_model *) oval_collection_iterator_next((struct oval_iterator *) itr);
}

void oval_variable_model_iterator_free(struct oval_variable_model_iterator *itr)
{
	oval_collection_iterator_free((struct oval_iterator *) itr);
}

static void _oval_variable_model_frame_free(_oval_variable_model_frame_t * frame)
{
	if (frame) {
		if (frame->id)
			oscap_free(frame->id);
		if (frame->comment)
			oscap_free(frame->comment);
		oval_collection_free_items(frame->values, (oscap_destruct_func) oval_value_free);
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
	model->generator = oval_generator_new();
	model->varmap = oval_string_map_new();
	return model;
}

struct oval_variable_model *oval_variable_model_clone(struct oval_variable_model *old_model)
{
	struct oval_variable_model *new_model = oval_variable_model_new();
	struct oval_string_iterator *old_varids = oval_variable_model_get_variable_ids(old_model);
	while (oval_string_iterator_has_more(old_varids)) {
		char *varid = oval_string_iterator_next(old_varids);
		oval_datatype_t datatype = oval_variable_model_get_datatype(old_model, varid);
		struct oval_value_iterator *values = oval_variable_model_get_values(old_model, varid);
		const char *comm = oval_variable_model_get_comment(old_model, varid);
		while (oval_value_iterator_has_more(values)) {
			struct oval_value *ov;
			char *text;

			ov = oval_value_iterator_next(values);
			text = oval_value_get_text(ov);
			oval_variable_model_add(new_model, varid, comm, datatype, text);
		}
		oval_value_iterator_free(values);
	} oval_string_iterator_free(old_varids);
	return new_model;
}

void oval_variable_model_free(struct oval_variable_model *model)
{
	if (model) {
		oval_string_map_free(model->varmap, (oscap_destruct_func) _oval_variable_model_frame_free);
		model->varmap = NULL;
		oval_generator_free(model->generator);
		oscap_free(model);
	}
}

struct oval_generator *oval_variable_model_get_generator(struct oval_variable_model *model)
{
	return model->generator;
}

void oval_variable_model_set_generator(struct oval_variable_model *model, struct oval_generator *generator)
{
	oval_generator_free(model->generator);
	model->generator = generator;
}

void oval_variable_model_add(struct oval_variable_model *model, char *varid, const char *comm,
			     oval_datatype_t datatype, char *value)
{
	struct oval_value *ov;

	struct _oval_variable_model_frame *frame =
	    (struct _oval_variable_model_frame *)oval_string_map_get_value(model->varmap, varid);
	if (frame == NULL) {
		frame = _oval_variable_model_frame_new(varid, comm, datatype);
		oval_string_map_put(model->varmap, varid, frame);
	}
	ov = oval_value_new(datatype, value);
	oval_collection_add(frame->values, ov);
}

#define NAMESPACE_VARIABLES "http://oval.mitre.org/XMLSchema/oval-variables-5"

static int _oval_variable_model_parse_variable_values
    (xmlTextReader * reader, struct oval_parser_context *context, _oval_variable_model_frame_t * frame) {
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int return_code;
	bool is_variable_ns = strcmp(NAMESPACE_VARIABLES, namespace) == 0;
	if (is_variable_ns && strcmp("value", tagname) == 0) {
		struct oval_value *ov;

		return_code = xmlTextReaderRead(reader);
		char *value = (char *)xmlTextReaderValue(reader);
		ov = oval_value_new(frame->datatype, value);
		oval_collection_add(frame->values, ov);
		oscap_free(value);
	} else {
		oscap_dlprintf(DBG_W, "Unprocessed tag: <%s:%s>.\n", namespace, tagname);
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
			oscap_dlprintf(DBG_W, "Unmatched variable datatypes: %s:%s.\n",
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
		oscap_dlprintf(DBG_W, "Unprocessed tag: <%s:%s>.\n", namespace, tagname);
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
		struct oval_generator *gen;

		gen = oval_variable_model_get_generator(context->variable_model);
		return_code = oval_generator_parse_tag(reader, context, gen);
	} else if (is_variable_ns && strcmp("variables", tagname) == 0) {
		return_code =
		    oval_parser_parse_tag(reader, context, (oval_xml_tag_parser) _oval_variable_model_parse_variables, model);
	} else {
		oscap_dlprintf(DBG_W, "Unprocessed tag: <%s:%s>.\n", namespace, tagname);
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
	context.variable_model = model;
	context.reader = reader;
	context.user_data = user_param;
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	bool is_variables = (strcmp(NAMESPACE_VARIABLES, namespace) == 0) && (strcmp(OVAL_ROOT_ELM_VARIABLES, tagname) == 0);
	if (is_variables) {
		return_code =
		    oval_parser_parse_tag(reader, &context, (oval_xml_tag_parser) _oval_variable_model_parse_tag, model);
	} else {
		oscap_dlprintf(DBG_W, "Unprocessed tag: <%s:%s>.\n", namespace, tagname);
		return_code = oval_parser_skip_tag(reader, &context);
	}
	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}

struct oval_variable_model *oval_variable_model_import_source(struct oscap_source *source)
{
	int ret;
	xmlTextReader *reader = oscap_source_get_xmlTextReader(source);
	if (reader == NULL) {
		oscap_source_free(source);
                return NULL;
	}

	xmlTextReaderRead(reader);
	struct oval_variable_model *model = oval_variable_model_new();
	ret = _oval_variable_model_parse(model, reader, NULL);
	if (ret != 1) {
		oval_variable_model_free(model);
		model = NULL;
	}
	xmlFreeTextReader(reader);
	return model;

}

struct oval_variable_model * oval_variable_model_import(const char *file)
{
	struct oscap_source *source = oscap_source_new_from_file(file);
	struct oval_variable_model *model = oval_variable_model_import_source(source);
	oscap_source_free(source);
	return model;
}


static xmlNode *oval_variable_model_to_dom(struct oval_variable_model * variable_model,
				   xmlDocPtr doc, xmlNode * parent, void *user_arg)
{

	xmlNodePtr root_node;

	if (parent) {
		root_node = xmlNewTextChild(parent, NULL, BAD_CAST OVAL_ROOT_ELM_VARIABLES, NULL);
	} else {
		root_node = xmlNewNode(NULL, BAD_CAST OVAL_ROOT_ELM_VARIABLES);
		xmlDocSetRootElement(doc, root_node);
	}

	xmlNewNsProp(root_node, lookup_xsi_ns(doc), BAD_CAST "schemaLocation", BAD_CAST OVAL_VAR_SCHEMA_LOCATION);

	xmlNs *ns_common = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE, BAD_CAST "oval");
	xmlNs *ns_variables = xmlNewNs(root_node, OVAL_VARIABLES_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_variables);

	oval_generator_to_dom(variable_model->generator, doc, root_node);

	xmlNode *variables = xmlNewTextChild(root_node, ns_variables, BAD_CAST "variables", NULL);


	struct oval_string_iterator *varids = oval_variable_model_get_variable_ids(variable_model);
	while (oval_string_iterator_has_more(varids)) {
		char *varid = oval_string_iterator_next(varids);
		oval_datatype_t datatype = oval_variable_model_get_datatype(variable_model, varid);
		const char *comm = oval_variable_model_get_comment(variable_model, varid);

                xmlNode *variable = xmlNewTextChild(variables, ns_variables, BAD_CAST "variable", NULL);
                xmlNewProp(variable, BAD_CAST "id", BAD_CAST varid);
                xmlNewProp(variable, BAD_CAST "datatype", BAD_CAST oval_datatype_get_text(datatype));
                xmlNewProp(variable, BAD_CAST "comment", BAD_CAST comm);

		struct oval_value_iterator *value_itr = oval_variable_model_get_values(variable_model, varid);
		while (oval_value_iterator_has_more(value_itr)) {
			struct oval_value *value;
			char *text;

			value = oval_value_iterator_next(value_itr);
			text = oval_value_get_text(value);
			xmlNewTextChild(variable, ns_variables, BAD_CAST "value", BAD_CAST text);
		}
                oval_value_iterator_free(value_itr);
	}
        oval_string_iterator_free(varids);

	return root_node;
}

int oval_variable_model_export(struct oval_variable_model *model, const char *file)
{

	__attribute__nonnull__(model);

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	oval_variable_model_to_dom(model, doc, NULL, NULL);
	return oscap_xml_save_filename_free(file, doc);
}

bool oval_variable_model_has_variable(struct oval_variable_model *model, const char * id)
{
        __attribute__nonnull__(model);
        struct oval_string_iterator * str_it = (struct oval_string_iterator *)oval_string_map_keys(model->varmap);
        while (oval_string_iterator_has_more(str_it)) {
            if (!strcmp(oval_string_iterator_next(str_it), id)) {
                oval_string_iterator_free(str_it);
                return true;
            }
        }
        oval_string_iterator_free(str_it);
        return false;
}

struct oval_string_iterator *oval_variable_model_get_variable_ids(struct oval_variable_model *model)
{
	__attribute__nonnull__(model);
	return (struct oval_string_iterator *)oval_string_map_keys(model->varmap);
}
oval_datatype_t oval_variable_model_get_datatype(struct oval_variable_model *model, char *varid)
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

struct oval_value_iterator *oval_variable_model_get_values(struct oval_variable_model *model, char *varid)
{
	__attribute__nonnull__(model);
	_oval_variable_model_frame_t *frame = oval_string_map_get_value(model->varmap, varid);
	return (frame) ? (struct oval_value_iterator *) oval_collection_iterator(frame->values) : NULL;
}

struct oval_collection *oval_variable_model_get_values_ref(struct oval_variable_model *model, char *varid)
{
	_oval_variable_model_frame_t *frame;

	frame = oval_string_map_get_value(model->varmap, varid);
	return (frame) ? frame->values : NULL;
}
