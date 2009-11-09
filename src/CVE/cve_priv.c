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
#include "../CPE/cpelang_priv.h"

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
	struct oscap_list* entries; // 1-n
	struct oscap_htable* entry;
};
OSCAP_IGETTER_GEN(cve_entry, cve_model, entries)
OSCAP_HGETTER(struct cve_entry*, cve_model, entry)

struct cve_reference {

        struct xml_metadata xml;
	char *value;		// summary
	char *href;		// href
	char *type;		// reference type
	char *source;		// source
};
OSCAP_ACCESSOR_STRING(cve_reference, value)
OSCAP_ACCESSOR_STRING(cve_reference, href)
OSCAP_ACCESSOR_STRING(cve_reference, type)
OSCAP_ACCESSOR_STRING(cve_reference, source)

struct cvss_entry {

        char *score;
        char *AV;
        char *AC;
        char *authentication;
        // impacts
        char *imp_confidentiality;
        char *imp_integrity;
        char *imp_availability;
        char *source;
        char *generated;
};
OSCAP_ACCESSOR_STRING(cvss_entry, score)
OSCAP_ACCESSOR_STRING(cvss_entry, AV)
OSCAP_ACCESSOR_STRING(cvss_entry, AC)
OSCAP_ACCESSOR_STRING(cvss_entry, authentication)
OSCAP_ACCESSOR_STRING(cvss_entry, imp_confidentiality)
OSCAP_ACCESSOR_STRING(cvss_entry, imp_integrity)
OSCAP_ACCESSOR_STRING(cvss_entry, imp_availability)
OSCAP_ACCESSOR_STRING(cvss_entry, source)
OSCAP_ACCESSOR_STRING(cvss_entry, generated)

struct cve_summary {

        struct xml_metadata xml;
	char *summary;		// summary
};
OSCAP_ACCESSOR_STRING(cve_summary, summary)

struct cve_product {

        struct xml_metadata xml;
        char *value;
};
OSCAP_ACCESSOR_STRING(cve_product, value)

struct cwe_entry {

        struct xml_metadata xml;
        char *value;
};
OSCAP_ACCESSOR_STRING(cwe_entry, value)

struct cve_configuration {

        struct xml_metadata xml;
        char *id;
	struct cpe_testexpr expr;       // [cpe:lang] expression for match evaluation (0-1)
};
OSCAP_ACCESSOR_STRING(cve_configuration, id)


struct cve_entry {

        struct xml_metadata xml;
	char *id;		        // id 
        char *cve_id;
        struct oscap_list *products;
	char *published;	        // published datetime
	char *modified;		        // last modified datetime
        struct cvss_entry *cvss;
        char *sec_protection;
	char *cwe;		        // cwe (0-1)
	struct oscap_list *summaries;	// summary
	struct oscap_list *references;	// cve references
        struct oscap_list *configurations;
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

struct cvss_entry * cvss_entry_new() {

        struct cvss_entry *ret;

        ret = oscap_alloc(sizeof(struct cvss_entry));
        if (ret == NULL)
                return NULL;

        ret->score                  = NULL;
        ret->AV                     = NULL;
        ret->AC                     = NULL;
        ret->authentication         = NULL;
        ret->imp_confidentiality    = NULL;
        ret->imp_integrity          = NULL;
        ret->imp_availability       = NULL;
        ret->source                 = NULL;
        ret->generated              = NULL;

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
	ret->entry          = oscap_htable_new();

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
              // if end of file
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

        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "nvd") &&
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

                ret = cve_model_new();
                if (ret == NULL)
                        return NULL;

                ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                
                // skip nodes until new element
                xmlTextReaderNextElement(reader);

                // CVE-specification: entry / 
                while (xmlStrcmp (xmlTextReaderConstLocalName(reader), (const xmlChar *)"entry") == 0) {
                        
                        entry = cve_entry_parse(reader);
                        if (entry) oscap_list_add(ret->entries, entry);
                        xmlTextReaderNextElement(reader);
                }
        }

        return ret;
}

struct cvss_entry * cvss_entry_parse(xmlTextReaderPtr reader) {

