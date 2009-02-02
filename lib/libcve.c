/*! \file libcve.c
 *  \brief Interface to Common Vulnerability and Exposure dictionary
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
 *      Tomas Heinrich <theinric@redhat.com>
 *      Brian Kolbay   <Brian.Kolbay@g2-inc.com>
 */

#include "config.h"

#include <libxml/xmlreader.h>
#include <libxml/xmlstring.h>

#include "libcve.h"

enum {
	TAG_UNKNOWN = 0,
	TAG_ENTRY,
	TAG_CVEID,
	TAG_PUBLISHED,
	TAG_MODIFIED,
	TAG_CWE,
	TAG_REFS,
	TAG_SOURCE,
	TAG_REF,
	TAG_SUMMARY,
	TAG_SCORE,
	TAG_VECTOR,
	TAG_COMPLEXITY,
	TAG_AUTHENTICATION,
	TAG_CONFIDENTIALITY,
	TAG_INTEGRITY,
	TAG_AVAILABILITY,
	TAG_CVSSSOURCE,
	TAG_GENERATED
};

// Vulnerability entry info
#define TAG_ENTRY_STR BAD_CAST "entry"
#define TAG_PUBLISHED_STR BAD_CAST "published-datetime"
#define TAG_MODIFIED_STR BAD_CAST "last-modified-datetime"
#define TAG_CWE_STR BAD_CAST "cwe"
#define TAG_SUMMARY_STR BAD_CAST "summary"
#define ATTR_CWEID_STR BAD_CAST "id"
#define ATTR_ID_STR BAD_CAST "id"

// Vulnerability cvss info
#define TAG_SCORE_STR BAD_CAST "score"
#define TAG_VECTOR_STR BAD_CAST "access-vector"
#define TAG_COMPLEXITY_STR BAD_CAST "access-complexity"
#define TAG_AUTHENTICATION_STR BAD_CAST "authentication"
#define TAG_CONFIDENTIALITY_STR BAD_CAST "confidentiality-impact"
#define TAG_INTEGRITY_STR BAD_CAST "integrity-impact"
#define TAG_AVAILABILITY_STR BAD_CAST "availability-impact"
#define TAG_CVSSSOURCE_STR BAD_CAST "source"
#define TAG_GENERATED_STR BAD_CAST "generated-on-datetime"

// Vulnerability reference info
#define TAG_REFS_STR BAD_CAST "references"
#define ATTR_REFTYPE_STR BAD_CAST "reference_type"
#define ATTR_HREF_STR BAD_CAST "href"
#define TAG_SOURCE_STR BAD_CAST "source"
#define TAG_REF_STR BAD_CAST "reference"

#define URI_VULN_STR BAD_CAST "http://scap.nist.gov/schema/vulnerability/0.4"
#define URI_CVSS_STR BAD_CAST "http://scap.nist.gov/schema/cvss-v2/0.2"

typedef struct {
	int depth;
	int top;
	int *stack;
} tagStack_t;

static void tagStackInit(tagStack_t *stack)
{
	stack->depth = 16;
	stack->top = -1;
	stack->stack = malloc(stack->depth * sizeof (*stack->stack));
}

static void tagStackDeinit(tagStack_t *stack)
{
	if (stack->stack != NULL) {
		free(stack->stack);
		stack->stack = NULL;
		stack->depth = 0;
		stack->top = -1;
	}
}

static void tagStackPush(tagStack_t *stack, int v)
{
	if (stack->depth == stack->top + 1) {
		stack->depth *= 2;
		stack->stack = realloc(stack->stack, stack->depth);
	}

	stack->top++;
	stack->stack[stack->top] = v;
}

static void tagStackPop(tagStack_t *stack)
{
	if (stack->top >= 0)
		stack->top--;
}

static int tagStackTop(tagStack_t *stack)
{
	if (stack->top >= 0)
		return stack->stack[stack->top];
	return -1;
}

