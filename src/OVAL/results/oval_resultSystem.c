/**
 * @file oval_resultSystem.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oval_definitions.h"
#include "oval_agent_api.h"
#include "oval_agent_api_impl.h"
#include "results/oval_results_impl.h"
#include "adt/oval_collection_impl.h"
#include "adt/oval_smc_impl.h"
#include "adt/oval_smc_iterator_impl.h"
#include "adt/oval_string_map_impl.h"
#include "oval_parser_impl.h"

#include "common/debug_priv.h"
#include "common/_error.h"
#include "common/util.h"

typedef struct oval_result_system {
	struct oval_results_model *model;
	struct oval_smc *definitions;			///< Map contains lists of oval_result_definition
	struct oval_smc *tests;				///< Map contains lists of oval_result_test
	struct oval_syschar_model *syschar_model;
} oval_result_system_t;


static void _oval_result_system_scan_criteria_for_references(struct oval_result_criteria_node *node, 
							     struct oval_smc *testmap);

static void _oval_result_system_scan_entity_for_references(struct oval_syschar_model *syschar_model, 
							   struct oval_entity *entity,
							   struct oval_string_map *objmap,
							   struct oval_string_map *sttmap, 
							   struct oval_string_map *varmap, 
							   struct oval_string_map *sysmap);

static void _oval_result_system_scan_set_for_references(struct oval_syschar_model *syschar_model, 
							struct oval_setobject *set,
							struct oval_string_map *objmap,
							struct oval_string_map *sttmap, 
							struct oval_string_map *varmap, 
							struct oval_string_map *sysmap);

static void _oval_result_system_scan_object_for_references(struct oval_syschar_model *syschar_model, 
							struct oval_object *object,
							struct oval_string_map *objmap,
							struct oval_string_map *sttmap, 
							struct oval_string_map *varmap, 
							struct oval_string_map *sysmap);
static void _oval_result_system_scan_state_for_references(struct oval_syschar_model *syschar_model, 
							struct oval_state *state,
							struct oval_string_map *objmap,
							struct oval_string_map *sttmap, 
							struct oval_string_map *varmap, 
							struct oval_string_map *sysmap);

static void _oval_result_system_scan_component_for_references(struct oval_syschar_model *syschar_model, 
							struct oval_component *component,	
							struct oval_string_map *objmap,
							struct oval_string_map *sttmap, 
							struct oval_string_map *varmap, 
							struct oval_string_map *sysmap);

static bool _oval_result_system_resolve_syschar(struct oval_syschar *syschar, struct oval_string_map *sysmap);





struct oval_result_system *oval_result_system_new(struct oval_results_model *model,
						  struct oval_syschar_model *syschar_model)
{
	__attribute__nonnull__(model);
	oval_result_system_t *sys;

	sys = (oval_result_system_t *) oscap_alloc(sizeof(oval_result_system_t));
	if (sys == NULL)
		return NULL;

	sys->definitions = oval_smc_new();
	sys->tests = oval_smc_new();
	sys->syschar_model = syschar_model;
	sys->model = model;

	oval_results_model_add_system(model, sys);

	return sys;
}

struct oval_result_system *oval_result_system_clone(struct oval_results_model *new_model,
						    struct oval_result_system *old_system)
{
	__attribute__nonnull__(old_system);

	struct oval_result_system *new_system =
	    oval_result_system_new(new_model, oval_result_system_get_syschar_model(old_system));

	oval_smc_free0(new_system->definitions);
	oval_smc_free0(new_system->tests);
	new_system->definitions = oval_smc_clone_user(old_system->definitions, (oval_smc_user_clone_func) oval_result_definition_clone, new_system);
	new_system->tests = oval_smc_clone_user(old_system->tests, (oval_smc_user_clone_func) oval_result_test_clone, new_system);

	return new_system;
}

void oval_result_system_free(struct oval_result_system *sys)
{
	__attribute__nonnull__(sys);

	oval_smc_free(sys->definitions, (oscap_destruct_func) oval_result_definition_free);
	oval_smc_free(sys->tests, (oscap_destruct_func) oval_result_test_free);

	sys->definitions = NULL;
	sys->syschar_model = NULL;
	sys->tests = NULL;

	free(sys);
}

bool oval_result_system_iterator_has_more(struct oval_result_system_iterator *sys) {
	return oval_collection_iterator_has_more((struct oval_iterator *)sys);
}

struct oval_result_system *oval_result_system_iterator_next(struct oval_result_system_iterator *sys) {
	return (struct oval_result_system *)
	    oval_collection_iterator_next((struct oval_iterator *)sys);
}

void oval_result_system_iterator_free(struct oval_result_system_iterator *sys) {
	oval_collection_iterator_free((struct oval_iterator *)sys);
}

struct oval_result_definition_iterator *oval_result_system_get_definitions(struct oval_result_system *sys) {
	__attribute__nonnull__(sys);

	return oval_result_definition_iterator_new(sys->definitions);
}

struct oval_result_test_iterator *oval_result_system_get_tests(struct oval_result_system *sys) {
	__attribute__nonnull__(sys);

	return oval_result_test_iterator_new(sys->tests);
}

struct oval_result_definition *oval_result_system_get_definition(struct oval_result_system *sys, const char *id) {
	__attribute__nonnull__(sys);

	// Previously, this structure used to hold only one result_definition per given ID.
	// Now we need to return the very last one from a list.
	return oval_smc_get_last(sys->definitions, id);
}

struct oval_result_test *oval_result_system_get_test(struct oval_result_system *sys, char *id) {
	__attribute__nonnull__(sys);

	// Previously, this structure used to hold only one result_test per given ID.
	// Now we need to return the very last one from a list.
	return oval_smc_get_last(sys->tests, id);
}

struct oval_result_definition *oval_result_system_get_new_definition
    (struct oval_result_system *sys, struct oval_definition *oval_definition, int variable_instance) {
	// This function is used from multiple different places which might not be sustainable.
	// variable_instance=0 means that caller has no special preference for variable_instance
	// 	and the very last definition shall be returned. Additionally, we should create
	// 	new result_definition when the variable_instance_hint does not match.
	// variable_instance!=0 means that we should find result_definition with given instance
	struct oval_result_definition *rslt_definition = NULL;
	if (oval_definition) {
		char *id = oval_definition_get_id(oval_definition);
		rslt_definition = oval_result_system_get_definition(sys, id);
		if (rslt_definition == NULL) {
			rslt_definition = make_result_definition_from_oval_definition(sys, oval_definition,
				variable_instance ? variable_instance : 1);
			oval_result_system_add_definition(sys, rslt_definition);
		}
		else if (oval_result_definition_get_variable_instance_hint(rslt_definition) != oval_result_definition_get_instance(rslt_definition)) {
			int hint = oval_result_definition_get_variable_instance_hint(rslt_definition);
			dI("Creating another result-definition for id=%s based on variable_instance: %d", id, hint);
			rslt_definition = make_result_definition_from_oval_definition(sys, oval_definition, hint);
			oval_result_system_add_definition(sys, rslt_definition);
		}
	}
	return rslt_definition;
}

struct oval_result_test *oval_result_system_get_new_test(struct oval_result_system *sys, struct oval_test *oval_test, int variable_instance) {
	char *id = oval_test_get_id(oval_test);
	struct oval_result_test *rslt_testtest = oval_result_system_get_test(sys, id);
	if (rslt_testtest == NULL) {
		//test = oval_result_test_new(sys, id);
		rslt_testtest = make_result_test_from_oval_test(sys, oval_test, variable_instance);
		oval_result_system_add_test(sys, rslt_testtest);
	}
	else if (variable_instance != 0 && oval_result_test_get_instance(rslt_testtest) != variable_instance) {
		dI("Creating another result-test for id=%s based on variable_instance: %d", id, variable_instance);
		rslt_testtest = make_result_test_from_oval_test(sys, oval_test, variable_instance);
		oval_result_system_add_test(sys, rslt_testtest);
	}
	return rslt_testtest;
}

struct oval_results_model *oval_result_system_get_results_model(struct oval_result_system *sys) {
	__attribute__nonnull__(sys);

	return sys->model;
}

struct oval_syschar_model *oval_result_system_get_syschar_model(struct oval_result_system *sys) {
	__attribute__nonnull__(sys);

	return sys->syschar_model;
}

struct oval_sysinfo *oval_result_system_get_sysinfo(struct oval_result_system *sys) {
	struct oval_syschar_model *syschar_model = oval_result_system_get_syschar_model(sys);
	return (syschar_model)
	    ? oval_syschar_model_get_sysinfo(syschar_model) : NULL;
}

void oval_result_system_add_definition(struct oval_result_system *sys, struct oval_result_definition *definition) 
{
	__attribute__nonnull__(sys);
	if (definition) {
		const char *id = oval_result_definition_get_id(definition);
		oval_smc_put_last(sys->definitions, id, definition);
	}
}

void oval_result_system_add_test(struct oval_result_system *sys, struct oval_result_test *test) 
{
	__attribute__nonnull__(sys);
	if (test) {
		const char *id = oval_result_test_get_id(test);
		oval_smc_put_last(sys->tests, id, test);
	}
}

static int oval_result_system_parse(xmlTextReaderPtr reader, struct oval_parser_context *context, void *usr) {
	__attribute__nonnull__(usr);

	struct oval_result_system *sys = (struct oval_result_system *) usr;
	xmlChar *localName = xmlTextReaderLocalName(reader);

	int return_code = 0;
	if (strcmp((const char *)localName, "definitions") == 0) {
                return_code = oval_parser_parse_tag(reader, context, oval_result_definition_parse_tag, sys);
	} else if (strcmp((const char *)localName, "tests") == 0) {
		return_code = oval_parser_parse_tag(reader, context, oval_result_test_parse_tag, sys);
	} else if (strcmp((const char *)localName, OVAL_ROOT_ELM_SYSCHARS) == 0) {
		return_code = oval_syschar_model_parse(reader, context);
	} else {
                dW("Skipping tag: %s", localName);
                oval_parser_skip_tag(reader, context);
	}

        if (return_code != 0) {
                dW("Parsing of <%s> terminated by an error at line %d.", localName, xmlTextReaderGetParserLineNumber(reader));
        }

	free(localName);
	return return_code;
}

int oval_result_system_parse_tag(xmlTextReaderPtr reader, struct oval_parser_context *context, void *usr) {
	__attribute__nonnull__(context);

        char *tagname = (char *)xmlTextReaderLocalName(reader);
        char *namespace = (char *)xmlTextReaderNamespaceUri(reader);
	int return_code = 0;

	int is_ovalres = strcmp((const char *)OVAL_RESULTS_NAMESPACE, namespace) == 0;
	if (is_ovalres && (strcmp(tagname, "system") == 0)) {
		/* create new empty syschar for this model */
		struct oval_syschar_model *syschar_model = oval_syschar_model_new(context->definition_model);
		context->syschar_model = syschar_model;
		/* create new results system instance */
		struct oval_result_system *sys = oval_result_system_new(context->results_model, syschar_model);
		/* populate results system  */
		return_code = oval_parser_parse_tag(reader, context, oval_result_system_parse, sys);
        } else {
                dW("Skipping tag: %s", tagname);
                oval_parser_skip_tag(reader, context);
        }

        if (return_code != 0) {
                dW("Parsing of <%s> terminated by an error at line %d.", tagname, xmlTextReaderGetParserLineNumber(reader));
        }

        free(tagname);
        free(namespace);

	return return_code;
}

