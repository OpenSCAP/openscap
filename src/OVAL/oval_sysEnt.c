/**
 * @file oval_sysEnt.c
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
#include "oval_parser_impl.h"
#include "oval_definitions_impl.h"

#include "common/util.h"
#include "common/debug_priv.h"
#include "common/elements.h"

typedef struct oval_sysent {
	struct oval_syschar_model *model;
	char *name;
	char *value;
	struct oval_collection *record_fields;
	int mask;
	oval_datatype_t datatype;
	oval_syschar_status_t status;
} oval_sysent_t;

struct oval_sysent *oval_sysent_new(struct oval_syschar_model *model)
{
	oval_sysent_t *sysent = (oval_sysent_t *) malloc(sizeof(oval_sysent_t));
	if (sysent == NULL)
		return NULL;

	sysent->name = NULL;
	sysent->value = NULL;
	sysent->record_fields = NULL;
	sysent->status = SYSCHAR_STATUS_UNKNOWN;
	sysent->datatype = OVAL_DATATYPE_UNKNOWN;
	sysent->mask = 0;
	sysent->model = model;
	return sysent;
}

struct oval_sysent *oval_sysent_clone(struct oval_syschar_model *new_model, struct oval_sysent *old_item)
{
	struct oval_sysent *new_item = oval_sysent_new(new_model);

	char *old_value = oval_sysent_get_value(old_item);
	if (old_value) {
		oval_sysent_set_value(new_item, old_value);
	}

	char *old_name = oval_sysent_get_name(old_item);
	if (old_name) {
		oval_sysent_set_name(new_item, oscap_strdup(old_name));
	}

	oval_sysent_set_datatype(new_item, oval_sysent_get_datatype(old_item));
	oval_sysent_set_mask(new_item, oval_sysent_get_mask(old_item));
	oval_sysent_set_status(new_item, oval_sysent_get_status(old_item));

	return new_item;
}

void oval_sysent_free(struct oval_sysent *sysent)
{
	if (sysent == NULL)
		return;

	if (sysent->name != NULL)
		free(sysent->name);
	if (sysent->value != NULL)
		free(sysent->value);
	if (sysent->record_fields)
		oval_collection_free_items(sysent->record_fields, (oscap_destruct_func) oval_record_field_free);

	sysent->name = NULL;
	sysent->value = NULL;

	free(sysent);
}

bool oval_sysent_iterator_has_more(struct oval_sysent_iterator *oc_sysent)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysent);
}

struct oval_sysent *oval_sysent_iterator_next(struct oval_sysent_iterator *oc_sysent)
{
	return (struct oval_sysent *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysent);
}

void oval_sysent_iterator_free(struct oval_sysent_iterator *oc_sysent)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_sysent);
}

char *oval_sysent_get_name(struct oval_sysent *sysent)
{
	__attribute__nonnull__(sysent);

	return sysent->name;
}

oval_syschar_status_t oval_sysent_get_status(struct oval_sysent * sysent)
{
	__attribute__nonnull__(sysent);

	return sysent->status;
}

char *oval_sysent_get_value(struct oval_sysent *sysent)
{
	__attribute__nonnull__(sysent);

	return sysent->value;
}

struct oval_record_field_iterator *oval_sysent_get_record_fields(struct oval_sysent *sysent)
{
	if (!sysent->record_fields)
		return (struct oval_record_field_iterator *)
			oval_collection_iterator_new();
	return (struct oval_record_field_iterator *)
		oval_collection_iterator(sysent->record_fields);
}

oval_datatype_t oval_sysent_get_datatype(struct oval_sysent * sysent)
{
	__attribute__nonnull__(sysent);

	return sysent->datatype;
}

int oval_sysent_get_mask(struct oval_sysent *sysent)
{
	__attribute__nonnull__(sysent);

	return sysent->mask;
}

void oval_sysent_set_name(struct oval_sysent *sysent, char *name)
{
	__attribute__nonnull__(sysent);
	if (sysent->name != NULL)
		free(sysent->name);
	sysent->name = name;
}

void oval_sysent_set_status(struct oval_sysent *sysent, oval_syschar_status_t status)
{
	__attribute__nonnull__(sysent);
	sysent->status = status;
}

void oval_sysent_set_datatype(struct oval_sysent *sysent, oval_datatype_t datatype)
{
	__attribute__nonnull__(sysent);
	sysent->datatype = datatype;
}

void oval_sysent_set_mask(struct oval_sysent *sysent, int mask)
{
	__attribute__nonnull__(sysent);
	sysent->mask = mask;
}

void oval_sysent_set_value(struct oval_sysent *sysent, char *value)
{
	__attribute__nonnull__(sysent);
	if (sysent->value != NULL)
		free(sysent->value);
	sysent->value = oscap_strdup(value);
}

void oval_sysent_add_record_field(struct oval_sysent *sysent, struct oval_record_field *rf)
{
	if (sysent->record_fields == NULL)
		sysent->record_fields = oval_collection_new();
	oval_collection_add(sysent->record_fields, rf);
}

static void oval_sysent_value_consumer_(char *value, void *sysent)
{
	oval_sysent_set_value(sysent, value);
}

static void _oval_sysent_record_field_consumer(struct oval_record_field *rf,
					       struct oval_sysent *sysent)
{
	oval_sysent_add_record_field(sysent, rf);
}

static int _oval_sysent_parse_record_field(xmlTextReaderPtr reader,
					   struct oval_parser_context *context,
					   void *user)
{
	return oval_record_field_parse_tag(reader, context, (oscap_consumer_func)
					   &_oval_sysent_record_field_consumer,
					   user, OVAL_RECORD_FIELD_ITEM);
}

int oval_sysent_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context,
			  oval_sysent_consumer consumer, void *user)
{
	int ret, mask;
	char *tagname;
	struct oval_sysent *sysent;
	oval_datatype_t datatype;
	oval_syschar_status_t status;

	__attribute__nonnull__(context);

	tagname = (char *) xmlTextReaderLocalName(reader);
	if (!strcmp("#text", tagname)) {
		xmlFree(tagname);
		return 0;
	}

	sysent = oval_sysent_new(context->syschar_model);
	oval_sysent_set_name(sysent, tagname);

	mask = oval_parser_boolean_attribute(reader, "mask", 0);
	oval_sysent_set_mask(sysent, mask);

	datatype = oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
	oval_sysent_set_datatype(sysent, datatype);

	status = oval_syschar_status_parse(reader, "status", SYSCHAR_STATUS_EXISTS);
        oval_sysent_set_status(sysent, status);

	if (datatype == OVAL_DATATYPE_RECORD)
		ret = oval_parser_parse_tag(reader, context, &_oval_sysent_parse_record_field, sysent);
	else
		ret = oscap_parser_text_value(reader, &oval_sysent_value_consumer_, sysent);

	if (ret == 0)
		(*consumer) (sysent, user);

	return ret;
}


void oval_sysent_to_dom(struct oval_sysent *sysent, xmlDoc * doc, xmlNode * parent)
{
	struct oval_record_field_iterator *rf_itr;
	xmlNsPtr ent_ns = parent->ns;
	xmlNodePtr root_node = xmlDocGetRootElement(doc);
	xmlNode *sysent_tag = NULL;

	char *tagname = oval_sysent_get_name(sysent);
	char *content = oval_sysent_get_value(sysent);
	bool mask = oval_sysent_get_mask(sysent);

	/* omit the value in oval_results if mask=true */
	if (mask && !xmlStrcmp(root_node->name, BAD_CAST OVAL_ROOT_ELM_RESULTS)) {
		sysent_tag = xmlNewTextChild(parent, ent_ns, BAD_CAST tagname, BAD_CAST "");
	} else {
		xmlChar *encoded_content = xmlEncodeEntitiesReentrant(doc, BAD_CAST content);
		sysent_tag = xmlNewChild(parent, ent_ns, BAD_CAST tagname, encoded_content);
		xmlFree(encoded_content);
	}

	if (mask) {
		xmlNewProp(sysent_tag, BAD_CAST "mask", BAD_CAST "true");
	}

	oval_datatype_t datatype_index = oval_sysent_get_datatype(sysent);
	if (datatype_index != OVAL_DATATYPE_STRING) {
		xmlNewProp(sysent_tag, BAD_CAST "datatype", BAD_CAST oval_datatype_get_text(datatype_index));
	}

	oval_syschar_status_t status_index = oval_sysent_get_status(sysent);
	if (status_index != SYSCHAR_STATUS_EXISTS) {
		xmlNewProp(sysent_tag, BAD_CAST "status", BAD_CAST oval_syschar_status_get_text(status_index));
	}

	rf_itr = oval_sysent_get_record_fields(sysent);
	if (oval_record_field_iterator_has_more(rf_itr)) {
		xmlNsPtr field_ns = xmlSearchNsByHref(doc, sysent_tag, OVAL_SYSCHAR_NAMESPACE);
		if (field_ns == NULL) {
			field_ns = xmlNewNs(xmlDocGetRootElement(doc), OVAL_SYSCHAR_NAMESPACE, NULL);
		}

		while (oval_record_field_iterator_has_more(rf_itr)) {
			struct oval_record_field *rf;

			rf = oval_record_field_iterator_next(rf_itr);
			oval_record_field_to_dom(rf, mask, doc, sysent_tag, field_ns);
		}
	}
	oval_record_field_iterator_free(rf_itr);
}
