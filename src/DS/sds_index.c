/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
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
 *      Martin Preisler <mpreisle@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "public/scap_ds.h"
#include "common/list.h"
#include "common/_error.h"
#include "common/alloc.h"
#include "common/elements.h"

#include <libxml/xmlreader.h>
#include <string.h>

struct ds_stream_index
{
	char* id;
	char* timestamp;
	char* version;

	struct oscap_stringlist* check_components;
	struct oscap_stringlist* checklist_components;
	struct oscap_stringlist* dictionary_components;
	struct oscap_stringlist* extended_components;
};

struct ds_stream_index* ds_stream_index_new(void)
{
	struct ds_stream_index* ret = oscap_alloc(sizeof(struct ds_stream_index));

	ret->id = NULL;
	ret->timestamp = NULL;
	ret->version = NULL;

	ret->check_components = oscap_stringlist_new();
	ret->checklist_components = oscap_stringlist_new();
	ret->dictionary_components = oscap_stringlist_new();
	ret->extended_components = oscap_stringlist_new();

	return ret;
}

void ds_stream_index_free(struct ds_stream_index* s)
{
	oscap_free(s->id);
	oscap_free(s->timestamp);
	oscap_free(s->version);

	oscap_stringlist_free(s->check_components);
	oscap_stringlist_free(s->checklist_components);
	oscap_stringlist_free(s->dictionary_components);
	oscap_stringlist_free(s->extended_components);

	oscap_free(s);
}

const char* ds_stream_index_get_id(struct ds_stream_index* s)
{
	return s->id;
}

const char* ds_stream_index_get_timestamp(struct ds_stream_index* s)
{
	return s->timestamp;
}

const char* ds_stream_index_get_version(struct ds_stream_index* s)
{
	return s->version;
}

struct oscap_string_iterator* ds_stream_index_get_checks(struct ds_stream_index* s)
{
	return oscap_iterator_new((struct oscap_list*)s->check_components);
}

struct oscap_string_iterator* ds_stream_index_get_checklists(struct ds_stream_index* s)
{
	return oscap_iterator_new((struct oscap_list*)s->checklist_components);
}

struct oscap_string_iterator* ds_stream_index_get_dictionaries(struct ds_stream_index* s)
{
	return oscap_iterator_new((struct oscap_list*)s->dictionary_components);
}

struct oscap_string_iterator* ds_stream_index_get_extended_components(struct ds_stream_index* s)
{
	return oscap_iterator_new((struct oscap_list*)s->extended_components);
}

static struct ds_stream_index* ds_stream_index_parse(xmlTextReaderPtr reader)
{
	// sanity check
	if (xmlTextReaderNodeType(reader) != 1 ||
	    strcmp((const char*)xmlTextReaderConstLocalName(reader), "data-stream") != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML,
		             "Expected to have xmlTextReader at start of <ds:data-stream>, "
		             "the current event is '%i' at '%s' instead. I refuse to parse!",
		             xmlTextReaderNodeType(reader), (const char*)xmlTextReaderConstLocalName(reader));

		return NULL;
	}

	struct ds_stream_index* ret = ds_stream_index_new();

	ret->id = (char*)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	ret->timestamp = (char*)xmlTextReaderGetAttribute(reader, BAD_CAST "timestamp");
	ret->version = (char*)xmlTextReaderGetAttribute(reader, BAD_CAST "scap-version");

	// We assume well-formedness and validity of the datastream.
	// The parser can be broken with invalid content such as:
	// .. <checklists><checks/><component-ref ../></checklists> ..

	struct oscap_stringlist* cref_target = NULL;
	while (xmlTextReaderRead(reader) == 1)
	{
		int node_type = xmlTextReaderNodeType(reader);
		const char* local_name = (const char*)xmlTextReaderConstLocalName(reader);

		if (node_type == 15 && // 15 == end element
		    strcmp(local_name, "data-stream") == 0)
		{
			// we are done reading
			break;
		}
		// the following code switches where we push component refs
		else if (strcmp(local_name, "checklists") == 0)
		{
			cref_target = node_type == 1 ? ret->checklist_components : NULL;
		}
		else if (strcmp(local_name, "checks") == 0)
		{
			cref_target = node_type == 1 ? ret->check_components : NULL;
		}
		else if (strcmp(local_name, "dictionaries") == 0)
		{
			cref_target = node_type == 1 ? ret->dictionary_components : NULL;
		}
		else if (strcmp(local_name, "extended-components") == 0)
		{
			cref_target = node_type == 1 ? ret->extended_components : NULL;
		}
		// reading of the component refs, we only care about their ID
		else if (strcmp(local_name, "component-ref") == 0 &&
		         node_type == 1)
		{
			// sanity check
			if (cref_target == NULL) {
				oscap_seterr(OSCAP_EFAMILY_XML,
				             "Encountered <ds:component-ref> but it is either not inside "
				             "any container element or container elements interleave. "
				             "Please make sure the datastream is valid!");
			}
			else {
				xmlChar* id_attr = xmlTextReaderGetAttribute(reader, BAD_CAST "id");
				oscap_stringlist_add_string(cref_target, (const char*)id_attr);
				xmlFree(id_attr);
			}
		}
	}

	return ret;
}

struct ds_sds_index
{
	struct oscap_list* streams;
};

struct ds_sds_index* ds_sds_index_new(void)
{
	struct ds_sds_index* ret = oscap_alloc(sizeof(struct ds_sds_index));
	ret->streams = oscap_list_new();

	return ret;
}

void ds_sds_index_free(struct ds_sds_index* s)
{
	if (s != NULL) {
		oscap_list_free(s->streams, (oscap_destruct_func)ds_stream_index_free);
		oscap_free(s);
	}
}

