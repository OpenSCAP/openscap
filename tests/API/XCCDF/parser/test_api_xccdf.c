/*
 * Copyright 2010--2014 Red Hat Inc., Durham, North Carolina.
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <xccdf_benchmark.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <oscap_source.h>
#include "common/public/oscap_error.h"


int main(int argc, char **argv)
{
	if (argc < 2) return 1;

	if (strcmp(argv[1], "--export") == 0) {
		if (argc != 4) return 1;
		struct oscap_source *source = oscap_source_new_from_file(argv[2]);
		struct xccdf_benchmark *bench = xccdf_benchmark_import_source(source);
		oscap_source_free(source);
		if (bench == NULL) return 1;
		xccdf_benchmark_export(bench, argv[3]);
		xccdf_benchmark_free(bench);
		oscap_cleanup();
		return 0;
	}
	else if (strcmp(argv[1], "--validate") == 0) {
		if (argc != 4) {
			fprintf(stderr, "Usage: %s --validate ver xccdf\n", argv[0]);
			return 1;
		}

		if (oscap_validate_document(argv[3], OSCAP_DOCUMENT_XCCDF, argv[2], NULL, stdout)) {
			fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
			return 1;
		}
		return 0;

	}
	else {
		fprintf(stderr, "Unknown mode: %s\n", argv[1]);
	}

	return 1;
}

