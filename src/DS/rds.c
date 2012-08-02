/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
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

#include "public/ds.h"
#include "oscap.h"
#include "alloc.h"
#include "common/_error.h"
#include "common/util.h"

#include <sys/stat.h>
#include <time.h>
#include <libgen.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <string.h>
#include <text.h>

static const char* arf_ns_uri = "http://scap.nist.gov/schema/asset-reporting-format/1.1";
static const char* core_ns_uri = "http://scap.nist.gov/schema/reporting-core/1.1";
static const char* arfvocab_ns_uri = "http://scap.nist.gov/vocabulary/arf/relationships/1.0#";
static const char* ai_ns_uri = "http://scap.nist.gov/schema/asset-identification/1.1";

static xmlNodePtr ds_rds_create_report(xmlDocPtr target_doc, xmlNodePtr reports_node, xmlDocPtr source_doc, const char* report_id)
{
	xmlNsPtr arf_ns = xmlSearchNsByHref(target_doc, xmlDocGetRootElement(target_doc), BAD_CAST arf_ns_uri);

	xmlNodePtr report = xmlNewNode(arf_ns, BAD_CAST "report");
	xmlSetProp(report, BAD_CAST "id", BAD_CAST report_id);

	xmlNodePtr report_content = xmlNewNode(arf_ns, BAD_CAST "content");
	xmlAddChild(report, report_content);

	xmlDOMWrapCtxtPtr wrap_ctxt = xmlDOMWrapNewCtxt();
	xmlNodePtr res_node = NULL;
	xmlDOMWrapCloneNode(wrap_ctxt, source_doc, xmlDocGetRootElement(source_doc),
			&res_node, target_doc, NULL, 1, 0);
	xmlAddChild(report_content, res_node);
	xmlDOMWrapReconcileNamespaces(wrap_ctxt, res_node, 0);
	xmlDOMWrapFreeCtxt(wrap_ctxt);

	xmlAddChild(reports_node, report);

	return report;
}

static void ds_rds_add_relationship(xmlDocPtr doc, xmlNodePtr relationships,
		const char* type, const char* subject, const char* ref)
{
	xmlNsPtr core_ns = xmlSearchNsByHref(doc, xmlDocGetRootElement(doc), BAD_CAST core_ns_uri);

	// create relationship between given request and the report
	xmlNodePtr relationship = xmlNewNode(core_ns, BAD_CAST "relationship");
	xmlSetProp(relationship, BAD_CAST "type", BAD_CAST type);
	xmlSetProp(relationship, BAD_CAST "subject", BAD_CAST subject);

	xmlNodePtr ref_node = xmlNewNode(core_ns, BAD_CAST "ref");
	xmlNodeSetContent(ref_node, BAD_CAST ref);
	xmlAddChild(relationship, ref_node);

	xmlAddChild(relationships, relationship);
}

static xmlNodePtr ds_rds_add_ai_from_xccdf_results(xmlDocPtr doc, xmlNodePtr assets,
		xmlDocPtr xccdf_result_doc)
{
	xmlNsPtr arf_ns = xmlSearchNsByHref(doc, xmlDocGetRootElement(doc), BAD_CAST arf_ns_uri);
	xmlNsPtr ai_ns = xmlSearchNsByHref(doc, xmlDocGetRootElement(doc), BAD_CAST ai_ns_uri);

	xmlNodePtr asset = xmlNewNode(arf_ns, BAD_CAST "asset");

	// Lets figure out a unique asset identification
	// The format is: "asset%i" where %i is a increasing integer suffix
	//
	// We use a very simple optimization, we know that assets will be "ordered"
	// by their @id because we are adding them there in that order.
	// Whenever we get a collision we can simply bump the suffix and continue,
	// no need to go back and check the previous assets.

	xmlNodePtr child_asset = assets->children;

	unsigned int suffix = 0;
	for (; child_asset != NULL; child_asset = child_asset->next)
	{
		if (child_asset->type != XML_ELEMENT_NODE)
			continue;

		if (strcmp((const char*)(child_asset->name), "asset") != 0)
			continue;

		char* id_candidate = oscap_sprintf("asset%i", suffix);
		xmlChar* id = xmlGetProp(child_asset, BAD_CAST "id");
		oscap_free(id_candidate);

		if (strcmp(id_candidate, (const char*)id) == 0)
		{
			suffix++;
		}
	}

	char* id = oscap_sprintf("asset%i", suffix);
	xmlSetProp(asset, BAD_CAST "id", BAD_CAST id);
	oscap_free(id);

	xmlAddChild(assets, asset);

	xmlNodePtr computing_device = xmlNewNode(ai_ns, BAD_CAST "computing-device");
	xmlAddChild(asset, computing_device);

	xmlNodePtr connections = xmlNewNode(ai_ns, BAD_CAST "connections");
	xmlAddChild(computing_device, connections);

	xmlNodePtr test_result = xmlDocGetRootElement(xccdf_result_doc);

	xmlNodePtr test_result_child = test_result->children;

	for (; test_result_child != NULL; test_result_child = test_result_child->next)
	{
		if (test_result_child->type != XML_ELEMENT_NODE)
			continue;

		if (strcmp((const char*)(test_result_child->name), "target") == 0)
		{
			xmlChar* content = xmlNodeGetContent(test_result_child);
			xmlNewTextChild(computing_device, ai_ns, BAD_CAST "fqdn", content);
			xmlFree(content);
		}
		else if (strcmp((const char*)(test_result_child->name), "target-address") == 0)
		{
			xmlNodePtr connection = xmlNewNode(ai_ns, BAD_CAST "connection");
			xmlAddChild(connections, connection);
			xmlNodePtr ip_address = xmlNewNode(ai_ns, BAD_CAST "ip-address");
			xmlAddChild(connection, ip_address);

			xmlChar* content = xmlNodeGetContent(test_result_child);
			xmlNewTextChild(ip_address, ai_ns, BAD_CAST "ip-v4", content);
			xmlFree(content);
		}
	}

	return asset;
}