static void ds_sds_index_add_stream(struct ds_sds_index* s, struct ds_stream_index* stream)
{
	oscap_list_add(s->streams, stream);
}

struct ds_stream_index* ds_sds_index_get_stream(struct ds_sds_index* sds, const char* stream_id)
{
	struct ds_stream_index* ret = NULL;

	struct ds_stream_index_iterator* streams = ds_sds_index_get_streams(sds);
	while (ds_stream_index_iterator_has_more(streams))
	{
		struct ds_stream_index* stream = ds_stream_index_iterator_next(streams);
		if (strcmp(ds_stream_index_get_id(stream), stream_id) == 0)
		{
			ret = stream;
			break;
		}

	}
	ds_stream_index_iterator_free(streams);

	return ret;
}

struct ds_stream_index_iterator* ds_sds_index_get_streams(struct ds_sds_index* s)
{
	return (struct ds_stream_index_iterator*)oscap_iterator_new(s->streams);
}

static struct ds_sds_index* ds_sds_index_parse(xmlTextReaderPtr reader)
{
	if (!oscap_to_start_element(reader, 0)) {
		oscap_seterr(OSCAP_EFAMILY_XML,
		             "Expected to to have start of <ds:data-stream-collection> at document root, "
		             "no elements were found! I refuse to parse!");

		return NULL;
	}

	struct ds_sds_index* ret = ds_sds_index_new();

	if (xmlTextReaderNodeType(reader) != 1 ||
	    strcmp((const char*)xmlTextReaderConstLocalName(reader), "data-stream-collection") != 0)
	{
		ds_sds_index_free(ret);
		oscap_seterr(OSCAP_EFAMILY_XML,
		             "Expected to to have start of <ds:data-stream-collection> at document root, "
		             "the current event is '%i' at '%s' instead. I refuse to parse!",
		             xmlTextReaderNodeType(reader), (const char*)xmlTextReaderConstLocalName(reader));

		return NULL;
	}

	if (xmlTextReaderRead(reader) != 1)
	{
		ds_sds_index_free(ret);
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	while (oscap_to_start_element(reader, 1))
	{
		const char* name = (const char *)xmlTextReaderConstLocalName(reader);
		if (strcmp(name, "component") == 0)
		{
			// XSD of source datastream dictates that no ds:data-stream elements
			// follow after ds:component element. We can safely stop reading here.
			break;
		}

		if (strcmp(name, "data-stream") == 0)
		{
			struct ds_stream_index* s = ds_stream_index_parse(reader);
			// NULL means error happened, the ds_stream_index_parse already set the error
			// in that case
			if (s != NULL)
				ds_sds_index_add_stream(ret, s);
		}
		else
		{
			oscap_seterr(OSCAP_EFAMILY_XML, "Unknown element '%s' encountered while parsing Source DataStream to ds_sds_index, skipping...", name);
		}

		xmlTextReaderRead(reader);
	}

	return ret;
}

struct ds_sds_index *ds_sds_index_import(const char* file)
{
	xmlTextReaderPtr reader = xmlReaderForFile(file, NULL, 0);
	if (!reader) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "Unable to open file: '%s'", file);
		return NULL;
	}

	while (xmlTextReaderRead(reader) == 1 && xmlTextReaderNodeType(reader) != 1);
	struct ds_sds_index* ret = ds_sds_index_parse(reader);
	xmlFreeTextReader(reader);

	return ret;
}

int ds_sds_index_select_checklist(struct ds_sds_index* s,
		const char** datastream_id, const char** component_id)
{
	// The following convoluted and complex code looks for a pair of
	// datastream and checklist inside it that satisfies conditions
	// (if any) of having specific IDs.
	//
	// It is possible to just pass xccdf-id in which case we have to
	// search through all the datastreams.
	//
	// It is also possible to just pass datastream-id in which case we
	// take the first checklist in given datastream.
	//
	// In case datastream-id is not passed AND xccdf-id is also not passed,
	// we look for the first datastream (topdown in XML) that has any
	// checklist. Taking the first checklist in such a datastream in this case.

	int ret = 1;

	struct ds_stream_index_iterator* streams_it = ds_sds_index_get_streams(s);
	while (ds_stream_index_iterator_has_more(streams_it))
	{
		struct ds_stream_index* stream_idx = ds_stream_index_iterator_next(streams_it);
		const char* stream_id = ds_stream_index_get_id(stream_idx);

		if (!*datastream_id || strcmp(stream_id, *datastream_id) == 0)
		{
			struct oscap_string_iterator* checklists_it = ds_stream_index_get_checklists(stream_idx);
			while (oscap_string_iterator_has_more(checklists_it))
			{
				const char* checklist_id = oscap_string_iterator_next(checklists_it);

				if (!*component_id || strcmp(checklist_id, *component_id) == 0)
				{
					*component_id = checklist_id;
					*datastream_id = ds_stream_index_get_id(stream_idx);
					ret = 0;
					break;
				}
			}
			oscap_string_iterator_free(checklists_it);
		}
	}
	ds_stream_index_iterator_free(streams_it);

	return ret;
}

struct ds_stream_index *ds_stream_index_iterator_next(struct ds_stream_index_iterator *it)
{
	return (struct ds_stream_index*)(oscap_iterator_next((struct oscap_iterator*)it));
}

bool ds_stream_index_iterator_has_more(struct ds_stream_index_iterator *it)
{
	return oscap_iterator_has_more((struct oscap_iterator*)it);
}

void ds_stream_index_iterator_free(struct ds_stream_index_iterator *it)
{
	oscap_iterator_free((struct oscap_iterator*)it);
}
