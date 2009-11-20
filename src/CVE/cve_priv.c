/*! \file cve_priv.c
 *  \brief Common Vulnerability and Exposure dictionary
 * 
 *   See details at:
 *     http://cve.mitre.org/
 *     http://nvd.nist.gov/download.cfm
 *  
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

#include "cve_priv.h"
#include "public/cve.h"

#include "cpelang_priv.h"
#include "cvss_priv.h"
#include "cvss.h"

#include "../list.h"



/***************************************************************************/
/* Variable definitions
 * */

/**
 * @structure cve_list
 * cve_list is the top level element of the CVE List provided by MITRE. 
 * It represents holds all CVE Items.
 */
struct cve_model {

        struct xml_metadata xml;
	struct oscap_list* entries; /* 1-n */
};
OSCAP_IGETINS_GEN(cve_entry, cve_model, entries, entry)

/*
 */
struct cve_reference {

        struct xml_metadata xml;
	char *value;		/* summary          */
	char *href;		/* href             */
	char *type;		/* reference type   */
	char *source;		/* source           */
};
OSCAP_ACCESSOR_STRING(cve_reference, value)
OSCAP_ACCESSOR_STRING(cve_reference, href)
OSCAP_ACCESSOR_STRING(cve_reference, type)
OSCAP_ACCESSOR_STRING(cve_reference, source)

/*
 */
struct cve_summary {

        struct xml_metadata xml;
	char *summary;
};
OSCAP_ACCESSOR_STRING(cve_summary, summary)

/*
 */
struct cve_product {

        struct xml_metadata xml;
        char *value;
};
OSCAP_ACCESSOR_STRING(cve_product, value)

/*
 */
struct cwe_entry {

        struct xml_metadata xml;
        char *value;
};
OSCAP_ACCESSOR_STRING(cwe_entry, value)

/*
 */
struct cve_configuration {

        struct xml_metadata xml;
        char *id;
	struct cpe_testexpr expr;       /* [cpe:lang] expression (0-1) */
};
OSCAP_ACCESSOR_STRING(cve_configuration, id)

/*
 */
struct cve_entry {

        struct xml_metadata xml;
	char *id;
        char *cve_id;
        struct oscap_list *products;        /* vulnerable SW list       */
	char *published;	            /* published datetime       */
	char *modified;		            /* last modified datetime   */
        struct cvss_entry *cvss;            /* [cvss] score definition  */
        char *sec_protection;
	char *cwe;		            /* cwe (0-1)                */
	struct oscap_list *summaries;	    /* summaries                */
	struct oscap_list *references;	    /* cve references           */
        struct oscap_list *configurations;  /* cve vulnerability confs  */
};
OSCAP_ACCESSOR_STRING(cve_entry, id)
OSCAP_ACCESSOR_STRING(cve_entry, published)
OSCAP_ACCESSOR_STRING(cve_entry, modified)
OSCAP_ACCESSOR_STRING(cve_entry, sec_protection)
OSCAP_ACCESSOR_STRING(cve_entry, cwe)
OSCAP_IGETTER_GEN(cve_product, cve_entry, products)
OSCAP_IGETTER_GEN(cve_reference, cve_entry, references)
OSCAP_IGETTER_GEN(cve_summary, cve_entry, summaries)
OSCAP_IGETTER_GEN(cve_configuration, cve_entry, configurations)
/* End of variable definitions
 * */
/***************************************************************************/

/***************************************************************************/
/* XML string variables definitions
 * */
#define TAG_NVD_STR BAD_CAST "nvd"
#define ATTR_XML_LANG_STR BAD_CAST "xml:lang"

/* Vulnerability entry info */
#define TAG_CVE_STR BAD_CAST "entry"
#define TAG_PUBLISHED_DATETIME_STR BAD_CAST "published-datetime"
#define TAG_LAST_MODIFIED_DATETIME_STR BAD_CAST "last-modified-datetime"
#define TAG_CWE_STR BAD_CAST "cwe"
#define TAG_SUMMARY_STR BAD_CAST "summary"
#define TAG_VULNERABLE_CONFIGURATION_STR BAD_CAST "vulnerable-configuration"
#define TAG_VULNERABLE_SOFTWARE_LIST_STR BAD_CAST "vulnerable-software-list"
#define TAG_SECURITY_PROTECTION_STR BAD_CAST "security-protection"
#define TAG_PRODUCT_STR BAD_CAST "product"
#define ATTR_CVE_ID_STR BAD_CAST "id"
#define ATTR_VULNERABLE_CONFIGURATION_ID_STR BAD_CAST "id"

