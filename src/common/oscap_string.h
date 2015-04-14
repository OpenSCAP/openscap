/*
 * Copyright 2015 Red Hat Inc., Durham, North Carolina.
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
 *      Jan Černý <jcerny@redhat.com>
 */

#ifndef OSCAP_STRING_
#define OSCAP_STRING_
#include <stdlib.h>
#include <stdbool.h>
#include "util.h"

OSCAP_HIDDEN_START;

/**
 * String with unlimited length
 * definition of data type - contains:
 * - pointer to data,
 * - actual length of string,
 * - capacity of allocated memory
 */
typedef struct {
	char *str;
	unsigned int length;
	unsigned int capacity;
} oscap_string;

/**
 * Create a new string.
 * @return pointer to a string on success, NULL on failure
 */
oscap_string *oscap_string_new(void);

/**
 * Append a single char at the end of a string.
 * @param s string
 * @param c to append
 * @return true on succes, false on failure
 */
bool oscap_string_append_char(oscap_string *s, char c);

/**
 * Append multiple characters at the end of string.
 * @param s string
 * @param t to append
 * @return true on succes, false on failure
 */
bool oscap_string_append_string(oscap_string *s, const char *t);

OSCAP_HIDDEN_END;

#endif
