/*
 * oval_component.c
 *
 *  Created on: Mar 3, 2009
 *      Author: david.niemoller
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "oval_definitions_impl.h"
#include "oval_collection_impl.h"
#include "oval_string_map_impl.h"
#include "oval_agent_api_impl.h"

typedef struct oval_component {
	oval_component_type_enum type;
} oval_component_t;

typedef struct oval_component_LITERAL {
	oval_component_type_enum type;
	struct oval_value *value;	//type==OVAL_COMPONENT_LITERAL
} oval_component_LITERAL_t;

typedef struct oval_component_OBJECTREF {
	oval_component_type_enum type;
	struct oval_object *object;	//type==OVAL_COMPONENT_OBJECTREF
	char *object_field;	//type==OVAL_COMPONENT_OBJECTREF
} oval_component_OBJECTREF_t;

typedef struct oval_component_VARREF {
	oval_component_type_enum type;
	struct oval_variable *variable;	//type==OVAL_COMPONENT_VARREF
} oval_component_VARREF_t;

typedef struct oval_component_FUNCTION {
	oval_component_type_enum type;
	struct oval_collection *function_components;	//type==OVAL_COMPONENT_FUNCTION
} oval_component_FUNCTION_t;

typedef struct oval_component_ARITHMETIC {
	oval_component_type_enum type;
	struct oval_collection *function_components;	//type==OVAL_COMPONENT_FUNCTION
	oval_arithmetic_operation_enum operation;	//type==OVAL_COMPONENT_ARITHMETIC
} oval_component_ARITHMETIC_t;

typedef struct oval_component_BEGEND {
	oval_component_type_enum type;
	struct oval_collection *function_components;	//type==OVAL_COMPONENT_FUNCTION
	char *character;	//type==OVAL_COMPONENT_BEGIN
} oval_component_BEGEND_t;

typedef struct oval_component_SPLIT {
	oval_component_type_enum type;
	struct oval_collection *function_components;	//type==OVAL_COMPONENT_FUNCTION
	char *delimiter;	//type==OVAL_COMPONENT_SPLIT
} oval_component_SPLIT_t;

typedef struct oval_component_SUBSTRING {
	oval_component_type_enum type;
	struct oval_collection *function_components;	//type==OVAL_COMPONENT_FUNCTION
	int start;		//type==OVAL_COMPONENT_SUBSTRING
	int length;		//type==OVAL_COMPONENT_SUBSTRING
} oval_component_SUBSTRING_t;

typedef struct oval_component_TIMEDIF {
	oval_component_type_enum type;
	struct oval_collection *function_components;	//type==OVAL_COMPONENT_FUNCTION
	oval_datetime_format_enum format_1;	//type==OVAL_COMPONENT_TIMEDIF
	oval_datetime_format_enum format_2;	//type==OVAL_COMPONENT_TIMEDIF
} oval_component_TIMEDIF_t;

int oval_iterator_component_has_more(struct oval_iterator_component
				     *oc_component)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_component);
}

struct oval_component *oval_iterator_component_next(struct
						    oval_iterator_component
						    *oc_component)
{
	return (struct oval_component *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_component);
}

oval_component_type_enum oval_component_type(struct oval_component *component)
{
	return component->type;
}

struct oval_value *oval_component_literal_value(struct oval_component
						*component)
{
	//type==OVAL_COMPONENT_LITERAL
	struct oval_value *value = NULL;
	if (oval_component_type(component) == OVAL_COMPONENT_LITERAL) {
		value = ((struct oval_component_LITERAL *)component)->value;
	}
	return value;
}

struct oval_object *oval_component_object(struct oval_component *component)
{
	//type==OVAL_COMPONENT_OBJECTREF
	struct oval_object *object = NULL;
	if (oval_component_type(component) == OVAL_COMPONENT_OBJECTREF) {
		object = ((struct oval_component_OBJECTREF *)component)->object;
	}
	return object;
}

char *oval_component_object_field(struct oval_component *component)
{
	//type==OVAL_COMPONENT_OBJECTREF
	char *field = NULL;
	if (oval_component_type(component) == OVAL_COMPONENT_OBJECTREF) {
		field =
		    ((struct oval_component_OBJECTREF *)component)->
		    object_field;
	}
	return field;
}

struct oval_variable *oval_component_variable(struct oval_component *component)
{
	//type==OVAL_COMPONENT_VARREF
	struct oval_variable *variable = NULL;
	if (oval_component_type(component) == OVAL_COMPONENT_VARREF) {
		variable =
		    ((struct oval_component_VARREF *)component)->variable;
	}
	return variable;
}

struct oval_iterator_component *oval_component_function_components(struct
								   oval_component
								   *component)
{
	//type==OVAL_COMPONENT_FUNCTION
	struct oval_iterator_component *iterator;
	if (component->type > OVAL_COMPONENT_FUNCTION) {
		oval_component_FUNCTION_t *function =
		    (oval_component_FUNCTION_t *) component;
		iterator =
		    (struct oval_iterator_component *)
		    oval_collection_iterator(function->function_components);
	} else
		iterator = NULL;
	return iterator;
}

oval_arithmetic_operation_enum oval_component_arithmetic_operation(struct
								   oval_component
								   * component)
{
	//type==OVAL_COMPONENT_ARITHMETIC
	oval_arithmetic_operation_enum operation;
	if (component->type == OVAL_FUNCTION_ARITHMETIC) {
		oval_component_ARITHMETIC_t *arithmetic =
		    (oval_component_ARITHMETIC_t *) component;
		operation = arithmetic->operation;
	} else
		operation = OVAL_ARITHMETIC_UNKNOWN;
	return operation;
}

char *oval_component_begin_character(struct oval_component *component)
{
	//type==OVAL_COMPONENT_BEGIN
	char *character;
	if (component->type == OVAL_FUNCTION_BEGIN) {
		oval_component_BEGEND_t *begin =
		    (oval_component_BEGEND_t *) component;
		character = begin->character;
	} else
		character = NULL;
	return character;
}

char *oval_component_end_character(struct oval_component *component)
{
	//type==OVAL_COMPONENT_END
	char *character;
	if (component->type == OVAL_FUNCTION_END) {
		oval_component_BEGEND_t *funcend =
		    (oval_component_BEGEND_t *) component;
		character = funcend->character;
	} else
		character = NULL;
	return character;
}

char *oval_component_split_delimiter(struct oval_component *component)
{
	//type==OVAL_COMPONENT_SPLIT
	char *delimiter;
	if (component->type == OVAL_FUNCTION_SPLIT) {
		oval_component_SPLIT_t *split =
		    (oval_component_SPLIT_t *) component;
		delimiter = split->delimiter;
	} else
		delimiter = NULL;
	return delimiter;
}

int oval_component_substring_start(struct oval_component *component)
{
	//type==OVAL_COMPONENT_SUBSTRING
	int start;
	if (component->type == OVAL_FUNCTION_SUBSTRING) {
		oval_component_SUBSTRING_t *substring =
		    (oval_component_SUBSTRING_t *) component;
		start = substring->start;
	} else
		start = -1;
	return start;
}

int oval_component_substring_length(struct oval_component *component)
{
	//type==OVAL_COMPONENT_SUBSTRING
	int length;
	if (component->type == OVAL_FUNCTION_SUBSTRING) {
		oval_component_SUBSTRING_t *substring =
		    (oval_component_SUBSTRING_t *) component;
		length = substring->length;
	} else
		length = -1;
	return length;
}

oval_datetime_format_enum oval_component_timedif_format_1(struct oval_component
							  * component)
{
	//type==OVAL_COMPONENT_TIMEDIF
	oval_datetime_format_enum format;
	if (component->type == OVAL_FUNCTION_TIMEDIF) {
		oval_component_TIMEDIF_t *timedif =
		    (oval_component_TIMEDIF_t *) component;
		format = timedif->format_1;
	} else
		format = OVAL_DATETIME_UNKNOWN;
	return format;
}

oval_datetime_format_enum oval_component_timedif_format_2(struct oval_component
							  * component)
{
	//type==OVAL_COMPONENT_TIMEDIF
	oval_datetime_format_enum format;
	if (component->type == OVAL_FUNCTION_TIMEDIF) {
		oval_component_TIMEDIF_t *timedif =
		    (oval_component_TIMEDIF_t *) component;
		format = timedif->format_2;
	} else
		format = OVAL_DATETIME_UNKNOWN;
	return format;
}

struct oval_component *oval_component_new(oval_component_type_enum type)
{
	oval_component_t *component;
	switch (type) {
	case OVAL_COMPONENT_LITERAL:{
			oval_component_LITERAL_t *literal
			    =
			    (oval_component_LITERAL_t *)
			    malloc(sizeof(oval_component_LITERAL_t));
			component = (oval_component_t *) literal;
			literal->value = NULL;
		}
		break;
	case OVAL_COMPONENT_OBJECTREF:{
			oval_component_OBJECTREF_t *objectref
			    =
			    (oval_component_OBJECTREF_t *)
			    malloc(sizeof(oval_component_OBJECTREF_t));
			component = (oval_component_t *) objectref;
			objectref->object = NULL;
			objectref->object_field = NULL;
		}
		break;
	case OVAL_COMPONENT_VARREF:{
			oval_component_VARREF_t *varref
			    =
			    (oval_component_VARREF_t *)
			    malloc(sizeof(oval_component_VARREF_t));
			component = (oval_component_t *) varref;
			varref->variable = NULL;
		}
		break;
	default:		//OVAL_COMPONENT_FUNCTION
		{
			oval_component_FUNCTION_t *function;
			switch (type) {
			case OVAL_FUNCTION_ARITHMETIC:{
					oval_component_ARITHMETIC_t *arithmetic
					    = (oval_component_ARITHMETIC_t *)
					    (function =
					     (oval_component_FUNCTION_t *)
					     malloc(sizeof
						    (oval_component_ARITHMETIC_t)));
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
					     malloc(sizeof
						    (oval_component_BEGEND_t)));
					begin->character = NULL;
				};
				break;
			case OVAL_FUNCTION_SPLIT:{
					oval_component_SPLIT_t *split =
					    (oval_component_SPLIT_t *)
					    (function =
					     (oval_component_FUNCTION_t *)
					     malloc(sizeof
						    (oval_component_SPLIT_t)));
					split->delimiter = NULL;
				};
				break;
			case OVAL_FUNCTION_SUBSTRING:{
					oval_component_SUBSTRING_t *substring =
					    (oval_component_SUBSTRING_t *)
					    (function =
					     (oval_component_FUNCTION_t *)
					     malloc(sizeof
						    (oval_component_SUBSTRING_t)));
					substring->length = -1;
					substring->start = -1;
				};
				break;
			case OVAL_FUNCTION_TIMEDIF:{
					oval_component_TIMEDIF_t *timedif =
					    (oval_component_TIMEDIF_t *)
					    (function =
					     (oval_component_FUNCTION_t *)
					     malloc(sizeof
						    (oval_component_TIMEDIF_t)));
					timedif->format_1 =
					    OVAL_DATETIME_UNKNOWN;
					timedif->format_2 =
					    OVAL_DATETIME_UNKNOWN;
				};
				break;
			default:{
					function =
					    (oval_component_FUNCTION_t *)
					    malloc(sizeof
						   (oval_component_FUNCTION_t));
				}
			}
			component = (oval_component_t *) function;
			function->function_components = oval_collection_new();
		}
		break;

	}
	component->type = type;
	return component;
}

void oval_component_free(struct oval_component *component)
{
	switch (component->type) {
	case OVAL_COMPONENT_LITERAL:{
			oval_component_LITERAL_t *literal =
			    (oval_component_LITERAL_t *) component;
			if (literal->value != NULL)
				oval_value_free(literal->value);
		}
		break;
	case OVAL_COMPONENT_OBJECTREF:{
			oval_component_OBJECTREF_t *objectref =
			    (oval_component_OBJECTREF_t *) component;
			if (objectref->object_field != NULL)
				free(objectref->object_field);
		}
		break;
	case OVAL_COMPONENT_UNKNOWN:
		/*NOOP*/ break;
	default:{
			oval_component_FUNCTION_t *function =
			    (oval_component_FUNCTION_t *) component;
			void free_subcomp(void *subcomp) {
				oval_component_free((struct oval_component *)
						    subcomp);
			}
			oval_collection_free_items(function->
						   function_components,
						   &free_subcomp);
			switch (component->type) {
			case OVAL_FUNCTION_BEGIN:
			case OVAL_FUNCTION_END:{
					oval_component_BEGEND_t *begin =
					    (oval_component_BEGEND_t *)
					    function;
					free(begin->character);
				};
				break;
			case OVAL_FUNCTION_SPLIT:{
					oval_component_SPLIT_t *split =
					    (oval_component_SPLIT_t *) function;
					free(split->delimiter);
				};
				break;
			}
		}
	}
	free(component);
}

