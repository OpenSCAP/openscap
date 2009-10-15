/**
 * @file cpedict_priv.c
 * \brief Interface to Common Platform Enumeration (CPE) Language
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
 *      Maros Barabas <mbarabas@redhat.com>
 */


#include <libxml/xmlreader.h>
#include "cpedict_priv.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "cpedict.h" // remove before release
#include "cpeuri.h"
#include "cpelang_priv.h"
#include "../common/util.h"
#include "../common/list.h"


// cpe_element_metadata -> meta structure for every XML element
// with all information that are common as "XML namespace"
struct xml_metadata {
        char *namespace;
        char *lang;
};

struct cpe_item_metadata {
        struct xml_metadata xml;
        char *modification_date;
        char *status;
        char *nvd_id;
        char *deprecated_by_nvd_id;
};

struct cpe_dict_check {
        struct xml_metadata xml;
	char *system;      // system check URI
	char *href;        // external file reference (NULL if not present)
	char *identifier;  // test identifier
};


struct cpe_dict_reference {
        struct xml_metadata xml;
	char *href;     // reference URL
	char *content;  // reference description
};

struct cpe_dictitem_title {
        struct xml_metadata xml;
	char *content;		        // human-readable name of this item
};

struct cpe_dictitem {                   // the node <cpe-item>

        struct xml_metadata xml;
	struct cpe_name *name;		// CPE name as CPE URI
        struct oscap_list* titles;      // titles of cpe-item (can be in various languages)

	struct cpe_name *deprecated;   // CPE that deprecated this one (or NULL)
	char *deprecation_date;	       // date of deprecation

	struct oscap_list* references; // list of references
	struct oscap_list* checks;     // list of checks
	struct oscap_list* notes;      // list of notes - it's the same structure as titles
        struct cpe_item_metadata *metadata;    // element <meta:item-metadata>
};

struct cpe_generator {

        struct xml_metadata xml;
	char *product_name;    // generator software name
	char *product_version; // generator software version
	char *schema_version;	 // generator schema version
	char *timestamp;       // generation date and time
};


struct cpe_dict {                        // the main node

        struct xml_metadata xml;
	struct oscap_list* items;        // dictionary items
        struct oscap_list* vendors;
        struct cpe_generator* generator;
};

/* **************************
 * Component-tree structures
 * **************************/
struct cpe_dict_language {
        struct xml_metadata xml;
        char *value;
};

struct cpe_dict_edition {
        struct xml_metadata xml;
        char *value;
        struct oscap_list* languages;
};

struct cpe_dict_update {
        struct xml_metadata xml;
        char *value;
        struct oscap_list* editions;
};

struct cpe_dict_version {
        struct xml_metadata xml;
        char *value;
        struct oscap_list* updates;
};

struct cpe_dict_product {
        struct xml_metadata xml;
        char *value;
        cpe_part_t part;                // enum with "h" or "a" or "o", see cpeuri
        struct oscap_list* versions;

};

struct cpe_dict_vendor {
        struct xml_metadata xml;
        char *value;
        struct oscap_list* titles;
        struct oscap_list* products;
};
/****************************/

static char *str_trim(char *str)
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

void cpedict_parse_file(const char *fname) {
        
    xmlTextReaderPtr reader;

    reader = xmlReaderForFile(fname, NULL, 0);
    if (reader != NULL) {
        xmlTextReaderRead(reader);
        print_node(reader);
        (void) parse_cpedict(reader);
    } else {
        fprintf(stderr, "Unable to open %s\n", fname);
    }
    xmlFreeTextReader(reader);
}

static struct cpe_dictitem *cpe_dictitem_new_empty() {
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

static struct cpe_dict_vendor *cpe_dictvendor_new_empty() {
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

static struct cpe_dict * parse_cpedict(xmlTextReaderPtr reader) {

        struct cpe_dict *ret = NULL;
        struct cpe_dictitem *item = NULL;
        struct cpe_dict_vendor *vendor = NULL;
        int next_ret = 1;

