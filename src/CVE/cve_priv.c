/*! \file cve_priv.c
 *  \brief Common Vulnerability and Exposure dictionary
 * 
 *   See details at:
 *     http://cve.mitre.org/
 *     http://nvd.nist.gov/download.cfm
 *  
 */

/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "public/cve_nvd.h"
#include "cve_priv.h"

#include "common/list.h"
#include "common/_error.h"
#include "common/xmltext_priv.h"
#include "common/elements.h"

#include "CPE/cpelang_priv.h"
#include "CVSS/cvss_priv.h"
#include "CVSS/public/cvss_score.h"

#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"

/***************************************************************************/
/* Variable definitions
 * */

/**
 * @struct cve_list
 * cve_list is the top level element of the CVE List provided by MITRE. 
 * It represents holds all CVE Items.
 */
struct cve_model {
	char  *pub_date;
	char  *nvd_xml_version;
	struct oscap_list *entries;	/* 1-n */
};
    OSCAP_IGETINS_GEN(cve_entry, cve_model, entries, entry)
    OSCAP_ITERATOR_REMOVE_F(cve_entry)
	OSCAP_ACCESSOR_STRING(cve_model, nvd_xml_version)
	OSCAP_ACCESSOR_STRING(cve_model, pub_date)

/*
 */
struct cve_reference {
	char *value;		/* summary          */
	char *href;		/* href             */
	char *type;		/* reference type   */
	char *source;		/* source           */
	char *lang;
};
OSCAP_ACCESSOR_STRING(cve_reference, value)
    OSCAP_ACCESSOR_STRING(cve_reference, href)
    OSCAP_ACCESSOR_STRING(cve_reference, type)
    OSCAP_ACCESSOR_STRING(cve_reference, source)
    OSCAP_ACCESSOR_STRING(cve_reference, lang)

/*
 */
struct cve_summary {
	char *summary;
};
OSCAP_ACCESSOR_STRING(cve_summary, summary)

/*
 */
struct cve_product {
	char *value;
};
OSCAP_ACCESSOR_STRING(cve_product, value)

/*
 */
struct cwe_entry {
	char *value;
};
OSCAP_ACCESSOR_STRING(cwe_entry, value)

/*
 */
struct cve_configuration {
	char *id;
	struct cpe_testexpr *expr;	/* [cpe:lang] expression (0-1) */
};
OSCAP_ACCESSOR_STRING(cve_configuration, id)

/*
 */
struct cve_entry {
	char *id;
	char *cve_id;
	struct oscap_list *products;	/* vulnerable SW list       */
	char *published;	/* published datetime       */
	char *modified;		/* last modified datetime   */
	struct cvss_impact *cvss;	/* [cvss] score definition  */
	char *sec_protection;
	char *cwe;		/* cwe (0-1)                */
	struct oscap_list *summaries;	/* summaries                */
	struct oscap_list *references;	/* cve references           */
	struct oscap_list *configurations;	/* cve vulnerability confs  */
};
OSCAP_ACCESSOR_STRING(cve_entry, id)
    OSCAP_ACCESSOR_STRING(cve_entry, published)
    OSCAP_ACCESSOR_STRING(cve_entry, modified)
    OSCAP_ACCESSOR_STRING(cve_entry, sec_protection)
    OSCAP_ACCESSOR_STRING(cve_entry, cwe)
    OSCAP_IGETINS_GEN(cve_product, cve_entry, products, product)
    OSCAP_IGETINS_GEN(cve_reference, cve_entry, references, reference)
    OSCAP_IGETINS_GEN(cve_summary, cve_entry, summaries, summary)
    OSCAP_IGETINS_GEN(cve_configuration, cve_entry, configurations, configuration)
    OSCAP_ITERATOR_REMOVE_F(cve_product)
    OSCAP_ITERATOR_REMOVE_F(cve_reference)
    OSCAP_ITERATOR_REMOVE_F(cve_summary)
    OSCAP_ITERATOR_REMOVE_F(cve_configuration)
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
#define NS_VULN_STR BAD_CAST "vuln"
/* namespaces */
#define CVE_NS  BAD_CAST "http://scap.nist.gov/schema/vulnerability/0.4"
#define FEED_NS BAD_CAST "http://scap.nist.gov/schema/feed/vulnerability/2.0"
#define FEED_NS_LOCATION BAD_CAST "http://scap.nist.gov/schema/feed/vulnerability/2.0 http://nvd.nist.gov/schema/nvd-cve-feed_2.0.xsd"
/* End of XML string variables definitions
 * */
