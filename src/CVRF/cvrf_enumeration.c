/*
 * Copyright 2017 Red Hat Inc., Durham, North Carolina.
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
 */

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


/*****************************************************************************
 * Static functions
 */

static int cvrf_enumeration_attr(xmlTextReaderPtr reader, char *attname, const struct oscap_string_map *map) {
	int ret = 0;
	char *attrstr = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST attname);
	if (attrstr) {
		ret = oscap_string_to_enum(map, attrstr);
		oscap_free(attrstr);
	}
	return ret;
}

static int cvrf_enumeration_node_value(xmlTextReaderPtr reader, const struct oscap_string_map *map) {
	int ret = 0;
	char *valuestr = (char *)oscap_element_string_copy(reader);
	if (valuestr) {
		ret = oscap_string_to_enum(map, valuestr);
		oscap_free(valuestr);
	}
	return ret;
}

static const char *cvrf_enumeration_get_text(const struct oscap_string_map *map, int val) {
	return oscap_enum_to_string(map, val);
}
static int cvrf_enumeration_from_text(const struct oscap_string_map *map, const char *text) {
	return oscap_string_to_enum(map, text);
}


/*****************************************************************************
 * CVRF enum map definitions
 */

const struct oscap_string_map CVRF_DOC_PUBLISHER_TYPE_MAP[] = {
	{CVRF_DOC_PUBLISHER_VENDOR, "Vendor"},
	{CVRF_DOC_PUBLISHER_DISCOVERER, "Discoverer"},
	{CVRF_DOC_PUBLISHER_COORDINATOR, "Coordinator"},
	{CVRF_DOC_PUBLISHER_USER, "User"},
	{CVRF_DOC_PUBLISHER_OTHER, "Other"},
	{0, NULL}
};

cvrf_doc_publisher_type_t cvrf_doc_publisher_type_parse(xmlTextReaderPtr reader, char *attr_name) {
	return cvrf_enumeration_attr(reader, attr_name, CVRF_DOC_PUBLISHER_TYPE_MAP);
}
const char *cvrf_doc_publisher_type_get_text(cvrf_doc_publisher_type_t doc_publisher_type) {
	return cvrf_enumeration_get_text(CVRF_DOC_PUBLISHER_TYPE_MAP, doc_publisher_type);
}

const struct oscap_string_map CVRF_DOC_STATUS_TYPE_MAP[] = {
	{CVRF_DOC_STATUS_DRAFT, "Draft"},
	{CVRF_DOC_STATUS_INTERIM, "Interim"},
	{CVRF_DOC_STATUS_FINAL, "Final"},
	{0, NULL}
};

static cvrf_doc_status_type_t cvrf_doc_status_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_node_value(reader, CVRF_DOC_STATUS_TYPE_MAP);
}
const char *cvrf_doc_status_type_get_text(cvrf_doc_status_type_t doc_status_type) {
	return cvrf_enumeration_get_text(CVRF_DOC_STATUS_TYPE_MAP, doc_status_type);
}

const struct oscap_string_map CVRF_REFERENCE_TYPE_MAP[] = {
	{CVRF_REFERENCE_EXTERNAL, "External"},
	{CVRF_REFERENCE_SELF, "Self"},
	{0, NULL}
};

static cvrf_reference_type_t cvrf_reference_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Type", CVRF_REFERENCE_TYPE_MAP);
}
const char *cvrf_reference_type_get_text(cvrf_reference_type_t reference_type) {
	return cvrf_enumeration_get_text(CVRF_REFERENCE_TYPE_MAP, reference_type);
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
static cvrf_branch_type_t cvrf_branch_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Type", CVRF_BRANCH_TYPE_MAP);
}
const char *cvrf_branch_type_get_text(cvrf_branch_type_t branch_type) {
	return cvrf_enumeration_get_text(CVRF_BRANCH_TYPE_MAP, branch_type);
}


const struct oscap_string_map CVRF_RELATIONSHIP_TYPE_MAP[] = {
	{CVRF_RELATIONSHIP_DEFAULT_COMPONENT, "Default Component Of"},
	{CVRF_RELATIONSHIP_OPTIONAL_COMPONENT, "Optional Component Of"},
	{CVRF_RELATIONSHIP_EXTERNAL_COMPONENT, "External Component Of"},
	{CVRF_RELATIONSHIP_INSTALLED_ON, "Installed On"},
	{CVRF_RELATIONSHIP_INSTALLED_WITH, "Installed With"},
	{0, NULL}
};

