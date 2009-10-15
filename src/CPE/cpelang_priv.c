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
#include <libxml/xmlwriter.h>
#include <libxml/tree.h>

#include "cpelang_priv.h"

#include <string.h>
#include "cpelang.h"
#include "../common/util.h"
#include "../common/list.h"

#define FILE_ENCODING "UTF-8"

struct xml_metadata {
        char *namespace;
        char *lang;
};

struct cpe_title {
        struct xml_metadata xml;
	char *content;		        // human-readable title
};

struct cpe_lang_expr {
        struct xml_metadata xml;
	cpe_lang_oper_t oper;	// operator
	union {
		struct cpe_lang_expr *expr;	// array of subexpressions for operators
		struct cpe_name *cpe;	        // CPE for match operation
	} meta;			                // operation metadata
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
	struct oscap_list* titles;   // human-readable platform description
	char *id;                   // platform ID
	char *remark;               // remark TODO: 0-n !!
	struct cpe_lang_expr expr;  // expression for match evaluation
};

OSCAP_GETTER(const char*, cpe_platform, id)
OSCAP_GETTER(const char*, cpe_platform, remark)
OSCAP_IGETTER_GEN(cpe_title, cpe_platform, titles)
OSCAP_IGETTER_GEN(cpe_platform, cpe_platformspec, items)
OSCAP_HGETTER_STRUCT(cpe_platform, cpe_platformspec, item)

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

struct cpe_platformspec * parse_file(const char *fname) {
        
    xmlTextReaderPtr reader;
    struct cpe_platformspec *ret = NULL;

    reader = xmlReaderForFile(fname, NULL, 0);
    if (reader != NULL) {
        xmlTextReaderRead(reader);
        ret = parse_platformspec(reader);
    } else {
        fprintf(stderr, "Unable to open %s\n", fname);
    }
    xmlFreeTextReader(reader);

    return ret;
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

struct cpe_platform * cpe_platform_new_empty()
{
	struct cpe_platform *ret;

	ret = oscap_alloc(sizeof(struct cpe_platform));
	if (ret == NULL)
		return NULL;

	ret->titles = oscap_list_new();

	return ret;
}

struct cpe_platformspec * parse_platformspec(xmlTextReaderPtr reader) {

        struct cpe_platformspec *ret = NULL;
        struct cpe_platform *platform;

        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "platform-specification") &&
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

                ret = cpe_platformspec_new_empty();
                if (ret == NULL)
                        return NULL;

                ret->ns_href = oscap_strdup((char *) xmlTextReaderConstNamespaceUri(reader));
                ret->ns_prefix = oscap_strdup((char *) xmlTextReaderConstPrefix(reader));
                ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                
                // skip nodes until new element
                xmlTextReaderNextElement(reader);

                while (xmlStrcmp (xmlTextReaderConstLocalName(reader), (const xmlChar *)"platform") == 0) {
                        
                        platform = parse_platform(reader);
                        if (platform) oscap_list_add(ret->items, platform);
                        xmlTextReaderNextElement(reader);
                }
        }

        return ret;
}

struct cpe_platform * parse_platform(xmlTextReaderPtr reader) {
        
        struct cpe_platform *ret;
        struct cpe_title *title;

        // allocate platform structure here
        ret = cpe_platform_new_empty();
	if (ret == NULL)
		return NULL;

        // parse platform attributes here
        ret->id = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "id");
        ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
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
                        title = cpe_title_parse(reader, "title");
                        if (title) oscap_list_add(ret->titles, title);
            } else 
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "remark") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->remark = parse_text_element(reader, "remark"); // TODO: 0-n remarks !
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "logical-test") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->expr = *(parse_ret_expr(reader));
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
        //print_node(reader);
        
        // it's fact-ref only, fill the structure and return it
        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "fact-ref") && 
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                ret->oper = CPE_LANG_OPER_MATCH;
                temp = xmlTextReaderGetAttribute(reader, BAD_CAST "name");
                ret->meta.cpe = cpe_name_new((char *)temp);
                xmlFree(temp);
                ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
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

                ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);

	        temp = xmlTextReaderGetAttribute(reader, BAD_CAST "negate");
	        if (temp && xmlStrcasecmp(temp, BAD_CAST "TRUE") == 0)
		        ret->oper |= CPE_LANG_OPER_NOT;
	        xmlFree(temp);
        }

        // go to next node
        // skip to next node
        xmlTextReaderRead(reader);
	ret->meta.expr = oscap_alloc(sizeof(struct cpe_lang_expr));

        // while it's not 'logical-test' or it's not ended element ..
        while (xmlStrcmp(xmlTextReaderConstLocalName(reader), (const xmlChar *) "logical-test") != 0 ||
               xmlTextReaderNodeType(reader) != XML_READER_TYPE_END_ELEMENT) {

                if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
                        xmlTextReaderRead(reader);
                        continue;
                }
                elem_cnt++;
                // realloc the current structure to handle more fact-refs or logical-tests
	        ret->meta.expr = (struct cpe_lang_expr *) oscap_realloc(ret->meta.expr, (elem_cnt+1) * sizeof(struct cpe_lang_expr));

                // .. and the next node is logical-test element, we need recursive call
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "logical-test") && 
                xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->meta.expr[elem_cnt-1] = *(parse_ret_expr(reader));
                } else // .. or it's fact-ref only
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "fact-ref") &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                // fill the structure
                                ret->meta.expr[elem_cnt-1].oper = CPE_LANG_OPER_MATCH;
                                temp = xmlTextReaderGetAttribute(reader, BAD_CAST "name");
                                ret->meta.expr[elem_cnt-1].meta.cpe = cpe_name_new((char *)temp);
                                xmlFree(temp);
                                ret->meta.expr[elem_cnt-1].xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                                ret->meta.expr[elem_cnt-1].xml.namespace = (char *) xmlTextReaderPrefix(reader);
                }
                xmlTextReaderRead(reader);
        }
        ret->meta.expr[elem_cnt].oper = CPE_LANG_OPER_HALT;

        return ret;
}

