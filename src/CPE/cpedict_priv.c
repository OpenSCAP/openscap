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
#include "cpelang_priv.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "cpeuri.h"
#include "../common/util.h"
#include "../common/list.h"

#define FILE_ENCODING "UTF-8"

/***************************************************************************/
/* Variable definitions
 *
 * cpe_element_metadata -> meta structure for every XML element
 * with all information that are common as "XML namespace"
 * */

const char *PART_TO_CHAR[] = { NULL, "h", "o", "a" };

struct xml_metadata {
        char *namespace;
        char *lang;
};

/* ****************************************
 * CPE-List structures
 * ***************************************/

/***********************************/
/* <cpe-item><item-metadata>
 * */
struct cpe_item_metadata {
        struct xml_metadata xml;
        char *modification_date;
        char *status;
        char *nvd_id;
        char *deprecated_by_nvd_id;
};
OSCAP_GETTER(const char*, cpe_item_metadata, modification_date)
OSCAP_GETTER(const char*, cpe_item_metadata, status)
OSCAP_GETTER(const char*, cpe_item_metadata, nvd_id)
OSCAP_GETTER(const char*, cpe_item_metadata, deprecated_by_nvd_id)

/***********************************/
/* <cpe-item><check>
 * */
struct cpe_dict_check {
        struct xml_metadata xml;
	char *system;      // system check URI
	char *href;        // external file reference (NULL if not present)
	char *identifier;  // test identifier
};
OSCAP_GETTER(const char*, cpe_dict_check, system)
OSCAP_GETTER(const char*, cpe_dict_check, href)
OSCAP_GETTER(const char*, cpe_dict_check, identifier)

/***********************************/
/* <cpe-item><references><reference>
 * */
struct cpe_dict_reference {
        struct xml_metadata xml;
	char *href;     // reference URL
	char *content;  // reference description
};
OSCAP_GETTER(const char*, cpe_dict_reference, href)
OSCAP_GETTER(const char*, cpe_dict_reference, content)

/***********************************/
/* <cpe-item><title>
 * */
struct cpe_dictitem_title {
        struct xml_metadata xml;
	char *content;		        // human-readable name of this item
};
OSCAP_GETTER(const char*, cpe_dictitem_title, content)

/***********************************/
/* <cpe-item>
 * */
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
OSCAP_GETTER(struct cpe_name*, cpe_dictitem, name)
OSCAP_GETTER(struct cpe_name*, cpe_dictitem, deprecated)
OSCAP_GETTER(const char*, cpe_dictitem, deprecation_date)
OSCAP_GETTER(struct cpe_item_metadata*, cpe_dictitem, metadata)
OSCAP_IGETTER_GEN(cpe_dict_reference, cpe_dictitem, references)
OSCAP_IGETTER_GEN(cpe_dict_check, cpe_dictitem, checks)
OSCAP_IGETTER_GEN(cpe_dictitem_title, cpe_dictitem, titles)
OSCAP_IGETTER(cpe_dictitem_title, cpe_dictitem, notes)

/***********************************/
/* <generator>
 * */
struct cpe_generator {

        struct xml_metadata xml;
	char *product_name;    // generator software name
	char *product_version; // generator software version
	char *schema_version;	 // generator schema version
	char *timestamp;       // generation date and time
};
OSCAP_GETTER(const char*, cpe_generator, product_name)
OSCAP_GETTER(const char*, cpe_generator, product_version)
OSCAP_GETTER(const char*, cpe_generator, schema_version)
OSCAP_GETTER(const char*, cpe_generator, timestamp)

/***********************************/
/* <cpe-list>
 * */
struct cpe_dict {                        // the main node

        struct xml_metadata xml;
	struct oscap_list* items;        // dictionary items
        struct oscap_list* vendors;
        struct cpe_generator* generator;
};
OSCAP_GETTER(struct cpe_generator*, cpe_dict, generator)
OSCAP_IGETTER_GEN(cpe_dictitem, cpe_dict, items)
OSCAP_IGETTER_GEN(cpe_dict_vendor, cpe_dict, vendors)

/* ****************************************
 * Component-tree structures
 * ***************************************/

/***********************************/
/* vendor
 * */
