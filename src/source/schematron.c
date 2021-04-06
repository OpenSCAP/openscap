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

#include <string.h>
#ifdef OS_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif

#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "common/_error.h"
#include "common/debug_priv.h"
#include "common/list.h"
#include "common/util.h"
#include "ds_sds_session.h"
#include "DS/ds_sds_session_priv.h"
#include "oscap.h"
#include "oscap_helpers.h"
#include "oscap_source.h"
#include "source/oscap_source_priv.h"
#include "source/schematron_priv.h"
#include "source/validate_priv.h"
#include "source/xslt_priv.h"

struct oscap_schema_table_entry OSCAP_SCHEMATRON_TABLE[] = {
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.3",          "oval/5.3/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.4",          "oval/5.4/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.5",          "oval/5.5/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.6",          "oval/5.6/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.7",          "oval/5.7/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.8",          "oval/5.8/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.9",          "oval/5.9/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.10",         "oval/5.10/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.10.1",       "oval/5.10.1/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.11",         "oval/5.11/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.11.1",       "oval/5.11.1/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.11.2",       "oval/5.11.2/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.11.3",       "oval/5.11.3/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.3",          "oval/5.3/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.4",          "oval/5.4/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.5",          "oval/5.5/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.6",          "oval/5.6/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.7",          "oval/5.7/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.8",          "oval/5.8/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.9",          "oval/5.9/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.10",         "oval/5.10/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.10.1",       "oval/5.10.1/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.11",         "oval/5.11/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.11.1",       "oval/5.11.1/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.11.2",       "oval/5.11.2/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.11.3",       "oval/5.11.3/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.3",          "oval/5.3/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.4",          "oval/5.4/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.5",          "oval/5.5/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.6",          "oval/5.6/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.7",          "oval/5.7/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.8",          "oval/5.8/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.9",          "oval/5.9/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.10",         "oval/5.10/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.10.1",       "oval/5.10.1/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.11",         "oval/5.11/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.11.1",       "oval/5.11.1/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.11.2",       "oval/5.11.2/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.11.3",       "oval/5.11.3/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.3",          "oval/5.3/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.4",          "oval/5.4/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.5",          "oval/5.5/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.6",          "oval/5.6/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.7",          "oval/5.7/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.8",          "oval/5.8/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.9",          "oval/5.9/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.10",         "oval/5.10/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.10.1",       "oval/5.10.1/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.11",         "oval/5.11/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.11.1",       "oval/5.11.1/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.11.2",       "oval/5.11.2/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.11.3",       "oval/5.11.3/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.8",          "oval/5.8/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.9",          "oval/5.9/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.10",         "oval/5.10/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.10.1",       "oval/5.10.1/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.11",         "oval/5.11/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.11.1",       "oval/5.11.1/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.11.2",       "oval/5.11.2/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.11.3",       "oval/5.11.3/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_XCCDF,                  "1.2",          "xccdf/1.2/xccdf_1.2-schematron.xsl"},
	{OSCAP_DOCUMENT_SDS,                    "1.3",          "sds/1.3/source-data-stream-1.3.xsl"}
};

static int _validate_sds_components(struct oscap_source *source, FILE *outfile_fd)
{
	int ret = 0;
	struct ds_sds_session *session = ds_sds_session_new_from_source(source);
	if (ds_sds_session_register_component_with_dependencies(session, "checklists", NULL, NULL) != 0) {
		return -1;
	}
	if (ds_sds_session_register_component_with_dependencies(session, "checks", NULL, NULL) != 0) {
		return -1;
	}
	struct oscap_htable *component_sources = ds_sds_session_get_component_sources(session);
	struct oscap_htable_iterator *it = oscap_htable_iterator_new(component_sources);
	while (oscap_htable_iterator_has_more(it)) {
		struct oscap_source *cs = oscap_htable_iterator_next_value(it);
		const char *type = oscap_document_type_to_string(oscap_source_get_scap_type(cs));
		const char *origin = oscap_source_readable_origin(cs);
		fprintf(outfile_fd, "Starting schematron validation of %s component '%s':\n", type, origin);
		int component_result = oscap_source_validate_schematron_priv(cs,
			oscap_source_get_scap_type(cs),
			oscap_source_get_schema_version(cs),
			outfile_fd);
		fprintf(outfile_fd, "Schematron validation of %s component '%s': %s\n\n", type, origin, component_result == 0 ? "PASS" : "FAIL");
		if (component_result != 0) {
			ret = component_result;
		}
	}
	oscap_htable_iterator_free(it);
	ds_sds_session_free(session);
	return ret;
}

