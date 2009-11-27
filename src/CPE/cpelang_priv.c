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
#include <string.h>

#include "cpelang_priv.h"
#include "public/cpelang.h"

#include "../common/util.h"
#include "../common/list.h"
#include "../common/elements.h"

/***************************************************************************/
/* Variable definitions
 * */
/*
 * */

OSCAP_GETTER(cpe_lang_oper_t, cpe_testexpr, oper)
//OSCAP_GENERIC_GETTER(struct cpe_testexpr *, cpe_testexpr, meta_expr, meta.expr)
//OSCAP_GENERIC_GETTER(struct cpe_name *, cpe_testexpr, meta_cpe, meta.cpe)

/*
 * */
struct cpe_lang_model {
        struct xml_metadata xml;
        struct oscap_list * xmlns;
        char *ns_href;
        char *ns_prefix;
	struct oscap_list* platforms;   // list of items
	struct oscap_htable* item;  // item by ID
};
OSCAP_IGETINS(xml_metadata, cpe_lang_model, xmlns, xml)
OSCAP_ACCESSOR_STRING(cpe_lang_model, ns_href)
OSCAP_ACCESSOR_STRING(cpe_lang_model, ns_prefix)
OSCAP_IGETTER_GEN(cpe_platform, cpe_lang_model, platforms)
OSCAP_HGETTER_STRUCT(cpe_platform, cpe_lang_model, item)

/*
 * */
struct cpe_platform {
        struct xml_metadata xml;
	struct oscap_list* titles;   // human-readable platform description
	char *id;                   // platform ID
	char *remark;               // remark TODO: 0-n !!
	struct cpe_testexpr expr;  // expression for match evaluation
};

OSCAP_ACCESSOR_STRING(cpe_platform, id)
OSCAP_ACCESSOR_STRING(cpe_platform, remark)
OSCAP_IGETINS(oscap_title, cpe_platform, titles, title)

/* End of variable definitions
 * */
/***************************************************************************/


/***************************************************************************/
/* Declaration of static (private to this file) functions
 * These function shoud not be called from outside. For exporting these elements
 * has to call parent element's 
 */
static int xmlTextReaderNextElement(xmlTextReaderPtr reader);
static char * parse_text_element(xmlTextReaderPtr reader, char *name);
static bool cpe_validate_xml(const char * filename);

/* End of static declarations 
 * */
/***************************************************************************/

/* Function that jump to next XML starting element.
 */
static int xmlTextReaderNextElement(xmlTextReaderPtr reader) {

        int ret;
        do { 
              ret = xmlTextReaderRead(reader); 
              // if end of file
              if (ret == 0) break;
        } while (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);
        return ret;
}

const struct cpe_testexpr * cpe_platform_get_expr(const struct cpe_platform *item) {

    if (item == NULL)
                return NULL;

    return &(item->expr);
}

const struct cpe_testexpr * cpe_testexpr_get_next(const struct cpe_testexpr * expr) {

    return ++(expr);
}

/***************************************************************************/
/* Constructors of CPE structures cpe_*<structure>*_new()
 * More info in representive header file.
 * returns the type of <structure>
 */

struct cpe_testexpr * cpe_testexpr_new() {

        struct cpe_testexpr *ret;

        ret = oscap_alloc(sizeof(struct cpe_testexpr));
        if (ret == NULL)
                return NULL;

        ret->meta.expr      = NULL;
        ret->meta.cpe       = NULL;

        ret->xml.lang       = NULL;
        ret->xml.namespace  = NULL;
        ret->xml.URI        = NULL;

        return ret;
}

struct cpe_lang_model * cpe_lang_model_new() {

	struct cpe_lang_model *ret;

	ret = oscap_alloc(sizeof(struct cpe_lang_model));
	if (ret == NULL)
		return NULL;

	ret->platforms      = oscap_list_new();
	ret->item           = oscap_htable_new();
        ret->xmlns          = oscap_list_new();
        ret->ns_href        = NULL;
        ret->ns_prefix      = NULL;
        ret->xml.lang       = NULL;
        ret->xml.namespace  = NULL;
        ret->xml.URI        = NULL;

	return ret;
}

