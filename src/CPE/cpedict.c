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


// cpe_element_metadata -> meta structure for every XML element
// with all information that are common as "XML namespace"
struct xml_metadata {
        char *namespace;
        char *lang;
};

struct cpe_item_metadata {
        char *modification_date;
        char *status;
        char *nvd_id;
};

struct cpe_dict_check {
	char *system;      // system check URI
	char *href;        // external file reference (NULL if not present)
	char *identifier;  // test identifier
};


struct cpe_dict_reference {
	char *href;     // reference URL
	char *content;  // reference description
};

struct cpe_dictitem_title {
	char *content;		        // human-readable name of this item
        char *xmllang;                  // property xml:lang of title
};

struct cpe_dictitem {                   // the node <cpe-item>

	struct cpe_name *name;		// CPE name as CPE URI
        struct oscap_list* titles;      // titles of cpe-item (can be in various languages)

	struct cpe_name *deprecated;   // CPE that deprecated this one (or NULL)
	char *deprecation_date;	       // date of deprecation

	struct oscap_list* references; // list of references
	struct oscap_list* checks;     // list of checks
	struct oscap_list* notes;      // list of notesa
        struct cpe_item_metadata *metadata;    // element <meta:item-metadata>
};


struct cpe_dict {                        // the main node
	struct oscap_list* items;        // dictionary items
        struct oscap_list* vendors;

	char *generator_product_name;    // generator software name
	char *generator_product_version; // generator software version
	char *generator_schema_version;	 // generator schema version
	char *generator_timestamp;       // generation date and time
};


/* **************************
 * Component-tree structures
 * **************************/
struct cpe_dict_edition {
        char *value;
};

struct cpe_dict_update {
        char *value;
        struct oscap_list* editions;
};

struct cpe_dict_version {
        char *value;
        struct oscap_list* updates;
};

struct cpe_dict_product {
        char *value;
        cpe_part_t part;                // enum with "h" or "a" or "o", see cpeuri
        struct oscap_list* versions;

};

struct cpe_dict_vendor {
        char *value;
        struct oscap_list* titles;
        struct oscap_list* products;
};
/****************************/

const char *PART_TO_CHAR[] = { NULL, "h", "o", "a" };

/*
 * OSCAP_GETTER(      <return value>, <structure name>, <value of structure>)
 * OSCAP_IGETTER_GEN( <return value>, <structure name>, <oscap_list *>)
 */

OSCAP_GETTER(const char*, cpe_dict_check, system)
OSCAP_GETTER(const char*, cpe_dict_check, href)
OSCAP_GETTER(const char*, cpe_dict_check, identifier)

OSCAP_GETTER(const char*, cpe_dict_reference, href)
OSCAP_GETTER(const char*, cpe_dict_reference, content)

OSCAP_GETTER(const char*, cpe_dictitem_title, content)
OSCAP_GETTER(const char*, cpe_dictitem_title, xmllang)

OSCAP_GETTER(const char*, cpe_item_metadata, modification_date)
OSCAP_GETTER(const char*, cpe_item_metadata, status)
OSCAP_GETTER(const char*, cpe_item_metadata, nvd_id)

OSCAP_GETTER(struct cpe_name*, cpe_dictitem, name)
OSCAP_GETTER(struct cpe_name*, cpe_dictitem, deprecated)
OSCAP_GETTER(const char*, cpe_dictitem, deprecation_date)
OSCAP_IGETTER(oscap_string, cpe_dictitem, notes)
OSCAP_IGETTER_GEN(cpe_dict_reference, cpe_dictitem, references)
OSCAP_IGETTER_GEN(cpe_dict_check, cpe_dictitem, checks)
OSCAP_IGETTER_GEN(cpe_dictitem_title, cpe_dictitem, titles)
OSCAP_GETTER(struct cpe_item_metadata*, cpe_dictitem, metadata)

