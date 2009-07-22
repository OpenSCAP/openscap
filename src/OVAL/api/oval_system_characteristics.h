/**
 * @file oval_system_characteristics.h
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

#ifndef OVAL_SYSTEM_CHARACTERISTICS
#define OVAL_SYSTEM_CHARACTERISTICS

#include "oval_definitions.h"

typedef enum {
	SYSCHAR_FLAG_UNKNOWN,
	SYSCHAR_FLAG_ERROR,
	SYSCHAR_FLAG_COMPLETE,
	SYSCHAR_FLAG_INCOMPLETE,
	SYSCHAR_FLAG_DOES_NOT_EXIST,
	SYSCHAR_FLAG_NOT_COLLECTED,
	SYSCHAR_FLAG_NOT_APPLICABLE
} oval_syschar_collection_flag_enum;

typedef enum{
	SYSCHAR_STATUS_UNKNOWN,
	SYSCHAR_STATUS_ERROR,
	SYSCHAR_STATUS_EXISTS,
	SYSCHAR_STATUS_DOES_NOT_EXIST,
	SYSCHAR_STATUS_NOT_COLLECTED
} oval_syschar_status_enum;

struct oval_sysint;
struct oval_iterator_sysint;

struct oval_sysinfo;
struct oval_iterator_sysinfo;

struct oval_sysdata;
struct oval_iterator_sysdata;

struct oval_sysitem;
struct oval_iterator_sysitem;

struct oval_syschar;
struct oval_iterator_syschar;

int oval_iterator_sysint_has_more(struct oval_iterator_sysint *);
struct oval_sysint *oval_iterator_sysint_next(struct oval_iterator_sysint *);

char *oval_sysint_name(struct oval_sysint *);
char *oval_sysint_ip_address(struct oval_sysint *);
char *oval_sysint_mac_address(struct oval_sysint *);

int oval_iterator_sysinfo_has_more(struct oval_iterator_sysinfo *);
struct oval_sysinfo *oval_iterator_sysinfo_next(struct oval_iterator_sysinfo *);

char *oval_sysinfo_os_name(struct oval_sysinfo *);
char *oval_sysinfo_os_version(struct oval_sysinfo *);
char *oval_sysinfo_os_architecture(struct oval_sysinfo *);
char *oval_sysinfo_primary_host_name(struct oval_sysinfo *);
struct oval_iterator_sysint *oval_sysinfo_interfaces(struct oval_sysinfo *);

int oval_iterator_sysdata_has_more(struct oval_iterator_sysdata *);
struct oval_sysdata *oval_iterator_sysdata_next(struct oval_iterator_sysdata *);

oval_subtype_enum oval_sysdata_subtype(struct oval_sysdata *);
char *oval_sysdata_id(struct oval_sysdata *);
oval_syschar_status_enum oval_sysdata_status(struct oval_sysdata *);
struct oval_iterator_sysitem *oval_sysdata_items(struct oval_sysdata *);
char *oval_sysdata_message(struct oval_sysdata *);
char *oval_sysdata_subtype_name(struct oval_sysdata *);
oval_message_level_enum oval_sysdata_message_level(struct oval_sysdata *);

int oval_iterator_sysitem_has_more(struct oval_iterator_sysitem *);
struct oval_sysitem *oval_iterator_sysitem_next(struct oval_iterator_sysitem *);

char *oval_sysitem_name(struct oval_sysitem *);
char *oval_sysitem_value(struct oval_sysitem *);
oval_syschar_status_enum oval_sysitem_status(struct oval_sysitem *);
oval_datatype_enum oval_sysitem_datatype(struct oval_sysitem *);
int oval_sysitem_mask(struct oval_sysitem *);

int oval_iterator_syschar_has_more(struct oval_iterator_syschar *);
struct oval_syschar *oval_iterator_syschar_next(struct oval_iterator_syschar *);

oval_syschar_collection_flag_enum oval_syschar_flag(struct oval_syschar *);
struct oval_iterator_message *oval_syschar_messages(struct oval_syschar *);
struct oval_sysinfo *oval_syschar_sysinfo(struct oval_syschar *);
struct oval_object *oval_syschar_object(struct oval_syschar *);
struct oval_iterator_variable_binding *oval_syschar_variable_bindings(struct
								      oval_syschar
								      *);
struct oval_iterator_sysdata *oval_syschar_sysdata(struct oval_syschar *);

#endif
