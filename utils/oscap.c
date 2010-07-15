/*
 * OVAL simple application for evaluating OVAL definition file.
 * The output is system characteristic xml file and oval results xml file.
 * Program iterate through tests in every definition and evaluate objects
 * of these tests.
 *
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
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
 *      Maros Barabas  <mbarabas@redhat.com>

 * Help:
 *      compile: $gcc oscap-scan.c -lcurl -lopenscap -I/usr/include/openscap/
 *               or
 *               $gcc -I../src/OVAL/public/ -I../src/common/public/ oscap-scan.c -L../src/.libs/ -lcurl -lopenscap
 *      run:     $./oscap-scan -v definition_file.xml --result-file=results.xml --syschar-file=syschars.xml
 */

/* Standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

/* Header files for curl */
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

/* Head files for lstat and errno */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

/* OVAL & OSCAP common */
#include <oval_probe.h>
#include <oval_agent_api.h>
#include <oval_results.h>
#include <oval_variables.h>
#include <error.h>
#include <text.h>
#include <reporter.h>

/* XCCDF */
#include <xccdf.h>
#include <xccdf_policy.h>

/* CVSS */
#include <cvss.h>

int VERBOSE = 1;

typedef enum {
	OSCAP_STD_UNKNOWN,
	OSCAP_STD_XCCDF,
	OSCAP_STD_OVAL,
	OSCAP_STD_CVSS
} oscap_standard_t;

typedef enum {
	OSCAP_OP_UNKNOWN,
	OSCAP_OP_COLLECT,
	OSCAP_OP_EVAL,
	OSCAP_OP_VALIDATE_XML,
	OSCAP_OP_BASE,
	OSCAP_OP_TEMP,
	OSCAP_OP_ENV
} oscap_operation_t;

struct oscap_action {
	oscap_standard_t std;
	oscap_operation_t op;
	oscap_document_type_t doctype;
	char *f_xccdf;
	char *f_oval;
	char *f_results;
	char *url_xccdf;
	char *url_oval;
	char *profile;
	char *file_version;
	struct cvss_metrics *cvss_metrics;
};

/**
 * User defined structure that is passed to OVAL callback in OVAL evaluation
 * Each integer variable represent one statistic entry for definition results
 * It is used to evaluate final return value of oscap-scan
 */
struct oval_usr {

	int result_false;
	int result_true;
	int result_invalid;
	int result_unknown;
	int result_neval;
	int result_napp;
};

struct cvss_metrics {
	cvss_access_vector_t ave;
	cvss_access_complexity_t ace;
	cvss_authentication_t aue;
	cvss_conf_impact_t cie;
	cvss_integ_impact_t iie;
	cvss_avail_impact_t aie;
	cvss_exploitability_t exe;
	cvss_remediation_level_t rle;
	cvss_report_confidence_t rce;
	cvss_collateral_damage_potential_t cde;
	cvss_target_distribution_t tde;
	cvss_conf_req_t cre;
	cvss_integ_req_t ire;
	cvss_avail_req_t are;
	double base;
};

static struct oscap_action *oscap_action_new()
{
	struct oscap_action *action;
	action = malloc(sizeof(struct oscap_action));
	if (action == NULL)
		return NULL;

	action->std = OSCAP_STD_UNKNOWN;
	action->op = OSCAP_OP_UNKNOWN;
	action->f_xccdf = NULL;
	action->f_oval = NULL;
	action->f_results = NULL;
	action->url_xccdf = NULL;
	action->url_oval = NULL;
	action->profile = NULL;
	action->cvss_metrics = NULL;
	action->file_version = NULL;

	return action;
}

static void oscap_action_free(struct oscap_action *action)
{
	if (action == NULL)
		return;

	if (action->f_xccdf)
		free(action->f_xccdf);
	if (action->f_oval)
		free(action->f_oval);
	action->f_results = NULL;
	action->url_oval = NULL;
	action->url_xccdf = NULL;
	if (action->cvss_metrics != NULL) {
		free(action->cvss_metrics);
		action->cvss_metrics = NULL;
	}
	//free(action);
}

/**
 * Function print usage of this program to specified output
 * @param pname name of program, standard usage argv[0]
 * @param out output stream for fprintf function, standard usage stdout or stderr
 */
static void print_usage(const char *pname, FILE * out)
{

	fprintf(out,
		"Usage: %s [general-options] module operation [operation-options-and-arguments]\n"
		"\n"
		"General options:\n"
		"   -h --help\r\t\t\t\t - show this help\n"
		"   -q --quiet\r\t\t\t\t - quiet mode\n"
		"   -V --version\r\t\t\t\t - print info about supported SCAP versions\n"
		"\n" "Module specific help\n" " %s [oval|xccdf|cvss] --help\n", pname, pname);
}

