/*
 * test_results.c
 *
 *  Created on: Mar 4, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "api/oval_agent_api.h"
#include "../common/oscap.h"


//typedef int (*oval_xml_error_handler) (struct oval_xml_error *, void *user_arg);
int _test_error_handler (struct oval_xml_error *error, void *null){
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
		struct oval_object_model *model = oval_object_model_new();
		printf("LOAD OVAL DEFINITIONS\n");
		struct import_source *source = import_source_file(argv[1]);
		load_oval_definitions(model, source, &_test_error_handler, NULL);
		import_source_free(source);
		printf("OVAL DEFINITIONS LOADED\n");
		if(argc>2){
			printf("LOAD OVAL RESULTS\n");
			source = import_source_file(argv[2]);
			struct oval_results_model *results_model = oval_results_model_new(model,NULL);
			load_oval_results(results_model, source, &_test_error_handler, NULL);
			import_source_free(source);
			printf("OVAL RESULTS LOADED\n");
			/*
			struct oval_iterator_results *results = oval_results_model_results(results_model);
			int numResults;for(numResults=0;oval_iterator_results_has_more(results);numResults++){
				struct oval_results *result = oval_iterator_results_next(results);
				oval_result_to_print(result, "", numResults+1);
			}
			printf("THERE ARE %d RESULTS\n",numResults);
			*/
			//oval_syschar_model_free(oval_results_model_syschar_model(results_model)); // Is this OK??
			if (argc>3) {
				printf("WRITE OVAL RESULTS\n");
				struct oval_result_directives *directives = oval_result_directives_new();
				struct export_target* target = export_target_new(argv[3], "UTF-8");
				export_results(results_model, directives, target);
				export_target_free(target);
				oval_result_directives_free(directives);
				printf("OVAL RESULTS WRITTEN\n");
			}
			oval_results_model_free(results_model);
		}
		oval_object_model_free(model);
	}else printf("USAGE: %s <oval_definitions.xml> [results.xml>]\n", argv[0]);
	oscap_cleanup();
}

