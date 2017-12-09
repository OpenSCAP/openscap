/**
 * @file oval_component.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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
 *      "Peter Vrabec" <pvrabec@redhat.com>
 *      "Tomas Heinrich" <theinric@redhat.com>
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "oval_definitions_impl.h"
#include "adt/oval_collection_impl.h"
#include "adt/oval_string_map_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#if defined(OVAL_PROBES_ENABLED)
#include "oval_probe.h"
#include "oval_probe_session.h"
#include "oval_probe_impl.h"
#endif
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"
#include "common/oscap_string.h"
#include "oval_glob_to_regex.h"
#include <pcre.h>

#if !defined(OVAL_PROBES_ENABLED)
const char *oval_subtype_to_str(oval_subtype_t subtype);
#endif

/***************************************************************************/
/* Variable definitions
 * */

static oval_syschar_collection_flag_t _flag_agg_map[7][7] = {
	{SYSCHAR_FLAG_UNKNOWN, SYSCHAR_FLAG_ERROR,
	 SYSCHAR_FLAG_COMPLETE, SYSCHAR_FLAG_INCOMPLETE,
	 SYSCHAR_FLAG_DOES_NOT_EXIST, SYSCHAR_FLAG_NOT_COLLECTED,
	 SYSCHAR_FLAG_NOT_APPLICABLE},

	{SYSCHAR_FLAG_ERROR, SYSCHAR_FLAG_ERROR,
	 SYSCHAR_FLAG_ERROR, SYSCHAR_FLAG_ERROR,
	 SYSCHAR_FLAG_ERROR, SYSCHAR_FLAG_ERROR,
	 SYSCHAR_FLAG_ERROR},

	{SYSCHAR_FLAG_COMPLETE, SYSCHAR_FLAG_ERROR,
	 SYSCHAR_FLAG_COMPLETE, SYSCHAR_FLAG_INCOMPLETE,
	 SYSCHAR_FLAG_DOES_NOT_EXIST, SYSCHAR_FLAG_NOT_COLLECTED,
	 SYSCHAR_FLAG_NOT_APPLICABLE},

	{SYSCHAR_FLAG_INCOMPLETE, SYSCHAR_FLAG_ERROR,
	 SYSCHAR_FLAG_INCOMPLETE, SYSCHAR_FLAG_INCOMPLETE,
	 SYSCHAR_FLAG_DOES_NOT_EXIST, SYSCHAR_FLAG_NOT_COLLECTED,
	 SYSCHAR_FLAG_NOT_APPLICABLE},

	{SYSCHAR_FLAG_DOES_NOT_EXIST, SYSCHAR_FLAG_ERROR,
	 SYSCHAR_FLAG_DOES_NOT_EXIST, SYSCHAR_FLAG_DOES_NOT_EXIST,
	 SYSCHAR_FLAG_DOES_NOT_EXIST, SYSCHAR_FLAG_NOT_COLLECTED,
	 SYSCHAR_FLAG_NOT_APPLICABLE},

	{SYSCHAR_FLAG_NOT_COLLECTED, SYSCHAR_FLAG_ERROR,
	 SYSCHAR_FLAG_NOT_COLLECTED, SYSCHAR_FLAG_NOT_COLLECTED,
	 SYSCHAR_FLAG_NOT_COLLECTED, SYSCHAR_FLAG_NOT_COLLECTED,
	 SYSCHAR_FLAG_NOT_APPLICABLE},

	{SYSCHAR_FLAG_NOT_APPLICABLE, SYSCHAR_FLAG_ERROR,
	 SYSCHAR_FLAG_NOT_APPLICABLE, SYSCHAR_FLAG_NOT_APPLICABLE,
	 SYSCHAR_FLAG_NOT_APPLICABLE, SYSCHAR_FLAG_NOT_APPLICABLE,
	 SYSCHAR_FLAG_NOT_APPLICABLE},
};

#define _AGG_FLAG(f1, f2) _flag_agg_map[f2][f1]
#define _COMP_TYPE(comp) oval_component_type_get_text(oval_component_get_type(comp))
#define _FLAG_TYPE(flag) oval_syschar_collection_flag_get_text(flag)

typedef struct {
	enum {
		OVAL_MODE_COMPUTE,
		OVAL_MODE_QUERY
	} mode;
	union {
		struct oval_syschar_model *sysmod;
#if defined(OVAL_PROBES_ENABLED)
		oval_probe_session_t *sess;
#endif
	} u;
} oval_argu_t;

typedef struct oval_component {
	struct oval_definition_model *model;
	oval_component_type_t type;
} oval_component_t;

typedef struct oval_component_LITERAL {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_value *value;	/*type==OVAL_COMPONENT_LITERAL */
} oval_component_LITERAL_t;

typedef struct oval_component_OBJECTREF {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_object *object;	/*type==OVAL_COMPONENT_OBJECTREF */
	char *item_field;
	char *record_field;
} oval_component_OBJECTREF_t;

typedef struct oval_component_VARREF {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_variable *variable;	/*type==OVAL_COMPONENT_VARREF */
} oval_component_VARREF_t;

typedef struct oval_component_FUNCTION {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	/*type==OVAL_COMPONENT_FUNCTION */
} oval_component_FUNCTION_t;

typedef struct oval_component_ARITHMETIC {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	/*type==OVAL_COMPONENT_FUNCTION */
	oval_arithmetic_operation_t operation;	/*type==OVAL_COMPONENT_ARITHMETIC */
} oval_component_ARITHMETIC_t;

typedef struct oval_component_BEGEND {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	//type==OVAL_COMPONENT_FUNCTION
	char *character;	//type==OVAL_COMPONENT_BEGIN/END
} oval_component_BEGEND_t;

typedef struct oval_component_SPLIT {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	/*type==OVAL_COMPONENT_FUNCTION */
	char *delimiter;	/*type==OVAL_COMPONENT_SPLIT */
} oval_component_SPLIT_t;

typedef struct oval_component_GLOB {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	/*type==OVAL_COMPONENT_FUNCTION */
	bool glob_noescape;	/*type==OVAL_COMPONENT_GLOB */
} oval_component_GLOB_t;

typedef struct oval_component_SUBSTRING {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	/*type==OVAL_COMPONENT_FUNCTION */
	int start;		/*type==OVAL_COMPONENT_SUBSTRING */
	int length;		/*type==OVAL_COMPONENT_SUBSTRING */
} oval_component_SUBSTRING_t;

typedef struct oval_component_TIMEDIF {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	/*type==OVAL_COMPONENT_FUNCTION */
	oval_datetime_format_t format_1;	/*/type==OVAL_COMPONENT_TIMEDIF */
	oval_datetime_format_t format_2;	/*type==OVAL_COMPONENT_TIMEDIF */
} oval_component_TIMEDIF_t;

typedef struct oval_component_REGEX_CAPTURE {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	/*type==OVAL_COMPONENT_FUNCTION */
	char *pattern;		/*type==OVAL_COMPONENT_REGEX_CAPTURE */
} oval_component_REGEX_CAPTURE_t;

void oval_component_to_print(struct oval_component *component, char *indent, int index);
//typedef void (*oval_component_consumer)(struct oval_component_node *, void*);

/*
	OVAL_COMPONENT_UNKNOWN = 0,
	OVAL_FUNCTION_BEGIN = OVAL_FUNCTION + 1,
	OVAL_FUNCTION_CONCAT = OVAL_FUNCTION + 2,
	OVAL_FUNCTION_END = OVAL_FUNCTION + 3,
	OVAL_FUNCTION_SPLIT = OVAL_FUNCTION + 4,
	OVAL_FUNCTION_SUBSTRING = OVAL_FUNCTION + 5,
	OVAL_FUNCTION_TIMEDIF = OVAL_FUNCTION + 6,
	OVAL_FUNCTION_ESCAPE_REGEX = OVAL_FUNCTION + 7,
	OVAL_FUNCTION_REGEX_CAPTURE = OVAL_FUNCTION + 8,
	OVAL_FUNCTION_ARITHMETIC = OVAL_FUNCTION + 9
	OVAL_FUNCTION_COUNT = OVAL_FUNCTION + 10,
	OVAL_FUNCTION_GLOB_TO_REGEX = OVAL_FUNCTION + 12,

 */
static const struct oscap_string_map _OVAL_COMPONENT_MAP[] = {
	{OVAL_COMPONENT_LITERAL, "literal_component"},
	{OVAL_COMPONENT_OBJECTREF, "object_component"},
	{OVAL_COMPONENT_VARREF, "variable_component"},
	{0, NULL}
};

static const struct oscap_string_map _OVAL_FUNCTION_MAP[] = {
	{OVAL_FUNCTION_BEGIN, "begin"},
	{OVAL_FUNCTION_CONCAT, "concat"},
	{OVAL_FUNCTION_END, "end"},
	{OVAL_FUNCTION_SPLIT, "split"},
	{OVAL_FUNCTION_SUBSTRING, "substring"},
	{OVAL_FUNCTION_TIMEDIF, "time_difference"},
	{OVAL_FUNCTION_ESCAPE_REGEX, "escape_regex"},
	{OVAL_FUNCTION_REGEX_CAPTURE, "regex_capture"},
	{OVAL_FUNCTION_ARITHMETIC, "arithmetic"},
	{OVAL_FUNCTION_COUNT, "count"},
	{OVAL_FUNCTION_UNIQUE, "unique"},
	{OVAL_FUNCTION_GLOB_TO_REGEX, "glob_to_regex"},
	{0, NULL}
};

/* End of variable definitions
 * */
/***************************************************************************/

bool oval_component_iterator_has_more(struct oval_component_iterator *oc_component)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)oc_component);
}

int oval_component_iterator_remaining(struct oval_component_iterator *oc_component)
{
	return oval_collection_iterator_remaining((struct oval_iterator *)oc_component);
}

struct oval_component *oval_component_iterator_next(struct oval_component_iterator *oc_component)
{
	return (struct oval_component *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_component);
}

void oval_component_iterator_free(struct oval_component_iterator *oc_component)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_component);
}

oval_component_type_t oval_component_get_type(struct oval_component *component)
{
	__attribute__nonnull__(component);

	return component->type;
}

struct oval_value *oval_component_get_literal_value(struct oval_component *component) {
	/* type == OVAL_COMPONENT_LITERAL */
	struct oval_value *value = NULL;
	if (oval_component_get_type(component) == OVAL_COMPONENT_LITERAL) {
		value = ((struct oval_component_LITERAL *)component)->value;
	}
	return value;
}

void oval_component_set_literal_value(struct oval_component *component, struct oval_value *value) {
	/* type == OVAL_COMPONENT_LITERAL */
	__attribute__nonnull__(component);
	if (oval_component_get_type(component) == OVAL_COMPONENT_LITERAL) {
		((struct oval_component_LITERAL *)component)->value = value;
	}
}

void oval_component_set_type(struct oval_component *component, oval_component_type_t type)
{
	__attribute__nonnull__(component);
	component->type = type;
}

struct oval_object *oval_component_get_object(struct oval_component *component)
{
	struct oval_object *object = NULL;
	if (oval_component_get_type(component) == OVAL_COMPONENT_OBJECTREF) {
		object = ((struct oval_component_OBJECTREF *)component)->object;
	}
	return object;
}

void oval_component_set_object(struct oval_component *component, struct oval_object *object) {
	__attribute__nonnull__(component);
	if (oval_component_get_type(component) == OVAL_COMPONENT_OBJECTREF) {
		((struct oval_component_OBJECTREF *)component)->object = object;
	}
}

