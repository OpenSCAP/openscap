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
#include <libxml/xmlreader.h>

#include "common/alloc.h"
#include "common/elements.h"
#include "common/_error.h"
#include "common/public/oscap.h"
#include "common/util.h"
#include "doc_type_priv.h"
#include "oscap_source.h"

typedef enum oscap_source_type {
	OSCAP_SRC_FROM_USER_XML_FILE = 1,               ///< The source originated from XML supplied by user
	// TODO: node originated from DataStream
	// TODO: originated from bzip2ed file
	// TODO: originated from internal temp file (and this shall be later removed)
	// TODO: downloaded from an http address (XCCDF can refer to remote sources)
} oscap_source_type_t;

struct oscap_source {
	oscap_document_type_t scap_type;                ///< Type of SCAP document (XCCDF, OVAL, ...)
        struct {
		oscap_source_type_t type;               ///< Internal type of the oscap_source
		const char *filepath;                   ///< Filepath (if originated from file)
	} origin;                                       ///
	struct {
		xmlTextReader *text_reader;		/// xmlTextReader assigned to read this source
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

void oscap_source_free(struct oscap_source *source)
{
	if (source != NULL) {
		oscap_free(source->origin.filepath);
		if (source->xml.text_reader != NULL) {
			xmlFreeTextReader(source->xml.text_reader);
		}
		oscap_free(source);
	}
}

/**
 * This attempts to builds a new xmlTextReader based on the information within the oscap_source.
 * Note that we tend to double/triple open as we need to determine document type, then to valide
 * it, then again to parse it.
 */
static inline xmlTextReader *_build_new_xmlTextReader(struct oscap_source *source)
{
	xmlTextReader *reader = xmlNewTextReaderFilename(source->origin.filepath);
	if (reader != NULL) {
		xmlTextReaderSetErrorHandler(reader, &libxml_error_handler, NULL);
	}
	return reader;
}

/**
 * Returns human readable description of oscap_source origin
 */
static inline const char *_readable_origin(const struct oscap_source *source)
{
	return source->origin.filepath;
}

xmlTextReader *oscap_source_get_xmlTextReader(struct oscap_source *source)
{
	if (source->xml.text_reader == NULL) {
		source->xml.text_reader = _build_new_xmlTextReader(source);
	}
	if (source->xml.text_reader == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Unable to open file: '%s' (%s)", _readable_origin(source), strerror(errno));
	}
	return source->xml.text_reader;
}

oscap_document_type_t oscap_source_get_scap_type(struct oscap_source *source)
{
	if (source->scap_type == 0) {
		xmlTextReader *reader = _build_new_xmlTextReader(source);
		if (reader == NULL) {
			oscap_seterr(OSCAP_EFAMILY_XML, "Unable to open file: '%s' (%s)", _readable_origin(source), strerror(errno));
			return 0;
		}
		if (oscap_determine_document_type_reader(reader, &(source->scap_type)) == -1) {
			oscap_seterr(OSCAP_EFAMILY_XML, "Unknown document type: '%s'", _readable_origin(source));
		}
		xmlFreeTextReader(reader);
	}
	return source->scap_type;
}
