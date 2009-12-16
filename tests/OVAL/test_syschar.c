/*
 * test_driver.c
 *
 *  Created on: Mar 4, 2009
 *      Author: david.niemoller
 */

#include <stdlib.h>
#include <stdio.h>
#include "oval_agent_api.h"
#include "oscap.h"

//typedef int (*oval_xml_error_handler) (struct oval_xml_error *, void *user_arg);
int _test_error_handler(struct oval_xml_error *error, void *null)
{
	char *label;
	switch (error->severity) {
	case XML_ERROR:
		label = "XML ERROR";
		break;
	case XML_VALIDITY_ERROR:
		label = "XML VALIDITY ERROR";
		break;
	case XML_WARNING:
		label = "XML WARNING";
		break;
	case XML_VALIDITY_WARNING:
		label = "XML VALIDITY WARNING";
		break;
	case OVAL_LOG_INFO:
		label = "LOG INFO";
		break;
	case OVAL_LOG_DEBUG:
		label = "LOG DEBUG";
		break;
	case OVAL_LOG_WARN:
		label = "LOG WARN";
		break;
	default:{
			char labelarray[200];
			*labelarray = 0;
			sprintf(labelarray, "UNHANDLED SEVERITY(%d)", error->severity);
			label = labelarray;
		}
	}
	printf("test_error_handler:%s: %s\n     %s(%d)\n", label, error->message, error->system_id, error->line_number);
	return 1;
}

int main(int argc, char **argv)
{
	printf("START\n");
	if (argc > 1) {
		struct oval_definition_model *model = oval_definition_model_new();
		printf("LOAD OVAL DEFINITIONS\n");
		struct oscap_import_source *source = oscap_import_source_new_file(argv[1], NULL);
		oval_definition_model_import(model, source, &_test_error_handler, NULL);
		oscap_import_source_free(source);
		printf("OVAL DEFINITIONS LOADED\n");
		if (argc > 2) {
			printf("LOAD OVAL SYSCHAR\n");
			source = oscap_import_source_new_file(argv[2], NULL);
			struct oval_syschar_model *syschar_model = oval_syschar_model_new(model);
			oval_syschar_model_import(syschar_model, source, &_test_error_handler, NULL);
			oscap_import_source_free(source);
			printf("OVAL SYSCHAR LOADED\n");

			int count;
			struct oval_syschar_iterator *syschars = oval_syschar_model_get_syschars(syschar_model);
			for (count = 0; oval_syschar_iterator_has_more(syschars); count++) {
				oval_syschar_iterator_next(syschars);
			}
			oval_syschar_iterator_free(syschars);

			if (count)
				printf("THERE ARE %d SYSCHARS\n", count);
			else
				printf("NO DEFINITIONS FOUND\n");

			oval_syschar_model_export(syschar_model, oscap_export_target_new_file("-", "UTF-8"));
			oval_syschar_model_free(syschar_model);
		}
		oval_definition_model_free(model);
	} else
		printf("USAGE:Test <oval_definitions.xml> [<system_characteristics.xml>]");
	oscap_cleanup();
}