static char *_xcf_resolve_in_catalog(xmlNodePtr resolver_node, const char *uri, xmlXPathContextPtr context)
{
	if (*uri == '#') {
		return oscap_strdup(uri);
	}
	if (resolver_node == NULL) {
		return NULL;
	}
	if (strcmp((char *)resolver_node->name, "uri") == 0) {
		char *name = (char *) xmlGetProp(resolver_node, BAD_CAST "name");
		if (name != NULL && strcmp(name, uri) == 0) {
			char *resolver_node_uri = (char *) xmlGetProp(resolver_node, BAD_CAST "uri");
			free(name);
			return resolver_node_uri;
		} else {
			free(name);
			return _xcf_resolve_in_catalog(resolver_node->next, uri, context);
		}
	} else if (strcmp((char *)resolver_node->name, "rewriteURI") == 0) {
		char *uri_start_string = (char *) xmlGetProp(resolver_node, BAD_CAST "uriStartString");
		if (oscap_str_startswith(uri, uri_start_string)) {
			char *rewrite_prefix = (char *) xmlGetProp(resolver_node, BAD_CAST "rewritePrefix");
			char *concat = oscap_sprintf("%s%s", rewrite_prefix, uri + strlen(uri_start_string) + 1);
			free(rewrite_prefix);
			free(uri_start_string);
			return concat;
		} else {
			free(uri_start_string);
			return _xcf_resolve_in_catalog(resolver_node->next, uri, context);
		}
	}
	return NULL;
}

static xmlNodePtr _xcf_get_component_ref(xmlNodePtr catalog, const char *uri, xmlXPathContextPtr context)
{
	char *component_ref_uri = _xcf_resolve_in_catalog(catalog->children->next, uri, context);
	if (component_ref_uri == NULL) {
		dD("Can't get component_ref URI using check-content-ref href='%s'", uri);
		return NULL;
	}
	char *cref = component_ref_uri + 1;
	xmlNodePtr component_ref_node = NULL;
	char *xpath = oscap_sprintf("ancestor::ds:data-stream//ds:component-ref[@id='%s']", cref);
	xmlXPathObjectPtr component_refs = xmlXPathNodeEval(catalog, BAD_CAST xpath, context);
	if (component_refs->nodesetval->nodeNr == 1) {
		component_ref_node = component_refs->nodesetval->nodeTab[0];
	}
	if (component_ref_node == NULL) {
		dD("component-ref '%s' not found", cref);
	}
	xmlXPathFreeObject(component_refs);
	free(xpath);
	free(component_ref_uri);

	return component_ref_node;
}

static xmlNodePtr _xcf_get_component(xmlNodePtr component_ref_node, xmlXPathContextPtr context)
{
	char *xlink_href = (char *) xmlGetNsProp(component_ref_node, BAD_CAST "href", BAD_CAST "http://www.w3.org/1999/xlink");
	char *xpath = oscap_sprintf("ancestor::ds:data-stream-collection//ds:component[@id='%s']", xlink_href + 1);
	free(xlink_href);
	xmlXPathObjectPtr components = xmlXPathNodeEval(component_ref_node, BAD_CAST xpath, context);
	xmlNodePtr component = NULL;
	if (components->nodesetval->nodeNr == 1) {
		component = components->nodesetval->nodeTab[0];
	}
	free(xpath);
	xmlXPathFreeObject(components);
	return component;
}

static xmlNodePtr _find_catalog(xmlNodePtr component_ref_node)
{
	/* find $m/catalog */
	xmlNodePtr catalog = NULL;
	xmlNodePtr cur = component_ref_node->children;
	while (cur != NULL) {
		if (!xmlStrcmp(cur->name, BAD_CAST "catalog")) {
			catalog = cur;
			break;
		}
		cur = cur->next;
	}
	return catalog;
}

static bool _xcf_is_external_ref(xmlNodePtr catalog, const char *uri, xmlXPathContextPtr context)
{
	xmlNodePtr comp_ref = _xcf_get_component_ref(catalog, uri, context);
	bool res = false;
	if (comp_ref != NULL) {
		char *xlink_href = (char *) xmlGetNsProp(comp_ref, BAD_CAST "href", BAD_CAST "http://www.w3.org/1999/xlink");
		if (!oscap_str_startswith(xlink_href, "#"))
			res = true;
		free(xlink_href);
	}
	return res;
}

