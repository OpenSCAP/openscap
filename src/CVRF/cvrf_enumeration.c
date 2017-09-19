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
 * Authors:
 * 		Katarina Jankov <kj226@cornell.edu>
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
	char *attrstr = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST attname);
	const int ret = oscap_string_to_enum(map, attrstr);
	free(attrstr);
	return ret;
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
	{CVRF_DOC_PUBLISHER_UNKNOWN, NULL}
};

cvrf_doc_publisher_type_t cvrf_doc_publisher_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Type", CVRF_DOC_PUBLISHER_TYPE_MAP);
}

cvrf_doc_publisher_type_t cvrf_involvement_party_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Party", CVRF_DOC_PUBLISHER_TYPE_MAP);
}

const char *cvrf_doc_publisher_type_get_text(cvrf_doc_publisher_type_t doc_publisher_type) {
	return oscap_enum_to_string(CVRF_DOC_PUBLISHER_TYPE_MAP, doc_publisher_type);
}

const struct oscap_string_map CVRF_DOC_STATUS_TYPE_MAP[] = {
	{CVRF_DOC_STATUS_DRAFT, "Draft"},
	{CVRF_DOC_STATUS_INTERIM, "Interim"},
	{CVRF_DOC_STATUS_FINAL, "Final"},
	{CVRF_DOC_STATUS_UNKNOWN, NULL}
};

cvrf_doc_status_type_t cvrf_doc_status_type_parse(xmlTextReaderPtr reader) {
	const char *valuestr = oscap_element_string_get(reader);
	if (valuestr) {
		return oscap_string_to_enum(CVRF_DOC_STATUS_TYPE_MAP, valuestr);
	}
	return CVRF_DOC_PUBLISHER_UNKNOWN;
}
const char *cvrf_doc_status_type_get_text(cvrf_doc_status_type_t doc_status_type) {
	return oscap_enum_to_string(CVRF_DOC_STATUS_TYPE_MAP, doc_status_type);
}

const struct oscap_string_map CVRF_REFERENCE_TYPE_MAP[] = {
	{CVRF_REFERENCE_EXTERNAL, "External"},
	{CVRF_REFERENCE_SELF, "Self"},
	{CVRF_REFERENCE_UNKNOWN, NULL}
};

cvrf_reference_type_t cvrf_reference_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Type", CVRF_REFERENCE_TYPE_MAP);
}
const char *cvrf_reference_type_get_text(cvrf_reference_type_t reference_type) {
	return oscap_enum_to_string(CVRF_REFERENCE_TYPE_MAP, reference_type);
}

const struct oscap_string_map CVRF_NOTE_TYPE_MAP[] = {
	{CVRF_NOTE_GENERAL, "General"},
	{CVRF_NOTE_DETAILS, "Details"},
	{CVRF_NOTE_DESCRIPTION, "Description"},
	{CVRF_NOTE_SUMMARY, "Summary"},
	{CVRF_NOTE_FAQ, "FAQ"},
	{CVRF_NOTE_LEGAL_DISCLAIMER, "Legal Disclaimer"},
	{CVRF_NOTE_OTHER, "Other"},
	{CVRF_NOTE_UNKNOWN, NULL}
};

cvrf_note_type_t cvrf_note_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Type", CVRF_NOTE_TYPE_MAP);
}
const char *cvrf_note_type_get_text(cvrf_note_type_t note_type) {
	return oscap_enum_to_string(CVRF_NOTE_TYPE_MAP, note_type);
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
	{CVRF_BRANCH_UNKNOWN, NULL}
};
cvrf_branch_type_t cvrf_branch_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Type", CVRF_BRANCH_TYPE_MAP);
}
const char *cvrf_branch_type_get_text(cvrf_branch_type_t branch_type) {
	return oscap_enum_to_string(CVRF_BRANCH_TYPE_MAP, branch_type);
}


