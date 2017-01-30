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
#include "common/xmlns_priv.h"

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
	{"1.1", "http://open-scap.org/page/Xccdf-1.1-tailoring", "2.0"},
	{NULL, NULL, NULL}
};

static const struct xccdf_version_info *_namespace_get_xccdf_version_info(const char *namespace_uri)
{
	const struct xccdf_version_info *mapptr;
	for (mapptr = XCCDF_VERSION_MAP; mapptr->version != 0; ++mapptr) {
		if (!strcmp(mapptr->namespace_uri, namespace_uri))
			return mapptr;
	}
	return NULL;
}

const struct xccdf_version_info* xccdf_detect_version_parser(xmlTextReaderPtr reader)
{
	const struct xccdf_version_info *mapptr;

	const char* namespace_uri = (const char*)xmlTextReaderConstNamespaceUri(reader);
	if (!namespace_uri) {
                oscap_setxmlerr(xmlGetLastError());
                return NULL;
	}

	mapptr = _namespace_get_xccdf_version_info(namespace_uri);
	if (mapptr == NULL)
		oscap_seterr(OSCAP_EFAMILY_XML, "This is not known XCCDF namespace: %s.", namespace_uri);
	return mapptr;
}

bool xccdf_is_supported_namespace(xmlNs *ns)
{
	return ns != NULL && ns->href != NULL &&
		_namespace_get_xccdf_version_info((const char *) ns->href) != NULL;
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

	xmlTextReaderSetErrorHandler(reader, &libxml_error_handler, NULL);

	while (xmlTextReaderRead(reader) == 1 && xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT);
	ver_info = xccdf_detect_version_parser(reader);

	if(!ver_info) {
		xmlFreeTextReader(reader);
		return NULL;
	}
	doc_version = strdup(xccdf_version_info_get_version(ver_info));

	xmlFreeTextReader(reader);

	return doc_version;
}

int
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

#ifdef __USE_GNU
	return strverscmp(xccdf_version_info_get_version(actual), desired);
#else
	return strcmp(xccdf_version_info_get_version(actual), desired);
#endif
}

struct xccdf_element_spec {
	xccdf_element_t id;	///< element ID
	const char *ns;		///< namespace URI
	const char *name;	///< element name
};

#define XCCDF_XMLNS_PREFIX "http://checklists.nist.gov/xccdf"
#define XCCDF_OR_TAILORING_EXTENSION_XMLNS "http://open-scap.org/page/Xccdf-1.1-tailoring"

