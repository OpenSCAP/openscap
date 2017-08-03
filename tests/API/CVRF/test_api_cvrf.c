/*
 * Copyright 2017 Red Hat Inc., Durham, North Carolina.
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
 */
#include <stdio.h>
#include <string.h>
#include <cvss_score.h>
#include <cvrf.h>
#include <oscap_source.h>


int main(int argc, char **argv)
{
	struct cvrf_model *model;

	/* test export */
	if (argc == 4 && !strcmp(argv[1], "--export-all")) {

		model = cvrf_model_import(oscap_source_new_from_file(argv[2]));
		if(!model)
			return 1;
		cvrf_model_export(model, argv[3]);
		cvrf_model_free(model);
		return 0;
	} else if (argc == 4 && !strcmp(argv[1], "--eval")) {
		const char *os_version = "Red Hat Enterprise Linux Desktop Supplementary (v. 6)";
		cvrf_export_results(oscap_source_new_from_file(argv[2]), argv[3], os_version);
		return 0;
	}

	fprintf(stdout,
		"Usage: \n\n"
		"  %s --help\n"
		"  %s --export-all input.xml output.xml\n"
		"  %s --eval input.xml results.xml\n",
		argv[0], argv[0], argv[0]);

	return 0;
}
