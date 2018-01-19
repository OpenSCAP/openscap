/**
 * @file oval_generator.c
 * @brief OVAL generator data type implementation
 * @author "Tomas Heinrich" <theinric@redhat.com>
 */

/*
 * Copyright 2010--2013 Red Hat Inc., Durham, North Carolina.
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
 *      "Tomas Heinrich" <theinric@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifndef _WIN32
#include <sys/time.h>
#endif

#include "common/util.h"
#include "common/debug_priv.h"
#include "oval_agent_api_impl.h"
#include "oval_definitions_impl.h"
#include "common/list.h"

struct oval_generator {
	char *product_name;
	char *product_version;
	char *core_schema_version;
	struct oscap_htable *platform_schema_versions;
	char *timestamp;
	char *anyxml;
};

struct oval_generator *oval_generator_new(void)
{
	struct oval_generator *gen;
	gen = malloc(sizeof(struct oval_generator));
	gen->product_name = NULL;
	gen->product_version = NULL;
	gen->core_schema_version = oscap_strdup(OVAL_SUPPORTED);
	gen->platform_schema_versions = oscap_htable_new();
	gen->anyxml = NULL;
	gen->timestamp = NULL;
	oval_generator_update_timestamp(gen);
	return gen;
}

void oval_generator_free(struct oval_generator *generator)
{
	free(generator->product_name);
	free(generator->product_version);
	free(generator->core_schema_version);
	oscap_htable_free(generator->platform_schema_versions,
		(oscap_destruct_func) free);
	free(generator->timestamp);
	free(generator->anyxml);
	free(generator);
}

struct oval_generator *oval_generator_clone(struct oval_generator *old_generator)
{
	struct oval_generator *new_gen;

	new_gen = malloc(sizeof(*new_gen));
	new_gen->product_name = oscap_strdup(old_generator->product_name);
	new_gen->product_version = oscap_strdup(old_generator->product_version);
	new_gen->core_schema_version = oscap_strdup(old_generator->core_schema_version);
	new_gen->platform_schema_versions = oscap_htable_clone(
		old_generator->platform_schema_versions, (oscap_clone_func) oscap_strdup);
	new_gen->timestamp = oscap_strdup(old_generator->timestamp);
	new_gen->anyxml = oscap_strdup(old_generator->anyxml);

	return new_gen;
}

char *oval_generator_get_product_name(struct oval_generator *generator)
{
	return generator->product_name;
}

char *oval_generator_get_product_version(struct oval_generator *generator)
{
	return generator->product_version;
}

char *oval_generator_get_schema_version(struct oval_generator *generator)
{
	/* Removed a const type because we can't change API of this function. */
	return (char *) oval_generator_get_core_schema_version(generator);
}

const char *oval_generator_get_core_schema_version(struct oval_generator *generator)
{
	return generator->core_schema_version;
}

char *oval_generator_get_timestamp(struct oval_generator *generator)
{
	return generator->timestamp;
}

const char *oval_generator_get_platform_schema_version (struct oval_generator *generator, const char *platform)
{
	char *platform_schema_version = oscap_htable_get(generator->platform_schema_versions, platform);
	if (platform_schema_version != NULL) {
		return platform_schema_version;
	} else {
		return generator->core_schema_version;
	}
}

void oval_generator_set_product_name(struct oval_generator *generator, const char *product_name)
{
	free(generator->product_name);
	generator->product_name = oscap_strdup(product_name);
}

void oval_generator_set_product_version(struct oval_generator *generator, const char *product_version)
{
	free(generator->product_version);
	generator->product_version = oscap_strdup(product_version);
}

void oval_generator_set_schema_version(struct oval_generator *generator, const char *schema_version)
{
	oval_generator_set_core_schema_version(generator, schema_version);
}

void oval_generator_set_core_schema_version(struct oval_generator *generator, const char *schema_version)
{
	free(generator->core_schema_version);
	generator->core_schema_version = oscap_strdup(schema_version);
}

void oval_generator_set_timestamp(struct oval_generator *generator, const char *timestamp)
{
	free(generator->timestamp);
	generator->timestamp = oscap_strdup(timestamp);
}