static void print_cvss_usage(const char *pname, FILE * out, char *msg)
{
	fprintf(out,
		"Usage: %s [general-options] cvss [operation|--help] metrics [--base <num>]\n"
		"\n"
		"\n"
		"Operation:\n"
		"   base\r\t\t\t\t - Calculate base score from metrics.\n"
		"   temporal \r\t\t\t\t - Calculate temporal score from metrics and base score.\n"
		"   environmental\r\t\t\t\t - Calculate environmental score from metrics.\n"
		"\n"
		"Metrics:\n"
		"   --AV=[local|adjacent-network|network]\r\t\t\t\t\t - Access Vector\n"
		"   --AC=[low|medium|high]\r\t\t\t\t - Access Complexity\n"
		"   --AU=[none|single|multiple]\r\t\t\t\t - Authentication\n"
		"   --CI=[none|partial|complete]\r\t\t\t\t - Confidentiality  Impact\n"
		"   --II=[none|partial|complete]\r\t\t\t\t - Integrity Impact\n"
		"   --AI=[none|partial|complete]\r\t\t\t\t - Availability Impact\n"
		"   --EX=[unproven|proof-of-concept|functional|high|not-defined]\r\t\t\t\t\t\t\t\t - Exploitability\n"
		"   --RL=[official-fix|temporary-fix|workaround|unavailable|not-defined]\r\t\t\t\t\t\t\t\t\t - Remediation Level\n"
		"   --RC=[unconfirmed|uncorrporated|confirmed|not-defined]\r\t\t\t\t\t\t\t\t - Report Confidence\n"
		"   --CD=[none|low|low-medium|medium-high|high|not-defined]\r\t\t\t\t\t\t\t\t - Collateral Damage Potential\n"
		"   --TD=[none|low|medium|high|not-defined]\r\t\t\t\t\t\t - Target Distribution\n"
		"   --CR=[low|medium|high|not-defined]\r\t\t\t\t\t - Confidentiality Requirement\n"
		"   --IR=[low|medium|high|not-defined]\r\t\t\t\t\t - Integrity Requirement\n"
		"   --AR=[low|medium|high|not-defined]\r\t\t\t\t\t - Availability Requirement\n"
		, pname);
	if (msg != NULL)
		fprintf(out, "\n%s\n", msg);
}


static void print_xccdf_usage(const char *pname, FILE * out, char *msg)
{
	fprintf(out,
		"Usage: %s [general-options] xccdf command [command-options] OVAL-DEFINITIONS-FILE XCCDF-FILE \n"
		"(Specify the --help global option for a list of other help options)\n"
		"\n"
		"OVAL-DEFINITIONS-FILE is the OVAL XML file specified either by the full path to the xml file "
		"or an URL from which to download it.\n"
		"XCCDF-FILE is the XCCDF XML file specified either by the full path to the xml file "
		"or an URL from which to download it.\n"
		"\n"
		"Commands:\n"
		"   eval\r\t\t\t\t - Perform evaluation driven by XCCDF file and use OVAL as checking engine.\n"
		"   validate-xml\r\t\t\t\t - validate XCCDF XML content.\n"
		"\n"
		"Command options:\n"
		"   -h --help\r\t\t\t\t - show this help\n"
		"   --result-file <file>\r\t\t\t\t - Write XCCDF Results into file.\n"
		"   --profile <name>\r\t\t\t\t - The name of Profile to be evaluated.\n", pname);
	if (msg != NULL)
		fprintf(out, "\n%s\n", msg);
}

static void print_oval_usage(const char *pname, FILE * out, char *msg)
{
	fprintf(out,
		"Usage: %s [general-options] oval command [command-options] OVAL-DEFINITIONS-FILE \n"
		"(Specify the --help global option for a list of other help options)\n"
		"\n"
		"OVAL-DEFINITIONS-FILE is the OVAL XML file specified either by the full path to the xml file "
		"or an URL from which to download it.\n"
		"\n"
		"Commands:\n"
		"   collect\r\t\t\t\t - Probe the system and gather system characteristics for objects in OVAL Definition file.\n"
		"   eval\r\t\t\t\t - Probe the system and evaluate all definitions from OVAL Definition file\n"
		"   validate-xml\r\t\t\t\t - validate OVAL XML content.\n"
		"\n"
		"Command options:\n"
		"   -h --help\r\t\t\t\t - show this help\n"
		"   --result-file <file>\r\t\t\t\t - Write OVAL Results into file.\n", pname);
	if (msg != NULL)
		fprintf(out, "\n%s\n", msg);
}

static void print_versions()
{

	fprintf(stdout,
		"OSCAP util (oscap) 0.5.12\n" "Copyright 2009,2010 Red Hat Inc., Durham, North Carolina.\n" "\n");
	fprintf(stdout, "OVAL Version: \r\t\t%s\n", oval_definition_model_supported());
#ifdef ENABLE_XCCDF
	fprintf(stdout, "XCCDF Version: \r\t\t%s\n", xccdf_benchmark_supported());
#endif
	fprintf(stdout, "CVSS Version: \r\t\t%s\n", cvss_model_supported());
	/*fprintf(stdout,
	   "CCE Version: \r\t\t%s\n"
	   , cce_supported());
	   fprintf(stdout,
	   "CPE Version: \r\t\t%s (name) %s (dict) %s (lang)\n"
	   , cpe_name_supported(), cpe_dict_model_supported(), cpe_lang_model_supported());
	   fprintf(stdout,
	   "CVE Version: \r\t\t%d\n"
	   , cve_model_supported()); */
}

/**
 * Function with CURL support to download file from remote storage. 
 * Function takes url (which can be path to local file), test if we can stat the file. If 
 * the file is local just returns the path otherwise starts downloading process and 
 * returns name of local stored file.
 * Tested on HTTP, FTP protocols.
 * @param url URL or PATH to file
 * @return path to local file
 */