#define TAG_CVE_ID_STR BAD_CAST "cve-id"
#define TAG_SOURCE_STR BAD_CAST "source"

/* CVSS */
#define TAG_CVSS_STR BAD_CAST "cvss"
 
/* CWE */
#define TAG_CWE_STR BAD_CAST "cwe"
#define ATTR_CWEID_STR BAD_CAST "id"

/* Vulnerability reference info */
#define TAG_REFERENCES_STR BAD_CAST "references"
#define TAG_REFERENCE_STR BAD_CAST "reference"
#define TAG_REFERENCE_SOURCE_STR BAD_CAST "source"
#define ATTR_REFERENCE_TYPE_STR BAD_CAST "reference_type"
#define ATTR_REFERENCE_HREF_STR BAD_CAST "href"

/* End of XML string variables definitions
 * */
/***************************************************************************/

const struct cpe_testexpr * cve_configuration_get_expr(const struct cve_configuration *item) {

    if (item == NULL)
                return NULL;

    return &(item->expr);
}

/***************************************************************************/
/* Constructors of CVE structures cve_*<structure>*_new()
 * More info in representive header file.
 * returns the type of <structure>
 */
struct cve_entry * cve_entry_new() {

        struct cve_entry *ret;

        ret = oscap_alloc(sizeof(struct cve_entry));
        if (ret == NULL)
                return NULL;

        ret->products           = oscap_list_new();
        ret->references         = oscap_list_new();
        ret->summaries          = oscap_list_new();
        ret->configurations     = oscap_list_new();
        ret->xml.lang           = NULL;
        ret->xml.namespace      = NULL;
        ret->id                 = NULL;
        ret->cve_id             = NULL;
        ret->published          = NULL;
        ret->modified           = NULL;
        ret->cvss               = NULL;
        ret->cwe                = NULL;
        ret->sec_protection     = NULL;

        return ret;
}

struct cve_configuration * cve_configuration_new() {

        struct cve_configuration *ret;

        ret = oscap_alloc(sizeof(struct cve_configuration));
        if (ret == NULL)
                return NULL;

        ret->id             = NULL;
        ret->expr           = *(cpe_testexpr_new());
        ret->xml.lang       = NULL;
        ret->xml.namespace  = NULL;

        return ret;
}

struct cwe_entry * cwe_entry_new() {

        struct cwe_entry *ret;

        ret = oscap_alloc(sizeof(struct cwe_entry));
        if (ret == NULL)
                return NULL;

        ret->xml.lang       = NULL;
        ret->xml.namespace  = NULL;
        ret->value          = NULL;

        return ret;
}

struct cve_product * cve_product_new() {

        struct cve_product *ret;

        ret = oscap_alloc(sizeof(struct cve_product));
        if (ret == NULL)
                return NULL;

        ret->xml.lang       = NULL;
        ret->xml.namespace  = NULL;
        ret->value          = NULL;

        return ret;
}

struct cve_summary * cve_summary_new() {

        struct cve_summary *ret;

        ret = oscap_alloc(sizeof(struct cve_summary));
        if (ret == NULL)
                return NULL;

        ret->xml.lang       = NULL;
        ret->xml.namespace  = NULL;
        ret->summary        = NULL;

        return ret;
}

struct cve_reference * cve_reference_new() {

        struct cve_reference *ret;
        ret = oscap_alloc(sizeof(struct cve_reference));
        if (ret == NULL)
                return NULL;

        ret->xml.lang       = NULL;
        ret->xml.namespace  = NULL;
	ret->value          = NULL;
	ret->href           = NULL;
	ret->type           = NULL;
	ret->source         = NULL;

        return ret;
}

struct cve_model * cve_model_new() {

        struct cve_model *ret;

        ret = oscap_alloc(sizeof(struct cve_model));
        if (ret == NULL)
                return NULL;

        ret->xml.lang       = NULL;
        ret->xml.namespace  = NULL;
	ret->entries        = oscap_list_new();

