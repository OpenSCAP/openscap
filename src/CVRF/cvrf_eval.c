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
#include "CPE/public/cpe_dict.h"
#include "CVSS/cvss_priv.h"
#include "CVSS/public/cvss_score.h"

#include "source/public/oscap_source.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"

#include "OVAL/public/oval_system_characteristics.h"
#include "OVAL/public/oval_definitions.h"
#include "OVAL/adt/oval_string_map_impl.h"
#include "OVAL/public/oval_types.h"
#include "OVAL/public/oval_probe_session.h"
#include "OVAL/public/oval_probe.h"
#include "OVAL/oval_definitions_impl.h"


/*****************************************************************************
 * Structure definitions
 */

struct cvrf_session {
	struct cvrf_model *model;
	char *os_name;
	struct oscap_source *source;
	char *export_file;
	char *results_file;
	struct oscap_stringlist *product_ids;
	struct oval_definition_model *def_model;
};
OSCAP_ACCESSOR_STRING(cvrf_session, os_name);
OSCAP_ACCESSOR_STRING(cvrf_session, export_file);
OSCAP_ACCESSOR_STRING(cvrf_session, results_file);

struct oscap_string_iterator *cvrf_session_get_product_ids(struct cvrf_session *session) {
	return oscap_stringlist_get_strings(session->product_ids);
}
struct cvrf_model *cvrf_session_get_model(struct cvrf_session *session) {
	return session->model;
}
void cvrf_session_set_model(struct cvrf_session *session, struct cvrf_model *model) {
	session->model = model;
}

struct cvrf_session *cvrf_session_new_from_source(struct oscap_source *source) {

	if (source == NULL)
		return NULL;

	struct cvrf_session *ret = oscap_alloc(sizeof(struct cvrf_session));
	ret->source = source;
	ret->model = cvrf_model_import(source);
	ret->os_name = NULL;
	ret->export_file = NULL;
	ret->results_file = NULL;
	ret->product_ids = oscap_stringlist_new();
	ret->def_model = oval_definition_model_new();
	return ret;
}

void cvrf_session_free(struct cvrf_session *session) {

	if (session == NULL)
		return;

	cvrf_model_free(session->model);
	oscap_free(session->os_name);
	oscap_source_free(session->source);
	oscap_free(session->export_file);
	oscap_free(session->results_file);
	oscap_stringlist_free(session->product_ids);
	oval_definition_model_free(session->def_model);
	oscap_free(session);
}


struct cvrf_rpm_attributes {
	char *full_package_name;
	char *rpm_name;
	char *evr_format;
};
OSCAP_ACCESSOR_STRING(cvrf_rpm_attributes, full_package_name)
OSCAP_ACCESSOR_STRING(cvrf_rpm_attributes, rpm_name)
OSCAP_ACCESSOR_STRING(cvrf_rpm_attributes, evr_format)


struct cvrf_rpm_attributes *cvrf_rpm_attributes_new() {
	struct cvrf_rpm_attributes *ret;

	ret = oscap_alloc(sizeof(struct cvrf_rpm_attributes));
	if (ret == NULL)
		return NULL;

	ret->full_package_name = NULL;
	ret->rpm_name = NULL;
	ret->evr_format = NULL;

	return ret;
}

void cvrf_rpm_attributes_free(struct cvrf_rpm_attributes *attributes) {

	if (attributes == NULL)
		return;

	oscap_free(attributes->full_package_name);
	oscap_free(attributes->rpm_name);
	oscap_free(attributes->evr_format);
	oscap_free(attributes);
}

/*
 * End of structure definitions
 *****************************************************************************/


#define TAG_CVRF_DOC BAD_CAST "cvrfdoc"
#define TAG_DOC_TITLE BAD_CAST "DocumentTitle"
#define TAG_DOC_TYPE BAD_CAST "DocumentType"
#define ATTR_PRODUCT_ID "ProductID"
//namespaces
#define CVRF_NS BAD_CAST "http://www.icasi.org/CVRF/schema/cvrf/1.1"
#define VULN_NS BAD_CAST "http://www.icasi.org/CVRF/schema/vuln/1.1"

