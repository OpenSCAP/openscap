/**
 * @file oval_resultItem.c
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
#include "oval_results_impl.h"
#include "oval_collection_impl.h"
#include "oval_system_characteristics_impl.h"

#define OVAL_RESULT_ITEM_DEBUG 1

typedef struct oval_result_item {
	oval_result_enum          result;
	struct oval_collection   *messages;
	struct oval_sysdata      *sysdata;
} oval_result_item_t;

struct oval_result_item *oval_result_item_new
	(struct oval_results_model *results_model, char *item_id)
{
	oval_result_item_t *item = (oval_result_item_t *)
		malloc(sizeof(oval_result_item_t));
	struct oval_syschar_model *syschar_model = oval_results_model_syschar_model(results_model);
	struct oval_sysdata *sysdata = get_oval_sysdata_new(syschar_model, item_id);

	item->sysdata = sysdata;
	item->messages = oval_collection_new();
	item->result = 0;

	return item;
}

void oval_result_item_free(struct oval_result_item *item){
	oval_collection_free_items
		(item->messages, (oscap_destruct_func)oval_message_free);

	item->messages     = NULL;
	item->result       = 0;
	item->sysdata      = NULL;
}

int oval_iterator_result_item_has_more(struct oval_iterator_result_item
				       *oc_result_item)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_result_item);
}

struct oval_result_item *oval_iterator_result_item_next(struct
							oval_iterator_result_item
							*oc_result_item)
{
	return (struct oval_result_item *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_result_item);
}

struct oval_sysdata *oval_result_item_sysdata(struct oval_result_item *item)
{
	return item->sysdata;
}


oval_result_enum oval_result_item_result(struct oval_result_item *item)
{
	return ((struct oval_result_item *)item)->result;
}

struct oval_iterator_message *oval_result_item_messages
	(struct oval_result_item *item)
{
	return (struct oval_iterator_message *)
		oval_collection_iterator(item->messages);
}

void set_oval_result_item_result(struct oval_result_item *item, oval_result_enum result)
{
	item->result = result;
}

void add_oval_result_item_message
	(struct oval_result_item *item, struct oval_message *message)
{
	oval_collection_add(item->messages, message);
}

void _oval_result_item_message_consumer
	(struct oval_message *message, struct oval_result_item *item)
{
	add_oval_result_item_message(item, message);
}

int _oval_result_item_message_parse
	(xmlTextReaderPtr reader, struct oval_parser_context* context,
			struct oval_result_item *item)
{
	return oval_message_parse_tag
		(reader, context,
			(oscap_consumer_func)_oval_result_item_message_consumer, item);
}

int oval_result_item_parse_tag
	(xmlTextReaderPtr reader, struct oval_parser_context *context,
		oscap_consumer_func consumer, void *user)
{
	int return_code = 0;

	xmlChar *item_id = xmlTextReaderGetAttribute(reader, "item_id");
	struct oval_result_item *item = oval_result_item_new
		(context->results_model, item_id);

	oval_result_enum result = oval_result_parse(reader, "result", 0);
	set_oval_result_item_result(item, result);

	if(OVAL_RESULT_ITEM_DEBUG){
		char message[200]; *message = '\0';
		sprintf
		(
				message,
				"oval_result_item_parse_tag:\n"
				"    item_id = %s\n"
				"    result  = %d",
				oval_sysdata_id(oval_result_item_sysdata(item)),
				oval_result_item_result(item)
		);
		oval_parser_log_debug(context, message);
	}

	return_code = oval_parser_parse_tag
		(reader, context,
				(oval_xml_tag_parser)_oval_result_item_message_parse, item);

	(*consumer)(item, user);

	free(item_id);
	return return_code;
}
