/**
 * @file oval_defModel.c
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
 *      Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <time.h>

#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "adt/oval_string_map_impl.h"
#include "oval_system_characteristics_impl.h"
#if defined(OVAL_PROBES_ENABLED)
# include "oval_probe_impl.h"
#endif
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"
#include "common/elements.h"
#include "oscap_source.h"
#include "source/oscap_source_priv.h"

typedef struct oval_definition_model {
	struct oval_generator *generator;
	struct oval_string_map *definition_map;
	struct oval_string_map *test_map;
	struct oval_string_map *object_map;
	struct oval_string_map *state_map;
	struct oval_string_map *variable_map;
	struct oval_collection *bound_variable_models;
        char *schema;
	struct oval_string_map *vardef_map;		///< look-up table for efficient @variable_instance processing
} oval_definition_model_t;

/* failed   - NULL
 * success  - oval_definition_model
 * */
struct oval_definition_model *oval_definition_model_new()
{
	oval_definition_model_t *newmodel = (oval_definition_model_t *) oscap_alloc(sizeof(oval_definition_model_t));
	if (newmodel == NULL)
		return NULL;

	newmodel->generator = oval_generator_new();
	newmodel->definition_map = oval_string_map_new();
	newmodel->object_map = oval_string_map_new();
	newmodel->state_map = oval_string_map_new();
	newmodel->test_map = oval_string_map_new();
	newmodel->variable_map = oval_string_map_new();
	newmodel->bound_variable_models = NULL;
        newmodel->schema = strdup(OVAL_DEF_SCHEMA_LOCATION);
	newmodel->vardef_map = NULL;

	return newmodel;
}

typedef void (*_oval_clone_func) (void *, struct oval_definition_model *);

static void _oval_definition_model_clone(struct oval_string_map *oldmap,
					 struct oval_definition_model *newmodel,
					 _oval_clone_func cloner)
{
	struct oval_string_iterator *keys = (struct oval_string_iterator *)oval_string_map_keys(oldmap);
	while (oval_string_iterator_has_more(keys)) {
		char *key = oval_string_iterator_next(keys);
		void *olditem = oval_string_map_get_value(oldmap, key);
		(*cloner) (newmodel, olditem);
	}
	oval_string_iterator_free(keys);
}

/* failed   - NULL
 * success  - oval_definition_model
 * */
struct oval_definition_model *oval_definition_model_clone(struct oval_definition_model *oldmodel)
{
	__attribute__nonnull__(oldmodel);

	struct oval_definition_model *newmodel = oval_definition_model_new();
	if (newmodel == NULL)
		return NULL;

	_oval_definition_model_clone
	    (oldmodel->definition_map, newmodel, (_oval_clone_func) oval_definition_clone);
	_oval_definition_model_clone
	    (oldmodel->object_map, newmodel, (_oval_clone_func) oval_object_clone);
	_oval_definition_model_clone(oldmodel->state_map, newmodel, (_oval_clone_func) oval_state_clone);
	_oval_definition_model_clone(oldmodel->test_map, newmodel, (_oval_clone_func) oval_test_clone);
	_oval_definition_model_clone
	    (oldmodel->variable_map, newmodel, (_oval_clone_func) oval_variable_clone);
        newmodel->schema = oscap_strdup(oldmodel->schema);
	newmodel->vardef_map = NULL;
	return newmodel;
}

void oval_definition_model_free(struct oval_definition_model *model)
{
	if (model != NULL) {
		oval_string_map_free(model->definition_map, (oscap_destruct_func) oval_definition_free);
		oval_string_map_free(model->object_map, (oscap_destruct_func) oval_object_free);
		oval_string_map_free(model->state_map, (oscap_destruct_func) oval_state_free);
		oval_string_map_free(model->test_map, (oscap_destruct_func) oval_test_free);
		oval_string_map_free(model->variable_map, (oscap_destruct_func) oval_variable_free);
		if (model->vardef_map != NULL)
			oval_string_map_free(model->vardef_map, (oscap_destruct_func) oval_string_map_free0);
		if (model->bound_variable_models)
			oval_collection_free_items(model->bound_variable_models,
					   (oscap_destruct_func) oval_variable_model_free);

	        if (model->schema != NULL)
			oscap_free(model->schema);

		oval_generator_free(model->generator);
		oscap_free(model);
	}
}