static void pushTag(tagStack_t *stack, const xmlChar *uri, const xmlChar *localName)
{
	if (xmlStrcmp(localName, TAG_ENTRY_STR) == 0) {
		tagStackPush(stack, TAG_ENTRY);
	} else if (xmlStrcmp(uri, URI_CVSS_STR) == 0) {
		if (xmlStrcmp(localName, TAG_SCORE_STR) == 0) {
			tagStackPush(stack, TAG_SCORE);
		} else if (xmlStrcmp(localName, TAG_VECTOR_STR) == 0) {
			tagStackPush(stack, TAG_VECTOR);
		} else if (xmlStrcmp(localName, TAG_COMPLEXITY_STR) == 0) {
			tagStackPush(stack, TAG_COMPLEXITY);
		} else if (xmlStrcmp(localName, TAG_AUTHENTICATION_STR) == 0) {
			tagStackPush(stack, TAG_AUTHENTICATION);
		} else if (xmlStrcmp(localName, TAG_CONFIDENTIALITY_STR) == 0) {
			tagStackPush(stack, TAG_CONFIDENTIALITY);
		} else if (xmlStrcmp(localName, TAG_INTEGRITY_STR) == 0) {
			tagStackPush(stack, TAG_INTEGRITY);
		} else if (xmlStrcmp(localName, TAG_AVAILABILITY_STR) == 0) {
			tagStackPush(stack, TAG_AVAILABILITY);
		} else if (xmlStrcmp(localName, TAG_CVSSSOURCE_STR) == 0) {
			tagStackPush(stack, TAG_CVSSSOURCE);
		} else if (xmlStrcmp(localName, TAG_GENERATED_STR) == 0) {
			tagStackPush(stack, TAG_GENERATED);
		} else {
			tagStackPush(stack, TAG_UNKNOWN);
		}
	} else if (xmlStrcmp(uri, URI_VULN_STR) == 0) {
		if (xmlStrcmp(localName, TAG_PUBLISHED_STR) == 0) {
			tagStackPush(stack, TAG_PUBLISHED);
		} else if (xmlStrcmp(localName, TAG_MODIFIED_STR) == 0) {
			tagStackPush(stack, TAG_MODIFIED);
		} else if (xmlStrcmp(localName, TAG_CWE_STR) == 0) {
			tagStackPush(stack, TAG_CWE);
		} else if (xmlStrcmp(localName, TAG_REFS_STR) == 0) {
			tagStackPush(stack, TAG_REFS);
		} else if (xmlStrcmp(localName, TAG_SOURCE_STR) == 0) {
			tagStackPush(stack, TAG_SOURCE);
		} else if (xmlStrcmp(localName, TAG_REF_STR) == 0) {
			tagStackPush(stack, TAG_REF);
		} else if (xmlStrcmp(localName, TAG_SUMMARY_STR) == 0) {
			tagStackPush(stack, TAG_SUMMARY);
		} else {
			tagStackPush(stack, TAG_UNKNOWN);
		}
	} else {
		tagStackPush(stack, TAG_UNKNOWN);
	}
}

cveReference_t *cveReferenceNew()
{
	return calloc(1, sizeof (cveReference_t));
}

void cveReferenceDel(cveReference_t *reference)
{
	if (reference->summary != NULL)
		free(reference->summary);
	if (reference->href != NULL)
		free(reference->href);
	if (reference->refType != NULL)
		free(reference->refType);
	if (reference->source != NULL)
		free(reference->source);

	free(reference);
}

void cveReferenceDelAll(cveReference_t *ref)
{
	cveReference_t *t;

	while (ref != NULL) {
		t = ref;
		ref = ref->next;
		cveReferenceDel(t);
	}
}

cveInfo_t *cveNew()
{
	return calloc(1, sizeof (cveInfo_t));
}

void cveDel(cveInfo_t *cve)
{
	if (cve->id != NULL)
		free(cve->id);
	if (cve->cwe != NULL)
		free(cve->cwe);
	if (cve->pub != NULL)
		free(cve->pub);
	if (cve->mod != NULL)
		free(cve->mod);
	if (cve->summary != NULL)
		free(cve->summary);
	if (cve->score  != NULL)
		free(cve->score );
	if (cve->vector != NULL)
		free(cve->vector);
	if (cve->complexity != NULL)
		free(cve->complexity);
	if (cve->authentication != NULL)
		free(cve->authentication);
	if (cve->confidentiality != NULL)
		free(cve->confidentiality);
	if (cve->integrity != NULL)
		free(cve->integrity);
	if (cve->availability != NULL)
		free(cve->availability);
	if (cve->source != NULL)
		free(cve->source);
	if (cve->generated != NULL)
		free(cve->generated);

	cveReferenceDelAll(cve->refs);
	free(cve);
}