const struct oscap_string_map CVRF_RELATIONSHIP_TYPE_MAP[] = {
	{CVRF_RELATIONSHIP_DEFAULT_COMPONENT, "Default Component Of"},
	{CVRF_RELATIONSHIP_OPTIONAL_COMPONENT, "Optional Component Of"},
	{CVRF_RELATIONSHIP_EXTERNAL_COMPONENT, "External Component Of"},
	{CVRF_RELATIONSHIP_INSTALLED_ON, "Installed On"},
	{CVRF_RELATIONSHIP_INSTALLED_WITH, "Installed With"},
	{CVRF_RELATIONSHIP_UNKNOWN, NULL}
};

cvrf_relationship_type_t cvrf_relationship_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "RelationType", CVRF_RELATIONSHIP_TYPE_MAP);
}
const char *cvrf_relationship_type_get_text(cvrf_relationship_type_t relationship_type) {
	return oscap_enum_to_string(CVRF_RELATIONSHIP_TYPE_MAP, relationship_type);
}

const struct oscap_string_map CVRF_INVOLVEMENT_STATUS_MAP[] = {
	{CVRF_INVOLVEMENT_OPEN, "Open"},
	{CVRF_INVOLVEMENT_DISPUTED, "Disputed"},
	{CVRF_INVOLVEMENT_IN_PROGRESS, "In Progress"},
	{CVRF_INVOLVEMENT_COMPLETED, "Completed"},
	{CVRF_INVOLVEMENT_CONTACT_ATTEMPTED, "Contact Attempted"},
	{CVRF_INVOLVEMENT_NOT_CONTACTED, "Not Contacted"},
	{CVRF_INVOLVEMENT_UNKNOWN, NULL}
};

cvrf_involvement_status_type_t cvrf_involvement_status_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Status", CVRF_INVOLVEMENT_STATUS_MAP);
}

const char *cvrf_involvement_status_type_get_text(cvrf_involvement_status_type_t involvement_type) {
	return oscap_enum_to_string(CVRF_INVOLVEMENT_STATUS_MAP, involvement_type);
}

const struct oscap_string_map CVRF_PRODUCT_STATUS_TYPE_MAP[] = {
	{CVRF_PRODUCT_STATUS_FIRST_AFFECTED, "First Affected"},
	{CVRF_PRODUCT_STATUS_KNOWN_AFFECTED, "Known Affected"},
	{CVRF_PRODUCT_STATUS_KNOWN_NOT_AFFECTED, "Known Not Affected"},
	{CVRF_PRODUCT_STATUS_FIRST_FIXED, "First Fixed"},
	{CVRF_PRODUCT_STATUS_FIXED, "Fixed"},
	{CVRF_PRODUCT_STATUS_RECOMMENDED, "Recommended"},
	{CVRF_PRODUCT_STATUS_LAST_AFFECTED, "Last Affected"},
	{CVRF_PRODUCT_STATUS_UNKNOWN, NULL}
};

cvrf_product_status_type_t cvrf_product_status_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Type", CVRF_PRODUCT_STATUS_TYPE_MAP);
}

const char *cvrf_product_status_type_get_text(cvrf_product_status_type_t product_status_type) {
	return oscap_enum_to_string(CVRF_PRODUCT_STATUS_TYPE_MAP, product_status_type);
}

const struct oscap_string_map CVRF_THREAT_TYPE_MAP[] = {
	{CVRF_THREAT_IMPACT, "Impact"},
	{CVRF_THREAT_EXPLOIT_STATUS, "Exploit Status"},
	{CVRF_THREAT_TARGET_SET, "Target Set"},
	{CVRF_THREAT_UNKNOWN, NULL}
};

cvrf_threat_type_t cvrf_threat_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Type", CVRF_THREAT_TYPE_MAP);
}
const char *cvrf_threat_type_get_text(cvrf_threat_type_t threat_type) {
	return oscap_enum_to_string(CVRF_THREAT_TYPE_MAP, threat_type);
}