struct oval_generator *oval_definition_model_get_generator(struct oval_definition_model *model)
{
	return model->generator;
}

void oval_definition_model_set_generator(struct oval_definition_model *model, struct oval_generator *generator)
{
	oval_generator_free(model->generator);
	model->generator = generator;
}

const char * oval_definition_model_get_schema(struct oval_definition_model * model)
{
        __attribute__nonnull__(model);

        return model->schema;
}

oval_version_t oval_definition_model_get_schema_version(struct oval_definition_model *model)
{
	if (model == NULL || model->generator == NULL) {
		return OVAL_VERSION_INVALID;
	}
	const char *ver_str = oval_generator_get_schema_version(model->generator);
	return oval_version_from_cstr(ver_str);
}

void oval_definition_model_add_definition(struct oval_definition_model *model, struct oval_definition *definition)
{
	__attribute__nonnull__(model);
	char *key = oval_definition_get_id(definition);
	oval_string_map_put(model->definition_map, key, (void *)definition);
}

void oval_definition_model_set_schema(struct oval_definition_model *model, const char *version)
{
	__attribute__nonnull__(model);
	model->schema = oscap_strdup(version);
}

void oval_definition_model_add_test(struct oval_definition_model *model, struct oval_test *test)
{
	__attribute__nonnull__(model);
	char *key = oval_test_get_id(test);
	oval_string_map_put(model->test_map, key, (void *)test);
}

void oval_definition_model_add_object(struct oval_definition_model *model, struct oval_object *object)
{
	__attribute__nonnull__(model);
	char *key = oval_object_get_id(object);
	oval_string_map_put(model->object_map, key, (void *)object);
}

void oval_definition_model_add_state(struct oval_definition_model *model, struct oval_state *state)
{
	__attribute__nonnull__(model);
	char *key = oval_state_get_id(state);
	oval_string_map_put(model->state_map, key, (void *)state);
}

void oval_definition_model_add_variable(struct oval_definition_model *model, struct oval_variable *variable)
{
	__attribute__nonnull__(model);
	char *key = oval_variable_get_id(variable);
	oval_string_map_put(model->variable_map, key, (void *)variable);
}

static inline int _oval_definition_model_merge_source(struct oval_definition_model *model, struct oscap_source *source)
{
	/* setup context */
	struct oval_parser_context context;
	context.reader = oscap_source_get_xmlTextReader(source);
	if (context.reader == NULL) {
		return -1;
	}
	context.definition_model = model;
	context.user_data = NULL;
	/* jump into oval_definitions */
	while (xmlTextReaderRead(context.reader) == 1
		&& xmlTextReaderNodeType(context.reader) != XML_READER_TYPE_ELEMENT) ;
	/* start parsing */
	int ret = oval_definition_model_parse(context.reader, &context);
	xmlFreeTextReader(context.reader);
	return ret;
}

struct oval_definition_model *oval_definition_model_import_source(struct oscap_source *source)
{
        struct oval_definition_model *model = oval_definition_model_new();
	int ret = _oval_definition_model_merge_source(model, source);
        if (ret == -1 ) {
                oval_definition_model_free(model);
                model = NULL;
        }
	return model;
}

struct oval_definition_model * oval_definition_model_import(const char *file)
{
	struct oscap_source *source = oscap_source_new_from_file(file);
	struct oval_definition_model *model = oval_definition_model_import_source(source);
	oscap_source_free(source);
        return model;
}

int oval_definition_model_merge(struct oval_definition_model *model, const char *file)
{
	__attribute__nonnull__(model);

	int ret;

	struct oscap_source *source = oscap_source_new_from_file(file);
	ret = _oval_definition_model_merge_source(model, source);

	oscap_source_free(source);

	return ret;
}

struct oval_definition *oval_definition_model_get_definition(struct oval_definition_model *model, const char *key)
{
	__attribute__nonnull__(model);

	return (struct oval_definition *)oval_string_map_get_value(model->definition_map, key);
}

struct oval_test *oval_definition_model_get_test(struct oval_definition_model *model, const char *key)
{
	__attribute__nonnull__(model);

	return (struct oval_test *)oval_string_map_get_value(model->test_map, key);
}

struct oval_object *oval_definition_model_get_object(struct oval_definition_model *model, const char *key)
{
	__attribute__nonnull__(model);

	return (struct oval_object *)oval_string_map_get_value(model->object_map, key);
}

