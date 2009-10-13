/*
 * test_probes.c
 *
 *  Created on: Aug 4, 2009
 *      Author: Peter Vrabec
 */

#include <stdio.h>
#include <stdlib.h>
#include "api/oval_agent_api.h"

int _test_error_handler(struct oval_xml_error *error, void *null)
{
	//ERROR HANDLING IS TODO
	return 1;
}

void oval_syschar_to_print(struct oval_syschar*, const char*, int);

int main(int argc, char **argv)
{
	struct oval_definition_model *model = oval_definition_model_new();

	struct oval_import_source *is = oval_import_source_new_file(argv[1]);
	oval_definition_model_load(model, is, &_test_error_handler, NULL);
	oval_import_source_free(is);

	struct oval_object_iterator *objects = oval_definition_model_get_objects(model);
	if (!oval_object_iterator_has_more(objects)) {
		printf("NO DEFINITIONS FOUND\n");
		return 1;
	}

	int index;
	for (index = 1; oval_object_iterator_has_more(objects); index++) {
		struct oval_object *object = oval_object_iterator_next(objects);
		oval_object_to_print(object, "    ", index);
		printf("Callin probe on object\n");
		struct oval_syschar* syschar = oval_object_probe(object, model);
		printf("System characteristics:\n");
		if (syschar)
			oval_syschar_to_print(syschar, "    ", index);
		else
			printf("(NULL)\n");
	}

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
               struct oval_sysdata_iterator *sysdatas = oval_syschar_sysdata(syschar);
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

