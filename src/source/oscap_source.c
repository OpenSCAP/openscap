/*
 * Copyright 2014 Red Hat Inc., Durham, North Carolina.
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
 * Author:
 *     Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>

#include "common/alloc.h"
#include "common/elements.h"
#include "common/_error.h"
#include "common/public/oscap.h"
#include "common/util.h"
#include "CPE/public/cpe_lang.h"
#include "doc_type_priv.h"
#include "oscap_source.h"
#include "oscap_source_priv.h"
#include "OVAL/oval_parser_impl.h"
#include "OVAL/public/oval_definitions.h"
#include "source/schematron_priv.h"
#include "source/validate_priv.h"
#include "XCCDF/public/xccdf_benchmark.h"

typedef enum oscap_source_type {
	OSCAP_SRC_FROM_USER_XML_FILE = 1,               ///< The source originated from XML supplied by user
	OSCAP_SRC_FROM_EXPORT_XML_DOM,                  ///< The source originated from our exporter
	// TODO: node originated from DataStream
	// TODO: originated from bzip2ed file
	// TODO: originated from internal temp file (and this shall be later removed)
	// TODO: downloaded from an http address (XCCDF can refer to remote sources)
} oscap_source_type_t;

struct oscap_source {
	oscap_document_type_t scap_type;                ///< Type of SCAP document (XCCDF, OVAL, ...)
        struct {
		oscap_source_type_t type;               ///< Internal type of the oscap_source
		char *version;                          ///< Version of the particular document type
		const char *filepath;                   ///< Filepath (if originated from file)
	} origin;                                       ///
	struct {
		xmlDoc *doc;                            /// DOM
	} xml;
};

struct oscap_source *oscap_source_new_from_file(const char *filepath)
{
	/* TODO: At the end of the day, this shall be the only place in
	 * the library  where a path to filename is set from the outside.
	 */
	struct oscap_source *source = (struct oscap_source *) oscap_calloc(1, sizeof(struct oscap_source));
	source->origin.filepath = oscap_strdup(filepath);
	source->origin.type = OSCAP_SRC_FROM_USER_XML_FILE;
	return source;
}

struct oscap_source *oscap_source_new_from_xmlDoc(xmlDoc *doc, const char *filepath)
{
	struct oscap_source *source = (struct oscap_source *) oscap_calloc(1, sizeof(struct oscap_source));
	source->origin.type = OSCAP_SRC_FROM_EXPORT_XML_DOM;
	source->origin.filepath = oscap_strdup(filepath ? filepath : "NONEXISTENT");
	source->xml.doc = doc;
	return source;
}

void oscap_source_free(struct oscap_source *source)
{
	if (source != NULL) {
		oscap_free(source->origin.filepath);
		if (source->xml.doc != NULL) {
			xmlFreeDoc(source->xml.doc);
		}
		oscap_free(source->origin.version);
		oscap_free(source);
	}
}

/**
 * Returns human readable description of oscap_source origin
 */
const char *oscap_source_readable_origin(const struct oscap_source *source)
{
	return source->origin.filepath;
}

xmlTextReader *oscap_source_get_xmlTextReader(struct oscap_source *source)
{
	xmlDoc *doc = oscap_source_get_xmlDoc(source);
	if (doc == NULL) {
		return NULL;
	}
	xmlTextReader *reader = xmlReaderWalker(doc);
	if (reader == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Unable to create xmlTextReader for %s", oscap_source_readable_origin(source));
		oscap_setxmlerr(xmlGetLastError());
	}
	return reader;
}

oscap_document_type_t oscap_source_get_scap_type(struct oscap_source *source)
{
	if (source->scap_type == 0) {
		xmlTextReader *reader = oscap_source_get_xmlTextReader(source);
		if (reader == NULL) {
			// the oscap error is already set
			return 0;
		}
		if (oscap_determine_document_type_reader(reader, &(source->scap_type)) == -1) {
			oscap_seterr(OSCAP_EFAMILY_XML, "Unknown document type: '%s'", oscap_source_readable_origin(source));
		}
		xmlFreeTextReader(reader);
	}
	return source->scap_type;
}

