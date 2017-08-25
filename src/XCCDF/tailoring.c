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
 *      Martin Preisler <mpreisle@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "item.h"
#include "helpers.h"
#include "xccdf_impl.h"
#include "common/alloc.h"
#include "common/_error.h"
#include "common/debug_priv.h"
#include "common/elements.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"

struct xccdf_tailoring *xccdf_tailoring_new(void)
{
	struct xccdf_tailoring *tailoring = calloc(1, sizeof(struct xccdf_tailoring));

	tailoring->id = NULL;

	tailoring->benchmark_ref = NULL;
	tailoring->benchmark_ref_version = NULL;

	tailoring->statuses = oscap_list_new();
	tailoring->dc_statuses = oscap_list_new();

	tailoring->version = NULL;
	tailoring->version_update = NULL;
	tailoring->version_time = NULL;

	tailoring->metadata = oscap_list_new();
	tailoring->profiles = oscap_list_new();

	return tailoring;
}

void xccdf_tailoring_free(struct xccdf_tailoring *tailoring)
{
	if (tailoring) {
		free(tailoring->id);

		free(tailoring->benchmark_ref);
		free(tailoring->benchmark_ref_version);

		oscap_list_free(tailoring->statuses, (oscap_destruct_func) xccdf_status_free);
		oscap_list_free(tailoring->dc_statuses, (oscap_destruct_func) oscap_reference_free);

		free(tailoring->version);
		free(tailoring->version_update);
		free(tailoring->version_time);

		oscap_list_free(tailoring->metadata, (oscap_destruct_func) free);
		oscap_list_free(tailoring->profiles, (oscap_destruct_func) xccdf_profile_free);

		free(tailoring);
	}
}

bool xccdf_tailoring_add_profile(struct xccdf_tailoring *tailoring, struct xccdf_profile *profile)
{
	xccdf_profile_set_tailoring(profile, true);
	return oscap_list_add(tailoring->profiles, XITEM(profile));
}

static bool _list_ptreq_cmp(void *a, void *b)
{
	return a == b;
}

bool xccdf_tailoring_remove_profile(struct xccdf_tailoring *tailoring, struct xccdf_profile *profile)
{
	assert(xccdf_profile_get_tailoring(profile));

	// We have to make sure there is no other profile in tailoring that inherits
	// the profile we are about to remove.

	const char *profile_id = xccdf_profile_get_id(profile);

	struct xccdf_profile_iterator* it = xccdf_tailoring_get_profiles(tailoring);
	while (xccdf_profile_iterator_has_more(it)) {
		struct xccdf_profile* prof = xccdf_profile_iterator_next(it);

		if (prof == profile)
			continue;

		const char *extends = xccdf_profile_get_extends(prof);
		if (oscap_strcmp(profile_id, extends) == 0) {
			oscap_seterr(OSCAP_EFAMILY_XML,
				"Can't remove given profile '%s' from tailoring. Other profiles are inheriting from it!",
				profile_id);

			xccdf_profile_iterator_free(it);
			return false;
		}
	}
	xccdf_profile_iterator_free(it);

	return oscap_list_remove(tailoring->profiles, XITEM(profile), (oscap_cmp_func)_list_ptreq_cmp, NULL);
}

struct xccdf_tailoring *xccdf_tailoring_parse(xmlTextReaderPtr reader, struct xccdf_item *benchmark)
{
	XCCDF_ASSERT_ELEMENT(reader, XCCDFE_TAILORING);

	struct xccdf_tailoring *tailoring = xccdf_tailoring_new();

	const char *id = xccdf_attribute_get(reader, XCCDFA_ID);
	xccdf_tailoring_set_id(tailoring, id);

	int depth = oscap_element_depth(reader) + 1;

	// Read to the inside of Tailoring.
	xmlTextReaderRead(reader);

