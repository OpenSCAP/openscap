/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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


#include <xccdf.h>
#include <stdio.h>
#include <stdlib.h>

void xccdf_benchmark_dump(struct xccdf_benchmark*);

bool dump_benchmark(const char* fname)
{
    struct xccdf_benchmark* benchmark = xccdf_benchmark_parse_xml(fname);
	if (benchmark == NULL) return false;
	printf("\n");
	xccdf_benchmark_dump(benchmark);
	printf("\n\n");
	xccdf_benchmark_free(benchmark);
	return true;
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s xccdf_benchmark.xml\n", argv[0]);
		return 1;
	}
	bool ret = dump_benchmark(argv[1]);
	oscap_cleanup(); // clean caches
	return (ret ? EXIT_SUCCESS : EXIT_FAILURE);
}


