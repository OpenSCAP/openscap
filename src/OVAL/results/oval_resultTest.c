/**
 * @file oval_resultTest.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 *      "Peter Vrabec" <pvrabec@redhat.com>
 *      Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <ctype.h>
#include "oval_agent_api_impl.h"
#include "oval_probe_impl.h"
#include "results/oval_results_impl.h"
#include "results/oval_status_counter.h"
#include "oval_cmp_impl.h"
#include "adt/oval_collection_impl.h"
#include "adt/oval_string_map_impl.h"
#include "collectVarRefs_impl.h"
#include "public/oval_types.h"
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"

typedef struct oval_result_test {
	struct oval_result_system *system;
	struct oval_test *test;
	oval_result_t result;
	struct oval_collection *messages;
	struct oval_collection *items;
	struct oval_collection *bindings;
	int instance;
	bool bindings_initialized;
} oval_result_test_t;

struct oval_result_test *oval_result_test_new(struct oval_result_system *sys, char *tstid)
{
	oval_result_test_t *test = (oval_result_test_t *)
	    malloc(sizeof(oval_result_test_t));
	if (test == NULL)
		return NULL;

	struct oval_syschar_model *syschar_model = oval_result_system_get_syschar_model(sys);
	struct oval_definition_model *definition_model = oval_syschar_model_get_definition_model(syschar_model);
	test->system = sys;
	test->test = oval_definition_model_get_new_test(definition_model, tstid);
	test->messages = oval_collection_new();
	test->result = OVAL_RESULT_NOT_EVALUATED;
	test->instance = 1;
	test->items = oval_collection_new();
	test->bindings = oval_collection_new();
	test->bindings_initialized = false;
	return test;
}

struct oval_result_test *oval_result_test_clone
    (struct oval_result_system *new_system, struct oval_result_test *old_test) {
	__attribute__nonnull__(old_test);

	const char *testid = oval_result_test_get_id(old_test);
	struct oval_result_test *new_test = oval_result_test_new(new_system, (char *) testid);
	struct oval_result_item_iterator *old_items = oval_result_test_get_items(old_test);
	while (oval_result_item_iterator_has_more(old_items)) {
		struct oval_result_item *old_item = oval_result_item_iterator_next(old_items);
		struct oval_result_item *new_item = oval_result_item_clone(new_system, old_item);
		oval_result_test_add_item(new_test, new_item);
	}
	oval_result_item_iterator_free(old_items);

	struct oval_variable_binding_iterator *old_bindings = oval_result_test_get_bindings(old_test);
	while (oval_variable_binding_iterator_has_more(old_bindings)) {
		struct oval_variable_binding *old_binding = oval_variable_binding_iterator_next(old_bindings);
		struct oval_variable_binding *new_binding = oval_variable_binding_clone(old_binding, NULL);
		oval_result_test_add_binding(new_test, new_binding);
	}
	oval_variable_binding_iterator_free(old_bindings);

	struct oval_message_iterator *old_messages = oval_result_test_get_messages(old_test);
	while (oval_message_iterator_has_more(old_messages)) {
		struct oval_message *old_message = oval_message_iterator_next(old_messages);
		struct oval_message *new_message = oval_message_clone(old_message);
		oval_result_test_add_message(new_test, new_message);
	}
	oval_message_iterator_free(old_messages);

	oval_result_test_set_instance(new_test, oval_result_test_get_instance(old_test));
	oval_result_test_set_result(new_test, old_test->result);

	return new_test;
}

struct oval_result_test *make_result_test_from_oval_test(struct oval_result_system *sys, struct oval_test *oval_test, int variable_instance) {
	char *test_id = oval_test_get_id(oval_test);
	struct oval_result_test *test = oval_result_test_new(sys, test_id);
	oval_result_test_set_instance(test, variable_instance);
	return test;
}

void oval_result_test_free(struct oval_result_test *test)
{
	__attribute__nonnull__(test);

	oval_collection_free_items(test->messages, (oscap_destruct_func) oval_message_free);
	oval_collection_free_items(test->items, (oscap_destruct_func) oval_result_item_free);
	oval_collection_free_items(test->bindings, (oscap_destruct_func) oval_variable_binding_free);

	test->system = NULL;
	test->test = NULL;
	test->messages = NULL;
	test->result = OVAL_RESULT_NOT_EVALUATED;
	test->items = NULL;
	test->bindings = NULL;
	test->instance = 1;
	free(test);
}

struct oval_result_system *oval_result_test_get_system(struct oval_result_test *rtest)
{
	__attribute__nonnull__(rtest);

	return rtest->system;
}

struct oval_test *oval_result_test_get_test(struct oval_result_test *rtest)
{
	__attribute__nonnull__(rtest);

	return ((struct oval_result_test *)rtest)->test;
}

int ores_add_res(struct oresults *ores, oval_result_t res)
{
		switch (res) {
		case OVAL_RESULT_TRUE:
			ores->true_cnt++;
			break;
		case OVAL_RESULT_FALSE:
			ores->false_cnt++;
			break;
		case OVAL_RESULT_UNKNOWN:
			ores->unknown_cnt++;
			break;
		case OVAL_RESULT_ERROR:
			ores->error_cnt++;
			break;
		case OVAL_RESULT_NOT_EVALUATED:
			ores->noteval_cnt++;
			break;
		case OVAL_RESULT_NOT_APPLICABLE:
			ores->notappl_cnt++;
			break;
		default:
			oscap_seterr(OSCAP_EFAMILY_OSCAP, "Invalid oval result type: %d.", res);
			return 1;
		}

		return 0;
}

void ores_clear(struct oresults *ores)
{
	memset(ores, 0, sizeof (*ores));
}

oval_result_t ores_get_result_bychk(struct oresults *ores, oval_check_t check)
{
	oval_result_t result = OVAL_RESULT_ERROR;

	if (ores->true_cnt == 0 &&
	    ores->false_cnt == 0 &&
	    ores->error_cnt == 0 &&
	    ores->unknown_cnt == 0 &&
	    ores->notappl_cnt == 0 &&
	    ores->noteval_cnt == 0)
		return OVAL_RESULT_UNKNOWN;

	if (ores->notappl_cnt > 0 &&
	    ores->noteval_cnt == 0 &&
	    ores->false_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->true_cnt == 0)
		return OVAL_RESULT_NOT_APPLICABLE;

	switch (check) {
	case OVAL_CHECK_ALL:
		if (ores->true_cnt > 0 &&
		    ores->false_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores->false_cnt > 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores->false_cnt == 0 && ores->error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores->false_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores->false_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OVAL_CHECK_AT_LEAST_ONE:
		if (ores->true_cnt > 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores->false_cnt > 0 &&
			   ores->true_cnt == 0 &&
			   ores->unknown_cnt == 0 && ores->error_cnt == 0 && ores->noteval_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores->true_cnt == 0 && ores->error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores->false_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores->false_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OVAL_CHECK_NONE_EXIST:
		dW("The 'none exist' CheckEnumeration value has been deprecated. "
		   "Converted to check='none satisfy'.");
		/* FALLTHROUGH */
	case OVAL_CHECK_NONE_SATISFY:
		if (ores->true_cnt > 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores->true_cnt == 0 && ores->error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores->true_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores->true_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		} else if (ores->false_cnt > 0 &&
			   ores->error_cnt == 0 &&
			   ores->unknown_cnt == 0 && ores->noteval_cnt == 0 && ores->true_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		}
		break;
	case OVAL_CHECK_ONLY_ONE:
		if (ores->true_cnt == 1 && ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores->true_cnt > 1) {
			result = OVAL_RESULT_FALSE;
		} else if (ores->true_cnt < 2 && ores->error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores->true_cnt < 2 && ores->error_cnt == 0 && ores->unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores->true_cnt < 2 && ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		} else if (ores->true_cnt != 1 && ores->false_cnt > 0) {
			result = OVAL_RESULT_FALSE;
		}
		break;
	default:
                oscap_seterr(OSCAP_EFAMILY_OSCAP, "Invalid check value: %d.", check);
		result = OVAL_RESULT_ERROR;
	}

	return result;
}

