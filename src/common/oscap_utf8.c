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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "oscap_utf8.h"

static int _utf8_char_len(const uint8_t *s, size_t remaining)
{
	uint8_t b = s[0];

	if (b <= 0x7F)
		return 1;

	if (b >= 0xC2 && b <= 0xDF) {
		if (remaining < 2 || (s[1] & 0xC0) != 0x80)
			return -1;
		return 2;
	}

	if (b >= 0xE0 && b <= 0xEF) {
		if (remaining < 3 || (s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80)
			return -1;
		if (b == 0xE0 && s[1] < 0xA0)
			return -1;
		if (b == 0xED && s[1] > 0x9F)
			return -1;
		return 3;
	}

	if (b >= 0xF0 && b <= 0xF4) {
		if (remaining < 4 || (s[1] & 0xC0) != 0x80 ||
		    (s[2] & 0xC0) != 0x80 || (s[3] & 0xC0) != 0x80)
			return -1;
		if (b == 0xF0 && s[1] < 0x90)
			return -1;
		if (b == 0xF4 && s[1] > 0x8F)
			return -1;
		return 4;
	}

	return -1;
}

char *oscap_sanitize_utf8(const char *input, size_t input_len, size_t *output_len)
{
	const uint8_t *in = (const uint8_t *)input;
	size_t i = 0;

	while (i < input_len) {
		int clen = _utf8_char_len(in + i, input_len - i);
		if (clen < 0)
			break;
		i += clen;
	}

	if (i == input_len)
		return NULL;

	size_t alloc = input_len + 64;
	uint8_t *out = malloc(alloc + 1);
	if (out == NULL)
		return NULL;

	if (i > 0)
		memcpy(out, in, i);
	size_t o = i;

	while (i < input_len) {
		int clen = _utf8_char_len(in + i, input_len - i);
		if (clen < 0) {
			if (o + 3 > alloc) {
				if (alloc > SIZE_MAX / 2) {
					free(out);
					return NULL;
				}
				alloc = alloc * 2;
				uint8_t *tmp = realloc(out, alloc + 1);
				if (tmp == NULL) {
					free(out);
					return NULL;
				}
				out = tmp;
			}
			out[o++] = 0xEF;
			out[o++] = 0xBF;
			out[o++] = 0xBD;
			i++;
		} else {
			if (o + clen > alloc) {
				if (alloc > SIZE_MAX / 2) {
					free(out);
					return NULL;
				}
				alloc = alloc * 2;
				uint8_t *tmp = realloc(out, alloc + 1);
				if (tmp == NULL) {
					free(out);
					return NULL;
				}
				out = tmp;
			}
			memcpy(out + o, in + i, clen);
			o += clen;
			i += clen;
		}
	}

	out[o] = '\0';
	if (output_len != NULL)
		*output_len = o;
	return (char *)out;
}
