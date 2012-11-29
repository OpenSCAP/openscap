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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <strings.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

#include "public/xccdf_benchmark.h"

#include "elements.h"
#include "common/elements.h"
#include "common/_error.h"
#include "common/debug_priv.h"

struct xccdf_version_info {
	const char* version; ///< MAJOR.MINOR, for example "1.1" or "1.2"
	const char* namespace_uri; ///< XML namespace URI of that particular version
	const char* cpe_version; ///< CPE version used in that particular XCCDF version
};

const char* xccdf_version_info_get_version(const struct xccdf_version_info* v)
{
	if (v == NULL)
		return "unknown";

	return v->version;
}

const char* xccdf_version_info_get_namespace_uri(const struct xccdf_version_info* v)
{
	if (v == NULL)
		return "unknown";

	return v->namespace_uri;
}

const char* xccdf_version_info_get_cpe_version(const struct xccdf_version_info* v)
{
	if (v == NULL)
		return "unknown";

	return v->cpe_version;
}

/// Map of XCCDF versions and their properties
static const struct xccdf_version_info XCCDF_VERSION_MAP[] = {
	{"1.2", "http://checklists.nist.gov/xccdf/1.2", "2.3"},
	{"1.1", "http://checklists.nist.gov/xccdf/1.1", "2.0"},
	{NULL, NULL, NULL}
};

const struct xccdf_version_info* xccdf_detect_version_parser(xmlTextReaderPtr reader)
{
	const struct xccdf_version_info *mapptr;

	const char* namespace_uri = (const char*)xmlTextReaderConstNamespaceUri(reader);
	if (!namespace_uri) {
                oscap_setxmlerr(xmlGetLastError());
                return NULL;
	}

	for (mapptr = XCCDF_VERSION_MAP; mapptr->version != 0; ++mapptr) {
		if (!strcmp(mapptr->namespace_uri, namespace_uri))
			return mapptr;
	}

	oscap_seterr(OSCAP_EFAMILY_XML, "This is not known XCCDF namespace: %s.", namespace_uri);
	return NULL;
}

char * xccdf_detect_version(const char* file)
{
	const struct xccdf_version_info *ver_info;
	char *doc_version;

	xmlTextReaderPtr reader = xmlReaderForFile(file, NULL, 0);
	if (!reader) {
		oscap_seterr(OSCAP_EFAMILY_GLIBC, "Unable to open file: '%s'", file);
		return NULL;
	}
	while (xmlTextReaderRead(reader) == 1 && xmlTextReaderNodeType(reader) != 1);
	ver_info = xccdf_detect_version_parser(reader);

	if(!ver_info) {
		xmlFreeTextReader(reader);
		return NULL;
	}
	doc_version = strdup(xccdf_version_info_get_version(ver_info));

	xmlFreeTextReader(reader);

	return doc_version;
}

bool
xccdf_version_cmp(const struct xccdf_version_info *actual, const char *desired)
{
	/* Since the "unknown" value is strdup-ed during clone, we cannot just use common strcmp.
	 * We need to handle "unknown" version first.
	 */
	if (actual == NULL)
		return -1;
	if (oscap_streq(xccdf_version_info_get_version(actual), "unknown"))
		return 1;
	if (desired == NULL)
		return 1;
	if (oscap_streq(desired, "") || oscap_streq(desired, "unknown"))
		return -1;

	// FIXME: This will only work as long as XCCDF version components don't
	//        go higher than 9. Consider strcmp("9.1", "19.1").
	return strcmp(xccdf_version_info_get_version(actual), desired);
}

struct xccdf_element_spec {
	xccdf_element_t id;	///< element ID
	const char *ns;		///< namespace URI
	const char *name;	///< element name
};

/// CPE version 2 Language XML namespace
#define CPE2L_XMLNS "http://cpe.mitre.org/language/2.0"
/// CPE version 1 dictionary XML namespace
#define CPE1D_XMLNS "http://cpe.mitre.org/XMLSchema/cpe/1.0"

