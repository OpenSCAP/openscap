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
#include <math.h>

#include <libxml/xmlreader.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>
#include <libxml/xpathInternals.h>

#include "public/cvrf.h"
#include "cvrf_priv.h"

#include "common/list.h"
#include "common/_error.h"
#include "common/xmltext_priv.h"
#include "common/elements.h"
#include "common/oscap_string.h"
#include "common/util.h"

#include "CPE/cpelang_priv.h"
#include "CVSS/cvss_priv.h"
#include "CVSS/public/cvss_score.h"

#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"


/***************************************************************************
 ***************************************************************************
 * Vulnerability offshoot of main CVRF model
 *
 */

/***************************************************************************
 * CVRF Remediation
 */
struct cvrf_remediation {
	cvrf_remediation_type_t type;
	char *date;
	char *description;
	char *url;
	char *entitlement;
	struct oscap_stringlist *product_ids;
	struct oscap_stringlist *group_ids;
};
OSCAP_ACCESSOR_STRING(cvrf_remediation, date)
OSCAP_ACCESSOR_STRING(cvrf_remediation, description)
OSCAP_ACCESSOR_STRING(cvrf_remediation, url)
OSCAP_ACCESSOR_STRING(cvrf_remediation, entitlement)

cvrf_remediation_type_t cvrf_remediation_get_type(struct cvrf_remediation *remed) {
	return remed->type;
}

struct oscap_string_iterator *cvrf_remediation_get_product_ids(struct cvrf_remediation *remed) {
	return oscap_stringlist_get_strings(remed->product_ids);
}
struct oscap_string_iterator *cvrf_remediation_get_group_ids(struct cvrf_remediation *remed) {
	return oscap_stringlist_get_strings(remed->group_ids);
}

struct cvrf_remediation *cvrf_remediation_new() {
	struct cvrf_remediation *ret =  oscap_alloc(sizeof(struct cvrf_remediation));
	if (ret == NULL)
		return NULL;

	ret->type = CVRF_ATTRIBUTE_UNKNOWN;
	ret->date = NULL;
	ret->description = NULL;
	ret->url = NULL;
	ret->entitlement = NULL;
	ret->product_ids = oscap_stringlist_new();
	ret->group_ids = oscap_stringlist_new();
	return ret;
}

void cvrf_remediation_free(struct cvrf_remediation *remed) {
	if (remed == NULL)
		return;

	oscap_free(remed->date);
	oscap_free(remed->description);
	oscap_free(remed->url);
	oscap_free(remed->entitlement);
	oscap_stringlist_free(remed->product_ids);
	oscap_stringlist_free(remed->group_ids);
	oscap_free(remed);
}

struct cvrf_remediation *cvrf_remediation_clone(const struct cvrf_remediation *remed) {
	struct cvrf_remediation *clone = oscap_alloc(sizeof(struct cvrf_remediation));
	clone->type = remed->type;
	clone->date = oscap_strdup(remed->date);
	clone->description = oscap_strdup(remed->description);
	clone->url = oscap_strdup(remed->url);
	clone->entitlement = oscap_strdup(remed->entitlement);
	clone->product_ids = oscap_stringlist_clone(remed->product_ids);
	clone->group_ids = oscap_stringlist_clone(remed->group_ids);
	return clone;
}

/***************************************************************************
 * CVRF Score Set
 */

struct cvrf_score_set {
	char *vector;
	struct cvss_impact *impact;
	struct oscap_stringlist *product_ids;
};
OSCAP_ACCESSOR_STRING(cvrf_score_set, vector)
OSCAP_ACCESSOR_SIMPLE(struct cvss_impact*, cvrf_score_set, impact)

struct oscap_string_iterator *cvrf_score_set_get_product_ids(struct cvrf_score_set *score_set) {
	return oscap_stringlist_get_strings(score_set->product_ids);
}

bool cvrf_score_set_add_metric(struct cvrf_score_set *score_set, enum cvss_category category, const char *score) {
	struct cvss_metrics *metric = cvss_metrics_new(category);
	cvss_metrics_set_score(metric, oscap_strtol(score, NULL, 10));
	return cvss_impact_set_metrics(score_set->impact, metric);
}

static char *cvrf_score_set_get_score(const struct cvrf_score_set *score_set, enum cvss_category category) {
	struct cvss_metrics *metric = NULL;
	if (category == CVSS_BASE) {
		metric = cvss_impact_get_base_metrics(score_set->impact);
	} else if (category == CVSS_ENVIRONMENTAL) {
		metric = cvss_impact_get_environmental_metrics(score_set->impact);
	} else if (category == CVSS_TEMPORAL) {
		metric = cvss_impact_get_temporal_metrics(score_set->impact);
	}
	if (metric == NULL)
		return NULL;

	float score = cvss_metrics_get_score(metric);
	if (isnan(score) != 0) {
		return NULL;
	} else {
		return oscap_sprintf("%f", score);
	}
}

char *cvrf_score_set_get_base_score(const struct cvrf_score_set *score_set) {
	return cvrf_score_set_get_score(score_set, CVSS_BASE);
}

char *cvrf_score_set_get_environmental_score(const struct cvrf_score_set *score_set) {
	return cvrf_score_set_get_score(score_set, CVSS_ENVIRONMENTAL);
}

char *cvrf_score_set_get_temporal_score(const struct cvrf_score_set *score_set) {
	return cvrf_score_set_get_score(score_set, CVSS_TEMPORAL);
}

struct cvrf_score_set *cvrf_score_set_new() {
	struct cvrf_score_set *ret = oscap_alloc(sizeof(struct cvrf_score_set));
	if (ret == NULL)
		return NULL;

	ret->vector = NULL;
	ret->impact = cvss_impact_new();
	ret->product_ids = oscap_stringlist_new();
	return ret;
}

void cvrf_score_set_free(struct cvrf_score_set *score_set) {
	if (score_set == NULL)
		return;

	oscap_free(score_set->vector);
	cvss_impact_free(score_set->impact);
	oscap_stringlist_free(score_set->product_ids);
	oscap_free(score_set);
}

struct cvrf_score_set *cvrf_score_set_clone(const struct cvrf_score_set *score_set) {
	struct cvrf_score_set *clone = oscap_alloc(sizeof(struct cvrf_score_set));
	clone->vector = oscap_strdup(score_set->vector);
	clone->impact= cvss_impact_clone(score_set->impact);
	clone->product_ids = oscap_stringlist_clone(score_set->product_ids);
	return clone;
}


/***************************************************************************
 * CVRF threat
 */
struct cvrf_threat {
	cvrf_threat_type_t type;
	char *date;
	char *description;
	struct oscap_stringlist *product_ids;
	struct oscap_stringlist *group_ids;
};
OSCAP_ACCESSOR_STRING(cvrf_threat, date)
OSCAP_ACCESSOR_STRING(cvrf_threat, description)

cvrf_threat_type_t cvrf_threat_get_threat_type(struct cvrf_threat *threat) {
	return threat->type;
}
struct oscap_string_iterator *cvrf_threat_get_product_ids(struct cvrf_threat *threat) {
	return oscap_stringlist_get_strings(threat->product_ids);
}
struct oscap_string_iterator *cvrf_threat_get_group_ids(struct cvrf_threat *threat) {
	return oscap_stringlist_get_strings(threat->group_ids);
}

struct cvrf_threat *cvrf_threat_new() {
	struct cvrf_threat *ret = oscap_alloc(sizeof(struct cvrf_threat));
	if (ret == NULL)
		return NULL;

	ret->type = CVRF_ATTRIBUTE_UNKNOWN;
	ret->date = NULL;
	ret->description = NULL;
	ret->product_ids = oscap_stringlist_new();
	ret->group_ids = oscap_stringlist_new();

	return ret;
}

void cvrf_threat_free(struct cvrf_threat *threat) {
	if (threat == NULL)
		return;

	oscap_free(threat->date);
	oscap_free(threat->description);
	oscap_stringlist_free(threat->product_ids);
	oscap_stringlist_free(threat->group_ids);
	oscap_free(threat);
}

struct cvrf_threat *cvrf_threat_clone(const struct cvrf_threat *threat) {
	struct cvrf_threat *clone = oscap_alloc(sizeof(struct cvrf_threat));
	clone->type = threat->type;
	clone->date = oscap_strdup(threat->date);
	clone->description = oscap_strdup(threat->description);
	clone->product_ids = oscap_stringlist_clone(threat->product_ids);
	clone->group_ids = oscap_stringlist_clone(threat->group_ids);
	return clone;
}

/***************************************************************************
 * CVRF ProductStatus
 */

struct cvrf_product_status {
	cvrf_product_status_type_t type;
	struct oscap_stringlist *product_ids;
};

struct oscap_string_iterator *cvrf_product_status_get_ids(struct cvrf_product_status *stat) {
	return oscap_stringlist_get_strings(stat->product_ids);
}
cvrf_product_status_type_t cvrf_product_status_get_type(struct cvrf_product_status *stat) {
	return stat->type;
}

struct cvrf_product_status *cvrf_product_status_new() {
	struct cvrf_product_status *ret = oscap_alloc(sizeof(struct cvrf_product_status));
	if (ret == NULL)
		return NULL;

	ret->type = CVRF_ATTRIBUTE_UNKNOWN;
	ret->product_ids = oscap_stringlist_new();
	return ret;
}

void cvrf_product_status_free(struct cvrf_product_status *status) {
	if (status == NULL)
		return;

	oscap_stringlist_free(status->product_ids);
	oscap_free(status);
}

struct cvrf_product_status *cvrf_product_status_clone(const struct cvrf_product_status *stat) {
	struct cvrf_product_status *clone = oscap_alloc(sizeof(struct cvrf_product_status));
	clone->type = stat->type;
	clone->product_ids = oscap_stringlist_clone(stat->product_ids);
	return clone;
}

/***************************************************************************
 * CVRF Involvement
 */

struct cvrf_involvement {
	cvrf_involvement_status_type_t status;
	cvrf_doc_publisher_type_t party;
	char *description;
};
OSCAP_ACCESSOR_STRING(cvrf_involvement, description)

cvrf_involvement_status_type_t cvrf_involvement_get_status_type(struct cvrf_involvement *involve) {
	return involve->status;
}

struct cvrf_involvement *cvrf_involvement_new() {
	struct cvrf_involvement *ret = oscap_alloc(sizeof(struct cvrf_involvement));
	if (ret == NULL)
		return NULL;

	ret->status = CVRF_ATTRIBUTE_UNKNOWN;
	ret->party = CVRF_ATTRIBUTE_UNKNOWN;
	ret->description = NULL;
	return ret;
}

void cvrf_involvement_free(struct cvrf_involvement *involve) {
	if (involve == NULL)
		return;

	oscap_free(involve->description);
	oscap_free(involve);
}

struct cvrf_involvement *cvrf_involvement_clone(const struct cvrf_involvement *involve) {
	struct cvrf_involvement *clone = oscap_alloc(sizeof(struct cvrf_involvement));
	clone->status = involve->status;
	clone->party = involve->party;
	clone->description = oscap_strdup(involve->description);
	return clone;
}

/***************************************************************************
 * CVRF Vulnerability
 */

struct cvrf_vulnerability {
	int ordinal;
	char *title;
	char *system_id;
	char *system_name;
	char *discovery_date;
	char *release_date;
	char *cve_id;
	struct oscap_stringlist *cwe_ids;

