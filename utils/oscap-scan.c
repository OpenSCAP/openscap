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

/* XCCDF */
#include <xccdf.h>
#include <xccdf_policy.h>

#ifdef ENABLE_XCCDF
/**
 * Specification of structure for transformation of OVAL Result type
 * to XCCDF result type.
 */
struct oval_result_to_xccdf_spec {
	oval_result_t oval;
	xccdf_test_result_type_t xccdf;
};

/**
 * Array of transformation rules from OVAL Result type to XCCDF result type
 */
static const struct oval_result_to_xccdf_spec XCCDF_OVAL_RESULTS_MAP[] = {
	{OVAL_RESULT_INVALID, XCCDF_RESULT_NOT_APPLICABLE},
	{OVAL_RESULT_TRUE, XCCDF_RESULT_PASS},
	{OVAL_RESULT_FALSE, XCCDF_RESULT_FAIL},
	{OVAL_RESULT_UNKNOWN, XCCDF_RESULT_UNKNOWN},
	{OVAL_RESULT_ERROR, XCCDF_RESULT_ERROR},
	{OVAL_RESULT_NOT_EVALUATED, XCCDF_RESULT_NOT_CHECKED},
	{OVAL_RESULT_NOT_APPLICABLE, XCCDF_RESULT_NOT_APPLICABLE},
	{0, 0}
};

/**
 * Function for OVAL Result type -> XCCDF result type transformation
 * @param id OVAL_RESULT_* type
 * @return xccdf_test_result_type_t
 */
static xccdf_test_result_type_t xccdf_get_result_from_oval(oval_result_t id)
{

	const struct oval_result_to_xccdf_spec *mapptr;

	for (mapptr = XCCDF_OVAL_RESULTS_MAP; mapptr->oval != 0; ++mapptr) {
		if (id == mapptr->oval)
			return mapptr->xccdf;
	}

	return XCCDF_RESULT_UNKNOWN;
}
#endif

/**
 * User defined structure that is passed to XCCDF evaluation and is returned 
 * in callback calls.
 */
struct xccdf_usr {

	int verbose;
	char *result_id;
	oval_agent_session_t *asess;
};

struct oval_usr {

	int result_false;
	int result_true;
	int result_invalid;
	int result_unknown;
	int result_neval;
	int result_napp;
	int verbose;
};

void print_usage(const char *pname, FILE * out);
int app_evaluate_test(struct oval_test *test, oval_probe_session_t * sess,
		      struct oval_definition_model *def_model, struct oval_syschar_model *sys_model, int verbose);
int app_evaluate_criteria(struct oval_criteria_node *cnode, oval_probe_session_t * sess,
			  struct oval_definition_model *def_model, struct oval_syschar_model *sys_model, int verbose);
char *app_curl_download(char *url);

/**
 * Function print usage of this program to specified output
 * @param pname name of program, standard usage argv[0]
 * @param out output stream for fprintf function, standard usage stdout or stderr
 */
