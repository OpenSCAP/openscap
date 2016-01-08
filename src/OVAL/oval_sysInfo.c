/**
 * @file oval_sysInfo.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oval_system_characteristics_impl.h"
#include "adt/oval_collection_impl.h"
#include "oval_agent_api_impl.h"
#include "common/util.h"
#include "common/debug_priv.h"

typedef struct oval_sysinfo {
	struct oval_syschar_model *model;
	char *osName;
	char *osVersion;
	char *osArchitecture;
	char *primaryHostName;
	struct oval_collection *interfaces;
	char *anyxml;
} oval_sysinfo_t;

struct oval_sysinfo *oval_sysinfo_new(struct oval_syschar_model *model)
{
	oval_sysinfo_t *sysinfo = (oval_sysinfo_t *) oscap_alloc(sizeof(oval_sysinfo_t));
	if (sysinfo == NULL)
		return NULL;

	sysinfo->osArchitecture = NULL;
	sysinfo->osName = NULL;
	sysinfo->osVersion = NULL;
	sysinfo->primaryHostName = NULL;
	sysinfo->interfaces = oval_collection_new();
	sysinfo->model = model;
	sysinfo->anyxml = NULL;
	return sysinfo;
}

struct oval_sysinfo *oval_sysinfo_clone(struct oval_syschar_model *new_model, struct oval_sysinfo *old_sysinfo)
{
	struct oval_sysinfo *new_sysinfo = oval_sysinfo_new(new_model);
	struct oval_sysint_iterator *interfaces = oval_sysinfo_get_interfaces(old_sysinfo);
	while (oval_sysint_iterator_has_more(interfaces)) {
		struct oval_sysint *interface = oval_sysint_iterator_next(interfaces);
		oval_sysinfo_add_interface(new_sysinfo, interface);
	}
	oval_sysint_iterator_free(interfaces);

	char *os_architecture = oval_sysinfo_get_os_architecture(old_sysinfo);
	if (os_architecture)
		oval_sysinfo_set_os_architecture(new_sysinfo, os_architecture);
	char *os_name = oval_sysinfo_get_os_name(old_sysinfo);
	if (os_name)
		oval_sysinfo_set_os_name(new_sysinfo, os_name);
	char *os_version = oval_sysinfo_get_os_version(old_sysinfo);
	if (os_version)
		oval_sysinfo_set_os_version(new_sysinfo, os_version);
	char *host_name = oval_sysinfo_get_primary_host_name(old_sysinfo);
	if (host_name)
		oval_sysinfo_set_primary_host_name(new_sysinfo, host_name);

	if (old_sysinfo->anyxml)
		new_sysinfo->anyxml = oscap_strdup(old_sysinfo->anyxml);

	return new_sysinfo;
}

void oval_sysinfo_free(struct oval_sysinfo *sysinfo)
{
	if (sysinfo) {
		if (sysinfo->osArchitecture)
			oscap_free(sysinfo->osArchitecture);
		if (sysinfo->osName)
			oscap_free(sysinfo->osName);
		if (sysinfo->osVersion)
			oscap_free(sysinfo->osVersion);
		if (sysinfo->primaryHostName)
			oscap_free(sysinfo->primaryHostName);
		if (sysinfo->anyxml)
			oscap_free(sysinfo->anyxml);

		oval_collection_free_items(sysinfo->interfaces, (oscap_destruct_func) oval_sysint_free);

		sysinfo->interfaces = NULL;
		sysinfo->osArchitecture = NULL;
		sysinfo->osName = NULL;
		sysinfo->osVersion = NULL;
		sysinfo->primaryHostName = NULL;
		sysinfo->anyxml = NULL;

		oscap_free(sysinfo);
	}
}

bool oval_sysinfo_iterator_has_more(struct oval_sysinfo_iterator *oc_sysinfo)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysinfo);
}

struct oval_sysinfo *oval_sysinfo_iterator_next(struct oval_sysinfo_iterator
						*oc_sysinfo)
{
	return (struct oval_sysinfo *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysinfo);
}

void oval_sysinfo_iterator_free(struct oval_sysinfo_iterator *oc_sysinfo)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_sysinfo);
}

char *oval_sysinfo_get_os_name(struct oval_sysinfo *sysinfo)
{
	__attribute__nonnull__(sysinfo);

	return sysinfo->osName;
}

void oval_sysinfo_set_os_name(struct oval_sysinfo *sysinfo, char *osName)
{
	__attribute__nonnull__(sysinfo);

	if (sysinfo->osName != NULL)
		oscap_free(sysinfo->osName);
	sysinfo->osName = oscap_strdup(osName);
}

char *oval_sysinfo_get_os_version(struct oval_sysinfo *sysinfo)
{
	return sysinfo->osVersion;
}

void oval_sysinfo_set_os_version(struct oval_sysinfo *sysinfo, char *osVersion)
{
	__attribute__nonnull__(sysinfo);
	if (sysinfo->osVersion != NULL)
		oscap_free(sysinfo->osVersion);
	sysinfo->osVersion = oscap_strdup(osVersion);
}

char *oval_sysinfo_get_os_architecture(struct oval_sysinfo *sysinfo)
{
	return sysinfo->osArchitecture;
}

void oval_sysinfo_set_os_architecture(struct oval_sysinfo *sysinfo, char *osArchitecture)
{
	__attribute__nonnull__(sysinfo);
	if (sysinfo->osArchitecture != NULL)
		oscap_free(sysinfo->osArchitecture);
	sysinfo->osArchitecture = oscap_strdup(osArchitecture);
}

char *oval_sysinfo_get_primary_host_name(struct oval_sysinfo *sysinfo)
{
	__attribute__nonnull__(sysinfo);
	return sysinfo->primaryHostName;
}

void oval_sysinfo_set_primary_host_name(struct oval_sysinfo *sysinfo, char *primaryHostName)
{
	__attribute__nonnull__(sysinfo);
	if (sysinfo->primaryHostName != NULL)
		oscap_free(sysinfo->primaryHostName);
	sysinfo->primaryHostName = oscap_strdup(primaryHostName);
}

struct oval_sysint_iterator *oval_sysinfo_get_interfaces(struct oval_sysinfo
							 *sysinfo)
{
	__attribute__nonnull__(sysinfo);

	return (struct oval_sysint_iterator *)oval_collection_iterator(sysinfo->interfaces);
}

void oval_sysinfo_add_interface(struct oval_sysinfo *sysinfo, struct oval_sysint *interface)
{
	__attribute__nonnull__(sysinfo);
	oval_collection_add(sysinfo->interfaces, oval_sysint_clone(sysinfo->model, interface));
}

static void _oval_sysinfo_parse_tag_consume_os_name(char *text, void *sysinfo)
{
	oval_sysinfo_set_os_name(sysinfo, text);
}

static void _oval_sysinfo_parse_tag_consume_os_version(char *text, void *sysinfo)
{
	oval_sysinfo_set_os_version(sysinfo, text);
}

static void _oval_sysinfo_parse_tag_consume_os_architecture(char *text, void *sysinfo)
{
	oval_sysinfo_set_os_architecture(sysinfo, text);
}

static void _oval_sysinfo_parse_tag_consume_primary_host_name(char *text, void *sysinfo)
{
	oval_sysinfo_set_primary_host_name(sysinfo, text);
}

static void _oval_sysinfo_parse_tag_consume_int(struct oval_sysint *sysint, void *sysinfo)
{
	oval_sysinfo_add_interface(sysinfo, sysint);
	oval_sysint_free(sysint);
}

static int _oval_sysinfo_parse_tag_parse_tag(xmlTextReaderPtr reader,
					     struct oval_parser_context *context, void *sysinfo)
{
	return oval_sysint_parse_tag(reader, context, &_oval_sysinfo_parse_tag_consume_int, sysinfo);
}

static int _oval_sysinfo_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_sysinfo *sysinfo = (struct oval_sysinfo *)user;
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int return_code = 0;

	int is_ovalsys = strcmp((const char *)OVAL_SYSCHAR_NAMESPACE, namespace) == 0;
	if (is_ovalsys && (strcmp(tagname, "os_name") == 0)) {
		return_code = oval_parser_text_value(reader, context, &_oval_sysinfo_parse_tag_consume_os_name, sysinfo);
	} else if (is_ovalsys && (strcmp(tagname, "os_version") == 0)) {
		return_code = oval_parser_text_value(reader, context, &_oval_sysinfo_parse_tag_consume_os_version, sysinfo);
	} else if (is_ovalsys && (strcmp(tagname, "architecture") == 0)) {
		return_code = oval_parser_text_value(reader, context, &_oval_sysinfo_parse_tag_consume_os_architecture, sysinfo);
	} else if (is_ovalsys && (strcmp(tagname, "primary_host_name") == 0)) {
		return_code = oval_parser_text_value(reader, context, &_oval_sysinfo_parse_tag_consume_primary_host_name, sysinfo);
	} else if (is_ovalsys && (strcmp(tagname, "interfaces") == 0)) {
		return_code = oval_parser_parse_tag(reader, context, &_oval_sysinfo_parse_tag_parse_tag, sysinfo);
	} else {
                sysinfo->anyxml = (char *) xmlTextReaderReadOuterXml(reader);
                return_code = oval_parser_skip_tag(reader, context);
	}

	oscap_free(tagname);
	oscap_free(namespace);

	return return_code;
}

int oval_sysinfo_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context)
{
	__attribute__nonnull__(context);

	oval_sysinfo_t *sysinfo = oval_sysinfo_new(context->syschar_model);
	char *tagname = (char *)xmlTextReaderLocalName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int return_code=0;

	int is_ovalsys = strcmp((const char *)OVAL_SYSCHAR_NAMESPACE, namespace) == 0;
	if (is_ovalsys) {
		return_code = oval_parser_parse_tag(reader, context, &_oval_sysinfo_parse_tag, sysinfo);
	} else {
		dW("Expected <system_info>, got <%s:%s>", namespace, tagname);
		oval_parser_skip_tag(reader, context);
	}

	if (return_code != 0) {
		dW("Parsing of <%s> terminated by an error at line %d.", tagname, xmlTextReaderGetParserLineNumber(reader));
	}

	oval_syschar_model_set_sysinfo(context->syschar_model, sysinfo);

	oval_sysinfo_free(sysinfo);
	oscap_free(tagname);
	oscap_free(namespace);

	return return_code;
}

void oval_sysinfo_to_dom(struct oval_sysinfo *sysinfo, xmlDoc * doc, xmlNode * tag_parent)
{
        xmlNode *nodestr, *nodelst;
        xmlDoc  *docstr;
	int i;

	if (sysinfo) {
		xmlNs *ns_syschar = xmlSearchNsByHref(doc, tag_parent, OVAL_SYSCHAR_NAMESPACE);
		xmlNode *tag_sysinfo = xmlNewTextChild(tag_parent, ns_syschar, BAD_CAST "system_info", NULL);

		xmlNewTextChild(tag_sysinfo, ns_syschar, BAD_CAST "os_name", BAD_CAST oval_sysinfo_get_os_name(sysinfo));
		xmlNewTextChild(tag_sysinfo, ns_syschar, BAD_CAST "os_version", BAD_CAST oval_sysinfo_get_os_version(sysinfo));
		xmlNewTextChild(tag_sysinfo, ns_syschar, BAD_CAST "architecture", BAD_CAST oval_sysinfo_get_os_architecture(sysinfo));
		xmlNewTextChild(tag_sysinfo, ns_syschar, BAD_CAST "primary_host_name", BAD_CAST oval_sysinfo_get_primary_host_name(sysinfo));

		xmlNode *tag_interfaces = xmlNewTextChild(tag_sysinfo, ns_syschar, BAD_CAST "interfaces", NULL);
		struct oval_sysint_iterator *intrfcs = oval_sysinfo_get_interfaces(sysinfo);
		for (i = 1; oval_sysint_iterator_has_more(intrfcs); i++) {
			struct oval_sysint *intrfc = oval_sysint_iterator_next(intrfcs);
			oval_sysint_to_dom(intrfc, doc, tag_interfaces);
		}
		oval_sysint_iterator_free(intrfcs);

	        if (sysinfo->anyxml) {
			docstr = xmlReadDoc(BAD_CAST sysinfo->anyxml, NULL, NULL, 0);
			nodestr = xmlDocGetRootElement(docstr);

			nodelst = xmlDocCopyNode(nodestr, doc, 1);
			xmlAddChildList(tag_sysinfo, nodelst);
			xmlFreeDoc(docstr);
        	}
	}
}
