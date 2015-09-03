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

#include "oscap_string.h"
#include "oscap_buffer.h"


typedef struct oscap_buffer oscap_string;

struct oscap_string *oscap_string_new()
{
	return (struct oscap_string*) oscap_buffer_new();
}

void oscap_string_free(struct oscap_string *s)
{
	oscap_buffer_free((struct oscap_buffer*)s);
}

char* oscap_string_bequeath(struct oscap_string *s)
{
	return oscap_buffer_bequeath((struct oscap_buffer*)s);
}

void oscap_string_clear(struct oscap_string *s)
{
	oscap_buffer_clear((struct oscap_buffer*)s);
}

void oscap_string_append_char(struct oscap_string *s, char c)
{
	oscap_buffer_append_binary_data((struct oscap_buffer*)s, &c, 1);
}

void oscap_string_append_string(struct oscap_string *s, const char *t)
{
	oscap_buffer_append_string((struct oscap_buffer*)s,t);
}

const char *oscap_string_get_cstr(const struct oscap_string *s)
{
	return oscap_buffer_get_raw((struct oscap_buffer*)s);
}

bool oscap_string_empty(const struct oscap_string *s)
{
	return (oscap_buffer_get_length((struct oscap_buffer*)s) == 0);
}