/// Mapping of the element names to the symbolic constants
static const struct xccdf_element_spec XCCDF_ELEMENT_MAP[] = {
	{XCCDFE_BENCHMARK, XCCDF_XMLNS_PREFIX, "Benchmark"},
	{XCCDFE_GROUP, XCCDF_XMLNS_PREFIX, "Group"},
	{XCCDFE_RULE, XCCDF_XMLNS_PREFIX, "Rule"},
	{XCCDFE_VALUE, XCCDF_XMLNS_PREFIX, "Value"},
	{XCCDFE_PROFILE, XCCDF_XMLNS_PREFIX, "Profile"},
	{XCCDFE_TESTRESULT, XCCDF_XMLNS_PREFIX, "TestResult"},
	{XCCDFE_RESULT_BENCHMARK, XCCDF_OR_TAILORING_EXTENSION_XMLNS, "benchmark"}, ///< this is also XCCDFE_BENCHMARK_REF
	{XCCDFE_CHECK, XCCDF_XMLNS_PREFIX, "check"},
	{XCCDFE_CHECK_IMPORT, XCCDF_XMLNS_PREFIX, "check-import"},
	{XCCDFE_CHECK_EXPORT, XCCDF_XMLNS_PREFIX, "check-export"},
	{XCCDFE_CHECK_CONTENT, XCCDF_XMLNS_PREFIX, "check-content"},
	{XCCDFE_CHECK_CONTENT_REF, XCCDF_XMLNS_PREFIX, "check-content-ref"},
	{XCCDFE_CHOICES, XCCDF_XMLNS_PREFIX, "choices"},
	{XCCDFE_CHOICE, XCCDF_XMLNS_PREFIX, "choice"},
	{XCCDFE_COMPLEX_CHECK, XCCDF_XMLNS_PREFIX, "complex-check"},
	{XCCDFE_CONFLICTS, XCCDF_XMLNS_PREFIX, "conflicts"},
	{XCCDFE_CPE_LIST, XMLNS_CPE1D, "cpe-list"},
	{XCCDFE_DC_STATUS, XCCDF_XMLNS_PREFIX, "dc-status"},
	{XCCDFE_DEFAULT, XCCDF_XMLNS_PREFIX, "default"},
	{XCCDFE_DESCRIPTION, XCCDF_XMLNS_PREFIX, "description"},
	{XCCDFE_FACT, XCCDF_XMLNS_PREFIX, "fact"},
	{XCCDFE_FIX, XCCDF_XMLNS_PREFIX, "fix"},
	{XCCDFE_FIXTEXT, XCCDF_XMLNS_PREFIX, "fixtext"},
	{XCCDFE_FRONT_MATTER, XCCDF_XMLNS_PREFIX, "front-matter"},
	{XCCDFE_IDENT, XCCDF_XMLNS_PREFIX, "ident"},
	{XCCDFE_IDENTITY, XCCDF_XMLNS_PREFIX, "identity"},
	{XCCDFE_IMPACT_METRIC, XCCDF_XMLNS_PREFIX, "impact-metric"},
	{XCCDFE_INSTANCE, XCCDF_XMLNS_PREFIX, "instance"},
	{XCCDFE_LOWER_BOUND, XCCDF_XMLNS_PREFIX, "lower-bound"},
	{XCCDFE_MATCH, XCCDF_XMLNS_PREFIX, "match"},
	{XCCDFE_MESSAGE, XCCDF_XMLNS_PREFIX, "message"},
	{XCCDFE_METADATA, XCCDF_XMLNS_PREFIX, "metadata"},
	{XCCDFE_MODEL, XCCDF_XMLNS_PREFIX, "model"},
	{XCCDFE_NEW_RESULT, XCCDF_XMLNS_PREFIX, "new-result"},
	{XCCDFE_NOTICE, XCCDF_XMLNS_PREFIX, "notice"},
	{XCCDFE_OLD_RESULT, XCCDF_XMLNS_PREFIX, "old-result"},
	{XCCDFE_ORGANIZATION, XCCDF_XMLNS_PREFIX, "organization"},
	{XCCDFE_OVERRIDE, XCCDF_XMLNS_PREFIX, "override"},
	{XCCDFE_PARAM, XCCDF_XMLNS_PREFIX, "param"},
	{XCCDFE_PLAIN_TEXT, XCCDF_XMLNS_PREFIX, "plain-text"},
	{XCCDFE_PLATFORM, XCCDF_XMLNS_PREFIX, "platform"},
	{XCCDFE_CPE2_PLATFORMSPEC, XMLNS_CPE2L, "platform-specification"},
	{XCCDFE_RESULT_PROFILE, XCCDF_XMLNS_PREFIX, "profile"},
	{XCCDFE_PROFILE_NOTE, XCCDF_XMLNS_PREFIX, "profile-note"},
	{XCCDFE_QUESTION, XCCDF_XMLNS_PREFIX, "question"},
	{XCCDFE_RATIONALE, XCCDF_XMLNS_PREFIX, "rationale"},
	{XCCDFE_REAR_MATTER, XCCDF_XMLNS_PREFIX, "rear-matter"},
	{XCCDFE_REFERENCE, XCCDF_XMLNS_PREFIX, "reference"},
	{XCCDFE_REFINE_RULE, XCCDF_XMLNS_PREFIX, "refine-rule"},
	{XCCDFE_REFINE_VALUE, XCCDF_XMLNS_PREFIX, "refine-value"},
	{XCCDFE_REMARK, XCCDF_XMLNS_PREFIX, "remark"},
	{XCCDFE_REQUIRES, XCCDF_XMLNS_PREFIX, "requires"},
	{XCCDFE_RESULT, XCCDF_XMLNS_PREFIX, "result"},
	{XCCDFE_RULE_RESULT, XCCDF_XMLNS_PREFIX, "rule-result"},
	{XCCDFE_SCORE, XCCDF_XMLNS_PREFIX, "score"},
	{XCCDFE_SELECT, XCCDF_XMLNS_PREFIX, "select"},
	{XCCDFE_SET_VALUE, XCCDF_XMLNS_PREFIX, "set-value"},
	{XCCDFE_SIGNATURE, XCCDF_XMLNS_PREFIX, "signature"},
	{XCCDFE_SOURCE, XCCDF_XMLNS_PREFIX, "source"},
	{XCCDFE_STATUS, XCCDF_XMLNS_PREFIX, "status"},
	{XCCDFE_SUB, XCCDF_XMLNS_PREFIX, "sub"},
	{XCCDFE_TAILORING, XCCDF_OR_TAILORING_EXTENSION_XMLNS, "Tailoring"},
	{XCCDFE_TARGET, XCCDF_XMLNS_PREFIX, "target"},
	{XCCDFE_TARGET_ADDRESS, XCCDF_XMLNS_PREFIX, "target-address"},
	{XCCDFE_TARGET_FACTS, XCCDF_XMLNS_PREFIX, "target-facts"},
	{XCCDFE_TARGET_IDENTIFIER, XCCDF_XMLNS_PREFIX, "target-id-ref"},
	{XCCDFE_TITLE, XCCDF_XMLNS_PREFIX, "title"},
	{XCCDFE_UPPER_BOUND, XCCDF_XMLNS_PREFIX, "upper-bound"},
	{XCCDFE_VALUE_VAL, XCCDF_XMLNS_PREFIX, "value"},
	{XCCDFE_VERSION, XCCDF_OR_TAILORING_EXTENSION_XMLNS, "version"},
	{XCCDFE_WARNING, XCCDF_XMLNS_PREFIX, "warning"},
	{0, NULL, NULL}
};

