/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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

struct rds_report_request_index
{
	char *id;
};

struct rds_report_request_index* rds_report_request_index_new(void)
{
	struct rds_report_request_index* ret = oscap_alloc(sizeof(struct rds_report_request_index));
	ret->id = NULL;

	return ret;
}

void rds_report_request_index_free(struct rds_report_request_index* s)
{
	oscap_free(s->id);
	oscap_free(s);
}

const char* rds_report_request_index_get_id(struct rds_report_request_index* s)
{
	return s->id;
}

static struct rds_report_request_index* rds_report_request_index_parse(xmlTextReaderPtr reader)
{
	// sanity check
	if (xmlTextReaderNodeType(reader) != 1 ||
	    strcmp((const char*)xmlTextReaderConstLocalName(reader), "report-request") != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML,
		             "Expected to have xmlTextReader at start of <arf:report-request>, "
		             "the current event is '%i' at '%s' instead. I refuse to parse!",
		             xmlTextReaderNodeType(reader), (const char*)xmlTextReaderConstLocalName(reader));

		return NULL;
	}

	struct rds_report_request_index* ret = rds_report_request_index_new();

	ret->id = (char*)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	return ret;
}

struct rds_asset_index
{
	char *id;
};

struct rds_asset_index* rds_asset_index_new(void)
{
	struct rds_asset_index* ret = oscap_alloc(sizeof(struct rds_asset_index));
	ret->id = NULL;

	return ret;
}

void rds_asset_index_free(struct rds_asset_index* s)
{
	oscap_free(s->id);
	oscap_free(s);
}

const char* rds_asset_index_get_id(struct rds_asset_index* s)
{
	return s->id;
}

static struct rds_asset_index* rds_asset_index_parse(xmlTextReaderPtr reader)
{
	// sanity check
	if (xmlTextReaderNodeType(reader) != 1 ||
	    strcmp((const char*)xmlTextReaderConstLocalName(reader), "asset") != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML,
		             "Expected to have xmlTextReader at start of <arf:report>, "
		             "the current event is '%i' at '%s' instead. I refuse to parse!",
		             xmlTextReaderNodeType(reader), (const char*)xmlTextReaderConstLocalName(reader));

		return NULL;
	}

	struct rds_asset_index* ret = rds_asset_index_new();

	ret->id = (char*)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	return ret;
}

struct rds_report_index
{
	char *id;
};

struct rds_report_index* rds_report_index_new(void)
{
	struct rds_report_index* ret = oscap_alloc(sizeof(struct rds_report_index));
	ret->id = NULL;

	return ret;
}

void rds_report_index_free(struct rds_report_index* s)
{
	oscap_free(s->id);
	oscap_free(s);
}

const char* rds_report_index_get_id(struct rds_report_index* s)
{
	return s->id;
}

static struct rds_report_index* rds_report_index_parse(xmlTextReaderPtr reader)
{
	// sanity check
	if (xmlTextReaderNodeType(reader) != 1 ||
	    strcmp((const char*)xmlTextReaderConstLocalName(reader), "report") != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML,
		             "Expected to have xmlTextReader at start of <arf:report>, "
		             "the current event is '%i' at '%s' instead. I refuse to parse!",
		             xmlTextReaderNodeType(reader), (const char*)xmlTextReaderConstLocalName(reader));

		return NULL;
	}

	struct rds_report_index* ret = rds_report_index_new();

	ret->id = (char*)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	return ret;
}

struct rds_index
{
	struct oscap_list* report_requests;
	struct oscap_list* assets;
	struct oscap_list* reports;
};

struct rds_index* rds_index_new(void)
{
	struct rds_index* ret = oscap_alloc(sizeof(struct rds_index));
	ret->report_requests = oscap_list_new();
	ret->assets = oscap_list_new();
	ret->reports = oscap_list_new();

	return ret;
}

void rds_index_free(struct rds_index* s)
{
	oscap_list_free(s->report_requests, (oscap_destruct_func)rds_report_request_index_free);
	oscap_list_free(s->assets, (oscap_destruct_func)rds_asset_index_free);
	oscap_list_free(s->reports, (oscap_destruct_func)rds_report_index_free);

	oscap_free(s);
}

