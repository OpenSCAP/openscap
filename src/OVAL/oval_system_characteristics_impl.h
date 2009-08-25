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

#include "api/oval_system_characteristics.h"
#include "oval_definitions_impl.h"


struct oval_sysint *oval_sysint_new();
void oval_sysint_free(struct oval_sysint *);

void set_oval_sysint_name(struct oval_sysint *, char *);
void set_oval_sysint_ip_address(struct oval_sysint *, char *);
void set_oval_sysint_mac_address(struct oval_sysint *, char *);
typedef void (*oval_sysint_consumer)(struct oval_sysint*, void*);
int oval_sysint_parse_tag
	(xmlTextReaderPtr, struct oval_parser_context*,
			oval_sysint_consumer, void*);
void oval_sysint_to_print(struct oval_sysint *, char *, int);
void oval_sysint_to_dom  (struct oval_sysint *, xmlDoc *, xmlNode *);

struct oval_sysinfo *oval_sysinfo_new();
void oval_sysinfo_free(struct oval_sysinfo *);
void oval_sysinfo_to_print(struct oval_sysinfo *, char *, int);
void oval_sysinfo_to_dom  (struct oval_sysinfo *, xmlDoc *, xmlNode *);
int oval_sysinfo_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *);

void set_oval_sysinfo_os_name(struct oval_sysinfo *, char *);
void set_oval_sysinfo_os_version(struct oval_sysinfo *, char *);
void set_oval_sysinfo_os_architecture(struct oval_sysinfo *, char *);
void set_oval_sysinfo_primary_host_name(struct oval_sysinfo *, char *);
void add_oval_sysinfo_interface(struct oval_sysinfo *, struct oval_sysint *);

struct oval_sysdata *oval_sysdata_new(char *id);
void oval_sysdata_free(struct oval_sysdata *);
void oval_sysdata_to_print(struct oval_sysdata *, char *, int);
void oval_sysdata_to_dom  (struct oval_sysdata *, xmlDoc *, xmlNode *);
int oval_sysdata_parse_tag(xmlTextReaderPtr, struct oval_parser_context *);

void set_oval_sysdata_status(struct oval_sysdata *, oval_syschar_status_enum);
void set_oval_sysdata_subtype(struct oval_sysdata *sysdata, oval_subtype_enum subtype);
void set_oval_sysdata_subtype_name(struct oval_sysdata *data, char *name);
void add_oval_sysdata_item(struct oval_sysdata *, struct oval_sysitem *);

struct oval_syschar *oval_syschar_new(struct oval_object *);
void oval_syschar_free(struct oval_syschar *);

void set_oval_syschar_flag(struct oval_syschar *,
			   oval_syschar_collection_flag_enum);
void add_oval_syschar_messages(struct oval_syschar *, char *);
void set_oval_syschar_sysinfo(struct oval_syschar *, struct oval_sysinfo *);
void set_oval_syschar_object(struct oval_syschar *, struct oval_object *);
void add_oval_syschar_variable_binding(struct oval_syschar *,
				       struct oval_variable_binding *);
void add_oval_syschar_sysdata(struct oval_syschar *, struct oval_sysdata *);
void oval_syschar_to_print(struct oval_syschar *, char *, int);
void oval_syschar_to_dom  (struct oval_syschar *, xmlDoc *, xmlNode *);
int oval_syschar_parse_tag(xmlTextReaderPtr, struct oval_parser_context *context);
oval_syschar_collection_flag_enum oval_syschar_flag_parse(xmlTextReaderPtr, char *,
                       oval_syschar_collection_flag_enum);
oval_syschar_status_enum oval_syschar_status_parse(xmlTextReaderPtr, char *,
                       oval_syschar_status_enum);
char* oval_syschar_status_text(oval_syschar_status_enum);

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
void set_oval_sysitem_name(struct oval_sysitem *sysitem, char *name);
void set_oval_sysitem_value(struct oval_sysitem *sysitem, char *value);
void set_oval_sysitem_status(struct oval_sysitem *sysitem, oval_syschar_status_enum status);
void set_oval_sysitem_datatype(struct oval_sysitem *sysitem, oval_datatype_enum type);
#endif
