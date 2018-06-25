/*
 * Copyright 2012--2014 Red Hat Inc., Durham, North Carolina.
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
#include "common/elements.h"
#include "sds_index_priv.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"

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

	struct oscap_htable *component_id_to_component_ref_id;
};

struct ds_stream_index* ds_stream_index_new(void)
{
	struct ds_stream_index* ret = malloc(sizeof(struct ds_stream_index));

	ret->id = NULL;
	ret->timestamp = NULL;
	ret->version = NULL;

	ret->check_components = oscap_stringlist_new();
	ret->checklist_components = oscap_stringlist_new();
	ret->dictionary_components = oscap_stringlist_new();
	ret->extended_components = oscap_stringlist_new();

	ret->component_id_to_component_ref_id = oscap_htable_new();

	return ret;
}

void ds_stream_index_free(struct ds_stream_index* s)
{
	free(s->id);
	free(s->timestamp);
	free(s->version);

	oscap_stringlist_free(s->check_components);
	oscap_stringlist_free(s->checklist_components);
	oscap_stringlist_free(s->dictionary_components);
	oscap_stringlist_free(s->extended_components);

	oscap_htable_free(s->component_id_to_component_ref_id, (oscap_destruct_func)free);

	free(s);
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
	if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT ||
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

		if (node_type == XML_READER_TYPE_END_ELEMENT && // 15 == end element
		    strcmp(local_name, "data-stream") == 0)
		{
			// we are done reading
			break;
		}
		// the following code switches where we push component refs
		else if (strcmp(local_name, "checklists") == 0)
		{
			cref_target = node_type == XML_READER_TYPE_ELEMENT ? ret->checklist_components : NULL;
		}
		else if (strcmp(local_name, "checks") == 0)
		{
			cref_target = node_type == XML_READER_TYPE_ELEMENT ? ret->check_components : NULL;
		}
		else if (strcmp(local_name, "dictionaries") == 0)
		{
			cref_target = node_type == XML_READER_TYPE_ELEMENT ? ret->dictionary_components : NULL;
		}
		else if (strcmp(local_name, "extended-components") == 0)
		{
			cref_target = node_type == XML_READER_TYPE_ELEMENT ? ret->extended_components : NULL;
		}
		// reading of the component refs, we only care about their ID
		else if (strcmp(local_name, "component-ref") == 0 &&
		         node_type == XML_READER_TYPE_ELEMENT)
		{
			// sanity check
			if (cref_target == NULL) {
				oscap_seterr(OSCAP_EFAMILY_XML,
				             "Encountered <ds:component-ref> but it is either not inside "
				             "any container element or container elements interleave. "
				             "Please make sure the datastream is valid!");
			}
			else {
				xmlChar *id_attr = xmlTextReaderGetAttribute(reader, BAD_CAST "id");
				xmlChar *href_attr = xmlTextReaderGetAttributeNs(reader, BAD_CAST "href", BAD_CAST "http://www.w3.org/1999/xlink");

				// this copies the id_attr string
				oscap_stringlist_add_string(cref_target, (const char*)id_attr);

				// because of the leading '#' in the href preceding the component id
				const char *component_id = (const char*)(href_attr && href_attr[0] ? (href_attr + 1 * sizeof(char)) : NULL);
				if (!component_id || !oscap_htable_add(ret->component_id_to_component_ref_id, component_id, (char*)id_attr)) {
					if (component_id) {
						oscap_seterr(OSCAP_EFAMILY_XML,
				             "There is already a mapping from component id '%s' to component-ref id '%s'. "
							 "Having multiple mappings is legal in a datastream but it may prove problematic "
							"when selecting component-refs using XCCDF Benchmark IDs.",
							component_id, id_attr);
					}

					xmlFree(id_attr);
				}

				xmlFree(href_attr);
			}
		}
	}

	return ret;
}

struct ds_sds_index
{
	struct oscap_list *streams;

	struct oscap_htable *benchmark_id_to_component_id;
};

struct ds_sds_index* ds_sds_index_new(void)
{
	struct ds_sds_index* ret = malloc(sizeof(struct ds_sds_index));
	ret->streams = oscap_list_new();

	ret->benchmark_id_to_component_id = oscap_htable_new();

	return ret;
}

void ds_sds_index_free(struct ds_sds_index* s)
{
	if (s != NULL) {
		oscap_list_free(s->streams, (oscap_destruct_func)ds_stream_index_free);

		oscap_htable_free(s->benchmark_id_to_component_id, (oscap_destruct_func)free);

		free(s);
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

static char *ds_sds_component_dig_benchmark_id(xmlTextReaderPtr reader)
{
	// sanity check
	if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT ||
	    strcmp((const char*)xmlTextReaderConstLocalName(reader), "component") != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML,
		             "Expected to have xmlTextReader at start of <ds:component>, "
		             "the current event is '%i' at '%s' instead. I refuse to parse!",
		             xmlTextReaderNodeType(reader), (const char*)xmlTextReaderConstLocalName(reader));

		return NULL;
	}

	char *ret = NULL;
	while (xmlTextReaderRead(reader) == 1)
	{
		int node_type = xmlTextReaderNodeType(reader);
		const char* local_name = (const char*)xmlTextReaderConstLocalName(reader);

		if (node_type == XML_READER_TYPE_END_ELEMENT &&
		    strcmp(local_name, "component") == 0) {
			// we are done reading
			break;
		}
		else if (node_type == XML_READER_TYPE_ELEMENT &&
				 strcmp(local_name, "Benchmark") == 0) {
			if (ret) {
				oscap_seterr(OSCAP_EFAMILY_XML,
		             "Found 2 Benchmark elements inside a single sds:component element! "
					 "Please make sure your datastream is valid. Skipping the second Benchmark.");
			}
			else {
				ret = (char*)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
			}
		}
		else {
			// ignore
		}
	}

	return ret;
}

struct ds_sds_index* ds_sds_index_parse(xmlTextReaderPtr reader)
{
	if (!oscap_to_start_element(reader, 0)) {
		oscap_seterr(OSCAP_EFAMILY_XML,
		             "Expected to to have start of <ds:data-stream-collection> at document root, "
		             "no elements were found! I refuse to parse!");

		return NULL;
	}

	if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT ||
	    strcmp((const char*)xmlTextReaderConstLocalName(reader), "data-stream-collection") != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML,
		             "Expected to to have start of <ds:data-stream-collection> at document root, "
		             "the current event is '%i' at '%s' instead. I refuse to parse!",
		             xmlTextReaderNodeType(reader), (const char*)xmlTextReaderConstLocalName(reader));

		return NULL;
	}

	if (xmlTextReaderRead(reader) != 1)
	{
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	struct ds_sds_index* ret = ds_sds_index_new();

	while (oscap_to_start_element(reader, 1))
	{
		const char* name = (const char *)xmlTextReaderConstLocalName(reader);
		/*if (strcmp(name, "component") == 0)
		{
			// XSD of source datastream dictates that no ds:data-stream elements
			// follow after ds:component element. We can safely stop reading here.
			break;
		}*/

		if (strcmp(name, "data-stream") == 0) {
			struct ds_stream_index* s = ds_stream_index_parse(reader);
			// NULL means error happened, the ds_stream_index_parse already set the error
			// in that case
			if (s != NULL)
				ds_sds_index_add_stream(ret, s);
		}
		else if (strcmp(name, "component") == 0) {
			char *component_id = (char*)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
			char *benchmark_id = ds_sds_component_dig_benchmark_id(reader);

			if (benchmark_id == NULL) {
				free(component_id);
			}
			else {
				if (!oscap_htable_add(ret->benchmark_id_to_component_id, benchmark_id, component_id)) {
					// This benchmark ID was already in the map, therefore there must be 2 components
					// with Benchmarks in them that have the same IDs. In this case, all bets are off
					// when it comes to selecting a component-ref using Benchmark ID.
					//
					// To establish a well defined behavior, we don't "overwrite" it with the newly
					// found benchmark but instead use the first benchmark with such ID there.

					// TODO: Warning?
					/*oscap_seterr(OSCAP_EFAMILY_XML, "There are at least two components containing "
						"an XCCDF Benchmark with exactly the same ID ('%s'). Selecting a component-ref "
						"using Benchmark ID will use the first component encountered and ignore "
						"the subsequent ones with the same ID.", benchmark_id);*/

					free(component_id);
				}

				free(benchmark_id);
			}

			// we are going to free the component_id string later (in ds_sds_index_free)
		}
		else if (strcmp(name, "extended-component") == 0) {
			// ignore, extended-component can't be an XCCDF, therefore we are sure
			// not to be able to dig any Benchmark @id from it
		}
		else if (strcmp(name, "Signature") == 0) {
			// ignore, Signatures are to be checked externally, we don't load them in
		}
		else {
			oscap_seterr(OSCAP_EFAMILY_XML, "Unknown element '%s' encountered while parsing Source DataStream to ds_sds_index, skipping...", name);
		}

		xmlTextReaderRead(reader);
	}

	return ret;
}

