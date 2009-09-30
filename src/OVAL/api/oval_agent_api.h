/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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

/**
 * @addtogroup OVAL
 * @{
 *
 * OVAL model is represented by several basic structures in OpenSCAP.
 *
 * - oval_definition_model structure represents an individual OVAL @ref Definitions document
 * - oval_syschar_model structure represents an OVAL @ref syschar document
 * - oval_results_model structure represents an OVAL @ref Results document
 *
 * See more details at http://oval.mitre.org/
 *
 * @file
 * OVAL agent API.
 * @author "David Niemoller" <David.Niemoller@g2-inc.com>
 */

#ifndef OVAL_AGENT_API_H_
#define OVAL_AGENT_API_H_

#include "oval_definitions.h"
#include "oval_system_characteristics.h"
#include "oval_results.h"

struct oval_export_target;
struct oval_import_source;

/**
 * Create an import source from filename.
 * Return an import_source that streams from a designated file.
 * @param filename the name of the designated file
 */
struct oval_import_source *oval_import_source_new_file(char *filename);

/** free a specified import_source.
 * @param source the specified import_source
 */
void oval_import_source_free(
		struct oval_import_source *source);

/** create export_target object.
 * Return the created export_target object.
 * @param filename the name of the target output file
 * @param encoding the target XML encoding.
 */
struct oval_export_target *oval_export_target_new_file(char *filename, char* encoding);

/** free an export_target object.
 * @param target the target to be freed.
 */
void oval_export_target_free(struct oval_export_target *target);


/**
 * @struct oval_definition_model
 * OVAL object model.
 * Object model holds OVAL definitions as a list of oval_definition
 * structure instances.
 */
struct oval_definition_model;

/**
 * @struct oval_syschar_model
 * OVAL object model.
 * Object model holds OVAL system characteristics as a list of oval_syschar
 * structure instances.
 */
struct oval_syschar_model;

/**
 * @struct oval_syschar_model
 * OVAL object model.
 * Object model holds OVAL results as a list of oval_system
 * structure instances.
 */
struct oval_results_model;

/**
 * Create an empty oval_definition_model.
 */
struct oval_definition_model *oval_definition_model_new();

/***
 * Free OVAL object model.
 */
void oval_definition_model_free(struct oval_definition_model * model);

/// severity level
typedef enum {
	XML_VALIDITY_WARNING = 1,
	XML_VALIDITY_ERROR   = 2,
	XML_WARNING          = 3,
	XML_ERROR            = 4,
	OVAL_LOG_INFO        = 15,
	OVAL_LOG_DEBUG       = 16,
	OVAL_LOG_WARN        = 17
} oval_xml_severity_t;

struct oval_xml_error {
	void *user_arg;
	char *message;
	oval_xml_severity_t severity;
	int line_number;
	char *system_id;
};

/// OVAL XML error handler function pointer type.
typedef int (*oval_xml_error_handler) (struct oval_xml_error *, void *user_arg);

/**
 * Load OVAL definitions from a XML stream.
 * Merge content from a specified XML stream into a specified oval_definition_model.
 * If the input_source specifies a model entity (id=A, version=V) and the model specifies no entity (id=A, ...) then (id=A, version=V) is merged into the model.
 * If the input_source specifies a model entity (id=A, version=V) and the model specifies an entity (id=A, ...) then (id=A, version=V) then the source specification is skipped.
 * @relates oval_definition_model
 * @param model the merge target model.
 * @param source an <oval_def:oval_definitions> XML stream
 * @param error_handler callback for detected error conditions (may be NULL)
 * @param client pointer to client data (may be NULL)
 */
void oval_definition_model_load(
			struct oval_definition_model *model,
			struct oval_import_source *source,
			oval_xml_error_handler error_handler, void *client);



/**
 * Get OVAL definition by ID.
 * Return a designated oval_definition from the specified oval_definition_model.
 * If the specified id does not resolve to an oval_definition the function returns NULL.
 * @relates oval_definition_model
 * @param model the queried model.
 * @param id the definition id.
 */