static char *app_curl_download(char *url)
{

	struct stat buf;
	/* Is the file local ? */
	if (lstat(url, &buf) == 0)
		return strdup(url);

	/* Remote file will be stored in this xml */
	char *outfile = strdup("definition_file.xml");

	CURL *curl;
	FILE *fp;
	CURLcode res;
	/* Initialize CURL for download remote file */
	curl = curl_easy_init();
	if (!curl)
		return NULL;

	fp = fopen(outfile, "wb");
	/* Set options for download file to *fp* from *url* */
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	res = curl_easy_perform(curl);
	if (res != 0) {
		curl_easy_cleanup(curl);
		fclose(fp);
		return NULL;
	}

	curl_easy_cleanup(curl);
	fclose(fp);

	return outfile;
}

#ifdef ENABLE_XCCDF
/**
 * Callback for XCCDF evaluation. Callback is called after each XCCDF Rule evaluation
 * @param msg OSCAP Reporter message
 * @param arg User defined data structure
 */
static void callback(const struct oscap_reporter_message *msg, void *arg)
{
	if (VERBOSE >= 0)
		printf("Rule \"%s\" result: %s\n",
		       oscap_reporter_message_get_user1str(msg),
		       xccdf_test_result_type_get_text((xccdf_test_result_type_t)
						       oscap_reporter_message_get_user2num(msg)));
	//return 0;
}

/**
 * XCCDF Processing fucntion
 * @param action OSCAP Action structure 
 * @param sess OVAL Agent Session
 */
static int app_evaluate_xccdf(const struct oscap_action *action)
{

	struct xccdf_policy_iterator *policy_it = NULL;
	struct xccdf_policy *policy = NULL;
	struct xccdf_benchmark *benchmark = NULL;
	struct xccdf_policy_model *policy_model = NULL;
	struct oval_definition_model *def_model = NULL;
	struct oval_agent_session *sess = NULL;

	def_model = oval_definition_model_import(action->f_oval);
	sess = oval_agent_new_session(def_model);

	/* Load XCCDF model and XCCDF Policy model */
	benchmark = xccdf_benchmark_import(action->f_xccdf);
	policy_model = xccdf_policy_model_new(benchmark);

	/* Get the first policy, just for prototype - if there is no Policy 
	 * report error and return -1 */
	if (action->profile != NULL) {
		policy = xccdf_policy_model_get_policy_by_id(policy_model, action->profile);
	} else {
		policy_it = xccdf_policy_model_get_policies(policy_model);
		if (xccdf_policy_iterator_has_more(policy_it)) {
			policy = xccdf_policy_iterator_next(policy_it);
		}
		xccdf_policy_iterator_free(policy_it);
	}

	if (policy == NULL) {
		fprintf(stderr, "No Policy to evaluate. \n");
		return -1;
	}

	/* Register callback */
	xccdf_policy_model_register_output_callback(policy_model, (oscap_reporter) callback, NULL);
	xccdf_policy_model_register_engine_oval(policy_model, sess);

	/* Perform evaluation */
	struct xccdf_result *ritem = xccdf_policy_evaluate(policy);

	/* Write results into XCCDF Test Result model */
	xccdf_result_set_benchmark_uri(ritem, action->url_xccdf);
	struct oscap_text *title = oscap_text_new();
	oscap_text_set_text(title, "OSCAP Scan Result");
	xccdf_result_add_title(ritem, title);
	if (policy != NULL) {
		const char *id = xccdf_profile_get_id(xccdf_policy_get_profile(policy));
		if (id != NULL)
			xccdf_result_set_profile(ritem, id);
	}
	oval_agent_export_sysinfo_to_xccdf_result(sess, ritem);

	struct xccdf_model_iterator *model_it = xccdf_benchmark_get_models(benchmark);
	while (xccdf_model_iterator_has_more(model_it)) {
		struct xccdf_model *model = xccdf_model_iterator_next(model_it);
		struct xccdf_score *score = xccdf_policy_get_score(policy, ritem, xccdf_model_get_system(model));
		xccdf_result_add_score(ritem, score);
	}
	xccdf_model_iterator_free(model_it);

	/* Export results */
	if (action->f_results != NULL)
		xccdf_result_export(ritem, action->f_results);

	/* Get the result from TestResult model and decide if end with error or with correct return code */
	int retval = 0;
	struct xccdf_rule_result_iterator *res_it = xccdf_result_get_rule_results(ritem);
	while (xccdf_rule_result_iterator_has_more(res_it)) {
		struct xccdf_rule_result *res = xccdf_rule_result_iterator_next(res_it);
		xccdf_test_result_type_t result = xccdf_rule_result_get_result(res);
		if ((result == XCCDF_RESULT_FAIL) || (result == XCCDF_RESULT_UNKNOWN))
			retval = 2;
	}
	xccdf_rule_result_iterator_free(res_it);

	/* Clear & End */
	oval_agent_destroy_session(sess);
	oval_definition_model_free(def_model);
	xccdf_policy_model_free(policy_model);

	return retval;
}
#endif

/**
 * OVAL Callback function that is passed to OVAL evaluation proccess
 * Function makes statistic of definition results
 */
