#ifndef OVAL_DEFINITIONS_IMPL
#define OVAL_DEFINITIONS_IMPL
#include <liboval/oval_definitions.h>

	OvalCollection_affected        newOvalCollection_affected       (Oval_affected*);
	OvalCollection_test            newOvalCollection_test           (Oval_test*);
	OvalCollection_criteriaNode    newOvalCollection_criteriaNode   (Oval_criteriaNode*);
	OvalCollection_reference       newOvalCollection_reference      (Oval_reference*);
	OvalCollection_definition      newOvalCollection_definition     (Oval_definition*);
	OvalCollection_object          newOvalCollection_object         (Oval_object*);
	OvalCollection_state           newOvalCollection_state          (Oval_state*);
	OvalCollection_variable        newOvalCollection_variable       (Oval_variable*);
	OvalCollection_variableBinding newOvalCollection_variableBinding(Oval_variableBinding*);
	OvalCollection_objectContent   newOvalCollection_objectContent  (Oval_objectContent*);
	OvalCollection_behavior        newOvalCollection_behavior       (Oval_behavior*);
	OvalCollection_entity          newOvalCollection_entity         (Oval_entity*);
	OvalCollection_set             newOvalCollection_set            (Oval_set*);
	OvalCollection_value           newOvalCollection_value          (Oval_value*);
	OvalCollection_component       newOvalCollection_component      (Oval_component*);
#endif
