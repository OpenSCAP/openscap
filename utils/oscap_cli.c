/*
 * OVAL simple application for evaluating OVAL definition file.
 * The output is system characteristic xml file and oval results xml file.
 * Program iterate through tests in every definition and evaluate objects
 * of these tests.
 *
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
 *      compile: $gcc oscap_cli.c -lcurl -lopenscap -I/usr/include/openscap/
 *               or
 *               $gcc -I../src/OVAL/public/ -I../src/common/public/ oscap_cli.c -L../src/.libs/ -lcurl -lopenscap
 *      run:     $./oscap_cli -vv definition_file.xml
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* OSCAP */
#include <oval_probe.h>
#include <oval_agent_api.h>
#include <error.h>

/* curl */
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

/* lstat and errno */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


void print_usage(const char *pname, FILE *out);
int app_evaluate_test(struct oval_test * test, oval_pctx_t * pctx, 
                      struct oval_definition_model *def_model, 
                      struct oval_syschar_model *sys_model,
                      int verbose);
int app_evaluate_criteria(struct oval_criteria_node *cnode, oval_pctx_t * pctx, 
                          struct oval_definition_model *def_model, 
                          struct oval_syschar_model *sys_model,
                          int verbose);
char * app_curl_download(char * url);


/**
 * Function print usage of this program to specified output
 * @param pname name of program, standard usage argv[0]
 * @param out output stream for fprintf function, standard usage stdout or stderr
 */
void print_usage(const char *pname, FILE *out) {

  fprintf(out, 
	  "Usage: %s [OPTION] XML_FILE\n"
          "Evaluate OVAL definition file specified by XML_FILE.\n"
          "Results are saved with suffix .syschar.xml and .results.xml\n"
          "XML_FILE can be specified by URL\n"
          "\n"
          "Options:\n"
	  "  %s -h --help\r\t\t\t\t - show this help\n"
	  "  %s -v\r\t\t\t\t - run in verbose mode \n"
	  "  %s -vv\r\t\t\t\t - run in verbose mode \n"
          "  %s -o --objects-only\r\t\t\t\t - evaluate objects only, ignore definitions "
             " \n\r\t\t\t\t   and verbosity\n",
	  pname, pname, pname, pname, pname);
}

/**
 * Function evaluating tests. First get object from test and try if it's already
 * evaluated. If not, process the evaluation.
 * @param test oval_test structure which is being evaluated
 * @param pctx oval_pctx structure for probe process
 * @param def_model Definition model
 * @param sys_model System characteristic model
 * @param verbose Verbosity level
 * @returns -1 in case of error, 0 in case of warning and 1 in case of success
 */
