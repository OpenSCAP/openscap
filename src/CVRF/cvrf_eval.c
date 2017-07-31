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

struct cvrf_model_eval {
	char *os_name;
	char *os_version;
	struct oscap_stringlist *product_ids;
	struct cvrf_model *model;
	struct oval_definition_model *def_model;
};
OSCAP_ACCESSOR_STRING(cvrf_model_eval, os_name);
OSCAP_ACCESSOR_STRING(cvrf_model_eval, os_version);

struct oscap_string_iterator *cvrf_model_eval_get_product_ids(struct cvrf_model_eval *eval) {
	return oscap_stringlist_get_strings(eval->product_ids);
}
struct cvrf_model *cvrf_eval_get_model(struct cvrf_model_eval *eval) {
	return eval->model;
}
void cvrf_eval_set_model(struct cvrf_model_eval *eval, struct cvrf_model *model) {
	eval->model = model;
}

struct cvrf_model_eval *cvrf_model_eval_new() {
	struct cvrf_model_eval *ret;

	ret = oscap_alloc(sizeof(struct cvrf_model_eval));
	if (ret == NULL)
		return NULL;

	ret->os_name = NULL;
	ret->os_version = NULL;
	ret->product_ids = oscap_stringlist_new();
	ret->model = cvrf_model_new();
	ret->def_model = oval_definition_model_new();

	return ret;
}

void cvrf_model_eval_free(struct cvrf_model_eval *eval) {

	if (eval == NULL)
		return;

	oscap_free(eval->os_name);
	oscap_free(eval->os_version);
	oscap_stringlist_free(eval->product_ids);
	cvrf_model_free(eval->model);
	oval_definition_model_free(eval->def_model);
	oscap_free(eval);
}



struct cvrf_rpm_attributes {
	char *full_package_name;
	char *rpm_name;
	char *rpm_version;
	char *rpm_release;
	char *rpm_architecture;
	char *evr_format;
};
OSCAP_ACCESSOR_STRING(cvrf_rpm_attributes, full_package_name)
OSCAP_ACCESSOR_STRING(cvrf_rpm_attributes, rpm_name)
OSCAP_ACCESSOR_STRING(cvrf_rpm_attributes, rpm_version)
OSCAP_ACCESSOR_STRING(cvrf_rpm_attributes, rpm_release)
OSCAP_ACCESSOR_STRING(cvrf_rpm_attributes, rpm_architecture)
OSCAP_ACCESSOR_STRING(cvrf_rpm_attributes, evr_format)


struct cvrf_rpm_attributes *cvrf_rpm_attributes_new() {
	struct cvrf_rpm_attributes *ret;

	ret = oscap_alloc(sizeof(struct cvrf_rpm_attributes));
	if (ret == NULL)
		return NULL;

	ret->full_package_name = NULL;
	ret->rpm_name = NULL;
	ret->rpm_version = NULL;
	ret->rpm_release = NULL;
	ret->rpm_architecture = NULL;
	ret->evr_format = NULL;

	return ret;
}