// for cpe_dict_[vendor, product, version, update, edition]_get_*
OSCAP_GETTER(const char*, cpe_dict_vendor, value)
OSCAP_GETTER(const char*, cpe_dict_product, value)
OSCAP_GETTER(int, cpe_dict_product, part)
OSCAP_GETTER(const char*, cpe_dict_version, value)
OSCAP_GETTER(const char*, cpe_dict_update, value)
OSCAP_GETTER(const char*, cpe_dict_edition, value)
OSCAP_IGETTER(cpe_dictitem_title, cpe_dict_vendor, titles)
OSCAP_IGETTER_GEN(cpe_dict_product, cpe_dict_vendor, products)
OSCAP_IGETTER_GEN(cpe_dict_version, cpe_dict_product, versions)
OSCAP_IGETTER_GEN(cpe_dict_update, cpe_dict_version, updates)
OSCAP_IGETTER_GEN(cpe_dict_edition, cpe_dict_update, editions)

OSCAP_GETTER(const char*, cpe_dict, generator_product_name)
OSCAP_GETTER(const char*, cpe_dict, generator_product_version)
OSCAP_GETTER(const char*, cpe_dict, generator_schema_version)
OSCAP_GETTER(const char*, cpe_dict, generator_timestamp)
OSCAP_IGETTER_GEN(cpe_dictitem, cpe_dict, items)
OSCAP_IGETTER_GEN(cpe_dict_vendor, cpe_dict, vendors)

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

struct cpe_dictitem_title *cpe_dictitemtitle_new_xml(xmlNode * node);

struct cpe_dict_vendor *cpe_dictvendor_new_xml(xmlNodePtr node);
struct cpe_dict_product *cpe_dictproduct_new_xml(xmlNodePtr node);
struct cpe_dict_version *cpe_dictversion_new_xml(xmlNodePtr node);
struct cpe_dict_update *cpe_dictupdate_new_xml(xmlNodePtr node);
struct cpe_dict_edition *cpe_dictedition_new_xml(xmlNodePtr node);

struct cpe_dictitem *cpe_dictitem_new_empty();

void cpe_dictitem_free(struct cpe_dictitem * item);

void cpe_dict_check_free(struct cpe_dict_check * check);

/* Declarations for private header
 */