static int app_oval_callback(const struct oscap_reporter_message *msg, void *arg)
{

	if (VERBOSE > 0)
		printf("Evalutated definition %s: %s\n",
		       oscap_reporter_message_get_user1str(msg),
		       oval_result_get_text(oscap_reporter_message_get_user2num(msg)));
	switch ((oval_result_t) oscap_reporter_message_get_user2num(msg)) {
	case OVAL_RESULT_TRUE:
		((struct oval_usr *)arg)->result_true++;
		break;
	case OVAL_RESULT_FALSE:
		((struct oval_usr *)arg)->result_false++;
		break;
	case OVAL_RESULT_UNKNOWN:
		((struct oval_usr *)arg)->result_unknown++;
		break;
	case OVAL_RESULT_NOT_EVALUATED:
		((struct oval_usr *)arg)->result_neval++;
		break;
	case OVAL_RESULT_NOT_APPLICABLE:
		((struct oval_usr *)arg)->result_napp++;
		break;
	default:
		break;
	}

	return 0;
}

static int app_collect_oval(const struct oscap_action *action)
{
	int ret;

	/* import definitions */
	struct oval_definition_model *def_model = oval_definition_model_import(action->f_oval);

	/* create empty syschar model */
	struct oval_syschar_model *sys_model = oval_syschar_model_new(def_model);

	/* create probe session */
	struct oval_probe_session *pb_sess = oval_probe_session_new(sys_model);

	/* query sysinfo */
	ret = oval_probe_session_query_sysinfo(pb_sess);
	if (ret != 0) {
		oval_probe_session_destroy(pb_sess);
		oval_syschar_model_free(sys_model);
		oval_definition_model_free(def_model);
		return 1;
	}
	/* query objects */
	ret = oval_probe_session_query_objects(pb_sess);
	if (ret != 0) {
		oval_probe_session_destroy(pb_sess);
		oval_syschar_model_free(sys_model);
		oval_definition_model_free(def_model);
		return 1;
	}

	/* report */
	oval_syschar_model_export(sys_model, "/dev/stdout");

	/* destroy */
	oval_probe_session_destroy(pb_sess);
	oval_syschar_model_free(sys_model);
	oval_definition_model_free(def_model);

	return 0;
}

/**
 * Function that evaluate OVAL content (without XCCDF).
 */
static int app_evaluate_oval(const struct oscap_action *action)
{

	struct oval_results_model *res_model = NULL;
	struct oval_usr *usr = NULL;
	int ret = 0;

	struct oval_definition_model *def_model = oval_definition_model_import(action->f_oval);
	oval_agent_session_t *sess = oval_agent_new_session(def_model);

	/* Import OVAL definition file */
	if (oscap_err()) {
		if (VERBOSE >= 0)
			fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
		return 1;
	}

	res_model = oval_agent_get_results_model(sess);

	/* Init usr structure */
	usr = malloc(sizeof(struct oval_usr));
	usr->result_false = 0;
	usr->result_true = 0;
	usr->result_invalid = 0;
	usr->result_unknown = 0;
	usr->result_neval = 0;
	usr->result_napp = 0;

	/* Evaluation */
	ret = oval_agent_eval_system(sess, app_oval_callback, usr);

	if (VERBOSE >= 0)
		printf("Evaluation: All done.\n");

	if (ret == -1) {
		if ((oscap_err()) && (VERBOSE >= 0))
			if (VERBOSE >= 0)
				fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
		return 1;
	}
	if (VERBOSE >= 0) {
		fprintf(stdout, "====== RESULTS ======\n");
		fprintf(stdout, "TRUE:          \r\t\t %d\n", usr->result_true);
		fprintf(stdout, "FALSE:         \r\t\t %d\n", usr->result_false);
		fprintf(stdout, "UNKNOWN:       \r\t\t %d\n", usr->result_unknown);
		fprintf(stdout, "NOT EVALUATED: \r\t\t %d\n", usr->result_neval);
		fprintf(stdout, "NOT APPLICABLE:\r\t\t %d\n", usr->result_napp);
	}

	/*=============== PRINT RESULTS =====================*/
	if (action->f_results != NULL) {
		// set up directives 
		struct oval_result_directives *res_direct = oval_result_directives_new(res_model);
		oval_result_directives_set_reported(res_direct, OVAL_RESULT_TRUE | OVAL_RESULT_FALSE |
						    OVAL_RESULT_UNKNOWN | OVAL_RESULT_NOT_EVALUATED |
						    OVAL_RESULT_ERROR | OVAL_RESULT_NOT_APPLICABLE, true);

		oval_result_directives_set_content(res_direct, OVAL_RESULT_FALSE, OVAL_DIRECTIVE_CONTENT_FULL);
		oval_result_directives_set_content(res_direct, OVAL_RESULT_TRUE, OVAL_DIRECTIVE_CONTENT_FULL);

		// Export result model to XML 
		oval_results_model_export(res_model, res_direct, action->f_results);
		oval_result_directives_free(res_direct);
	}
	/* Clear */
	oval_agent_destroy_session(sess);
	oval_definition_model_free(def_model);

	if (usr != NULL) {
		if ((usr->result_false == 0) && (usr->result_unknown == 0)) {
			free(usr);
			return 0;
		} else {
			free(usr);
			return 2;
		}
	} else
		return ret;
}

