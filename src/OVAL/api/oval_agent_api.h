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
 * - oval_object_model structure represents an individual OVAL @ref Definitions document
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

struct export_target;
struct import_source;

/**
 * Create an import source from filename.
 * Return an import_source that streams from a designated file.
 * @param filename the name of the designated file
 */
struct import_source *import_source_file(char *filename);
/** free a specified import_source.
 * @param source the specified import_source
 */
void import_source_free(
		struct import_source *source);

/** create export_target object.
 * Return the created export_target object.
 * @param filename the name of the target output file
 * @param encoding the target XML encoding.
 */
struct export_target *export_target_new(char *filename, char* encoding);

/** free an export_target object.
 * @param target the target to be freed.
 */
void export_target_free(struct export_target *target);

/**
 * @struct oval_object_model
 * OVAL object model.
 * Object model holds OVAL definitions as a list of oval_definition
 * structure instances.
 */
struct oval_object_model;

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
 * Create an empty oval_object_model.
 */
struct oval_object_model *oval_object_model_new();

/***
 * Free OVAL object model.
 */
void oval_object_model_free(struct oval_object_model * model);

/// severity level
typedef enum {
	XML_VALIDITY_WARNING = 1,
	XML_VALIDITY_ERROR   = 2,
	XML_WARNING          = 3,
	XML_ERROR            = 4,
	OVAL_LOG_INFO        = 15,
	OVAL_LOG_DEBUG       = 16,
	OVAL_LOG_WARN        = 17
} oval_xml_severity_enum;

struct oval_xml_error {
	void *user_arg;
	char *message;
	oval_xml_severity_enum severity;
	int line_number;
	char *system_id;
};

/// OVAL XML error handler function pointer type.
typedef int (*oval_xml_error_handler) (struct oval_xml_error *, void *user_arg);

/**
 * Load OVAL definitions from a XML stream.
 * Merge content from a specified XML stream into a specified oval_object_model.
 * If the input_source specifies a model entity (id=A, version=V) and the model specifies no entity (id=A, ...) then (id=A, version=V) is merged into the model.
 * If the input_source specifies a model entity (id=A, version=V) and the model specifies an entity (id=A, ...) then (id=A, version=V) then the source specification is skipped.
 * @relates oval_object_model
 * @param model the merge target model.
 * @param source an <oval_def:oval_definitions> XML stream
 * @param error_handler callback for detected error conditions (may be NULL)
 * @param client pointer to client data (may be NULL)
 */
void load_oval_definitions(
			struct oval_object_model *model,
			struct import_source *source,
			oval_xml_error_handler error_handler, void *client);

/**
 * Update OVAL definitions.
 * A new oval_object_model is created and returned by the update_oval_definitions.
 * If the oldModel contains a model entity (id=A, version=V) and the imput_source specifies no entity (id=A, ...) then (id=A, version=V) is contained by the returned model.
 * If the oldModel contains a model entity (id=A, version=V1) then the new model contains a content model entity (id=A, version=V2) then the new model contains (id=A, version=V2) if and only if V1<V2, otherwise the returned model contains the entity (id=A, version=V1).
 * If the input_source specifies a model entity (id=A, version=V) and the oldModel specifies no entity (id=A, ...) then (id=A, version=V) is contained by the returned model.
 * The replacement of (id=A, version=V1) with (id=A, version=V2) is driven by the existence of a specification of the latter entity in the import_source offered to update_oval_definitions.  As a matter of convenience, we can specify that
 *
 *  	struct oval_object_model *model = update_oval_definitions(NULL, source, ...);
 *  is equivalient to
 *  	struct oval_object_model *model = oval_object_model_new;
 *  	load_oval_definitions(model, source, ...);
 * @relates oval_object_model
 * @param old_model the base model (not changed by this operation; may be NULL)
 * @param source an <oval_def:oval_definitions> XML stream
 * @param error_handler callback for detected error conditions (may be NULL)
 * @param client pointer to client data (may be NULL)
 */