void cpe_dictvendor_free(struct cpe_dict_vendor * vendor);
void cpe_dictproduct_free(struct cpe_dict_product * product);
void cpe_dictversion_free(struct cpe_dict_version * version);
void cpe_dictupdate_free(struct cpe_dict_update * update);
void cpe_dictedition_free(struct cpe_dict_edition * edition);

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
        struct cpe_dict_vendor *vendor;
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
				cpe_dictitem_free(item);
				cpe_dict_free(ret);
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
		} else if (xmlStrcmp(node->name, BAD_CAST "component-tree") == 0 ) {
			for (cur = node->xmlChildrenNode; cur != NULL;
			     cur = cur->next) {
				if (xmlStrcmp(cur->name, BAD_CAST "vendor") != 0) continue;
			        vendor = cpe_dictvendor_new_xml(cur);
			        if (vendor) oscap_list_add(ret->vendors, vendor);
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
        dict->vendors   = oscap_list_new();
	dict->items     = oscap_list_new();
	return dict;
}

bool cpe_dict_add_item(struct cpe_dict * dict, struct cpe_dictitem * item)
{
	if (dict == NULL || item == NULL)
		return false;

	oscap_list_add(dict->items, item);
	return true;
}

void cpe_dict_free(struct cpe_dict * dict)
{
	if (dict == NULL) return;

	oscap_list_free(dict->items, (oscap_destruct_func)cpe_dictitem_free);
	oscap_list_free(dict->vendors, (oscap_destruct_func)cpe_dictvendor_free);
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
        item->titles     = oscap_list_new();

	return item;
}

struct cpe_dictitem *cpe_dictitem_new_xml(xmlNodePtr node)
{
	struct cpe_dictitem *item;
	struct cpe_dict_check *check;
        struct cpe_dictitem_title *title;
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
        data = xmlGetProp(node, BAD_CAST "deprecated");
        if (data != NULL) { // we have deprecation on this item !
	        oscap_free(data);
                data = xmlGetProp(node, BAD_CAST "deprecated_by");
	        if (data == NULL || (item->deprecated = cpe_name_new((char *)data)) == NULL) {
		        oscap_free(item);
		        oscap_free(data);
		        return NULL;
                }
	        oscap_free(data);
                data = xmlGetProp(node, BAD_CAST "deprecation_date");
	        if (data == NULL || (item->deprecation_date = malloc(strlen((char *)data)+1)) == NULL) {
		        oscap_free(item);
		        oscap_free(data);
		        return NULL;
                }
                strcpy(item->deprecation_date, (char *)data);
        }
        oscap_free(data);

	for (node = node->xmlChildrenNode; node != NULL; node = node->next) {
		    if(xmlStrcmp(node->name, BAD_CAST "title") == 0) {
                        title = cpe_dictitemtitle_new_xml(node);
                        if (title) oscap_list_add(item->titles, title);
                }
		else if (xmlStrcmp(node->name, BAD_CAST "notes") == 0) {
			for (cur = node->xmlChildrenNode; cur != NULL;
			     cur = cur->next) {
				if (xmlStrcmp(cur->name, BAD_CAST "note") != 0)
					continue;
				data =
				    BAD_CAST str_trim((char *)
						      xmlNodeGetContent(cur));
				if (data) oscap_list_add(item->notes, data);
                                oscap_free(data);
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
		} else if (xmlStrcmp(node->name, BAD_CAST "item-metadata") == 0) {
                        data = xmlGetProp(node, BAD_CAST "modification-date");
                        if (((item->metadata = oscap_alloc(sizeof(struct cpe_item_metadata))) == NULL) || (data == NULL)){
                            oscap_free(item);
                            oscap_free(data);
                            return NULL;
                        }
                        if (data) item->metadata->modification_date = (char *)data;
                        data = xmlGetProp(node, BAD_CAST "status");
	                if (data) item->metadata->status = (char *)data;
                        data = xmlGetProp(node, BAD_CAST "nvd-id");
	                if (data) item->metadata->nvd_id = (char *)data;
                        data = NULL;
                }
	}

	return item;
}

void cpe_dict_reference_free(struct cpe_dict_reference* ref)
{
	if (ref) {
		oscap_free(ref->href);
		oscap_free(ref->content);
		oscap_free(ref);
	}
}

void cpe_dictcheck_free(struct cpe_dict_check * check);
void cpe_dictitemtitle_free(struct cpe_dictitem_title * title);

void cpe_dictitem_free(struct cpe_dictitem * item)
{
	if (item == NULL) return;
	cpe_name_free(item->name);
	cpe_name_free(item->deprecated);
	oscap_free(item->deprecation_date);
	oscap_list_free(item->references, (oscap_destruct_func)cpe_dict_reference_free);
	oscap_list_free(item->checks, (oscap_destruct_func)cpe_dictcheck_free);
	oscap_list_free(item->notes, oscap_free);
        oscap_list_free(item->titles, (oscap_destruct_func)cpe_dictitemtitle_free);
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

void cpe_dictcheck_free(struct cpe_dict_check * check)
{
	if (check == NULL)
		return;
	oscap_free(check->identifier);
	oscap_free(check->system);
	oscap_free(check->href);
	oscap_free(check);
}

struct cpe_dictitem_title *cpe_dictitemtitle_new_xml(xmlNode * node)
{
	xmlChar *data;
	struct cpe_dictitem_title *title;

	if (xmlStrcmp(node->name, BAD_CAST "title") != 0)
		return NULL;

	if ((title = oscap_alloc(sizeof(struct cpe_dictitem_title))) == NULL)
		return NULL;
	memset(title, 0, sizeof(struct cpe_dictitem_title));

	data = xmlGetProp(node, BAD_CAST "lang");
	if (data)
		title->xmllang = (char *)data;

	title->content = str_trim((char *)xmlNodeGetContent(node));

	return title;
}

void cpe_dictitemtitle_free(struct cpe_dictitem_title * title)
{
	if (title == NULL)
		return;
	oscap_free(title->content);
	oscap_free(title->xmllang);
	oscap_free(title);
}

/* CPE -> Vendor
 */
struct cpe_dict_vendor *cpe_dictvendor_new_empty()
{
	struct cpe_dict_vendor *item;

	item = oscap_alloc(sizeof(struct cpe_dict_vendor));
	if (item == NULL)
		return NULL;

	memset(item, 0, sizeof(struct cpe_dict_vendor));
        item->value      = NULL;
	item->titles     = oscap_list_new();
	item->products   = oscap_list_new();

	return item;
}

struct cpe_dict_vendor *cpe_dictvendor_new_xml(xmlNodePtr node) {

	struct cpe_dict_vendor *item;
	struct cpe_dict_product *product;
        struct cpe_dictitem_title *title;
	xmlNodePtr cur;
	xmlChar *data;

	if (xmlStrcmp(node->name, BAD_CAST "vendor") != 0)
		return NULL;

	item = cpe_dictvendor_new_empty();
	if (item == NULL)
		return NULL;

	data = xmlGetProp(node, BAD_CAST "value");
        // TODO: Are we really going to return NULL if there is no "value" parameter ??
	if (data == NULL || (item->value = malloc(strlen((char *)data)+1)) == NULL) {
		oscap_free(item);
		oscap_free(data);
		return NULL;
	}
        strcpy(item->value, (char *)data);
	oscap_free(data);
        // There is 0-n titles and 0-n products (TODO: do we need to implement error
        // if there is no product for vendor ?)
        for (cur = node->xmlChildrenNode; cur != NULL; cur = cur->next) {
                if (xmlStrcmp(cur->name, BAD_CAST "title") == 0) {
                    // vendor -> title
                    title = cpe_dictitemtitle_new_xml(cur);
                    if (title) oscap_list_add(item->titles, title);
                } else if (xmlStrcmp(cur->name, BAD_CAST "product") == 0 ) {
                    // vendor -> product
                    product = cpe_dictproduct_new_xml(cur);
                    if (product) oscap_list_add(item->products, product);
                }
        }

        return item;
}

void cpe_dictvendor_free(struct cpe_dict_vendor * vendor) {

	if (vendor == NULL) return;

	oscap_list_free(vendor->titles, (oscap_destruct_func)cpe_dictitemtitle_free);
	oscap_list_free(vendor->products, (oscap_destruct_func)cpe_dictproduct_free);
	oscap_free(vendor);

}

/* CPE -> Vendor -> Product
 */
struct cpe_dict_product *cpe_dictproduct_new_empty() {

	struct cpe_dict_product *item;

	item = oscap_alloc(sizeof(struct cpe_dict_product));
	if (item == NULL)
		return NULL;

	memset(item, 0, sizeof(struct cpe_dict_product));
	item->value      = NULL;
	item->versions   = oscap_list_new();

	return item;
}

struct cpe_dict_product *cpe_dictproduct_new_xml(xmlNodePtr node) {

	struct cpe_dict_product *item;
	struct cpe_dict_version *version;
	xmlNodePtr cur;
	xmlChar *data;

	if (xmlStrcmp(node->name, BAD_CAST "product") != 0)
		return NULL;

	item = cpe_dictproduct_new_empty();
	if (item == NULL)
		return NULL;

	data = xmlGetProp(node, BAD_CAST "value");
        // TODO: Are we really going to return NULL if there is no "value" parameter ??
	if (data == NULL || (item->value = malloc(strlen((char *)data)+1)) == NULL) {
		oscap_free(item);
		oscap_free(data);
		return NULL;
	}
        strcpy(item->value, (char *)data);
	oscap_free(data);

	data = xmlGetProp(node, BAD_CAST "part");
	if (data) {
	    if (strcasecmp(data, "h") == 0)
		item->part = CPE_PART_HW;
	    else if (strcasecmp(data, "o") == 0)
		item->part = CPE_PART_OS;
	    else if (strcasecmp(data, "a") == 0)
		item->part = CPE_PART_APP;
	    else {
		oscap_free(item);
		oscap_free(data);
	        return NULL;
            }
	} else {
            item->part = -1;
        }

        for (cur = node->xmlChildrenNode; cur != NULL; cur = cur->next) {
                if (xmlStrcmp(cur->name, BAD_CAST "version") == 0) {
                    // vendor -> product -> version
                    version = cpe_dictversion_new_xml(cur);
                    if (version) oscap_list_add(item->versions, version);
                }
        }

        return item;
}

void cpe_dictproduct_free(struct cpe_dict_product * product) {
	if (product == NULL)
		return;
	oscap_free(product->value);
	oscap_list_free(product->versions, (oscap_destruct_func)cpe_dictversion_free);
	oscap_free(product);
}

/* CPE -> Vendor -> Product -> Version
 */
struct cpe_dict_version *cpe_dictversion_new_empty() {

	struct cpe_dict_version *item;

	item = oscap_alloc(sizeof(struct cpe_dict_version));
	if (item == NULL)
		return NULL;

	memset(item, 0, sizeof(struct cpe_dict_version));
	item->value      = NULL;
	item->updates   = oscap_list_new();

	return item;
}

struct cpe_dict_version *cpe_dictversion_new_xml(xmlNodePtr node) {

	struct cpe_dict_version *item;
	struct cpe_dict_update *update;
	xmlNodePtr cur;
	xmlChar *data;

	if (xmlStrcmp(node->name, BAD_CAST "version") != 0)
		return NULL;

	item = cpe_dictversion_new_empty();
	if (item == NULL)
		return NULL;

	data = xmlGetProp(node, BAD_CAST "value");
        // TODO: Are we really going to return NULL if there is no "value" parameter ??
	if (data == NULL || (item->value = malloc(strlen((char *)data)+1)) == NULL) {
		oscap_free(item);
		oscap_free(data);
		return NULL;
	}
        strcpy(item->value, (char *)data);
	oscap_free(data);
        for (cur = node->xmlChildrenNode; cur != NULL; cur = cur->next) {
                if (xmlStrcmp(cur->name, BAD_CAST "update") == 0) {
                    // vendor -> product -> version -> update
                    update = cpe_dictupdate_new_xml(cur);
                    if (update) oscap_list_add(item->updates, update);
                }
        }

        return item;
}

void cpe_dictversion_free(struct cpe_dict_version * version) {

	if (version == NULL)
		return;
	oscap_free(version->value);
	oscap_list_free(version->updates, (oscap_destruct_func)cpe_dictupdate_free);
	oscap_free(version);
}

/* CPE -> Vendor -> Product -> Version -> Update
 */
struct cpe_dict_update *cpe_dictupdate_new_empty() {

	struct cpe_dict_update *item;

	item = oscap_alloc(sizeof(struct cpe_dict_update));
	if (item == NULL)
		return NULL;

	memset(item, 0, sizeof(struct cpe_dict_update));
	item->value      = NULL;
	item->editions   = oscap_list_new();

	return item;
}

struct cpe_dict_update *cpe_dictupdate_new_xml(xmlNodePtr node) {

	struct cpe_dict_update *item;
	struct cpe_dict_edition *edition;
	xmlNodePtr cur;
	xmlChar *data;

	if (xmlStrcmp(node->name, BAD_CAST "update") != 0)
		return NULL;

	item = cpe_dictversion_new_empty();
	if (item == NULL)
		return NULL;

	data = xmlGetProp(node, BAD_CAST "value");
        // TODO: Are we really going to return NULL if there is no "value" parameter ??
	if (data == NULL || (item->value = malloc(strlen((char *)data)+1)) == NULL) {
		oscap_free(item);
		oscap_free(data);
		return NULL;
	}
        strcpy(item->value, (char *)data);
	oscap_free(data);
        for (cur = node->xmlChildrenNode; cur != NULL; cur = cur->next) {
                if (xmlStrcmp(cur->name, BAD_CAST "edition") == 0) {
                    // vendor -> product -> version -> update -> edition
                    edition = cpe_dictedition_new_xml(cur);
                    if (edition) oscap_list_add(item->editions, edition);
                }
        }

        return item;
}

void cpe_dictupdate_free(struct cpe_dict_update * update) {
	if (update == NULL)
		return;
	oscap_free(update->value);
	oscap_list_free(update->editions, (oscap_destruct_func)cpe_dictedition_free);
	oscap_free(update);
}

/* CPE -> Vendor -> Product -> Version -> Update -> Edition
 */
struct cpe_dict_edition *cpe_dictedition_new_empty() {

	struct cpe_dict_edition *item;

	item = oscap_alloc(sizeof(struct cpe_dict_edition));
	if (item == NULL)
		return NULL;

	memset(item, 0, sizeof(struct cpe_dict_edition));
	item->value      = NULL;

	return item;
}

struct cpe_dict_edition *cpe_dictedition_new_xml(xmlNodePtr node) {

	struct cpe_dict_update *item;
	struct cpe_dict_edition *edition;
	xmlChar *data;

	if (xmlStrcmp(node->name, BAD_CAST "edition") != 0)
		return NULL;

	item = cpe_dictedition_new_empty();
	if (item == NULL)
		return NULL;

	data = xmlGetProp(node, BAD_CAST "value");
        // TODO: Are we really going to return NULL if there is no "value" parameter ??
	if (data == NULL || (item->value = malloc(strlen((char *)data)+1)) == NULL) {
		oscap_free(item);
		oscap_free(data);
		return NULL;
	}
        strcpy(item->value, (char *)data);
	oscap_free(data);

        return item;
}

void cpe_dictedition_free(struct cpe_dict_edition * edition) {
	if (edition == NULL)
		return;
	oscap_free(edition->value);
	oscap_free(edition);
}

bool cpe_name_match_dict(struct cpe_name * cpe, struct cpe_dict * dict) {
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
	cpe_name_free(cpe);
	return ret;
}

void cpe_dictitem_title_export(const struct cpe_dictitem_title * item, xmlNodePtr root_node, const xmlNsPtr xmlns) {

        xmlNodePtr title_node = NULL;
        title_node = xmlNewChild(root_node, xmlns, BAD_CAST "title", BAD_CAST cpe_dictitem_title_get_content(item) );
        if (cpe_dictitem_title_get_xmllang(item) != NULL) {
                xmlNewProp(title_node, BAD_CAST "xml:lang", BAD_CAST cpe_dictitem_title_get_xmllang(item));
        }
}

void cpe_dict_check_export(const struct cpe_dict_check * check, xmlNodePtr root_node, const xmlNsPtr xmlns) {

        xmlNodePtr node = NULL;
        node = xmlNewChild(root_node, xmlns, BAD_CAST "check", BAD_CAST cpe_dict_check_get_identifier(check) );
        if (cpe_dict_check_get_system(check) != NULL)
                xmlNewProp(node, BAD_CAST "system", BAD_CAST cpe_dict_check_get_system(check));
        if (cpe_dict_check_get_href(check) != NULL)
                xmlNewProp(node, BAD_CAST "href", BAD_CAST cpe_dict_check_get_href(check));
}

void cpe_dict_references_export(const struct cpe_dict_reference * ref, xmlNodePtr root_node, const xmlNsPtr xmlns) {

        xmlNodePtr node = NULL;
        node = xmlNewChild(root_node, xmlns, BAD_CAST "reference", BAD_CAST cpe_dict_reference_get_content(ref) );
        if (cpe_dict_reference_get_href(ref) != NULL)
                xmlNewProp(node, BAD_CAST "href", BAD_CAST cpe_dict_reference_get_href(ref));
}

void cpe_dict_note_export(const char * note, xmlNodePtr root_node, const xmlNsPtr xmlns) {

        xmlNodePtr node = NULL;
        node = xmlNewChild(root_node, xmlns, BAD_CAST "note", BAD_CAST note );
}

void cpe_dictitem_export(const struct cpe_dictitem * item, xmlNodePtr root_node, const xmlNsPtr xmlns) {

        xmlNodePtr node = NULL;
        xmlNodePtr notes_node = NULL;
        node = xmlNewChild(root_node, xmlns, BAD_CAST "cpe-item", NULL);
        if (cpe_dictitem_get_name(item) != NULL) {
                xmlNewProp(node, BAD_CAST "name", BAD_CAST cpe_name_get_uri(cpe_dictitem_get_name(item)));
        }
        if (cpe_dictitem_get_deprecated(item) != NULL) {
                xmlNewProp(node, BAD_CAST "deprecated_by", BAD_CAST cpe_name_get_uri(cpe_dictitem_get_deprecated(item)));
                xmlNewProp(node, BAD_CAST "deprecated", BAD_CAST "true");
                xmlNewProp(node, BAD_CAST "deprecation_date", BAD_CAST cpe_dictitem_get_deprecation_date(item));
        }
        OSCAP_FOREACH (cpe_dictitem_title, title, cpe_dictitem_get_titles(item),
                cpe_dictitem_title_export(title, node, xmlns);
        )
        OSCAP_FOREACH (cpe_dict_check, check, cpe_dictitem_get_checks(item),
                cpe_dict_check_export(check, node, xmlns);
        )
        OSCAP_FOREACH (cpe_dict_reference, ref, cpe_dictitem_get_references(item),
                cpe_dict_references_export(ref, node, xmlns);
        )
        /*if (cpe_dictitem_get_notes(item) != NULL) {
                notes_node = xmlNewChild(node, xmlns, BAD_CAST "notes", NULL);
                OSCAP_FOREACH_STR (itn, cpe_dictitem_get_notes(item),
                        cpe_dict_note_export( (char *) oscap_string_iterator_next(itn), notes_node, xmlns);
                )
        }*/

        if (cpe_dictitem_get_metadata(item) != NULL) {
                xmlNodePtr metadata_node = xmlNewChild(node, xmlns, BAD_CAST "item-metadata", NULL);
                metadata_node->ns = xmlNewNs(NULL, NULL, "meta");
                if (cpe_item_metadata_get_modification_date(item->metadata) != NULL)
                    xmlNewProp(metadata_node, BAD_CAST "modification-date", BAD_CAST cpe_item_metadata_get_modification_date(item->metadata));
                if (cpe_item_metadata_get_status(item->metadata) != NULL)
                    xmlNewProp(metadata_node, BAD_CAST "status", BAD_CAST cpe_item_metadata_get_status(item->metadata));
                if (cpe_item_metadata_get_nvd_id(item->metadata) != NULL)
                    xmlNewProp(metadata_node, BAD_CAST "nvd-id", BAD_CAST cpe_item_metadata_get_nvd_id(item->metadata));
        }

}

void cpe_dict_editions_export(const struct cpe_dict_edition * edition, xmlNodePtr root_node, xmlNsPtr xmlns) {

        xmlNodePtr node  = NULL;

        node = xmlNewChild(root_node, xmlns, BAD_CAST "edition", NULL);
        if (cpe_dict_edition_get_value(edition) != NULL)
            xmlNewProp(node, BAD_CAST "value", BAD_CAST cpe_dict_edition_get_value(edition));
}

void cpe_dict_updates_export(const struct cpe_dict_update * update, xmlNodePtr root_node, xmlNsPtr xmlns) {

        xmlNodePtr node  = NULL;

        node = xmlNewChild(root_node, xmlns, BAD_CAST "update", NULL);
        // beware ! Value is required here !
        if (cpe_dict_update_get_value(update) != NULL)
            xmlNewProp(node, BAD_CAST "value", BAD_CAST cpe_dict_update_get_value(update));

        OSCAP_FOREACH (cpe_dict_edition, edition, cpe_dict_update_get_editions(update),
                cpe_dict_editions_export(edition, node, xmlns);
        )
}

void cpe_dict_version_export(const struct cpe_dict_version * version, xmlNodePtr root_node, xmlNsPtr xmlns) {

        xmlNodePtr node = NULL;

        node = xmlNewChild(root_node, xmlns, BAD_CAST "version", NULL);
        // beware ! Value is required here !
        if (cpe_dict_version_get_value(version) != NULL)
            xmlNewProp(node, BAD_CAST "value", BAD_CAST cpe_dict_version_get_value(version));

        OSCAP_FOREACH (cpe_dict_update, update, cpe_dict_version_get_updates(version),
                cpe_dict_updates_export(update, node, xmlns);
        )

}

void cpe_dict_products_export(const struct cpe_dict_product * product, xmlNodePtr root_node, xmlNsPtr xmlns) {

        xmlNodePtr product_node = NULL;

        product_node = xmlNewChild(root_node, xmlns, BAD_CAST "product", NULL);
        if (cpe_dict_product_get_value(product) != NULL)
            xmlNewProp(product_node, BAD_CAST "value", BAD_CAST cpe_dict_product_get_value(product));
        if (cpe_dict_product_get_part(product) != -1) // -1, because product->part is enum
            xmlNewProp(product_node, BAD_CAST "part", BAD_CAST PART_TO_CHAR[cpe_dict_product_get_part(product)]);

        OSCAP_FOREACH (cpe_dict_version, version, cpe_dict_product_get_versions(product),
                cpe_dict_version_export(version, product_node, xmlns);
        )
}


void cpe_dict_vendors_export(const struct cpe_dict_vendor * vendor, xmlNodePtr root_node, xmlNsPtr xmlns){

        xmlNodePtr node = NULL;
        node = xmlNewChild(root_node, xmlns, BAD_CAST "vendor", NULL);
        if (cpe_dict_vendor_get_value(vendor) != NULL) {
                xmlNewProp(node, BAD_CAST "value", BAD_CAST cpe_dict_vendor_get_value(vendor));
        }

        OSCAP_FOREACH (cpe_dictitem_title, title, cpe_dict_vendor_get_titles(vendor),
                cpe_dictitem_title_export(title, node, xmlns);
        )
        OSCAP_FOREACH (cpe_dict_product, product, cpe_dict_vendor_get_products(vendor),
                cpe_dict_products_export(product, node, xmlns);
        )
}

bool cpe_dict_export(const struct cpe_dict * dict, const char * fname) {


        if (dict == NULL)
                return false;

        xmlDocPtr   doc             = NULL;             // document pointer
        xmlNodePtr  root_node       = NULL;             // root node pointer
        xmlNodePtr  component_node  = NULL;             // root node pointer for component-tree
        xmlNsPtr    xmlns           = NULL;             // xml namespace nodea

        doc = xmlNewDoc(BAD_CAST "1.0");
        root_node = xmlNewNode(NULL, BAD_CAST "cpe-list");
        //xmlns = xmlNewNs(root_node, BAD_CAST res->ns_href, BAD_CAST res->ns_prefix);
        //root_node->ns = xmlns;
        xmlDocSetRootElement(doc, root_node);

        if (dict->items == NULL) {
            xmlFreeDoc(doc);
            return NULL;
        }

        xmlNodePtr node = NULL;
        xmlNodePtr generator_node = xmlNewChild(root_node, NULL, BAD_CAST "generator", NULL);
        if (dict->generator_product_name != NULL) {
            node = xmlNewChild(generator_node, NULL, BAD_CAST "product_name", dict->generator_product_name);
        }
        if (dict->generator_product_version != NULL) {
            node = xmlNewChild(generator_node, NULL, BAD_CAST "product_version", dict->generator_product_version);
        }
        if (dict->generator_schema_version != NULL) {
            node = xmlNewChild(generator_node, NULL, BAD_CAST "schema_version", dict->generator_schema_version);
        }
        if (dict->generator_timestamp != NULL) {
            node = xmlNewChild(generator_node, NULL, BAD_CAST "timestamp", dict->generator_timestamp);
        }

	OSCAP_FOREACH (cpe_dictitem, dictitem, cpe_dict_get_items(dict),
		// dump items to XML tree
                cpe_dictitem_export(dictitem, root_node, xmlns);
	)

        component_node = xmlNewChild(root_node, NULL, BAD_CAST "component-tree", NULL);
        xmlns = xmlNewNs(component_node, NULL, BAD_CAST "meta");
        component_node->ns = xmlns;

        OSCAP_FOREACH (cpe_dict_vendor, vendor, cpe_dict_get_vendors(dict),
                // dump vendors to XML tree
                cpe_dict_vendors_export(vendor, component_node, xmlns);
        )

        xmlSaveFormatFileEnc(fname, doc, "UTF-8", 1);

        xmlFreeDoc(doc);

        return true;
}