static bool _req_src_236_2_sub6(xmlNodePtr component, xmlXPathContextPtr context, const char *name, const char *rule_id)
{
	bool exists_ocil = false;
	char *ocil_xpath = oscap_sprintf(".//ocil:questionnaire[@id='%s']", name);
	xmlXPathObjectPtr ocil_questionnaires = xmlXPathNodeEval(component, BAD_CAST ocil_xpath, context);
	for (int m = 0; m < ocil_questionnaires->nodesetval->nodeNr; m++) {
		xmlNodePtr ocil_questionnaire = ocil_questionnaires->nodesetval->nodeTab[m];
		char *ocil_questionnaire_id = (char *) xmlGetProp(ocil_questionnaire, BAD_CAST "id");
		dD("Found OCIL questionnaire id='%s' for rule id='%s'", ocil_questionnaire_id, rule_id);
		free(ocil_questionnaire_id);
		exists_ocil = true;
	}
	free(ocil_xpath);
	xmlXPathFreeObject(ocil_questionnaires);
	return exists_ocil;
}

static bool _req_src_236_2_sub5(xmlNodePtr component, xmlXPathContextPtr context, const char *name, const char *rule_id)
{
	/* exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//oval-def:definition[@id eq $o/@name and matches(@class, '^(compliance|patch)$')]) */
	bool exists_oval = false;
	char *oval_xpath = oscap_sprintf(".//oval-def:definition[@id='%s' and (@class='compliance' or @class='patch')]", name);
	xmlXPathObjectPtr oval_definitions = xmlXPathNodeEval(component, BAD_CAST oval_xpath, context);
	for (int m = 0; m < oval_definitions->nodesetval->nodeNr; m++) {
		xmlNodePtr oval_definition = oval_definitions->nodesetval->nodeTab[m];
		char *oval_def_id = (char *) xmlGetProp(oval_definition, BAD_CAST "id");
		dD("Found OVAL definition id='%s' for rule id='%s'", oval_def_id, rule_id);
		free(oval_def_id);
		exists_oval = true;
	}
	free(oval_xpath);
	xmlXPathFreeObject(oval_definitions);
	return exists_oval;
}

static bool _req_src_236_2_sub4(xmlNodePtr check_content_ref_node, xmlNodePtr catalog, xmlXPathContextPtr context, const char *rule_id)
{
	char *href = (char *) xmlGetProp(check_content_ref_node, BAD_CAST "href");
	/* xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href) */
	xmlNodePtr component_ref =  _xcf_get_component_ref(catalog, href, context);
	free(href);
	if (component_ref == NULL) {
		dD("component_ref not found");
		return false;
	}
	xmlNodePtr component = _xcf_get_component(component_ref, context);
	if (component == NULL) {
		dD("component not found\n");
		return false;
	}

	char *name = (char *) xmlGetProp(check_content_ref_node, BAD_CAST "name");

	/* exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//oval-def:definition[@id eq $o/@name and matches(@class, '^(compliance|patch)$')]) or exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//ocil:questionnaire[@id eq $o/@name])) */
	bool res = (_req_src_236_2_sub5(component, context, name, rule_id) || _req_src_236_2_sub6(component, context, name, rule_id));

	free(name);
	return res;
}

static bool _req_src_236_2_sub3(xmlNodePtr rule_node, xmlNodePtr catalog, xmlXPathContextPtr context)
{
	bool res = true;
	char *rule_id = (char *) xmlGetProp(rule_node, BAD_CAST "id");

	/* note: $n is an xccdf:Rule */
	/* if(exists($n/xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system eq 'http://scap.nist.gov/schema/ocil/2']/xccdf:check-content-ref[exists(@name) and not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)])) then ... else true() */
	xmlXPathObjectPtr check_content_refs = xmlXPathNodeEval(rule_node, BAD_CAST "xccdf:check[@system='http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system='http://scap.nist.gov/schema/ocil/2']/xccdf:check-content-ref[@name]", context);
	/* TODO: external refs check */
	if (check_content_refs == NULL) {
		dD("Rule '%s' has no suitable check-content-refs", rule_id);
		free(rule_id);
		return true;
	}

	/* ... then some $o in $n/xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system eq 'http://scap.nist.gov/schema/ocil/2']/xccdf:check-content-ref[exists(@name) and not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)] satisfies ... */
	bool found = false;
	for (int l = 0; l < check_content_refs->nodesetval->nodeNr; l++) {
		xmlNodePtr check_content_ref_node = check_content_refs->nodesetval->nodeTab[l];
		char *href = (char *) xmlGetProp(check_content_ref_node, BAD_CAST "href");
		if (_xcf_is_external_ref(catalog, href, context)) {
			free(href);
			continue;
		}
		free(href);
		if (_req_src_236_2_sub4(check_content_ref_node, catalog, context, rule_id)) {
			found = true;
			break;
		}
	}
	if (!found) {
		res = false;
	}
	free(rule_id);
	xmlXPathFreeObject(check_content_refs);
	return res;
}

