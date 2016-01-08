/**
 * @file oval_sysItem.c
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
#include <string.h>

#include "oval_agent_api_impl.h"
#include "oval_system_characteristics_impl.h"
#include "adt/oval_collection_impl.h"
#include "oval_definitions_impl.h"
#include "common/util.h"
#include "common/debug_priv.h"

typedef struct oval_sysitem {
	//oval_family_enum family;
	struct oval_syschar_model *model;
	oval_subtype_t subtype;
	char *id;
	struct oval_collection *messages;
	struct oval_collection *sysents;
	oval_syschar_status_t status;
} oval_sysitem_t;				///< Represents a single <*_item> element

struct oval_sysitem *oval_sysitem_new(struct oval_syschar_model *model, const char *id)
{
	__attribute__nonnull__(model);
	oval_sysitem_t *sysitem;

	sysitem = (oval_sysitem_t *) oscap_alloc(sizeof(oval_sysitem_t));
	if (sysitem == NULL)
		return NULL;

	sysitem->id = oscap_strdup(id);
	sysitem->subtype = OVAL_SUBTYPE_UNKNOWN;
	sysitem->status = SYSCHAR_STATUS_UNKNOWN;
	sysitem->messages = oval_collection_new();
	sysitem->sysents = oval_collection_new();
	sysitem->model = model;

	oval_syschar_model_add_sysitem(model, sysitem);

	return sysitem;
}

struct oval_sysitem *oval_sysitem_clone(struct oval_syschar_model *new_model, struct oval_sysitem *old_item)
{
	struct oval_sysitem *new_item = oval_sysitem_new(new_model, oval_sysitem_get_id(old_item));

	struct oval_message_iterator *old_messages = oval_sysitem_get_messages(old_item);
	while (oval_message_iterator_has_more(old_messages)) {
		struct oval_message *old_message = oval_message_iterator_next(old_messages);
		struct oval_message *new_message = oval_message_clone(old_message);
		oval_sysitem_add_message(new_item, new_message);
	}
	oval_message_iterator_free(old_messages);

	oval_sysitem_set_status(new_item, oval_sysitem_get_status(old_item));
	oval_sysitem_set_subtype(new_item, oval_sysitem_get_subtype(old_item));

	struct oval_sysent_iterator *old_sysent_itr = oval_sysitem_get_sysents(old_item);
	while (oval_sysent_iterator_has_more(old_sysent_itr)) {
		struct oval_sysent *old_sysent = oval_sysent_iterator_next(old_sysent_itr);
		struct oval_sysent *new_sysent = oval_sysent_clone(new_model, old_sysent);
		oval_sysitem_add_sysent(new_item, new_sysent);
	}
	oval_sysent_iterator_free(old_sysent_itr);

	return new_item;
}

void oval_sysitem_free(struct oval_sysitem *sysitem)
{
	if (sysitem == NULL)
		return;

	oval_collection_free_items(sysitem->messages, (oscap_destruct_func) oval_message_free);
	oval_collection_free_items(sysitem->sysents, (oscap_destruct_func) oval_sysent_free);
	oscap_free(sysitem->id);

	sysitem->id = NULL;
	sysitem->sysents = NULL;
	sysitem->messages = NULL;
	oscap_free(sysitem);
}

bool oval_sysitem_iterator_has_more(struct oval_sysitem_iterator *oc_sysitem)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysitem);
}

struct oval_sysitem *oval_sysitem_iterator_next(struct oval_sysitem_iterator
						*oc_sysitem)
{
	return (struct oval_sysitem *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysitem);
}

void oval_sysitem_iterator_free(struct oval_sysitem_iterator
				*oc_sysitem)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_sysitem);
}

oval_subtype_t oval_sysitem_get_subtype(struct oval_sysitem *sysitem)
{
	__attribute__nonnull__(sysitem);

	return sysitem->subtype;
}

void oval_sysitem_set_subtype(struct oval_sysitem *sysitem, oval_subtype_t subtype)
{
	__attribute__nonnull__(sysitem);
	sysitem->subtype = subtype;
}

char *oval_sysitem_get_id(struct oval_sysitem *item)
{
	__attribute__nonnull__(item);
	return item->id;
}

struct oval_message_iterator *oval_sysitem_get_messages(struct oval_sysitem *item)
{
	__attribute__nonnull__(item);
	return (struct oval_message_iterator *)oval_collection_iterator(item->messages);
}

void oval_sysitem_add_message(struct oval_sysitem *item, struct oval_message *message)
{
	__attribute__nonnull__(item);
	oval_collection_add(item->messages, message);
}

struct oval_sysent_iterator *oval_sysitem_get_sysents(struct oval_sysitem *sysitem)
{
	__attribute__nonnull__(sysitem);
	return (struct oval_sysent_iterator *)oval_collection_iterator(sysitem->sysents);
}

void oval_sysitem_add_sysent(struct oval_sysitem *sysitem, struct oval_sysent *sysent)
{
	__attribute__nonnull__(sysitem);
	oval_collection_add(sysitem->sysents, sysent);
}

oval_syschar_status_t oval_sysitem_get_status(struct oval_sysitem *data)
{
	__attribute__nonnull__(data);

	return data->status;
}

void oval_sysitem_set_status(struct oval_sysitem *data, oval_syschar_status_t status)
{
	__attribute__nonnull__(data);
	data->status = status;
}

static void _oval_sysitem_parse_subtag_message_consumer(struct oval_message *message, void *sysitem)
{
	oval_sysitem_add_message(sysitem, message);
}

static void _oval_sysitem_parse_subtag_sysent_consumer(struct oval_sysent *sysent, void *sysitem)
{
	oval_sysitem_add_sysent(sysitem, sysent);
}

static int _oval_sysitem_parse_subtag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *client)
{
	struct oval_sysitem *sysitem = client;
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int return_code = 0;
	if (strcmp((const char *)OVAL_SYSCHAR_NAMESPACE, namespace) == 0) {
		/* This is a message */
		return_code = oval_message_parse_tag(reader, context, (oscap_consumer_func) _oval_sysitem_parse_subtag_message_consumer, sysitem);
	} else {
		/*typedef *(oval_sysent_consumer)(struct oval_sysent *, void* client); */
		return_code = oval_sysent_parse_tag(reader, context, _oval_sysitem_parse_subtag_sysent_consumer, sysitem);
	}
	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}

