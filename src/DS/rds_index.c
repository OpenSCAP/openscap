/*
 * Copyright 2013--2014 Red Hat Inc., Durham, North Carolina.
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
#include "rds_index_priv.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"

#include <libxml/xmlreader.h>
#include <string.h>

struct rds_index
{
	struct oscap_list *report_requests;
	struct oscap_list *assets;
	struct oscap_list *reports;
};

struct rds_index *rds_index_new(void)
{
	struct rds_index *ret = oscap_calloc(1, sizeof(struct rds_index));
	ret->report_requests = oscap_list_new();
	ret->assets = oscap_list_new();
	ret->reports = oscap_list_new();

	return ret;
}

void rds_index_free(struct rds_index *s)
{
	if (s != NULL) {
		oscap_list_free(s->report_requests, (oscap_destruct_func)rds_report_request_index_free);
		oscap_list_free(s->assets, (oscap_destruct_func)rds_asset_index_free);
		oscap_list_free(s->reports, (oscap_destruct_func)rds_report_index_free);
		oscap_free(s);
	}
}

static void rds_index_add_report_request(struct rds_index *s, struct rds_report_request_index *rr_index)
{
	oscap_list_add(s->report_requests, rr_index);
}

struct rds_report_request_index_iterator *rds_index_get_report_requests(struct rds_index *s)
{
	return (struct rds_report_request_index_iterator*)oscap_iterator_new(s->report_requests);
}

static void rds_index_add_asset(struct rds_index *s, struct rds_asset_index *a_index)
{
	oscap_list_add(s->assets, a_index);
}

struct rds_asset_index_iterator *rds_index_get_assets(struct rds_index *s)
{
	return (struct rds_asset_index_iterator*)oscap_iterator_new(s->assets);
}

static void rds_index_add_report(struct rds_index* s, struct rds_report_index *r_index)
{
	oscap_list_add(s->reports, r_index);
}

struct rds_report_index_iterator *rds_index_get_reports(struct rds_index* s)
{
	return (struct rds_report_index_iterator*)oscap_iterator_new(s->reports);
}

struct rds_report_request_index* rds_index_get_report_request(struct rds_index* rds, const char* id)
{
	struct rds_report_request_index* ret = NULL;

	struct rds_report_request_index_iterator* it = rds_index_get_report_requests(rds);
	while (rds_report_request_index_iterator_has_more(it))
	{
		struct rds_report_request_index* rr_index = rds_report_request_index_iterator_next(it);
		if (strcmp(rds_report_request_index_get_id(rr_index), id) == 0) {
			ret = rr_index;
			break;
		}

	}
	rds_report_request_index_iterator_free(it);

	return ret;
}

struct rds_asset_index* rds_index_get_asset(struct rds_index *rds, const char *id)
{
	struct rds_asset_index *ret = NULL;

	struct rds_asset_index_iterator *it = rds_index_get_assets(rds);
	while (rds_asset_index_iterator_has_more(it))
	{
		struct rds_asset_index *a_index = rds_asset_index_iterator_next(it);
		if (strcmp(rds_asset_index_get_id(a_index), id) == 0) {
			ret = a_index;
			break;
		}

	}
	rds_asset_index_iterator_free(it);

	return ret;
}

struct rds_report_index *rds_index_get_report(struct rds_index *rds, const char *id)
{
	struct rds_report_index *ret = NULL;

	struct rds_report_index_iterator *it = rds_index_get_reports(rds);
	while (rds_report_index_iterator_has_more(it))
	{
		struct rds_report_index *r_index = rds_report_index_iterator_next(it);
		if (strcmp(rds_report_index_get_id(r_index), id) == 0) {
			ret = r_index;
			break;
		}

	}
	rds_report_index_iterator_free(it);

	return ret;
}

static xmlChar *relationship_get_inner_ref(xmlNodePtr node)
{
	xmlChar *ret = NULL;
	xmlNodePtr ref_node = node->children;

	for (; ref_node != NULL; ref_node = ref_node->next)
	{
		if (ref_node->type != XML_ELEMENT_NODE)
			continue;

		if (strcmp((const char*)(ref_node->name), "ref") != 0)
			continue; // TODO: Warning?

		if (ret) {
			oscap_seterr(OSCAP_EFAMILY_XML,
					"Multiple <core:ref> elements found in a <core:relationship> element.\n"
					"Only the first ref will be used!\n"
					"Make sure the Result DataStream is valid!");
		}
		else
			ret = xmlNodeGetContent(ref_node);
	}

	return ret;
}

static inline void _parse_relationships_node(struct rds_index *ret, xmlNodePtr relationships_node)
{
	if (relationships_node == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, "There is no <core:relationships> element in the Result DataStream.\n");
		return;
	}

	xmlNodePtr relationship_node = relationships_node->children;
	for (; relationship_node != NULL; relationship_node = relationship_node->next)
	{
		if (relationship_node->type != XML_ELEMENT_NODE)
			continue;

		if (strcmp((const char*)(relationship_node->name), "relationship") != 0)
			continue; // TODO: Warning?

		xmlChar *type_attr = xmlGetProp(relationship_node, BAD_CAST "type");
		xmlChar *subject_attr = xmlGetProp(relationship_node, BAD_CAST "subject");
		xmlChar *inner_ref = relationship_get_inner_ref(relationship_node);

		if (strcmp((const char*)type_attr, "arfvocab:isAbout") == 0) {
			struct rds_asset_index* asset = rds_index_get_asset(ret, (const char*)inner_ref);
			struct rds_report_index* report = rds_index_get_report(ret, (const char*)subject_attr);

			rds_asset_index_add_report_ref(asset, report);
		}
		else if (strcmp((const char*)type_attr, "arfvocab:createdFor") == 0) {
			struct rds_report_request_index *request = rds_index_get_report_request(ret, (const char*)inner_ref);
			struct rds_report_index *report = rds_index_get_report(ret, (const char*)subject_attr);

			// This is based on the assumption that every report has at most 1 request
			// it was "created for".
			rds_report_index_set_request(report, request);
		}

		xmlFree(type_attr);
		xmlFree(subject_attr);
		xmlFree(inner_ref);
	}
}

struct rds_index *rds_index_parse(xmlTextReaderPtr reader)
{
	if (!oscap_to_start_element(reader, 0)) {
		oscap_seterr(OSCAP_EFAMILY_XML,
		             "Expected to to have start of <arf:asset-report-collection> at document root, "
		             "no elements were found! I refuse to parse!");

		return NULL;
	}

	if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT ||
	    strcmp((const char*)xmlTextReaderConstLocalName(reader), "asset-report-collection") != 0) {
		oscap_seterr(OSCAP_EFAMILY_XML,
		             "Expected to to have start of <arf:asset-report-collection> at document root, "
		             "the current event is '%i' at '%s' instead. I refuse to parse!",
		             xmlTextReaderNodeType(reader), (const char*)xmlTextReaderConstLocalName(reader));

		return NULL;
	}

	if (xmlTextReaderRead(reader) != 1) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	struct rds_index* ret = rds_index_new();
	xmlNodePtr relationships_node = NULL;

	while (oscap_to_start_element(reader, 1))
	{
		const char *name = (const char*)xmlTextReaderConstLocalName(reader);

		if (strcmp(name, "report-requests") == 0) {
			if (xmlTextReaderRead(reader) != 1) {
				oscap_setxmlerr(xmlGetLastError());
			}

			while (oscap_to_start_element(reader, 2))
			{
				if (strcmp((const char*)xmlTextReaderConstLocalName(reader), "report-request") != 0) {
					// TODO: warning?
					continue;
				}

				struct rds_report_request_index* rr_index = rds_report_request_index_parse(reader);
				rds_index_add_report_request(ret, rr_index);

				if (xmlTextReaderRead(reader) != 1) {
					oscap_setxmlerr(xmlGetLastError());
				}
			}
		}
		else if (strcmp(name, "assets") == 0)
		{
			if (xmlTextReaderRead(reader) != 1) {
				oscap_setxmlerr(xmlGetLastError());
			}

			while (oscap_to_start_element(reader, 2))
			{
				if (strcmp((const char*)xmlTextReaderConstLocalName(reader), "asset") != 0) {
					// TODO: warning?
					continue;
				}

				struct rds_asset_index* a_index = rds_asset_index_parse(reader);
				rds_index_add_asset(ret, a_index);

				if (xmlTextReaderRead(reader) != 1) {
					oscap_setxmlerr(xmlGetLastError());
				}
			}
		}
		else if (strcmp(name, "reports") == 0)
		{
			if (xmlTextReaderRead(reader) != 1) {
				oscap_setxmlerr(xmlGetLastError());
			}

			while (oscap_to_start_element(reader, 2))
			{
				if (strcmp((const char*)xmlTextReaderConstLocalName(reader), "report") != 0) {
					// TODO: warning?
					continue;
				}

				struct rds_report_index* r_index = rds_report_index_parse(reader);
				rds_index_add_report(ret, r_index);

				if (xmlTextReaderRead(reader) != 1) {
					oscap_setxmlerr(xmlGetLastError());
				}
			}
		}
		else if (strcmp(name, "relationships") == 0) {
			// We have to copy the node, xmlTextReader destroys it when
			// xmlTextReaderRead is next called.
			//
			// extended = 1 means that we want to copy all including children
			xmlNodePtr new_relationships_node = xmlCopyNode(xmlTextReaderExpand(reader), 1);

			if (relationships_node)
			{
				oscap_seterr(OSCAP_EFAMILY_XML, "There is more than 1 <core:relationships> element in the Result DataStream.\n"
					"Please make sure the input file is valid! Only the first element will be used to build the index!");
				xmlFreeNode(new_relationships_node);
			}
			else
				relationships_node = new_relationships_node;
		}
		else
		{
			oscap_seterr(OSCAP_EFAMILY_XML, "Unknown element '%s' encountered while parsing Result DataStream to rds_index, skipping...", name);
		}

		xmlTextReaderRead(reader);
	}

	_parse_relationships_node(ret, relationships_node);
	xmlFreeNode(relationships_node);

	return ret;
}

struct rds_index *rds_index_import(const char *file)
{
	struct oscap_source *source = oscap_source_new_from_file(file);
	xmlTextReader *reader = oscap_source_get_xmlTextReader(source);
	if (!reader) {
		oscap_source_free(source);
		return NULL;
	}

	while (xmlTextReaderRead(reader) == 1 && xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);
	struct rds_index *ret = rds_index_parse(reader);
	xmlFreeTextReader(reader);
	oscap_source_free(source);
	return ret;
}

int rds_index_select_report(struct rds_index *s, const char **report_id)
{
	int ret = 1;

	struct rds_report_index_iterator *reports_it = rds_index_get_reports(s);
	while (rds_report_index_iterator_has_more(reports_it))
	{
		struct rds_report_index* report_idx = rds_report_index_iterator_next(reports_it);
		const char *report_idx_id = rds_report_index_get_id(report_idx);

		if (!*report_id || strcmp(report_idx_id, *report_id) == 0) {
			*report_id = report_idx_id;
			ret = 0;
			break;
		}
	}
	rds_report_index_iterator_free(reports_it);

	return ret;
}
