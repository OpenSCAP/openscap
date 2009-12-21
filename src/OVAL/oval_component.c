/**
 * @file oval_component.c
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
#include "oval_parser_impl.h"
#include "../common/util.h"
#include "../common/public/debug.h"

/***************************************************************************/
/* Variable definitions
 * */

static oval_syschar_collection_flag_t _flag_agg_map[7][7] =
{
	{SYSCHAR_FLAG_UNKNOWN,          SYSCHAR_FLAG_ERROR, 
        SYSCHAR_FLAG_COMPLETE,          SYSCHAR_FLAG_INCOMPLETE,
        SYSCHAR_FLAG_DOES_NOT_EXIST,    SYSCHAR_FLAG_NOT_COLLECTED,
        SYSCHAR_FLAG_NOT_APPLICABLE},

	{SYSCHAR_FLAG_ERROR,            SYSCHAR_FLAG_ERROR,
        SYSCHAR_FLAG_ERROR,             SYSCHAR_FLAG_ERROR,
        SYSCHAR_FLAG_ERROR,             SYSCHAR_FLAG_ERROR,
        SYSCHAR_FLAG_ERROR},

	{SYSCHAR_FLAG_COMPLETE,         SYSCHAR_FLAG_ERROR,
        SYSCHAR_FLAG_COMPLETE,          SYSCHAR_FLAG_INCOMPLETE,
        SYSCHAR_FLAG_DOES_NOT_EXIST,    SYSCHAR_FLAG_NOT_COLLECTED,
        SYSCHAR_FLAG_NOT_APPLICABLE},

	{SYSCHAR_FLAG_INCOMPLETE,       SYSCHAR_FLAG_ERROR,
        SYSCHAR_FLAG_INCOMPLETE,        SYSCHAR_FLAG_INCOMPLETE,
        SYSCHAR_FLAG_DOES_NOT_EXIST,    SYSCHAR_FLAG_NOT_COLLECTED,
        SYSCHAR_FLAG_NOT_APPLICABLE},

	{SYSCHAR_FLAG_DOES_NOT_EXIST,   SYSCHAR_FLAG_ERROR,
        SYSCHAR_FLAG_DOES_NOT_EXIST,    SYSCHAR_FLAG_DOES_NOT_EXIST,
        SYSCHAR_FLAG_DOES_NOT_EXIST,    SYSCHAR_FLAG_NOT_COLLECTED,
        SYSCHAR_FLAG_NOT_APPLICABLE},

	{SYSCHAR_FLAG_NOT_COLLECTED,    SYSCHAR_FLAG_ERROR,
        SYSCHAR_FLAG_NOT_COLLECTED,     SYSCHAR_FLAG_NOT_COLLECTED,
        SYSCHAR_FLAG_NOT_COLLECTED,     SYSCHAR_FLAG_NOT_COLLECTED, 
        SYSCHAR_FLAG_NOT_APPLICABLE},

	{SYSCHAR_FLAG_NOT_APPLICABLE,   SYSCHAR_FLAG_ERROR,
        SYSCHAR_FLAG_NOT_APPLICABLE,    SYSCHAR_FLAG_NOT_APPLICABLE,
        SYSCHAR_FLAG_NOT_APPLICABLE,    SYSCHAR_FLAG_NOT_APPLICABLE,
        SYSCHAR_FLAG_NOT_APPLICABLE},
};

#define _AGG_FLAG(f1, f2) _flag_agg_map[f2][f1]
#define _COMP_TYPE(comp) oval_component_type_get_text(oval_component_get_type(comp))
#define _FLAG_TYPE(flag) oval_syschar_collection_flag_get_text(flag)

typedef struct oval_component {
	struct oval_definition_model *model;
	oval_component_type_t type;
} oval_component_t;

typedef struct oval_component_LITERAL {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_value *value;	/*type==OVAL_COMPONENT_LITERAL*/
} oval_component_LITERAL_t;

typedef struct oval_component_OBJECTREF {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_object *object;	/*type==OVAL_COMPONENT_OBJECTREF*/
	char *object_field;	/*type==OVAL_COMPONENT_OBJECTREF*/
} oval_component_OBJECTREF_t;

typedef struct oval_component_VARREF {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_variable *variable;	/*type==OVAL_COMPONENT_VARREF*/
} oval_component_VARREF_t;

typedef struct oval_component_FUNCTION {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	/*type==OVAL_COMPONENT_FUNCTION*/
} oval_component_FUNCTION_t;

typedef struct oval_component_ARITHMETIC {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	/*type==OVAL_COMPONENT_FUNCTION*/
	oval_arithmetic_operation_t operation;	/*type==OVAL_COMPONENT_ARITHMETIC*/
} oval_component_ARITHMETIC_t;

typedef struct oval_component_BEGEND {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	//type==OVAL_COMPONENT_FUNCTION
	char *character;	                            //type==OVAL_COMPONENT_BEGIN/END
} oval_component_BEGEND_t;

typedef struct oval_component_SPLIT {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	/*type==OVAL_COMPONENT_FUNCTION*/
	char *delimiter;	/*type==OVAL_COMPONENT_SPLIT*/
} oval_component_SPLIT_t;

typedef struct oval_component_SUBSTRING {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	/*type==OVAL_COMPONENT_FUNCTION*/
	int start;		/*type==OVAL_COMPONENT_SUBSTRING*/
	int length;		/*type==OVAL_COMPONENT_SUBSTRING*/
} oval_component_SUBSTRING_t;

typedef struct oval_component_TIMEDIF {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	/*type==OVAL_COMPONENT_FUNCTION*/
	oval_datetime_format_t format_1;	/*/type==OVAL_COMPONENT_TIMEDIF*/
	oval_datetime_format_t format_2;	/*type==OVAL_COMPONENT_TIMEDIF*/
} oval_component_TIMEDIF_t;

