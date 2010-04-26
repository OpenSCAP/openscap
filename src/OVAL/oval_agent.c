/**
 * @file oval_agent.c
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
 */

#include <string.h>
#include <time.h>
#include "config.h"
#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "oval_string_map_impl.h"
#include "oval_system_characteristics_impl.h"
#include "oval_probe_impl.h"
#include "oval_results_impl.h"
#include "../common/util.h"
#include "../common/public/debug.h"
#include "../common/_error.h"

/***************************************************************************/
/* Variable definitions
 * */

typedef struct oval_definition_model {
	struct oval_string_map *definition_map;
	struct oval_string_map *test_map;
	struct oval_string_map *object_map;
	struct oval_string_map *state_map;
	struct oval_string_map *variable_map;
	bool is_locked;
} oval_definition_model_t;

typedef struct oval_syschar_model {
	struct oval_sysinfo *sysinfo;
	struct oval_definition_model *definition_model;
	struct oval_string_map *syschar_map;
	struct oval_string_map *sysdata_map;
	struct oval_string_map *variable_binding_map;
	bool is_locked;
} oval_syschar_model_t;

struct oval_results_model {
	struct oval_definition_model *definition_model;
	struct oval_collection *systems;
	bool is_locked;
};

typedef struct oval_results_model oval_results_model_t;
typedef void (*_oval_result_system_clone_func) (void *, struct oval_definition_model *);
typedef void (*_oval_syschar_model_clone_func) (void *, struct oval_syschar_model *);

/* End of variable definitions
 * */
/***************************************************************************/
/***************************************************************************/
/* Static functions definitions
 * */

static void _oval_agent_scan_entity_for_references(struct oval_entity *entity,
						   struct oval_string_map *objmap,
						   struct oval_string_map *sttmap, struct oval_string_map *varmap);

static void _oval_agent_scan_set_for_references(struct oval_setobject *set,
						struct oval_string_map *objmap,
						struct oval_string_map *sttmap, struct oval_string_map *varmap);
/* End of static functions definitions
 * */
/***************************************************************************/

/* failed   - NULL 
 * success  - oval_definition_model 
 * */
struct oval_definition_model *oval_definition_model_new()
{
	oval_definition_model_t *newmodel = (oval_definition_model_t *) oscap_alloc(sizeof(oval_definition_model_t));
	if (newmodel == NULL)
		return NULL;

	newmodel->definition_map = oval_string_map_new();
	newmodel->object_map = oval_string_map_new();
	newmodel->state_map = oval_string_map_new();
	newmodel->test_map = oval_string_map_new();
	newmodel->variable_map = oval_string_map_new();
	newmodel->is_locked = false;
	return newmodel;
}

void oval_definition_model_lock(struct oval_definition_model *definition_model)
{
	__attribute__nonnull__(definition_model);

	if (definition_model && oval_definition_model_is_valid(definition_model))
		definition_model->is_locked = true;
}

bool oval_definition_model_is_locked(struct oval_definition_model *definition_model)
{
	__attribute__nonnull__(definition_model);

	return definition_model->is_locked;
}

bool oval_definition_model_is_valid(struct oval_definition_model * definition_model)
{
	bool is_valid = true;
	struct oval_definition_iterator *definitions_itr;
	struct oval_test_iterator *tests_itr;
	struct oval_object_iterator *objects_itr;
	struct oval_state_iterator *states_itr;

	if (definition_model == NULL)
		return false;

	/* validate definitions */
	definitions_itr = oval_definition_model_get_definitions(definition_model);
	while (oval_definition_iterator_has_more(definitions_itr)) {
		struct oval_definition *definition;

		definition = oval_definition_iterator_next(definitions_itr);
		if (oval_definition_is_valid(definition) != true) {
			is_valid = false;
			break;
		}
	}
	oval_definition_iterator_free(definitions_itr);
	if (is_valid != true)
		return false;

	/* validate tests */
	tests_itr = oval_definition_model_get_tests(definition_model);
	while (oval_test_iterator_has_more(tests_itr)) {
		struct oval_test *test;

		test = oval_test_iterator_next(tests_itr);
		if (oval_test_is_valid(test) != true) {
			is_valid = false;
			break;
		}
	}
	oval_test_iterator_free(tests_itr);
	if (is_valid != true)
		return false;

	/* validate objects */
	objects_itr = oval_definition_model_get_objects(definition_model);
	while (oval_object_iterator_has_more(objects_itr)) {
		struct oval_object *obj;

		obj = oval_object_iterator_next(objects_itr);
		if (oval_object_is_valid(obj) != true) {
			is_valid = false;
			break;
		}
	}
	oval_object_iterator_free(objects_itr);
	if (is_valid != true)
		return false;

	/* validate states */
	states_itr = oval_definition_model_get_states(definition_model);
	while (oval_state_iterator_has_more(states_itr)) {
		struct oval_state *state;

		state = oval_state_iterator_next(states_itr);
		if (oval_state_is_valid(state) != true) {
			is_valid = false;
			break;
		}
	}
	oval_state_iterator_free(states_itr);
	if (is_valid != true)
		return false;

	return true;
}

static void _oval_definition_model_clone(struct oval_string_map *oldmap,
					 struct oval_definition_model *newmodel, _oval_result_system_clone_func cloner)
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
	    (oldmodel->definition_map, newmodel, (_oval_result_system_clone_func) oval_definition_clone);
	_oval_definition_model_clone
	    (oldmodel->object_map, newmodel, (_oval_result_system_clone_func) oval_object_clone);
	_oval_definition_model_clone(oldmodel->state_map, newmodel, (_oval_result_system_clone_func) oval_state_clone);
	_oval_definition_model_clone(oldmodel->test_map, newmodel, (_oval_result_system_clone_func) oval_test_clone);
	_oval_definition_model_clone
	    (oldmodel->variable_map, newmodel, (_oval_result_system_clone_func) oval_variable_clone);
	return newmodel;
}

