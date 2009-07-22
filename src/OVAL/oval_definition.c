/**
 * @file oval_definition.c
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

typedef struct oval_definition {
	char *id;
	int version;
	oval_definition_class_enum class;
	int deprecated;
	char *title;
	char *description;
	struct oval_collection *affected;
	struct oval_collection *reference;
	struct oval_criteria_node *criteria;
} oval_definition_t;

char *oval_definition_id(struct oval_definition *definition)
{
	return ((struct oval_definition *)definition)->id;
}

int oval_definition_version(struct oval_definition *definition)
{
	return ((struct oval_definition *)definition)->version;
}

oval_definition_class_enum oval_definition_class(struct oval_definition
						 *definition)
{
	return ((struct oval_definition *)definition)->class;
}

int oval_definition_deprecated(struct oval_definition *definition)
{
	return ((struct oval_definition *)definition)->deprecated;
}

char *oval_definition_title(struct oval_definition *definition)
{
	return ((struct oval_definition *)definition)->title;
}

char *oval_definition_description(struct oval_definition *definition)
{
	return ((struct oval_definition *)definition)->description;
}

struct oval_iterator_affected *oval_definition_affected(struct oval_definition
							*definition)
{
	return (struct oval_iterator_affected *)
	    oval_collection_iterator(definition->affected);
}

struct oval_iterator_reference *oval_definition_reference(struct oval_definition
							  *definition)
{
	return (struct oval_iterator_reference *)
	    oval_collection_iterator(definition->reference);
}

struct oval_criteria_node *oval_definition_criteria(struct oval_definition
						    *definition)
{
	return ((struct oval_definition *)definition)->criteria;
}

struct oval_definition *oval_definition_new()
{
	struct oval_definition *definition =
	    (struct oval_definition *)malloc(sizeof(oval_definition_t));
	definition->id = NULL;
	definition->version = 0;
	definition->class = CLASS_UNKNOWN;
	definition->deprecated = 0;
	definition->title = NULL;
	definition->description = NULL;
	definition->affected = oval_collection_new();
	definition->reference = oval_collection_new();
	definition->criteria = NULL;
	return definition;
}

void oval_definition_free(struct oval_definition *definition)
{
	if (definition->id != NULL)
		free(definition->id);
	if (definition->title != NULL)
		free(definition->title);
	if (definition->description != NULL)
		free(definition->description);
	if (definition->criteria != NULL)
		oval_criteria_node_free(definition->criteria);
	void free_affected(void *affected) {
		oval_affected_free((struct oval_affected *)affected);
	}
	oval_collection_free_items(definition->affected, &free_affected);
	void free_reference(void *reference) {
		oval_reference_free((struct oval_reference *)reference);
	}
	oval_collection_free_items(definition->reference, &free_reference);
	free(definition);
}

int oval_iterator_definition_has_more(struct oval_iterator_definition
				      *oc_definition)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_definition);
}

struct oval_definition *oval_iterator_definition_next(struct
						      oval_iterator_definition
						      *oc_definition)
{
	return (struct oval_definition *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_definition);
}

void set_oval_definition_id(struct oval_definition *definition, char *id)
{
	definition->id = id;
};

void set_oval_definition_version(struct oval_definition *definition,
				 int version)
{
	definition->version = version;
};

void set_oval_definition_class(struct oval_definition *definition,
			       oval_definition_class_enum class)
{
	definition->class = class;
};

void set_oval_definition_deprecated(struct oval_definition *definition,
				    int deprecated)
{
	definition->deprecated = deprecated;
};

void set_oval_definition_title(struct oval_definition *definition, char *title)
{
	definition->title = title;
};

void set_oval_definition_description(struct oval_definition *definition,
				     char *description)
{
	definition->description = description;
};

void set_oval_definition_criteria(struct oval_definition *definition,
				  struct oval_criteria_node *criteria)
{
	definition->criteria = criteria;
};

void add_oval_definition_affected(struct oval_definition *definition,
				  struct oval_affected *affected)
{
};

void add_oval_definition_reference(struct oval_definition *definition,
				   struct oval_reference *reference)
{
};

struct oval_string_map *_odaclassMap = NULL;
typedef struct _odaclass {
	int value;
} _odaclass_t;
void _odaclass_set(char *name, int val)
{
	_odaclass_t *enumval = (_odaclass_t *) malloc(sizeof(_odaclass_t));
	enumval->value = val;
	oval_string_map_put(_odaclassMap, name, (void *)enumval);
}

oval_definition_class_enum _odaclass(char *class)
{
	if (_odaclassMap == NULL) {
		_odaclassMap = oval_string_map_new();
		_odaclass_set("compliance", CLASS_COMPLIANCE);
		_odaclass_set("inventory", CLASS_INVENTORY);
		_odaclass_set("miscellaneous", CLASS_MISCELLANEOUS);
		_odaclass_set("patch", CLASS_PATCH);
		_odaclass_set("vulnerability", CLASS_VULNERABILITY);
	}
	_odaclass_t *valstar =
	    (_odaclass_t *) oval_string_map_get_value(_odaclassMap, class);
	return (valstar == NULL) ? CLASS_UNKNOWN : valstar->value;
}

void _oval_definition_title_consumer(char *string, void *user)
{
	struct oval_definition *definition = (struct oval_definition *)user;
	char *title = definition->title;
	if (title == NULL)
		title = string;
	else {
		int newsize = strlen(title) + strlen(string) + 1;
		char *newtitle = (char *)malloc(newsize * sizeof(char));
		strcat(newtitle, title);
		strcat(newtitle, string);
		free(title);
		free(string);
		title = newtitle;
	}
	definition->title = title;
}

void _oval_definition_description_consumer(char *string, void *user)
{
	struct oval_definition *definition = (struct oval_definition *)user;
	char *description = definition->description;
	if (description == NULL)
		description = string;
	else {
		int newsize = strlen(description) + strlen(string) + 1;
		char *newdescription = (char *)malloc(newsize * sizeof(char));
		strcat(newdescription, description);
		strcat(newdescription, string);
		free(description);
		free(string);
		description = newdescription;
	}
	definition->description = description;
}

void _oval_definition_affected_consumer(struct oval_affected *affected,
					void *user)
{
	struct oval_definition *definition = (struct oval_definition *)user;
	oval_collection_add(definition->affected, (void *)affected);
}

int _oval_definition_parse_metadata(xmlTextReaderPtr reader,
				    struct oval_parser_context *context,
				    void *user)
{
	struct oval_definition *definition = (struct oval_definition *)user;
	xmlChar *tagname = xmlTextReaderName(reader);
	int return_code;
	if ((strcmp(tagname, "title") == 0)) {
		return_code =
		    oval_parser_text_value(reader, context,
					   &_oval_definition_title_consumer,
					   definition);
	} else if (strcmp(tagname, "description") == 0) {
		return_code =
		    oval_parser_text_value(reader, context,
					   &_oval_definition_description_consumer,
					   definition);
	} else if (strcmp(tagname, "affected") == 0) {
		return_code =
		    oval_affected_parse_tag(reader, context,
					    &_oval_definition_affected_consumer,
					    definition);
	} else if (strcmp(tagname, "oval_repository") == 0) {	//NOOP
		return_code = oval_parser_skip_tag(reader, context);
	} else if (strcmp(tagname, "reference") == 0) {
		void reference_consumer(struct oval_reference *reference,
					void *null) {
			oval_collection_add(definition->reference,
					    (void *)reference);
		}
		return_code =
		    oval_reference_parse_tag(reader, context,
					     &reference_consumer, NULL);
	} else {
		int linno = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE::(oval_definition_parse_metadata)skipping <%s> depth = %d line = %d\n",
		     tagname, xmlTextReaderDepth(reader), linno);
		return_code = oval_parser_skip_tag(reader, context);
	}
	free(tagname);
	return return_code;
}

void _oval_definition_criteria_consumer(struct oval_criteria_node *criteria,
					void *user)
{
	struct oval_definition *definition = (struct oval_definition *)user;
	definition->criteria = criteria;
}

int _oval_definition_parse_tag(xmlTextReaderPtr reader,
			       struct oval_parser_context *context, void *user)
{
	struct oval_definition *definition = (struct oval_definition *)user;
	xmlChar *tagname = xmlTextReaderName(reader);
	int return_code;
	if ((strcmp(tagname, "metadata") == 0)) {
		return_code =
		    oval_parser_parse_tag(reader, context,
					  &_oval_definition_parse_metadata,
					  definition);
	} else if ((strcmp(tagname, "criteria") == 0)) {
		return_code = oval_criteria_parse_tag
		    (reader, context, &_oval_definition_criteria_consumer,
		     definition);
	} else {
		int linno = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE::(oval_definition)skipping <%s> depth = %d line = %d\n",
		     tagname, xmlTextReaderDepth(reader), linno);
		return_code = oval_parser_skip_tag(reader, context);
	}
	free(tagname);
	return return_code;
}

int oval_definition_parse_tag(xmlTextReaderPtr reader,
			      struct oval_parser_context *context)
{
	char *id = xmlTextReaderGetAttribute(reader, "id");
	struct oval_object_model *model = oval_parser_context_model(context);
	struct oval_definition *definition = get_oval_definition_new(model, id);
	char *version = xmlTextReaderGetAttribute(reader, "version");
	set_oval_definition_version(definition, atoi(version));
	free(version);
	char *class = xmlTextReaderGetAttribute(reader, "class");
	set_oval_definition_class(definition, _odaclass(class));
	free(class);
	int deprecated = oval_parser_boolean_attribute(reader, "deprecated", 0);
	set_oval_definition_deprecated(definition, deprecated);
	int return_code =
	    oval_parser_parse_tag(reader, context, &_oval_definition_parse_tag,
				  definition);
	return return_code;
}

void oval_definition_to_print(struct oval_definition *definition, char *indent,
			      int index)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (index == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sDEFINITION.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sDEFINITION[%d].", indent, index);

	printf("%sID          = %s\n", nxtindent, definition->id);
	printf("%sVERSION     = %d\n", nxtindent, definition->version);
	printf("%sCLASS       = %d\n", nxtindent, definition->class);
	printf("%sDEPRECATED  = %d\n", nxtindent, definition->deprecated);
	printf("%sTITLE       = %s\n", nxtindent, definition->title);
	printf("%sDESCRIPTION = %s\n", nxtindent, definition->description);
	struct oval_iterator *affecteds =
	    oval_collection_iterator(definition->affected);
	for (index = 1; oval_collection_iterator_has_more(affecteds); index++) {
		void *affected = oval_collection_iterator_next(affecteds);
		oval_affected_to_print(affected, nxtindent, index);
	}
	struct oval_iterator *references =
	    oval_collection_iterator(definition->reference);
	for (index = 1; oval_collection_iterator_has_more(references); index++) {
		void *reference = oval_collection_iterator_next(references);
		oval_reference_to_print(reference, nxtindent, index);
	}
	if (definition->criteria != NULL)
		oval_criteria_node_to_print(definition->criteria, nxtindent, 0);
}