struct ds_sds_index *ds_sds_index_import(const char* file)
{
	struct oscap_source *source = oscap_source_new_from_file(file);
	xmlTextReader *reader = oscap_source_get_xmlTextReader(source);
	if (!reader) {
		oscap_source_free(source);
		return NULL;
	}

	while (xmlTextReaderRead(reader) == 1 && xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);
	struct ds_sds_index* ret = ds_sds_index_parse(reader);
	xmlFreeTextReader(reader);
	oscap_source_free(source);

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

int ds_sds_index_select_checklist_by_benchmark_id(struct ds_sds_index* s,
		const char *benchmark_id, const char **datastream_id, const char **component_ref_id)
{
	const char *mapped_component_id = (const char*)oscap_htable_get(s->benchmark_id_to_component_id, benchmark_id);
	if (!mapped_component_id) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Can't map benchmark ID '%s' to any component ID.", benchmark_id);
		return 1;
	}

	int ret = 1;

	struct ds_stream_index_iterator* streams_it = ds_sds_index_get_streams(s);
	while (ds_stream_index_iterator_has_more(streams_it))
	{
		struct ds_stream_index *stream_idx = ds_stream_index_iterator_next(streams_it);
		const char *stream_id = ds_stream_index_get_id(stream_idx);

		if (!*datastream_id || strcmp(stream_id, *datastream_id) == 0)
		{
			const char *candidate_component_ref_id = (const char*)oscap_htable_get(stream_idx->component_id_to_component_ref_id, mapped_component_id);
			if (candidate_component_ref_id) {
				*datastream_id = stream_id;
				*component_ref_id = candidate_component_ref_id;
				ret = 0;
				break;
			}
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
