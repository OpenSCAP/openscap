/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#include <libxml/parser.h>
#include "public/oscap.h"
#include "_error.h"
#include "util.h"
#include "elements.h"
#include "reporter_priv.h"
#include <libxml/xmlschemas.h>
#include <string.h>

void oscap_cleanup(void)
{
	xmlCleanupParser();
}

#ifdef WIN32
const char *OSCAP_OS_PATH_DELIM  = "\\";
#else
const char *OSCAP_OS_PATH_DELIM  = "/";
#endif

const char *OSCAP_PATH_SEPARATOR = ":";



static const char *oscap_get_xsd_dir(const char *dir)
{
#ifdef OSCAP_XSD_DIR_ENVVAR
	if (dir == NULL) dir = getenv(OSCAP_XSD_DIR_ENVVAR);
#endif
#ifdef OSCAP_XSD_DIR
	if (dir == NULL) dir = OSCAP_XSD_DIR;
#endif
	if (dir == NULL) dir = ".";
	return dir;
}

static char *oscap_get_schema_filename(const char *xmlfile)
{
	if (xmlfile == NULL) return NULL;
	char *ret = NULL;
	struct oscap_nsinfo *info = oscap_nsinfo_new_file(xmlfile);
	if (info && info->root_entry && info->root_entry->schema_location)
		ret = oscap_strdup(info->root_entry->schema_location);
	oscap_nsinfo_free(info);
	return ret;
}

static char *oscap_get_schema_path(const char *xmlfile, const char *directory)
{
	if (xmlfile == NULL) return NULL;
	const char *dir = oscap_get_xsd_dir(directory);
	char *schemafile = oscap_get_schema_filename(xmlfile);
	if (schemafile == NULL) return NULL;
	char *schemapath = oscap_alloc(sizeof(char) * (strlen(schemafile) + strlen(OSCAP_OS_PATH_DELIM) + strlen(dir) + 1));
	sprintf(schemapath, "%s%s%s", dir, OSCAP_OS_PATH_DELIM, schemafile);
	oscap_free(schemafile);
	return schemapath;
}

static void oscap_xml_validity_handler(void *user, xmlErrorPtr error)
{
    oscap_reporter_report_xml(user, error);
}

bool oscap_validate_xml(const char *xmlfile, const char *schemafile, struct oscap_reporter *reporter)
{
	assert(xmlfile != NULL);
	assert(schemafile != NULL); // TODO: validate even w/o schema, just for well-formness

	bool result = false; int ret = 0;
	xmlSchemaParserCtxtPtr parser_ctxt = NULL;
	xmlSchemaPtr schema = NULL;
	xmlSchemaValidCtxtPtr ctxt = NULL;

	parser_ctxt = xmlSchemaNewParserCtxt(schemafile);
	if (parser_ctxt == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, xmlGetLastError() ? xmlGetLastError()->code : 0, "Could not create parser context for validation");
		goto cleanup;
	}

    xmlSchemaSetParserStructuredErrors(parser_ctxt, oscap_xml_validity_handler, reporter);

	schema = xmlSchemaParse(parser_ctxt);
	if (schema == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, xmlGetLastError() ? xmlGetLastError()->code : 0, "Could not parse XML schema");
		goto cleanup;
	}

	ctxt = xmlSchemaNewValidCtxt(schema);
	if (ctxt == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, xmlGetLastError() ? xmlGetLastError()->code : 0, "Could not create validation context");
		goto cleanup;
	}

    xmlSchemaSetValidStructuredErrors(ctxt, oscap_xml_validity_handler, reporter);

	ret = xmlSchemaValidateFile(ctxt, xmlfile, 0);
	switch (ret) {
		case  0: result = true; break;
		case -1: oscap_seterr(OSCAP_EFAMILY_XML, xmlGetLastError() ? xmlGetLastError()->code : 0, "Validation failure"); break;
		default: result = false; break;
	}

cleanup:
	if (ctxt)        xmlSchemaFreeValidCtxt(ctxt);
	if (schema)      xmlSchemaFree(schema);
	if (parser_ctxt) xmlSchemaFreeParserCtxt(parser_ctxt);

	return result;
}

