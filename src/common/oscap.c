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
#include "reporter_priv.h"

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

#ifdef WIN32
const char * const OSCAP_OS_PATH_DELIM  = "\\";
#else
const char * const OSCAP_OS_PATH_DELIM  = "/";
#endif

const char *OSCAP_PATH_SEPARATOR = ":";

bool oscap_file_exists(const char *path, int mode)
{
	if (path == NULL) return false;
#if HAVE_UNISTD_H
	return access(path, mode) == 0;
#else
	FILE *f = fopen(path, "r");
	if (f) { fclose(f); return true; }
	else return false;
#endif
}

char *oscap_find_file(const char *filename, int mode, const char *pathvar, const char *defpath)
{
	if (filename == NULL) return NULL;
	if (strstr(filename, OSCAP_OS_PATH_DELIM) == filename)
		return oscap_strdup(filename); // it is an absolute path

    const char *path = NULL;
    char *pathdup = NULL;
    if (pathvar != NULL) path = getenv(pathvar);
	if (path == NULL || oscap_streq(path, "")) path = defpath;
    else pathdup = oscap_sprintf("%s:%s", path, defpath);

	if (!pathdup) pathdup = oscap_strdup(path);
	char **paths = oscap_split(pathdup, OSCAP_PATH_SEPARATOR);
	char **paths_bck = paths;
	char *ret = NULL;

	while (*paths) {
		if (oscap_streq(*paths, "")) continue;
		oscap_rtrim(*paths, '/');  // strip slases at the end of the path
		if (oscap_streq(*paths, "")) **paths = '/';

		char *curpath = oscap_sprintf("%s%s%s", *paths, OSCAP_OS_PATH_DELIM, filename);
		if (oscap_file_exists(curpath, mode)) {
			ret = curpath;
			break;
		}
		oscap_free(curpath);
		++paths;
	}

	oscap_free(pathdup);
	oscap_free(paths_bck);
	return ret;
}

static char *oscap_get_schema_path(const char *filename)
{
	return oscap_find_file(filename, R_OK, "OSCAP_SCHEMA_PATH", OSCAP_SCHEMA_PATH);
}

static void oscap_xml_validity_handler(void *user, xmlErrorPtr error)
{
    oscap_reporter_report_xml(user, error);
}

bool oscap_validate_xml(const char *xmlfile, const char *schemafile, oscap_reporter reporter, void *arg)
{
	assert(xmlfile != NULL);
	assert(schemafile != NULL); // TODO: validate even w/o schema, just for well-formness

	bool result = false; int ret = 0;
	xmlSchemaParserCtxtPtr parser_ctxt = NULL;
	xmlSchemaPtr schema = NULL;
	xmlSchemaValidCtxtPtr ctxt = NULL;
	struct oscap_reporter_context reporter_ctxt = { reporter, arg, (void*) xmlfile };
	char *schemapath = oscap_get_schema_path(schemafile);
	if (schemapath == NULL) {
		char* message = oscap_sprintf("Schema file '%s' not found! (When trying to validate '%s')", schemafile, xmlfile);
		oscap_seterr(OSCAP_EFAMILY_OSCAP, 0, message);
		oscap_free(message);
		goto cleanup;
	}

	parser_ctxt = xmlSchemaNewParserCtxt(schemapath);
	if (parser_ctxt == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, xmlGetLastError() ? xmlGetLastError()->code : 0, "Could not create parser context for validation");
		goto cleanup;
	}

	xmlSchemaSetParserStructuredErrors(parser_ctxt, oscap_xml_validity_handler, &reporter_ctxt);

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

	xmlSchemaSetValidStructuredErrors(ctxt, oscap_xml_validity_handler, &reporter_ctxt);

	ret = xmlSchemaValidateFile(ctxt, xmlfile, 0);
	switch (ret) {
		case  0: result = true; break;
		case -1: oscap_seterr(OSCAP_EFAMILY_XML, xmlGetLastError() ? xmlGetLastError()->code : 0, "Validation failure"); break;
		default: result = false; break;
	}

cleanup:
	oscap_free(schemapath);
	if (ctxt)        xmlSchemaFreeValidCtxt(ctxt);
	if (schema)      xmlSchemaFree(schema);
	if (parser_ctxt) xmlSchemaFreeParserCtxt(parser_ctxt);

	return result;
}

struct oscap_schema_table_entry {
	oscap_document_type_t type;
	const char *stdname;
	const char *def_version;
	const char *schema_fname;
};

// TODO do not hardcode versions... (duplicities)

// patch version fragments are intentionally left out, we strive to ship
// schemas of the newest patch versions of that particular minor.major version
struct oscap_schema_table_entry OSCAP_SCHEMAS_TABLE[] = {
	{ OSCAP_DOCUMENT_OVAL_DEFINITIONS, "oval",  "5.8",   "oval-definitions-schema.xsd"            },
	{ OSCAP_DOCUMENT_OVAL_VARIABLES,   "oval",  "5.8",   "oval-variables-schema.xsd"              },
	{ OSCAP_DOCUMENT_OVAL_RESULTS,     "oval",  "5.8",   "oval-results-schema.xsd"                },
	{ OSCAP_DOCUMENT_OVAL_SYSCHAR,     "oval",  "5.8",   "oval-system-characteristics-schema.xsd" },
	{ OSCAP_DOCUMENT_OVAL_DIRECTIVES,  "oval",  "5.8",   "oval-directives-schema.xsd"             },
	{ OSCAP_DOCUMENT_XCCDF,            "xccdf", "1.2",   "xccdf_1.2.xsd"                          },
	{ OSCAP_DOCUMENT_XCCDF,            "xccdf", "1.1",   "xccdf-schema.xsd"                       },
	{ OSCAP_DOCUMENT_SCE_RESULT,       "sce",   "1.0",   "sce-result-schema.xsd"                  },
	{ 0, NULL, NULL, NULL }
};