char *oval_component_get_item_field(struct oval_component *component) {
	char *field = NULL;
	if (oval_component_get_type(component) == OVAL_COMPONENT_OBJECTREF) {
		field = ((struct oval_component_OBJECTREF *)component)->item_field;
	}
	return field;
}

void oval_component_set_item_field(struct oval_component *component, char *field) {
	__attribute__nonnull__(component);
	if (oval_component_get_type(component) == OVAL_COMPONENT_OBJECTREF) {
		((struct oval_component_OBJECTREF *)component)->item_field = oscap_strdup(field);
	}
}

char *oval_component_get_record_field(struct oval_component *component) {
	if (oval_component_get_type(component) != OVAL_COMPONENT_OBJECTREF)
		return NULL;

	return ((struct oval_component_OBJECTREF *) component)->record_field;
}

void oval_component_set_record_field(struct oval_component *component, char *field) {
	__attribute__nonnull__(component);

	if (oval_component_get_type(component) != OVAL_COMPONENT_OBJECTREF) {
		dW("Wrong component type: %d.", oval_component_get_type(component));
		return;
	}

	((struct oval_component_OBJECTREF *) component)->record_field = oscap_strdup(field);
}

struct oval_variable *oval_component_get_variable(struct oval_component *component) {
	/* type == OVAL_COMPONENT_VARREF */
	struct oval_variable *variable = NULL;
	if (oval_component_get_type(component) == OVAL_COMPONENT_VARREF) {
		variable = ((struct oval_component_VARREF *)component)->variable;
	}
	return variable;
}

struct oval_component_iterator *oval_component_get_function_components(struct
								       oval_component
								       *component)
{
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_FUNCTION */
	struct oval_component_iterator *iterator;
	if (component->type > OVAL_COMPONENT_FUNCTION) {
		oval_component_FUNCTION_t *function = (oval_component_FUNCTION_t *) component;
		iterator = (struct oval_component_iterator *)
		    oval_collection_iterator(function->function_components);
	} else
		iterator = NULL;
	return iterator;
}

oval_arithmetic_operation_t oval_component_get_arithmetic_operation(struct oval_component * component)
{
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_ARITHMETIC */
	oval_arithmetic_operation_t operation;
	if (component->type == OVAL_FUNCTION_ARITHMETIC) {
		oval_component_ARITHMETIC_t *arithmetic = (oval_component_ARITHMETIC_t *) component;
		operation = arithmetic->operation;
	} else
		operation = OVAL_ARITHMETIC_UNKNOWN;
	return operation;
}

void oval_component_set_arithmetic_operation(struct oval_component *component, oval_arithmetic_operation_t operation) 
{
	__attribute__nonnull__(component);
	
	/* type == OVAL_COMPONENT_ARITHMETIC */
	if (component->type == OVAL_FUNCTION_ARITHMETIC) {
		oval_component_ARITHMETIC_t *arithmetic = (oval_component_ARITHMETIC_t *) component;
		arithmetic->operation = operation;
	}
}

char *oval_component_get_prefix(struct oval_component *component)
{
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_BEGIN/END */
	char *character;
	if (component->type == OVAL_FUNCTION_BEGIN) {
		oval_component_BEGEND_t *begin = (oval_component_BEGEND_t *) component;
		character = begin->character;
	} else
		character = NULL;
	return character;
}

void oval_component_set_prefix(struct oval_component *component, char *character) 
{
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_BEGIN */
	if (component->type == OVAL_FUNCTION_BEGIN) {
		oval_component_BEGEND_t *begin = (oval_component_BEGEND_t *) component;
		begin->character = oscap_strdup(character);
	}
}

char *oval_component_get_suffix(struct oval_component *component)
{
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_END */
	char *character;
	if (component->type == OVAL_FUNCTION_END) {
		oval_component_BEGEND_t *funcend = (oval_component_BEGEND_t *) component;
		character = funcend->character;
	} else
		character = NULL;
	return character;
}

void oval_component_set_suffix(struct oval_component *component, char *character) {
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_END */
	if (component->type == OVAL_FUNCTION_END) {
		oval_component_BEGEND_t *funcend = (oval_component_BEGEND_t *) component;
		funcend->character = oscap_strdup(character);
	}
}

char *oval_component_get_split_delimiter(struct oval_component *component)
{
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_SPLIT */
	char *delimiter;
	if (component->type == OVAL_FUNCTION_SPLIT) {
		oval_component_SPLIT_t *split = (oval_component_SPLIT_t *) component;
		delimiter = split->delimiter;
	} else
		delimiter = NULL;
	return delimiter;
}

void oval_component_set_split_delimiter(struct oval_component *component, char *delimeter) {
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_SPLIT */
	if (component->type == OVAL_FUNCTION_SPLIT) {
		oval_component_SPLIT_t *split = (oval_component_SPLIT_t *) component;
		split->delimiter = oscap_strdup(delimeter);
	}
}

bool oval_component_get_glob_to_regex_glob_noescape(struct oval_component *component)
{
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_GLOB */
	bool glob_noescape;
	if (component->type == OVAL_FUNCTION_GLOB_TO_REGEX) {
		oval_component_GLOB_t *glob_to_regex = (oval_component_GLOB_t *) component;
		glob_noescape = glob_to_regex->glob_noescape;
	} else
		glob_noescape = false;
	return glob_noescape;
}

void oval_component_set_glob_to_regex_glob_noescape(struct oval_component *component, bool glob_noescape) {
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_GLOB */
	if (component->type == OVAL_FUNCTION_GLOB_TO_REGEX) {
		oval_component_GLOB_t *glob_to_regex = (oval_component_GLOB_t *) component;
		glob_to_regex->glob_noescape = glob_noescape;
	}
}

int oval_component_get_substring_start(struct oval_component *component)
{
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_SUBSTRING */
	int start;
	if (component->type == OVAL_FUNCTION_SUBSTRING) {
		oval_component_SUBSTRING_t *substring = (oval_component_SUBSTRING_t *) component;
		start = substring->start;
	} else
		start = -1;
	return start;
}

void oval_component_set_substring_start(struct oval_component *component, int start)
{
	__attribute__nonnull__(component);
	
	/* type==OVAL_COMPONENT_SUBSTRING */
	if (component->type == OVAL_FUNCTION_SUBSTRING) {
		oval_component_SUBSTRING_t *substring = (oval_component_SUBSTRING_t *) component;
		substring->start = start;
	}
}

int oval_component_get_substring_length(struct oval_component *component)
{
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_SUBSTRING */
	int length;
	if (component->type == OVAL_FUNCTION_SUBSTRING) {
		oval_component_SUBSTRING_t *substring = (oval_component_SUBSTRING_t *) component;
		length = substring->length;
	} else
		length = -1;
	return length;
}

void oval_component_set_substring_length(struct oval_component *component, int length) 
{
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_SUBSTRING */
	if (component->type == OVAL_FUNCTION_SUBSTRING) {
		oval_component_SUBSTRING_t *substring = (oval_component_SUBSTRING_t *) component;
		substring->length = length;
	}
}

oval_datetime_format_t oval_component_get_timedif_format_1(struct oval_component
							   *component)
{
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_TIMEDIF */
	oval_datetime_format_t format;
	if (component->type == OVAL_FUNCTION_TIMEDIF) {
		oval_component_TIMEDIF_t *timedif = (oval_component_TIMEDIF_t *) component;
		format = timedif->format_1;
	} else
		format = OVAL_DATETIME_UNKNOWN;
	return format;
}

void oval_component_set_timedif_format_1(struct oval_component *component, oval_datetime_format_t format) 
{
	__attribute__nonnull__(component);
	/* type == OVAL_COMPONENT_TIMEDIF */
	if (component->type == OVAL_FUNCTION_TIMEDIF) {
		oval_component_TIMEDIF_t *timedif = (oval_component_TIMEDIF_t *) component;
		timedif->format_1 = format;
	}
}

oval_datetime_format_t oval_component_get_timedif_format_2(struct oval_component
							   *component)
{
	__attribute__nonnull__(component);

	/* typ == OVAL_COMPONENT_TIMEDIF */
	oval_datetime_format_t format;
	if (component->type == OVAL_FUNCTION_TIMEDIF) {
		oval_component_TIMEDIF_t *timedif = (oval_component_TIMEDIF_t *) component;
		format = timedif->format_2;
	} else
		format = OVAL_DATETIME_UNKNOWN;
	return format;
}

void oval_component_set_timedif_format_2(struct oval_component *component, oval_datetime_format_t format) 
{
	__attribute__nonnull__(component);
	/* type == OVAL_COMPONENT_TIMEDIF */
	if (component->type == OVAL_FUNCTION_TIMEDIF) {
		oval_component_TIMEDIF_t *timedif = (oval_component_TIMEDIF_t *) component;
		timedif->format_2 = format;
	}
}

char *oval_component_get_regex_pattern(struct oval_component *component) {
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_REGEX_CAPTURE */
	char *pattern = NULL;
	if (component->type == OVAL_FUNCTION_REGEX_CAPTURE) {
		oval_component_REGEX_CAPTURE_t *regex = (oval_component_REGEX_CAPTURE_t *) component;
		pattern = regex->pattern;
	}
	return pattern;
}

void oval_component_set_regex_pattern(struct oval_component *component, char *pattern) 
{
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_REGEX_CAPTURE */
	if (component->type == OVAL_FUNCTION_REGEX_CAPTURE) {
		oval_component_REGEX_CAPTURE_t *regex = (oval_component_REGEX_CAPTURE_t *) component;
		regex->pattern = oscap_strdup(pattern);
	}
}

struct oval_component *oval_component_new(struct oval_definition_model *model, oval_component_type_t type)
{
	oval_component_t *component = NULL;