struct cpe_dict_vendor {
        struct xml_metadata xml;
        char *value;
        struct oscap_list* titles;
        struct oscap_list* products;
};
OSCAP_GETTER(const char*, cpe_dict_vendor, value)
OSCAP_IGETTER(cpe_dictitem_title, cpe_dict_vendor, titles)
OSCAP_IGETTER_GEN(cpe_dict_product, cpe_dict_vendor, products)

/***********************************/
/* vendor -> product 
 * */
struct cpe_dict_product {
        struct xml_metadata xml;
        char *value;
        cpe_part_t part;
        struct oscap_list* versions;
};
OSCAP_GETTER(const char*, cpe_dict_product, value)
OSCAP_GETTER(int, cpe_dict_product, part)
OSCAP_IGETTER_GEN(cpe_dict_version, cpe_dict_product, versions)

/***********************************/
/* vendor -> product -> version 
 * */
struct cpe_dict_version {
        struct xml_metadata xml;
        char *value;
        struct oscap_list* updates;
};
OSCAP_GETTER(const char*, cpe_dict_version, value)
OSCAP_IGETTER_GEN(cpe_dict_update, cpe_dict_version, updates)

/***********************************/
/* vendor -> product -> version -> update 
 * */
struct cpe_dict_update {
        struct xml_metadata xml;
        char *value;
        struct oscap_list* editions;
};
OSCAP_GETTER(const char*, cpe_dict_update, value)
OSCAP_IGETTER_GEN(cpe_dict_edition, cpe_dict_update, editions)

/***********************************/
/* vendor -> product -> version -> update -> edition 
 * */
struct cpe_dict_edition {
        struct xml_metadata xml;
        char *value;
        struct oscap_list* languages;
};
OSCAP_GETTER(const char*, cpe_dict_edition, value)
OSCAP_IGETTER_GEN(cpe_dict_language, cpe_dict_edition, languages)

/***********************************/
/* vendor -> product -> version -> update -> edition -> language
 * */
struct cpe_dict_language {
        struct xml_metadata xml;
        char *value;
};
OSCAP_GETTER(const char*, cpe_dict_language, value)

/* End of variable definitions
 * */
/***************************************************************************/

void print_node(xmlTextReaderPtr reader);

bool cpe_dict_add_item(struct cpe_dict * dict, struct cpe_dictitem * item)
{
	if (dict == NULL || item == NULL)
		return false;

	oscap_list_add(dict->items, item);
	return true;
}

/* str_trim function that trims input string and returns string
 * without white characters as space or tabulator.
 * */
char * str_trim(char *str)
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

/***************************************************************************/
/* Constructors of CPE structures cpe_*<structure>*_new()
 * More info in representive header file.
 * returns the type of <structure>
 */
struct cpe_dict *cpe_dict_new_empty() {

        struct cpe_dict *dict;

	dict = oscap_alloc(sizeof(struct cpe_dict));
	if (dict == NULL)
		return NULL;
	memset(dict, 0, sizeof(struct cpe_dict));

        dict->vendors   = oscap_list_new();
	dict->items     = oscap_list_new();

	return dict;
}

struct cpe_dictitem *cpe_dictitem_new_empty() {
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

struct cpe_dict_vendor *cpe_dictvendor_new_empty() {
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
/* End of CPE structures' contructors
 * */
/***************************************************************************/

/***************************************************************************/
/* Private parsing functions cpe_*<structure>*_parse( xmlTextReaderPtr )
 * More info in representive header file.
 * returns the type of <structure>
 */
struct cpe_dict * cpedict_parse_file(const char *fname) {
        
    xmlTextReaderPtr reader;
    struct cpe_dict *dict;