static void ds_rds_add_xccdf_test_results(xmlDocPtr doc, xmlNodePtr reports,
		xmlDocPtr xccdf_result_file_doc, xmlNodePtr relationships, xmlNodePtr assets,
		const char* report_request_id)
{
	xmlNodePtr root_element = xmlDocGetRootElement(xccdf_result_file_doc);

	// There are 2 possible scenarios here:

	// 1) root element of given xccdf result file doc is a TestResult element
	// This is the easier scenario, we will just use ds_rds_create_report and
	// be done with it.
	if (strcmp((const char*)root_element->name, "TestResult") == 0)
	{
		ds_rds_create_report(doc, reports, xccdf_result_file_doc, "xccdf1");
		ds_rds_add_relationship(doc, relationships, "arfvocab:createdFor",
				"xccdf1", report_request_id);

		xmlNodePtr asset = ds_rds_add_ai_from_xccdf_results(doc, assets, xccdf_result_file_doc);
		char* asset_id = (char*)xmlGetProp(asset, BAD_CAST "id");
		ds_rds_add_relationship(doc, relationships, "arfvocab:isAbout",
				"xccdf1", asset_id);
		xmlFree(asset_id);
	}

	// 2) the root element is a Benchmark, TestResults are embedded within
	// We will have to walk through all elements, wrap each TestResult
	// in a xmlDoc and add them separately
	else if (strcmp((const char*)root_element->name, "Benchmark") == 0)
	{
		unsigned int report_suffix = 1;

		xmlNodePtr candidate_result = root_element->children;

		for (; candidate_result != NULL; candidate_result = candidate_result->next)
		{
			if (candidate_result->type != XML_ELEMENT_NODE)
				continue;

			if (strcmp((const char*)(candidate_result->name), "TestResult") != 0)
				continue;

			xmlDocPtr wrap_doc = xmlNewDoc(BAD_CAST "1.0");

			xmlDOMWrapCtxtPtr wrap_ctxt = xmlDOMWrapNewCtxt();
			xmlNodePtr res_node = NULL;
			xmlDOMWrapCloneNode(wrap_ctxt, xccdf_result_file_doc, candidate_result,
					&res_node, wrap_doc, NULL, 1, 0);
			xmlDocSetRootElement(wrap_doc, res_node);
			xmlDOMWrapReconcileNamespaces(wrap_ctxt, res_node, 0);
			xmlDOMWrapFreeCtxt(wrap_ctxt);

			char* report_id = oscap_sprintf("xccdf%i", report_suffix++);
			ds_rds_create_report(doc, reports, wrap_doc, report_id);
			ds_rds_add_relationship(doc, relationships, "arfvocab:createdFor",
					report_id, report_request_id);

			xmlNodePtr asset = ds_rds_add_ai_from_xccdf_results(doc, assets, wrap_doc);
			char* asset_id = (char*)xmlGetProp(asset, BAD_CAST "id");
			ds_rds_add_relationship(doc, relationships, "arfvocab:isAbout",
					report_id, asset_id);
			xmlFree(asset_id);

			oscap_free(report_id);

			xmlFreeDoc(wrap_doc);
		}
	}

	else
	{
		char* error = oscap_sprintf(
				"Unknown root element '%s' in given XCCDF result document, expected TestResult or Benchmark.",
				(const char*)root_element->name);

		oscap_seterr(OSCAP_EFAMILY_XML, 0, error);
		oscap_free(error);
	}
}