        // let's find "<cpe-list>" element
        while (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "cpe-list") && 
                (next_ret == 1)) {
                next_ret = xmlTextReaderNextElement(reader);
                // There is no "<cpe-list>" element :(( and we are at the end of file !
                if (next_ret == 0) {
                        fprintf(stderr, "There is no \"cpe-list\" element in the provided xml tree !\n");
                        return NULL;
                }
        }

        // we found cpe-list element, let's roll !
        // allocate memory for cpe_dict so we can fill items and vendors and general structures
        ret = cpe_dict_new_empty();
        if (ret == NULL)
            return NULL;

        // TODO: here comes some header processing (for namespace definitions)


        // go through elements and switch through actions till end of file..
        next_ret = xmlTextReaderNextElement(reader);
        ret->vendors = oscap_list_new(); // why this has to be here ? 
        while (next_ret != 0) {
                
            if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "generator")) { // <generator> | count = 1
                        ret->generator = cpe_generator_new_xml(reader);
            } else 
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "cpe-item")) { // <cpe-item> | cout = 0-n
			if ((item = cpe_dictitem_new_xml(reader)) == NULL) {
				// something bad happend, let's try to recover and continue
                                // add here some bad nodes list to write it to stdout after parsing is done
                                // get the next node
                                next_ret = xmlTextReaderNextElement(reader);
                                continue;
                        }
			if (!cpe_dict_add_item(ret, item)) {
				//cpe_dictitem_free(item);
				cpe_dict_free(ret);
				return NULL;
			}
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "vendor")) { // <vendor> | count = 0-n
                        vendor = cpe_dict_vendor_parse(reader);
			if (vendor) oscap_list_add(ret->vendors, vendor);
            } else
                // TODO: we need to store meta xml data of <component-tree> element
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "component-tree")) { // <vendor> | count = 0-n
                        // we just need to jump over this element
            }
            // get the next node
            next_ret = xmlTextReaderNextElement(reader);
        }

	return ret;
}

static struct cpe_generator * cpe_generator_new_xml(xmlTextReaderPtr reader) {

        struct cpe_generator *ret = NULL;

        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "generator") &&
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

                // we are on "<generator>" element, let's alloc structure
	        ret = oscap_alloc(sizeof(struct cpe_generator));
                if (ret == NULL)
                        return NULL;

                /* We don't have a language or namespace in <generator> elements, otherwise use this code:
                 * ret->xml.lang = (char *) xmlTextReaderConstXmlLang(reader);
                 * ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                 * probably the product_name below will need language specification...
                */
                
                // skip nodes until new element
                xmlTextReaderNextElement(reader);

                while (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST "generator") != 0) {

                        if ((xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                            (const xmlChar *)"product_name") == 0) &&
                            (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)){
			            ret->product_name = (char *) xmlTextReaderValue(reader);
                        } else 
                            if ((xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"product_version") == 0) &&
                                (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)){
			            ret->product_version = (char *) xmlTextReaderValue(reader);
                        } else 
                            if ((xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"schema_version") == 0) &&
                                (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)){
			            ret->schema_version = (char *) xmlTextReaderValue(reader);
                        } else 
                            if ((xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"timestamp") == 0) &&
                                (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)){
			            ret->timestamp = (char *) xmlTextReaderValue(reader);
                        }

                        // element saved. Let's jump on the very next one node (not element, because we need to 
                        // find XML_READER_TYPE_END_ELEMENT node, see "while" condition and the condition below "while"
                        xmlTextReaderRead(reader);

                }
                // we found another element generator which is not "end element" ? Horrible !
                if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_END_ELEMENT) 
                        // print some nasty error message ?
                        return NULL;
        }

        return ret;

}


static struct cpe_dictitem * cpe_dictitem_new_xml(xmlTextReaderPtr reader) {

