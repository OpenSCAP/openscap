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
#include "oscap_source.h"
#include "oscapxml.h"
#include "source/schematron_priv.h"
#include "source/validate_priv.h"
#include "source/xslt_priv.h"

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

const char *oscap_path_to_xslt(void)
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

int oscap_validate_document(const char *xmlfile, oscap_document_type_t doctype, const char *version, xml_reporter reporter, void *arg)
{
	if (xmlfile == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "'xmlfile' == NULL");
		return -1;
	}

	if (access(xmlfile, R_OK)) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "%s '%s'", strerror(errno), xmlfile);
		return -1;
	}

	struct oscap_source *source = oscap_source_new_from_file(xmlfile);
	int ret = oscap_source_validate_priv(source, doctype, version, reporter, arg);
	oscap_source_free(source);
	return ret;
}

int oscap_schematron_validate_document(const char *xmlfile, oscap_document_type_t doctype, const char *version, const char *outfile) {

	struct oscap_source *source = oscap_source_new_from_file(xmlfile);
	int ret = oscap_source_validate_schematron_priv(source, doctype, version, outfile);
	oscap_source_free(source);
	return ret;
}

int oscap_apply_xslt(const char *xmlfile, const char *xsltfile, const char *outfile, const char **params)
{
	struct oscap_source *source = oscap_source_new_from_file(xmlfile);
	int ret = oscap_source_apply_xslt_path(source, xsltfile, outfile, params, oscap_path_to_xslt());
	oscap_source_free(source);
	return ret;
}

int oscap_determine_document_type(const char *document, oscap_document_type_t *doc_type) {
	struct oscap_source *source = oscap_source_new_from_file(document);
	*doc_type = oscap_source_get_scap_type(source);
	oscap_source_free(source);
	return (*doc_type == OSCAP_DOCUMENT_UNKNOWN) ? -1 : 0;
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