static cvrf_relationship_type_t cvrf_relationship_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "RelationType", CVRF_RELATIONSHIP_TYPE_MAP);
}
const char *cvrf_relationship_type_get_text(cvrf_relationship_type_t relationship_type) {
	return cvrf_enumeration_get_text(CVRF_RELATIONSHIP_TYPE_MAP, relationship_type);
}

const struct oscap_string_map CVRF_INVOLVEMENT_STATUS_MAP[] = {
	{CVRF_INVOLVEMENT_OPEN, "Open"},
	{CVRF_INVOLVEMENT_DISPUTED, "Disputed"},
	{CVRF_INVOLVEMENT_IN_PROGRESS, "In Progress"},
	{CVRF_INVOLVEMENT_COMPLETED, "Completed"},
	{CVRF_INVOLVEMENT_CONTACT_ATTEMPTED, "Contact Attempted"},
	{CVRF_INVOLVEMENT_NOT_CONTACTED, "Not Contacted"},
	{0, NULL}
};

static cvrf_involvement_status_type_t cvrf_involvement_status_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Status", CVRF_INVOLVEMENT_STATUS_MAP);
}

const char *cvrf_involvement_status_type_get_text(cvrf_involvement_status_type_t involvement_type) {
	return cvrf_enumeration_get_text(CVRF_INVOLVEMENT_STATUS_MAP, involvement_type);
}

const struct oscap_string_map CVRF_PRODUCT_STATUS_TYPE_MAP[] = {
	{CVRF_PRODUCT_STATUS_FIRST_AFFECTED, "First Affected"},
	{CVRF_PRODUCT_STATUS_KNOWN_AFFECTED, "Known Affected"},
	{CVRF_PRODUCT_STATUS_KNOWN_NOT_AFFECTED, "Known Not Affected"},
	{CVRF_PRODUCT_STATUS_FIRST_FIXED, "First Fixed"},
	{CVRF_PRODUCT_STATUS_FIXED, "Fixed"},
	{CVRF_PRODUCT_STATUS_RECOMMENDED, "Recommended"},
	{CVRF_PRODUCT_STATUS_LAST_AFFECTED, "Last Affected"},
	{0, NULL}
};

static cvrf_product_status_type_t cvrf_product_status_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Type", CVRF_PRODUCT_STATUS_TYPE_MAP);
}

const char *cvrf_product_status_type_get_text(cvrf_product_status_type_t product_status_type) {
	return cvrf_enumeration_get_text(CVRF_PRODUCT_STATUS_TYPE_MAP, product_status_type);
}


const struct oscap_string_map CVRF_THREAT_TYPE_MAP[] = {
	{CVRF_THREAT_IMPACT, "Impact"},
	{CVRF_THREAT_EXPLOIT_STATUS, "Exploit Status"},
	{CVRF_THREAT_TARGET_SET, "Target Set"},
	{0, NULL}
};

static cvrf_threat_type_t cvrf_threat_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Type", CVRF_THREAT_TYPE_MAP);
}
const char *cvrf_threat_type_get_text(cvrf_threat_type_t threat_type) {
	return cvrf_enumeration_get_text(CVRF_THREAT_TYPE_MAP, threat_type);
}


const struct oscap_string_map CVRF_REMEDIATION_TYPE_MAP[] = {
	{CVRF_REMEDIATION_WORKAROUND, "Workaround"},
	{CVRF_REMEDIATION_MITIGATION, "Mitigation"},
	{CVRF_REMEDIATION_VENDOR_FIX, "Vendor Fix"},
	{CVRF_REMEDIATION_NONE_AVAILABLE, "None Available"},
	{CVRF_REMEDIATION_WILL_NOT_FIX, "Will Not Fix"},
	{0, NULL}
};

static cvrf_remediation_type_t cvrf_remediation_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Type", CVRF_REMEDIATION_TYPE_MAP);
}
const char *cvrf_remediation_type_get_text(cvrf_remediation_type_t remediation_type) {
	return cvrf_enumeration_get_text(CVRF_REMEDIATION_TYPE_MAP, remediation_type);
}

/*****************************************************************************
 * CVRF Item Type
 */

