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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifndef XCCDF_ELEMENTS_H_
#define XCCDF_ELEMENTS_H_

#include <stdbool.h>
#include <time.h>

#include "common/util.h"
#include "common/elements.h"
#include "common/_error.h"

#include <libxml/xmlreader.h>

OSCAP_HIDDEN_START;

/**
 * Detects version from given xmlTextReader
 *
 * The reader has to be at the root <Benchmark> element
 */
const struct xccdf_version_info* xccdf_detect_version_parser(xmlTextReaderPtr reader);

/**
 * Detects version from the xmlTextReader.
 *
 * The reader needs to be at the start of document.
 */
char *xccdf_detect_version_priv(xmlTextReader *reader);

/**
 * Return true if the given namespace is supported XCCDF namespace.
 */
bool xccdf_is_supported_namespace(xmlNs *ns);
int xccdf_version_cmp(const struct xccdf_version_info *actual, const char *desired);

typedef enum {
	XCCDFE_ERROR = -1,
	XCCDFE_UNMATCHED = 0,

	// XCCDF 1.1+ elements
	XCCDFE_BENCHMARK,
	XCCDFE_GROUP,
	XCCDFE_RULE,
	XCCDFE_VALUE,
	XCCDFE_PROFILE,
	XCCDFE_TESTRESULT,
	XCCDFE_RESULT_BENCHMARK,
	XCCDFE_CHECK,
	XCCDFE_CHECK_IMPORT,
	XCCDFE_CHECK_EXPORT,
	XCCDFE_CHECK_CONTENT,
	XCCDFE_CHECK_CONTENT_REF,
	XCCDFE_CHOICES,
	XCCDFE_CHOICE,
	XCCDFE_COMPLEX_CHECK,
	XCCDFE_CONFLICTS,
	XCCDFE_CPE_LIST,
	XCCDFE_DC_STATUS,
	XCCDFE_DEFAULT,
	XCCDFE_DESCRIPTION,
	XCCDFE_FACT,
	XCCDFE_FIX,
	XCCDFE_FIXTEXT,
	XCCDFE_FRONT_MATTER,
	XCCDFE_IDENT,
	XCCDFE_IDENTITY,
	XCCDFE_IMPACT_METRIC,
	XCCDFE_INSTANCE,
	XCCDFE_LOWER_BOUND,
	XCCDFE_MATCH,
	XCCDFE_MESSAGE,
	XCCDFE_METADATA,
	XCCDFE_MODEL,
	XCCDFE_NEW_RESULT,
	XCCDFE_NOTICE,
	XCCDFE_OLD_RESULT,
	XCCDFE_ORGANIZATION,
	XCCDFE_OVERRIDE,
	XCCDFE_PARAM,
	XCCDFE_PLAIN_TEXT,
	XCCDFE_PLATFORM,
	XCCDFE_CPE2_PLATFORMSPEC,
	XCCDFE_RESULT_PROFILE,
	XCCDFE_PROFILE_NOTE,
	XCCDFE_QUESTION,
	XCCDFE_RATIONALE,
	XCCDFE_REAR_MATTER,
	XCCDFE_REFERENCE,
	XCCDFE_REFINE_RULE,
	XCCDFE_REFINE_VALUE,
	XCCDFE_REMARK,
	XCCDFE_REQUIRES,
	XCCDFE_RESULT,
	XCCDFE_RULE_RESULT,
	XCCDFE_SCORE,
	XCCDFE_SELECT,
	XCCDFE_SET_VALUE,
	XCCDFE_SIGNATURE,
	XCCDFE_SOURCE,
	XCCDFE_STATUS,
	XCCDFE_SUB,
	XCCDFE_TAILORING,
	XCCDFE_TARGET,
	XCCDFE_TARGET_ADDRESS,
	XCCDFE_TARGET_FACTS,
	XCCDFE_TARGET_IDENTIFIER,
	XCCDFE_TITLE,
	XCCDFE_UPPER_BOUND,
	XCCDFE_VALUE_VAL,
	XCCDFE_VERSION,
	XCCDFE_WARNING,
	/// benchmark ref and result benchmark have the same element name and namespace
	XCCDFE_BENCHMARK_REF = XCCDFE_RESULT_BENCHMARK,

	XCCDFE_END_
} xccdf_element_t;