	while (oscap_to_start_element(reader, depth)) {
		switch (xccdf_element_get(reader)) {
		case XCCDFE_BENCHMARK_REF: {
			free(tailoring->benchmark_ref);
			tailoring->benchmark_ref = 0;

			free(tailoring->benchmark_ref_version);
			tailoring->benchmark_ref_version = 0;

			const char *ref = xccdf_attribute_get(reader, XCCDFA_HREF);
			if (ref)
				tailoring->benchmark_ref = oscap_strdup(ref);

			const char *ref_version = xccdf_attribute_get(reader, XCCDFA_VERSION);
			if (ref_version)
				tailoring->benchmark_ref_version = oscap_strdup(ref_version);

			break;
		}
		case XCCDFE_STATUS: {
			const char *date = xccdf_attribute_get(reader, XCCDFA_DATE);
			char *str = oscap_element_string_copy(reader);
			struct xccdf_status *status = xccdf_status_new_fill(str, date);
			free(str);
			oscap_list_add(tailoring->statuses, status);
			break;
		}
		case XCCDFE_DC_STATUS:
			oscap_list_add(tailoring->dc_statuses, oscap_reference_new_parse(reader));
			break;
		case XCCDFE_VERSION: {
			xmlNode *ver = xmlTextReaderExpand(reader);
			/* optional attributes */
			tailoring->version_time = (char*) xmlGetProp(ver, BAD_CAST "time");
			tailoring->version_update = (char*) xmlGetProp(ver, BAD_CAST "update");
			/* content */
			tailoring->version = (char *) xmlNodeGetContent(ver);
			if (oscap_streq(tailoring->version, "")) {
				free(tailoring->version);
				tailoring->version = NULL;
			}
			break;
		}
		case XCCDFE_METADATA: {
			char* xml = oscap_get_xml(reader);
			oscap_list_add(tailoring->metadata, oscap_strdup(xml));
			free(xml);
			break;
		}
		case XCCDFE_PROFILE: {
			if (benchmark != NULL) {
				dI("Parsing Tailoring Profiles without reference to Benchmark");
			}
			struct xccdf_item *item = xccdf_profile_parse(reader, benchmark);
			if (!xccdf_tailoring_add_profile(tailoring, XPROFILE(item))) {
				dW("Failed to add profile to tailoring while parsing!");
			}
			break;
		}
		default:
			dW("Encountered an unknown element '%s' while parsing XCCDF Tailoring element.",
				xmlTextReaderConstLocalName(reader));
		}
		xmlTextReaderRead(reader);
	}

	return tailoring;
}

struct xccdf_tailoring *xccdf_tailoring_import_source(struct oscap_source *source, struct xccdf_benchmark *benchmark)
{
	xmlTextReaderPtr reader = oscap_source_get_xmlTextReader(source);
	if (!reader) {
		return NULL;
	}

	while (xmlTextReaderRead(reader) == 1 && xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) ;
	struct xccdf_tailoring *tailoring = xccdf_tailoring_parse(reader, XITEM(benchmark));
	xmlFreeTextReader(reader);
	if (!tailoring) { // parsing fatal error
		oscap_seterr(OSCAP_EFAMILY_XML, "Failed to parse tailoring from '%s'.", oscap_source_readable_origin(source));
	}
	return tailoring;
}

struct xccdf_tailoring *xccdf_tailoring_import(const char *file, struct xccdf_benchmark *benchmark)
{
	struct oscap_source *source = oscap_source_new_from_file(file);

	struct xccdf_tailoring *tailoring = xccdf_tailoring_import_source(source, benchmark);
	oscap_source_free(source);
	return tailoring;
}