static int find_all_cvrf_product_ids_from_cpe(struct cvrf_session *session) {

	if (cvrf_model_filter_by_cpe(session->model, session->os_name) == -1)
		return -1;

	struct cvrf_product_tree *tree = cvrf_model_get_product_tree(session->model);
	struct cvrf_relationship_iterator *relationships = cvrf_product_tree_get_relationships(tree);
	while (cvrf_relationship_iterator_has_more(relationships)) {
		struct cvrf_relationship *relation = cvrf_relationship_iterator_next(relationships);
		struct cvrf_product_name *name = cvrf_relationship_get_product_name(relation);
		oscap_stringlist_add_string(session->product_ids, cvrf_product_name_get_product_id(name));
	}
	cvrf_relationship_iterator_free(relationships);
	return 0;
}

int cvrf_export_results(struct oscap_source *import_source, const char *export_file, const char *os_name) {
	__attribute__nonnull__(import_source);
	__attribute__nonnull__(export_file);

	struct cvrf_session *session = cvrf_session_new_from_source(import_source);
	cvrf_session_set_os_name(session, os_name);
	cvrf_session_set_results_file(session, export_file);

	find_all_cvrf_product_ids_from_cpe(session);
	cvrf_session_construct_definition_model(session);
	cvrf_model_export(session->model, "cvrf_model_export.xml");

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
	}
	xmlNode *root_node = xmlNewNode(NULL, BAD_CAST "cvrfdoc");
	xmlDocSetRootElement(doc, root_node);
	xmlNewNs(root_node, CVRF_NS, NULL);
	cvrf_element_add_child("DocumentTitle", cvrf_model_get_doc_title(session->model), root_node);
	cvrf_element_add_child("DocumentType", cvrf_model_get_doc_type(session->model), root_node);
	//cvrf_export_element(os_name, "OS Name", writer);

	struct cvrf_vulnerability_iterator *it = cvrf_model_get_vulnerabilities(session->model);
	bool vulnerable = false;
	while (cvrf_vulnerability_iterator_has_more(it)) {
		struct cvrf_vulnerability *vuln = cvrf_vulnerability_iterator_next(it);
		xmlNode *vuln_node = xmlNewTextChild(root_node, NULL, BAD_CAST "Vulnerability", NULL);

		cvrf_element_add_child("Title", cvrf_vulnerability_get_title(vuln), vuln_node);
		cvrf_element_add_child("ID", cvrf_vulnerability_get_system_id(vuln), vuln_node);
		cvrf_element_add_child("DiscoveryDate", cvrf_vulnerability_get_discovery_date(vuln), vuln_node);
		cvrf_element_add_child("ReleaseDate", cvrf_vulnerability_get_release_date(vuln), vuln_node);
		cvrf_element_add_child("CVE", cvrf_vulnerability_get_cve_id(vuln), vuln_node);

		xmlNode *statuses_node = xmlNewTextChild(vuln_node, NULL, BAD_CAST "ProductStatuses", NULL);
		struct oscap_string_iterator *product_ids = cvrf_session_get_product_ids(session);
		while (oscap_string_iterator_has_more(product_ids)) {
			const char *product_id = oscap_string_iterator_next(product_ids);
			/*
			if (cvrf_product_vulnerability_fixed(vuln, product_id)) {

			}
			else {

				vulnerable = true;
			}*/
		}

		if (vulnerable && cvrf_vulnerability_get_remediation_count(vuln) > 0) {
			xmlNode *remediations_node = xmlNewTextChild(vuln_node, NULL, BAD_CAST "Remediations", NULL);
			struct cvrf_remediation_iterator *remediations = cvrf_vulnerability_get_remediations(vuln);
			while (cvrf_remediation_iterator_has_more(remediations)) {
				xmlNode *remed_node = cvrf_remediation_to_dom(cvrf_remediation_iterator_next(remediations));
				xmlAddChild(remediations_node, remed_node);
			}
			cvrf_remediation_iterator_free(remediations);
		}

		oscap_string_iterator_free(product_ids);
		vulnerable = false;
	}
	cvrf_vulnerability_iterator_free(it);

	struct oscap_source *source = oscap_source_new_from_xmlDoc(doc, export_file);
	int ret = oscap_source_save_as(source, NULL);
	oscap_source_free(source);
	cvrf_session_free(session);
	return ret;
}


