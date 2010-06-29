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

int VERBOSE = 1;

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

/**
 * Function print usage of this program to specified output
 * @param pname name of program, standard usage argv[0]
 * @param out output stream for fprintf function, standard usage stdout or stderr
 */
static void print_usage(const char *pname, FILE * out)
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
static char *app_curl_download(char *url)
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
/**
 * Callback for XCCDF evaluation. Callback is called after each OVAL definition evaluation
 * right before adding results to Test result model.
 * @param id ID of Rules that is being evaluated
 * @param result XCCDF Result of evaluated rule
 * @param arg User defined data structure
 */
static int callback(const char *id, int result, void *arg)
{
    if (VERBOSE >= 0) printf("Rule \"%s\" result: %s\n", id, xccdf_test_result_type_get_text(result));
    return 0;
}

/**
 * XCCDF Processing fucntion
 * @param f_XCCDF XCCDF XML file that is imported
 * @param f_Results Name of the result file specified by user
 * @param url_XCCDF Url value of XCCDF file defined by user and used in Test Result
 * @param sess OVAL Agent Session
 * @param s_Profile String with Profile ID that will be evaluated
 */
static int app_evaluate_xccdf(const char *f_XCCDF, const char *f_Results, const char *url_XCCDF,
			      oval_agent_session_t * sess, const char *s_Profile)
{

	struct xccdf_policy_iterator *policy_it = NULL;
	struct xccdf_policy *policy = NULL;
	struct xccdf_benchmark *benchmark = NULL;
	struct xccdf_policy_model *policy_model = NULL;

        /* Load XCCDF model and XCCDF Policy model */
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

        /* Initialize OVAL Agent data */
        struct oval_agent_cb_data *usr = oval_agent_cb_data_new();
        oval_agent_cb_data_set_session(usr, sess);
        oval_agent_cb_data_set_callback(usr, NULL, NULL);
        oval_agent_cb_data_set_usr(usr, (void *) policy_model);

	/* Register callback */
        xccdf_policy_model_register_output_callback(policy_model, callback, NULL);
	xccdf_policy_model_register_callback(policy_model,
					     "http://oval.mitre.org/XMLSchema/oval-definitions-5",
					     oval_agent_eval_rule, (void *) usr);
	/* Perform evaluation */
	struct xccdf_result * ritem = xccdf_policy_evaluate(policy);

        /* Write results into XCCDF Test Result model */
	xccdf_result_set_benchmark_uri(ritem, url_XCCDF);
	struct oscap_text *title = oscap_text_new();
	oscap_text_set_text(title, "OSCAP Scan Result");
	xccdf_result_add_title(ritem, title);
	if (policy != NULL) {
		const char * id = xccdf_profile_get_id(xccdf_policy_get_profile(policy));
		if (id != NULL)
			xccdf_result_set_profile(ritem, id);
	}
        oval_agent_export_sysinfo_to_xccdf_result(sess, ritem);
        
        struct xccdf_model_iterator * model_it = xccdf_benchmark_get_models(benchmark);
        while (xccdf_model_iterator_has_more(model_it)) {
            struct xccdf_model * model = xccdf_model_iterator_next(model_it);
            struct xccdf_score * score = xccdf_policy_get_score(policy, ritem, xccdf_model_get_system(model));
            xccdf_result_add_score(ritem, score);
        }
        xccdf_model_iterator_free(model_it);

	/* Export results */
	if (f_Results != NULL)
		xccdf_result_export(ritem, f_Results);

        /* Get the result from TestResult model and decide if end with error or with correct return code */
        int retval = 0;
        struct xccdf_rule_result_iterator * res_it = xccdf_result_get_rule_results(ritem);
        while (xccdf_rule_result_iterator_has_more(res_it)) {
            struct xccdf_rule_result * res = xccdf_rule_result_iterator_next(res_it);
            xccdf_test_result_type_t result = xccdf_rule_result_get_result(res);
            if ((result == XCCDF_RESULT_FAIL) || (result == XCCDF_RESULT_UNKNOWN)) 
                    retval = 2;
        }
        xccdf_rule_result_iterator_free(res_it);


	/* Clear & End */
        oval_agent_cb_data_free(usr);
	xccdf_policy_model_free(policy_model);

	return retval;
}
#endif

/**
 * OVAL Callback function that is passed to OVAL evaluation proccess
 * Function makes statistic of definition results
 */
static int app_oval_callback(const char *id, int result, void *usr)
{

	if (VERBOSE > 0)
		printf("Evalutated definition %s: %s\n", id, oval_result_get_text(result));
	switch ((oval_result_t) result) {
	case OVAL_RESULT_TRUE:
		((struct oval_usr *)usr)->result_true++;
		break;
	case OVAL_RESULT_FALSE:
		((struct oval_usr *)usr)->result_false++;
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
static int app_evaluate_oval(const char *f_Results, oval_agent_session_t * sess)
{
	struct oval_results_model *res_model = NULL;
	struct oval_usr *usr = NULL;
	int ret = 0;

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
	if (f_Results != NULL) {
		// set up directives 
		struct oval_result_directives *res_direct = oval_result_directives_new(res_model);
		oval_result_directives_set_reported(res_direct, OVAL_RESULT_TRUE	| OVAL_RESULT_FALSE |
                						OVAL_RESULT_UNKNOWN	| OVAL_RESULT_NOT_EVALUATED |
	                                                        OVAL_RESULT_ERROR	| OVAL_RESULT_NOT_APPLICABLE, true);

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
			{"xccdf", 1, 0, 2},
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
		case 2:	/* XCCDF */
			url_XCCDF = strdup(optarg);
			break;
		case 'q':
			VERBOSE = -1;
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
			if (VERBOSE >= 0)
				fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
			return 1;
		}
	}
	f_OVAL = app_curl_download(url_OVAL);
	if (!f_OVAL) {
		if (VERBOSE >= 0)
			fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
		return 1;
	}

	/* Print options */
	if (VERBOSE >= 2) {
		printf("Running options:\n");
		printf("\tOption Verbose:\r\t\t\t %d\n", VERBOSE);
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
		if (VERBOSE >= 0)
			fprintf(stderr, "Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
		return 1;

	}

	/* Create agent session */
	sess = oval_agent_new_session(def_model);

	int retval = 0;

	/* Call OVAL or XCCDF evaluation */

#ifdef ENABLE_XCCDF
	if (f_XCCDF != NULL) {
		retval = app_evaluate_xccdf(f_XCCDF, f_Results, url_XCCDF, sess, s_Profile);
	} else if (f_OVAL != NULL)
		retval = app_evaluate_oval(f_Results, sess);
#else
	if (f_OVAL != NULL) {
		retval = app_evaluate_oval(f_Results, sess);
        }
#endif

        /* Cleanup time */
	if (f_XCCDF != NULL)
		free(f_XCCDF);
	if (f_OVAL != NULL)
		free(f_OVAL);
	if (s_Profile != NULL)
		free(s_Profile);
	if (f_Results != NULL)
		free(f_Results);

        oval_agent_destroy_session(sess);
        oval_definition_model_free(def_model);
	oscap_cleanup();

        return retval;
}
