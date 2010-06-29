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
#include "common/util.h"
#include "common/debug_priv.h"
#include "common/_error.h"

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
        char *schema;
} oval_definition_model_t;

typedef struct oval_syschar_model {
	struct oval_sysinfo *sysinfo;
	struct oval_definition_model *definition_model;
	struct oval_string_map *syschar_map;
	struct oval_string_map *sysdata_map;
	struct oval_string_map *variable_binding_map;
	bool is_locked;
        char *schema;
} oval_syschar_model_t;

struct oval_results_model {
	struct oval_definition_model *definition_model;
	struct oval_collection *systems;
	bool is_locked;
        char *schema;
};

struct oval_agent_session {
	char *filename;
	struct oval_definition_model * def_model;
	struct oval_syschar_model    * sys_model;
	struct oval_syschar_model    * sys_models[2];
	struct oval_results_model    * res_model;
	oval_probe_session_t  * psess;
};


#ifdef ENABLE_XCCDF

struct oval_agent_cb_data {
        struct oval_agent_session   * session;  ///< OVAL Agent session
        oval_agent_result_cb_t      * callback; ///< User callback that is called after evaluation
        void                        * usr;      ///< User data passed to callback
};

/* Macros to generate iterators, getters and setters */
OSCAP_ACCESSOR_SIMPLE(struct oval_agent_session *, oval_agent_cb_data, session)
OSCAP_ACCESSOR_SIMPLE(oval_agent_result_cb_t *, oval_agent_cb_data, callback)
OSCAP_ACCESSOR_SIMPLE(void *, oval_agent_cb_data, usr)

struct oval_agent_cb_data * oval_agent_cb_data_new(void)
{
    struct oval_agent_cb_data * data = oscap_alloc(sizeof(struct oval_agent_cb_data));

    data->session = NULL;
    data->callback = NULL;
    data->usr = NULL;

    return data;
}

void oval_agent_cb_data_free(struct oval_agent_cb_data * data)
{
    if (data == NULL) return;

    //if (data->session != NULL) oval_agent_destroy_session(data->session);
    // We don't want to free usr data, user has to free it by himself
    data->callback = NULL;
    oscap_free(data);
}
/**
 * Specification of structure for transformation of OVAL Result type
 * to XCCDF result type.
 */
struct oval_result_to_xccdf_spec {
	oval_result_t oval;
	xccdf_test_result_type_t xccdf;
};

/**
 * Array of transformation rules from OVAL Result type to XCCDF result type
 */
static const struct oval_result_to_xccdf_spec XCCDF_OVAL_RESULTS_MAP[] = {
	{OVAL_RESULT_TRUE, XCCDF_RESULT_PASS},
	{OVAL_RESULT_FALSE, XCCDF_RESULT_FAIL},
	{OVAL_RESULT_UNKNOWN, XCCDF_RESULT_UNKNOWN},
	{OVAL_RESULT_ERROR, XCCDF_RESULT_ERROR},
	{OVAL_RESULT_NOT_EVALUATED, XCCDF_RESULT_NOT_CHECKED},
	{OVAL_RESULT_NOT_APPLICABLE, XCCDF_RESULT_NOT_APPLICABLE},
	{0, 0}
};

#endif

typedef struct oval_results_model oval_results_model_t;
typedef void (*_oval_result_system_clone_func) (void *, struct oval_definition_model *);
typedef void (*_oval_syschar_model_clone_func) (void *, struct oval_syschar_model *);

