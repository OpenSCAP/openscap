/*
 * test_results.c
 *
 *  Created on: Mar 4, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "oval_agent_api.h"
#include "oval_results.h"
#include "oscap.h"
#include "error.h"

//typedef int (*oval_xml_error_handler) (struct oval_xml_error *, void *user_arg);
int _test_error()
{
        if (oscap_err ()) {
                oscap_errfamily_t f;
                oscap_errcode_t   c;
                const char       *d;

                fprintf (stderr, "GOT error: %d, %d, %s.\n",
                        f = oscap_err_family (),
                        c = oscap_err_code (),
                        d = oscap_err_desc ());
        }
                
	return 0;
}

int _test_error_handler (struct oval_xml_error *error, void *null){ /* <-- deprecated */
	char *label;
	switch(error->severity){
	case XML_ERROR           :label = "XML ERROR"           ;break;
	case XML_VALIDITY_ERROR  :label = "XML VALIDITY ERROR"  ;break;
	case XML_WARNING         :label = "XML WARNING"         ;break;
	case XML_VALIDITY_WARNING:label = "XML VALIDITY WARNING";break;
	case OVAL_LOG_INFO       :label = "LOG INFO"            ;break;
	case OVAL_LOG_DEBUG      :label = "LOG DEBUG"           ;break;
	case OVAL_LOG_WARN       :label = "LOG WARN"            ;break;
	default:{
		char labelarray[200]; *labelarray = 0;
		sprintf(labelarray, "UNHANDLED SEVERITY(%d)",error->severity);
		label = labelarray;
	}
	}
	printf("test_error_handler:%s: %s\n     %s(%d)\n",label,error->message, error->system_id, error->line_number);
	return 1;
}


int main(int argc, char **argv)
{
	printf("START\n");
	if(argc>1){
		struct oval_definition_model *model = oval_definition_model_new();
		printf("LOAD OVAL DEFINITIONS\n");
		struct oscap_import_source *source = oscap_import_source_new_file(argv[1], NULL);
		if (oval_definition_model_import(model, source, NULL) < 1)
                        _test_error();
		oscap_import_source_free(source);
		printf("OVAL DEFINITIONS LOADED\n");
		if(argc>2){
			printf("LOAD OVAL RESULTS\n");
			source = oscap_import_source_new_file(argv[2], NULL);
			struct oval_results_model *results_model = oval_results_model_new(model,NULL);
			oval_results_model_import(results_model, source, NULL);
                        _test_error();
			oscap_import_source_free(source);
			printf("OVAL RESULTS LOADED\n");
			if (argc>3) {
				printf("WRITE OVAL RESULTS\n");
				struct oval_result_directives *directives = oval_result_directives_new(results_model);
				struct oscap_export_target* target = oscap_export_target_new_file(argv[3], "UTF-8");
				oval_results_model_export(results_model, directives, target);
				oscap_export_target_free(target);
				oval_result_directives_free(directives);
				printf("OVAL RESULTS WRITTEN\n");
			}
			oval_results_model_free(results_model);
		}
		oval_definition_model_free(model);
	}else printf("USAGE: %s <oval_definitions.xml> [results.xml>]\n", argv[0]);
	oscap_cleanup();
}

