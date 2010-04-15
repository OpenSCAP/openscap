/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALVAR OVAL External Variable Binding
 * Public interface to OVAL External Variable Binding
 * @{
 *
 * @file
 *
 * @author "David Niemoller" <David.Niemoller@g2-inc.com>
 * 
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
    
#ifndef OVAL_VARIABLES_H_
#define OVAL_VARIABLES_H_
    
#include "oval_types.h"

/**
 * Load the specified oval_variable_model from an XML stream.
 * The stream document element must be a valid instance of <http://oval.mitre.org/XMLSchema/oval-variables-5:oval_variables>.
 * If the oval_variable model is not empty, the loaded content will be appended to the existing content.
 * @param variable_model the specified oval_variable_model.
 * @param import_source the oscap_import_source that resolves the XML stream.
 * @param user_param a user parameter that is passed the the error handler implementation.
 * @memberof oval_variable_model
 */ 
int oval_variable_model_import (struct oval_variable_model *, struct oscap_import_source *, void *);
/**
 * Create a new OVAL variable model
 * @memberof oval_variable_model
 */ 
struct oval_variable_model *oval_variable_model_new(void);
/**
 * Clone an OVAL variable model
 * @return A copy of the specified @ref oval_variable_model.
 * @memberof oval_variable_model
 */ 
struct oval_variable_model *oval_variable_model_clone(struct oval_variable_model *);
/**
 * Free memory allocated to a specified oval_variable_model
 * @param variable_model the specified oval_variable_model
 * @memberof oval_variable_model
 */ 
void oval_variable_model_free(struct oval_variable_model *);
/**
 * Export the specified oval_variable_model to an XML stream.
 * The exported document element is a valid instance of <http://oval.mitre.org/XMLSchema/oval-variables-5:oval_variables>.
 * @param variable_model the specified oval_variable_model.
 * @param export_target the oscap_export_target that resolves the output XML stream.
 * @memberof oval_variable_model
 */ 
int oval_variable_model_export (struct oval_variable_model *, struct oscap_export_target *);


/**
 * @name Setters
 * @{
 */
/**
 * Lock the variable_model instance.
 * The state of a locked instance cannot be changed.
 * This operation has no effect if the model is already locked.
 * @memberof oval_variable_model
 */ 
void oval_variable_model_lock(struct oval_variable_model *variable_model);
/**
 * Get the values bound to a specified external variable.
 * If the varid does not resolve to a managed external variable, this method returns NULL.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 * @memberof oval_variable_model
 */ 
void oval_variable_model_add(struct oval_variable_model *model, char *varid, const char *comment, oval_datatype_t datatype, char *value);
/** @} */

/**
 * @name Getters
 * @{
 */
/**
 * Get all external variables managed by a specified oval_variable_model.
 * @param variable_model the specified oval_variable_model.
 * @memberof oval_variable_model
 */ 
struct oval_string_iterator *oval_variable_model_get_variable_ids (struct oval_variable_model *);
/**
 * Get a specified external variable datatype.
 * If the varid does not resolve to a managed external variable, this method returns 0.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 * @memberof oval_variable_model
 */ 
oval_datatype_t oval_variable_model_get_datatype (struct oval_variable_model *, char *);
/**
 * Get a specified external variable comment.
 * If the varid does not resolve to a managed external variable, this method returns NULL.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 * @memberof oval_variable_model
 */ 
const char *oval_variable_model_get_comment (struct oval_variable_model *, char *);
/**
 * Get the values bound to a specified external variable.
 * If the varid does not resolve to a managed external variable, this method returns NULL.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 * @memberof oval_variable_model
 */ 
struct oval_string_iterator *oval_variable_model_get_values (struct oval_variable_model *, char *);
/**
 * Return <b>true</b> if the variable_model instance is locked.
 * The state of a locked instance cannot be changed.
 * @memberof oval_variable_model
 */ 
bool oval_variable_model_is_locked(struct oval_variable_model *variable_model);
/** @} */


/**
 * @name Evaluators
 * @{
 */
/**
 * Return <b>true</b> if the variable_model instance is valid
 * @memberof oval_variable_model
 */ 
bool oval_variable_model_is_valid(struct oval_variable_model *variable_model);
/** @} */



/**
 * @}END OVALVAR
 * @}END OVAL
 */ 
#endif	/* OVAL_VARIABLES_H_ */