	switch (type) {
	case OVAL_COMPONENT_LITERAL:{
			oval_component_LITERAL_t *literal =
			    (oval_component_LITERAL_t *) malloc(sizeof(oval_component_LITERAL_t));
			if (literal == NULL)
				return NULL;

			component = (oval_component_t *) literal;
			literal->value = NULL;
		}
		break;
	case OVAL_COMPONENT_OBJECTREF:{
			oval_component_OBJECTREF_t *objectref =
			    (oval_component_OBJECTREF_t *) malloc(sizeof(oval_component_OBJECTREF_t));
			if (objectref == NULL)
				return NULL;

			component = (oval_component_t *) objectref;
			objectref->object = NULL;
			objectref->item_field = NULL;
			objectref->record_field = NULL;
		}
		break;
	case OVAL_COMPONENT_VARREF:{
			oval_component_VARREF_t *varref =
			    (oval_component_VARREF_t *) malloc(sizeof(oval_component_VARREF_t));
			if (varref == NULL)
				return NULL;

			component = (oval_component_t *) varref;
			varref->variable = NULL;
		}
		break;
	default:		/* OVAL_COMPONENT_FUNCTION */
		{
			oval_component_FUNCTION_t *function;
			switch (type) {
			case OVAL_FUNCTION_ARITHMETIC:{
					oval_component_ARITHMETIC_t *arithmetic = (oval_component_ARITHMETIC_t *)
					    (function = (oval_component_FUNCTION_t *)
					     malloc(sizeof(oval_component_ARITHMETIC_t)));
					if (arithmetic == NULL)
						return NULL;

					arithmetic->operation = OVAL_ARITHMETIC_UNKNOWN;
				};
				break;
			case OVAL_FUNCTION_BEGIN:
			case OVAL_FUNCTION_END:{
					oval_component_BEGEND_t *begin = (oval_component_BEGEND_t *)
					    (function = (oval_component_FUNCTION_t *)
					     malloc(sizeof(oval_component_BEGEND_t)));
					if (begin == NULL)
						return NULL;

					begin->character = NULL;
				};
				break;
			case OVAL_FUNCTION_SPLIT:{
					oval_component_SPLIT_t *split = (oval_component_SPLIT_t *)
					    (function = (oval_component_FUNCTION_t *)
					     malloc(sizeof(oval_component_SPLIT_t)));
					if (split == NULL)
						return NULL;

					split->delimiter = NULL;
				};
				break;
			case OVAL_FUNCTION_GLOB_TO_REGEX:{
					oval_component_GLOB_t *glob_to_regex = (oval_component_GLOB_t *)
					    (function = (oval_component_FUNCTION_t *)
					     malloc(sizeof(oval_component_GLOB_t)));
					if (glob_to_regex == NULL)
						return NULL;
					glob_to_regex->glob_noescape = false;
				};
				break;
			case OVAL_FUNCTION_SUBSTRING:{
					oval_component_SUBSTRING_t *substring = (oval_component_SUBSTRING_t *)
					    (function = (oval_component_FUNCTION_t *)
					     malloc(sizeof(oval_component_SUBSTRING_t)));
					if (substring == NULL)
						return NULL;

					substring->length = -1;
					substring->start = -1;
				};
				break;
			case OVAL_FUNCTION_TIMEDIF:{
					oval_component_TIMEDIF_t *timedif = (oval_component_TIMEDIF_t *)
					    (function = (oval_component_FUNCTION_t *)
					     malloc(sizeof(oval_component_TIMEDIF_t)));
					if (timedif == NULL)
						return NULL;

					timedif->format_1 = OVAL_DATETIME_UNKNOWN;
					timedif->format_2 = OVAL_DATETIME_UNKNOWN;
				};
				break;
			case OVAL_FUNCTION_REGEX_CAPTURE:{
					oval_component_REGEX_CAPTURE_t *regex = (oval_component_REGEX_CAPTURE_t *)
					    (function = (oval_component_FUNCTION_t *)
					     malloc(sizeof(oval_component_REGEX_CAPTURE_t)));
					if (regex == NULL)
						return NULL;

					regex->pattern = NULL;
				};
				break;
			default:{
					function = (oval_component_FUNCTION_t *)
					    malloc(sizeof(oval_component_FUNCTION_t));
					if (function == NULL)
						return NULL;
				}
			}

			component = (oval_component_t *) function;
			function->function_components = oval_collection_new();
		}
		break;

	}
	component->type = type;
	component->model = model;
	return component;
}

static void _oval_component_clone_subcomponents
    (struct oval_component *old_component, struct oval_component *new_component, struct oval_definition_model *model) {
	struct oval_component_iterator *subcomps = oval_component_get_function_components(old_component);
	if (subcomps)
		while (oval_component_iterator_has_more(subcomps)) {
			struct oval_component *subcomp = oval_component_iterator_next(subcomps);
			oval_component_add_function_component(new_component, oval_component_clone(model, subcomp));
		}
	oval_component_iterator_free(subcomps);
}

struct oval_component *oval_component_clone(struct oval_definition_model *new_model,
					    struct oval_component *old_component)
{
	__attribute__nonnull__(old_component);

	struct oval_component *new_component = oval_component_new(new_model, old_component->type);
	if (new_component == NULL)
		return NULL;

	switch (new_component->type) {
	case OVAL_FUNCTION_ARITHMETIC:{
			oval_arithmetic_operation_t operation = oval_component_get_arithmetic_operation(old_component);
			if (operation)
				oval_component_set_arithmetic_operation(new_component, operation);
		}
		break;
	case OVAL_FUNCTION_BEGIN:{
			char *begchar = oval_component_get_prefix(old_component);
			if (begchar)
				oval_component_set_prefix(new_component, begchar);
		}
		break;
	case OVAL_FUNCTION_END:{
			char *endchar = oval_component_get_suffix(old_component);
			if (endchar)
				oval_component_set_suffix(new_component, endchar);
		}
		break;
	case OVAL_COMPONENT_LITERAL:{
			struct oval_value *value = oval_component_get_literal_value(old_component);
			if (value)
				oval_component_set_literal_value(new_component, oval_value_clone(value));
		}
		break;
	case OVAL_COMPONENT_OBJECTREF:{
			char *field;
			struct oval_object *old_object = oval_component_get_object(old_component);
			if (old_object) {
				struct oval_object *new_object = oval_object_clone(new_model, old_object);
				oval_component_set_object(new_component, new_object);
			}
			field = oval_component_get_item_field(old_component);
			if (field)
				oval_component_set_item_field(new_component, field);
			field = oval_component_get_record_field(old_component);
			if (field)
				oval_component_set_record_field(new_component, field);
		}
		break;
	case OVAL_FUNCTION_REGEX_CAPTURE:{
			char *pattern = oval_component_get_regex_pattern(old_component);
			if (pattern)
				oval_component_set_regex_pattern(new_component, pattern);
		}
		break;
	case OVAL_FUNCTION_SPLIT:{
			char *delimiter = oval_component_get_split_delimiter(old_component);
			if (delimiter)
				oval_component_set_split_delimiter(new_component, delimiter);
		}
		break;
	case OVAL_FUNCTION_GLOB_TO_REGEX:{
			bool glob_noescape = oval_component_get_glob_to_regex_glob_noescape(old_component);
			oval_component_set_glob_to_regex_glob_noescape(new_component, glob_noescape);
		}
		break;
	case OVAL_FUNCTION_SUBSTRING:{
			int length = oval_component_get_substring_length(old_component);
			oval_component_set_substring_length(new_component, length);
			int start = oval_component_get_substring_start(old_component);
			oval_component_set_substring_start(new_component, start);
		} break;
	case OVAL_FUNCTION_TIMEDIF:{
			oval_datetime_format_t format1 = oval_component_get_timedif_format_1(old_component);
			if (format1)
				oval_component_set_timedif_format_1(new_component, format1);
			oval_datetime_format_t format2 = oval_component_get_timedif_format_2(old_component);
			if (format2)
				oval_component_set_timedif_format_1(new_component, format2);
		}
		break;
	case OVAL_COMPONENT_VARREF:{
			struct oval_variable *old_variable = oval_component_get_variable(old_component);
			if (old_variable) {
				struct oval_variable *new_variable = oval_variable_clone(new_model, old_variable);
				oval_component_set_variable(new_component, new_variable);
			}
		} break;
	default:
		/*NOOP*/ break;
	}

	if (new_component->type > OVAL_FUNCTION)
		_oval_component_clone_subcomponents(old_component, new_component, new_model);

	return new_component;
}

void oval_component_free(struct oval_component *component)
{
	if (component == NULL)
		return;

	switch (component->type) {
	case OVAL_COMPONENT_LITERAL:{
			oval_component_LITERAL_t *literal = (oval_component_LITERAL_t *) component;
			oval_value_free(literal->value);
			literal->value = NULL;
		}
		break;
	case OVAL_COMPONENT_OBJECTREF:{
			oval_component_OBJECTREF_t *objectref = (oval_component_OBJECTREF_t *) component;
			free(objectref->item_field);
			objectref->item_field = NULL;
			free(objectref->record_field);
			objectref->record_field = NULL;
		}
		break;
	case OVAL_FUNCTION_BEGIN:
	case OVAL_FUNCTION_END:{
			oval_component_BEGEND_t *begin = (oval_component_BEGEND_t *) component;
			free(begin->character);
			begin->character = NULL;
		};
		break;
	case OVAL_FUNCTION_SPLIT:{
			oval_component_SPLIT_t *split = (oval_component_SPLIT_t *) component;
			free(split->delimiter);
			split->delimiter = NULL;
		};
		break;
	case OVAL_FUNCTION_REGEX_CAPTURE:{
			oval_component_REGEX_CAPTURE_t *regex = (oval_component_REGEX_CAPTURE_t *) component;
			free(regex->pattern);
			regex->pattern = NULL;
		};
		break;
	case OVAL_FUNCTION_GLOB_TO_REGEX:
	case OVAL_FUNCTION_CONCAT:
	case OVAL_FUNCTION_COUNT:
	case OVAL_FUNCTION_UNIQUE:
	case OVAL_FUNCTION_SUBSTRING:
	case OVAL_FUNCTION_TIMEDIF:
	case OVAL_FUNCTION_ESCAPE_REGEX:
	case OVAL_FUNCTION_ARITHMETIC:
	case OVAL_FUNCTION_LAST:
	case OVAL_COMPONENT_VARREF:
	case OVAL_COMPONENT_FUNCTION:
	case OVAL_COMPONENT_UNKNOWN:
		 /*NOOP*/ break;
	}
	if (component->type > OVAL_FUNCTION) {
		oval_component_FUNCTION_t *function = (oval_component_FUNCTION_t *) component;
		oval_collection_free_items(function->function_components, (oscap_destruct_func) oval_component_free);
		function->function_components = NULL;
	}
	free(component);
}

void oval_component_add_function_component(struct oval_component *component, struct oval_component *func_component) 
{
	__attribute__nonnull__(component);
	
	if (component->type > OVAL_FUNCTION) {
		oval_component_FUNCTION_t *function = (oval_component_FUNCTION_t *) component;
		if (func_component)
			oval_collection_add(function->function_components, func_component);
	}
}

void oval_component_set_variable(struct oval_component *component, struct oval_variable *variable)
{
	__attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_VARREF */
	if (component->type == OVAL_COMPONENT_VARREF) {
		oval_component_VARREF_t *varref = (oval_component_VARREF_t *) component;
		varref->variable = variable;
	}
}

static void oval_value_consume(struct oval_value *value, void *component)
{
	oval_component_set_literal_value(component, value);
}

static int _oval_component_parse_LITERAL_tag(xmlTextReaderPtr reader,
					     struct oval_parser_context *context, struct oval_component *component)
{

	return oval_value_parse_tag(reader, context, oval_value_consume, component);
}

static int _oval_component_parse_OBJECTREF_tag(xmlTextReaderPtr reader,
					       struct oval_parser_context *context, struct oval_component *component)
{

	struct oval_definition_model *model = context->definition_model;
	char *objref = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "object_ref");
	struct oval_object *object = oval_definition_model_get_new_object(model, objref);
	char *field;

	free(objref);
	objref = NULL;
	oval_component_set_object(component, object);

	field = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "item_field");
	oval_component_set_item_field(component, field);
	if (field)
		free(field);
	field = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "record_field");
	oval_component_set_record_field(component, field);
	if (field)
		free(field);

	return 0;
}

static int _oval_component_parse_VARREF_tag(xmlTextReaderPtr reader,
					    struct oval_parser_context *context, struct oval_component *component)
{

	struct oval_definition_model *model = context->definition_model;
	char *varref = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "var_ref");
	struct oval_variable *variable = oval_definition_model_get_new_variable(model, varref, OVAL_VARIABLE_UNKNOWN);
	if (varref != NULL) {
		free(varref);
		varref = NULL;
	}
	oval_component_set_variable(component, variable);

	return 0;
}

static void oval_subcomp_consume(struct oval_component *subcomp, void *func)
{

	__attribute__nonnull__(func);

	oval_component_FUNCTION_t *function = func;
	oval_collection_add(function->function_components, (void *)subcomp);
}

static int oval_subcomp_tag_consume(xmlTextReaderPtr reader, struct oval_parser_context *context, void *func)
{

	return oval_component_parse_tag(reader, context, &oval_subcomp_consume, func);
}