/***************************************************************************/
const struct cvss_impact *cve_entry_get_cvss(const struct cve_entry *item)
{

	if (item == NULL)
		return NULL;

	return item->cvss;
}

const struct cpe_testexpr *cve_configuration_get_expr(const struct cve_configuration *item)
{

	if (item == NULL)
		return NULL;

	return item->expr;
}

/***************************************************************************/
/* Constructors of CVE structures cve_*<structure>*_new()
 * More info in representive header file.
 * returns the type of <structure>
 */
struct cve_entry *cve_entry_new()
{

	struct cve_entry *ret;

	ret = oscap_alloc(sizeof(struct cve_entry));
	if (ret == NULL)
		return NULL;

	ret->products = oscap_list_new();
	ret->references = oscap_list_new();
	ret->summaries = oscap_list_new();
	ret->configurations = oscap_list_new();
	ret->id = NULL;
	ret->cve_id = NULL;
	ret->published = NULL;
	ret->modified = NULL;
	ret->cvss = NULL;
	ret->cwe = NULL;
	ret->sec_protection = NULL;

	return ret;
}

struct cve_entry * cve_entry_clone(struct cve_entry * old_entry)
{
    struct cve_entry * new_entry = cve_entry_new();
    new_entry->id = oscap_strdup(old_entry->id);
    new_entry->cve_id = oscap_strdup(old_entry->cve_id);
    new_entry->published = oscap_strdup(old_entry->published);
    new_entry->modified = oscap_strdup(old_entry->modified);
    new_entry->cwe = oscap_strdup(old_entry->cwe);
    new_entry->sec_protection = oscap_strdup(old_entry->sec_protection);

    new_entry->cvss = cvss_impact_clone(old_entry->cvss);

    new_entry->products = oscap_list_clone(old_entry->products, (oscap_clone_func) cve_product_clone);
    new_entry->references = oscap_list_clone(old_entry->references, (oscap_clone_func) cve_reference_clone);
    new_entry->summaries = oscap_list_clone(old_entry->summaries, (oscap_clone_func) cve_summary_clone);
    new_entry->configurations = oscap_list_clone(old_entry->configurations, (oscap_clone_func) cve_configuration_clone);

    return new_entry;
}

struct cve_configuration *cve_configuration_new()
{

	struct cve_configuration *ret;

	ret = oscap_alloc(sizeof(struct cve_configuration));
	if (ret == NULL)
		return NULL;

	ret->id = NULL;
	ret->expr = cpe_testexpr_new();

	return ret;
}

struct cve_configuration * cve_configuration_clone(struct cve_configuration * old_conf)
{
    struct cve_configuration * new_conf = cve_configuration_new();
    new_conf->id = oscap_strdup(old_conf->id);
    new_conf->expr = cpe_testexpr_clone(old_conf->expr);
    return new_conf;
}

struct cwe_entry *cwe_entry_new()
{

	struct cwe_entry *ret;

	ret = oscap_alloc(sizeof(struct cwe_entry));
	if (ret == NULL)
		return NULL;

	ret->value = NULL;

	return ret;
}

struct cwe_entry * cwe_entry_clone(struct cwe_entry * old_entry)
{
    struct cwe_entry * new_entry = cwe_entry_new();
    new_entry->value = oscap_strdup(old_entry->value);
    return new_entry;
}

struct cve_product *cve_product_new()
{

	struct cve_product *ret;