static bool _req_src_236_2_sub2(xmlNodePtr component_ref_node, xmlNodePtr catalog, xmlXPathContextPtr context)
{
	bool res = true;
	/* every $n in xcf:get-component($m)//xccdf:Rule satisfies ... */
	xmlNodePtr component_node = _xcf_get_component(component_ref_node, context);
	if (component_node == NULL) {
		char *xlink_href = (char *) xmlGetNsProp(component_ref_node, BAD_CAST "href", BAD_CAST "http://www.w3.org/1999/xlink");
		dD("Can't find component using component-ref '%s'", xlink_href);
		free(xlink_href);
		return false;
	}
	xmlXPathObjectPtr rules = xmlXPathNodeEval(component_node, BAD_CAST ".//xccdf:Rule", context);
	for (int i = 0; i < rules->nodesetval->nodeNr; i++) {
		xmlNodePtr rule_node = rules->nodesetval->nodeTab[i];
		char *rule_id = (char *) xmlGetProp(rule_node, BAD_CAST "id");
		dD("Checking xccdf:Rule id='%s'", rule_id);
		free(rule_id);
		if (!_req_src_236_2_sub3(rule_node, catalog, context)) {
			res = false;
			break;
		}
	}
	xmlXPathFreeObject(rules);
	return res;
}

static bool _req_src_236_2_sub1(xmlNodePtr data_stream_node, xmlXPathContextPtr context)
{
	int res = true;
	/* every $m in ds:checklists/ds:component-ref satisfies ... */
	xmlXPathObjectPtr component_refs = xmlXPathNodeEval(data_stream_node, BAD_CAST "ds:checklists/ds:component-ref", context);
	for (int i = 0; i < component_refs->nodesetval->nodeNr; i++) {
		xmlNodePtr component_ref_node = component_refs->nodesetval->nodeTab[i];
		char *component_ref_id = (char *) xmlGetProp(component_ref_node, BAD_CAST "id");
		dD("Checking ds:component-ref id='%s'", component_ref_id);
		free(component_ref_id);

		/* find $m/catalog */
		xmlNodePtr catalog = _find_catalog(component_ref_node);
		if (catalog == NULL) {
			res = false;
			break;
		}

		if (!_req_src_236_2_sub2(component_ref_node, catalog, context)) {
			res = false;
			break;
		}
	}
	xmlXPathFreeObject(component_refs);
	return res;
}