xccdf_element_t xccdf_element_get(xmlTextReaderPtr reader)
{
	if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT)
		return XCCDFE_ERROR;

	const struct xccdf_element_spec *mapptr;
	const char *name = (const char *)xmlTextReaderConstLocalName(reader);
	const char *nsuri = (const char *)xmlTextReaderConstNamespaceUri(reader);

	for (mapptr = XCCDF_ELEMENT_MAP; mapptr->id != 0; ++mapptr) {
		if (name && strcmp(mapptr->name, name) == 0) {
			// element name exists and matches

			if (!nsuri && !mapptr->ns)
				// no namespace for the element and no namespace defined in the table
				return mapptr->id;

			if (nsuri && strcmp(mapptr->ns, nsuri) == 0)
				// namespace exists and matches the table
				return mapptr->id;

			if (nsuri && (strcmp(mapptr->ns, XCCDF_XMLNS_PREFIX) == 0 || strcmp(mapptr->ns, XCCDF_OR_TAILORING_EXTENSION_XMLNS) == 0) &&
			    strncmp(XCCDF_XMLNS_PREFIX, nsuri, strlen(XCCDF_XMLNS_PREFIX)) == 0)
				// Namespace exists and XCCDF version-less namespace prefix matches
				// it. This is done to support multiple XCCDF versions.
				//
				// XSD schema will make sure there is no XCCDF version mixing in
				// the document. The only thing we are solving here is making sure
				// the element is some kind of an XCCDF element.
				return mapptr->id;

			if (nsuri && strcmp(mapptr->ns, XCCDF_OR_TAILORING_EXTENSION_XMLNS) == 0 &&
			    strcmp(XCCDF_OR_TAILORING_EXTENSION_XMLNS, nsuri) == 0)
				// In case we are supposed to accept XCCDF tailoring extension
				return mapptr->id;
		}
	}

	return XCCDFE_UNMATCHED;
}

struct xccdf_attribute_spec {
	xccdf_attribute_t id;	///< element ID
	const char *ns;		///< namespace URI
	const char *name;	///< element name
};