	ret = oscap_alloc(sizeof(struct cve_product));
	if (ret == NULL)
		return NULL;

	ret->value = NULL;

	return ret;
}

struct cve_product * cve_product_clone(struct cve_product * old_product)
{
    struct cve_product * product = cve_product_new();
    product->value = oscap_strdup(old_product->value);
    return product;
}

struct cve_summary *cve_summary_new()
{

	struct cve_summary *ret;

	ret = oscap_alloc(sizeof(struct cve_summary));
	if (ret == NULL)
		return NULL;

	ret->summary = NULL;

	return ret;
}

struct cve_summary * cve_summary_clone(struct cve_summary * old_sum)
{
    struct cve_summary * sum = cve_summary_new();
    sum->summary = oscap_strdup(old_sum->summary);
    return sum;
}

struct cve_reference *cve_reference_new()
{

	struct cve_reference *ret;
	ret = oscap_calloc(1, sizeof(struct cve_reference));
	if (ret == NULL)
		return NULL;

	return ret;
}

struct cve_reference * cve_reference_clone(struct cve_reference * old_ref)
{
    struct cve_reference * ref = cve_reference_new();
    ref->value = oscap_strdup(old_ref->value);
    ref->href = oscap_strdup(old_ref->href);
    ref->type = oscap_strdup(old_ref->type);
    ref->source = oscap_strdup(old_ref->source);
    ref->lang = oscap_strdup(old_ref->lang);
    return ref;
}

struct cve_model *cve_model_new()
{

	struct cve_model *ret;

	ret = oscap_alloc(sizeof(struct cve_model));
	if (ret == NULL)
		return NULL;
	memset(ret, 0, sizeof(struct cve_model));

	ret->nvd_xml_version = NULL;
	ret->entries = oscap_list_new();

	return ret;
}

struct cve_model * cve_model_clone(struct cve_model * old_model)
{
    struct cve_model * new_model = cve_model_new();
    new_model->entries = oscap_list_clone(old_model->entries, (oscap_clone_func) cve_entry_clone);
    return new_model;
}

/* End of CVE structures' contructors
 * */
/***************************************************************************/

/***************************************************************************/
/* Private parsing functions cve_*<structure>*_parse( xmlTextReaderPtr )
 * More info in representive header file.
 * returns the type of <structure>
 */
struct cve_model *cve_model_parse_xml(const char *file)
{

	__attribute__nonnull__(file);

	struct cve_model *ret = NULL;
	int rc;

	struct oscap_source *source = oscap_source_new_from_file(file);
	xmlTextReader *reader = oscap_source_get_xmlTextReader(source);
	if (!reader) {
		oscap_source_free(source);
		return NULL;
	}

	rc = xmlTextReaderNextNode(reader);
	if (rc == -1) {
		xmlFreeTextReader(reader);
		return NULL;
	}

	ret = cve_model_parse(reader);

	xmlFreeTextReader(reader);
	oscap_source_free(source);
	return ret;
}

struct cve_model *cve_model_parse(xmlTextReaderPtr reader)
{

	__attribute__nonnull__(reader);

	struct cve_model *ret = NULL;
	struct cve_entry *entry = NULL;

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_NVD_STR) &&
	    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

		ret = cve_model_new();
		if (ret == NULL)
			return NULL;

		ret->nvd_xml_version = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "nvd_xml_version");
		ret->pub_date = (char*) xmlTextReaderGetAttribute(reader, BAD_CAST "pub_date");

		/* skip nodes until new element */
		xmlTextReaderNextElement(reader);

		/* CVE-specification: entry */
		while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVE_STR) == 0) {

			entry = cve_entry_parse(reader);
			if (entry)
				oscap_list_add(ret->entries, entry);
			xmlTextReaderNextElement(reader);
		}
	}

	return ret;
}

struct cve_entry *cve_entry_parse(xmlTextReaderPtr reader)
{

