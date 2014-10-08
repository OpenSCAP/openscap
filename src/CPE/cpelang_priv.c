/**
 * @file cpelang_priv.c
 * \brief Interface to Common Platform Enumeration (CPE) Language
 *
 * See more details at http://nvd.nist.gov/cpe.cfm
 */

/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <libxml/tree.h>
#include <string.h>

#include "public/cpe_lang.h"
#include "cpelang_priv.h"

#include "common/util.h"
#include "common/list.h"
#include "common/text_priv.h"
#include "common/elements.h"
#include "common/_error.h"
#include "common/xmlns_priv.h"
#include "common/xmltext_priv.h"

/***************************************************************************/
/* Variable definitions
 * */
/*
 * */

OSCAP_GETTER(cpe_lang_oper_t, cpe_testexpr, oper)
OSCAP_ITERATOR_GEN(cpe_testexpr)

/*
 * */
struct cpe_lang_model {
	struct oscap_list *platforms;	// list of items
	struct oscap_htable *item;	// item by ID
	char* origin_file; // from which file the lang model was loaded
};
OSCAP_IGETTER_GEN(cpe_platform, cpe_lang_model, platforms)
OSCAP_HGETTER_STRUCT(cpe_platform, cpe_lang_model, item)

/*
 * */
struct cpe_platform {
	struct oscap_list *titles;	// human-readable platform description
	char *id;		// platform ID
	char *remark;		// remark TODO: 0-n !!
	struct cpe_testexpr *expr;	// expression for match evaluation
};

OSCAP_ACCESSOR_STRING(cpe_platform, id)
OSCAP_ACCESSOR_STRING(cpe_platform, remark)
OSCAP_IGETINS(oscap_text, cpe_platform, titles, title)
OSCAP_GETTER(const struct cpe_testexpr*, cpe_platform, expr)

/* End of variable definitions
 * */
/***************************************************************************/
/***************************************************************************/
/* XML string variables definitions
 * */
#define TAG_PLATFORM_SPEC_STR   BAD_CAST "platform-specification"
#define TAG_PLATFORM_STR        BAD_CAST "platform"
#define TAG_LOGICAL_TEST_STR    BAD_CAST "logical-test"
#define TAG_FACT_REF_STR        BAD_CAST "fact-ref"
#define TAG_CHECK_FACT_REF_STR  BAD_CAST "check-fact-ref"
#define TAG_REMARK_STR          BAD_CAST "remark"
#define ATTR_TITLE_STR      BAD_CAST "title"
#define ATTR_NAME_STR       BAD_CAST "name"
#define ATTR_OPERATOR_STR   BAD_CAST "operator"
#define ATTR_NEGATE_STR     BAD_CAST "negate"
#define ATTR_ID_STR         BAD_CAST "id"
#define ATTR_HREF_STR       BAD_CAST "href"
#define ATTR_SYSTEM_STR     BAD_CAST "system"
#define ATTR_ID_REF_STR     BAD_CAST "id-ref"
#define VAL_AND_STR     BAD_CAST "AND"
#define VAL_OR_STR      BAD_CAST "OR"
#define VAL_FALSE_STR   BAD_CAST "false"
#define VAL_TRUE_STR    BAD_CAST "true"
/* End of XML string variables definitions
 * */
/***************************************************************************/
/***************************************************************************/
/* Declaration of static (private to this file) functions
 * These function shoud not be called from outside. For exporting these elements
 * has to call parent element's 
 */
static char *parse_text_element(xmlTextReaderPtr reader, char *name);
static bool cpe_validate_xml(const char *filename);

/* End of static declarations 
 * */
/***************************************************************************/

const struct cpe_testexpr *cpe_testexpr_get_next(const struct cpe_testexpr *expr)
{

	__attribute__nonnull__(expr);

	return ++(expr);
}

/***************************************************************************/
/* Constructors of CPE structures cpe_*<structure>*_new()
 * More info in representive header file.
 * returns the type of <structure>
 */

struct cpe_testexpr *cpe_testexpr_new()
{
	struct cpe_testexpr *ret;

	ret = oscap_calloc(1, sizeof(struct cpe_testexpr));
	if (ret == NULL)
		return NULL;

	ret->oper = CPE_LANG_OPER_AND;
	ret->meta.expr = NULL;

	return ret;
}

