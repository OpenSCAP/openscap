/*
 * Copyright 2009,2010,2011 Red Hat Inc., Durham, North Carolina.
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
 *      Peter Vrabec <pvrabec@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libexslt/exslt.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "public/oscap.h"
#include "_error.h"
#include "util.h"
#include "list.h"
#include "elements.h"
#include "assume.h"
#include "debug_priv.h"

#ifndef OSCAP_DEFAULT_SCHEMA_PATH
const char * const OSCAP_SCHEMA_PATH = "/usr/local/share/openscap/schemas";
#else
const char * const OSCAP_SCHEMA_PATH = OSCAP_DEFAULT_SCHEMA_PATH;
#endif

#ifndef OSCAP_DEFAULT_XSLT_PATH
const char * const OSCAP_XSLT_PATH = "/usr/local/share/openscap/xsl";
#else
const char * const OSCAP_XSLT_PATH = OSCAP_DEFAULT_XSLT_PATH;
#endif

#ifndef OSCAP_DEFAULT_CPE_PATH
const char * const OSCAP_CPE_PATH = "/usr/local/share/openscap/cpe";
#else
const char * const OSCAP_CPE_PATH = OSCAP_DEFAULT_CPE_PATH;
#endif

/* return default path if pathvar is not defined */
static const char * oscap_path_to(const char *pathvar, const char *defpath) {
	const char *path = NULL;

	if (pathvar != NULL)
		path = getenv(pathvar);

	if (path == NULL || oscap_streq(path, ""))
		path = defpath;

	return path;
}

const char * oscap_path_to_schemas() {
	return oscap_path_to("OSCAP_SCHEMA_PATH", OSCAP_SCHEMA_PATH);
}

static inline const char *oscap_path_to_xslt(void)
{
	return oscap_path_to("OSCAP_XSLT_PATH", OSCAP_XSLT_PATH);
}

OSCAP_DEPRECATED(
const char * oscap_path_to_schematron() {
	// It has never returned correct path to schematron files.
	return oscap_path_to_xslt();
}
)

const char * oscap_path_to_cpe() {
	return oscap_path_to("OSCAP_CPE_PATH", OSCAP_CPE_PATH);
}

void oscap_init(void)
{
    xmlInitParser();
    xsltInit();
    exsltRegisterAll();
}

void oscap_cleanup(void)
{
	oscap_clearerr();
	xsltCleanupGlobals();
	xmlCleanupParser();
}

const char *oscap_get_version(void) { return VERSION; }

struct ctxt {
	xml_reporter reporter;
	void *arg; 
	void *user;
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
		file = context->user;

	context->reporter(file, error->line, error->message, context->arg);
}

int oscap_validate_xml(const char *xmlfile, const char *schemafile, xml_reporter reporter, void *arg)
{
	int result = -1;
	xmlSchemaParserCtxtPtr parser_ctxt = NULL;
	xmlSchemaPtr schema = NULL;
	xmlSchemaValidCtxtPtr ctxt = NULL;
	xmlDocPtr doc = NULL;
	struct ctxt context = { reporter, arg, (void*) xmlfile };

        if (xmlfile == NULL) {
                oscap_seterr(OSCAP_EFAMILY_OSCAP, "'xmlfile' == NULL");
                return -1;
        }

        if (schemafile == NULL) {
                oscap_seterr(OSCAP_EFAMILY_OSCAP, "'schemafile' == NULL");
                return -1;
        }

	char * schemapath = oscap_sprintf("%s%s%s", oscap_path_to_schemas(), "/", schemafile);
	if (access(schemapath, R_OK)) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Schema file '%s' not found in path '%s' when trying to validate '%s'", schemafile, oscap_path_to_schemas(), xmlfile);
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

	doc = xmlReadFile(xmlfile, NULL, 0);
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
	if (doc) xmlFreeDoc(doc);
	if (ctxt)        xmlSchemaFreeValidCtxt(ctxt);
	if (schema)      xmlSchemaFree(schema);
	if (parser_ctxt) xmlSchemaFreeParserCtxt(parser_ctxt);
	oscap_free(schemapath);

	return result;
}