/* TODO: Use this to generate links
static char * _oval_generate_schema_location(const char * version)
{

        char * schema = oscap_alloc(sizeof(char)*(strlen(OVAL_DEF_SCHEMA_LOCATION_DEF_PX)+strlen(version)+strlen(OVAL_DEF_SCHEMA_LOCATION_DEF_SX)+2));
        strncat(schema, OVAL_DEF_SCHEMA_LOCATION_DEF_PX, strlen(OVAL_DEF_SCHEMA_LOCATION_DEF_PX));
        strncat(schema, version, strlen(version));
        strncat(schema, OVAL_DEF_SCHEMA_LOCATION_DEF_SX, strlen(OVAL_DEF_SCHEMA_LOCATION_DEF_SX));
        strncat(schema, " ", 1);

        oscap_realloc(schema, sizeof(char)*(strlen(schema)+strlen(OVAL_DEF_SCHEMA_LOCATION_IND_PX)+strlen(version)+strlen(OVAL_DEF_SCHEMA_LOCATION_IND_SX)+1));
        strncat(schema, OVAL_DEF_SCHEMA_LOCATION_IND_PX, strlen(OVAL_DEF_SCHEMA_LOCATION_IND_PX));
        strncat(schema, version, strlen(version));
        strncat(schema, OVAL_DEF_SCHEMA_LOCATION_IND_SX, strlen(OVAL_DEF_SCHEMA_LOCATION_IND_SX));
        strncat(schema, " ", 1);

        oscap_realloc(schema, sizeof(char)*(strlen(schema)+strlen(OVAL_DEF_SCHEMA_LOCATION_UNX_PX)+strlen(version)+strlen(OVAL_DEF_SCHEMA_LOCATION_UNX_SX)+1));
        strncat(schema, OVAL_DEF_SCHEMA_LOCATION_UNX_PX, strlen(OVAL_DEF_SCHEMA_LOCATION_UNX_PX));
        strncat(schema, version, strlen(version));
        strncat(schema, OVAL_DEF_SCHEMA_LOCATION_UNX_SX, strlen(OVAL_DEF_SCHEMA_LOCATION_UNX_SX));
        strncat(schema, " ", 1);

        oscap_realloc(schema, sizeof(char)*(strlen(schema)+strlen(OVAL_DEF_SCHEMA_LOCATION_LNX_PX)+strlen(version)+strlen(OVAL_DEF_SCHEMA_LOCATION_LNX_SX)+1));
        strncat(schema, OVAL_DEF_SCHEMA_LOCATION_LNX_PX, strlen(OVAL_DEF_SCHEMA_LOCATION_LNX_PX));
        strncat(schema, version, strlen(version));
        strncat(schema, OVAL_DEF_SCHEMA_LOCATION_LNX_SX, strlen(OVAL_DEF_SCHEMA_LOCATION_LNX_SX));
        strncat(schema, " ", 1);

        oscap_realloc(schema, sizeof(char)*(strlen(schema)+strlen(OVAL_DEF_SCHEMA_LOCATION_CMN_PX)+strlen(version)+strlen(OVAL_DEF_SCHEMA_LOCATION_CMN_SX)+1));
        strncat(schema, OVAL_DEF_SCHEMA_LOCATION_CMN_PX, strlen(OVAL_DEF_SCHEMA_LOCATION_CMN_PX));
        strncat(schema, version, strlen(version));
        strncat(schema, OVAL_DEF_SCHEMA_LOCATION_CMN_SX, strlen(OVAL_DEF_SCHEMA_LOCATION_CMN_SX));


        return schema;
}*/

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
        newmodel->schema = strdup(OVAL_DEF_SCHEMA_LOCATION);
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

	if (definition_model == NULL) {
                oscap_dprintf("WARNING: argument is not valid: NULL.\n");
		return false;
        }

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
					 struct oval_definition_model *newmodel, 
					 _oval_result_system_clone_func cloner)
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
        newmodel->schema = oscap_strdup(oldmodel->schema);
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

        if (model->schema != NULL)
            oscap_free(model->schema);

	model->definition_map = NULL;
	model->object_map = NULL;
	model->state_map = NULL;
	model->test_map = NULL;
	model->variable_map = NULL;
        model->schema = NULL;

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
        newmodel->schema = oscap_strdup(OVAL_SYS_SCHEMA_LOCATION);

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
	bool is_valid = true;
	struct oval_syschar_iterator *syschars_itr;
	struct oval_sysinfo *sysinfo;
	struct oval_definition_model *definition_model;

	if (syschar_model == NULL) {
                oscap_dprintf("WARNING: argument is not valid: NULL.\n");
		return false;
        }

	/* validate sysinfo */
	sysinfo = oval_syschar_model_get_sysinfo(syschar_model);
	if (oval_sysinfo_is_valid(sysinfo) != true)
		return false;

	/* validate definition_model */
	definition_model = oval_syschar_model_get_definition_model(syschar_model);
	if (oval_definition_model_is_valid(definition_model) != true)
		return false;

	/* validate syschars */
	syschars_itr = oval_syschar_model_get_syschars(syschar_model);
	while (oval_syschar_iterator_has_more(syschars_itr)) {
		struct oval_syschar *syschar;
		syschar = oval_syschar_iterator_next(syschars_itr);
		if (oval_syschar_is_valid(syschar) != true) {
			is_valid = false;
			break;
		}
	}
	oval_syschar_iterator_free(syschars_itr);
	if (is_valid != true)
		return false;

	/* validate variable_bindings */
	// ToDo

	return true;
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
        new_model->schema = oscap_strdup(old_model->schema);

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
        if (model->schema)
                oscap_free(model->schema);

	model->sysinfo = NULL;
	model->definition_model = NULL;
	model->syschar_map = NULL;
	model->sysdata_map = NULL;
	model->variable_binding_map = NULL;
        model->schema = NULL;
	oscap_free(model);
}