typedef struct oval_component_REGEX_CAPTURE {
	struct oval_definition_model *model;
	oval_component_type_t type;
	struct oval_collection *function_components;	/*type==OVAL_COMPONENT_FUNCTION*/
	char *pattern;	/*type==OVAL_COMPONENT_REGEX_CAPTURE*/
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
 */
static const struct oscap_string_map _OVAL_COMPONENT_MAP[] = {
		{OVAL_COMPONENT_LITERAL   ,"literal_component"},
		{OVAL_COMPONENT_OBJECTREF ,"object_component"},
		{OVAL_COMPONENT_VARREF    ,"variable_component"},
		{0, NULL}
};

static const struct oscap_string_map _OVAL_FUNCTION_MAP[] = {
		{OVAL_FUNCTION_BEGIN        , "begin"          },
		{OVAL_FUNCTION_CONCAT       , "concat"         },
		{OVAL_FUNCTION_END          , "end"            },
		{OVAL_FUNCTION_SPLIT        , "split"          },
		{OVAL_FUNCTION_SUBSTRING    , "substring"      },
		{OVAL_FUNCTION_TIMEDIF      , "time_difference"},
		{OVAL_FUNCTION_ESCAPE_REGEX , "escape_regex"   },
		{OVAL_FUNCTION_REGEX_CAPTURE, "regex_capture"  },
		{OVAL_FUNCTION_ARITHMETIC   , "arithmetic"     },
		{0, NULL}
};

/* End of variable definitions
 * */
/***************************************************************************/

bool oval_component_iterator_has_more(struct oval_component_iterator *oc_component)
{
	return oval_collection_iterator_has_more((struct oval_iterator *) oc_component);
}
int oval_component_iterator_remaining(struct oval_component_iterator *oc_component)
{
	return oval_collection_iterator_remaining((struct oval_iterator *) oc_component);
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

struct oval_value *oval_component_get_literal_value
	(struct oval_component *component)
{
	/* type == OVAL_COMPONENT_LITERAL */
	struct oval_value *value = NULL;
	if (oval_component_get_type(component) == OVAL_COMPONENT_LITERAL) {
		value = ((struct oval_component_LITERAL *)component)->value;
	}
	return value;
}

void oval_component_set_literal_value
	(struct oval_component *component, struct oval_value *value)
{
	/* type == OVAL_COMPONENT_LITERAL */
	if(component && !oval_component_is_locked(component)){
		if (oval_component_get_type(component) == OVAL_COMPONENT_LITERAL) {
			((struct oval_component_LITERAL *)component)->value = value;
		}
	} else 
                oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_component_set_type(struct oval_component *component, oval_component_type_t type) {

	if(component && !oval_component_is_locked(component)){
		component->type = type;
	} else 
                oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

struct oval_object *oval_component_get_object(struct oval_component *component)
{
	/* type == OVAL_COMPONENT_OBJECTREF */
	struct oval_object *object = NULL;
	if (oval_component_get_type(component) == OVAL_COMPONENT_OBJECTREF) {
		object = ((struct oval_component_OBJECTREF *)component)->object;
	}
	return object;
}
void oval_component_set_object
	(struct oval_component *component, struct oval_object *object)
{
	/* type == OVAL_COMPONENT_OBJECTREF */
	if(component && !oval_component_is_locked(component)){
		if (oval_component_get_type(component) == OVAL_COMPONENT_OBJECTREF) {
			((struct oval_component_OBJECTREF *)component)->object = object;
		}
	} else 
                oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

char *oval_component_get_object_field
	(struct oval_component *component)
{
	/* type == OVAL_COMPONENT_OBJECTREF */
	char *field = NULL;
	if (oval_component_get_type(component) == OVAL_COMPONENT_OBJECTREF) {
		field =
		    ((struct oval_component_OBJECTREF *)component)->
		    object_field;
	}
	return field;
}
void oval_component_set_object_field
	(struct oval_component *component, char *field)
{
	if(component && !oval_component_is_locked(component)){
		/* type == OVAL_COMPONENT_OBJECTREF */
		if (oval_component_get_type(component) == OVAL_COMPONENT_OBJECTREF) {
				((struct oval_component_OBJECTREF *)component)->
				object_field = oscap_strdup(field);
		}
	} else 
                oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

struct oval_variable *oval_component_get_variable
	(struct oval_component *component)
{
	/* type == OVAL_COMPONENT_VARREF */
	struct oval_variable *variable = NULL;
	if (oval_component_get_type(component) == OVAL_COMPONENT_VARREF) {
		variable =
		    ((struct oval_component_VARREF *)component)->variable;
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
		oval_component_FUNCTION_t *function =
		    (oval_component_FUNCTION_t *) component;
		iterator =
		    (struct oval_component_iterator *)
		    oval_collection_iterator(function->function_components);
	} else
		iterator = NULL;
	return iterator;
}

oval_arithmetic_operation_t oval_component_get_arithmetic_operation(struct
								   oval_component
								   * component)
{
        __attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_ARITHMETIC */
	oval_arithmetic_operation_t operation;
	if (component->type == OVAL_FUNCTION_ARITHMETIC) {
		oval_component_ARITHMETIC_t *arithmetic =
		    (oval_component_ARITHMETIC_t *) component;
		operation = arithmetic->operation;
	} else
		operation = OVAL_ARITHMETIC_UNKNOWN;
	return operation;
}
void oval_component_set_arithmetic_operation
	(struct oval_component *component, oval_arithmetic_operation_t operation)
{
	if(component && !oval_component_is_locked(component)){
		/* type == OVAL_COMPONENT_ARITHMETIC */
		if (component->type == OVAL_FUNCTION_ARITHMETIC) {
			oval_component_ARITHMETIC_t *arithmetic =
				(oval_component_ARITHMETIC_t *) component;
			arithmetic->operation = operation;
		}
	} else 
                oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

char *oval_component_get_prefix(struct oval_component *component)
{
        __attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_BEGIN/END */
	char *character;
	if (component->type == OVAL_FUNCTION_BEGIN) {
		oval_component_BEGEND_t *begin =
		    (oval_component_BEGEND_t *) component;
		character = begin->character;
	} else
		character = NULL;
	return character;
}
void oval_component_set_prefix
	(struct oval_component *component, char *character)
{
	if(component && !oval_component_is_locked(component)){
		/* type == OVAL_COMPONENT_BEGIN */
		if (component->type == OVAL_FUNCTION_BEGIN) {
			oval_component_BEGEND_t *begin =
				(oval_component_BEGEND_t *) component;
			begin->character = oscap_strdup(character);
		}
	} else 
                oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

char *oval_component_get_suffix(struct oval_component *component)
{
        __attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_END */
	char *character;
	if (component->type == OVAL_FUNCTION_END) {
		oval_component_BEGEND_t *funcend =
		    (oval_component_BEGEND_t *) component;
		character = funcend->character;
	} else
		character = NULL;
	return character;
}

void oval_component_set_suffix
	(struct oval_component *component, char *character)
{
        __attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_END */
	if (component->type == OVAL_FUNCTION_END) {
		oval_component_BEGEND_t *funcend =
		    (oval_component_BEGEND_t *) component;
		funcend->character = oscap_strdup(character);
	}
}

char *oval_component_get_split_delimiter(struct oval_component *component)
{
        __attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_SPLIT */
	char *delimiter;
	if (component->type == OVAL_FUNCTION_SPLIT) {
		oval_component_SPLIT_t *split =
		    (oval_component_SPLIT_t *) component;
		delimiter = split->delimiter;
	} else
		delimiter = NULL;
	return delimiter;
}

void oval_component_set_split_delimiter
	(struct oval_component *component, char *delimeter)
{
        __attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_SPLIT */
	if (component->type == OVAL_FUNCTION_SPLIT) {
		oval_component_SPLIT_t *split =
		    (oval_component_SPLIT_t *) component;
		split->delimiter = oscap_strdup(delimeter);
	}
}

int oval_component_get_substring_start(struct oval_component *component)
{
        __attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_SUBSTRING */
	int start;
	if (component->type == OVAL_FUNCTION_SUBSTRING) {
		oval_component_SUBSTRING_t *substring =
		    (oval_component_SUBSTRING_t *) component;
		start = substring->start;
	} else
		start = -1;
	return start;
}

void oval_component_set_substring_start(struct oval_component *component, int start)
{
	if(component && !oval_component_is_locked(component)){
		//type==OVAL_COMPONENT_SUBSTRING
		if (component->type == OVAL_FUNCTION_SUBSTRING) {
			oval_component_SUBSTRING_t *substring =
				(oval_component_SUBSTRING_t *) component;
			substring->start = start;
		}
	} else 
                oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

int oval_component_get_substring_length(struct oval_component *component)
{
        __attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_SUBSTRING */
	int length;
	if (component->type == OVAL_FUNCTION_SUBSTRING) {
		oval_component_SUBSTRING_t *substring =
		    (oval_component_SUBSTRING_t *) component;
		length = substring->length;
	} else
		length = -1;
	return length;
}
void oval_component_set_substring_length
	(struct oval_component *component, int length)
{
	if(component && !oval_component_is_locked(component)){
		/* type == OVAL_COMPONENT_SUBSTRING */
		if (component->type == OVAL_FUNCTION_SUBSTRING) {
			oval_component_SUBSTRING_t *substring =
				(oval_component_SUBSTRING_t *) component;
			substring->length = length;
		}
	} else 
                oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

oval_datetime_format_t oval_component_get_timedif_format_1(struct oval_component
							  * component)
{
        __attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_TIMEDIF */
	oval_datetime_format_t format;
	if (component->type == OVAL_FUNCTION_TIMEDIF) {
		oval_component_TIMEDIF_t *timedif =
		    (oval_component_TIMEDIF_t *) component;
		format = timedif->format_1;
	} else
		format = OVAL_DATETIME_UNKNOWN;
	return format;
}

void oval_component_set_timedif_format_1
	(struct oval_component *component, oval_datetime_format_t format)
{
	if(component && !oval_component_is_locked(component)){
		/* type == OVAL_COMPONENT_TIMEDIF */
		if (component->type == OVAL_FUNCTION_TIMEDIF) {
			oval_component_TIMEDIF_t *timedif =
				(oval_component_TIMEDIF_t *) component;
			timedif->format_1 = format;
		}
	} else 
                oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

oval_datetime_format_t oval_component_get_timedif_format_2(struct oval_component
							  * component)
{
        __attribute__nonnull__(component);

	/* typ == OVAL_COMPONENT_TIMEDIF */
	oval_datetime_format_t format;
	if (component->type == OVAL_FUNCTION_TIMEDIF) {
		oval_component_TIMEDIF_t *timedif =
		    (oval_component_TIMEDIF_t *) component;
		format = timedif->format_2;
	} else
		format = OVAL_DATETIME_UNKNOWN;
	return format;
}
void oval_component_set_timedif_format_2
	(struct oval_component *component, oval_datetime_format_t format)
{
	if(component && !oval_component_is_locked(component)){
		/* type == OVAL_COMPONENT_TIMEDIF */
		if (component->type == OVAL_FUNCTION_TIMEDIF) {
			oval_component_TIMEDIF_t *timedif =
				(oval_component_TIMEDIF_t *) component;
			timedif->format_2 = format;
		}
	} else 
                oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

char *oval_component_get_regex_pattern
	(struct oval_component *component)
{
        __attribute__nonnull__(component);

	/* type == OVAL_COMPONENT_REGEX_CAPTURE */
	char *pattern = NULL;
	if (component->type == OVAL_FUNCTION_REGEX_CAPTURE) {
		oval_component_REGEX_CAPTURE_t *regex =
		    (oval_component_REGEX_CAPTURE_t *) component;
		pattern = regex->pattern;
	}
	return pattern;
}
void oval_component_set_regex_pattern
	(struct oval_component *component, char *pattern)
{
	if(component && !oval_component_is_locked(component)){
		/* type == OVAL_COMPONENT_REGEX_CAPTURE */
		if (component->type == OVAL_FUNCTION_REGEX_CAPTURE) {
			oval_component_REGEX_CAPTURE_t *regex =
				(oval_component_REGEX_CAPTURE_t *) component;
			regex->pattern = oscap_strdup(pattern);
		}
	} else 
                oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

struct oval_component *oval_component_new(struct oval_definition_model* model, oval_component_type_t type)
{
	oval_component_t *component = NULL;

	switch (type) {
	case OVAL_COMPONENT_LITERAL:{
			oval_component_LITERAL_t *literal =
			    (oval_component_LITERAL_t *) oscap_alloc(sizeof(oval_component_LITERAL_t));
                        if (literal == NULL)
                                return NULL;

			component = (oval_component_t *) literal;
			literal->value = NULL;
		}
		break;
	case OVAL_COMPONENT_OBJECTREF:{
			oval_component_OBJECTREF_t *objectref =
                            (oval_component_OBJECTREF_t *) oscap_alloc(sizeof(oval_component_OBJECTREF_t));
                        if (objectref == NULL)
                                return NULL;

			component = (oval_component_t *) objectref;
			objectref->object = NULL;
			objectref->object_field = NULL;
		}
		break;
	case OVAL_COMPONENT_VARREF:{
			oval_component_VARREF_t *varref =
			    (oval_component_VARREF_t *) oscap_alloc(sizeof(oval_component_VARREF_t));
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
					oval_component_ARITHMETIC_t *arithmetic
					    = (oval_component_ARITHMETIC_t *)
					    (function =
					     (oval_component_FUNCTION_t *)
					     oscap_alloc(sizeof(oval_component_ARITHMETIC_t)));
                                        if (arithmetic == NULL)
                                                return NULL;

					arithmetic->operation =
					    OVAL_ARITHMETIC_UNKNOWN;
				};
				break;
			case OVAL_FUNCTION_BEGIN:
			case OVAL_FUNCTION_END:{
					oval_component_BEGEND_t *begin =
					    (oval_component_BEGEND_t *)
					    (function =
					     (oval_component_FUNCTION_t *)
					     oscap_alloc(sizeof(oval_component_BEGEND_t)));
                                        if (begin == NULL)
                                                return NULL;

					begin->character = NULL;
				};
				break;
			case OVAL_FUNCTION_SPLIT:{
					oval_component_SPLIT_t *split =
					    (oval_component_SPLIT_t *)
					    (function =
					     (oval_component_FUNCTION_t *)
					     oscap_alloc(sizeof(oval_component_SPLIT_t)));
                                        if (split == NULL)
                                                return NULL;

					split->delimiter = NULL;
				};
				break;
			case OVAL_FUNCTION_SUBSTRING:{
					oval_component_SUBSTRING_t *substring =
					    (oval_component_SUBSTRING_t *)
					    (function =
					     (oval_component_FUNCTION_t *)
					     oscap_alloc(sizeof(oval_component_SUBSTRING_t)));
                                        if (substring == NULL)
                                                return NULL;

					substring->length = -1;
					substring->start = -1;
				};
				break;
			case OVAL_FUNCTION_TIMEDIF:{
					oval_component_TIMEDIF_t *timedif =
					    (oval_component_TIMEDIF_t *)
					    (function =
					     (oval_component_FUNCTION_t *)
					     oscap_alloc(sizeof(oval_component_TIMEDIF_t)));
                                        if (timedif == NULL)
                                                return NULL;

					timedif->format_1 =
					    OVAL_DATETIME_UNKNOWN;
					timedif->format_2 =
					    OVAL_DATETIME_UNKNOWN;
				};
				break;
			case OVAL_FUNCTION_REGEX_CAPTURE:{
				oval_component_REGEX_CAPTURE_t *regex =
				    (oval_component_REGEX_CAPTURE_t *)
				    (function =
				     (oval_component_FUNCTION_t *)
				     oscap_alloc(sizeof(oval_component_REGEX_CAPTURE_t)));
                                if (regex == NULL)
                                        return NULL;

				regex->pattern = NULL;
				};
				break;
			default:{
					function =
					    (oval_component_FUNCTION_t *)
					    oscap_alloc(sizeof(oval_component_FUNCTION_t));
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

bool oval_component_is_valid(struct oval_component *component)
{
	return true;//TODO
}

bool oval_component_is_locked(struct oval_component *component)
{
        __attribute__nonnull__(component);

	return oval_definition_model_is_locked(component->model);
}

static void _oval_component_clone_subcomponents
	(struct oval_component *old_component, struct oval_component *new_component, struct oval_definition_model *model)
{
	struct oval_component_iterator *subcomps = oval_component_get_function_components(old_component);
	if(subcomps)while(oval_component_iterator_has_more(subcomps)){
		struct oval_component *subcomp = oval_component_iterator_next(subcomps);
		oval_component_add_function_component(new_component, oval_component_clone(model, subcomp));
	}
	oval_component_iterator_free(subcomps);
}

struct oval_component *oval_component_clone(struct oval_definition_model *new_model, struct oval_component *old_component)
{
        __attribute__nonnull__(old_component);

	struct oval_component *new_component = oval_component_new(new_model, old_component->type);
        if (new_component == NULL)
                return NULL;

	switch(new_component->type){
	case OVAL_FUNCTION_ARITHMETIC:{
		oval_arithmetic_operation_t operation = oval_component_get_arithmetic_operation(old_component);
		if(operation)oval_component_set_arithmetic_operation(new_component, operation);
	}break;
	case OVAL_FUNCTION_BEGIN:{
		char *begchar = oval_component_get_prefix(old_component);
		if(begchar)oval_component_set_prefix(new_component, begchar);
	}break;
	case OVAL_FUNCTION_END:{
		char *endchar = oval_component_get_suffix(old_component);
		if(endchar)oval_component_set_suffix(new_component, endchar);
	}break;
	case OVAL_COMPONENT_LITERAL:{
		struct oval_value *value = oval_component_get_literal_value(old_component);
		if(value)oval_component_set_literal_value(new_component, oval_value_clone(value));
	}break;
	case OVAL_COMPONENT_OBJECTREF:{
		struct oval_object *old_object = oval_component_get_object(old_component);
		if(old_object){
			struct oval_object *new_object = oval_object_clone(new_model, old_object);
			oval_component_set_object(new_component, new_object);
		}
		char *field = oval_component_get_object_field(old_component);
		if(field)oval_component_set_object_field(new_component, field);
	}break;
	case OVAL_FUNCTION_REGEX_CAPTURE:{
		char *pattern = oval_component_get_regex_pattern(old_component);
		if(pattern)oval_component_set_regex_pattern(new_component, pattern);
	}break;
	case OVAL_FUNCTION_SPLIT:{
		char *delimiter = oval_component_get_split_delimiter(old_component);
		if(delimiter)oval_component_set_split_delimiter(new_component, delimiter);
	}break;
	case OVAL_FUNCTION_SUBSTRING:{
		int length = oval_component_get_substring_length(old_component);
		if(length>0)oval_component_set_substring_length(new_component, length);
		int start = oval_component_get_substring_start(old_component);
		oval_component_set_substring_start(new_component, start);
	}break;
	case OVAL_FUNCTION_TIMEDIF:{
		oval_datetime_format_t format1 = oval_component_get_timedif_format_1(old_component);
		if(format1)oval_component_set_timedif_format_1(new_component, format1);
		oval_datetime_format_t format2 = oval_component_get_timedif_format_2(old_component);
		if(format2)oval_component_set_timedif_format_1(new_component, format2);
	}break;
	case OVAL_COMPONENT_VARREF:{
		struct oval_variable *old_variable = oval_component_get_variable(old_component);
		if(old_variable){
			struct oval_variable *new_variable = oval_variable_clone(new_model, old_variable);
			oval_component_set_variable(new_component, new_variable);
		}
	}break;
	default: /*NOOP*/break;
	}

	if(new_component->type>OVAL_FUNCTION)
                _oval_component_clone_subcomponents(old_component, new_component, new_model);

	return new_component;
}

void oval_component_free(struct oval_component *component)
{
        __attribute__nonnull__(component);

	switch (component->type) {
	case OVAL_COMPONENT_LITERAL:{
			oval_component_LITERAL_t *literal =
			    (oval_component_LITERAL_t *) component;
			if (literal->value != NULL)
				oval_value_free(literal->value);
			literal->value = NULL;
		}
		break;
	case OVAL_COMPONENT_OBJECTREF:{
			oval_component_OBJECTREF_t *objectref =
			    (oval_component_OBJECTREF_t *) component;
			if (objectref->object_field != NULL)
				oscap_free(objectref->object_field);
			objectref->object_field = NULL;
		}
		break;
	case OVAL_FUNCTION_BEGIN:
	case OVAL_FUNCTION_END:{
			oval_component_BEGEND_t *begin = (oval_component_BEGEND_t *)component;
			oscap_free(begin->character);
			begin->character = NULL;
		};
		break;
	case OVAL_FUNCTION_SPLIT:{
			oval_component_SPLIT_t *split = (oval_component_SPLIT_t *) component;
			oscap_free(split->delimiter);
			split->delimiter = NULL;
		};
		break;
	case OVAL_FUNCTION_REGEX_CAPTURE:{
		oval_component_REGEX_CAPTURE_t *regex = (oval_component_REGEX_CAPTURE_t *) component;
		oscap_free(regex->pattern);
		regex->pattern = NULL;
		};
		break;
	case OVAL_FUNCTION_CONCAT:
	case OVAL_FUNCTION_SUBSTRING:
	case OVAL_FUNCTION_TIMEDIF:
	case OVAL_FUNCTION_ESCAPE_REGEX:
	case OVAL_FUNCTION_ARITHMETIC:
	case OVAL_COMPONENT_VARREF:
	case OVAL_COMPONENT_FUNCTION:
	case OVAL_COMPONENT_UNKNOWN:
		/*NOOP*/break;
	}
	if(component->type>OVAL_FUNCTION){
		oval_component_FUNCTION_t *function =
		    (oval_component_FUNCTION_t *) component;
		oval_collection_free_items(function->
					   function_components,
					   (oscap_destruct_func)oval_component_free);
		function->function_components = NULL;
	}
	oscap_free(component);
}

void oval_component_add_function_component
	(struct oval_component *component, struct oval_component *func_component)
{
	if(component && !oval_component_is_locked(component)){
		if(component->type>OVAL_FUNCTION){
			oval_component_FUNCTION_t *function = (oval_component_FUNCTION_t *)component;
			if(func_component)oval_collection_add(function->function_components, func_component);
		}
	} else 
                oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_component_set_variable(struct oval_component *component,
				 struct oval_variable *variable)
{
	if(component && !oval_component_is_locked(component)){
		/* type == OVAL_COMPONENT_VARREF */
		if (component->type == OVAL_COMPONENT_VARREF) {
			oval_component_VARREF_t *varref =
				(oval_component_VARREF_t *) component;
			varref->variable = variable;
		}
	} else 
                oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

static void oval_value_consume(struct oval_value *value, void *component) {
	oval_component_set_literal_value(component, value);
}

static int _oval_component_parse_LITERAL_tag(xmlTextReaderPtr reader, 
                                             struct oval_parser_context *context,
                                             struct oval_component *component) {

        return oval_value_parse_tag(reader, context, oval_value_consume, component);
}

static int _oval_component_parse_OBJECTREF_tag(xmlTextReaderPtr reader, 
                                               struct oval_parser_context *context,
                                               struct oval_component *component) {

	struct oval_definition_model *model = oval_parser_context_model(context);
	char *objref = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "object_ref");
	struct oval_object *object = oval_object_get_new(model, objref);
	oscap_free(objref);
        objref=NULL;
	oval_component_set_object(component, object);

	char *objfld = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "item_field");
	oval_component_set_object_field(component, objfld);
	if (objfld != NULL) {
            oscap_free(objfld);
            objfld = NULL;
        }

        /* TODO: always 1 => return int -> void */
	return 1;
}

static int _oval_component_parse_VARREF_tag(xmlTextReaderPtr reader, 
                                            struct oval_parser_context *context,
                                            struct oval_component *component) {

	struct oval_definition_model *model = oval_parser_context_model(context);
	char *varref = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "var_ref");
	struct oval_variable *variable = oval_variable_get_new(model, varref, OVAL_VARIABLE_UNKNOWN);
	if (varref != NULL) {
            oscap_free(varref);
            varref = NULL;
        }
	oval_component_set_variable(component, variable);

        /* TODO: always 1 => return int -> void */
	return 1;
}

static void oval_subcomp_consume(struct oval_component *subcomp, void *func) {

        __attribute__nonnull__(func);

        oval_component_FUNCTION_t *function = func;
	oval_collection_add(function->function_components, (void *)subcomp);
}
	
static int oval_subcomp_tag_consume(xmlTextReaderPtr reader,
				    struct oval_parser_context *context,
                                    void *func) {

        return oval_component_parse_tag(reader, context,
					&oval_subcomp_consume, func);
}

static int _oval_component_parse_FUNCTION_tag(xmlTextReaderPtr reader, 
                                              struct oval_parser_context *context,
                                              struct oval_component *component) {
	oval_component_FUNCTION_t *function =
	    (oval_component_FUNCTION_t *) component;
	return oval_parser_parse_tag(reader, context, &oval_subcomp_tag_consume, function);
}

static int _oval_component_parse_ARITHMETIC_tag(xmlTextReaderPtr reader, 
                                                struct oval_parser_context *context,
                                                struct oval_component *component) {
        __attribute__nonnull__(component);

	oval_component_ARITHMETIC_t *arithmetic =
	    (oval_component_ARITHMETIC_t *) component;
	oval_arithmetic_operation_t operation =
	    oval_arithmetic_operation_parse(reader, "arithmetic_operation",
					    OVAL_ARITHMETIC_UNKNOWN);
	arithmetic->operation = operation;
	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

static int _oval_component_parse_BEGEND_tag(xmlTextReaderPtr reader, 
                                            struct oval_parser_context *context,
                                            struct oval_component *component) {

        __attribute__nonnull__(component);

	oval_component_BEGEND_t *begend = (oval_component_BEGEND_t *) component;
	char *character = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "character");
        /* TODO: Is here any case that begend->character is not NULL ? If so, free it,
         * if not, rewrite it without 3rd variable */
	if (character != NULL) {
		begend->character = oscap_strdup(character);
		oscap_free(character);
                character = NULL;
	}
	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

static int _oval_component_parse_SPLIT_tag(xmlTextReaderPtr reader, 
                                           struct oval_parser_context *context,
                                           struct oval_component *component) {

        __attribute__nonnull__(component);

	oval_component_SPLIT_t *split = (oval_component_SPLIT_t *) component;
	char *delimiter = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "delimiter");
        /* TODO: Is here any case that split->delimiter is not NULL ? If so, free it,
         * if not, rewrite it without 3rd variable */
	if(delimiter != NULL) {
		split->delimiter = oscap_strdup(delimiter);
		oscap_free(delimiter);
                delimiter=NULL;
	}
	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

static int _oval_component_parse_SUBSTRING_tag(xmlTextReaderPtr reader, 
                                               struct oval_parser_context *context,
                                               struct oval_component *component) {

        __attribute__nonnull__(component);

        oval_component_SUBSTRING_t *substring =
	    (oval_component_SUBSTRING_t *) component;
	char *start_text  = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "start");
	char *length_text = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "length");
	int start = (start_text == NULL) ? 0 : atoi(start_text);
	int length = (length_text == NULL) ? 0 : atoi(length_text);
	if (start_text != NULL)
		oscap_free(start_text);
	if (length_text != NULL)
		oscap_free(length_text);
	substring->start = start;
	substring->length = length;

	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

static int _oval_component_parse_TIMEDIF_tag(xmlTextReaderPtr reader, 
                                             struct oval_parser_context *context,
                                             struct oval_component *component) {

        __attribute__nonnull__(component);

	oval_component_TIMEDIF_t *timedif =
	    (oval_component_TIMEDIF_t *) component;
	oval_datetime_format_t format_1 =
	    oval_datetime_format_parse(reader, "format_1",
				       OVAL_DATETIME_YEAR_MONTH_DAY);
	oval_datetime_format_t format_2 =
	    oval_datetime_format_parse(reader, "format_2",
				       OVAL_DATETIME_YEAR_MONTH_DAY);
	timedif->format_1 = format_1;
	timedif->format_2 = format_2;

	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

static int _oval_component_parse_REGEX_CAPTURE_tag(xmlTextReaderPtr reader, 
                                                   struct oval_parser_context *context,
                                                   struct oval_component *component) {
        __attribute__nonnull__(component);

	oval_component_REGEX_CAPTURE_t *regex =
	    (oval_component_REGEX_CAPTURE_t *) component;
	char *pattern = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "pattern");
	regex->pattern = oscap_strdup(pattern);
	if (pattern)
                oscap_free(pattern);

	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

int oval_component_parse_tag(xmlTextReaderPtr reader,
			     struct oval_parser_context *context,
			     oval_component_consumer consumer, 
                             void *user) {

        __attribute__nonnull__(context);

	int return_code = 0;

	struct oval_definition_model *model = context->definition_model;
	char *tagname = (char *) xmlTextReaderName(reader);
	/* oval_component_type_enum type; */
	struct oval_component *component = NULL;
	if (strcmp(tagname, "literal_component") == 0) {
		component = oval_component_new(model, OVAL_COMPONENT_LITERAL);
		return_code =
		    _oval_component_parse_LITERAL_tag(reader, context,
						      component);
	} else if (strcmp(tagname, "object_component") == 0) {
		component = oval_component_new(model, OVAL_COMPONENT_OBJECTREF);
		return_code =
		    _oval_component_parse_OBJECTREF_tag(reader, context,
							component);
	} else if (strcmp(tagname, "variable_component") == 0) {
		component = oval_component_new(model, OVAL_COMPONENT_VARREF);
		return_code =
		    _oval_component_parse_VARREF_tag(reader, context,
						     component);
	} else if (strcmp(tagname, "arithmetic") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_ARITHMETIC);
		return_code =
		    _oval_component_parse_ARITHMETIC_tag(reader, context,
							 component);
	} else if (strcmp(tagname, "begin") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_BEGIN);
		return_code =
		    _oval_component_parse_BEGEND_tag(reader, context,
						     component);
	} else if (strcmp(tagname, "concat") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_CONCAT);
		return_code =
		    _oval_component_parse_FUNCTION_tag(reader, context,
						       component);
	} else if (strcmp(tagname, "end") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_END);
		return_code =
		    _oval_component_parse_BEGEND_tag(reader, context,
						     component);
	} else if (strcmp(tagname, "escape_regex") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_ESCAPE_REGEX);
		return_code =
		    _oval_component_parse_FUNCTION_tag(reader, context,
						       component);
	} else if (strcmp(tagname, "split") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_SPLIT);
		return_code =
		    _oval_component_parse_SPLIT_tag(reader, context, component);
	} else if (strcmp(tagname, "substring") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_SUBSTRING);
		return_code =
		    _oval_component_parse_SUBSTRING_tag(reader, context,
							component);
	} else if (strcmp(tagname, "time_difference") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_TIMEDIF);
		return_code =
		    _oval_component_parse_TIMEDIF_tag(reader, context,
						      component);
	} else if (strcmp(tagname, "regex_capture") == 0) {
		component = oval_component_new(model, OVAL_FUNCTION_REGEX_CAPTURE);
		return_code =
		    _oval_component_parse_REGEX_CAPTURE_tag(reader, context,
						       component);
	} else {
		int line = xmlTextReaderGetParserLineNumber(reader);
		oscap_dprintf
		    ("NOTICE::oval_component_parse_tag::<%s> not handled (line = %d)",
		     tagname, line);
		return_code = oval_parser_skip_tag(reader, context);
	}
	if (component != NULL)
		(*consumer) (component, user);
	if (return_code != 1) {
		int line = xmlTextReaderGetParserLineNumber(reader);
		oscap_dprintf
		    ("NOTICE: oval_component_parse_tag::parse of <%s> terminated on error at line %d",
		     tagname, line);
	}
	oscap_free(tagname);
	return return_code;
}

static void function_comp_to_print(struct oval_component *component, char* nxtindent) {

	int i;
	struct oval_component_iterator *subcomps =
		oval_component_get_function_components(component);
	if (oval_component_iterator_has_more(subcomps)) {
		for (i = 1;  oval_component_iterator_has_more(subcomps); i++) {
			struct oval_component *subcomp =
				oval_component_iterator_next(subcomps);
			oval_component_to_print(subcomp, nxtindent, i);
		}
	} else
		oscap_dprintf("%s FUNCTION_COMPONENTS <<NONE BOUND>>\n",
			   nxtindent);
	oval_component_iterator_free(subcomps);
}

void oval_component_to_print(struct oval_component *component, char *indent,
			     int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sCOMPONENT.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sCOMPONENT[%d].", indent, idx);

	oscap_dprintf("%sTYPE(%lx) = %d\n", nxtindent, (unsigned long)component,
	       oval_component_get_type(component));
	if (oval_component_get_type(component) > OVAL_COMPONENT_FUNCTION) {
		/* oval_component_FUNCTION_t *function = (oval_component_FUNCTION_t *) component;*/
	}
	switch (oval_component_get_type(component)) {
	case OVAL_COMPONENT_LITERAL:{
			struct oval_value *value =
			    oval_component_get_literal_value(component);
			if (value == NULL)
				oscap_dprintf("%sVALUE <<NOT BOUND>>\n", nxtindent);
			else
				oval_value_to_print(value, nxtindent, 0);
		}
		break;
	case OVAL_COMPONENT_OBJECTREF:{
			oscap_dprintf("%sOBJECT_FIELD %s\n", nxtindent,
			       oval_component_get_object_field(component));
			struct oval_object *object =
			    oval_component_get_object(component);
			if (object == NULL)
				oscap_dprintf("%sOBJECT <<NOT BOUND>>\n", nxtindent);
			else
				oval_object_to_print(object, nxtindent, 0);
		}
		break;
	case OVAL_COMPONENT_VARREF:{
			struct oval_variable *variable =
			    oval_component_get_variable(component);
			if (variable == NULL)
				oscap_dprintf("%sVARIABLE <<NOT BOUND>>\n", nxtindent);
			else
				oval_variable_to_print(variable, nxtindent, 0);
		}
		break;
	case OVAL_FUNCTION_ARITHMETIC:{
			oscap_dprintf("%sARITHMETIC_OPERATION %d\n", nxtindent,
			       oval_component_get_arithmetic_operation(component));
			function_comp_to_print(component, nxtindent);
		}
		break;
	case OVAL_FUNCTION_BEGIN:{
			oscap_dprintf("%sBEGIN_CHARACTER %s\n", nxtindent,
			       oval_component_get_prefix(component));
			function_comp_to_print(component, nxtindent);
		}
		break;
	case OVAL_FUNCTION_END:{
			oscap_dprintf("%sEND_CHARACTER %s\n", nxtindent,
			       oval_component_get_suffix(component));
			function_comp_to_print(component, nxtindent);
		}
		break;
	case OVAL_FUNCTION_SPLIT:{
			oscap_dprintf("%sSPLIT_DELIMITER %s\n", nxtindent,
			       oval_component_get_split_delimiter(component));
			function_comp_to_print(component, nxtindent);
		}
		break;
	case OVAL_FUNCTION_SUBSTRING:{
			oscap_dprintf("%sSUBSTRING_START  %d\n", nxtindent,
			       oval_component_get_substring_start(component));
			oscap_dprintf("%sSUBSTRING_LENGTH %d\n", nxtindent,
			       oval_component_get_substring_length(component));
			function_comp_to_print(component, nxtindent);
		}
		break;
	case OVAL_FUNCTION_TIMEDIF:{
			oscap_dprintf("%sTIMEDIF_FORMAT_1  %d\n", nxtindent,
			       oval_component_get_timedif_format_1(component));
			oscap_dprintf("%sTIMEDIF_FORMAT_2  %d\n", nxtindent,
			       oval_component_get_timedif_format_2(component));
			function_comp_to_print(component, nxtindent);
		}
		break;
	case OVAL_FUNCTION_REGEX_CAPTURE:
	case OVAL_FUNCTION_ESCAPE_REGEX:
	case OVAL_FUNCTION_CONCAT:{
			function_comp_to_print(component, nxtindent);
		}
		break;
	default: break;
	}
}

xmlNode *oval_component_to_dom(struct oval_component *component, 
                               xmlDoc *doc, xmlNode *parent)
{
	oval_component_type_t type = oval_component_get_type(component);
	const char *local_name = type<OVAL_FUNCTION
		?_OVAL_COMPONENT_MAP[type-1].string
		:_OVAL_FUNCTION_MAP [type-OVAL_FUNCTION-1].string;

	char *content;
	if(type == OVAL_COMPONENT_LITERAL){
		struct oval_value *value = oval_component_get_literal_value(component);
		content = oval_value_get_text(value);
	}else{
		content = NULL;
	}

	xmlNs *ns_definitions = xmlSearchNsByHref(doc, parent, OVAL_DEFINITIONS_NAMESPACE);
	xmlNode *component_node = xmlNewChild
		(parent, ns_definitions,
				BAD_CAST local_name,
				BAD_CAST content);

	switch (oval_component_get_type(component))
	{
	case OVAL_COMPONENT_LITERAL:{
		struct oval_value *value = oval_component_get_literal_value(component);
		oval_datatype_t datatype = oval_value_get_datatype(value);
		if(datatype != OVAL_DATATYPE_STRING)
			xmlNewProp
				(component_node,
						BAD_CAST "datatype",
						BAD_CAST oval_datatype_get_text(datatype));
	}break;
	case OVAL_COMPONENT_OBJECTREF:{
		struct oval_object *object = oval_component_get_object(component);
		char *object_ref = oval_object_get_id(object);
		xmlNewProp
			(component_node,
					BAD_CAST "object_ref",
					BAD_CAST  object_ref);
		char *item_field = oval_component_get_object_field(component);
		xmlNewProp
			(component_node,
					BAD_CAST "item_field",
					BAD_CAST  item_field);
	}break;
	case OVAL_COMPONENT_VARREF:{
		struct oval_variable *variable = oval_component_get_variable(component);
		char *var_ref = oval_variable_get_id(variable);
		xmlNewProp
			(component_node,
					BAD_CAST "var_ref",
					BAD_CAST  var_ref);

	}break;
	case OVAL_FUNCTION_ARITHMETIC:{
		oval_arithmetic_operation_t operation = oval_component_get_arithmetic_operation(component);
		xmlNewProp
			(component_node,
					BAD_CAST "arithmetic_operation",
					BAD_CAST oval_arithmetic_operation_get_text(operation));
	}break;
	case OVAL_FUNCTION_BEGIN:{
		char *character = oval_component_get_prefix(component);
		xmlNewProp
			(component_node,
					BAD_CAST "character",
					BAD_CAST  character);
	}break;
	case OVAL_FUNCTION_END:{
		char *character = oval_component_get_suffix(component);
		xmlNewProp
			(component_node,
					BAD_CAST "character",
					BAD_CAST  character);
	}break;
	case OVAL_FUNCTION_SUBSTRING:{
		int start = oval_component_get_substring_start(component);
		char substring_start[10]; *substring_start = '\0';
		snprintf(substring_start, sizeof(substring_start), "%d", start);
		xmlNewProp
			(component_node,
					BAD_CAST "substring_start",
					BAD_CAST  substring_start);
		int length = oval_component_get_substring_length(component);
		char substring_length[10]; *substring_length = '\0';
		snprintf(substring_length, sizeof(substring_length), "%d", length);
		xmlNewProp
			(component_node,
					BAD_CAST "substring_length",
					BAD_CAST  substring_length);
	}break;
	case OVAL_FUNCTION_TIMEDIF:{
		oval_datetime_format_t format_1 = oval_component_get_timedif_format_1(component);
		if(format_1 != OVAL_DATETIME_YEAR_MONTH_DAY){
			xmlNewProp
				(component_node,
						BAD_CAST "format_1",
						BAD_CAST oval_datetime_format_get_text(format_1));
		}
		oval_datetime_format_t format_2 = oval_component_get_timedif_format_2(component);
		if(format_2 != OVAL_DATETIME_YEAR_MONTH_DAY){
			xmlNewProp
				(component_node,
						BAD_CAST "format_2",
						BAD_CAST oval_datetime_format_get_text(format_2));
		}
	}break;
	case OVAL_FUNCTION_REGEX_CAPTURE:{
		char *pattern = oval_component_get_regex_pattern(component);
		xmlNewProp
			(component_node,
					BAD_CAST "pattern",
					BAD_CAST  pattern);
	}break;
	case OVAL_FUNCTION_CONCAT:
	case OVAL_FUNCTION_ESCAPE_REGEX:
	case OVAL_FUNCTION_SPLIT:break;
	default: break;
	}

	if(type > OVAL_FUNCTION){
		struct oval_component_iterator *components = oval_component_get_function_components(component);
		while(oval_component_iterator_has_more(components)){
			struct oval_component *sub_component = oval_component_iterator_next(components);
			oval_component_to_dom(sub_component, doc, component_node);
		}
		oval_component_iterator_free(components);
	}

	return component_node;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_LITERAL(struct oval_syschar_model *sysmod, 
                                                                       struct oval_component *component, 
                                                                       struct oval_collection *value_collection)
{
        __attribute__nonnull__(component);

	struct oval_component_LITERAL *literal = (struct oval_component_LITERAL *)component;
	oval_collection_add(value_collection, oval_value_clone(literal->value));
	return SYSCHAR_FLAG_COMPLETE;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_OBJECTREF(struct oval_syschar_model *sysmod, 
                                                                         struct oval_component *component, 
                                                                         struct oval_collection *value_collection)
{
        __attribute__nonnull__(component);

	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_ERROR;
	struct oval_component_OBJECTREF *objref = (struct oval_component_OBJECTREF *)component;
	struct oval_object *object = objref->object;
	if(object){
                oval_pctx_t *pctx;
		struct oval_syschar *syschar;
                
                pctx = oval_pctx_new (sysmod);
                syschar = oval_probe_object_eval (pctx, object);
                oval_pctx_free (pctx);
                
		if(syschar){
			flag = oval_syschar_get_flag(syschar);
			char* field_name = objref->object_field;
			struct oval_sysdata_iterator *sysdatas = oval_syschar_get_sysdata(syschar);
			while(oval_sysdata_iterator_has_more(sysdatas)){
				struct oval_sysdata *sysdata = oval_sysdata_iterator_next(sysdatas);
				struct oval_sysitem_iterator *items = oval_sysdata_get_items(sysdata);
				while(oval_sysitem_iterator_has_more(items)){
					struct oval_sysitem *item = oval_sysitem_iterator_next(items);
					char *item_name = oval_sysitem_get_name(item);
					if(strcmp(field_name, item_name)==0){
						char* text = oval_sysitem_get_value(item);
						oval_datatype_t datatype = oval_sysitem_get_datatype(item);
						struct oval_value *value = oval_value_new(datatype, text);
						oval_collection_add(value_collection, value);
					}
				}
				oval_sysitem_iterator_free(items);
			}
			oval_sysdata_iterator_free(sysdatas);
		}
	}
	return flag;
}

#define _HAS_VALUES(flag) (flag==SYSCHAR_FLAG_COMPLETE || flag==SYSCHAR_FLAG_INCOMPLETE)
static oval_syschar_collection_flag_t _oval_component_evaluate_VARREF(struct oval_syschar_model *sysmod, 
                                                                      struct oval_component *component, 
                                                                      struct oval_collection *value_collection)
{
        __attribute__nonnull__(component);

	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_ERROR;
	struct oval_component_VARREF *varref = (struct oval_component_VARREF *)component;
	struct oval_variable *variable = varref->variable;
	if(variable){
		flag = oval_syschar_model_get_variable_collection_flag(sysmod, variable);
		if(_HAS_VALUES(flag)){
			struct oval_value_iterator *values = oval_syschar_model_get_variable_values(sysmod, variable);
			while(oval_value_iterator_has_more(values)){
				struct oval_value *value = oval_value_iterator_next(values);
				oval_collection_add(value_collection, oval_value_clone(value));
			}
			oval_value_iterator_free(values);
		}
	} else {
		oscap_dprintf("ERROR: No variable bound to VARREF Component");
	}
	return flag;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_BEGIN(struct oval_syschar_model *sysmod, 
                                                                     struct oval_component *component, 
                                                                     struct oval_collection *value_collection)
{
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_ERROR;
	char *prefix = oval_component_get_prefix(component);
	if(prefix){
		int len_prefix = strlen(prefix);
		struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
		if(oval_component_iterator_has_more(subcomps)){/*only the first component is processed*/
			struct oval_collection *subcoll = oval_collection_new();
			struct oval_component  *subcomp = oval_component_iterator_next(subcomps);
			flag = oval_component_evaluate(sysmod, subcomp, subcoll);
			struct oval_value_iterator *values = (struct oval_value_iterator *)oval_collection_iterator(subcoll);
			while(oval_value_iterator_has_more(values)){
				char *key = oval_value_get_text(oval_value_iterator_next(values));
				char concat[len_prefix+strlen(key)+1];
				if(strncmp(prefix, key, len_prefix)){
					snprintf(concat, sizeof(concat), "%s%s", prefix, key);
				}else{
					snprintf(concat, sizeof(concat), "%s", key);
				}
				struct oval_value *concat_value = oval_value_new(OVAL_DATATYPE_STRING, concat);
				oval_collection_add(value_collection, concat_value);
			}
			oval_value_iterator_free(values);
			oval_collection_free_items(subcoll, (oscap_destruct_func)oval_value_free);
		}
		oval_component_iterator_free(subcomps);
	}else{
		oscap_dprintf("ERROR: No prefix specified for begin function (%s:%d)", __FILE__, __LINE__);
	}
	return flag;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_END
	(struct oval_syschar_model *sysmod, struct oval_component *component, struct oval_collection *value_collection)
{
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_ERROR;
	char *suffix = oval_component_get_suffix(component);
	if(suffix){
		int len_suffix = strlen(suffix);
		struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
		if(oval_component_iterator_has_more(subcomps)){/*only the first component is processed*/
			struct oval_collection *subcoll = oval_collection_new();
			struct oval_component *subcomp = oval_component_iterator_next(subcomps);
			flag = oval_component_evaluate(sysmod, subcomp, subcoll);
			struct oval_value_iterator *values = (struct oval_value_iterator *)oval_collection_iterator(subcoll);
			while(oval_value_iterator_has_more(values)){
				char *key = oval_value_get_text(oval_value_iterator_next(values));
				int len_key = strlen(key);
				char concat[len_suffix+len_key+1];
				if((len_suffix<=len_key) && strncmp(suffix, key+len_key-len_suffix, len_suffix)){
					snprintf(concat, sizeof(concat), "%s%s", key, suffix);
				}else{
					snprintf(concat, sizeof(concat), "%s", key);
				}
				struct oval_value *concat_value = oval_value_new(OVAL_DATATYPE_STRING, concat);
				oval_collection_add(value_collection, concat_value);
			}
			oval_value_iterator_free(values);
			oval_collection_free_items(subcoll, (oscap_destruct_func)oval_value_free);
		}
		oval_component_iterator_free(subcomps);
	}else{
		oscap_dprintf("ERROR: No suffix specified for end function  (%s:%d)", __FILE__, __LINE__);
	}
	return flag;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_CONCAT(struct oval_syschar_model *sysmod, 
                                                                      struct oval_component *component, 
                                                                      struct oval_collection *value_collection)
{
	int idx0 = 0;
	int passnum = 0;

	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
	int len_subcomps = oval_component_iterator_remaining(subcomps);
	struct oval_collection *component_colls[len_subcomps];
        for(idx0=0; oval_component_iterator_has_more(subcomps); idx0++){
		struct oval_collection *subcoll = oval_collection_new();
		struct oval_component  *subcomp = oval_component_iterator_next(subcomps);
		oval_syschar_collection_flag_t subflag = oval_component_evaluate(sysmod, subcomp, subcoll);
		flag = _AGG_FLAG(flag, subflag);
		component_colls[idx0] = subcoll;
	}
	bool not_finished = (len_subcomps>0) && _HAS_VALUES(flag);
	if(not_finished){
		struct oval_value_iterator *values[len_subcomps];
		/* bool has_some[len_subcomps]; <-- unused variable */
		not_finished = false;
		char *texts[len_subcomps];
		int  counts[len_subcomps];
		int  catnum = 1;
		for(idx0=0;idx0<len_subcomps;idx0++){
			struct oval_value_iterator *comp_values = (struct oval_value_iterator *)oval_collection_iterator(component_colls[idx0]);
			counts [idx0] = oval_value_iterator_remaining(comp_values);
			if(counts[idx0]){
				/* int dbgnum = catnum; <-- unused variable */
				catnum = catnum*counts[idx0];
				values[idx0] = comp_values;
				texts [idx0] = oval_value_get_text(oval_value_iterator_next(comp_values));
				not_finished = true;
			}else{
				oval_value_iterator_free(values[idx0]);
				oval_collection_free_items(component_colls[idx0], (oscap_destruct_func)oval_value_free);
				component_colls[idx0] = NULL;
				values       [idx0] = NULL;
				texts        [idx0] = NULL;
			}
		}
                for(passnum=1; passnum-1<catnum; passnum++){
			int len_cat = 1;
			for(idx0=0; idx0<len_subcomps; idx0++)if(texts[idx0])len_cat += strlen(texts[idx0]);
			char concat[len_cat];*concat = '\0';
			for(idx0=0; idx0<len_subcomps; idx0++)if(texts[idx0])strcat(concat, texts[idx0]);
			struct oval_value *value = oval_value_new(OVAL_DATATYPE_STRING, concat);
			oval_collection_add(value_collection, value);
			bool rotate = true;
			if(passnum<catnum){
				for(idx0=0; idx0<len_subcomps && rotate; idx0++){
					if(texts[idx0]){
						if(oval_value_iterator_has_more(values[idx0])){
							rotate = false;
						}else{
							oval_value_iterator_free(values[idx0]);
							values[idx0] = (struct oval_value_iterator *)oval_collection_iterator(component_colls[idx0]);
						}
						texts[idx0] = oval_value_get_text(oval_value_iterator_next(values[idx0]));
					}
				}
			}
		}
		for(idx0=0; idx0<len_subcomps; idx0++){
			if(counts[idx0]){
				oval_value_iterator_free(values[idx0]);
				oval_collection_free_items(component_colls[idx0], (oscap_destruct_func)oval_value_free);
			}
		}
	}
	oval_component_iterator_free(subcomps);
	return flag;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_SPLIT(struct oval_syschar_model *sysmod, 
                                                                     struct oval_component *component, 
                                                                     struct oval_collection *value_collection){
	
        oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
	char *delimiter = oval_component_get_split_delimiter(component);
	int  len_delim  = strlen(delimiter);
	if(oval_component_iterator_has_more(subcomps)){/* Only first component is considered */
		struct oval_component  *subcomp = oval_component_iterator_next(subcomps);
		struct oval_collection *subcoll = oval_collection_new();
		flag = oval_component_evaluate(sysmod, subcomp, subcoll);
		struct oval_value_iterator *values = (struct oval_value_iterator *)oval_collection_iterator(subcoll);
		struct oval_value *value;
		while(oval_value_iterator_has_more(values)){
			char *text = oval_value_get_text(oval_value_iterator_next(values));
			if(len_delim){
				char split[strlen(text)+2], *split0 = split; *split0 = '\0'; strcat(split0, text);
				split0[strlen(text)+1] = '\0';/*last two characters are EOS*/
				char *split1;for(split1=strstr(split0, delimiter); split1; split1=strstr(split0, delimiter) ){
					*split1 = '\0';/*terminate the text at the delimeter*/
					value = oval_value_new(OVAL_DATATYPE_STRING, split0);
					oval_collection_add(value_collection, value);
					split0 = split1+len_delim;/*advance split1*/
				}
				value = oval_value_new(OVAL_DATATYPE_STRING, split0);
				oval_collection_add(value_collection, value);
			}else{ /*Empty delimiter, Split at every character*/
				char split[] = {'\0','\0'};
				int idx;
                                for(idx=0;text[idx];idx++){
					*split = text[idx];
					value = oval_value_new(OVAL_DATATYPE_STRING, split);
					oval_collection_add(value_collection, value);
				}
			}
		}
		oval_value_iterator_free(values);
		oval_collection_free_items(subcoll, (oscap_destruct_func)oval_value_free);
	}
	oval_component_iterator_free(subcomps);
	return flag;
};
static oval_syschar_collection_flag_t _oval_component_evaluate_SUBSTRING(struct oval_syschar_model *sysmod, 
                                                                         struct oval_component *component, 
                                                                         struct oval_collection *value_collection)
{
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
	int start = oval_component_get_substring_start(component)-1; /* TODO: is this always unsigned ? */
        int len = oval_component_get_substring_length(component);
	start = (start < 0) ? 0 : start;
	if(oval_component_iterator_has_more(subcomps)){/*Only first component is considered*/
		struct oval_component  *subcomp = oval_component_iterator_next(subcomps);
		struct oval_collection *subcoll = oval_collection_new();
		flag = oval_component_evaluate(sysmod, subcomp, subcoll);
		struct oval_value_iterator *values = (struct oval_value_iterator *)oval_collection_iterator(subcoll);
		struct oval_value *value;
		while(oval_value_iterator_has_more(values)){
			char *text = oval_value_get_text(oval_value_iterator_next(values));
			char substr[len+1];
			text += (start < (int) strlen(text)) ? start : (int) strlen(text);
			strncpy(substr, text, len);
			value = oval_value_new(OVAL_DATATYPE_STRING, substr);
			oval_collection_add(value_collection, value);
		}
		oval_value_iterator_free(values);
		oval_collection_free_items(subcoll, (oscap_destruct_func)oval_value_free);
	}
	return flag;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_TIMEDIF(struct oval_syschar_model *sysmod, 
                                                                       struct oval_component *component, 
                                                                       struct oval_collection *value_collection)
{
	return SYSCHAR_FLAG_NOT_COLLECTED;/*TODO: implement this function*/
}

static bool _isEscape(char chr) {
        return false;/*TODO: implement this function*/
}

static oval_syschar_collection_flag_t _oval_component_evaluate_ESCAPE_REGEX(struct oval_syschar_model *sysmod, 
                                                                            struct oval_component *component, 
                                                                            struct oval_collection *value_collection)
{
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_UNKNOWN;
	struct oval_component_iterator *subcomps = oval_component_get_function_components(component);
	int start = oval_component_get_substring_start(component)-1; 
        /* int len = oval_component_get_substring_length(component); */
	start = (start<0)?0:start;
	if(oval_component_iterator_has_more(subcomps)){//Only first component is considered
		struct oval_component  *subcomp = oval_component_iterator_next(subcomps);
		struct oval_collection *subcoll = oval_collection_new();
		flag = oval_component_evaluate(sysmod, subcomp, subcoll);
		struct oval_value_iterator *values = (struct oval_value_iterator *)oval_collection_iterator(subcoll);
		struct oval_value *value;
		while(oval_value_iterator_has_more(values)){
			char *text = oval_value_get_text(oval_value_iterator_next(values));
                        /* TODO: this len shadows the above one in comment, which one is right ? */
			int len = strlen(text);
			char string[2*len+1], *insert = string;
			while(*text){
				if(_isEscape(*text)){
					*insert     = '\\';
					insert += 1;
				}
				*insert = *text;
				insert += 1;
			}
			value = oval_value_new(OVAL_DATATYPE_STRING, string);
			oval_collection_add(value_collection, value);
		}
		oval_value_iterator_free(values);
		oval_collection_free_items(subcoll, (oscap_destruct_func)oval_value_free);
	}
	return flag;
}

static oval_syschar_collection_flag_t _oval_component_evaluate_REGEX_CAPTURE
	(struct oval_syschar_model *sysmod, struct oval_component *component, struct oval_collection *value_collection)
{
    return SYSCHAR_FLAG_UNKNOWN;
    //TODO: Missing implementation   
}

static oval_syschar_collection_flag_t _oval_component_evaluate_ARITHMETIC   (struct oval_syschar_model *sysmod, struct oval_component *component, struct oval_collection *value_collection)
{
    return SYSCHAR_FLAG_UNKNOWN;
    //TODO: Missing implementation   
}

typedef oval_syschar_collection_flag_t (_oval_component_evaluator)
	(struct oval_syschar_model *, struct oval_component *, struct oval_collection *);

static _oval_component_evaluator *_component_evaluators[15] =
{
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
		NULL
};

oval_syschar_collection_flag_t oval_component_evaluate(struct oval_syschar_model *sysmod, 
                                                       struct oval_component *component, 
                                                       struct oval_collection *value_collection)
{
        __attribute__nonnull__(component);

	oval_component_type_t type = component->type;
	int evidx = (type>OVAL_FUNCTION)?type-OVAL_FUNCTION+OVAL_COMPONENT_FUNCTION:type;
	_oval_component_evaluator *evaluator = (OVAL_COMPONENT_LITERAL<=type && type<= OVAL_FUNCTION_ARITHMETIC)
	?_component_evaluators[evidx]: NULL;
	oval_syschar_collection_flag_t flag = SYSCHAR_FLAG_ERROR;
	if(evaluator){
		flag = (*evaluator)(sysmod, component, value_collection);
	} else 
                oscap_dprintf("ERROR component type %d not supported (%s:%d)", __FILE__, __LINE__);
	return flag;
}