    reader = xmlReaderForFile(fname, NULL, 0);
    if (reader != NULL) {
        xmlTextReaderRead(reader);
        dict = parse_cpedict(reader);
    } else {
        fprintf(stderr, "Unable to open %s\n", fname);
    }
    xmlFreeTextReader(reader);
    return dict;
}

struct cpe_dict * parse_cpedict(xmlTextReaderPtr reader) {

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
        //ret->vendors = oscap_list_new(); // why this has to be here ? 
        while (next_ret != 0) {
                
            if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "generator")) { // <generator> | count = 1
                        ret->generator = cpe_generator_parse(reader);
            } else 
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "cpe-item")) { // <cpe-item> | cout = 0-n
			if ((item = cpe_dictitem_parse(reader)) == NULL) {
				// something bad happend, let's try to recover and continue
                                // add here some bad nodes list to write it to stdout after parsing is done
                                // get the next node
                                next_ret = xmlTextReaderNextElement(reader);
                                continue;
                        }
                        // We got an item !
			if (!cpe_dict_add_item(ret, item)) {
				//cpe_dictitem_free(item);
				//cpe_dict_free(ret);
				return NULL;
			}
                        continue;
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

struct cpe_generator * cpe_generator_parse(xmlTextReaderPtr reader) {

        struct cpe_generator *ret = NULL;

        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "generator") &&
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

                // we are on "<generator>" element, let's alloc structure
	        ret = oscap_alloc(sizeof(struct cpe_generator));
                if (ret == NULL)
                        return NULL;

                /* We don't have a language or namespace in <generator> elements, otherwise use this code:
                 * ret->xml.lang = strdup((char *) xmlTextReaderConstXmlLang(reader));
                 * ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                 * probably the product_name below will need language specification...
                */
                
                // skip nodes until new element
                xmlTextReaderNextElement(reader);

                while (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST "generator") != 0) {

                        if ((xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                            (const xmlChar *)"product_name") == 0) &&
                            (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)){
			            ret->product_name = (char *) xmlTextReaderReadString(reader);
                        } else 
                            if ((xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"product_version") == 0) &&
                                (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)){
			            ret->product_version = (char *) xmlTextReaderReadString(reader);
                        } else 
                            if ((xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"schema_version") == 0) &&
                                (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)){
			            ret->schema_version = (char *) xmlTextReaderReadString(reader);
                        } else 
                            if ((xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"timestamp") == 0) &&
                                (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)){
			            ret->timestamp = (char *) xmlTextReaderReadString(reader);
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


struct cpe_dictitem * cpe_dictitem_parse(xmlTextReaderPtr reader) {

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
                        
                        if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
                                xmlTextReaderRead(reader);
                                continue;
                        }

                        if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                            (const xmlChar *)"title") == 0) {
                                    title = cpe_dictitem_title_parse(reader, "title");
                                    if (title) oscap_list_add(ret->titles, title);
                        } else 
                            if (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"note") == 0) {
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
				    ref->content = oscap_strdup(str_trim((char *) xmlTextReaderConstValue(reader)));
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
                                    if (data) {
                                        ret->metadata->modification_date = data;
                                        ret->metadata->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                                        ret->metadata->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                                    }

                                    data = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "status");
	                            if (data) ret->metadata->status = data;
                                    data = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "nvd-id");
	                            if (data) ret->metadata->nvd_id = (char *)data;
                                    data = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "deprecated-by-nvd-id");
	                            if (data) ret->metadata->deprecated_by_nvd_id = (char *)data;
                                    else ret->metadata->deprecated_by_nvd_id = NULL;
                                    data = NULL;

                        } else 
                            if ((xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"references") == 0) || 
                                (xmlStrcmp(xmlTextReaderConstLocalName(reader), 
                                (const xmlChar *)"notes") == 0)) {
                                // we just need to jump over this element
                        
                        } else {
                                return ret; // <-- we need to return here, because we don't want to jump to next element 
                        }
                        xmlTextReaderNextElement(reader);
                }
        }

        return ret;
}

struct cpe_dictitem_title * cpe_dictitem_title_parse(xmlTextReaderPtr reader, const char * name) {

	struct cpe_dictitem_title *ret;

        if (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST name) != 0)
		return NULL;

	if ((ret = oscap_alloc(sizeof(struct cpe_dictitem_title))) == NULL)
		return NULL;
	memset(ret, 0, sizeof(struct cpe_dictitem_title));

        ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
        ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);

	ret->content = oscap_strdup(str_trim((char *) xmlTextReaderReadString(reader)));

	return ret;
}

struct cpe_dict_check * cpe_dict_check_parse(xmlTextReaderPtr reader) {

	struct cpe_dict_check *ret;

