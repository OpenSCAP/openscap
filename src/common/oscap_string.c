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

#include <stdlib.h>
#include <string.h>
#include "oscap_string.h"

#define INITIAL_CAPACITY 64

/**
 * String with unlimited length
 * contains:
 * - pointer to data,
 * - actual length of string,
 * - capacity of allocated memory
 */
struct oscap_string {
	char *str;
	unsigned int length;
	unsigned int capacity;
};

struct oscap_string *oscap_string_new()
{
	struct oscap_string *s;
	s = oscap_alloc(sizeof(struct oscap_string));
	s->str = oscap_alloc(INITIAL_CAPACITY);
	s->str[0] = '\0';
	s->length = 0;
	s->capacity = INITIAL_CAPACITY;
	return s;
}

void oscap_string_free(struct oscap_string *s)
{
	oscap_free(s->str);
	oscap_free(s);
}

void oscap_string_append_char(struct oscap_string *s, char c)
{
	if (s == NULL)
		return;
	if ((s->length + 1) + 1 > s->capacity) {
		s->capacity += INITIAL_CAPACITY;
		s->str = oscap_realloc(s->str, s->capacity);
	}
	s->str[s->length++] = c;
	s->str[s->length] = '\0';
}

void oscap_string_append_string(struct oscap_string *s, const char *t)
{
	if (s == NULL || t == NULL)
		return;
	int append_length = strlen(t);
	if (s->length + append_length + 1 > s->capacity)  {
		s->capacity += append_length;
		s->str = oscap_realloc(s->str, s->capacity);
	}
	strcpy(s->str + s->length, t);
	s->length += append_length;
}

const char *oscap_string_get_cstr(const struct oscap_string *s)
{
	return s->str;
}
