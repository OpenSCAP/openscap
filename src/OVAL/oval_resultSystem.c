/**
 * @file oval_resultSystem.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oval_definitions.h"
#include "oval_agent_api.h"
#include "oval_results_impl.h"
#include "oval_collection_impl.h"
#include "oval_string_map_impl.h"
#include "common/debug_priv.h"
#include "common/_error.h"
#include "common/util.h"

typedef struct oval_result_system {
	struct oval_results_model *model;
	struct oval_string_map *definitions;
	struct oval_string_map *tests;
	struct oval_syschar_model *syschar_model;
	bool definitions_initialized;
} oval_result_system_t;


static void _oval_result_system_scan_criteria_for_references(struct oval_result_criteria_node *node, 
							     struct oval_string_map *testmap);

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
	oval_result_system_t *sys;

        if (model && oval_results_model_is_locked(model)) {
                oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
                return NULL;
        }

	sys = (oval_result_system_t *) oscap_alloc(sizeof(oval_result_system_t));
	if (sys == NULL)
		return NULL;

	sys->definitions = oval_string_map_new();
	sys->tests = oval_string_map_new();
	sys->syschar_model = syschar_model;
	sys->definitions_initialized = false;
	sys->model = model;

	oval_results_model_add_system(model, sys);

	return sys;
}

bool oval_result_system_is_valid(struct oval_result_system * result_system)
{
	bool is_valid = true;
	struct oval_result_definition_iterator *rslt_definitions_itr;
	struct oval_result_test_iterator *rslt_tests_itr;
	struct oval_syschar_model *syschar_model;

	if (result_system == NULL) {
                oscap_dlprintf(DBG_W, "Argument is not valid: NULL.\n");
		return false;
        }

	/* validate syschar_model */
	syschar_model = oval_result_system_get_syschar_model(result_system);
	if (oval_syschar_model_is_valid(syschar_model) != true)
		return false;

	/* validate result definitions */
	rslt_definitions_itr = oval_result_system_get_definitions(result_system);
	while (oval_result_definition_iterator_has_more(rslt_definitions_itr)) {
		struct oval_result_definition *rslt_definition;

		rslt_definition = oval_result_definition_iterator_next(rslt_definitions_itr);
		if (oval_result_definition_is_valid(rslt_definition) != true) {
			is_valid = false;
			break;
		}
	}
	oval_result_definition_iterator_free(rslt_definitions_itr);
	if (is_valid != true)
		return false;

	/* validate result tests */
	rslt_tests_itr = oval_result_system_get_tests(result_system);
	while (oval_result_test_iterator_has_more(rslt_tests_itr)) {
		struct oval_result_test *rslt_test;

		rslt_test = oval_result_test_iterator_next(rslt_tests_itr);
		if (oval_result_test_is_valid(rslt_test) != true) {
			is_valid = false;
			break;
		}
	}
	oval_result_test_iterator_free(rslt_tests_itr);
	if (is_valid != true)
		return false;

	return true;
}

bool oval_result_system_is_locked(struct oval_result_system * result_system)
{
	__attribute__nonnull__(result_system);

	return oval_results_model_is_locked(result_system->model);
}

typedef void (*_oval_result_system_clone_func) (struct oval_result_system *, void *);

static void _oval_result_system_clone
    (struct oval_string_map *oldmap, struct oval_result_system *new_system, _oval_result_system_clone_func cloner) {
	struct oval_string_iterator *keys = (struct oval_string_iterator *)oval_string_map_keys(oldmap);
	while (oval_string_iterator_has_more(keys)) {
		char *key = oval_string_iterator_next(keys);
		void *old_item = oval_string_map_get_value(oldmap, key);
		(*cloner) (new_system, old_item);
	}
	oval_string_iterator_free(keys);
}

struct oval_result_system *oval_result_system_clone(struct oval_results_model *new_model,
						    struct oval_result_system *old_system)
{
	__attribute__nonnull__(old_system);

	struct oval_result_system *new_system =
	    oval_result_system_new(new_model, oval_result_system_get_syschar_model(old_system));

	_oval_result_system_clone
	    (old_system->definitions, new_system, (_oval_result_system_clone_func) oval_result_definition_clone);

	_oval_result_system_clone
	    (old_system->tests, new_system, (_oval_result_system_clone_func) oval_result_test_clone);

