/**
 * @file cpedict.c
 * \brief Interface to Common Platform Enumeration (CPE) Dictionary.
 *
 * See more details at http://nvd.nist.gov/cpe.cfm
 */

/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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
 *      Maros Barabas <mbarabas@redhat.com>
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "public/cpe_dict.h"
#include "cpedict_priv.h"

#include "common/list.h"
#include "common/util.h"
#include "common/_error.h"
#include "common/xmlns_priv.h"
#include "common/elements.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"
#include <string.h>

#define CPE_DICT_SUPPORTED "2.3"

struct cpe_dict_model *cpe_dict_model_import(const char *file)
{
	__attribute__nonnull__(file);

	if (file == NULL)
		return NULL;

	struct cpe_dict_model *dict;

	if ((dict = cpe_dict_model_parse_xml(file)) == NULL)
		return NULL;

	dict->origin_file = oscap_strdup(file);

	return dict;
}

bool cpe_dict_model_set_origin_file(struct cpe_dict_model* dict, const char* origin_file)
{
	oscap_free(dict->origin_file);
	dict->origin_file = oscap_strdup(origin_file);

	return true;
}

const char* cpe_dict_model_get_origin_file(const struct cpe_dict_model* dict)
{
	return dict->origin_file;
}

void cpe_dict_model_export(const struct cpe_dict_model *dict, const char *file)
{

	__attribute__nonnull__(dict);
	__attribute__nonnull__(file);

	if (file == NULL)
		return;

	cpe_dict_model_export_xml(dict, file);

}

bool cpe_name_match_dict(struct cpe_name * cpe, struct cpe_dict_model * dict)
{
	// FIXME: We could match faster by matching per component and storing all
	//        cpe_dict items in a big tree where leaves contain cpe_items and
	//        inner nodes are the components.

	__attribute__nonnull__(cpe);
	__attribute__nonnull__(dict);

	if (cpe == NULL || dict == NULL)
		return false;

	struct cpe_item_iterator *items = cpe_dict_model_get_items(dict);

	bool ret = false;
	while (cpe_item_iterator_has_more(items)) {
		struct cpe_item* item = cpe_item_iterator_next(items);
		struct cpe_name* name = cpe_item_get_name(item);

		if (cpe_name_match_one(name, cpe)) {
			ret = true;
			break;
		}
	}
	cpe_item_iterator_free(items);
	return ret;
}

bool cpe_name_match_dict_str(const char *cpestr, struct cpe_dict_model * dict)
{
	__attribute__nonnull__(cpestr);
	__attribute__nonnull__(dict);

	bool ret;
	if (cpestr == NULL)
		return false;
	struct cpe_name *cpe = cpe_name_new(cpestr);
	if (cpe == NULL)
		return false;
	ret = cpe_name_match_dict(cpe, dict);
	cpe_name_free(cpe);
	return ret;
}

bool cpe_name_applicable_dict(struct cpe_name *cpe, struct cpe_dict_model *dict, cpe_check_fn cb, void* usr)
{
	// FIXME: We could match faster by matching per component and storing all
	//        cpe_dict items in a big tree where leaves contain cpe_items and
	//        inner nodes are the components.

	__attribute__nonnull__(cpe);
	__attribute__nonnull__(dict);

	if (cpe == NULL || dict == NULL)
		return false;

	struct cpe_item_iterator *items = cpe_dict_model_get_items(dict);

	// essentially, we want at least one applicable match so as soon as we find
	// a match we break and return true

	bool ret = false;
	while (cpe_item_iterator_has_more(items)) {
		struct cpe_item* item = cpe_item_iterator_next(items);
		struct cpe_name* name = cpe_item_get_name(item);

		if (cpe_name_match_one(cpe, name)) {
			if (cpe_item_is_applicable(item, cb, usr)) {
				ret = true;
				break;
			}
		}
	}
	cpe_item_iterator_free(items);
	return ret;
}

static bool cpe_check_evaluate(const struct cpe_check* check, cpe_check_fn cb, void* usr)
{
	const char* sys = cpe_check_get_system(check);
	const char* href = cpe_check_get_href(check);
	const char* name = cpe_check_get_identifier(check);

	return cb(sys, href, name, usr);
}

bool cpe_item_is_applicable(struct cpe_item* item, cpe_check_fn cb, void* usr)
{
	struct cpe_check_iterator* checks = cpe_item_get_checks(item);

	bool ret = false;
	while (cpe_check_iterator_has_more(checks)) {
		struct cpe_check* check = cpe_check_iterator_next(checks);
		if (cpe_check_evaluate(check, cb, usr)) {
			ret = true;
			break;
		}
	}
	cpe_check_iterator_free(checks);

	return ret;
}

const char * cpe_dict_model_supported(void) 
{
        return CPE_DICT_SUPPORTED;
}

char *cpe_dict_detect_version_priv(xmlTextReader *reader)
{
	char *version = NULL;
	/* find root element */
	while (xmlTextReaderRead(reader) == 1
	       && xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);

	const char* elm_name = (const char *) xmlTextReaderConstLocalName(reader);
	if (!elm_name || strcmp("cpe-list", elm_name)) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Expected root element name 'cpe-list', found '%s'.", elm_name);
		xmlFreeTextReader(reader);
		return NULL;
	}
	const char* ns_uri = (const char *) xmlTextReaderConstNamespaceUri(reader);

	/* find generator */
	while (xmlTextReaderRead(reader) == 1
		   && xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);
	elm_name = (const char *) xmlTextReaderConstLocalName(reader);

	// if the element found is a generator, use it to figure out the version
	if (elm_name && strcmp(elm_name, "generator") == 0) {
		/* find schema_version */
		const int depth = xmlTextReaderDepth(reader);
		while (xmlTextReaderRead(reader) == 1 && xmlTextReaderDepth(reader) > depth) {
			if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT)
				continue;

			elm_name = (const char *) xmlTextReaderConstLocalName(reader);
			if (!strcmp(elm_name, "schema_version")) {
				version = oscap_element_string_copy(reader);
				break;
			}
		}
	}
	else {
		if (strcmp(ns_uri, XMLNS_CPE2D) == 0) {
			// return the newest 2.x version we support
			version = oscap_strdup("2.3");
		}
	}
	return version;
}

char * cpe_dict_detect_version(const char* file)
{
	char *version = NULL;
	struct oscap_source *source = oscap_source_new_from_file(file);
	xmlTextReaderPtr reader = oscap_source_get_xmlTextReader(source);
	if (reader != NULL) {
		version = cpe_dict_detect_version_priv(reader);
	}
	xmlFreeTextReader(reader);
	oscap_source_free(source);
	return version;
}
