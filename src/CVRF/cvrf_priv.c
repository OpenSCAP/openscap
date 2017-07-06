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

#include "CPE/cpelang_priv.h"
#include "CVSS/cvss_priv.h"
#include "CVSS/public/cvss_score.h"

#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"

/***************************************************************************/

/**
 * Top-level structure of the CVRF hierarchy
 *
 */
struct cvrf_model {
	struct product_tree tree;
	struct oscap_list *vulnerabilities;	/* 1-n */
};
OSCAP_IGETINS_GEN(cvrf_vulnerability, cvrf_model, vulnerabilities, vulnerability)
OSCAP_ITERATOR_REMOVE_F(cvrf_vulnerability)

/***************************************************************************
 * Product tree offshoot of main CVRF model
 */
struct product_tree {
	struct product_name *full_name;
	struct oscap_list *branches;
};
OSCAP_IGETINS_GEN(cvrf_branch, product_tree, branches, branch)
OSCAP_ITERATOR_REMOVE_F(cvrf_branch)

struct cvrf_branch {
	char *branch_type;
	char *branch_name;
	struct product_name *full_name;
	struct oscap_list *subbranches;
};
OSCAP_ACCESSOR_STRING(cvrf_branch, branch_type)
OSCAP_ACCESSOR_STRING(cvrf_branch, branch_name)

struct product_name {
	char *product_id;
	char *cpe;
};
OSCAP_ACCESSOR_STRING(product_name, product_id)
OSCAP_ACCESSOR_STRING(product_name, cpe)


/***************************************************************************
 * Vulnerability offshoot of main CVRF model
 *
 */
struct cvrf_vulnerability {
	int ordinal;
	char *vulnerability_title;
	char *cve_id;
	char *cwe_id;

	//Product Status
	char *product_status;
	struct oscap_list *product_ids;

};
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, vulnerability_title)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, cve_id)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, cwe_id)
OSCAP_ACCESSOR_STRING(cvrf_vulnerability, product_status)
OSCAP_IGETINS_GEN(oscap_string, cvrf_vulnerability, product_ids, product_id)
OSCAP_ITERATOR_REMOVE_F(oscap_string)



/***************************************************************************
 * Parsing functions
 */

struct cvrf_model *cvrf_model_parse_xml(const char *file) {
	struct cvrf_model cvrf = NULL;

	struct oscap_source *source = oscap_source_new_from_file(file);
	struct xmlTextReaderPtr reader = oscap_source_get_xmlTextReader(source);

	return cvrf;
}

void cvrf_model_export_xml(struct cvrf_model *cvrf, const char *file) {

}



/***************************************************************************
 * Free functions
 */

void cvrf_model_free(struct cvrf_model *cvrf) {

	if (cvrf == NULL)
		return;

	oscap_list_free(cvrf->vulnerabilities, (oscap_destruct_func) cvrf_vulnerability_free);
	product_tree_free(cvrf->tree);
	oscap_free(cvrf);
}

void product_tree_free(struct product_tree *tree) {

	if (tree == NULL)
		return;

	oscap_list_free(tree->branches, (oscap_destruct_func) cvrf_branch_free);
	product_name_free(tree->full_name);
	oscap_free(tree);
}

void cvrf_branch_free(struct cvrf_branch *branch) {

	if (branch == NULL)
		return;

	oscap_free(branch->branch_type);
	oscap_free(branch->branch_name);
	product_name_free(branch->full_name);
	oscap_list_free(branch->subbranches, (oscap_destruct_func) cvrf_branch_free);
	oscap_free(branch);
}

void product_name_free(struct product_name *full_name) {
	if (full_name == NULL)
		return;

	oscap_free(full_name->cpe);
	oscap_free(full_name->product_id);
	oscap_free(full_name);
}

void cvrf_vulnerability_free(struct cvrf_vulnerability *vulnerability) {

	if (vulnerability == NULL)
		return;

	oscap_free(vulnerability->vulnerability_title);
	oscap_free(vulnerability->cve_id);
	oscap_free(vulnerability->cwe_id);
	oscap_free(vulnerability->product_status);
	oscap_list_free(vulnerability->product_ids, (oscap_destruct_func) oscap_string_free);
	oscap_free(vulnerability);
}