void cvrf_rpm_attributes_free(struct cvrf_rpm_attributes *attributes) {

	if (attributes == NULL)
		return;

	oscap_free(attributes->full_package_name);
	oscap_free(attributes->rpm_name);
	oscap_free(attributes->rpm_version);
	oscap_free(attributes->rpm_release);
	oscap_free(attributes->rpm_architecture);
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


static const char *get_cvrf_product_id_from_branch(struct cvrf_model_eval *eval, struct cvrf_branch *branch) {

	const char *product_id = NULL;
	const char *os_name = cvrf_model_eval_get_os_name(eval);
	struct cvrf_branch *subbranch;
	struct cvrf_product_name *full_name;

	if (cvrf_branch_get_branch_type(branch) == CVRF_BRANCH_PRODUCT_FAMILY) {
		struct oscap_iterator *subbranches = cvrf_branch_get_subbranches(branch);

		while(oscap_iterator_has_more(subbranches)) {
			subbranch = oscap_iterator_next(subbranches);
			product_id = get_cvrf_product_id_from_branch(eval, subbranch);

			if (product_id != NULL)
				break;
		}
		oscap_iterator_free(subbranches);
		return product_id;
	}
	else {
		if (!strcmp(cvrf_branch_get_branch_name(branch), os_name)) {
			full_name = cvrf_branch_get_cvrf_product_name(branch);
			product_id = cvrf_product_name_get_product_id(full_name);
			return product_id;
		}
	}
	return product_id;
}

static void find_all_cvrf_product_ids_by_OS(struct cvrf_model_eval *eval, struct cvrf_model *model) {

	const char *branch_product_id = NULL;
	const char *relation_product_id = NULL;
	struct cvrf_product_tree *tree = cvrf_model_get_product_tree(model);
	struct cvrf_relationship *relation;
	struct cvrf_product_name *full_name;
	struct cvrf_branch *branch;

	struct oscap_iterator *branches = cvrf_product_tree_get_branches(tree);
	while (oscap_iterator_has_more(branches)) {
		branch = oscap_iterator_next(branches);
		branch_product_id = get_cvrf_product_id_from_branch(eval, branch);

		if (branch_product_id != NULL)
			break;
	}
	oscap_iterator_free(branches);
	printf("Branch Product ID: %s\n", branch_product_id);
	if (branch_product_id == NULL)
		return;


	struct cvrf_relationship_iterator *relationships = cvrf_product_tree_get_relationships(tree);
	while (cvrf_relationship_iterator_has_more(relationships)) {
		relation = cvrf_relationship_iterator_next(relationships);
		if (!strcmp(branch_product_id, cvrf_relationship_get_relates_to_ref(relation))) {
			full_name = cvrf_relationship_get_product_name(relation);
			relation_product_id = cvrf_product_name_get_product_id(full_name);
			printf("Relation Product ID: %s\n", relation_product_id);
			oscap_stringlist_add_string(eval->product_ids, relation_product_id);
		}
	}
	cvrf_relationship_iterator_free(relationships);
}

void cvrf_export_results(struct oscap_source *import_source, const char *export_file, const char *os_name) {
	__attribute__nonnull__(import_source);
	__attribute__nonnull__(export_file);

	struct cvrf_model_eval *eval = cvrf_model_eval_new();
	struct cvrf_model *model = cvrf_model_import(import_source);
	cvrf_eval_set_model(eval, model);
	cvrf_model_eval_set_os_name(eval, os_name);
	find_all_cvrf_product_ids_by_OS(eval, model);
	cvrf_model_eval_construct_definition_model(eval);

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.1");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
	}
	xmlNode *root_node = xmlNewNode(NULL, BAD_CAST "cvrfdoc");
	xmlDocSetRootElement(doc, root_node);
	xmlNs *cvrf_ns = xmlNewNs(root_node, CVRF_NS, NULL);
	xmlNode *title_node = xmlNewTextChild(root_node, NULL, TAG_DOC_TITLE, BAD_CAST cvrf_model_get_doc_title(model));
	xmlNode *type_node = xmlNewTextChild(root_node, NULL, TAG_DOC_TYPE, BAD_CAST cvrf_model_get_doc_type(model));
	//cvrf_export_element(os_name, "OS Name", writer);

	struct cvrf_vulnerability_iterator *it = cvrf_model_get_vulnerabilities(model);
	bool vulnerable = false;
	while (cvrf_vulnerability_iterator_has_more(it)) {
		struct cvrf_vulnerability *vuln = cvrf_vulnerability_iterator_next(it);
		xmlNode *vuln_node = xmlNewTextChild(root_node, NULL, BAD_CAST "Vulnerability", NULL);

		cvrf_element_to_dom("Title", cvrf_vulnerability_get_vulnerability_title(vuln), vuln_node);
		cvrf_element_to_dom("ID", cvrf_vulnerability_get_vulnerability_id(vuln), vuln_node);
		cvrf_element_to_dom("DiscoveryDate", cvrf_vulnerability_get_discovery_date(vuln), vuln_node);
		cvrf_element_to_dom("ReleaseDate", cvrf_vulnerability_get_release_date(vuln), vuln_node);
		cvrf_element_to_dom("CVE", cvrf_vulnerability_get_cve_id(vuln), vuln_node);
		cvrf_element_to_dom("CWE", cvrf_vulnerability_get_cwe_id(vuln), vuln_node);

		xmlNode *statuses_node = xmlNewTextChild(vuln_node, NULL, BAD_CAST "ProductStatuses", NULL);
		struct oscap_string_iterator *product_ids = cvrf_model_eval_get_product_ids(eval);
		while (oscap_string_iterator_has_more(product_ids)) {
			const char *product_id = oscap_string_iterator_next(product_ids);

			if (cvrf_product_vulnerability_fixed(vuln, product_id)) {

			}
			else {

				vulnerable = true;
			}
		}

		if (vulnerable && cvrf_vulnerability_get_remediation_count(vuln) > 0) {
			xmlNode *remediations_node = xmlNewTextChild(vuln_node, NULL, BAD_CAST "Remediations", NULL);
			struct cvrf_remediation_iterator *remediations = cvrf_vulnerability_get_remediations(vuln);
			while (cvrf_remediation_iterator_has_more(remediations)) {
				xmlNode *remediation_node = xmlNewTextChild(remediations_node, NULL, BAD_CAST "Remediation", NULL);
				cvrf_remediation_to_dom(cvrf_remediation_iterator_next(remediations), remediation_node);
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
	cvrf_model_eval_free(eval);
}


static const char *get_rpm_name_from_cvrf_product_id(struct cvrf_model_eval *eval, const char *product_id) {

	const char *rpm_name = NULL;
	struct cvrf_model *model = cvrf_eval_get_model(eval);
	struct cvrf_product_tree *tree = cvrf_model_get_product_tree(model);
	struct cvrf_branch *branch;
	struct cvrf_product_name *full_name;

	struct oscap_iterator *branches = cvrf_product_tree_get_branches(tree);
	while (oscap_iterator_has_more(branches)) {
		branch = oscap_iterator_next(branches);

		if (cvrf_branch_get_branch_type(branch) == CVRF_BRANCH_PRODUCT_VERSION) {
			full_name = cvrf_branch_get_cvrf_product_name(branch);

			if (oscap_str_endswith(product_id, cvrf_product_name_get_product_id(full_name))) {
				rpm_name = cvrf_product_name_get_cpe(full_name);
				break;
			}
		}

	}
	oscap_iterator_free(branches);
	return rpm_name;
}

static struct cvrf_rpm_attributes *parse_rpm_name_into_components(struct cvrf_model_eval *eval, const char *product_id) {

	struct cvrf_rpm_attributes *attributes = cvrf_rpm_attributes_new();
	const char *rpm_name = get_rpm_name_from_cvrf_product_id(eval, product_id);
	attributes->full_package_name = strdup(rpm_name);

	struct oscap_string *rpm_name_suffix = oscap_string_new();
	struct oscap_string *rpm_release = oscap_string_new();
	struct oscap_string *evr_format = oscap_string_new();
	int dashes_size = 1;
	int periods_size = 1;

	char *rpm_name_dup = strdup(rpm_name);
	char **split_by_dashes = oscap_split(rpm_name_dup, "-");
	while (split_by_dashes[dashes_size-1] != NULL)
		dashes_size++;
	for (int index = 0; index < dashes_size-3; index++) {
		oscap_string_append_string(rpm_name_suffix, split_by_dashes[index]);
		if (index < dashes_size-4)
			oscap_string_append_char(rpm_name_suffix, '-');
	}
	attributes->rpm_name = strdup(oscap_string_get_cstr(rpm_name_suffix));
	attributes->rpm_version = strdup(split_by_dashes[dashes_size-3]);

	char **split_by_periods = oscap_split(split_by_dashes[dashes_size-2], ".");
	while (split_by_periods[periods_size-1] != NULL)
		periods_size++;
	for (int index = 0; index < periods_size-3; index++) {
		oscap_string_append_string(rpm_release, split_by_periods[index]);
		if (index < periods_size-4)
			oscap_string_append_char(rpm_release, '.');
	}
	attributes->rpm_release = strdup(oscap_string_get_cstr(rpm_release));
	attributes->rpm_architecture = strdup(split_by_periods[periods_size-3]);

	char *product_id_dup = strdup(product_id);
	char **split_by_colon = oscap_split(product_id_dup, ":");
	char *name_with_epoch = oscap_strdup(split_by_colon[1]);
	int name_length = strlen(attributes->rpm_name);

	if (name_length == strlen(name_with_epoch)) {
		oscap_string_append_char(evr_format, '0');
	} else {
		for (int epoch_index = name_length+1; epoch_index < strlen(name_with_epoch); epoch_index++)
			oscap_string_append_char(evr_format, name_with_epoch[epoch_index]);
	}
	oscap_string_append_char(evr_format, ':');
	oscap_string_append_string(evr_format, attributes->rpm_version);
	oscap_string_append_char(evr_format, '-');
	oscap_string_append_string(evr_format, attributes->rpm_release);

	attributes->evr_format = strdup(oscap_string_get_cstr(evr_format));


	oscap_free(rpm_name_dup);
	oscap_free(product_id_dup);
	oscap_string_free(evr_format);
	oscap_string_free(rpm_name_suffix);
	oscap_string_free(rpm_release);
	return attributes;
}

bool cvrf_product_vulnerability_fixed(struct cvrf_vulnerability *vuln, char *product) {

	struct cvrf_product_status_iterator *it = cvrf_vulnerability_get_cvrf_product_statuses(vuln);
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
			oscap_string_append_string(string, "oval:org.open-scap.cpe.unix:obj:");
	} else if (!strcmp(type, "state")) {
			oscap_string_append_string(string, "oval:org.open-scap.cpe.unix:ste:");
	} else if (!strcmp(type, "test")) {
			oscap_string_append_string(string, "oval:org.open-scap.cpe.wrlinux:tst:");
	}

	oscap_string_append_string(string, number_as_string);
	char *oval_id = strdup(oscap_string_get_cstr(string));
	oscap_string_free(string);
	oscap_free(number_as_string);

	return oval_id;
}

static struct oval_test *get_new_rpminfo_test_for_cvrf(struct oval_definition_model *def_model, int testNo) {

	struct oval_test *rpm_test = oval_test_new(def_model, get_oval_id_string("test", testNo));
	oval_test_set_subtype(rpm_test,OVAL_LINUX_RPM_INFO);
	oval_test_set_version(rpm_test, 1);
	oval_test_set_check(rpm_test, OVAL_CHECK_AT_LEAST_ONE);
	oval_test_set_existence(rpm_test, OVAL_AT_LEAST_ONE_EXISTS);

	return rpm_test;

}

static struct oval_object *get_new_oval_object_for_cvrf(struct oval_definition_model *def_model,
		struct cvrf_rpm_attributes *attributes, int objectNo) {

	const char *object_id;
	object_id = get_oval_id_string("object", objectNo);

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

	const char *state_id;
	state_id = get_oval_id_string("state", stateNo);

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


int cvrf_model_eval_construct_definition_model(struct cvrf_model_eval *eval) {

	struct oval_definition_model *def_model = eval->def_model;
	struct oscap_string_iterator *product_ids = cvrf_model_eval_get_product_ids(eval);
	const char *product_id;
	int index = 1;

	struct oval_definition *definition = oval_definition_model_get_new_definition(def_model, "oval:org.open-scap.cpe.wrlinux:def:1");
	oval_definition_set_version(definition, 1);
	oval_definition_set_title(definition, "CVRF RPM Vulnerability Test");
	struct oval_criteria_node *criteria = oval_criteria_node_new(def_model, OVAL_NODETYPE_CRITERIA);
	oval_definition_set_criteria(definition, criteria);

	while (oscap_string_iterator_has_more(product_ids)) {
		product_id = oscap_string_iterator_next(product_ids);
		struct cvrf_rpm_attributes *rpm_attr = parse_rpm_name_into_components(eval, product_id);

		struct oval_object *object = get_new_oval_object_for_cvrf(def_model, rpm_attr, index);
		struct oval_state *state = get_new_oval_state_for_cvrf(def_model, rpm_attr, index);

		struct oval_test *rpm_test = get_new_rpminfo_test_for_cvrf(def_model, index);
		oval_test_set_object(rpm_test, object);
		oval_test_add_state(rpm_test, state);

		struct oval_criteria_node *criterion = oval_criteria_node_new(def_model, OVAL_NODETYPE_CRITERION);
		oval_criteria_node_set_test(criterion, rpm_test);
		char *comment = oscap_sprintf("Check for vulnerability of package %s", rpm_attr->rpm_name);
		oval_criteria_node_set_comment(criterion, comment);
		oval_criteria_node_add_subnode(criteria, criterion);

		cvrf_rpm_attributes_free(rpm_attr);
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