struct oval_definition *oval_definition_model_get_definition(
		struct oval_definition_model *model,
		char *id);

/**
 * Get oval test by ID.
 * Return a designated oval_test from the specified oval_definition_model.
 * If the specified id does not resolve to an oval_test the function returns NULL.
 * @relates oval_definition_model
 * @param model the queried model.
 * @param id the test id.
 */
struct oval_test *oval_definition_model_get_test(
		struct oval_definition_model *model,
		char *id);

/**
 * Get OVAL object by ID.
 * Return a designated oval_object from the specified oval_definition_model.
 * If the specified id does not resolve to an oval_object the function returns NULL.
 * @relates oval_definition_model
 * @param model the queried model.
 * @param id the object id.
 */
struct oval_object *oval_definition_model_get_object(
		struct oval_definition_model *model,
		char *id);

/**
 * Get OVAL state by ID.
 * Return a designated oval_state from the specified oval_definition_model.
 * If the specified id does not resolve to an oval_state the function returns NULL.
 * @relates oval_definition_model
 * @param model the queried model.
 * @param id the state id.
 */
struct oval_state *oval_definition_model_get_state(
		struct oval_definition_model *model,
		char *id);

/**
 * Get OVAL variable by ID.
 * Return a designated oval_variable from the specified oval_definition_model.
 * If the specified id does not resolve to an oval_variable the function returns NULL.
 * @relates oval_definition_model
 * @param model the queried model.
 * @param id the variable id.
 */
struct oval_variable *oval_definition_model_get_variable(
		struct oval_definition_model *model,
		char *id);

/**
 * Get OVAL definitions.
 * Return all oval_definitions from the specified oval_definition_model.
 * @relates oval_definition_model
 * @param model the queried model.
 */
struct oval_definition_iterator *oval_definition_model_get_definitions(
		struct oval_definition_model *model);

/**
 * Get OVAL tests.
 * Return all oval_tests from the specified oval_definition_model.
 * @relates oval_definition_model
 * @param model the queried model.
 */
struct oval_test_iterator *oval_definition_model_get_tests(
		struct oval_definition_model *model);

/**
 * Get OVAL objects.
 * Return all oval_objects from the specified oval_definition_model.
 * @relates oval_definition_model
 * @param model the queried model.
 */
struct oval_object_iterator *oval_definition_model_get_objects(
		struct oval_definition_model *model);

/**
 * Get OVAL states.
 * Return all oval_states from the specified oval_definition_model.
 * @relates oval_definition_model
 * @param model the queried model.
 */
struct oval_state_iterator *oval_definition_model_get_states(
		struct oval_definition_model *model);

/**
 * Get OVAL variables.
 * Return all oval_variables from the specified oval_definition_model.
 * @relates oval_definition_model
 * @param model the queried model.
 */
struct oval_variable_iterator *oval_definition_model_get_variables(
		struct oval_definition_model *model);

int oval_definition_model_export(
		struct oval_definition_model *, struct oval_export_target *);


void oval_definition_model_add_definition(struct oval_definition_model *, struct oval_definition *);
void oval_definition_model_add_test(struct oval_definition_model *, struct oval_test *);
void oval_definition_model_add_object(struct oval_definition_model *, struct oval_object *);
void oval_definition_model_add_state(struct oval_definition_model *, struct oval_state *);
void oval_definition_model_add_variable(struct oval_definition_model *, struct oval_variable *);

/**
 * Create new oval_syschar_model.
 * The new model is bound to a specified oval_definition_model and variable bindings.
 * @relates oval_definition_model
 * @param definition_model the specified oval_definition_model.
 * @param bindings the specified oval_variable_bindings.
 */
struct oval_syschar_model *oval_syschar_model_new(
		struct oval_definition_model *definition_model,
		struct oval_variable_binding_iterator *bindings);



/**
 * free memory allocated to a specified syschar model.
 * @relates oval_syschar_model
 * @param model the specified syschar model
 */