/// Mapping of the element names to the symbolic constants
static const struct xccdf_element_spec XCCDF_ELEMENT_MAP[] = {
	{XCCDFE_BENCHMARK, XCCDF_XMLNS, "Benchmark"},
	{XCCDFE_GROUP, XCCDF_XMLNS, "Group"},
	{XCCDFE_RULE, XCCDF_XMLNS, "Rule"},
	{XCCDFE_VALUE, XCCDF_XMLNS, "Value"},
	{XCCDFE_PROFILE, XCCDF_XMLNS, "Profile"},
	{XCCDFE_TESTRESULT, XCCDF_XMLNS, "TestResult"},
	{XCCDFE_RESULT_BENCHMARK, XCCDF_XMLNS, "benchmark"},
	{XCCDFE_CHECK, XCCDF_XMLNS, "check"},
	{XCCDFE_CHECK_IMPORT, XCCDF_XMLNS, "check-import"},
	{XCCDFE_CHECK_EXPORT, XCCDF_XMLNS, "check-export"},
	{XCCDFE_CHECK_CONTENT, XCCDF_XMLNS, "check-content"},
	{XCCDFE_CHECK_CONTENT_REF, XCCDF_XMLNS, "check-content-ref"},
	{XCCDFE_CHOICES, XCCDF_XMLNS, "choices"},
	{XCCDFE_CHOICE, XCCDF_XMLNS, "choice"},
	{XCCDFE_COMPLEX_CHECK, XCCDF_XMLNS, "complex-check"},
	{XCCDFE_CONFLICTS, XCCDF_XMLNS, "conflicts"},
	{XCCDFE_CPE_LIST, CPE1D_XMLNS, "cpe-list"},
	{XCCDFE_DC_STATUS, XCCDF_XMLNS, "dc-status"},
	{XCCDFE_DEFAULT, XCCDF_XMLNS, "default"},
	{XCCDFE_DESCRIPTION, XCCDF_XMLNS, "description"},
	{XCCDFE_FACT, XCCDF_XMLNS, "fact"},
	{XCCDFE_FIX, XCCDF_XMLNS, "fix"},
	{XCCDFE_FIXTEXT, XCCDF_XMLNS, "fixtext"},
	{XCCDFE_FRONT_MATTER, XCCDF_XMLNS, "front-matter"},
	{XCCDFE_IDENT, XCCDF_XMLNS, "ident"},
	{XCCDFE_IDENTITY, XCCDF_XMLNS, "identity"},
	{XCCDFE_IMPACT_METRIC, XCCDF_XMLNS, "impact-metric"},
	{XCCDFE_INSTANCE, XCCDF_XMLNS, "instance"},
	{XCCDFE_LOWER_BOUND, XCCDF_XMLNS, "lower-bound"},
	{XCCDFE_MATCH, XCCDF_XMLNS, "match"},
	{XCCDFE_MESSAGE, XCCDF_XMLNS, "message"},
	{XCCDFE_METADATA, XCCDF_XMLNS, "metadata"},
	{XCCDFE_MODEL, XCCDF_XMLNS, "model"},
	{XCCDFE_NEW_RESULT, XCCDF_XMLNS, "new-result"},
	{XCCDFE_NOTICE, XCCDF_XMLNS, "notice"},
	{XCCDFE_OLD_RESULT, XCCDF_XMLNS, "old-result"},
	{XCCDFE_ORGANIZATION, XCCDF_XMLNS, "organization"},
	{XCCDFE_OVERRIDE, XCCDF_XMLNS, "override"},
	{XCCDFE_PARAM, XCCDF_XMLNS, "param"},
	{XCCDFE_PLAIN_TEXT, XCCDF_XMLNS, "plain-text"},
	{XCCDFE_PLATFORM, XCCDF_XMLNS, "platform"},
	{XCCDFE_CPE2_PLATFORMSPEC, CPE2L_XMLNS, "platform-specification"},
	{XCCDFE_RESULT_PROFILE, XCCDF_XMLNS, "profile"},
	{XCCDFE_PROFILE_NOTE, XCCDF_XMLNS, "profile-note"},
	{XCCDFE_QUESTION, XCCDF_XMLNS, "question"},
	{XCCDFE_RATIONALE, XCCDF_XMLNS, "rationale"},
	{XCCDFE_REAR_MATTER, XCCDF_XMLNS, "rear-matter"},
	{XCCDFE_REFERENCE, XCCDF_XMLNS, "reference"},
	{XCCDFE_REFINE_RULE, XCCDF_XMLNS, "refine-rule"},
	{XCCDFE_REFINE_VALUE, XCCDF_XMLNS, "refine-value"},
	{XCCDFE_REMARK, XCCDF_XMLNS, "remark"},
	{XCCDFE_REQUIRES, XCCDF_XMLNS, "requires"},
	{XCCDFE_RESULT, XCCDF_XMLNS, "result"},
	{XCCDFE_RULE_RESULT, XCCDF_XMLNS, "rule-result"},
	{XCCDFE_SCORE, XCCDF_XMLNS, "score"},
	{XCCDFE_SELECT, XCCDF_XMLNS, "select"},
	{XCCDFE_SET_VALUE, XCCDF_XMLNS, "set-value"},
	{XCCDFE_SIGNATURE, XCCDF_XMLNS, "signature"},
	{XCCDFE_SOURCE, XCCDF_XMLNS, "source"},
	{XCCDFE_STATUS, XCCDF_XMLNS, "status"},
	{XCCDFE_SUB, XCCDF_XMLNS, "sub"},
	{XCCDFE_TARGET, XCCDF_XMLNS, "target"},
	{XCCDFE_TARGET_ADDRESS, XCCDF_XMLNS, "target-address"},
	{XCCDFE_TARGET_FACTS, XCCDF_XMLNS, "target-facts"},
	{XCCDFE_TARGET_IDENTIFIER, XCCDF_XMLNS, "target-id-ref"},
	{XCCDFE_TITLE, XCCDF_XMLNS, "title"},
	{XCCDFE_UPPER_BOUND, XCCDF_XMLNS, "upper-bound"},
	{XCCDFE_VALUE_VAL, XCCDF_XMLNS, "value"},
	{XCCDFE_VERSION, XCCDF_XMLNS, "version"},
	{XCCDFE_WARNING, XCCDF_XMLNS, "warning"},
	{0, NULL, NULL}
};