struct oval_state *oval_definition_model_get_state(struct oval_definition_model *model, const char *key)
{
	__attribute__nonnull__(model);

	return (struct oval_state *)oval_string_map_get_value(model->state_map, key);
}

struct oval_variable *oval_definition_model_get_variable(struct oval_definition_model *model, const char *key)
{
	__attribute__nonnull__(model);

	return (struct oval_variable *)oval_string_map_get_value(model->variable_map, key);
}

int oval_definition_model_bind_variable_model(struct oval_definition_model *defmodel,
					       struct oval_variable_model *varmodel)
{
	struct oval_string_iterator *evar_id_itr;

	if (!defmodel->bound_variable_models)
		defmodel->bound_variable_models = oval_collection_new();

	oval_collection_add(defmodel->bound_variable_models, varmodel);

	/* todo: keep reference count for each variable model if it can be bound to multiple definition models */

	evar_id_itr = oval_variable_model_get_variable_ids(varmodel);
	while (oval_string_iterator_has_more(evar_id_itr)) {
		char *evar_id;
		struct oval_variable *var;

		evar_id = oval_string_iterator_next(evar_id_itr);
		var = oval_definition_model_get_variable(defmodel, evar_id);
		if (!var)
			continue;

		oval_variable_bind_ext_var(var, varmodel, evar_id);
	}
	oval_string_iterator_free(evar_id_itr);

	return 0;
}

struct oval_variable_model_iterator *oval_definition_model_get_variable_models(struct oval_definition_model *model)
{
	if (model->bound_variable_models)
		return (struct oval_variable_model_iterator *) oval_collection_iterator(model->bound_variable_models);
	else
		return (struct oval_variable_model_iterator *) oval_collection_iterator_new();
}

void oval_definition_model_clear_external_variables(struct oval_definition_model *model)
{
	struct oval_variable_iterator *vars_itr;

	vars_itr = oval_definition_model_get_variables(model);
	while (oval_variable_iterator_has_more(vars_itr)) {
		struct oval_variable *var;

		var = oval_variable_iterator_next(vars_itr);
		if (oval_variable_get_type(var) != OVAL_VARIABLE_EXTERNAL)
			continue;

		oval_variable_clear_values(var);
	}
	oval_variable_iterator_free(vars_itr);
}

struct oval_definition_iterator *oval_definition_model_get_definitions(struct oval_definition_model
								       *model)
{
	__attribute__nonnull__(model);

	struct oval_definition_iterator *iterator =
	    (struct oval_definition_iterator *)oval_string_map_values(model->definition_map);

	return iterator;
}

struct oval_string_iterator *oval_definition_model_get_definitions_dependent_on_variable(struct oval_definition_model *model, struct oval_variable *variable)
{
	__attribute__nonnull__(model);
	__attribute__nonnull__(variable);

	if (model->vardef_map == NULL)
		model->vardef_map = oval_definition_model_build_vardef_mapping(model);

	struct oval_string_map *def_list = (struct oval_string_map *) oval_string_map_get_value(model->vardef_map, oval_variable_get_id(variable));
	return (struct oval_string_iterator *) (def_list != NULL ?
		oval_string_map_keys(def_list) : oval_collection_iterator_new());
}

struct oval_test_iterator *oval_definition_model_get_tests(struct oval_definition_model *model)
{
	__attribute__nonnull__(model);

	struct oval_test_iterator *iterator = (struct oval_test_iterator *)oval_string_map_values(model->test_map);

	return iterator;
}

struct oval_object_iterator *oval_definition_model_get_objects(struct oval_definition_model *model)
{
	__attribute__nonnull__(model);

	struct oval_object_iterator *iterator =
	    (struct oval_object_iterator *)oval_string_map_values(model->object_map);

	return iterator;
}

struct oval_state_iterator *oval_definition_model_get_states(struct oval_definition_model *model)
{
	__attribute__nonnull__(model);

	struct oval_state_iterator *iterator = (struct oval_state_iterator *)oval_string_map_values(model->state_map);

	return iterator;
}

struct oval_variable_iterator *oval_definition_model_get_variables(struct oval_definition_model
								   *model)
{
	__attribute__nonnull__(model);

	struct oval_variable_iterator *iterator =
	    (struct oval_variable_iterator *)oval_string_map_values(model->variable_map);

	return iterator;
}