struct oval_object_model *update_oval_definitions(
		struct oval_object_model *oldModel,
		struct import_source *source,
		oval_xml_error_handler error_handler, void *client);


/**
 * Get OVAL definition by ID.
 * Return a designated oval_definition from the specified oval_object_model.
 * If the specified id does not resolve to an oval_definition the function returns NULL.
 * @relates oval_object_model
 * @param model the queried model.
 * @param id the definition id.
 */
struct oval_definition *get_oval_definition(
		struct oval_object_model *model,
		char *id);

/**
 * Get oval test by ID.
 * Return a designated oval_test from the specified oval_object_model.
 * If the specified id does not resolve to an oval_test the function returns NULL.
 * @relates oval_object_model
 * @param model the queried model.
 * @param id the test id.
 */
struct oval_test *get_oval_test(
		struct oval_object_model *model,
		char *id);

/**
 * Get OVAL object by ID.
 * Return a designated oval_object from the specified oval_object_model.
 * If the specified id does not resolve to an oval_object the function returns NULL.
 * @relates oval_object_model
 * @param model the queried model.
 * @param id the object id.
 */
struct oval_object *get_oval_object(
		struct oval_object_model *model,
		char *id);

/**
 * Get OVAL state by ID.
 * Return a designated oval_state from the specified oval_object_model.
 * If the specified id does not resolve to an oval_state the function returns NULL.
 * @relates oval_object_model
 * @param model the queried model.
 * @param id the state id.
 */
struct oval_state *get_oval_state(
		struct oval_object_model *model,
		char *id);

/**
 * Get OVAL variable by ID.
 * Return a designated oval_variable from the specified oval_object_model.
 * If the specified id does not resolve to an oval_variable the function returns NULL.
 * @relates oval_object_model
 * @param model the queried model.
 * @param id the variable id.
 */
struct oval_variable *get_oval_variable(
		struct oval_object_model *model,
		char *id);

/**
 * Get OVAL definitions.
 * Return all oval_definitions from the specified oval_object_model.
 * @relates oval_object_model
 * @param model the queried model.
 */
struct oval_iterator_definition *get_oval_definitions(
		struct oval_object_model *model);

/**
 * Get OVAL tests.
 * Return all oval_tests from the specified oval_object_model.
 * @relates oval_object_model
 * @param model the queried model.
 */
struct oval_iterator_test *get_oval_tests(
		struct oval_object_model *model);

/**
 * Get OVAL objects.
 * Return all oval_objects from the specified oval_object_model.
 * @relates oval_object_model
 * @param model the queried model.
 */
struct oval_iterator_object *get_oval_objects(
		struct oval_object_model *model);

/**
 * Get OVAL states.
 * Return all oval_states from the specified oval_object_model.
 * @relates oval_object_model
 * @param model the queried model.
 */
struct oval_iterator_state *get_oval_states(
		struct oval_object_model *model);

/**
 * Get OVAL variables.
 * Return all oval_variables from the specified oval_object_model.
 * @relates oval_object_model
 * @param model the queried model.
 */
struct oval_iterator_variable *get_oval_variables(
		struct oval_object_model *model);

int export_definitions(
		struct oval_object_model *, struct export_target *);

/**
 * Create new oval_syschar_model.
 * The new model is bound to a specified oval_object_model and variable bindings.
 * @relates oval_object_model
 * @param object_model the specified oval_object_model.
 * @param bindings the specified oval_variable_bindings.
 */
struct oval_syschar_model *oval_syschar_model_new(
		struct oval_object_model *object_model,
		struct oval_iterator_variable_binding *bindings);



/**
 * free memory allocated to a specified syschar model.
 * @relates oval_syschar_model
 * @param model the specified syschar model
 */
void oval_syschar_model_free(struct oval_syschar_model *model);

/**
 * Return related oval_object_model from an oval_syschar_model.
 * @relates oval_syschar_model
 * @param model the specified oval_syschar_model.
 */