static int app_validate_xml(const struct oscap_action *action)
{
	const char *xml_file = action->f_oval;
	if (!xml_file)
		xml_file = action->f_xccdf;
	if (!xml_file)
		return 2;

	if (!oscap_validate_document
	    (xml_file, action->doctype, action->file_version, (VERBOSE >= 0 ? oscap_reporter_fd : NULL), stdout)) {
		if (oscap_err()) {
			fprintf(stderr, "ERROR: %s\n", oscap_err_desc());
			return 2;
		}
		return 1;
	}
	return 0;
}

static int getopt_cvss(int argc, char **argv, struct oscap_action *action)
{
	/* Usage: oscap cvss command metrics */
	if (action == NULL) {
		return -1;
	}

	action->std = OSCAP_STD_CVSS;
	action->cvss_metrics = malloc(sizeof(struct cvss_metrics));
	action->cvss_metrics->exe = EX_NOT_DEFINED;
	action->cvss_metrics->rle = RL_NOT_DEFINED;
	action->cvss_metrics->rce = RC_NOT_DEFINED;
	action->cvss_metrics->cde = CD_NOT_DEFINED;
	action->cvss_metrics->tde = TD_NOT_DEFINED;
	action->cvss_metrics->cre = CR_NOT_DEFINED;
	action->cvss_metrics->ire = IR_NOT_DEFINED;
	action->cvss_metrics->are = AR_NOT_DEFINED;

	/* Command */
	optind++;
	if (optind >= argc) {
		print_cvss_usage("oscap", stderr, "Error: Bad number of parameters !");
		return -1;
	}
	if (!strcmp(argv[optind], "base"))
		action->op = OSCAP_OP_BASE;
	else if (!strcmp(argv[optind], "temporal"))
		action->op = OSCAP_OP_TEMP;
	else if (!strcmp(argv[optind], "environmental"))
		action->op = OSCAP_OP_ENV;
	else
		optind--;	/* oscap xccdf --help */

	/* Command-options */
	struct option long_options[] = {
		{"help", 0, 0, 'h'},
		{"AV", 1, 0, 0},
		{"AC", 1, 0, 1},
		{"AU", 1, 0, 2},
		{"CI", 1, 0, 3},
		{"II", 1, 0, 4},
		{"AI", 1, 0, 5},
		{"EX", 1, 0, 6},
		{"RL", 1, 0, 7},
		{"RC", 1, 0, 8},
		{"CD", 1, 0, 9},
		{"TD", 1, 0, 10},
		{"CR", 1, 0, 11},
		{"IR", 1, 0, 12},
		{"AR", 1, 0, 13},
		{"base", 1, 0, 14},
		{0, 0, 0, 0}
	};

	int c;
	int getopt_index = 0;	/* index is not neccesary because we know the option from "val" */
	int ave=0, ace=0, aue=0, cie=0, iie=0, aie=0, base=0;
	optind++;		/* Increment global variable pointeing to argv array to get next opt */
	while ((c = getopt_long(argc, argv, "+h0:1:2:3:4:5:6:7:8:9:10:11:12:13:14:", long_options, &getopt_index)) != -1) {
		switch (c) {
		case 'h':	/* CVSS HELP */
			print_cvss_usage("oscap", stdout, NULL);
			return 0;
			break;
		case 0:
			ave++;
			if (!strcmp(optarg, "local")) action->cvss_metrics->ave = AV_LOCAL;
			else if (!strcmp(optarg, "adjacent-network")) action->cvss_metrics->ave = AV_ADJACENT_NETWORK;
			else if (!strcmp(optarg, "network")) action->cvss_metrics->ave = AV_NETWORK;
			else return -1;
			break;
		case 1:
		        ace++;
			if (!strcmp(optarg, "low")) action->cvss_metrics->ace = AC_LOW;
			else if (!strcmp(optarg, "medium")) action->cvss_metrics->ace = AC_MEDIUM;
			else if (!strcmp(optarg, "high")) action->cvss_metrics->ace = AC_HIGH;
			else return -1;
			break;
		case 2:
			aue++;
			if (!strcmp(optarg, "none")) action->cvss_metrics->aue = AU_NONE;
			else if (!strcmp(optarg, "single")) action->cvss_metrics->aue = AU_SINGLE_INSTANCE;
			else if (!strcmp(optarg, "multiple")) action->cvss_metrics->aue = AU_MULTIPLE_INSTANCE;
			else return -1;
			break;
		case 3:
			cie++;
			if (!strcmp(optarg, "none")) action->cvss_metrics->cie = CI_NONE;
			else if (!strcmp(optarg, "partial")) action->cvss_metrics->cie = CI_PARTIAL;
			else if (!strcmp(optarg, "complete")) action->cvss_metrics->cie = CI_COMPLETE;
			else return -1;
			break;
		case 4:
			iie++;
			if (!strcmp(optarg, "none")) action->cvss_metrics->iie = II_NONE;
			else if (!strcmp(optarg, "partial")) action->cvss_metrics->iie = II_PARTIAL;
			else if (!strcmp(optarg, "complete")) action->cvss_metrics->iie = II_COMPLETE;
			else return -1;
			break;
		case 5:
			aie++;
			if (!strcmp(optarg, "none")) action->cvss_metrics->aie = II_NONE;
			else if (!strcmp(optarg, "partial")) action->cvss_metrics->aie = II_PARTIAL;
			else if (!strcmp(optarg, "complete")) action->cvss_metrics->aie = II_COMPLETE;
			else return -1;
			break;
		case 6:
			if (!strcmp(optarg, "unproven")) action->cvss_metrics->exe = EX_UNPROVEN;
			else if (!strcmp(optarg, "proof-of-concept")) action->cvss_metrics->exe = EX_PROOF_OF_CONCEPT;
			else if (!strcmp(optarg, "functional")) action->cvss_metrics->exe = EX_FUNCTIONAL;
			else if (!strcmp(optarg, "high")) action->cvss_metrics->exe = EX_HIGH;
			else if (!strcmp(optarg, "not-defined")) action->cvss_metrics->exe = EX_NOT_DEFINED;
			else return -1;
			break;
		case 7:
			if (!strcmp(optarg, "official-fix")) action->cvss_metrics->rle = RL_OFFICIAL_FIX;
			else if (!strcmp(optarg, "temporary-fix")) action->cvss_metrics->rle = RL_TEMPORARY_FIX;
			else if (!strcmp(optarg, "workaround")) action->cvss_metrics->rle = RL_WORKAROUND;
			else if (!strcmp(optarg, "unavailable")) action->cvss_metrics->rle = RL_UNAVAILABLE;
			else if (!strcmp(optarg, "not-defined")) action->cvss_metrics->rle = RL_NOT_DEFINED;
			else return -1;
			break;
		case 8:
			if (!strcmp(optarg, "unconfirmed")) action->cvss_metrics->rce = RC_UNCONFIRMED;
			else if (!strcmp(optarg, "uncorrporated")) action->cvss_metrics->rce = RC_UNCORROBORATED;
			else if (!strcmp(optarg, "confirmed")) action->cvss_metrics->rce = RC_CONFIRMED;
			else if (!strcmp(optarg, "not-defined")) action->cvss_metrics->rce = RC_NOT_DEFINED;
			else return -1;
			break;
		case 9:
			if (!strcmp(optarg, "none")) action->cvss_metrics->cde = CD_NONE;
			else if (!strcmp(optarg, "low")) action->cvss_metrics->cde = CD_LOW;
			else if (!strcmp(optarg, "low-medium")) action->cvss_metrics->cde = CD_LOW_MEDIUM;
			else if (!strcmp(optarg, "medium-high")) action->cvss_metrics->cde = CD_MEDIUM_HIGH;
			else if (!strcmp(optarg, "high")) action->cvss_metrics->cde = CD_HIGH;
			else if (!strcmp(optarg, "not-defined")) action->cvss_metrics->cde = CD_NOT_DEFINED;
			else return -1;
			break;
		case 10:
			if (!strcmp(optarg, "none")) action->cvss_metrics->tde = TD_NONE;
			else if (!strcmp(optarg, "low")) action->cvss_metrics->tde = TD_LOW;
			else if (!strcmp(optarg, "medium")) action->cvss_metrics->tde = TD_MEDIUM;
			else if (!strcmp(optarg, "high")) action->cvss_metrics->tde = TD_HIGH;
			else if (!strcmp(optarg, "not-defined")) action->cvss_metrics->tde = TD_NOT_DEFINED;
			else return -1;
			break;
		case 11:
			if (!strcmp(optarg, "low")) action->cvss_metrics->cre = CR_LOW;
			else if (!strcmp(optarg, "medium")) action->cvss_metrics->cre = CR_MEDIUM;
			else if (!strcmp(optarg, "high")) action->cvss_metrics->cre = CR_HIGH;
			else if (!strcmp(optarg, "not-defined")) action->cvss_metrics->cre = CR_NOT_DEFINED;
			else return -1;
			break;
		case 12:
			if (!strcmp(optarg, "low")) action->cvss_metrics->ire = IR_LOW;
			else if (!strcmp(optarg, "medium")) action->cvss_metrics->ire = IR_MEDIUM;
			else if (!strcmp(optarg, "high")) action->cvss_metrics->ire = IR_HIGH;
			else if (!strcmp(optarg, "not-defined")) action->cvss_metrics->ire = IR_NOT_DEFINED;
			else return -1;
			break;
		case 13:
			if (!strcmp(optarg, "low")) action->cvss_metrics->are = AR_LOW;
			else if (!strcmp(optarg, "medium")) action->cvss_metrics->are = AR_MEDIUM;
			else if (!strcmp(optarg, "high")) action->cvss_metrics->are = AR_HIGH;
			else if (!strcmp(optarg, "not-defined")) action->cvss_metrics->are = AR_NOT_DEFINED;
			else return -1;
			break;
		case 14:
			base++;
			action->cvss_metrics->base = atof(optarg);
			break;
		default:
			fprintf(stderr, "FOUND BAD OPTION %d :: %d :: %s\n", optind, optopt, argv[optind]);
			break;
		}
	}

	/* check parametres */
        if (action->op == OSCAP_OP_UNKNOWN) {
                print_cvss_usage("oscap", stderr, "Error: No operation specified");
                return -1;
        }

	if ( (action->op == OSCAP_OP_BASE || action->op == OSCAP_OP_ENV) &&
	     ( !ave || !ace || !aue || !cie || !iie || !aie) ) {
	        print_cvss_usage("oscap", stderr, "Error: Required metrics were not specified");
                return -1;
	}

	if ((action->op == OSCAP_OP_TEMP) && !base) {
	        print_cvss_usage("oscap", stderr, "Error: Base score was not specified");
                return -1;
	}

	return 0;
}

