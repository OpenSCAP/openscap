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
#include "common/alloc.h"
#include "common/_error.h"
#include "common/util.h"

#include <sys/stat.h>
#include <time.h>
#include <libgen.h>

#include <libxml/xmlreader.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <string.h>
#include <text.h>

#ifndef MAXPATHLEN
#   define MAXPATHLEN 1024
#endif

static const char* datastream_ns_uri = "http://scap.nist.gov/schema/scap/source/1.2";
static const char* xlink_ns_uri = "http://www.w3.org/1999/xlink";
static const char* cat_ns_uri = "urn:oasis:names:tc:entity:xmlns:xml:catalog";

int ds_is_sds(const char* file)
{
	xmlTextReaderPtr reader = xmlReaderForFile(file, NULL, 0);
	if (!reader) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "Unable to open file: '%s'", file);
		return -1;
	}

	int result = -1;

	int ret;
	ret = xmlTextReaderRead(reader);
	while (ret == 1)
	{
		if (xmlTextReaderNodeType(reader) == 1)
		{
			const char* name = (const char*)xmlTextReaderConstLocalName(reader);
			if (name == NULL)
				break;

			result = strcmp(name, "data-stream-collection") == 0 ? 0 : 1;
			break;
		}

		ret = xmlTextReaderRead(reader);
	}

	xmlFreeTextReader(reader);

	if (result == -1)
		oscap_seterr(OSCAP_EFAMILY_XML, "Can't locate the root element of '%s' XML file.", file);

	return result;
}

static int mkdir_p(const char* path)
{
	if (strlen(path) > MAXPATHLEN)
	{
		return -1;
	}
	else
	{
		char temp[MAXPATHLEN + 1]; // +1 for \0
		unsigned int i;

		for (i = 0; i <= strlen(path); i++)
		{
			if (path[i] == '/' || path[i] == '\0')
			{
				strncpy(temp, path, i);
				temp[i] = '\0';
				mkdir(temp, S_IRWXU);
			}
		}

		return 0;
	}
}

static xmlNodePtr node_get_child_element(xmlNodePtr parent, const char* name)
{
	xmlNodePtr candidate = parent->children;

	for (; candidate != NULL; candidate = candidate->next)
	{
		if (candidate->type != XML_ELEMENT_NODE)
			continue;

		if (name != NULL && strcmp((const char*)(candidate->name), name) != 0)
			continue;

		return candidate;
	}

	return NULL;
}

static xmlNodePtr ds_sds_find_component_ref(xmlDocPtr doc, xmlNodePtr datastream, const char* id)
{
	xmlNodePtr cref_parent = datastream->children;

	for (; cref_parent != NULL; cref_parent = cref_parent->next)
	{
		if (cref_parent->type != XML_ELEMENT_NODE)
			continue;

		xmlNodePtr component_ref = cref_parent->children;

		for (; component_ref != NULL; component_ref = component_ref->next)
		{
			if (component_ref->type != XML_ELEMENT_NODE)
				continue;

			if (strcmp((const char*)(component_ref->name), "component-ref") != 0)
				continue;

			char* cref_id = (char*)xmlGetProp(component_ref, BAD_CAST "id");
			if (strcmp(cref_id, id) == 0)
			{
				xmlFree(cref_id);
				return component_ref;
			}
			xmlFree(cref_id);
		}
	}

	return NULL;
}