void oval_definition_model_free(struct oval_definition_model *model)
{
	__attribute__nonnull__(model);

	oval_string_map_free(model->definition_map, (oscap_destruct_func) oval_definition_free);
	oval_string_map_free(model->object_map, (oscap_destruct_func) oval_object_free);
	oval_string_map_free(model->state_map, (oscap_destruct_func) oval_state_free);
	oval_string_map_free(model->test_map, (oscap_destruct_func) oval_test_free);
	oval_string_map_free(model->variable_map, (oscap_destruct_func) oval_variable_free);

	model->definition_map = NULL;
	model->object_map = NULL;
	model->state_map = NULL;
	model->test_map = NULL;
	model->variable_map = NULL;

	oscap_free(model);
}

/* failed   - NULL 
 * success  - oval_definition_model 
 * */
struct oval_syschar_model *oval_syschar_model_new(struct oval_definition_model *definition_model)
{
	oval_syschar_model_t *newmodel = (oval_syschar_model_t *) oscap_alloc(sizeof(oval_syschar_model_t));
	if (newmodel == NULL)
		return NULL;

	newmodel->sysinfo = NULL;
	newmodel->definition_model = definition_model;
	newmodel->syschar_map = oval_string_map_new();
	newmodel->sysdata_map = oval_string_map_new();
	newmodel->variable_binding_map = oval_string_map_new();
	newmodel->is_locked = false;

	/* check possible allocation problems */
	if ((newmodel->syschar_map == NULL) ||
	    (newmodel->sysdata_map == NULL) || (newmodel->variable_binding_map == NULL)) {
		oval_syschar_model_free(newmodel);
		return NULL;
	}

	return newmodel;
}

void oval_syschar_model_lock(struct oval_syschar_model *syschar_model)
{
	__attribute__nonnull__(syschar_model);

	if (syschar_model && oval_syschar_model_is_valid(syschar_model))
		syschar_model->is_locked = true;
}

bool oval_syschar_model_is_locked(struct oval_syschar_model *syschar_model)
{
	__attribute__nonnull__(syschar_model);

	return syschar_model->is_locked;
}

bool oval_syschar_model_is_valid(struct oval_syschar_model * syschar_model)
{
	return true;		//TODO
}

static void _oval_syschar_model_clone_variable_binding(struct oval_variable_binding *old_binding,
						       struct oval_syschar_model *new_model)
{

	struct oval_definition_model *new_defmodel = oval_syschar_model_get_definition_model(new_model);
	struct oval_variable_binding *new_binding = oval_variable_binding_clone(old_binding, new_defmodel);
	oval_syschar_model_add_variable_binding(new_model, new_binding);
}

static void _oval_syschar_model_clone(struct oval_string_map *oldmap,
				      struct oval_syschar_model *newmodel, _oval_syschar_model_clone_func cloner)
{
	struct oval_string_iterator *keys = (struct oval_string_iterator *)oval_string_map_keys(oldmap);

	while (oval_string_iterator_has_more(keys)) {
		char *key = oval_string_iterator_next(keys);
		void *olditem = oval_string_map_get_value(oldmap, key);
		(*cloner) (newmodel, olditem);
	}
	oval_string_iterator_free(keys);
}

struct oval_syschar_model *oval_syschar_model_clone(struct oval_syschar_model *old_model)
{

	__attribute__nonnull__(old_model);

	struct oval_syschar_model *new_model = oval_syschar_model_new(old_model->definition_model);

	_oval_syschar_model_clone(old_model->syschar_map, new_model,
				  (_oval_syschar_model_clone_func) oval_syschar_clone);
	_oval_syschar_model_clone(old_model->sysdata_map, new_model,
				  (_oval_syschar_model_clone_func) oval_sysdata_clone);
	_oval_syschar_model_clone(old_model->variable_binding_map, new_model,
				  (_oval_syschar_model_clone_func) _oval_syschar_model_clone_variable_binding);

	struct oval_sysinfo *old_sysinfo = oval_syschar_model_get_sysinfo(old_model);
	struct oval_sysinfo *new_sysinfo = oval_sysinfo_clone(new_model, old_sysinfo);
	oval_syschar_model_set_sysinfo(new_model, new_sysinfo);

	return new_model;
}

void oval_syschar_model_free(struct oval_syschar_model *model)
{
	__attribute__nonnull__(model);

	if (model->sysinfo)
		oval_sysinfo_free(model->sysinfo);
	if (model->syschar_map)
		oval_string_map_free(model->syschar_map, (oscap_destruct_func) oval_syschar_free);
	if (model->sysdata_map)
		oval_string_map_free(model->sysdata_map, (oscap_destruct_func) oval_sysdata_free);
	if (model->variable_binding_map)
		oval_string_map_free(model->variable_binding_map, (oscap_destruct_func) oval_variable_binding_free);

	model->sysinfo = NULL;
	model->definition_model = NULL;
	model->syschar_map = NULL;
	model->sysdata_map = NULL;
	model->variable_binding_map = NULL;
	oscap_free(model);
}

struct oval_definition_model *oval_syschar_model_get_definition_model(struct oval_syschar_model *model)
{
	__attribute__nonnull__(model);

	return model->definition_model;
}

struct oval_syschar_iterator *oval_syschar_model_get_syschars(struct oval_syschar_model *model)
{
	__attribute__nonnull__(model);

	struct oval_syschar_iterator *iterator =
	    (struct oval_syschar_iterator *)oval_string_map_values(model->syschar_map);
	return iterator;
}

struct oval_sysinfo *oval_syschar_model_get_sysinfo(struct oval_syschar_model *model)
{
	__attribute__nonnull__(model);

	return model->sysinfo;
}

