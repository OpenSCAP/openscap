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
#include <fcntl.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlerror.h>

#include "common/alloc.h"
#include "common/elements.h"
#include "common/_error.h"
#include "common/debug_priv.h"
#include "common/public/oscap.h"
#include "common/util.h"
#include "CPE/public/cpe_lang.h"
#include "CPE/cpedict_priv.h"
#include "CPE/cpelang_priv.h"
#include "doc_type_priv.h"
#include "oscap_source.h"
#include "common/oscap_string.h"
#include "oscap_source_priv.h"
#include "OVAL/oval_parser_impl.h"
#include "OVAL/public/oval_definitions.h"
#include "source/bz2_priv.h"
#include "source/schematron_priv.h"
#include "source/validate_priv.h"
#include "XCCDF/elements.h"
#include "XCCDF/public/xccdf_benchmark.h"

typedef enum oscap_source_type {
	OSCAP_SRC_FROM_USER_XML_FILE = 1,               ///< The source originated from XML file supplied by user
	OSCAP_SRC_FROM_USER_MEMORY,                     ///< The source originated from memory supplied by user
	OSCAP_SRC_FROM_XML_DOM,                         ///< The source originated from XML DOM (most often from DataStream).
	// TODO: downloaded from an http address (XCCDF can refer to remote sources)
} oscap_source_type_t;

