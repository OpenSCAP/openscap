/**
 * @file oval_variable.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"

typedef struct oval_variable {
	struct oval_definition_model *model;
	char *id;
	char *comment;
	int version;
	int deprecated;
	oval_variable_type_t type;
	oval_datatype_t datatype;
	oval_syschar_collection_flag_t flag;
	struct oval_collection *values;
} oval_variable_t, oval_variable_CONEXT_t;

typedef struct oval_variable_LOCAL {
	struct oval_definition_model *model;
	char *id;
	char *comment;
	int version;
	int deprecated;
	oval_variable_type_t type;
	oval_datatype_t datatype;
	oval_syschar_collection_flag_t flag;
	struct oval_collection *values;
	struct oval_component *component;	//type==OVAL_VARIABLE_LOCAL
} oval_variable_LOCAL_t, oval_variable_UNKNOWN_t;

bool oval_variable_iterator_has_more(struct oval_variable_iterator
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
	__attribute__nonnull__(variable);

	return variable->id;
}

char *oval_variable_get_comment(struct oval_variable *variable)
{
	__attribute__nonnull__(variable);

	return variable->comment;
}

int oval_variable_get_version(struct oval_variable *variable)
{
	__attribute__nonnull__(variable);

	return variable->version;
}

bool oval_variable_get_deprecated(struct oval_variable * variable)
{
	__attribute__nonnull__(variable);

	return variable->deprecated;
}

oval_variable_type_t oval_variable_get_type(struct oval_variable * variable)
{
	__attribute__nonnull__(variable);

	return variable->type;
}

oval_datatype_t oval_variable_get_datatype(struct oval_variable * variable)
{
	__attribute__nonnull__(variable);

	return variable->datatype;
}

struct oval_value_iterator *oval_variable_get_values(struct oval_variable *variable)
{
	__attribute__nonnull__(variable);

	struct oval_collection *value_collection = variable->values;
	return (value_collection) ? (struct oval_value_iterator *)oval_collection_iterator(variable->values) : NULL;
}

oval_syschar_collection_flag_t oval_syschar_model_get_variable_collection_flag
    (struct oval_syschar_model *sysmod, struct oval_variable *variable) {
	__attribute__nonnull__(variable);

	if (variable->flag == SYSCHAR_FLAG_UNKNOWN) {
		oval_syschar_model_get_variable_values(sysmod, variable);
	}
	return variable->flag;
}

struct oval_value_iterator *oval_syschar_model_get_variable_values
    (struct oval_syschar_model *sysmod, struct oval_variable *variable) {
	__attribute__nonnull__(variable);

	if (variable->flag == SYSCHAR_FLAG_UNKNOWN) {
		variable->values = oval_collection_new();
		struct oval_component *component = oval_variable_get_component(variable);
		if (component) {
			variable->flag = oval_component_evaluate(sysmod, component, variable->values);
		} else
			oscap_dprintf("WARNING: NULL component bound to variable\n"
				      "    variable type = %s\n"
				      "    variable id   = %s\n"
				      "    codeloc = %s(%d)\n",
				      oval_variable_type_get_text(variable->type), oval_variable_get_id(variable),
				      __FILE__, __LINE__);
	}
	return (variable->values) ? (struct oval_value_iterator *)oval_collection_iterator(variable->values) : NULL;
}

struct oval_component *oval_variable_get_component(struct oval_variable *variable)
{
	__attribute__nonnull__(variable);

	/*type==OVAL_VARIABLE_LOCAL */
	struct oval_component *component = NULL;
	if (oval_variable_get_type(variable) == OVAL_VARIABLE_LOCAL) {
		oval_variable_LOCAL_t *local = (oval_variable_LOCAL_t *) variable;
		component = local->component;
	}
	return component;
}

/* failed   - NULL 
 * success  - oval_variable
 * */
