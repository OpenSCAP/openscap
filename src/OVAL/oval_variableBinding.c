/**
 * @file oval_variableBinding.c
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

static int DEBUG_OVAL_VARIABLE_BINDING = 0;


typedef struct oval_variable_binding {
	struct oval_variable *variable;
	char *value;
} oval_variable_binding_t;

int oval_variable_binding_iterator_has_more(struct
					    oval_variable_binding_iterator
					    *oc_variable_binding)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_variable_binding);
}

struct oval_variable_binding *oval_variable_binding_iterator_next(struct
								  oval_variable_binding_iterator
								  *oc_variable_binding)
{
	return (struct oval_variable_binding *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_variable_binding);
}
void oval_variable_binding_iterator_free(struct
								  oval_variable_binding_iterator
								  *oc_variable_binding)
{
    oval_collection_iterator_free((struct oval_iterator *)
					  oc_variable_binding);
}

struct oval_variable *oval_variable_binding_get_variable(struct
						     oval_variable_binding
						     *binding)
{
	return ((struct oval_variable_binding *)binding)->variable;
}

char *oval_variable_binding_get_value(struct oval_variable_binding *binding)
{
	return ((struct oval_variable_binding *)binding)->value;
}

void oval_variable_binding_set_variable(struct oval_variable_binding *binding, struct oval_variable *variable)
{
	binding->variable = variable;
}
void oval_variable_binding_set_value   (struct oval_variable_binding *binding, char *value)
{
	if(binding->value!=NULL)free(binding->value);
	binding->value = ((value==NULL)?NULL:strdup(value));
}

struct oval_variable_binding *oval_variable_binding_new(struct oval_variable *variable, char *value)
{
	oval_variable_binding_t *binding = (oval_variable_binding_t*)malloc(sizeof(oval_variable_binding_t));
	binding->variable = variable;
	binding->value    = value;
	return binding;
}

static struct oval_variable_binding *_oval_variable_binding_new()
{
	oval_variable_binding_t *binding = (oval_variable_binding_t*)malloc(sizeof(oval_variable_binding_t));
	binding->variable = NULL;
	binding->value    = NULL;
	return binding;
}

void oval_variable_binding_free(struct oval_variable_binding *binding)
{
	if (binding) {
		if(binding->value!=NULL)free(binding->value);

		binding->value = NULL;
		binding->variable = NULL;

		free(binding);
	}
}

static void _oval_variable_binding_value_consumer(char * value, void * user)
{
	oval_variable_binding_set_value((struct oval_variable_binding *)user, value);
}

int oval_variable_binding_parse_tag(xmlTextReaderPtr reader,
			       struct oval_parser_context *context, oval_variable_binding_consumer consumer, void* client)
{
	int return_code = 1;
	struct oval_variable_binding *binding = _oval_variable_binding_new();
	{//variable
		char* variableId = xmlTextReaderGetAttribute(reader, BAD_CAST "variable_id");
		struct oval_variable *variable = get_oval_variable_new(context->definition_model, variableId, OVAL_VARIABLE_UNKNOWN);
		oval_variable_binding_set_variable(binding, variable);
		free(variableId);variableId=NULL;
	}
	{//bound value
		return_code = oval_parser_text_value(reader, context, &_oval_variable_binding_value_consumer, binding);
	}
	if(return_code!=1){
		char warning[200]; *warning = 0;
		sprintf(warning, "oval_warning_parse_tag:: return code is not 1::(%d)",return_code);
		oval_parser_log_warn(context, warning);
	}else{
		if(DEBUG_OVAL_VARIABLE_BINDING){
			int numchars = 0;
			char debug[2000];debug[numchars]='\0';
			numchars = numchars + sprintf(debug+numchars,"oval_variable_binding_parse_tag::");
			numchars = numchars + sprintf(debug+numchars,"\n    binding->variable = %s",oval_variable_get_id(oval_variable_binding_get_variable(binding)));
			numchars = numchars + sprintf(debug+numchars,"\n    binding->value    = %s",oval_variable_binding_get_value(binding));
			oval_parser_log_debug(context, debug);
		}
		(*consumer)(binding, client);
	}
	return return_code;
}

void oval_variable_binding_to_dom  (struct oval_variable_binding *binding, xmlDoc *doc, xmlNode *tag_parent){
	if(binding){
		xmlNs *ns_syschar = xmlSearchNsByHref(doc, tag_parent, OVAL_SYSCHAR_NAMESPACE);
	    xmlNode *tag_variable_binding = xmlNewChild
			(tag_parent, ns_syschar, BAD_CAST "variable_value",
					BAD_CAST oval_variable_binding_get_value(binding));

	    {//attributes
	    	struct oval_variable *variable = oval_variable_binding_get_variable(binding);
	    	xmlNewProp(tag_variable_binding, BAD_CAST "variable_id", BAD_CAST oval_variable_get_id(variable));
	    }
	}
}


