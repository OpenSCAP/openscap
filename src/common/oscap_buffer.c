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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdlib.h>
#include <string.h>
#include "oscap_buffer.h"

#define INITIAL_CAPACITY 64

/**
 * Buffer with unlimited length
 * contains:
 * - pointer to data,
 * - actual length of string,
 * - capacity of allocated memory
 */
struct oscap_buffer {
	char *str;
	size_t length;
	size_t capacity;
};

struct oscap_buffer *oscap_buffer_new()
{
	struct oscap_buffer *s;
	s = oscap_alloc(sizeof(struct oscap_buffer));
	s->str = oscap_alloc(INITIAL_CAPACITY);
	s->str[0] = '\0';
	s->length = 0;
	s->capacity = INITIAL_CAPACITY;
	return s;
}

void oscap_buffer_clear(struct oscap_buffer *s)
{
	s->str[0] = '\0';
	s->length = 0;
}

void oscap_buffer_free(struct oscap_buffer *s)
{
	oscap_free(s->str);
	oscap_free(s);
}

char* oscap_buffer_bequeath(struct oscap_buffer *s){
	char* str = s->str;
	oscap_free(s);
	return str;
}

void oscap_buffer_append_binary_data(struct oscap_buffer *s, const char *data, const size_t append_length)
{
	if (s == NULL || data == NULL)
		return;
	if (s->length + append_length + 1 > s->capacity)  {
		/* Aligning allocated memory to multiples of INITIAL_CAPACITY.
		 * We pass to realloc the nearest greater muliple of INITIAL_CAPACITY
		 * rather than only needed capacity in order to not fragment
		 * the memory.
		 */
		s->capacity = ((s->capacity + append_length - 1) / INITIAL_CAPACITY + 1) * INITIAL_CAPACITY;
		s->str = oscap_realloc(s->str, s->capacity);
	}

	memcpy(s->str + s->length, data, append_length);
	s->length += append_length;
	s->str[s->length] = '\0';
}

void oscap_buffer_append_string(struct oscap_buffer *s, const char *t)
{
	if (t == NULL)
		return;
	size_t append_length = strlen(t);
	oscap_buffer_append_binary_data(s, t, append_length);
}

char *oscap_buffer_get_raw(const struct oscap_buffer *s)
{
	return s->str;
}

size_t oscap_buffer_get_length(const struct oscap_buffer *s){
	return s->length;
}


