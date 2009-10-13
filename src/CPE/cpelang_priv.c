/**
 * @file cpelang_priv.c
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
#include <libxml/tree.h>

#include "cpelang_priv.h"

#include <string.h>
#include "cpelang.h"
#include "../common/util.h"
#include "../common/list.h"

struct xml_metadata {
        char *namespace;
        char *lang;
};

struct cpe_lang_expr {
        struct xml_metadata xml;
	cpe_lang_oper_t oper;	// operator
	union {
		struct cpe_lang_expr *expr;	// array of subexpressions for operators
		struct cpe_name *cpe;	// CPE for match operation
	} meta;			// operation metadata
};

struct cpe_platformspec {
        struct xml_metadata xml;
        char *ns_href;
        char *ns_prefix;
	struct oscap_list* items;   // list of items
	struct oscap_htable* item;  // item by ID
};

struct cpe_platform {
        struct xml_metadata xml;
	char *id;                   // platform ID
	char *title;                // human-readable platform description TODO: 0-n !!
	char *remark;               // remark TODO: 0-n !!
	struct cpe_lang_expr expr;	// expression for match evaluation
};

void print_node(xmlTextReaderPtr reader);

/*
 * Function that jump to next XML element.
 */
int xmlTextReaderNextElement(xmlTextReaderPtr reader) {

        int ret;
        do { 
              ret = xmlTextReaderRead(reader); 
              if (ret == 0) break;
        } while (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);
        return ret;
}

void parse_file(const char *fname) {
        
    xmlTextReaderPtr reader;

    reader = xmlReaderForFile(fname, NULL, 0);
    if (reader != NULL) {
        xmlTextReaderRead(reader);
        //print_node(reader);
        (void) parse_platformspec(reader);
    } else {
        fprintf(stderr, "Unable to open %s\n", fname);
    }
    xmlFreeTextReader(reader);
}

struct cpe_platformspec *cpe_platformspec_new_empty()
{
	struct cpe_platformspec *res;

	res = oscap_alloc(sizeof(struct cpe_platformspec));
	if (res == NULL)
		return NULL;

	res->items = oscap_list_new();
	res->item = oscap_htable_new();
        res->ns_href = NULL;
        res->ns_prefix = NULL;

	return res;
}

struct cpe_platformspec * parse_platformspec(xmlTextReaderPtr reader) {

        struct cpe_platformspec *ret = NULL;

        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "platform-specification") &&
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

                ret = cpe_platformspec_new_empty();
                if (ret == NULL)
                        return NULL;

                ret->ns_href = (char *) xmlTextReaderConstNamespaceUri(reader);
                ret->ns_prefix = (char *) xmlTextReaderConstPrefix(reader);
                ret->xml.lang = (char *) xmlTextReaderConstXmlLang(reader);
                ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                
                // skip nodes until new element
                xmlTextReaderNextElement(reader);

                while (xmlStrcmp (xmlTextReaderConstLocalName(reader), (const xmlChar *)"platform") == 0) {
                        
                        //print_node(reader);
                        parse_platform(reader);
                        xmlTextReaderNextElement(reader);
                }
                //print_node(reader);
        }

        return ret;
}

struct cpe_platform * parse_platform(xmlTextReaderPtr reader) {
        
        struct cpe_platform *ret;

        // allocate platform structure here
	ret = oscap_alloc(sizeof(struct cpe_platform));
	if (ret == NULL)
		return NULL;
	memset(ret, 0, sizeof(struct cpe_platform));

        // parse platform attributes here
        ret->id = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "id");
        ret->xml.lang = (char *) xmlTextReaderConstXmlLang(reader);
        ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
        if (ret->id == NULL)
                return NULL;    // if there is no "id" in platform element, return NULL

        // skip from <platform> node to next one
        xmlTextReaderRead(reader);

        // while we have element that is not "platform", it is inside this element, otherwise it's ended 
        // element </platform> and we should end. If there is no one from "if" statement cases, we are parsing
        // attribute or text ,.. and we can continue to next node.
        while (xmlStrcmp (xmlTextReaderConstLocalName(reader), (const xmlChar *)"platform") != 0) {
                
            if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "title") &&
                xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT){
                        ret->title = parse_text_element(reader, "title"); // TODO: 0-n titles !
            } else 
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "remark") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->remark = parse_text_element(reader, "remark"); // TODO: 0-n remarks !
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "logical-test") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->expr = * (parse_ret_expr(reader));
            }
            // get the next node
            xmlTextReaderRead(reader);
        }

        return ret;
}