struct cpe_testexpr * cpe_testexpr_clone(struct cpe_testexpr * old_expr)
{
	struct cpe_testexpr * new_expr = cpe_testexpr_new();
	new_expr->oper = old_expr->oper;

	switch (new_expr->oper & CPE_LANG_OPER_MASK) {
	case CPE_LANG_OPER_AND:
	case CPE_LANG_OPER_OR:
		new_expr->meta.expr = oscap_list_clone(old_expr->meta.expr, (oscap_clone_func) cpe_testexpr_clone);
		break;
	case CPE_LANG_OPER_MATCH:
		new_expr->meta.cpe = cpe_name_clone(old_expr->meta.cpe);
		break;
	case CPE_LANG_OPER_CHECK:
		new_expr->meta.check.system = oscap_strdup(old_expr->meta.check.system);
		new_expr->meta.check.href = oscap_strdup(old_expr->meta.check.href);
		new_expr->meta.check.id = oscap_strdup(old_expr->meta.check.id);
		break;
	default:
		break;
	}

	return new_expr;
}

struct cpe_lang_model *cpe_lang_model_new()
{

	struct cpe_lang_model *ret;

	ret = oscap_alloc(sizeof(struct cpe_lang_model));
	if (ret == NULL)
		return NULL;

	ret->platforms = oscap_list_new();
	ret->item = oscap_htable_new();
	ret->origin_file = NULL;

	return ret;
}

struct cpe_platform *cpe_platform_new()
{

	struct cpe_platform *ret;

	ret = oscap_alloc(sizeof(struct cpe_platform));
	if (ret == NULL)
		return NULL;

	ret->titles = oscap_list_new();
	ret->expr = cpe_testexpr_new();
	ret->id = NULL;
	ret->remark = NULL;

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

static bool cpe_validate_xml(const char *filename)
{

	__attribute__nonnull__(filename);

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
		oscap_setxmlerr(xmlCtxtGetLastError(ctxt));
		xmlFreeParserCtxt(ctxt);
		return false;
	}
	/* check if validation suceeded */
	if (ctxt->valid)
		ret = true;
	else			/* set xml error */
		oscap_setxmlerr(xmlCtxtGetLastError(ctxt));
	xmlFreeDoc(doc);
	/* free up the parser context */
	xmlFreeParserCtxt(ctxt);
	return ret;
}

struct cpe_lang_model *cpe_lang_model_parse_xml(const char *file)
{

	__attribute__nonnull__(file);

	xmlTextReaderPtr reader;
	struct cpe_lang_model *ret = NULL;

	if (!cpe_validate_xml(file))
		return NULL;

	reader = xmlReaderForFile(file, NULL, 0);
	if (reader != NULL) {
		xmlTextReaderSetErrorHandler(reader, &libxml_error_handler, NULL);
		xmlTextReaderNextNode(reader);
		ret = cpe_lang_model_parse(reader);
	} else {
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "Unable to open file: '%s'", file);
	}
	xmlFreeTextReader(reader);

	return ret;
}

struct cpe_lang_model *cpe_lang_model_parse(xmlTextReaderPtr reader)
{

	struct cpe_lang_model *ret = NULL;
	struct cpe_platform *platform = NULL;

	__attribute__nonnull__(reader);

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PLATFORM_SPEC_STR) &&
	    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {

		ret = cpe_lang_model_new();
		if (ret == NULL)
			return NULL;

		// skip nodes until new element
		xmlTextReaderNextElementWE(reader, TAG_PLATFORM_SPEC_STR);

		while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PLATFORM_STR) == 0) {

			platform = cpe_platform_parse(reader);
			if (platform)
				cpe_lang_model_add_platform(ret, platform);
			xmlTextReaderNextElementWE(reader, TAG_PLATFORM_SPEC_STR);
		}
	}

	return ret;
}

struct cpe_platform *cpe_platform_parse(xmlTextReaderPtr reader)
{

	struct cpe_platform *ret;

	__attribute__nonnull__(reader);

	// allocate platform structure here
	ret = cpe_platform_new();
	if (ret == NULL)
		return NULL;

	// parse platform attributes here
	ret->id = (char *)xmlTextReaderGetAttribute(reader, ATTR_ID_STR);
	if (ret->id == NULL) {
		cpe_platform_free(ret);
		return NULL;	// if there is no "id" in platform element, return NULL
	}
	// skip from <platform> node to next one
	xmlTextReaderNextNode(reader);