static int ds_sds_dump_component(const char* component_id, xmlDocPtr doc, const char* filename)
{
	xmlNodePtr root = xmlDocGetRootElement(doc);
	xmlNodePtr component = NULL;
	xmlNodePtr candidate = root->children;

	for (; candidate != NULL; candidate = candidate->next)
	{
		if (candidate->type != XML_ELEMENT_NODE)
			continue;

		if (strcmp((const char*)(candidate->name), "component") != 0)
			continue;

		char* candidate_id = (char*)xmlGetProp(candidate, BAD_CAST "id");
		if (strcmp(candidate_id, component_id) == 0)
		{
			component = candidate;
			xmlFree(candidate_id);
			break;
		}
		xmlFree(candidate_id);
	}

	if (component == NULL)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Component of given id '%s' was not found in the document.", component_id);
		return -1;
	}

	xmlNodePtr inner_root = node_get_child_element(component, NULL);

	if (inner_root == NULL)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Found component (id='%s') but it has no element contents, nothing to dump, skipping...", component_id);
		return -1;
	}

	xmlDOMWrapCtxtPtr wrap_ctxt = xmlDOMWrapNewCtxt();

	xmlDocPtr new_doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr res_node = NULL;
	if (xmlDOMWrapCloneNode(wrap_ctxt, doc, inner_root, &res_node, new_doc, NULL, 1, 0) != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Error when cloning node while dumping component (id='%s').", component_id);
		xmlFreeDoc(new_doc);
		xmlDOMWrapFreeCtxt(wrap_ctxt);
		return -1;
	}
	xmlDocSetRootElement(new_doc, res_node);
	if (xmlDOMWrapReconcileNamespaces(wrap_ctxt, res_node, 0) != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Internal libxml error when reconciling namespaces while dumping component (id='%s').", component_id);
		xmlFreeDoc(new_doc);
		xmlDOMWrapFreeCtxt(wrap_ctxt);
		return -1;
	}
	if (xmlSaveFileEnc(filename, new_doc, "utf-8") == -1)
	{
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "Error when saving resulting DOM to file '%s' while dumping component (id='%s').", filename, component_id);
		xmlFreeDoc(new_doc);
		xmlDOMWrapFreeCtxt(wrap_ctxt);
		return -1;
	}
	xmlFreeDoc(new_doc);

	xmlDOMWrapFreeCtxt(wrap_ctxt);

	return 0;
}

static int ds_sds_dump_component_ref_as(xmlNodePtr component_ref, xmlDocPtr doc, xmlNodePtr datastream, const char* target_dir, const char* filename)
{
	char* cref_id = (char*)xmlGetProp(component_ref, BAD_CAST "id");
	if (!cref_id)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "No or invalid id attribute on given component-ref.");
		xmlFree(cref_id);
		return -1;
	}

	char* xlink_href = (char*)xmlGetNsProp(component_ref, BAD_CAST "href", BAD_CAST xlink_ns_uri);
	if (!xlink_href || strlen(xlink_href) < 2)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "No or invalid xlink:href attribute on given component-ref.");
		xmlFree(cref_id);
		xmlFree(xlink_href);
		return -1;
	}

	const char* component_id = xlink_href + 1 * sizeof(char);
	char* filename_cpy = oscap_sprintf("./%s", filename);
	char* file_reldir = dirname(filename_cpy);

	// the cast is safe to do because we are using the GNU basename, it doesn't
	// modify the string
	const char* file_basename = basename((char*)filename);

	const char* target_filename_dirname = oscap_sprintf("%s/%s", target_dir, file_reldir);
	if (mkdir_p(target_filename_dirname) == -1)
	{
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "Error making directory '%s' while dumping component to file '%s'.", target_dir, filename);
		xmlFree(cref_id);
		xmlFree(xlink_href);
		return -1;
	}
	const char* target_filename = oscap_sprintf("%s/%s/%s", target_dir, file_reldir, file_basename);
	ds_sds_dump_component(component_id, doc, target_filename);
	oscap_free(target_filename);
	oscap_free(filename_cpy);

	xmlNodePtr catalog = node_get_child_element(component_ref, "catalog");
	if (catalog)
	{
		xmlNodePtr uri = catalog->children;

		for (; uri != NULL; uri = uri->next)
		{
			if (uri->type != XML_ELEMENT_NODE)
				continue;

			if (strcmp((const char*)(uri->name), "uri") != 0)
				continue;

			char* name = (char*)xmlGetProp(uri, BAD_CAST "name");

			if (!name)
			{
				oscap_seterr(OSCAP_EFAMILY_XML, "No 'name' attribute for a component referenced in the catalog of component '%s'.", component_id);
				xmlFree(cref_id);
				xmlFree(xlink_href);
				return -1;
			}

			char* str_uri = (char*)xmlGetProp(uri, BAD_CAST "uri");

			if (!str_uri || strlen(str_uri) < 2)
			{
				oscap_seterr(OSCAP_EFAMILY_XML, "No or invalid 'uri' attribute for a component referenced in the catalog of component '%s'.", component_id);
				xmlFree(str_uri);
				xmlFree(name);
				xmlFree(cref_id);
				xmlFree(xlink_href);
				return -1;
			}

			// the pointer arithmetics simply skips the first character which is '#'
			xmlNodePtr cat_component_ref = ds_sds_find_component_ref(doc, datastream, str_uri + 1 * sizeof(char));

			if (!cat_component_ref)
			{
				oscap_seterr(OSCAP_EFAMILY_XML, "component-ref with given id '%s' wasn't found in the document! We are looking for it because it's in the catalog of component '%s'.", str_uri + 1 * sizeof(char), component_id);
				xmlFree(str_uri);
				xmlFree(name);
				xmlFree(cref_id);
				xmlFree(xlink_href);
				return -1;
			}
			xmlFree(str_uri);

			if (ds_sds_dump_component_ref_as(cat_component_ref, doc, datastream, target_filename_dirname, name) != 0)
				return -1; // no need to call oscap_seterr here, it's already set

			xmlFree(name);
		}
	}

	oscap_free(target_filename_dirname);
	xmlFree(cref_id);
	xmlFree(xlink_href);

	return 0;
}