struct cpe_platform * cpe_platform_new() {

	struct cpe_platform *ret;

	ret = oscap_alloc(sizeof(struct cpe_platform));
	if (ret == NULL)
		return NULL;

	ret->titles         = oscap_list_new();
	ret->expr           = *(cpe_testexpr_new());
        ret->xml.lang       = NULL;
        ret->xml.namespace  = NULL;
        ret->xml.URI        = NULL;
	ret->id             = NULL;
	ret->remark         = NULL;

	return ret;
}
/* End of CPE structures' contructors
 * */
/***************************************************************************/

/***************************************************************************/
/* Private parsing functions cpe_*<structure>*_parse( xmlTextReaderPtr )
 * More info in representive header file.
 * returns the type of <structure>
 */

static bool cpe_validate_xml(const char * filename) {

	xmlParserCtxtPtr ctxt;	/* the parser context */
	xmlDocPtr doc;		/* the resulting document tree */
	bool ret = false;

	/* create a parser context */
	ctxt = xmlNewParserCtxt();
	if (ctxt == NULL)
		return false;
	/* parse the file, activating the DTD validation option */
	doc = xmlCtxtReadFile(ctxt, filename, NULL, XML_PARSE_DTDATTR);
	/* check if parsing suceeded */
	if (doc == NULL) {
		xmlFreeParserCtxt(ctxt);
		return false;
	}
	/* check if validation suceeded */
	if (ctxt->valid)
		ret = true;
	xmlFreeDoc(doc);
	/* free up the parser context */
	xmlFreeParserCtxt(ctxt);
	return ret;
}

struct cpe_lang_model * cpe_lang_model_parse_xml(const struct oscap_import_source * source) {
        
        xmlTextReaderPtr reader;
        struct cpe_lang_model *ret = NULL;

        if (!cpe_validate_xml(oscap_import_source_get_filename(source))) return NULL;

        reader = xmlReaderForFile(oscap_import_source_get_filename(source),
                                  oscap_import_source_get_encoding(source), 0);
        if (reader != NULL) {
            xmlTextReaderRead(reader);
            ret = cpe_lang_model_parse(reader);
        } else {
            fprintf(stderr, "Unable to open %s\n", oscap_import_source_get_filename(source));
        }
        xmlFreeTextReader(reader);

        return ret;
}

struct cpe_lang_model * cpe_lang_model_parse(xmlTextReaderPtr reader) {

        struct cpe_lang_model *ret      = NULL;
        struct xml_metadata *xml        = NULL;
        struct cpe_platform *platform   = NULL;

        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "platform-specification") &&
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

                ret = cpe_lang_model_new();
                if (ret == NULL)
                        return NULL;

                ret->ns_prefix = oscap_strdup((char *) xmlTextReaderConstPrefix(reader));
                ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);

                /* Reading XML namespaces */
                if (xmlTextReaderHasAttributes(reader) && xmlTextReaderMoveToFirstAttribute(reader)==1) {
                        do {
                                xml = oscap_alloc(sizeof(struct xml_metadata));
                                xml->lang = NULL;
                                xml->namespace = oscap_strdup((char *) xmlTextReaderConstName(reader));
                                xml->URI = oscap_strdup((char *) xmlTextReaderConstValue(reader));
                                oscap_list_add(ret->xmlns, xml);
                        } while (xmlTextReaderMoveToNextAttribute(reader)==1);
                }
                
                // skip nodes until new element
                xmlTextReaderNextElement(reader);

                while (xmlStrcmp (xmlTextReaderConstLocalName(reader), (const xmlChar *)"platform") == 0) {
                        
                        platform = cpe_platform_parse(reader);
                        if (platform) cpe_lang_model_add_platform(ret, platform);
                        xmlTextReaderNextElement(reader);
                }
        }

        return ret;
}

struct cpe_platform * cpe_platform_parse(xmlTextReaderPtr reader) {
        
        struct cpe_platform *ret;
        struct oscap_title *title;

        // allocate platform structure here
        ret = cpe_platform_new();
	if (ret == NULL)
		return NULL;

