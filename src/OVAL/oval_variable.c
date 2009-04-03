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
#include "oval_agent_api_impl.h"

typedef struct oval_variable {
	char *id;
	char *comment;
	int version;
	int deprecated;
	oval_variable_type_enum type;
	oval_datatype_enum datatype;
	void *extension;
} oval_variable_t;

typedef struct oval_variable_CONSTANT {
	struct oval_collection *values;	//type==OVAL_VARIABLE_CONSTANT
} oval_variable_CONSTANT_t;

typedef struct oval_variable_LOCAL {
	struct oval_component *component;	//type==OVAL_VARIABLE_LOCAL
} oval_variable_LOCAL_t;

int oval_collection_variable_has_more(struct oval_iterator_variable
				      *oc_variable)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_variable);
}

struct oval_variable *oval_collection_variable_next(struct
						    oval_iterator_variable
						    *oc_variable)
{
	return (struct oval_variable *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_variable);
}

char *oval_variable_id(struct oval_variable *variable)
{
	return variable->id;
}

char *oval_variable_comment(struct oval_variable *variable)
{
	return variable->comment;
}

int oval_variable_version(struct oval_variable *variable)
{
	return variable->version;
}

int oval_variable_deprecated(struct oval_variable *variable)
{
	return variable->deprecated;
}

oval_variable_type_enum oval_variable_type(struct oval_variable * variable)
{
	return variable->type;
}

oval_datatype_enum oval_variable_datatype(struct oval_variable * variable)
{
	return variable->datatype;
}

struct oval_iterator_value *oval_variable_values(struct oval_variable *variable)
{
	//type==OVAL_VARIABLE_CONSTANT
	struct oval_iterator_value *values = NULL;
	if (oval_variable_type(variable) == OVAL_VARIABLE_CONSTANT) {
		oval_variable_CONSTANT_t *constant =
		    (oval_variable_CONSTANT_t *) variable->extension;
		values =
		    (struct oval_iterator_value *)
		    oval_collection_iterator(constant->values);
	}
	return values;
}

struct oval_component *oval_variable_component(struct oval_variable *variable)
{
	//type==OVAL_VARIABLE_LOCAL
	struct oval_component *component = NULL;
	if (oval_variable_type(variable) == OVAL_VARIABLE_LOCAL) {
		oval_variable_LOCAL_t *local =
		    (oval_variable_LOCAL_t *) variable->extension;
		component = local->component;
	}
	return component;
}

struct oval_variable *oval_variable_new()
{
	oval_variable_t *variable =
	    (oval_variable_t *) malloc(sizeof(oval_variable_t));
	variable->id = NULL;
	variable->type = OVAL_VARIABLE_UNKNOWN;
	variable->datatype = OVAL_DATATYPE_UNKNOWN;
	variable->extension = NULL;
	return variable;
};

void oval_variable_free(struct oval_variable *variable)
{
	if (variable->id != NULL)
		free(variable->id);
	if (variable->extension != NULL) {
		switch (variable->type) {
		case OVAL_VARIABLE_LOCAL:{
				oval_component_free(variable->extension);
			}
			break;
		case OVAL_VARIABLE_CONSTANT:{
				void free_value(void *value) {
					oval_value_free((struct oval_value *)
							value);
				}
				oval_collection_free_items((struct
							    oval_collection *)
							   variable->extension,
							   &free_value);
			} break;
		}
	}
	free(variable);
}

void set_oval_variable_type(struct oval_variable *variable,
			    oval_variable_type_enum type)
{
	variable->type = type;
	switch (type) {
	case OVAL_VARIABLE_CONSTANT:{
			variable->extension =
			    malloc(sizeof(oval_variable_CONSTANT_t));
			((oval_variable_CONSTANT_t *) variable->extension)->
			    values = oval_collection_new();
		}
		break;
	case OVAL_VARIABLE_LOCAL:{
			variable->extension =
			    malloc(sizeof(oval_variable_LOCAL_t));
			((oval_variable_LOCAL_t *) variable->extension)->
			    component = NULL;
		}
		break;
	}
}

void set_oval_variable_id(struct oval_variable *variable, char *id)
{
	variable->id = id;
}

void set_oval_variable_datatype(struct oval_variable *variable,
				oval_datatype_enum datatype)
{
	variable->datatype = datatype;
}

void set_oval_variable_comment(struct oval_variable *variable, char *comment)
{
	variable->comment = comment;
}

void set_oval_variable_deprecated(struct oval_variable *variable,
				  int deprecated)
{
	variable->deprecated = deprecated;
}

void set_oval_variable_version(struct oval_variable *variable, int version)
{
	variable->version = version;
}