struct oscap_schema_table_entry {
	oscap_document_type_t doc_type;
	const char *schema_version;
	const char *schema_path;
};

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

int oscap_validate_document(const char *xmlfile, oscap_document_type_t doctype, const char *version, xml_reporter reporter, void *arg)
{
	struct oscap_schema_table_entry *entry;

	if (xmlfile == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "'xmlfile' == NULL");
		return -1;
	}

	if (access(xmlfile, R_OK)) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "%s '%s'", strerror(errno), xmlfile);
		return -1;
	}

	if (version == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not determine version for file: %s", xmlfile);
		return -1;
	}

	/* find a right schema file */
	for (entry = OSCAP_SCHEMAS_TABLE; entry->doc_type != 0; ++entry) {
		if (entry->doc_type != doctype || strcmp(entry->schema_version, version))
			continue;

		/* validate */
		return oscap_validate_xml(xmlfile, entry->schema_path, reporter, arg);
	}

	/* schema not found */
	if (entry->doc_type == 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Schema file not found when trying to validate '%s'", xmlfile);
		return -1;
	}

	/* we shouldn't get here */
	return -1;
}

#define XCCDF11_NS "http://checklists.nist.gov/xccdf/1.1"
#define XCCDF12_NS "http://checklists.nist.gov/xccdf/1.2"

/*
 * Goes through the tree (DFS) and changes namespace of all XCCDF 1.2 elements
 * to XCCDF 1.1 namespace URI. This ensures that the XCCDF works fine with
 * XSLTs provided in openscap.
 */
static int xccdf_ns_xslt_workaround(xmlDocPtr doc, xmlNodePtr node)
{
	if (node == NULL || node->ns == NULL || strcmp((const char*)node->ns->href, XCCDF12_NS) != 0) {
		// nothing to do, this part of the document isn't XCCDF 1.2
		return 0;
	}

	xmlNsPtr xccdf11 = xmlNewNs(node,
			BAD_CAST XCCDF11_NS,
			BAD_CAST "cdf11");

	xmlSetNs(node, xccdf11);

	xmlNodePtr child = node->children;

	for (; child != NULL; child = child->next) {
		if (child->type != XML_ELEMENT_NODE)
			continue;

		int res = xccdf_ns_xslt_workaround(doc, child);
		if (res != 0)
			return res;
	}

	return 0;
}

/*
 * Apply stylesheet on XML file.
 * If xsltfile is an absolute path to the stylesheet, path_to_xslt will not be used.
 *
 */
static int oscap_apply_xslt_path(const char *xmlfile, const char *xsltfile,
				 const char *outfile, const char **params, const char *path_to_xslt)
{
	xsltStylesheetPtr cur = NULL;
	xmlDocPtr doc = NULL, res = NULL;
	FILE *f = NULL;
	int ret = -1;

	size_t argc = 0;
	while(params[argc]) argc += 2;

	char *args[argc+1];
	memset(args, 0, sizeof(char*) * (argc + 1));

	// Should we change all XCCDF namespaces (versioned) to one?
	// This is a workaround needed to make XSLTs work with multiple versions.
	// (currently 1.1 and 1.2)
	bool ns_workaround = false;

	/*  is it an absolute path? */
	char * xsltpath;
	if (strstr(xsltfile, "/") == xsltfile) {
		xsltpath = strdup(xsltfile);
		if (access(xsltpath, R_OK)) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "XSLT file '%s' not found when trying to transform '%s'", xsltfile, xmlfile);
			goto cleanup;
		}
	}
	else {
		xsltpath = oscap_sprintf("%s%s%s", path_to_xslt, "/", xsltfile);
		if (access(xsltpath, R_OK)) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "XSLT file '%s' not found in path '%s' when trying to transform '%s'", xsltfile, path_to_xslt, xmlfile);
			goto cleanup;
		}

		if (strcmp(xsltfile, "xccdf-report.xsl") == 0 ||
				strcmp(xsltfile, "fix.xsl") == 0 ||
				strcmp(xsltfile, "security-guide.xsl") == 0)
			ns_workaround = true;
	}

	cur = xsltParseStylesheetFile(BAD_CAST xsltpath);
	if (cur == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not parse XSLT file '%s'", xsltpath);
		goto cleanup;
	}

	doc = xmlParseFile(xmlfile);
	if (doc == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not parse the XML document '%s'", xmlfile);
		goto cleanup;
	}

	if (ns_workaround) {
		if (xccdf_ns_xslt_workaround(doc, xmlDocGetRootElement(doc)) != 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Had problems employing XCCDF XSLT namespace workaround for XML document '%s'", xmlfile);
			goto cleanup;
		}
	}

	for (size_t i = 0; i < argc; i += 2) {
		args[i] = (char*) params[i];
		if (params[i+1]) args[i+1] = oscap_sprintf("'%s'", params[i+1]);
	}

	res = xsltApplyStylesheet(cur, doc, (const char **) args);
	if (res == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not apply XSLT(%s) to XML file(%s)", xsltpath, xmlfile);
		goto cleanup;
	}

	if (outfile)
		f = fopen(outfile, "w");
	else
		f = stdout;

	if (f == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not open output file '%s'", outfile ? outfile : "stdout");
		goto cleanup;
	}

	/* "calculate" return code */
	if ((ret=xsltSaveResultToFile(f, res, cur)) < 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not save result document");
		goto cleanup;
	}