struct cvrf_item_spec {
	cvrf_item_type_t type;
	const char *tag_name;
	const char *container_name;
};

static const struct cvrf_item_spec CVRF_ITEM_TYPE_MAP[] = {
	{CVRF_DOCUMENT_PUBLISHER, "DocumentPublisher", NULL},
	{CVRF_DOCUMENT_TRACKING, "DocumentTracking", NULL},
	{CVRF_REVISION, "Revision", "RevisionHistory"},
	{CVRF_DOCUMENT_REFERENCE, "Reference", "DocumentReferences"},
	{CVRF_ACKNOWLEDGMENT, "Acknowledgment", "Acknowledgments"},
	{CVRF_PRODUCT_TREE, "ProductTree", NULL},
	{CVRF_BRANCH, "Branch", NULL},
	{CVRF_GROUP, "Group", "ProductGroups"},
	{CVRF_RELATIONSHIP, "Relationship", NULL},
	{CVRF_PRODUCT_NAME, "FullProductName", NULL},
	{CVRF_VULNERABILITY, "Vulnerability", NULL},
	{CVRF_INVOLVEMENT, "Involvement", "Involvements"},
	{CVRF_SCORE_SET, "ScoreSet", "CVSSScoreSet"},
	{CVRF_PRODUCT_STATUS, "Status", "ProductStatuses"},
	{CVRF_THREAT, "Threat", "Threats"},
	{CVRF_REMEDIATION, "Remediation", "Remediations"},
	{CVRF_REFERENCE, "Reference", "References"},
	{0, NULL, NULL}
};

static const struct cvrf_item_spec *cvrf_find_item_from_type(cvrf_item_type_t item_type) {
	for (const struct cvrf_item_spec *mapptr = CVRF_ITEM_TYPE_MAP; mapptr->tag_name != NULL; ++mapptr) {
		if (item_type == mapptr->type)
			return mapptr;
	}
	return NULL;
}

static const struct cvrf_item_spec *cvrf_find_item_from_text(const char *item) {
	for (const struct cvrf_item_spec *mapptr = CVRF_ITEM_TYPE_MAP; mapptr->tag_name != NULL; ++mapptr) {
		if (oscap_strcmp(item, mapptr->tag_name) || oscap_strcmp(item, mapptr->container_name))
			return mapptr;
	}
	return NULL;
}

const char *cvrf_item_type_get_text(cvrf_item_type_t type) {
	return cvrf_find_item_from_type(type) != NULL ? cvrf_find_item_from_type(type)->tag_name : NULL;
}

cvrf_item_type_t cvrf_item_type_from_text(const char *item) {
	return cvrf_find_item_from_text(item) != NULL ? cvrf_find_item_from_text(item)->type : 0;
}

bool cvrf_is_valid_item_type(const char *item_name) {
	return cvrf_item_type_from_text(item_name) != 0;
}

const char *cvrf_item_type_get_container(cvrf_item_type_t type) {
	return cvrf_find_item_from_type(type) != NULL ? cvrf_find_item_from_type(type)->container_name : NULL;
}

bool cvrf_item_type_has_container(cvrf_item_type_t item_type) {
	return cvrf_item_type_get_container(item_type) != NULL;
}

int cvrf_item_parse_type_attribute(xmlTextReaderPtr reader, cvrf_item_type_t item) {
	switch (item) {
		case CVRF_DOCUMENT_PUBLISHER:
			return cvrf_doc_publisher_type_parse(reader, "Type");
		case CVRF_DOCUMENT_TRACKING:
			return cvrf_doc_status_type_parse(reader);
		case CVRF_REFERENCE:
		case CVRF_DOCUMENT_REFERENCE:
			return cvrf_reference_type_parse(reader);
		case CVRF_BRANCH:
			return cvrf_branch_type_parse(reader);
		case CVRF_RELATIONSHIP:
			return cvrf_relationship_type_parse(reader);
		case CVRF_INVOLVEMENT:
			return cvrf_involvement_status_type_parse(reader);
		case CVRF_PRODUCT_STATUS:
			return cvrf_product_status_type_parse(reader);
		case CVRF_THREAT:
			return cvrf_threat_type_parse(reader);
		case CVRF_REMEDIATION:
			return cvrf_remediation_type_parse(reader);
		default:
			return 0;
	}
}

