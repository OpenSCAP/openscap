/**
 * @file oval_glob_to_regex.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

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
 *      "Jan Černý" <jcerny@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include "oval_glob_to_regex.h"
#include "common/oscap_string.h"
#include "common/debug_priv.h"

typedef enum {
	START,
	NORMAL,
	LEFT_BRACKET,
	CLASS,
	ESCAPE,
	SLASH
} states;

char *oval_glob_to_regex (const char *glob, int noescape)
{
	oscap_string *regex;
	char * result;
	char c;
	int i = 0;
	int state = START;
	regex = oscap_string_new();
	if (regex == NULL) {
		return NULL;
	}
	oscap_string_append_char(regex, '^'); // regex must match whole string
	while(1) {
		c = glob[i++];
		switch (state) {
		case START:
			if (c == '\0') {
				goto finish;
			} else if (c== '/') {
				oscap_string_append_char(regex, c);
				state = SLASH;
			} else if (c == '?') {
				// a ? matches any single character, but
				// a ? at the begining of glob pattern can't match a .
				// a ? never matches a / (see man 7 glob - Pathnames)
				oscap_string_append_string(regex, "[^./]");
				state = NORMAL;
			} else if (c == '*') {
				// a * matches any string, but
				// a * at the begining of glob pattern can't match a .
				// a * never matches a / (see man 7 glob - Pathnames)
				oscap_string_append_string(regex, "(?=[^.])[^/]*");
				state = NORMAL;
			} else if (c == '.' || c == '|' || c == '^' || c == '(' || c == ')'
					|| c == '{' || c == '}' || c == '+' || c == '$') {
				oscap_string_append_char(regex, '\\');
				oscap_string_append_char(regex, c);
				state = NORMAL;
			} else if (c == '[') {
				oscap_string_append_char(regex,'[');
				state = LEFT_BRACKET;
			} else if (c == '\\') {
				if (noescape) {
					oscap_string_append_char(regex, '\\');
					oscap_string_append_char(regex, '\\');
					state = NORMAL;
				} else {
					state = ESCAPE;
				}
			} else {
				oscap_string_append_char(regex, c);
				state = NORMAL;
			}
			break;
		case NORMAL:
			if (c == '\0') {
				goto finish;
			} else if (c== '/') {
				oscap_string_append_char(regex, c);
				state = SLASH;
			} else if (c == '?') {
				// a ? matches any single character, but
				// it can never match a /
				oscap_string_append_string(regex, "[^/]");
			} else if (c == '*') {
				// a * matches any string, but
				// it can never match a /
				oscap_string_append_string(regex, "[^/]*");
			} else if (c == '.' || c == '|' || c == '^' || c == '(' || c == ')'
					|| c == '{' || c == '}' || c == '+' || c == '$' ) {
				oscap_string_append_char(regex, '\\');
				oscap_string_append_char(regex, c);
			} else if (c == '[') {
				oscap_string_append_char(regex,'[');
				state = LEFT_BRACKET;
			} else if (c == '\\') {
				if (noescape) {
					oscap_string_append_char(regex, '\\');
					oscap_string_append_char(regex, '\\');
				} else {
					state = ESCAPE;
				}
			} else {
				oscap_string_append_char(regex, c);
			}
			break;
		case LEFT_BRACKET:
			if (c == '!') {
				oscap_string_append_char(regex, '^');
			} else if (c == '\0') {
				free(regex->str);
				free(regex);
				return NULL;
			} else {
				oscap_string_append_char(regex, c);
			}
			state = CLASS;
			break;
		case CLASS:
			if (c == '\\') {
				if (noescape) {
					oscap_string_append_char(regex, '\\');
				}
			} else if (c == ']') {
				state = NORMAL;
			} else if (c == '\0') {
				free(regex->str);
				free(regex);
				return NULL;
			}
			oscap_string_append_char(regex, c);
			break;
		case ESCAPE:
			// ?, *, [ and ] are special characters, they must be escaped in glob.
			// A backslash is treated as an escape character only for these characters.
			// For all other characters the backslash is just an ordinary character.
			// Other characters, that are special in perl's regex,
			// are not special in a glob.
			if (c == '?' || c == '*' || c == '[' || c == ']') {
				oscap_string_append_char(regex, '\\');
				oscap_string_append_char(regex, c);
			} else if (c == '\0') {
				oscap_string_append_char(regex, '\\');
				oscap_string_append_char(regex, '\\');
				goto finish;
			} else {
				oscap_string_append_char(regex, '\\');
				oscap_string_append_char(regex, '\\');
				oscap_string_append_char(regex, c);
			}
			state = NORMAL;
			break;
		case SLASH:
			if (c == '\0') {
				goto finish;
			} else if (c == '?') {
				// a ? matches any single character, but
				// a ? at the begining of glob pattern can't match a .
				// a ? never matches a / (see man 7 glob - Pathnames)
				oscap_string_append_string(regex, "[^./]");
				state = NORMAL;
			} else if (c == '*') {
				// a * matches any string, but
				// a * at the begining of glob pattern can't match a .
				// a * never matches a / (see man 7 glob - Pathnames)
				oscap_string_append_string(regex, "(?=[^.])[^/]*");
				state = NORMAL;
			} else if (c == '.' || c == '|' || c == '^' || c == '(' || c == ')'
					|| c == '{' || c == '}' || c == '+' || c == '$' ) {
				oscap_string_append_char(regex, '\\');
				oscap_string_append_char(regex, c);
				state = NORMAL;
			} else if (c == '[') {
				state = LEFT_BRACKET;
			} else if (c == '\\') {
				if (noescape) {
					oscap_string_append_char(regex, '\\');
					oscap_string_append_char(regex, '\\');
					state = NORMAL;
				} else {
					state = ESCAPE;
				}
			} else {
				oscap_string_append_char(regex, c);
				state = NORMAL;
			}
			break;
		default:
			break;
		}
	}
finish:
	oscap_string_append_char(regex, '$'); // regex must match only whole string
	result = regex->str;
	free(regex);
	return result;
}