	struct oscap_list *notes;
	struct oscap_list *involvements;
	struct oscap_list *score_sets;
	struct oscap_list *product_statuses;
	struct oscap_list *threats;
	struct oscap_list *remediations;
	struct oscap_list *references;
	struct oscap_list *acknowledgments;

};
OSCAP_ACCESSOR_SIMPLE(int, cvrf_vulnerability, ordinal)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, title)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, system_id)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, system_name)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, discovery_date)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, release_date)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, cve_id)
OSCAP_IGETINS_GEN(cvrf_involvement, cvrf_vulnerability, involvements, involvement)
OSCAP_ITERATOR_REMOVE_F(cvrf_involvement)
OSCAP_IGETINS_GEN(cvrf_score_set, cvrf_vulnerability, score_sets, score_set)
OSCAP_ITERATOR_REMOVE_F(cvrf_score_set)
OSCAP_IGETINS_GEN(cvrf_product_status, cvrf_vulnerability, product_statuses, cvrf_product_status)
OSCAP_ITERATOR_REMOVE_F(cvrf_product_status)
OSCAP_IGETINS_GEN(cvrf_remediation, cvrf_vulnerability, remediations, remediation)
OSCAP_ITERATOR_REMOVE_F(cvrf_remediation)
OSCAP_IGETINS_GEN(cvrf_threat, cvrf_vulnerability, threats, threat)
OSCAP_ITERATOR_REMOVE_F(cvrf_threat)

struct oscap_string_iterator *cvrf_vulnerability_get_cwe_ids(struct cvrf_vulnerability *vuln) {
	return oscap_stringlist_get_strings(vuln->cwe_ids);
}

struct oscap_iterator *cvrf_vulnerability_get_references(struct cvrf_vulnerability *vuln) {
	return oscap_iterator_new(vuln->references);
}

struct oscap_iterator *cvrf_vulnerability_get_acknowledgments(struct cvrf_vulnerability *vuln) {
	return oscap_iterator_new(vuln->acknowledgments);
}

struct oscap_iterator *cvrf_vulnerability_get_notes(struct cvrf_vulnerability *vuln) {
	return oscap_iterator_new(vuln->notes);
}


struct cvrf_vulnerability *cvrf_vulnerability_new() {
	struct cvrf_vulnerability *ret = oscap_alloc(sizeof(struct cvrf_vulnerability));
	if (ret == NULL)
		return NULL;

	ret->ordinal = 0;
	ret->title = NULL;
	ret->system_id = NULL;
	ret->system_name = NULL;
	ret->discovery_date = NULL;
	ret->release_date = NULL;
	ret->cve_id = NULL;
	ret->cwe_ids = oscap_stringlist_new();
	ret->notes = oscap_list_new();
	ret->involvements = oscap_list_new();
	ret->score_sets = oscap_list_new();
	ret->product_statuses = oscap_list_new();
	ret->threats = oscap_list_new();
	ret->remediations = oscap_list_new();
	ret->references = oscap_list_new();
	ret->acknowledgments = oscap_list_new();
	return ret;
}

void cvrf_vulnerability_free(struct cvrf_vulnerability *vulnerability) {
	if (vulnerability == NULL)
		return;

	oscap_free(vulnerability->title);
	oscap_free(vulnerability->system_id);
	oscap_free(vulnerability->system_name);
	oscap_free(vulnerability->discovery_date);
	oscap_free(vulnerability->release_date);
	oscap_free(vulnerability->cve_id);
	oscap_stringlist_free(vulnerability->cwe_ids);
	oscap_list_free(vulnerability->notes, (oscap_destruct_func) cvrf_note_free);
	oscap_list_free(vulnerability->involvements, (oscap_destruct_func) cvrf_involvement_free);
	oscap_list_free(vulnerability->score_sets, (oscap_destruct_func) cvrf_score_set_free);
	oscap_list_free(vulnerability->product_statuses, (oscap_destruct_func) cvrf_product_status_free);
	oscap_list_free(vulnerability->threats, (oscap_destruct_func) cvrf_threat_free);
	oscap_list_free(vulnerability->remediations, (oscap_destruct_func) cvrf_remediation_free);
	oscap_list_free(vulnerability->references, (oscap_destruct_func) cvrf_reference_free);
	oscap_list_free(vulnerability->acknowledgments, (oscap_destruct_func) cvrf_acknowledgment_free);
	oscap_free(vulnerability);
}

struct cvrf_vulnerability *cvrf_vulnerability_clone(const struct cvrf_vulnerability *vuln) {
	struct cvrf_vulnerability *clone = oscap_alloc(sizeof(struct cvrf_vulnerability));
	clone->ordinal = vuln->ordinal;
	clone->title = oscap_strdup(vuln->title);
	clone->system_id = oscap_strdup(vuln->system_id);
	clone->system_id = oscap_strdup(vuln->system_name);
	clone->discovery_date = oscap_strdup(vuln->discovery_date);
	clone->release_date = oscap_strdup(vuln->release_date);
	clone->notes = oscap_list_clone(vuln->notes, (oscap_clone_func) cvrf_note_clone);
	clone->involvements = oscap_list_clone(vuln->involvements, (oscap_clone_func) cvrf_involvement_clone);
	clone->product_statuses = oscap_list_clone(vuln->product_statuses, (oscap_clone_func) cvrf_product_status_clone);
	clone->threats = oscap_list_clone(vuln->threats, (oscap_clone_func) cvrf_threat_clone);
	clone->score_sets = oscap_list_clone(vuln->score_sets, (oscap_clone_func) cvrf_score_set_clone);
	clone->remediations = oscap_list_clone(vuln->remediations, (oscap_clone_func) cvrf_remediation_clone);
	clone->references = oscap_list_clone(vuln->references, (oscap_clone_func) cvrf_reference_clone);
	clone->acknowledgments = oscap_list_clone(vuln->acknowledgments, (oscap_clone_func) cvrf_acknowledgment_clone);
	return clone;
}

void cvrf_vulnerability_filter_by_product(struct cvrf_vulnerability *vuln, const char *prod) {
	struct oscap_stringlist *filtered_ids = oscap_stringlist_new();

	struct cvrf_product_status_iterator *statuses = cvrf_vulnerability_get_product_statuses(vuln);
	while (cvrf_product_status_iterator_has_more(statuses)) {
		struct cvrf_product_status *stat = cvrf_product_status_iterator_next(statuses);

		struct oscap_string_iterator *products = cvrf_product_status_get_ids(stat);
		while (oscap_string_iterator_has_more(products)) {
			const char *product_id = oscap_string_iterator_next(products);
			if (oscap_str_startswith(product_id, prod))
				oscap_stringlist_add_string(filtered_ids, product_id);
		}
		oscap_string_iterator_free(products);
		oscap_stringlist_free(stat->product_ids);
		stat->product_ids = filtered_ids;
	}
	cvrf_product_status_iterator_free(statuses);
}



/***************************************************************************
****************************************************************************
 * Product tree offshoot of main CVRF model
 */

/***************************************************************************
 * CVRF FullProductName
 */
struct cvrf_product_name {
	char *product_id;
	char *cpe;
};
OSCAP_ACCESSOR_STRING(cvrf_product_name, product_id)
OSCAP_ACCESSOR_STRING(cvrf_product_name, cpe)

struct cvrf_product_name *cvrf_product_name_new() {
	struct cvrf_product_name *ret = oscap_alloc(sizeof(struct cvrf_product_name));
	if (ret == NULL)
		return NULL;

	ret->product_id = NULL;
	ret->cpe = NULL;
	return ret;
}

void cvrf_product_name_free(struct cvrf_product_name *full_name) {
	if (full_name == NULL)
		return;

	oscap_free(full_name->product_id);
	oscap_free(full_name->cpe);
	oscap_free(full_name);
}

struct cvrf_product_name *cvrf_product_name_clone(const struct cvrf_product_name *full_name) {
	struct cvrf_product_name *clone = oscap_alloc(sizeof(struct cvrf_product_name));
	clone->product_id = oscap_strdup(full_name->product_id);
	clone->cpe = oscap_strdup(full_name->cpe);
	return clone;
}

/***************************************************************************
 * CVRF ProductGroup
 */
struct cvrf_group {
	char *group_id;
	char *description;
	struct oscap_stringlist *product_ids;
};
OSCAP_ACCESSOR_STRING(cvrf_group, group_id)
OSCAP_ACCESSOR_STRING(cvrf_group, description)

struct oscap_string_iterator *cvrf_group_get_product_ids(struct cvrf_group *group) {
	return oscap_stringlist_get_strings(group->product_ids);
}

struct cvrf_group *cvrf_group_new() {
	struct cvrf_group *ret = oscap_alloc(sizeof(struct cvrf_group));
	if (ret == NULL)
		return NULL;

	ret->group_id = NULL;
	ret->description = NULL;
	ret->product_ids = oscap_stringlist_new();
	return ret;
}

void cvrf_group_free(struct cvrf_group *group) {
	if (group == NULL)
		return;

	oscap_free(group->group_id);
	oscap_free(group->description);
	oscap_stringlist_free(group->product_ids);
	oscap_free(group);
}

struct cvrf_group *cvrf_group_clone(const struct cvrf_group *group) {
	struct cvrf_group *clone = oscap_alloc(sizeof(struct cvrf_group));
	clone->group_id = oscap_strdup(group->group_id);
	clone->description = oscap_strdup(group->description);
	clone->product_ids = oscap_stringlist_clone(group->product_ids);
	return clone;
}

/***************************************************************************
 * CVRF Relationship
 */
struct cvrf_relationship {
	char *product_reference;
	cvrf_relationship_type_t relation_type;
	char *relates_to_ref;
	struct cvrf_product_name *product_name;
};
OSCAP_ACCESSOR_STRING(cvrf_relationship, product_reference)
OSCAP_ACCESSOR_STRING(cvrf_relationship, relates_to_ref)
OSCAP_ACCESSOR_SIMPLE(struct cvrf_product_name*, cvrf_relationship, product_name)

cvrf_relationship_type_t cvrf_relationship_get_relation_type(struct cvrf_relationship *relation) {
	return relation->relation_type;
}

struct cvrf_relationship *cvrf_relationship_new() {
	struct cvrf_relationship *ret = oscap_alloc(sizeof(struct cvrf_relationship));
	if (ret == NULL)
		return NULL;

	ret->product_reference = NULL;
	ret->relation_type = CVRF_ATTRIBUTE_UNKNOWN;
	ret->relates_to_ref = NULL;
	ret->product_name = cvrf_product_name_new();

	return ret;
}

void cvrf_relationship_free(struct cvrf_relationship *relationship) {
	if (relationship == NULL)
		return;

	oscap_free(relationship->product_reference);
	oscap_free(relationship->relates_to_ref);
	cvrf_product_name_free(relationship->product_name);
	oscap_free(relationship);
}

struct cvrf_relationship *cvrf_relationship_clone(const struct cvrf_relationship *relation) {
	struct cvrf_relationship *clone = oscap_alloc(sizeof(struct cvrf_relationship));
	clone->relation_type = relation->relation_type;
	clone->product_reference = oscap_strdup(relation->product_reference);
	clone->relates_to_ref = oscap_strdup(relation->relates_to_ref);
	clone->product_name = cvrf_product_name_clone(relation->product_name);
	return clone;
}

/***************************************************************************
 * CVRF Branch
 */
struct cvrf_branch {
	cvrf_branch_type_t type;
	char *branch_name;
	struct cvrf_product_name *product_name;
	struct oscap_list *subbranches;
};
OSCAP_ACCESSOR_STRING(cvrf_branch, branch_name)
OSCAP_ACCESSOR_SIMPLE(struct cvrf_product_name*, cvrf_branch, product_name)