        struct cpe_dictitem *ret = NULL;
        struct cpe_dictitem_title *title = NULL;
        struct cpe_dict_check *check = NULL;
        struct cpe_dict_reference *ref = NULL;
        char *data;

        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "cpe-item") &&
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

                // we are on "<cpe-item>" element, let's alloc structure
	        ret = oscap_alloc(sizeof(struct cpe_dictitem));
                ret = cpe_dictitem_new_empty();
                if (ret == NULL)
                        return NULL;

                ret->xml.lang = (char *) xmlTextReaderConstXmlLang(reader);
                ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);

                // Get a name attribute of cpe-item
                data = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "name");
                if (data != NULL) 
                    ret->name = cpe_name_new(data);
                oscap_free(data);

                // if there is "deprecated", "deprecated_by" and "deprecation_date" in cpe-item element
                // ************************************************************************************
                data = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "deprecated");
                if (data != NULL) { // we have a deprecation here !
	                oscap_free(data);
                        data = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "deprecated_by");
	                if (data == NULL || (ret->deprecated = cpe_name_new(data)) == NULL) {
		                oscap_free(ret);
		                oscap_free(data);
		                return NULL;
                        }
	                oscap_free(data);

                        data = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "deprecation_date");
	                if(data == NULL || (ret->deprecation_date = malloc(strlen(data)+1)) == NULL)   {
		                oscap_free(ret);
		                oscap_free(data);
		                return NULL;
                        }
                        strcpy(ret->deprecation_date, (char *)data);
                }
                oscap_free(data);
                // ************************************************************************************

                xmlTextReaderNextElement(reader);
                // Now it's time to go deaply to cpe-item element and parse it's children
                // Do while there is another cpe-item element. Then return.
                while (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST "cpe-item") != 0) {
                        
                        if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) 
                                continue;

                        if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                            (const xmlChar *)"title") == 0) {
                                    title = cpe_dictitem_title_parse(reader, "title");
                                    if (title) oscap_list_add(ret->titles, title);
                        } else 
                            if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"notes") == 0) {
                                    // it's OK to use title varible here, because note is the same structure,
                                    // not the reason to make a new same one
                                    title = cpe_dictitem_title_parse(reader, "note");
                                    if (title) oscap_list_add(ret->notes, title);
                        } else 
                            if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"check") == 0) {
                                    check = cpe_dict_check_parse(reader);
                                    if (check) oscap_list_add(ret->checks, check);
                        } else 
                            if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"reference") == 0) {
				    ref = oscap_calloc(1, sizeof(struct cpe_dict_reference));
				    ref->content = str_trim((char *) xmlTextReaderConstValue(reader));
				    ref->href = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "href");
				    if (ref) oscap_list_add(ret->references, ref);
                        } else 
                            if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"item-metadata") == 0) {
                                    data = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "modification-date");
                                    if ((data == NULL) || ((ret->metadata = oscap_alloc(sizeof(struct cpe_item_metadata))) == NULL)){
                                            oscap_free(ret);
                                            oscap_free(data);
                                            return NULL;
                                    }
                                    if (data) ret->metadata->modification_date = data;

                                    data = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "status");
	                            if (data) ret->metadata->status = data;
                                    data = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "nvd-id");
	                            if (data) ret->metadata->nvd_id = (char *)data;
                                    data = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "deprecated-by-nvd-id");
	                            if (data) ret->metadata->deprecated_by_nvd_id = (char *)data;
                                    else ret->metadata->deprecated_by_nvd_id = NULL;
                                    data = NULL;

                        } else 
                            if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"references") == 0) {
                                // we just need to jump over this element
                        
                        } else {
                                return ret; // <-- we need to return here, because we don't want to jump to next element 
                        }
                        xmlTextReaderNextElement(reader);
                }
        }

        return ret;
}

static struct cpe_dicitem_title * cpe_dictitem_title_parse(xmlTextReaderPtr reader, char * name) {

	xmlChar *data;
	struct cpe_dictitem_title *ret;