        struct cvss_entry *ret = NULL;

        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "cvss") &&
            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

                xmlTextReaderNextElement(reader); /* We are on base_metrics now */

                char *ss;
                ss = (char *) xmlTextReaderConstLocalName(reader);

                if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "base_metrics"))
                    // we want to end if there are no bas-metrics ! What else could be here ?
                    //
                    return NULL;
                
                // skip nodes until new element
                xmlTextReaderNextElement(reader); /* We are on score now */

                ret = cvss_entry_new();
                if (ret == NULL)
                        return NULL;

                //ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                //ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);

                // CVSS-specification: score / 
                while (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST "cvss") != 0) {
                        
                    if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "score") &&
                        xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                ret->score = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "access-vector") &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->AV = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "access-complexity") &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->AC = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "authentication") &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->authentication = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "confidentiality-impact") &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->imp_confidentiality = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "integrity-impact") &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->imp_integrity = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "availability-impact") &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->imp_availability = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "source") &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->source = (char *) xmlTextReaderReadString(reader);
                    } else
                        if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "generated-on-datetime") &&
                            xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                    ret->generated = (char *) xmlTextReaderReadString(reader);
                    }

                    xmlTextReaderRead(reader);
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

        // allocate platform structure here
        ret = cve_entry_new();
	if (ret == NULL)
		return NULL;

        // parse platform attributes here
        ret->id = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "id");
        ret->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
        ret->xml.namespace = (char *) xmlTextReaderPrefix(reader);
        if (ret->id == NULL) {
                cve_entry_free(ret);
                return NULL;    // if there is no "id" in entry element, return NULL
        }

        // skip from <entry> node to next one
        xmlTextReaderRead(reader);

        // while we have element that is not "entry", it is inside this element, otherwise it's ended 
        // element </entry> and we should end. If there is no one from "if" statement cases, we are parsing
        // attribute or text ,.. and we can continue to next node.
        while (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST "entry") != 0) {
                
            if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "vulnerable-configuration") &&
                xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        // here will come function to parse test-expr
                        conf = cve_configuration_new();
                        conf->id = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "id");
                        conf->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                        conf->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                        xmlTextReaderNextElement(reader);
                        conf->expr = *(cpe_testexpr_parse(reader));
                        if (conf) oscap_list_add(ret->configurations, conf);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "vulnerable-software-list") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        // this will be list of products
                        xmlTextReaderRead(reader);
                        while (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST "vulnerable-software-list") != 0) {
                                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "product") &&
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
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "cve-id") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->cve_id = (char *) xmlTextReaderReadString(reader);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "published-datetime") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->published = (char *) xmlTextReaderReadString(reader);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "last-modified-datetime") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->modified = (char *) xmlTextReaderReadString(reader);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "cvss") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        // here will come function to parse cvss
                        ret->cvss = cvss_entry_parse(reader);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "security-protection") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->sec_protection = (char *) xmlTextReaderReadString(reader);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "cwe") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        ret->cwe = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "id");
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "references") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        refer = cve_reference_new();
                        refer->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                        refer->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                        refer->type  = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "reference_type");
                        xmlTextReaderRead(reader);
                        while (xmlStrcmp (xmlTextReaderConstLocalName(reader), BAD_CAST "references") != 0) {

                                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "source") &&
                                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                        refer->source = (char *) xmlTextReaderReadString(reader);
                                } else
                                    if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "reference") &&
                                        xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                                        refer->href = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "href");
                                        refer->value = (char *) xmlTextReaderReadString(reader);

                                }
                                xmlTextReaderRead(reader);
                        }
                        if (refer) oscap_list_add(ret->references, refer);
            } else
                if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "summary") &&
                    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
                        summary = cve_summary_new();
                        summary->summary = (char *) xmlTextReaderReadString(reader);
                        summary->xml.lang = oscap_strdup((char *) xmlTextReaderConstXmlLang(reader));
                        summary->xml.namespace = (char *) xmlTextReaderPrefix(reader);
                        if (summary) oscap_list_add(ret->summaries, summary);
            }
            
        
            // get the next node
            xmlTextReaderRead(reader);
        }

        return ret;
}


void print_entries(const struct cve_model * cve) {

	/*OSCAP_FOREACH (cve_entry, entry, cve_model_get_entries(cve),
		fprintf(stdout, "[cveInfo: %s (%s)]\n\t[CWE: %s]\n\t[ID: %s %d]\n", cve_entry_get_id(entry), 
                    cve_entry_get_published(entry), cve_entry_get_cwe(entry), 
                    cve_entry_get_configuration_id(entry), entry->expr.oper);
	        OSCAP_FOREACH (cve_product, product, cve_entry_get_products(entry),
		        fprintf(stdout, "\t[cveProduct: %s]\n", cve_product_get_value(product));
                    )
	        OSCAP_FOREACH (cve_reference, ref, cve_entry_get_references(entry),
		        fprintf(stdout, "\t[cveRef: %s %s]\n", cve_reference_get_href(ref),
                            cve_reference_get_value(ref));
                    )
	        OSCAP_FOREACH (cve_summary, sum, cve_entry_get_summaries(entry),
                        fprintf(stdout, "\t[cveSummary: %s]\n", cve_summary_get_summary(sum));
                    )
                )*/

}

/***************************************************************************/
/* Private exporting functions cpe_*<structure>*_export( xmlTextWriterPtr )
 * More info in representive header file.
 * returns the type of <structure>
 */