static void rds_index_add_report_request(struct rds_index* s, struct rds_report_request_index* index)
{
	oscap_list_add(s->report_requests, index);
}

struct rds_report_request_index_iterator* rds_index_get_report_requests(struct rds_index* s)
{
	return (struct rds_report_request_index_iterator*)oscap_iterator_new(s->report_requests);
}

static void rds_index_add_asset(struct rds_index* s, struct rds_asset_index* index)
{
	oscap_list_add(s->assets, index);
}

struct rds_asset_index_iterator* rds_index_get_assets(struct rds_index* s)
{
	return (struct rds_asset_index_iterator*)oscap_iterator_new(s->assets);
}

static void rds_index_add_report(struct rds_index* s, struct rds_report_index* index)
{
	oscap_list_add(s->reports, index);
}

struct rds_report_index_iterator* rds_index_get_reports(struct rds_index* s)
{
	return (struct rds_report_index_iterator*)oscap_iterator_new(s->reports);
}

struct rds_report_request_index* rds_index_get_report_request(struct rds_index* rds, const char* id)
{
	struct rds_report_request_index* ret = NULL;

	struct rds_report_request_index_iterator* it = rds_index_get_report_requests(rds);
	while (rds_report_request_index_iterator_has_more(it))
	{
		struct rds_report_request_index* index = rds_report_request_index_iterator_next(it);
		if (strcmp(rds_report_request_index_get_id(index), id) == 0)
		{
			ret = index;
			break;
		}

	}
	rds_report_request_index_iterator_free(it);

	return ret;
}

struct rds_asset_index* rds_index_get_asset(struct rds_index* rds, const char* id)
{
	struct rds_asset_index* ret = NULL;

	struct rds_asset_index_iterator* it = rds_index_get_assets(rds);
	while (rds_asset_index_iterator_has_more(it))
	{
		struct rds_asset_index* index = rds_asset_index_iterator_next(it);
		if (strcmp(rds_asset_index_get_id(index), id) == 0)
		{
			ret = index;
			break;
		}

	}
	rds_asset_index_iterator_free(it);

	return ret;
}

struct rds_report_index* rds_index_get_report(struct rds_index* rds, const char* id)
{
	struct rds_report_index* ret = NULL;

	struct rds_report_index_iterator* it = rds_index_get_reports(rds);
	while (rds_report_index_iterator_has_more(it))
	{
		struct rds_report_index* index = rds_report_index_iterator_next(it);
		if (strcmp(rds_report_index_get_id(index), id) == 0)
		{
			ret = index;
			break;
		}

	}
	rds_report_index_iterator_free(it);

	return ret;
}

static struct rds_index* rds_index_parse(xmlTextReaderPtr reader)
{
	if (!oscap_to_start_element(reader, 0)) {
		oscap_seterr(OSCAP_EFAMILY_XML,
		             "Expected to to have start of <arf:asset-report-collection> at document root, "
		             "no elements were found! I refuse to parse!");

		return NULL;
	}

	if (xmlTextReaderNodeType(reader) != 1 ||
	    strcmp((const char*)xmlTextReaderConstLocalName(reader), "asset-report-collection") != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML,
		             "Expected to to have start of <arf:asset-report-collection> at document root, "
		             "the current event is '%i' at '%s' instead. I refuse to parse!",
		             xmlTextReaderNodeType(reader), (const char*)xmlTextReaderConstLocalName(reader));

