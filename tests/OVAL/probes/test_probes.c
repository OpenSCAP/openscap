/*
 * test_probes.c
 *
 *  Created on: Aug 4, 2009
 *      Author: Peter Vrabec
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "public/oval_agent_api.h"
#include "oval_definitions_impl.h"
#include "oval_system_characteristics_impl.h"

int _test_error_handler(struct oval_xml_error *error, void *null)
{
	return 1;
}

void oval_syschar_to_print(struct oval_syschar*, const char*, int);

int main(int argc, char **argv)
{
	int idx, ret;
        oval_pctx_t *pctx;
	char *id, *probe_dir;
	struct oval_definition_model *definition_model;
	struct oval_syschar_model *syschar_model;
	struct oval_import_source *source;
	struct oval_object_iterator *obj_it;
	struct oval_object *object;
	struct oval_syschar *syschar;

	if (argc != 2) {
		printf("usage: %s <oval_definitions.xml>\n", argv[0]);
		return 1;
	}

	source = oval_import_source_new_file(argv[1]);
	definition_model = oval_definition_model_new();
	ret = oval_definition_model_import(definition_model, source, &_test_error_handler, NULL);
	if(ret != 1) {
		printf("oval_definition_model_import() failed.\n");
		return 2;
	}
	oval_import_source_free(source);
	syschar_model = oval_syschar_model_new(definition_model);

	obj_it = oval_definition_model_get_objects(definition_model);
	if (!oval_object_iterator_has_more(obj_it)) {
		printf("No definitions found.\n");
		return 3;
	}

	probe_dir = getenv ("OVAL_PROBE_DIR");
	if (probe_dir == NULL) {
		printf("'OVAL_PROBE_DIR' not found in environment.\n");
		return 4;
	}
	printf("probe_dir: %s.\n", probe_dir);
        pctx = oval_pctx_new (syschar_model);
	if (pctx == NULL) {
		printf("oval_pctx_new() failed.\n");
		return 5;
	}
        oval_pctx_setattr (pctx, OVAL_PCTX_ATTR_DIR, probe_dir);

	for (idx = 1; oval_object_iterator_has_more(obj_it); idx++) {
		object = oval_object_iterator_next(obj_it);
		id = oval_object_get_id(object);

		printf("=== Object definition ===\n");
		oval_object_to_print(object, "    ", idx);

		syschar = oval_syschar_model_get_syschar(syschar_model, id);

		if (syschar == NULL) {
			printf("=== Calling probe on object ===\n");
			syschar = oval_probe_object_eval(pctx, object);

			if (syschar == NULL) {
				syschar = oval_syschar_new(syschar_model, object);
				oval_syschar_set_flag(syschar, SYSCHAR_FLAG_NOT_COLLECTED);
			}

			oval_syschar_model_add_syschar(syschar_model, syschar);
		}

		printf("=== System characteristics ===\n");
		oval_syschar_to_print(syschar, "    ", idx);
	}

        oval_pctx_free (pctx);
	oval_object_iterator_free(obj_it);
        oval_definition_model_free(definition_model);
        oval_syschar_model_free(syschar_model);


	return 0;
}

void oval_syschar_to_print(struct oval_syschar *syschar, const char *indent, int idx)
{
       char nxtindent[100];

       if (strlen(indent) > 80)
               indent = "....";
       if (idx == 0)
               snprintf(nxtindent, sizeof(nxtindent), "%sSYSCHAR.", indent);
       else
               snprintf(nxtindent, sizeof(nxtindent), "%sSYSCHAR[%d].", indent, idx);

       /*
       oval_syschar_collection_flag_enum flag;
       struct oval_collection *messages;
       struct oval_sysinfo *sysinfo;
       struct oval_object *object;
       struct oval_collection *sysdata;
        */
       printf("%sFLAG    = %d\n", nxtindent, oval_syschar_get_flag(syschar));
      {//messages
               struct oval_message_iterator *messages = oval_syschar_get_messages(syschar);
               int i;for(i=1;oval_message_iterator_has_more(messages);i++){
                       struct oval_message *message = oval_message_iterator_next(messages);
                       oval_message_to_print(message, nxtindent, i);
               }
               oval_message_iterator_free(messages);
       }
       {//object
               struct oval_object *object = oval_syschar_get_object(syschar);
               if (object) oval_object_to_print(object, nxtindent, 0);
       }
       {//sysdata
               struct oval_sysdata_iterator *sysdatas = oval_syschar_get_sysdata(syschar);
               int hasMore = oval_sysdata_iterator_has_more(sysdatas);
               if(hasMore){
                       int i;for(i=1;oval_sysdata_iterator_has_more(sysdatas);i++){
                               struct oval_sysdata *sysdata = oval_sysdata_iterator_next(sysdatas);
                               oval_sysdata_to_print(sysdata, nxtindent, i);
                      }
               }
               oval_sysdata_iterator_free(sysdatas);
       }
}

