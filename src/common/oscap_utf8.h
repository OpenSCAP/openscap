/*
 * Copyright 2026 Red Hat LLC, Raleigh, North Carolina.
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
 *      Jan Černý <jcerny@redhat.com>
 */

#ifndef OSCAP_UTF8_H_
#define OSCAP_UTF8_H_

#include <stddef.h>

/**
 * Sanitize a string by replacing invalid UTF-8 byte sequences with the
 * Unicode replacement character (U+FFFD).
 *
 * @param input the input string (not necessarily null-terminated)
 * @param input_len byte length of the input
 * @param output_len if not NULL, set to the byte length of the sanitized string
 * @return newly allocated null-terminated sanitized string if any invalid
 *         sequences were found (caller must free), or NULL if the input
 *         is already valid UTF-8
 */
char *oscap_sanitize_utf8(const char *input, size_t input_len, size_t *output_len);

#endif