int oval_result_system_eval(struct oval_result_system *sys)
{
	struct oval_results_model *res_model;
	struct oval_definition_model *definition_model;
	struct oval_definition_iterator *definitions_itr;

	res_model = oval_result_system_get_results_model(sys);
	definition_model = oval_results_model_get_definition_model(res_model);
	definitions_itr = oval_definition_model_get_definitions(definition_model);

	while (oval_definition_iterator_has_more(definitions_itr)) {
		struct oval_definition *definition;
		struct oval_result_definition *rslt_definition;

		definition = oval_definition_iterator_next(definitions_itr);
		rslt_definition = oval_result_system_get_new_definition(sys, definition, 0);
		oval_result_definition_eval(rslt_definition);
	}

	oval_definition_iterator_free(definitions_itr);
	return 0;
}

int oval_result_system_eval_definition(struct oval_result_system *sys, const char *id)
{
	struct oval_result_definition *rslt_definition = oval_result_system_prepare_definition(sys, id);
	if (rslt_definition == NULL) {
		return -1;
	}
	oval_result_definition_eval(rslt_definition);
	return 0;
}

struct oval_result_definition *oval_result_system_prepare_definition(struct oval_result_system *sys, const char *id)
{
        struct oval_results_model *res_model;
        struct oval_definition_model *definition_model;
        struct oval_definition *oval_definition;
	struct oval_result_definition *rslt_definition;