        if (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST name) != 0)
		return NULL;

	if ((ret = oscap_alloc(sizeof(struct cpe_dictitem_title))) == NULL)
		return NULL;
	memset(ret, 0, sizeof(struct cpe_dictitem_title));

        ret->xml.lang = (char *) xmlTextReaderConstXmlLang(reader);
        ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);

	ret->content = str_trim((char *) xmlTextReaderConstValue(reader));

	return ret;
}

static struct cpe_dict_check * cpe_dict_check_parse(xmlTextReaderPtr reader) {

	struct cpe_dict_check *ret;

        if (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST "check") != 0)
		return NULL;

	if ((ret = oscap_alloc(sizeof(struct cpe_dict_check))) == NULL)
		return NULL;
	memset(ret, 0, sizeof(struct cpe_dict_check));

        /* TODO: find out if there should be xml:lang lookup
         * ret->xml.lang = (char *) xmlTextReaderConstXmlLang(reader);
         */ 
        ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
        ret->system = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "system");
        ret->href = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "href");
	ret->identifier = str_trim((char *) xmlTextReaderConstValue(reader));

	return ret;
}

static struct cpe_dict_vendor * cpe_dict_vendor_parse(xmlTextReaderPtr reader) {


        struct cpe_dict_vendor *ret;
        struct cpe_dictitem_title *title;
        struct cpe_dict_product *product;
        struct cpe_dict_version *version;
        struct cpe_dict_update *update;
        struct cpe_dict_edition *edition;
        struct cpe_dict_language *language;
        char *data;

        if (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST "vendor") != 0)
		return NULL;

        ret = cpe_dictvendor_new_empty();
        if (ret == NULL)
                return NULL;

        ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
        
        // jump to next element (which should be product)
        xmlTextReaderNextElement(reader);

        while(xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST "vendor") != 0) {
                        
                        if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                            (const xmlChar *)"title") == 0) {
                                    title = cpe_dictitem_title_parse(reader, "title");
                                    if (title) oscap_list_add(ret->titles, title);
                        } else 
                            if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"product") == 0) {
                                    // initialization
                                    product = oscap_alloc(sizeof(struct cpe_dict_product));
                                    product->versions = oscap_list_new();
                                    product->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                                    product->value = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "value");

	                            data = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "part");
	                            if (data) {
	                                    if (strcasecmp((const char *)data, "h") == 0)
		                                    product->part = CPE_PART_HW;
	                                    else if (strcasecmp((const char *)data, "o") == 0)
		                                    product->part = CPE_PART_OS;
	                                    else if (strcasecmp((const char *)data, "a") == 0)
		                                    product->part = CPE_PART_APP;
	                                    else {
		                                    oscap_free(ret);
		                                    oscap_free(data);
	                                            return NULL;
                                            }
	                            } else {
                                            product->part = -1;
                                    }
                                    oscap_free(data);

                                    if (product) oscap_list_add(ret->products, product);
                        } else 
                            if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"version") == 0) {
                                    // initialization
                                    version = oscap_alloc(sizeof(struct cpe_dict_version));
                                    version->updates = oscap_list_new();
                                    version->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                                    version->value = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "value");
                                    oscap_list_add(product->versions, version);
                        } else 
                            if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"update") == 0) {
                                    // initialization
                                    update = oscap_alloc(sizeof(struct cpe_dict_update));
                                    update->editions = oscap_list_new();
                                    update->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                                    update->value = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "value");
                                    oscap_list_add(version->updates, update);
                        } else 
                            if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"edition") == 0) {
                                    // initialization
                                    edition = oscap_alloc(sizeof(struct cpe_dict_edition));
                                    edition->languages = oscap_list_new();
                                    edition->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                                    edition->value = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "value");
                                    oscap_list_add(update->editions, edition);
                        } else 
                            if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"language") == 0) {
                                    // initialization
                                    language = oscap_alloc(sizeof(struct cpe_dict_language));
                                    language->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                                    language->value = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "value");
                                    oscap_list_add(edition->languages, language);
                        }
                        xmlTextReaderNextElement(reader);
        }

        return ret;
        
}
