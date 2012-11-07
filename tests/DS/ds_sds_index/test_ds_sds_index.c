/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * Authors:
 *      Martin Preisler <mpreisle@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scap_ds.h"
#include "common/list.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Invalid arguments, usage: ./test_ds_sds_index FILE");
		return 2;
	}

	struct ds_sds_index* index = ds_sds_index_import(argv[1]);
	struct ds_stream_index_iterator* streams = ds_sds_index_get_streams(index);

	// number of streams in the collection
	int nr_streams = 0;

	while (ds_stream_index_iterator_has_more(streams))
	{
		struct ds_stream_index* stream = ds_stream_index_iterator_next(streams);
		nr_streams++;

		if (strcmp(ds_stream_index_get_id(stream),
		    "scap_org.open-scap_datastream_from_xccdf_scap-fedora14-xccdf.xml") != 0)
		{
			printf("Failed to read datastream ID correctly. "
			       "Expected 'scap_org.open-scap_datastream_from_xccdf_scap-fedora14-xccdf.xml', "
			       "found '%s'.\n", ds_stream_index_get_id(stream));
			return 1;
		}
	}
	ds_stream_index_iterator_free(streams);
	ds_sds_index_free(index);

	if (nr_streams != 1)
	{
		printf("Expected to read 1 data-stream from the source datastream collection, "
		       "instead read %i!\n", nr_streams);
		return 1;
	}

	return 0;
}