static bool _req_src_236_2(xmlXPathContextPtr context, FILE *outfile_fd)
{
	/*
	 * This function implements the check for SCAPVAL requirement SRC-236-2
	 * which is implemented in XML Schematron file 'source-data-stream-1.3.sch'
	 * assert scap-use-case-conf-verification-rule-ref-oval.
	 *
	 * <sch:assert id="scap-use-case-conf-verification-rule-ref-oval" test="if( @use-case eq 'CONFIGURATION' ) then every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:Rule satisfies (if(exists($n/ xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system eq 'http://scap.nist.gov/schema/ocil/2']/xccdf:check-content-ref[exists(@name) and not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)])) then some $o in $n/xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5' or @system eq 'http://scap.nist.gov/schema/ocil/2']/xccdf:check-content-ref[exists(@name) and not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)] satisfies (exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href))//oval-def:definition[@id eq $o/@name and matches(@class,'^(compliance|patch)$')]) or exists(xcf:get-component(xcf:  get-component-ref($m/cat:catalog, $o/@href))//ocil:questionnaire[@id eq $o/@name])) else true()) else true()">SRC-236-2|scap:data-stream <sch:value-of select="@id"/></sch:assert>
	 */

	bool res = true;
	/* The parent rule element in the schematron matches all scap:data-stream elements */
	xmlXPathObjectPtr data_streams = xmlXPathEval(BAD_CAST "//scap:data-stream", context);
	for (int i = 0; i < data_streams->nodesetval->nodeNr; i++) {
		xmlNodePtr data_stream_node = data_streams->nodesetval->nodeTab[i];
		/* The assert applies only to configuration use cases */
		/* if(@use-case eq 'CONFIGURATION') then ...  else true() */
		char *use_case = (char *) xmlGetProp(data_stream_node, BAD_CAST "use-case");
		if (use_case == NULL || strcmp(use_case, "CONFIGURATION") != 0) {
			continue;
		}
		free(use_case);
		if (!_req_src_236_2_sub1(data_stream_node, context)) {
			char *data_stream_id = (char *) xmlGetProp(data_stream_node, BAD_CAST "id");
			fprintf(outfile_fd, "Error: SRC-236-2|scap:data-stream %s\n", data_stream_id);
			free(data_stream_id);
			res = false;
			break;
		}
	}
	xmlXPathFreeObject(data_streams);
	return res;
}


static bool _req_src_346_1_sub5(xmlNodePtr component, xmlXPathContextPtr context, const char *name)
{
	/* .//oval-def:definition[@id eq $n/@name]) */
	bool exists_oval = false;
	char *oval_xpath = oscap_sprintf(".//oval-def:definition[@id='%s']", name);
	xmlXPathObjectPtr oval_definitions = xmlXPathNodeEval(component, BAD_CAST oval_xpath, context);
	for (int m = 0; m < oval_definitions->nodesetval->nodeNr; m++) {
		xmlNodePtr oval_definition = oval_definitions->nodesetval->nodeTab[m];
		char *oval_def_id = (char *) xmlGetProp(oval_definition, BAD_CAST "id");
		dD("Found OVAL definition id='%s'", oval_def_id);
		free(oval_def_id);
		exists_oval = true;
	}
	free(oval_xpath);
	xmlXPathFreeObject(oval_definitions);
	return exists_oval;
}

static bool _req_src_346_1_sub4(xmlNodePtr check_content_ref_node, xmlNodePtr catalog, xmlXPathContextPtr context)
{

	char *href = (char *) xmlGetProp(check_content_ref_node, BAD_CAST "href");
	/* xcf:get-component(xcf:get-component-ref($m/cat:catalog, $o/@href) */
	xmlNodePtr component_ref =  _xcf_get_component_ref(catalog, href, context);
	free(href);
	if (component_ref == NULL) {
		dD("component_ref not found");
		return false;
	}
	xmlNodePtr component = _xcf_get_component(component_ref, context);
	if (component == NULL) {
		dD("component not found\n");
		return false;
	}

	/* not(exists($n/@name)) or exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//oval-def:definition[@id eq $n/@name]) */
	char *name = (char *) xmlGetProp(check_content_ref_node, BAD_CAST "name");
	bool res = true;
	if(name != NULL) {
		res = _req_src_346_1_sub5(component, context, name);
	}

	free(name);
	return res;
}

static bool _req_src_346_1_sub3(xmlNodePtr rule_node, xmlNodePtr catalog, xmlXPathContextPtr context)
{
	bool res = true;

	/* xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5']//xccdf:check-content-ref[not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)] */
	xmlXPathObjectPtr check_content_refs = xmlXPathNodeEval(rule_node, BAD_CAST ".//xccdf:Rule/xccdf:check[@system='http://oval.mitre.org/XMLSchema/oval-definitions-5']/xccdf:check-content-ref", context);

	if (check_content_refs == NULL) {
		dD("There are no check-content-refs elements to be checked.");
		return true;
	}

	/* ... (not(exists($n/@name)) or exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//oval-def:definition[@id eq $n/@name])) */
	bool found = true;
	for (int l = 0; l < check_content_refs->nodesetval->nodeNr; l++) {
		xmlNodePtr check_content_ref_node = check_content_refs->nodesetval->nodeTab[l];
		char *href = (char *) xmlGetProp(check_content_ref_node, BAD_CAST "href");
		if (_xcf_is_external_ref(catalog, href, context)) {
			free(href);
			continue;
		}
		if (!_req_src_346_1_sub4(check_content_ref_node, catalog, context)) {
			found = false;
			break;
		}
	}
	if (!found) {
		res = false;
	}
	xmlXPathFreeObject(check_content_refs);
	return res;
}