	__attribute__nonnull__(reader);

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
	ret->id = (char *)xmlTextReaderGetAttribute(reader, ATTR_CVE_ID_STR);
	if (ret->id == NULL) {
		cve_entry_free(ret);
		return NULL;	/* if there is no "id" in entry element, return NULL */
	}

	/* If <empty /> then return, because there is no child element */
	if (xmlTextReaderIsEmptyElement(reader))
		return ret;

	/* skip from <entry> node to next one */
	xmlTextReaderNextNode(reader);

	/* while we have element that is not "entry", it is inside this element, otherwise it's ended 
	 * element </entry> and we should end. If there is no one from "if" statement cases, we are parsing
	 * attribute or text ,.. and we can continue to next node.
	 * */
	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVE_STR) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABLE_CONFIGURATION_STR)) {

			conf = oscap_alloc(sizeof(struct cve_configuration));

			conf->id = (char *)xmlTextReaderGetAttribute(reader, ATTR_CVE_ID_STR);
			xmlTextReaderNextElement(reader);
			conf->expr = cpe_testexpr_parse(reader);

			oscap_list_add(ret->configurations, conf);
                        continue;
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABLE_SOFTWARE_LIST_STR)) {
			/* this will be list of products */
			xmlTextReaderNextNode(reader);
			while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABLE_SOFTWARE_LIST_STR) != 0) {
				if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_STR) &&
				    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
					product = cve_product_new();

                                        if (product) {
                                                product->value = oscap_element_string_copy(reader);
						oscap_list_add(ret->products, product);
                                        }
				}
				xmlTextReaderNextNode(reader);
			}
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVE_ID_STR)) {
			ret->cve_id = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PUBLISHED_DATETIME_STR)) {
			ret->published = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_LAST_MODIFIED_DATETIME_STR)) {
			ret->modified = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVSS_STR)) {
		    if (ret->cvss == NULL) ret->cvss = cvss_impact_new_from_xml(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_SECURITY_PROTECTION_STR)) {
			ret->sec_protection = oscap_element_string_copy(reader);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CWE_STR)) {
			ret->cwe = (char *)xmlTextReaderGetAttribute(reader, ATTR_CVE_ID_STR);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REFERENCES_STR)) {
			refer = cve_reference_new();

			if (refer) {
			    refer->type = (char *)xmlTextReaderGetAttribute(reader, ATTR_REFERENCE_TYPE_STR);
			    refer->lang = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "xml:lang");
			    xmlTextReaderNextNode(reader);
			    while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REFERENCES_STR) != 0) {

				if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_SOURCE_STR) &&
				    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
				    refer->source = oscap_element_string_copy(reader);
				} else
				    if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REFERENCE_STR) &&
					xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
					refer->href =
					    (char *)xmlTextReaderGetAttribute(reader, ATTR_REFERENCE_HREF_STR);
					refer->value = oscap_element_string_copy(reader);

				    }
				xmlTextReaderNextNode(reader);
			    }
			    oscap_list_add(ret->references, refer);
			}
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_SUMMARY_STR)) {
			summary = cve_summary_new();

                        if (summary) {
                                summary->summary = oscap_element_string_copy(reader);
				oscap_list_add(ret->summaries, summary);
                        }
		} else {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unknown XML element in CVE entry: %s",
				(const char*) xmlTextReaderConstLocalName(reader));
			cve_entry_free(ret);
			return NULL;
		}

		/* get the next node */
		xmlTextReaderNextNode(reader);
	}

	return ret;
}

/***************************************************************************/
/* Private exporting functions cve_*<structure>*_export( xmlTextWriterPtr )
 * More info in representive header file.
 * returns the type of <structure>
 */
