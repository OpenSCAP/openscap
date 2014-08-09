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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/parser.h>
#include <libxml/xmlerror.h>
#include <libxml/xmlschemas.h>
#include <unistd.h>

#include "common/_error.h"
#include "common/util.h"
#include "oscap.h"
#include "oscap_source.h"
#include "source/oscap_source_priv.h"
#include "source/validate_priv.h"

struct ctxt {
	xml_reporter reporter;
	void *arg;
	char *filename;
};

static void oscap_xml_validity_handler(void *user, xmlErrorPtr error)
{
	struct ctxt * context = (struct ctxt *) user;

	if (context == NULL || context->reporter == NULL)
		return;

	if (error == NULL)
		error = xmlGetLastError();

	if (error->code == 3083) {
		/*
		libxml2 outputs a warning for something that is completely harmless
		and happens very often, clogging the screen and making real issues
		hard to spot.

		The message of the warning is:

		Skipping import of schema located at ...  for the namespace ...,
		since this namespace was already imported with the schema located at ..."

		We can't prevent this with the schemas because they are interdependent
		and it is not a good idea to alter XSD schemas comming from standard
		bodies anyways.
		*/

		// ignore the warning as if it never existed
		return;
	}

	const char *file = error->file;
	if (file == NULL)
		file = context->filename;

	context->reporter(file, error->line, error->message, context->arg);
}

int oscap_validate_xml(struct oscap_source *source, const char *schemafile, xml_reporter reporter, void *arg)
{
	int result = -1;
	xmlSchemaParserCtxtPtr parser_ctxt = NULL;
	xmlSchemaPtr schema = NULL;
	xmlSchemaValidCtxtPtr ctxt = NULL;
	xmlDocPtr doc = NULL;

	struct ctxt context = { reporter, arg, (void*) oscap_source_readable_origin(source)};

	if (schemafile == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "'schemafile' == NULL");
		return -1;
	}

	char * schemapath = oscap_sprintf("%s%s%s", oscap_path_to_schemas(), "/", schemafile);
	if (access(schemapath, R_OK)) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Schema file '%s' not found in path '%s' when trying to validate '%s'",
				schemafile, oscap_path_to_schemas(), oscap_source_readable_origin(source));
		goto cleanup;
	}

	parser_ctxt = xmlSchemaNewParserCtxt(schemapath);
	if (parser_ctxt == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Could not create parser context for validation");
		goto cleanup;
	}

	xmlSchemaSetParserStructuredErrors(parser_ctxt, oscap_xml_validity_handler, &context);

	schema = xmlSchemaParse(parser_ctxt);
	if (schema == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Could not parse XML schema");
		goto cleanup;
	}

	ctxt = xmlSchemaNewValidCtxt(schema);
	if (ctxt == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Could not create validation context");
		goto cleanup;
	}

	xmlSchemaSetValidStructuredErrors(ctxt, oscap_xml_validity_handler, &context);

	doc = oscap_source_get_xmlDoc(source);
	if (!doc)
		goto cleanup;

	result = xmlSchemaValidateDoc(ctxt, doc);

	/*
	 * xmlSchemaValidateFile() returns "-1" if document is not well formed
	 * thefore we ignore libxml internal errors here and map return code to
	 * either pass or fail.
	 */
	if (result != 0)
		result = 1;
	/* This would be nicer
	 * if (result ==  -1)
	 *	oscap_setxmlerr(xmlGetLastError());
	*/

cleanup:
	if (ctxt)
		xmlSchemaFreeValidCtxt(ctxt);
	if (schema)
		xmlSchemaFree(schema);
	if (parser_ctxt)
		xmlSchemaFreeParserCtxt(parser_ctxt);
	oscap_free(schemapath);

	return result;
}