void print_usage(const char *pname, FILE * out)
{

	fprintf(out,
		"Usage: %s [options] [url|path/]filename\n"
		"\n"
		"Evaluate OVAL definition file specified by filename.\n"
		"The input source may be either the full path to the xml file "
		"or an URL from which to download it.\n"
		"\n"
		"Options:\n"
		"   -h --help\r\t\t\t\t - show this help\n"
		"   -v --verbose <integer>\r\t\t\t\t - run in verbose mode (0,1).\n"
		"   -q --quiet\r\t\t\t\t - Quiet mode. Suppress all warning and messages.\n"
		" \n\r\t\t\t\t   and verbosity\n"
		"   --xccdf <file>\r\t\t\t\t - The name of XCCDF file (required XCCDF support).\n"
		"   --result-file <file>\r\t\t\t\t - The name of file with OVAL results output.\n"
		"   --xccdf-profile <name>\r\t\t\t\t - The name of Profile to be evaluated in case of evaluating XCCDF.\n",
		pname);
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
char *app_curl_download(char *url)
{

	struct stat buf;
	/* Is the file local ? */
	if (lstat(url, &buf) == 0)
		return url;

	/* Remote file will be stored in this xml */
	char *outfile = "definition_file.xml";

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
static bool app_xccdf_callback(struct xccdf_policy_model *model, const char *rule_id, const char *id,
			       struct xccdf_value_binding_iterator *it, void *usr)
{
	int verbose = ((struct xccdf_usr *)usr)->verbose;
	struct oval_agent_session *session = ((struct xccdf_usr *)usr)->asess;

	/* Get the definition model from OVAL agent session */
	struct oval_definition_model *def_model =
	    oval_results_model_get_definition_model(oval_agent_get_results_model(session));

	while (xccdf_value_binding_iterator_has_more(it)) {

		struct xccdf_value_binding *binding = xccdf_value_binding_iterator_next(it);
		char *name = xccdf_value_binding_get_name(binding);
		char *value = xccdf_value_binding_get_value(binding);
		struct oval_variable *variable = oval_definition_model_get_variable(def_model, name);
		if (variable != NULL) {

			struct oval_value_iterator *value_it = oval_variable_get_values(variable);
			if (oval_value_iterator_has_more(value_it)) {	/* We have conflict here */
				oval_definition_model_clear_external_variables(def_model);
				oval_agent_destroy_session(session);
				((struct xccdf_usr *)usr)->asess = oval_agent_new_session(def_model);
			}
			struct oval_variable_model *var_model = oval_variable_model_new();
			oval_datatype_t o_type = oval_variable_get_datatype(variable);
			oval_variable_model_add(var_model, name, "Unknown", o_type, value);
			oval_definition_model_bind_variable_model(def_model, var_model);

		} else if (verbose >= 0)
			fprintf(stderr, "VARIABLE %s DOES NOT EXIST\n", name);
	}

	oval_result_t result;
	result = oval_agent_eval_definition(session, id);

	if (verbose > 0)
		printf("Definition \"%s::%s\" result: %s\n", rule_id, id, oval_result_get_text(result));

	/* Add result to Policy Model */
	struct xccdf_result *ritem;
	char *rid = ((struct xccdf_usr *)usr)->result_id;

	/* Is the Result already existing ? */
	ritem = xccdf_policy_model_get_result_by_id(model, rid);
	if (ritem == NULL) {
		/* Should be here an error ? */
	} else {
		struct xccdf_rule_result *rule_ritem = xccdf_rule_result_new();
		xccdf_rule_result_set_result(rule_ritem, xccdf_get_result_from_oval(result));
		xccdf_rule_result_set_idref(rule_ritem, rule_id);
		xccdf_result_add_rule_result(ritem, rule_ritem);
	}

	return true;
}

/**
 * XCCDF Preprocessing 
 */
static int app_evaluate_xccdf(const char *f_XCCDF, const char *f_Results, const char *url_XCCDF,
			      oval_agent_session_t * sess, int verbose, const char *s_Profile)
{

	struct xccdf_policy_iterator *policy_it = NULL;
	struct xccdf_policy *policy = NULL;
	struct xccdf_benchmark *benchmark = NULL;
	struct xccdf_policy_model *policy_model = NULL;

	struct oval_syschar_model *sys_model = NULL;
	struct oval_definition_model *def_model = NULL;
	struct oval_results_model *res_model = NULL;
	struct oval_result_system *re_system = NULL;
	struct oval_sysinfo *sysinfo = NULL;

	benchmark = xccdf_benchmark_import(f_XCCDF);
	policy_model = xccdf_policy_model_new(benchmark);

	/* Get the first policy, just for prototype - if there is no Policy 
	 * report error and return -1 */
	if (s_Profile != NULL) {
		policy = xccdf_policy_model_get_policy_by_id(policy_model, s_Profile);
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

	struct xccdf_usr *usr = malloc(sizeof(struct xccdf_usr));
	usr->result_id = "oscap_scan-test";	/* ID of TestResult in XCCDF model */
	usr->asess = sess;
	usr->verbose = verbose;

	/* New TestResult structure */
	struct xccdf_result *ritem = xccdf_result_new();
	xccdf_result_set_id(ritem, usr->result_id);
	/* Fill the structure */
	xccdf_result_set_benchmark_uri(ritem, url_XCCDF);
	struct oscap_text *title = oscap_text_new();
	oscap_text_set_text(title, "OSCAP Scan Result");
	xccdf_result_add_title(ritem, title);
	xccdf_policy_model_add_result(policy_model, ritem);

	/* Register callback */
	xccdf_policy_model_register_callback(policy_model,
					     "http://oval.mitre.org/XMLSchema/oval-definitions-5",
					     app_xccdf_callback, (void *)usr);
	/* Perform evaluation */
	xccdf_policy_evaluate(policy);
	/* Clear after eval */
	/* ================== RESULTS ========================== */
	/* Somehow we need Syschar model */
	res_model = oval_agent_get_results_model(usr->asess);
	def_model = oval_results_model_get_definition_model(res_model);
	re_system = oval_result_system_iterator_next(oval_results_model_get_systems(res_model));	/* Get the very first system */
	sys_model = oval_result_system_get_syschar_model(re_system);
	sysinfo = oval_syschar_model_get_sysinfo(sys_model);

	xccdf_result_set_test_system(ritem, oval_sysinfo_get_primary_host_name(sysinfo));
	if (policy != NULL) {
		struct xccdf_profile *profile = xccdf_policy_get_profile(policy);
		if (xccdf_profile_get_id(profile) != NULL)
			xccdf_result_set_profile(ritem, xccdf_profile_get_id(profile));
	}

	struct xccdf_target_fact *fact = NULL;
	struct oval_sysint *sysint = NULL;

	struct oval_sysint_iterator *sysint_it = oval_sysinfo_get_interfaces(sysinfo);
	while (oval_sysint_iterator_has_more(sysint_it)) {
		sysint = oval_sysint_iterator_next(sysint_it);
		xccdf_result_add_target_address(ritem, oval_sysint_get_ip_address(sysint));

		if (oval_sysint_get_mac_address(sysint) != NULL) {
			fact = xccdf_target_fact_new();
			xccdf_target_fact_set_name(fact, "urn:xccdf:fact:ethernet:MAC");
			xccdf_target_fact_set_string(fact, oval_sysint_get_mac_address(sysint));
			xccdf_result_add_target_fact(ritem, fact);
		}
	}
	oval_sysint_iterator_free(sysint_it);
	/* TODO: Here will come score system export to result */
	if (f_Results != NULL)
		xccdf_result_export(ritem, f_Results);

	/* -- */
	oval_agent_destroy_session(usr->asess);
	free(usr);
	xccdf_policy_model_free(policy_model);
	return 0;
}
#endif

static int app_oval_callback(const char *id, oval_result_t result, void *usr)
{

	if (((struct oval_usr *)usr)->verbose > 0)
		printf("Evalutated definition %s: %s\n", id, oval_result_get_text(result));
	switch (result) {
	case OVAL_RESULT_TRUE:
		((struct oval_usr *)usr)->result_true++;
		break;
	case OVAL_RESULT_FALSE:
		((struct oval_usr *)usr)->result_false++;
		break;
	case OVAL_RESULT_INVALID:
		((struct oval_usr *)usr)->result_invalid++;
		break;
	case OVAL_RESULT_UNKNOWN:
		((struct oval_usr *)usr)->result_unknown++;
		break;
	case OVAL_RESULT_NOT_EVALUATED:
		((struct oval_usr *)usr)->result_neval++;
		break;
	case OVAL_RESULT_NOT_APPLICABLE:
		((struct oval_usr *)usr)->result_napp++;
		break;
	default:
		break;
	}

	return 0;
}

/**
 * Function that evaluate OVAL content (without XCCDF).
 */
static int app_evaluate_oval(const char *f_OVAL, const char *f_Results, oval_agent_session_t * sess, int verbose)
{
	struct oval_results_model *res_model = NULL;
	struct oval_usr *usr = NULL;
	int ret = 0;

	res_model = oval_agent_get_results_model(sess);

	/* Init usr structure */
	usr = malloc(sizeof(struct oval_usr));
	usr->verbose = verbose;
	usr->result_false = 0;
	usr->result_true = 0;
	usr->result_invalid = 0;
	usr->result_unknown = 0;
	usr->result_neval = 0;
	usr->result_napp = 0;

	ret = oval_agent_eval_system(sess, app_oval_callback, usr);

	if (verbose >= 0)
		printf("Evaluation: All done.\n");

	if (ret == -1) {
		if ((oscap_err()) && (verbose >= 0))
			if (verbose >= 0)
				fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
		return 1;
	}
	if (verbose >= 2) {
		fprintf(stdout, "====== RESULTS ======\n");
		fprintf(stdout, "TRUE:          \r\t\t %d\n", usr->result_true);
		fprintf(stdout, "FALSE:         \r\t\t %d\n", usr->result_false);
		fprintf(stdout, "INVALID:       \r\t\t %d\n", usr->result_invalid);
		fprintf(stdout, "UNKNOWN:       \r\t\t %d\n", usr->result_unknown);
		fprintf(stdout, "NOT EVALUATED: \r\t\t %d\n", usr->result_neval);
		fprintf(stdout, "NOT APPLICABLE:\r\t\t %d\n", usr->result_napp);
	}

    /*=============== PRINT RESULTS =====================*/
	if (f_Results != NULL) {
		// set up directives 
		struct oval_result_directives *res_direct = oval_result_directives_new(res_model);
		oval_result_directives_set_reported(res_direct, OVAL_RESULT_INVALID |
						    OVAL_RESULT_TRUE |
						    OVAL_RESULT_FALSE |
						    OVAL_RESULT_UNKNOWN |
						    OVAL_RESULT_ERROR |
						    OVAL_RESULT_NOT_EVALUATED | OVAL_RESULT_NOT_APPLICABLE, true);

		oval_result_directives_set_content(res_direct, OVAL_RESULT_FALSE, OVAL_DIRECTIVE_CONTENT_FULL);
		oval_result_directives_set_content(res_direct, OVAL_RESULT_TRUE, OVAL_DIRECTIVE_CONTENT_FULL);

		// Export result model to XML 
		oval_results_model_export(res_model, res_direct, f_Results);
		oval_result_directives_free(res_direct);
	}

	/* Clear and return */
	oval_agent_destroy_session(sess);

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

/**
 * Main function
 */
int main(int argc, char **argv)
{
	int verbose = 0;	    /**< Verbosity level variable */
	char *url_OVAL = NULL;	    /**< URL of OVAL definition file */
	char *url_XCCDF = NULL;	    /**< URL of OVAL definition file */
	oval_agent_session_t *sess = NULL;   /**< */
	char *f_OVAL = NULL;	    /**< Name of OVAL definition file*/
	char *f_XCCDF = NULL;	    /**< Name of XCCDF benchmark file*/
	char *f_Results = NULL;
	char *s_Profile = NULL;

	/**************** GETOPT  ***************/
	int c;

	while (1) {

		int option_index = 0;
		static struct option long_options[] = {
			/* Long options. */
			{"quiet", 0, 0, 'q'},
			{"help", 0, 0, 'h'},
			{"result-file", 1, 0, 0},
			{"xccdf-profile", 1, 0, 1},
			{"verbose", 1, 0, 2},
			{"xccdf", 1, 0, 3},
			{0, 0, 0, 0}
		};

		c = getopt_long(argc, argv, "qhv0123", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:	/* Results */
			f_Results = strdup(optarg);
			break;
		case 1:	/* Syschars */
			s_Profile = strdup(optarg);
			break;
		case 2:	/* Verbose */
			if (verbose != -1)
				verbose = atoi(optarg);
			if (verbose >= 1)
				verbose = 2;
			break;
		case 3:	/* XCCDF */
			url_XCCDF = strdup(optarg);
			break;
		case 'v':	/* quiet is higher priority then verbose */
			if (verbose != -1)
				verbose += 1;
			if (verbose >= 1)
				verbose = 2;
			break;
		case 'q':
			verbose = -1;
			break;
		case 'h':	/* Help */
			print_usage(argv[0], stderr);
			return 0;
		default:
			fprintf(stderr, "Bad usage of %s !\n\n", argv[0]);
			print_usage(argv[0], stderr);
			return 1;
		}
	}

	if (optind < argc)	/* mandatory OVAL file */
		url_OVAL = strdup(argv[optind++]);

	if (optind < argc) {	/* No other not-optional argument */
		fprintf(stderr, "Bad usage of %s !\n\n", argv[0]);
		print_usage(argv[0], stderr);
		return 1;
	}
	if (url_OVAL == NULL) {
		fprintf(stderr, "Bad usage of %s !\n\n", argv[0]);
		print_usage(argv[0], stderr);
		return 1;
	}

	/* Post processing of options */
	/* fetch file from remote source */
	if (url_XCCDF != NULL) {

		f_XCCDF = app_curl_download(url_XCCDF);
		if (!f_XCCDF) {
			if (verbose >= 0)
				fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
			return 1;
		}
	}
	f_OVAL = app_curl_download(url_OVAL);
	if (!f_OVAL) {
		if (verbose >= 0)
			fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
		return 1;
	}

	/* Print options */
	if (verbose >= 2) {
		printf("Running options:\n");
		printf("\tOption Verbose:\r\t\t\t %d\n", verbose);
		printf("\tOption OVAL:\r\t\t\t %s\n", url_OVAL);
		printf("\tOption Results:\r\t\t\t %s\n", f_Results);
		printf("\tOption XCCDF:\r\t\t\t %s\n", url_XCCDF);
		printf("\tOption XCCDF Profile:\r\t\t\t %s\n", s_Profile);
	}
	/**************** GETOPT ***************/

	/*if (url_XCCDF != NULL)
	   free(url_XCCDF);
	   if (url_OVAL != NULL)
	   free(url_OVAL); */

	/* Import OVAL definition file */
	struct oval_definition_model *def_model = oval_definition_model_import(f_OVAL);
	if (oscap_err()) {
		if (verbose >= 0)
			fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
		return 1;

	}

	/* Create agent session */
	sess = oval_agent_new_session(def_model);

	int retval = 0;

	/* Call OVAL or XCCDF evaluation */

#ifdef ENABLE_XCCDF
	if (f_XCCDF != NULL) {
		retval = app_evaluate_xccdf(f_XCCDF, f_Results, url_XCCDF, sess, verbose, s_Profile);
	} else if (f_OVAL != NULL)
		retval = app_evaluate_oval(f_OVAL, f_Results, sess, verbose);
#else
	if (f_OVAL != NULL) {
		retval = app_evaluate_oval(f_OVAL, f_Results, sess, verbose);
	} else
		printf("Missing OVAL file !\n");
#endif

	if (f_XCCDF != NULL)
		free(f_XCCDF);
	if (f_OVAL != NULL)
		free(f_OVAL);
	return retval;

	oscap_cleanup();
}