static bool _req_src_346_1_sub2(xmlNodePtr component_ref_node, xmlNodePtr catalog, xmlXPathContextPtr context)
{
	/* every $n in xcf:get-component($m)//xccdf:check satisfies ... */
	xmlNodePtr component_node = _xcf_get_component(component_ref_node, context);
	if (component_node == NULL) {
		char *xlink_href = (char *) xmlGetNsProp(component_ref_node, BAD_CAST "href", BAD_CAST "http://www.w3.org/1999/xlink");
		dD("Can't find component using component-ref '%s'", xlink_href);
		free(xlink_href);
		return false;
	}
	if (!_req_src_346_1_sub3(component_node, catalog, context)) {
		return false;
	}
	return true;
}

static bool _req_src_346_1_sub1(xmlNodePtr data_stream_node, xmlXPathContextPtr context)
{
	int res = true;
	/* every $m in ds:checklists/ds:component-ref satisfies ... */
	xmlXPathObjectPtr component_refs = xmlXPathNodeEval(data_stream_node, BAD_CAST "ds:checklists/ds:component-ref", context);
	for (int i = 0; i < component_refs->nodesetval->nodeNr; i++) {
		xmlNodePtr component_ref_node = component_refs->nodesetval->nodeTab[i];
		char *component_ref_id = (char *) xmlGetProp(component_ref_node, BAD_CAST "id");
		dD("Checking ds:component-ref id='%s'", component_ref_id);
		free(component_ref_id);

		/* find $m/catalog */
		xmlNodePtr catalog = _find_catalog(component_ref_node);
		if (catalog == NULL) {
			res = false;
			break;
		}

		if (!_req_src_346_1_sub2(component_ref_node, catalog, context)) {
			res = false;
			break;
		}
	}
	xmlXPathFreeObject(component_refs);
	return res;
}

static bool _req_src_346_1(xmlXPathContextPtr context, FILE *outfile_fd)
{
	/*
	 * This function implements the check for SCAPVAL requirement SRC-346-1
	 * which is implemented in XML Schematron file 'source-data-stream-1.3.sch'
	 * assert scap-general-scap-content-xccdf-check-content-ref-name-not-req.
	 *
	 * <sch:assert id="scap-general-scap-content-xccdf-check-content-ref-name-not-req" test="every $m in ds:checklists/ds:component-ref satisfies every $n in xcf:get-component($m)//xccdf:check[@system eq 'http://oval.mitre.org/XMLSchema/oval-definitions-5']//xccdf:check-content-ref[not(xcf:is-external-ref($m/cat:catalog, @href) cast as xsd:boolean)] satisfies (not(exists($n/@name)) or exists(xcf:get-component(xcf:get-component-ref($m/cat:catalog, $n/@href))//oval-def:definition[@id eq $n/@name]))">SRC-346-1|scap:data-stream <sch:value-of select="@id"/></sch:assert>
	 */

	bool res = true;
	/* The parent rule element in the schematron matches all scap:data-stream elements */
	xmlXPathObjectPtr data_streams = xmlXPathEval(BAD_CAST "//scap:data-stream", context);
	for (int i = 0; i < data_streams->nodesetval->nodeNr; i++) {
		xmlNodePtr data_stream_node = data_streams->nodesetval->nodeTab[i];
		if (!_req_src_346_1_sub1(data_stream_node, context)) {
			char *data_stream_id = (char *) xmlGetProp(data_stream_node, BAD_CAST "id");
			fprintf(outfile_fd, "Error: SRC-346-1|scap:data-stream %s\n", data_stream_id);
			free(data_stream_id);
			res = false;
			break;
		}
	}
	xmlXPathFreeObject(data_streams);
	return res;
}