	res_model = oval_result_system_get_results_model(sys);
	definition_model = oval_results_model_get_definition_model(res_model);
	oval_definition = oval_definition_model_get_definition(definition_model, id);
	if (oval_definition == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "No definition with ID: %s in definition model.", id);
		return NULL;
	}

        rslt_definition = oval_result_system_get_definition(sys, id);
        if (rslt_definition == NULL) {
		rslt_definition = make_result_definition_from_oval_definition(sys, oval_definition, 1);
                oval_result_system_add_definition(sys, rslt_definition);
        }
	else if (oval_result_definition_get_variable_instance_hint(rslt_definition) != oval_result_definition_get_instance(rslt_definition)) {
		int hint = oval_result_definition_get_variable_instance_hint(rslt_definition);
		dI("Creating another result-definition for id=%s based on variable_instance: %d", id, hint);
		rslt_definition = make_result_definition_from_oval_definition(sys, oval_definition, hint);
		oval_result_system_add_definition(sys, rslt_definition);
	}
	return rslt_definition;
}

static void _oval_result_definition_to_dom_based_on_directives(struct oval_result_definition *rslt_definition,
						   struct oval_result_directives * directives,
						   xmlDocPtr doc,
						   xmlNode *definitions_node,
						   struct oval_smc *tstmap)
{
	oval_result_t result = oval_result_definition_get_result(rslt_definition);
	if (oval_result_directives_get_reported(directives, result)) {
		oval_result_directive_content_t content = oval_result_directives_get_content(directives, result);
		/* report definition according to directives settings */
		oval_result_definition_to_dom(rslt_definition, content, doc, definitions_node);
		if (content == OVAL_DIRECTIVE_CONTENT_FULL) {
			struct oval_result_criteria_node *criteria = oval_result_definition_get_criteria(rslt_definition);
			/* collect the tests that are referenced from reported definitions */
			if (criteria)
				_oval_result_system_scan_criteria_for_references(criteria, tstmap);
		} else if (content == OVAL_DIRECTIVE_CONTENT_THIN) {
			// NOOP
		}
	}
}