oval_result_t ores_get_result_byopr(struct oresults *ores, oval_operator_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;

	if (ores->true_cnt == 0 &&
	    ores->false_cnt == 0 &&
	    ores->error_cnt == 0 &&
	    ores->unknown_cnt == 0 &&
	    ores->notappl_cnt == 0 &&
	    ores->noteval_cnt == 0)
		return OVAL_RESULT_UNKNOWN;

	if (ores->notappl_cnt > 0 &&
	    ores->noteval_cnt == 0 &&
	    ores->false_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->true_cnt == 0)
		return OVAL_RESULT_NOT_APPLICABLE;

	switch (op) {
	case OVAL_OPERATOR_AND:
		if (ores->true_cnt > 0 &&
		    ores->false_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores->false_cnt > 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores->false_cnt == 0 && ores->error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores->false_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores->false_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OVAL_OPERATOR_ONE:
		if (ores->true_cnt == 1 &&
		    ores->false_cnt >= 0 &&
		    ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt == 0 && ores->notappl_cnt >= 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores->true_cnt >= 2 &&
			   ores->false_cnt >= 0 &&
			   ores->error_cnt >= 0 &&
			   ores->unknown_cnt >= 0 && ores->noteval_cnt >= 0 && ores->notappl_cnt >= 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores->true_cnt == 0 &&
			   ores->false_cnt >= 0 &&
			   ores->error_cnt == 0 &&
			   ores->unknown_cnt == 0 && ores->noteval_cnt == 0 && ores->notappl_cnt >= 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores->true_cnt < 2 &&
			   ores->false_cnt >= 0 &&
			   ores->error_cnt > 0 &&
			   ores->unknown_cnt >= 0 && ores->noteval_cnt >= 0 && ores->notappl_cnt >= 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores->true_cnt < 2 &&
			   ores->false_cnt >= 0 &&
			   ores->error_cnt == 0 &&
			   ores->unknown_cnt >= 1 && ores->noteval_cnt >= 0 && ores->notappl_cnt >= 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores->true_cnt < 2 &&
			   ores->false_cnt >= 0 &&
			   ores->error_cnt == 0 &&
			   ores->unknown_cnt == 0 && ores->noteval_cnt > 0 && ores->notappl_cnt >= 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OVAL_OPERATOR_OR:
		if (ores->true_cnt > 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores->true_cnt == 0 &&
			   ores->false_cnt > 0 &&
			   ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores->true_cnt == 0 && ores->error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores->true_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores->true_cnt == 0 && ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OVAL_OPERATOR_XOR:
		if ((ores->true_cnt % 2) == 1 && ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if ((ores->true_cnt % 2) == 0 &&
			   ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores->error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores->error_cnt == 0 && ores->unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores->error_cnt == 0 && ores->unknown_cnt == 0 && ores->noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	default:
                oscap_seterr(OSCAP_EFAMILY_OSCAP, "Invalid operator value: %d.", op);
		result = OVAL_RESULT_ERROR;
		break;
	}

	return result;
}

static inline oval_result_t _evaluate_sysent_with_variable(struct oval_syschar_model *syschar_model, struct oval_entity *state_entity, struct oval_sysent *item_entity, oval_operation_t state_entity_operation, struct oval_state_content *content)
{
	oval_syschar_collection_flag_t flag;
	oval_result_t ent_val_res;

	struct oval_variable *state_entity_var;
	if ((state_entity_var = oval_entity_get_variable(state_entity)) == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "OVAL internal error: found NULL variable");
		return -1;
	}

	if (0 != oval_syschar_model_compute_variable(syschar_model, state_entity_var)) {
		return -1;
	}

	flag = oval_variable_get_collection_flag(state_entity_var);
	switch (flag) {
	case SYSCHAR_FLAG_COMPLETE:
	case SYSCHAR_FLAG_INCOMPLETE:{
		struct oresults var_ores;
		struct oval_value_iterator *val_itr;

		ores_clear(&var_ores);

		val_itr = oval_variable_get_values(state_entity_var);
		while (oval_value_iterator_has_more(val_itr)) {
			struct oval_value *var_val;
			char *state_entity_val_text = NULL;
			oval_result_t var_val_res;

			var_val = oval_value_iterator_next(val_itr);
			state_entity_val_text = oval_value_get_text(var_val);
			if (state_entity_val_text == NULL) {
				dE("Found NULL variable value text.");
				ores_add_res(&var_ores, OVAL_RESULT_ERROR);
				break;
			}
			oval_datatype_t state_entity_val_datatype = oval_value_get_datatype(var_val);

			var_val_res = oval_ent_cmp_str(state_entity_val_text, state_entity_val_datatype, item_entity, state_entity_operation);
			if (var_val_res == OVAL_RESULT_ERROR) {
				dE("Error occured when comparing a variable '%s' value '%s' with collected item entity = '%s'",
					oval_variable_get_id(state_entity_var), state_entity_val_text, oval_sysent_get_value(item_entity));
			}
			ores_add_res(&var_ores, var_val_res);
		}
		oval_value_iterator_free(val_itr);

		oval_check_t var_check = oval_state_content_get_var_check(content);
		ent_val_res = ores_get_result_bychk(&var_ores, var_check);
		} break;
	case SYSCHAR_FLAG_ERROR:
	case SYSCHAR_FLAG_DOES_NOT_EXIST:
	case SYSCHAR_FLAG_NOT_COLLECTED:
	case SYSCHAR_FLAG_NOT_APPLICABLE:
		ent_val_res = OVAL_RESULT_ERROR;
		break;
	default:
		ent_val_res = -1;
	}

	return ent_val_res;
}

struct record_field_instance {
	char *name;
	char *value;
	oval_datatype_t data_type;
	oval_check_t ent_check;
};

static struct record_field_instance _oval_record_field_iterator_next_instance(struct oval_record_field_iterator *iterator)
{
	struct record_field_instance instance;
	struct oval_record_field *rf = oval_record_field_iterator_next(iterator);
	instance.name = oval_record_field_get_name(rf);
	instance.value = oval_record_field_get_value(rf);
	instance.data_type = oval_record_field_get_datatype(rf);
	if (oval_record_field_get_type(rf) == OVAL_RECORD_FIELD_STATE) {
		instance.ent_check = oval_record_field_get_ent_check(rf);
	}
	return instance;
}

static oval_result_t _evaluate_sysent_record(struct oval_state_content *state_content, struct oval_sysent *item_entity)
{
	struct oresults record_ores;
	ores_clear(&record_ores);
	/* During analysis of a system characteristics item, each record field is
	 * analyzed and then the overall result for elements of the record type is
	 * computed by logically ANDing the results for each field and applying
	 * the entity_check attribute.
	 */
	struct oval_record_field_iterator *state_it = oval_state_content_get_record_fields(state_content);
	while (oval_record_field_iterator_has_more(state_it)) {
		struct record_field_instance state_rf = _oval_record_field_iterator_next_instance(state_it);
		bool field_found = false;
		struct oresults field_ores;
		ores_clear(&field_ores);
		struct oval_record_field_iterator *item_it = oval_sysent_get_record_fields(item_entity);
		while (oval_record_field_iterator_has_more(item_it)) {
			struct record_field_instance item_rf = _oval_record_field_iterator_next_instance(item_it);
			if (strcmp(state_rf.name, item_rf.name) == 0) {
				field_found = true;
				oval_result_t fields_comparison_result = oval_str_cmp_str(state_rf.value, state_rf.data_type, item_rf.value, OVAL_OPERATION_EQUALS);
				ores_add_res(&field_ores, fields_comparison_result);
			}
		}
		oval_record_field_iterator_free(item_it);
		/* When analyzing system characteristics an error should be reported
		 * for the result of a field that is present in the OVAL State, but
		 * not found in the system characteristics item.
		 */
		if (!field_found) {
			ores_add_res(&record_ores, OVAL_RESULT_ERROR);
		} else {
			oval_result_t field_result = ores_get_result_bychk(&field_ores, state_rf.ent_check);
			ores_add_res(&record_ores, field_result);
		}
	}
	oval_record_field_iterator_free(state_it);
	return ores_get_result_byopr(&record_ores, OVAL_OPERATOR_AND);
}

static inline oval_result_t _evaluate_sysent(struct oval_syschar_model *syschar_model, struct oval_sysent *item_entity, struct oval_entity *state_entity, oval_operation_t state_entity_operation, struct oval_state_content *content)
{
	if (oval_sysent_get_status(item_entity) == SYSCHAR_STATUS_DOES_NOT_EXIST) {
		return OVAL_RESULT_FALSE;
	} else if (oval_entity_get_varref_type(state_entity) == OVAL_ENTITY_VARREF_ATTRIBUTE) {

		return _evaluate_sysent_with_variable(syschar_model,
				state_entity, item_entity,
				state_entity_operation, content);
	} else {
		struct oval_value *state_entity_val;
		char *state_entity_val_text;
		oval_datatype_t state_entity_val_datatype;

		oval_datatype_t state_entity_type = oval_entity_get_datatype(state_entity);
		if (state_entity_type == OVAL_DATATYPE_RECORD) {
			if (state_entity_operation != OVAL_OPERATION_EQUALS) {
				dE("The only allowed operation for comparing record types is 'equals'.");
				return OVAL_RESULT_ERROR;
			}
			return _evaluate_sysent_record(content, item_entity);
		} else {
			if ((state_entity_val = oval_entity_get_value(state_entity)) == NULL) {
				oscap_seterr(OSCAP_EFAMILY_OVAL, "OVAL internal error: found NULL entity value");
				return -1;
			}
			if ((state_entity_val_text = oval_value_get_text(state_entity_val)) == NULL) {
				oscap_seterr(OSCAP_EFAMILY_OVAL, "OVAL internal error: found NULL entity value text");
				return -1;
			}
			state_entity_val_datatype = oval_value_get_datatype(state_entity_val);

			return oval_ent_cmp_str(state_entity_val_text, state_entity_val_datatype, item_entity, state_entity_operation);
		}
	}
}

static oval_result_t eval_item(struct oval_syschar_model *syschar_model, struct oval_sysitem *cur_sysitem, struct oval_state *state)
{
	struct oval_state_content_iterator *state_contents_itr;
	struct oresults ste_ores;
	oval_operator_t operator;
	oval_result_t result = OVAL_RESULT_ERROR;

	ores_clear(&ste_ores);

	state_contents_itr = oval_state_get_contents(state);
	while (oval_state_content_iterator_has_more(state_contents_itr)) {
		struct oval_state_content *content;
		struct oval_entity *state_entity;
		char *state_entity_name;
		oval_operation_t state_entity_operation;
		oval_check_t entity_check;
		oval_existence_t check_existence;
		oval_result_t ste_ent_res;
		struct oval_sysent_iterator *item_entities_itr;
		struct oresults ent_ores;
		struct oval_status_counter counter;
		bool found_matching_item;

		if ((content = oval_state_content_iterator_next(state_contents_itr)) == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OVAL, "OVAL internal error: found NULL state content");
			goto fail;
		}
		if ((state_entity = oval_state_content_get_entity(content)) == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OVAL, "OVAL internal error: found NULL entity");
			goto fail;
		}
		if ((state_entity_name = oval_entity_get_name(state_entity)) == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OVAL, "OVAL internal error: found NULL entity name");
			goto fail;
		}

		if (oscap_streq(state_entity_name, "line") &&
			oval_state_get_subtype(state) == (oval_subtype_t) OVAL_INDEPENDENT_TEXT_FILE_CONTENT) {
			/* Hack: textfilecontent_state/line shall be compared against textfilecontent_item/text.
			 *
			 * textfilecontent_test and textfilecontent54_test share the same syschar
			 * (textfilecontent_item). In OVAL 5.3 and below this syschar did not hold any usable
			 * information ('text' ent). In OVAL 5.4 textfilecontent_test was deprecated. But the
			 * 'text' ent has been added to textfilecontent_item, making it potentially usable. */
			oval_schema_version_t over = oval_state_get_platform_schema_version(state);
			if (oval_schema_version_cmp(over, OVAL_SCHEMA_VERSION(5.4)) >= 0) {
				/* The OVAL-5.3 does not have textfilecontent_item/text */
				state_entity_name = "text";
			}
		}

		entity_check = oval_state_content_get_ent_check(content);
		check_existence = oval_state_content_get_check_existence(content);
		state_entity_operation = oval_entity_get_operation(state_entity);

		ores_clear(&ent_ores);
		found_matching_item = false;
		oval_status_counter_clear(&counter);

		item_entities_itr = oval_sysitem_get_sysents(cur_sysitem);
		while (oval_sysent_iterator_has_more(item_entities_itr)) {
			struct oval_sysent *item_entity;
			oval_result_t ent_val_res;
			char *item_entity_name;
			oval_syschar_status_t item_status;

			item_entity = oval_sysent_iterator_next(item_entities_itr);
			if (item_entity == NULL) {
				oscap_seterr(OSCAP_EFAMILY_OVAL, "OVAL internal error: found NULL sysent");
				oval_sysent_iterator_free(item_entities_itr);
				goto fail;
			}
			item_status = oval_sysent_get_status(item_entity);
			oval_status_counter_add_status(&counter, item_status);

			item_entity_name = oval_sysent_get_name(item_entity);
			if (strcmp(item_entity_name, state_entity_name))
				continue;

			found_matching_item = true;

			/* copy mask attribute from state to item */
			if (oval_entity_get_mask(state_entity))
				oval_sysent_set_mask(item_entity,1);

			ent_val_res = _evaluate_sysent(syschar_model, item_entity, state_entity,
					state_entity_operation, content);
			if (ent_val_res == OVAL_RESULT_TRUE) {
				dI("Entity '%s'='%s' of item '%s' matches corresponding entity in state '%s'.",
						oval_sysent_get_name(item_entity),
						oval_sysent_get_value(item_entity),
						oval_sysitem_get_id(cur_sysitem), oval_state_get_id(state));
			}
			if (ent_val_res == OVAL_RESULT_ERROR) {
				dI("Comparing entity '%s'='%s' of item '%s' to corresponding entity in state '%s' was not successful.",
						oval_sysent_get_name(item_entity),
						oval_sysent_get_value(item_entity),
						oval_sysitem_get_id(cur_sysitem), oval_state_get_id(state));
			}
			if (((signed) ent_val_res) == -1) {
				oval_sysent_iterator_free(item_entities_itr);
				goto fail;
			}

			ores_add_res(&ent_ores, ent_val_res);
		}
		oval_sysent_iterator_free(item_entities_itr);

		if (!found_matching_item)
			dW("Entity name '%s' from state (id: '%s') not found in item (id: '%s').",
			   state_entity_name, oval_state_get_id(state), oval_sysitem_get_id(cur_sysitem));

		ste_ent_res = ores_get_result_bychk(&ent_ores, entity_check);
		ores_add_res(&ste_ores, ste_ent_res);
		oval_result_t cres = oval_status_counter_get_result(&counter, check_existence);
		ores_add_res(&ste_ores, cres);
	}
	oval_state_content_iterator_free(state_contents_itr);

	operator = oval_state_get_operator(state);
	result = ores_get_result_byopr(&ste_ores, operator);
	dI("Item '%s' compared to state '%s' with result %s.",
			   oval_sysitem_get_id(cur_sysitem), oval_state_get_id(state),
			   oval_result_get_text(result));

	return result;

 fail:
	oval_state_content_iterator_free(state_contents_itr);

	return OVAL_RESULT_ERROR;
}

#define ITEMMAP (struct oval_string_map    *)args[2]
#define TEST    (struct oval_result_test   *)args[1]
#define SYSTEM  (struct oval_result_system *)args[0]

static void _oval_test_item_consumer(struct oval_result_item *item, void **args) {
	struct oval_sysitem *oval_sysitem = oval_result_item_get_sysitem(item);
	char *item_id = oval_sysitem_get_id(oval_sysitem);
	struct oval_result_item *mapped_item = oval_string_map_get_value(ITEMMAP, item_id);
	if (mapped_item == NULL) {
		oval_string_map_put(ITEMMAP, item_id, item);
		oval_result_test_add_item(TEST, item);
	} else {
		oval_result_item_free(item);
	}
}

static oval_result_t eval_check_state(struct oval_test *test, void **args)
{
	struct oval_syschar_model *syschar_model;
	struct oval_result_item_iterator *ritems_itr;
	struct oresults item_ores;
	oval_result_t result;
	oval_check_t ste_check;
	oval_operator_t ste_opr;

	ste_check = oval_test_get_check(test);
	ste_opr = oval_test_get_state_operator(test);
	syschar_model = oval_result_system_get_syschar_model(SYSTEM);
	ores_clear(&item_ores);

	char *state_names = oval_test_get_state_names(test);
	if (state_names) {
		dI("In test '%s' %s of the collected items must satisfy these states: %s.",
			oval_test_get_id(test), oval_check_get_description(ste_check), state_names);
		free(state_names);
	}

	ritems_itr = oval_result_test_get_items(TEST);
	while (oval_result_item_iterator_has_more(ritems_itr)) {
		struct oval_result_item *ritem;
		struct oval_sysitem *item;
		oval_syschar_status_t item_status;
		struct oresults ste_ores;
		struct oval_state_iterator *ste_itr;
		oval_result_t item_res;

		ritem = oval_result_item_iterator_next(ritems_itr);
		item = oval_result_item_get_sysitem(ritem);

		item_status = oval_sysitem_get_status(item);
		switch (item_status) {
		case SYSCHAR_STATUS_ERROR:
		case SYSCHAR_STATUS_NOT_COLLECTED:
			item_res = OVAL_RESULT_ERROR;
			ores_add_res(&item_ores, item_res);
			oval_result_item_set_result(ritem, item_res);
			continue;
		case SYSCHAR_STATUS_DOES_NOT_EXIST:
			item_res = OVAL_RESULT_FALSE;
			ores_add_res(&item_ores, item_res);
			oval_result_item_set_result(ritem, item_res);
			continue;
		default:
			break;
		}

		ores_clear(&ste_ores);

		ste_itr = oval_test_get_states(test);
		while (oval_state_iterator_has_more(ste_itr)) {
			struct oval_state *ste;
			oval_result_t ste_res;

			ste = oval_state_iterator_next(ste_itr);
			ste_res = eval_item(syschar_model, item, ste);
			ores_add_res(&ste_ores, ste_res);
		}
		oval_state_iterator_free(ste_itr);

		item_res = ores_get_result_byopr(&ste_ores, ste_opr);
		ores_add_res(&item_ores, item_res);
		oval_result_item_set_result(ritem, item_res);
	}
	oval_result_item_iterator_free(ritems_itr);

	result = ores_get_result_bychk(&item_ores, ste_check);

	return result;
}

static oval_result_t eval_check_existence(oval_existence_t check_existence, int exists_cnt, int error_cnt)
{
	oval_result_t result = OVAL_RESULT_ERROR;

	switch (check_existence) {
	case OVAL_ALL_EXIST:
		if (exists_cnt >= 1 && error_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (exists_cnt == 0 && error_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (exists_cnt >= 0 && error_cnt >= 1) {
			result = OVAL_RESULT_ERROR;
		}
		break;
	case OVAL_ANY_EXIST:
		if (exists_cnt >= 0 && error_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (exists_cnt >= 1 && error_cnt >= 1) {
			result = OVAL_RESULT_TRUE;
		} else if (exists_cnt == 0 && error_cnt >= 1) {
			result = OVAL_RESULT_ERROR;
		}
		break;
	case OVAL_AT_LEAST_ONE_EXISTS:
		if (exists_cnt >= 1 && error_cnt >= 0) {
			result = OVAL_RESULT_TRUE;
		} else if (exists_cnt == 0 && error_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (exists_cnt == 0 && error_cnt >= 1) {
			result = OVAL_RESULT_ERROR;
		}
		break;
	case OVAL_NONE_EXIST:
		if (exists_cnt == 0 && error_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (exists_cnt >= 1 && error_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (exists_cnt == 0 && error_cnt >= 1) {
			result = OVAL_RESULT_ERROR;
		}
		break;
	case OVAL_ONLY_ONE_EXISTS:
		if (exists_cnt == 1 && error_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (exists_cnt >= 2 && error_cnt >= 0) {
			result = OVAL_RESULT_FALSE;
		} else if (exists_cnt == 0 && error_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (exists_cnt == 0 && error_cnt >= 1) {
			result = OVAL_RESULT_ERROR;
		} else if (exists_cnt == 1 && error_cnt >= 1) {
			result = OVAL_RESULT_ERROR;
		}
		break;
	default:
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Invalid check_existence value: %d.", check_existence);
                result = OVAL_RESULT_ERROR;
		break;
	}

	return result;
}

static oval_result_t
_oval_result_test_evaluate_items(struct oval_test *test, struct oval_syschar *syschar_object, void **args)
{
	struct oval_sysitem_iterator *collected_items_itr;
	oval_result_t result;
	int exists_cnt, error_cnt;
	bool hasstate;
	const char *test_id, *object_id, *flag_text;
	oval_check_t test_check;
	oval_existence_t test_check_existence;
	struct oval_state_iterator *ste_itr;
	oval_syschar_collection_flag_t flag;
	struct oval_object *object;

	exists_cnt = error_cnt = 0;
	test_id = oval_test_get_id(test);
	collected_items_itr = oval_syschar_get_sysitem(syschar_object);
	while (oval_sysitem_iterator_has_more(collected_items_itr)) {
		struct oval_sysitem *item;
		char *item_id;
		oval_syschar_status_t item_status;
		struct oval_result_item *ritem;

		item = oval_sysitem_iterator_next(collected_items_itr);
		if (item == NULL) {
			oscap_seterr(OSCAP_EFAMILY_OVAL, "Iterator returned null.");
			oval_sysitem_iterator_free(collected_items_itr);
			return OVAL_RESULT_ERROR;
		}

		item_status = oval_sysitem_get_status(item);
		if (item_status == SYSCHAR_STATUS_EXISTS)
			exists_cnt++;
		if (item_status == SYSCHAR_STATUS_ERROR)
			error_cnt++;

		item_id = oval_sysitem_get_id(item);
		ritem = oval_result_item_new(SYSTEM, item_id);
		oval_result_item_set_result(ritem, OVAL_RESULT_NOT_EVALUATED);
		_oval_test_item_consumer(ritem, args);
	}
	oval_sysitem_iterator_free(collected_items_itr);

	test_check = oval_test_get_check(test);
	test_check_existence = oval_test_get_existence(test);
	ste_itr = oval_test_get_states(test);
	hasstate = oval_state_iterator_has_more(ste_itr);
	oval_state_iterator_free(ste_itr);
	object = oval_syschar_get_object(syschar_object);
	object_id = object ? oval_object_get_id(object) : "<UNKNOWN>";

	switch (test_check_existence) {
	case OVAL_ALL_EXIST:
		dI("Test '%s' requires that every object defined by '%s' exists on the system.", test_id, object_id);
		break;
	case OVAL_ANY_EXIST:
		dI("Test '%s' requires that zero or more objects defined by '%s' exist on the system.", test_id, object_id);
		break;
	case OVAL_AT_LEAST_ONE_EXISTS:
		dI("Test '%s' requires that at least one object defined by '%s' exists on the system.", test_id, object_id);
		break;
	case OVAL_NONE_EXIST:
		dI("Test '%s' requires that none of the objects defined by '%s' exist on the system.", test_id, object_id);
		break;
	case OVAL_ONLY_ONE_EXISTS:
		dI("Test '%s' requires that only one object defined by '%s' exists on the system.", test_id, object_id);
		break;
	default:
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Check_existence parameter of test '%s' is unknown. This may indicate a bug in OpenSCAP.", test_id);
	}

	dI("%d objects defined by '%s' exist on the system.", exists_cnt, object_id);
	if (!hasstate) {
		dI("Test '%s' does not contain any state to compare object with.", test_id);
	}
	flag = oval_syschar_get_flag(syschar_object);
	flag_text = oval_syschar_collection_flag_get_text(flag);

	switch (flag) {
	case SYSCHAR_FLAG_ERROR:
		dI("An error occured while collecting items matching object '%s'. (flag=%s)", object_id, flag_text);
		if (test_check_existence == OVAL_ANY_EXIST
		    && !hasstate) {
			result = OVAL_RESULT_TRUE;
		} else {
			result = OVAL_RESULT_ERROR;
		}
		break;
	case SYSCHAR_FLAG_NOT_COLLECTED:
		dI("No attempt was made to collect items matching object '%s'. (flag=%s)", object_id, flag_text);
		if (test_check_existence == OVAL_ANY_EXIST
		    && !hasstate) {
			result = OVAL_RESULT_TRUE;
		} else {
			result = OVAL_RESULT_UNKNOWN;
		}
		break;
	case SYSCHAR_FLAG_NOT_APPLICABLE:
		dI("Object '%s' is not applicable to the system. (flag=%s)", object_id, flag_text);
		if (test_check_existence == OVAL_ANY_EXIST
		    && !hasstate) {
			result = OVAL_RESULT_TRUE;
		} else {
			result = OVAL_RESULT_NOT_APPLICABLE;
		}
		break;
	case SYSCHAR_FLAG_DOES_NOT_EXIST:
		dI("No item matching object '%s' was found on the system. (flag=%s)", object_id, flag_text);
		if (test_check_existence == OVAL_NONE_EXIST
		    || test_check_existence == OVAL_ANY_EXIST) {
			result = OVAL_RESULT_TRUE;
		} else {
			result = OVAL_RESULT_FALSE;
		}
		break;
	case SYSCHAR_FLAG_COMPLETE:
		dI("All items matching object '%s' were collected. (flag=%s)", object_id, flag_text);
		result = eval_check_existence(test_check_existence, exists_cnt, error_cnt);
		if (result == OVAL_RESULT_TRUE
		    && hasstate) {
			result = eval_check_state(test, args);
		}
		break;
	case SYSCHAR_FLAG_INCOMPLETE:
		dI("Only some of items matching object '%s' have been collected from the system. It is unknown if other matching items also exist. (flag=%s)", object_id, flag_text);
		if (test_check_existence == OVAL_ANY_EXIST) {
			result = OVAL_RESULT_TRUE;
		} else if (test_check_existence == OVAL_AT_LEAST_ONE_EXISTS
			   && exists_cnt > 0) {
			result = OVAL_RESULT_TRUE;
		} else if (test_check_existence == OVAL_NONE_EXIST
			   && exists_cnt > 0) {
			result = OVAL_RESULT_FALSE;
		} else if (test_check_existence == OVAL_ONLY_ONE_EXISTS
			   && exists_cnt > 1) {
			result = OVAL_RESULT_FALSE;
		} else {
			result = OVAL_RESULT_UNKNOWN;
		}

		if (result == OVAL_RESULT_TRUE
		    && hasstate) {
			result = eval_check_state(test, args);
			if (result == OVAL_RESULT_TRUE) {
				if (test_check != OVAL_CHECK_AT_LEAST_ONE) {
					result = OVAL_RESULT_UNKNOWN;
				}
			} else if (result != OVAL_RESULT_FALSE) {
				result = OVAL_RESULT_UNKNOWN;
			}
		}
		break;
	default: {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Item corresponding to object '%s' from test '%s' has an unknown flag. This may indicate a bug in OpenSCAP.",
				object_id, test_id);
		return OVAL_RESULT_ERROR;
		}
	}

	return result;
}

/* this function will gather all the necessary ingredients and call 'evaluate_items' when it finds them */
static oval_result_t _oval_result_test_result(struct oval_result_test *rtest, void **args)
{
	__attribute__nonnull__(rtest);

	/* is the test already evaluated? */
	if (rtest->result != OVAL_RESULT_NOT_EVALUATED) {
		dI("Found result from previous evaluation: %d, returning without further processing.", rtest->result);
		return (rtest->result);
	}

	/* get syschar of rtest */
	struct oval_test *test = oval_result_test_get_test(rtest);
	struct oval_object * object = oval_test_get_object(test);
	char * object_id = oval_object_get_id(object);

	struct oval_result_system *sys = oval_result_test_get_system(rtest);
	struct oval_results_model *results_model = oval_result_system_get_results_model(sys);
	struct oval_probe_session *probe_session = oval_results_model_get_probe_session(results_model);
	if (probe_session != NULL) {
		/* probe test */
		int ret = oval_probe_query_test(probe_session, test);
		if (ret != 0) {
			return ret;
		}
	}

	struct oval_syschar_model *syschar_model = oval_result_system_get_syschar_model(sys);

	struct oval_syschar * syschar = oval_syschar_model_get_syschar(syschar_model, object_id);
	if (syschar == NULL) {
		dW("No syschar for object: %s", object_id);
		return OVAL_RESULT_UNKNOWN;
	}

	/* evaluate items */
	oval_result_t result = _oval_result_test_evaluate_items(test, syschar, args);

	return result;
}

static void _oval_result_test_initialize_bindings(struct oval_result_test *rslt_test)
{
	__attribute__nonnull__(rslt_test);

	struct oval_test *oval_test = oval_result_test_get_test(rslt_test);
	struct oval_string_map *vm;
	struct oval_state_iterator *ste_itr;
	struct oval_iterator *var_itr;

	vm = oval_string_map_new();

	/* Gather bindings pertaining to the referenced states */
	/* TODO: cache bindings collected for each state */
	ste_itr = oval_test_get_states(oval_test);
	while (oval_state_iterator_has_more(ste_itr)) {
		struct oval_state *ste;

		ste = oval_state_iterator_next(ste_itr);
		oval_ste_collect_var_refs(ste, vm);
	}
	oval_state_iterator_free(ste_itr);

	var_itr = oval_string_map_values(vm);
	while (oval_collection_iterator_has_more(var_itr)) {
		struct oval_variable *var;
		struct oval_value_iterator *val_itr;
		struct oval_variable_binding *binding;

		var = oval_collection_iterator_next(var_itr);
		binding = oval_variable_binding_new(var, NULL);

		val_itr = oval_variable_get_values(var);
		while (oval_value_iterator_has_more(val_itr)) {
			struct oval_value *val;
			char *txt;

			val = oval_value_iterator_next(val_itr);
			txt = oval_value_get_text(val);
			txt = oscap_strdup(txt);
			oval_variable_binding_add_value(binding, txt);
		}
		oval_value_iterator_free(val_itr);
		oval_result_test_add_binding(rslt_test, binding);
	}
	oval_collection_iterator_free(var_itr);

	/* Gather bindings pertaining to the collected object */
	struct oval_object *oval_object = oval_test_get_object(oval_test);
	if (oval_object) {
		char *object_id = oval_object_get_id(oval_object);
		struct oval_result_system *sys = oval_result_test_get_system(rslt_test);
		struct oval_syschar_model *syschar_model = oval_result_system_get_syschar_model(sys);
		struct oval_syschar *syschar = oval_syschar_model_get_syschar(syschar_model, object_id);
		/* no syschar if system characteristics was a subset of definitions */
		if(syschar) {
			struct oval_variable_binding_iterator *bindings = oval_syschar_get_variable_bindings(syschar);
			while (oval_variable_binding_iterator_has_more(bindings)) {
				struct oval_variable *var;
				char *var_id;
				struct oval_variable_binding *binding = oval_variable_binding_iterator_next(bindings);

				var = oval_variable_binding_get_variable(binding);
				var_id = oval_variable_get_id(var);
				/* Don't add bindings that were already
				 * collected from states. Assumtion is made
				 * that object's own bindings don't contain
				 * duplicates.
				 */
				if (oval_string_map_get_value(vm, var_id) == NULL) {
					struct oval_definition_model *definition_model = oval_syschar_model_get_definition_model(syschar_model);
					struct oval_variable_binding *binding_copy = oval_variable_binding_clone(binding, definition_model);
					oval_result_test_add_binding(rslt_test, binding_copy);
				}
			}
			oval_variable_binding_iterator_free(bindings);
		}
	}

	oval_string_map_free(vm, NULL);

	rslt_test->bindings_initialized = true;
}

oval_result_t oval_result_test_eval(struct oval_result_test *rtest)
{
	__attribute__nonnull__(rtest);

	struct oval_test *test = oval_result_test_get_test(rtest);
	const char *type = oval_subtype_get_text(oval_test_get_subtype(test));
	const char *test_id = oval_test_get_id(test);
	const char *comment = oval_test_get_comment(test);
	dI("Evaluating %s test '%s': %s.", type, test_id, comment);

	if (rtest->result == OVAL_RESULT_NOT_EVALUATED) {
		if ((oval_independent_subtype_t)oval_test_get_subtype(oval_result_test_get_test(rtest)) != OVAL_INDEPENDENT_UNKNOWN ) {
			struct oval_string_map *tmp_map = oval_string_map_new();
			void *args[] = { rtest->system, rtest, tmp_map };
			dIndent(1);
			rtest->result = _oval_result_test_result(rtest, args);
			dIndent(-1);
			oval_string_map_free(tmp_map, NULL);

			if (!rtest->bindings_initialized) {
				_oval_result_test_initialize_bindings(rtest);
			}
		}
		else
			rtest->result = OVAL_RESULT_UNKNOWN;
	}

	dI("Test '%s' evaluated as %s.", test_id, oval_result_get_text(rtest->result));

	return rtest->result;
}

oval_result_t oval_result_test_get_result(struct oval_result_test * rtest)
{
	__attribute__nonnull__(rtest);

	return rtest->result;
}

int oval_result_test_get_instance(struct oval_result_test *rtest)
{
	__attribute__nonnull__(rtest);

	return rtest->instance;
}

struct oval_message_iterator *oval_result_test_get_messages(struct oval_result_test *rtest)
{
	__attribute__nonnull__(rtest);

	return (struct oval_message_iterator *) oval_collection_iterator(rtest->messages);
}

struct oval_result_item_iterator *oval_result_test_get_items(struct oval_result_test *rtest)
{
	__attribute__nonnull__(rtest);

	return (struct oval_result_item_iterator *)
	    oval_collection_iterator(rtest->items);
}

struct oval_variable_binding_iterator *oval_result_test_get_bindings(struct oval_result_test
								     *rtest)
{
	__attribute__nonnull__(rtest);

	return (struct oval_variable_binding_iterator *)
	    oval_collection_iterator(rtest->bindings);
}

void oval_result_test_set_result(struct oval_result_test *test, oval_result_t result)
{
	__attribute__nonnull__(test);
	test->result = result;
}

void oval_result_test_set_instance(struct oval_result_test *test, int instance)
{
	__attribute__nonnull__(test);
	test->instance = instance;
}

void oval_result_test_add_message(struct oval_result_test *test, struct oval_message *message) 
{
	__attribute__nonnull__(test);
	oval_collection_add(test->messages, message);
}

void oval_result_test_add_item(struct oval_result_test *test, struct oval_result_item *item) 
{
	__attribute__nonnull__(test);
	oval_collection_add(test->items, item);
}

void oval_result_test_add_binding(struct oval_result_test *test, struct oval_variable_binding *binding) 
{
	__attribute__nonnull__(test);
	oval_collection_add(test->bindings, binding);
}

//void(*oscap_consumer_func)(void*, void*);
static void _oval_test_message_consumer(struct oval_message *message, struct oval_result_test *test) {
	oval_result_test_add_message(test, message);
}

static int _oval_result_test_binding_parse(xmlTextReaderPtr reader, struct oval_parser_context *context, void **args) {
	int return_code = 0;

	xmlChar *variable_id = xmlTextReaderGetAttribute(reader, BAD_CAST "variable_id");

	struct oval_syschar_model *syschar_model = oval_result_system_get_syschar_model(SYSTEM);
	struct oval_definition_model *definition_model = oval_syschar_model_get_definition_model(syschar_model);

	struct oval_variable *variable = oval_definition_model_get_new_variable
	    (definition_model, (char *)variable_id, OVAL_VARIABLE_UNKNOWN);

	xmlChar *value = xmlTextReaderValue(reader);

	struct oval_variable_binding *binding = oval_variable_binding_new(variable, oscap_strdup((char *) value));
	oval_result_test_add_binding(TEST, binding);

	xmlFree(value);
	xmlFree(variable_id);

	return return_code;
}

static int _oval_result_test_parse(xmlTextReaderPtr reader, struct oval_parser_context *context, void **args) {
	int return_code = 0;
	xmlChar *localName = xmlTextReaderLocalName(reader);

	if (strcmp((const char *)localName, "message") == 0) {
		return_code = oval_message_parse_tag(reader, context, (oscap_consumer_func) _oval_test_message_consumer, TEST);
	} else if (strcmp((const char *)localName, "tested_item") == 0) {
		return_code = oval_result_item_parse_tag(reader, context, SYSTEM, (oscap_consumer_func) _oval_test_item_consumer, args);
	} else if (strcmp((const char *)localName, "tested_variable") == 0) {
		return_code = _oval_result_test_binding_parse(reader, context, args);
	} else {
		dW("Unhandled tag: <%s>.", localName);
		oval_parser_skip_tag(reader, context);
	}

	free(localName);

	return return_code;
}

int oval_result_test_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *usr) {

	struct oval_result_system *sys = (struct oval_result_system *) usr;
	int return_code = 0;
	struct oval_definition_model *dmod;
	struct oval_test *dtst;
	struct oval_result_test *test;
	xmlChar *test_id = xmlTextReaderGetAttribute(reader, BAD_CAST "test_id");

	dmod = context->definition_model;
	dtst = oval_definition_model_get_new_test(dmod, (char *) test_id);
	oval_result_t result = oval_result_parse(reader, "result", 0);
	int variable_instance = oval_parser_int_attribute(reader, "variable_instance", 1);

	test = oval_result_system_get_new_test(sys, dtst, variable_instance);
	if (test == NULL)
		return -1;
	oval_result_test_set_result(test, result);
	oval_result_test_set_instance(test, variable_instance);

	struct oval_test *ovaltst = oval_result_test_get_test(test);

	oval_existence_t check_existence = oval_existence_parse(reader, "check_existence", OVAL_AT_LEAST_ONE_EXISTS);
	oval_existence_t tst_check_existence = oval_test_get_existence(ovaltst);
	if (tst_check_existence == OVAL_EXISTENCE_UNKNOWN) {
		oval_test_set_existence(ovaltst, check_existence);
	} else if (tst_check_existence != check_existence) {
		dW("@check_existence does not match, test_id: %s.", test_id);
	}

	oval_check_t check = oval_check_parse(reader, "check", OVAL_CHECK_UNKNOWN);
	oval_check_t tst_check = oval_test_get_check(ovaltst);
	if (tst_check == OVAL_CHECK_UNKNOWN) {
		oval_test_set_check(ovaltst, check);
	} else if (tst_check != check) {
		dW("@check does not match, test_id: %s.", test_id);
	}

	int version = oval_parser_int_attribute(reader, "version", 0);
	int tst_version = oval_test_get_version(ovaltst);
	if (tst_version == 0) {
		oval_test_set_version(ovaltst, version);
	} else if (tst_version != version) {
		dW("@version does not match, test_id: %s.", test_id);
	}

	struct oval_string_map *itemmap = oval_string_map_new();
	void *args[] = { sys, test, itemmap };
	return_code = oval_parser_parse_tag(reader, context, (oval_xml_tag_parser) _oval_result_test_parse, args);
	oval_string_map_free(itemmap, NULL);
	test->bindings_initialized = true;

	free(test_id);
	return return_code;
}

static void _oval_result_binding_to_dom(struct oval_variable_binding *binding, xmlDocPtr doc, xmlNode *parent) {
	xmlNs *ns_results = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	struct oval_variable *oval_variable = oval_variable_binding_get_variable(binding);
	char *variable_id = oval_variable_get_id(oval_variable);
	struct oval_string_iterator *str_itr;

	str_itr = oval_variable_binding_get_values(binding);
	while (oval_string_iterator_has_more(str_itr)) {
		char *value;
		xmlNode *binding_node;

		value = oval_string_iterator_next(str_itr);
		binding_node = xmlNewTextChild(parent, ns_results, BAD_CAST "tested_variable", BAD_CAST value);
		xmlNewProp(binding_node, BAD_CAST "variable_id", BAD_CAST variable_id);
	}
	oval_string_iterator_free(str_itr);
}

xmlNode *oval_result_test_to_dom(struct oval_result_test *rslt_test, xmlDocPtr doc, xmlNode * parent) {
	__attribute__nonnull__(rslt_test);

	xmlNs *ns_results = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	xmlNode *test_node = xmlNewTextChild(parent, ns_results, BAD_CAST "test", NULL);

	struct oval_test *oval_test = oval_result_test_get_test(rslt_test);
	char *test_id = oval_test_get_id(oval_test);
	xmlNewProp(test_node, BAD_CAST "test_id", BAD_CAST test_id);

	char version[10];
	*version = '\0';
	snprintf(version, sizeof(version), "%d", oval_test_get_version(oval_test));
	xmlNewProp(test_node, BAD_CAST "version", BAD_CAST version);

	oval_existence_t existence = oval_test_get_existence(oval_test);
	if (existence != OVAL_AT_LEAST_ONE_EXISTS) {
		xmlNewProp(test_node, BAD_CAST "check_existence", BAD_CAST oval_existence_get_text(existence));
	}

	oval_check_t check = oval_test_get_check(oval_test);
	xmlNewProp(test_node, BAD_CAST "check", BAD_CAST oval_check_get_text(check));

	int instance_val = oval_result_test_get_instance(rslt_test);
	if (instance_val > 1) {
		char instance[10];
		*instance = '\0';
		snprintf(instance, sizeof(instance), "%d", instance_val);
		xmlNewProp(test_node, BAD_CAST "variable_instance", BAD_CAST instance);
	}

	oval_result_t result = oval_result_test_get_result(rslt_test);
	xmlNewProp(test_node, BAD_CAST "result", BAD_CAST oval_result_get_text(result));

	/* does not make sense to report these when test(definition) is not evaluated */
	if( result != OVAL_RESULT_NOT_EVALUATED) {
		struct oval_result_item_iterator *items = oval_result_test_get_items(rslt_test);
		while (oval_result_item_iterator_has_more(items)) {
			struct oval_result_item *item = oval_result_item_iterator_next(items);
			oval_result_item_to_dom(item, doc, test_node);
		}
		oval_result_item_iterator_free(items);

		struct oval_variable_binding_iterator *bindings = oval_result_test_get_bindings(rslt_test);
		while (oval_variable_binding_iterator_has_more(bindings)) {
			struct oval_variable_binding *binding = oval_variable_binding_iterator_next(bindings);
			_oval_result_binding_to_dom(binding, doc, test_node);
		}
		oval_variable_binding_iterator_free(bindings);
	}

	return test_node;
}

const char *oval_result_test_get_id(const struct oval_result_test *rslt_test)
{
	__attribute__nonnull__(rslt_test);
	struct oval_test *test = oval_result_test_get_test((struct oval_result_test *) rslt_test);
	return (test == NULL) ? NULL : oval_test_get_id(test);
}