void cve_model_export_xml(struct cve_model * cve, struct oscap_export_target * target) {

        // TODO: ad macro to check return value from xmlTextWriter* functions
        xmlTextWriterPtr writer;

        writer = xmlNewTextWriterFilename(oscap_export_target_get_filename(target), 0);

        // Set properties of writer TODO: make public function to edit this ??
        xmlTextWriterSetIndent(writer, oscap_export_target_get_indent(target));
        xmlTextWriterSetIndentString(writer, BAD_CAST oscap_export_target_get_indent_string(target));

        xmlTextWriterStartDocument(writer, NULL, oscap_export_target_get_encoding(target), NULL);

        cve_export(cve, writer);
        xmlTextWriterEndDocument(writer);
        xmlFreeTextWriter(writer);
        cve_model_free(cve);
}

void cve_export(const struct cve_model * cve, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST NULL, BAD_CAST "nvd", BAD_CAST NULL);
        OSCAP_FOREACH (cve_entry, e, cve_model_get_entries(cve),
		// dump its contents to XML tree
                cve_entry_export( e, writer );
	)
        xmlTextWriterEndElement(writer);
}

void cve_entry_export(const struct cve_entry * entry, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST entry->xml.namespace, BAD_CAST "entry", BAD_CAST NULL);
        if ((entry->id) != NULL) 
                xmlTextWriterWriteAttribute(writer, BAD_CAST "id", BAD_CAST entry->id);
 
        OSCAP_FOREACH (cve_configuration, conf, cve_entry_get_configurations(entry),
		/* dump its contents to XML tree */
                xmlTextWriterStartElementNS(writer, BAD_CAST conf->xml.namespace, BAD_CAST "vulnerable-configuration", BAD_CAST NULL);
                xmlTextWriterWriteAttribute(writer, BAD_CAST "id", BAD_CAST conf->id);
                cpe_testexpr_export(conf->expr, writer);
                xmlTextWriterEndElement(writer);
        )

        if (oscap_list_get_itemcount(entry->products) != 0) {
            /* TODO: make vulnerable-software-list element with namespace */
            xmlTextWriterStartElementNS(writer, BAD_CAST "vuln", BAD_CAST "vulnerable-software-list", BAD_CAST NULL);
            OSCAP_FOREACH (cve_product, product, cve_entry_get_products(entry),
		    /* dump its contents to XML tree */
                    xmlTextWriterStartElementNS(writer, BAD_CAST product->xml.namespace, BAD_CAST "product", BAD_CAST NULL);
                    xmlTextWriterWriteString(writer, BAD_CAST product->value);
                    xmlTextWriterEndElement(writer);
	    )
            /*</vulnerable-software-list>*/
            xmlTextWriterEndElement(writer);
        }
        if ((entry->cve_id) != NULL)
                xmlTextWriterWriteElementNS(writer, BAD_CAST "vuln", BAD_CAST "cve-id", NULL, BAD_CAST entry->cve_id);

        if ((entry->published) != NULL)
                xmlTextWriterWriteElementNS(writer, BAD_CAST "vuln", BAD_CAST "published-datetime", NULL, BAD_CAST entry->published);

        if ((entry->modified) != NULL)
                xmlTextWriterWriteElementNS(writer, BAD_CAST "vuln", BAD_CAST "last-modified-datetime", NULL, BAD_CAST entry->modified);

        if ((entry->cvss) != NULL) {
                xmlTextWriterStartElementNS(writer, BAD_CAST "vuln", BAD_CAST "cvss", BAD_CAST NULL);
                xmlTextWriterStartElementNS(writer, BAD_CAST "cvss", BAD_CAST "base_metrics", BAD_CAST NULL);

                if ((entry->cvss->score) != NULL)
                    xmlTextWriterWriteElementNS(writer, BAD_CAST "cvss", BAD_CAST "score", NULL, 
                                                                         BAD_CAST entry->cvss->score);
                if ((entry->cvss->AV) != NULL)
                    xmlTextWriterWriteElementNS(writer, BAD_CAST "cvss", BAD_CAST "access-vector", NULL, 
                                                                         BAD_CAST entry->cvss->AV);
                if ((entry->cvss->AC) != NULL)
                    xmlTextWriterWriteElementNS(writer, BAD_CAST "cvss", BAD_CAST "access-complexity", NULL, 
                                                                         BAD_CAST entry->cvss->AC);
                if ((entry->cvss->authentication) != NULL)
                    xmlTextWriterWriteElementNS(writer, BAD_CAST "cvss", BAD_CAST "authentication", NULL, 
                                                                         BAD_CAST entry->cvss->authentication);
                if ((entry->cvss->imp_confidentiality) != NULL)
                    xmlTextWriterWriteElementNS(writer, BAD_CAST "cvss", BAD_CAST "confidentiality-impact", NULL, 
                                                                         BAD_CAST entry->cvss->imp_confidentiality);
                if ((entry->cvss->imp_integrity) != NULL)
                    xmlTextWriterWriteElementNS(writer, BAD_CAST "cvss", BAD_CAST "integrity-impact", NULL, 
                                                                         BAD_CAST entry->cvss->imp_integrity);
                if ((entry->cvss->imp_availability) != NULL)
                    xmlTextWriterWriteElementNS(writer, BAD_CAST "cvss", BAD_CAST "availability-impact", NULL, 
                                                                         BAD_CAST entry->cvss->imp_availability);
                if ((entry->cvss->source) != NULL)
                    xmlTextWriterWriteElementNS(writer, BAD_CAST "cvss", BAD_CAST "source", NULL, 
                                                                         BAD_CAST entry->cvss->source);
                if ((entry->cvss->generated) != NULL)
                    xmlTextWriterWriteElementNS(writer, BAD_CAST "cvss", BAD_CAST "generated-on-datetime", NULL, 
                                                                         BAD_CAST entry->cvss->generated);

                /*</base-metrics>*/
                xmlTextWriterEndElement(writer);
                /*</cvss>*/
                xmlTextWriterEndElement(writer);
        }
        if ((entry->sec_protection) != NULL)
                xmlTextWriterWriteElementNS(writer, BAD_CAST "vuln", BAD_CAST "security-protection", NULL, BAD_CAST entry->sec_protection);

        if ((entry->cwe) != NULL) {
                xmlTextWriterStartElementNS(writer, BAD_CAST "vuln", BAD_CAST "cwe", BAD_CAST NULL);
                xmlTextWriterWriteAttribute(writer, BAD_CAST "id", BAD_CAST entry->cwe);
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

void cve_reference_export(const struct cve_reference * refer, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST refer->xml.namespace, BAD_CAST "references", BAD_CAST NULL);

        if ((refer->xml.lang) != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang", BAD_CAST refer->xml.lang);
        if ((refer->type) != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "reference_type", BAD_CAST refer->type);

        if ((refer->source) != NULL) {
                xmlTextWriterStartElementNS(writer, BAD_CAST refer->xml.namespace, BAD_CAST "source", BAD_CAST NULL);
                xmlTextWriterWriteString(writer, BAD_CAST refer->source);
                /*</source>*/
                xmlTextWriterEndElement(writer);
        }

        xmlTextWriterStartElementNS(writer, BAD_CAST refer->xml.namespace, BAD_CAST "reference", BAD_CAST NULL);

        if ((refer->xml.lang) != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang", BAD_CAST refer->xml.lang);
        if ((refer->href) != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "href", BAD_CAST refer->href);

        xmlTextWriterWriteString(writer, BAD_CAST refer->value);

        /*</reference>*/
        xmlTextWriterEndElement(writer);

        /*</references>*/
        xmlTextWriterEndElement(writer);
}

void cve_summary_export(const struct cve_summary * sum, xmlTextWriterPtr writer) {

        xmlTextWriterStartElementNS(writer, BAD_CAST sum->xml.namespace, BAD_CAST "summary", BAD_CAST NULL);
        if ((sum->xml.lang) != NULL)
                xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang", BAD_CAST sum->xml.lang);
        xmlTextWriterWriteString(writer, BAD_CAST sum->summary);
        /*</summary>*/
        xmlTextWriterEndElement(writer);
}

/***************************************************************************/
/* Free functions - all will free their subtree so use carefuly !
 */

void cve_model_free(struct cve_model * cve_model) {

	if (cve_model == NULL) return;

	//oscap_htable_free(cve_model->entry, NULL);
	oscap_list_free(cve_model->entries, (oscap_destruct_func)cve_entry_free);
        xml_metadata_free(cve_model->xml);
	oscap_free(cve_model);
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
	//cpe_testexpr_free(&entry->expr);
        xml_metadata_free(entry->xml);
	oscap_free(entry);
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

void cvss_entry_free(struct cvss_entry * entry) {

        if (entry == NULL) return;

        xmlFree(entry->score);
        xmlFree(entry->AV);
        xmlFree(entry->AC);
        xmlFree(entry->authentication);
        xmlFree(entry->imp_confidentiality);
        xmlFree(entry->imp_integrity);
        xmlFree(entry->imp_availability);
        xmlFree(entry->source);
        xmlFree(entry->generated);
        oscap_free(entry);
}

void cve_summary_free(struct cve_summary * summary) {

        if (summary == NULL) return;

        xmlFree(summary->summary);
        xml_metadata_free(summary->xml);
        oscap_free(summary);
}

void cwe_entry_free(struct cwe_entry * entry) {

        if (entry == NULL) return;

        xmlFree(entry->value);
        xml_metadata_free(entry->xml);
        oscap_free(entry);
}

/* End of free functions
 * */
/***************************************************************************/