void set_oval_component_literal_value(struct oval_component *component,
				      struct oval_value *value)
{
	//type==OVAL_COMPONENT_LITERAL
	if (component->type == OVAL_COMPONENT_LITERAL) {
		oval_component_LITERAL_t *literal =
		    (oval_component_LITERAL_t *) component;
		literal->value = value;
	}
}

void set_oval_component_object(struct oval_component *component,
			       struct oval_object *object)
{
	//type==OVAL_COMPONENT_OBJECTREF
	if (component->type == OVAL_COMPONENT_OBJECTREF) {
		oval_component_OBJECTREF_t *objref =
		    (oval_component_OBJECTREF_t *) component;
		objref->object = object;
	}
}

void set_oval_component_object_field(struct oval_component *component,
				     char *field)
{
	//type==OVAL_COMPONENT_OBJECTREF
	if (component->type == OVAL_COMPONENT_OBJECTREF) {
		oval_component_OBJECTREF_t *objref =
		    (oval_component_OBJECTREF_t *) component;
		objref->object_field = field;
	}
}

void set_oval_component_variable(struct oval_component *component,
				 struct oval_variable *variable)
{
	//type==OVAL_COMPONENT_VARREF
	if (component->type == OVAL_COMPONENT_VARREF) {
		oval_component_VARREF_t *varref =
		    (oval_component_VARREF_t *) component;
		varref->variable = variable;
	}
}

