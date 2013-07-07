/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "public/oval_definitions.h"
#include "public/oval_system_characteristics.h"
#include "oval_system_characteristics_impl.h"
#include "oval_probe_impl.h"
#include "_oval_probe_session.h"

static int _oval_probe_hint_criteria(oval_probe_session_t *sess, struct oval_criteria_node *cnode, int variable_instance_hint);
static int _oval_probe_hint_object(oval_probe_session_t *psess, struct oval_object *object, int variable_instance_hint);

/**
 * Finds all the oval_syschars (collected objects) assigned with a given definition
 * and sets the variable_instance_hint attribute thereof. That is to mark these
 * collected objects with the hint that a new round of collection might be needed
 * when these objects are again probed by @ref oval_probe_query_object. That is
 * usefull when a new variable instance is injected into the oval_agent_session.
 * @param variable_instance_hint new hint to set
 * @returns 0 on success; -1 on error; 1 on warning
 */
int oval_probe_hint_definition(oval_probe_session_t *sess, struct oval_definition *definition, int variable_instance_hint)
{
	if (definition == NULL)
		return -1;
	struct oval_criteria_node *cnode = oval_definition_get_criteria(definition);
	if (cnode == NULL)
		return -1;

	return _oval_probe_hint_criteria(sess, cnode, variable_instance_hint);
}

int _oval_probe_hint_criteria(oval_probe_session_t *sess, struct oval_criteria_node *cnode, int variable_instance_hint)
{
	switch (oval_criteria_node_get_type(cnode)) {
	case OVAL_NODETYPE_CRITERION:{
		struct oval_test *test = oval_criteria_node_get_test(cnode);
		if (test == NULL)
			return 0;
		struct oval_object *object = oval_test_get_object(test);
		if (object == NULL)
			return 0;
		// TODO: Do we need to similarly hint all the object refereced like: test->state->variable->object?
		return _oval_probe_hint_object(sess, object, variable_instance_hint);
	}
	case OVAL_NODETYPE_CRITERIA:{
		struct oval_criteria_node_iterator *cnode_it = oval_criteria_node_get_subnodes(cnode);
		if (cnode_it == NULL)
			return 0;
		int ret = 0;
		while (ret == 0 && oval_criteria_node_iterator_has_more(cnode_it)) {
			struct oval_criteria_node *node = oval_criteria_node_iterator_next(cnode_it);
			ret = _oval_probe_hint_criteria(sess, node, variable_instance_hint);
		}
		oval_criteria_node_iterator_free(cnode_it);
		return ret;
	}
	case OVAL_NODETYPE_EXTENDDEF:{
		struct oval_definition *oval_def = oval_criteria_node_get_definition(cnode);
		return oval_probe_hint_definition(sess, oval_def, variable_instance_hint);
	}
	case OVAL_NODETYPE_UNKNOWN:{
		assert(false);
		return -1;
	}
	}
	return -1;
}

int _oval_probe_hint_object(oval_probe_session_t *psess, struct oval_object *object, int variable_instance_hint)
{
	const char *oid = oval_object_get_id(object);
	struct oval_syschar *syschar = oval_syschar_model_get_syschar(psess->sys_model, oid);
	if (syschar != NULL) {
		oval_syschar_set_variable_instance_hint(syschar, variable_instance_hint);
	}
	return 0;
}