const char *xccdf_element_to_str(xccdf_element_t element);
xccdf_element_t xccdf_element_get(xmlTextReaderPtr reader);

#define XCCDF_ASSERT_ELEMENT(reader, element) do { if (xccdf_element_get(reader) != element) { oscap_seterr(OSCAP_EFAMILY_XCCDF, "Find element '%s' while expecting element: '%s'", xccdf_element_to_str((xccdf_element_get(reader))), xccdf_element_to_str(element)); return false; } } while(false)

typedef enum {
	XCCDFA_NONE,
	XCCDFA_ABSTRACT,
	XCCDFA_AUTHENTICATED,
	XCCDFA_AUTHORITY,
	XCCDFA_CATEGORY,
	XCCDFA_CLUSTER_ID,
	XCCDFA_COMPLEXITY,
	XCCDFA_CONTEXT,
	XCCDFA_DATE,
	XCCDFA_DISRUPTION,
	XCCDFA_END_TIME,
	XCCDFA_EXPORT_NAME,
	XCCDFA_EXTENDS,
	XCCDFA_FIXREF,
	XCCDFA_HIDDEN,
	XCCDFA_HREF,
	XCCDFA_ID,
	XCCDFA_IDREF,
	XCCDFA_IID,
	XCCDFA_IMPORT_NAME,
	XCCDFA_IMPORT_XPATH,
	XCCDFA_INTERACTIVE,
	XCCDFA_INTERFACEHINT,
	XCCDFA_MAXIMUM,
	XCCDFA_MULTICHECK,
	XCCDFA_MULTIPLE,
	XCCDFA_MUSTMATCH,
	XCCDFA_NAME,
	XCCDFA_NEGATE,
	XCCDFA_NOTE_TAG,
	XCCDFA_OPERATOR,
	XCCDFA_OVERRIDE,
	XCCDFA_PARENTCONTEXT,
	XCCDFA_PLATFORM,
	XCCDFA_PRIVILEDGED,
	XCCDFA_PROHIBITCHANGES,
	XCCDFA_REBOOT,
	XCCDFA_RESOLVED,
	XCCDFA_ROLE,
	XCCDFA_SELECTED,
	XCCDFA_SELECTOR,
	XCCDFA_SEVERITY,
	XCCDFA_START_TIME,
	XCCDFA_STRATEGY,
	XCCDFA_STYLE,
	XCCDFA_STYLE_HREF,
	XCCDFA_SYSTEM,
	XCCDFA_TAG,
	XCCDFA_TEST_SYSTEM,
	XCCDFA_TIME,
	XCCDFA_TYPE,
	XCCDFA_UPDATE,
	XCCDFA_URI,
	XCCDFA_VALUE_ID,
	XCCDFA_VERSION,
	XCCDFA_WEIGHT,
	XCCDFA_END_
} xccdf_attribute_t;

bool xccdf_attribute_has(xmlTextReaderPtr reader, xccdf_attribute_t attr);
const char *xccdf_attribute_get(xmlTextReaderPtr reader, xccdf_attribute_t attr);
char *xccdf_attribute_copy(xmlTextReaderPtr reader, xccdf_attribute_t attr);
bool xccdf_attribute_get_bool(xmlTextReaderPtr reader, xccdf_attribute_t attr);
int xccdf_attribute_get_int(xmlTextReaderPtr reader, xccdf_attribute_t attr);
float xccdf_attribute_get_float(xmlTextReaderPtr reader, xccdf_attribute_t attr);

extern const struct oscap_string_map XCCDF_BOOL_MAP[];

void xccdf_print_depth(int depth);
void xccdf_print_max(const char *str, int max, const char *ellipsis);
void xccdf_print_max_text(const struct oscap_text *txt, int max, const char *ellipsis);
void xccdf_print_textlist(struct oscap_text_iterator *txt, int depth, int max, const char *ellipsis);

xmlNs *lookup_xccdf_ns(xmlDoc *doc, xmlNode *parent, const struct xccdf_version_info *version_info);

OSCAP_HIDDEN_END;

#endif