xccdf_element_t xccdf_element_get(xmlTextReaderPtr reader)
{
	if (xmlTextReaderNodeType(reader) != 1)
		return XCCDFE_ERROR;

	const struct xccdf_element_spec *mapptr;
	const char *name = (const char *)xmlTextReaderConstLocalName(reader);
	/* const char *nsuri = (const char *)xmlTextReaderConstNamespaceUri(reader); */

	for (mapptr = XCCDF_ELEMENT_MAP; mapptr->id != 0; ++mapptr) {
		// FIXME: We are not checking namespaces as a temporary workaround for multiple versions
		if ((!name/* && !nsuri*/) ||
		    //(name && nsuri && strcmp(mapptr->name, name) == 0 ))
		    (name && /*nsuri &&*/ strcmp(mapptr->name, name) == 0 /*&& strcmp(mapptr->ns, nsuri) == 0*/))
			return mapptr->id;
	}

	return XCCDFE_UNMATCHED;
}

struct xccdf_attribute_spec {
	xccdf_attribute_t id;	///< element ID
	const char *ns;		///< namespace URI
	const char *name;	///< element name
};

static const struct xccdf_attribute_spec XCCDF_ATTRIBUTE_MAP[] = {
	{XCCDFA_ABSTRACT, XCCDF_XMLNS, "abstract"},
	{XCCDFA_AUTHENTICATED, XCCDF_XMLNS, "authenticated"},
	{XCCDFA_AUTHORITY, XCCDF_XMLNS, "authority"},
	{XCCDFA_CATEGORY, XCCDF_XMLNS, "category"},
	{XCCDFA_CLUSTER_ID, XCCDF_XMLNS, "cluster-id"},
	{XCCDFA_COMPLEXITY, XCCDF_XMLNS, "complexity"},
	{XCCDFA_CONTEXT, XCCDF_XMLNS, "context"},
	{XCCDFA_DATE, XCCDF_XMLNS, "date"},
	{XCCDFA_DISRUPTION, XCCDF_XMLNS, "disruption"},
	{XCCDFA_END_TIME, XCCDF_XMLNS, "end-time"},
	{XCCDFA_EXPORT_NAME, XCCDF_XMLNS, "export-name"},
	{XCCDFA_EXTENDS, XCCDF_XMLNS, "extends"},
	{XCCDFA_FIXREF, XCCDF_XMLNS, "fixref"},
	{XCCDFA_HIDDEN, XCCDF_XMLNS, "hidden"},
	{XCCDFA_HREF, XCCDF_XMLNS, "href"},
	{XCCDFA_ID, XCCDF_XMLNS, "id"},
	{XCCDFA_IDREF, XCCDF_XMLNS, "idref"},
	{XCCDFA_IID, XCCDF_XMLNS, "Id"},
	{XCCDFA_IMPORT_NAME, XCCDF_XMLNS, "import-name"},
	{XCCDFA_IMPORT_XPATH, XCCDF_XMLNS, "import-xpath"},
	{XCCDFA_INTERACTIVE, XCCDF_XMLNS, "interactive"},
	{XCCDFA_INTERFACEHINT, XCCDF_XMLNS, "interfaceHint"},
	{XCCDFA_MAXIMUM, XCCDF_XMLNS, "maximum"},
	{XCCDFA_MULTICHECK, XCCDF_XMLNS, "multi-check"},
	{XCCDFA_MULTIPLE, XCCDF_XMLNS, "multiple"},
	{XCCDFA_MUSTMATCH, XCCDF_XMLNS, "mustMatch"},
	{XCCDFA_NAME, XCCDF_XMLNS, "name"},
	{XCCDFA_NEGATE, XCCDF_XMLNS, "negate"},
	{XCCDFA_NOTE_TAG, XCCDF_XMLNS, "note-tag"},
	{XCCDFA_OPERATOR, XCCDF_XMLNS, "operator"},
	{XCCDFA_OVERRIDE, XCCDF_XMLNS, "override"},
	{XCCDFA_PARENTCONTEXT, XCCDF_XMLNS, "parentContext"},
	{XCCDFA_PLATFORM, XCCDF_XMLNS, "platform"},
	{XCCDFA_PRIVILEDGED, XCCDF_XMLNS, "priviledged"},
	{XCCDFA_PROHIBITCHANGES, XCCDF_XMLNS, "prohibitChanges"},
	{XCCDFA_REBOOT, XCCDF_XMLNS, "reboot"},
	{XCCDFA_RESOLVED, XCCDF_XMLNS, "resolved"},
	{XCCDFA_ROLE, XCCDF_XMLNS, "role"},
	{XCCDFA_SELECTED, XCCDF_XMLNS, "selected"},
	{XCCDFA_SELECTOR, XCCDF_XMLNS, "selector"},
	{XCCDFA_SEVERITY, XCCDF_XMLNS, "severity"},
	{XCCDFA_START_TIME, XCCDF_XMLNS, "start-time"},
	{XCCDFA_STRATEGY, XCCDF_XMLNS, "strategy"},
	{XCCDFA_STYLE, XCCDF_XMLNS, "style"},
	{XCCDFA_STYLE_HREF, XCCDF_XMLNS, "style-href"},
	{XCCDFA_SYSTEM, XCCDF_XMLNS, "system"},
	{XCCDFA_TAG, XCCDF_XMLNS, "tag"},
	{XCCDFA_TEST_SYSTEM, XCCDF_XMLNS, "test-system"},
	{XCCDFA_TIME, XCCDF_XMLNS, "time"},
	{XCCDFA_TYPE, XCCDF_XMLNS, "type"},
	{XCCDFA_UPDATE, XCCDF_XMLNS, "update"},
	{XCCDFA_URI, XCCDF_XMLNS, "uri"},
	{XCCDFA_VALUE_ID, XCCDF_XMLNS, "value-id"},
	{XCCDFA_VERSION, XCCDF_XMLNS, "version"},
	{XCCDFA_WEIGHT, XCCDF_XMLNS, "weight"},
	{0, NULL, NULL}
};