xmlNodePtr xccdf_tailoring_to_dom(struct xccdf_tailoring *tailoring, xmlDocPtr doc, xmlNodePtr parent, const struct xccdf_version_info *version_info)
{
	xmlNs *ns_tailoring = NULL;

	xmlNode *tailoring_node = xmlNewNode(NULL, BAD_CAST "Tailoring");

	const char *xccdf_version = xccdf_version_info_get_version(version_info);
#ifdef __USE_GNU
	if (strverscmp(xccdf_version, "1.1") >= 0 && strverscmp(xccdf_version, "1.2") < 0) {
#else
	if (strcmp(xccdf_version, "1.1") >= 0 && strcmp(xccdf_version, "1.2") < 0) {
#endif
		// XCCDF 1.1 does not support Tailoring!
		// However we will allow Tailoring export if it is done to an external
		// file. The namespace will be our custom xccdf-1.1-tailoring extension
		// namespace.

		if (parent != NULL) {
			oscap_seterr(OSCAP_EFAMILY_XML, "XCCDF 1.1 does not support embedded Tailoring elements!");
			xmlFreeNode(tailoring_node);
			return NULL;
		}

		ns_tailoring = xmlNewNs(tailoring_node,
			BAD_CAST "http://open-scap.org/page/Xccdf-1.1-tailoring",
			BAD_CAST "cdf-11-tailoring"
		);
	}
#ifdef __USE_GNU
	else if (strverscmp(xccdf_version, "1.1") < 0) {
#else
	else if (strcmp(xccdf_version, "1.1") < 0) {
#endif
		oscap_seterr(OSCAP_EFAMILY_XML, "XCCDF Tailoring isn't supported in XCCDF version '%s',"
			"nor does openscap have a custom extension for this scenario. "
			"XCCDF Tailoring requires XCCDF 1.1 and higher, 1.2 is recommended.");

		xmlFreeNode(tailoring_node);
		return NULL;
	}

	xmlNs *ns_xccdf = parent != NULL ? lookup_xccdf_ns(doc, parent, version_info) : NULL;
	if (!ns_xccdf) {
		// In cases where tailoring ends up being the root node we have to create
		// a namespace at the node itself.
		ns_xccdf = xmlNewNs(tailoring_node,
			BAD_CAST xccdf_version_info_get_namespace_uri(version_info),
			BAD_CAST "xccdf");
	}

	if (!ns_tailoring)
		ns_tailoring = ns_xccdf;

	// We intentionally set the wrong namespace here since children of tailoring
	// will reuse it and we want them to have the xccdf namespace, the namespace
	// is set to the proper namespace before returning the tailoring.
	xmlSetNs(tailoring_node, ns_xccdf);

	if (parent)
		xmlAddChild(parent, tailoring_node);
	else
		xmlDocSetRootElement(doc, tailoring_node);

	if (tailoring->id) {
		xmlNewProp(tailoring_node, BAD_CAST "id", BAD_CAST tailoring->id);
	}

	if (tailoring->benchmark_ref || tailoring->benchmark_ref_version) {
		xmlNodePtr benchmark_ref_node = xmlNewChild(tailoring_node, ns_tailoring, BAD_CAST "benchmark", NULL);

		if (tailoring->benchmark_ref)
			xmlNewProp(benchmark_ref_node, BAD_CAST "href", BAD_CAST tailoring->benchmark_ref);

		if (tailoring->benchmark_ref_version)
			xmlNewProp(benchmark_ref_node, BAD_CAST "version", BAD_CAST tailoring->benchmark_ref_version);
	}

	struct xccdf_status_iterator *statuses = xccdf_tailoring_get_statuses(tailoring);
	while (xccdf_status_iterator_has_more(statuses)) {
		struct xccdf_status *status = xccdf_status_iterator_next(statuses);
		xccdf_status_to_dom(status, doc, tailoring_node, version_info);
	}
	xccdf_status_iterator_free(statuses);

	struct oscap_reference_iterator *dc_statuses = xccdf_tailoring_get_dc_statuses(tailoring);
	while (oscap_reference_iterator_has_more(dc_statuses)) {
		struct oscap_reference *ref = oscap_reference_iterator_next(dc_statuses);
		oscap_reference_to_dom(ref, tailoring_node, doc, "dc-status");
	}
	oscap_reference_iterator_free(dc_statuses);

	/* version and attributes */
	const char *version = xccdf_tailoring_get_version(tailoring);
	if (version) {
		xmlNode* version_node = xmlNewTextChild(tailoring_node, ns_tailoring, BAD_CAST "version", BAD_CAST version);

		const char *version_update = xccdf_tailoring_get_version_update(tailoring);
		if (version_update)
			xmlNewProp(version_node, BAD_CAST "update", BAD_CAST version_update);

		const char *version_time = xccdf_tailoring_get_version_time(tailoring);
		if (version_time)
			xmlNewProp(version_node, BAD_CAST "time", BAD_CAST version_time);
	}

	struct oscap_string_iterator* metadata = xccdf_tailoring_get_metadata(tailoring);
	while (oscap_string_iterator_has_more(metadata))
	{
		const char* meta = oscap_string_iterator_next(metadata);
		xmlNode *m = oscap_xmlstr_to_dom(tailoring_node, "metadata", meta);
		xmlSetNs(m, ns_xccdf);
	}
	oscap_string_iterator_free(metadata);

	struct xccdf_profile_iterator *profiles = xccdf_tailoring_get_profiles(tailoring);
	while (xccdf_profile_iterator_has_more(profiles)) {
		struct xccdf_profile *profile = xccdf_profile_iterator_next(profiles);
		xccdf_item_to_dom(XITEM(profile), doc, tailoring_node, version_info);
	}
	xccdf_profile_iterator_free(profiles);

	xmlSetNs(tailoring_node, ns_tailoring);

	return tailoring_node;
}

int xccdf_tailoring_export(struct xccdf_tailoring *tailoring, const char *file, const struct xccdf_version_info *version_info)
{
	__attribute__nonnull__(file);

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	xccdf_tailoring_to_dom(tailoring, doc, NULL, version_info);

	return oscap_xml_save_filename_free(file, doc);
}

const char *xccdf_tailoring_get_id(const struct xccdf_tailoring *tailoring)
{
	return tailoring->id;
}

const char *xccdf_tailoring_get_version(const struct xccdf_tailoring *tailoring)
{
	return tailoring->version;
}

const char *xccdf_tailoring_get_version_update(const struct xccdf_tailoring *tailoring)
{
	return tailoring->version_update;
}

const char *xccdf_tailoring_get_version_time(const struct xccdf_tailoring *tailoring)
{
	return tailoring->version_time;
}

const char *xccdf_tailoring_get_benchmark_ref(const struct xccdf_tailoring *tailoring)
{
	return tailoring->benchmark_ref;
}

const char *xccdf_tailoring_get_benchmark_ref_version(const struct xccdf_tailoring *tailoring)
{
	return tailoring->benchmark_ref_version;
}

bool xccdf_tailoring_set_id(struct xccdf_tailoring *tailoring, const char* newval)
{
	if (tailoring->id)
		free(tailoring->id);

	tailoring->id = oscap_strdup(newval);
	return true;
}

bool xccdf_tailoring_set_version(struct xccdf_tailoring *tailoring, const char *newval)
{
	if (tailoring->version)
		free(tailoring->version);

	tailoring->version = oscap_strdup(newval);
	return true;
}

bool xccdf_tailoring_set_version_update(struct xccdf_tailoring *tailoring, const char *newval)
{
	if (tailoring->version_update)
		free(tailoring->version_update);

	tailoring->version_update = oscap_strdup(newval);
	return true;
}

bool xccdf_tailoring_set_version_time(struct xccdf_tailoring *tailoring, const char *newval)
{
	if (tailoring->version_time)
		free(tailoring->version_time);

	tailoring->version_time = oscap_strdup(newval);
	return true;
}

bool xccdf_tailoring_set_benchmark_ref(struct xccdf_tailoring *tailoring, const char *newval)
{
	if (tailoring->benchmark_ref)
		free(tailoring->benchmark_ref);

	tailoring->benchmark_ref = oscap_strdup(newval);
	return true;
}

bool xccdf_tailoring_set_benchmark_ref_version(struct xccdf_tailoring *tailoring, const char *newval)
{
	if (tailoring->benchmark_ref_version)
		free(tailoring->benchmark_ref_version);

	tailoring->benchmark_ref_version = oscap_strdup(newval);
	return true;
}

struct oscap_string_iterator *xccdf_tailoring_get_metadata(const struct xccdf_tailoring *tailoring)
{
	return (struct oscap_string_iterator*) oscap_iterator_new(tailoring->metadata);
}

struct xccdf_profile_iterator *xccdf_tailoring_get_profiles(const struct xccdf_tailoring *tailoring)
{
	return (struct xccdf_profile_iterator*) oscap_iterator_new(tailoring->profiles);
}

struct xccdf_status_iterator *xccdf_tailoring_get_statuses(const struct xccdf_tailoring *tailoring)
{
	return (struct xccdf_status_iterator*) oscap_iterator_new(tailoring->statuses);
}

struct oscap_reference_iterator *xccdf_tailoring_get_dc_statuses(const struct xccdf_tailoring *tailoring)
{
	return (struct oscap_reference_iterator*) oscap_iterator_new(tailoring->dc_statuses);
}

struct xccdf_profile *
xccdf_tailoring_get_profile_by_id(const struct xccdf_tailoring *tailoring, const char *profile_id)
{
	struct xccdf_profile_iterator *profit = xccdf_tailoring_get_profiles(tailoring);
	while (xccdf_profile_iterator_has_more(profit)) {
		struct xccdf_profile *profile = xccdf_profile_iterator_next(profit);
		if (profile == NULL) {
			assert(profile != NULL);
			continue;
		}
		if (oscap_streq(xccdf_profile_get_id(profile), profile_id)) {
			xccdf_profile_iterator_free(profit);
			return profile;
		}
	}
	xccdf_profile_iterator_free(profit);
	return NULL;
}