bool oscap_validate_document(const char *xmlfile, oscap_document_type_t doctype, const char *version, oscap_reporter reporter, void *arg)
{
	if (xmlfile == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, OSCAP_EINVARG, "No XML file given.");
		return false;
	}

	if (access(xmlfile, R_OK)) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC, errno, strerror(errno));
		return false;
	}

	struct oscap_schema_table_entry *entry = OSCAP_SCHEMAS_TABLE;
	for (; entry->type != 0; ++entry) {
		if (entry->type == doctype)  { // found entry
			if (version == NULL || strcmp(entry->def_version, version) == 0) {
				char *schemafile = oscap_sprintf("%s%s%s%s%s", entry->stdname, OSCAP_OS_PATH_DELIM, entry->def_version, OSCAP_OS_PATH_DELIM, entry->schema_fname);

				int old_stdout;
				if (version == NULL)
				{
					// we will not output various XSD validation warnings and errors
					// in case we don't even know which XSD to use precisely

					int devnull;
					fflush(stdout);
					old_stdout = dup(1);
					devnull = open("/dev/null", O_WRONLY);
					dup2(devnull, 1);
					close(devnull);
				}

				bool ret = oscap_validate_xml(xmlfile, schemafile, reporter, arg);

				if (version == NULL)
				{
					// restores stdout

					fflush(stdout);
					dup2(old_stdout, 1);
					close(old_stdout);
				}

				oscap_free(schemafile);

				if (ret) // the file is valid in at least one applicable schema
					return true;
				else if (version != NULL) // version was explicitly specified and the file didn't validate!
					return false;
			}
		}
	}

	oscap_seterr(OSCAP_EFAMILY_OSCAP, 0, "OpenSCAP is not able to validate this document with any schemas known to it.");
	return false;
}

bool oscap_apply_xslt_var(const char *xmlfile, const char *xsltfile, const char *outfile, const char **params, const char *pathvar, const char *defpath)
{
    bool ret = false;
    char *xsltpath = oscap_find_file(xsltfile, R_OK, pathvar, defpath);
    xsltStylesheetPtr cur = NULL;
    xmlDocPtr doc = NULL, res = NULL;
    FILE *f = NULL;
    size_t argc = 0; while(params[argc]) argc += 2;
    char *args[argc+1]; memset(args, 0, sizeof(char*) * (argc + 1));

    if (xsltpath == NULL) {
        oscap_seterr(OSCAP_EFAMILY_OSCAP, 0, "XSLT file to be used by the transformation was not found.");
        goto cleanup;
    }

    cur = xsltParseStylesheetFile(BAD_CAST xsltpath);
    if (cur == NULL) {
        oscap_seterr(OSCAP_EFAMILY_OSCAP, 0, "Could not parse XSLT file");
        goto cleanup;
    }

    doc = xmlParseFile(xmlfile);
    if (doc == NULL) {
        oscap_seterr(OSCAP_EFAMILY_OSCAP, 0, "Could not parse the XML document");
        goto cleanup;
    }

    for (size_t i = 0; i < argc; i += 2) {
        args[i] = (char*) params[i];
        if (params[i+1]) args[i+1] = oscap_sprintf("'%s'", params[i+1]);
    }

    res = xsltApplyStylesheet(cur, doc, (const char **) args);
    if (res == NULL) {
        oscap_seterr(OSCAP_EFAMILY_OSCAP, 0, "Could not apply XSLT to your XML file");
        goto cleanup;
    }

    if (outfile) f = fopen(outfile, "w");
    else f = stdout;

    if (f == NULL) {
        oscap_seterr(OSCAP_EFAMILY_OSCAP, 0, "Could not open output file");
        goto cleanup;
    }

    if (xsltSaveResultToFile(f, res, cur) < 0) {
        oscap_seterr(OSCAP_EFAMILY_OSCAP, 0, "Could not save result document");
        goto cleanup;
    }

    ret = true;

cleanup:
    for (size_t i = 0; args[i]; i += 2) oscap_free(args[i+1]);
    if (f) fclose(f);
    if (cur) xsltFreeStylesheet(cur);
    if (res) xmlFreeDoc(res);
    if (doc) xmlFreeDoc(doc);
    oscap_free(xsltpath);
    return ret;
}

bool oscap_apply_xslt(const char *xmlfile, const char *xsltfile, const char *outfile, const char **params)
{
	return oscap_apply_xslt_var(xmlfile, xsltfile, outfile, params, "OSCAP_XSLT_PATH", OSCAP_XSLT_PATH);
}