struct oval_definition_model *oval_syschar_model_get_definition_model(struct oval_syschar_model *model)
{
	__attribute__nonnull__(model);

	return model->definition_model;
}

const char * oval_definition_model_get_schema(struct oval_definition_model * model)
{
        __attribute__nonnull__(model);

        return model->schema;
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

const char * oval_syschar_model_get_schema(struct oval_syschar_model * model)
{
        __attribute__nonnull__(model);

        return model->schema;
}

void oval_syschar_model_set_sysinfo(struct oval_syschar_model *model, struct oval_sysinfo *sysinfo)
{
	if (model && !oval_syschar_model_is_locked(model)) {
		model->sysinfo = oval_sysinfo_clone(model, sysinfo);
	} else
		oscap_dprintf("WARNING: attempt to update locked content (%s:%d)", __FILE__, __LINE__);
}

void oval_syschar_model_set_schema(struct oval_syschar_model *model, const char * schema)
{
	if (model && !oval_syschar_model_is_locked(model)) {
		model->schema = oscap_strdup(schema);
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

void oval_definition_model_set_schema(struct oval_definition_model *model, const char *version)
{
	if (model && !oval_definition_model_is_locked(model)) {
                model->schema = oscap_strdup(version);
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

struct oval_definition_model * oval_definition_model_import(const char *file)
{
        struct oval_definition_model *model = oval_definition_model_new();
        int ret = oval_definition_model_merge(model,file);
        if (ret == -1) {
                oval_definition_model_free(model);
                model = NULL;
        }

        return model;
}

int oval_definition_model_merge(struct oval_definition_model *model, const char *file)
{
	__attribute__nonnull__(model);

	xmlTextReader *reader = xmlNewTextReaderFilename(file);

	int retcode = 0;
	if (reader) {
		if (xmlTextReaderRead(reader) > -1) {
			retcode = ovaldef_parser_parse(model, reader, NULL);
		}
		xmlFreeTextReader(reader);
	} else {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	return retcode;
}

int oval_syschar_model_import(struct oval_syschar_model *model, const char *file)
{
	__attribute__nonnull__(model);

	int ret;

	xmlDoc *doc = xmlParseFile(file);
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	xmlTextReader *reader = xmlNewTextReaderFilename(file);
	if (reader == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	xmlTextReaderRead(reader);
	ret = ovalsys_parser_parse(model, reader, NULL);

	xmlFreeTextReader(reader);
	xmlFreeDoc(doc);
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

void oval_syschar_model_bind_variable_model(struct oval_syschar_model *sysmodel, struct oval_variable_model *varmodel)
{
	__attribute__nonnull__(sysmodel);

	oval_definition_model_bind_variable_model(sysmodel->definition_model, varmodel);
}

struct oval_syschar *oval_syschar_model_get_syschar(struct oval_syschar_model *model, const char *object_id)
{
	__attribute__nonnull__(model);

	return (struct oval_syschar *)oval_string_map_get_value(model->syschar_map, object_id);
}

struct oval_sysdata *oval_syschar_model_get_sysdata(struct oval_syschar_model *model, const char *id)
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

struct oval_sysdata *oval_sysdata_get_new(struct oval_syschar_model *model, const char *id)
{
	struct oval_sysdata *sysdata = oval_syschar_model_get_sysdata(model, id);
	if (sysdata == NULL) {
		sysdata = oval_sysdata_new(model, id);
		oval_syschar_model_add_sysdata(model, sysdata);
	}
	return sysdata;
}

struct oval_definition *oval_definition_get_new(struct oval_definition_model *model, const char *id)
{
	struct oval_definition *definition = oval_definition_model_get_definition(model, id);
	if (definition == NULL) {
		definition = oval_definition_new(model, id);
		oval_definition_model_add_definition(model, definition);
	}
	return definition;
}

struct oval_variable *oval_variable_get_new(struct oval_definition_model *model, const char *id, oval_variable_type_t type)
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

struct oval_state *oval_state_get_new(struct oval_definition_model *model, const char *id)
{
	struct oval_state *state = oval_definition_model_get_state(model, id);
	if (state == NULL) {
		state = oval_state_new(model, id);
		oval_definition_model_add_state(model, state);
	}
	return state;
}

struct oval_object *oval_object_get_new(struct oval_definition_model *model, const char *id)
{
	struct oval_object *object = oval_definition_model_get_object(model, id);
	if (object == NULL) {
		object = oval_object_new(model, id);
		oval_definition_model_add_object(model, object);
	}
	return object;
}

struct oval_test *oval_test_get_new(struct oval_definition_model *model, const char *id)
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
        model->schema = oscap_strdup(OVAL_RES_SCHEMA_LOCATION);
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
	bool is_valid = true;
	struct oval_result_system_iterator *systems_itr;
	struct oval_definition_model *definition_model;

	if (results_model == NULL) {
                oscap_dprintf("WARNING: argument is not valid: NULL.\n");
		return false;
        }

	/* validate definition_model */
	definition_model = oval_results_model_get_definition_model(results_model);
	if (oval_definition_model_is_valid(definition_model) != true)
		return false;

	/* validate systems */
	systems_itr = oval_results_model_get_systems(results_model);
	while (oval_result_system_iterator_has_more(systems_itr)) {
		struct oval_result_system *rslt_system;

		rslt_system = oval_result_system_iterator_next(systems_itr);
		if (oval_result_system_is_valid(rslt_system) != true) {
			is_valid = false;
			break;
		}
	}
	oval_result_system_iterator_free(systems_itr);
	if (is_valid != true)
		return false;

	return true;
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
        new_resmodel->schema = strdup(old_resmodel->schema);

	return new_resmodel;
}

void oval_results_model_free(struct oval_results_model *model)
{
	__attribute__nonnull__(model);

        if (model->schema)
                oscap_free(model->schema);
	oval_collection_free_items(model->systems, (oscap_destruct_func) oval_result_system_free);
	model->definition_model = NULL;
	model->systems = NULL;
        model->schema = NULL;
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

struct oval_result_directives *oval_results_model_import(struct oval_results_model *model, const char *file)
{
	__attribute__nonnull__(model);

	xmlDoc *doc = xmlParseFile(file);
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	xmlTextReader *reader = xmlNewTextReaderFilename(file);
	if (reader == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}

	if (xmlTextReaderRead(reader) < 1) {
		oscap_setxmlerr(xmlGetLastError());
		return NULL;
	}
	struct oval_result_directives *directives = ovalres_parser_parse(model, reader, NULL);

	xmlFreeTextReader(reader);
	xmlFreeDoc(doc);

	return directives;
}

xmlNode *oval_definitions_to_dom(struct oval_definition_model *definition_model, xmlDocPtr doc, xmlNode * parent)
{

	xmlNodePtr root_node = NULL;

	if (parent) {
		root_node = xmlNewChild(parent, NULL, BAD_CAST "oval_definitions", NULL);
	} else {
		root_node = xmlNewNode(NULL, BAD_CAST "oval_definitions");
		xmlDocSetRootElement(doc, root_node);
	}
	xmlNewProp(root_node, BAD_CAST "xsi:schemaLocation", BAD_CAST definition_model->schema);

	xmlNs *ns_common = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE, BAD_CAST "oval");
	xmlNs *ns_xsi = xmlNewNs(root_node, OVAL_XMLNS_XSI, BAD_CAST "xsi");
	xmlNs *ns_defntns = xmlNewNs(root_node, OVAL_DEFINITIONS_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_xsi);
	xmlSetNs(root_node, ns_defntns);

	/* Report generator */
	if (!parent) {
		xmlNode *tag_generator = xmlNewChild(root_node, ns_defntns, BAD_CAST "generator", NULL);
		_generator_to_dom(doc, tag_generator);
	}

	/* Report definitions */
	struct oval_definition_iterator *definitions = oval_definition_model_get_definitions(definition_model);
	if (oval_definition_iterator_has_more(definitions)) {
		xmlNode *definitions_node = NULL;
		while(oval_definition_iterator_has_more(definitions)) {
			struct oval_definition *definition = oval_definition_iterator_next(definitions);
			if (definitions_node == NULL) {
				definitions_node = xmlNewChild(root_node, ns_defntns, BAD_CAST "definitions", NULL);
			}
			oval_definition_to_dom(definition, doc, definitions_node);
		}
	}
        oval_definition_iterator_free(definitions);

	/* Report tests */
	struct oval_test_iterator *tests = oval_definition_model_get_tests(definition_model);
	if (oval_test_iterator_has_more(tests)) {
		xmlNode *tests_node = xmlNewChild(root_node, ns_defntns, BAD_CAST "tests", NULL);
		while (oval_test_iterator_has_more(tests)) {
			struct oval_test *test = oval_test_iterator_next(tests);
			oval_test_to_dom(test, doc, tests_node);
		}
	}
	oval_test_iterator_free(tests);

	/* Report objects */
	struct oval_object_iterator *objects = oval_definition_model_get_objects(definition_model);
	if (oval_object_iterator_has_more(objects)) {
		xmlNode *objects_node = xmlNewChild(root_node, ns_defntns, BAD_CAST "objects", NULL);
		while(oval_object_iterator_has_more(objects)) {
			struct oval_object *object = oval_object_iterator_next(objects);
			oval_object_to_dom(object, doc, objects_node);
		}
	}
	oval_object_iterator_free(objects);

	/* Report states */
	struct oval_state_iterator *states = oval_definition_model_get_states(definition_model);
	if (oval_state_iterator_has_more(states)) {
		xmlNode *states_node = xmlNewChild(root_node, ns_defntns, BAD_CAST "states", NULL);
		while (oval_state_iterator_has_more(states)) {
			struct oval_state *state = oval_state_iterator_next(states);
			oval_state_to_dom(state, doc, states_node);
		}
	}
	oval_state_iterator_free(states);

	/* Report variables */
	struct oval_variable_iterator *variables = oval_definition_model_get_variables(definition_model);
	if (oval_variable_iterator_has_more(variables)) {
		xmlNode *variables_node = xmlNewChild(root_node, ns_defntns, BAD_CAST "variables", NULL);
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

	int retcode = 0;

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	oval_definitions_to_dom(model, doc, NULL);
	retcode = xmlSaveFormatFileEnc(file, doc, "UTF-8", 1);

	if (retcode < 1)
		oscap_setxmlerr(xmlGetLastError());

	xmlFreeDoc(doc);

	return retcode;
}

xmlNode *oval_syschar_model_to_dom(struct oval_syschar_model * syschar_model, xmlDocPtr doc, xmlNode * parent, 
			           oval_syschar_resolver resolver, void *user_arg)
{

	xmlNodePtr root_node;

	if (parent) {
		root_node = xmlNewChild(parent, NULL, BAD_CAST "oval_system_characteristics", NULL);
	} else {
		root_node = xmlNewNode(NULL, BAD_CAST "oval_system_characteristics");
		xmlDocSetRootElement(doc, root_node);
	}
	xmlNewProp(root_node, BAD_CAST "xsi:schemaLocation", BAD_CAST syschar_model->schema);

	xmlNs *ns_common = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE, BAD_CAST "oval");
	xmlNs *ns_xsi = xmlNewNs(root_node, OVAL_XMLNS_XSI, BAD_CAST "xsi");
	xmlNs *ns_syschar = xmlNewNs(root_node, OVAL_SYSCHAR_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_xsi);
	xmlSetNs(root_node, ns_syschar);

        /* Report generator */
        if (!parent) {
		xmlNode *tag_generator = xmlNewChild(root_node, ns_syschar, BAD_CAST "generator", NULL);
                _generator_to_dom(doc, tag_generator);
        }

        /* Report sysinfo */
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

int oval_syschar_model_export(struct oval_syschar_model *model, const char *file)
{

	__attribute__nonnull__(model);

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
	retcode = xmlSaveFormatFileEnc(file, doc, "UTF-8", 1);

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
	xmlNewProp(root_node, BAD_CAST "xsi:schemaLocation", BAD_CAST results_model->schema);

	xmlNs *ns_common = xmlNewNs(root_node, OVAL_COMMON_NAMESPACE, BAD_CAST "oval");
	xmlNs *ns_xsi = xmlNewNs(root_node, OVAL_XMLNS_XSI, BAD_CAST "xsi");
	xmlNs *ns_results = xmlNewNs(root_node, OVAL_RESULTS_NAMESPACE, NULL);

	xmlSetNs(root_node, ns_common);
	xmlSetNs(root_node, ns_xsi);
	xmlSetNs(root_node, ns_results);

	xmlNode *tag_generator = xmlNewChild(root_node, ns_results, BAD_CAST "generator", NULL);

	/* Report generator & directices */
	_generator_to_dom(doc, tag_generator);
	oval_result_directives_to_dom(directives, doc, root_node);

	/* Report definitions */
	struct oval_definition_model *definition_model = oval_results_model_get_definition_model(results_model);
	oval_definitions_to_dom(definition_model, doc, root_node);

	xmlNode *results_node = xmlNewChild(root_node, ns_results, BAD_CAST "results", NULL);
	struct oval_result_system_iterator *systems = oval_results_model_get_systems(results_model);
	while (oval_result_system_iterator_has_more(systems)) {
		struct oval_result_system *sys = oval_result_system_iterator_next(systems);
		oval_result_system_to_dom(sys, results_model, directives, doc, results_node);
	}
	oval_result_system_iterator_free(systems);

	return root_node;
}

int oval_results_model_export(struct oval_results_model *results_model,  struct oval_result_directives *directives,
                              const char *file)
{
	__attribute__nonnull__(results_model);

	int xmlCode = 0;

	LIBXML_TEST_VERSION;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc == NULL) {
		oscap_setxmlerr(xmlGetLastError());
		return -1;
	}

	oval_results_to_dom(results_model, directives, doc, NULL);
	xmlCode = xmlSaveFormatFileEnc(file, doc, "UTF-8", 1);
	if (xmlCode <= 0) {
		oscap_setxmlerr(xmlGetLastError());
		oscap_dprintf("WARNING: No bytes exported: xmlCode = %d", xmlCode);
	}

	xmlFreeDoc(doc);

	return ((xmlCode >= 1) ? 1 : -1);
}

oval_agent_session_t * oval_agent_new_session(struct oval_definition_model *model) {

	oval_agent_session_t *ag_sess;
	int ret;

	ag_sess = oscap_talloc(oval_agent_session_t);
	ag_sess->def_model = model;
	ag_sess->sys_model = oval_syschar_model_new(model);
	ag_sess->psess     = oval_probe_session_new(ag_sess->sys_model);
	/* probe sysinfo */
	ret = oval_probe_session_query_sysinfo(ag_sess->psess);
	if (ret != 0) {
		oval_probe_session_destroy(ag_sess->psess);
		oval_syschar_model_free(ag_sess->sys_model);
		oscap_free(ag_sess);
		return NULL;
	}
	/* one system only */
	ag_sess->sys_models[0] = ag_sess->sys_model;
	ag_sess->sys_models[1] = NULL;
	ag_sess->res_model = oval_results_model_new(model, ag_sess->sys_models);
	return ag_sess;
}

oval_result_t oval_agent_eval_definition(oval_agent_session_t * ag_sess, const char *id) {
	int ret;
	struct oval_result_system_iterator *rsystem_it;
	struct oval_result_system *rsystem;

	/* probe */
	ret = oval_probe_session_query_definition(ag_sess->psess, id);
	if (ret!=0)
		return OVAL_RESULT_UNKNOWN;

	/* take the first system */
	rsystem_it = oval_results_model_get_systems(ag_sess->res_model);
	rsystem = oval_result_system_iterator_next(rsystem_it);
        oval_result_system_iterator_free(rsystem_it);
	/* eval */
	return oval_result_system_eval_definition(rsystem, id);
}

int oval_agent_reset_session(oval_agent_session_t * ag_sess) {
	int ret;

	/* Clean up syschar model */
        if (ag_sess->sys_model->syschar_map)
                oval_string_map_free(ag_sess->sys_model->syschar_map, (oscap_destruct_func) oval_syschar_free);
        if (ag_sess->sys_model->sysdata_map)
                oval_string_map_free(ag_sess->sys_model->sysdata_map, (oscap_destruct_func) oval_sysdata_free);
        if (ag_sess->sys_model->variable_binding_map)
                oval_string_map_free(ag_sess->sys_model->variable_binding_map, (oscap_destruct_func) oval_variable_binding_free);
	ag_sess->sys_model->syschar_map = oval_string_map_new();
	ag_sess->sys_model->sysdata_map = oval_string_map_new();
	ag_sess->sys_model->variable_binding_map = oval_string_map_new();

	/* Replace result model */
	oval_results_model_free(ag_sess->res_model);
	ag_sess->res_model = oval_results_model_new(ag_sess->def_model, ag_sess->sys_models);

	/* Clean up probe cache */	
	ret = oval_probe_session_reset(ag_sess->psess, ag_sess->sys_model);
	return ret;
}


int oval_agent_eval_system(oval_agent_session_t * ag_sess, oval_agent_result_cb_t * cb, void *arg) {
	struct oval_definition *oval_def;
	struct oval_definition_iterator *oval_def_it;
	char   *id;
	int ret;
	oval_result_t result;

	oval_def_it = oval_definition_model_get_definitions(ag_sess->def_model);
	while (oval_definition_iterator_has_more(oval_def_it)) {
		oval_def = oval_definition_iterator_next(oval_def_it);
		id = oval_definition_get_id(oval_def);
		/* probe and eval */
		result = oval_agent_eval_definition(ag_sess, id);
		/* callback */
		ret = (*cb) (id, result, arg);
		if ( ret!=0 )
			return 0;
	}
	oval_definition_iterator_free(oval_def_it);
	return 0;
}

struct oval_results_model * oval_agent_get_results_model(oval_agent_session_t * ag_sess) {
	return ag_sess->res_model;
}

void oval_agent_destroy_session(oval_agent_session_t * ag_sess) {
	oval_probe_session_destroy(ag_sess->psess);
	oval_syschar_model_free(ag_sess->sys_model);
	oval_results_model_free(ag_sess->res_model);
	oscap_free(ag_sess);
	ag_sess=NULL;
}


#ifdef ENABLE_XCCDF
/**
 * Function for OVAL Result type -> XCCDF result type transformation
 * @param id OVAL_RESULT_* type
 * @return xccdf_test_result_type_t
 */
static xccdf_test_result_type_t xccdf_get_result_from_oval(oval_result_t id)
{

	const struct oval_result_to_xccdf_spec *mapptr;

	for (mapptr = XCCDF_OVAL_RESULTS_MAP; mapptr->oval != 0; ++mapptr) {
		if (id == mapptr->oval)
			return mapptr->xccdf;
	}

	return XCCDF_RESULT_UNKNOWN;
}

void oval_agent_resolve_variables(struct oval_agent_session * session, struct xccdf_value_binding_iterator *it)
{

    bool conflict = false;
    struct oval_value_iterator * value_it;

    /* Get the definition model from OVAL agent session */
    struct oval_definition_model *def_model =
        oval_results_model_get_definition_model(oval_agent_get_results_model(session));

    /* Check the conflict */
    while (xccdf_value_binding_iterator_has_more(it)) {
        struct xccdf_value_binding *binding = xccdf_value_binding_iterator_next(it);
        struct oval_variable *variable = oval_definition_model_get_variable(def_model, xccdf_value_binding_get_name(binding));
        /* Do we have comflict ? */
        if (variable != NULL) {
            value_it = oval_variable_get_values(variable);
            if (oval_value_iterator_has_more(value_it))
                conflict = true;
            oval_value_iterator_free(value_it);
        }
    }

    xccdf_value_binding_iterator_reset(it);
    if (conflict) {
        /* We have a conflict, clear session and external variables */
        oval_definition_model_clear_external_variables(def_model);
        oval_agent_reset_session(session);
    }

    struct oval_variable_model *var_model = oval_variable_model_new();
    /* Iterate through variable bindings and add variables into the variable model */
    while (xccdf_value_binding_iterator_has_more(it)) {

        struct xccdf_value_binding *binding = xccdf_value_binding_iterator_next(it);
        char *name = xccdf_value_binding_get_name(binding);
        char *value = xccdf_value_binding_get_value(binding);
        struct oval_variable *variable = oval_definition_model_get_variable(def_model, name);
        if (variable != NULL) {
                oval_datatype_t o_type = oval_variable_get_datatype(variable);
                /* TODO: check of variable type ? */
                if (oval_variable_model_has_variable(var_model, name))
                    oscap_dprintf("ERROR! External variable %s in conflict ! Probably content failure.\n", name);
                /* Add variable to variable model */
                else oval_variable_model_add(var_model, name, "Unknown", o_type, value); // TODO comment
        } else {
                oscap_dprintf("Variable %s does not exist, skipping\n", name);
        }
    }
    /* Finalize - bind variable model to definition model */
    oval_definition_model_bind_variable_model(def_model, var_model);
    oval_variable_model_free(var_model);
}

xccdf_test_result_type_t oval_agent_eval_rule(struct xccdf_policy *policy, const char *rule_id, const char *id,
			       struct xccdf_value_binding_iterator *it, void *usr)
{
        oval_result_t result;
	struct oval_agent_cb_data * data = (struct oval_agent_cb_data *) usr;

        /* If there is no such OVAL definition, return XCCDF_RESUL_NOT_CHECKED. XDCCDF should look for alternative definition in this case. */
        if (oval_definition_model_get_definition(oval_results_model_get_definition_model(oval_agent_get_results_model(data->session)), id) == NULL)
                return XCCDF_RESULT_NOT_CHECKED;

        /* Resolve variables */
        oval_agent_resolve_variables(data->session, it);
        /* Evaluate OVAL definition */
	result = oval_agent_eval_definition(data->session, id);

        /* Call user callback */
        if (data->callback != NULL)
                (*data->callback) (rule_id, xccdf_get_result_from_oval(result), (void *) data->usr);

	return xccdf_get_result_from_oval(result);
}

void oval_agent_export_sysinfo_to_xccdf_result(struct oval_agent_session * sess, struct xccdf_result * ritem)
{
	struct oval_syschar_model *sys_model = NULL;
	struct oval_definition_model *def_model = NULL;
	struct oval_results_model *res_model = NULL;
	struct oval_result_system *re_system = NULL;
	struct oval_result_system_iterator *re_system_it = NULL;
	struct oval_sysinfo *sysinfo = NULL;

        /* Get all models we will need */
	res_model = oval_agent_get_results_model(sess);
	def_model = oval_results_model_get_definition_model(res_model);
        /* Get the very first system */
        re_system_it = oval_results_model_get_systems(res_model);
	re_system = oval_result_system_iterator_next(re_system_it);
        oval_result_system_iterator_free(re_system_it);

	sys_model = oval_result_system_get_syschar_model(re_system);
	sysinfo = oval_syschar_model_get_sysinfo(sys_model);

        /* Set the test system */
	xccdf_result_set_test_system(ritem, oval_sysinfo_get_primary_host_name(sysinfo));

	struct xccdf_target_fact *fact = NULL;
	struct oval_sysint *sysint = NULL;

        /* Add info for all interfaces */
	struct oval_sysint_iterator *sysint_it = oval_sysinfo_get_interfaces(sysinfo);
	while (oval_sysint_iterator_has_more(sysint_it)) {
		sysint = oval_sysint_iterator_next(sysint_it);
		xccdf_result_add_target_address(ritem, oval_sysint_get_ip_address(sysint));

		if (oval_sysint_get_mac_address(sysint) != NULL) {
			fact = xccdf_target_fact_new();
			xccdf_target_fact_set_name(fact, "urn:xccdf:fact:ethernet:MAC");
			xccdf_target_fact_set_string(fact, oval_sysint_get_mac_address(sysint));
			xccdf_result_add_target_fact(ritem, fact);
		}
	}
	oval_sysint_iterator_free(sysint_it);
}

#endif
