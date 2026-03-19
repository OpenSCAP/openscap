// SPDX-License-Identifier: LGPL-2.1-or-later

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "xccdf_benchmark.h"

int main(void)
{
	struct xccdf_result *result = xccdf_result_new();
	struct xccdf_target_fact_iterator *facts = NULL;
	bool saw_mac = false;
	bool saw_asset_mac = false;

	xccdf_result_fill_sysinfo(result);
	facts = xccdf_result_get_target_facts(result);

	while (xccdf_target_fact_iterator_has_more(facts)) {
		struct xccdf_target_fact *fact = xccdf_target_fact_iterator_next(facts);
		const char *name = xccdf_target_fact_get_name(fact);

		if (name == NULL)
			continue;

		if (strcmp(name, "urn:xccdf:fact:ethernet:MAC") == 0)
			saw_mac = true;
		if (strcmp(name, "urn:xccdf:fact:asset:identifier:mac") == 0)
			saw_asset_mac = true;
	}

	xccdf_target_fact_iterator_free(facts);
	xccdf_result_free(result);

#if defined(OS_APPLE)
	if (!saw_mac) {
		fprintf(stderr, "Expected at least one urn:xccdf:fact:ethernet:MAC fact on macOS.\n");
		return 1;
	}
	if (!saw_asset_mac) {
		fprintf(stderr, "Expected at least one urn:xccdf:fact:asset:identifier:mac fact on macOS.\n");
		return 1;
	}
#endif

	return 0;
}
