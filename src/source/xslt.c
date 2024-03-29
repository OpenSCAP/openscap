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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/parser.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libexslt/exslt.h>
#include <string.h>

#ifdef OS_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif

#include "common/_error.h"
#include "common/util.h"
#include "oscap.h"
#include "oscap_source.h"
#include "source/oscap_source_priv.h"
#include "source/xslt_priv.h"
#include "oscap_helpers.h"

#define XCCDF11_NS "http://checklists.nist.gov/xccdf/1.1"
#define XCCDF12_NS "http://checklists.nist.gov/xccdf/1.2"

/*
 * Goes through the tree (DFS) and changes namespace of all XCCDF 1.1 elements
 * to XCCDF 1.2 namespace URI. This ensures that the XCCDF works fine with
 * XSLTs provided in openscap.
 */
static int xccdf_ns_xslt_workaround(xmlDocPtr doc, xmlNodePtr node)
{
	if (node == NULL) {
		// nothing to do, this part of the document isn't XCCDF 1.1
		return 0;
	}

	if (node->ns != NULL && strcmp((const char*)node->ns->href, XCCDF11_NS) == 0) {
		xmlNsPtr xccdf12 = xmlNewNs(node,
				BAD_CAST XCCDF12_NS,
				BAD_CAST "cdf12");

		xmlSetNs(node, xccdf12);
	}

	xmlNodePtr child = node->children;

	for (; child != NULL; child = child->next) {
		if (child->type != XML_ELEMENT_NODE)
			continue;

		const int res = xccdf_ns_xslt_workaround(doc, child);
		if (res != 0)
			return res;
	}

	return 0;
}

static inline int save_stylesheet_result_to_file(xmlDoc *resulting_doc, xsltStylesheet *stylesheet, const char *outfile)
{
#ifdef OS_WINDOWS
	int fd = _fileno(stdout);
#else
	int fd = STDOUT_FILENO;
#endif
	if (outfile) {
		fd = oscap_open_writable(outfile);
	}
	if (fd == -1)
		return -1;

	int ret = xsltSaveResultToFd(fd, resulting_doc, stylesheet);
	if (ret < 0) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not save result document");
	}
#ifdef OS_WINDOWS
	if (fd != _fileno(stdout))
#else
	if (fd != STDOUT_FILENO)
#endif
		close(fd);
	return ret;
}

static xmlDoc *apply_xslt_path_internal(struct oscap_source *source, const char *xsltfile, const char **params, const char *path_to_xslt, xsltStylesheet **stylesheet)
{
	xmlDoc *doc = oscap_source_get_xmlDoc(source);
	if (doc == NULL || stylesheet == NULL || xsltfile == NULL) {
		return NULL;
	}

	size_t argc = 0;
	while(params[argc]) argc += 2;

	// Should we change all XCCDF namespaces (versioned) to one?
	// This is a workaround needed to make XSLTs work with multiple versions.
	// (currently 1.1 and 1.2)
	bool ns_workaround = false;

	/* is it an absolute path? */
	char *xsltpath;
	if (strstr(xsltfile, "/") == xsltfile) {
		xsltpath = oscap_strdup(xsltfile);
		if (access(xsltpath, R_OK)) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "XSLT file '%s' not found when trying to transform '%s'",
				xsltfile, oscap_source_readable_origin(source));
			free(xsltpath);
			return NULL;
		}
	}
	else {
		xsltpath = oscap_sprintf("%s%s%s", path_to_xslt, "/", xsltfile);
		if (access(xsltpath, R_OK)) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "XSLT file '%s' not found in path '%s' when trying to transform '%s'",
				xsltfile, path_to_xslt, oscap_source_readable_origin(source));
			free(xsltpath);
			return NULL;
		}

		if (strcmp(xsltfile, "xccdf-report.xsl") == 0 ||
				strcmp(xsltfile, "legacy-fix.xsl") == 0 ||
				strcmp(xsltfile, "xccdf-guide.xsl") == 0)
			ns_workaround = true;
	}

	*stylesheet = xsltParseStylesheetFile(BAD_CAST xsltpath);
	if (*stylesheet == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not parse XSLT file '%s'", xsltpath);
		free(xsltpath);
		return NULL;
	}

	if (ns_workaround) {
		if (xccdf_ns_xslt_workaround(doc, xmlDocGetRootElement(doc)) != 0) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Had problems employing XCCDF XSLT namespace workaround for XML document '%s'",
				oscap_source_readable_origin(source));
			free(xsltpath);
			xsltFreeStylesheet(*stylesheet);
			*stylesheet = NULL;
			return NULL;
		}
	}

	char **args = calloc(argc + 1, sizeof(char *));

	for (size_t i = 0; i < argc; i += 2) {
		args[i] = (char*) params[i];
		if (params[i+1]) args[i+1] = oscap_sprintf("'%s'", params[i+1]);
	}

	xmlDoc *transformed = xsltApplyStylesheet(*stylesheet, doc, (const char **) args);
	for (size_t i = 0; args[i]; i += 2) {
		free(args[i+1]);
	}
	free(args);
	if (transformed == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not apply XSLT %s to XML file: %s", xsltpath,
			oscap_source_readable_origin(source));
		free(xsltpath);
		xsltFreeStylesheet(*stylesheet);
		*stylesheet = NULL;
		return NULL;
	}
	free(xsltpath);
	return transformed;

}

int oscap_source_apply_xslt_path(struct oscap_source *source, const char *xsltfile, const char *outfile, const char **params, const char *path_to_xslt)
{
	xsltStylesheet *stylesheet = NULL;
	xmlDocPtr transformed = apply_xslt_path_internal(source, xsltfile, params, path_to_xslt, &stylesheet);
	if (transformed == NULL) {
		return -1;
	}
	int ret = save_stylesheet_result_to_file(transformed, stylesheet, outfile);
	xsltFreeStylesheet(stylesheet);
	xmlFreeDoc(transformed);
	return ret;
}

char *oscap_source_apply_xslt_path_mem(struct oscap_source *source, const char *xsltfile, const char **params, const char *path_to_xslt)
{
	xsltStylesheet *stylesheet = NULL;
	xmlDocPtr transformed = apply_xslt_path_internal(source, xsltfile, params, path_to_xslt, &stylesheet);
	if (transformed == NULL) {
		return NULL;
	}
	xmlChar *result = NULL;
	int len;
	if (xsltSaveResultToString(&result, &len, transformed, stylesheet) != 0) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Could not save transformend content to buffer, after applying XSLT %s",
				xsltfile);
		free(result);
		result = NULL;
	}
	return (char *)result;
}