        return ret;
}

/* End of CVE structures' contructors
 * */
/***************************************************************************/

/***************************************************************************/
/* Declaration of static (private to this file) functions
 * These function shoud not be called from outside. For exporting these elements
 * has to call parent element's 
 */

static int xmlTextReaderNextElement(xmlTextReaderPtr reader);
static bool cve_validate_xml(const char * filename);


/* End of static declarations 
 * */
/***************************************************************************/
    
/* Function that jump to next XML starting element.
 */
static int xmlTextReaderNextElement(xmlTextReaderPtr reader) {

        int ret;
        do { 
              ret = xmlTextReaderRead(reader); 
              /* if end of file */
              if (ret == 0) break;
        } while (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);
        return ret;
}

static bool cve_validate_xml(const char * filename) {

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

/***************************************************************************/
/* Private parsing functions cve_*<structure>*_parse( xmlTextReaderPtr )
 * More info in representive header file.
 * returns the type of <structure>
 */
struct cve_model * cve_model_parse_xml(const struct oscap_import_source * source) {
        
        xmlTextReaderPtr reader;
        struct cve_model *ret = NULL;

        if (!cve_validate_xml(oscap_import_source_get_filename(source))) return NULL;

        reader = xmlReaderForFile(oscap_import_source_get_filename(source),
                                  oscap_import_source_get_encoding(source), 0);
        if (reader != NULL) {
            xmlTextReaderRead(reader);
            ret = cve_model_parse(reader);
        } else {
            fprintf(stderr, "Unable to open %s\n", oscap_import_source_get_filename(source));
        }
        xmlFreeTextReader(reader);

        return ret;
}

struct cve_model * cve_model_parse(xmlTextReaderPtr reader) {

        struct cve_model *ret = NULL;
        struct cve_entry *entry;

        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_NVD_STR) &&
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

                ret = cve_model_new();
                if (ret == NULL)
                        return NULL;

                ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                
                /* skip nodes until new element */
                xmlTextReaderNextElement(reader);

                /* CVE-specification: entry */ 
                while (xmlStrcmp (xmlTextReaderConstLocalName(reader), TAG_CVE_STR) == 0) {
                        
                        entry = cve_entry_parse(reader);
                        if (entry) oscap_list_add(ret->entries, entry);
                        xmlTextReaderNextElement(reader);
                }
        }

        return ret;
}

struct cve_entry * cve_entry_parse(xmlTextReaderPtr reader) {
        
        struct cve_entry *ret;
        struct cve_product *product;
        struct cve_reference *refer;
        struct cve_summary *summary;
        struct cve_configuration *conf;

        /* allocate platform structure here */
        ret = cve_entry_new();
	if (ret == NULL)
		return NULL;

        /* parse platform attributes here */
        ret->id = (char *) xmlTextReaderGetAttribute(reader, ATTR_CVE_ID_STR);
        ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
        ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
        if (ret->id == NULL) {
                cve_entry_free(ret);
                return NULL;    /* if there is no "id" in entry element, return NULL */
        }

        /* If <empty /> then return, because there is no child element */
        if (xmlTextReaderIsEmptyElement(reader)) return ret;

        /* skip from <entry> node to next one */
        xmlTextReaderRead(reader);