static const struct xccdf_attribute_spec XCCDF_ATTRIBUTE_MAP[] = {
	{XCCDFA_ABSTRACT, XCCDF_XMLNS_PREFIX, "abstract"},
	{XCCDFA_AUTHENTICATED, XCCDF_XMLNS_PREFIX, "authenticated"},
	{XCCDFA_AUTHORITY, XCCDF_XMLNS_PREFIX, "authority"},
	{XCCDFA_CATEGORY, XCCDF_XMLNS_PREFIX, "category"},
	{XCCDFA_CLUSTER_ID, XCCDF_XMLNS_PREFIX, "cluster-id"},
	{XCCDFA_COMPLEXITY, XCCDF_XMLNS_PREFIX, "complexity"},
	{XCCDFA_CONTEXT, XCCDF_XMLNS_PREFIX, "context"},
	{XCCDFA_DATE, XCCDF_XMLNS_PREFIX, "date"},
	{XCCDFA_DISRUPTION, XCCDF_XMLNS_PREFIX, "disruption"},
	{XCCDFA_END_TIME, XCCDF_XMLNS_PREFIX, "end-time"},
	{XCCDFA_EXPORT_NAME, XCCDF_XMLNS_PREFIX, "export-name"},
	{XCCDFA_EXTENDS, XCCDF_XMLNS_PREFIX, "extends"},
	{XCCDFA_FIXREF, XCCDF_XMLNS_PREFIX, "fixref"},
	{XCCDFA_HIDDEN, XCCDF_XMLNS_PREFIX, "hidden"},
	{XCCDFA_HREF, XCCDF_OR_TAILORING_EXTENSION_XMLNS, "href"},
	{XCCDFA_ID, XCCDF_XMLNS_PREFIX, "id"},
	{XCCDFA_IDREF, XCCDF_XMLNS_PREFIX, "idref"},
	{XCCDFA_IID, XCCDF_XMLNS_PREFIX, "Id"},
	{XCCDFA_IMPORT_NAME, XCCDF_XMLNS_PREFIX, "import-name"},
	{XCCDFA_IMPORT_XPATH, XCCDF_XMLNS_PREFIX, "import-xpath"},
	{XCCDFA_INTERACTIVE, XCCDF_XMLNS_PREFIX, "interactive"},
	{XCCDFA_INTERFACEHINT, XCCDF_XMLNS_PREFIX, "interfaceHint"},
	{XCCDFA_MAXIMUM, XCCDF_XMLNS_PREFIX, "maximum"},
	{XCCDFA_MULTICHECK, XCCDF_XMLNS_PREFIX, "multi-check"},
	{XCCDFA_MULTIPLE, XCCDF_XMLNS_PREFIX, "multiple"},
	{XCCDFA_MUSTMATCH, XCCDF_XMLNS_PREFIX, "mustMatch"},
	{XCCDFA_NAME, XCCDF_XMLNS_PREFIX, "name"},
	{XCCDFA_NEGATE, XCCDF_XMLNS_PREFIX, "negate"},
	{XCCDFA_NOTE_TAG, XCCDF_XMLNS_PREFIX, "note-tag"},
	{XCCDFA_OPERATOR, XCCDF_XMLNS_PREFIX, "operator"},
	{XCCDFA_OVERRIDE, XCCDF_XMLNS_PREFIX, "override"},
	{XCCDFA_PARENTCONTEXT, XCCDF_XMLNS_PREFIX, "parentContext"},
	{XCCDFA_PLATFORM, XCCDF_XMLNS_PREFIX, "platform"},
	{XCCDFA_PRIVILEDGED, XCCDF_XMLNS_PREFIX, "priviledged"},
	{XCCDFA_PROHIBITCHANGES, XCCDF_XMLNS_PREFIX, "prohibitChanges"},
	{XCCDFA_REBOOT, XCCDF_XMLNS_PREFIX, "reboot"},
	{XCCDFA_RESOLVED, XCCDF_XMLNS_PREFIX, "resolved"},
	{XCCDFA_ROLE, XCCDF_XMLNS_PREFIX, "role"},
	{XCCDFA_SELECTED, XCCDF_XMLNS_PREFIX, "selected"},
	{XCCDFA_SELECTOR, XCCDF_XMLNS_PREFIX, "selector"},
	{XCCDFA_SEVERITY, XCCDF_XMLNS_PREFIX, "severity"},
	{XCCDFA_START_TIME, XCCDF_XMLNS_PREFIX, "start-time"},
	{XCCDFA_STRATEGY, XCCDF_XMLNS_PREFIX, "strategy"},
	{XCCDFA_STYLE, XCCDF_XMLNS_PREFIX, "style"},
	{XCCDFA_STYLE_HREF, XCCDF_XMLNS_PREFIX, "style-href"},
	{XCCDFA_SYSTEM, XCCDF_XMLNS_PREFIX, "system"},
	{XCCDFA_TAG, XCCDF_XMLNS_PREFIX, "tag"},
	{XCCDFA_TEST_SYSTEM, XCCDF_XMLNS_PREFIX, "test-system"},
	{XCCDFA_TIME, XCCDF_OR_TAILORING_EXTENSION_XMLNS, "time"},
	{XCCDFA_TYPE, XCCDF_XMLNS_PREFIX, "type"},
	{XCCDFA_UPDATE, XCCDF_XMLNS_PREFIX, "update"},
	{XCCDFA_URI, XCCDF_XMLNS_PREFIX, "uri"},
	{XCCDFA_VALUE_ID, XCCDF_XMLNS_PREFIX, "value-id"},
	{XCCDFA_VERSION, XCCDF_XMLNS_PREFIX, "version"},
	{XCCDFA_WEIGHT, XCCDF_XMLNS_PREFIX, "weight"},
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