static int _oval_component_parse_FUNCTION_tag(xmlTextReaderPtr reader,
					      struct oval_parser_context *context, struct oval_component *component)
{
	oval_component_FUNCTION_t *function = (oval_component_FUNCTION_t *) component;
	return oval_parser_parse_tag(reader, context, &oval_subcomp_tag_consume, function);
}

static int _oval_component_parse_ARITHMETIC_tag(xmlTextReaderPtr reader,
						struct oval_parser_context *context, struct oval_component *component)
{
	__attribute__nonnull__(component);

	oval_component_ARITHMETIC_t *arithmetic = (oval_component_ARITHMETIC_t *) component;
	oval_arithmetic_operation_t operation = oval_arithmetic_operation_parse(reader, "arithmetic_operation",
										OVAL_ARITHMETIC_UNKNOWN);
	arithmetic->operation = operation;
	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

static int _oval_component_parse_BEGEND_tag(xmlTextReaderPtr reader,
					    struct oval_parser_context *context, struct oval_component *component)
{

	__attribute__nonnull__(component);

	oval_component_BEGEND_t *begend = (oval_component_BEGEND_t *) component;
	begend->character = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "character");

	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

static int _oval_component_parse_SPLIT_tag(xmlTextReaderPtr reader,
					   struct oval_parser_context *context, struct oval_component *component)
{

	__attribute__nonnull__(component);

	oval_component_SPLIT_t *split = (oval_component_SPLIT_t *) component;
	split->delimiter = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "delimiter");

	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

static int _oval_component_parse_GLOB_TO_REGEX_tag(xmlTextReaderPtr reader,
					   struct oval_parser_context *context, struct oval_component *component)
{

	__attribute__nonnull__(component);

	oval_component_GLOB_t *glob_to_regex = (oval_component_GLOB_t *) component;
	glob_to_regex->glob_noescape = oval_parser_boolean_attribute(reader, "glob_noescape", 0);

	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}


static int _oval_component_parse_SUBSTRING_tag(xmlTextReaderPtr reader,
					       struct oval_parser_context *context, struct oval_component *component)
{

	__attribute__nonnull__(component);

	oval_component_SUBSTRING_t *substring = (oval_component_SUBSTRING_t *) component;
	char *start_text = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "substring_start");
	char *length_text = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "substring_length");
	int start = (start_text == NULL) ? 0 : atoi(start_text);
	int length = (length_text == NULL) ? 0 : atoi(length_text);
	if (start_text != NULL)
		free(start_text);
	if (length_text != NULL)
		free(length_text);
	substring->start = start;
	substring->length = length;

	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

static int _oval_component_parse_TIMEDIF_tag(xmlTextReaderPtr reader,
					     struct oval_parser_context *context, struct oval_component *component)
{

	__attribute__nonnull__(component);

	oval_component_TIMEDIF_t *timedif = (oval_component_TIMEDIF_t *) component;
	oval_datetime_format_t format_1 = oval_datetime_format_parse(reader, "format_1",
								     OVAL_DATETIME_YEAR_MONTH_DAY);
	oval_datetime_format_t format_2 = oval_datetime_format_parse(reader, "format_2",
								     OVAL_DATETIME_YEAR_MONTH_DAY);
	timedif->format_1 = format_1;
	timedif->format_2 = format_2;

	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

static int _oval_component_parse_REGEX_CAPTURE_tag(xmlTextReaderPtr reader,
						   struct oval_parser_context *context,
						   struct oval_component *component)
{
	__attribute__nonnull__(component);

	oval_component_REGEX_CAPTURE_t *regex = (oval_component_REGEX_CAPTURE_t *) component;

	regex->pattern = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "pattern");

	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

int oval_component_parse_tag(xmlTextReaderPtr reader,
			     struct oval_parser_context *context, oval_component_consumer consumer, void *user)
{

	__attribute__nonnull__(context);

	int return_code = 0;

	struct oval_definition_model *model = context->definition_model;
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	/* oval_component_type_enum type; */
	struct oval_component *component = NULL;
	if (strcmp(tagname, "literal_component") == 0) {
		component = oval_component_new(model, OVAL_COMPONENT_LITERAL);
		return_code = _oval_component_parse_LITERAL_tag(reader, context, component);
	} else if (strcmp(tagname, "object_component") == 0) {
		component = oval_component_new(model, OVAL_COMPONENT_OBJECTREF);
		return_code = _oval_component_parse_OBJECTREF_tag(reader, context, component);
	} else if (strcmp(tagname, "variable_component") == 0) {
		component = oval_component_new(model, OVAL_COMPONENT_VARREF);
		return_code = _oval_component_parse_VARREF_tag(reader, context, component);
	} else if (strcmp(tagname, "arithmetic") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_ARITHMETIC);
		return_code = _oval_component_parse_ARITHMETIC_tag(reader, context, component);
	} else if (strcmp(tagname, "begin") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_BEGIN);
		return_code = _oval_component_parse_BEGEND_tag(reader, context, component);
	} else if (strcmp(tagname, "concat") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_CONCAT);
		return_code = _oval_component_parse_FUNCTION_tag(reader, context, component);
	} else if (strcmp(tagname, "count") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_COUNT);
		return_code = _oval_component_parse_FUNCTION_tag(reader, context, component);
	} else if (strcmp(tagname, "unique") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_UNIQUE);
		return_code = _oval_component_parse_FUNCTION_tag(reader, context, component);
	} else if (strcmp(tagname, "end") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_END);
		return_code = _oval_component_parse_BEGEND_tag(reader, context, component);
	} else if (strcmp(tagname, "escape_regex") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_ESCAPE_REGEX);
		return_code = _oval_component_parse_FUNCTION_tag(reader, context, component);
	} else if (strcmp(tagname, "glob_to_regex") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_GLOB_TO_REGEX);
		return_code = _oval_component_parse_GLOB_TO_REGEX_tag(reader, context, component);
	} else if (strcmp(tagname, "split") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_SPLIT);
		return_code = _oval_component_parse_SPLIT_tag(reader, context, component);
	} else if (strcmp(tagname, "substring") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_SUBSTRING);
		return_code = _oval_component_parse_SUBSTRING_tag(reader, context, component);
	} else if (strcmp(tagname, "time_difference") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_TIMEDIF);
		return_code = _oval_component_parse_TIMEDIF_tag(reader, context, component);
	} else if (strcmp(tagname, "regex_capture") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_REGEX_CAPTURE);
		return_code = _oval_component_parse_REGEX_CAPTURE_tag(reader, context, component);
	} else {
		dI("Tag <%s> not handled, line: %d.", tagname,
                              xmlTextReaderGetParserLineNumber(reader));
		return_code = oval_parser_skip_tag(reader, context);
	}
	if (component != NULL)
		(*consumer) (component, user);

	if (return_code != 0 ) {
		dW("Parsing of <%s> terminated by an error at line %d.", tagname, xmlTextReaderGetParserLineNumber(reader));
	}
	free(tagname);
	return return_code;
}

xmlNode *oval_component_to_dom(struct oval_component *component, xmlDoc * doc, xmlNode * parent)
{
	oval_component_type_t type = oval_component_get_type(component);
	const char *local_name = type < OVAL_FUNCTION
	    ? _OVAL_COMPONENT_MAP[type - 1].string : _OVAL_FUNCTION_MAP[type - OVAL_FUNCTION - 1].string;

	char *content;
	if (type == OVAL_COMPONENT_LITERAL) {
		struct oval_value *value = oval_component_get_literal_value(component);
		content = oval_value_get_text(value);
	} else {
		content = NULL;
	}

	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *component_node = xmlNewTextChild(parent, ns_definitions,
					      BAD_CAST local_name,
					      BAD_CAST content);

	switch (oval_component_get_type(component)) {
	case OVAL_COMPONENT_LITERAL:{
			struct oval_value *value = oval_component_get_literal_value(component);
			oval_datatype_t datatype = oval_value_get_datatype(value);
			if (datatype != OVAL_DATATYPE_STRING)
				xmlNewProp
				    (component_node, BAD_CAST "datatype", BAD_CAST oval_datatype_get_text(datatype));
		}
		break;
	case OVAL_COMPONENT_OBJECTREF:{
			char *item_field, *record_field;
			struct oval_object *object = oval_component_get_object(component);
			char *object_ref = oval_object_get_id(object);
			xmlNewProp(component_node, BAD_CAST "object_ref", BAD_CAST object_ref);
			item_field = oval_component_get_item_field(component);
			xmlNewProp(component_node, BAD_CAST "item_field", BAD_CAST item_field);
			record_field = oval_component_get_record_field(component);
			if (record_field)
				xmlNewProp(component_node, BAD_CAST "record_field", BAD_CAST record_field);
		} break;
	case OVAL_COMPONENT_VARREF:{
			struct oval_variable *variable = oval_component_get_variable(component);
			char *var_ref = oval_variable_get_id(variable);
			xmlNewProp(component_node, BAD_CAST "var_ref", BAD_CAST var_ref);

		} break;
	case OVAL_FUNCTION_ARITHMETIC:{
			oval_arithmetic_operation_t operation = oval_component_get_arithmetic_operation(component);
			xmlNewProp
			    (component_node,
			     BAD_CAST "arithmetic_operation", BAD_CAST oval_arithmetic_operation_get_text(operation));
		}
		break;
	case OVAL_FUNCTION_BEGIN:{
			char *character = oval_component_get_prefix(component);
			xmlNewProp(component_node, BAD_CAST "character", BAD_CAST character);
		} break;
	case OVAL_FUNCTION_END:{
			char *character = oval_component_get_suffix(component);
			xmlNewProp(component_node, BAD_CAST "character", BAD_CAST character);
		} break;
	case OVAL_FUNCTION_SUBSTRING:{
			int start = oval_component_get_substring_start(component);
			char substring_start[10];
			*substring_start = '\0';
			snprintf(substring_start, sizeof(substring_start), "%d", start);
			xmlNewProp(component_node, BAD_CAST "substring_start", BAD_CAST substring_start);
			int length = oval_component_get_substring_length(component);
			char substring_length[10];
			*substring_length = '\0';
			snprintf(substring_length, sizeof(substring_length), "%d", length);
			xmlNewProp(component_node, BAD_CAST "substring_length", BAD_CAST substring_length);
		}
		break;
	case OVAL_FUNCTION_TIMEDIF:{
			oval_datetime_format_t format_1 = oval_component_get_timedif_format_1(component);
			if (format_1 != OVAL_DATETIME_YEAR_MONTH_DAY) {
				xmlNewProp
				    (component_node,
				     BAD_CAST "format_1", BAD_CAST oval_datetime_format_get_text(format_1));
			}
			oval_datetime_format_t format_2 = oval_component_get_timedif_format_2(component);
			if (format_2 != OVAL_DATETIME_YEAR_MONTH_DAY) {
				xmlNewProp
				    (component_node,
				     BAD_CAST "format_2", BAD_CAST oval_datetime_format_get_text(format_2));
			}
		}
		break;
	case OVAL_FUNCTION_REGEX_CAPTURE:{
			char *pattern = oval_component_get_regex_pattern(component);
			xmlNewProp(component_node, BAD_CAST "pattern", BAD_CAST pattern);
		} break;
	case OVAL_FUNCTION_SPLIT:{
			char * delimiter = oval_component_get_split_delimiter(component);
			xmlNewProp(component_node, BAD_CAST "delimiter", BAD_CAST delimiter);
		} break;
	case OVAL_FUNCTION_GLOB_TO_REGEX:{
			bool glob_noescape = oval_component_get_glob_to_regex_glob_noescape(component);
			if (glob_noescape) {
				xmlNewProp(component_node, BAD_CAST "glob_noescape", BAD_CAST "true");
			} else {
				xmlNewProp(component_node, BAD_CAST "glob_noescape", BAD_CAST "false");
			}
		} break;
	case OVAL_FUNCTION_CONCAT:
	case OVAL_FUNCTION_ESCAPE_REGEX:
		break;
	default:
		break;
	}

	if (type > OVAL_FUNCTION) {
		struct oval_component_iterator *components = oval_component_get_function_components(component);
		while (oval_component_iterator_has_more(components)) {
			struct oval_component *sub_component = oval_component_iterator_next(components);
			oval_component_to_dom(sub_component, doc, component_node);
		}
		oval_component_iterator_free(components);
	}

	return component_node;
}

