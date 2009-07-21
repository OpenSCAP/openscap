/**
 * @file cpedict.c
 * \brief Interface to Common Platform Enumeration (CPE) Dictionary.
 *
 * See more details at http://nvd.nist.gov/cpe.cfm
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libxml/tree.h>

#include "cpedict.h"
#include "../common/list.h"
#include "../common/util.h"

struct cpe_dict_check {
	char *system;      // system check URI
	char *href;        // external file reference (NULL if not present)
	char *identifier;  // test identifier
};


struct cpe_dict_reference {
	char *href;     // reference URL
	char *content;  // reference description
};


struct cpe_dictitem {

	struct cpe_name *name;		   // CPE name as CPE URI
	char *title;		           // human-readable name of this item

	struct cpe_name *depracated;   // CPE that depracated this one (or NULL)
	char *depracation_date;	       // date of depracation

	struct oscap_list* references; // list of references
	struct oscap_list* checks;     // list of checks
	struct oscap_list* notes;      // list of notes
};


struct cpe_dict {
	struct oscap_list* items;        // dictionary items

	char *generator_product_name;    // generator software name
	char *generator_product_version; // generator software version
	char *generator_schema_version;	 // generator schema version
	char *generator_timestamp;       // generation date and time
};


OSCAP_GETTER(const char*, cpe_dict_check, system)
OSCAP_GETTER(const char*, cpe_dict_check, href)
OSCAP_GETTER(const char*, cpe_dict_check, identifier)

OSCAP_GETTER(const char*, cpe_dict_reference, href)
OSCAP_GETTER(const char*, cpe_dict_reference, content)

OSCAP_GETTER(struct cpe_name*, cpe_dictitem, name)
OSCAP_GETTER(const char*, cpe_dictitem, title)
OSCAP_GETTER(struct cpe_name*, cpe_dictitem, depracated)
OSCAP_GETTER(const char*, cpe_dictitem, depracation_date)
OSCAP_IGETTER(oscap_string, cpe_dictitem, notes)
OSCAP_IGETTER_GEN(cpe_dict_reference, cpe_dictitem, references)
OSCAP_IGETTER_GEN(cpe_dict_check, cpe_dictitem, checks)

OSCAP_GETTER(const char*, cpe_dict, generator_product_name)
OSCAP_GETTER(const char*, cpe_dict, generator_product_version)
OSCAP_GETTER(const char*, cpe_dict, generator_schema_version)
OSCAP_GETTER(const char*, cpe_dict, generator_timestamp)
OSCAP_IGETTER_GEN(cpe_dictitem, cpe_dict, items)


/*
 * Load new CPE dictionary from XML node
 * @param node file name of dictionary to load
 * @return new dictionary
 * @retval NULL on failure
 */
struct cpe_dict *cpe_dict_new_xml(xmlNodePtr node);

/*
 * New dictionary item from XML
 * @param node cpe-item node
 * @return new dictionary item
 * @retval NULL on failure
 */
struct cpe_dictitem *cpe_dictitem_new_xml(xmlNodePtr node);

struct cpe_dict_check *cpe_dictcheck_new_xml(xmlNode * node);

struct cpe_dictitem *cpe_dictitem_new_empty();

void cpe_dictitem_delete(struct cpe_dictitem * item);

void cpe_dict_check_delete(struct cpe_dict_check * check);


char *str_trim(char *str)
{
	int off, i;
	if (str == NULL)
		return NULL;
	for (i = 0; isspace(str[i]); ++i) ;
	off = i;
	while (str[i]) {
		str[i - off] = str[i];
		++i;
	}
	for (i -= off; isspace(str[--i]) && i >= 0;) ;
	str[++i] = '\0';
	return str;
}

struct cpe_dict *cpe_dict_new(const char *fname)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	struct cpe_dict *ret;

	if ((doc = xmlParseFile(fname)) == NULL)
		return NULL;

	if ((root = xmlDocGetRootElement(doc)) == NULL)
		return NULL;

	ret = cpe_dict_new_xml(root);

	xmlFreeDoc(doc);

	return ret;
}

