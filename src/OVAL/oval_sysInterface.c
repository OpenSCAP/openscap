/**
 * @file oval_sysInterface.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "oval_system_characteristics_impl.h"
#include "oval_collection_impl.h"
#include "../common/util.h"
#include "../common/public/debug.h"

typedef struct oval_sysint {
	struct oval_syschar_model *model;
	char *name;
	char *ipAddress;
	char *macAddress;
} oval_sysint_t;

bool oval_sysint_iterator_has_more(struct oval_sysint_iterator *oc_sysint)
{
	return oval_collection_iterator_has_more((struct oval_iterator *)
						 oc_sysint);
}

struct oval_sysint *oval_sysint_iterator_next(struct oval_sysint_iterator *oc_sysint)
{
	return (struct oval_sysint *)
	    oval_collection_iterator_next((struct oval_iterator *)oc_sysint);
}

void oval_sysint_iterator_free(struct oval_sysint_iterator *oc_sysint)
{
	oval_collection_iterator_free((struct oval_iterator *)oc_sysint);
}

char *oval_sysint_get_name(struct oval_sysint *sysint)
{
	__attribute__nonnull__(sysint);

	return sysint->name;
}

void oval_sysint_set_name(struct oval_sysint *sysint, char *name)
{
	if (sysint && !oval_sysint_is_locked(sysint)) {
		if (sysint->name != NULL)
			oscap_free(sysint->name);
		sysint->name = oscap_strdup(name);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

char *oval_sysint_get_ip_address(struct oval_sysint *sysint)
{
	__attribute__nonnull__(sysint);

	return ((struct oval_sysint *)sysint)->ipAddress;
}

void oval_sysint_set_ip_address(struct oval_sysint *sysint, char *ip_address)
{
	if (sysint && !oval_sysint_is_locked(sysint)) {
		if (sysint->ipAddress != NULL)
			oscap_free(sysint->ipAddress);
		sysint->ipAddress = oscap_strdup(ip_address);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

char *oval_sysint_get_mac_address(struct oval_sysint *sysint)
{
	return ((struct oval_sysint *)sysint)->macAddress;
}

void oval_sysint_set_mac_address(struct oval_sysint *sysint, char *mac_address)
{
	if (sysint && !oval_sysint_is_locked(sysint)) {
		if (sysint->macAddress != NULL)
			oscap_free(sysint->macAddress);
		sysint->macAddress = oscap_strdup(mac_address);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

struct oval_sysint *oval_sysint_new(struct oval_syschar_model *model)
{
	oval_sysint_t *sysint = (oval_sysint_t *) oscap_alloc(sizeof(oval_sysint_t));
	if (sysint == NULL)
		return NULL;

	sysint->ipAddress = NULL;
	sysint->macAddress = NULL;
	sysint->name = NULL;
	sysint->model = model;
	return sysint;
}

bool oval_sysint_is_valid(struct oval_sysint * sysint)
{
	return true;		//TODO
}

bool oval_sysint_is_locked(struct oval_sysint * sysint)
{
	__attribute__nonnull__(sysint);

	return oval_syschar_model_is_locked(sysint->model);
}

struct oval_sysint *oval_sysint_clone(struct oval_syschar_model *new_model, struct oval_sysint *old_sysint)
{
	struct oval_sysint *new_sysint = oval_sysint_new(new_model);
	char *ip_address = oval_sysint_get_ip_address(old_sysint);
	if (ip_address) {
		oval_sysint_set_ip_address(new_sysint, ip_address);
	}
	char *mac_address = oval_sysint_get_mac_address(old_sysint);
	if (mac_address) {
		oval_sysint_set_mac_address(new_sysint, mac_address);
	}
	char *name = oval_sysint_get_name(old_sysint);
	if (name) {
		oval_sysint_set_name(new_sysint, name);
	}
	return new_sysint;
}

void oval_sysint_free(struct oval_sysint *sysint)
{
	if (sysint == NULL)
		return;

	if (sysint->ipAddress != NULL)
		oscap_free(sysint->ipAddress);
	if (sysint->macAddress != NULL)
		oscap_free(sysint->macAddress);
	if (sysint->name != NULL)
		oscap_free(sysint->name);

	sysint->ipAddress = NULL;
	sysint->macAddress = NULL;
	sysint->name = NULL;

	oscap_free(sysint);
}

static void oval_consume_interface_name(char *text, void *sysint)
{
	oval_sysint_set_name(sysint, text);
}

static void oval_consume_ip_address(char *text, void *sysint)
{
	oval_sysint_set_ip_address(sysint, text);
}

static void oval_consume_mac_address(char *text, void *sysint)
{
	oval_sysint_set_mac_address(sysint, text);
}

static int _oval_sysint_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *user)
{
	struct oval_sysint *sysint = (struct oval_sysint *)user;
	char *tagname = (char *)xmlTextReaderName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int is_ovalsys = strcmp(namespace, NAMESPACE_OVALSYS) == 0;
	int return_code;
	if (is_ovalsys && (strcmp(tagname, "interface_name") == 0)) {
		return_code = oval_parser_text_value(reader, context, &oval_consume_interface_name, sysint);
	} else if (is_ovalsys && (strcmp(tagname, "ip_address") == 0)) {
		return_code = oval_parser_text_value(reader, context, &oval_consume_ip_address, sysint);
	} else if (is_ovalsys && (strcmp(tagname, "mac_address") == 0)) {
		return_code = oval_parser_text_value(reader, context, &oval_consume_mac_address, sysint);
	} else {
		oscap_dprintf("WARNING: _oval_sysint_parse_tag:: skipping <%s:%s>", namespace, tagname);
		return_code = oval_parser_skip_tag(reader, context);
	}
	oscap_free(tagname);
	oscap_free(namespace);
	return return_code;
}

int oval_sysint_parse_tag(xmlTextReaderPtr reader,
			  struct oval_parser_context *context, oval_sysint_consumer consumer, void *user)
{

	__attribute__nonnull__(context);

	struct oval_sysint *sysint = oval_sysint_new(context->syschar_model);
	char *tagname = (char *)xmlTextReaderName(reader);
	char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int is_ovalsys = strcmp(namespace, NAMESPACE_OVALSYS) == 0;
	int return_code;
	if (is_ovalsys && (strcmp(tagname, "interface") == 0)) {
		return_code = oval_parser_parse_tag(reader, context, &_oval_sysint_parse_tag, sysint);
	} else {
		oscap_dprintf("WARNING: oval_sysint_parse_tag:: expecting <interface> skipping <%s:%s>",
			      namespace, tagname);
		return_code = oval_parser_skip_tag(reader, context);
	}
	oscap_free(tagname);
	oscap_free(namespace);
	(*consumer) (sysint, user);
	return return_code;
}

void oval_sysint_to_print(struct oval_sysint *sysint, char *indent, int idx)
{
	char nxtindent[100];

	if (strlen(indent) > 80)
		indent = "....";

	if (idx == 0)
		snprintf(nxtindent, sizeof(nxtindent), "%sINTERFACE.", indent);
	else
		snprintf(nxtindent, sizeof(nxtindent), "%sINTERFACE[%d].", indent, idx);
	/*
	   char *name;
	   char *ipAddress;
	   char *macAddress;
	 */
	{			//name
		oscap_dprintf("%sNAME           = %s\n", nxtindent, oval_sysint_get_name(sysint));
	}
	char *ipadd = oval_sysint_get_ip_address(sysint);
	if (ipadd != NULL) {	//ipaddress
		oscap_dprintf("%sIP_ADDRESS      = %s\n", nxtindent, ipadd);
	}
	char *macadd = oval_sysint_get_mac_address(sysint);
	if (macadd != NULL) {	//mac address
		oscap_dprintf("%sMAC_ADDRESS     = %s\n", nxtindent, macadd);
	}
}

void oval_sysint_to_dom(struct oval_sysint *sysint, xmlDoc * doc, xmlNode * tag_parent)
{
	if (sysint) {
		xmlNs *ns_syschar = xmlSearchNsByHref(doc, tag_parent, OVAL_SYSCHAR_NAMESPACE);
		xmlNode *tag_sysint = xmlNewChild(tag_parent, ns_syschar, BAD_CAST "interface", NULL);
		xmlNewChild(tag_sysint, ns_syschar, BAD_CAST "interface_name", BAD_CAST oval_sysint_get_name(sysint));
		xmlNewChild(tag_sysint, ns_syschar, BAD_CAST "ip_address", BAD_CAST oval_sysint_get_ip_address(sysint));
		xmlNewChild
		    (tag_sysint, ns_syschar, BAD_CAST "mac_address", BAD_CAST oval_sysint_get_mac_address(sysint));
	}
}