static oval_syschar_collection_flag_t oval_component_eval_common(oval_argu_t *argu,
								 struct oval_component *component,
								 struct oval_collection *value_collection);

static oval_syschar_collection_flag_t _oval_component_evaluate_LITERAL(oval_argu_t *argu,
								       struct oval_component *component,
								       struct oval_collection *value_collection)
{
	__attribute__nonnull__(component);

	struct oval_component_LITERAL *literal = (struct oval_component_LITERAL *)component;
	oval_collection_add(value_collection, oval_value_clone(literal->value));
	return SYSCHAR_FLAG_COMPLETE;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_OBJECTREF(oval_argu_t *argu,
									 struct oval_component *component,
									 struct oval_collection *value_collection)
{
	__attribute__nonnull__(component);

	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_ERROR;

	struct oval_component_OBJECTREF *objref = (struct oval_component_OBJECTREF *)component;
	struct oval_object *object = objref->object;
	struct oval_syschar *syschar;

	if (!object)
		return flag;

	const char *obj_id = oval_object_get_id(object);
	dI("Variable component references to object '%s'.", obj_id);

	if (argu->mode == OVAL_MODE_QUERY) {
#if defined(OVAL_PROBES_ENABLED)
		if (oval_probe_query_object(argu->u.sess, object, 0, &syschar) != 0)
			return flag;
#else
		return SYSCHAR_FLAG_ERROR;
#endif
	} else {
		syschar = oval_syschar_model_get_syschar(argu->u.sysmod, obj_id);
	}

	if (syschar) {
		char *ifield_name, *rfield_name;

		flag = oval_syschar_get_flag(syschar);
		ifield_name = objref->item_field;
		rfield_name = objref->record_field;
		struct oval_sysitem_iterator *sysitems = oval_syschar_get_sysitem(syschar);
		while (oval_sysitem_iterator_has_more(sysitems)) {
			struct oval_sysitem *sysitem = oval_sysitem_iterator_next(sysitems);
			struct oval_sysent_iterator *sysent_itr = oval_sysitem_get_sysents(sysitem);
			const char *oval_sysitem_id = oval_sysitem_get_id(sysitem);
			const char *oval_sysitem_subtype = oval_subtype_to_str(oval_sysitem_get_subtype(sysitem));
			bool entity_matched = false;
			while (oval_sysent_iterator_has_more(sysent_itr)) {
				oval_datatype_t dt;
				struct oval_sysent *sysent = oval_sysent_iterator_next(sysent_itr);
				char *sysent_name = oval_sysent_get_name(sysent);

				if (strcmp(ifield_name, sysent_name))
					continue;

				entity_matched = true;
				dt = oval_sysent_get_datatype(sysent);
				if (dt == OVAL_DATATYPE_RECORD && rfield_name == NULL) {
					oscap_seterr(OSCAP_EFAMILY_OVAL,
							"Unexpected record data type in %s_item (id: %s) specified by object '%s'.",
							oval_sysitem_subtype, oval_sysitem_id, obj_id);
					oval_sysent_iterator_free(sysent_itr);
					oval_sysitem_iterator_free(sysitems);
					return SYSCHAR_FLAG_ERROR;
				}
				if (dt != OVAL_DATATYPE_RECORD && rfield_name != NULL) {
					oscap_seterr(OSCAP_EFAMILY_OVAL,
							"Expected record data type, but found %s data type in %s entity in %s_item (id: %s) specified by object '%s'.",
							oval_datatype_get_text(dt), ifield_name, oval_sysitem_subtype, oval_sysitem_id, obj_id);
					oval_sysent_iterator_free(sysent_itr);
					oval_sysitem_iterator_free(sysitems);
					return SYSCHAR_FLAG_ERROR;
				}

				if (dt == OVAL_DATATYPE_RECORD) {
					struct oval_record_field_iterator *rf_itr;
					bool field_matched = false;

					rf_itr = oval_sysent_get_record_fields(sysent);
					while (oval_record_field_iterator_has_more(rf_itr)) {
						struct oval_record_field *rf;
						char *txtval;
						struct oval_value *val;

						rf = oval_record_field_iterator_next(rf_itr);
						txtval = oval_record_field_get_name(rf);
						if (strcmp(rfield_name, txtval))
							continue;

						field_matched = true;
						dt = oval_record_field_get_datatype(rf);
						txtval = oval_record_field_get_value(rf);
						val = oval_value_new(dt, txtval);
						oval_collection_add(value_collection, val);
					}
					/* throw error if none matched */
					if (!field_matched) {
						oscap_seterr(OSCAP_EFAMILY_OVAL,
								"Record field '%s' has not been found in %s_item (id: %s) specified by object '%s'.",
								rfield_name, oval_sysitem_subtype, oval_sysitem_id, obj_id);
						oval_record_field_iterator_free(rf_itr);
						oval_sysent_iterator_free(sysent_itr);
						oval_sysitem_iterator_free(sysitems);
						return SYSCHAR_FLAG_ERROR;
					}
					oval_record_field_iterator_free(rf_itr);
				} else {
					char *txtval;
					struct oval_value *val;

					txtval = oval_sysent_get_value(sysent);
					val = oval_value_new(dt, txtval);
					oval_collection_add(value_collection, val);
				}
			}
			/* throw error if none matched */
			if (!entity_matched) {
				oscap_seterr(OSCAP_EFAMILY_OVAL,
						"Entity '%s' has not been found in %s_item (id: %s) specified by object '%s'.",
						ifield_name, oval_sysitem_subtype, oval_sysitem_id, obj_id);
				oval_sysent_iterator_free(sysent_itr);
				oval_sysitem_iterator_free(sysitems);
				return SYSCHAR_FLAG_ERROR;
			}
			oval_sysent_iterator_free(sysent_itr);
		}
		oval_sysitem_iterator_free(sysitems);
	}

	return flag;
}

#define _HAS_VALUES(flag) (flag==SYSCHAR_FLAG_COMPLETE || flag==SYSCHAR_FLAG_INCOMPLETE)
static oval_syschar_collection_flag_t _oval_component_evaluate_VARREF(oval_argu_t *argu,
								      struct oval_component *component,
								      struct oval_collection *value_collection)
{
	__attribute__nonnull__(component);

	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_ERROR;
	struct oval_component_VARREF *varref = (struct oval_component_VARREF *)component;
	struct oval_variable *variable = varref->variable;

	if (!variable) {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "No variable bound to VARREF componenet");
		return flag;
	}

	dIndent(1);
	dI("Variable component references %s '%s'",
		oval_variable_type_get_text(oval_variable_get_type(variable)),
		oval_variable_get_id(variable));

	if (argu->mode == OVAL_MODE_QUERY) {
#if defined(OVAL_PROBES_ENABLED)
		if (oval_probe_query_variable(argu->u.sess, variable) != 0) {
			dIndent(-1);
			return flag;
		}
#else
		return SYSCHAR_FLAG_ERROR;
#endif
	} else {
		if (oval_syschar_model_compute_variable(argu->u.sysmod, variable) != 0) {
			dIndent(-1);
			return flag;
		}
	}

	flag = oval_variable_get_collection_flag(variable);
	if (_HAS_VALUES(flag)) {
		struct oval_value_iterator *values = oval_variable_get_values(variable);
		while (oval_value_iterator_has_more(values)) {
			struct oval_value *value = oval_value_iterator_next(values);
			oval_collection_add(value_collection, oval_value_clone(value));
		}
		oval_value_iterator_free(values);
	}