xmlNode *oval_result_system_to_dom(struct oval_result_system * sys,
				   struct oval_results_model * results_model,
				   struct oval_directives_model * directives_model, 
				   xmlDocPtr doc, xmlNode * parent) {

	struct oval_result_directives * directives;
	struct oval_result_directives * class_dirs;
	struct oval_result_directives * def_dirs = oval_directives_model_get_defdirs(directives_model);

	xmlNs *ns_results = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	xmlNode *system_node = xmlNewTextChild(parent, ns_results, BAD_CAST "system", NULL);

	struct oval_smc *tstmap = oval_smc_new();

	xmlNode *definitions_node = xmlNewTextChild(system_node, ns_results, BAD_CAST "definitions", NULL);
	struct oval_definition_model *definition_model = oval_results_model_get_definition_model(results_model);
	struct oval_definition_iterator *oval_definitions = oval_definition_model_get_definitions(definition_model);
	while(oval_definition_iterator_has_more(oval_definitions)) {
		struct oval_definition *oval_definition = oval_definition_iterator_next(oval_definitions);

		oval_definition_class_t def_class = oval_definition_get_class(oval_definition);
		class_dirs = oval_directives_model_get_classdir(directives_model, def_class);
		directives = class_dirs ? class_dirs : def_dirs;

		bool exported = false;
		struct oval_iterator *rslt_definitions_it = oval_smc_get_all_it(sys->definitions, oval_definition_get_id(oval_definition));
		if (rslt_definitions_it != NULL) {
			while (oval_collection_iterator_has_more(rslt_definitions_it)) {
				struct oval_result_definition *rslt_definition = oval_collection_iterator_next(rslt_definitions_it);
				_oval_result_definition_to_dom_based_on_directives(rslt_definition, directives, doc, definitions_node, tstmap);
				exported = true;
			}
			oval_collection_iterator_free(rslt_definitions_it);
		}
		if (!exported) {
			struct oval_result_definition *rslt_definition = oval_result_system_get_new_definition(sys, oval_definition, 1);
			if (rslt_definition) {
				_oval_result_definition_to_dom_based_on_directives(rslt_definition, directives, doc, definitions_node, tstmap);
			}
		}
	}
	oval_definition_iterator_free(oval_definitions);

	struct oval_syschar_model *syschar_model = oval_result_system_get_syschar_model(sys);
	struct oval_string_map *sysmap = oval_string_map_new();
	struct oval_string_map *objmap = oval_string_map_new();
	struct oval_string_map *sttmap = oval_string_map_new();
	struct oval_string_map *varmap = oval_string_map_new();

	struct oval_smc_iterator *result_tests = oval_smc_iterator_new(tstmap);
	if (oval_smc_iterator_has_more(result_tests)) {
		xmlNode *tests_node = xmlNewTextChild(system_node, ns_results, BAD_CAST "tests", NULL);
		while (oval_smc_iterator_has_more(result_tests)) {
			struct oval_state_iterator *ste_itr;
			struct oval_result_test *result_test = oval_smc_iterator_next(result_tests);
			/* report the test */
			oval_result_test_to_dom(result_test, doc, tests_node);
			struct oval_test *oval_test = oval_result_test_get_test(result_test);
			/* collect the objects that are referenced from reported test */
			/* look for objects in path: test->object ...  */
			struct oval_object *object = oval_test_get_object(oval_test);
			if (object) {
				char *objid = oval_object_get_id(object);
				/* is object already "collected" */
				void *value = oval_string_map_get_value(objmap, objid);
				if (value == NULL) {
					/* is there a system characteristic for the object */
					struct oval_syschar *syschar =
					    oval_syschar_model_get_syschar(syschar_model, objid);
					if (syschar) {
						/* put object into map */
						oval_string_map_put(objmap, objid, object);
						_oval_result_system_scan_object_for_references
						    (syschar_model, object, objmap, sttmap, varmap, sysmap);
					}
				}
			}
			/* look for objects in test->state->... */
			ste_itr = oval_test_get_states(oval_test);
			while (oval_state_iterator_has_more(ste_itr)) {
				struct oval_state *state = oval_state_iterator_next(ste_itr);
				char *sttid = oval_state_get_id(state);
				void *value = oval_string_map_get_value(sttmap, sttid);
				if (value == NULL) {
					oval_string_map_put(sttmap, sttid, state);
					_oval_result_system_scan_state_for_references(syschar_model, state, objmap, sttmap,
										      varmap, sysmap);
				}
			}
			oval_state_iterator_free(ste_itr);
		}
	}
	oval_smc_iterator_free(result_tests);

	bool export_sys_char = oval_results_model_get_export_system_characteristics(results_model);
	oval_syschar_model_to_dom(syschar_model, doc, system_node, 
				  (oval_syschar_resolver *) _oval_result_system_resolve_syschar, sysmap, export_sys_char);

	oval_string_map_free(sysmap, NULL);
	oval_string_map_free(objmap, NULL);
	oval_string_map_free(sttmap, NULL);
	oval_string_map_free(varmap, NULL);
	oval_smc_free0(tstmap);

	return system_node;
}





