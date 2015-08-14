#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include "public/oval_version.h"

oval_version_t oval_version_from_cstr(const char *version_str_orig)
{
	const char *component_str[3];
	char version_str[12];
	long int component_int[3];
	size_t i, l;
	unsigned int c;
	bool found_digit;

	for (i = 0; i < 3; ++i) {
		component_str[i] = NULL;
		component_int[i] = 0;
	}

	if (version_str_orig == NULL)
		return OVAL_VERSION_INVALID;

	l = strlen(version_str_orig);
	found_digit = false;

	if (l > sizeof(version_str) - 1)
		return OVAL_VERSION_INVALID;
	else
		strncpy(version_str, version_str_orig, l);

	version_str[l] = '\0';
	component_str[0] = version_str;

	for (c = 0, i = 0; i < l; ++i) {
		if (isdigit(version_str[i])) {
			found_digit = true;
			continue;
		}

		if (version_str[i] == '.' && found_digit) {
			++c;
			if (c < (sizeof component_str)/sizeof(const char *)) {
				version_str[i] = '\0';
				component_str[c] = version_str + i + 1;
				found_digit = false;
			} else
				return OVAL_VERSION_INVALID;
		} else if (version_str[i] == '\0' && found_digit) {
			break;
		} else
			return OVAL_VERSION_INVALID;
	}

	for (i = 0; i <= c; ++i) {
		component_int[i] = strtol(component_str[i], NULL, 10);

		if (component_int[i] < 0 || component_int[i] > 255)
			return OVAL_VERSION_INVALID;
	}

	return \
		component_int[0] << 24 | \
		component_int[1] << 16 | \
		component_int[2] <<  8;
}

int oval_version_to_cstr(oval_version_t version, char *buffer, size_t buflen)
{
	uint8_t major, minor, patch;
	int s;

	major = oval_version_major(version);
	minor = oval_version_minor(version);
	patch = oval_version_patch(version);

	if (major == 0)
		return 1;

	if (patch != 0)
		s = snprintf(buffer, buflen, "%"PRIu8".%"PRIu8".%"PRIu8, major, minor, patch);
	else if (minor != 0)
		s = snprintf(buffer, buflen, "%"PRIu8".%"PRIu8, major, minor);
	else
		s = snprintf(buffer, buflen, "%"PRIu8, major);

	return s < 0 || (size_t)s >= buflen ? -1 : 0;
}

uint8_t oval_version_major(oval_version_t version)
{
	return (uint8_t)((version >> 24) & 0xFF);
}

uint8_t oval_version_minor(oval_version_t version)
{
	return (uint8_t)((version >> 16) & 0xFF);
}

uint8_t oval_version_patch(oval_version_t version)
{
	return (uint8_t)((version >> 8) & 0xFF);
}

int oval_version_cmp(oval_version_t v1, oval_version_t v2)
{
	return v1 == v2 ? 0 : v1 < v2 ? -1 : 1;
}