	dIndent(-1);
	return flag;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_BEGIN(oval_argu_t *argu,
								     struct oval_component *component,
								     struct oval_collection *value_collection)
{
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_ERROR;
	char *prefix = oval_component_get_prefix(component);
	if (prefix) {
		int len_prefix = strlen(prefix);
		struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
		if (oval_component_iterator_has_more(subcomps)) {	/*only the first component is processed */
			struct oval_collection *subcoll = oval_collection_new();
			struct oval_component *subcomp = oval_component_iterator_next(subcomps);
			flag = oval_component_eval_common(argu, subcomp, subcoll);
			struct oval_value_iterator *values =
			    (struct oval_value_iterator *)oval_collection_iterator(subcoll);
			while (oval_value_iterator_has_more(values)) {
				char *key = oval_value_get_text(oval_value_iterator_next(values));
				const size_t concat_len = len_prefix + strlen(key) + 1;
				char *concat = malloc(concat_len);
				if (strncmp(prefix, key, len_prefix)) {
					snprintf(concat, concat_len, "%s%s", prefix, key);
				} else {
					snprintf(concat, concat_len, "%s", key);
				}
				struct oval_value *concat_value = oval_value_new(OVAL_DATATYPE_STRING, concat);
				free(concat);
				oval_collection_add(value_collection, concat_value);
			}
			oval_value_iterator_free(values);
			oval_collection_free_items(subcoll, (oscap_destruct_func) oval_value_free);
		}
		oval_component_iterator_free(subcomps);
	} else {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "No prefix specified for component evaluation");
	}
	return flag;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_END(oval_argu_t *argu,
								   struct oval_component *component,
								   struct oval_collection *value_collection) {
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_ERROR;
	char *suffix = oval_component_get_suffix(component);
	if (suffix) {
		int len_suffix = strlen(suffix);
		struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
		if (oval_component_iterator_has_more(subcomps)) {	/*only the first component is processed */
			struct oval_collection *subcoll = oval_collection_new();
			struct oval_component *subcomp = oval_component_iterator_next(subcomps);
			flag = oval_component_eval_common(argu, subcomp, subcoll);
			struct oval_value_iterator *values =
			    (struct oval_value_iterator *)oval_collection_iterator(subcoll);
			while (oval_value_iterator_has_more(values)) {
				char *key = oval_value_get_text(oval_value_iterator_next(values));
				int len_key = strlen(key);
				size_t concat_len = len_suffix + len_key + 1;
				char *concat = malloc(concat_len);
				if ((len_suffix > len_key) || strncmp(suffix, key + len_key - len_suffix, len_suffix)) {
					snprintf(concat, concat_len, "%s%s", key, suffix);
				} else {
					snprintf(concat, concat_len, "%s", key);
				}
				struct oval_value *concat_value = oval_value_new(OVAL_DATATYPE_STRING, concat);
				free(concat);
				oval_collection_add(value_collection, concat_value);
			}
			oval_value_iterator_free(values);
			oval_collection_free_items(subcoll, (oscap_destruct_func) oval_value_free);
		}
		oval_component_iterator_free(subcomps);
	} else {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "No suffix specified for component evaluation");
	}
	return flag;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_CONCAT(oval_argu_t *argu,
								      struct oval_component *component,
								      struct oval_collection *value_collection)
{
	int idx0 = 0;

	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
	int len_subcomps = oval_component_iterator_remaining(subcomps);
	struct oval_collection **component_colls = malloc(len_subcomps * sizeof(struct oval_collection *));
	for (idx0 = 0; oval_component_iterator_has_more(subcomps); idx0++) {
		struct oval_collection *subcoll = oval_collection_new();
		struct oval_component *subcomp = oval_component_iterator_next(subcomps);
		oval_syschar_collection_flag_t subflag = oval_component_eval_common(argu, subcomp, subcoll);
		flag = _AGG_FLAG(flag, subflag);
		component_colls[idx0] = subcoll;
	}
	bool not_finished = (len_subcomps > 0) && _HAS_VALUES(flag);
	if (not_finished) {
		struct oval_value_iterator **values = malloc(len_subcomps * sizeof(struct oval_value_iterator *));
		/* bool has_some[len_subcomps]; <-- unused variable */
		not_finished = false;
		char **texts = malloc(len_subcomps * sizeof(char *));
		int *counts = malloc(len_subcomps * sizeof(int));
		int catnum = 1;
		for (idx0 = 0; idx0 < len_subcomps; idx0++) {
			struct oval_value_iterator *comp_values =
			    (struct oval_value_iterator *)oval_collection_iterator(component_colls[idx0]);
			counts[idx0] = oval_value_iterator_remaining(comp_values);
			if (counts[idx0]) {
				/* int dbgnum = catnum; <-- unused variable */
				catnum = catnum * counts[idx0];
				values[idx0] = comp_values;
				texts[idx0] = oval_value_get_text(oval_value_iterator_next(comp_values));
				not_finished = true;
			} else {
				oval_collection_free_items(component_colls[idx0],
							   (oscap_destruct_func) oval_value_free);
				component_colls[idx0] = NULL;
				values[idx0] = NULL;
				texts[idx0] = NULL;
			}
		}
		for (int passnum = 1; passnum - 1 < catnum; passnum++) {
			int len_cat = 1;
			for (idx0 = 0; idx0 < len_subcomps; idx0++)
				if (texts[idx0])
					len_cat += strlen(texts[idx0]);
			char *concat = malloc(len_cat);
			*concat = '\0';
			for (idx0 = 0; idx0 < len_subcomps; idx0++)
				if (texts[idx0])
					strcat(concat, texts[idx0]);
			struct oval_value *value = oval_value_new(OVAL_DATATYPE_STRING, concat);
			free(concat);
			oval_collection_add(value_collection, value);
			bool rotate = true;
			if (passnum < catnum) {
				for (idx0 = 0; idx0 < len_subcomps && rotate; idx0++) {
					if (texts[idx0]) {
						if (oval_value_iterator_has_more(values[idx0])) {
							rotate = false;
						} else {
							oval_value_iterator_free(values[idx0]);
							values[idx0] =
							    (struct oval_value_iterator *)
							    oval_collection_iterator(component_colls[idx0]);
						}
						texts[idx0] =
						    oval_value_get_text(oval_value_iterator_next(values[idx0]));
					}
				}
			}
		}
		for (idx0 = 0; idx0 < len_subcomps; idx0++) {
			if (counts[idx0]) {
				oval_value_iterator_free(values[idx0]);
				oval_collection_free_items(component_colls[idx0],
							   (oscap_destruct_func) oval_value_free);
			}
		}
		free(counts);
		free(texts);
		free(values);
	}
	free(component_colls);
	oval_component_iterator_free(subcomps);
	return flag;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_COUNT(oval_argu_t *argu,
								      struct oval_component *component,
								      struct oval_collection *value_collection)
{
	int count = 0;
	int idx0 = 0;

	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
	int len_subcomps = oval_component_iterator_remaining(subcomps);
	struct oval_collection **component_colls = malloc(len_subcomps * sizeof(struct oval_collection *));
	for (idx0 = 0; oval_component_iterator_has_more(subcomps); idx0++) {
		struct oval_collection *subcoll = oval_collection_new();
		struct oval_component *subcomp = oval_component_iterator_next(subcomps);
		oval_syschar_collection_flag_t subflag = oval_component_eval_common(argu, subcomp, subcoll);
		flag = _AGG_FLAG(flag, subflag);
		component_colls[idx0] = subcoll;
	}
	bool not_finished = (len_subcomps > 0) && _HAS_VALUES(flag);
	if (not_finished) {
		for (idx0 = 0; idx0 < len_subcomps; idx0++) {
			struct oval_value_iterator *comp_values =
			    (struct oval_value_iterator *)oval_collection_iterator(component_colls[idx0]);
			while (oval_value_iterator_has_more(comp_values)) {
				if (oval_value_get_text(oval_value_iterator_next(comp_values)))
					count++;
			}
			oval_value_iterator_free(comp_values);

		}
	}
	char count_str[128];
	snprintf(count_str, sizeof(count_str), "%d", count);
	struct oval_value *value = oval_value_new(OVAL_DATATYPE_INTEGER, count_str);
	oval_collection_add(value_collection, value);

	oval_component_iterator_free(subcomps);

	for (idx0 = 0; idx0 < len_subcomps; ++idx0)
	  oval_collection_free_items(component_colls[idx0], (oscap_destruct_func) oval_value_free);

	free(component_colls);
	return flag;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_UNIQUE(oval_argu_t *argu,
								      struct oval_component *component,
								      struct oval_collection *value_collection)
{
	int idx0 = 0;
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
	int len_subcomps = oval_component_iterator_remaining(subcomps);
	struct oval_collection **component_colls = malloc(len_subcomps * sizeof(struct oval_collection *));

	for (idx0 = 0; oval_component_iterator_has_more(subcomps); idx0++) {
		struct oval_collection *subcoll = oval_collection_new();
		struct oval_component *subcomp = oval_component_iterator_next(subcomps);
		oval_syschar_collection_flag_t subflag = oval_component_eval_common(argu, subcomp, subcoll);
		flag = _AGG_FLAG(flag, subflag);
		component_colls[idx0] = subcoll;
	}

	oval_component_iterator_free(subcomps);

	bool not_finished = (len_subcomps > 0) && _HAS_VALUES(flag);
	struct oval_string_map *valmap = oval_string_map_new();

	if (not_finished) {
		for (idx0 = 0; idx0 < len_subcomps; idx0++) {
			struct oval_value_iterator *comp_values =
			    (struct oval_value_iterator *)oval_collection_iterator(component_colls[idx0]);

			while (oval_value_iterator_has_more(comp_values)) {
				char *valtxt;
				if ((valtxt = oval_value_get_text(oval_value_iterator_next(comp_values))) != NULL) {
				  if (oval_string_map_get_value(valmap, valtxt) == NULL) {
					struct oval_value *value = oval_value_new(OVAL_DATATYPE_STRING, valtxt);
					oval_string_map_put(valmap, valtxt, value);
				  }
				}
			}
			oval_value_iterator_free(comp_values);
		}
	}

	oval_string_map_collect_values(valmap, value_collection);
	oval_string_map_free(valmap, NULL);

	for (idx0 = 0; idx0 < len_subcomps; ++idx0)
	  oval_collection_free_items(component_colls[idx0], (oscap_destruct_func) oval_value_free);

	free(component_colls);
	return flag;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_SPLIT(oval_argu_t *argu,
								     struct oval_component *component,
								     struct oval_collection *value_collection)
{

	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
	char *delimiter = oval_component_get_split_delimiter(component);
	int len_delim = strlen(delimiter);
	if (oval_component_iterator_has_more(subcomps)) {	/* Only first component is considered */
		struct oval_component *subcomp = oval_component_iterator_next(subcomps);
		struct oval_collection *subcoll = oval_collection_new();
		flag = oval_component_eval_common(argu, subcomp, subcoll);
		struct oval_value_iterator *values = (struct oval_value_iterator *)oval_collection_iterator(subcoll);
		struct oval_value *value;
		while (oval_value_iterator_has_more(values)) {
			char *text = oval_value_get_text(oval_value_iterator_next(values));
			if (len_delim) {
				char *split = malloc(strlen(text) + 2);
				char *split0 = split;
				*split0 = '\0';
				strcat(split0, text);
				split0[strlen(text) + 1] = '\0';	/*last two characters are EOS */
				char *split1;
				for (split1 = strstr(split0, delimiter); split1; split1 = strstr(split0, delimiter)) {
					*split1 = '\0';	/*terminate the text at the delimeter */
					value = oval_value_new(OVAL_DATATYPE_STRING, split0);
					oval_collection_add(value_collection, value);
					split0 = split1 + len_delim;	/*advance split1 */
				}
				value = oval_value_new(OVAL_DATATYPE_STRING, split0);
				oval_collection_add(value_collection, value);
				free(split);
			} else {	/*Empty delimiter, Split at every character */
				char split[] = { '\0', '\0' };
				int idx;
				for (idx = 0; text[idx]; idx++) {
					*split = text[idx];
					value = oval_value_new(OVAL_DATATYPE_STRING, split);
					oval_collection_add(value_collection, value);
				}
			}
		}
		oval_value_iterator_free(values);
		oval_collection_free_items(subcoll, (oscap_destruct_func) oval_value_free);
	}
	oval_component_iterator_free(subcomps);
	return flag;
};

static oval_syschar_collection_flag_t _oval_component_evaluate_SUBSTRING(oval_argu_t *argu,
									 struct oval_component *component,
									 struct oval_collection *value_collection)
{
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
        ssize_t beg = oval_component_get_substring_start(component);
	ssize_t len = oval_component_get_substring_length(component);

	beg = (beg < 1) ? 0 : beg - 1;

