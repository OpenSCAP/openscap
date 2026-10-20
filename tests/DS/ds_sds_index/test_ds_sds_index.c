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
#include "oscap_source.h"
#include "ds_sds_session.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Invalid arguments, usage: ./test_ds_sds_index FILE");
		return 2;
	}

	struct oscap_source *source = oscap_source_new_from_file(argv[1]);
	struct ds_sds_session *session = ds_sds_session_new_from_source(source);
	struct ds_sds_index *idx = ds_sds_session_get_sds_idx(session);
	struct ds_stream_index_iterator* streams = ds_sds_index_get_streams(idx);

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
			ds_sds_session_free(session);
			oscap_source_free(source);
			return 1;
		}
		if (strcmp(ds_stream_index_get_timestamp(stream),
		    "2012-11-01T12:22:58") != 0)
		{
			printf("Failed to read datastream timestamp correctly. "
			       "Expected '2012-11-01T12:22:58', "
			       "found '%s'.\n", ds_stream_index_get_timestamp(stream));
			ds_sds_session_free(session);
			oscap_source_free(source);
			return 1;
		}
		if (strcmp(ds_stream_index_get_version(stream),
		    "1.2") != 0)
		{
			printf("Failed to read datastream scap-version correctly. "
			       "Expected '1.2', "
			       "found '%s'.\n", ds_stream_index_get_version(stream));
			ds_sds_session_free(session);
			oscap_source_free(source);
			return 1;
		}
	}
	ds_stream_index_iterator_free(streams);

	if (ds_sds_index_get_stream(idx, "scap_org.open-scap_datastream_from_xccdf_scap-fedora14-xccdf.xml") == NULL)
	{
		printf("Attempted to retrieve 'scap_org.open-scap_datastream_from_xccdf_scap-fedora14-xccdf.xml' "
		       "by ID but got NULL as a result!\n");
		ds_sds_session_free(session);
		oscap_source_free(source);
		return 1;
	}
	if (ds_sds_index_get_stream(idx, "nonexistant_rubbish") != NULL)
	{
		printf("Attempted to retrieve a nonexistant stream by ID but got a non-NULL result!\n");
		ds_sds_session_free(session);
		oscap_source_free(source);
		return 1;
	}

	ds_sds_session_free(session);
	oscap_source_free(source);

	if (nr_streams != 1)
	{
		printf("Expected to read 1 data-stream from the source datastream collection, "
		       "instead read %i!\n", nr_streams);
		return 1;
	}

	return 0;
}