		return NULL;
	}

	if (xmlTextReaderRead(reader) != 1)
	{
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	struct rds_index* ret = rds_index_new();

	while (oscap_to_start_element(reader, 1))
	{
		const char* name = (const char*)xmlTextReaderConstLocalName(reader);

		if (strcmp(name, "report-requests") == 0)
		{
			if (xmlTextReaderRead(reader) != 1)
			{
				oscap_setxmlerr(xmlGetLastError());
			}

			while (oscap_to_start_element(reader, 2))
			{
				if (strcmp((const char*)xmlTextReaderConstLocalName(reader), "report-request") != 0)
				{
					// TODO: warning?
					continue;
				}

				struct rds_report_request_index* index = rds_report_request_index_parse(reader);
				rds_index_add_report_request(ret, index);

				if (xmlTextReaderRead(reader) != 1)
				{
					oscap_setxmlerr(xmlGetLastError());
				}
			}
		}
		else if (strcmp(name, "assets") == 0)
		{
			if (xmlTextReaderRead(reader) != 1)
			{
				oscap_setxmlerr(xmlGetLastError());
			}

			while (oscap_to_start_element(reader, 2))
			{
				if (strcmp((const char*)xmlTextReaderConstLocalName(reader), "asset") != 0)
				{
					// TODO: warning?
					continue;
				}

				struct rds_asset_index* index = rds_asset_index_parse(reader);
				rds_index_add_asset(ret, index);

				if (xmlTextReaderRead(reader) != 1)
				{
					oscap_setxmlerr(xmlGetLastError());
				}
			}
		}
		else if (strcmp(name, "reports") == 0)
		{
			if (xmlTextReaderRead(reader) != 1)
			{
				oscap_setxmlerr(xmlGetLastError());
			}

			while (oscap_to_start_element(reader, 2))
			{
				if (strcmp((const char*)xmlTextReaderConstLocalName(reader), "report") != 0)
				{
					// TODO: warning?
					continue;
				}

				struct rds_report_index* index = rds_report_index_parse(reader);
				rds_index_add_report(ret, index);

				if (xmlTextReaderRead(reader) != 1)
				{
					oscap_setxmlerr(xmlGetLastError());
				}
			}
		}
		else
		{
			oscap_seterr(OSCAP_EFAMILY_XML, "Unknown element '%s' encountered while parsing Result DataStream to rds_index, skipping...", name);
		}

		xmlTextReaderRead(reader);
	}

	return ret;
}

struct rds_index *rds_index_import(const char* file)
{
	xmlTextReaderPtr reader = xmlReaderForFile(file, NULL, 0);
	if (!reader) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "Unable to open file: '%s'", file);
		return NULL;
	}

	while (xmlTextReaderRead(reader) == 1 && xmlTextReaderNodeType(reader) != 1);
	struct rds_index* ret = rds_index_parse(reader);
	xmlFreeTextReader(reader);

	return ret;
}

struct rds_report_request_index *rds_report_request_index_iterator_next(struct rds_report_request_index_iterator *it)
{
	return (struct rds_report_request_index*)(oscap_iterator_next((struct oscap_iterator*)it));
}

bool rds_report_request_index_iterator_has_more(struct rds_report_request_index_iterator *it)
{
	return oscap_iterator_has_more((struct oscap_iterator*)it);
}

void rds_report_request_index_iterator_free(struct rds_report_request_index_iterator *it)
{
	oscap_iterator_free((struct oscap_iterator*)it);
}

struct rds_asset_index *rds_asset_index_iterator_next(struct rds_asset_index_iterator *it)
{
	return (struct rds_asset_index*)(oscap_iterator_next((struct oscap_iterator*)it));
}

bool rds_asset_index_iterator_has_more(struct rds_asset_index_iterator *it)
{
	return oscap_iterator_has_more((struct oscap_iterator*)it);
}

void rds_asset_index_iterator_free(struct rds_asset_index_iterator *it)
{
	oscap_iterator_free((struct oscap_iterator*)it);
}

struct rds_report_index *rds_report_index_iterator_next(struct rds_report_index_iterator *it)
{
	return (struct rds_report_index*)(oscap_iterator_next((struct oscap_iterator*)it));
}

bool rds_report_index_iterator_has_more(struct rds_report_index_iterator *it)
{
	return oscap_iterator_has_more((struct oscap_iterator*)it);
}

void rds_report_index_iterator_free(struct rds_report_index_iterator *it)
{
	oscap_iterator_free((struct oscap_iterator*)it);
}