struct oscap_iterator *cvrf_branch_get_subbranches(struct cvrf_branch *branch) {
	return oscap_iterator_new(branch->subbranches);
}

cvrf_branch_type_t cvrf_branch_get_branch_type(struct cvrf_branch *branch) {
	return branch->type;
}

struct cvrf_branch *cvrf_branch_new() {
	struct cvrf_branch *ret = oscap_alloc(sizeof(struct cvrf_branch));
	if (ret == NULL)
		return NULL;

	ret->type = CVRF_ATTRIBUTE_UNKNOWN;
	ret->branch_name = NULL;
	ret->product_name = cvrf_product_name_new();
	ret->subbranches = oscap_list_new();
	return ret;
}

void cvrf_branch_free(struct cvrf_branch *branch) {
	if (branch == NULL)
		return;

	oscap_free(branch->branch_name);
	cvrf_product_name_free(branch->product_name);
	oscap_list_free(branch->subbranches, (oscap_destruct_func) cvrf_branch_free);
	oscap_free(branch);
}

struct cvrf_branch *cvrf_branch_clone(const struct cvrf_branch *branch) {
	struct cvrf_branch *clone = oscap_alloc(sizeof(struct cvrf_branch));
	clone->branch_name = oscap_strdup(branch->branch_name);
	clone->type = branch->type;
	clone->product_name = cvrf_product_name_clone(branch->product_name);
	clone->subbranches = oscap_list_clone(branch->subbranches, (oscap_clone_func) cvrf_branch_clone);
	return clone;
}

static const char *get_cvrf_product_id_from_branch(struct cvrf_branch *branch, const char *cpe) {
	const char *product_id = NULL;
	if (cvrf_branch_get_branch_type(branch) == CVRF_BRANCH_PRODUCT_FAMILY) {
		struct oscap_iterator *subbranches = cvrf_branch_get_subbranches(branch);
		while(oscap_iterator_has_more(subbranches) && product_id == NULL) {
			product_id = get_cvrf_product_id_from_branch(oscap_iterator_next(subbranches), cpe);
		}
		oscap_iterator_free(subbranches);
	}
	else {
		if (!strcmp(cvrf_branch_get_branch_name(branch), cpe))
			return cvrf_product_name_get_product_id(branch->product_name);
	}
	return product_id;
}

/***************************************************************************
 * CVRF ProductTree
 */

struct cvrf_product_tree {
	struct oscap_list *product_names;
	struct oscap_list *branches;
	struct oscap_list *relationships;
	struct oscap_list *product_groups;
};
OSCAP_IGETINS_GEN(cvrf_product_name, cvrf_product_tree, product_names, product_name)
OSCAP_ITERATOR_REMOVE_F(cvrf_product_name)
OSCAP_IGETINS_GEN(cvrf_relationship, cvrf_product_tree, relationships, relationship)
OSCAP_ITERATOR_REMOVE_F(cvrf_relationship)
OSCAP_IGETINS_GEN(cvrf_group, cvrf_product_tree, product_groups, group)
OSCAP_ITERATOR_REMOVE_F(cvrf_group)

struct oscap_iterator *cvrf_product_tree_get_branches(struct cvrf_product_tree *tree) {
	return oscap_iterator_new(tree->branches);
}

struct cvrf_product_tree *cvrf_product_tree_new() {
	struct cvrf_product_tree *ret = oscap_alloc(sizeof(struct cvrf_product_tree));
	if (ret == NULL)
		return NULL;

	ret->product_names = oscap_list_new();
	ret->branches = oscap_list_new();
	ret->relationships = oscap_list_new();
	ret->product_groups = oscap_list_new();
	return ret;
}

void cvrf_product_tree_free(struct cvrf_product_tree *tree) {
	if (tree == NULL)
		return;

	oscap_list_free(tree->product_names, (oscap_destruct_func) cvrf_product_name_free);
	oscap_list_free(tree->branches, (oscap_destruct_func) cvrf_branch_free);
	oscap_list_free(tree->relationships, (oscap_destruct_func) cvrf_relationship_free);
	oscap_list_free(tree->product_groups, (oscap_destruct_func) cvrf_group_free);
	oscap_free(tree);
}

struct cvrf_product_tree *cvrf_product_tree_clone(const struct cvrf_product_tree *tree) {
	struct cvrf_product_tree *clone = oscap_alloc(sizeof(struct cvrf_product_tree));
	clone->product_names = oscap_list_clone(tree->product_names, (oscap_clone_func) cvrf_product_name_clone);
	clone->branches = oscap_list_clone(tree->branches, (oscap_clone_func) cvrf_branch_clone);
	clone->relationships = oscap_list_clone(tree->relationships, (oscap_clone_func) cvrf_relationship_clone);
	clone->product_groups = oscap_list_clone(tree->product_groups, (oscap_clone_func) cvrf_group_clone);
	return clone;
}

const char *get_cvrf_product_id_from_cpe(struct cvrf_product_tree *tree, const char *cpe) {
	const char *branch_id = NULL;
	struct oscap_iterator *branches = cvrf_product_tree_get_branches(tree);
	while (oscap_iterator_has_more(branches) && branch_id == NULL) {
		branch_id = get_cvrf_product_id_from_branch(oscap_iterator_next(branches), cpe);
	}
	oscap_iterator_free(branches);

	return branch_id;
}

int cvrf_product_tree_filter_by_cpe(struct cvrf_product_tree *tree, const char *cpe) {
	const char *branch_id = get_cvrf_product_id_from_cpe(tree, cpe);
	if (branch_id == NULL)
		return -1;

	struct oscap_list *filtered_relation = oscap_list_new();
	struct cvrf_relationship_iterator *relationships = cvrf_product_tree_get_relationships(tree);
	while (cvrf_relationship_iterator_has_more(relationships)) {
		struct cvrf_relationship *relation = cvrf_relationship_iterator_next(relationships);
		if (!strcmp(branch_id, cvrf_relationship_get_relates_to_ref(relation)))
			oscap_list_add(filtered_relation, cvrf_relationship_clone(relation));
	}
	cvrf_relationship_iterator_free(relationships);

	if (oscap_list_get_itemcount(filtered_relation) == 0) {
		oscap_list_free(filtered_relation, (oscap_destruct_func) cvrf_relationship_free);
		return -1;
	} else {
		oscap_list_free(tree->relationships, (oscap_destruct_func) cvrf_relationship_free);
		tree->relationships = filtered_relation;
		return 0;
	}
}

/***************************************************************************
 * CVRF Acknowledgments
 */

struct cvrf_acknowledgment {
	struct oscap_stringlist *names;
	struct oscap_stringlist *organizations;
	char *description;
	struct oscap_stringlist *urls;
};
OSCAP_ACCESSOR_STRING(cvrf_acknowledgment, description)

struct cvrf_acknowledgment *cvrf_acknowledgment_new() {
	struct cvrf_acknowledgment *ret = oscap_alloc(sizeof(struct cvrf_acknowledgment));
	if (ret == NULL)
		return NULL;

	ret->names = oscap_stringlist_new();
	ret->organizations = oscap_stringlist_new();
	ret->description = NULL;
	ret->urls = oscap_stringlist_new();
	return ret;
}

void cvrf_acknowledgment_free(struct cvrf_acknowledgment *ack) {
	if (ack == NULL)
		return;

	oscap_stringlist_free(ack->names);
	oscap_stringlist_free(ack->organizations);
	oscap_free(ack->description);
	oscap_stringlist_free(ack->urls);
	oscap_free(ack);
}

struct cvrf_acknowledgment *cvrf_acknowledgment_clone(const struct cvrf_acknowledgment *ack) {
	struct cvrf_acknowledgment *clone = oscap_alloc(sizeof(struct cvrf_acknowledgment));
	clone->names = oscap_stringlist_clone(ack->names);
	clone->organizations = oscap_stringlist_clone(ack->organizations);
	clone->description = oscap_strdup(ack->description);
	clone->urls = oscap_stringlist_clone(ack->urls);
	return clone;
}

/***************************************************************************
 * CVRF Notes
 */
struct cvrf_note {
	cvrf_note_type_t type;
	int ordinal;
	char *audience;
	char *title;
	char *contents;
};
OSCAP_ACCESSOR_SIMPLE(int, cvrf_note, ordinal)
OSCAP_ACCESSOR_STRING(cvrf_note, audience)
OSCAP_ACCESSOR_STRING(cvrf_note, title)
OSCAP_ACCESSOR_STRING(cvrf_note, contents)

cvrf_note_type_t cvrf_note_get_note_type(const struct cvrf_note *note) {
	return note->type;
}

struct cvrf_note *cvrf_note_new() {
	struct cvrf_note *ret = oscap_alloc(sizeof(struct cvrf_note));
	if (ret == NULL)
		return NULL;

	ret->type = CVRF_ATTRIBUTE_UNKNOWN;
	ret->ordinal = 0;
	ret->audience = NULL;
	ret->title = NULL;
	ret->contents = NULL;
	return ret;
}

void cvrf_note_free(struct cvrf_note *note) {
	if (note == NULL)
		return;

	oscap_free(note->audience);
	oscap_free(note->title);
	oscap_free(note->contents);
	oscap_free(note);
}

struct cvrf_note *cvrf_note_clone(const struct cvrf_note *note) {
	struct cvrf_note *clone = oscap_alloc(sizeof(struct cvrf_note));
	clone->type = note->type;
	clone->ordinal = note->ordinal;
	clone->audience = oscap_strdup(note->audience);
	clone->title = oscap_strdup(note->title);
	clone->contents = oscap_strdup(note->contents);
	return clone;
}

/***************************************************************************
 * CVRF Revision
 */

struct cvrf_revision {
	char *number;
	char *date;
	char *description;
};
OSCAP_ACCESSOR_STRING(cvrf_revision, number)
OSCAP_ACCESSOR_STRING(cvrf_revision, date)
OSCAP_ACCESSOR_STRING(cvrf_revision, description)

struct cvrf_revision *cvrf_revision_new() {
	struct cvrf_revision *ret = oscap_alloc(sizeof(struct cvrf_revision));
	if (ret == NULL)
		return NULL;

	ret->number = NULL;
	ret->date = NULL;
	ret->description = NULL;
	return ret;
}

void cvrf_revision_free(struct cvrf_revision *revision) {
	if (revision == NULL)
		return;

	oscap_free(revision->number);
	oscap_free(revision->date);
	oscap_free(revision->description);
	oscap_free(revision);
}

struct cvrf_revision *cvrf_revision_clone(const struct cvrf_revision *revision) {
	struct cvrf_revision *clone = oscap_alloc(sizeof(struct cvrf_revision));
	clone->number = oscap_strdup(revision->number);
	clone->date = oscap_strdup(revision->date);
	clone->description = oscap_strdup(revision->description);
	return clone;
}

/***************************************************************************
 * CVRF DocumentTracking
 */
struct cvrf_doc_tracking {
	char *tracking_id;
	struct oscap_stringlist *aliases;
	cvrf_doc_status_type_t status;
	char *version;
	struct oscap_list *revision_history;
	char *init_release_date;
	char *cur_release_date;
	// Generator
	char *generator_engine;
	char *generator_date;
};
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, tracking_id)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, version)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, init_release_date)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, cur_release_date)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, generator_engine)
OSCAP_ACCESSOR_STRING(cvrf_doc_tracking, generator_date)

