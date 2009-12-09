/**
 * @file oval_system_characteristics_impl.h
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

#ifndef OVAL_SYSCHAR_IMPL
#define OVAL_SYSCHAR_IMPL

#include "public/oval_system_characteristics.h"
#include "oval_definitions_impl.h"
#include "../common/util.h"

OSCAP_HIDDEN_START;

extern const char NAMESPACE_OVALSYS[];

typedef void (*oval_sysint_consumer)(struct oval_sysint*, void*);
int oval_sysint_parse_tag
	(xmlTextReaderPtr, struct oval_parser_context*,
			oval_sysint_consumer, void*);
void oval_sysint_to_print(struct oval_sysint *, char *, int);
void oval_sysint_to_dom  (struct oval_sysint *, xmlDoc *, xmlNode *);

void oval_sysinfo_to_print(struct oval_sysinfo *, char *, int);
void oval_sysinfo_to_dom  (struct oval_sysinfo *, xmlDoc *, xmlNode *);
int oval_sysinfo_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *);

void oval_sysdata_to_print(struct oval_sysdata *, char *, int);
void oval_sysdata_to_dom  (struct oval_sysdata *, xmlDoc *, xmlNode *);
int oval_sysdata_parse_tag(xmlTextReaderPtr, struct oval_parser_context *);

void oval_syschar_to_dom  (struct oval_syschar *, xmlDoc *, xmlNode *);
int oval_syschar_parse_tag(xmlTextReaderPtr, struct oval_parser_context *context);
oval_syschar_collection_flag_t oval_syschar_flag_parse(xmlTextReaderPtr, char *, oval_syschar_collection_flag_t);
oval_syschar_status_t oval_syschar_status_parse(xmlTextReaderPtr, char *, oval_syschar_status_t);

typedef void (*oval_sysitem_consumer)(struct oval_sysitem *, void* client);
int oval_sysitem_parse_tag(
		xmlTextReaderPtr,
	   struct oval_parser_context *,
	   oval_sysitem_consumer, void*);
void oval_sysitem_to_dom  (struct oval_sysitem *sysitem, xmlDoc *doc, xmlNode *tag_parent);
void oval_sysitem_to_print(struct oval_sysitem *, char *, int);

OSCAP_HIDDEN_END;

#endif
