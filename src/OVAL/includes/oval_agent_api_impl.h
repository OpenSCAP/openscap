/*
 * oval_agent_api_impl.h
 *
 *  Created on: Mar 9, 2009
 *      Author: Compaq_Administrator
 */

#ifndef OVAL_AGENT_API_IMPL_H_
#define OVAL_AGENT_API_IMPL_H_
#include <liboval/oval_agent_api.h>
#include "oval_definitions_impl.h"
	void add_oval_definition(struct oval_object_model_s*, struct oval_definition_s*);
	void add_oval_test      (struct oval_object_model_s*, struct oval_test_s*      );
	void add_oval_object    (struct oval_object_model_s*, struct oval_object_s*    );
	void add_oval_state     (struct oval_object_model_s*, struct oval_state_s*     );
	void add_oval_variable  (struct oval_object_model_s*, struct oval_variable_s*  );

	struct oval_definition_s *get_oval_definition_new(struct oval_object_model_s*,char*);
	struct oval_test_s       *get_oval_test_new      (struct oval_object_model_s*,char*);
	struct oval_object_s     *get_oval_object_new    (struct oval_object_model_s*,char*);
	struct oval_state_s      *get_oval_state_new     (struct oval_object_model_s*,char*);
	struct oval_variable_s   *get_oval_variable_new  (struct oval_object_model_s*,char*);
#endif /* OVAL_AGENT_API_IMPL_H_ */