bool xccdf_attribute_has(xmlTextReaderPtr reader, xccdf_attribute_t attr)
{
	return xccdf_attribute_get(reader, attr) != NULL;
}

const char *xccdf_attribute_get(xmlTextReaderPtr reader, xccdf_attribute_t attr)
{
	bool found = false;
	const struct xccdf_attribute_spec *mapptr = XCCDF_ATTRIBUTE_MAP;

	while (mapptr->id) {
		if (attr == mapptr->id) {
			found = true;
			break;
		}
		++mapptr;
	}

	if (!found)
		return NULL;

	if (xmlTextReaderMoveToAttribute(reader, BAD_CAST mapptr->name) != 1)
		return NULL;
	// do not check the XML namespace for now... maybe a libxml bug?
	// if (strcmp((const char*)xmlTextReaderConstNamespaceUri(reader), BAD_CAST mapptr->ns) != 0) return NULL;

	return (const char *)xmlTextReaderConstValue(reader);
}

char *xccdf_attribute_copy(xmlTextReaderPtr reader, xccdf_attribute_t attr)
{
	const char *ret = xccdf_attribute_get(reader, attr);
	if (ret)
		return strdup(ret);
	return NULL;
}


bool xccdf_attribute_get_bool(xmlTextReaderPtr reader, xccdf_attribute_t attr)
{
	return oscap_string_to_enum(OSCAP_BOOL_MAP, xccdf_attribute_get(reader, attr));
}