cvrf_doc_status_type_t cvrf_doc_tracking_get_status(struct cvrf_doc_tracking *tracking) {
	return tracking->status;
}
struct oscap_string_iterator *cvrf_doc_tracking_get_aliases(struct cvrf_doc_tracking *tracking) {
	return oscap_stringlist_get_strings(tracking->aliases);
}

struct cvrf_doc_tracking *cvrf_doc_tracking_new() {
	struct cvrf_doc_tracking *ret = oscap_alloc(sizeof(struct cvrf_doc_tracking));
	if (ret == NULL)
		return NULL;

	ret->tracking_id = NULL;
	ret->aliases = oscap_stringlist_new();
	ret->status = CVRF_ATTRIBUTE_UNKNOWN;
	ret->version = NULL;
	ret->revision_history = oscap_list_new();
	ret->init_release_date = NULL;
	ret->cur_release_date = NULL;
	ret->generator_engine = NULL;
	ret->generator_date = NULL;
	return ret;
}

void cvrf_doc_tracking_free(struct cvrf_doc_tracking *tracking) {
	if (tracking == NULL)
		return;

	oscap_free(tracking->tracking_id);
	oscap_stringlist_free(tracking->aliases);
	oscap_free(tracking->version);
	oscap_list_free(tracking->revision_history, (oscap_destruct_func) cvrf_revision_free);
	oscap_free(tracking->init_release_date);
	oscap_free(tracking->cur_release_date);
	oscap_free(tracking->generator_engine);
	oscap_free(tracking->generator_date);
	oscap_free(tracking);
}

struct cvrf_doc_tracking *cvrf_doc_tracking_clone(const struct cvrf_doc_tracking *tracking) {
	struct cvrf_doc_tracking *clone = oscap_alloc(sizeof(struct cvrf_doc_tracking));
	clone->tracking_id = oscap_strdup(tracking->tracking_id);
	clone->aliases = oscap_stringlist_clone(tracking->aliases);
	clone->status = tracking->status;
	clone->version = oscap_strdup(tracking->version);
	clone->revision_history = oscap_list_clone(tracking->revision_history, (oscap_clone_func) cvrf_revision_clone);
	clone->init_release_date = oscap_strdup(tracking->init_release_date);
	clone->cur_release_date = oscap_strdup(tracking->cur_release_date);
	clone->generator_engine = oscap_strdup(tracking->generator_engine);
	clone->generator_date = oscap_strdup(tracking->generator_date);
	return clone;
}

/***************************************************************************
 * CVRF DocumentPublisher
 */

struct cvrf_doc_publisher {
	cvrf_doc_publisher_type_t type;
	char *vendor_id;
	char *contact_details;
	char *issuing_authority;
};
OSCAP_ACCESSOR_STRING(cvrf_doc_publisher, vendor_id)
OSCAP_ACCESSOR_STRING(cvrf_doc_publisher, contact_details)
OSCAP_ACCESSOR_STRING(cvrf_doc_publisher, issuing_authority)

cvrf_doc_publisher_type_t cvrf_doc_publisher_get_type(struct cvrf_doc_publisher *publisher) {
	return publisher->type;
}

struct cvrf_doc_publisher *cvrf_doc_publisher_new() {
	struct cvrf_doc_publisher *ret = oscap_alloc(sizeof(struct cvrf_doc_publisher));
	if (ret == NULL)
		return NULL;

	ret->type = CVRF_ATTRIBUTE_UNKNOWN;
	ret->vendor_id = NULL;
	ret->contact_details = NULL;
	ret->issuing_authority = NULL;
	return ret;
}

void cvrf_doc_publisher_free(struct cvrf_doc_publisher *publisher) {
	if (publisher == NULL)
		return;

	oscap_free(publisher->vendor_id);
	oscap_free(publisher->contact_details);
	oscap_free(publisher->issuing_authority);
	oscap_free(publisher);
}

struct cvrf_doc_publisher *cvrf_doc_publisher_clone(const struct cvrf_doc_publisher *publisher) {
	struct cvrf_doc_publisher *clone = oscap_alloc(sizeof(struct cvrf_doc_publisher));
	clone->type = publisher->type;
	clone->vendor_id = oscap_strdup(publisher->vendor_id);
	clone->contact_details = oscap_strdup(publisher->contact_details);
	clone->issuing_authority = oscap_strdup(publisher->issuing_authority);
	return clone;
}


/***************************************************************************
 * CVRF References
 */

struct cvrf_reference {
	cvrf_reference_type_t type;
	char *url;
	char *description;
};
OSCAP_ACCESSOR_STRING(cvrf_reference, url)
OSCAP_ACCESSOR_STRING(cvrf_reference, description)

cvrf_reference_type_t cvrf_reference_get_reference_type(struct cvrf_reference *reference) {
	return reference->type;
}

struct cvrf_reference *cvrf_reference_new() {
	struct cvrf_reference *ret = oscap_alloc(sizeof(struct cvrf_reference));
	if (ret == NULL)
		return NULL;

	ret->type = CVRF_ATTRIBUTE_UNKNOWN;
	ret->url = NULL;
	ret->description = NULL;

	return ret;
}

void cvrf_reference_free(struct cvrf_reference *ref) {
	if (ref == NULL)
		return;

	oscap_free(ref->url);
	oscap_free(ref->description);
	oscap_free(ref);
}

struct cvrf_reference *cvrf_reference_clone(const struct cvrf_reference *ref) {
	struct cvrf_reference *clone = oscap_alloc(sizeof(struct cvrf_reference));
	clone->type = ref->type;
	clone->url = oscap_strdup(ref->url);
	clone->description = oscap_strdup(ref->description);
	return clone;
}

/***************************************************************************
 * CVRF Document
 */
struct cvrf_document {
	char *doc_distribution;
	char *aggregate_severity;
	char *namespace;
	struct cvrf_doc_tracking *tracking;
	struct cvrf_doc_publisher *publisher;
	struct oscap_list *doc_notes;
	struct oscap_list *doc_references;
	struct oscap_list *acknowledgments;
};
OSCAP_ACCESSOR_STRING(cvrf_document, doc_distribution)
OSCAP_ACCESSOR_STRING(cvrf_document, aggregate_severity)
OSCAP_ACCESSOR_STRING(cvrf_document, namespace)
OSCAP_ACCESSOR_SIMPLE(struct cvrf_doc_tracking*, cvrf_document, tracking)
OSCAP_ACCESSOR_SIMPLE(struct cvrf_doc_publisher*, cvrf_document, publisher)

struct oscap_iterator *cvrf_document_get_references(struct cvrf_document *doc) {
	return oscap_iterator_new(doc->doc_references);
}

struct oscap_iterator *cvrf_document_get_acknowledgments(struct cvrf_document *doc) {
	return oscap_iterator_new(doc->acknowledgments);
}

struct cvrf_document *cvrf_document_new() {
	struct cvrf_document *ret = oscap_alloc(sizeof(struct cvrf_document));
	if (ret == NULL)
		return NULL;

	ret->doc_distribution = NULL;
	ret->aggregate_severity = NULL;
	ret->namespace = NULL;
	ret->tracking = cvrf_doc_tracking_new();
	ret->publisher = cvrf_doc_publisher_new();
	ret->doc_notes = oscap_list_new();
	ret->doc_references = oscap_list_new();
	ret->acknowledgments = oscap_list_new();
	return ret;
}

void cvrf_document_free(struct cvrf_document *doc) {
	if (doc == NULL)
		return;

	oscap_free(doc->doc_distribution);
	oscap_free(doc->aggregate_severity);
	oscap_free(doc->namespace);
	cvrf_doc_tracking_free(doc->tracking);
	cvrf_doc_publisher_free(doc->publisher);
	oscap_list_free(doc->doc_notes, (oscap_destruct_func) cvrf_note_free);
	oscap_list_free(doc->doc_references, (oscap_destruct_func) cvrf_reference_free);
	oscap_list_free(doc->acknowledgments, (oscap_destruct_func) cvrf_acknowledgment_free);
	oscap_free(doc);
}

struct cvrf_document *cvrf_document_clone(const struct cvrf_document *doc) {
	struct cvrf_document *clone = oscap_alloc(sizeof(struct cvrf_document));
	clone->doc_distribution = oscap_strdup(doc->doc_distribution);
	clone->aggregate_severity = oscap_strdup(doc->aggregate_severity);
	clone->namespace = oscap_strdup(doc->namespace);
	clone->tracking = cvrf_doc_tracking_clone(doc->tracking);
	clone->publisher = cvrf_doc_publisher_clone(doc->publisher);
	clone->doc_notes = oscap_list_clone(doc->doc_notes, (oscap_clone_func) cvrf_note_clone);
	clone->doc_references = oscap_list_clone(doc->doc_references, (oscap_clone_func) cvrf_reference_clone);
	clone->acknowledgments = oscap_list_clone(doc->acknowledgments, (oscap_clone_func) cvrf_acknowledgment_clone);
	return clone;
}

/***************************************************************************
 * CVRF Model
 * Top-level structure of the CVRF hierarchy
 */
struct cvrf_model {
	char *doc_title;
	char *doc_type;
	struct cvrf_document *document;
	struct cvrf_product_tree *tree;
	struct oscap_list *vulnerabilities;	/* 1-n */
};
OSCAP_ACCESSOR_STRING(cvrf_model, doc_title)
OSCAP_ACCESSOR_STRING(cvrf_model, doc_type)
OSCAP_ACCESSOR_SIMPLE(struct cvrf_document*, cvrf_model, document)
OSCAP_IGETINS_GEN(cvrf_vulnerability, cvrf_model, vulnerabilities, vulnerability)
OSCAP_ITERATOR_REMOVE_F(cvrf_vulnerability)

struct cvrf_product_tree *cvrf_model_get_product_tree(struct cvrf_model *model) {
	return model->tree;
}

const char *cvrf_model_get_identification(struct cvrf_model *model) {
	struct cvrf_doc_tracking *tracking = cvrf_document_get_tracking(model->document);
	return (cvrf_doc_tracking_get_tracking_id(tracking));
}

struct cvrf_model *cvrf_model_new() {
	struct cvrf_model *ret = oscap_alloc(sizeof(struct cvrf_model));
	if (ret == NULL)
		return NULL;

	ret->doc_title = NULL;
	ret->doc_type = NULL;
	ret->document = cvrf_document_new();
	ret->tree = cvrf_product_tree_new();
	ret->vulnerabilities = oscap_list_new();
	return ret;
}

void cvrf_model_free(struct cvrf_model *cvrf) {
	if (cvrf == NULL)
		return;

	oscap_free(cvrf->doc_title);
	oscap_free(cvrf->doc_type);
	cvrf_document_free(cvrf->document);
	cvrf_product_tree_free(cvrf->tree);
	oscap_list_free(cvrf->vulnerabilities, (oscap_destruct_func) cvrf_vulnerability_free);
	oscap_free(cvrf);
}

struct cvrf_model *cvrf_model_clone(const struct cvrf_model *model) {
	struct cvrf_model *clone = oscap_alloc(sizeof(struct cvrf_model));
	clone->doc_title = oscap_strdup(model->doc_title);
	clone->doc_type = oscap_strdup(model->doc_type);
	clone->document = cvrf_document_clone(model->document);
	clone->tree = cvrf_product_tree_clone(model->tree);
	clone->vulnerabilities = oscap_list_clone(model->vulnerabilities, (oscap_clone_func) cvrf_vulnerability_clone);
	return clone;
}