int app_evaluate_test(struct oval_test * test, oval_pctx_t * pctx, 
                      struct oval_definition_model *def_model, 
                      struct oval_syschar_model *sys_model,
                      int verbose) {

    char                    *objid;
    struct oval_object      *object;
    struct oval_syschar     *syschar = NULL;

    object = oval_test_get_object(test);
    
    /* Check if there is an object to evaluate */
    if (object != NULL) {
        objid = oval_object_get_id(object);
        /* Is the object already evaluated ? */
        syschar = oval_syschar_model_get_syschar(sys_model, objid);
        if (syschar == NULL) {
            /* NO it's not done yet, continue .. */
            if (verbose == 2) fprintf(stdout, "Probing object (%s)\n", objid);
            syschar = oval_probe_object_eval (pctx, object);
            /* There is a problem with evaluating .. */
            if (syschar == NULL) {
                    printf("WARNING: Syschar for object (%s) is not available\n", objid);
                    if( oscap_err() ) {
                            printf("Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
                             /* does it make sense to continue? depens on error code */
                    }
                    syschar = oval_syschar_new(sys_model, object);
                    oval_syschar_set_flag(syschar,SYSCHAR_FLAG_NOT_COLLECTED);
            }
            /* Everything OK, add characteristic for this object to syschar model */
            oval_syschar_model_add_syschar(sys_model, syschar);
        /* Object is already evaluated and present in syschar model */
        } else if (verbose == 2) fprintf(stdout, "Object (%s) already done, skipping\n", objid);
    /* Probably we are on unknown_test which does noto contain objects */
    } else if (verbose == 2) fprintf(stdout, "Test (%s) contains no objects\n", oval_test_get_id(test));

    return 1;
}

/**
 * Function that takes criteria node, find it's type and then goes recursively deeper
 * to criteria XML tree to evaluate all criterions and extended definitions by calling
 * app_evaluate_test on final criterion node.
 * @param cnode oval_criteria_node structure represents criteria, criterion and 
 *              extended definition node
 * @param pctx oval_pctx structure for probe process
 * @param def_model Definition model
 * @param sys_model System characteristic model
 * @param verbose Verbosity level
 * @returns -1 in case of error, 0 in case of warning and 1 in case of success
 */
int app_evaluate_criteria(struct oval_criteria_node *cnode, oval_pctx_t * pctx, 
                          struct oval_definition_model *def_model, 
                          struct oval_syschar_model *sys_model,
                          int verbose) {

    switch (oval_criteria_node_get_type(cnode)) {
        /* Criterion node is final node that has reference to test */
        case OVAL_NODETYPE_CRITERION:{
            /* There should be a test .. */
            struct oval_test * test = oval_criteria_node_get_test(cnode);
            if (test == NULL) return -1;
            /* .. evaluate it */
            app_evaluate_test(test, pctx, def_model, sys_model, verbose);

        } break;
        /* Criteria node is type of set that contains more criterias. Criteria node
         * child can be also type of criteria, criterion or extended definition */
        case OVAL_NODETYPE_CRITERIA:{
            /* group of criterion nodes, get subnodes, continue recursive */
            struct oval_criteria_node_iterator * cnode_it = oval_criteria_node_get_subnodes(cnode);
            if (cnode_it == NULL) return 0;
            /* we have subnotes */
            struct oval_criteria_node * node;
            while (oval_criteria_node_iterator_has_more(cnode_it)) {
                node = oval_criteria_node_iterator_next(cnode_it);
                app_evaluate_criteria(node, pctx, def_model, sys_model, verbose);
            }
            oval_criteria_node_iterator_free(cnode_it);
        } break;
        /* Extended definition contains reference to definition, we need criteria of this
         * definition to be evaluated completely */
        case OVAL_NODETYPE_EXTENDDEF:{
            struct oval_definition * oval_def = oval_criteria_node_get_definition(cnode);
            cnode = oval_definition_get_criteria(oval_def);
            app_evaluate_criteria(cnode, pctx, def_model, sys_model, verbose);
        } break;
        case OVAL_NODETYPE_UNKNOWN:
            break;
    }
    return 1;
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
char * app_curl_download(char * url) {

    struct stat buf;
    /* Is the file local ? */
    if (lstat(url, &buf) == 0 ) return url;

    /* Remote file will be stored in this xml */
    char * outfile = "definition_file.xml";

    CURL *curl;
    FILE *fp;
    CURLcode res;
    /* Initialize CURL for download remote file */
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(outfile,"wb");
        /* Set options for download file to *fp* from *url* */
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        /* Always cleanup */
        curl_easy_cleanup(curl);
        /* Close the file finally*/
        fclose(fp);
    }

    return outfile;
}


/**
 * Main function
 */
int main(int argc, char **argv)
{
        int verbose = 0;
        int method = 0;
        int i;
        char * url = NULL;
       
        /* Parse parameters */
        for (i=1; i<argc; i++) {
                if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
                        print_usage(argv[0], stdout);
                        return 0;
                }
                else if (!strcmp(argv[i], "-v" )) verbose = 1;
                else if (!strcmp(argv[i], "-vv")) verbose = 2;
                else if (!strcmp(argv[i], "-o"))  method = 1;
                else if (!strcmp(argv[i], "--objects-only")) method = 1;
                else if (url == NULL) url = argv[i];
                else {
                        fprintf(stderr, "Bad usage of %s !\n\n", argv[0]);
                        print_usage(argv[0], stderr);
                        return -1;
                }
        }

        /* prevent fail behaviour by validating OVAL content */
        /* Note: XCCDF is not supported yet */

        /* fetch file from remote source */
        char * file_path = app_curl_download(url);

        /* Get definition model from XML file */
        struct oscap_import_source *def_in = oscap_import_source_new_file(file_path,NULL);
        struct oval_definition_model *def_model = oval_definition_model_new();
        oval_definition_model_import(def_model, def_in, NULL);
        /* Import problems ? Do not continue then ! */
	if( oscap_err() ) {
		printf("Error: %s\n", oscap_err_desc());
		return -1;
	}
        oscap_import_source_free(def_in);

	/* create syschar model */
	struct oval_syschar_model *sys_model = oval_syschar_model_new(def_model);
        
        oval_pctx_t *pctx;
        pctx = oval_pctx_new(sys_model);
	/* probe sysinfo */
	struct oval_sysinfo *sysinfo;
        sysinfo = oval_probe_sysinf_eval(pctx);
	if( sysinfo == NULL ) {
		printf("Warning: sysinfo not available\n");
		if( oscap_err() ) {
			printf("Error: (%d) %s\n", oscap_err_code(), oscap_err_desc());
			return 1;
		}
	}
	oval_syschar_model_set_sysinfo(sys_model, sysinfo);
	oval_sysinfo_free(sysinfo);

        /* Use method 1 to suppress syschar assertion abort caused by bug in library and process
         * all objects first */
	if (method == 1) { 
            /* First method */
            oval_syschar_model_probe_objects(sys_model);

        } else {
            /* Get the list of definitions */
            struct oval_definition_iterator * oval_def_it = oval_definition_model_get_definitions(def_model);

            struct oval_criteria_node * cnode = NULL;
            struct oval_definition * oval_def = NULL;
            /* Iterate through definitions and evaluate all criteria */
            while (oval_definition_iterator_has_more(oval_def_it) ) {
                oval_def = oval_definition_iterator_next(oval_def_it);
                if (verbose >= 1 ) fprintf(stdout, "Evaluating definition: %s\n", oval_definition_get_title(oval_def));
                cnode = oval_definition_get_criteria(oval_def);
                app_evaluate_criteria(cnode, pctx, def_model, sys_model, verbose);
            }
            oval_definition_iterator_free(oval_def_it);
        }
        oval_pctx_free (pctx);

	/* print # syschars */
        int count = 0;
        struct oval_syschar_iterator *syschars = oval_syschar_model_get_syschars(sys_model);
        for (count = 0; oval_syschar_iterator_has_more(syschars); count++) {
        	oval_syschar_iterator_next(syschars);
        }
        oval_syschar_iterator_free(syschars);
	printf("THERE ARE %d SYSCHARS\n", count);

        /* Export syschar model to XML */
	char * syschar_fname = malloc( sizeof(char) * strlen(file_path) + 15 );
	sprintf(syschar_fname, "%s.syschar.xml", file_path);
	struct oscap_export_target *syschar_out  = oscap_export_target_new_file(syschar_fname, "UTF-8");
	oval_syschar_model_export(sys_model, syschar_out);
	free(syschar_fname);
	oscap_export_target_free(syschar_out);

	/* create result model */
	struct oval_syschar_model *sys_models[] = {sys_model, NULL};
        struct oval_results_model* res_model = oval_results_model_new( def_model, sys_models );

	/* set up directives */
	struct oval_result_directives * res_direct = oval_result_directives_new(res_model);
	oval_result_directives_set_reported(res_direct, OVAL_RESULT_INVALID, true);
	oval_result_directives_set_reported(res_direct, OVAL_RESULT_TRUE, true);
	oval_result_directives_set_reported(res_direct, OVAL_RESULT_FALSE, true);
	oval_result_directives_set_reported(res_direct, OVAL_RESULT_UNKNOWN, true);
	oval_result_directives_set_reported(res_direct, OVAL_RESULT_ERROR, true);
	oval_result_directives_set_reported(res_direct, OVAL_RESULT_NOT_EVALUATED, true);
	oval_result_directives_set_reported(res_direct, OVAL_RESULT_NOT_APPLICABLE , true);
	oval_result_directives_set_content(res_direct,OVAL_RESULT_FALSE, OVAL_DIRECTIVE_CONTENT_FULL);
	oval_result_directives_set_content(res_direct,OVAL_RESULT_TRUE, OVAL_DIRECTIVE_CONTENT_FULL);
        
        /* Export result model to XML */
	char * results_fname = malloc( sizeof(char) * strlen(file_path) + 15 );
	sprintf(results_fname, "%s.results.xml", file_path);
	struct oscap_export_target *result_out  = oscap_export_target_new_file(results_fname, "UTF-8");
	oval_results_model_export(res_model, res_direct, result_out);
	free(results_fname);
	oscap_export_target_free(result_out);

	oval_result_directives_free(res_direct);
	oval_definition_model_free(def_model);
	oval_syschar_model_free(sys_model);
	oval_results_model_free(res_model);

        /* FIN */
        return 0;
}
