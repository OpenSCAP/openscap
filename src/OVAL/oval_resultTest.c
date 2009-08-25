/**
 * @file oval_resultTest.c
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
#include "oval_results_impl.h"
#include "oval_collection_impl.h"

#define OVAL_RESULT_TEST_DEBUG 1

typedef struct oval_result_test {
	struct oval_test *test;
	oval_result_enum result;
	struct oval_message    *message;
	struct oval_collection *items;
	struct oval_collection *bindings;
	int variable_instance;
} oval_result_test_t;

struct oval_result_test *oval_result_test_new(struct oval_result_system *system, char* tstid)
{
	oval_result_test_t *test = (oval_result_test_t *)
		malloc(sizeof(oval_result_test_t));
	struct oval_results_model *results_model
		= oval_result_system_results_model(system);
	struct oval_syschar_model *syschar_model
		= oval_results_model_syschar_model(results_model);
	struct oval_object_model *object_model
		= oval_syschar_model_object_model(syschar_model);
	test->test = get_oval_test_new(object_model, tstid);
	test->message           = NULL;
	test->result            = 0;
	test->variable_instance = 0;
	test->items             = oval_collection_new();
	test->bindings          = oval_collection_new();
	return test;
}

void oval_result_test_free(struct oval_result_test *test)
{
	if(test->message)free(test->message);
	oval_collection_free_items
		(test->items, (oscap_destruct_func)oval_result_item_free);
	oval_collection_free_items
		(test->bindings, (oscap_destruct_func)oval_variable_binding_free);

	test->test              = NULL;
	test->message           = NULL;
	test->result            = 0;
	test->items             = NULL;
	test->bindings          = NULL;
	test->variable_instance = 0;
	free(test);
}

int oval_iterator_result_test_has_more(struct oval_iterator_result_test
				       *oc_result_test)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_result_test);
}

struct oval_result_test *oval_iterator_result_test_next(struct
							oval_iterator_result_test
							*oc_result_test)
{
	return (struct oval_result_test *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_result_test);
}

struct oval_test *oval_result_test_test(struct oval_result_test *rtest)
{
	return ((struct oval_result_test *)rtest)->test;
}

oval_result_enum oval_result_test_result(struct oval_result_test *rtest)
{
	return ((struct oval_result_test *)rtest)->result;
}

struct oval_message *oval_result_test_message(struct oval_result_test *rtest)
{
	return ((struct oval_result_test *)rtest)->message;
}

struct oval_iterator_result_item *oval_result_test_items(struct oval_result_test
							 *rtest)
{
	return (struct oval_iterator_result_item *)
	    oval_collection_iterator(rtest->items);
}

void set_oval_result_test_result(struct oval_result_test *test, oval_result_enum result)
{
	test->result = result;
}

void set_oval_result_test_variable_instance(struct oval_result_test *test, int instance)
{
	test->variable_instance = instance;
}

void set_oval_result_test_message
	(struct oval_result_test *test, struct oval_message *message)
{
	if(test->message)oval_message_free(test->message);
	test->message = message;
}

void add_oval_result_test_item
	(struct oval_result_test *test, struct oval_result_item *item)
{
	oval_collection_add(test->items, item);
}

void add_oval_result_test_binding
	(struct oval_result_test *test, struct oval_variable_binding *binding)
{
	oval_collection_add(test->bindings, binding);
}

//void(*oscap_consumer_func)(void*, void*);
void _oval_test_message_consumer
	(struct oval_message *message, struct oval_result_test *test)
{
	set_oval_result_test_message(test, message);
}

void _oval_test_item_consumer
	(struct oval_result_item *item, struct oval_result_test *test)
{
	add_oval_result_test_item(test, item);
}

int _oval_result_test_binding_parse
	(xmlTextReaderPtr reader, struct oval_parser_context *context,
		struct oval_result_test *test)
{
	int return_code = 1;

	xmlChar *variable_id = xmlTextReaderGetAttribute(reader, "variable_id");

	struct oval_syschar_model *syschar_model = oval_results_model_syschar_model
		(context->results_model);
	struct oval_object_model *object_model = oval_syschar_model_object_model
		(syschar_model);
	struct oval_variable *variable = get_oval_variable_new
		(object_model, variable_id, OVAL_VARIABLE_UNKNOWN);

	xmlChar *value = xmlTextReaderValue(reader);

	struct oval_variable_binding *binding = oval_variable_binding_new(variable, value);
	add_oval_result_test_binding(test, binding);

	xmlFree(value);
	xmlFree(variable_id);

	return return_code;
}

int _oval_result_test_parse
	(xmlTextReaderPtr reader, struct oval_parser_context *context,
			struct oval_result_test *test)
{
	int return_code = 1;
	xmlChar *localName = xmlTextReaderLocalName(reader);

	if(OVAL_RESULT_TEST_DEBUG){
		char message[200]; *message = '\0';
		sprintf(message, "_oval_result_test_parse: parsing <%s>", localName);
		oval_parser_log_debug(context, message);
	}

	if      (strcmp(localName, "message")==0){
		return_code  = oval_message_parse_tag
			(reader, context, (oscap_consumer_func)_oval_test_message_consumer, test);
	}else if(strcmp(localName, "tested_item")==0){
		return_code = oval_result_item_parse_tag
			(reader, context,
				(oscap_consumer_func)_oval_test_item_consumer, test);
	}else if(strcmp(localName, "tested-variable")==0){
		return_code = _oval_result_test_binding_parse(reader, context, test);
	}else{
		char message[200]; *message = '\0';
		sprintf(message, "_oval_result_test_parse: TODO: <%s> not handled", localName);
		oval_parser_log_warn(context, message);
		return_code = oval_parser_skip_tag(reader, context);
	}

	free(localName);

	return return_code;
}

int oval_result_test_parse_tag
	(xmlTextReaderPtr reader, struct oval_parser_context *context,
			struct oval_result_system *system,
			oscap_consumer_func consumer, void *client)
{
	int return_code = 1;
	if(OVAL_RESULT_TEST_DEBUG){
		oval_parser_log_debug(context, "oval_result_test_parse: BEGIN");
	}

	xmlChar *test_id = xmlTextReaderGetAttribute(reader, "test_id");
	struct oval_result_test *test = oval_result_test_new(system, test_id);
	oval_result_enum result = oval_result_parse(reader, "result",0);
	set_oval_result_test_result(test, result);
	int veriable_instance = oval_parser_int_attribute(reader, "veriable_instance", 1);
	set_oval_result_test_variable_instance(test, veriable_instance);

	struct oval_test *ovaltst = oval_result_test_test(test);

	oval_existence_enum check_existence = oval_existence_parse
		(reader,"check_existence",AT_LEAST_ONE_EXISTS);
	oval_existence_enum tst_check_existence
		= oval_test_existence(ovaltst);
	if(tst_check_existence==EXISTENCE_UNKNOWN){
		set_oval_test_existence(ovaltst, check_existence);
	}else if(tst_check_existence!=tst_check_existence){
		char message[200];*message = '\0';
		sprintf
		(
				message,
				"oval_result_test_parse: @check_existence does not match\n"
				"    test_id = %s",
				test_id
		);
		oval_parser_log_warn(context, message);
	}

	oval_check_enum check = oval_check_parse
		(reader, "check",OVAL_CHECK_UNKNOWN);
	oval_check_enum tst_check
		= oval_test_check(ovaltst);
	if(tst_check==OVAL_CHECK_UNKNOWN){
		set_oval_test_check(ovaltst, check);
	}else if(tst_check!=check){
		char message[200];*message = '\0';
		sprintf
		(
				message,
				"oval_result_test_parse: @check does not match\n"
				"    test_id = %s",
				test_id
		);
		oval_parser_log_warn(context, message);
	}

	int version = oval_parser_int_attribute(reader, "version",0);
	int tst_version = oval_test_version(ovaltst);
	if(tst_version==0){
		set_oval_test_version(ovaltst, version);
	}else if(tst_version!=version){
		char message[200];*message = '\0';
		sprintf
		(
				message,
				"oval_result_test_parse: @version does not match\n"
				"    test_id = %s",
				test_id
		);
		oval_parser_log_warn(context, message);
	}

	return_code = oval_parser_parse_tag
		(reader, context, (oval_xml_tag_parser)_oval_result_test_parse, test);

	(*consumer)(test, client);
	if(OVAL_RESULT_TEST_DEBUG){
		oval_parser_log_debug(context, "oval_result_test_parse: END");
	}
	free(test_id);
	return return_code;
}