void oval_syschar_model_set_sysinfo(struct oval_syschar_model *model, struct oval_sysinfo *sysinfo)
{
	if (model && !oval_syschar_model_is_locked(model)) {
		model->sysinfo = oval_sysinfo_clone(model, sysinfo);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_definition_model_add_definition(struct oval_definition_model *model, struct oval_definition *definition)
{
	if (model && !oval_definition_model_is_locked(model)) {
		char *key = oval_definition_get_id(definition);
		oval_string_map_put(model->definition_map, key, (void *)definition);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_definition_model_add_test(struct oval_definition_model *model, struct oval_test *test)
{
	if (model && !oval_definition_model_is_locked(model)) {
		char *key = oval_test_get_id(test);
		oval_string_map_put(model->test_map, key, (void *)test);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_definition_model_add_object(struct oval_definition_model *model, struct oval_object *object)
{
	if (model && !oval_definition_model_is_locked(model)) {
		char *key = oval_object_get_id(object);
		oval_string_map_put(model->object_map, key, (void *)object);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_definition_model_add_state(struct oval_definition_model *model, struct oval_state *state)
{
	if (model && !oval_definition_model_is_locked(model)) {
		char *key = oval_state_get_id(state);
		oval_string_map_put(model->state_map, key, (void *)state);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_definition_model_add_variable(struct oval_definition_model *model, struct oval_variable *variable)
{
	if (model && !oval_definition_model_is_locked(model)) {
		char *key = oval_variable_get_id(variable);
		oval_string_map_put(model->variable_map, key, (void *)variable);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_syschar_model_add_syschar(struct oval_syschar_model *model, struct oval_syschar *syschar)
{
	if (model && !oval_syschar_model_is_locked(model)) {
		struct oval_object *object = oval_syschar_get_object(syschar);
		if (object != NULL) {
			char *id = oval_object_get_id(object);
			oval_string_map_put(model->syschar_map, id, syschar);
		}
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_syschar_model_add_variable_binding(struct oval_syschar_model *model, struct oval_variable_binding *binding)
{
	if (model && !oval_syschar_model_is_locked(model)) {
		struct oval_variable *variable = oval_variable_binding_get_variable(binding);
		char *varid = oval_variable_get_id(variable);
		oval_string_map_put(model->variable_binding_map, varid, binding);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

int oval_syschar_model_probe_objects(struct oval_syschar_model *syschar_model)
{
#ifdef ENABLE_PROBES
	__attribute__nonnull__(syschar_model);

	struct oval_definition_model *definition_model = oval_syschar_model_get_definition_model(syschar_model);

	if (definition_model) {
		oval_pctx_t *pctx;
		struct oval_object_iterator *objects = oval_definition_model_get_objects(definition_model);

		pctx = oval_pctx_new(syschar_model);

		while (oval_object_iterator_has_more(objects)) {
			struct oval_object *object = oval_object_iterator_next(objects);
			char *objid = oval_object_get_id(object);
			struct oval_syschar *syschar = oval_syschar_model_get_syschar(syschar_model, objid);

			if (syschar == NULL) {
				syschar = oval_probe_object_eval(pctx, object);
				if (syschar == NULL) {
					syschar = oval_syschar_new(syschar_model, object);
					oval_syschar_set_flag(syschar, SYSCHAR_FLAG_NOT_COLLECTED);
					if(  oscap_err() ) {
						oval_syschar_model_add_syschar(syschar_model, syschar);
						oval_pctx_free(pctx);
						oval_object_iterator_free(objects);
						return 1;
					} 
				}
				oval_syschar_model_add_syschar(syschar_model, syschar);
			}
		}
		oval_pctx_free(pctx);
		oval_object_iterator_free(objects);
	}
	
	return 0;
#else
	oscap_seterr(OSCAP_EFAMILY_OSCAP, OSCAP_ENOTIMPL, "This feature is not implemented, compiled without probes support.");
#endif
}

int oval_syschar_model_probe_sysinfo(struct oval_syschar_model *syschar_model) {
#ifdef ENABLE_PROBES
	oval_pctx_t *pctx;
        struct oval_sysinfo *sysinfo;

	pctx = oval_pctx_new(syschar_model);
        sysinfo = oval_probe_sysinf_eval(pctx);
        if( sysinfo == NULL && oscap_err() ) {
		oval_pctx_free(pctx);
		return 1;
	}

        oval_syschar_model_set_sysinfo(syschar_model, sysinfo);
        oval_sysinfo_free(sysinfo);
	oval_pctx_free(pctx);

	return 0;
#else
	oscap_seterr(OSCAP_EFAMILY_OSCAP, OSCAP_ENOTIMPL, "This feature is not implemented, compiled without probes support.");
#endif
}


void oval_syschar_model_add_sysdata(struct oval_syschar_model *model, struct oval_sysdata *sysdata)
{
	if (model && !oval_syschar_model_is_locked(model)) {
		char *id = oval_sysdata_get_id(sysdata);
		if (id != NULL) {
			oval_string_map_put(model->sysdata_map, id, sysdata);
		}
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

int oval_definition_model_import(struct oval_definition_model *model,
				 struct oscap_import_source *source, void *user_arg)
{
	__attribute__nonnull__(source);

	xmlTextReader *reader = xmlNewTextReaderFilename(oscap_import_source_get_name(source));

	int retcode = 0;
	if (reader) {
		if (xmlTextReaderRead(reader) > -1) {
			retcode = ovaldef_parser_parse(model, reader, user_arg);
		}
		xmlFreeTextReader(reader);
	} else {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	return retcode;
}

int oval_syschar_model_import(struct oval_syschar_model *model, struct oscap_import_source *source, void *user_arg)
{
	__attribute__nonnull__(source);

	int ret;

	xmlDoc *doc = xmlParseFile(oscap_import_source_get_name(source));
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	xmlTextReader *reader = xmlNewTextReaderFilename(oscap_import_source_get_name(source));
	if (reader == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	xmlTextReaderRead(reader);
	ret = ovalsys_parser_parse(model, reader, user_arg);

	xmlFreeTextReader(reader);
	xmlFreeDoc(doc);
	return ret;
}

struct oval_definition *oval_definition_model_get_definition(struct oval_definition_model *model, char *key)
{
	__attribute__nonnull__(model);

	return (struct oval_definition *)oval_string_map_get_value(model->definition_map, key);
}

struct oval_test *oval_definition_model_get_test(struct oval_definition_model *model, char *key)
{
	__attribute__nonnull__(model);

	return (struct oval_test *)oval_string_map_get_value(model->test_map, key);
}

struct oval_object *oval_definition_model_get_object(struct oval_definition_model *model, char *key)
{
	__attribute__nonnull__(model);

	return (struct oval_object *)oval_string_map_get_value(model->object_map, key);
}

struct oval_state *oval_definition_model_get_state(struct oval_definition_model *model, char *key)
{
	__attribute__nonnull__(model);

	return (struct oval_state *)oval_string_map_get_value(model->state_map, key);
}

struct oval_variable *oval_definition_model_get_variable(struct oval_definition_model *model, char *key)
{
	__attribute__nonnull__(model);

	return (struct oval_variable *)oval_string_map_get_value(model->variable_map, key);
}

void oval_definition_model_bind_variable_model(struct oval_definition_model *defmodel,
					       struct oval_variable_model *varmodel)
{
	//Bind values to all external variables specified in the variable model.
	struct oval_variable_iterator *variables = oval_definition_model_get_variables(defmodel);
	while (oval_variable_iterator_has_more(variables)) {
		struct oval_variable *variable = oval_variable_iterator_next(variables);
		if (oval_variable_get_type(variable) == OVAL_VARIABLE_EXTERNAL) {
			char *varid = oval_variable_get_id(variable);
			oval_datatype_t var_datatype = oval_variable_model_get_datatype(varmodel, varid);
			if (var_datatype) {	//values are bound in the variable model
				oval_datatype_t def_datatype = oval_variable_get_datatype(variable);
				if (def_datatype == var_datatype) {
					struct oval_string_iterator *values =
					    oval_variable_model_get_values(varmodel, varid);
					while (oval_string_iterator_has_more(values)) {
						char *text = oval_string_iterator_next(values);
						struct oval_value *value = oval_value_new(var_datatype, text);
						oval_variable_add_value(variable, value);
					}
					oval_string_iterator_free(values);
				} else {
					oscap_dprintf("WARNING: Unmatched variable datatypes"
						      "                             varid = %s; "
						      "         definition_model datatype = %s; "
						      "           variable_model datatype = %s; ",
						      varid,
						      oval_datatype_get_text(def_datatype),
						      oval_datatype_get_text(var_datatype));
				}
			}
		}
	}
	oval_variable_iterator_free(variables);
}

void oval_syschar_model_bind_variable_model(struct oval_syschar_model *sysmodel, struct oval_variable_model *varmodel)
{
	__attribute__nonnull__(sysmodel);

	oval_definition_model_bind_variable_model(sysmodel->definition_model, varmodel);
}

struct oval_syschar *oval_syschar_model_get_syschar(struct oval_syschar_model *model, char *object_id)
{
	__attribute__nonnull__(model);

	return (struct oval_syschar *)oval_string_map_get_value(model->syschar_map, object_id);
}

struct oval_sysdata *oval_syschar_model_get_sysdata(struct oval_syschar_model *model, char *id)
{
	__attribute__nonnull__(model);

	return (struct oval_sysdata *)oval_string_map_get_value(model->sysdata_map, id);
}

struct oval_definition_iterator *oval_definition_model_get_definitions(struct oval_definition_model
								       *model)
{
	__attribute__nonnull__(model);

	struct oval_definition_iterator *iterator =
	    (struct oval_definition_iterator *)oval_string_map_values(model->definition_map);

	return iterator;
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

struct oval_syschar *oval_syschar_get_new(struct oval_syschar_model *model, struct oval_object *object)
{
	char *object_id = oval_object_get_id(object);
	struct oval_syschar *syschar = oval_syschar_model_get_syschar(model, object_id);
	if (syschar == NULL) {
		syschar = oval_syschar_new(model, object);
		oval_syschar_model_add_syschar(model, syschar);
	}
	return syschar;
}

struct oval_sysdata *oval_sysdata_get_new(struct oval_syschar_model *model, char *id)
{
	struct oval_sysdata *sysdata = oval_syschar_model_get_sysdata(model, id);
	if (sysdata == NULL) {
		sysdata = oval_sysdata_new(model, id);
		oval_syschar_model_add_sysdata(model, sysdata);
	}
	return sysdata;
}

struct oval_definition *oval_definition_get_new(struct oval_definition_model *model, char *id)
{
	struct oval_definition *definition = oval_definition_model_get_definition(model, id);
	if (definition == NULL) {
		definition = oval_definition_new(model, id);
		oval_definition_model_add_definition(model, definition);
	}
	return definition;
}

struct oval_variable *oval_variable_get_new(struct oval_definition_model *model, char *id, oval_variable_type_t type)
{
	struct oval_variable *variable = oval_definition_model_get_variable(model, id);
	if (variable == NULL) {
		variable = oval_variable_new(model, id, type);
		oval_definition_model_add_variable(model, variable);
	} else {
		oval_variable_set_type(variable, type);
	}
	return variable;
}

struct oval_state *oval_state_get_new(struct oval_definition_model *model, char *id)
{
	struct oval_state *state = oval_definition_model_get_state(model, id);
	if (state == NULL) {
		state = oval_state_new(model, id);
		oval_definition_model_add_state(model, state);
	}
	return state;
}

struct oval_object *oval_object_get_new(struct oval_definition_model *model, char *id)
{
	struct oval_object *object = oval_definition_model_get_object(model, id);
	if (object == NULL) {
		object = oval_object_new(model, id);
		oval_definition_model_add_object(model, object);
	}
	return object;
}

struct oval_test *oval_test_get_new(struct oval_definition_model *model, char *id)
{
	struct oval_test *test = oval_definition_model_get_test(model, id);
	if (test == NULL) {
		test = oval_test_new(model, id);
		oval_definition_model_add_test(model, test);
	}
	return test;
}

static int _generator_to_dom(xmlDocPtr doc, xmlNode * tag_generator)
{
	xmlNs *ns_common = xmlSearchNsByHref(doc, tag_generator, OVAL_COMMON_NAMESPACE);
	xmlNewChild(tag_generator, ns_common, BAD_CAST "product_name", BAD_CAST "OPEN SCAP");
	xmlNewChild(tag_generator, ns_common, BAD_CAST "schema_version", BAD_CAST "5.5");
	time_t epoch_time[] = { 0 };
	time(epoch_time);
	struct tm *lt = localtime(epoch_time);
	char timestamp[] = "yyyy-mm-ddThh:mm:ss";
	snprintf(timestamp, sizeof(timestamp), "%4d-%02d-%02dT%02d:%02d:%02d",
		 1900 + lt->tm_year, 1 + lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
	xmlNewChild(tag_generator, ns_common, BAD_CAST "timestamp", BAD_CAST timestamp);

	return 1;
}

struct oval_results_model *oval_results_model_new(struct oval_definition_model *definition_model,
						  struct oval_syschar_model **syschar_models)
{
	oval_results_model_t *model = (oval_results_model_t *) oscap_alloc(sizeof(oval_results_model_t));
	if (model == NULL)
		return NULL;

	model->systems = oval_collection_new();
	model->definition_model = definition_model;
	model->is_locked = false;
	if (syschar_models) {
		struct oval_syschar_model *syschar_model;
		for (syschar_model = *syschar_models; syschar_model; syschar_model = *(++syschar_models)) {
			struct oval_result_system *sys = oval_result_system_new(model, syschar_model);
			oval_results_model_add_system(model, sys);
		}
	}
	return model;
}

void oval_results_model_lock(struct oval_results_model *results_model)
{
	__attribute__nonnull__(results_model);

	if (results_model && oval_results_model_is_valid(results_model))
		results_model->is_locked = true;
}

bool oval_results_model_is_locked(struct oval_results_model *results_model)
{
	__attribute__nonnull__(results_model);

	return results_model->is_locked;
}

bool oval_results_model_is_valid(struct oval_results_model * results_model)
{
	return true;		//TODO
}

struct oval_results_model *oval_results_model_clone(struct oval_results_model *old_resmodel)
{
	struct oval_definition_model *old_defmodel = oval_results_model_get_definition_model(old_resmodel);
	struct oval_results_model *new_resmodel = oval_results_model_new(old_defmodel, NULL);
	struct oval_result_system_iterator *old_systems = oval_results_model_get_systems(old_resmodel);

	while (oval_result_system_iterator_has_more(old_systems)) {

		struct oval_result_system *old_system = oval_result_system_iterator_next(old_systems);
		struct oval_result_system *new_system = oval_result_system_clone(new_resmodel, old_system);

		oval_results_model_add_system(new_resmodel, new_system);
	}

	oval_result_system_iterator_free(old_systems);
	return new_resmodel;
}

void oval_results_model_free(struct oval_results_model *model)
{
	__attribute__nonnull__(model);

	oval_collection_free_items(model->systems, (oscap_destruct_func) oval_result_system_free);
	model->definition_model = NULL;
	model->systems = NULL;
	oscap_free(model);
}

struct oval_definition_model *oval_results_model_get_definition_model(struct oval_results_model *model) {
	__attribute__nonnull__(model);

	return model->definition_model;
}

struct oval_result_system_iterator *oval_results_model_get_systems(struct oval_results_model *model)
{
	__attribute__nonnull__(model);

	return (struct oval_result_system_iterator *)
	    oval_collection_iterator(model->systems);
}

void oval_results_model_add_system(struct oval_results_model *model, struct oval_result_system *sys)
{
	if (model && !oval_results_model_is_locked(model)) {
		if (sys)
			oval_collection_add(model->systems, sys);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

struct oval_result_directives *oval_results_model_import(struct oval_results_model *model,
							 struct oscap_import_source *source, void *client_data)
{
	__attribute__nonnull__(source);

	xmlDoc *doc = xmlParseFile(oscap_import_source_get_name(source));
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	xmlTextReader *reader = xmlNewTextReaderFilename(oscap_import_source_get_name(source));
	if (reader == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	if (xmlTextReaderRead(reader) < 1) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}
	struct oval_result_directives *directives = ovalres_parser_parse(model, reader, client_data);

	xmlFreeTextReader(reader);
	xmlFreeDoc(doc);

	return directives;
}

static void _oval_agent_scan_for_extensions_tests(struct oval_criteria_node *node,
						  struct oval_string_map *extmap,
						  struct oval_string_map *tstmap,
						  oval_definitions_resolver resolver, void *user_arg)
{
	oval_criteria_node_type_t type = oval_criteria_node_get_type(node);
	switch (type) {
	case OVAL_NODETYPE_CRITERIA:{
			struct oval_criteria_node_iterator *subnodes = oval_criteria_node_get_subnodes(node);
			while (oval_criteria_node_iterator_has_more(subnodes)) {
				struct oval_criteria_node *subnode = oval_criteria_node_iterator_next(subnodes);
				_oval_agent_scan_for_extensions_tests(subnode, extmap, tstmap, resolver, user_arg);
			}
			oval_criteria_node_iterator_free(subnodes);
		};
		break;
	case OVAL_NODETYPE_CRITERION:{
			struct oval_test *test = oval_criteria_node_get_test(node);
			char *tstid = oval_test_get_id(test);
			void *value = oval_string_map_get_value(tstmap, tstid);
			if (value == NULL) {
				oval_string_map_put(tstmap, tstid, test);
			}
		};
		break;
	case OVAL_NODETYPE_EXTENDDEF:{
			struct oval_definition *extends = oval_criteria_node_get_definition(node);
			if (resolver && !(*resolver) (extends, user_arg)) {
				char *extid = oval_definition_get_id(extends);
				void *value = oval_string_map_get_value(extmap, extid);
				if (value == NULL) {
					oval_string_map_put(extmap, extid, extends);
				}
			}
		};
		break;
	default:
		break;
	}
}

static void _oval_agent_scan_object_for_references(struct oval_object *object,
						   struct oval_string_map *objmap,
						   struct oval_string_map *sttmap, struct oval_string_map *varmap)
{
	struct oval_object_content_iterator *contents = oval_object_get_object_contents(object);

	while (oval_object_content_iterator_has_more(contents)) {
		struct oval_object_content *content = oval_object_content_iterator_next(contents);
		struct oval_entity *entity = oval_object_content_get_entity(content);
		if (entity)
			_oval_agent_scan_entity_for_references(entity, objmap, sttmap, varmap);
		struct oval_setobject *set = oval_object_content_get_setobject(content);
		if (set)
			_oval_agent_scan_set_for_references(set, objmap, sttmap, varmap);
	}
	oval_object_content_iterator_free(contents);
}

static void _oval_agent_scan_state_for_references(struct oval_state *state,
						  struct oval_string_map *objmap,
						  struct oval_string_map *sttmap, struct oval_string_map *varmap)
{
	struct oval_state_content_iterator *contents = oval_state_get_contents(state);

	while (oval_state_content_iterator_has_more(contents)) {
		struct oval_state_content *content = oval_state_content_iterator_next(contents);
		struct oval_entity *entity = oval_state_content_get_entity(content);
		if (entity)
			_oval_agent_scan_entity_for_references(entity, objmap, sttmap, varmap);
	}
	oval_state_content_iterator_free(contents);
}

static void _oval_agent_scan_component_for_references(struct oval_component *component,
						      struct oval_string_map *objmap,
						      struct oval_string_map *sttmap, struct oval_string_map *varmap)
{
	struct oval_variable *variable = oval_component_get_variable(component);

	if (variable) {
		char *varid = oval_variable_get_id(variable);
		void *value = oval_string_map_get_value(varmap, varid);
		if (value == NULL) {
			oval_string_map_put(varmap, varid, variable);
			struct oval_component *component2 = oval_variable_get_component(variable);
			if (component2) {
				_oval_agent_scan_component_for_references(component2, objmap, sttmap, varmap);
			}
		}
	} else {
		struct oval_component_iterator *fcomponents = oval_component_get_function_components(component);
		if (fcomponents)
			while (oval_component_iterator_has_more(fcomponents)) {
				struct oval_component *fcomponent = oval_component_iterator_next(fcomponents);
				_oval_agent_scan_component_for_references(fcomponent, objmap, sttmap, varmap);
			}
		oval_component_iterator_free(fcomponents);

		struct oval_object *object = oval_component_get_object(component);
		if (object) {
			char *objid = oval_object_get_id(object);
			void *value = oval_string_map_get_value(objmap, objid);
			if (value == NULL) {
				oval_string_map_put(objmap, objid, object);
				_oval_agent_scan_object_for_references(object, objmap, sttmap, varmap);
			}
		}
	}
}

static void _oval_agent_scan_entity_for_references(struct oval_entity *entity,
						   struct oval_string_map *objmap,
						   struct oval_string_map *sttmap, struct oval_string_map *varmap)
{
	struct oval_variable *variable = oval_entity_get_variable(entity);

	if (variable) {
		char *varid = oval_variable_get_id(variable);
		void *value = oval_string_map_get_value(varmap, varid);
		if (value == NULL) {
			oval_string_map_put(varmap, varid, variable);
			struct oval_component *component = oval_variable_get_component(variable);
			if (component) {
				_oval_agent_scan_component_for_references(component, objmap, sttmap, varmap);
			}
		}
	}
}

static void _oval_agent_scan_set_for_references(struct oval_setobject *set,
						struct oval_string_map *objmap,
						struct oval_string_map *sttmap, struct oval_string_map *varmap)
{
	struct oval_object_iterator *objects = oval_setobject_get_objects(set);

	if (objects) {
		while (oval_object_iterator_has_more(objects)) {
			struct oval_object *object = oval_object_iterator_next(objects);
			char *objid = oval_object_get_id(object);
			void *value = oval_string_map_get_value(objmap, objid);
			if (value == NULL) {
				oval_string_map_put(objmap, objid, object);
				_oval_agent_scan_object_for_references(object, objmap, sttmap, varmap);
			}
		}
	}

	oval_object_iterator_free(objects);
	struct oval_state_iterator *states = oval_setobject_get_filters(set);

	if (states) {
		while (oval_state_iterator_has_more(states)) {
			struct oval_state *state = oval_state_iterator_next(states);
			char *sttid = oval_state_get_id(state);
			void *value = oval_string_map_get_value(sttmap, sttid);
			if (value == NULL) {
				oval_string_map_put(sttmap, sttid, state);
				_oval_agent_scan_state_for_references(state, objmap, sttmap, varmap);
			}
		}
	}
	oval_state_iterator_free(states);
	struct oval_setobject_iterator *subsets = oval_setobject_get_subsets(set);
	if (subsets) {
		while (oval_setobject_iterator_has_more(subsets)) {
			struct oval_setobject *subset = oval_setobject_iterator_next(subsets);
			_oval_agent_scan_set_for_references(subset, objmap, sttmap, varmap);
		}
	}
	oval_setobject_iterator_free(subsets);
}

xmlNode *oval_definitions_to_dom(struct oval_definition_model *definition_model,
				 xmlDocPtr doc, xmlNode * parent, oval_definitions_resolver resolver, void *user_arg)
{

	xmlNodePtr root_node = NULL;
	int i = 0;

	if (parent) {
		root_node = xmlNewChild(parent, NULL, BAD_CAST "oval_definitions", NULL);
	} else {
		root_node = xmlNewNode(NULL, BAD_CAST "oval_definitions");
		xmlDocSetRootElement(doc, root_node);
	}
	xmlNs *ns_common = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE, BAD_CAST "oval");
	xmlNs *ns_defntns = xmlNewNs(root_node, OVAL_DEFINITIONS_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_defntns);

	xmlNode *tag_generator = xmlNewChild(root_node, ns_defntns, BAD_CAST "generator", NULL);

	_generator_to_dom(doc, tag_generator);

	struct oval_string_map *tstmap = oval_string_map_new();
	struct oval_definition_iterator *definitions = oval_definition_model_get_definitions(definition_model);
	if (oval_definition_iterator_has_more(definitions)) {
		struct oval_string_map *extmap = oval_string_map_new();
		xmlNode *definitions_node = NULL;
		for (i = 0; oval_definition_iterator_has_more(definitions); i++) {
			struct oval_definition *definition = oval_definition_iterator_next(definitions);
			if (resolver == NULL || (*resolver) (definition, user_arg)) {
				if (definitions_node == NULL) {
					definitions_node
					    = xmlNewChild(root_node, ns_defntns, BAD_CAST "definitions", NULL);
				}
				oval_definition_to_dom(definition, doc, definitions_node);
				struct oval_criteria_node *criteria = oval_definition_get_criteria(definition);
				if (criteria)
					_oval_agent_scan_for_extensions_tests(criteria, extmap, tstmap, resolver,
									      user_arg);
			}
		}
		oval_definition_iterator_free(definitions);
		definitions = (struct oval_definition_iterator *)oval_string_map_values(extmap);
		while (oval_definition_iterator_has_more(definitions)) {
			struct oval_definition *definition = oval_definition_iterator_next(definitions);
			oval_definition_to_dom(definition, doc, definitions_node);
		}
		oval_string_map_free(extmap, NULL);
		oval_definition_iterator_free(definitions);
	}

	struct oval_string_map *objmap = oval_string_map_new();
	struct oval_string_map *sttmap = oval_string_map_new();
	struct oval_string_map *varmap = oval_string_map_new();

	struct oval_test_iterator *tests = oval_definition_model_get_tests(definition_model);
	if (oval_test_iterator_has_more(tests)) {
		xmlNode *tests_node = xmlNewChild(root_node, ns_defntns, BAD_CAST "tests", NULL);
		while (oval_test_iterator_has_more(tests)) {
			struct oval_test *test = oval_test_iterator_next(tests);
			oval_test_to_dom(test, doc, tests_node);
			struct oval_object *object = oval_test_get_object(test);
			if (object) {
				char *object_id = oval_object_get_id(object);
				void *value = oval_string_map_get_value(objmap, object_id);
				if (value == NULL) {
					oval_string_map_put(objmap, object_id, object);
					_oval_agent_scan_object_for_references(object, objmap, sttmap, varmap);
				}
			}
			struct oval_state *state = oval_test_get_state(test);
			if (state) {
				char *state_id = oval_state_get_id(state);
				void *value = oval_string_map_get_value(sttmap, state_id);
				if (value == NULL) {
					oval_string_map_put(sttmap, state_id, state);
					_oval_agent_scan_state_for_references(state, objmap, sttmap, varmap);
				}
			}
		}
	}
	oval_test_iterator_free(tests);

	struct oval_object_iterator *objects = oval_definition_model_get_objects(definition_model);
	if (oval_object_iterator_has_more(objects)) {
		xmlNode *objects_node = xmlNewChild(root_node, ns_defntns, BAD_CAST "objects", NULL);
		for (i = 0; oval_object_iterator_has_more(objects); i++) {
			struct oval_object *object = oval_object_iterator_next(objects);
			oval_object_to_dom(object, doc, objects_node);
		}
	}
	oval_object_iterator_free(objects);
	struct oval_state_iterator *states = oval_definition_model_get_states(definition_model);
	if (oval_state_iterator_has_more(states)) {
		xmlNode *states_node = xmlNewChild(root_node, ns_defntns, BAD_CAST "states", NULL);
		while (oval_state_iterator_has_more(states)) {
			struct oval_state *state = oval_state_iterator_next(states);
			oval_state_to_dom(state, doc, states_node);
		}
	}
	oval_state_iterator_free(states);
	struct oval_variable_iterator *variables = oval_definition_model_get_variables(definition_model);
	if (oval_variable_iterator_has_more(variables)) {
		xmlNode *variables_node = xmlNewChild(root_node, ns_defntns, BAD_CAST "variables", NULL);
		while (oval_variable_iterator_has_more(variables)) {
			struct oval_variable *variable = oval_variable_iterator_next(variables);
			oval_variable_to_dom(variable, doc, variables_node);
		}
	}
	oval_variable_iterator_free(variables);

	oval_string_map_free(objmap, NULL);
	oval_string_map_free(sttmap, NULL);
	oval_string_map_free(varmap, NULL);
	oval_string_map_free(tstmap, NULL);

	return root_node;
}

int oval_definition_model_export(struct oval_definition_model *model, struct oscap_export_target *target)
{

	__attribute__nonnull__(target);

	int retcode = 0;

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	oval_definitions_to_dom(model, doc, NULL, NULL, NULL);
	/*
	 * Dumping document to stdio or file
	 */
	retcode =
	    xmlSaveFormatFileEnc(oscap_export_target_get_name(target), doc, oscap_export_target_get_encoding(target),
				 1);
	if (retcode < 1)
		oscap_setxmlerr(xmlGetLastError());

	xmlFreeDoc(doc);

	return retcode;
}

xmlNode *oval_syschar_model_to_dom(struct oval_syschar_model * syschar_model,
				   xmlDocPtr doc, xmlNode * parent, oval_syschar_resolver resolver, void *user_arg)
{

	xmlNodePtr root_node;

	if (parent) {
		root_node = xmlNewChild(parent, NULL, BAD_CAST "oval_system_characteristics", NULL);
	} else {
		root_node = xmlNewNode(NULL, BAD_CAST "oval_system_characteristics");
		xmlDocSetRootElement(doc, root_node);
	}
	xmlNs *ns_common = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE, BAD_CAST "oval");
	xmlNs *ns_syschar = xmlNewNs(root_node, OVAL_SYSCHAR_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_syschar);

	xmlNode *tag_generator = xmlNewChild(root_node, ns_syschar, BAD_CAST "generator", NULL);

	_generator_to_dom(doc, tag_generator);

	oval_sysinfo_to_dom(oval_syschar_model_get_sysinfo(syschar_model), doc, root_node);

	struct oval_collection *collection = NULL;
	struct oval_syschar_iterator *syschars = oval_syschar_model_get_syschars(syschar_model);
	if (resolver) {
		collection = oval_collection_new();
		while (oval_syschar_iterator_has_more(syschars)) {
			struct oval_syschar *syschar = oval_syschar_iterator_next(syschars);
			if ((*resolver) (syschar, user_arg)) {
				oval_collection_add(collection, syschar);
			}
		}
		oval_syschar_iterator_free(syschars);
		syschars = (struct oval_syschar_iterator *)oval_collection_iterator(collection);
	}

	struct oval_string_map *sysdata_map = oval_string_map_new();
	if (oval_syschar_iterator_has_more(syschars)) {
		xmlNode *tag_objects = xmlNewChild(root_node, ns_syschar, BAD_CAST "collected_objects", NULL);

		while (oval_syschar_iterator_has_more(syschars)) {
			struct oval_syschar *syschar = oval_syschar_iterator_next(syschars);
			oval_syschar_to_dom(syschar, doc, tag_objects);
			struct oval_sysdata_iterator *sysdatas = oval_syschar_get_sysdata(syschar);
			while (oval_sysdata_iterator_has_more(sysdatas)) {
				struct oval_sysdata *sysdata = oval_sysdata_iterator_next(sysdatas);
				oval_string_map_put(sysdata_map, oval_sysdata_get_id(sysdata), sysdata);
			}
			oval_sysdata_iterator_free(sysdatas);
		}
	}
	oval_collection_free(collection);
	oval_syschar_iterator_free(syschars);

	struct oval_iterator *sysdatas = oval_string_map_values(sysdata_map);
	if (oval_collection_iterator_has_more(sysdatas)) {
		xmlNode *tag_items = xmlNewChild(root_node, ns_syschar, BAD_CAST "system_data", NULL);
		while (oval_collection_iterator_has_more(sysdatas)) {
			struct oval_sysdata *sysdata = (struct oval_sysdata *)
			    oval_collection_iterator_next(sysdatas);
			oval_sysdata_to_dom(sysdata, doc, tag_items);
		}
	}
	oval_collection_iterator_free(sysdatas);
	oval_string_map_free(sysdata_map, NULL);

	return root_node;
}

int oval_syschar_model_export(struct oval_syschar_model *model, struct oscap_export_target *target)
{

	__attribute__nonnull__(target);

	int retcode = 0;

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	oval_syschar_model_to_dom(model, doc, NULL, NULL, NULL);
	/*
	 * Dumping document to stdio or file
	 */
	retcode =
	    xmlSaveFormatFileEnc(oscap_export_target_get_name(target), doc, oscap_export_target_get_encoding(target),
				 1);
	if (retcode < 1)
		oscap_setxmlerr(xmlGetLastError());

	xmlFreeDoc(doc);
	return retcode;
}

int oval_results_model_eval(struct oval_results_model *res_model)
{
	struct oval_result_system_iterator *systems_itr;

	systems_itr = oval_results_model_get_systems(res_model);

	while (oval_result_system_iterator_has_more(systems_itr)) {
		struct oval_result_system *sys;

		sys = oval_result_system_iterator_next(systems_itr);
		if( oval_result_system_eval(sys) != 0 ) { 	/* evaluate each result_system */
			oval_result_system_iterator_free(systems_itr);
			return -1;
		}
	}
	oval_result_system_iterator_free(systems_itr);
	return 0;
}

static void _scan_for_viewable_definitions(struct oval_results_model *results_model,
					   struct oval_result_directives *directives, struct oval_string_map *defids)
{
	int i;
	struct oval_result_system_iterator *systems = oval_results_model_get_systems(results_model);

	while (oval_result_system_iterator_has_more(systems)) {
		struct oval_result_system *sys = oval_result_system_iterator_next(systems);
		struct oval_result_definition_iterator *rslt_definitions = oval_result_system_get_definitions(sys);
		for (i = 0; oval_result_definition_iterator_has_more(rslt_definitions); i++) {
			struct oval_result_definition *rslt_definition =
			    oval_result_definition_iterator_next(rslt_definitions);
			oval_result_t result = oval_result_definition_get_result(rslt_definition);
			if (oval_result_directives_get_reported(directives, result)) {
				struct oval_definition *oval_definition =
				    oval_result_definition_get_definition(rslt_definition);
				if (oval_definition) {
					char *defid = oval_definition_get_id(oval_definition);
					oval_string_map_put(defids, defid, oval_definition);
				}
			}
		}
		oval_result_definition_iterator_free(rslt_definitions);
	}
	oval_result_system_iterator_free(systems);
}

static bool _resolve_oval_definition_from_map(struct oval_definition *oval_definition, struct oval_string_map *defids)
{
	char *defid = oval_definition_get_id(oval_definition);
	bool resolved = (oval_string_map_get_value(defids, defid) != NULL);
	return resolved;
}

static xmlNode *oval_results_to_dom(struct oval_results_model *results_model,
				    struct oval_result_directives *directives, xmlDocPtr doc, xmlNode * parent)
{
	xmlNode *root_node;
	if (parent) {
		root_node = xmlNewChild(parent, NULL, BAD_CAST "oval_results", NULL);
	} else {
		root_node = xmlNewNode(NULL, BAD_CAST "oval_results");
		xmlDocSetRootElement(doc, root_node);
	}
	xmlNs *ns_common = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE, BAD_CAST "oval");
	xmlNs *ns_results = xmlNewNs(root_node, OVAL_RESULTS_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_results);

	xmlNode *tag_generator = xmlNewChild(root_node, ns_results, BAD_CAST "generator", NULL);

	_generator_to_dom(doc, tag_generator);
	oval_result_directives_to_dom(directives, doc, root_node);

	//Scan for viewable definitions.
	struct oval_string_map *defids = oval_string_map_new();
	_scan_for_viewable_definitions(results_model, directives, defids);

	struct oval_definition_model *definition_model = oval_results_model_get_definition_model(results_model);
	oval_definitions_to_dom
	    (definition_model, doc, root_node, (oval_definitions_resolver *) _resolve_oval_definition_from_map, defids);

	oval_string_map_free(defids, NULL);

	xmlNode *results_node = xmlNewChild(root_node, ns_results, BAD_CAST "results", NULL);

	struct oval_result_system_iterator *systems = oval_results_model_get_systems(results_model);
	while (oval_result_system_iterator_has_more(systems)) {
		struct oval_result_system *sys = oval_result_system_iterator_next(systems);
		oval_result_system_to_dom(sys, results_model, directives, doc, results_node);
	}
	oval_result_system_iterator_free(systems);

	return root_node;
}

int oval_results_model_export(struct oval_results_model *results_model,
			      struct oval_result_directives *directives, struct oscap_export_target *target)
{
	__attribute__nonnull__(target);

	int xmlCode = 0;

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	oval_results_to_dom(results_model, directives, doc, NULL);
	xmlCode = xmlSaveFormatFileEnc
	    (oscap_export_target_get_name(target), doc, oscap_export_target_get_encoding(target), 1);
	if (xmlCode <= 0) {
		oscap_setxmlerr(xmlGetLastError());
		oscap_dprintf("WARNING: No bytes exported: xmlCode = %d", xmlCode);
	}

	xmlFreeDoc(doc);

	return ((xmlCode >= 1) ? 1 : -1);
}