int cvrf_model_filter_by_cpe(struct cvrf_model *model, const char *cpe) {
	const char *product = get_cvrf_product_id_from_cpe(model->tree, cpe);
	int ret = cvrf_product_tree_filter_by_cpe(model->tree, cpe);

	struct cvrf_vulnerability_iterator *it = cvrf_model_get_vulnerabilities(model);
	while (cvrf_vulnerability_iterator_has_more(it)) {
		cvrf_vulnerability_filter_by_product(cvrf_vulnerability_iterator_next(it), product);
	}
	cvrf_vulnerability_iterator_free(it);
	return ret;
}

/***************************************************************************
 * CVRF Index
 */

struct cvrf_index {
	char *source_url;
	char *index_file;
	struct oscap_list *models;
};
OSCAP_ACCESSOR_STRING(cvrf_index, source_url)
OSCAP_ACCESSOR_STRING(cvrf_index, index_file)
OSCAP_IGETINS_GEN(cvrf_model, cvrf_index, models, model)
OSCAP_ITERATOR_REMOVE_F(cvrf_model)

struct cvrf_index *cvrf_index_new() {
	struct cvrf_index *ret = oscap_alloc(sizeof(struct cvrf_index));
	if (ret == NULL)
		return NULL;

	ret->source_url = NULL;
	ret->index_file = NULL;
	ret->models = oscap_list_new();
	return ret;
}

void cvrf_index_free(struct cvrf_index *index) {
	if (index == NULL)
		return;

	oscap_free(index->source_url);
	oscap_free(index->index_file);
	oscap_list_free(index->models, (oscap_destruct_func) cvrf_model_free);
	oscap_free(index);
}

struct cvrf_index *cvrf_index_clone(const struct cvrf_index *index) {
	struct cvrf_index *clone = oscap_alloc(sizeof(struct cvrf_index));
	clone->source_url = oscap_strdup(index->source_url);
	clone->index_file = oscap_strdup(index->index_file);
	clone->models = oscap_list_clone(index->models, (oscap_clone_func) cvrf_model_clone);
	return clone;
}

/* End of CVRF structure definitions
 ***************************************************************************/


/****************************************************************************
 * XML string variables definitions
 * */

#define TAG_CVRF_DOC BAD_CAST "cvrfdoc"
#define TAG_DOC_TITLE BAD_CAST "DocumentTitle"
#define TAG_DOC_TYPE BAD_CAST "DocumentType"
#define ATTR_LANG BAD_CAST "xml:lang"
#define TAG_DISTRIBUTION BAD_CAST "DocumentDistribution"
#define TAG_AGGREGATE_SEVERITY BAD_CAST "AggregateSeverity"
#define ATTR_NAMESPACE BAD_CAST "Namespace"
// DocumentPublisher
#define TAG_PUBLISHER BAD_CAST "DocumentPublisher"
#define ATTR_VENDOR_ID BAD_CAST "VendorID"
#define TAG_CONTACT_DETAILS BAD_CAST "ContactDetails"
#define TAG_ISSUING_AUTHORITY BAD_CAST "IssuingAuthority"
//Document
#define TAG_DOCUMENT_TRACKING BAD_CAST "DocumentTracking"
#define TAG_IDENTIFICATION BAD_CAST "Identification"
#define TAG_ALIAS BAD_CAST "Alias"
#define TAG_REVISION_HISTORY BAD_CAST "RevisionHistory"
#define TAG_REVISION BAD_CAST "Revision"
#define TAG_GENERATOR BAD_CAST "Generator"
#define TAG_GENERATOR_ENGINE BAD_CAST "Engine"
// Reference
#define TAG_DOCUMENT_REFERENCES BAD_CAST "DocumentReferences"
#define TAG_REFERENCES BAD_CAST "References"
#define TAG_REFERENCE BAD_CAST "Reference"
// Acknowledgment
#define TAG_ACKNOWLEDGMENTS BAD_CAST "Acknowledgments"
#define TAG_ACKNOWLEDGMENT BAD_CAST "Acknowledgment"
// Product Tree
#define TAG_PRODUCT_TREE BAD_CAST "ProductTree"
#define TAG_BRANCH BAD_CAST "Branch"
#define TAG_PRODUCT_NAME BAD_CAST "FullProductName"
//Relationship
#define TAG_RELATIONSHIP BAD_CAST "Relationship"
#define ATTR_PRODUCT_REFERENCE BAD_CAST "ProductReference"
#define ATTR_RELATES_TO_REF BAD_CAST "RelatesToProductReference"
// Group
#define TAG_PRODUCT_GROUPS BAD_CAST "ProductGroups"
#define TAG_GROUP BAD_CAST "Group"
// Vulnerabilities
#define TAG_VULNERABILITY BAD_CAST "Vulnerability"
#define ATTR_ORDINAL BAD_CAST "Ordinal"
#define TAG_DISCOVERY_DATE BAD_CAST "DiscoveryDate"
#define TAG_RELEASE_DATE BAD_CAST "ReleaseDate"
#define TAG_VULNERABILITY_CVE BAD_CAST "CVE"
#define TAG_VULNERABILITY_CWE BAD_CAST "CWE"
#define TAG_PRODUCT_STATUSES BAD_CAST "ProductStatuses"
#define TAG_INVOLVEMENTS BAD_CAST "Involvements"
#define TAG_INVOLVEMENT BAD_CAST "Involvement"
// ScoreSets
#define TAG_CVSS_SCORE_SETS BAD_CAST "CVSSScoreSets"
#define TAG_SCORE_SET BAD_CAST "ScoreSet"
#define TAG_VECTOR BAD_CAST "Vector"
#define TAG_BASE_SCORE BAD_CAST "BaseScore"
#define TAG_ENVIRONMENTAL_SCORE BAD_CAST "EnvironmentalScore"
#define TAG_TEMPORAL_SCORE BAD_CAST "TemporalScore"
// Remediations
#define TAG_REMEDIATIONS BAD_CAST "Remediations"
#define TAG_REMEDIATION BAD_CAST "Remediation"
// Threats
#define TAG_THREATS BAD_CAST "Threats"
#define TAG_THREAT BAD_CAST "Threat"
// General tags
#define TAG_DATE BAD_CAST "Date"
#define TAG_DESCRIPTION BAD_CAST "Description"
#define TAG_GROUP_ID BAD_CAST "GroupID"
#define TAG_ID BAD_CAST "ID"
#define TAG_NAME BAD_CAST "Name"
#define TAG_NUMBER BAD_CAST "Number"
#define TAG_ORGANIZATION BAD_CAST "Organization"
#define TAG_PRODUCT_ID BAD_CAST "ProductID"
#define TAG_STATUS BAD_CAST "Status"
#define TAG_TITLE BAD_CAST "Title"
#define ATTR_TYPE BAD_CAST "Type"
#define TAG_URL BAD_CAST "URL"
#define TAG_VERSION BAD_CAST "Version"
// namespaces
#define CVRF_NS BAD_CAST "http://www.icasi.org/CVRF/schema/cvrf/1.1"
#define PROD_NS BAD_CAST "http://www.icasi.org/CVRF/schema/prod/1.1"
#define VULN_NS BAD_CAST "http://www.icasi.org/CVRF/schema/vuln/1.1"

/* End of XML variable definitions
 *
 ***************************************************************************/

/****************************************************************************
 * Parsing functions
 *
 */

static void cvrf_set_parsing_error(const char *element) {
	oscap_seterr(OSCAP_EFAMILY_XML, "Could not parse CVRF file: Missing or invalid"
		"%s element\n", element);
}

static void cvrf_parse_container(xmlTextReaderPtr reader, struct oscap_list *list,
		cvrf_item_type_t item_type) {

	xmlTextReaderNextElement(reader);
	const char *tag = cvrf_item_type_get_text(item_type);
	bool error = false;
	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST tag) == 0) {
		if (item_type == CVRF_REVISION) {
			error = !oscap_list_add(list, cvrf_revision_parse(reader));
		} else if (item_type == CVRF_NOTE || item_type == CVRF_DOCUMENT_NOTE) {
			error = !oscap_list_add(list, cvrf_note_parse(reader));
		} else if (item_type == CVRF_REFERENCE || item_type == CVRF_DOCUMENT_REFERENCE) {
			error = !oscap_list_add(list, cvrf_reference_parse(reader));
		} else if (item_type == CVRF_ACKNOWLEDGMENT) {
			error = !oscap_list_add(list, cvrf_acknowledgment_parse(reader));
		} else if (item_type == CVRF_GROUP) {
			error = !oscap_list_add(list, cvrf_group_parse(reader));
		} else if (item_type == CVRF_INVOLVEMENT) {
			error = !oscap_list_add(list, cvrf_involvement_parse(reader));
		} else if (item_type == CVRF_PRODUCT_STATUS) {
			error = !oscap_list_add(list, cvrf_product_status_parse(reader));
		} else if (item_type ==  CVRF_REMEDIATION) {
			error = !oscap_list_add(list, cvrf_remediation_parse(reader));
		} else if (item_type == CVRF_THREAT) {
			error = !oscap_list_add(list, cvrf_threat_parse(reader));
		} else if (item_type == CVRF_SCORE_SET) {
			error = !oscap_list_add(list, cvrf_score_set_parse(reader));
		} else if (item_type == CVRF_VULNERABILITY) {
			error = !oscap_list_add(list, cvrf_vulnerability_parse(reader));
		}

		xmlTextReaderNextNode(reader);
		if (error) {
			cvrf_set_parsing_error(tag);
			break;
		}
	}
}

static char *cvrf_parse_element(xmlTextReaderPtr reader, const char *tagname, bool next_elm) {
	char *elm_value = NULL;
	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST tagname) == 0) {
		elm_value = oscap_element_string_copy(reader);
	}
	if (next_elm)
		xmlTextReaderNextElement(reader);

	return elm_value;
}

static int cvrf_parse_ordinal(xmlTextReaderPtr reader) {
	char *attribute = (char *)xmlTextReaderGetAttribute(reader, ATTR_ORDINAL);
	int ordinal = strtol(attribute, NULL, 10);
	oscap_free(attribute);
	return ordinal;
}

struct cvrf_index *cvrf_index_parse_xml(struct oscap_source *index_source) {
	__attribute__nonnull__(index_source);

	struct cvrf_index *index = cvrf_index_new();
	cvrf_index_set_index_file(index, oscap_source_readable_origin(index_source));
	struct oscap_stringlist *file_list = oscap_stringlist_new();
	struct cvrf_model *model;

	FILE* file = fopen(oscap_source_get_filepath(index_source), "r");
	char line[256];

	while (fgets(line, sizeof(line), file)) {
		printf("%s", line);
		oscap_trim(line);
		oscap_stringlist_add_string(file_list, line);
	}
	fclose(file);

	struct oscap_string_iterator *iterator = oscap_stringlist_get_strings(file_list);
	while (oscap_string_iterator_has_more(iterator)) {
		const char *filename = oscap_string_iterator_next(iterator);
		model = cvrf_model_import(oscap_source_new_from_file(filename));
		if (model)
			oscap_list_add(index->models, model);
	}
	oscap_string_iterator_free(iterator);
	oscap_source_free(index_source);

	return index;
}

struct cvrf_model *cvrf_model_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);
	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVRF_DOC)  != 0 ||
			xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT)
		return NULL;

	struct cvrf_model *ret = cvrf_model_new();
	xmlTextReaderNextElement(reader);
	ret->doc_title = cvrf_parse_element(reader, "DocumentTitle", true);
	ret->doc_type = cvrf_parse_element(reader, "DocumentType", true);
	ret->document = cvrf_document_parse(reader);
	ret->tree = cvrf_product_tree_parse(reader);
	cvrf_parse_container(reader, ret->vulnerabilities, CVRF_VULNERABILITY);

	return ret;
}