int _oval_component_parse_LITERAL_tag
    (xmlTextReaderPtr reader, struct oval_parser_context *context,
     struct oval_component *component) {
	void value_consumer(struct oval_value *value, void *null) {
		set_oval_component_literal_value(component, value);
	}
	int return_code =
	    oval_value_parse_tag(reader, context, value_consumer, NULL);
	return return_code;
}

int _oval_component_parse_OBJECTREF_tag
    (xmlTextReaderPtr reader, struct oval_parser_context *context,
     struct oval_component *component) {
	char *objref = xmlTextReaderGetAttribute(reader, "object_ref");
	struct oval_object_model *model = oval_parser_context_model(context);
	struct oval_object *object = get_oval_object_new(model, objref);
	set_oval_component_object(component, object);

	char *objfld = xmlTextReaderGetAttribute(reader, "item_field");
	set_oval_component_object_field(component, objfld);

	int return_code = 1;
	return return_code;
}

int _oval_component_parse_VARREF_tag
    (xmlTextReaderPtr reader, struct oval_parser_context *context,
     struct oval_component *component) {
	char *varref = xmlTextReaderGetAttribute(reader, "var_ref");
	struct oval_object_model *model = oval_parser_context_model(context);
	struct oval_variable *variable = get_oval_variable_new(model, varref);
	set_oval_component_variable(component, variable);

	int return_code = 1;
	return return_code;
}