struct cpe_dict *cpe_dict_new_xml(xmlNodePtr node)
{
	struct cpe_dict *ret;
	struct cpe_dictitem *item;
	xmlNodePtr cur;

	if (xmlStrcmp(node->name, BAD_CAST "cpe-list") != 0)
		return NULL;

	if ((ret = cpe_dict_new_empty()) == NULL)
		return NULL;

	for (node = node->xmlChildrenNode; node != NULL; node = node->next) {
		if (xmlStrcmp(node->name, BAD_CAST "cpe-item") == 0) {
			if ((item = cpe_dictitem_new_xml(node)) == NULL)
				continue;
			if (!cpe_dict_add_item(ret, item)) {
				cpe_dictitem_delete(item);
				cpe_dict_delete(ret);
				return NULL;
			}
		} else if (xmlStrcmp(node->name, BAD_CAST "generator") == 0) {
			for (cur = node->xmlChildrenNode; cur != NULL;
			     cur = cur->next) {
				if (xmlStrcmp
				    (cur->name, BAD_CAST "product_name") == 0)
					ret->generator_product_name =
					    (char *)xmlNodeGetContent(cur);
				if (xmlStrcmp
				    (cur->name,
				     BAD_CAST "product_version") == 0)
					ret->generator_product_version =
					    (char *)xmlNodeGetContent(cur);
				if (xmlStrcmp
				    (cur->name, BAD_CAST "schema_version") == 0)
					ret->generator_schema_version =
					    (char *)xmlNodeGetContent(cur);
				if (xmlStrcmp(cur->name, BAD_CAST "timestamp")
				    == 0)
					ret->generator_timestamp =
					    (char *)xmlNodeGetContent(cur);
			}
		}
	}

	return ret;
}

const size_t CPE_DICT_CPES_INITIAL_ALLOC = 8;

struct cpe_dict *cpe_dict_new_empty(void)
{
	struct cpe_dict *dict;

	dict = oscap_alloc(sizeof(struct cpe_dict));
	if (dict == NULL)
		return NULL;

	memset(dict, 0, sizeof(struct cpe_dict));
	dict->items = oscap_list_new();
	return dict;
}

bool cpe_dict_add_item(struct cpe_dict * dict, struct cpe_dictitem * item)
{
	if (dict == NULL || item == NULL)
		return false;

	oscap_list_add(dict->items, item);
	return true;
}

void cpe_dict_delete(struct cpe_dict * dict)
{
	if (dict == NULL) return;

	oscap_list_delete(dict->items, (oscap_destruct_func)cpe_dictitem_delete);
	oscap_free(dict->generator_product_name);
	oscap_free(dict->generator_product_version);
	oscap_free(dict->generator_schema_version);
	oscap_free(dict->generator_timestamp);
	oscap_free(dict);
}

struct cpe_dictitem *cpe_dictitem_new_empty()
{
	struct cpe_dictitem *item;

	item = oscap_alloc(sizeof(struct cpe_dictitem));
	if (item == NULL)
		return NULL;

	memset(item, 0, sizeof(struct cpe_dictitem));
	item->notes      = oscap_list_new();
	item->references = oscap_list_new();
	item->checks     = oscap_list_new();

	return item;
}

struct cpe_dictitem *cpe_dictitem_new_xml(xmlNodePtr node)
{
	struct cpe_dictitem *item;
	struct cpe_dict_check *check;
	xmlNodePtr cur;
	xmlChar *data;
	struct cpe_dict_reference *pref;

	if (xmlStrcmp(node->name, BAD_CAST "cpe-item") != 0)
		return NULL;

	item = cpe_dictitem_new_empty();
	if (item == NULL)
		return NULL;

	data = xmlGetProp(node, BAD_CAST "name");
	if (data == NULL || (item->name = cpe_name_new((char *)data)) == NULL) {
		oscap_free(item);
		oscap_free(data);
		return NULL;
	}
	oscap_free(data);