static void _oval_result_system_scan_criteria_for_references
    (struct oval_result_criteria_node *node, struct oval_smc *testmap) {
	struct oval_result_criteria_node_iterator *subnodes = oval_result_criteria_node_get_subnodes(node);
	while (subnodes && oval_result_criteria_node_iterator_has_more(subnodes)) {
		struct oval_result_criteria_node *subnode = oval_result_criteria_node_iterator_next(subnodes);
		_oval_result_system_scan_criteria_for_references(subnode, testmap);
	}
	oval_result_criteria_node_iterator_free(subnodes);
	struct oval_result_test *result_test = oval_result_criteria_node_get_test(node);
	if (result_test) {
		const char *testid = oval_result_test_get_id(result_test);
		oval_smc_put_last_if_not_exists(testmap, testid, result_test);
	}
}

static void _oval_result_system_scan_object_for_references
    (struct oval_syschar_model *syschar_model, struct oval_object *object,
     struct oval_string_map *objmap,
     struct oval_string_map *sttmap, 
     struct oval_string_map *varmap, 
     struct oval_string_map *sysmap) {
	char *objid = oval_object_get_id(object);
	struct oval_syschar *syschar = oval_syschar_model_get_syschar(syschar_model, objid);
	if (syschar) /* put syschar of the object into map */
		oval_string_map_put(sysmap, objid, syschar);
	struct oval_object_content_iterator *contents = oval_object_get_object_contents(object);
	/* see if there are more objects referenced from this object's content */ 
	while (oval_object_content_iterator_has_more(contents)) {
		struct oval_object_content *content = oval_object_content_iterator_next(contents);
		struct oval_entity *entity = oval_object_content_get_entity(content);
		if (entity)
			_oval_result_system_scan_entity_for_references(syschar_model, entity, objmap, sttmap, varmap,
								       sysmap);
		struct oval_setobject *set = oval_object_content_get_setobject(content);
		if (set)
			_oval_result_system_scan_set_for_references(syschar_model, set, objmap, sttmap, varmap, sysmap);
	}
	oval_object_content_iterator_free(contents);
}

