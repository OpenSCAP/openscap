/**
 * @file oval_sysEnt.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
#include "oval_agent_api_impl.h"
#include "oval_system_characteristics_impl.h"
#include "oval_collection_impl.h"
#include "common/util.h"
#include "common/debug_priv.h"

typedef struct oval_sysent {
	struct oval_syschar_model *model;
	char *name;
	char *value;
	int mask;
	oval_datatype_t datatype;
	oval_syschar_status_t status;
} oval_sysent_t;

struct oval_sysent *oval_sysent_new(struct oval_syschar_model *model)
{
	oval_sysent_t *sysent = (oval_sysent_t *) oscap_alloc(sizeof(oval_sysent_t));
	if (sysent == NULL)
		return NULL;

	sysent->name = NULL;
	sysent->value = NULL;
	sysent->status = SYSCHAR_STATUS_UNKNOWN;
	sysent->datatype = OVAL_DATATYPE_UNKNOWN;
	sysent->mask = 0;
	sysent->model = model;
	return sysent;
}

bool oval_sysent_is_valid(struct oval_sysent * sysent)
{
	if (sysent == NULL) {
                oscap_dlprintf(DBG_W, "Argument is not valid: NULL.\n");
		return false;
        }

        if (oval_sysent_get_datatype(sysent) == OVAL_DATATYPE_UNKNOWN) {
                oscap_dlprintf(DBG_W, "Argument is not valid: datatype == OVAL_DATATYPE_UNKNOWN.\n");
                return false;
        }

	return true;
}

bool oval_sysent_is_locked(struct oval_sysent * sysent)
{
	__attribute__nonnull__(sysent);

	return oval_syschar_model_is_locked(sysent->model);
}

struct oval_sysent *oval_sysent_clone(struct oval_syschar_model *new_model, struct oval_sysent *old_item)
{
	struct oval_sysent *new_item = oval_sysent_new(new_model);

	char *old_value = oval_sysent_get_value(old_item);
	if (old_value) {
		oval_sysent_set_value(new_item, oscap_strdup(old_value));
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
		oscap_free(sysent->name);
	if (sysent->value != NULL)
		oscap_free(sysent->value);

	sysent->name = NULL;
	sysent->value = NULL;

	oscap_free(sysent);
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
	if (sysent && !oval_sysent_is_locked(sysent)) {
		if (sysent->name != NULL)
			oscap_free(sysent->name);
		sysent->name = oscap_strdup(name);
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_sysent_set_status(struct oval_sysent *sysent, oval_syschar_status_t status)
{
	if (sysent && !oval_sysent_is_locked(sysent)) {
		sysent->status = status;
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_sysent_set_datatype(struct oval_sysent *sysent, oval_datatype_t datatype)
{
	if (sysent && !oval_sysent_is_locked(sysent)) {
		sysent->datatype = datatype;
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_sysent_set_mask(struct oval_sysent *sysent, int mask)
{
	if (sysent && !oval_sysent_is_locked(sysent)) {
		sysent->mask = mask;
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_sysent_set_value(struct oval_sysent *sysent, char *value)
{
	if (sysent && !oval_sysent_is_locked(sysent)) {
		if (sysent->value != NULL)
			oscap_free(sysent->value);
		sysent->value = oscap_strdup(value);
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

static void oval_sysent_value_consumer_(char *value, void *sysent)
{
	oval_sysent_set_value(sysent, value);
}

int oval_sysent_parse_tag(xmlTextReaderPtr reader,
			   struct oval_parser_context *context, oval_sysent_consumer consumer, void *client)
{
	/*
	   char*              name;
	   int                mask;
	   oval_datatype_enum datatype;
	   oval_syschar_status_enum status;
	   char*              value;
	 */

	__attribute__nonnull__(context);

	int return_code = 1;
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	if (strcmp("#text", tagname)) {
		struct oval_sysent *sysent = oval_sysent_new(context->syschar_model);
		{		/*sysent->name */
			oval_sysent_set_name(sysent, tagname);
		}
		{		/*sysent->mask */
			int mask = oval_parser_boolean_attribute(reader, "mask", 0);
			oval_sysent_set_mask(sysent, mask);
		}
		{		/*sysent->datatype */
			oval_datatype_t datatype = oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
			oval_sysent_set_datatype(sysent, datatype);
		}
		{		/*sysent->status */
			oval_syschar_status_t status =
			    oval_syschar_status_parse(reader, "status", SYSCHAR_STATUS_EXISTS);
			oval_sysent_set_status(sysent, status);
		}
		{		/*sysent->value */
			return_code = oval_parser_text_value(reader, context, &oval_sysent_value_consumer_, sysent);
		}
		if (return_code != 1) {
			oscap_dlprintf(DBG_W, "Return code is not 1: %d.\n", return_code);
		} else {
			/*
			int numchars = 0;
			char message[2000];
			message[numchars] = '\0';
			numchars = numchars + sprintf(message + numchars, "oval_sysent_parse_tag::");
			numchars =
			    numchars + sprintf(message + numchars, "\n    sysent->name     = %s",
					       oval_sysent_get_name(sysent));
			numchars =
			    numchars + sprintf(message + numchars, "\n    sysent->mask     = %d",
					       oval_sysent_get_mask(sysent));
			numchars =
			    numchars + sprintf(message + numchars, "\n    sysent->datatype = %d",
					       oval_sysent_get_datatype(sysent));
			numchars =
			    numchars + sprintf(message + numchars, "\n    sysent->status   = %d",
					       oval_sysent_get_status(sysent));
			numchars =
			    numchars + sprintf(message + numchars, "\n    sysent->value    = %s",
					       oval_sysent_get_value(sysent));
			oscap_dprintf("DEBUG: %s", message);	// TODO: Make this as string ^ */
			(*consumer) (sysent, client);
		}
	}
	oscap_free(tagname);
	return return_code;
}

