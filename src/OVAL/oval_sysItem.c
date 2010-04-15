/**
 * @file oval_sysItem.c
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
#include "../common/util.h"
#include "../common/public/debug.h"

typedef struct oval_sysitem {
	struct oval_syschar_model *model;
	char *name;
	char *value;
	int mask;
	oval_datatype_t datatype;
	oval_syschar_status_t status;
} oval_sysitem_t;

struct oval_sysitem *oval_sysitem_new(struct oval_syschar_model *model)
{
	oval_sysitem_t *sysitem = (oval_sysitem_t *) oscap_alloc(sizeof(oval_sysitem_t));
	if (sysitem == NULL)
		return NULL;

	sysitem->name = NULL;
	sysitem->value = NULL;
	sysitem->status = SYSCHAR_STATUS_UNKNOWN;
	sysitem->datatype = OVAL_DATATYPE_UNKNOWN;
	sysitem->mask = 0;
	sysitem->model = model;
	return sysitem;
}

bool oval_sysitem_is_valid(struct oval_sysitem * sysitem)
{
	return true;		//TODO
}

bool oval_sysitem_is_locked(struct oval_sysitem * sysitem)
{
	__attribute__nonnull__(sysitem);

	return oval_syschar_model_is_locked(sysitem->model);
}

struct oval_sysitem *oval_sysitem_clone(struct oval_syschar_model *new_model, struct oval_sysitem *old_item)
{
	struct oval_sysitem *new_item = oval_sysitem_new(new_model);

	char *old_value = oval_sysitem_get_value(old_item);
	if (old_value) {
		oval_sysitem_set_value(new_item, oscap_strdup(old_value));
	}

	char *old_name = oval_sysitem_get_name(old_item);
	if (old_name) {
		oval_sysitem_set_name(new_item, oscap_strdup(old_name));
	}

	oval_sysitem_set_datatype(new_item, oval_sysitem_get_datatype(old_item));
	oval_sysitem_set_mask(new_item, oval_sysitem_get_mask(old_item));
	oval_sysitem_set_status(new_item, oval_sysitem_get_status(old_item));

	return new_item;
}

void oval_sysitem_free(struct oval_sysitem *sysitem)
{
	if (sysitem == NULL)
		return;

	if (sysitem->name != NULL)
		oscap_free(sysitem->name);
	if (sysitem->value != NULL)
		oscap_free(sysitem->value);

	sysitem->name = NULL;
	sysitem->value = NULL;

	oscap_free(sysitem);
}

bool oval_sysitem_iterator_has_more(struct oval_sysitem_iterator *oc_sysitem)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysitem);
}

struct oval_sysitem *oval_sysitem_iterator_next(struct oval_sysitem_iterator *oc_sysitem)
{
	return (struct oval_sysitem *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysitem);
}

void oval_sysitem_iterator_free(struct oval_sysitem_iterator *oc_sysitem)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_sysitem);
}

char *oval_sysitem_get_name(struct oval_sysitem *sysitem)
{
	__attribute__nonnull__(sysitem);

	return sysitem->name;
}

oval_syschar_status_t oval_sysitem_get_status(struct oval_sysitem * sysitem)
{
	__attribute__nonnull__(sysitem);

	return sysitem->status;
}

char *oval_sysitem_get_value(struct oval_sysitem *sysitem)
{
	__attribute__nonnull__(sysitem);

	return sysitem->value;
}

oval_datatype_t oval_sysitem_get_datatype(struct oval_sysitem * sysitem)
{
	__attribute__nonnull__(sysitem);

	return sysitem->datatype;
}

int oval_sysitem_get_mask(struct oval_sysitem *sysitem)
{
	__attribute__nonnull__(sysitem);

	return sysitem->mask;
}

void oval_sysitem_set_name(struct oval_sysitem *sysitem, char *name)
{
	if (sysitem && !oval_sysitem_is_locked(sysitem)) {
		if (sysitem->name != NULL)
			oscap_free(sysitem->name);
		sysitem->name = oscap_strdup(name);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_sysitem_set_status(struct oval_sysitem *sysitem, oval_syschar_status_t status)
{
	if (sysitem && !oval_sysitem_is_locked(sysitem)) {
		sysitem->status = status;
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_sysitem_set_datatype(struct oval_sysitem *sysitem, oval_datatype_t datatype)
{
	if (sysitem && !oval_sysitem_is_locked(sysitem)) {
		sysitem->datatype = datatype;
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_sysitem_set_mask(struct oval_sysitem *sysitem, int mask)
{
	if (sysitem && !oval_sysitem_is_locked(sysitem)) {
		sysitem->mask = mask;
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_sysitem_set_value(struct oval_sysitem *sysitem, char *value)
{
	if (sysitem && !oval_sysitem_is_locked(sysitem)) {
		if (sysitem->value != NULL)
			oscap_free(sysitem->value);
		sysitem->value = oscap_strdup(value);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

static void oval_sysitem_value_consumer_(char *value, void *sysitem)
{
	oval_sysitem_set_value(sysitem, value);
}

int oval_sysitem_parse_tag(xmlTextReaderPtr reader,
			   struct oval_parser_context *context, oval_sysitem_consumer consumer, void *client)
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
	char *tagname = (char *)xmlTextReaderName(reader);
	if (strcmp("#text", tagname)) {
		struct oval_sysitem *sysitem = oval_sysitem_new(context->syschar_model);
		{		/*sysitem->name */
			oval_sysitem_set_name(sysitem, tagname);
		}
		{		/*sysitem->mask */
			int mask = oval_parser_boolean_attribute(reader, "mask", 0);
			oval_sysitem_set_mask(sysitem, mask);
		}
		{		/*sysitem->datatype */
			oval_datatype_t datatype = oval_datatype_parse(reader, "datatype", OVAL_DATATYPE_STRING);
			oval_sysitem_set_datatype(sysitem, datatype);
		}
		{		/*sysitem->status */
			oval_syschar_status_t status =
			    oval_syschar_status_parse(reader, "status", SYSCHAR_STATUS_EXISTS);
			oval_sysitem_set_status(sysitem, status);
		}
		{		/*sysitem->value */
			return_code = oval_parser_text_value(reader, context, &oval_sysitem_value_consumer_, sysitem);
		}
		if (return_code != 1) {
			oscap_dprintf("WARNING: oval_sysitem_parse_tag:: return code is not 1::(%d)", return_code);
		} else {
			int numchars = 0;
			char message[2000];
			message[numchars] = '\0';
			numchars = numchars + sprintf(message + numchars, "oval_sysitem_parse_tag::");
			numchars =
			    numchars + sprintf(message + numchars, "\n    sysitem->name     = %s",
					       oval_sysitem_get_name(sysitem));
			numchars =
			    numchars + sprintf(message + numchars, "\n    sysitem->mask     = %d",
					       oval_sysitem_get_mask(sysitem));
			numchars =
			    numchars + sprintf(message + numchars, "\n    sysitem->datatype = %d",
					       oval_sysitem_get_datatype(sysitem));
			numchars =
			    numchars + sprintf(message + numchars, "\n    sysitem->status   = %d",
					       oval_sysitem_get_status(sysitem));
			numchars =
			    numchars + sprintf(message + numchars, "\n    sysitem->value    = %s",
					       oval_sysitem_get_value(sysitem));
			oscap_dprintf("DEBUG: %s", message);	/* TODO: Make this as string ^ */
			(*consumer) (sysitem, client);
		}
	}
	oscap_free(tagname);
	return return_code;
}