static void _oval_result_system_scan_state_for_references
    (struct oval_syschar_model *syschar_model, struct oval_state *state,
     struct oval_string_map *objmap,
     struct oval_string_map *sttmap, struct oval_string_map *varmap, struct oval_string_map *sysmap) {
	struct oval_state_content_iterator *contents = oval_state_get_contents(state);
	while (oval_state_content_iterator_has_more(contents)) {
		struct oval_state_content *content = oval_state_content_iterator_next(contents);
		struct oval_entity *entity = oval_state_content_get_entity(content);
		if (entity)
			_oval_result_system_scan_entity_for_references(syschar_model, entity, objmap, sttmap, varmap,
								       sysmap);
	}
	oval_state_content_iterator_free(contents);
}

static void _oval_result_system_scan_component_for_references
    (struct oval_syschar_model *syschar_model, struct oval_component *component,
     struct oval_string_map *objmap,
     struct oval_string_map *sttmap, struct oval_string_map *varmap, struct oval_string_map *sysmap) {
	struct oval_variable *variable = oval_component_get_variable(component);
	if (variable) {
		char *varid = oval_variable_get_id(variable);
		void *value = oval_string_map_get_value(varmap, varid);
		if (value == NULL) {
			oval_string_map_put(varmap, varid, variable);
			struct oval_component *component2 = oval_variable_get_component(variable);
			if (component2) {
				_oval_result_system_scan_component_for_references(syschar_model, component2, objmap,
										  sttmap, varmap, sysmap);
			}
		}
	} else {
		struct oval_component_iterator *fcomponents = oval_component_get_function_components(component);
		if (fcomponents)
			while (oval_component_iterator_has_more(fcomponents)) {
				struct oval_component *fcomponent = oval_component_iterator_next(fcomponents);
				_oval_result_system_scan_component_for_references(syschar_model, fcomponent, objmap,
										  sttmap, varmap, sysmap);
			}
		oval_component_iterator_free(fcomponents);

		struct oval_object *object = oval_component_get_object(component);
		if (object) {
			char *objid = oval_object_get_id(object);
			void *value = oval_string_map_get_value(objmap, objid);
			if (value == NULL) {
				oval_string_map_put(objmap, objid, object);
				_oval_result_system_scan_object_for_references(syschar_model, object, objmap, sttmap,
									       varmap, sysmap);
			}
		}
	}
}

