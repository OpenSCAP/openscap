/*
 * Copyright 2013--2016 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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
#include "rds_index_priv.h"


#include <libxml/xmlreader.h>
#include <string.h>

struct rds_report_request_index
{
	char *id;
};

struct rds_report_request_index *rds_report_request_index_new(void)
{
	struct rds_report_request_index *ret = calloc(1, sizeof(struct rds_report_request_index));
	ret->id = NULL;

	return ret;
}

void rds_report_request_index_free(struct rds_report_request_index *s)
{
	if (s != NULL) {
		free(s->id);
		free(s);
	}
}

const char *rds_report_request_index_get_id(struct rds_report_request_index *s)
{
	return s->id;
}

struct rds_report_request_index *rds_report_request_index_parse(xmlTextReaderPtr reader)
{
	// sanity check
	if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT ||
			strcmp((const char*)xmlTextReaderConstLocalName(reader), "report-request") != 0) {
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
