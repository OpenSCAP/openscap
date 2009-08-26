/*
 * test_driver.c
 *
 *  Created on: Mar 4, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "api/oval_agent_api.h"



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
			printf("LOAD OVAL SYSCHAR\n");
			source = import_source_file(argv[2]);
			struct oval_syschar_model *syschar_model = oval_syschar_model_new(model,NULL);
			load_oval_syschar(syschar_model, source, &_test_error_handler, NULL);
			import_source_free(source);
			printf("OVAL SYSCHAR LOADED\n");
			struct oval_iterator_syschar *syschars = oval_syschar_model_syschars(syschar_model);
			int numSyschars;for(numSyschars=0;oval_iterator_syschar_has_more(syschars);numSyschars++){
				struct oval_syschar *syschar = oval_iterator_syschar_next(syschars);
				oval_syschar_to_print(syschar, "", numSyschars+1);
			}
			printf("THERE ARE %d SYSCHARS\n",numSyschars);
			oval_syschar_model_free(syschar_model);
		}
		oval_object_model_free(model);
	}else printf("USAGE:Test <oval_definitions.xml> [<system_characteristics.xml>]");
	oscap_cleanup();
}