int oval_sysitem_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *usr)
{
	__attribute__nonnull__(context);

	char *tagname = (char *)xmlTextReaderLocalName(reader);
	oval_subtype_t subtype = oval_subtype_parse(reader);
	int return_code = 0;
	if (subtype != OVAL_SUBTYPE_UNKNOWN) {
		char *item_id = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
		struct oval_sysitem *sysitem = oval_syschar_model_get_new_sysitem(context->syschar_model, item_id);
		oscap_free(item_id);

		oval_sysitem_set_subtype(sysitem, subtype);

		oval_syschar_status_t status_enum = oval_syschar_status_parse(reader, "status", SYSCHAR_STATUS_EXISTS);
		oval_sysitem_set_status(sysitem, status_enum);

		return_code = oval_parser_parse_tag(reader, context, &_oval_sysitem_parse_subtag, sysitem);
	} else {
		dW("Unknown sysitem: %s", tagname);
		return_code = oval_parser_skip_tag(reader, context);
	}

        if (return_code != 0) {
                dW("Parsing of <%s> terminated by an error at line %d.", tagname, xmlTextReaderGetParserLineNumber(reader));
        }

	oscap_free(tagname);

	return return_code;
}


void oval_sysitem_to_dom(struct oval_sysitem *sysitem, xmlDoc * doc, xmlNode * parent)
{
	if (sysitem) {
		oval_subtype_t subtype = oval_sysitem_get_subtype(sysitem);
		if (subtype) {
			/* get item subtype */
			const char *subtype_text = oval_subtype_get_text(subtype);
			char tagname[strlen(subtype_text) + 6];
			sprintf(tagname, "%s_item", subtype_text);

			oval_family_t family = oval_subtype_get_family(subtype);

			/* search namespace & create child */
			xmlNs *ns_family = oval_family_to_namespace(family, (const char *) OVAL_SYSCHAR_NAMESPACE, doc, parent);
			xmlNode *tag_sysitem = xmlNewTextChild(parent, ns_family, BAD_CAST tagname, NULL);

			/* attributes */
			xmlNewProp(tag_sysitem, BAD_CAST "id", BAD_CAST oval_sysitem_get_id(sysitem));
			oval_syschar_status_t status_index = oval_sysitem_get_status(sysitem);
			const char *status = oval_syschar_status_get_text(status_index);
			xmlNewProp(tag_sysitem, BAD_CAST "status", BAD_CAST status);
			
			/* messages */
			struct oval_message_iterator *messages = oval_sysitem_get_messages(sysitem);
			while (oval_message_iterator_has_more(messages)) {
				struct oval_message *message = oval_message_iterator_next(messages);
				oval_message_to_dom(message, doc, tag_sysitem);
			}
			oval_message_iterator_free(messages);

			/* sysents */
			struct oval_sysent_iterator *sysent_itr = oval_sysitem_get_sysents(sysitem);
			while (oval_sysent_iterator_has_more(sysent_itr)) {
				struct oval_sysent *sysent = oval_sysent_iterator_next(sysent_itr);
				oval_sysent_to_dom(sysent, doc, tag_sysitem);
			}
			oval_sysent_iterator_free(sysent_itr);
		}
	}
}