static const char *get_rpm_name_from_cvrf_product_id(struct cvrf_session *session, const char *product_id) {

	const char *rpm_name = NULL;
	struct cvrf_product_tree *tree = cvrf_model_get_product_tree(session->model);

	struct oscap_iterator *branches = cvrf_product_tree_get_branches(tree);
	while (oscap_iterator_has_more(branches)) {
		struct cvrf_branch *branch = oscap_iterator_next(branches);
		if (cvrf_branch_get_branch_type(branch) == CVRF_BRANCH_PRODUCT_VERSION) {
			struct cvrf_product_name *full_name = cvrf_branch_get_cvrf_product_name(branch);

			if (oscap_str_endswith(product_id, cvrf_product_name_get_product_id(full_name))) {
				rpm_name = cvrf_product_name_get_cpe(full_name);
				break;
			}
		}
	}
	oscap_iterator_free(branches);
	return rpm_name;
}

static struct cvrf_rpm_attributes *parse_rpm_attributes_from_cvrf_product_id(struct cvrf_session *session,
		const char *product_id) {

	struct cvrf_rpm_attributes *attributes = cvrf_rpm_attributes_new();
	attributes->full_package_name = strdup(get_rpm_name_from_cvrf_product_id(session, product_id));
	char *package = strdup(strchr(product_id, ':')+1);
	unsigned int index = 0;
	while (index < strlen(package)) {
		if (package[index] == ':')
			break;
		index++;
	}
	char *evr = &package[index-1];
	package[index-2] = NULL;

	attributes->evr_format = evr;
	attributes->rpm_name = package;
	return attributes;
}


bool cvrf_product_vulnerability_fixed(struct cvrf_vulnerability *vuln, char *product) {

	struct cvrf_product_status_iterator *it = cvrf_vulnerability_get_product_statuses(vuln);
	while (cvrf_product_status_iterator_has_more(it)) {
		struct cvrf_product_status *stat = cvrf_product_status_iterator_next(it);
		struct oscap_string_iterator *product_ids = cvrf_product_status_get_ids(stat);

		while (oscap_string_iterator_has_more(product_ids)) {
			const char *product_id = oscap_string_iterator_next(product_ids);

			if (strcmp(product_id, product)) {
				oscap_string_iterator_free(product_ids);
				cvrf_product_status_iterator_free(it);
				return true;
			}
		}
		oscap_string_iterator_free(product_ids);
	}
	cvrf_product_status_iterator_free(it);

	return false;
}


static const char *get_oval_id_string(const char *type, int object_number) {

	struct oscap_string *string = oscap_string_new();
	char *number_as_string = oscap_sprintf("%d", object_number);

	if (!strcmp(type, "object")) {
			oscap_string_append_string(string, "oval:org.open-scap.unix:obj:");
	} else if (!strcmp(type, "state")) {
			oscap_string_append_string(string, "oval:org.open-scap.unix:ste:");
	} else if (!strcmp(type, "test")) {
			oscap_string_append_string(string, "oval:org.open-scap.unix:tst:");
	} else if (!strcmp(type, "definition")) {
		oscap_string_append_string(string, "oval:org.open-scap.unix:def:");
	}

	oscap_string_append_string(string, number_as_string);
	char *oval_id = strdup(oscap_string_get_cstr(string));
	oscap_string_free(string);
	oscap_free(number_as_string);

	return oval_id;
}


static struct oval_object *get_new_oval_object_for_cvrf(struct oval_definition_model *def_model,
		struct cvrf_rpm_attributes *attributes, int objectNo) {

	const char *object_id = get_oval_id_string("object", objectNo);

	struct oval_object *object = oval_definition_model_get_new_object(def_model, object_id);
	oval_object_set_subtype(object, OVAL_LINUX_RPM_INFO);
	struct oval_object_content *object_content = oval_object_content_new(def_model, OVAL_OBJECTCONTENT_ENTITY);
	struct oval_entity *object_entity = oval_entity_new(def_model);
	oval_entity_set_name(object_entity, attributes->rpm_name);
	oval_object_content_set_entity(object_content, object_entity);
	oval_object_add_object_content(object, object_content);

	return object;
}

