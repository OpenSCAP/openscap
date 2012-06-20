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

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <string.h>
#include <text.h>

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

static xmlNodePtr ds_ids_find_component_ref(xmlDocPtr doc, xmlNodePtr datastream, const char* id)
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

            const char* cref_id = (const char*)xmlGetProp(component_ref, (const xmlChar*)"id");
            if (strcmp(cref_id, id) == 0)
            {
                return component_ref;
            }
        }
    }

    return NULL;
}

static void ds_ids_dump_component(const char* component_id, xmlDocPtr doc, const char* filename)
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

        const char* candidate_id = (const char*)xmlGetProp(candidate, (const xmlChar*)"id");
        if (strcmp(candidate_id, component_id) == 0)
        {
            component = candidate;
            break;
        }
    }

    if (component == NULL)
    {
        oscap_seterr(OSCAP_EFAMILY_XML, 0, "Component of given id was not found in the document.");
        return;
    }

    xmlNodePtr inner_root = node_get_child_element(component, NULL); // FIXME: More checking!

    if (inner_root == NULL)
    {
        oscap_seterr(OSCAP_EFAMILY_XML, 0, "Found component has no element contents, nothing to dump.");
        return;
    }

    xmlDOMWrapCtxtPtr wrap_ctxt = xmlDOMWrapNewCtxt();

    xmlDocPtr new_doc = xmlNewDoc((const xmlChar*)"1.0");
    xmlNodePtr res_node = NULL;
    xmlDOMWrapCloneNode(wrap_ctxt, doc, inner_root, &res_node, new_doc, NULL, 1, 0);
    xmlDocSetRootElement(new_doc, res_node);
    xmlDOMWrapReconcileNamespaces(wrap_ctxt, res_node, 0);
    xmlSaveFileEnc(filename, new_doc, "utf-8");
    xmlFreeDoc(new_doc);

    xmlDOMWrapFreeCtxt(wrap_ctxt);
}

static void ds_ids_dump_component_ref_as(xmlNodePtr component_ref, xmlDocPtr doc, xmlNodePtr datastream, const char* target_dir, const char* filename)
{
    const char* cref_id = (const char*)xmlGetProp(component_ref, (const xmlChar*)"id");
    if (!cref_id)
    {
        oscap_seterr(OSCAP_EFAMILY_XML, 0, "No or invalid id attribute on given component-ref.");
        return;
    }

    const char* xlink_href = (const char*)xmlGetProp(component_ref, (const xmlChar*)"href");
    if (!xlink_href || xlink_href == '\0')
    {
        oscap_seterr(OSCAP_EFAMILY_XML, 0, "No or invalid xlink:href attribute on given component-ref.");
        return;
    }

    const char* component_id = xlink_href + 1 * sizeof(char);
    const char* target_filename = oscap_sprintf("%s/%s", target_dir, filename);
    ds_ids_dump_component(component_id, doc, target_filename);
    oscap_free(target_filename);

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

            const char* name = (const char*)xmlGetProp(uri, (const xmlChar*)"name");

            if (!name)
            {
                oscap_seterr(OSCAP_EFAMILY_XML, 0, "No or invalid name for a component referenced in the catalog. Skipping...");
                continue;
            }

            const char* str_uri = (const char*)xmlGetProp(uri, (const xmlChar*)"uri");

            if (!str_uri || str_uri == '\0')
            {
                oscap_seterr(OSCAP_EFAMILY_XML, 0, "No or invalid URI for a component referenced in the catalog. Skipping...");
                continue;
            }

            // the pointer arithmetics simply skips the first character which is '#'
            xmlNodePtr cat_component_ref = ds_ids_find_component_ref(doc, datastream, str_uri + 1 * sizeof(char));

            if (!cat_component_ref)
            {
                oscap_seterr(OSCAP_EFAMILY_XML, 0, "component-ref with given id wasn't found in the document!");
                continue;
            }

            ds_ids_dump_component_ref_as(cat_component_ref, doc, datastream, target_dir, name);
        }
    }
}

static void ds_ids_dump_component_ref(xmlNodePtr component_ref, xmlDocPtr doc, xmlNodePtr datastream, const char* target_dir)
{
    const char* xlink_href = (const char*)xmlGetProp(component_ref, (const xmlChar*)"href");
    if (!xlink_href || xlink_href == '\0')
    {
        oscap_seterr(OSCAP_EFAMILY_XML, 0, "No or invalid xlink:href attribute on given component-ref.");
        return;
    }

    ds_ids_dump_component_ref_as(component_ref, doc, datastream, target_dir, xlink_href + 1 * sizeof(char));
}

void ds_ids_decompose(const char* input_file, const char* id, const char* target_dir)
{
	xmlDocPtr doc = xmlReadFile(input_file, NULL, 0);

    if (!doc)
    {
        oscap_seterr(OSCAP_EFAMILY_XML, xmlGetLastError() ? xmlGetLastError()->code : 0, "Could not read given input file.");
        return;
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

        const char* candidate_id = (const char*)xmlGetProp(candidate_datastream, (const xmlChar*)"id");
        if (id == NULL || (candidate_id != NULL && strcmp(id, candidate_id) == 0))
        {
            datastream = candidate_datastream;
            break;
        }
    }

    if (!datastream)
    {
        oscap_seterr(OSCAP_EFAMILY_XML, 0, "Could not find any matching datastream.");
        return;
    }

    xmlNodePtr checklists = node_get_child_element(datastream, "checklists");

    if (!checklists)
    {
        oscap_seterr(OSCAP_EFAMILY_XML, 0, "No checklists element found in the matching datastream.");
        return;
    }

    xmlNodePtr component_ref = checklists->children;

    for (; component_ref != NULL; component_ref = component_ref->next)
    {
        if (component_ref->type != XML_ELEMENT_NODE)
            continue;

        if (strcmp((const char*)(component_ref->name), "component-ref") != 0)
            continue;

        ds_ids_dump_component_ref(component_ref, doc, datastream, target_dir);
    }

    xmlFreeDoc(doc);
}