	// while we have element that is not "platform", it is inside this element, otherwise it's ended 
	// element </platform> and we should end. If there is no one from "if" statement cases, we are parsing
	// attribute or text ,.. and we can continue to next node.
	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PLATFORM_STR) != 0) {

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), ATTR_TITLE_STR) &&
		    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			oscap_list_add(ret->titles, oscap_text_new_parse(OSCAP_TEXT_TRAITS_PLAIN, reader));
		} else
		    if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMARK_STR) &&
			xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			ret->remark = parse_text_element(reader, (char *)TAG_REMARK_STR);	// TODO: 0-n remarks !
		} else
		    if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_LOGICAL_TEST_STR) &&
			xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			/* Maybe we shall not allocate this in constructor? */
			cpe_testexpr_free(ret->expr);
			ret->expr = cpe_testexpr_parse(reader);
		} else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unknown XML element in platform");
		// get the next node
		xmlTextReaderNextNode(reader);
	}
	return ret;
}

struct cpe_testexpr *cpe_testexpr_parse(xmlTextReaderPtr reader)
{

	xmlChar *temp = NULL;
	size_t elem_cnt = 0;
	struct cpe_testexpr *ret = NULL;

	__attribute__nonnull__(reader);

	// allocation
	ret = cpe_testexpr_new();
	if (ret == NULL)
		return NULL;

	// it's fact-ref only, fill the structure and return it
	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_FACT_REF_STR) &&
	    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
		ret->oper = CPE_LANG_OPER_MATCH;
		temp = xmlTextReaderGetAttribute(reader, ATTR_NAME_STR);
		ret->meta.cpe = cpe_name_new((char *)temp);
		xmlFree(temp);
		return ret;
	// it's check-fact-ref only, fill the structure and return it
	} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CHECK_FACT_REF_STR) &&
	    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
		ret->oper = CPE_LANG_OPER_CHECK;
		ret->meta.check.system = (char*)xmlTextReaderGetAttribute(reader, ATTR_SYSTEM_STR);
		ret->meta.check.href = (char*)xmlTextReaderGetAttribute(reader, ATTR_HREF_STR);
		ret->meta.check.id = (char*)xmlTextReaderGetAttribute(reader, ATTR_ID_REF_STR);
		return ret;
	} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_LOGICAL_TEST_STR) &&
		   xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
		// it's logical-test, fill the structure and go to next node

		temp = xmlTextReaderGetAttribute(reader, ATTR_OPERATOR_STR);
		if (xmlStrcasecmp(temp, VAL_AND_STR) == 0)
			ret->oper = CPE_LANG_OPER_AND;
		else if (xmlStrcasecmp(temp, VAL_OR_STR) == 0)
			ret->oper = CPE_LANG_OPER_OR;
		else {
			// unknown operator problem
			xmlFree(temp);
			oscap_free(ret);
			return NULL;
		}
		xmlFree(temp);

		ret->meta.expr = oscap_list_new(); // initialise a list of subexpressions

		temp = xmlTextReaderGetAttribute(reader, ATTR_NEGATE_STR);
		if (temp && xmlStrcasecmp(temp, VAL_TRUE_STR) == 0)
			ret->oper |= CPE_LANG_OPER_NOT;
		xmlFree(temp);
	} else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unknown XML element in test expression");

	// go to next node
	// skip to next node
	xmlTextReaderNextNode(reader);
        int depth = xmlTextReaderDepth(reader);
        //printf("[%d] logical-test\n", depth);
	// while it's not 'logical-test' or it's not ended element ..
	//while (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_FACT_REF_STR) ||
	//       !xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_LOGICAL_TEST_STR)) {
        while (xmlTextReaderDepth(reader) >= depth) {

                //printf("[%d:%d] logical-test::%s\n", depth, xmlTextReaderDepth(reader), xmlTextReaderConstLocalName(reader));

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}
		elem_cnt++;

		// We assume that the expression is a logical one (meaning that it
		// can have subexpressions).
		// TODO: Enforce that assumption and don't rely on just validation.

		// .. and the next node is logical-test element, we need recursive call
		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_LOGICAL_TEST_STR) &&
		    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			// ret->meta.expr[elem_cnt - 1] = *(cpe_testexpr_parse(reader));
			oscap_list_add(ret->meta.expr, cpe_testexpr_parse(reader));
                        if (xmlTextReaderDepth(reader) < depth) {
                                return ret;
                        } else if (xmlTextReaderDepth(reader) == depth) continue;

		}
		else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_FACT_REF_STR) &&
			    xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			// fill the structure
			struct cpe_testexpr *subexpr = cpe_testexpr_new();
			subexpr->oper = CPE_LANG_OPER_MATCH;
			temp = xmlTextReaderGetAttribute(reader, ATTR_NAME_STR);
			subexpr->meta.cpe = cpe_name_new((char *)temp);
                        //printf("FACT-REF: %s\n", temp);
			xmlFree(temp);
			oscap_list_add(ret->meta.expr, subexpr);
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CHECK_FACT_REF_STR) &&
				xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			struct cpe_testexpr *subexpr = cpe_testexpr_new();
			subexpr->oper = CPE_LANG_OPER_CHECK;
			subexpr->meta.check.system = (char*)xmlTextReaderGetAttribute(reader, ATTR_SYSTEM_STR);
			subexpr->meta.check.href = (char*)xmlTextReaderGetAttribute(reader, ATTR_HREF_STR);
			subexpr->meta.check.id = (char*)xmlTextReaderGetAttribute(reader, ATTR_ID_REF_STR);
			oscap_list_add(ret->meta.expr, subexpr);
		} else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unknown XML element in test expression");
		}
		xmlTextReaderNextNode(reader);
	}
	//ret->meta.expr[elem_cnt].oper = CPE_LANG_OPER_HALT;

	return ret;
}