        /* while we have element that is not "entry", it is inside this element, otherwise it's ended 
         * element </entry> and we should end. If there is no one from "if" statement cases, we are parsing
         * attribute or text ,.. and we can continue to next node.
         * */
        while (xmlStrcmp (xmlTextReaderConstLocalName(reader), TAG_CVE_STR) != 0) {
                
            if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABLE_CONFIGURATION_STR) &&
                xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        /* here will come function to parse test-expr */
                        conf = cve_configuration_new();
                        conf->id = (char *) xmlTextReaderGetAttribute(reader, ATTR_CVE_ID_STR);
                        conf->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                        conf->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                        xmlTextReaderNextElement(reader);
                        conf->expr = *(cpe_testexpr_parse(reader));
                        if (conf) oscap_list_add(ret->configurations, conf);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABLE_SOFTWARE_LIST_STR) &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        /* this will be list of products */
                        xmlTextReaderRead(reader);
                        while (xmlStrcmp (xmlTextReaderConstLocalName(reader), TAG_VULNERABLE_SOFTWARE_LIST_STR) != 0) {
                                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_STR) &&
                                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    product = cve_product_new();
                                    product->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                                    product->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                                    product->value = (char *) xmlTextReaderReadString(reader);
                                    /*if (!data) { 
                                        cve_entry_free(ret);
                                        return NULL;
                                    }
                                    product->value = cpe_name_new(data);
                                    oscap_free(data);*/
                                    if (product) oscap_list_add(ret->products, product);
                                }
                                xmlTextReaderRead(reader);
                        }
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVE_ID_STR) &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->cve_id = (char *) xmlTextReaderReadString(reader);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PUBLISHED_DATETIME_STR) &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->published = (char *) xmlTextReaderReadString(reader);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_LAST_MODIFIED_DATETIME_STR) &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->modified = (char *) xmlTextReaderReadString(reader);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVSS_STR) &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->cvss = cvss_entry_parse(reader);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_SECURITY_PROTECTION_STR) &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->sec_protection = (char *) xmlTextReaderReadString(reader);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CWE_STR) &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->cwe = (char *) xmlTextReaderGetAttribute(reader, ATTR_CVE_ID_STR);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REFERENCES_STR) &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        refer = cve_reference_new();
                        refer->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                        refer->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                        refer->type  = (char *) xmlTextReaderGetAttribute(reader, ATTR_REFERENCE_TYPE_STR);
                        xmlTextReaderRead(reader);
                        while (xmlStrcmp (xmlTextReaderConstLocalName(reader), TAG_REFERENCES_STR) != 0) {

                                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_SOURCE_STR) &&
                                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                        refer->source = (char *) xmlTextReaderReadString(reader);
                                } else
                                    if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REFERENCE_STR) &&
                                        xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                        refer->href = (char *) xmlTextReaderGetAttribute(reader, ATTR_REFERENCE_HREF_STR);
                                        refer->value = (char *) xmlTextReaderReadString(reader);

                                }
                                xmlTextReaderRead(reader);
                        }
                        if (refer) oscap_list_add(ret->references, refer);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_SUMMARY_STR) &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        summary = cve_summary_new();
                        summary->summary = (char *) xmlTextReaderReadString(reader);
                        summary->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                        summary->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                        if (summary) oscap_list_add(ret->summaries, summary);
            }
            
            /* get the next node*/
            xmlTextReaderRead(reader);
        }

        return ret;
}

/***************************************************************************/
/* Private exporting functions cve_*<structure>*_export( xmlTextWriterPtr )
 * More info in representive header file.
 * returns the type of <structure>
 */
void cve_model_export_xml(struct cve_model * cve, const struct oscap_export_target * target) {

        /* TODO: ad macro to check return value from xmlTextWriter* functions */
        xmlTextWriterPtr writer;

        writer = xmlNewTextWriterFilename(oscap_export_target_get_filename(target), 0);

        /* Set properties of writer TODO: make public function to edit this ?? */
        xmlTextWriterSetIndent(writer, oscap_export_target_get_indent(target));
        xmlTextWriterSetIndentString(writer, BAD_CAST oscap_export_target_get_indent_string(target));

        xmlTextWriterStartDocument(writer, NULL, oscap_export_target_get_encoding(target), NULL);

        cve_export(cve, writer);
        xmlTextWriterEndDocument(writer);
        xmlFreeTextWriter(writer);
}

void cve_export(const struct cve_model * cve, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST NULL, TAG_NVD_STR, BAD_CAST NULL);
        // TODO: This has to be done by some algorithm that read namespaces.
        xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:vuln", BAD_CAST 
                "http://scap.nist.gov/schema/vulnerability/0.4");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:cvss", BAD_CAST 
                "http://scap.nist.gov/schema/cvss-v2/0.2");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:cpe-lang", BAD_CAST 
                "http://cpe.mitre.org/language/2.0");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns", BAD_CAST 
                "http://scap.nist.gov/schema/feed/vulnerability/2.0");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:xsi", BAD_CAST 
                "http://www.w3.org/2001/XMLSchema-instance");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "nvd_xml_version", BAD_CAST 
                "2.0");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "pub_date", BAD_CAST 
                "2009-03-23T06:05:00");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "xsi:schemaLocation", BAD_CAST 
                "http://scap.nist.gov/schema/feed/vulnerability/2.0 http://nvd.nist.gov/schema/nvd-cve-feed_2.0.xsd");
        OSCAP_FOREACH (cve_entry, e, cve_model_get_entries(cve),
		/* dump its contents to XML tree */
                cve_entry_export( e, writer );
	)
        xmlTextWriterEndElement(writer);
}

