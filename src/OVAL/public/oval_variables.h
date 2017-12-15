/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALVAR
 * Interface to Variable model
 * @{
 *
 * @file
 *
 * @author "David Niemoller" <David.Niemoller@g2-inc.com>
 * 
 */ 

/*
 * Copyright 2009-2014 Red Hat Inc., Durham, North Carolina.
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
#include "oscap.h"
#include "oscap_source.h"
#include "oscap_export.h"

/**
 * Create a new empty OVAL variable model
 * @memberof oval_variable_model
 */ 
OSCAP_API struct oval_variable_model *oval_variable_model_new(void);

/**
 * Import the content from the oscap_source into a new oval_variable_model.
 * @param source The oscap_source to import from.
 * @return new oval_variable_model, or NULL if an error occurred
 * @memberof oval_variable_model
 */
OSCAP_API struct oval_variable_model *oval_variable_model_import_source(struct oscap_source *source);

/**
 * Import the content from the file into a new oval_variable_model.
 * @param file filename
 * @return new oval_variable_model, or NULL if an error occurred
 * @memberof oval_variable_model
 * @deprecated This function has been deprecated and it may be dropped from later
 * OpenSCAP releases. Please use oval_variable_model_import_source instead.
 *
 */
OSCAP_API OSCAP_DEPRECATED(struct oval_variable_model *oval_variable_model_import(const char *file));

/**
 * Clone an OVAL variable model
 * @return A copy of the specified @ref oval_variable_model.
 * @memberof oval_variable_model
 */ 
OSCAP_API struct oval_variable_model *oval_variable_model_clone(struct oval_variable_model *);
/**
 * Free memory allocated to a specified oval_variable_model
 * @param variable_model the specified oval_variable_model
 * @memberof oval_variable_model
 */ 
OSCAP_API void oval_variable_model_free(struct oval_variable_model *);
/**
 * Export the specified oval_variable_model into file
 * @memberof oval_variable_model
 */ 
OSCAP_API int oval_variable_model_export (struct oval_variable_model *, const char *file);


/**
 * @name Setters
 * @{
 */
OSCAP_API void oval_variable_model_set_generator(struct oval_variable_model *model, struct oval_generator *generator);
/**
 * Get the values bound to a specified external variable.
 * If the varid does not resolve to a managed external variable, this method returns NULL.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 * @memberof oval_variable_model
 */ 
OSCAP_API void oval_variable_model_add(struct oval_variable_model *model, char *varid, const char *comment, oval_datatype_t datatype, char *value);
/** @} */

/**
 * @name Getters
 * @{
 */
OSCAP_API struct oval_generator *oval_variable_model_get_generator(struct oval_variable_model *model);
/**
 * Get all external variables managed by a specified oval_variable_model.
 * @param variable_model the specified oval_variable_model.
 * @memberof oval_variable_model
 */ 
OSCAP_API struct oval_string_iterator *oval_variable_model_get_variable_ids (struct oval_variable_model *);

/**
 * Return true if variable with ID is present in variable model, false otherwise
 * @param model Variable model
 * @param id ID of variable
 * @memberof oval_variable_model
 */
OSCAP_API bool oval_variable_model_has_variable(struct oval_variable_model *model, const char * id);
/**
 * Get a specified external variable datatype.
 * If the varid does not resolve to a managed external variable, this method returns 0.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 * @memberof oval_variable_model
 */ 
OSCAP_API oval_datatype_t oval_variable_model_get_datatype (struct oval_variable_model *, char *);
/**
 * Get a specified external variable comment.
 * If the varid does not resolve to a managed external variable, this method returns NULL.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 * @memberof oval_variable_model
 */ 
OSCAP_API const char *oval_variable_model_get_comment (struct oval_variable_model *, char *);
/**
 * Get the values bound to a specified external variable.
 * If the varid does not resolve to a managed external variable, this method returns NULL.
 * @param variable_model the specified oval_variable_model.
 * @param varid the identifier of the required oval_variable.
 * @memberof oval_variable_model
 */ 
OSCAP_API struct oval_value_iterator *oval_variable_model_get_values(struct oval_variable_model *, char *);
/** @} */

/**
 * @name Iterators
 * @{
 */
/**
 * @struct oval_variable_model_iterator
 * @see oval_definition_model_get_bound_variable_models
 */
struct oval_variable_model_iterator;
/**
 * Returns <b>true</b> if iterator not exhausted.
 * @memberof oval_variable_model_iterator
 */
OSCAP_API bool oval_variable_model_iterator_has_more(struct oval_variable_model_iterator *);
/**
 * Returns next instance of @ref oval_variable_model.
 * @memberof oval_variable_model_iterator
 */
OSCAP_API struct oval_variable_model *oval_variable_model_iterator_next(struct oval_variable_model_iterator *);
/**
 * Free iterator.
 * @memberof oval_variable_model_iterator
 */
OSCAP_API void oval_variable_model_iterator_free(struct oval_variable_model_iterator *);
/** @} */

/**
 * @name Evaluators
 * @{
 */
/** @} */

/**
 * @}END OVALVAR
 * @}END OVAL
 */ 
#endif	/* OVAL_VARIABLES_H_ */


