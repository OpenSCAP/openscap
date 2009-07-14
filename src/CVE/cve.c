/*! \file cve.c
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

#include "cve.h"
#include "../common/util.h"
#include "../common/list.h"

/*
 * Create new CVE Reference structure.
 *
 * @note The returned memory must be freed by the caller.
 * @return new zeroed CVE Reference structure
 * @retval NULL on failure
 */
struct cve_reference *cve_reference_new();

/*
 * Free the CVE Reference structure and its data.
 *
 * @param reference CVE Reference to be freed
 */
void cve_reference_delete(struct cve_reference * reference);

struct cve_info *cve_info_new();

void cve_info_delete(struct cve_info * cve);

/*
 * Parses the specified XML file and creates a list of CVE data structures.
 * The returned list can be freed with cveDelAll().
 *
 * @param xmlfile path to the file to be parsed
 * @param outCveList address of the pointer to which the root element of the list is to be stored
 * @return non-negative value indicates the number of CVEs in the list, negative value indicates an error
 */
int cve_parse(const char *xmlfile, struct cve* outCveList);

struct cve_reference {
	char *summary;		// summary
	char *href;		// href
	char *type;		// reference type
	char *source;		// source
};

struct cve_info {
	char *id;		// id 
	char *pub;		// published datetime
	char *mod;		// last modified datetime
	char *cwe;		// cwe
	char *summary;		// summary

	char *score;		// score
	char *vector;		// access vector
	char *complexity;	// access complexity
	char *authentication;	// authentication
	char *confidentiality;	// confidentiality impact
	char *integrity;	// integrity impact
	char *availability;	// availability impact
	char *source;		// source
	char *generated;	// generated on datetime

	struct oscap_list *references;	// cve references
};

struct cve {
	struct oscap_list* entries;
	struct oscap_htable* entry_by_id;
};


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

static void tagStackInit(tagStack_t * stack)
{
	stack->depth = 16;
	stack->top = -1;
	stack->stack = malloc(stack->depth * sizeof(*stack->stack));
}

static void tagStackDeinit(tagStack_t * stack)
{
	if (stack->stack != NULL) {
		free(stack->stack);
		stack->stack = NULL;
		stack->depth = 0;
		stack->top = -1;
	}
}

static void tagStackPush(tagStack_t * stack, int v)
{
	if (stack->depth == stack->top + 1) {
		stack->depth *= 2;
		stack->stack = realloc(stack->stack, stack->depth);
	}

	stack->top++;
	stack->stack[stack->top] = v;
}

static void tagStackPop(tagStack_t * stack)
{
	if (stack->top >= 0)
		stack->top--;
}

static int tagStackTop(tagStack_t * stack)
{
	if (stack->top >= 0)
		return stack->stack[stack->top];
	return -1;
}

static void pushTag(tagStack_t * stack, const xmlChar * uri,
		    const xmlChar * localName)
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

struct cve_reference *cve_reference_new()
{
	return calloc(1, sizeof(struct cve_reference));
}

void cve_reference_delete(struct cve_reference * ref)
{
	if (ref == NULL)
		return;
	if (ref->summary != NULL)
		free(ref->summary);
	if (ref->href != NULL)
		free(ref->href);
	if (ref->type != NULL)
		free(ref->type);
	if (ref->source != NULL)
		free(ref->source);

	free(ref);
}

struct cve* cve_new_empty(void)
{
	struct cve* cve = calloc(1, sizeof(struct cve));
	cve->entries = oscap_list_new();
	cve->entry_by_id = oscap_htable_new();
	return cve;
}

struct cve* cve_new(const char* fname)
{
	struct cve* cve = cve_new_empty();
	int ret = cve_parse(fname, cve);
	if (ret < 0) {
		cve_delete(cve);
		return NULL;
	}
	return cve;
}

void cve_delete(struct cve* cve)
{
	if (cve) {
		oscap_htable_delete(cve->entry_by_id, NULL);
		oscap_list_delete(cve->entries, (oscap_destruct_func)cve_info_delete);
		free(cve);
	}
}

bool cve_add_info(struct cve* cve, struct cve_info* info)
{
	oscap_list_add(cve->entries, info);
	oscap_htable_add(cve->entry_by_id, info->id, info);
	return true;
}

struct cve_info *cve_info_new()
{
	struct cve_info* info = calloc(1, sizeof(struct cve_info));
	info->references = oscap_list_new();
	return info;
}