struct oval_definition *oval_definition_model_get_new_definition(struct oval_definition_model *model, const char *id)
{
	struct oval_definition *definition = oval_definition_model_get_definition(model, id);
	if (definition == NULL) {
		definition = oval_definition_new(model, id);
	}
	return definition;
}

struct oval_variable *oval_definition_model_get_new_variable(struct oval_definition_model *model, const char *id, oval_variable_type_t type)
{
	struct oval_variable *variable = oval_definition_model_get_variable(model, id);
	if (variable == NULL) {
		variable = oval_variable_new(model, id, type);
	} else if (type != OVAL_VARIABLE_UNKNOWN) {
		oval_variable_set_type(variable, type);
	}
	return variable;
}

struct oval_state *oval_definition_model_get_new_state(struct oval_definition_model *model, const char *id)
{
	struct oval_state *state = oval_definition_model_get_state(model, id);
	if (state == NULL) {
		state = oval_state_new(model, id);
	}
	return state;
}

struct oval_object *oval_definition_model_get_new_object(struct oval_definition_model *model, const char *id)
{
	struct oval_object *object = oval_definition_model_get_object(model, id);
	if (object == NULL) {
		object = oval_object_new(model, id);
	}
	return object;
}

struct oval_test *oval_definition_model_get_new_test(struct oval_definition_model *model, const char *id)
{
	struct oval_test *test = oval_definition_model_get_test(model, id);
	if (test == NULL) {
		test = oval_test_new(model, id);
	}
	return test;
}

xmlNode *oval_definition_model_to_dom(struct oval_definition_model *definition_model, xmlDocPtr doc, xmlNode * parent)
{

	xmlNodePtr root_node = NULL;

	if (parent) { /* result file */
		root_node = xmlNewTextChild(parent, NULL, BAD_CAST OVAL_ROOT_ELM_DEFINITIONS, NULL);
	} else {      /* definitions file, we are the root */
		root_node = xmlNewNode(NULL, BAD_CAST OVAL_ROOT_ELM_DEFINITIONS);
		xmlDocSetRootElement(doc, root_node);
	}
	xmlNewNsProp(root_node, lookup_xsi_ns(doc), BAD_CAST "schemaLocation", BAD_CAST definition_model->schema);

	xmlNs *ns_common = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE, BAD_CAST "oval");
	xmlNs *ns_unix = xmlNewNs(root_node, OVAL_DEFINITIONS_UNIX_NS, BAD_CAST "unix-def");
	xmlNs *ns_ind = xmlNewNs(root_node, OVAL_DEFINITIONS_IND_NS, BAD_CAST "ind-def");
	xmlNs *ns_lin = xmlNewNs(root_node, OVAL_DEFINITIONS_LIN_NS, BAD_CAST "lin-def");
	xmlNs *ns_defntns = xmlNewNs(root_node, OVAL_DEFINITIONS_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_unix);
	xmlSetNs(root_node, ns_ind);
	xmlSetNs(root_node, ns_lin);
	xmlSetNs(root_node, ns_defntns);

	/* Always report the generator */
	oval_generator_to_dom(definition_model->generator, doc, root_node);

	/* Report definitions */
	struct oval_definition_iterator *definitions = oval_definition_model_get_definitions(definition_model);
	if (oval_definition_iterator_has_more(definitions)) {
		xmlNode *definitions_node = NULL;
		while(oval_definition_iterator_has_more(definitions)) {
			struct oval_definition *definition = oval_definition_iterator_next(definitions);
			if (definitions_node == NULL) {
				definitions_node = xmlNewTextChild(root_node, ns_defntns, BAD_CAST "definitions", NULL);
			}
			oval_definition_to_dom(definition, doc, definitions_node);
		}
	}
        oval_definition_iterator_free(definitions);

	/* Report tests */
	struct oval_test_iterator *tests = oval_definition_model_get_tests(definition_model);
	if (oval_test_iterator_has_more(tests)) {
		xmlNode *tests_node = xmlNewTextChild(root_node, ns_defntns, BAD_CAST "tests", NULL);
		while (oval_test_iterator_has_more(tests)) {
			struct oval_test *test = oval_test_iterator_next(tests);
			oval_test_to_dom(test, doc, tests_node);
		}
	}
	oval_test_iterator_free(tests);

	/* Report objects */
	struct oval_object_iterator *objects = oval_definition_model_get_objects(definition_model);
	if (oval_object_iterator_has_more(objects)) {
		xmlNode *objects_node = xmlNewTextChild(root_node, ns_defntns, BAD_CAST "objects", NULL);
		while(oval_object_iterator_has_more(objects)) {
			struct oval_object *object = oval_object_iterator_next(objects);
			if (oval_object_get_base_obj(object))
				/* Skip internal objects */
				continue;
			oval_object_to_dom(object, doc, objects_node);
		}
	}
	oval_object_iterator_free(objects);

	/* Report states */
	struct oval_state_iterator *states = oval_definition_model_get_states(definition_model);
	if (oval_state_iterator_has_more(states)) {
		xmlNode *states_node = xmlNewTextChild(root_node, ns_defntns, BAD_CAST "states", NULL);
		while (oval_state_iterator_has_more(states)) {
			struct oval_state *state = oval_state_iterator_next(states);
			oval_state_to_dom(state, doc, states_node);
		}
	}
	oval_state_iterator_free(states);

	/* Report variables */
	struct oval_variable_iterator *variables = oval_definition_model_get_variables(definition_model);
	if (oval_variable_iterator_has_more(variables)) {
		xmlNode *variables_node = xmlNewTextChild(root_node, ns_defntns, BAD_CAST "variables", NULL);
		while (oval_variable_iterator_has_more(variables)) {
			struct oval_variable *variable = oval_variable_iterator_next(variables);
			oval_variable_to_dom(variable, doc, variables_node);
		}
	}
	oval_variable_iterator_free(variables);

	return root_node;
}