int _oval_component_parse_FUNCTION_tag
    (xmlTextReaderPtr reader, struct oval_parser_context *context,
     struct oval_component *component) {
	oval_component_FUNCTION_t *function =
	    (oval_component_FUNCTION_t *) component;
	void subcomp_consumer(struct oval_component *subcomp, void *null) {
		oval_collection_add(function->function_components,
				    (void *)subcomp);
	}
	int subcomp_tag_consumer(xmlTextReaderPtr reader,
				 struct oval_parser_context *context,
				 void *null) {
		return oval_component_parse_tag(reader, context,
						&subcomp_consumer, NULL);
	}
	int return_code =
	    oval_parser_parse_tag(reader, context, &subcomp_tag_consumer, NULL);
	return return_code;
}

int _oval_component_parse_ARITHMETIC_tag
    (xmlTextReaderPtr reader, struct oval_parser_context *context,
     struct oval_component *component) {
	oval_component_ARITHMETIC_t *arithmetic =
	    (oval_component_ARITHMETIC_t *) component;
	oval_arithmetic_operation_enum operation =
	    oval_arithmetic_operation_parse(reader, "arithmetic_operation",
					    OVAL_ARITHMETIC_UNKNOWN);
	arithmetic->operation = operation;
	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

int _oval_component_parse_BEGEND_tag
    (xmlTextReaderPtr reader, struct oval_parser_context *context,
     struct oval_component *component) {
	oval_component_BEGEND_t *begend = (oval_component_BEGEND_t *) component;
	char *character = xmlTextReaderGetAttribute(reader, "character");
	begend->character = character;
	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

int _oval_component_parse_SPLIT_tag
    (xmlTextReaderPtr reader, struct oval_parser_context *context,
     struct oval_component *component) {
	oval_component_SPLIT_t *split = (oval_component_SPLIT_t *) component;
	char *delimiter = xmlTextReaderGetAttribute(reader, "delimiter");
	split->delimiter = delimiter;
	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

int _oval_component_parse_SUBSTRING_tag
    (xmlTextReaderPtr reader, struct oval_parser_context *context,
     struct oval_component *component) {
	oval_component_SUBSTRING_t *substring =
	    (oval_component_SUBSTRING_t *) component;
	char *start_text = xmlTextReaderGetAttribute(reader, "start");
	char *length_text = xmlTextReaderGetAttribute(reader, "start");
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

int _oval_component_parse_TIMEDIF_tag
    (xmlTextReaderPtr reader, struct oval_parser_context *context,
     struct oval_component *component) {
	oval_component_TIMEDIF_t *timedif =
	    (oval_component_TIMEDIF_t *) component;
	oval_datetime_format_enum format_1 =
	    oval_datetime_format_parse(reader, "format_1",
				       OVAL_DATETIME_YEAR_MONTH_DAY);
	oval_datetime_format_enum format_2 =
	    oval_datetime_format_parse(reader, "format_2",
				       OVAL_DATETIME_YEAR_MONTH_DAY);
	timedif->format_1 = format_1;
	timedif->format_2 = format_2;
	return _oval_component_parse_FUNCTION_tag(reader, context, component);
}

void oval_component_to_print(struct oval_component *component, char *indent,
			     int index);
//typedef void (*oval_component_consumer)(struct oval_component_node *, void*);
int oval_component_parse_tag(xmlTextReaderPtr reader,
			     struct oval_parser_context *context,
			     oval_component_consumer consumer, void *user)
{
	char *tagname = xmlTextReaderName(reader);
	oval_component_type_enum type;
	int return_code;
	struct oval_component *component;;
	if (strcmp(tagname, "literal_component") == 0) {
		component = oval_component_new(OVAL_COMPONENT_LITERAL);
		return_code =
		    _oval_component_parse_LITERAL_tag(reader, context,
						      component);
	} else if (strcmp(tagname, "object_component") == 0) {
		component = oval_component_new(OVAL_COMPONENT_OBJECTREF);
		return_code =
		    _oval_component_parse_OBJECTREF_tag(reader, context,
							component);
	} else if (strcmp(tagname, "variable_component") == 0) {
		component = oval_component_new(OVAL_COMPONENT_VARREF);
		return_code =
		    _oval_component_parse_VARREF_tag(reader, context,
						     component);
	} else if (strcmp(tagname, "arithmetic") == 0) {
		component = oval_component_new(OVAL_FUNCTION_ARITHMETIC);
		return_code =
		    _oval_component_parse_ARITHMETIC_tag(reader, context,
							 component);
	} else if (strcmp(tagname, "begin") == 0) {
		component = oval_component_new(OVAL_FUNCTION_BEGIN);
		return_code =
		    _oval_component_parse_BEGEND_tag(reader, context,
						     component);
	} else if (strcmp(tagname, "concat") == 0) {
		component = oval_component_new(OVAL_FUNCTION_CONCAT);
		return_code =
		    _oval_component_parse_FUNCTION_tag(reader, context,
						       component);
	} else if (strcmp(tagname, "end") == 0) {
		component = oval_component_new(OVAL_FUNCTION_END);
		return_code =
		    _oval_component_parse_BEGEND_tag(reader, context,
						     component);
	} else if (strcmp(tagname, "escape_regex") == 0) {
		component = oval_component_new(OVAL_FUNCTION_ESCAPE_REGEX);
		return_code =
		    _oval_component_parse_FUNCTION_tag(reader, context,
						       component);
	} else if (strcmp(tagname, "split") == 0) {
		component = oval_component_new(OVAL_FUNCTION_SPLIT);
		return_code =
		    _oval_component_parse_SPLIT_tag(reader, context, component);
	} else if (strcmp(tagname, "substring") == 0) {
		component = oval_component_new(OVAL_FUNCTION_SUBSTRING);
		return_code =
		    _oval_component_parse_SUBSTRING_tag(reader, context,
							component);
	} else if (strcmp(tagname, "time_difference") == 0) {
		component = oval_component_new(OVAL_FUNCTION_TIMEDIF);
		return_code =
		    _oval_component_parse_TIMEDIF_tag(reader, context,
						      component);
	} else if (strcmp(tagname, "regex_capture") == 0) {
		component = oval_component_new(OVAL_FUNCTION_REGEX_CAPTURE);
		return_code =
		    _oval_component_parse_FUNCTION_tag(reader, context,
						       component);
	} else {
		int line = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE::oval_component_parse_tag::<%s> not handled (line = %d)\n",
		     tagname, line);
		return_code = oval_parser_skip_tag(reader, context);
	}
	(*consumer) (component, user);
	if (return_code != 1) {
		int line = xmlTextReaderGetParserLineNumber(reader);
		printf
		    ("NOTICE: oval_component_parse_tag::parse of <%s> terminated on error at line %d\n",
		     tagname, line);
	}
	free(tagname);
	return return_code;
}

void oval_component_to_print(struct oval_component *component, char *indent,
			     int index)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (index == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sCOMPONENT.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sCOMPONENT[%d].", indent, index);

	printf("%sTYPE(%d) = %d\n", nxtindent, component,
	       oval_component_type(component));
	if (oval_component_type(component) > OVAL_COMPONENT_FUNCTION) {
		oval_component_FUNCTION_t *function =
		    (oval_component_FUNCTION_t *) component;
	}
	void function_to_print() {
		struct oval_iterator_component *subcomps =
		    oval_component_function_components(component);
		if (oval_iterator_component_has_more(subcomps)) {
			int idx;
			for (idx = 1;
			     oval_iterator_component_has_more(subcomps);
			     idx++) {
				struct oval_component *subcomp =
				    oval_iterator_component_next(subcomps);
				oval_component_to_print(subcomp, nxtindent,
							idx);
			}
		} else
			printf("%sFUNCTION_COMPONENTS <<NONE BOUND>>\n",
			       nxtindent);
	}
	switch (oval_component_type(component)) {
	case OVAL_COMPONENT_LITERAL:{
			struct oval_value *value =
			    oval_component_literal_value(component);
			if (value == NULL)
				printf("%sVALUE <<NOT BOUND>>\n", nxtindent);
			else
				oval_value_to_print(value, nxtindent, 0);
		}
		break;
	case OVAL_COMPONENT_OBJECTREF:{
			printf("%sOBJECT_FIELD %s\n", nxtindent,
			       oval_component_object_field(component));
			struct oval_object *object =
			    oval_component_object(component);
			if (object == NULL)
				printf("%sOBJECT <<NOT BOUND>>\n", nxtindent);
			else
				oval_object_to_print(object, nxtindent, 0);
		}
		break;
	case OVAL_COMPONENT_VARREF:{
			struct oval_variable *variable =
			    oval_component_variable(component);
			if (variable == NULL)
				printf("%sVARIABLE <<NOT BOUND>>\n", nxtindent);
			else
				oval_variable_to_print(variable, nxtindent, 0);
		}
		break;
	case OVAL_FUNCTION_ARITHMETIC:{
			printf("%sARITHMETIC_OPERATION %d\n", nxtindent,
			       oval_component_arithmetic_operation(component));
			function_to_print();
		}
		break;
	case OVAL_FUNCTION_BEGIN:{
			printf("%sBEGIN_CHARACTER %s\n", nxtindent,
			       oval_component_begin_character(component));
			function_to_print();
		}
		break;
	case OVAL_FUNCTION_END:{
			printf("%sEND_CHARACTER %s\n", nxtindent,
			       oval_component_end_character(component));
			function_to_print();
		}
		break;
	case OVAL_FUNCTION_SPLIT:{
			printf("%sSPLIT_DELIMITER %s\n", nxtindent,
			       oval_component_split_delimiter(component));
			function_to_print();
		}
		break;
	case OVAL_FUNCTION_SUBSTRING:{
			printf("%sSUBSTRING_START  %d\n", nxtindent,
			       oval_component_substring_start(component));
			printf("%sSUBSTRING_LENGTH %d\n", nxtindent,
			       oval_component_substring_length(component));
			function_to_print();
		}
		break;
	case OVAL_FUNCTION_TIMEDIF:{
			printf("%sTIMEDIF_FORMAT_1  %d\n", nxtindent,
			       oval_component_timedif_format_1(component));
			printf("%sTIMEDIF_FORMAT_2  %d\n", nxtindent,
			       oval_component_timedif_format_2(component));
			function_to_print();
		}
		break;
	case OVAL_FUNCTION_REGEX_CAPTURE:
	case OVAL_FUNCTION_ESCAPE_REGEX:
	case OVAL_FUNCTION_CONCAT:{
			function_to_print();
		}
		break;
	}
}