static int ds_sds_dump_component_ref(xmlNodePtr component_ref, xmlDocPtr doc, xmlNodePtr datastream, const char* target_dir)
{
	char* xlink_href = (char*)xmlGetNsProp(component_ref, BAD_CAST "href", BAD_CAST xlink_ns_uri);
	if (!xlink_href || strlen(xlink_href) < 2)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "No or invalid xlink:href attribute on given component-ref.");
		xmlFree(xlink_href);
		return -1;
	}

	size_t offset = 1;

	// the prefix that we artificially add to conform the XSD for valiation
	// can be stripped for easier to use filenames
	const char* filler_prefix = "scap_org.open-scap_comp_";
	if (strncmp(xlink_href + offset * sizeof(char), filler_prefix, strlen(filler_prefix)) == 0)
	{
		offset += strlen(filler_prefix);
	}

	int result = ds_sds_dump_component_ref_as(component_ref, doc, datastream, target_dir, xlink_href + offset * sizeof(char));
	xmlFree(xlink_href);

	// if result is -1, oscap_seterr was already called, no need to call it again
	return result;
}

int ds_sds_decompose(const char* input_file, const char* id, const char* target_dir, const char* xccdf_filename)
{
	xmlDocPtr doc = xmlReadFile(input_file, NULL, 0);

	if (!doc)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Could not read/parse XML of given input file at path '%s'.", input_file);
		return -1;
	}

	xmlNodePtr root = xmlDocGetRootElement(doc);

	xmlNodePtr datastream = NULL;
	xmlNodePtr candidate_datastream = root->children;

	for (; candidate_datastream != NULL; candidate_datastream = candidate_datastream->next)
	{
		if (candidate_datastream->type != XML_ELEMENT_NODE)
			continue;

		if (strcmp((const char*)(candidate_datastream->name), "data-stream") != 0)
			continue;

		// at this point it is sure to be a <data-stream> element

		char* candidate_id = (char*)xmlGetProp(candidate_datastream, BAD_CAST "id");
		if (id == NULL || (candidate_id != NULL && strcmp(id, candidate_id) == 0))
		{
			datastream = candidate_datastream;
			xmlFree(candidate_id);
			break;
		}
		xmlFree(candidate_id);
	}

	if (!datastream)
	{
		const char* error = id ?
			oscap_sprintf("Could not find any datastream of id '%s'", id) :
			oscap_sprintf("Could not find any datastream inside the file");

		oscap_seterr(OSCAP_EFAMILY_XML, error);
		oscap_free(error);
		xmlFreeDoc(doc);
		return -1;
	}

	xmlNodePtr checklists = node_get_child_element(datastream, "checklists");

	if (!checklists)
	{
		if (!id)
			oscap_seterr(OSCAP_EFAMILY_XML, "No checklists element found in file '%s' in the first datastream.", input_file);
		else
			oscap_seterr(OSCAP_EFAMILY_XML, "No checklists element found in file '%s' in datastream of id '%s'.", input_file, id);

		xmlFreeDoc(doc);
		return -1;
	}

	xmlNodePtr component_ref = checklists->children;

	for (; component_ref != NULL; component_ref = component_ref->next)
	{
		if (component_ref->type != XML_ELEMENT_NODE)
			continue;

		if (strcmp((const char*)(component_ref->name), "component-ref") != 0)
			continue;

		int result;

		if (xccdf_filename == NULL)
		{
			result = ds_sds_dump_component_ref(component_ref, doc, datastream, strcmp(target_dir, "") == 0 ? "." : target_dir);
		}
		else
		{
			result = ds_sds_dump_component_ref_as(component_ref, doc, datastream, strcmp(target_dir, "") == 0 ? "." : target_dir, xccdf_filename);
		}

		if (result != 0)
		{
			// oscap_seterr was already called in ds_sds_dump_component[_as]
			xmlFreeDoc(doc);
			return -1;
		}
	}

	xmlFreeDoc(doc);
	return 0;
}