struct oscap_source {
	oscap_document_type_t scap_type;                ///< Type of SCAP document (XCCDF, OVAL, ...)
	struct {
		oscap_source_type_t type;               ///< Internal type of the oscap_source
		char *version;                          ///< Version of the particular document type
		char *filepath;                         ///< Filepath (if originated from file)
		char *memory;                           ///< Memory buffer (if originated from memory)
		size_t memory_size;                     ///< Size of the memory buffer (if originated from memory)
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

/**
 * Allocate oscap_source struct and fill for memory data
 * @param size_t size Size of data
 * @param const char* filepath Path of file
 * @return Allocated struct
 */
static struct oscap_source* _create_oscap_source(size_t size, const char* filepath)
{
	struct oscap_source *source = (struct oscap_source *) oscap_calloc(1, sizeof(struct oscap_source));
	source->origin.memory_size = size;
	source->origin.type = OSCAP_SRC_FROM_USER_MEMORY;
	source->origin.filepath = oscap_strdup(filepath ? filepath : "NONEXISTENT");
	return source;
}

struct oscap_source *oscap_source_new_from_memory(const char *buffer, size_t size, const char *filepath)
{
	struct oscap_source *source = _create_oscap_source(size, filepath);
	source->origin.memory = oscap_calloc(1, size);
	memcpy(source->origin.memory, buffer, size);
	return source;
}

struct oscap_source *oscap_source_new_take_memory(char *buffer, size_t size, const char *filepath)
{
	struct oscap_source* source = _create_oscap_source(size, filepath);
	source->origin.memory = buffer;
	return source;
}

struct oscap_source *oscap_source_new_from_xmlDoc(xmlDoc *doc, const char *filepath)
{
	struct oscap_source *source = (struct oscap_source *) oscap_calloc(1, sizeof(struct oscap_source));
	source->origin.type = OSCAP_SRC_FROM_XML_DOM;
	source->origin.filepath = oscap_strdup(filepath ? filepath : "NONEXISTENT");
	source->xml.doc = doc;
	return source;
}

void oscap_source_free(struct oscap_source *source)
{
	if (source != NULL) {
		oscap_free(source->origin.filepath);
		oscap_free(source->origin.memory);
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
	// TODO: This may just return NONEXISTANT for sources from raw memory or xmlDoc
	//       and that's not very useful.
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
	if (source->scap_type == OSCAP_DOCUMENT_UNKNOWN) {
		xmlTextReader *reader = oscap_source_get_xmlTextReader(source);
		if (reader == NULL) {
			// the oscap error is already set
			return OSCAP_DOCUMENT_UNKNOWN;
		}
		if (oscap_determine_document_type_reader(reader, &(source->scap_type)) == -1) {
			oscap_seterr(OSCAP_EFAMILY_XML, "Unknown document type: '%s'", oscap_source_readable_origin(source));
			// in case of error scap_type must remain UNKNOWN
			assert(source->scap_type == OSCAP_DOCUMENT_UNKNOWN);
		}
		xmlFreeTextReader(reader);
	}
	return source->scap_type;
}

static void xmlErrorCb(struct oscap_string *buffer, const char * format, ...)
{
	va_list ap;
	va_start(ap, format);

	char* error_msg = oscap_vsprintf(format, ap);
	oscap_string_append_string(buffer, error_msg);
	oscap_free(error_msg);

	va_end(ap);
}

xmlDoc *oscap_source_get_xmlDoc(struct oscap_source *source)
{
	// We check origin.memory first because even with it being non-NULL
	// filepath will be non-NULL, it will contain the filepath hint.
	struct oscap_string *xml_error_string = oscap_string_new();
	xmlSetGenericErrorFunc(xml_error_string, (xmlGenericErrorFunc)xmlErrorCb);

	if (source->xml.doc == NULL) {
		if (source->origin.memory != NULL) {
#ifdef HAVE_BZ2
			if (bz2_memory_is_bzip(source->origin.memory, source->origin.memory_size)) {
				source->xml.doc = bz2_mem_read_doc(source->origin.memory, source->origin.memory_size);
			} else
#endif
			{
				source->xml.doc = xmlReadMemory(source->origin.memory, source->origin.memory_size, NULL, NULL, 0);
				if (source->xml.doc == NULL) {
					oscap_setxmlerr(xmlGetLastError());
					const char *error_msg = oscap_string_get_cstr(xml_error_string);
					oscap_seterr(OSCAP_EFAMILY_XML, "%sUnable to parse XML from user memory buffer", error_msg);
					oscap_string_clear(xml_error_string);
				}
			}
		}
		else {
			int fd = open(source->origin.filepath, O_RDONLY);
			if ( fd == -1 ){
				source->xml.doc = NULL;
				oscap_seterr(OSCAP_EFAMILY_GLIBC, "Unable to open file: '%s'", oscap_source_readable_origin(source));
			} else {
#ifdef HAVE_BZ2
				if (bz2_fd_is_bzip(fd)) {
					source->xml.doc = bz2_fd_read_doc(fd);
				} else
#endif
				{
					source->xml.doc = xmlReadFd(fd, NULL, NULL, 0);
					if (source->xml.doc == NULL) {
						oscap_setxmlerr(xmlGetLastError());
						const char *error_msg = oscap_string_get_cstr(xml_error_string);
						oscap_seterr(OSCAP_EFAMILY_XML, "%sUnable to parse XML at: '%s'", error_msg, oscap_source_readable_origin(source));
						oscap_string_clear(xml_error_string);
					}
				}
				close(fd);
			}
		}
	}

	initGenericErrorDefaultFunc(NULL);

	if (!oscap_string_empty(xml_error_string)) {
		const char *error_msg = oscap_string_get_cstr(xml_error_string);
		oscap_seterr(OSCAP_EFAMILY_XML, "%sFound xml error.", error_msg);
	}

	oscap_string_free(xml_error_string);
	return source->xml.doc;
}

int oscap_source_validate(struct oscap_source *source, xml_reporter reporter, void *user)
{
	int ret;
	oscap_document_type_t scap_type = oscap_source_get_scap_type(source);

	if (scap_type == OSCAP_DOCUMENT_UNKNOWN) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unrecognized document type for: %s", oscap_source_readable_origin(source));
		ret = -1;
	} else {
		const char *schema_version = oscap_source_get_schema_version(source);
		const char *type_name = oscap_document_type_to_string(scap_type);
		const char *origin = oscap_source_readable_origin(source);
		dD("Validating %s (%s) document from %s.\n", type_name, schema_version, origin);
		ret = oscap_source_validate_priv(source, scap_type, schema_version, reporter, user);
		if (ret != 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Invalid %s (%s) content in %s.", type_name, schema_version, origin);
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
				break;
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
				source->origin.version = xccdf_detect_version_priv(reader);
				break;
			case OSCAP_DOCUMENT_CPE_DICTIONARY:
				source->origin.version = cpe_dict_detect_version_priv(reader);
				break;
			case OSCAP_DOCUMENT_CPE_LANGUAGE:
				source->origin.version = cpe_lang_model_detect_version_priv(reader);
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
	// TODO: This assumes XML and xmlDoc being available
	const char *target = filename != NULL ? filename : oscap_source_readable_origin(source);
	xmlDoc *doc = oscap_source_get_xmlDoc(source);
	if (doc == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not save document to %s: DOM representation not available.", target);
		return -1;
	}
	return oscap_xml_save_filename(target, doc) == 1 ? 0 : -1;
}

int oscap_source_get_raw_memory(struct oscap_source *source, char **buffer, size_t *size)
{
	if (source->origin.memory != NULL) {
		char *ret = (char*)malloc(source->origin.memory_size);
		memcpy(ret, source->origin.memory, source->origin.memory_size);
		*buffer = ret;
		*size = source->origin.memory_size;
		return 0;
	}
	else {
		xmlDoc *doc = oscap_source_get_xmlDoc(source);

		if (doc == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP,
				"Can't retrieve raw memory. Given oscap_source doesn't originate from "
				"raw memory and xmlDoc isn't available.");
			return 1;
		}

		// libxml2 asks us to use xmlFree on the returned buffer,
		// free works fine however. Instead of doing a memory copy dance
		// here we just let libxml2 fill the buffer.
		// However int and size_t can be different so we do the safe thing.
		int isize = 0;
		xmlDocDumpMemory(doc, (xmlChar**)buffer, &isize);
		*size = isize;
		return 0;
	}
}