void cve_model_export_xml(struct cve_model *cve, const char *file)
{

	__attribute__nonnull__(cve);
	__attribute__nonnull__(file);

	/* TODO: ad macro to check return value from xmlTextWriter* functions */
	xmlTextWriterPtr writer;

	writer = xmlNewTextWriterFilename(file, 0);
	if (writer == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return;
	}

	/* Set properties of writer TODO: make public function to edit this ?? */
	xmlTextWriterSetIndent(writer, 1);
	xmlTextWriterSetIndentString(writer, BAD_CAST "    ");

	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

	cve_export(cve, writer);
	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cve_export(const struct cve_model *cve, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(cve);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_NVD_STR, FEED_NS);
	xmlTextWriterWriteAttribute(writer, BAD_CAST "nvd_xml_version", BAD_CAST cve->nvd_xml_version);

	if (cve->pub_date) {
		xmlTextWriterWriteAttribute(writer, BAD_CAST "pub_date", BAD_CAST cve->pub_date);
	}

	xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:xsi", BAD_CAST "http://www.w3.org/2001/XMLSchema-instance");
	xmlTextWriterWriteAttribute(writer, BAD_CAST "xsi:schemaLocation", FEED_NS_LOCATION);

	/* dump its contents to XML tree */
	OSCAP_FOREACH(cve_entry, e, cve_model_get_entries(cve), cve_entry_export(e, writer);)

	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cve_reference_export(const struct cve_reference *refer, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(refer);
	__attribute__nonnull__(writer);

	/* references */
	xmlTextWriterStartElementNS(writer, NULL, TAG_REFERENCES_STR, CVE_NS);


	if ((refer->type) != NULL)
		xmlTextWriterWriteAttribute(writer, ATTR_REFERENCE_TYPE_STR, BAD_CAST refer->type);
	if (refer->lang)
		xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang", BAD_CAST refer->lang);

	if ((refer->source) != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_SOURCE_STR, NULL);
		xmlTextWriterWriteString(writer, BAD_CAST refer->source);
		xmlTextWriterEndElement(writer);
	}

	/* reference */
	xmlTextWriterStartElementNS(writer, NULL, TAG_REFERENCE_STR, NULL);
	if (refer->lang) xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang", BAD_CAST refer->lang);

	if ((refer->href) != NULL)
		xmlTextWriterWriteAttribute(writer, ATTR_REFERENCE_HREF_STR, BAD_CAST refer->href);

	xmlTextWriterWriteString(writer, BAD_CAST refer->value);

	/* reference */
	xmlTextWriterEndElement(writer);

	/* references */
	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cve_summary_export(const struct cve_summary *sum, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(sum);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_SUMMARY_STR, CVE_NS);
	xmlTextWriterWriteString(writer, BAD_CAST sum->summary);
	/*</summary> */
	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cve_entry_export(const struct cve_entry *entry, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(entry);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_CVE_STR, NULL);
	if ((entry->id) != NULL)
		xmlTextWriterWriteAttribute(writer, ATTR_CVE_ID_STR, BAD_CAST entry->id);

	OSCAP_FOREACH(cve_configuration, conf, cve_entry_get_configurations(entry),
		      /* dump its contents to XML tree */
		      xmlTextWriterStartElementNS(writer, NULL, TAG_VULNERABLE_CONFIGURATION_STR, CVE_NS);
		      xmlTextWriterWriteAttribute(writer, ATTR_VULNERABLE_CONFIGURATION_ID_STR, BAD_CAST conf->id);
		      cpe_testexpr_export(conf->expr, writer); xmlTextWriterEndElement(writer);)

	    if (oscap_list_get_itemcount(entry->products) != 0) {
		/* TODO: make vulnerable-software-list element with namespace */
		xmlTextWriterStartElementNS(writer, NULL, TAG_VULNERABLE_SOFTWARE_LIST_STR, CVE_NS);
		OSCAP_FOREACH(cve_product, product, cve_entry_get_products(entry),
			      /* dump its contents to XML tree */
			      xmlTextWriterStartElementNS(writer, NULL, TAG_PRODUCT_STR, NULL);
			      xmlTextWriterWriteString(writer, BAD_CAST product->value);
			      xmlTextWriterEndElement(writer);)
		    /*</vulnerable-software-list> */
		    xmlTextWriterEndElement(writer);
	}
	if ((entry->cve_id) != NULL)
		xmlTextWriterWriteElementNS(writer, NULL, TAG_CVE_ID_STR, CVE_NS, BAD_CAST entry->cve_id);

	if ((entry->published) != NULL)
		xmlTextWriterWriteElementNS(writer, NULL, TAG_PUBLISHED_DATETIME_STR, CVE_NS, BAD_CAST entry->published);

	if ((entry->modified) != NULL)
		xmlTextWriterWriteElementNS(writer, NULL, TAG_LAST_MODIFIED_DATETIME_STR, CVE_NS, BAD_CAST entry->modified);

	if ((entry->cvss) != NULL) {
		cvss_impact_export(entry->cvss, writer);
	}
	if ((entry->sec_protection) != NULL)
		xmlTextWriterWriteElementNS(writer, NULL, TAG_SECURITY_PROTECTION_STR, CVE_NS, BAD_CAST entry->sec_protection);

	if ((entry->cwe) != NULL) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_CWE_STR, CVE_NS);
		xmlTextWriterWriteAttribute(writer, ATTR_CWEID_STR, BAD_CAST entry->cwe);
		xmlTextWriterEndElement(writer);
	}

	if ((entry->references) != NULL) {
		/* TODO: make references element with namespace */
		OSCAP_FOREACH(cve_reference, refer, cve_entry_get_references(entry),
			      /* dump its contents to XML tree */
			      cve_reference_export(refer, writer);)
	}
	if ((entry->references) != NULL) {
		/* TODO: make references element with namespace */
		OSCAP_FOREACH(cve_summary, sum, cve_entry_get_summaries(entry),
			      /* dump its contents to XML tree */
			      cve_summary_export(sum, writer);)
	}

	/* </entry> */
	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