static bool strendswith(const char* str, const char* suffix)
{
	int str_shift = strlen(str) - strlen(suffix);
	if (str_shift < 0)
		return false;

	return strcmp(str + str_shift * sizeof(char), suffix) == 0;
}

static int ds_sds_compose_add_component(xmlDocPtr doc, xmlNodePtr datastream, const char* filepath, const char* comp_id)
{
	xmlNsPtr ds_ns = xmlSearchNsByHref(doc, datastream, BAD_CAST datastream_ns_uri);
	if (!ds_ns)
	{
		oscap_seterr(OSCAP_EFAMILY_GLIBC,
				"Unable to find namespace '%s' in the XML DOM tree when create "
				"source datastream. This is most likely an internal error!",
				datastream_ns_uri);
		return -1;
	}

	xmlNodePtr component = xmlNewNode(ds_ns, BAD_CAST "component");
	xmlSetProp(component, BAD_CAST "id", BAD_CAST comp_id);

	char file_timestamp[32];
	strcpy(file_timestamp, "0000-00-00T00:00:00");

	struct stat file_stat;
	if (stat(filepath, &file_stat) == 0)
		strftime(file_timestamp, 32, "%Y-%m-%dT%H:%M:%S", localtime(&file_stat.st_mtime));

	xmlSetProp(component, BAD_CAST "timestamp", BAD_CAST file_timestamp);

	xmlDocPtr component_doc = xmlReadFile(filepath, NULL, 0);

	if (!component_doc)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Could not read/parse XML of given input file at path '%s'.", filepath);
		xmlFreeNode(component);
		return -1;
	}

	xmlNodePtr component_root = xmlDocGetRootElement(component_doc);

	xmlDOMWrapCtxtPtr wrap_ctxt = xmlDOMWrapNewCtxt();

	xmlNodePtr res_component_root = NULL;
	if (xmlDOMWrapCloneNode(wrap_ctxt, component_doc, component_root, &res_component_root, doc, NULL, 1, 0) != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML,
				"Cannot clone node when adding component from file '%s' with id '%s' while "
				"creating source datastream.", filepath, comp_id);

		xmlDOMWrapFreeCtxt(wrap_ctxt);
		xmlFreeDoc(component_doc);
		xmlFreeNode(component);

		return -1;
	}
	if (xmlDOMWrapReconcileNamespaces(wrap_ctxt, res_component_root, 0) != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML,
				"Cannot reconcile namespaces when adding component from file '%s' with id '%s' while "
				"creating source datastream.", filepath, comp_id);

		xmlDOMWrapFreeCtxt(wrap_ctxt);
		xmlFreeDoc(component_doc);
		xmlFreeNode(component);

		return -1;
	}

	xmlAddChild(component, res_component_root);

	xmlDOMWrapFreeCtxt(wrap_ctxt);

	xmlNodePtr doc_root = xmlDocGetRootElement(doc);
	xmlAddChild(doc_root, component);

	xmlFreeDoc(component_doc);

	return 0;
}

