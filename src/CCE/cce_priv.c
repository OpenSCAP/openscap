/*
 * @file cce_priv.c
 * \brief Interface to Common Configuration Enumeration (CCE)
 *
 * See more details at http://cce.mitre.org/
 */

/*
 * Copyright 2008-2009 Red Hat Inc., Durham, North Carolina.
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 *      Riley C. Porter <Riley.Porter@g2-inc.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "cce_priv.h"

void process_description(xmlTextReaderPtr reader, struct cce_entry *cce)
{
	while (xmlTextReaderRead(reader)) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT &&
		    !xmlStrcmp(xmlTextReaderConstName(reader), BAD_CAST "description")) {
			return;
		}

		switch (xmlTextReaderNodeType(reader)) {
		case XML_READER_TYPE_TEXT:
			if (cce->description == NULL)
				cce->description = (char *)xmlTextReaderValue(reader);
			break;
		}
	}
	return;
}

void process_node(xmlTextReaderPtr reader, struct cce *cce_list)
{
	if (!xmlStrcmp(xmlTextReaderConstName(reader), (const xmlChar *)"item") && xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

		struct cce_entry *cce = cce_entry_new_empty();
		cce->id = (char *)xmlTextReaderGetAttribute(reader, (const xmlChar *)"id");
		if (!cce->id) {
			cce_entry_free(cce);
			return;
		}
		oscap_list_add(cce_list->entries, cce);
		oscap_htable_add(cce_list->entry, cce->id, cce);

		xmlTextReaderRead(reader);
		/* const xmlChar *name = xmlTextReaderConstName(reader); */
		/* int type = xmlTextReaderNodeType(reader); */
		while (xmlStrcmp(xmlTextReaderConstName(reader), (const xmlChar *)"item") != 0) {
			if (!xmlStrcmp(xmlTextReaderConstName(reader), (const xmlChar *)"description")
			    && xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
				process_description(reader, cce);
			} else if (!xmlStrcmp(xmlTextReaderConstName(reader), (const xmlChar *)"parameter")
				   && xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
				process_parameter(reader, cce);
			} else if (!xmlStrcmp(xmlTextReaderConstName(reader), (const xmlChar *)"technicalmechanisms")
				   && xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
				process_tech_mech(reader, cce);
			} else if (!xmlStrcmp(xmlTextReaderConstName(reader), (const xmlChar *)"ref")
				   && xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
				process_refs(reader, cce);
			}
			xmlTextReaderRead(reader);

			/* name = xmlTextReaderConstName(reader); */
			/* type = xmlTextReaderNodeType(reader); */
		}
	}
}

void process_parameter(xmlTextReaderPtr reader, struct cce_entry *cce)
{
	while (xmlTextReaderRead(reader)) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT &&
		    !xmlStrcmp(xmlTextReaderConstName(reader), (const xmlChar *)"parameter")) {
			return;
		}

		switch (xmlTextReaderNodeType(reader)) {
		case XML_READER_TYPE_TEXT:
			oscap_list_add(cce->params, xmlTextReaderValue(reader));
			break;
		}
	}
	return;
}

void process_refs(xmlTextReaderPtr reader, struct cce_entry *cce)
{
	xmlChar *source = NULL;
	xmlChar *value = NULL;

	source = xmlTextReaderGetAttribute(reader, (const xmlChar *)"source");
	while (xmlTextReaderRead(reader)) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT &&
		    !xmlStrcmp(xmlTextReaderConstName(reader), (const xmlChar *)"ref")) {
			struct cce_reference *ref = oscap_calloc(1, sizeof(struct cce_reference));
			ref->source = (char *)source;
			ref->value = (char *)value;
			oscap_list_add(cce->references, ref);
			return;
		}

		switch (xmlTextReaderNodeType(reader)) {
		case XML_READER_TYPE_TEXT:
			value = xmlTextReaderValue(reader);
			break;
		}
	}
	return;
}

void process_tech_mech(xmlTextReaderPtr reader, struct cce_entry *cce)
{
	while (xmlTextReaderRead(reader)) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT &&
		    !xmlStrcmp(xmlTextReaderConstName(reader), (const xmlChar *)"technicalmechanisms")) {
			return;
		}

		switch (xmlTextReaderNodeType(reader)) {
		case XML_READER_TYPE_TEXT:
			oscap_list_add(cce->tech_mechs, xmlTextReaderValue(reader));
			break;
		}
	}
	return;
}

void cce_reference_free(struct cce_reference *ref)
{
	if (ref) {
		free(ref->source);
		free(ref->value);
		free(ref);
	}
}