static void _oval_result_system_scan_entity_for_references
    (struct oval_syschar_model *syschar_model, struct oval_entity *entity,
     struct oval_string_map *objmap,
     struct oval_string_map *sttmap, struct oval_string_map *varmap, struct oval_string_map *sysmap) {
	struct oval_variable *variable = oval_entity_get_variable(entity);
	if (variable) {
		char *varid = oval_variable_get_id(variable);
		void *value = oval_string_map_get_value(varmap, varid);
		if (value == NULL) {
			oval_string_map_put(varmap, varid, variable);
			struct oval_component *component = oval_variable_get_component(variable);
			if (component) {
				_oval_result_system_scan_component_for_references(syschar_model, component, objmap,
										  sttmap, varmap, sysmap);
			}
		}
	}
}

static void _oval_result_system_scan_set_for_references
    (struct oval_syschar_model *syschar_model, struct oval_setobject *set,
     struct oval_string_map *objmap,
     struct oval_string_map *sttmap, struct oval_string_map *varmap, struct oval_string_map *sysmap) {
	struct oval_object_iterator *objects = oval_setobject_get_objects(set);
	if (objects)
		while (oval_object_iterator_has_more(objects)) {
			struct oval_object *object = oval_object_iterator_next(objects);
			char *objid = oval_object_get_id(object);
			void *value = oval_string_map_get_value(objmap, objid);
			if (value == NULL) {
				oval_string_map_put(objmap, objid, object);
				_oval_result_system_scan_object_for_references(syschar_model, object, objmap, sttmap,
									       varmap, sysmap);
			}
		}
	oval_object_iterator_free(objects);
	struct oval_filter_iterator *filters = oval_setobject_get_filters(set);
	if (filters)
		while (oval_filter_iterator_has_more(filters)) {
			struct oval_filter *filter;
			struct oval_state *state;

			filter = oval_filter_iterator_next(filters);
			state = oval_filter_get_state(filter);
			char *sttid = oval_state_get_id(state);
			void *value = oval_string_map_get_value(sttmap, sttid);
			if (value == NULL) {
				oval_string_map_put(sttmap, sttid, state);
				_oval_result_system_scan_state_for_references(syschar_model, state, objmap, sttmap,
									      varmap, sysmap);
			}
		}
	oval_filter_iterator_free(filters);
	struct oval_setobject_iterator *subsets = oval_setobject_get_subsets(set);
	if (subsets)
		while (oval_setobject_iterator_has_more(subsets)) {
			struct oval_setobject *subset = oval_setobject_iterator_next(subsets);
			_oval_result_system_scan_set_for_references(syschar_model, subset, objmap, sttmap, varmap,
								    sysmap);
		}
	oval_setobject_iterator_free(subsets);
}

static bool _oval_result_system_resolve_syschar(struct oval_syschar *syschar, struct oval_string_map *sysmap) {
	const char *objid = oval_syschar_get_id(syschar);
	return oval_string_map_get_value(sysmap, objid) != NULL;
}

