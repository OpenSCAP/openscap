/*
 * oval_variables.h
 *
 *  Created on: Nov 13, 2009
 *      Author: david.niemoller
 */

#ifndef OVAL_VARIABLES_H_
#define OVAL_VARIABLES_H_

#include "oval_types.h"
#include "oval_definitions.h"

/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALVAR OVAL External Variable Binding API
 * @{
 */
/**
* @addtogroup OVALVAR_setters Setters
* @{
* @ref OVALVAR set methods.
*	These methods will not change the state of a locked instance.
*	@see oval_syschar_model_get_locked
*	@see oval_syschar_model_set_locked
* @}
* @addtogroup OVALVAR_getters Getters
* @{
* @ref OVALVAR get methods.
* @}
* @addtogroup OVALVAR_service Service
* @{
* @ref OVALVAR import/export methods.
* @}
* @addtogroup OVALVAR_eval    Evaluators
* @{
* @ref OVALVAR evaluation methods.
* @}
*/

/**
 * Create a new OVAL variable model
 * @ingroup OVALVAR
 */
struct oval_variable_model *oval_variable_model_new(void);
/**
 * return <b>true</b> if the variable_model instance is valid
 * @ingroup OVALVAR_eval
 */
bool oval_variable_model_is_valid(struct oval_variable_model *variable_model);
/**
 * return <b>true</b> if the variable_model instance is locked.
 * The state of a locked instance cannot be changed.
 * @ingroup OVALVAR_getters
 */
bool oval_variable_model_is_locked(struct oval_variable_model *variable_model);
/**
 * Lock the variable_model instance.
 * The state of a locked instance cannot be changed.
 * This operation has no effect if the model is already locked.
 * @ingroup OVALVAR_setters
 */
void oval_variable_model_lock(struct oval_variable_model *variable_model);

/**
 * Create an OVAL variable model
 * @ingroup OVALVAR
 */
struct oval_variable_model *oval_variable_model_clone(struct oval_variable_model *);


/**
 * Free memory allocated to a specified oval_variable_model
 * @param variable_model the specified oval_variable_model
 * @ingroup OVALVAR
 */
void oval_variable_model_free(struct oval_variable_model *);

/**
 * Load the specified oval_variable_model from an XML stream.
 * The stream document element must be a valid instance of <http://oval.mitre.org/XMLSchema/oval-variables-5:oval_variables>.
 * If the oval_variable model is not empty, the loaded content will be appended to the existing content.
 * @param variable_model the specified oval_variable_model.
 * @param import_source the oscap_import_source that resolves the XML stream.
 * @param user_param a user parameter that is passed the the error handler implementation.
 * @ingroup OVALVAR_service
 */
int oval_variable_model_import
	(struct oval_variable_model *,
	 struct oscap_import_source *,
         void*);

/**
 * Export the specified oval_variable_model to an XML stream.
 * The exported document element is a valid instance of <http://oval.mitre.org/XMLSchema/oval-variables-5:oval_variables>.
 * @param variable_model the specified oval_variable_model.
 * @param export_target the oscap_export_target that resolves the output XML stream.
 * @ingroup OVALVAR_service
 */
int oval_variable_model_export
	(struct oval_variable_model *,
	 struct oscap_export_target *);

/**
 * Get all external variables managed by a specified oval_variable_model.
 * @param variable_model the specified oval_variable_model.
 * @ingroup OVALVAR_getters
 */
struct oval_string_iterator *oval_variable_model_get_variable_ids
	(struct oval_variable_model *);

/**
 * Get a specified external variable datatype.
 * If the varid does not resolve to a managed external variable, this method returns 0.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 * @ingroup OVALVAR_getters
 */
oval_datatype_t oval_variable_model_get_datatype
	(struct oval_variable_model *, char *);

/**
 * Get a specified external variable comment.
 * If the varid does not resolve to a managed external variable, this method returns NULL.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 * @ingroup OVALVAR_getters
 */
const char *oval_variable_model_get_comment
	(struct oval_variable_model *, char *);

/**
 * Get the values bound to a specified external variable.
 * If the varid does not resolve to a managed external variable, this method returns NULL.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 * @ingroup OVALVAR_getters
 */
struct oval_string_iterator *oval_variable_model_get_values
	(struct oval_variable_model *, char *);

/**
 * Get the values bound to a specified external variable.
 * If the varid does not resolve to a managed external variable, this method returns NULL.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 * @ingroup OVALVAR_setters
 */
void oval_variable_model_add(struct oval_variable_model *model, char *varid, const char* comment, oval_datatype_t datatype, char *value);

/**
 * @}END OVALVAR
 * @}END OVAL
 */
#endif /* OVAL_VARIABLES_H_ */