void cve_reference_export(const struct cve_reference * refer, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST refer->xml.namespace, TAG_REFERENCES_STR, BAD_CAST NULL);

        if ((refer->xml.lang) != NULL)
                xmlTextWriterWriteAttribute(writer, ATTR_XML_LANG_STR, BAD_CAST refer->xml.lang);
        if ((refer->type) != NULL)
                xmlTextWriterWriteAttribute(writer, ATTR_REFERENCE_TYPE_STR, BAD_CAST refer->type);

        if ((refer->source) != NULL) {
                xmlTextWriterStartElementNS(writer, BAD_CAST refer->xml.namespace, TAG_SOURCE_STR, BAD_CAST NULL);
                xmlTextWriterWriteString(writer, BAD_CAST refer->source);
                /*</source>*/
                xmlTextWriterEndElement(writer);
        }

        xmlTextWriterStartElementNS(writer, BAD_CAST refer->xml.namespace, TAG_REFERENCE_STR, BAD_CAST NULL);

        if ((refer->xml.lang) != NULL)
                xmlTextWriterWriteAttribute(writer, ATTR_XML_LANG_STR, BAD_CAST refer->xml.lang);
        if ((refer->href) != NULL)
                xmlTextWriterWriteAttribute(writer, ATTR_REFERENCE_HREF_STR, BAD_CAST refer->href);

        xmlTextWriterWriteString(writer, BAD_CAST refer->value);

        /*</reference>*/
        xmlTextWriterEndElement(writer);

        /*</references>*/
        xmlTextWriterEndElement(writer);
}

void cve_summary_export(const struct cve_summary * sum, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST sum->xml.namespace, TAG_SUMMARY_STR, BAD_CAST NULL);
        if ((sum->xml.lang) != NULL)
                xmlTextWriterWriteAttribute(writer, ATTR_XML_LANG_STR, BAD_CAST sum->xml.lang);
        xmlTextWriterWriteString(writer, BAD_CAST sum->summary);
        /*</summary>*/
        xmlTextWriterEndElement(writer);
}

