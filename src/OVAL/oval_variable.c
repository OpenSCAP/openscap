/**
 * @file oval_variable.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "oval_definitions_impl.h"
#include "oval_collection_impl.h"
#include "oval_string_map_impl.h"
#include "oval_agent_api_impl.h"


typedef struct oval_variable {
	char *id;
	char *comment;
	int version;
	int deprecated;
	oval_variable_type_t type;
	oval_datatype_t datatype;
	void *extension;
} oval_variable_t;

typedef struct oval_variable_CONEXT {
	char *id;
	char *comment;
	int version;
	int deprecated;
	oval_variable_type_t type;
	oval_datatype_t datatype;
	struct oval_string_map *values;	//type==OVAL_VARIABLE_CONSTANT/EXTERNAL
} oval_variable_CONEXT_t;

typedef struct oval_variable_LOCAL {
	char *id;
	char *comment;
	int version;
	int deprecated;
	oval_variable_type_t type;
	oval_datatype_t datatype;
	struct oval_component *component;	//type==OVAL_VARIABLE_LOCAL
} oval_variable_LOCAL_t;

int oval_variable_iterator_has_more(struct oval_variable_iterator
				      *oc_variable)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_variable);
}

struct oval_variable *oval_variable_iterator_next(struct
						    oval_variable_iterator
						    *oc_variable)
{
	return (struct oval_variable *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_variable);
}

void oval_variable_iterator_free(struct
						    oval_variable_iterator
						    *oc_variable)
{
    oval_collection_iterator_free((struct oval_iterator *)oc_variable);
}

char *oval_variable_get_id(struct oval_variable *variable)
{
	return variable->id;
}

char *oval_variable_get_comment(struct oval_variable *variable)
{
	return variable->comment;
}

int oval_variable_get_version(struct oval_variable *variable)
{
	return variable->version;
}

int oval_variable_get_deprecated(struct oval_variable *variable)
{
	return variable->deprecated;
}

oval_variable_type_t oval_variable_get_type(struct oval_variable * variable)
{
	return variable->type;
}

oval_datatype_t oval_variable_get_datatype(struct oval_variable * variable)
{
	return variable->datatype;
}

struct oval_value_iterator *oval_variable_get_values(struct oval_variable *variable)
{
	//type==OVAL_VARIABLE_CONSTANT or OVAL_VARIABLE_EXTERNAL
	struct oval_value_iterator *values = NULL;
	oval_variable_type_t type = oval_variable_get_type(variable);
	if (type == OVAL_VARIABLE_EXTERNAL || type==OVAL_VARIABLE_CONSTANT) {
		oval_variable_CONEXT_t *constant =
		    (oval_variable_CONEXT_t *) variable;
		values =
		    (struct oval_value_iterator *)
		    oval_string_map_values(constant->values);
	}
	return values;
}

struct oval_component *oval_variable_get_component(struct oval_variable *variable)
{
	//type==OVAL_VARIABLE_LOCAL
	struct oval_component *component = NULL;
	if (oval_variable_get_type(variable) == OVAL_VARIABLE_LOCAL) {
		oval_variable_LOCAL_t *local =
		    (oval_variable_LOCAL_t *) variable;
		component = local->component;
	}
	return component;
}

static void _set_oval_variable_type
	(struct oval_variable *variable, oval_variable_type_t type)
{
	variable->type = type;
	switch(type)
	{
		case OVAL_VARIABLE_CONSTANT:
		case OVAL_VARIABLE_EXTERNAL:{
			oval_variable_CONEXT_t *conext
			= (oval_variable_CONEXT_t *)variable;
			conext->values = oval_string_map_new();
		}break;
		default: variable->extension = NULL;
	}
}

  struct oval_variable *oval_variable_new(char *id, oval_variable_type_t type)
{
	oval_variable_t *variable =
	    (oval_variable_t *) malloc(sizeof(oval_variable_t));
	variable->id = strdup(id);
	variable->comment = NULL;
	variable->datatype = OVAL_DATATYPE_UNKNOWN;
	_set_oval_variable_type(variable, type);
	return variable;
}

void _oval_variable_clone_CONEXT
	(struct oval_variable *new_variable, struct oval_variable *old_variable)
{
	struct oval_value_iterator *values = oval_variable_get_values(old_variable);
	while(oval_value_iterator_has_more(values)){
		struct oval_value *value = oval_value_iterator_next(values);
		oval_variable_add_value(new_variable, oval_value_clone(value));
	}
	oval_value_iterator_free(values);
}

void _oval_variable_clone_LOCAL
	(struct oval_variable *new_variable, struct oval_variable *old_variable,
	 struct oval_definition_model *model)
{
	struct oval_component *component = oval_variable_get_component(old_variable);
	oval_variable_set_component(new_variable, oval_component_clone(component, model));
}

struct oval_variable   *oval_variable_clone
	(struct oval_variable *old_variable, struct oval_definition_model *model)
{
	oval_variable_t *new_variable = oval_definition_model_get_variable(model, old_variable->id);
	if(new_variable==NULL){
		new_variable = oval_variable_new(old_variable->id, old_variable->type);

		oval_variable_set_comment   (new_variable, old_variable->comment);
		oval_variable_set_version   (new_variable, old_variable->version);
		oval_variable_set_deprecated(new_variable, old_variable->deprecated);
		oval_variable_set_datatype  (new_variable, old_variable->datatype);

		switch(new_variable->type)
		{
		case OVAL_VARIABLE_EXTERNAL:
		case OVAL_VARIABLE_CONSTANT: _oval_variable_clone_CONEXT(new_variable, old_variable);break;
		case OVAL_VARIABLE_LOCAL   : _oval_variable_clone_LOCAL (new_variable, old_variable, model);break;
		default: /*NOOP*/break;
		}

		oval_definition_model_add_variable(model, new_variable);
	}
	return new_variable;
}