struct oval_variable *oval_variable_new(struct oval_definition_model *model, const char *id, oval_variable_type_t type)
{
	oval_variable_t *variable;
	switch (type) {
	case OVAL_VARIABLE_CONSTANT:{
			variable = (oval_variable_t *) oscap_alloc(sizeof(oval_variable_CONEXT_t));
			if (variable == NULL)
				return NULL;

			oval_variable_CONEXT_t *conext = (oval_variable_CONEXT_t *) variable;
			conext->values = oval_collection_new();
			conext->flag = SYSCHAR_FLAG_NOT_COLLECTED;
		}
		break;
	case OVAL_VARIABLE_EXTERNAL:{
			variable = (oval_variable_t *) oscap_alloc(sizeof(oval_variable_CONEXT_t));
			if (variable == NULL)
				return NULL;

			oval_variable_CONEXT_t *conext = (oval_variable_CONEXT_t *) variable;
			conext->values = oval_collection_new();
			conext->flag = SYSCHAR_FLAG_NOT_COLLECTED;
		}
		break;
	case OVAL_VARIABLE_LOCAL:{
			variable = (oval_variable_t *) oscap_alloc(sizeof(oval_variable_LOCAL_t));
			if (variable == NULL)
				return NULL;

			oval_variable_LOCAL_t *local = (oval_variable_LOCAL_t *) variable;
			local->component = NULL;
			local->values = NULL;
			local->flag = SYSCHAR_FLAG_UNKNOWN;
		}
		break;
	case OVAL_VARIABLE_UNKNOWN:{
			variable = (oval_variable_t *) oscap_alloc(sizeof(oval_variable_UNKNOWN_t));
			if (variable == NULL)
				return NULL;

			oval_variable_UNKNOWN_t *unknwn = (oval_variable_UNKNOWN_t *) variable;
			unknwn->component = NULL;
			unknwn->values = NULL;
			unknwn->flag = SYSCHAR_FLAG_UNKNOWN;
		};
		break;
	default:
		oscap_dprintf(" oval_variable type not valid: type = %d (%s:%d)", type, __FILE__, __LINE__);
		oscap_seterr(OSCAP_EFAMILY_OVAL, OVAL_EOVALINT, "Invalid OVAL variable type");
		return NULL;
	}

	variable->model = model;
	variable->id = oscap_strdup(id);
	variable->comment = NULL;
	variable->datatype = OVAL_DATATYPE_UNKNOWN;
	variable->type = type;
	return variable;
}

bool oval_variable_is_valid(struct oval_variable * variable)
{
        oval_variable_type_t type;

        if (variable == NULL) {
                oscap_dprintf("WARNING: argument is not valid: NULL.\n");
                return false;
        }

        if (oval_variable_get_datatype(variable) == OVAL_DATATYPE_UNKNOWN) {
                oscap_dprintf("WARNING: argument is not valid: datatype == OVAL_DATATYPE_UNKNOWN.\n");
                return false;
        }

        type = oval_variable_get_type(variable);
        switch (type) {
        case OVAL_VARIABLE_EXTERNAL:
                break;
        case OVAL_VARIABLE_CONSTANT:
                break;
        case OVAL_VARIABLE_LOCAL:
                {
                        oval_variable_LOCAL_t *var = (oval_variable_LOCAL_t *) variable;

                        if (var->component == NULL) {
                                oscap_dprintf("WARNING: argument is not valid: component == NULL.\n");
                                return false;
                        }
                }
                break;
        default:
                oscap_dprintf("WARNING: argument is not valid: wrong variable type: %d.\n", type);
                return false;
        }

	return true;
}

bool oval_variable_is_locked(struct oval_variable * variable)
{
	__attribute__nonnull__(variable);

	return oval_definition_model_is_locked(variable->model);
}

struct oval_variable *oval_variable_clone(struct oval_definition_model *new_model, struct oval_variable *old_variable) {
	__attribute__nonnull__(old_variable);

	oval_variable_t *new_variable = oval_definition_model_get_variable(new_model, old_variable->id);
	if (new_variable == NULL) {
		new_variable = oval_variable_new(new_model, old_variable->id, old_variable->type);

		oval_variable_set_comment(new_variable, old_variable->comment);
		oval_variable_set_version(new_variable, old_variable->version);
		oval_variable_set_deprecated(new_variable, old_variable->deprecated);
		oval_variable_set_datatype(new_variable, old_variable->datatype);
		new_variable->flag = old_variable->flag;

		if (old_variable->values) {
			struct oval_value_iterator *old_values =
			    (struct oval_value_iterator *)oval_collection_iterator(old_variable->values);
			if (new_variable->values == NULL)
				new_variable->values = oval_collection_new();
			while (oval_value_iterator_has_more(old_values)) {
				struct oval_value *value = oval_value_iterator_next(old_values);
				/* char *text = oval_value_get_text(value); <-- unused */
				oval_collection_add(new_variable->values, value);
			}
			oval_value_iterator_free(old_values);
		}

		if (new_variable->type == OVAL_VARIABLE_LOCAL) {
			struct oval_component *component = oval_variable_get_component(old_variable);
			oval_variable_set_component(new_variable, oval_component_clone(new_model, component));
		}
		oval_definition_model_add_variable(new_model, new_variable);
	}
	return new_variable;
}

