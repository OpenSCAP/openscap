#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "public/cvrf.h"
#include "cvrf_priv.h"

#include "common/list.h"
#include "common/_error.h"
#include "common/xmltext_priv.h"
#include "common/elements.h"
#include "common/oscap_string.h"
#include "common/util.h"

#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"



static int cvrf_enumeration_attr(xmlTextReaderPtr reader, char *attname, const struct oscap_string_map *map)
{
	char *attrstr = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST attname);
	if (attrstr == NULL)
		return 0;
	int ret = oscap_string_to_enum(map, attrstr);
	oscap_free(attrstr);
	return ret;
}

static const char *cvrf_enumeration_get_text(const struct oscap_string_map *map, int val)
{
	return oscap_enum_to_string(map, val);
}
static int cvrf_enumeration_from_text(const struct oscap_string_map *map, const char *text)
{
	return oscap_string_to_enum(map, text);
}


/*****************************************************************************
 * CVRF enum map definitions
 */

const struct oscap_string_map CVRF_DOC_STATUS_TYPE_MAP[] = {
	{CVRF_DOC_STATUS_DRAFT, "Draft"},
	{CVRF_DOC_STATUS_INTERIM, "Interim"},
	{CVRF_DOC_STATUS_FINAL, "Final"},
	{0, NULL}
};
cvrf_doc_status_type_t cvrf_doc_status_type_parse(xmlTextReaderPtr reader, char *attname)
{
	return cvrf_enumeration_attr(reader, attname, CVRF_DOC_STATUS_TYPE_MAP);
}
const char *cvrf_doc_status_type_get_text(cvrf_doc_status_type_t doc_status_type)
{
	return cvrf_enumeration_get_text(CVRF_DOC_STATUS_TYPE_MAP, doc_status_type);
}


const struct oscap_string_map CVRF_BRANCH_TYPE_MAP[] = {
	{CVRF_BRANCH_VENDOR, "Vendor"},
	{CVRF_BRANCH_PRODUCT_FAMILY, "Product Family"},
	{CVRF_BRANCH_PRODUCT_NAME, "Product Name"},
	{CVRF_BRANCH_PRODUCT_VERSION, "Product Version"},
	{CVRF_BRANCH_PATCH_LEVEL, "Patch Level"},
	{CVRF_BRANCH_SERVICE_PACK, "Service Pack"},
	{CVRF_BRANCH_ARCHITECTURE, "Architecture"},
	{CVRF_BRANCH_LANGUAGE, "Language"},
	{CVRF_BRANCH_LEGACY, "Legacy"},
	{CVRF_BRANCH_SPECIFICATION, "Specification"},
	{0, NULL}
};
cvrf_branch_type_t cvrf_branch_type_parse(xmlTextReaderPtr reader, char *attname)
{
	return cvrf_enumeration_attr(reader, attname, CVRF_BRANCH_TYPE_MAP);
}
const char *cvrf_branch_type_get_text(cvrf_branch_type_t branch_type)
{
	return cvrf_enumeration_get_text(CVRF_BRANCH_TYPE_MAP, branch_type);
}


const struct oscap_string_map CVRF_RELATIONSHIP_TYPE_MAP[] = {
	{CVRF_RELATIONSHIP_DEFAULT_COMPONENT, "default component of"},
	{CVRF_RELATIONSHIP_OPTIONAL_COMPONENT, "optional component of"},
	{CVRF_RELATIONSHIP_EXTERNAL_COMPONENT, "external component of"},
	{CVRF_RELATIONSHIP_INSTALLED_ON, "installed on"},
	{CVRF_RELATIONSHIP_INSTALLED_WITH, "installed with"},
	{0, NULL}
};
cvrf_relationship_type_t cvrf_relationship_type_parse(xmlTextReaderPtr reader, char *attname)
{
	return cvrf_enumeration_attr(reader, attname, CVRF_RELATIONSHIP_TYPE_MAP);
}
const char *cvrf_relationship_type_get_text(cvrf_relationship_type_t relationship_type)
{
	return cvrf_enumeration_get_text(CVRF_RELATIONSHIP_TYPE_MAP, relationship_type);
}


const struct oscap_string_map CVRF_PRODUCT_STATUS_TYPE_MAP[] = {
	{CVRF_PRODUCT_STATUS_FIRST_AFFECTED, "First Affected"},
	{CVRF_PRODUCT_STATUS_KNOWN_AFFECTED, "Known Affected"},
	{CVRF_PRODUCT_STATUS_KNOWN_NOT_AFFECTED, "Known Not Affected"},
	{CVRF_PRODUCT_STATUS_FIXED, "First Fixed"},
	{CVRF_PRODUCT_STATUS_RECOMMENDED, "Recommended"},
	{CVRF_PRODUCT_STATUS_LAST_AFFECTED, "Last Affected"},
	{0, NULL}
};
cvrf_product_status_type_t cvrf_product_status_type_parse(xmlTextReaderPtr reader, char *attname)
{
	return cvrf_enumeration_attr(reader, attname, CVRF_PRODUCT_STATUS_TYPE_MAP);
}
const char *cvrf_product_status_type_get_text(cvrf_product_status_type_t product_status_type)
{
	return cvrf_enumeration_get_text(CVRF_PRODUCT_STATUS_TYPE_MAP, product_status_type);
}

const struct oscap_string_map CVRF_REMEDIATION_TYPE_MAP[] = {
	{CVRF_REMEDIATION_WORKAROUND, "Workaround"},
	{CVRF_REMEDIATION_MITIGATION, "Mitigation"},
	{CVRF_REMEDIATION_VENDOR_FIX, "Vendor Fix"},
	{CVRF_REMEDIATION_NONE_AVAILABLE, "None Available"},
	{CVRF_REMEDIATION_WILL_NOT_FIX, "Will Not Fix"},
	{0, NULL}
};
cvrf_remediation_type_t cvrf_remediation_type_parse(xmlTextReaderPtr reader, char *attname)
{
	return cvrf_enumeration_attr(reader, attname, CVRF_REMEDIATION_TYPE_MAP);
}
const char *cvrf_remediation_type_get_text(cvrf_remediation_type_t remediation_type)
{
	return cvrf_enumeration_get_text(CVRF_REMEDIATION_TYPE_MAP, remediation_type);
}


