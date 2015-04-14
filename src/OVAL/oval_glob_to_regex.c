#include <stdlib.h>
#include "_oval_glob_to_regex.h"
#include "common/oscap_string.h"

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