const struct oscap_string_map CVRF_REMEDIATION_TYPE_MAP[] = {
	{CVRF_REMEDIATION_WORKAROUND, "Workaround"},
	{CVRF_REMEDIATION_MITIGATION, "Mitigation"},
	{CVRF_REMEDIATION_VENDOR_FIX, "Vendor Fix"},
	{CVRF_REMEDIATION_NONE_AVAILABLE, "None Available"},
	{CVRF_REMEDIATION_WILL_NOT_FIX, "Will Not Fix"},
	{CVRF_REMEDIATION_UNKNOWN, NULL}
};

cvrf_remediation_type_t cvrf_remediation_type_parse(xmlTextReaderPtr reader) {
	return cvrf_enumeration_attr(reader, "Type", CVRF_REMEDIATION_TYPE_MAP);
}
const char *cvrf_remediation_type_get_text(cvrf_remediation_type_t remediation_type) {
	return oscap_enum_to_string(CVRF_REMEDIATION_TYPE_MAP, remediation_type);
}

/*****************************************************************************
 * CVRF Item Type
 */

struct cvrf_item_spec {
	const cvrf_item_type_t type;
	const char *tag_name;
	const char *container_name;
};

static const struct cvrf_item_spec CVRF_ITEM_TYPE_MAP[] = {
	{CVRF_DOCUMENT_PUBLISHER, "DocumentPublisher", NULL},
	{CVRF_DOCUMENT_TRACKING, "DocumentTracking", NULL},
	{CVRF_REVISION, "Revision", "RevisionHistory"},
	{CVRF_DOCUMENT_NOTE, "Note", "DocumentNotes"},
	{CVRF_DOCUMENT_REFERENCE, "Reference", "DocumentReferences"},
	{CVRF_ACKNOWLEDGMENT, "Acknowledgment", "Acknowledgments"},
	{CVRF_PRODUCT_TREE, "ProductTree", NULL},
	{CVRF_BRANCH, "Branch", NULL},
	{CVRF_GROUP, "Group", "ProductGroups"},
	{CVRF_RELATIONSHIP, "Relationship", NULL},
	{CVRF_PRODUCT_NAME, "FullProductName", NULL},
	{CVRF_VULNERABILITY, "Vulnerability", NULL},
	{CVRF_VULNERABILITY_CWE, "CWE", NULL},
	{CVRF_NOTE, "Note", "Notes"},
	{CVRF_INVOLVEMENT, "Involvement", "Involvements"},
	{CVRF_SCORE_SET, "ScoreSet", "CVSSScoreSet"},
	{CVRF_PRODUCT_STATUS, "Status", "ProductStatuses"},
	{CVRF_THREAT, "Threat", "Threats"},
	{CVRF_REMEDIATION, "Remediation", "Remediations"},
	{CVRF_REFERENCE, "Reference", "References"},
	{CVRF_ITEM_UNKNOWN, NULL, NULL}
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
		if (oscap_strcmp(item, mapptr->tag_name) == 0 || oscap_strcmp(item, mapptr->container_name) == 0)
			return mapptr;
	}
	return NULL;
}

const char *cvrf_item_type_get_text(cvrf_item_type_t type) {
	const struct cvrf_item_spec *spec = cvrf_find_item_from_type(type);
	return spec != NULL ? spec->tag_name : NULL;
}

cvrf_item_type_t cvrf_item_type_from_text(const char *item) {
	const struct cvrf_item_spec *spec = cvrf_find_item_from_text(item);
	return spec != NULL ? spec->type : CVRF_ITEM_UNKNOWN;
}

bool cvrf_is_valid_item_type(const char *item_name) {
	return cvrf_item_type_from_text(item_name) != CVRF_ITEM_UNKNOWN;
}

const char *cvrf_item_type_get_container(cvrf_item_type_t type) {
	const struct cvrf_item_spec *spec = cvrf_find_item_from_type(type);
	return spec != NULL ? spec->container_name : NULL;
}

bool cvrf_item_type_has_container(cvrf_item_type_t item_type) {
	return cvrf_item_type_get_container(item_type) != NULL;
}