void oval_syschar_model_free(struct oval_syschar_model *model);

/**
 * Return related oval_definition_model from an oval_syschar_model.
 * @relates oval_syschar_model
 * @param model the specified oval_syschar_model.
 */
struct oval_definition_model *oval_syschar_model_get_definition_model(
		struct oval_syschar_model *model);
/**
 * Return an iterator over the oval_sychar objects persisted by this model.
 * @relates oval_syschar_model
 * @param model the specified oval_syschar_model.
 */
struct oval_syschar_iterator *oval_syschar_model_get_syschars(
		struct oval_syschar_model *model);

/**
 * Return default sysinfo bound to syschar model.
 * @relates oval_syschar_model
 * @param model the specified oval_syschar_model.
 */
struct oval_sysinfo *oval_syschar_model_get_sysinfo(
		struct oval_syschar_model *model);

/**
 * Return the oval_syschar bound to a specified object_id.
 * Returns NULL if the object_id does not resolve to an oval_object in the bound oval_definition_model.
 * @relates oval_syschar_model
 * @param model the queried oval_syschar_model.
 * @param object_id the specified object_id.
 */

struct oval_syschar *oval_syschar_model_get_syschar(
		struct oval_syschar_model *model,
		char *object_id);

bool oval_syschar_model_add_variable_binding(struct oval_syschar_model *, struct oval_variable_binding *);

/**
 * Export system characteristics as a XML file.
 * @relates oval_syschar_model
 */
int oval_syschar_model_export(
		struct oval_syschar_model *, struct oval_export_target *);

/**
 * Load OVAL system characteristics from a file.
 * @relates oval_syschar_model
 */
void oval_syschar_model_load(struct oval_syschar_model*, struct oval_import_source*,
			oval_xml_error_handler, void*);

/**
 * Create new oval_results_model.
 * The new model is bound to a specified oval_definition_model and variable bindings.
 * @relates oval_results_model
 * @param syschar_model the specified oval_syschar_model.
 */
struct oval_results_model *oval_results_model_new(
		struct oval_definition_model *definition_model, struct oval_syschar_model **);

/**
 * free memory allocated to a specified oval results model.
 * @relates oval_results_model
 * @param the specified oval_results model
 */
void oval_results_model_free(struct oval_results_model *model);


/**
 * oval_results_model_definition_model Return bound object model from an oval_results_model.
 * @relates oval_results_model
 * @param model the specified oval_results_model.
 */
struct oval_definition_model *oval_results_model_get_definition_model
	(struct oval_results_model *model);

/**
 * Return iterator over reporting systems.
 * @relates oval_results_model
 * @param model the specified results model
 */
struct oval_result_system_iterator *oval_results_model_get_systems
		(struct oval_results_model *);

void oval_results_model_add_system(struct oval_results_model *, struct oval_result_system *);

/**
 * load oval results from XML file.
 * @relates oval_results_model
 * @param model the oval_results_model
 * @param source the input source (XML)
 * @param error_handler the error handler
 * @param client_data client data;
 */
struct oval_result_directives *oval_results_model_load
	(struct oval_results_model *, struct oval_import_source *,
			oval_xml_error_handler, void*);

/**
 * Create new OVAL results directives.
 * @relates oval_result_directives
 */
struct oval_result_directives *oval_result_directives_new();

/**
 * Destroy OVAL results directives.
 * @relates oval_result_directives
 */
void oval_result_directives_free(struct oval_result_directives *);

/**
 * export oval results to XML file.
 * @param model the oval_results_model
 * @param target the export target stream (XML)
 */
int oval_results_model_export(struct oval_results_model *, struct oval_result_directives *, struct oval_export_target *);


/**
 * Probe single OVAL object.
 * @relates oval_object
 */
struct oval_syschar *oval_object_probe(struct oval_object *, struct oval_definition_model *model);

#endif				/**OVAL_AGENT_API_H_ */
