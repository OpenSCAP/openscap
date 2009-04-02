/*
 * oval_agent_api_impl.h
 *
 *  Created on: Mar 9, 2009
 *      Author: Compaq_Administrator
 */

#ifndef OVAL_AGENT_API_IMPL_H_
#define OVAL_AGENT_API_IMPL_H_
#include "api/oval_agent_api.h"
#include "oval_definitions_impl.h"
void add_oval_definition(struct oval_object_model *, struct oval_definition *);
void add_oval_test(struct oval_object_model *, struct oval_test *);
void add_oval_object(struct oval_object_model *, struct oval_object *);
void add_oval_state(struct oval_object_model *, struct oval_state *);
void add_oval_variable(struct oval_object_model *, struct oval_variable *);

struct oval_definition *get_oval_definition_new(struct oval_object_model *,
						char *);
struct oval_test *get_oval_test_new(struct oval_object_model *, char *);
struct oval_object *get_oval_object_new(struct oval_object_model *, char *);
struct oval_state *get_oval_state_new(struct oval_object_model *, char *);
struct oval_variable *get_oval_variable_new(struct oval_object_model *, char *);
#endif				/* OVAL_AGENT_API_IMPL_H_ */
