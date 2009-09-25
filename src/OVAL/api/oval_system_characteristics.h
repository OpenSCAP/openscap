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

/**
 * @addtogroup OVAL
 * @{
 * @addtogroup syschar System Characteristics
 * @{
 * @file
 * OVAL System Characteristics interface
 * @author "David Niemoller" <David.Niemoller@g2-inc.com>
 */


#ifndef OVAL_SYSTEM_CHARACTERISTICS
#define OVAL_SYSTEM_CHARACTERISTICS

#include "oval_definitions.h"

/// System characteristics result flag
typedef enum {
	SYSCHAR_FLAG_UNKNOWN         = 0,
	SYSCHAR_FLAG_ERROR           = 1,
	SYSCHAR_FLAG_COMPLETE        = 2,
	SYSCHAR_FLAG_INCOMPLETE      = 3,
	SYSCHAR_FLAG_DOES_NOT_EXIST  = 4,
	SYSCHAR_FLAG_NOT_COLLECTED   = 5,
	SYSCHAR_FLAG_NOT_APPLICABLE  = 6
} oval_syschar_collection_flag_t;

/// System characteristics status
typedef enum{
	SYSCHAR_STATUS_UNKNOWN        = 0,
	SYSCHAR_STATUS_ERROR          = 1,
	SYSCHAR_STATUS_EXISTS         = 2,
	SYSCHAR_STATUS_DOES_NOT_EXIST = 3,
	SYSCHAR_STATUS_NOT_COLLECTED  = 4
} oval_syschar_status_t;

/**
 * Network interface desription.
 */
struct oval_sysint;
struct oval_sysint_iterator;

/**
 * System information.
 */
struct oval_sysinfo;
struct oval_sysinfo_iterator;

/**
 * System characteristics data entry.
 */
struct oval_sysdata;
struct oval_sysdata_iterator;

/**
 * System characteristics item.
 */
struct oval_sysitem;
struct oval_sysitem_iterator;

/**
 * Collection of characteristics bound to an object.
 */
struct oval_syschar;
struct oval_syschar_iterator;

int                 oval_sysint_iterator_has_more(struct oval_sysint_iterator *);
struct oval_sysint *oval_sysint_iterator_next    (struct oval_sysint_iterator *);
void                oval_sysint_iterator_free    (struct oval_sysint_iterator *);

/**
 * Get interface name.
 * @relates oval_sysint
 */
char *oval_sysint_get_name       (struct oval_sysint *);

/**
 * Get interface IP address.
 * @relates oval_sysint
 */
char *oval_sysint_get_ip_address (struct oval_sysint *);

/**
 * Get interface MAC address.
 * @relates oval_sysint
 */
char *oval_sysint_get_mac_address(struct oval_sysint *);

int                  oval_sysinfo_iterator_has_more(struct oval_sysinfo_iterator *);
struct oval_sysinfo *oval_sysinfo_iterator_next    (struct oval_sysinfo_iterator *);
void                 oval_sysinfo_iterator_free    (struct oval_sysinfo_iterator *);

/**
 * Get operating system name.
 * @relates oval_sysinfo
 */
char                        *oval_sysinfo_get_os_name          (struct oval_sysinfo *);

/**
 * Get operating system version.
 * @relates oval_sysinfo
 */
char                        *oval_sysinfo_get_os_version       (struct oval_sysinfo *);

/**
 * Get operating system architecture.
 * @relates oval_sysinfo
 */
char                        *oval_sysinfo_get_os_architecture  (struct oval_sysinfo *);

/**
 * Get primary host name of the tested machine.
 * @relates oval_sysinfo
 */
char                        *oval_sysinfo_get_primary_host_name(struct oval_sysinfo *);

/**
 * Get an iterator to the list of network interfaces.
 * @relates oval_sysinfo
 */
struct oval_sysint_iterator *oval_sysinfo_get_interfaces       (struct oval_sysinfo *);

int                  oval_sysdata_iterator_has_more(struct oval_sysdata_iterator *);
struct oval_sysdata *oval_sysdata_iterator_next    (struct oval_sysdata_iterator *);
void                 oval_sysdata_iterator_free    (struct oval_sysdata_iterator *);

/**
 * Get system data subtype.
 * @relates oval_sysdata
 */
oval_subtype_t                oval_sysdata_get_subtype      (struct oval_sysdata *);

/**
 * Get system data ID.
 * @relates oval_sysdata
 */
char                         *oval_sysdata_get_id           (struct oval_sysdata *);

/**
 * Get system data status.
 * @relates oval_sysdata
 */
oval_syschar_status_t         oval_sysdata_get_status       (struct oval_sysdata *);

/**
 * Get system data individual items.
 * @relates oval_sysdata
 */
struct oval_sysitem_iterator *oval_sysdata_get_items        (struct oval_sysdata *);

/**
 * Get system data message.
 * @relates oval_sysdata
 */
char                         *oval_sysdata_get_message      (struct oval_sysdata *);

/**
 * Get system data message level.
 * @relates oval_sysdata
 */
oval_message_level_t          oval_sysdata_get_message_level(struct oval_sysdata *);

int                  oval_sysitem_iterator_has_more(struct oval_sysitem_iterator *);
struct oval_sysitem *oval_sysitem_iterator_next    (struct oval_sysitem_iterator *);
void                 oval_sysitem_iterator_free    (struct oval_sysitem_iterator *);

/**
 * Get system data item name.
 * @relates oval_sysitem
 */
char                    *oval_sysitem_get_name    (struct oval_sysitem *);

/**
 * Get system data item value.
 * @relates oval_sysitem
 */
char                    *oval_sysitem_get_value   (struct oval_sysitem *);

/**
 * Get system data item status.
 * @relates oval_sysitem
 */
oval_syschar_status_t    oval_sysitem_get_status  (struct oval_sysitem *);

/**
 * Get system data item data type.
 * @relates oval_sysitem
 */
oval_datatype_t          oval_sysitem_get_datatype(struct oval_sysitem *);

/**
 * Get system data item mask.
 * @relates oval_sysitem
 */
int                      oval_sysitem_get_mask    (struct oval_sysitem *);

int                  oval_syschar_iterator_has_more(struct oval_syschar_iterator *);
struct oval_syschar *oval_syschar_iterator_next    (struct oval_syschar_iterator *);
void                 oval_syschar_iterator_free    (struct oval_syschar_iterator *);

/**
 * Get system characteristic flag.
 * @relates oval_syschar
 */
oval_syschar_collection_flag_t         oval_syschar_get_flag             (struct oval_syschar *);

/**
 * Get messages bound to this system characteristic.
 * @relates oval_syschar
 */
struct oval_message_iterator          *oval_syschar_get_messages         (struct oval_syschar *);

/**
 * Get characteristic system information.
 * @relates oval_syschar
 */
struct oval_sysinfo                   *oval_syschar_get_sysinfo          (struct oval_syschar *);

/**
 * Get object associated with this system characteristic.
 * @relates oval_syschar
 */
struct oval_object                    *oval_syschar_get_object           (struct oval_syschar *);

/**
 * Get system characteristic variable bindings.
 * @relates oval_syschar
 */
struct oval_variable_binding_iterator *oval_syschar_get_variable_bindings(struct oval_syschar *);

/**
 * Get system characteristic data.
 * @relates oval_syschar
 */
struct oval_sysdata_iterator          *oval_syschar_sysdata              (struct oval_syschar *);

const char *oval_syschar_collection_flag_get_text(oval_syschar_collection_flag_t flag);

#endif