struct cvrf_document *cvrf_document_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_document *doc = cvrf_document_new();
	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PUBLISHER) == 0) {
		doc->publisher = cvrf_doc_publisher_parse(reader);
		xmlTextReaderNextElement(reader);
	}
	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DOCUMENT_TRACKING) == 0) {
		doc->tracking = cvrf_doc_tracking_parse(reader);
		xmlTextReaderNextElement(reader);
	}
	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "DocumentNotes") == 0) {
		cvrf_parse_container(reader, doc->doc_notes, CVRF_DOCUMENT_NOTE);
		xmlTextReaderNextNode(reader);
		xmlTextReaderNextNode(reader);
	}
	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DISTRIBUTION) == 0) {
		doc->doc_distribution = oscap_element_string_copy(reader);
		xmlTextReaderNextElement(reader);
	}
	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_AGGREGATE_SEVERITY) == 0) {
		doc->namespace = (char *)xmlTextReaderGetAttribute(reader, ATTR_NAMESPACE);
		doc->aggregate_severity = oscap_element_string_copy(reader);
		xmlTextReaderNextElement(reader);
	}
	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DOCUMENT_REFERENCES) == 0) {
		cvrf_parse_container(reader, doc->doc_references, CVRF_DOCUMENT_REFERENCE);
		xmlTextReaderNextElement(reader);
	}
	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_ACKNOWLEDGMENTS) == 0) {
		cvrf_parse_container(reader, doc->acknowledgments, CVRF_ACKNOWLEDGMENT);
		xmlTextReaderNextElement(reader);
	}
	return doc;
}

struct cvrf_note *cvrf_note_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_note *note = cvrf_note_new();
	if (xmlTextReaderIsEmptyElement(reader))
		return NULL;

	note->ordinal = cvrf_parse_ordinal(reader);
	note->type = cvrf_item_parse_type_attribute(reader, CVRF_NOTE);
	note->audience = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "Audience");
	note->title = (char *)xmlTextReaderGetAttribute(reader, TAG_TITLE);
	xmlTextReaderNextNode(reader);
	note->contents =oscap_element_string_copy(reader);
	xmlTextReaderNextNode(reader);
	xmlTextReaderNextNode(reader);
	return note;
}

struct cvrf_doc_publisher *cvrf_doc_publisher_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_doc_publisher *publisher = cvrf_doc_publisher_new();
	if (xmlTextReaderIsEmptyElement(reader))
		return publisher;

	publisher->type = cvrf_item_parse_type_attribute(reader, CVRF_DOCUMENT_PUBLISHER);
	publisher->vendor_id = (char *)xmlTextReaderGetAttribute(reader, ATTR_VENDOR_ID);
	xmlTextReaderNextElement(reader);

	publisher->contact_details = cvrf_parse_element(reader, "ContactDetails", true);
	publisher->issuing_authority = cvrf_parse_element(reader, "IssuingAuthority", false);
	xmlTextReaderNextNode(reader);

	return publisher;
}

struct cvrf_doc_tracking *cvrf_doc_tracking_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_doc_tracking *tracking = cvrf_doc_tracking_new();
	if (xmlTextReaderIsEmptyElement(reader))
		return tracking;

	xmlTextReaderNextElement(reader);
	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DOCUMENT_TRACKING) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}
		if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_IDENTIFICATION) == 0) {
			xmlTextReaderNextElement(reader);
			tracking->tracking_id = cvrf_parse_element(reader, "ID", false);
			while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_IDENTIFICATION) != 0) {
				if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_ALIAS) == 0) {
					oscap_stringlist_add_string(tracking->aliases, cvrf_parse_element(reader, "Alias", false));
					xmlTextReaderNextNode(reader);
				}
				xmlTextReaderNextNode(reader);
			}
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_STATUS) == 0) {
			tracking->status = cvrf_item_parse_type_attribute(reader, CVRF_DOCUMENT_TRACKING);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VERSION) == 0) {
			tracking->version = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REVISION_HISTORY) == 0) {
			cvrf_parse_container(reader, tracking->revision_history, CVRF_REVISION);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "InitialReleaseDate") == 0) {
			tracking->init_release_date = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "CurrentReleaseDate") == 0) {
			tracking->cur_release_date = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GENERATOR) == 0) {
			xmlTextReaderNextElement(reader);
			if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GENERATOR_ENGINE) == 0) {
				tracking->generator_engine = oscap_element_string_copy(reader);
				xmlTextReaderNextElement(reader);
			}
			if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DATE) == 0) {
				tracking->generator_date = oscap_element_string_copy(reader);
			}
		}

		xmlTextReaderNextNode(reader);
	}

	return tracking;
}

struct cvrf_revision *cvrf_revision_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_revision *revision = cvrf_revision_new();
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REVISION) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_NUMBER) == 0) {
			revision->number = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DATE) == 0) {
			revision->date = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DESCRIPTION) == 0) {
			revision->description = oscap_element_string_copy(reader);
		}
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);
	return revision;
}

struct cvrf_reference *cvrf_reference_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_reference *ref = cvrf_reference_new();
	ref->type = cvrf_item_parse_type_attribute(reader, CVRF_REFERENCE);

	xmlTextReaderNextElement(reader);
	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REFERENCE) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}
		if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_URL) == 0) {
			ref->url = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DESCRIPTION) == 0) {
			ref->description = oscap_element_string_copy(reader);
		}
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);

	return ref;
}

struct cvrf_acknowledgment *cvrf_acknowledgment_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);
	struct cvrf_acknowledgment *ack = cvrf_acknowledgment_new();
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_ACKNOWLEDGMENT) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_NAME) == 0) {
			oscap_stringlist_add_string(ack->names, oscap_element_string_copy(reader));
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_ORGANIZATION) == 0) {
			oscap_stringlist_add_string(ack->organizations, oscap_element_string_copy(reader));
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_URL) == 0) {
			oscap_stringlist_add_string(ack->urls, oscap_element_string_copy(reader));
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DESCRIPTION) == 0) {
			ack->description = oscap_element_string_copy(reader);
		}
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);
	return ack;
}

struct cvrf_product_tree *cvrf_product_tree_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_product_tree *tree = cvrf_product_tree_new();
	if (xmlTextReaderIsEmptyElement(reader))
		return tree;
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_TREE) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_NAME)) {
			if (!oscap_list_add(tree->product_names, cvrf_product_name_parse(reader)))
				cvrf_set_parsing_error("FullProductName");
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BRANCH)) {
			while(xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BRANCH) == 0) {
				if (!oscap_list_add(tree->branches, cvrf_branch_parse(reader)))
					cvrf_set_parsing_error("Branch");
			}
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_RELATIONSHIP)) {
			if (!oscap_list_add(tree->relationships, cvrf_relationship_parse(reader)))
				cvrf_set_parsing_error("Relationship");
		} else if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_GROUPS)) {
			cvrf_parse_container(reader, tree->product_groups, CVRF_GROUP);
		}
		xmlTextReaderNextNode(reader);
	}
	return tree;
}

struct cvrf_branch *cvrf_branch_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_branch *branch = cvrf_branch_new();
	branch->branch_name = (char *)xmlTextReaderGetAttribute(reader, TAG_NAME);
	branch->type = cvrf_item_parse_type_attribute(reader, CVRF_BRANCH);
	xmlTextReaderNextElement(reader);

	if (!xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_NAME)) {
		branch->product_name = cvrf_product_name_parse(reader);
		xmlTextReaderNextNode(reader);
		xmlTextReaderNextNode(reader);
	}
	else {
		while(xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BRANCH) == 0) {
			if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
				xmlTextReaderNextNode(reader);
				continue;
			}
			if (!oscap_list_add(branch->subbranches, cvrf_branch_parse(reader))) {
				cvrf_set_parsing_error("Branch");
			}
			xmlTextReaderNextNode(reader);
		}
	}
	xmlTextReaderNextNode(reader);
	return branch;
}

struct cvrf_relationship *cvrf_relationship_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_relationship *relation = cvrf_relationship_new();
	relation->product_reference = (char *)xmlTextReaderGetAttribute(reader, ATTR_PRODUCT_REFERENCE);
	relation->relation_type = cvrf_item_parse_type_attribute(reader, CVRF_RELATIONSHIP);
	relation->relates_to_ref = (char *)xmlTextReaderGetAttribute(reader, ATTR_RELATES_TO_REF);
	xmlTextReaderNextElement(reader);

	if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_NAME) == 0) {
		relation->product_name = cvrf_product_name_parse(reader);
	}
	xmlTextReaderNextNode(reader);
	return relation;
}

struct cvrf_group *cvrf_group_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_group *group = cvrf_group_new();
	group->group_id = (char *)xmlTextReaderGetAttribute(reader, TAG_GROUP_ID);
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GROUP) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}
		if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DESCRIPTION) == 0) {
			group->description = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_ID) == 0) {
			oscap_stringlist_add_string(group->product_ids, oscap_element_string_copy(reader));
		}
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);
	return group;
}

struct cvrf_product_name *cvrf_product_name_parse(xmlTextReaderPtr reader) {
	struct cvrf_product_name *full_name = cvrf_product_name_new();
	full_name->product_id = (char *)xmlTextReaderGetAttribute(reader, TAG_PRODUCT_ID);
	full_name->cpe = oscap_element_string_copy(reader);
	xmlTextReaderNextNode(reader);
	return full_name;
}

struct cvrf_vulnerability *cvrf_vulnerability_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);

	struct cvrf_vulnerability *vuln = cvrf_vulnerability_new();
	vuln->ordinal = cvrf_parse_ordinal(reader);
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TITLE) == 0) {
			vuln->title = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_ID) == 0) {
			vuln->system_name = (char *)xmlTextReaderGetAttribute(reader, BAD_CAST "SystemName");
			vuln->system_id = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DISCOVERY_DATE) == 0) {
			vuln->discovery_date = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_RELEASE_DATE) == 0) {
			vuln->release_date = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY_CVE) == 0) {
			vuln->cve_id = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VULNERABILITY_CWE) == 0) {
			oscap_stringlist_add_string(vuln->cwe_ids, oscap_element_string_copy(reader));
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "Notes") == 0) {
			cvrf_parse_container(reader, vuln->notes, CVRF_NOTE);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_INVOLVEMENTS) == 0) {
			cvrf_parse_container(reader, vuln->involvements, CVRF_INVOLVEMENT);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_STATUSES) == 0) {
			cvrf_parse_container(reader, vuln->product_statuses, CVRF_PRODUCT_STATUS);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_STATUS) == 0) {
			struct cvrf_product_status *stat = cvrf_product_status_parse(reader);
			if (stat != NULL)
				cvrf_vulnerability_add_cvrf_product_status(vuln, stat);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_THREATS) == 0) {
			cvrf_parse_container(reader, vuln->threats, CVRF_THREAT);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_CVSS_SCORE_SETS) == 0) {
			cvrf_parse_container(reader, vuln->score_sets, CVRF_SCORE_SET);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATIONS) == 0) {
			cvrf_parse_container(reader, vuln->remediations, CVRF_REMEDIATION);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REFERENCES) == 0) {
			cvrf_parse_container(reader, vuln->references, CVRF_REFERENCE);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_ACKNOWLEDGMENTS) == 0) {
			cvrf_parse_container(reader, vuln->acknowledgments, CVRF_ACKNOWLEDGMENT);
		}
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);
	return vuln;
}