void oval_generator_update_timestamp(struct oval_generator *generator)
{
	time_t et;
	struct tm *lt;
	char timestamp[] = "yyyy-mm-ddThh:mm:ss";

	time(&et);
	lt = localtime(&et);
	snprintf(timestamp, sizeof(timestamp), "%4d-%02d-%02dT%02d:%02d:%02d",
		 1900 + lt->tm_year, 1 + lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
	oval_generator_set_timestamp(generator, timestamp);
}

void oval_generator_add_platform_schema_version(struct oval_generator *generator, const char *platform, const char *schema_version)
{
	oscap_htable_add(generator->platform_schema_versions, platform, oscap_strdup(schema_version));
}


xmlNode *oval_generator_to_dom(struct oval_generator *generator, xmlDocPtr doc, xmlNode *parent)
{
	struct oscap_htable_iterator *sv_itr;
	xmlNode *gen_node;
	xmlNs *ns_common;
	xmlNode *nodestr, *nodelst;
	xmlDoc  *docstr;

	gen_node = xmlNewTextChild(parent, NULL, BAD_CAST "generator", NULL);
	ns_common = xmlSearchNsByHref(doc, parent, OVAL_COMMON_NAMESPACE);
	if (generator->product_name)
		xmlNewTextChild(gen_node, ns_common, BAD_CAST "product_name", BAD_CAST generator->product_name);
	if (generator->product_version)
		xmlNewTextChild(gen_node, ns_common, BAD_CAST "product_version", BAD_CAST generator->product_version);
	if (generator->core_schema_version)
		xmlNewTextChild(gen_node, ns_common, BAD_CAST "schema_version", BAD_CAST generator->core_schema_version);

	const char *namespace_uri = "http://oval.mitre.org/XMLSchema/oval-definitions-5";
	sv_itr = oscap_htable_iterator_new(generator->platform_schema_versions);
	while (oscap_htable_iterator_has_more(sv_itr)) {
		const char *platform, *version;
		oscap_htable_iterator_next_kv(sv_itr, &platform, (void **) &version);
		xmlNode *sv_node = xmlNewTextChild(gen_node, ns_common,
			BAD_CAST "schema_version", BAD_CAST version);
		size_t namespace_uri_length = strlen(namespace_uri) + 1 + strlen(platform) + 1;
		char *platform_uri = malloc(namespace_uri_length);
		snprintf(platform_uri, namespace_uri_length, "%s#%s", namespace_uri, platform);
		xmlNewProp(sv_node, BAD_CAST "platform", BAD_CAST platform_uri);
		free(platform_uri);
	}
	oscap_htable_iterator_free(sv_itr);

	if (generator->timestamp)
		xmlNewTextChild(gen_node, ns_common, BAD_CAST "timestamp", BAD_CAST generator->timestamp);

	if (generator->anyxml) {
		docstr = xmlReadDoc(BAD_CAST generator->anyxml, NULL, NULL, 0);
        	nodestr = xmlDocGetRootElement(docstr);

	        nodelst = xmlDocCopyNode(nodestr, doc, 1);
        	xmlAddChildList(gen_node, nodelst);
	        xmlFreeDoc(docstr);
	}

	return gen_node;
}

int oval_generator_parse_tag(xmlTextReader *reader, struct oval_parser_context *context, void *user)
{
	char *tagname, *namespace, *val = NULL;
	struct oval_generator *gen = user;
	int ret=0;

	tagname = (char *) xmlTextReaderLocalName(reader);
	namespace = (char *) xmlTextReaderNamespaceUri(reader);

	if (!strcmp("product_name", tagname)) {
		xmlTextReaderRead(reader);
		val = (char *) xmlTextReaderValue(reader);
		oval_generator_set_product_name(gen, val);
	} else if (!strcmp("product_version", tagname)) {
		xmlTextReaderRead(reader);
		val = (char *) xmlTextReaderValue(reader);
		oval_generator_set_product_version(gen, val);
	} else if (!strcmp("schema_version", tagname)) {
		char *platform = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "platform");
		xmlTextReaderRead(reader);
		val = (char *) xmlTextReaderValue(reader);
		if (platform != NULL) {
			char *platform_name = strrchr(platform, '#');
			if (platform_name != NULL) {
				platform_name++;
				oval_generator_add_platform_schema_version(gen, platform_name, val);
			}
			free(platform);
		} else {
			oval_generator_set_core_schema_version(gen, val);
		}
	} else if (!strcmp("timestamp", tagname)) {
		xmlTextReaderRead(reader);
		val = (char *) xmlTextReaderValue(reader);
		oval_generator_set_timestamp(gen, val);
	} else {
		gen->anyxml = (char *) xmlTextReaderReadOuterXml(reader);
		ret = oval_parser_skip_tag(reader, context);
	}

	free(tagname);
	free(namespace);
	free(val);

	return ret;
}

