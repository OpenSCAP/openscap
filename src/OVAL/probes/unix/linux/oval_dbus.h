/*
 * Copyright 2023 Red Hat Inc., Durham, North Carolina.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      Evgenii Kolesnikov <ekolesni@redhat.com>
 */

#ifndef OPENSCAP_OVAL_DBUS_H_
#define OPENSCAP_OVAL_DBUS_H_

#include <dbus/dbus.h>


// Old versions of libdbus API don't have DBusBasicValue and DBus8ByteStruct
// as a public typedefs.
// These two typedefs were copied from libdbus 1.8 branch, see
// http://cgit.freedesktop.org/dbus/dbus/tree/dbus/dbus-types.h?h=dbus-1.8#n137
typedef struct
{
	dbus_uint32_t first32;
	dbus_uint32_t second32;
} _DBus8ByteStruct;

typedef union
{
	unsigned char bytes[8]; /**< as 8 individual bytes */
	dbus_int16_t  i16;   /**< as int16 */
	dbus_uint16_t u16;   /**< as int16 */
	dbus_int32_t  i32;   /**< as int32 */
	dbus_uint32_t u32;   /**< as int32 */
	dbus_bool_t   bool_val; /**< as boolean */
#ifdef DBUS_HAVE_INT64
	dbus_int64_t  i64;   /**< as int64 */
	dbus_uint64_t u64;   /**< as int64 */
#endif
	_DBus8ByteStruct eight; /**< as 8-byte struct */
	double dbl;          /**< as double */
	unsigned char byt;   /**< as byte */
	char *str;           /**< as char* (string, object path or signature) */
	int fd;              /**< as Unix file descriptor */
} _DBusBasicValue;


char *oval_dbus_value_to_string(DBusMessageIter *iter);

DBusConnection *oval_connect_dbus(void);

void oval_disconnect_dbus(DBusConnection *conn);

#endif