static int ds_sds_compose_catalog_has_uri(xmlDocPtr doc, xmlNodePtr catalog, const char* uri)
{
	xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
	if (xpathCtx == NULL)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Error: unable to create a new XPath context.");
		return -1;
	}

	xmlXPathRegisterNs(xpathCtx, BAD_CAST "cat", BAD_CAST cat_ns_uri);
	xmlXPathRegisterNs(xpathCtx, BAD_CAST "xlink", BAD_CAST xlink_ns_uri);

	// limit xpath execution to just the catalog node
	// this is done for performance reasons
	xpathCtx->node = catalog;

	const char* expression = oscap_sprintf("cat:uri[@uri = '%s']", uri);

	xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(
			BAD_CAST expression,
			xpathCtx);

	oscap_free(expression);

	if (xpathObj == NULL)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Error: Unable to evalute XPath expression.");
		xmlXPathFreeContext(xpathCtx);

		return -1;
	}

	int result = 0;

	xmlNodeSetPtr nodeset = xpathObj->nodesetval;
	if (nodeset != NULL)
		result = nodeset->nodeNr > 0 ? 0 : 1;

	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx);

	return result;
}

static int ds_sds_compose_add_component_with_ref(xmlDocPtr doc, xmlNodePtr datastream, const char* filepath, const char* cref_id);

static int ds_sds_compose_add_xccdf_dependencies(xmlDocPtr doc, xmlNodePtr datastream, const char* filepath, xmlNodePtr catalog)
{
	xmlDocPtr component_doc = xmlReadFile(filepath, NULL, 0);
	if (component_doc == NULL)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Error: unable to read XCCDF from file '%s' while creating source datastream.", filepath);
		return -1;
	}

	xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
	if (xpathCtx == NULL)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Error: unable to create new XPath context.");
		xmlFreeDoc(component_doc);
		return -1;
	}

	//xmlXPathRegisterNs(xpathCtx, BAD_CAST "cdf11", BAD_CAST "http://checklists.nist.gov/xccdf/1.1");
	//xmlXPathRegisterNs(xpathCtx, BAD_CAST "cdf12", BAD_CAST "http://checklists.nist.gov/xccdf/1.2");

	xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(
			// we want robustness and support for future versions, this expression
			// retrieves check-content-refs from any namespace
			BAD_CAST "//*[local-name() = 'check-content-ref']",
			xpathCtx);

	if (xpathObj == NULL)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Error: Unable to evalute XPath expression.");
		xmlXPathFreeContext(xpathCtx);
		xmlFreeDoc(component_doc);

		return -1;
	}

	xmlNsPtr cat_ns = xmlSearchNsByHref(doc, datastream, BAD_CAST cat_ns_uri);

	xmlNodeSetPtr nodeset = xpathObj->nodesetval;
	if (nodeset != NULL)
	{
		char* filepath_cpy = oscap_strdup(filepath);
		const char* dir = dirname(filepath_cpy);

		for (int i = 0; i < nodeset->nodeNr; i++)
		{
			xmlNodePtr node = nodeset->nodeTab[i];

			if (node->type != XML_ELEMENT_NODE)
				continue;

			if (xmlHasProp(node, BAD_CAST "href"))
			{
				char* href = (char*)xmlGetProp(node, BAD_CAST "href");
				char* real_path = (strcmp(dir, "") == 0 || strcmp(dir, ".") == 0) ?
					oscap_strdup(href) : oscap_sprintf("%s/%s", dir, href);

				char* cref_id = oscap_sprintf("scap_org.open-scap_cref_%s", real_path);
				char* uri = oscap_sprintf("#%s", cref_id);

				// we don't want duplicated uri elements in the catalog
				if (ds_sds_compose_catalog_has_uri(doc, catalog, uri) == 0)
				{
					oscap_free(uri);
					oscap_free(cref_id);
					oscap_free(real_path);
					xmlFree(href);
					continue;
				}

				if (ds_sds_compose_add_component_with_ref(doc, datastream, real_path, cref_id) != 0)
				{
					// oscap_seterr has already been called

					xmlFreeDoc(component_doc);
					oscap_free(uri);
					oscap_free(cref_id);
					oscap_free(real_path);
					xmlFree(href);
					return -1;
				}
				oscap_free(cref_id);

				xmlNodePtr catalog_uri = xmlNewNode(cat_ns, BAD_CAST "uri");
				xmlSetProp(catalog_uri, BAD_CAST "name", BAD_CAST href);
				xmlSetProp(catalog_uri, BAD_CAST "uri", BAD_CAST uri);

				oscap_free(uri);
				oscap_free(real_path);
				xmlFree(href);

				xmlAddChild(catalog, catalog_uri);
			}
		}

		oscap_free(filepath_cpy);
	}

	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx);

	xmlFreeDoc(component_doc);

	return 0;
}

