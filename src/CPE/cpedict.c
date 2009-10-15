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
#include "cpedict_priv.h"
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
        char *deprecated_by_nvd_id;
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
struct cpe_dict_language {
        char *value;
};

struct cpe_dict_edition {
        char *value;
        struct oscap_list* languages;
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


/*
 * OSCAP_GETTER(      <return value>, <structure name>, <value of structure>)
 * OSCAP_IGETTER_GEN( <return value>, <structure name>, <oscap_list *>)
 */


static void cpe_dictitem_free(struct cpe_dictitem * item);

void cpe_dict_check_free(struct cpe_dict_check * check);

/* Declarations for private header
 */
static void cpe_dictvendor_free(struct cpe_dict_vendor * vendor);
static void cpe_dictproduct_free(struct cpe_dict_product * product);
static void cpe_dictversion_free(struct cpe_dict_version * version);
static void cpe_dictupdate_free(struct cpe_dict_update * update);
static void cpe_dictedition_free(struct cpe_dict_edition * edition);
static void cpe_dictlanguage_free(struct cpe_dict_language * language);

static const size_t CPE_DICT_CPES_INITIAL_ALLOC = 8;

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

static void cpe_dict_reference_free(struct cpe_dict_reference* ref)
{
	if (ref) {
		oscap_free(ref->href);
		oscap_free(ref->content);
		oscap_free(ref);
	}
}

static void cpe_dictcheck_free(struct cpe_dict_check * check);
static void cpe_dictitemtitle_free(struct cpe_dictitem_title * title);

static void cpe_dictitem_free(struct cpe_dictitem * item)
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

static void cpe_dictcheck_free(struct cpe_dict_check * check)
{
	if (check == NULL)
		return;
	oscap_free(check->identifier);
	oscap_free(check->system);
	oscap_free(check->href);
	oscap_free(check);
}

static void cpe_dictitemtitle_free(struct cpe_dictitem_title * title)
{
	if (title == NULL)
		return;
	oscap_free(title->content);
	oscap_free(title->xmllang);
	oscap_free(title);
}

/* CPE -> Vendor
 */
static void cpe_dictvendor_free(struct cpe_dict_vendor * vendor) {

	if (vendor == NULL) return;

	oscap_list_free(vendor->titles, (oscap_destruct_func)cpe_dictitemtitle_free);
	oscap_list_free(vendor->products, (oscap_destruct_func)cpe_dictproduct_free);
	oscap_free(vendor);

}

static void cpe_dictproduct_free(struct cpe_dict_product * product) {
	if (product == NULL)
		return;
	oscap_free(product->value);
	oscap_list_free(product->versions, (oscap_destruct_func)cpe_dictversion_free);
	oscap_free(product);
}

/* CPE -> Vendor -> Product -> Version
 */
static void cpe_dictversion_free(struct cpe_dict_version * version) {

	if (version == NULL)
		return;
	oscap_free(version->value);
	oscap_list_free(version->updates, (oscap_destruct_func)cpe_dictupdate_free);
	oscap_free(version);
}

static void cpe_dictupdate_free(struct cpe_dict_update * update) {
	if (update == NULL)
		return;
	oscap_free(update->value);
	oscap_list_free(update->editions, (oscap_destruct_func)cpe_dictedition_free);
	oscap_free(update);
}

static void cpe_dictedition_free(struct cpe_dict_edition * edition) {
	if (edition == NULL)
		return;
	oscap_free(edition->value);
	oscap_list_free(edition->languages, (oscap_destruct_func)cpe_dictlanguage_free);
	oscap_free(edition);
}

static void cpe_dictlanguage_free(struct cpe_dict_language * language) {
	if (language == NULL)
		return;
	oscap_free(language->value);
	oscap_free(language);
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
/*
static void cpe_dictitem_title_export(const struct cpe_dictitem_title * item, xmlNodePtr root_node, const xmlNsPtr xmlns) {

        xmlNodePtr title_node = NULL;
        title_node = xmlNewChild(root_node, xmlns, BAD_CAST "title", BAD_CAST cpe_dictitem_title_get_content(item) );
        if (cpe_dictitem_title_get_xmllang(item) != NULL) {
                xmlNewProp(title_node, BAD_CAST "xml:lang", BAD_CAST cpe_dictitem_title_get_xmllang(item));
        }
}

static void cpe_dict_check_export(const struct cpe_dict_check * check, xmlNodePtr root_node, const xmlNsPtr xmlns) {

        xmlNodePtr node = NULL;
        node = xmlNewChild(root_node, xmlns, BAD_CAST "check", BAD_CAST cpe_dict_check_get_identifier(check) );
        if (cpe_dict_check_get_system(check) != NULL)
                xmlNewProp(node, BAD_CAST "system", BAD_CAST cpe_dict_check_get_system(check));
        if (cpe_dict_check_get_href(check) != NULL)
                xmlNewProp(node, BAD_CAST "href", BAD_CAST cpe_dict_check_get_href(check));
}

static void cpe_dict_references_export(const struct cpe_dict_reference * ref, xmlNodePtr root_node, const xmlNsPtr xmlns) {

        xmlNodePtr node = NULL;
        node = xmlNewChild(root_node, xmlns, BAD_CAST "reference", BAD_CAST cpe_dict_reference_get_content(ref) );
        if (cpe_dict_reference_get_href(ref) != NULL)
                xmlNewProp(node, BAD_CAST "href", BAD_CAST cpe_dict_reference_get_href(ref));
}

static void cpe_dict_note_export(const char * note, xmlNodePtr root_node, const xmlNsPtr xmlns) {

        xmlNodePtr node = NULL;
        node = xmlNewChild(root_node, xmlns, BAD_CAST "note", BAD_CAST note );
}

static void cpe_dictitem_export(const struct cpe_dictitem * item, xmlNodePtr root_node, const xmlNsPtr xmlns) {

        xmlNodePtr node = NULL;
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
        if (cpe_dictitem_get_notes(item) != NULL) {
                notes_node = xmlNewChild(node, xmlns, BAD_CAST "notes", NULL);
                OSCAP_FOREACH_STR (itn, cpe_dictitem_get_notes(item),
                        cpe_dict_note_export( (char *) oscap_string_iterator_next(itn), notes_node, xmlns);
                )
        }

        if (cpe_dictitem_get_metadata(item) != NULL) {
                xmlNodePtr metadata_node = xmlNewChild(node, xmlns, BAD_CAST "item-metadata", NULL);
                metadata_node->ns = xmlNewNs(NULL, NULL, BAD_CAST "meta");
                if (cpe_item_metadata_get_deprecated_by_nvd_id(item->metadata) != NULL)
                    xmlNewProp(metadata_node, BAD_CAST "deprecated-by-nvd-id", BAD_CAST cpe_item_metadata_get_deprecated_by_nvd_id(item->metadata));
                if (cpe_item_metadata_get_modification_date(item->metadata) != NULL)
                    xmlNewProp(metadata_node, BAD_CAST "modification-date", BAD_CAST cpe_item_metadata_get_modification_date(item->metadata));
                if (cpe_item_metadata_get_status(item->metadata) != NULL)
                    xmlNewProp(metadata_node, BAD_CAST "status", BAD_CAST cpe_item_metadata_get_status(item->metadata));
                if (cpe_item_metadata_get_nvd_id(item->metadata) != NULL)
                    xmlNewProp(metadata_node, BAD_CAST "nvd-id", BAD_CAST cpe_item_metadata_get_nvd_id(item->metadata));
        }

}

static void cpe_dict_languages_export(const struct cpe_dict_language * language, xmlNodePtr root_node, xmlNsPtr xmlns) {

        xmlNodePtr node  = NULL;

        node = xmlNewChild(root_node, xmlns, BAD_CAST "language", NULL);
        if (cpe_dict_language_get_value(language) != NULL)
            xmlNewProp(node, BAD_CAST "value", BAD_CAST cpe_dict_language_get_value(language));
}

static void cpe_dict_editions_export(const struct cpe_dict_edition * edition, xmlNodePtr root_node, xmlNsPtr xmlns) {

        xmlNodePtr node  = NULL;

        node = xmlNewChild(root_node, xmlns, BAD_CAST "edition", NULL);
        if (cpe_dict_edition_get_value(edition) != NULL)
            xmlNewProp(node, BAD_CAST "value", BAD_CAST cpe_dict_edition_get_value(edition));

        OSCAP_FOREACH (cpe_dict_language, language, cpe_dict_edition_get_languages(edition),
                cpe_dict_languages_export(language, node, xmlns);
        )
}

static void cpe_dict_updates_export(const struct cpe_dict_update * update, xmlNodePtr root_node, xmlNsPtr xmlns) {

        xmlNodePtr node  = NULL;

        node = xmlNewChild(root_node, xmlns, BAD_CAST "update", NULL);
        // beware ! Value is required here !
        if (cpe_dict_update_get_value(update) != NULL)
            xmlNewProp(node, BAD_CAST "value", BAD_CAST cpe_dict_update_get_value(update));

        OSCAP_FOREACH (cpe_dict_edition, edition, cpe_dict_update_get_editions(update),
                cpe_dict_editions_export(edition, node, xmlns);
        )
}

static void cpe_dict_version_export(const struct cpe_dict_version * version, xmlNodePtr root_node, xmlNsPtr xmlns) {

        xmlNodePtr node = NULL;

        node = xmlNewChild(root_node, xmlns, BAD_CAST "version", NULL);
        // beware ! Value is required here !
        if (cpe_dict_version_get_value(version) != NULL)
            xmlNewProp(node, BAD_CAST "value", BAD_CAST cpe_dict_version_get_value(version));

        OSCAP_FOREACH (cpe_dict_update, update, cpe_dict_version_get_updates(version),
                cpe_dict_updates_export(update, node, xmlns);
        )

}

static void cpe_dict_products_export(const struct cpe_dict_product * product, xmlNodePtr root_node, xmlNsPtr xmlns) {

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


static void cpe_dict_vendors_export(const struct cpe_dict_vendor * vendor, xmlNodePtr root_node, xmlNsPtr xmlns){

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
*/