struct cpe_lang_expr * parse_ret_expr(xmlTextReaderPtr reader) {

	xmlChar *temp;
	size_t elem_cnt = 0;
        struct cpe_lang_expr * ret;

        // allocation
        ret = oscap_alloc(sizeof(struct cpe_lang_expr));
        if (ret == NULL)
                return NULL;
        ret->meta.expr = NULL;
        //print_node(reader);
        
        // it's fact-ref only, fill the structure and return it
        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "fact-ref") && 
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                ret->oper = CPE_LANG_OPER_MATCH;
                temp = xmlTextReaderGetAttribute(reader, BAD_CAST "name");
                ret->meta.cpe = cpe_name_new((char *)temp);
                xmlFree(temp);
                ret->xml.lang = (char *) xmlTextReaderConstXmlLang(reader);
                ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                return ret;
        } else {
                // it's logical-test, fill the structure and go to next node
                
	        temp = xmlTextReaderGetAttribute(reader, BAD_CAST "operator");
                //printf("Logical-test operator: %s\n", temp);
	        if (xmlStrcasecmp(temp, BAD_CAST "AND") == 0)
		        ret->oper = CPE_LANG_OPER_AND;
	        else if (xmlStrcasecmp(temp, BAD_CAST "OR") == 0)
		        ret->oper = CPE_LANG_OPER_OR;
	        else {
                        // unknown operator problem
		        xmlFree(temp);
		        oscap_free(ret);
		        return NULL;
	        }
	        xmlFree(temp);

	        temp = xmlTextReaderGetAttribute(reader, BAD_CAST "negate");
	        if (temp && xmlStrcasecmp(temp, BAD_CAST "TRUE") == 0)
		        ret->oper |= CPE_LANG_OPER_NOT;
	        xmlFree(temp);
        }

        // go to next node
        // skip nodes until new element
        xmlTextReaderNextElement(reader);
	ret->meta.expr = oscap_alloc(sizeof(struct cpe_lang_expr));

        // while it's not 'logical-test' or it's not ended element ..
        while (xmlStrcmp(xmlTextReaderConstLocalName(reader), (const xmlChar *) "logical-test") != 0 ||
               xmlTextReaderNodeType(reader) != XML_READER_TYPE_END_ELEMENT) {

                // realloc the current structure to handle more fact-refs or logical-tests
	        ret->meta.expr = (struct cpe_lang_expr *) oscap_realloc(ret->meta.expr, ++elem_cnt * sizeof(struct cpe_lang_expr));

                // .. and the next node is logical-test element, we need recursive call
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "logical-test") && 
                xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        //printf("Recursive call: "); print_node(reader);
                        ret->meta.expr[elem_cnt-1] = *(parse_ret_expr(reader));
                } else // .. or it's fact-ref only
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "fact-ref") &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                // fill the structure
                                //print_node(reader);
                                ret->meta.expr[elem_cnt-1].oper = CPE_LANG_OPER_MATCH;
                                temp = xmlTextReaderGetAttribute(reader, BAD_CAST "name");
                                ret->meta.expr[elem_cnt-1].meta.cpe = cpe_name_new((char *)temp);
                                xmlFree(temp);
                }
                xmlTextReaderRead(reader);
        }

        return ret;
}

char * parse_text_element(xmlTextReaderPtr reader, char *name) {

    char *string = NULL;

    // parse string element attributes here (like xml:lang)

    //print_node(reader);
    while (xmlTextReaderRead(reader)) {
        if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT &&
            !xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST name)) {
                //printf("New parsed string: %s\n", string);
                return string;
        }
        
        switch (xmlTextReaderNodeType(reader)) {
            case XML_READER_TYPE_TEXT:
                    string = (char *)xmlTextReaderValue(reader);
                    break;
        }
    }
    //printf("New parsed string: %s\n", string);
    fflush(stdout);
    return string;
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