	for (node = node->xmlChildrenNode; node != NULL; node = node->next) {
		if (item->title == NULL
		    && xmlStrcmp(node->name, BAD_CAST "title") == 0)
			item->title = str_trim((char *)xmlNodeGetContent(node));
		else if (xmlStrcmp(node->name, BAD_CAST "notes") == 0) {
			for (cur = node->xmlChildrenNode; cur != NULL;
			     cur = cur->next) {
				if (xmlStrcmp(cur->name, BAD_CAST "note") != 0)
					continue;
				data =
				    BAD_CAST str_trim((char *)
						      xmlNodeGetContent(cur));
				if (data) oscap_list_add(item->notes, data);
			}
		} else if (xmlStrcmp(node->name, BAD_CAST "check") == 0) {
			check = cpe_dictcheck_new_xml(node);
			if (check) oscap_list_add(item->checks, check);
		} else if (xmlStrcmp(node->name, BAD_CAST "references") == 0) {
			for (cur = node->xmlChildrenNode; cur != NULL;
			     cur = cur->next) {
				if (xmlStrcmp(cur->name, BAD_CAST "reference") != 0) continue;
				pref = oscap_calloc(1, sizeof(struct cpe_dict_reference));
				pref->content = str_trim((char *)xmlNodeGetContent(cur));
				pref->href = (char *)xmlGetProp(cur, BAD_CAST "href");
				oscap_list_add(item->references, pref);
			}
		}
	}

	return item;
}

void cpe_dict_reference_delete(struct cpe_dict_reference* ref)
{
	if (ref) {
		oscap_free(ref->href);
		oscap_free(ref->content);
		oscap_free(ref);
	}
}

void cpe_dictcheck_delete(struct cpe_dict_check * check);

void cpe_dictitem_delete(struct cpe_dictitem * item)
{
	if (item == NULL) return;
	cpe_name_delete(item->name);
	oscap_free(item->title);
	cpe_name_delete(item->depracated);
	oscap_free(item->depracation_date);
	oscap_list_delete(item->references, (oscap_destruct_func)cpe_dict_reference_delete);
	oscap_list_delete(item->checks, (oscap_destruct_func)cpe_dictcheck_delete);
	oscap_list_delete(item->notes, oscap_free);
	oscap_free(item);
}

struct cpe_dict_check *cpe_dictcheck_new_xml(xmlNode * node)
{
	xmlChar *data;
	struct cpe_dict_check *check;

	if (xmlStrcmp(node->name, BAD_CAST "check") != 0)
		return NULL;

	if ((check = oscap_alloc(sizeof(struct cpe_dict_check))) == NULL)
		return NULL;
	memset(check, 0, sizeof(struct cpe_dict_check));

	data = xmlGetProp(node, BAD_CAST "system");
	if (data)
		check->system = (char *)data;

	data = xmlGetProp(node, BAD_CAST "href");
	if (data)
		check->href = (char *)data;

	check->identifier = str_trim((char *)xmlNodeGetContent(node));

	return check;
}

void cpe_dictcheck_delete(struct cpe_dict_check * check)
{
	if (check == NULL)
		return;
	oscap_free(check->identifier);
	oscap_free(check->system);
	oscap_free(check->href);
	oscap_free(check);
}

bool cpe_name_match_dict(struct cpe_name * cpe, struct cpe_dict * dict)
{
	if (cpe == NULL || dict == NULL)
		return false;
	
	size_t n = dict->items->itemcount;
	struct cpe_name** cpes = oscap_alloc(sizeof(struct cpe_name*) * n);
	struct oscap_list_item* cur = dict->items->first;

	for (int i = 0; cur != NULL; ++i) {
		cpes[i] = ((struct cpe_dictitem*)cur->data)->name;
		cur = cur->next;
	}
	
	bool ret = cpe_name_match_cpes(cpe, n, cpes);

	oscap_free(cpes);

	return ret;
}

bool cpe_name_match_dict_str(const char *cpestr, struct cpe_dict * dict)
{
	bool ret;
	if (cpestr == NULL)
		return false;
	struct cpe_name *cpe = cpe_name_new(cpestr);
	if (cpe == NULL) return false;
	ret = cpe_name_match_dict(cpe, dict);
	cpe_name_delete(cpe);
	return ret;
}