        // parse platform attributes here
        ret->id = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "id");
        ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
        ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
        if (ret->id == NULL) {
                cpe_platform_free(ret);
                return NULL;    // if there is no "id" in platform element, return NULL
        }

        // skip from <platform> node to next one
        xmlTextReaderRead(reader);

        // while we have element that is not "platform", it is inside this element, otherwise it's ended 
        // element </platform> and we should end. If there is no one from "if" statement cases, we are parsing
        // attribute or text ,.. and we can continue to next node.
        while (xmlStrcmp (xmlTextReaderConstLocalName(reader), (const xmlChar *)"platform") != 0) {
                
            if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "title") &&
                xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT){
                        title = oscap_title_parse(reader, "title");
                        if (title) oscap_list_add(ret->titles, title);
            } else 
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "remark") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->remark = parse_text_element(reader, "remark"); // TODO: 0-n remarks !
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "logical-test") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->expr = *(cpe_testexpr_parse(reader));
            }
            // get the next node
            xmlTextReaderRead(reader);
        }

        return ret;
}

struct cpe_testexpr * cpe_testexpr_parse(xmlTextReaderPtr reader) {

	xmlChar *temp;
	size_t elem_cnt = 0;
        struct cpe_testexpr * ret;

        // allocation
        ret = cpe_testexpr_new();
        if (ret == NULL)
                return NULL;
        
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
	ret->meta.expr = cpe_testexpr_new();

        // while it's not 'logical-test' or it's not ended element ..
        while (xmlStrcmp(xmlTextReaderConstLocalName(reader), (const xmlChar *) "logical-test") != 0 ||
               xmlTextReaderNodeType(reader) != XML_READER_TYPE_END_ELEMENT) {

                if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
                        xmlTextReaderRead(reader);
                        continue;
                }
                elem_cnt++;
                // realloc the current structure to handle more fact-refs or logical-tests
	        ret->meta.expr = (struct cpe_testexpr *) oscap_realloc(ret->meta.expr, (elem_cnt+1) * sizeof(struct cpe_testexpr));

                // .. and the next node is logical-test element, we need recursive call
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "logical-test") && 
                xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->meta.expr[elem_cnt-1] = *(cpe_testexpr_parse(reader));
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
                                ret->meta.expr[elem_cnt-1].xml.URI = NULL;
                }
                xmlTextReaderRead(reader);
        }
        ret->meta.expr[elem_cnt].oper = CPE_LANG_OPER_HALT;

        return ret;
}

static char * parse_text_element(xmlTextReaderPtr reader, char *name) {

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

/* End of private parsing functions
 * */
/***************************************************************************/

/***************************************************************************/
/* Private exporting functions cpe_*<structure>*_export( xmlTextWriterPtr )
 * More info in representive header file.
 * returns the type of <structure>
 */
void cpe_lang_model_export_xml(struct cpe_lang_model * spec, struct oscap_export_target * target) {

        // TODO: ad macro to check return value from xmlTextWriter* functions
        xmlTextWriterPtr writer;

        writer = xmlNewTextWriterFilename(oscap_export_target_get_filename(target), 0);

        // Set properties of writer TODO: make public function to edit this ??
        xmlTextWriterSetIndent(writer, oscap_export_target_get_indent(target));
        xmlTextWriterSetIndentString(writer, BAD_CAST oscap_export_target_get_indent_string(target));

        if ( xmlFindCharEncodingHandler(oscap_export_target_get_encoding(target)) == NULL ) 
            // forced default encoding
            xmlTextWriterStartDocument(writer, NULL, NULL, NULL);
            else xmlTextWriterStartDocument(writer, NULL, oscap_export_target_get_encoding(target), NULL);

        cpe_lang_export(spec, writer);
        xmlTextWriterEndDocument(writer);
        xmlFreeTextWriter(writer);
}

void cpe_lang_export(const struct cpe_lang_model * spec, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST spec->ns_prefix, BAD_CAST "platform-specification", BAD_CAST spec->ns_href);

        OSCAP_FOREACH (xml_metadata, xml, cpe_lang_model_get_xmlns(spec),
                if (xml->URI != NULL) xmlTextWriterWriteAttribute(writer, BAD_CAST xml->namespace, BAD_CAST xml->URI);
        )

        OSCAP_FOREACH (cpe_platform, p, cpe_lang_model_get_platforms(spec),
		// dump its contents to XML tree
                cpe_platform_export( p, writer );
	)
        xmlTextWriterEndElement(writer);
}