void oval_variable_free(struct oval_variable *variable)
{
	if (variable){
		if (variable->id)free(variable->id);
		if (variable->comment)free(variable->comment);
		if (variable->extension != NULL) {
			switch (variable->type) {
			case OVAL_VARIABLE_LOCAL:{
					oval_variable_LOCAL_t *local
						= (oval_variable_LOCAL_t *)variable;
					if(local->component)
						oval_component_free(local->component);
					local->component = NULL;
				}
				break;
			case OVAL_VARIABLE_EXTERNAL:
			case OVAL_VARIABLE_CONSTANT:{
					oval_variable_CONEXT_t *conext
						= (oval_variable_CONEXT_t *)variable;
					oval_string_map_free
						(conext->values, (oscap_destruct_func)oval_value_free);
					conext->values = NULL;
				} break;
			case OVAL_VARIABLE_UNKNOWN: break;
			}
		}
		variable->comment = NULL;
		variable->id = NULL;

		free(variable);
	}
}

void oval_variable_set_datatype(struct oval_variable *variable,
				oval_datatype_t datatype)
{
	variable->datatype = datatype;
}

void oval_variable_set_comment(struct oval_variable *variable, char *comm)
{
	if(variable->comment!=NULL)free(variable->comment);
	variable->comment = comm==NULL?NULL:strdup(comm);
}

void oval_variable_set_deprecated(struct oval_variable *variable,
				  bool deprecated)
{
	variable->deprecated = deprecated;
}

void oval_variable_set_version(struct oval_variable *variable, int version)
{
	variable->version = version;
}

void oval_variable_add_value(struct oval_variable *variable,
			      struct oval_value *value)
{
	//type==OVAL_VARIABLE_CONSTANT
	if (variable->type == OVAL_VARIABLE_CONSTANT || variable->type == OVAL_VARIABLE_EXTERNAL) {
		oval_variable_CONEXT_t *conext =
		    (oval_variable_CONEXT_t *) variable;
		char *text = oval_value_get_text(value);
		oval_string_map_put(conext->values, text, (void *)value);
	}
}

void oval_variable_set_component(struct oval_variable *variable,
				 struct oval_component *component)
{
	//type==OVAL_VARIABLE_LOCAL
	if (variable->type == OVAL_VARIABLE_LOCAL) {
		oval_variable_LOCAL_t *local =
		    (oval_variable_LOCAL_t *) variable;
		local->component = component;
	}
}