/***************************************************************************/
/* Free functions - all will free their subtree so use carefuly !
 */
void cve_summary_free(struct cve_summary *summary)
{

	if (summary == NULL)
		return;

	xmlFree(summary->summary);
	oscap_free(summary);
}

void cve_model_free(struct cve_model *cve_model)
{

	if (cve_model == NULL)
		return;

	oscap_list_free(cve_model->entries, (oscap_destruct_func) cve_entry_free);
	oscap_free(cve_model->pub_date);
	oscap_free(cve_model->nvd_xml_version);
	oscap_free(cve_model);
}

void cve_configuration_free(struct cve_configuration *conf)
{

	if (conf == NULL)
		return;

	xmlFree(conf->id);
	cpe_testexpr_free(conf->expr);
	oscap_free(conf);
}

void cve_product_free(struct cve_product *product)
{

	if (product == NULL)
		return;

	xmlFree(product->value);
	oscap_free(product);
}

void cve_reference_free(struct cve_reference *ref)
{

	if (ref == NULL)
		return;

	xmlFree(ref->value);
	xmlFree(ref->href);
	xmlFree(ref->type);
	xmlFree(ref->source);
	xmlFree(ref->lang);
	oscap_free(ref);
}

void cwe_entry_free(struct cwe_entry *entry)
{

	if (entry == NULL)
		return;

	xmlFree(entry->value);
	oscap_free(entry);
}

void cve_entry_free(struct cve_entry *entry)
{

	if (entry == NULL)
		return;

	xmlFree(entry->id);
	xmlFree(entry->cve_id);
	xmlFree(entry->published);
	xmlFree(entry->modified);
	xmlFree(entry->sec_protection);
	xmlFree(entry->cwe);
	cvss_impact_free(entry->cvss);
	oscap_list_free(entry->products, (oscap_destruct_func) cve_product_free);
	oscap_list_free(entry->references, (oscap_destruct_func) cve_reference_free);
	oscap_list_free(entry->summaries, (oscap_destruct_func) cve_summary_free);
	oscap_list_free(entry->configurations, (oscap_destruct_func) cve_configuration_free);
	oscap_free(entry);
}

/* End of free functions
 * */
/***************************************************************************/