void add_oval_variable_values(struct oval_variable *variable,
			      struct oval_value *value)
{
	//type==OVAL_VARIABLE_CONSTANT
	if (variable->type == OVAL_VARIABLE_CONSTANT) {
		oval_variable_CONSTANT_t *extension =
		    (oval_variable_CONSTANT_t *) variable->extension;
		oval_collection_add(extension->values, (void *)value);
	}
}

void set_oval_variable_component(struct oval_variable *variable,
				 struct oval_component *component)
{
	//type==OVAL_VARIABLE_LOCAL
	if (variable->type == OVAL_VARIABLE_LOCAL) {
		oval_variable_LOCAL_t *extension =
		    (oval_variable_LOCAL_t *) variable->extension;
		extension->component = component;
	}
}

int _oval_variable_parse_local_tag(xmlTextReaderPtr reader,
				   struct oval_parser_context *context,
				   void *user)
{
	struct oval_variable *variable = (struct oval_variable *)user;
	xmlChar *tagname = xmlTextReaderName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	void component_consumer(struct oval_component *component, void *null) {
		set_oval_variable_component(variable, component);
	}
	int return_code =
	    oval_component_parse_tag(reader, context, &component_consumer,
				     NULL);
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

int _oval_variable_parse_constant_tag(xmlTextReaderPtr reader,
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
			    int index);
int oval_variable_parse_tag(xmlTextReaderPtr reader,
			    struct oval_parser_context *context)
{
	char *id = xmlTextReaderGetAttribute(reader, "id");
	struct oval_object_model *model = oval_parser_context_model(context);
	struct oval_variable *variable = get_oval_variable_new(model, id);
	id = variable->id;
	char *tagname = xmlTextReaderName(reader);
	oval_variable_type_enum type;
	if (strcmp(tagname, "constant_variable") == 0)
		type = OVAL_VARIABLE_CONSTANT;
	else if (strcmp(tagname, "external_variable") == 0)
		type = OVAL_VARIABLE_EXTERNAL;
	else if (strcmp(tagname, "local_variable") == 0)
		type = OVAL_VARIABLE_LOCAL;
	else {
		type = OVAL_VARIABLE_UNKNOWN;
		int line = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE::oval_variable_parse_tag: <%s> unhandled variable type::line = %d\n",
		     tagname, line);
	}
	set_oval_variable_type(variable, type);
	//printf("DEBUG::oval_variable_parse_tag::id = %s <%s>\n", id, tagname);

	char *comment = xmlTextReaderGetAttribute(reader, "comment");
	set_oval_variable_comment(variable, comment);
	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	set_oval_variable_deprecated(variable, deprecated);
	char *version = xmlTextReaderGetAttribute(reader, "version");
	set_oval_variable_version(variable, atoi(version));

	oval_datatype_enum datatype =
	    oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_UNKNOWN);
	set_oval_variable_datatype(variable, datatype);
	free(version);
	int return_code;
	switch (type) {
	case OVAL_VARIABLE_CONSTANT:{
			return_code =
			    oval_parser_parse_tag(reader, context,
						  &_oval_variable_parse_constant_tag,
						  variable);
		}
		break;
	case OVAL_VARIABLE_LOCAL:{
			return_code =
			    oval_parser_parse_tag(reader, context,
						  &_oval_variable_parse_local_tag,
						  variable);
		}
		break;
	default:
		return_code = 1;
	}
	free(tagname);
	return return_code;
}

void oval_variable_to_print(struct oval_variable *variable, char *indent,
			    int index)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (index == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sVARIABLE.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sVARIABLE[%d].", indent, index);

	printf("%sID         = %s\n", nxtindent, oval_variable_id(variable));
	printf("%sVERSION    = %d\n", nxtindent,
	       oval_variable_version(variable));
	printf("%sCOMMENT    = %s\n", nxtindent,
	       oval_variable_comment(variable));
	printf("%sDEPRECATED = %d\n", nxtindent,
	       oval_variable_deprecated(variable));
	printf("%sTYPE       = %d\n", nxtindent, oval_variable_type(variable));
	printf("%sDATATYPE   = %d\n", nxtindent,
	       oval_variable_datatype(variable));
	switch (oval_variable_type(variable)) {
	case OVAL_VARIABLE_CONSTANT:{
			struct oval_iterator_value *values =
			    oval_variable_values(variable);
			if (oval_iterator_value_has_more(values)) {
				int i;
				for (i = 0;
				     oval_iterator_value_has_more(values);
				     i++) {
					struct oval_value *value =
					    oval_iterator_value_next(values);
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
			    oval_variable_component(variable);
			if (component == NULL)
				printf
				    ("%sCOMPONENT  = <<NO COMPONENT BOUND>>\n",
				     nxtindent);
			else
				oval_component_to_print(component, nxtindent,
							0);
		}
		break;
	}
}
