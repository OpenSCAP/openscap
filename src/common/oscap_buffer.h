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

#ifndef OSCAP_BUFFER_H_
#define OSCAP_BUFFER_H_
#include "util.h"

struct oscap_buffer;

/**
 * Create a new buffer.
 * @return pointer to a buffer on success, NULL on failure
 */
struct oscap_buffer *oscap_buffer_new(void);

/**
 * Free the buffer from memory.
 * @param s buffer
 */
void oscap_buffer_free(struct oscap_buffer *s);

/**
 * Append a single char at the end of a buffer.
 * @param s buffer
 * @param c to append
 */
void oscap_buffer_append_char(struct oscap_buffer *s, char c);

/**
 * Append multiple characters at the end of string. These characters can contains '\0'
 * @param s string
 * @param t binary data to append
 * @param append_length size of data to append in bytes
 */
void oscap_buffer_append_binary_data(struct oscap_buffer *s, const char *data, const size_t append_length);

/**
 * Append multiple characters at the end of buffer.
 * @param s buffer
 * @param t to append
 */
void oscap_buffer_append_string(struct oscap_buffer *s, const char *t);

/**
 * Get buffer data as pointer to char
 * @param s buffer
 * @return pointer to data
 */
char *oscap_buffer_get_raw(const struct oscap_buffer *s);

#endif