	if (oval_component_iterator_has_more(subcomps)) {	/*Only first component is considered */
		struct oval_component *subcomp = oval_component_iterator_next(subcomps);
		struct oval_collection *subcoll = oval_collection_new();
		flag = oval_component_eval_common(argu, subcomp, subcoll);
		struct oval_value_iterator *values = (struct oval_value_iterator *)oval_collection_iterator(subcoll);
		struct oval_value *value;

		while (oval_value_iterator_has_more(values)) {
			char *text = oval_value_get_text(oval_value_iterator_next(values));
			size_t txtlen, sublen;

			txtlen = strlen(text);
			sublen = (len < 0 || (size_t) len > txtlen) ? txtlen : (size_t) len;
			if ((size_t) beg < txtlen) {
				char *substr = malloc(sublen + 1);

				strncpy(substr, text + beg, sublen);
				substr[sublen] = '\0';

				value = oval_value_new(OVAL_DATATYPE_STRING, substr);
				free(substr);
				oval_collection_add(value_collection, value);
			} else {
				flag = SYSCHAR_FLAG_ERROR;
			}
		}
		oval_value_iterator_free(values);
		oval_collection_free_items(subcoll, (oscap_destruct_func) oval_value_free);
	}
	oval_component_iterator_free(subcomps);
	return flag;
}

static long unsigned int _comp_sec(int year, int month, int day, int hour, int minute, int second)
{
	time_t t;
	struct tm *ts;

	t = time(NULL);
	ts = localtime(&t);

	ts->tm_year = year - 1900;
	ts->tm_mon = month - 1;
	ts->tm_mday = day;
	ts->tm_hour = hour;
	ts->tm_min = minute;
	ts->tm_sec = second;
	ts->tm_isdst = -1;
	t = mktime(ts);
	ts = localtime(&t);

	if (ts->tm_isdst == 1)
		t -= 3600;

	return (long unsigned int) t;
}

/*
 * year_month_day
 * The year_month_day value specifies date-time strings that follow the formats:
 *
 * strptime(3)
 */
static const char *_dtfmt_ymd[] = {
        "%Y%m%d",        /* yyyymmdd */
        "%Y/%m/%d",      /* yyyy/mm/dd */
        "%Y-%m-%d",      /* yyyy-mm-dd */
        "%Y%m%dT%H%M%S", /* yyyymmddThhmmss */
        "%Y/%m/%d%n%T",  /* yyyy/mm/dd hh:mm:ss */
        "%Y-%m-%d%n%T"   /* yyyy-mm-dd hh:mm:ss */
};

/*
 * month_day_year
 * The month_day_year value specifies date-time strings that follow the formats:
 *
 * strptime(3)
 */
static const char *_dtfmt_mdy[] = {
        "%m/%d/%Y",       /* mm/dd/yyyy */
        "%m-%d-%Y",       /* mm-dd-yyyy */
        "%b,%n%d%n%Y",    /* AbreviatedNameOfMonth, dd yyyy */
        "%m/%d/%Y%n%T",   /* mm/dd/yyyy hh:mm:ss */
        "%m-%d-%Y%n%T",   /* mm-dd-yyyy hh:mm:ss */
        "%b,%n%d%n%Y%n%T" /* NameOfMonth, dd yyyy hh:mm:ss & AbreviatedNameOfMonth, dd yyyy hh:mm:ss */
};

/*
 * day_month_year
 * The day_month_year value specifies date-time strings that follow the formats:
 *
 * strptime(3)
 */
static const char *_dtfmt_dmy[] = {
        "%d/%m/%Y",     /* dd/mm/yyyy */
        "%d-%m-%Y",     /* dd-mm-yyyy */
        "%d/%m/%Y%n%T", /* dd/mm/yyyy hh:mm:ss */
        "%d-%m-%Y%n%T"  /* dd-mm-yyyy hh:mm:ss */
};

static long unsigned int _parse_datetime(char *datetime, const char *fmt[], size_t fmtcnt)
{
        struct tm t;
        size_t    i;
        char     *r;

        dI("Parsing datetime string \"%s\"", datetime);

        for (i = 0; i < fmtcnt; ++i) {
                dI("%s", fmt[i]);
                memset(&t, 0, sizeof t);
                r = strptime(datetime, fmt[i], &t);

                if (r != NULL) {
                        if (*r == '\0') {
                                dI("Success!");
                                return _comp_sec(t.tm_year, t.tm_mon, t.tm_mday,
                                                 t.tm_hour, t.tm_min, t.tm_sec);
                        }
                }
        }

        dE("Unable to interpret \"%s\" as a datetime string");

        return (0);
}

static long unsigned int _parse_fmt_win(char *dt)
{
	// todo
	return 0;
}

static long unsigned int _parse_fmt_sse(char *dt)
{
	time_t t;
	struct tm *ts;

	t = (time_t) atol(dt);
	ts = localtime(&t);
	if (ts->tm_isdst == 1)
		t -= 3600;

	return (long unsigned int) t;
}

static bool _match(const char *pattern, const char *string)
{
	bool match = false;
	pcre *re;
	const char *error;
	int erroffset = -1, ovector[60], ovector_len = sizeof (ovector) / sizeof (ovector[0]);
	re = pcre_compile(pattern, PCRE_UTF8, &error, &erroffset, NULL);
	match = (pcre_exec(re, NULL, string, strlen(string), 0, 0, ovector, ovector_len) >= 0);
	pcre_free(re);
	return match;
}

static long unsigned int _parse_fmt_cim(char *dt)
{
	const char *pattern1 = "^[0-9]{14}\\.[0-9]{6}[+-][0-9]{3}$"; // yyyymmddHHMMSS.mmmmmmsUUU
	const char *fmt_str1 = "%4u%2u%2u%2u%2u%2u.%*6u%*[+-]%*3u";
	const char *pattern2 = "^[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}:[0-9]{3}$"; // yyyy-mm-dd HH:MM:SS:mmm
	const char *fmt_str2 = "%4d-%2d-%2d %2d:%2d:%2d:%*3d";
	const char *pattern3 = "^[0-9]{2}-[0-9]{2}-[0-9]{4} [0-9]{2}:[0-9]{2}:[0-9]{2}:[0-9]{3}$"; // mm-dd-yyyy hh:mm:ss:mmm
	const char *fmt_str3 = "%2d-%2d-%4d %2d:%2d:%2d:%*3d";
	int year, month, day, hour, minute, second, r = 0;
	const int number_of_items = 6;
	if (_match(pattern1, dt)) {
		r = sscanf(dt, fmt_str1, &year, &month, &day, &hour, &minute, &second);
	} else if (_match(pattern2, dt)) {
		r = sscanf(dt, fmt_str2, &year, &month, &day, &hour, &minute, &second);
	} else if (_match(pattern3, dt)) {
		r = sscanf(dt, fmt_str3, &month, &day, &year, &hour, &minute, &second);
	}
	if (r != number_of_items) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unable to interpret \"%s\" as a cim_datetime string\n", dt);
		return 0;
	}
	return _comp_sec(year, month, day, hour, minute, second);
}

static long unsigned int _parse_fmt(struct oval_value *val, oval_datetime_format_t fmt)
{
	long unsigned int v = 0;
	char *sv;

	sv = oval_value_get_text(val);

	switch (fmt) {
	case OVAL_DATETIME_YEAR_MONTH_DAY:
                v = _parse_datetime(sv, _dtfmt_ymd, sizeof _dtfmt_ymd/sizeof(char *));
		break;
	case OVAL_DATETIME_MONTH_DAY_YEAR:
                v = _parse_datetime(sv, _dtfmt_mdy, sizeof _dtfmt_mdy/sizeof(char *));
		break;
	case OVAL_DATETIME_DAY_MONTH_YEAR:
                v = _parse_datetime(sv, _dtfmt_dmy, sizeof _dtfmt_dmy/sizeof(char *));
		break;
	case OVAL_DATETIME_WIN_FILETIME:
		v = _parse_fmt_win(sv);
		break;
	case OVAL_DATETIME_SECONDS_SINCE_EPOCH:
		v = _parse_fmt_sse(sv);
		break;
	case OVAL_DATETIME_CIM_DATETIME:
		v = _parse_fmt_cim(sv);
		break;
	default:
		break;
	}

	return v;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_TIMEDIF(oval_argu_t *argu,
								       struct oval_component *component,
								       struct oval_collection *value_collection)
{
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	struct oval_component_iterator *subcomps;
	int subcomp_idx;
	struct oval_collection *val_cols[2];
	struct oval_value_iterator *v1_itr;
	oval_datetime_format_t fmt1, fmt2;

	subcomps = oval_component_get_function_components(component);
	subcomp_idx = 0;
	while (oval_component_iterator_has_more(subcomps) && subcomp_idx < 2) {
		struct oval_component *subcomp;

		subcomp = oval_component_iterator_next(subcomps);
		val_cols[subcomp_idx] = oval_collection_new();
		flag = oval_component_eval_common(argu, subcomp, val_cols[subcomp_idx]);
		subcomp_idx++;
	}

	if (oval_component_iterator_has_more(subcomps)) {
		oval_component_iterator_free(subcomps);
		oval_collection_free_items(val_cols[0], (oscap_destruct_func) oval_value_free);
		oval_collection_free_items(val_cols[1], (oscap_destruct_func) oval_value_free);
		return SYSCHAR_FLAG_ERROR;
	}
	oval_component_iterator_free(subcomps);
	if (subcomp_idx == 0) {
		return SYSCHAR_FLAG_ERROR;
	}

	fmt1 = oval_component_get_timedif_format_1(component);
	fmt2 = oval_component_get_timedif_format_2(component);

	if (subcomp_idx == 1) {
		struct oval_value *ov;
		char ts[16];

		val_cols[1] = val_cols[0];
		val_cols[0] = oval_collection_new();
		snprintf(ts, sizeof (ts), "%lu", (long unsigned int) time(NULL));
		ov = oval_value_new(OVAL_DATATYPE_INTEGER, ts);
		oval_collection_add(val_cols[0], ov);
		fmt1 = OVAL_DATETIME_SECONDS_SINCE_EPOCH;
	}

	v1_itr = (struct oval_value_iterator *) oval_collection_iterator(val_cols[0]);
	while (oval_value_iterator_has_more(v1_itr)) {
		struct oval_value *ov1;
		struct oval_value_iterator *v2_itr;

		ov1 = oval_value_iterator_next(v1_itr);

		v2_itr = (struct oval_value_iterator *) oval_collection_iterator(val_cols[1]);
		while (oval_value_iterator_has_more(v2_itr)) {
			long unsigned int v;
			char ts[16];
			struct oval_value *ov2, *ov;

			ov2 = oval_value_iterator_next(v2_itr);
			v = _parse_fmt(ov1, fmt1) - _parse_fmt(ov2, fmt2);
			snprintf(ts, sizeof (ts), "%lu", v);
			ov = oval_value_new(OVAL_DATATYPE_INTEGER, ts);
			oval_collection_add(value_collection, ov);
		}
		oval_value_iterator_free(v2_itr);
	}
	oval_value_iterator_free(v1_itr);

	oval_collection_free_items(val_cols[0], (oscap_destruct_func) oval_value_free);
	oval_collection_free_items(val_cols[1], (oscap_destruct_func) oval_value_free);

	return flag;
}

static bool _isEscape(char chr)
{
	const char *regex_chars = "^$\\.[](){}*+?|";

	return (strchr(regex_chars, chr) == NULL) ? false : true;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_ESCAPE_REGEX(oval_argu_t *argu,
									    struct oval_component *component,
									    struct oval_collection *value_collection)
{
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	struct oval_component_iterator *subcomps = oval_component_get_function_components(component);

	if (oval_component_iterator_has_more(subcomps)) {	//Only first component is considered
		struct oval_component *subcomp = oval_component_iterator_next(subcomps);
		struct oval_collection *subcoll = oval_collection_new();
		flag = oval_component_eval_common(argu, subcomp, subcoll);
		struct oval_value_iterator *values = (struct oval_value_iterator *)oval_collection_iterator(subcoll);
		while (oval_value_iterator_has_more(values)) {
			struct oval_value *value = oval_value_iterator_next(values);
			char *text = oval_value_get_text(value);
			int len = strlen(text);
			char *string = malloc(2 * len + 1);
			char *insert = string;
			while (*text) {
				if (_isEscape(*text))
					*insert++ = '\\';
				*insert++ = *text++;
			}
			*insert = '\0';
			value = oval_value_new(OVAL_DATATYPE_STRING, string);
			free(string);
			oval_collection_add(value_collection, value);
		}
		oval_value_iterator_free(values);
		oval_collection_free_items(subcoll, (oscap_destruct_func) oval_value_free);
	}
	oval_component_iterator_free(subcomps);
	return flag;
}





static oval_syschar_collection_flag_t _oval_component_evaluate_GLOB_TO_REGEX(oval_argu_t *argu,
										struct oval_component *component,
										struct oval_collection *value_collection)
{
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	bool glob_noescape = oval_component_get_glob_to_regex_glob_noescape(component);
	struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
	if (oval_component_iterator_has_more(subcomps)) {	//Only first component is considered
		struct oval_component *subcomp = oval_component_iterator_next(subcomps);
		struct oval_collection *subcoll = oval_collection_new();
		flag = oval_component_eval_common(argu, subcomp, subcoll);
		struct oval_value_iterator *values = (struct oval_value_iterator *)oval_collection_iterator(subcoll);
		while (oval_value_iterator_has_more(values)) {
			struct oval_value *value = oval_value_iterator_next(values);
			char *text = oval_value_get_text(value);
			char *string = oval_glob_to_regex(text, glob_noescape);
			if (string == NULL) {
				flag = SYSCHAR_FLAG_ERROR;
				break;
			}
			value = oval_value_new(OVAL_DATATYPE_STRING, string);
			free(string);
			oval_collection_add(value_collection, value);
		}
		oval_value_iterator_free(values);
		oval_collection_free_items(subcoll, (oscap_destruct_func) oval_value_free);
	}
	oval_component_iterator_free(subcomps);
	return flag;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_REGEX_CAPTURE(oval_argu_t *argu,
									     struct oval_component *component,
									     struct oval_collection *value_collection)
{
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	int rc;
	char *pattern;
	int erroffset = -1;
	pcre *re = NULL;
	const char *error;

	pattern = oval_component_get_regex_pattern(component);
	re = pcre_compile(pattern, PCRE_UTF8, &error, &erroffset, NULL);
	if (re == NULL) {
		dE("pcre_compile() failed: \"%s\".", error);
		return SYSCHAR_FLAG_ERROR;
	}

	struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
	if (oval_component_iterator_has_more(subcomps)) {	//Only first component is considered
		struct oval_component *subcomp = oval_component_iterator_next(subcomps);
		struct oval_collection *subcoll = oval_collection_new();
		flag = oval_component_eval_common(argu, subcomp, subcoll);
		struct oval_value_iterator *values = (struct oval_value_iterator *)oval_collection_iterator(subcoll);
		while (oval_value_iterator_has_more(values)) {
			struct oval_value *value = oval_value_iterator_next(values);
			char *text = oval_value_get_text(value);
			char *nval = NULL;
			int i, ovector[60], ovector_len = sizeof (ovector) / sizeof (ovector[0]);

			for (i = 0; i < ovector_len; ++i)
				ovector[i] = -1;

			rc = pcre_exec(re, NULL, text, strlen(text), 0, 0, ovector, ovector_len);
			if (rc < -1) {
				dE("pcre_exec() failed: %d.", rc);
				flag = SYSCHAR_FLAG_ERROR;
				break;
			}

			if (rc > 1 && ovector[2] != -1) {
				int substr_len = ovector[3] - ovector[2];

				nval = malloc(substr_len + 1);
				memcpy(nval, text + ovector[2], substr_len);
				nval[substr_len] = '\0';
			} else {
				nval = NULL;
			}
			flag = SYSCHAR_FLAG_COMPLETE;

			if (nval != NULL) {
				value = oval_value_new(OVAL_DATATYPE_STRING, nval);
				free(nval);
			} else {
				value = oval_value_new(OVAL_DATATYPE_STRING, "");
			}
			oval_collection_add(value_collection, value);
		}
		oval_value_iterator_free(values);
		oval_collection_free_items(subcoll, (oscap_destruct_func) oval_value_free);
	}
	oval_component_iterator_free(subcomps);
	pcre_free(re);
	return flag;
}

struct val_col_lst_s {
	void *val_col;
	struct val_col_lst_s *next;
};

static oval_syschar_collection_flag_t _oval_component_evaluate_ARITHMETIC_rec(struct val_col_lst_s *val_col_lst, double val,
						    oval_datatype_t datatype, oval_arithmetic_operation_t op,
						    struct oval_collection *res_val_col)
{
	struct oval_value_iterator *val_itr;

	if (val_col_lst == NULL) {
		struct oval_value *ov;
		char sv[32];

		if (datatype == OVAL_DATATYPE_INTEGER) {
			snprintf(sv, sizeof (sv), "%ld", (long int) val);
		} else if (datatype == OVAL_DATATYPE_FLOAT) {
			snprintf(sv, sizeof (sv), "%f", val);
		}
		ov = oval_value_new(datatype, sv);
		oval_collection_add(res_val_col, ov);

		return SYSCHAR_FLAG_COMPLETE;
	}

	val_itr = (struct oval_value_iterator *) oval_collection_iterator(val_col_lst->val_col);
	while (oval_value_iterator_has_more(val_itr)) {
		struct oval_value *ov;
		oval_datatype_t dt;
		double new_val;

		ov = oval_value_iterator_next(val_itr);
		dt = oval_value_get_datatype(ov);
		if (dt == OVAL_DATATYPE_STRING) {
			errno = 0; // Setting errno to 0 as suggested by strtod() manpage, as 0 is used both on success and failure
			new_val = strtod(oval_value_get_text(ov), NULL);
			if (errno) {
				oscap_seterr(OSCAP_EFAMILY_OVAL, "Unexpected content: %s.", oval_value_get_text(ov));
				oval_value_iterator_free(val_itr);
				return SYSCHAR_FLAG_ERROR;
			}
		} else if (dt == OVAL_DATATYPE_INTEGER) {
			new_val = (double) oval_value_get_integer(ov);
		} else if (dt == OVAL_DATATYPE_FLOAT) {
			new_val = (double) oval_value_get_float(ov);
		} else {
			oscap_seterr(OSCAP_EFAMILY_OVAL, "Unexpected value type: %s.", oval_datatype_get_text(dt));
			oval_value_iterator_free(val_itr);
			return SYSCHAR_FLAG_ERROR;
		}

		if (op == OVAL_ARITHMETIC_ADD) {
			new_val += val;
		} else if (op == OVAL_ARITHMETIC_MULTIPLY) {
			new_val *= val;
		} else {
			oscap_seterr(OSCAP_EFAMILY_OVAL, "Unexpected arithmetic operation: %s.", oval_arithmetic_operation_get_text(op));
			oval_value_iterator_free(val_itr);
			return SYSCHAR_FLAG_ERROR;
		}

		if (datatype == OVAL_DATATYPE_FLOAT)
			dt = OVAL_DATATYPE_FLOAT;
		_oval_component_evaluate_ARITHMETIC_rec(val_col_lst->next, new_val, dt, op, res_val_col);
	}
	oval_value_iterator_free(val_itr);

	return SYSCHAR_FLAG_COMPLETE;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_ARITHMETIC(oval_argu_t *argu,
									  struct oval_component *component,
									  struct oval_collection *value_collection)
{
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	struct oval_component_iterator *subcomps;
	struct val_col_lst_s *vcl_root, *vcl_elm;
	oval_arithmetic_operation_t op;
	struct oval_value_iterator *val_itr;

	op = oval_component_get_arithmetic_operation(component);
	if (op != OVAL_ARITHMETIC_ADD && op != OVAL_ARITHMETIC_MULTIPLY) {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Unexpected arithmetic operation: %s.", oval_arithmetic_operation_get_text(op));
		return SYSCHAR_FLAG_ERROR;
	}

	vcl_root = NULL;
	subcomps = oval_component_get_function_components(component);
	while (oval_component_iterator_has_more(subcomps)) {
		struct oval_component *subcomp;
		struct oval_collection *val_col;

		subcomp = oval_component_iterator_next(subcomps);
		val_col = oval_collection_new();
		// todo: combine flags
		flag = oval_component_eval_common(argu, subcomp, val_col);
		vcl_elm = malloc(sizeof (struct val_col_lst_s));
		vcl_elm->val_col = val_col;
		vcl_elm->next = vcl_root;
		vcl_root = vcl_elm;
	}
	oval_component_iterator_free(subcomps);

	val_itr = (struct oval_value_iterator *) oval_collection_iterator(vcl_root->val_col);
	while (oval_value_iterator_has_more(val_itr)) {
		struct oval_value *ov;
		oval_datatype_t datatype;
		double val;

		ov = oval_value_iterator_next(val_itr);
		datatype = oval_value_get_datatype(ov);
		if (datatype == OVAL_DATATYPE_STRING) {
			errno = 0; // Setting errno to 0 as suggested by strtod() manpage, as 0 is used both on success and failure
			val = strtod(oval_value_get_text(ov), NULL);
			if (errno) {
				oscap_seterr(OSCAP_EFAMILY_OVAL, "Unexpected content: %s.", oval_value_get_text(ov));
				flag = SYSCHAR_FLAG_ERROR;
				goto cleanup;
			}
		} else if (datatype == OVAL_DATATYPE_INTEGER) {
			val = (double) oval_value_get_integer(ov);
		} else if (datatype == OVAL_DATATYPE_FLOAT) {
			val = (double) oval_value_get_float(ov);
		} else {
			oscap_seterr(OSCAP_EFAMILY_OVAL, "Unexpected value type: %s.", oval_datatype_get_text(datatype));
			flag = SYSCHAR_FLAG_ERROR;
			goto cleanup;
		}

		flag = _oval_component_evaluate_ARITHMETIC_rec(vcl_root->next, val, datatype, op, value_collection);
	}

 cleanup:
	oval_value_iterator_free(val_itr);
	while (vcl_root != NULL) {
		oval_collection_free_items(vcl_root->val_col, (oscap_destruct_func) oval_value_free);
		vcl_elm = vcl_root;
		vcl_root = vcl_root->next;
		free(vcl_elm);
	}

	return flag;
}

typedef oval_syschar_collection_flag_t(_oval_component_evaluator)
 (oval_argu_t *, struct oval_component *, struct oval_collection *);

static _oval_component_evaluator *_component_evaluators[] = {
	NULL,
	_oval_component_evaluate_LITERAL,
	_oval_component_evaluate_OBJECTREF,
	_oval_component_evaluate_VARREF,
	NULL,
	_oval_component_evaluate_BEGIN,
	_oval_component_evaluate_CONCAT,
	_oval_component_evaluate_END,
	_oval_component_evaluate_SPLIT,
	_oval_component_evaluate_SUBSTRING,
	_oval_component_evaluate_TIMEDIF,
	_oval_component_evaluate_ESCAPE_REGEX,
	_oval_component_evaluate_REGEX_CAPTURE,
	_oval_component_evaluate_ARITHMETIC,
	_oval_component_evaluate_COUNT,
	_oval_component_evaluate_UNIQUE,
	_oval_component_evaluate_GLOB_TO_REGEX,
	NULL,
};

static oval_syschar_collection_flag_t oval_component_eval_common(oval_argu_t *argu,
								 struct oval_component *component,
								 struct oval_collection *value_collection)
{
	__attribute__nonnull__(component);

	oval_component_type_t type = component->type;
	int evidx = (type > OVAL_FUNCTION) ? type - OVAL_FUNCTION + OVAL_COMPONENT_FUNCTION : type;
	_oval_component_evaluator *evaluator = (OVAL_COMPONENT_LITERAL <= type && type <= OVAL_FUNCTION_LAST)
	    ? _component_evaluators[evidx] : NULL;
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_ERROR;
	if (evaluator) {
		flag = (*evaluator) (argu, component, value_collection);
	} else {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Component type %d not supported.", type);
	}
	return flag;
}

oval_syschar_collection_flag_t oval_component_compute(struct oval_syschar_model *sysmod,
						      struct oval_component *component,
						      struct oval_collection *value_collection)
{
	oval_argu_t argu;

	argu.mode = OVAL_MODE_COMPUTE;
	argu.u.sysmod = sysmod;

	return oval_component_eval_common(&argu, component, value_collection);
}
#if defined(OVAL_PROBES_ENABLED)
oval_syschar_collection_flag_t oval_component_query(oval_probe_session_t *sess,
						    struct oval_component *component,
						    struct oval_collection *value_collection)
{
	oval_argu_t argu;

	argu.mode = OVAL_MODE_QUERY;
	argu.u.sess = sess;

	return oval_component_eval_common(&argu, component, value_collection);
}
#endif /* OVAL_PROBES_ENABLED */