float xccdf_attribute_get_float(xmlTextReaderPtr reader, xccdf_attribute_t attr)
{
	float res;
	if (xccdf_attribute_has(reader, attr) && sscanf(xccdf_attribute_get(reader, attr), "%f", &res) == 1)
		return res;
	else
		return NAN;
}


void xccdf_print_depth(int depth)
{
	while (depth--)
		printf("  ");
}

void xccdf_print_max_text(const struct oscap_text *txt, int max, const char *ellipsis)
{
    if (txt == NULL) return;

    printf("[%c%c%c|%s] ",
        oscap_text_get_is_html(txt) ? 'h' : '-',
        oscap_text_get_can_override(txt) ? (oscap_text_get_overrides(txt) ? 'O' : 'o') : '-',
        oscap_text_get_can_substitute(txt) ? 's' : '-',
        oscap_text_get_lang(txt));
    xccdf_print_max(oscap_text_get_text(txt), max, ellipsis);
}

void xccdf_print_textlist(struct oscap_text_iterator *txt, int depth, int max, const char *ellipsis)
{
    if (txt == NULL) return;

    printf("\n");

    while (oscap_text_iterator_has_more(txt)) {
        struct oscap_text *text = oscap_text_iterator_next(txt);
        xccdf_print_depth(depth);
        xccdf_print_max_text(text, max, ellipsis);
        printf("\n");
    }

    oscap_text_iterator_free(txt);
}

void xccdf_print_max(const char *str, int max, const char *ellipsis)
{
	if (str)
		while (isspace(*str))
			++str;
	int len = strlen("(null)");;
	char buf[32];
	if (str)
		len = strlen(str);
	sprintf(buf, "%%.%ds", max);
	printf(buf, str);
	if (len > max)
		printf("%s", ellipsis);
}

