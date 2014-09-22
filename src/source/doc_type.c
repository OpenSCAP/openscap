/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/xmlreader.h>
#include <string.h>

#include "common/debug_priv.h"
#include "common/_error.h"
#include "common/public/oscap.h"
#include "doc_type_priv.h"

int oscap_determine_document_type_reader(xmlTextReader *reader, oscap_document_type_t *doc_type)
{
        const char* elm_name = NULL;
        *doc_type = 0;

        /* find root element */
        while (xmlTextReaderRead(reader) == 1
               && xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);

        /* identify document type */
        elm_name = (const char *) xmlTextReaderConstLocalName(reader);
        if (!elm_name) {
                oscap_setxmlerr(xmlGetLastError());
                return -1;
        }
        else if (!strcmp("oval_definitions", elm_name)) {
                *doc_type = OSCAP_DOCUMENT_OVAL_DEFINITIONS;
        }
        else if (!strcmp("oval_directives", elm_name)) {
                *doc_type = OSCAP_DOCUMENT_OVAL_DIRECTIVES;
        }
        else if (!strcmp("oval_results", elm_name)) {
                *doc_type = OSCAP_DOCUMENT_OVAL_RESULTS;
        }
        else if (!strcmp("oval_system_characteristics", elm_name)) {
                *doc_type = OSCAP_DOCUMENT_OVAL_SYSCHAR;
        }
        else if (!strcmp("oval_variables", elm_name)) {
                *doc_type = OSCAP_DOCUMENT_OVAL_VARIABLES;
        }
	else if (oscap_streq("Benchmark", elm_name) || oscap_streq("TestResult", elm_name)) {
                *doc_type = OSCAP_DOCUMENT_XCCDF;
        }
		else if (!strcmp("Tailoring", elm_name)) {
			*doc_type = OSCAP_DOCUMENT_XCCDF_TAILORING;
		}
        else if (!strcmp("cpe-list", elm_name)) {
                *doc_type = OSCAP_DOCUMENT_CPE_DICTIONARY;
        }
        else if (!strcmp("platform-specification", elm_name)) {
                *doc_type = OSCAP_DOCUMENT_CPE_LANGUAGE;
        }
        else if (!strcmp("nvd", elm_name)) {
                *doc_type = OSCAP_DOCUMENT_CVE_FEED;
        }
        else if (!strcmp("data-stream-collection", elm_name)) {
                *doc_type = OSCAP_DOCUMENT_SDS;
        }
        else if (!strcmp("asset-report-collection", elm_name)) {
                *doc_type = OSCAP_DOCUMENT_ARF;
	}
	else if (!strcmp("sce_results", elm_name)) {
                *doc_type = OSCAP_DOCUMENT_SCE_RESULT;
        }
	else {
                return -1;
        }

        dI("Identified document type: %s\n", elm_name);

        return 0;
}