static int getopt_xccdf(int argc, char **argv, struct oscap_action *action)
{
	/* Usage: oscap xccdf command [command-options] */
	if (action == NULL) {
		/* TODO: Problem ? */
		return -1;
	}

	action->std = OSCAP_STD_XCCDF;
	action->doctype = OSCAP_DOCUMENT_XCCDF;

	/* Command */
	optind++;
	if (optind >= argc) {
		print_xccdf_usage("oscap", stderr, "Error: Bad number of parameters !");
		return -1;
	}
	if (!strcmp(argv[optind], "eval"))
		action->op = OSCAP_OP_EVAL;
	else if (!strcmp(argv[optind], "validate-xml")) {
		action->op = OSCAP_OP_VALIDATE_XML;
	} else {
		/* oscap xccdf --help */
		optind--;
	}

	/* Command-options */
	struct option long_options[] = {
		{"help", 0, 0, 'h'},
		{"result-file", 1, 0, 0},
		{"xccdf-profile", 1, 0, 1},
		{"file-version", 1, 0, 2},
		{0, 0, 0, 0}
	};

	int c;
	int getopt_index = 0;	/* index is not neccesary because we know the option from "val" */
	optind++;		/* Increment global variable pointeing to argv array to get next opt */
	while ((c = getopt_long(argc, argv, "+h012", long_options, &getopt_index)) != -1) {
		switch (c) {
		case 'h':	/* XCCDF HELP */
			print_xccdf_usage("oscap", stdout, NULL);
			return 0;
		case 0:	/* RESULT FILE */
			if (optarg == NULL)
				return -1;
			action->f_results = optarg;
			break;
		case 1:	/* RESULT FILE */
			if (optarg == NULL)
				return -1;
			action->profile = optarg;
			break;
		case 2:
			if (optarg == NULL)
				return -1;
			action->file_version = optarg;
			break;
		default:
			fprintf(stderr, "FOUND BAD OPTION %d :: %d :: %s\n", optind, optopt, argv[optind]);
			break;
		}
	}
	if (action->op == OSCAP_OP_UNKNOWN) {
		print_xccdf_usage("oscap", stderr,
				  "Error: No operation specified. Use \"oscap xccdf eval OVAL_FILE XCCDF_FILE\"");
		return -1;
	}

	if (action->op == OSCAP_OP_EVAL) {
		/* We should have XCCDF file here */
		if (optind + 1 >= argc) {
			/* TODO */
			print_xccdf_usage("oscap", stderr,
					  "Error: Bad number of parameters. OVAL file and XCCDF file need to be specified !");
			return -1;
		}
		action->url_oval = argv[optind];
		action->url_xccdf = argv[optind + 1];
	} else {
		if (optind >= argc) {
			print_xccdf_usage("oscap", stderr,
					  "Error: Bad number of parameters. XCCDF file need to be specified !");
			return -1;
		}
		action->url_xccdf = argv[optind];
	}

	return 0;
}