cleanup:
	for (size_t i = 0; args[i]; i += 2) oscap_free(args[i+1]);
	if (outfile && f) fclose(f);
	if (cur) xsltFreeStylesheet(cur);
	if (res) xmlFreeDoc(res);
	if (doc) xmlFreeDoc(doc);
	oscap_free(xsltpath);

	return ret;
}

struct oscap_schema_table_entry OSCAP_SCHEMATRON_TABLE[] = {
        {OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.3",		"oval/5.3/oval-definitions-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.4",  	"oval/5.4/oval-definitions-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.5",  	"oval/5.5/oval-definitions-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.6",  	"oval/5.6/oval-definitions-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.7",  	"oval/5.7/oval-definitions-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.8",  	"oval/5.8/oval-definitions-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.9",  	"oval/5.9/oval-definitions-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.10",  	"oval/5.10/oval-definitions-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.10.1",	"oval/5.10.1/oval-definitions-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.3",		"oval/5.3/oval-system-characteristics-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.4",		"oval/5.4/oval-system-characteristics-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.5",		"oval/5.5/oval-system-characteristics-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.6",		"oval/5.6/oval-system-characteristics-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.7",		"oval/5.7/oval-system-characteristics-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.8",		"oval/5.8/oval-system-characteristics-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.9",		"oval/5.9/oval-system-characteristics-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.10",		"oval/5.10/oval-system-characteristics-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_SYSCHAR,		"5.10.1",	"oval/5.10.1/oval-system-characteristics-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_RESULTS,		"5.3",		"oval/5.3/oval-results-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_RESULTS,		"5.4",		"oval/5.4/oval-results-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_RESULTS,		"5.5",		"oval/5.5/oval-results-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_RESULTS,		"5.6",		"oval/5.6/oval-results-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_RESULTS,		"5.7",		"oval/5.7/oval-results-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_RESULTS,		"5.8",		"oval/5.8/oval-results-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_RESULTS,		"5.9",		"oval/5.9/oval-results-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_RESULTS,		"5.10",		"oval/5.10/oval-results-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_RESULTS,		"5.10.1",	"oval/5.10.1/oval-results-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.3",		"oval/5.3/oval-variables-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.4",		"oval/5.4/oval-variables-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.5",		"oval/5.5/oval-variables-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.6",		"oval/5.6/oval-variables-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.7",		"oval/5.7/oval-variables-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.8",		"oval/5.8/oval-variables-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.9",		"oval/5.9/oval-variables-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.10",		"oval/5.10/oval-variables-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_VARIABLES,		"5.10.1",	"oval/5.10.1/oval-variables-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_DIRECTIVES,	"5.8",		"oval/5.8/oval-directives-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_DIRECTIVES,	"5.9",		"oval/5.9/oval-directives-schematron.xsl"},
        {OSCAP_DOCUMENT_OVAL_DIRECTIVES,	"5.10",		"oval/5.10/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.10.1",       "oval/5.10.1/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_XCCDF,                  "1.2",          "xccdf/1.2/xccdf_1.2-schematron.xsl"},
};

int oscap_schematron_validate_document(const char *xmlfile, oscap_document_type_t doctype, const char *version, const char *outfile) {

        struct oscap_schema_table_entry *entry;
	const char *params[] = { NULL };

        if (xmlfile == NULL) {
                oscap_seterr(OSCAP_EFAMILY_OSCAP, "'xmlfile' == NULL");
                return -1;
        }

        if (access(xmlfile, R_OK)) {
                oscap_seterr(OSCAP_EFAMILY_GLIBC, "%s '%s'", strerror(errno), xmlfile);
                return -1;
        }

        if (version == NULL) {
                oscap_seterr(OSCAP_EFAMILY_OSCAP, "'version' == NULL");
                return -1;
        }

	/* find a right schematron file */
	for (entry = OSCAP_SCHEMATRON_TABLE; entry->doc_type != 0; ++entry) {
		if (entry->doc_type != doctype || strcmp(entry->schema_version, version))
			continue;

		/* validate */
                return oscap_apply_xslt_path(xmlfile, entry->schema_path, NULL, params, oscap_path_to_schemas());
	}

	/* schematron not found */
	if (entry->doc_type == 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Schematron rules not found when trying to validate '%s'", xmlfile);
		return -1;
	}

	/* we shouldn't get here */
	return -1;
}

int oscap_apply_xslt(const char *xmlfile, const char *xsltfile, const char *outfile, const char **params)
{
	return oscap_apply_xslt_path(xmlfile, xsltfile, outfile, params, oscap_path_to_xslt());
}

int oscap_determine_document_type(const char *document, oscap_document_type_t *doc_type) {
        xmlTextReaderPtr reader;
        const char* elm_name = NULL;
        *doc_type = 0;

        reader = xmlReaderForFile(document, NULL, 0);
        if (!reader) {
                oscap_seterr(OSCAP_EFAMILY_GLIBC, "Unable to open file: '%s'", document);
                return -1;
        }

	xmlTextReaderSetErrorHandler(reader, &libxml_error_handler, NULL);

        /* find root element */
        while (xmlTextReaderRead(reader) == 1
               && xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);

        /* identify document type */
        elm_name = (const char *) xmlTextReaderConstLocalName(reader);
        if (!elm_name) {
                oscap_setxmlerr(xmlGetLastError());
                xmlFreeTextReader(reader);
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
        else if (!strcmp("Benchmark", elm_name)) {
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
                oscap_seterr(OSCAP_EFAMILY_OVAL, "Unknown document type: '%s'", document);
                xmlFreeTextReader(reader);
                return -1;
        }

        dI("Identified document type: %s\n", elm_name);

        xmlFreeTextReader(reader);
        return 0;
}

const char *oscap_document_type_to_string(oscap_document_type_t type)
{
	switch (type) {
	case OSCAP_DOCUMENT_OVAL_DEFINITIONS:
		return "OVAL Definition";
	case OSCAP_DOCUMENT_OVAL_DIRECTIVES:
		return "OVAL Directives";
	case OSCAP_DOCUMENT_OVAL_RESULTS:
		return "OVAL Results";
	case OSCAP_DOCUMENT_OVAL_SYSCHAR:
		return "OVAL System Characteristics";
	case OSCAP_DOCUMENT_OVAL_VARIABLES:
		return "OVAL Variables";
	case OSCAP_DOCUMENT_SDS:
		return "SCAP Source Datastream";
	case OSCAP_DOCUMENT_XCCDF:
		return "XCCDF Checklist";
	case OSCAP_DOCUMENT_XCCDF_TAILORING:
		return "XCCDF Tailoring";
	case OSCAP_DOCUMENT_SCE_RESULT:
		return "SCE Results";
	case OSCAP_DOCUMENT_CPE_DICTIONARY:
		return "CPE Dictionary";
	case OSCAP_DOCUMENT_CPE_LANGUAGE:
		return "CPE Language";
	case OSCAP_DOCUMENT_ARF:
		return "ARF Result Datastream";
	case OSCAP_DOCUMENT_CVE_FEED:
		return "CVE NVD Feed";
	default:
		return NULL;
	}
}