        if (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST "check") != 0)
		return NULL;

	if ((ret = oscap_alloc(sizeof(struct cpe_dict_check))) == NULL)
		return NULL;
	memset(ret, 0, sizeof(struct cpe_dict_check));

        /* TODO: find out if there should be xml:lang lookup
         * ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
         */ 
        ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
        ret->system = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "system");
        ret->href = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "href");
	ret->identifier = str_trim((char *) xmlTextReaderReadString(reader));

	return ret;
}

struct cpe_dict_vendor * cpe_dict_vendor_parse(xmlTextReaderPtr reader) {


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
        ret->value = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "value");
        
        // jump to next element (which should be product)
        xmlTextReaderNextElement(reader);

        while(xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST "vendor") != 0) {
                        
                        if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
                                xmlTextReaderRead(reader);
                                continue;
                        }

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
                                            product->part = CPE_PART_NONE;
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
                        xmlTextReaderRead(reader);
        }
        return ret;
        
}

/* End of private parsing functions
 * */
/***************************************************************************/

/***************************************************************************/
/* Private exporting functions cpe_*<structure>*_export( xmlTextWriterPtr )
 * More info in representive header file.
 * returns the type of <structure>
 */
void dict_export(const struct cpe_dict * dict, const char * fname) {

        // TODO: ad macro to check return value from xmlTextWriter* functions
        xmlTextWriterPtr writer;

        writer = xmlNewTextWriterFilename(fname, 0);

        // Set properties of writer TODO: make public function to edit this ??
        xmlTextWriterSetIndent(writer, 1);
        xmlTextWriterSetIndentString(writer, BAD_CAST "    ");

        xmlTextWriterStartDocument(writer, NULL, FILE_ENCODING, NULL);

        cpe_dict_export(dict, writer);
        xmlTextWriterEndDocument(writer);
        xmlFreeTextWriter(writer);
}

void cpe_dict_export(const struct cpe_dict * dict, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST dict->xml.namespace, BAD_CAST "cpe-list", NULL);
        // TODO: this shouldn't be hardcoded - find another way !
        xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns", BAD_CAST 
                "http://cpe.mitre.org/dictionary/2.0");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:xsi", BAD_CAST 
                "http://www.w3.org/2001/XMLSchema-instance");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:meta", BAD_CAST 
                "http://scap.nist.gov/schema/cpe-dictionary-metadata/0.2");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "xsi:schemaLocation", BAD_CAST 
                "http://scap.nist.gov/schema/cpe-dictionary-metadata/0.2 http://nvd.nist.gov/schema/cpe-dictionary-metadata_0.2.xsd http://cpe.mitre.org/dictionary/2.0 http://cpe.mitre.org/files/cpe-dictionary_2.1.xsd");
        if (dict->generator != NULL) {
            cpe_generator_export( dict->generator, writer);
        }
        OSCAP_FOREACH (cpe_dictitem, item, cpe_dict_get_items(dict),
		// dump its contents to XML tree
                cpe_dictitem_export( item, writer );
	)
        // TODO: NEED TO HAVE COMPONENT-TREE STRUCTURE TO GET XML-NAMESPACE 
        xmlTextWriterStartElementNS(writer, BAD_CAST "meta", BAD_CAST "component-tree", NULL);
        OSCAP_FOREACH (cpe_dict_vendor, vendor, cpe_dict_get_vendors(dict),
                cpe_dict_vendor_export( vendor, writer );
	)
        xmlTextWriterEndElement(writer);//</component-tree>

        xmlTextWriterEndElement(writer);
}