struct cvrf_score_set *cvrf_score_set_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);
	struct cvrf_score_set *score_set = cvrf_score_set_new();
	xmlTextReaderNextElement(reader);
	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_SCORE_SET) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_VECTOR) == 0) {
			score_set->vector = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_ID) == 0) {
			oscap_stringlist_add_string(score_set->product_ids, oscap_element_string_copy(reader));
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_BASE_SCORE) == 0) {
			cvrf_score_set_add_metric(score_set, CVSS_BASE, oscap_element_string_copy(reader));
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_ENVIRONMENTAL_SCORE) == 0) {
			cvrf_score_set_add_metric(score_set, CVSS_ENVIRONMENTAL, oscap_element_string_copy(reader));
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_TEMPORAL_SCORE) == 0) {
			cvrf_score_set_add_metric(score_set, CVSS_TEMPORAL, oscap_element_string_copy(reader));
		}
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);
	return score_set;
}

struct cvrf_involvement *cvrf_involvement_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);
	struct cvrf_involvement *involve = cvrf_involvement_new();
	involve->status = cvrf_item_parse_type_attribute(reader, CVRF_INVOLVEMENT);
	involve->party = cvrf_doc_publisher_type_parse(reader, "Party");
	xmlTextReaderNextNode(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_INVOLVEMENT) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}
		if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DESCRIPTION) == 0) {
			involve->description = oscap_element_string_copy(reader);
		}
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);
	return involve;
}

struct cvrf_threat *cvrf_threat_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);
	struct cvrf_threat *threat = cvrf_threat_new();
	threat->type = cvrf_item_parse_type_attribute(reader, CVRF_THREAT);
	threat->date = (char *)xmlTextReaderGetAttribute(reader, TAG_DATE);
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_THREAT) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DESCRIPTION) == 0) {
			threat->description = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_ID) == 0) {
			oscap_stringlist_add_string(threat->product_ids, oscap_element_string_copy(reader));
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GROUP_ID) == 0) {
			oscap_stringlist_add_string(threat->group_ids, oscap_element_string_copy(reader));
		}
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);
	return threat;
}

struct cvrf_remediation *cvrf_remediation_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);
	struct cvrf_remediation *remed = cvrf_remediation_new();
	remed->type = cvrf_item_parse_type_attribute(reader, CVRF_REMEDIATION);
	remed->date = (char *)xmlTextReaderGetAttribute(reader, TAG_DATE);
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_REMEDIATION) != 0) {

		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}

		if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_DESCRIPTION) == 0) {
			remed->description = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_URL) == 0) {
			remed->url = oscap_element_string_copy(reader);
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_ID) == 0) {
			oscap_stringlist_add_string(remed->product_ids, oscap_element_string_copy(reader));
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_GROUP_ID) == 0) {
			oscap_stringlist_add_string(remed->group_ids, oscap_element_string_copy(reader));
		} else if (xmlStrcmp(xmlTextReaderConstLocalName(reader), BAD_CAST "Entitlement") == 0) {
			remed->entitlement = oscap_element_string_copy(reader);
		}
		xmlTextReaderNextNode(reader);
	}
	xmlTextReaderNextNode(reader);

	return remed;
}


struct cvrf_product_status *cvrf_product_status_parse(xmlTextReaderPtr reader) {
	__attribute__nonnull__(reader);
	struct cvrf_product_status *stat = cvrf_product_status_new();
	if (stat == NULL)
		return NULL;

	stat->type = cvrf_item_parse_type_attribute(reader, CVRF_PRODUCT_STATUS);
	xmlTextReaderNextElement(reader);

	while (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_STATUS) != 0) {
		if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			xmlTextReaderNextNode(reader);
			continue;
		}
		if (xmlStrcmp(xmlTextReaderConstLocalName(reader), TAG_PRODUCT_ID) == 0) {
			char *product_id = oscap_element_string_copy(reader);
			if (product_id) {
				oscap_stringlist_add_string(stat->product_ids, product_id);
				oscap_free(product_id);
			}
		}
		xmlTextReaderNextNode(reader);
	}

	return stat;
}


/***************************************************************************
 * Export functions
 *
 */

static xmlNode *cvrf_list_to_dom(struct oscap_list *list, xmlNode *parent, cvrf_item_type_t cvrf_type) {
	if (oscap_list_get_itemcount(list) == 0)
		return NULL;

	if (parent == NULL) {
		const char *container_tag = cvrf_item_type_get_container(cvrf_type);
		parent = xmlNewNode(NULL, BAD_CAST container_tag);
	}
	xmlNode *child = NULL;
	struct oscap_iterator *it = oscap_iterator_new(list);
	while (oscap_iterator_has_more(it)) {
		if (cvrf_type == CVRF_REVISION) {
			child = cvrf_revision_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_NOTE || cvrf_type == CVRF_DOCUMENT_NOTE) {
			child = cvrf_note_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_REFERENCE || cvrf_type == CVRF_DOCUMENT_REFERENCE) {
			child = cvrf_reference_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_ACKNOWLEDGMENT) {
			child = cvrf_acknowledgment_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_PRODUCT_NAME) {
			child = cvrf_product_name_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_BRANCH) {
			child = cvrf_branch_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_RELATIONSHIP) {
			child = cvrf_relationship_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_GROUP) {
			child = cvrf_group_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_VULNERABILITY) {
			child = cvrf_vulnerability_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_INVOLVEMENT) {
			child = cvrf_involvement_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_PRODUCT_STATUS) {
			child = cvrf_product_status_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_THREAT) {
			child = cvrf_threat_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_SCORE_SET) {
			child = cvrf_score_set_to_dom(oscap_iterator_next(it));
		} else if (cvrf_type == CVRF_REMEDIATION) {
			child = cvrf_remediation_to_dom(oscap_iterator_next(it));
		}
		xmlAddChild(parent, child);
	}
	oscap_iterator_free(it);
	return parent;
}

void cvrf_element_add_container(struct oscap_list *list, cvrf_item_type_t cvrf_type, xmlNode *parent) {
	xmlNode *container = cvrf_list_to_dom(list, NULL, cvrf_type);
	if (container)
		xmlAddChild(parent, container);
}

void cvrf_element_add_stringlist(struct oscap_stringlist *list, const char *tag_name, xmlNode *parent) {
	if (oscap_list_get_itemcount((struct oscap_list *)list) == 0)
		return;

	struct oscap_string_iterator *iterator = oscap_stringlist_get_strings(list);
	while (oscap_string_iterator_has_more(iterator)) {
		const char *string = oscap_string_iterator_next(iterator);
		xmlNewTextChild(parent, NULL, BAD_CAST tag_name, BAD_CAST string);
	}
	oscap_string_iterator_free(iterator);
}

void cvrf_element_add_attribute(const char *attr_name, const char *attr_value, xmlNode *element) {
	if (attr_value == NULL)
		return;

	xmlNewProp(element, BAD_CAST attr_name, BAD_CAST attr_value);
}

static void cvrf_element_add_ordinal(int ordinal, xmlNode *element) {
	char *ordinal_str = oscap_sprintf("%d", ordinal);
	xmlNewProp(element, ATTR_ORDINAL, BAD_CAST ordinal_str);
	oscap_free(ordinal_str);
}

void cvrf_element_add_child(const char *elm_name, const char *elm_value, xmlNode *parent) {
	if (elm_value == NULL)
		return;

	xmlNode *child = cvrf_element_to_dom(elm_name, elm_value);
	xmlAddChild(parent, child);
}

xmlNode *cvrf_element_to_dom(const char *elm_name, const char *elm_value) {
	if (elm_value == NULL)
		return NULL;

	xmlNode *elm_node = xmlNewNode(NULL, BAD_CAST elm_name);
	xmlNodeAddContent(elm_node, BAD_CAST elm_value);
	return elm_node;
}

struct oscap_source *cvrf_index_get_export_source(struct cvrf_index *index) {
	if (index == NULL)
		return NULL;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	cvrf_index_to_dom(index, doc, NULL, NULL);
	return oscap_source_new_from_xmlDoc(doc, NULL);
}

xmlNode *cvrf_index_to_dom(struct cvrf_index *index, xmlDocPtr doc, xmlNode *parent, void *user_args) {
	xmlNode *index_node = NULL;
	if (parent == NULL) {
		index_node = xmlNewNode(NULL, BAD_CAST "Index");
		xmlDocSetRootElement(doc, index_node);
	} else {
		index_node = xmlNewTextChild(parent, NULL, BAD_CAST "Index", NULL);
	}

	struct cvrf_model_iterator *models = cvrf_index_get_models(index);
	while (cvrf_model_iterator_has_more(models)) {
		struct cvrf_model *model = cvrf_model_iterator_next(models);
		cvrf_model_to_dom(model, doc, index_node, user_args);
	}
	cvrf_model_iterator_free(models);

	return index_node;
}

struct oscap_source *cvrf_model_get_export_source(struct cvrf_model *model) {
	if (model == NULL)
		return NULL;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}
	cvrf_model_to_dom(model, doc, NULL, NULL);
	return oscap_source_new_from_xmlDoc(doc, NULL);
}

xmlNode *cvrf_model_to_dom(struct cvrf_model *model, xmlDocPtr doc, xmlNode *parent, void *user_args) {
	xmlNode *root_node = NULL;
	if (parent == NULL) {
		root_node = xmlNewNode(NULL, BAD_CAST "cvrfdoc");
		xmlDocSetRootElement(doc, root_node);
	} else {
		root_node = xmlNewTextChild(parent, NULL, BAD_CAST "cvrfdoc", NULL);
	}
	xmlNewNs(root_node, CVRF_NS, NULL);
	xmlNewNs(root_node, CVRF_NS, BAD_CAST "cvrf");
	struct cvrf_document *cvrf_doc = model->document;

	xmlNode *title_node = xmlNewTextChild(root_node, NULL, TAG_DOC_TITLE, BAD_CAST model->doc_title);
	cvrf_element_add_attribute("xml:lang", "en", title_node);
	cvrf_element_add_child("DocumentType", model->doc_type, root_node);

	xmlAddChild(root_node, cvrf_doc_publisher_to_dom(cvrf_doc->publisher));
	xmlAddChild(root_node, cvrf_doc_tracking_to_dom(cvrf_doc->tracking));

	cvrf_element_add_container(cvrf_doc->doc_notes, CVRF_DOCUMENT_NOTE, root_node);
	xmlNode *distribution = xmlNewTextChild(root_node, NULL, TAG_DISTRIBUTION, BAD_CAST cvrf_doc->doc_distribution);
	cvrf_element_add_attribute("xml:lang", "en", distribution);
	xmlNode *severity = xmlNewTextChild(root_node, NULL, TAG_AGGREGATE_SEVERITY, BAD_CAST cvrf_doc->aggregate_severity);
	cvrf_element_add_attribute("Namespace", cvrf_doc->namespace, severity);
	xmlAddChild(root_node, cvrf_list_to_dom(cvrf_doc->doc_references, NULL, CVRF_DOCUMENT_REFERENCE));
	xmlAddChild(root_node, cvrf_list_to_dom(cvrf_doc->acknowledgments, NULL, CVRF_ACKNOWLEDGMENT));

	xmlAddChild(root_node, cvrf_product_tree_to_dom(model->tree));
	cvrf_list_to_dom(model->vulnerabilities, root_node, CVRF_VULNERABILITY);
	return root_node;
}