	return new_system;
}

void oval_result_system_free(struct oval_result_system *sys)
{
	__attribute__nonnull__(sys);

	oval_string_map_free(sys->definitions, (oscap_destruct_func) oval_result_definition_free);
	oval_string_map_free(sys->tests, (oscap_destruct_func) oval_result_test_free);

	sys->definitions = NULL;
	sys->syschar_model = NULL;
	sys->tests = NULL;

	oscap_free(sys);
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

static void _oval_result_system_initialize(struct oval_result_system *sys) {
	__attribute__nonnull__(sys);

	sys->definitions_initialized = true;
	struct oval_definition_model *definition_model = oval_syschar_model_get_definition_model(sys->syschar_model);

	struct oval_definition_iterator *oval_definitions = oval_definition_model_get_definitions(definition_model);
	while (oval_definition_iterator_has_more(oval_definitions)) {
		struct oval_definition *oval_definition = oval_definition_iterator_next(oval_definitions);
		oval_result_system_get_new_definition(sys, oval_definition);
	}
	oval_definition_iterator_free(oval_definitions);

	struct oval_test_iterator *oval_tests = oval_definition_model_get_tests(definition_model);
	while (oval_test_iterator_has_more(oval_tests)) {
		struct oval_test *oval_test = oval_test_iterator_next(oval_tests);
		get_oval_result_test_new(sys, oval_test);
	}
	oval_test_iterator_free(oval_tests);
}

struct oval_result_definition_iterator *oval_result_system_get_definitions(struct oval_result_system *sys) {
	__attribute__nonnull__(sys);

	if (!sys->definitions_initialized) {
		_oval_result_system_initialize(sys);
	}
	struct oval_result_definition_iterator *iterator = (struct oval_result_definition_iterator *)
	    oval_string_map_values(sys->definitions);
	return iterator;
}

struct oval_result_test_iterator *oval_result_system_get_tests(struct oval_result_system *sys) {
	__attribute__nonnull__(sys);

	if (!sys->definitions_initialized) {
		_oval_result_system_initialize(sys);
	}
	struct oval_result_test_iterator *iterator = (struct oval_result_test_iterator *)
	    oval_string_map_values(sys->tests);
	return iterator;
}

struct oval_result_definition *oval_result_system_get_definition(struct oval_result_system *sys, const char *id) {
	__attribute__nonnull__(sys);

	if (!sys->definitions_initialized) {
		_oval_result_system_initialize(sys);
	}
	return (struct oval_result_definition *)
	    oval_string_map_get_value(sys->definitions, id);

}

struct oval_result_test *oval_result_system_get_test(struct oval_result_system *sys, char *id) {
	__attribute__nonnull__(sys);

	if (!sys->definitions_initialized) {
		_oval_result_system_initialize(sys);
	}
	return (struct oval_result_test *)
	    oval_string_map_get_value(sys->tests, id);

}

struct oval_result_definition *oval_result_system_get_new_definition
    (struct oval_result_system *sys, struct oval_definition *oval_definition) {
	struct oval_result_definition *rslt_definition = NULL;
	if (oval_definition) {
		char *id = oval_definition_get_id(oval_definition);
		rslt_definition = oval_result_system_get_definition(sys, id);
		if (rslt_definition == NULL) {
			rslt_definition = make_result_definition_from_oval_definition(sys, oval_definition);
			oval_result_system_add_definition(sys, rslt_definition);
		}
	}
	return rslt_definition;
}

struct oval_result_test *get_oval_result_test_new(struct oval_result_system *sys, struct oval_test *oval_test) {
	char *id = oval_test_get_id(oval_test);
	struct oval_result_test *rslt_testtest = oval_result_system_get_test(sys, id);
	if (rslt_testtest == NULL) {
		//test = oval_result_test_new(sys, id);
		rslt_testtest = make_result_test_from_oval_test(sys, oval_test);
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

void oval_result_system_add_definition(struct oval_result_system *sys, struct oval_result_definition *definition) {
	if (sys && !oval_result_system_is_locked(sys)) {
		if (definition) {
			struct oval_definition *ovaldef = oval_result_definition_get_definition(definition);
			char *id = oval_definition_get_id(ovaldef);
			oval_string_map_put(sys->definitions, id, definition);
		}
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

void oval_result_system_add_test(struct oval_result_system *sys, struct oval_result_test *test) {
	if (sys && !oval_result_system_is_locked(sys)) {
		if (test) {
			struct oval_test *ovaldef = oval_result_test_get_test(test);
			char *id = oval_test_get_id(ovaldef);
			oval_string_map_put(sys->tests, id, test);
		}
	} else
		oscap_dlprintf(DBG_W, "Attempt to update locked content.\n");
}

static void _oval_result_system_test_consume(struct oval_result_test *test, struct oval_result_system *sys) {
	oval_result_system_add_test(sys, test);
}

static int _oval_result_system_test_parse
    (xmlTextReaderPtr reader, struct oval_parser_context *context, struct oval_result_system *sys) {
	return oval_result_test_parse_tag
	    (reader, context, sys, (oscap_consumer_func) _oval_result_system_test_consume, sys);

}

static void _oval_result_system_definition_consume
    (struct oval_result_definition *definition, struct oval_result_system *sys) {
	oval_result_system_add_definition(sys, definition);
}

static int _oval_result_system_definition_parse
    (xmlTextReaderPtr reader, struct oval_parser_context *context, struct oval_result_system *sys) {
	return oval_result_definition_parse
	    (reader, context, sys, (oscap_consumer_func) _oval_result_system_definition_consume, sys);
}

static int _oval_result_system_parse
    (xmlTextReaderPtr reader, struct oval_parser_context *context, struct oval_result_system *sys) {
	__attribute__nonnull__(sys);

	xmlChar *localName = xmlTextReaderLocalName(reader);
	oscap_dlprintf(DBG_I, "Parsing <%s>.\n", localName);
	int return_code = 0;
	if (strcmp((const char *)localName, "definitions") == 0) {
		return_code = oval_parser_parse_tag
		    (reader, context, (oval_xml_tag_parser) _oval_result_system_definition_parse, sys);
	} else if (strcmp((const char *)localName, "tests") == 0) {
		return_code = oval_parser_parse_tag
		    (reader, context, (oval_xml_tag_parser) _oval_result_system_test_parse, sys);
	} else if (strcmp((const char *)localName, "oval_system_characteristics") == 0) {
		return_code = ovalsys_parser_parse(sys->syschar_model, reader, context->user_data);
		//return_code = oval_parser_skip_tag(reader, context);
	} else {
		return_code = 0;
		oscap_dlprintf(DBG_W, "Unhandled tag: <%s>.\n", localName);
	}
	oscap_free(localName);
	return return_code;
}

int oval_result_system_parse
    (xmlTextReaderPtr reader, struct oval_parser_context *context, struct oval_syschar_model *syschar_model,
     oscap_consumer_func consumer, void *client) {
	__attribute__nonnull__(context);

	int return_code = 1;
	struct oval_result_system *sys = oval_result_system_new(context->results_model, syschar_model);

	return_code = oval_parser_parse_tag(reader, context, (oval_xml_tag_parser) _oval_result_system_parse, sys);

	(*consumer) (sys, client);

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
		rslt_definition = oval_result_system_get_new_definition(sys, definition);
		oval_result_definition_eval(rslt_definition);
	}

	oval_definition_iterator_free(definitions_itr);
	return 0;
}

oval_result_t oval_result_system_eval_definition(struct oval_result_system *sys, const char *id)
{
        struct oval_results_model *res_model;
        struct oval_definition_model *definition_model;
        struct oval_definition *oval_definition;
	struct oval_result_definition *rslt_definition;

	res_model = oval_result_system_get_results_model(sys);
	definition_model = oval_results_model_get_definition_model(res_model);
	oval_definition = oval_definition_model_get_definition(definition_model, id);
	if (oval_definition == NULL) {
		oscap_dlprintf(DBG_E, "No definition with ID: %s in definition model.\n", id);
		oscap_seterr(OSCAP_EFAMILY_OSCAP, OVAL_EOVALINT, "No definition with such an ID in definition model.");
		return OVAL_RESULT_ERROR;
	}

        rslt_definition = oval_result_system_get_definition(sys, id);
        if (rslt_definition == NULL) {
        	rslt_definition = make_result_definition_from_oval_definition(sys, oval_definition);
                oval_result_system_add_definition(sys, rslt_definition);
        }

	return oval_result_definition_eval(rslt_definition);
}


xmlNode *oval_result_system_to_dom
    (struct oval_result_system * sys,
     struct oval_results_model * results_model,
     struct oval_result_directives * directives, xmlDocPtr doc, xmlNode * parent) {
	xmlNs *ns_results = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	xmlNode *system_node = xmlNewTextChild(parent, ns_results, BAD_CAST "system", NULL);

	struct oval_string_map *tstmap = oval_string_map_new();

	xmlNode *definitions_node = xmlNewTextChild(system_node, ns_results, BAD_CAST "definitions", NULL);
	struct oval_definition_model *definition_model = oval_results_model_get_definition_model(results_model);
	struct oval_definition_iterator *oval_definitions = oval_definition_model_get_definitions(definition_model);
	while(oval_definition_iterator_has_more(oval_definitions)) {
		struct oval_definition *oval_definition = oval_definition_iterator_next(oval_definitions);
		struct oval_result_definition *rslt_definition
		    = oval_result_system_get_new_definition(sys, oval_definition);
		if (rslt_definition) {
			oval_result_t result = oval_result_definition_get_result(rslt_definition);
			if (oval_result_directives_get_reported(directives, result)) {
				oval_result_directive_content_t content
				    = oval_result_directives_get_content(directives, result);
				/* report definition according to directives settings */
				oval_result_definition_to_dom(rslt_definition, content, doc, definitions_node);
				if (content == OVAL_DIRECTIVE_CONTENT_FULL) {
					struct oval_result_criteria_node *criteria
					    = oval_result_definition_get_criteria(rslt_definition);
					/* collect the tests that are referenced from reported definitions */
					if (criteria)
						_oval_result_system_scan_criteria_for_references(criteria, tstmap);
				}
			}
		}
	}
	oval_definition_iterator_free(oval_definitions);

	struct oval_syschar_model *syschar_model = oval_result_system_get_syschar_model(sys);
	struct oval_string_map *sysmap = oval_string_map_new();
	struct oval_string_map *objmap = oval_string_map_new();
	struct oval_string_map *sttmap = oval_string_map_new();
	struct oval_string_map *varmap = oval_string_map_new();

	struct oval_result_test_iterator *result_tests = (struct oval_result_test_iterator *)  oval_string_map_values(tstmap);
	if (oval_result_test_iterator_has_more(result_tests)) {
		xmlNode *tests_node = xmlNewTextChild(system_node, ns_results, BAD_CAST "tests", NULL);
		while (oval_result_test_iterator_has_more(result_tests)) {
			struct oval_state_iterator *ste_itr;
			struct oval_result_test *result_test = oval_result_test_iterator_next(result_tests);
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
	oval_result_test_iterator_free(result_tests);

	oval_syschar_model_to_dom(syschar_model, doc, system_node, 
				  (oval_syschar_resolver *) _oval_result_system_resolve_syschar, sysmap);

	oval_string_map_free(sysmap, NULL);
	oval_string_map_free(objmap, NULL);
	oval_string_map_free(sttmap, NULL);
	oval_string_map_free(varmap, NULL);
	oval_string_map_free(tstmap, NULL);

	return system_node;
}





static void _oval_result_system_scan_criteria_for_references
    (struct oval_result_criteria_node *node, struct oval_string_map *testmap) {
	struct oval_result_criteria_node_iterator *subnodes = oval_result_criteria_node_get_subnodes(node);
	while (subnodes && oval_result_criteria_node_iterator_has_more(subnodes)) {
		struct oval_result_criteria_node *subnode = oval_result_criteria_node_iterator_next(subnodes);
		_oval_result_system_scan_criteria_for_references(subnode, testmap);
	}
	oval_result_criteria_node_iterator_free(subnodes);
	struct oval_result_test *result_test = oval_result_criteria_node_get_test(node);
	if (result_test) {
		struct oval_test *oval_test = oval_result_test_get_test(result_test);
		char *testid = oval_test_get_id(oval_test);
		void *value = oval_string_map_get_value(testmap, testid);
		if (value == NULL) {
			oval_string_map_put(testmap, testid, result_test);
		}
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
	struct oval_object *object = oval_syschar_get_object(syschar);
	char *objid = oval_object_get_id(object);
	return oval_string_map_get_value(sysmap, objid) != NULL;
}