static xmlDocPtr ds_rds_create_from_dom(xmlDocPtr sds_doc, xmlDocPtr xccdf_result_file_doc, xmlDocPtr* oval_result_docs)
{
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root = xmlNewNode(NULL, BAD_CAST "asset-report-collection");
	xmlDocSetRootElement(doc, root);

	xmlNsPtr arf_ns = xmlNewNs(root, BAD_CAST arf_ns_uri, BAD_CAST "arf");
	xmlSetNs(root, arf_ns);

	xmlNsPtr core_ns = xmlNewNs(root, BAD_CAST core_ns_uri, BAD_CAST "core");
	xmlNewNs(root, BAD_CAST ai_ns_uri, BAD_CAST "ai");

	xmlNodePtr relationships = xmlNewNode(core_ns, BAD_CAST "relationships");
	xmlNewNs(relationships, BAD_CAST arfvocab_ns_uri, BAD_CAST "arfvocab");
	xmlAddChild(root, relationships);

	xmlNodePtr report_requests = xmlNewNode(arf_ns, BAD_CAST "report-requests");
	xmlAddChild(root, report_requests);

	xmlNodePtr assets = xmlNewNode(arf_ns, BAD_CAST "assets");
	xmlAddChild(root, assets);

	xmlNodePtr report_request = xmlNewNode(arf_ns, BAD_CAST "report-request");
	xmlSetProp(report_request, BAD_CAST "id", BAD_CAST "collection1");

	xmlNodePtr arf_content = xmlNewNode(arf_ns, BAD_CAST "content");

	xmlDOMWrapCtxtPtr sds_wrap_ctxt = xmlDOMWrapNewCtxt();
	xmlNodePtr sds_res_node = NULL;
	xmlDOMWrapCloneNode(sds_wrap_ctxt, sds_doc, xmlDocGetRootElement(sds_doc),
			&sds_res_node, doc, NULL, 1, 0);
	xmlAddChild(arf_content, sds_res_node);
	xmlDOMWrapReconcileNamespaces(sds_wrap_ctxt, sds_res_node, 0);
	xmlDOMWrapFreeCtxt(sds_wrap_ctxt);

	xmlAddChild(report_request, arf_content);

	xmlAddChild(report_requests, report_request);

	xmlNodePtr reports = xmlNewNode(arf_ns, BAD_CAST "reports");

	ds_rds_add_xccdf_test_results(doc, reports, xccdf_result_file_doc,
			relationships, assets, "collection1");

	unsigned int oval_report_suffix = 2;
	while (*oval_result_docs != NULL)
	{
		xmlDocPtr oval_result_doc = *oval_result_docs;

		char* report_id = oscap_sprintf("oval%i", oval_report_suffix++);
		ds_rds_create_report(doc, reports, oval_result_doc, report_id);
		oscap_free(report_id);

		oval_result_docs++;
	}

	xmlAddChild(root, reports);

	return doc;
}

void ds_rds_create(const char* sds_file, const char* xccdf_result_file, const char** oval_result_files, const char* target_file)
{
	xmlDocPtr sds_doc = xmlReadFile(sds_file, NULL, 0);
	xmlDocPtr result_file_doc = xmlReadFile(xccdf_result_file, NULL, 0);

	xmlDocPtr* oval_result_docs = oscap_alloc(1 * sizeof(xmlDocPtr));
	size_t oval_result_docs_count = 0;
	oval_result_docs[0] = NULL;

	// this check is there to allow passing NULL instead of having to allocate
	// an empty array
	if (oval_result_files != NULL)
	{
		while (*oval_result_files != NULL)
		{
			oval_result_docs[oval_result_docs_count] = xmlReadFile(*oval_result_files, NULL, 0);
			oval_result_docs = oscap_realloc(oval_result_docs, (++oval_result_docs_count + 1) * sizeof(xmlDocPtr));
			oval_result_docs[oval_result_docs_count] = 0;
			oval_result_files++;
		}
	}

	xmlDocPtr rds_doc = ds_rds_create_from_dom(sds_doc, result_file_doc, oval_result_docs);
	xmlSaveFileEnc(target_file, rds_doc, "utf-8");
	xmlFreeDoc(rds_doc);

	xmlDocPtr* oval_result_docs_ptr = oval_result_docs;
	while (*oval_result_docs_ptr != NULL)
	{
		xmlFreeDoc(*oval_result_docs_ptr);
		oval_result_docs_ptr++;
	}

	oscap_free(oval_result_docs);

	xmlFreeDoc(sds_doc);
	xmlFreeDoc(result_file_doc);
}