static int _additional_schematron_checks(struct oscap_source *source, FILE *outfile_fd)
{
	xmlDocPtr doc = oscap_source_get_xmlDoc(source);
	xmlXPathContextPtr context = xmlXPathNewContext(doc);
	if (context == NULL) {
		oscap_seterr(OSCAP_EFAMILY_XML, "Error in xmlXPathNewContext");
		return -1;
	}
	xmlXPathRegisterNs(context, BAD_CAST "xccdf",  BAD_CAST "http://checklists.nist.gov/xccdf/1.2");
	xmlXPathRegisterNs(context, BAD_CAST "scap", BAD_CAST "http://scap.nist.gov/schema/scap/source/1.2");
	xmlXPathRegisterNs(context, BAD_CAST "ds", BAD_CAST "http://scap.nist.gov/schema/scap/source/1.2");
	xmlXPathRegisterNs(context, BAD_CAST "xlink", BAD_CAST "http://www.w3.org/1999/xlink");
	xmlXPathRegisterNs(context, BAD_CAST "oval-def", BAD_CAST "http://oval.mitre.org/XMLSchema/oval-definitions-5");
	xmlXPathRegisterNs(context, BAD_CAST "ocil", BAD_CAST "http://scap.nist.gov/schema/ocil/2.0");

	int res = 0;
	/* Assert ID: scap-use-case-conf-verification-benchmark-one-rule-ref-oval-ocil */
	if (!_req_src_236_2(context, outfile_fd))
		res = 1;
	/* Assert ID: scap-general-scap-content-xccdf-check-content-ref-name-not-req */
	if (!_req_src_346_1(context, outfile_fd))
		res = 1;

	xmlXPathFreeContext(context);
	return res;
}

static int _sds_schematron_validation(struct oscap_source *source, const char *schema_path, FILE *outfile_fd)
{
	int validity = 0;
	const char *params[] = { NULL };
	fprintf(outfile_fd, "Starting global schematron validation using the source data stream schematron\n");

	if (_additional_schematron_checks(source, outfile_fd) != 0) {
		validity = 1;
	}
	char *xslt_output = oscap_source_apply_xslt_path_mem(source, schema_path, params, oscap_path_to_schemas());
	if (xslt_output == NULL) {
		validity = 1;
	}
	char *err_substr = strstr(xslt_output, "Error:");
	if (err_substr != NULL) {
		/* "Error:" found in schematron output */
		validity = 1;
	}
	fputs(xslt_output, outfile_fd);
	free(xslt_output);
	fprintf(outfile_fd, "Global schematron validation using the source data stream schematron: %s\n\n", validity == 0 ? "PASS" : "FAIL");
	return validity;
}

int oscap_source_validate_schematron_priv(struct oscap_source *source, oscap_document_type_t scap_type, const char *version, FILE *outfile_fd)
{
	const char *origin = oscap_source_readable_origin(source);
	if (version == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not determine document version for %s",
			origin);
		return -1;
	}

	/* Skip document types that don't have a schematron available */
	if (scap_type == OSCAP_DOCUMENT_CPE_LANGUAGE ||
			scap_type == OSCAP_DOCUMENT_CPE_DICTIONARY ||
			scap_type == OSCAP_DOCUMENT_CVE_FEED ||
			scap_type == OSCAP_DOCUMENT_SCE_RESULT ||
			scap_type == OSCAP_DOCUMENT_OCIL ||
			scap_type == OSCAP_DOCUMENT_CVRF_FEED) {
		fprintf(outfile_fd, "Skipped\n");
		return 0;
	}

	/* find a right schematron file */
	const char *schematron_path = NULL;
	for (struct oscap_schema_table_entry *entry = OSCAP_SCHEMATRON_TABLE; entry->doc_type != 0; ++entry) {
		if (entry->doc_type == scap_type && !strcmp(entry->schema_version, version)) {
			schematron_path = entry->schema_path;
			break;
		}
	}
	if (schematron_path == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Schematron rules not found when trying to validate '%s'", origin);
		return -1;
	}

	/* validate */
	int ret = 0;
	if (scap_type == OSCAP_DOCUMENT_SDS) {
		int component_validity = _validate_sds_components(source, outfile_fd);
		int global_validity = _sds_schematron_validation(source, schematron_path, outfile_fd);
		if (component_validity != 0 || global_validity != 0) {
			ret = 1;
		}
		fprintf(outfile_fd, "Complete result of schematron validation of '%s': %s\n", origin, ret == 0 ? "PASS" : "FAIL");
	} else {
		const char *params[] = { NULL };
		char *xslt_output = oscap_source_apply_xslt_path_mem(source, schematron_path, params, oscap_path_to_schemas());
		if (xslt_output != NULL) {
			ret = 1;
			fputs(xslt_output, outfile_fd);
		}
		free(xslt_output);
	}
	return ret;
}
