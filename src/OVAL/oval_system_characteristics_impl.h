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

struct oval_sysint *oval_sysint_new();
struct oval_sysint *oval_sysint_clone(struct oval_sysint *);
void oval_sysint_free(struct oval_sysint *);

void oval_sysint_set_name(struct oval_sysint *, char *);
void oval_sysint_set_ip_address(struct oval_sysint *, char *);
void oval_sysint_set_mac_address(struct oval_sysint *, char *);
typedef void (*oval_sysint_consumer)(struct oval_sysint*, void*);
int oval_sysint_parse_tag
	(xmlTextReaderPtr, struct oval_parser_context*,
			oval_sysint_consumer, void*);
void oval_sysint_to_print(struct oval_sysint *, char *, int);
void oval_sysint_to_dom  (struct oval_sysint *, xmlDoc *, xmlNode *);

struct oval_sysinfo *oval_sysinfo_new();
struct oval_sysinfo *oval_sysinfo_clone(struct oval_sysinfo *);
void oval_sysinfo_free(struct oval_sysinfo *);
void oval_sysinfo_to_print(struct oval_sysinfo *, char *, int);
void oval_sysinfo_to_dom  (struct oval_sysinfo *, xmlDoc *, xmlNode *);
int oval_sysinfo_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *);

void oval_sysinfo_set_os_name(struct oval_sysinfo *, char *);
void oval_sysinfo_set_os_version(struct oval_sysinfo *, char *);
void oval_sysinfo_set_os_architecture(struct oval_sysinfo *, char *);
void oval_sysinfo_set_primary_host_name(struct oval_sysinfo *, char *);
void oval_sysinfo_add_interface(struct oval_sysinfo *, struct oval_sysint *);

struct oval_sysdata *oval_sysdata_new(char *id);
void oval_sysdata_free(struct oval_sysdata *);
void oval_sysdata_to_print(struct oval_sysdata *, char *, int);
void oval_sysdata_to_dom  (struct oval_sysdata *, xmlDoc *, xmlNode *);
int oval_sysdata_parse_tag(xmlTextReaderPtr, struct oval_parser_context *);

void oval_sysdata_set_status(struct oval_sysdata *, oval_syschar_status_t);
void oval_sysdata_set_subtype(struct oval_sysdata *sysdata, oval_subtype_t subtype);
void oval_sysdata_add_item(struct oval_sysdata *, struct oval_sysitem *);

struct oval_syschar *oval_syschar_new(struct oval_object *);
void oval_syschar_free(struct oval_syschar *);

void oval_syschar_add_messages(struct oval_syschar *, char *);
void oval_syschar_set_flag(struct oval_syschar *model, oval_syschar_collection_flag_t flag);
void oval_syschar_set_object(struct oval_syschar *, struct oval_object *);
void oval_syschar_add_variable_binding(struct oval_syschar *, struct oval_variable_binding *);
void oval_syschar_add_sysdata(struct oval_syschar *, struct oval_sysdata *);
void oval_syschar_to_dom  (struct oval_syschar *, xmlDoc *, xmlNode *);
int oval_syschar_parse_tag(xmlTextReaderPtr, struct oval_parser_context *context);
oval_syschar_collection_flag_t oval_syschar_flag_parse(xmlTextReaderPtr, char *, oval_syschar_collection_flag_t);
oval_syschar_status_t oval_syschar_status_parse(xmlTextReaderPtr, char *, oval_syschar_status_t);
char* oval_syschar_status_text(oval_syschar_status_t);

//typedef void (*oval_affected_consumer) (struct oval_affected *, void *);
//int oval_affected_parse_tag(xmlTextReaderPtr reader,
//			    struct oval_parser_context *context,
//			    oval_affected_consumer, void *);

struct oval_sysitem *oval_sysitem_new();
typedef void (*oval_sysitem_consumer)(struct oval_sysitem *, void* client);
int oval_sysitem_parse_tag(
		xmlTextReaderPtr,
	   struct oval_parser_context *,
	   oval_sysitem_consumer, void*);
void oval_sysitem_free(struct oval_sysitem *);
void oval_sysitem_to_print(struct oval_sysitem *, char *, int);
void oval_sysitem_set_name(struct oval_sysitem *sysitem, char *name);
void oval_sysitem_set_value(struct oval_sysitem *sysitem, char *value);
void oval_sysitem_set_status(struct oval_sysitem *sysitem, oval_syschar_status_t status);
void oval_sysitem_set_datatype(struct oval_sysitem *sysitem, oval_datatype_t type);
void oval_sysitem_set_mask(struct oval_sysitem *sysitem, int mask);
void oval_sysitem_to_dom  (struct oval_sysitem *sysitem, xmlDoc *doc, xmlNode *tag_parent);

const char *oval_syschar_collection_flag_get_text(oval_syschar_collection_flag_t);

OSCAP_HIDDEN_END;

#endif