xmlDoc *oscap_source_get_xmlDoc(struct oscap_source *source)
{
	if (source->xml.doc == NULL) {
		source->xml.doc = xmlReadFile(source->origin.filepath, NULL, 0);
		if (source->xml.doc == NULL) {
			oscap_setxmlerr(xmlGetLastError());
			oscap_seterr(OSCAP_EFAMILY_XML, "Unable to parse XML at: '%s'", oscap_source_readable_origin(source));
		}
	}
	return source->xml.doc;
}

int oscap_source_validate(struct oscap_source *source, xml_reporter reporter, void *user)
{
	int ret = oscap_source_validate_priv(source, oscap_source_get_scap_type(source),
			oscap_source_get_schema_version(source), reporter, user);
	if (ret != 0) {
		const char *type_name = oscap_document_type_to_string(oscap_source_get_scap_type(source));
		if (type_name == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unrecognized document type for: ", oscap_source_readable_origin(source));
		} else {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Invalid %s (%s) content in %s.\n", type_name,
				oscap_source_get_schema_version(source), oscap_source_readable_origin(source));
		}
	}
	return ret;
}

int oscap_source_validate_schematron(struct oscap_source *source, const char *outfile)
{
	return oscap_source_validate_schematron_priv(source, oscap_source_get_scap_type(source),
			oscap_source_get_schema_version(source), outfile);
}

const char *oscap_source_get_schema_version(struct oscap_source *source)
{
	if (source->origin.version == NULL) {
		xmlTextReader *reader = oscap_source_get_xmlTextReader(source);
		if (reader == NULL) {
			return NULL;
		}
		switch (oscap_source_get_scap_type(source)) {
			case OSCAP_DOCUMENT_SDS:
				source->origin.version = strdup("1.2");
				break;
			case OSCAP_DOCUMENT_ARF:
				source->origin.version = strdup("1.1");
			case OSCAP_DOCUMENT_OVAL_DEFINITIONS:
			case OSCAP_DOCUMENT_OVAL_VARIABLES:
			case OSCAP_DOCUMENT_OVAL_DIRECTIVES:
			case OSCAP_DOCUMENT_OVAL_SYSCHAR:
			case OSCAP_DOCUMENT_OVAL_RESULTS:
				source->origin.version = oval_determine_document_schema_version_priv(
					reader, oscap_source_get_scap_type(source));
				break;
			case OSCAP_DOCUMENT_XCCDF:
			case OSCAP_DOCUMENT_XCCDF_TAILORING:
				source->origin.version = xccdf_detect_version(source->origin.filepath);
				break;
			case OSCAP_DOCUMENT_CPE_DICTIONARY:
				source->origin.version = cpe_dict_detect_version(source->origin.filepath);
				break;
			case OSCAP_DOCUMENT_CPE_LANGUAGE:
				source->origin.version = cpe_lang_model_detect_version(source->origin.filepath);
				break;
			case OSCAP_DOCUMENT_CVE_FEED:
				source->origin.version = strdup("2.0");
				break;
			case OSCAP_DOCUMENT_SCE_RESULT:
				source->origin.version = strdup("1.0");
				break;
			default:
				oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not determine origin.version for document %s: Unknown type: %s",
					oscap_source_readable_origin(source),
					oscap_document_type_to_string(oscap_source_get_scap_type(source)));
				break;
		}
		xmlFreeTextReader(reader);
	}
	return source->origin.version;
}

int oscap_source_save_as(struct oscap_source *source, const char *filename)
{
	const char *target = filename != NULL ? filename : oscap_source_readable_origin(source);
	xmlDoc *doc = oscap_source_get_xmlDoc(source);
	if (doc == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not save document to %s: DOM representation not available.", target);
		return -1;
	}
	return oscap_xml_save_filename(target, doc);
}
