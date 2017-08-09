/**
 * @file oval_resultItem.c
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "oval_agent_api_impl.h"
#include "results/oval_results_impl.h"
#include "adt/oval_collection_impl.h"
#include "oval_system_characteristics_impl.h"
#include "common/util.h"
#include "common/debug_priv.h"

typedef struct oval_result_item {
	struct oval_result_system *sys;
	oval_result_t result;
	struct oval_collection *messages;
	struct oval_sysitem *sysitem;
} oval_result_item_t;

struct oval_result_item *oval_result_item_new(struct oval_result_system *sys, char *item_id) {
	oval_result_item_t *item = (oval_result_item_t *)
	    oscap_alloc(sizeof(oval_result_item_t));
	if (item == NULL)
		return NULL;

	struct oval_syschar_model *syschar_model = oval_result_system_get_syschar_model(sys);
	struct oval_sysitem *sysitem = oval_syschar_model_get_new_sysitem(syschar_model, item_id);

	item->sysitem = sysitem;
	item->messages = oval_collection_new();
	item->result = OVAL_RESULT_NOT_EVALUATED;
	item->sys = sys;

	return item;
}

struct oval_result_item *oval_result_item_clone
    (struct oval_result_system *new_system, struct oval_result_item *old_item) {
	struct oval_sysitem *old_sysitem = oval_result_item_get_sysitem(old_item);
	char *datid = oval_sysitem_get_id(old_sysitem);
	struct oval_result_item *new_item = oval_result_item_new(new_system, datid);
	struct oval_message_iterator *old_messages = oval_result_item_get_messages(old_item);
	while (oval_message_iterator_has_more(old_messages)) {
		struct oval_message *old_message = oval_message_iterator_next(old_messages);
		struct oval_message *new_message = oval_message_clone(old_message);
		oval_result_item_add_message(new_item, new_message);
	}
	oval_message_iterator_free(old_messages);

	oval_result_item_set_result(old_item, oval_result_item_get_result(old_item));

	return new_item;
}

void oval_result_item_free(struct oval_result_item *item)
{
	if (item == NULL)
		return;

	oval_collection_free_items(item->messages, (oscap_destruct_func) oval_message_free);

	item->messages = NULL;
	item->result = OVAL_RESULT_NOT_EVALUATED;
	item->sysitem = NULL;

	free(item);
}

bool oval_result_item_iterator_has_more(struct oval_result_item_iterator * oc_result_item)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_result_item);
}

struct oval_result_item *oval_result_item_iterator_next(struct
							oval_result_item_iterator
							*oc_result_item)
{
	return (struct oval_result_item *)
	    oval_collection_iterator_next((struct oval_iterator *)
					  oc_result_item);
}

void oval_result_item_iterator_free(struct
				    oval_result_item_iterator
				    *oc_result_item)
{
	oval_collection_iterator_free((struct oval_iterator *)
				      oc_result_item);
}

struct oval_sysitem *oval_result_item_get_sysitem(struct oval_result_item *item)
{
	__attribute__nonnull__(item);

	return item->sysitem;
}

oval_result_t oval_result_item_get_result(struct oval_result_item * item)
{
	__attribute__nonnull__(item);

	return ((struct oval_result_item *)item)->result;
}

struct oval_message_iterator *oval_result_item_get_messages(struct oval_result_item *item) {
	__attribute__nonnull__(item);

	return (struct oval_message_iterator *)
	    oval_collection_iterator(item->messages);
}

void oval_result_item_set_result(struct oval_result_item *item, oval_result_t result)
{
	__attribute__nonnull__(item);
	item->result = result;
}

void oval_result_item_add_message(struct oval_result_item *item, struct oval_message *message) 
{
	__attribute__nonnull__(item);
	oval_collection_add(item->messages, message);
}

static void _oval_result_item_message_consumer(struct oval_message *message, struct oval_result_item *item) {
	oval_result_item_add_message(item, message);
}

static int _oval_result_item_message_parse
    (xmlTextReaderPtr reader, struct oval_parser_context *context, struct oval_result_item *item) {
	return oval_message_parse_tag(reader, context, (oscap_consumer_func) _oval_result_item_message_consumer, item);
}

int oval_result_item_parse_tag
    (xmlTextReaderPtr reader, struct oval_parser_context *context,
     struct oval_result_system *sys, oscap_consumer_func consumer, void *user) {
	int return_code = 0;

	xmlChar *item_id = xmlTextReaderGetAttribute(reader, BAD_CAST "item_id");
	struct oval_result_item *item = oval_result_item_new(sys, (char *)item_id);

	oval_result_t result = oval_result_parse(reader, "result", 0);
	oval_result_item_set_result(item, result);

	return_code = oval_parser_parse_tag
	    (reader, context, (oval_xml_tag_parser) _oval_result_item_message_parse, item);

	(*consumer) (item, user);

	free(item_id);
	return return_code;
}

xmlNode *oval_result_item_to_dom(struct oval_result_item * rslt_item, xmlDocPtr doc, xmlNode * parent) {
	xmlNs *ns_results = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	xmlNode *item_node = xmlNewTextChild(parent, ns_results, BAD_CAST "tested_item", NULL);

	struct oval_sysitem *oval_sysitem = oval_result_item_get_sysitem(rslt_item);
	char *item_id = oval_sysitem_get_id(oval_sysitem);
	xmlNewProp(item_node, BAD_CAST "item_id", BAD_CAST item_id);

	oval_result_t result = oval_result_item_get_result(rslt_item);
	xmlNewProp(item_node, BAD_CAST "result", BAD_CAST oval_result_get_text(result));

	return item_node;
}