static char *parse_text_element(xmlTextReaderPtr reader, char *name)
{

	char *string = NULL;

	__attribute__nonnull__(reader);
	__attribute__nonnull__(name);

	// parse string element attributes here (like xml:lang)

	while (xmlTextReaderNextNode(reader)) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT &&
		    !xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST name)) {
			return string;
		}

		switch (xmlTextReaderNodeType(reader)) {
		case XML_READER_TYPE_TEXT:
			string = (char *)xmlTextReaderValue(reader);
			break;
		default:
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Unknown XML element in platform");
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
void cpe_lang_model_export_xml(const struct cpe_lang_model *spec, const char *file)
{

	__attribute__nonnull__(spec);
	__attribute__nonnull__(file);

	// TODO: ad macro to check return value from xmlTextWriter* functions
	xmlTextWriterPtr writer;

	writer = xmlNewTextWriterFilename(file, 0);
	if (writer == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return;
	}
	// Set properties of writer TODO: make public function to edit this ??
	xmlTextWriterSetIndent(writer, 1);
	xmlTextWriterSetIndentString(writer, BAD_CAST "    ");

	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

	cpe_lang_export(spec, writer);
	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cpe_lang_export(const struct cpe_lang_model *spec, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(spec);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_PLATFORM_SPEC_STR, BAD_CAST XMLNS_CPE2L);
		OSCAP_FOREACH(cpe_platform, p, cpe_lang_model_get_platforms(spec),
			      // dump its contents to XML tree
			      cpe_platform_export(p, writer);)
	    xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cpe_platform_export(const struct cpe_platform *platform, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(platform);
	__attribute__nonnull__(writer);

	xmlTextWriterStartElementNS(writer, NULL, TAG_PLATFORM_STR, NULL);
	if (cpe_platform_get_id(platform) != NULL)
		xmlTextWriterWriteAttribute(writer, ATTR_ID_STR, BAD_CAST cpe_platform_get_id(platform));
	oscap_textlist_export(cpe_platform_get_titles(platform), writer, "title");
	cpe_testexpr_export(platform->expr, writer);
	xmlTextWriterEndElement(writer);
	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

void cpe_testexpr_export(const struct cpe_testexpr *expr, xmlTextWriterPtr writer)
{

	__attribute__nonnull__(writer);

	if (expr == NULL || expr->oper == CPE_LANG_OPER_INVALID) return;

	if (expr->oper == CPE_LANG_OPER_MATCH) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_FACT_REF_STR, NULL);
		char *cpe_name_str = cpe_name_get_as_str(expr->meta.cpe);
		xmlTextWriterWriteAttribute(writer, ATTR_NAME_STR, BAD_CAST cpe_name_str);
		oscap_free(cpe_name_str);
		xmlTextWriterEndElement(writer);
	}
	else if (expr->oper == CPE_LANG_OPER_CHECK) {
		xmlTextWriterStartElementNS(writer, NULL, TAG_CHECK_FACT_REF_STR, NULL);
		xmlTextWriterWriteAttribute(writer, ATTR_SYSTEM_STR, BAD_CAST expr->meta.check.system);
		xmlTextWriterWriteAttribute(writer, ATTR_HREF_STR, BAD_CAST expr->meta.check.href);
		xmlTextWriterWriteAttribute(writer, ATTR_ID_REF_STR, BAD_CAST expr->meta.check.id);
		xmlTextWriterEndElement(writer);
	}
	else {
		xmlTextWriterStartElementNS(writer, NULL, TAG_LOGICAL_TEST_STR, BAD_CAST XMLNS_CPE2L);

		if (expr->oper == CPE_LANG_OPER_AND) {
			xmlTextWriterWriteAttribute(writer, ATTR_OPERATOR_STR, VAL_AND_STR);
			xmlTextWriterWriteAttribute(writer, ATTR_NEGATE_STR, VAL_FALSE_STR);
		} else if (expr->oper == CPE_LANG_OPER_OR) {
			xmlTextWriterWriteAttribute(writer, ATTR_OPERATOR_STR, VAL_OR_STR);
			xmlTextWriterWriteAttribute(writer, ATTR_NEGATE_STR, VAL_FALSE_STR);
		} else if (expr->oper == CPE_LANG_OPER_NOR) {
			xmlTextWriterWriteAttribute(writer, ATTR_OPERATOR_STR, VAL_OR_STR);
			xmlTextWriterWriteAttribute(writer, ATTR_NEGATE_STR, VAL_TRUE_STR);
		} else if (expr->oper == CPE_LANG_OPER_NAND) {
			xmlTextWriterWriteAttribute(writer, ATTR_OPERATOR_STR, VAL_AND_STR);
			xmlTextWriterWriteAttribute(writer, ATTR_NEGATE_STR, VAL_TRUE_STR);
		} else {
			/* can this happen? */
			return;
		}

		if (expr->meta.expr == NULL)
			return;
		OSCAP_FOREACH(cpe_testexpr, subexpr, oscap_iterator_new(expr->meta.expr),
			cpe_testexpr_export(subexpr, writer);
		);
		xmlTextWriterEndElement(writer);
	}

	if (xmlGetLastError() != NULL)
		oscap_setxmlerr(xmlGetLastError());
}

/* End of private export functions
 * */
/***************************************************************************/

/***************************************************************************/
/* Free functions - all are static private, do not use them outside this file
 */

void cpe_lang_model_free(struct cpe_lang_model *platformspec)
{
	if (platformspec == NULL)
		return;

	oscap_htable_free0(platformspec->item);
	oscap_list_free(platformspec->platforms, (oscap_destruct_func) cpe_platform_free);
	oscap_free(platformspec->origin_file);

	oscap_free(platformspec);
}

void cpe_platform_free(struct cpe_platform *platform)
{
	if (platform == NULL)
		return;

	xmlFree(platform->id);
	xmlFree(platform->remark);
	oscap_list_free(platform->titles, (oscap_destruct_func) oscap_text_free);
	cpe_testexpr_free(platform->expr);
	oscap_free(platform);
}

static void cpe_testexpr_meta_free(struct cpe_testexpr *expr)
{
	assert(expr != NULL);

	switch (expr->oper & CPE_LANG_OPER_MASK) {
	case CPE_LANG_OPER_AND:
	case CPE_LANG_OPER_OR:
		oscap_list_free(expr->meta.expr, (oscap_destruct_func) cpe_testexpr_free);
		expr->meta.expr = NULL;
		break;
	case CPE_LANG_OPER_MATCH:
		cpe_name_free(expr->meta.cpe);
		expr->meta.cpe = NULL;
		break;
	case CPE_LANG_OPER_CHECK:
		oscap_free(expr->meta.check.system);
		expr->meta.check.system = NULL;
		oscap_free(expr->meta.check.href);
		expr->meta.check.href = NULL;
		oscap_free(expr->meta.check.id);
		expr->meta.check.id = NULL;
		break;
	default:
		break;
	}
}

void cpe_testexpr_free(struct cpe_testexpr *expr)
{
	if (expr == NULL)
		return;

	cpe_testexpr_meta_free(expr);
	expr->oper = 0;
	oscap_free(expr);
}

/* End of free functions
 * */
/***************************************************************************/

/* **************************************************************************
 * Getters / setters / adders (not generated)
 */

struct cpe_testexpr_iterator *cpe_testexpr_get_meta_expr(const struct cpe_testexpr *expr)
{
	if (expr == NULL) return NULL;
	if ((expr->oper & (CPE_LANG_OPER_AND | CPE_LANG_OPER_OR)))
		return (struct cpe_testexpr_iterator*) oscap_iterator_new(expr->meta.expr);
	return NULL;
}

const struct cpe_name *cpe_testexpr_get_meta_cpe(const struct cpe_testexpr *expr)
{
	assert(expr != NULL);
	if ((expr->oper & CPE_LANG_OPER_MASK) != CPE_LANG_OPER_MATCH)
		return NULL;
	return expr->meta.cpe;
}

const char* cpe_testexpr_get_meta_check_system(const struct cpe_testexpr *expr)
{
	assert(expr != NULL);
	if ((expr->oper & CPE_LANG_OPER_MASK) != CPE_LANG_OPER_CHECK)
		return NULL;
	return expr->meta.check.system;
}

const char* cpe_testexpr_get_meta_check_href(const struct cpe_testexpr *expr)
{
	assert(expr != NULL);
	if ((expr->oper & CPE_LANG_OPER_MASK) != CPE_LANG_OPER_CHECK)
		return NULL;
	return expr->meta.check.href;
}

const char* cpe_testexpr_get_meta_check_id(const struct cpe_testexpr *expr)
{
	assert(expr != NULL);
	if ((expr->oper & CPE_LANG_OPER_MASK) != CPE_LANG_OPER_CHECK)
		return NULL;
	return expr->meta.check.id;
}

bool cpe_testexpr_set_oper(struct cpe_testexpr *expr, cpe_lang_oper_t oper)
{
	assert(expr != NULL);

	cpe_testexpr_meta_free(expr);
	expr->oper = oper;
	return true;
}

bool cpe_testexpr_set_name(struct cpe_testexpr *expr, struct cpe_name *name)
{
	assert(expr != NULL);

	if ((expr->oper & CPE_LANG_OPER_MASK) != CPE_LANG_OPER_MATCH)
		return false;

	cpe_testexpr_meta_free(expr);
	expr->meta.cpe = name;
	return true;
}

bool cpe_testexpr_add_subexpression(struct cpe_testexpr *expr, struct cpe_testexpr *sub)
{
	assert(expr != NULL);
	assert(sub != NULL);

	int oper = expr->oper & CPE_LANG_OPER_MASK;
	if (oper != CPE_LANG_OPER_AND && oper != CPE_LANG_OPER_OR)
		return false;
	oscap_list_add(expr->meta.expr, sub);
	return true;
}

bool cpe_lang_model_add_platform(struct cpe_lang_model * lang, struct cpe_platform * platform)
{
	if (lang == NULL || platform == NULL || platform->id == NULL)
		return false;
	oscap_list_add(lang->platforms, platform);
	oscap_htable_add(lang->item, platform->id, platform);
	return true;
}

void cpe_platform_iterator_remove(struct cpe_platform_iterator *it, struct cpe_lang_model *parent)
{
	struct cpe_platform *plat = oscap_iterator_detach((struct oscap_iterator *)it);
	if (plat) {
		oscap_htable_detach(parent->item, cpe_platform_get_id(plat));
		cpe_platform_free(plat);
	}
}

bool cpe_platform_set_expr(struct cpe_platform *platform, struct cpe_testexpr *expr)
{
	assert(platform != NULL);

	if (expr != NULL && !(expr->oper & (CPE_LANG_OPER_AND | CPE_LANG_OPER_OR)))
		return false;

	cpe_testexpr_free(platform->expr);
	platform->expr = expr;
	return true;
}

bool cpe_lang_model_set_origin_file(struct cpe_lang_model* lang_model, const char* origin_file)
{
	oscap_free(lang_model->origin_file);
	lang_model->origin_file = oscap_strdup(origin_file);

	return true;
}

const char* cpe_lang_model_get_origin_file(const struct cpe_lang_model* lang_model)
{
	return lang_model->origin_file;
}