struct oval_object_model *oval_syschar_model_object_model(
		struct oval_syschar_model *model);
/**
 * Return an iterator over the oval_sychar objects persisted by this model.
 * @relates oval_syschar_model
 * @param model the specified oval_syschar_model.
 */
struct oval_iterator_syschar *oval_syschar_model_syschars(
		struct oval_syschar_model *model);

/**
 * Return default sysinfo bound to syschar model.
 * @relates oval_syschar_model
 * @param model the specified oval_syschar_model.
 */
struct oval_sysinfo *oval_syschar_model_sysinfo(
		struct oval_syschar_model *model);

/**
 * Return the oval_syschar bound to a specified object_id.
 * Returns NULL if the object_id does not resolve to an oval_object in the bound oval_object_model.
 * @relates oval_syschar_model
 * @param model the queried oval_syschar_model.
 * @param object_id the specified object_id.
 */

struct oval_syschar *get_oval_syschar(
		struct oval_syschar_model *model,
		char *object_id);

/**
 * return the system characteristics of all.
 * @relates oval_syschar_model
 */
struct oval_iterator_syschar *oval_syschar_dependencies(struct oval_syschar *);

/**
 * Export system characteristics as a XML file.
 * @relates oval_syschar_model
 */
int export_characteristics(
		struct oval_syschar_model *, struct export_target *);

/**
 * Load OVAL system characteristics from a file.
 * @relates oval_syschar_model
 */
void load_oval_syschar(struct oval_syschar_model*, struct import_source*,
			oval_xml_error_handler, void*);

/**
 * Create new oval_results_model.
 * The new model is bound to a specified oval_object_model and variable bindings.
 * @relates oval_results_model
 * @param syschar_model the specified oval_syschar_model.
 */
struct oval_results_model *oval_results_model_new(
		struct oval_object_model *object_model, struct oval_syschar_model **);

/**
 * free memory allocated to a specified oval results model.
 * @relates oval_results_model
 * @param the specified oval_results model
 */
void oval_results_model_free(struct oval_results_model *model);


/**
 * oval_results_model_object_model Return bound object model from an oval_results_model.
 * @relates oval_results_model
 * @param model the specified oval_results_model.
 */
struct oval_object_model *oval_results_model_object_model
	(struct oval_results_model *model);

/**
 * Return iterator over reporting systems.
 * @relates oval_results_model
 * @param model the specified results model
 */
struct oval_iterator_result_system *oval_results_model_systems
		(struct oval_results_model *);
/**
 * Return an iterator over the oval_sychar objects persisted by this model.
 * @relates oval_results_model
 * @param model the specified oval_results_model.
 */
struct oval_iterator_results *oval_results_model_results(
		struct oval_results_model *model);

/**
 * load oval results from XML file.
 * @relates oval_results_model
 * @param model the oval_results_model
 * @param source the input source (XML)
 * @param error_handler the error handler
 * @param client_data client data;
 */
struct oval_result_directives *load_oval_results
	(struct oval_results_model *, struct import_source *,
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
int export_results(struct oval_results_model *, struct oval_result_directives *, struct export_target *);

/*
 * Return default sysinfo bound to results model.
 * @param model the specified oval_results_model.
struct oval_results_directives *oval_results_model_directives(
		struct oval_results_model *model);
 */


/**
 * Probe single OVAL object.
 * @relates oval_object
 */
struct oval_syschar *probe_object(struct oval_object *, struct oval_object_model *model);

/**
 * Resolve single OVAL test.
 * @relates oval_test
 */
struct oval_result_test *resolve_test(struct oval_test *,
				      struct oval_iterator_syschar *,
				      struct oval_iterator_variable_binding *);

/**
 * Resolve single OVAL definition.
 * @relates oval_definition
 */
struct oval_result *resolve_definition(struct oval_definition *,
				       struct oval_iterator_syschar *,
				       struct oval_iterator_variable_binding *);

#endif				/**OVAL_AGENT_API_H_ */