char * parse_text_element(xmlTextReaderPtr reader, char *name) {

    char *string = NULL;

    // parse string element attributes here (like xml:lang)

    while (xmlTextReaderRead(reader)) {
        if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT &&
            !xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST name)) {
                return string;
        }
        
        switch (xmlTextReaderNodeType(reader)) {
            case XML_READER_TYPE_TEXT:
                    string = (char *)xmlTextReaderValue(reader);
                    break;
        }
    }
    return string;
}

static struct cpe_title * cpe_title_parse(xmlTextReaderPtr reader, char * name) {

	struct cpe_title *ret;

        if (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST name) != 0)
		return NULL;

	if ((ret = oscap_alloc(sizeof(struct cpe_title))) == NULL)
		return NULL;
	memset(ret, 0, sizeof(struct cpe_title));

        ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
        ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);

	ret->content = (char *) xmlTextReaderReadString(reader);

	return ret;
}


void cpe_lang_export(const struct cpe_platformspec * spec, const char * fname) {

        // TODO: ad macro to check return value from xmlTextWriter* functions
        xmlTextWriterPtr writer;

        writer = xmlNewTextWriterFilename(fname, 0);

        // Set properties of writer TODO: make public function to edit this ??
        xmlTextWriterSetIndent(writer, 1);
        xmlTextWriterSetIndentString(writer, BAD_CAST "    ");

        xmlTextWriterStartDocument(writer, NULL, FILE_ENCODING, NULL);

        cpe_platformspec_export2(spec, writer);
        xmlTextWriterEndDocument(writer);
        xmlFreeTextWriter(writer);
}

void cpe_platformspec_export2(const struct cpe_platformspec * spec, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST spec->ns_prefix, BAD_CAST "platform-specification", BAD_CAST spec->ns_href);
        OSCAP_FOREACH (cpe_platform, p, cpe_platformspec_get_items(spec),
		// dump its contents to XML tree
                cpe_platform_export( p, writer );
	)
        xmlTextWriterEndElement(writer);
}

void cpe_platform_export(const struct cpe_platform * platform, xmlTextWriterPtr writer) {
    
        xmlTextWriterStartElementNS(writer, BAD_CAST platform->xml.namespace, BAD_CAST "platform", NULL);
        if (cpe_platform_get_id(platform) != NULL) 
                xmlTextWriterWriteAttribute(writer, BAD_CAST "id", BAD_CAST cpe_platform_get_id(platform));
        OSCAP_FOREACH (cpe_title, title, cpe_platform_get_titles(platform),
		// dump its contents to XML tree
                cpe_title_export(title, writer);
	)
        cpe_ret_expr_export(platform->expr, writer);
        xmlTextWriterEndElement(writer);
}

void cpe_title_export(const struct cpe_title * title, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST title->xml.namespace, BAD_CAST "title", NULL);
        if (title->xml.lang != NULL) 
                xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang", BAD_CAST title->xml.lang);
        xmlTextWriterWriteString(writer, BAD_CAST title->content);
        xmlTextWriterEndElement(writer);
}

void cpe_ret_expr_export(struct cpe_lang_expr expr, xmlTextWriterPtr writer) {

        if (expr.oper == CPE_LANG_OPER_HALT)
            return;

        if (expr.oper == CPE_LANG_OPER_MATCH) {
                xmlTextWriterStartElementNS(writer, BAD_CAST expr.xml.namespace, BAD_CAST "fact-ref", NULL);
                xmlTextWriterWriteAttribute(writer, BAD_CAST "name", BAD_CAST cpe_name_get_uri(expr.meta.cpe));
                xmlTextWriterEndElement(writer);
                return;
        } else {
                xmlTextWriterStartElementNS(writer, BAD_CAST expr.xml.namespace, BAD_CAST "logical-test", NULL);
        }

        if (expr.oper == CPE_LANG_OPER_AND) {
                xmlTextWriterWriteAttribute(writer, BAD_CAST "operator", BAD_CAST "AND");
                xmlTextWriterWriteAttribute(writer, BAD_CAST "negate", BAD_CAST "FALSE");
        } else if (expr.oper == CPE_LANG_OPER_OR) {
                xmlTextWriterWriteAttribute(writer, BAD_CAST "operator", BAD_CAST "OR");
                xmlTextWriterWriteAttribute(writer, BAD_CAST "negate", BAD_CAST "FALSE");
        } else if (expr.oper == CPE_LANG_OPER_NOR) {
                xmlTextWriterWriteAttribute(writer, BAD_CAST "operator", BAD_CAST "OR");
                xmlTextWriterWriteAttribute(writer, BAD_CAST "negate", BAD_CAST "TRUE");
        } else if (expr.oper == CPE_LANG_OPER_NAND) {
                xmlTextWriterWriteAttribute(writer, BAD_CAST "operator", BAD_CAST "AND");
                xmlTextWriterWriteAttribute(writer, BAD_CAST "negate", BAD_CAST "TRUE");
        } else {
            // CPE_LANG_OPER_MATCH or raise exception ! We are dumped !
        }

        if ( expr.meta.expr == NULL) return;
        int i = 0;
        while (expr.meta.expr[i].oper != CPE_LANG_OPER_HALT){
            cpe_ret_expr_export(expr.meta.expr[i], writer);
            ++i;
        }
        xmlTextWriterEndElement(writer);

}