xmlNode *cvrf_doc_publisher_to_dom(struct cvrf_doc_publisher *publisher) {
	xmlNode *pub_node = xmlNewNode(NULL, TAG_PUBLISHER);
	cvrf_element_add_attribute("Type", cvrf_doc_publisher_type_get_text(publisher->type), pub_node);
	cvrf_element_add_child("ContactDetails", publisher->contact_details, pub_node);
	cvrf_element_add_child("IssuingAuthority", publisher->issuing_authority, pub_node);
	return pub_node;
}

xmlNode *cvrf_doc_tracking_to_dom(struct cvrf_doc_tracking *tracking) {
	xmlNode *tracking_node = xmlNewNode(NULL, TAG_DOCUMENT_TRACKING);
	if (tracking->tracking_id) {
		xmlNode *ident_node = xmlNewTextChild(tracking_node, NULL, TAG_IDENTIFICATION, NULL);
		cvrf_element_add_child("ID", tracking->tracking_id, ident_node);
		cvrf_element_add_stringlist(tracking->aliases, "Alias", ident_node);
	}

	const char *status = cvrf_doc_status_type_get_text(tracking->status);
	cvrf_element_add_child("Status", status, tracking_node);
	cvrf_element_add_child("Version", tracking->version, tracking_node);

	cvrf_element_add_container(tracking->revision_history, CVRF_REVISION, tracking_node);
	cvrf_element_add_child("InitialReleaseDate", tracking->init_release_date, tracking_node);
	cvrf_element_add_child("CurrentReleaseDate", tracking->cur_release_date, tracking_node);

	if (tracking->generator_engine) {
		xmlNode *generator_node = xmlNewTextChild(tracking_node, NULL, BAD_CAST "Generator", NULL);
		cvrf_element_add_child("Engine", tracking->generator_engine, generator_node);
		cvrf_element_add_child("Date", tracking->generator_date, generator_node);
	}
	return tracking_node;
}

xmlNode *cvrf_note_to_dom(struct cvrf_note *note) {
	xmlNode *note_node = cvrf_element_to_dom("Note", note->contents);
	cvrf_element_add_ordinal(note->ordinal, note_node);
	cvrf_element_add_attribute("Type", cvrf_note_type_get_text(note->type), note_node);
	cvrf_element_add_attribute("Title", note->title, note_node);
	cvrf_element_add_attribute("Audience", note->audience, note_node);
	return note_node;
}

xmlNode *cvrf_revision_to_dom(struct cvrf_revision *revision) {
	xmlNode *revision_node = xmlNewNode(NULL, TAG_REVISION);
	cvrf_element_add_child("Number", revision->number, revision_node);
	cvrf_element_add_child("Date", revision->date, revision_node);
	cvrf_element_add_child("Description", revision->description, revision_node);
	return revision_node;
}

xmlNode *cvrf_reference_to_dom(struct cvrf_reference *ref) {
	xmlNode *ref_node = xmlNewNode(NULL, TAG_REFERENCE);
	cvrf_element_add_attribute("Type", cvrf_reference_type_get_text(ref->type), ref_node);
	cvrf_element_add_child("URL", ref->url, ref_node);
	cvrf_element_add_child("Description", ref->description, ref_node);
	return ref_node;
}

xmlNode *cvrf_acknowledgment_to_dom(struct cvrf_acknowledgment *ack) {
	xmlNode *ack_node = xmlNewNode(NULL, TAG_ACKNOWLEDGMENT);
	cvrf_element_add_stringlist(ack->names, "Name", ack_node);
	cvrf_element_add_stringlist(ack->organizations, "Organization", ack_node);
	cvrf_element_add_child("Description", ack->description, ack_node);
	cvrf_element_add_stringlist(ack->urls, "URL", ack_node);
	return ack_node;
}

xmlNode *cvrf_product_name_to_dom(struct cvrf_product_name *full_name) {
	if (full_name->cpe == NULL)
		return NULL;

	xmlNode *name_node = cvrf_element_to_dom("FullProductName", full_name->cpe);
	cvrf_element_add_attribute("ProductID", full_name->product_id, name_node);
	return name_node;
}

xmlNode *cvrf_product_tree_to_dom(struct cvrf_product_tree *tree) {
	xmlNode *tree_node = xmlNewNode(NULL, TAG_PRODUCT_TREE);
	xmlNewNs(tree_node, PROD_NS, NULL);
	cvrf_list_to_dom(tree->product_names, tree_node, CVRF_PRODUCT_NAME);
	cvrf_list_to_dom(tree->branches, tree_node, CVRF_BRANCH);
	cvrf_list_to_dom(tree->relationships, tree_node, CVRF_RELATIONSHIP);
	cvrf_element_add_container(tree->product_groups, CVRF_GROUP, tree_node);
	return tree_node;
}

xmlNode *cvrf_branch_to_dom(struct cvrf_branch *branch) {
	xmlNode *branch_node = xmlNewNode(NULL, TAG_BRANCH);
	cvrf_element_add_attribute("Type", cvrf_branch_type_get_text(branch->type), branch_node);
	cvrf_element_add_attribute("Name", branch->branch_name, branch_node);

	if (branch->type == CVRF_BRANCH_PRODUCT_FAMILY) {
		cvrf_list_to_dom(branch->subbranches, branch_node, CVRF_BRANCH);
	} else {
		xmlAddChild(branch_node, cvrf_product_name_to_dom(branch->product_name));
	}
	return branch_node;
}

xmlNode *cvrf_relationship_to_dom(const struct cvrf_relationship *relation) {
	xmlNode *relation_node = xmlNewNode(NULL, TAG_RELATIONSHIP);
	cvrf_element_add_attribute("ProductReference", relation->product_reference, relation_node);
	cvrf_element_add_attribute("RelationType", cvrf_relationship_type_get_text(relation->relation_type), relation_node);
	cvrf_element_add_attribute("RelatesToProductReference", relation->relates_to_ref, relation_node);
	xmlAddChild(relation_node, cvrf_product_name_to_dom(relation->product_name));
	return relation_node;
}

xmlNode *cvrf_group_to_dom(const struct cvrf_group *group) {
	xmlNode *group_node = xmlNewNode(NULL, TAG_GROUP);
	cvrf_element_add_attribute("GroupID", group->group_id, group_node);
	cvrf_element_add_child("Description", group->description, group_node);
	cvrf_element_add_stringlist(group->product_ids, "ProductID", group_node);
	return group_node;
}

xmlNode *cvrf_vulnerability_to_dom(const struct cvrf_vulnerability *vuln) {
	xmlNode *vuln_node = xmlNewNode(NULL, TAG_VULNERABILITY);
	cvrf_element_add_ordinal(vuln->ordinal, vuln_node);
	xmlNewNs(vuln_node, VULN_NS, NULL);

	cvrf_element_add_child("Title", vuln->title, vuln_node);
	if (vuln->system_id) {
		xmlNode *id_node = xmlNewTextChild(vuln_node, NULL, BAD_CAST "ID", BAD_CAST vuln->system_id);
		cvrf_element_add_attribute("SystemName", vuln->system_name, id_node);
	}
	cvrf_element_add_container(vuln->notes, CVRF_NOTE, vuln_node);
	cvrf_element_add_child("DiscoveryDate", vuln->discovery_date, vuln_node);
	cvrf_element_add_child("ReleaseDate", vuln->release_date, vuln_node);
	cvrf_element_add_container(vuln->involvements, CVRF_INVOLVEMENT, vuln_node);
	cvrf_element_add_child("CVE", vuln->cve_id, vuln_node);
	cvrf_element_add_stringlist(vuln->cwe_ids, "CWE", vuln_node);

	cvrf_element_add_container(vuln->product_statuses, CVRF_PRODUCT_STATUS, vuln_node);
	cvrf_element_add_container(vuln->threats, CVRF_THREAT, vuln_node);
	cvrf_element_add_container(vuln->score_sets, CVRF_SCORE_SET, vuln_node);
	cvrf_element_add_container(vuln->remediations, CVRF_REMEDIATION, vuln_node);
	cvrf_element_add_container(vuln->references, CVRF_REFERENCE, vuln_node);
	cvrf_element_add_container(vuln->acknowledgments, CVRF_ACKNOWLEDGMENT, vuln_node);

	return vuln_node;
}

xmlNode *cvrf_involvement_to_dom(const struct cvrf_involvement *involve) {
	xmlNode *involve_node = xmlNewNode(NULL, TAG_INVOLVEMENT);
	cvrf_element_add_attribute("Status", cvrf_involvement_status_type_get_text(involve->status), involve_node);
	cvrf_element_add_attribute("Party",cvrf_doc_publisher_type_get_text(involve->party), involve_node);
	cvrf_element_add_child("Description", involve->description, involve_node);
	return involve_node;
}

xmlNode *cvrf_score_set_to_dom(const struct cvrf_score_set *score_set) {
	xmlNode *score_node = xmlNewNode(NULL, TAG_SCORE_SET);
	char *base = cvrf_score_set_get_base_score(score_set);
	cvrf_element_add_child("BaseScore", base, score_node);
	char *environmental = cvrf_score_set_get_environmental_score(score_set);
	cvrf_element_add_child("EnvironmentalScore", environmental, score_node);
	char *temporal = cvrf_score_set_get_temporal_score(score_set);
	cvrf_element_add_child("TemporalScore", temporal, score_node);
	cvrf_element_add_child("Vector", score_set->vector, score_node);
	cvrf_element_add_stringlist(score_set->product_ids, "ProductID", score_node);

	oscap_free(base);
	oscap_free(temporal);
	oscap_free(environmental);
	return score_node;
}

xmlNode *cvrf_product_status_to_dom(const struct cvrf_product_status *stat) {
	xmlNode *status_node = xmlNewNode(NULL, TAG_STATUS);
	cvrf_element_add_attribute("Type", cvrf_product_status_type_get_text(stat->type), status_node);
	cvrf_element_add_stringlist(stat->product_ids, "ProductID", status_node);
	return status_node;
}

xmlNode *cvrf_remediation_to_dom(const struct cvrf_remediation *remed) {
	xmlNode *remed_node = xmlNewNode(NULL, TAG_REMEDIATION);
	cvrf_element_add_attribute("Type", cvrf_remediation_type_get_text(remed->type), remed_node);

	xmlNode *desc_node = cvrf_element_to_dom("Description", remed->description);
	xmlNewProp(desc_node, ATTR_LANG, BAD_CAST "en");
	xmlAddChild(remed_node, desc_node);
	cvrf_element_add_child("URL", remed->url, remed_node);
	cvrf_element_add_child("Entitlement", remed->entitlement, remed_node);
	cvrf_element_add_stringlist(remed->product_ids, "ProductID", remed_node);
	cvrf_element_add_stringlist(remed->group_ids, "GroupID", remed_node);
	return remed_node;
}

xmlNode *cvrf_threat_to_dom(const struct cvrf_threat *threat) {
	xmlNode *threat_node = xmlNewNode(NULL, TAG_THREAT);
	cvrf_element_add_attribute("Type", cvrf_threat_type_get_text(threat->type), threat_node);
	cvrf_element_add_attribute("Date", threat->date, threat_node);

	cvrf_element_add_child("Description", threat->description, threat_node);
	cvrf_element_add_stringlist(threat->product_ids, "ProductID", threat_node);
	cvrf_element_add_stringlist(threat->group_ids, "GroupID", threat_node);
	return threat_node;
}


/* End of export functions
 * */
/***************************************************************************/

