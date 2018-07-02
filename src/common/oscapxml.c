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
#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#include <windows.h>
#endif
#include <fcntl.h>

#include "public/oscap.h"
#include "_error.h"
#include "util.h"
#include "list.h"
#include "elements.h"
#include "debug_priv.h"
#include "oscap_source.h"
#include "oscapxml.h"
#include "source/schematron_priv.h"
#include "source/validate_priv.h"
#include "source/xslt_priv.h"

const char *const OSCAP_SCHEMA_PATH = OSCAP_DEFAULT_SCHEMA_PATH;
const char *const OSCAP_XSLT_PATH = OSCAP_DEFAULT_XSLT_PATH;
const char *const OSCAP_CPE_PATH = OSCAP_DEFAULT_CPE_PATH;

#ifdef _WIN32
static const char *_get_default_path(const char *defpath)
{
	/* On Windows, default paths are directory names of directories that are
	 * located in the same directory as oscap.exe. We will make a full path
	 * based on oscap.exe location.
	 */
	char oscap_path[PATH_MAX];
	GetModuleFileName(NULL, oscap_path, PATH_MAX);
	char *oscap_path_dirname = oscap_dirname(oscap_path);
	const char *path = oscap_sprintf("%s\\%s", oscap_path_dirname, defpath);
	free(oscap_path_dirname);
	return path;
}
#else
static const char *_get_default_path(const char *defpath)
{
	return defpath;
}
#endif

/*
 * If environment variable specified by 'pathvar' is defined, returns value of this environment variable.
 * If this environment variable is not defined, returns default path, provided by 'defpath' argument.
 */
static const char * oscap_path_to(const char *pathvar, const char *defpath) {
	const char *path = NULL;

	if (pathvar != NULL)
		path = getenv(pathvar);

	if (path == NULL || oscap_streq(path, "")) {
		path = _get_default_path(defpath);
	}

	return path;
}

const char * oscap_path_to_schemas() {
	return oscap_path_to("OSCAP_SCHEMA_PATH", OSCAP_SCHEMA_PATH);
}

const char *oscap_path_to_xslt(void)
{
	return oscap_path_to("OSCAP_XSLT_PATH", OSCAP_XSLT_PATH);
}

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

const char *oscap_get_version(void) { return OPENSCAP_VERSION; }

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
	case OSCAP_DOCUMENT_CVRF_FEED:
		return "CVRF Feed";
	default:
		return NULL;
	}
}