void cpe_dictitem_export(const struct cpe_dictitem * item, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST item->xml.namespace, BAD_CAST "cpe-item", NULL);
        if (item->name != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "name", BAD_CAST cpe_name_get_uri(item->name));
        if (item->deprecated != NULL) {
                xmlTextWriterWriteAttribute(writer, BAD_CAST "deprecated", BAD_CAST "true");
                xmlTextWriterWriteAttribute(writer, BAD_CAST "deprecation_date", BAD_CAST item->deprecation_date);
                xmlTextWriterWriteAttribute(writer, BAD_CAST "deprecated_by", BAD_CAST cpe_name_get_uri(item->deprecated));
        }
        
        OSCAP_FOREACH (cpe_dictitem_title, title, cpe_dictitem_get_titles(item),
                cpe_title_export( (const struct cpe_title *) title, writer);
	)

        if (item->metadata != NULL) {
                xmlTextWriterStartElementNS(writer, BAD_CAST item->metadata->xml.namespace, BAD_CAST "item-metadata", NULL);
                if (item->metadata->modification_date != NULL)
                        xmlTextWriterWriteAttribute(writer, BAD_CAST "modification-date", BAD_CAST item->metadata->modification_date);
                if (item->metadata->status != NULL)
                        xmlTextWriterWriteAttribute(writer, BAD_CAST "status", BAD_CAST item->metadata->status);
                if (item->metadata->nvd_id != NULL)
                        xmlTextWriterWriteAttribute(writer, BAD_CAST "nvd-id", BAD_CAST item->metadata->nvd_id);
                if (item->metadata->deprecated_by_nvd_id != NULL)
                        xmlTextWriterWriteAttribute(writer, BAD_CAST "deprecated-by-nvd-id", BAD_CAST item->metadata->deprecated_by_nvd_id);
                xmlTextWriterEndElement(writer);
        }

        /*OSCAP_FOREACH (cpe_dict_reference, ref, cpe_dictitem_get_references(item), 
                cpe_dict_reference_export(ref, writer); 
        )*/

        struct oscap_iterator *it = oscap_iterator_new(item->notes);
        if (oscap_iterator_has_more(it)) {
                xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "notes", NULL);
                OSCAP_FOREACH (cpe_dictitem_title, note, cpe_dictitem_get_notes(item), 
                        cpe_dict_note_export(note, writer); 
                )
                xmlTextWriterEndElement(writer);
        }

        OSCAP_FOREACH (cpe_dict_check, check, cpe_dictitem_get_checks(item), 
                cpe_dict_check_export(check, writer); 
        )

        xmlTextWriterEndElement(writer);

}

void cpe_dict_product_export(const struct cpe_dict_product * product, xmlTextWriterPtr writer);
void cpe_dict_version_export(const struct cpe_dict_version * version, xmlTextWriterPtr writer);
void cpe_dict_update_export(const struct cpe_dict_update * update, xmlTextWriterPtr writer);
void cpe_dict_edition_export(const struct cpe_dict_edition * edition, xmlTextWriterPtr writer);
void cpe_dict_language_export(const struct cpe_dict_language * language, xmlTextWriterPtr writer);

void cpe_dict_vendor_export(const struct cpe_dict_vendor * vendor, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST vendor->xml.namespace, BAD_CAST "vendor", NULL);
        if (vendor->value != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "value", BAD_CAST vendor->value);

        OSCAP_FOREACH (cpe_dictitem_title, title, cpe_dict_vendor_get_titles(vendor),
                cpe_title_export( (const struct cpe_title *) title, writer);
	)

        OSCAP_FOREACH (cpe_dict_product, product, cpe_dict_vendor_get_products(vendor),
                cpe_dict_product_export(product, writer);
	)

        xmlTextWriterEndElement(writer);
}

void cpe_dict_product_export(const struct cpe_dict_product * product, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST product->xml.namespace, BAD_CAST "product", NULL);
        if (product->value != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "value", BAD_CAST product->value);
        if (product->part != CPE_PART_NONE)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "part", BAD_CAST PART_TO_CHAR[product->part] );

        OSCAP_FOREACH (cpe_dict_version, version, cpe_dict_product_get_versions(product),
                cpe_dict_version_export(version, writer);
	)

        xmlTextWriterEndElement(writer);
}

void cpe_dict_version_export(const struct cpe_dict_version * version, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST version->xml.namespace, BAD_CAST "version", NULL);
        if (version->value != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "value", BAD_CAST version->value);

        OSCAP_FOREACH (cpe_dict_update, update, cpe_dict_version_get_updates(version),
                cpe_dict_update_export(update, writer);
	)

        xmlTextWriterEndElement(writer);

}