static void _oval_variable_parse_local_tag_component_consumer(struct oval_component *component, void *variable) {
	oval_variable_set_component(variable, component);
}
static int _oval_variable_parse_local_tag(xmlTextReaderPtr reader,
					  struct oval_parser_context *context,
					  void *user)
{
	struct oval_variable *variable = (struct oval_variable *)user;
	xmlChar *tagname = xmlTextReaderName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	int return_code =
	    oval_component_parse_tag(reader, context, &_oval_variable_parse_local_tag_component_consumer,
				     variable);
	if (return_code != 1) {
		int line = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE: oval_variable_parse_local_tag::parse of %s terminated on error at <%s> line %d\n",
		     variable->id, tagname, line);
	}
	free(tagname);
	free(namespace);
	return return_code;
}

static int _oval_variable_parse_constant_tag(xmlTextReaderPtr reader,
					     struct oval_parser_context *context,
					     void *user)
{
	struct oval_variable *variable = (struct oval_variable *)user;
	xmlChar *tagname = xmlTextReaderName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	int return_code = 1;
	if (1) {
		int line = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE: _oval_variable_parse_constant_tag::parse of <%s> TODO at line %d\n",
		     tagname, line);
		return_code = oval_parser_skip_tag(reader, context);
	}
	if (return_code != 1) {
		int line = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE: oval_variable_parse_constant_tag::parse of %s terminated on error at <%s> line %d\n",
		     variable->id, tagname, line);
	}
	free(tagname);
	free(namespace);
	return return_code;
}

void oval_variable_to_print(struct oval_variable *variable, char *indent,
			    int idx);
int oval_variable_parse_tag(xmlTextReaderPtr reader,
			    struct oval_parser_context *context)
{
	struct oval_definition_model *model = oval_parser_context_model(context);
	char *tagname = (char*) xmlTextReaderName(reader);
	oval_variable_type_t type;
	if (strcmp(tagname, "constant_variable") == 0)
		type = OVAL_VARIABLE_CONSTANT;
	else if (strcmp(tagname, "external_variable") == 0)
		type = OVAL_VARIABLE_EXTERNAL;
	else if (strcmp(tagname, "local_variable") == 0)
		type = OVAL_VARIABLE_LOCAL;
	else {
		type = OVAL_VARIABLE_UNKNOWN;
		int line = xmlTextReaderGetParserLineNumber(reader);
		fprintf
		    (stderr, "NOTICE::oval_variable_parse_tag: <%s> unhandled variable type::line = %d\n",
		     tagname, line);
	}
	char *id = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	struct oval_variable *variable = get_oval_variable_new(model, id, type);
	if(variable->type==OVAL_VARIABLE_UNKNOWN){
		_set_oval_variable_type(variable, type);
	}
	free(id);id = variable->id;

	char *comm = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "comment");
	if(comm!=NULL){
		oval_variable_set_comment(variable, comm);
		free(comm);comm=NULL;
	}
	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	oval_variable_set_deprecated(variable, deprecated);
	char *version = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	oval_variable_set_version(variable, atoi(version));
	free(version);version = NULL;

	oval_datatype_t datatype =
	    oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_UNKNOWN);
	oval_variable_set_datatype(variable, datatype);
	int return_code = 1;
	switch (type) {
	case OVAL_VARIABLE_CONSTANT:{
			return_code =
			    oval_parser_parse_tag(reader, context,
						  _oval_variable_parse_constant_tag,
						  variable);
		}
		break;
	case OVAL_VARIABLE_LOCAL:{
			return_code =
			    oval_parser_parse_tag(reader, context,
						  _oval_variable_parse_local_tag,
						  variable);
		}
		break;
	case OVAL_VARIABLE_EXTERNAL: {
		// There's no content to process.
		oval_parser_skip_tag(reader, context);
		break;
	}
	default:
		return_code = 1;
	}
	free(tagname);
	return return_code;
}