int oval_definition_model_export(struct oval_definition_model *model, const char *file)
{

	__attribute__nonnull__(model);

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	oval_definition_model_to_dom(model, doc, NULL);
	return oscap_xml_save_filename_free(file, doc);
}

static void _fp_set_recurse(struct oval_definition_model *model, struct oval_setobject *set, char *set_id)
{
	struct oval_setobject_iterator *subset_itr;

	subset_itr = oval_setobject_get_subsets(set);
	while (oval_setobject_iterator_has_more(subset_itr)) {
		struct oval_setobject *subset;

		subset = oval_setobject_iterator_next(subset_itr);
		if (oval_setobject_get_type(subset) == OVAL_SET_COLLECTIVE)
			oval_set_propagate_filters(model, subset, set_id);
		else
			_fp_set_recurse(model, subset, set_id);
	}
	oval_setobject_iterator_free(subset_itr);
}

void oval_definition_model_optimize_by_filter_propagation(struct oval_definition_model *model)
{
	struct oval_object_iterator *obj_itr;
	struct oval_string_map *processed_obj_map;

	processed_obj_map = oval_string_map_new();

	obj_itr = oval_definition_model_get_objects(model);
	while (oval_object_iterator_has_more(obj_itr)) {
		struct oval_object *obj;
		char *obj_id;
		struct oval_object_content_iterator *cont_itr;
		struct oval_object_content *cont;
		struct oval_setobject *set;
		struct oval_filter_iterator *filter_itr;

		obj = oval_object_iterator_next(obj_itr);
		obj_id = oval_object_get_id(obj);
		if (oval_string_map_get_value(processed_obj_map, obj_id) != NULL)
			continue;

		oval_string_map_put(processed_obj_map, obj_id, obj);

		cont_itr = oval_object_get_object_contents(obj);
		if (!oval_object_content_iterator_has_more(cont_itr)) {
			oval_object_content_iterator_free(cont_itr);
			continue;
		}

		cont = oval_object_content_iterator_next(cont_itr);
		oval_object_content_iterator_free(cont_itr);
		if (oval_object_content_get_type(cont) != OVAL_OBJECTCONTENT_SET)
			continue;

		set = oval_object_content_get_setobject(cont);
		if (oval_setobject_get_type(set) == OVAL_SET_AGGREGATE) {
			_fp_set_recurse(model, set, obj_id);
			continue;
		}

		filter_itr = oval_setobject_get_filters(set);
		if (!oval_filter_iterator_has_more(filter_itr)) {
			oval_filter_iterator_free(filter_itr);
			continue;
		}
		oval_filter_iterator_free(filter_itr);

		oval_set_propagate_filters(model, set, obj_id);
	}
	oval_object_iterator_free(obj_itr);

	oval_string_map_free(processed_obj_map, NULL);
}