void cpe_platform_export(const struct cpe_platform * platform, xmlTextWriterPtr writer) {
    
        xmlTextWriterStartElementNS(writer, BAD_CAST platform->xml.namespace, BAD_CAST "platform", NULL);
        if (cpe_platform_get_id(platform) != NULL) 
                xmlTextWriterWriteAttribute(writer, BAD_CAST "id", BAD_CAST cpe_platform_get_id(platform));
        OSCAP_FOREACH (oscap_title, title, cpe_platform_get_titles(platform),
		// dump its contents to XML tree
                oscap_title_export(title, writer);
	)
        cpe_testexpr_export(platform->expr, writer);
        xmlTextWriterEndElement(writer);
}

void cpe_testexpr_export(struct cpe_testexpr expr, xmlTextWriterPtr writer) {

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
            cpe_testexpr_export(expr.meta.expr[i], writer);
            ++i;
        }
        xmlTextWriterEndElement(writer);

}
/* End of private export functions
 * */
/***************************************************************************/

/***************************************************************************/
/* Free functions - all are static private, do not use them outside this file
 */

void cpe_lang_model_free(struct cpe_lang_model * platformspec)
{
	if (platformspec == NULL) return;

	oscap_htable_free(platformspec->item, NULL);
	oscap_list_free(platformspec->platforms, (oscap_destruct_func)cpe_platform_free);
        oscap_list_free(platformspec->xmlns, (oscap_destruct_func)xml_metadata_free);
        xml_metadata_free(&platformspec->xml);
	oscap_free(platformspec);
}

void cpe_platform_free(struct cpe_platform * platform)
{
	if (platform == NULL) return;

	xmlFree(platform->id);
	xmlFree(platform->remark);
        oscap_list_free(platform->titles, (oscap_destruct_func)oscap_title_free);
	cpe_testexpr_free(&platform->expr);
        xml_metadata_free(&platform->xml);
	oscap_free(platform);
}

void cpe_testexpr_free(struct cpe_testexpr * expr)
{
	struct cpe_testexpr *cur;

	if (expr == NULL) return;

	switch (expr->oper & CPE_LANG_OPER_MASK) {
	case CPE_LANG_OPER_AND:
	case CPE_LANG_OPER_OR:
		for (cur = expr->meta.expr; cur->oper; ++cur)
			cpe_testexpr_free(cur);
		oscap_free(expr->meta.expr);
		break;
	case CPE_LANG_OPER_MATCH:
		cpe_name_free(expr->meta.cpe);
		break;
	default:
		break;
	}

        xml_metadata_free(&expr->xml);
	expr->oper = 0;
}
/* End of free functions
 * */
/***************************************************************************/

/* **************************************************************************
 * Getters / setters / adders (not generated)
 */

struct cpe_testexpr *cpe_testexpr_get_meta_expr(const struct cpe_testexpr *item)
{
	if (item == NULL || (item->oper & 0x03) == 0) return NULL;
	return item->meta.expr;
}

struct cpe_name *cpe_testexpr_get_meta_cpe(const struct cpe_testexpr *item)
{
	if (item == NULL || (item->oper & CPE_LANG_OPER_MASK) != CPE_LANG_OPER_MATCH) return NULL;
	return item->meta.cpe;
}


bool cpe_lang_model_add_platform(struct cpe_lang_model *lang, struct cpe_platform *platform)
{
	if (lang == NULL || platform == NULL || platform->id == NULL) return false;
	oscap_list_add(lang->platforms, platform);
	oscap_htable_add(lang->item, platform->id, platform);
	return true;
}

void cpe_platform_iterator_remove(struct cpe_platform_iterator *it, struct cpe_lang_model *parent)
{
	struct cpe_platform *plat = oscap_iterator_detach((struct oscap_iterator *) it);
	if (plat) {
		oscap_htable_detach(parent->item, cpe_platform_get_id(plat));
		cpe_platform_free(plat);
	}
}