void oval_variable_free(struct oval_variable *variable)
{
	if (variable) {
		if (variable->id)
			oscap_free(variable->id);
		if (variable->comment)
			oscap_free(variable->comment);
		oval_variable_CONEXT_t *conext = (oval_variable_CONEXT_t *) variable;
		if (conext->values) {
			oval_collection_free_items(conext->values, (oscap_destruct_func) oval_value_free);
			conext->values = NULL;
		}
		if (variable->type == OVAL_VARIABLE_LOCAL) {
			oval_variable_LOCAL_t *local = (oval_variable_LOCAL_t *) variable;
			if (local->component)
				oval_component_free(local->component);
			local->component = NULL;
		}
		variable->comment = NULL;
		variable->id = NULL;

		oscap_free(variable);
	}
}

void oval_variable_set_datatype(struct oval_variable *variable, oval_datatype_t datatype)
{
	if (variable && !oval_variable_is_locked(variable)) {
		variable->datatype = datatype;
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_variable_set_type(struct oval_variable *variable, oval_variable_type_t type)
{
	if (variable && !oval_variable_is_locked(variable)) {
		if (variable->type == OVAL_VARIABLE_UNKNOWN) {
			variable->type = type;
			switch (type) {
			case OVAL_VARIABLE_CONSTANT:
			case OVAL_VARIABLE_EXTERNAL:
				{
					oval_variable_CONEXT_t *conext = (oval_variable_CONEXT_t *) variable;
					conext->values = oval_collection_new();
					conext->flag = SYSCHAR_FLAG_NOT_COLLECTED;
				}
				break;
			case OVAL_VARIABLE_LOCAL:
			case OVAL_VARIABLE_UNKNOWN:
				variable->flag = SYSCHAR_FLAG_UNKNOWN;
				break;
			}
		} else if (variable->type != type) {
			/* TODO: Should we set and propagate error here ? */
			oscap_dprintf("ERROR: attempt to reset valid variable type    oldtype = %s    newtype = %s",
				      oval_variable_type_get_text(variable->type), oval_variable_type_get_text(type));
		}
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_variable_set_comment(struct oval_variable *variable, char *comm)
{
	if (variable && !oval_variable_is_locked(variable)) {
		if (variable->comment != NULL)
			oscap_free(variable->comment);
		variable->comment = oscap_strdup(comm);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);

}

void oval_variable_set_deprecated(struct oval_variable *variable, bool deprecated)
{
	if (variable && !oval_variable_is_locked(variable)) {
		variable->deprecated = deprecated;
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_variable_set_version(struct oval_variable *variable, int version)
{
	if (variable && !oval_variable_is_locked(variable)) {
		variable->version = version;
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_variable_add_value(struct oval_variable *variable, struct oval_value *value)
{
	if (variable && !oval_variable_is_locked(variable)) {
		if (variable->type == OVAL_VARIABLE_CONSTANT || variable->type == OVAL_VARIABLE_EXTERNAL) {
			/* char *text = oval_value_get_text(value); <-- unused */
			oval_collection_add(variable->values, value);
			variable->flag = SYSCHAR_FLAG_COMPLETE;
		}
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_variable_set_component(struct oval_variable *variable, struct oval_component *component)
{
	if (variable && !oval_variable_is_locked(variable)) {
		if (variable->type == OVAL_VARIABLE_LOCAL) {
			oval_variable_LOCAL_t *local = (oval_variable_LOCAL_t *) variable;
			local->component = component;
		}
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

static void _oval_variable_parse_local_tag_component_consumer(struct oval_component *component, void *variable)
{
	oval_variable_set_component(variable, component);
}

static int _oval_variable_parse_local_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_variable *variable = (struct oval_variable *)user;
	xmlChar *tagname = xmlTextReaderLocalName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	int return_code = oval_component_parse_tag(reader, context, &_oval_variable_parse_local_tag_component_consumer,
						   variable);
	if (return_code != 1) {
		oscap_dprintf
		    ("NOTICE: oval_variable_parse_local_tag::parse of %s terminated on error at <%s> line %d",
		     variable->id, tagname, xmlTextReaderGetParserLineNumber(reader));
	}
	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}

#define DEFINITION_NAMESPACE "http://oval.mitre.org/XMLSchema/oval-definitions-5"

static void _oval_variable_parse_value(char *text_value, struct oval_variable *variable)
{

	struct oval_value *value = oval_value_new(oval_variable_get_datatype(variable), text_value);
	oval_variable_add_value(variable, value);
}

static int _oval_variable_parse_constant_tag(xmlTextReaderPtr reader,
					     struct oval_parser_context *context, struct oval_variable *variable)
{
	xmlChar *tagname = xmlTextReaderLocalName(reader);
	xmlChar *namespace = xmlTextReaderNamespaceUri(reader);
	int return_code = 1;
	if (strcmp("value", (char *)tagname) == 0 && strcmp(DEFINITION_NAMESPACE, (char *)namespace) == 0) {
		oval_parser_text_value(reader, context, (oval_xml_value_consumer) _oval_variable_parse_value, variable);
	} else {
		oscap_dprintf("NOTICE: Invalid element <%s:%s> in constant variable"
			      "    <constant_variable id = %s> at line %d"
			      "    %s(%d)", namespace, tagname, variable->id, xmlTextReaderGetParserLineNumber(reader), __FILE__, __LINE__);
	}
	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}

int oval_variable_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context)
{
	struct oval_definition_model *model = oval_parser_context_model(context);
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	oval_variable_type_t type;
	if (strcmp(tagname, "constant_variable") == 0)
		type = OVAL_VARIABLE_CONSTANT;
	else if (strcmp(tagname, "external_variable") == 0)
		type = OVAL_VARIABLE_EXTERNAL;
	else if (strcmp(tagname, "local_variable") == 0)
		type = OVAL_VARIABLE_LOCAL;
	else {
		type = OVAL_VARIABLE_UNKNOWN;
		oscap_dprintf("NOTICE::oval_variable_parse_tag: <%s> unhandled variable type::line = %d",
			      tagname, xmlTextReaderGetParserLineNumber(reader));
	}
	char *id = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	struct oval_variable *variable = oval_variable_get_new(model, id, type);
	oscap_free(id);
	id = variable->id;

	char *comm = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "comment");
	if (comm != NULL) {
		oval_variable_set_comment(variable, comm);
		oscap_free(comm);
		comm = NULL;

	}
	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	oval_variable_set_deprecated(variable, deprecated);
	char *version = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "version");
	oval_variable_set_version(variable, atoi(version));
	oscap_free(version);
	version = NULL;

	oval_datatype_t datatype = oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_UNKNOWN);
	oval_variable_set_datatype(variable, datatype);
	int return_code = 1;
	switch (type) {
	case OVAL_VARIABLE_CONSTANT:{
			return_code =
			    oval_parser_parse_tag(reader, context,
						  (oval_xml_tag_parser) _oval_variable_parse_constant_tag, variable);
		}
		break;
	case OVAL_VARIABLE_LOCAL:{
			return_code = oval_parser_parse_tag(reader, context, _oval_variable_parse_local_tag, variable);
		}
		break;
	case OVAL_VARIABLE_EXTERNAL:{
			// There's no content to process.
			oval_parser_skip_tag(reader, context);
			break;
		}
	default:
		return_code = 1;
	}
	oscap_free(tagname);
	return return_code;
}

void oval_variable_to_print(struct oval_variable *variable, char *indent, int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sVARIABLE.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sVARIABLE[%d].", indent, idx);

	oscap_dprintf("%sID         = %s\n", nxtindent, oval_variable_get_id(variable));
	oscap_dprintf("%sVERSION    = %d\n", nxtindent, oval_variable_get_version(variable));
	oscap_dprintf("%sCOMMENT    = %s\n", nxtindent, oval_variable_get_comment(variable));
	oscap_dprintf("%sDEPRECATED = %d\n", nxtindent, oval_variable_get_deprecated(variable));
	oscap_dprintf("%sTYPE       = %d\n", nxtindent, oval_variable_get_type(variable));
	oscap_dprintf("%sDATATYPE   = %d\n", nxtindent, oval_variable_get_datatype(variable));
	switch (oval_variable_get_type(variable)) {
	case OVAL_VARIABLE_CONSTANT:{
			struct oval_value_iterator *values = oval_variable_get_values(variable);
			if (oval_value_iterator_has_more(values)) {
				int i;
				for (i = 0; oval_value_iterator_has_more(values); i++) {
					struct oval_value *value = oval_value_iterator_next(values);
					oval_value_to_print(value, nxtindent, i);
				}
			} else
				oscap_dprintf("%sVALUES     = <<NO CONSTANTS BOUND>>\n", nxtindent);
		}
		break;
	case OVAL_VARIABLE_EXTERNAL:{
			oscap_dprintf("%sEXTERNAL   <<TODO>>\n", nxtindent);
		}
		break;
	case OVAL_VARIABLE_LOCAL:{
			struct oval_component *component = oval_variable_get_component(variable);
			if (component == NULL)
				oscap_dprintf("%sCOMPONENT  = <<NO COMPONENT BOUND>>\n", nxtindent);
			else
				oval_component_to_print(component, nxtindent, 0);
		}
		break;
	case OVAL_VARIABLE_UNKNOWN:
		break;
	}
}

static xmlNode *_oval_VARIABLE_CONSTANT_to_dom(struct oval_variable *variable, xmlDoc * doc, xmlNode * parent) {
	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *variable_node = xmlNewChild(parent, ns_definitions, BAD_CAST "constant_variable", NULL);

	struct oval_value_iterator *values = oval_variable_get_values(variable);
	while (oval_value_iterator_has_more(values)) {
		struct oval_value *value = oval_value_iterator_next(values);
		char *text = oval_value_get_text(value);
		xmlNewChild(variable_node, ns_definitions, BAD_CAST "value", BAD_CAST text);
	}
	oval_value_iterator_free(values);

	return variable_node;
}

static xmlNode *_oval_VARIABLE_EXTERNAL_to_dom(struct oval_variable *variable, xmlDoc * doc, xmlNode * parent) {
	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *variable_node = xmlNewChild(parent, ns_definitions, BAD_CAST "external_variable", NULL);

	return variable_node;
}

static xmlNode *_oval_VARIABLE_LOCAL_to_dom(struct oval_variable *variable, xmlDoc * doc, xmlNode * parent) {
	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *variable_node = xmlNewChild(parent, ns_definitions, BAD_CAST "local_variable", NULL);

	struct oval_component *component = oval_variable_get_component(variable);
	oval_component_to_dom(component, doc, variable_node);

	return variable_node;
}

xmlNode *oval_variable_to_dom(struct oval_variable * variable, xmlDoc * doc, xmlNode * parent)
{
	xmlNode *variable_node = NULL;
	switch (oval_variable_get_type(variable)) {
	case OVAL_VARIABLE_CONSTANT:{
			variable_node = _oval_VARIABLE_CONSTANT_to_dom(variable, doc, parent);
		}
		break;
	case OVAL_VARIABLE_EXTERNAL:{
			variable_node = _oval_VARIABLE_EXTERNAL_to_dom(variable, doc, parent);
		}
		break;
	case OVAL_VARIABLE_LOCAL:{
			variable_node = _oval_VARIABLE_LOCAL_to_dom(variable, doc, parent);
		}
		break;
	default:
		break;
	};

	char *id = oval_variable_get_id(variable);
	xmlNewProp(variable_node, BAD_CAST "id", BAD_CAST id);

	char version[10];
	*version = '\0';
	snprintf(version, sizeof(version), "%d", oval_variable_get_version(variable));
	xmlNewProp(variable_node, BAD_CAST "version", BAD_CAST version);

	oval_datatype_t datatype = oval_variable_get_datatype(variable);
	xmlNewProp(variable_node, BAD_CAST "datatype", BAD_CAST oval_datatype_get_text(datatype));

	char *comm = oval_variable_get_comment(variable);
	xmlNewProp(variable_node, BAD_CAST "comment", BAD_CAST comm);

	bool deprecated = oval_variable_get_deprecated(variable);
	if (deprecated)
		xmlNewProp(variable_node, BAD_CAST "deprecated", BAD_CAST "true");

	return variable_node;
}