static int getopt_oval(int argc, char **argv, struct oscap_action *action)
{
	/* Usage: oscap oval command [command-options] */
	if (action == NULL) {
		/* TODO: Problem ? */
		return -1;
	}

	action->std = OSCAP_STD_OVAL;
	action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;

	/* Command */
	optind++;
	if (optind >= argc) {
		print_oval_usage("oscap", stderr, "Error: Bad number of parameters. Command and OVAL file required.");
		return -1;
	}
	if (!strcmp(argv[optind], "eval"))
		action->op = OSCAP_OP_EVAL;
	else if (!strcmp(argv[optind], "collect"))
		action->op = OSCAP_OP_COLLECT;
	else if (!strcmp(argv[optind], "validate-xml"))
		action->op = OSCAP_OP_VALIDATE_XML;
	else {
		/* oscap OVAL --help */
		optind--;
	}

	/* Command-options */
	struct option long_options[] = {
		{"help", 0, 0, 'h'},
		{"result-file", 1, 0, 0},
		{"file-version", 1, 0, 1},
		{"definitions", 0, 0, 2},
		{"syschar", 0, 0, 3},
		{"results", 0, 0, 4},
		{0, 0, 0, 0}
	};

	int c;
	int getopt_index = 0;	/* index is not neccesary because we know the option from "val" */
	optind++;		/* Increment global variable pointeing to argv array to get next opt */
	while ((c = getopt_long(argc, argv, "+h012", long_options, &getopt_index)) != -1) {
		switch (c) {
		case 'h':	/* XCCDF HELP */
			print_oval_usage("oscap", stdout, NULL);
			return 0;
		case 0:	/* RESULT FILE */
			if (optarg == NULL)
				return -1;
			action->f_results = optarg;
			break;
		case 1:
			if (optarg == NULL)
				return -1;
			action->file_version = optarg;
			break;
		case 2:
			action->doctype = OSCAP_DOCUMENT_OVAL_DEFINITIONS;
			break;
		case 3:
			action->doctype = OSCAP_DOCUMENT_OVAL_SYSCHAR;
			break;
		case 4:
			action->doctype = OSCAP_DOCUMENT_OVAL_RESULTS;
			break;
		default:
			fprintf(stderr, "FOUND BAD OPTION %d :: %d :: %s\n", optind, optopt, argv[optind]);
			break;
		}
	}
	if (action->op == OSCAP_OP_UNKNOWN) {
		print_oval_usage("oscap", stderr,
				  "Error: No operation specified. Use \"oscap oval eval/collect OVAL_FILE\"");
		return -1;
	}

	/* We should have OVAL file here */
	if (optind >= argc) {
		/* TODO */
		print_oval_usage("oscap", stderr,
				  "Error: Bad number of parameters. OVAL file needs to be specified !");
		return -1;
	}
	action->url_oval = argv[optind];

	return 0;
}

