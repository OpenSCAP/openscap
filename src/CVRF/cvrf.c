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
 * Authors:
 * 		Katarina Jankov <kj226@cornell.edu>
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "public/cvrf.h"
#include "cvrf_priv.h"

#include <libxml/xmlreader.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>
#include <libxml/xpathInternals.h>

#include "common/_error.h"
#include "common/util.h"
#include "common/xmltext_priv.h"
#include "common/list.h"
#include "source/public/oscap_source.h"
#include "source/oscap_source_priv.h"

#define CVRF_SUPPORTED "1.1"


struct cvrf_index *cvrf_index_import(struct oscap_source *index_source) {
	__attribute__nonnull__(index_source);
	if (index_source == NULL)
		return NULL;

	return cvrf_index_parse_xml(index_source);
}

/**
 * Public function to import CVRF model from OSCAP import source.
 * Function returns CVRF model, need to free source after calling this function
 */
struct cvrf_model *cvrf_model_import(struct oscap_source *source) {
	__attribute__nonnull__(source);
	if (source == NULL)
		return NULL;

	xmlTextReaderPtr reader = oscap_source_get_xmlTextReader(source);
	if (!reader) {
		return NULL;
	}
	if (xmlTextReaderNextNode(reader) == -1) {
		xmlFreeTextReader(reader);
		return NULL;
	}
	struct cvrf_model *model = cvrf_model_parse(reader);
	xmlFreeTextReader(reader);
	return model;
}

struct oscap_source *cvrf_index_get_export_source(struct cvrf_index *index) {
	if (index == NULL)
		return NULL;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}
	cvrf_index_to_dom(index, doc, NULL, NULL);
	return oscap_source_new_from_xmlDoc(doc, NULL);
}

struct oscap_source *cvrf_model_get_export_source(struct cvrf_model *model) {
	if (model == NULL)
		return NULL;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}
	cvrf_model_to_dom(model, doc, NULL, NULL);
	return oscap_source_new_from_xmlDoc(doc, NULL);
}

const char * cvrf_model_supported(void)
{
	return CVRF_SUPPORTED;
}