void cve_entry_export(const struct cve_entry * entry, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST entry->xml.namespace, TAG_CVE_STR, BAD_CAST NULL);
        if ((entry->id) != NULL) 
                xmlTextWriterWriteAttribute(writer, ATTR_CVE_ID_STR, BAD_CAST entry->id);
 
        OSCAP_FOREACH (cve_configuration, conf, cve_entry_get_configurations(entry),
		/* dump its contents to XML tree */
                xmlTextWriterStartElementNS(writer, BAD_CAST conf->xml.namespace, TAG_VULNERABLE_CONFIGURATION_STR, BAD_CAST NULL);
                xmlTextWriterWriteAttribute(writer, ATTR_VULNERABLE_CONFIGURATION_ID_STR, BAD_CAST conf->id);
                cpe_testexpr_export(conf->expr, writer);
                xmlTextWriterEndElement(writer);
        )

        if (oscap_list_get_itemcount(entry->products) != 0) {
            /* TODO: make vulnerable-software-list element with namespace */
            xmlTextWriterStartElementNS(writer, BAD_CAST "vuln", TAG_VULNERABLE_SOFTWARE_LIST_STR, BAD_CAST NULL);
            OSCAP_FOREACH (cve_product, product, cve_entry_get_products(entry),
		    /* dump its contents to XML tree */
                    xmlTextWriterStartElementNS(writer, BAD_CAST product->xml.namespace, TAG_PRODUCT_STR, BAD_CAST NULL);
                    xmlTextWriterWriteString(writer, BAD_CAST product->value);
                    xmlTextWriterEndElement(writer);
	    )
            /*</vulnerable-software-list>*/
            xmlTextWriterEndElement(writer);
        }
        if ((entry->cve_id) != NULL)
                xmlTextWriterWriteElementNS(writer, BAD_CAST "vuln", TAG_CVE_ID_STR, NULL, BAD_CAST entry->cve_id);

        if ((entry->published) != NULL)
                xmlTextWriterWriteElementNS(writer, BAD_CAST "vuln", TAG_PUBLISHED_DATETIME_STR, NULL, BAD_CAST entry->published);

        if ((entry->modified) != NULL)
                xmlTextWriterWriteElementNS(writer, BAD_CAST "vuln", TAG_LAST_MODIFIED_DATETIME_STR, NULL, BAD_CAST entry->modified);

        if ((entry->cvss) != NULL) {
                cvss_entry_export(entry->cvss, writer);
        }
        if ((entry->sec_protection) != NULL)
                xmlTextWriterWriteElementNS(writer, BAD_CAST "vuln", TAG_SECURITY_PROTECTION_STR, NULL, BAD_CAST entry->sec_protection);

        if ((entry->cwe) != NULL) {
                xmlTextWriterStartElementNS(writer, BAD_CAST "vuln", TAG_CWE_STR, BAD_CAST NULL);
                xmlTextWriterWriteAttribute(writer, ATTR_CWEID_STR, BAD_CAST entry->cwe);
                xmlTextWriterEndElement(writer);
        }

        if ((entry->references) != NULL) {
            /* TODO: make references element with namespace */
            OSCAP_FOREACH (cve_reference, refer, cve_entry_get_references(entry),
		    /* dump its contents to XML tree */
                    cve_reference_export(refer, writer);
	    )
        }
        if ((entry->references) != NULL) {
            /* TODO: make references element with namespace */
            OSCAP_FOREACH (cve_summary, sum, cve_entry_get_summaries(entry),
		    /* dump its contents to XML tree */
                    cve_summary_export(sum, writer);
            )
        }

        /* </entry> */
        xmlTextWriterEndElement(writer);
}

/***************************************************************************/
/* Free functions - all will free their subtree so use carefuly !
 */
void cve_summary_free(struct cve_summary * summary) {

        if (summary == NULL) return;

        xmlFree(summary->summary);
        xml_metadata_free(summary->xml);
        oscap_free(summary);
}

void cve_model_free(struct cve_model * cve_model) {

	if (cve_model == NULL) return;

	oscap_list_free(cve_model->entries, (oscap_destruct_func)cve_entry_free);
        xml_metadata_free(cve_model->xml);
	oscap_free(cve_model);
}

void cve_configuration_free(struct cve_configuration * conf)  {
	
        if (conf == NULL) return;

        xmlFree(conf->id);
        cpe_testexpr_free(&conf->expr);
        xml_metadata_free(conf->xml);
        oscap_free(conf);
}

void cve_product_free(struct cve_product * product) {

        if (product == NULL) return;

        xmlFree(product->value);
        xml_metadata_free(product->xml);
        oscap_free(product);
}

void cve_reference_free(struct cve_reference * ref) {

        if (ref == NULL) return;

        xmlFree(ref->value);
        xmlFree(ref->href);
        xmlFree(ref->type);
        xmlFree(ref->source);
        xml_metadata_free(ref->xml);
        oscap_free(ref);
}

void cwe_entry_free(struct cwe_entry * entry) {

        if (entry == NULL) return;

        xmlFree(entry->value);
        xml_metadata_free(entry->xml);
        oscap_free(entry);
}

void cve_entry_free(struct cve_entry * entry) {

	if (entry == NULL) return;

        if (entry->id != NULL)
	    xmlFree(entry->id);
	xmlFree(entry->published);
	xmlFree(entry->modified);
        xmlFree(entry->sec_protection);
	xmlFree(entry->cwe);
        cvss_entry_free(entry->cvss);
        oscap_list_free(entry->products, (oscap_destruct_func)cve_product_free);
        oscap_list_free(entry->references, (oscap_destruct_func)cve_reference_free);
        oscap_list_free(entry->summaries, (oscap_destruct_func)cve_summary_free);
        oscap_list_free(entry->configurations, (oscap_destruct_func)cve_configuration_free);
        xml_metadata_free(entry->xml);
	oscap_free(entry);
}

/* End of free functions
 * */
/***************************************************************************/