/**
 * Main function
 */
int main(int argc, char **argv)
{
	/**************** GETOPT  ***************/
	int c;
	struct oscap_action *action = oscap_action_new();
	if (action == NULL)
		return 1;

	while (1) {

		int option_index = 0;
		static struct option long_options[] = {
			/* Long options. */
			{"quiet", 0, 0, 'q'},
			{"help", 0, 0, 'h'},
			{"version", 0, 0, 'V'},
			{0, 0, 0, 0}
		};

		c = getopt_long(argc, argv, "+qhV", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'q':	/* QUIET */
			VERBOSE = -1;
			break;
		case 'h':	/* HELP */
			print_usage(argv[0], stdout);
			return 0;
		case 'V':	/* VERSIONS */
			print_versions();
			return 0;
		default:
			break;
		}
	}

	/* MODULE */
	if (optind >= argc) {
		print_usage(argv[0], stderr);
		return 1;
	}

	if ((!strcmp(argv[optind], "xccdf")) || (!strcmp(argv[optind], "XCCDF"))) {
		if (getopt_xccdf(argc, argv, action) == -1)
			return 1;
	} else if ((!strcmp(argv[optind], "oval")) || (!strcmp(argv[optind], "OVAL"))) {
		if (getopt_oval(argc, argv, action) == -1)
			return 1;
	} else if ((!strcmp(argv[optind], "cvss")) || (!strcmp(argv[optind], "CVSS"))) {
		if (getopt_cvss(argc, argv, action) == -1)
			return 1;
	}

	/* Post processing of options */
	/* fetch file from remote source */
	if (action->url_xccdf != NULL) {

		action->f_xccdf = app_curl_download(action->url_xccdf);
		if (!action->f_xccdf) {
			if (VERBOSE >= 0)
				fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
			return 1;
		}
	}
	if (action->url_oval != NULL) {
		action->f_oval = app_curl_download(action->url_oval);
		if (!action->f_oval) {
			if (VERBOSE >= 0)
				fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
			return 1;
		}
	}

	/**************** GETOPT END ***************/
	int retval = 0;

	switch (action->std) {
	case OSCAP_STD_XCCDF:
		switch (action->op) {
		case OSCAP_OP_VALIDATE_XML:
			retval = app_validate_xml(action);
			break;
		case OSCAP_OP_EVAL:
#ifdef ENABLE_XCCDF
			retval = app_evaluate_xccdf(action);
#else
			fprintf(stderr,
				"OSCAP is not compiled with XCCDF support ! Please configure OSCAP library with option --enable-xccdf !\n");
#endif
			break;
		default:
			break;
		}
		break;
	case OSCAP_STD_OVAL:
		switch (action->op) {
		case OSCAP_OP_VALIDATE_XML:
			retval = app_validate_xml(action);
			break;
		case OSCAP_OP_COLLECT:
			retval = app_collect_oval(action);
			break;
		case OSCAP_OP_EVAL:
			retval = app_evaluate_oval(action);
			break;
		default:
			break;
		}
		break;
	case OSCAP_STD_CVSS:
		switch (action->op) {
		case OSCAP_OP_BASE: {
			double base_score;
			cvss_base_score(action->cvss_metrics->ave, 
					action->cvss_metrics->ace, 
					action->cvss_metrics->aue, 
					action->cvss_metrics->cie, 
					action->cvss_metrics->iie, 
					action->cvss_metrics->aie, 
					&base_score, NULL, NULL);
			fprintf(stdout, "Base score: %f\n", base_score);
		}
		break;
		case OSCAP_OP_TEMP: {
			double temp_score;
			cvss_temp_score(action->cvss_metrics->exe, 
					action->cvss_metrics->rle, 
					action->cvss_metrics->rce, 
					action->cvss_metrics->base, 
					&temp_score);
			fprintf(stdout, "Temporal score: %f\n", temp_score);
		}
		break;
		case OSCAP_OP_ENV: {
			double temp_env;
			cvss_env_score(	action->cvss_metrics->cde, action->cvss_metrics->tde,
					action->cvss_metrics->cre, action->cvss_metrics->ire,
					action->cvss_metrics->are, action->cvss_metrics->ave,
					action->cvss_metrics->ace, action->cvss_metrics->aue,
					action->cvss_metrics->cie, action->cvss_metrics->iie,
					action->cvss_metrics->aie, action->cvss_metrics->exe,
					action->cvss_metrics->rle, action->cvss_metrics->rce,
					&temp_env);
			fprintf(stdout, "Environmental score: %f\n", temp_env);
		}
		break;
		default:
			break;
		}
		break;
	default:
		printf("%s unrecognized module '%s'\n", argv[0], argv[1]);
		print_usage(argv[0], stderr);
		break;
	}

	oscap_action_free(action);
	oscap_cleanup();

	return retval;
}