static struct oval_state *get_new_oval_state_for_cvrf(struct oval_definition_model *def_model,
		struct cvrf_rpm_attributes *attributes, int stateNo) {

	const char *state_id = get_oval_id_string("state", stateNo);

	// Entity (Package name match)
	struct oval_entity *state_entity = oval_entity_new(def_model);
	oval_entity_set_name(state_entity, "name");
	oval_entity_set_operation(state_entity, OVAL_OPERATION_PATTERN_MATCH);
	struct oval_value *state_value = oval_value_new(OVAL_DATATYPE_STRING, attributes->rpm_name);
	oval_entity_set_value(state_entity, state_value);
	// Content (Package name match)
	struct oval_state_content *state_content = oval_state_content_new(def_model);
	oval_state_content_set_entity(state_content, state_entity);


	// Entity (EVR format less than)
	struct oval_entity *evr_entity = oval_entity_new(def_model);
	oval_entity_set_name(evr_entity, "evr");
	oval_entity_set_datatype(evr_entity, OVAL_DATATYPE_EVR_STRING);
	oval_entity_set_operation(evr_entity, OVAL_OPERATION_LESS_THAN);
	struct oval_value *evr_value = oval_value_new(OVAL_DATATYPE_EVR_STRING, attributes->evr_format);
	oval_entity_set_value(evr_entity, evr_value);
	// Content (EVR format less than)
	struct oval_state_content *evr_content = oval_state_content_new(def_model);
	oval_state_content_set_entity(evr_content, evr_entity);

	struct oval_state *state = oval_definition_model_get_new_state(def_model, state_id);
	oval_state_set_comment(state, attributes->full_package_name);
	oval_state_set_subtype(state, OVAL_LINUX_RPM_INFO);
	oval_state_set_operator(state, OVAL_OPERATOR_AND);
	oval_state_set_version(state, 1);
	oval_state_add_content(state, state_content);
	oval_state_add_content(state, evr_content);

	//oscap_free(rpm_name_match);
	return state;
}

static struct oval_test *get_new_rpminfo_test_for_cvrf(struct oval_definition_model *def_model,
		struct cvrf_rpm_attributes *attributes, int testNo) {

	struct oval_test *rpm_test = oval_test_new(def_model, get_oval_id_string("test", testNo));
	oval_test_set_subtype(rpm_test,OVAL_LINUX_RPM_INFO);
	oval_test_set_version(rpm_test, 1);
	oval_test_set_check(rpm_test, OVAL_CHECK_AT_LEAST_ONE);
	oval_test_set_existence(rpm_test, OVAL_AT_LEAST_ONE_EXISTS);

	oval_test_add_state(rpm_test, get_new_oval_state_for_cvrf(def_model, attributes, testNo));
	oval_test_set_object(rpm_test, get_new_oval_object_for_cvrf(def_model, attributes, testNo));

	return rpm_test;
}

static struct oval_definition *get_new_oval_definition_for_cvrf(struct oval_definition_model *def_model,
		struct cvrf_rpm_attributes *attributes, int index) {

	const char *definition_id = get_oval_id_string("definition", index);
	struct oval_definition *definition = oval_definition_model_get_new_definition(def_model, definition_id);
	oval_definition_set_version(definition, 1);
	oval_definition_set_title(definition, "CVRF RPM Vulnerability Test");

	struct oval_criteria_node *criteria = oval_criteria_node_new(def_model, OVAL_NODETYPE_CRITERIA);
	oval_definition_set_criteria(definition, criteria);

	struct oval_criteria_node *criterion = oval_criteria_node_new(def_model, OVAL_NODETYPE_CRITERION);
	oval_criteria_node_set_test(criterion, get_new_rpminfo_test_for_cvrf(def_model, attributes, index));
	char *comment = oscap_sprintf("Check for vulnerability of package %s", attributes->rpm_name);
	oval_criteria_node_set_comment(criterion, comment);
	oval_criteria_node_add_subnode(criteria, criterion);

	return definition;
}

int cvrf_session_construct_definition_model(struct cvrf_session *session) {

	struct oval_definition_model *def_model = session->def_model;
	struct oscap_string_iterator *product_ids = cvrf_session_get_product_ids(session);
	int index = 1;

	while (oscap_string_iterator_has_more(product_ids)) {
		const char *product_id = oscap_string_iterator_next(product_ids);
		struct cvrf_rpm_attributes *attributes = parse_rpm_attributes_from_cvrf_product_id(session, product_id);
		struct oval_definition *definition = get_new_oval_definition_for_cvrf(def_model, attributes, index);
		index++;
	}
	oscap_string_iterator_free(product_ids);

	oval_definition_model_export(def_model, "cvrf-definition-model-export.xml");

	/*
	struct oval_syschar_model *syschar_model = oval_syschar_model_new(def_model);
	struct oval_probe_session_t *probe_session = oval_probe_session_new(syschar_model);
	oval_syschar_model_free(syschar_model);
	oval_probe_session_free(probe_session);
	*/
	return 0;
}

