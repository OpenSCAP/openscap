#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "common/public/oscap.h"
#include "common/public/oscap_debug.h"
#include "common/public/oscap_error.h"

int main(void)
{
	oscap_init();
	oscap_clearerr();
	/* An empty filename cannot be opened, regardless of filesystem permissions. */
	bool success = oscap_set_verbose("INFO", "");
	const char *description = oscap_err_desc();
	int ret = 0;
	if (success || !oscap_err() || oscap_err_family() != OSCAP_EFAMILY_OSCAP ||
		description == NULL || strstr(description, "Failed to open file") == NULL) {
		fprintf(stderr, "Log initialization failure did not populate the library error queue.\n");
		ret = 1;
	}
	oscap_cleanup();
	return ret;
}
