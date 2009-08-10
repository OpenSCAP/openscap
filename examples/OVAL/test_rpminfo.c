#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "api/oval_agent_api.h"


int _test_error_handler(struct oval_xml_error *error, void *null)
{
        return 1;
}

int main(int argc, char **argv)
{
	struct oval_object_model *model;
	struct import_source     *isrc;
        struct oval_iterator_definition *defs;
	
        setbuf (stdout, NULL);

        model = oval_object_model_new ();
        isrc  = import_source_file(argv[1]);
        
	load_oval_definitions(model, isrc, &_test_error_handler, NULL);
	import_source_free(isrc);
        
	defs = get_oval_definitions(model);

        while (oval_iterator_definition_has_more (defs)) {
                struct oval_criteria_node *crit;
                struct oval_definition    *def;
                struct  oval_iterator_criteria_node *cit;
                
                def  = oval_iterator_definition_next (defs);
                crit = oval_definition_criteria (def);
                cit  = oval_criteria_node_subnodes (crit);
                
                while (oval_iterator_criteria_node_has_more (cit)) {
                        struct oval_criteria_node *subcrit;
         
                        subcrit = oval_iterator_criteria_node_next (cit);
                        
                        if (oval_criteria_node_type (subcrit) == NODETYPE_CRITERION) {
                                struct oval_test *test;
                                
                                test = oval_criteria_node_test (subcrit);
                                switch (oval_test_subtype (test)) {
                                case LINUX_RPM_INFO:
                                case UNIX_RUNLEVEL: {
                                        struct oval_object *obj;
                                        
                                        obj = oval_test_object (test);
                                        printf ("obj: id=%s\n", oval_object_id (obj));
                                        
                                        /* Hooray! */
                                        probe_object (obj, NULL);
                                        printf ("obj: errno=%u, %s.\n", errno, strerror (errno));
                                }
                                        break;
                                default:
                                        continue;
                                }
                        }
                }
        }
        
	return 0;
}