static int ds_sds_compose_has_component_ref(xmlDocPtr doc, xmlNodePtr datastream, const char* filepath, const char* cref_id)
{
	xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
	if (xpathCtx == NULL)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Error: unable to create new XPath context.");
		return -1;
	}

	xmlXPathRegisterNs(xpathCtx, BAD_CAST "ds", BAD_CAST datastream_ns_uri);
	xmlXPathRegisterNs(xpathCtx, BAD_CAST "xlink", BAD_CAST xlink_ns_uri);

	// limit xpath execution to just the datastream node
	// this is done for performance reasons
	xpathCtx->node = datastream;

	const char* expression = oscap_sprintf("*/ds:component-ref[@xlink:href = '#%s' and @id = '%s']", filepath, cref_id);

	xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(
			BAD_CAST expression,
			xpathCtx);

	oscap_free(expression);

	if (xpathObj == NULL)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Error: Unable to evalute XPath expression.");
		xmlXPathFreeContext(xpathCtx);

		return -1;
	}

	int result = 1;

	xmlNodeSetPtr nodeset = xpathObj->nodesetval;
	if (nodeset != NULL)
		result = nodeset->nodeNr > 0 ? 0 : 1;

	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx);

	return result;
}

int ds_sds_compose_add_component_with_ref(xmlDocPtr doc, xmlNodePtr datastream, const char* filepath, const char* cref_id)
{
	xmlNsPtr ds_ns = xmlSearchNsByHref(doc, datastream, BAD_CAST datastream_ns_uri);
	xmlNsPtr xlink_ns = xmlSearchNsByHref(doc, datastream, BAD_CAST xlink_ns_uri);
	xmlNsPtr cat_ns = xmlSearchNsByHref(doc, datastream, BAD_CAST cat_ns_uri);

	// In case we already have this component we just return, no need to add
	// it twice. We will typically have many references to OVAL files, adding
	// component for each reference would create unnecessarily huge datastreams
	int result = ds_sds_compose_has_component_ref(doc, datastream, filepath, cref_id);
	if (result == 0)
	{
		return 0;
	}

	if (result == -1)
	{
		// no need to free anything
		// oscap_seterr has already been called
		return -1;
	}

	char* comp_id = oscap_sprintf("scap_org.open-scap_comp_%s", filepath);
	ds_sds_compose_add_component(doc, datastream, filepath, comp_id);

	xmlNodePtr cref = xmlNewNode(ds_ns, BAD_CAST "component-ref");

	xmlSetProp(cref, BAD_CAST "id", BAD_CAST cref_id);

	const char* xlink_href = oscap_sprintf("#%s", comp_id);
	oscap_free(comp_id);

	xmlSetNsProp(cref, xlink_ns, BAD_CAST "href", BAD_CAST xlink_href);
	oscap_free(xlink_href);

	xmlNodePtr cref_catalog = xmlNewNode(cat_ns, BAD_CAST "catalog");
	xmlAddChild(cref, cref_catalog);

	xmlNodePtr cref_parent;

	if (strendswith(filepath, "-xccdf.xml"))
	{
		cref_parent = node_get_child_element(datastream, "checklists");
		if (ds_sds_compose_add_xccdf_dependencies(doc, datastream, filepath, cref_catalog) != 0)
		{
			// oscap_seterr has already been called
			return -1;
		}
	}
	else if (strendswith(filepath, "-oval.xml"))
	{
		cref_parent = node_get_child_element(datastream, "checks");
	}
	else if (strendswith(filepath, "-cpe-oval.xml") || strendswith(filepath, "-cpe-dictionary.xml"))
	{
		cref_parent = node_get_child_element(datastream, "dictionaries");
	}
	else
	{
		cref_parent = node_get_child_element(datastream, "extended-components");
	}

	// the source data stream XSD requires either no catalog or a non-empty one
	if (cref_catalog->children == NULL)
	{
		xmlUnlinkNode(cref_catalog);
		xmlFreeNode(cref_catalog);
	}

	xmlAddChild(cref_parent, cref);

	return 0;
}

