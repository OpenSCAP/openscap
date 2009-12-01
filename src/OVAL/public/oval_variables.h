/*
 * oval_variables.h
 *
 *  Created on: Nov 13, 2009
 *      Author: david.niemoller
 */

#ifndef OVAL_VARIABLES_H_
#define OVAL_VARIABLES_H_

#include "oval_types.h"

/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALVAR OVAL External Variable Binding API
 * @{
 */

/**
 * Create a new OVAL variable model
 */
struct oval_variable_model *oval_variable_model_new(void);

/**
 * Create an OVAL variable model
 */
struct oval_variable_model *oval_variable_model_clone(struct oval_variable_model *);


/**
 * Free memory allocated to a specified oval_variable_model
 * @param variable_model the specified oval_variable_model
 */
void oval_variable_model_free(struct oval_variable_model *);

/**
 * Load the specified oval_variable_model from an XML stream.
 * The stream document element must be a valid instance of <http://oval.mitre.org/XMLSchema/oval-variables-5:oval_variables>.
 * If the oval_variable model is not empty, the loaded content will be appended to the existing content.
 * @param variable_model the specified oval_variable_model.
 * @param import_source the oval_import_source that resolves the XML stream.
 * @param error_handler the oval_xml_error_handler that tracks the parsing of the XML stream (may be NULL)
 * @param user_param a user parameter that is passed the the error handler implementation.
 */
void oval_variable_model_import
	(struct oval_variable_model *,
	 struct oval_import_source *,
	 oval_xml_error_handler, void*);

/**
 * Export the specified oval_variable_model to an XML stream.
 * The exported document element is a valid instance of <http://oval.mitre.org/XMLSchema/oval-variables-5:oval_variables>.
 * @param variable_model the specified oval_variable_model.
 * @param export_target the oval_export_target that resolves the output XML stream.
 */
void oval_variable_model_export
	(struct oval_variable_model *,
	 struct oval_export_target *);

/**
 * Get all external variables managed by a specified oval_variable_model.
 * @param variable_model the specified oval_variable_model.
 */
struct oval_string_iterator *oval_variable_model_get_variable_ids
	(struct oval_variable_model *);

/**
 * Get a specified external variable datatype.
 * If the varid does not resolve to a managed external variable, this method returns 0.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 */
oval_datatype_t oval_variable_model_get_datatype
	(struct oval_variable_model *, char *);

/**
 * Get a specified external variable comment.
 * If the varid does not resolve to a managed external variable, this method returns NULL.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 */
const char *oval_variable_model_get_comment
	(struct oval_variable_model *, char *);

/**
 * Get the values bound to a specified external variable.
 * If the varid does not resolve to a managed external variable, this method returns NULL.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 */
struct oval_string_iterator *oval_variable_model_get_values
	(struct oval_variable_model *, char *);


void oval_variable_model_add(struct oval_variable_model *model, char *varid, const char* comment, oval_datatype_t datatype, char *value);

/**
 * @}END OVALVAR
 * @}END OVAL
 */
#endif /* OVAL_VARIABLES_H_ */