void cpe_dict_update_export(const struct cpe_dict_update * update, xmlTextWriterPtr writer){ 

        xmlTextWriterStartElementNS(writer, BAD_CAST update->xml.namespace, BAD_CAST "update", NULL);
        if (update->value != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "value", BAD_CAST update->value);

        OSCAP_FOREACH (cpe_dict_edition, edition, cpe_dict_update_get_editions(update),
                cpe_dict_edition_export(edition, writer);
	)

        xmlTextWriterEndElement(writer);
}
void cpe_dict_edition_export(const struct cpe_dict_edition * edition, xmlTextWriterPtr writer){

        xmlTextWriterStartElementNS(writer, BAD_CAST edition->xml.namespace, BAD_CAST "edition", NULL);
        if (edition->value != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "value", BAD_CAST edition->value);

        OSCAP_FOREACH (cpe_dict_language, language, cpe_dict_edition_get_languages(edition),
                cpe_dict_language_export(language, writer);
	)

        xmlTextWriterEndElement(writer);
}
void cpe_dict_language_export(const struct cpe_dict_language * language, xmlTextWriterPtr writer){

        xmlTextWriterStartElementNS(writer, BAD_CAST language->xml.namespace, BAD_CAST "language", NULL);
        if (language->value != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "value", BAD_CAST language->value);

        xmlTextWriterEndElement(writer);
}

/*void cpe_dict_reference_export(const struct cpe_dict_reference * reference, xmlTextWriterPtr writer) {
}*/
void cpe_dict_note_export(const struct cpe_dictitem_title * title, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST title->xml.namespace, BAD_CAST "note", NULL);
        if (title->xml.lang != NULL) 
                xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang", BAD_CAST title->xml.lang);
        xmlTextWriterWriteString(writer, BAD_CAST title->content);
        xmlTextWriterEndElement(writer);
}
void cpe_dict_check_export(const struct cpe_dict_check *check, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST check->xml.namespace, BAD_CAST "check", NULL);
        if (check->system != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "system", BAD_CAST check->system);
        if (check->href != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "href", BAD_CAST check->href);
        if (check->identifier != NULL)
                xmlTextWriterWriteString(writer, BAD_CAST check->identifier);
        xmlTextWriterEndElement(writer);
}

void cpe_generator_export(const struct cpe_generator * generator, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST generator->xml.namespace, BAD_CAST "generator", NULL);
        if (generator->product_name != NULL) {
                xmlTextWriterStartElementNS(writer, BAD_CAST generator->xml.namespace, BAD_CAST "product_name", NULL);
                xmlTextWriterWriteString(writer, BAD_CAST generator->product_name);
                xmlTextWriterEndElement(writer); //</product_name>
        }
        if (generator->product_version != NULL) {
                xmlTextWriterStartElementNS(writer, BAD_CAST generator->xml.namespace, BAD_CAST "product_version", NULL);
                xmlTextWriterWriteString(writer, BAD_CAST generator->product_version);
                xmlTextWriterEndElement(writer); //</product_name>
        }
        if (generator->schema_version != NULL) {
                xmlTextWriterStartElementNS(writer, BAD_CAST generator->xml.namespace, BAD_CAST "schema_version", NULL);
                xmlTextWriterWriteString(writer, BAD_CAST generator->schema_version);
                xmlTextWriterEndElement(writer); //</product_name>
        }
        if (generator->timestamp != NULL) {
                xmlTextWriterStartElementNS(writer, BAD_CAST generator->xml.namespace, BAD_CAST "timestamp", NULL);
                xmlTextWriterWriteString(writer, BAD_CAST generator->timestamp);
                xmlTextWriterEndElement(writer); //</product_name>
        }
        xmlTextWriterEndElement(writer); //</gnerator>

}

/**
 * print_node:
 * @reader: the xmlReader
 *
 * Dump information about the current node
 */
void print_node(xmlTextReaderPtr reader) {

    const xmlChar *name, *value;

    name = xmlTextReaderConstName(reader);
    if (name == NULL)
        name = BAD_CAST "--";

    value = xmlTextReaderConstValue(reader);

    printf("%d %d %s %d %d", 
            xmlTextReaderDepth(reader),
            xmlTextReaderNodeType(reader),
            name,
            xmlTextReaderIsEmptyElement(reader),
            xmlTextReaderHasValue(reader));
    if (value == NULL)
        printf("\n");
    else {
        if (xmlStrlen(value) > 40)
            printf(" %.40s...\n", value);
        else
            printf(" %s\n", value);
    }
}