void oval_variable_to_print(struct oval_variable *variable, char *indent,
			    int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sVARIABLE.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sVARIABLE[%d].", indent, idx);

	printf("%sID         = %s\n", nxtindent, oval_variable_get_id(variable));
	printf("%sVERSION    = %d\n", nxtindent,
	       oval_variable_get_version(variable));
	printf("%sCOMMENT    = %s\n", nxtindent,
	       oval_variable_get_comment(variable));
	printf("%sDEPRECATED = %d\n", nxtindent,
	       oval_variable_get_deprecated(variable));
	printf("%sTYPE       = %d\n", nxtindent, oval_variable_get_type(variable));
	printf("%sDATATYPE   = %d\n", nxtindent,
	       oval_variable_get_datatype(variable));
	switch (oval_variable_get_type(variable)) {
	case OVAL_VARIABLE_CONSTANT:{
			struct oval_value_iterator *values =
			    oval_variable_get_values(variable);
			if (oval_value_iterator_has_more(values)) {
				int i;
				for (i = 0;
				     oval_value_iterator_has_more(values);
				     i++) {
					struct oval_value *value =
					    oval_value_iterator_next(values);
					oval_value_to_print(value, nxtindent,
							    i);
				}
			} else
				printf
				    ("%sVALUES     = <<NO CONSTANTS BOUND>>\n",
				     nxtindent);
		}
		break;
	case OVAL_VARIABLE_EXTERNAL:{
			printf("%sEXTERNAL   <<TODO>>\n", nxtindent);
		}
		break;
	case OVAL_VARIABLE_LOCAL:{
			struct oval_component *component =
			    oval_variable_get_component(variable);
			if (component == NULL)
				printf
				    ("%sCOMPONENT  = <<NO COMPONENT BOUND>>\n",
				     nxtindent);
			else
				oval_component_to_print(component, nxtindent,
							0);
		}
		break;
	case OVAL_VARIABLE_UNKNOWN: break;
	}
}
static xmlNode *_oval_VARIABLE_CONSTANT_to_dom
	(struct oval_variable *variable, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *variable_node = xmlNewChild(parent, ns_definitions, BAD_CAST "constant_variable", NULL);

	struct oval_value_iterator *values = oval_variable_get_values(variable);
	while(oval_value_iterator_has_more(values)){
		struct oval_value *value = oval_value_iterator_next(values);
		char * text = oval_value_get_text(value);
		xmlNewChild(variable_node, ns_definitions, BAD_CAST "value", text);
	}
	oval_value_iterator_free(values);

	return variable_node;
}
static xmlNode *_oval_VARIABLE_EXTERNAL_to_dom
	(struct oval_variable *variable, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *variable_node = xmlNewChild(parent, ns_definitions, BAD_CAST "external_variable", NULL);

	return variable_node;
}
static xmlNode *_oval_VARIABLE_LOCAL_to_dom
	(struct oval_variable *variable, xmlDoc *doc, xmlNode *parent)
{
	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *variable_node = xmlNewChild(parent, ns_definitions, BAD_CAST "local_variable", NULL);

	struct oval_component *component = oval_variable_get_component(variable);
	oval_component_to_dom(component, doc, variable_node);

	return variable_node;
}

xmlNode *oval_variable_to_dom (struct oval_variable *variable, xmlDoc *doc, xmlNode *parent)
{

	xmlNode *variable_node = NULL;
	switch(oval_variable_get_type(variable))
	{
	case OVAL_VARIABLE_CONSTANT:{
		variable_node = _oval_VARIABLE_CONSTANT_to_dom(variable, doc, parent);
	}break;
	case OVAL_VARIABLE_EXTERNAL:{
		variable_node = _oval_VARIABLE_EXTERNAL_to_dom(variable, doc, parent);
	}break;
	case OVAL_VARIABLE_LOCAL:{
		variable_node = _oval_VARIABLE_LOCAL_to_dom(variable, doc, parent);
	}break;
	default:break;
	};

	char *id = oval_variable_get_id(variable);
	xmlNewProp(variable_node, BAD_CAST "id", id);

	char version[10]; *version = '\0';
	snprintf(version, sizeof(version), "%d", oval_variable_get_version(variable));
	xmlNewProp(variable_node, BAD_CAST "version", BAD_CAST version);

	oval_datatype_t datatype = oval_variable_get_datatype(variable);
	xmlNewProp(variable_node, BAD_CAST "datatype", oval_datatype_get_text(datatype));

	char *comm = oval_variable_get_comment(variable);
	xmlNewProp(variable_node, BAD_CAST "comment", comm);

	bool deprecated = oval_variable_get_deprecated(variable);
	if(deprecated)
		xmlNewProp(variable_node, BAD_CAST "deprecated", BAD_CAST "true");


	return variable_node;
}
