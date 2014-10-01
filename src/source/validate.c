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
#include <string.h>
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

static inline int oscap_validate_xml(struct oscap_source *source, const char *schemafile, xml_reporter reporter, void *arg)
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

// TODO do not hardcode versions... (duplicities)
// patch version fragments are intentionally left out, we strive to ship
// schemas of the newest patch versions of that particular minor.major version
// todo: ugly
// FIXME: we ship OCIL schemes but they aren't supported in openscap
//        and aren't in this list
struct oscap_schema_table_entry OSCAP_SCHEMAS_TABLE[] = {
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,	"5.3",	"oval/5.3/oval-definitions-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,	"5.4",	"oval/5.4/oval-definitions-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,	"5.5",	"oval/5.5/oval-definitions-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,	"5.6",	"oval/5.6/oval-definitions-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,	"5.7",	"oval/5.7/oval-definitions-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,	"5.8",	"oval/5.8/oval-definitions-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,	"5.9",	"oval/5.9/oval-definitions-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,	"5.10",	"oval/5.10/oval-definitions-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,	"5.10.1","oval/5.10.1/oval-definitions-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,	"5.8",	"oval/5.8/oval-directives-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,	"5.9",	"oval/5.9/oval-directives-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,	"5.10",	"oval/5.10/oval-directives-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,	"5.10.1","oval/5.10.1/oval-directives-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,		"5.3",	"oval/5.3/oval-results-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,		"5.4",	"oval/5.4/oval-results-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,		"5.5",	"oval/5.5/oval-results-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,		"5.6",	"oval/5.6/oval-results-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,		"5.7",	"oval/5.7/oval-results-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,		"5.8",	"oval/5.8/oval-results-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,		"5.9",	"oval/5.9/oval-results-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,		"5.10",	"oval/5.10/oval-results-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,		"5.10.1","oval/5.10.1/oval-results-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.3",	"oval/5.3/oval-system-characteristics-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.4",	"oval/5.4/oval-system-characteristics-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.5",	"oval/5.5/oval-system-characteristics-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.6",	"oval/5.6/oval-system-characteristics-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.7",	"oval/5.7/oval-system-characteristics-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.8",	"oval/5.8/oval-system-characteristics-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.9",	"oval/5.9/oval-system-characteristics-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.10",	"oval/5.10/oval-system-characteristics-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.10.1","oval/5.10.1/oval-system-characteristics-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.3",	"oval/5.3/oval-variables-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.4",	"oval/5.4/oval-variables-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.5",	"oval/5.5/oval-variables-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.6",	"oval/5.6/oval-variables-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.7",	"oval/5.7/oval-variables-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.8",	"oval/5.8/oval-variables-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.9",	"oval/5.9/oval-variables-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.10",	"oval/5.10/oval-variables-schema.xsd"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.10.1","oval/5.10.1/oval-variables-schema.xsd"},
	{OSCAP_DOCUMENT_SCE_RESULT,		"1.0",	"sce/1.0/sce-result-schema.xsd"},
	{OSCAP_DOCUMENT_XCCDF,			"1.2",	"xccdf/1.2/xccdf_1.2.xsd"},
	{OSCAP_DOCUMENT_XCCDF,			"1.1",	"xccdf/1.1/xccdf-schema.xsd"},
	{OSCAP_DOCUMENT_XCCDF_TAILORING,	"1.2",	"xccdf/1.2/xccdf_1.2.xsd"},
	{OSCAP_DOCUMENT_XCCDF_TAILORING,	"1.1",	"xccdf/1.1-tailoring/xccdf-1.1-tailoring.xsd"}, // unofficial openscap extension!
	{OSCAP_DOCUMENT_SDS,                "1.2",  "sds/1.2/scap-source-data-stream_1.2.xsd"},
	{OSCAP_DOCUMENT_ARF,                "1.1",  "arf/1.1/asset-reporting-format_1.1.0.xsd"},
	{OSCAP_DOCUMENT_CPE_DICTIONARY,		"2.0", "cpe/2.0/cpe-dictionary_2.0.xsd"},
	{OSCAP_DOCUMENT_CPE_DICTIONARY,		"2.1", "cpe/2.1/cpe-dictionary_2.1.xsd"},
	{OSCAP_DOCUMENT_CPE_DICTIONARY,		"2.2", "cpe/2.2/cpe-dictionary_2.2.xsd"},
	{OSCAP_DOCUMENT_CPE_DICTIONARY,		"2.3", "cpe/2.3/cpe-dictionary_2.3.xsd"},
	{OSCAP_DOCUMENT_CPE_LANGUAGE,		"2.3", "cpe/2.3/cpe-language_2.3.xsd"},
	{OSCAP_DOCUMENT_CVE_FEED,		"2.0", "cve/nvd-cve-feed_2.0.xsd"},
	{0, NULL, NULL }
};

int oscap_source_validate_priv(struct oscap_source *source, oscap_document_type_t doc_type, const char *version, xml_reporter reporter, void *user)
{
	if (version == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not determine version for file: %s", oscap_source_readable_origin(source));
		return -1;
	}

	/* find a right schema file */
	for (struct oscap_schema_table_entry *entry = OSCAP_SCHEMAS_TABLE; entry->doc_type != 0; ++entry) {
		if (entry->doc_type != doc_type || strcmp(entry->schema_version, version))
			continue;

		return oscap_validate_xml(source, entry->schema_path, reporter, user);
	}

	oscap_seterr(OSCAP_EFAMILY_OSCAP, "Schema file not found when trying to validate '%s'", oscap_source_readable_origin(source));
	return -1;
}
