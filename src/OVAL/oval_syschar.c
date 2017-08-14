/**
 * @file oval_syschar.c
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
 *      Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oval_system_characteristics_impl.h"
#include "adt/oval_collection_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_definitions_impl.h"

#include "common/assume.h"
#include "common/util.h"
#include "common/debug_priv.h"

typedef struct oval_syschar {
	struct oval_syschar_model *model;
	oval_syschar_collection_flag_t flag;
	struct oval_collection *messages;
	struct oval_object *object;
	struct oval_collection *variable_bindings;	///< Represents <variable_value> elements
	struct oval_collection *sysitem;		///< Represents <reference> elements
	int variable_instance;				///< Represents variable_instance attribute
	int variable_instance_hint;			///< Internal hint of the next possible variable_instance attribute
} oval_syschar_t;					///< Represents a single collected <object> element

oval_syschar_collection_flag_t oval_syschar_get_flag(struct oval_syschar
						     *syschar)
{
	__attribute__nonnull__(syschar);

	return ((struct oval_syschar *)syschar)->flag;
}

void oval_syschar_set_flag(struct oval_syschar *syschar, oval_syschar_collection_flag_t flag) {
	__attribute__nonnull__(syschar);
	syschar->flag = flag;
}

void oval_syschar_set_object(struct oval_syschar *syschar, struct oval_object *object)
{
	__attribute__nonnull__(syschar);
	syschar->object = object;
}

struct oval_message_iterator *oval_syschar_get_messages(struct oval_syschar *syschar)
{
	return (struct oval_message_iterator *)oval_collection_iterator(syschar->messages);
}

void oval_syschar_add_message(struct oval_syschar *syschar, struct oval_message *message)
{
	__attribute__nonnull__(syschar);
	oval_collection_add(syschar->messages, message);
}

void oval_syschar_add_new_message(struct oval_syschar *syschar, char *text, oval_message_level_t level)
{
	__attribute__nonnull__(syschar);
	struct oval_message *msg;

	msg = oval_message_new();
	oval_message_set_text(msg, text);
	oval_message_set_level(msg, level);
	oval_collection_add(syschar->messages, msg);
}

struct oval_object *oval_syschar_get_object(struct oval_syschar *syschar)
{
	__attribute__nonnull__(syschar);

	return ((struct oval_syschar *)syschar)->object;
}

struct oval_syschar_model *oval_syschar_get_model(struct oval_syschar *syschar)
{
	__attribute__nonnull__(syschar);

	return ((struct oval_syschar *)syschar)->model;
}

struct oval_variable_binding_iterator *oval_syschar_get_variable_bindings(struct
									  oval_syschar
									  *syschar)
{
	__attribute__nonnull__(syschar);

	return (struct oval_variable_binding_iterator *)
	    oval_collection_iterator(syschar->variable_bindings);
}

struct oval_sysitem_iterator *oval_syschar_get_sysitem(struct oval_syschar *syschar)
{
	__attribute__nonnull__(syschar);

	return (struct oval_sysitem_iterator *)
	    oval_collection_iterator(syschar->sysitem);
}

void oval_syschar_add_sysitem(struct oval_syschar *syschar, struct oval_sysitem *sysitem) {
	__attribute__nonnull__(syschar);

	oval_collection_add(syschar->sysitem, sysitem);
}

void oval_syschar_add_variable_binding(struct oval_syschar *syschar, struct oval_variable_binding *binding) {
	__attribute__nonnull__(syschar);

	oval_collection_add(syschar->variable_bindings, binding);
}

struct oval_syschar *oval_syschar_new(struct oval_syschar_model *model, struct oval_object *object)
{
	__attribute__nonnull__(model);
	oval_syschar_t *syschar;

	syschar = (oval_syschar_t *) malloc(sizeof(oval_syschar_t));
	if (syschar == NULL)
		return NULL;

	syschar->flag = SYSCHAR_FLAG_UNKNOWN;
	syschar->variable_instance = 1;
	syschar->variable_instance_hint = 1;
	syschar->object = object;
	syschar->messages = oval_collection_new();
	syschar->sysitem = oval_collection_new();
	syschar->variable_bindings = oval_collection_new();
	syschar->model = model;

	oval_syschar_model_add_syschar(model, syschar);

	return syschar;
}

struct oval_syschar *oval_syschar_clone(struct oval_syschar_model *new_model, struct oval_syschar *old_syschar)
{
	struct oval_definition_model *def_model = oval_syschar_model_get_definition_model(new_model);
	struct oval_object *old_object = oval_syschar_get_object(old_syschar);
	struct oval_object *new_object = oval_definition_model_get_object(def_model, oval_object_get_id(old_object));
	if (new_object == NULL) {
		new_object = oval_object_clone(def_model, old_object);
	}

	struct oval_syschar *new_syschar = oval_syschar_new(new_model, new_object);

	oval_syschar_collection_flag_t flag = oval_syschar_get_flag(old_syschar);
	oval_syschar_set_flag(new_syschar, flag);
	int variable_instance = oval_syschar_get_variable_instance(old_syschar);
	oval_syschar_set_variable_instance(new_syschar, variable_instance);
	oval_syschar_set_variable_instance_hint(new_syschar, variable_instance);

	struct oval_message_iterator *old_messages = oval_syschar_get_messages(old_syschar);
	while (oval_message_iterator_has_more(old_messages)) {
		struct oval_message *old_message = oval_message_iterator_next(old_messages);
		struct oval_message *new_message = oval_message_clone(old_message);
		oval_syschar_add_message(new_syschar, new_message);
	}
	oval_message_iterator_free(old_messages);

	struct oval_sysitem_iterator *old_sysitems = oval_syschar_get_sysitem(old_syschar);
	while (oval_sysitem_iterator_has_more(old_sysitems)) {
		struct oval_sysitem *old_sysitem = oval_sysitem_iterator_next(old_sysitems);
		struct oval_sysitem *new_sysitem =
		    oval_syschar_model_get_sysitem(new_model, oval_sysitem_get_id(old_sysitem));
		if (new_sysitem == NULL)
			new_sysitem = oval_sysitem_clone(new_model, old_sysitem);
		oval_syschar_add_sysitem(new_syschar, new_sysitem);
	}
	oval_sysitem_iterator_free(old_sysitems);

	struct oval_variable_binding_iterator *old_bindings = oval_syschar_get_variable_bindings(old_syschar);
	while (oval_variable_binding_iterator_has_more(old_bindings)) {
		struct oval_variable_binding *old_binding = oval_variable_binding_iterator_next(old_bindings);
		struct oval_variable_binding *new_binding = oval_variable_binding_clone(old_binding, def_model);
		oval_syschar_add_variable_binding(new_syschar, new_binding);
	}
	oval_variable_binding_iterator_free(old_bindings);

	return new_syschar;
}

void oval_syschar_free(struct oval_syschar *syschar)
{
	if (syschar == NULL)
		return;

	oval_collection_free_items(syschar->messages, (oscap_destruct_func) oval_message_free);
	oval_collection_free_items(syschar->sysitem, NULL);	//sysitems are shared with syschar_model
	oval_collection_free_items(syschar->variable_bindings, (oscap_destruct_func) oval_variable_binding_free);

	syschar->messages = NULL;
	syschar->object = NULL;
	syschar->sysitem = NULL;
	syschar->variable_bindings = NULL;
	free(syschar);
}

static void add_oval_syschar_message(struct oval_syschar *syschar, struct oval_message *message) {
	__attribute__nonnull__(syschar);

	oval_collection_add(syschar->messages, message);
}

static void _oval_syschar_parse_subtag_consume_message(struct oval_message *message, void *syschar)
{
	add_oval_syschar_message((struct oval_syschar *)syschar, message);
}

struct oval_syschar_parse_subtag_varval_context {
	struct oval_syschar_model *model;
	struct oval_syschar *syschar;
};
static void _oval_syschar_parse_subtag_consume_variable_binding(struct oval_variable_binding *binding, void *user)
{
	struct oval_syschar_parse_subtag_varval_context *ctx = user;

	__attribute__nonnull__(ctx);

	oval_syschar_add_variable_binding(ctx->syschar, binding);
}

static int _oval_syschar_parse_subtag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *client)
{

	__attribute__nonnull__(context);

	struct oval_syschar *syschar = client;
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int return_code = 0;

	if (strcmp("message", tagname) == 0) {
		return_code = oval_message_parse_tag
		    (reader, context, (oscap_consumer_func) _oval_syschar_parse_subtag_consume_message, syschar);
	} else if (strcmp("variable_value", tagname) == 0) {
		struct oval_syschar_parse_subtag_varval_context ctx = { context->syschar_model, syschar };
		return_code = oval_variable_binding_parse_tag
		    (reader, context, &_oval_syschar_parse_subtag_consume_variable_binding, &ctx);
	} else if (strcmp("reference", tagname) == 0) {
		char *itemid = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "item_ref");
		struct oval_sysitem *sysitem = oval_syschar_model_get_new_sysitem(context->syschar_model, itemid);
		free(itemid);
		oval_syschar_add_sysitem(syschar, sysitem);
	}

	if (return_code != 0) {
                dW("Parsing of <%s> terminated by an error at line %d.", tagname, xmlTextReaderGetParserLineNumber(reader));
        }

	free(tagname);
	free(namespace);

	return return_code;
}

int oval_syschar_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *usr)
{
	__attribute__nonnull__(context);

	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int return_code = 0;

	int is_ovalsys = strcmp((const char *)OVAL_SYSCHAR_NAMESPACE, namespace) == 0;
	if (is_ovalsys && (strcmp(tagname, "object") == 0)) {
		char *object_id = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
		struct oval_object *object = oval_definition_model_get_new_object(context->definition_model, object_id);
		free(object_id);

		oval_syschar_t *syschar = oval_syschar_model_get_new_syschar(context->syschar_model, object);
		char *flag = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "flag");
		oval_syschar_collection_flag_t flag_enum = oval_syschar_flag_parse(reader, "flag", SYSCHAR_FLAG_UNKNOWN);
		free(flag);
		oval_syschar_set_flag(syschar, flag_enum);

		return_code = oval_parser_parse_tag(reader, context, &_oval_syschar_parse_subtag, syschar);
	} else {
                dW("Skipping tag: %s", tagname);
                oval_parser_skip_tag(reader, context);
	}

        if (return_code != 0) {
                dW("Parsing of <%s> terminated by an error at line %d.", tagname, xmlTextReaderGetParserLineNumber(reader));
        }

	free(tagname);
	free(namespace);

	return return_code;
}

void oval_syschar_to_dom(struct oval_syschar *syschar, xmlDoc * doc, xmlNode * tag_parent)
{

	if (syschar) {
		xmlNs *ns_syschar = xmlSearchNsByHref(doc, tag_parent, OVAL_SYSCHAR_NAMESPACE);
		xmlNode *tag_syschar = xmlNewTextChild(tag_parent, ns_syschar, BAD_CAST "object", NULL);

		{		/*attributes */
			struct oval_object *object = oval_syschar_get_object(syschar);
			xmlNewProp(tag_syschar, BAD_CAST "id", BAD_CAST oval_object_get_id(object));
			char version[17];
			snprintf(version, sizeof(version), "%d", oval_object_get_version(object));
			xmlNewProp(tag_syschar, BAD_CAST "version", BAD_CAST version);
			oval_syschar_collection_flag_t flag = oval_syschar_get_flag(syschar);
			xmlNewProp(tag_syschar, BAD_CAST "flag", BAD_CAST oval_syschar_collection_flag_get_text(flag));

			int instance = oval_syschar_get_variable_instance(syschar);
			if (instance != 1 ||
				oval_syschar_get_variable_instance_hint(syschar) != instance) {
				char instance_att[10] = "";
				snprintf(instance_att, sizeof(instance_att), "%d", instance);
				xmlNewProp(tag_syschar, BAD_CAST "variable_instance", BAD_CAST instance_att);
			}
		}
		{		/*messages */
			struct oval_message_iterator *messages = oval_syschar_get_messages(syschar);
			while (oval_message_iterator_has_more(messages)) {
				struct oval_message *message = oval_message_iterator_next(messages);
				oval_message_to_dom(message, doc, tag_syschar);
			}
			oval_message_iterator_free(messages);
		}
		{		/*variable values */
			struct oval_variable_binding_iterator *bindings = oval_syschar_get_variable_bindings(syschar);
			while (oval_variable_binding_iterator_has_more(bindings)) {
				struct oval_variable_binding *binding = oval_variable_binding_iterator_next(bindings);
				oval_variable_binding_to_dom(binding, doc, tag_syschar);
			}
			oval_variable_binding_iterator_free(bindings);
		}
		{		/*references */
			struct oval_sysitem_iterator *sysitems = oval_syschar_get_sysitem(syschar);
			while (oval_sysitem_iterator_has_more(sysitems)) {
				struct oval_sysitem *sysitem = oval_sysitem_iterator_next(sysitems);
				xmlNode *tag_reference = xmlNewTextChild
				    (tag_syschar, ns_syschar, BAD_CAST "reference", NULL);
				xmlNewProp(tag_reference, BAD_CAST "item_ref", BAD_CAST oval_sysitem_get_id(sysitem));
			}
			oval_sysitem_iterator_free(sysitems);
		}
	}
}

int oval_syschar_get_variable_instance(const struct oval_syschar *syschar)
{
	__attribute__nonnull__(syschar);
	return syschar->variable_instance;
}

void oval_syschar_set_variable_instance(struct oval_syschar *syschar, int variable_instance_in)
{
	__attribute__nonnull__(syschar);
	syschar->variable_instance = variable_instance_in;
}

int oval_syschar_get_variable_instance_hint(const struct oval_syschar *syschar)
{
	__attribute__nonnull__(syschar);
	return syschar->variable_instance_hint;
}

void oval_syschar_set_variable_instance_hint(struct oval_syschar *syschar, int variable_instance_hint_in)
{
	__attribute__nonnull__(syschar);
	syschar->variable_instance_hint = variable_instance_hint_in;
}

const char *oval_syschar_get_id(const struct oval_syschar *syschar)
{
	__attribute__nonnull__(syschar);
	struct oval_object *obj = oval_syschar_get_object((struct oval_syschar *) syschar);
	return (obj == NULL) ? NULL : oval_object_get_id(obj);
}