int ds_sds_compose_from_xccdf(const char* xccdf_file, const char* target_datastream)
{
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root = xmlNewNode(NULL, BAD_CAST "data-stream-collection");
	xmlDocSetRootElement(doc, root);

	xmlNsPtr ds_ns = xmlNewNs(root, BAD_CAST datastream_ns_uri, BAD_CAST "ds");
	xmlSetNs(root, ds_ns);

	// we will need this namespace later when creating xlink:href attr in
	// component-ref
	xmlNewNs(root, BAD_CAST xlink_ns_uri, BAD_CAST "xlink");

	char* collection_id = oscap_sprintf("scap_org.open-scap_collection_from_xccdf_%s", xccdf_file);
	xmlSetProp(root, BAD_CAST "id", BAD_CAST collection_id);
	oscap_free(collection_id);

	xmlSetProp(root, BAD_CAST "schematron-version", BAD_CAST "1.0");

	// we will need this namespace later when creating component-ref
	// dependency catalog
	xmlNewNs(root, BAD_CAST cat_ns_uri, BAD_CAST "cat");

	xmlNodePtr datastream = xmlNewNode(ds_ns, BAD_CAST "data-stream");
	xmlAddChild(root, datastream);

	char* datastream_id = oscap_sprintf("scap_org.open-scap_datastream_from_xccdf_%s", xccdf_file);
	xmlSetProp(datastream, BAD_CAST "id", BAD_CAST datastream_id);
	oscap_free(datastream_id);

	xmlSetProp(datastream, BAD_CAST "scap-version", BAD_CAST "1.2");

	xmlSetProp(datastream, BAD_CAST "use-case", BAD_CAST "OTHER");

	xmlNodePtr dictionaries = xmlNewNode(ds_ns, BAD_CAST "dictionaries");
	xmlAddChild(datastream, dictionaries);

	xmlNodePtr checklists = xmlNewNode(ds_ns, BAD_CAST "checklists");
	xmlAddChild(datastream, checklists);

	xmlNodePtr checks = xmlNewNode(ds_ns, BAD_CAST "checks");
	xmlAddChild(datastream, checks);

	xmlNodePtr extended_components = xmlNewNode(ds_ns, BAD_CAST "extended-components");
	xmlAddChild(datastream, extended_components);

	char* cref_id = oscap_sprintf("scap_org.open-scap_cref_%s", xccdf_file);
	if (ds_sds_compose_add_component_with_ref(doc, datastream, xccdf_file, cref_id) != 0)
	{
		// oscap_seterr already called
		oscap_free(cref_id);
		return -1;
	}
	oscap_free(cref_id);

	// the XSD of source data stream enforces that the collection elements are
	// not empty if they are there, we will therefore now removes collections
	// where nothing has been added

	if (dictionaries->children == NULL)
	{
		xmlUnlinkNode(dictionaries);
		xmlFreeNode(dictionaries);
	}
	if (checklists->children == NULL)
	{
		xmlUnlinkNode(checklists);
		xmlFreeNode(checklists);
	}
	if (checks->children == NULL)
	{
		xmlUnlinkNode(checks);
		xmlFreeNode(checks);
	}
	if (extended_components->children == NULL)
	{
		xmlUnlinkNode(extended_components);
		xmlFreeNode(extended_components);
	}

	if (xmlSaveFileEnc(target_datastream, doc, "utf-8") == -1)
	{
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "Error saving source datastream to '%s'.", target_datastream);
		xmlFreeDoc(doc);
		return -1;
	}

	xmlFreeDoc(doc);
	return 0;
}
