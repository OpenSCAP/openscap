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

static void ds_rds_create_report(xmlDocPtr target_doc, xmlNodePtr reports_node, xmlDocPtr source_doc, const char* report_id)
{
    xmlNsPtr arf_ns = xmlSearchNsByHref(target_doc, xmlDocGetRootElement(target_doc), BAD_CAST arf_ns_uri);

    xmlNodePtr report = xmlNewNode(arf_ns, BAD_CAST "report");
    xmlSetProp(report, BAD_CAST "id", BAD_CAST report_id);

    xmlDOMWrapCtxtPtr wrap_ctxt = xmlDOMWrapNewCtxt();
    xmlNodePtr res_node = NULL;
    xmlDOMWrapCloneNode(wrap_ctxt, source_doc, xmlDocGetRootElement(source_doc),
                        &res_node, target_doc, NULL, 1, 0);
    xmlAddChild(report, res_node);
    xmlDOMWrapReconcileNamespaces(wrap_ctxt, res_node, 0);
    xmlDOMWrapFreeCtxt(wrap_ctxt);

    xmlAddChild(reports_node, report);
}

static xmlDocPtr ds_rds_create_from_dom(xmlDocPtr sds_doc, xmlDocPtr xccdf_result_file_doc, xmlDocPtr* oval_result_files)
{
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root = xmlNewNode(NULL, BAD_CAST "asset-report-collection");
    xmlDocSetRootElement(doc, root);

    xmlNsPtr arf_ns = xmlNewNs(root, BAD_CAST arf_ns_uri, BAD_CAST "arf");
    xmlSetNs(root, arf_ns);

    xmlNodePtr report_requests = xmlNewNode(arf_ns, BAD_CAST "report-requests");
    xmlAddChild(root, report_requests);

    xmlNodePtr report_request = xmlNewNode(arf_ns, BAD_CAST "report-request");
    xmlSetProp(report_request, BAD_CAST "id", BAD_CAST "collection1");

    xmlNodePtr arf_content = xmlNewNode(arf_ns, BAD_CAST "arf-content");

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

    ds_rds_create_report(doc, reports, xccdf_result_file_doc, "xccdf1");

    unsigned int oval_report_suffix = 2;
    while (oval_result_files != NULL)
    {
        xmlDocPtr oval_result_file = *oval_result_files;

        char* report_id = oscap_sprintf("oval%i", oval_report_suffix++);
        ds_rds_create_report(doc, reports, oval_result_file, report_id);
        oscap_free(report_id);

        oval_result_files++;
    }

    xmlAddChild(root, reports);

    return doc;
}

void ds_rds_create(const char* sds_file, const char* xccdf_result_file, const char* target_file)
{
    xmlDocPtr sds_doc = xmlReadFile(sds_file, NULL, 0);
    xmlDocPtr result_file_doc = xmlReadFile(xccdf_result_file, NULL, 0);

    xmlDocPtr rds_doc = ds_rds_create_from_dom(sds_doc, result_file_doc, NULL);
    xmlSaveFileEnc(target_file, rds_doc, "utf-8");
    xmlFreeDoc(rds_doc);

    xmlFreeDoc(sds_doc);
    xmlFreeDoc(result_file_doc);
}