void cveDelAll(cveInfo_t *cve)
{
	cveInfo_t *t;

	while (cve != NULL) {
		t = cve;
		cve = cve->next;
		cveDel(t);
	}
}

int cveParse(char *xmlfile, cveInfo_t **outCveList)
{
	xmlTextReaderPtr reader;
	int ret, cve_cnt = 0;
	tagStack_t tagStack;
	cveInfo_t *cveList = NULL, *cve;
	cveReference_t *reference;

	/*
	 * this initialize the library and check potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */
	LIBXML_TEST_VERSION;

	reader = xmlReaderForFile(xmlfile, NULL, 0);
	if (reader == NULL) {
		return -1;
	}

	tagStackInit(&tagStack);
	ret = xmlTextReaderRead(reader);
	while (ret == 1) {
		const xmlChar *localName, *uri, *text = BAD_CAST "";
		int nodeType;

		nodeType = xmlTextReaderNodeType(reader);
		switch (nodeType) {
		case XML_READER_TYPE_ELEMENT:
			localName = xmlTextReaderConstLocalName(reader);
			uri = xmlTextReaderNamespaceUri(reader);
			pushTag(&tagStack, uri, localName);
			xmlFree((void *) uri);

			switch (tagStackTop(&tagStack)) {
			case TAG_ENTRY:
				++cve_cnt;
				if (cveList == NULL) {
					cveList = cve = cveNew();
				} else {
					cve->next = cveNew();
					cve = cve->next;
				}
				cve->id = (char *) xmlTextReaderGetAttribute(reader, ATTR_ID_STR);
				break;
			case TAG_CWE:
				/* can there be more than one? */
				if (cve->cwe != NULL)
					xmlFree(cve->cwe);
				cve->cwe = (char *) xmlTextReaderGetAttribute(reader, ATTR_CWEID_STR);
				break;
			case TAG_REFS:
				if (cve->refs == NULL) {
					cve->refs = reference = cveReferenceNew();
				} else {
					reference->next = cveReferenceNew();
					reference = reference->next;
				}
				reference->refType = (char *) xmlTextReaderGetAttribute(reader, ATTR_REFTYPE_STR);
				break;
			case TAG_REF:
				if (reference->href != NULL)
					xmlFree(reference->href);
				reference->href = (char *) xmlTextReaderGetAttribute(reader, ATTR_HREF_STR);
				break;
			}

			if (xmlTextReaderIsEmptyElement(reader))
				tagStackPop(&tagStack);
			break;

		case XML_READER_TYPE_END_ELEMENT:
			tagStackPop(&tagStack);
			break;

		case XML_READER_TYPE_TEXT:
			text = xmlTextReaderConstValue(reader);
			switch (tagStackTop(&tagStack)) {
			case TAG_PUBLISHED:
				cve->pub = (char *) xmlStrdup(text);
				break;
			case TAG_MODIFIED:
				cve->mod = (char *) xmlStrdup(text);
				break;
			case TAG_SCORE:
				cve->score = (char *) xmlStrdup(text);
				break;
			case TAG_VECTOR:
				cve->vector = (char *) xmlStrdup(text);
				break;
			case TAG_COMPLEXITY:
				cve->complexity = (char *) xmlStrdup(text);
				break;
			case TAG_AUTHENTICATION:
				cve->authentication = (char *) xmlStrdup(text);
				break;
			case TAG_CONFIDENTIALITY:
				cve->confidentiality = (char *) xmlStrdup(text);
				break;
			case TAG_INTEGRITY:
				cve->integrity = (char *) xmlStrdup(text);
				break;
			case TAG_AVAILABILITY:
				cve->availability = (char *) xmlStrdup(text);
				break;
			case TAG_CVSSSOURCE:
				cve->source = (char *) xmlStrdup(text);
				break;
			case TAG_GENERATED:
				cve->generated = (char *) xmlStrdup(text);
				break;
			case TAG_SUMMARY:
				cve->summary = (char *) xmlStrdup(text);
				break;
			case TAG_REF:
				reference->summary = (char *) xmlStrdup(text);
				break ;
			case TAG_SOURCE:
				reference->source = (char *) xmlStrdup(text);
				break;
			}

			break;
		}

		ret = xmlTextReaderRead(reader);
	}

	xmlFreeTextReader(reader);
	xmlCleanupParser();
	tagStackDeinit(&tagStack);

	*outCveList = cveList;

	return ret ? -1 : cve_cnt;
}
