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
#include "util.h"

/**
 * Create a new string.
 * @return pointer to a string on success, NULL on failure
 */
struct oscap_string *oscap_string_new(void);

/**
 * Free the string from memory.
 * @param s string
 */
void oscap_string_free(struct oscap_string *s);

/**
 * Append a single char at the end of a string.
 * @param s string
 * @param c to append
 */
void oscap_string_append_char(struct oscap_string *s, char c);

/**
 * Append multiple characters at the end of string.
 * @param s string
 * @param t to append
 */
void oscap_string_append_string(struct oscap_string *s, const char *t);

/**
 * Get string data as constant pointer to char
 * @param s string
 * @return pointer to data
 */
const char *oscap_string_get_cstr(const struct oscap_string *s);

OSCAP_HIDDEN_START;

/**
 * Returns true if the string has no characters; otherwise returns false
 * @param s string
 * @return true if empty
 */
bool oscap_string_empty(const struct oscap_string *s);

/**
 * Return pointer to internal string
 * Free oscap_string structure
 * @param s buffer
 */
char* oscap_string_bequeath(struct oscap_string *s);

/**
 * Erases the contents of the string. Length of string becomes 0
 * @param s string
 */
void oscap_string_clear(struct oscap_string *s);

OSCAP_HIDDEN_END;

#endif