void cve_info_delete(struct cve_info * cve)
{
	if (cve == NULL)
		return;
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
	if (cve->score != NULL)
		free(cve->score);
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

	oscap_list_delete(cve->references, (oscap_destruct_func)cve_reference_delete);
	free(cve);
}

int cve_parse(const char *xmlfile, struct cve* out)
{
	xmlTextReaderPtr reader;
	int ret, cve_cnt = 0;
	tagStack_t tagStack;
	struct cve_info *cve = NULL;
	struct cve_reference *ref = NULL;

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
			xmlFree((void *)uri);

			switch (tagStackTop(&tagStack)) {
			case TAG_ENTRY:
				++cve_cnt;
				cve = cve_info_new();
				cve->id =
				    (char *)xmlTextReaderGetAttribute(reader,
								      ATTR_ID_STR);
				cve_add_info(out, cve);
				break;
			case TAG_CWE:
				/* can there be more than one? */
				if (cve->cwe != NULL)
					xmlFree(cve->cwe);
				cve->cwe =
				    (char *)xmlTextReaderGetAttribute(reader,
								      ATTR_CWEID_STR);
				break;
			case TAG_REFS:
				ref = cve_reference_new();
				ref->type =
				    (char *)xmlTextReaderGetAttribute(reader,
								      ATTR_REFTYPE_STR);
				oscap_list_add(cve->references, ref);
				break;
			case TAG_REF:
				if (ref->href != NULL)
					xmlFree(ref->href);
				ref->href =
				    (char *)xmlTextReaderGetAttribute(reader,
								      ATTR_HREF_STR);
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
				cve->pub = (char *)xmlStrdup(text);
				break;
			case TAG_MODIFIED:
				cve->mod = (char *)xmlStrdup(text);
				break;
			case TAG_SCORE:
				cve->score = (char *)xmlStrdup(text);
				break;
			case TAG_VECTOR:
				cve->vector = (char *)xmlStrdup(text);
				break;
			case TAG_COMPLEXITY:
				cve->complexity = (char *)xmlStrdup(text);
				break;
			case TAG_AUTHENTICATION:
				cve->authentication = (char *)xmlStrdup(text);
				break;
			case TAG_CONFIDENTIALITY:
				cve->confidentiality = (char *)xmlStrdup(text);
				break;
			case TAG_INTEGRITY:
				cve->integrity = (char *)xmlStrdup(text);
				break;
			case TAG_AVAILABILITY:
				cve->availability = (char *)xmlStrdup(text);
				break;
			case TAG_CVSSSOURCE:
				cve->source = (char *)xmlStrdup(text);
				break;
			case TAG_GENERATED:
				cve->generated = (char *)xmlStrdup(text);
				break;
			case TAG_SUMMARY:
				cve->summary = (char *)xmlStrdup(text);
				break;
			case TAG_REF:
				ref->summary = (char *)xmlStrdup(text);
				break;
			case TAG_SOURCE:
				ref->source = (char *)xmlStrdup(text);
				break;
			}

			break;
		}

		ret = xmlTextReaderRead(reader);
	}

	xmlFreeTextReader(reader);
	xmlCleanupParser();
	tagStackDeinit(&tagStack);

	//*outCveList = cveList;

	return ret ? -1 : cve_cnt;
}


OSCAP_IGETTER_GEN(cve_info, cve, entries)
OSCAP_HGETTER(struct cve_info*, cve, entry_by_id)

OSCAP_GETTER(const char*, cve_info, id)
OSCAP_GETTER(const char*, cve_info, pub)
OSCAP_GETTER(const char*, cve_info, mod)
OSCAP_GETTER(const char*, cve_info, cwe)
OSCAP_GETTER(const char*, cve_info, summary)
OSCAP_GETTER(const char*, cve_info, score)
OSCAP_GETTER(const char*, cve_info, vector)
OSCAP_GETTER(const char*, cve_info, complexity)
OSCAP_GETTER(const char*, cve_info, authentication)
OSCAP_GETTER(const char*, cve_info, confidentiality)
OSCAP_GETTER(const char*, cve_info, integrity)
OSCAP_GETTER(const char*, cve_info, availability)
OSCAP_GETTER(const char*, cve_info, source)
OSCAP_GETTER(const char*, cve_info, generated)
OSCAP_IGETTER_GEN(cve_reference, cve_info, references)

OSCAP_GETTER(const char*, cve_reference, summary)
OSCAP_GETTER(const char*, cve_reference, href)
OSCAP_GETTER(const char*, cve_reference, type)
OSCAP_GETTER(const char*, cve_reference, source)