void oval_sysent_to_print(struct oval_sysent *sysent, char *indent, int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sSYSITEM.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sSYSITEM[%d].", indent, idx);

	/*
	   char*              name;
	   char*              value;
	   int                mask;
	   oval_datatype_enum datatype;
	   oval_syschar_status_enum status;
	 */
	{			/*id */
		oscap_dprintf("%sNAME          = %s\n", nxtindent, oval_sysent_get_name(sysent));
	}
	{			/*id */
		oscap_dprintf("%sVALUE         = %s\n", nxtindent, oval_sysent_get_value(sysent));
	}
	{			/*mask */
		oscap_dprintf("%sMASK          = %d\n", nxtindent, oval_sysent_get_mask(sysent));
	}
	{			/*datatype */
		oscap_dprintf("%sDATATYPE      = %d\n", nxtindent, oval_sysent_get_datatype(sysent));
	}
	{			/*status */
		oscap_dprintf("%sSTATUS        = %d\n", nxtindent, oval_sysent_get_status(sysent));
	}
}

void oval_sysent_to_dom(struct oval_sysent *sysent, xmlDoc * doc, xmlNode * parent)
{
	xmlNsPtr *ns_parent = xmlGetNsList(doc, parent);
	xmlNodePtr root_node = xmlDocGetRootElement(doc);

	xmlNode *sysent_tag = NULL;

	char *tagname = oval_sysent_get_name(sysent);
	char *content = oval_sysent_get_value(sysent);
	bool mask = oval_sysent_get_mask(sysent);

	/* omit the value in oval_results if mask=true */
	if(mask && !xmlStrcmp(root_node->name, (const xmlChar *) "oval_results")) {
		xmlNewTextChild(parent, ns_parent[0], BAD_CAST tagname, BAD_CAST "");
	} else {
		xmlNewTextChild(parent, ns_parent[0], BAD_CAST tagname, BAD_CAST content);
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

	if(ns_parent)
		xmlFree(ns_parent);
}