void oval_sysitem_to_print(struct oval_sysitem *sysitem, char *indent, int idx)
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
		oscap_dprintf("%sNAME          = %s\n", nxtindent, oval_sysitem_get_name(sysitem));
	}
	{			/*id */
		oscap_dprintf("%sVALUE         = %s\n", nxtindent, oval_sysitem_get_value(sysitem));
	}
	{			/*mask */
		oscap_dprintf("%sMASK          = %d\n", nxtindent, oval_sysitem_get_mask(sysitem));
	}
	{			/*datatype */
		oscap_dprintf("%sDATATYPE      = %d\n", nxtindent, oval_sysitem_get_datatype(sysitem));
	}
	{			/*status */
		oscap_dprintf("%sSTATUS        = %d\n", nxtindent, oval_sysitem_get_status(sysitem));
	}
}

void oval_sysitem_to_dom(struct oval_sysitem *sysitem, xmlDoc * doc, xmlNode * parent)
{
	xmlNsPtr *ns_parent = xmlGetNsList(doc, parent);
	xmlNode *sysitem_tag =
	    xmlNewChild(parent, ns_parent[0], BAD_CAST oval_sysitem_get_name(sysitem),
			BAD_CAST oval_sysitem_get_value(sysitem));

	if(ns_parent)
		xmlFree(ns_parent);

	bool mask_value = oval_sysitem_get_mask(sysitem);
	if (mask_value) {
		xmlNewProp(sysitem_tag, BAD_CAST "mask", BAD_CAST "true");
	}

	oval_datatype_t datatype_index = oval_sysitem_get_datatype(sysitem);
	if (datatype_index != OVAL_DATATYPE_STRING) {
		xmlNewProp(sysitem_tag, BAD_CAST "datatype", BAD_CAST oval_datatype_get_text(datatype_index));
	}

	oval_syschar_status_t status_index = oval_sysitem_get_status(sysitem);
	if (status_index != SYSCHAR_STATUS_EXISTS) {
		xmlNewProp(sysitem_tag, BAD_CAST "status", BAD_CAST oval_syschar_status_get_text(status_index));
	}
}
