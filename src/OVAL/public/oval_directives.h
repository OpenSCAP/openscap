/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALDIR
 * Interface for Directives model
 * @{
 *
 * @file
 *
 * @author "Peter Vrabec" <pvrabec@redhat.com>
 *
 *
 */

/*
 * Copyright 2011--2014 Red Hat Inc., Durham, North Carolina.
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

#ifndef OVAL_DIRECTIVES_H_
#define OVAL_DIRECTIVES_H_

#include "oscap.h"
#include "oscap_source.h"
#include "oval_types.h"
#include "oscap_export.h"

/**
 * @typedef oval_result_directive_content_t
 * Values for the directives controlling the expected content of the results file
 */
typedef enum {
	OVAL_DIRECTIVE_CONTENT_UNKNOWN = 0, /**< Undefined value */
	OVAL_DIRECTIVE_CONTENT_THIN = 1,    /**< Only the minimal amount of information will be provided. */
	OVAL_DIRECTIVE_CONTENT_FULL = 2     /**< Very detailed information will be provided allowing in-depth reports to be generated from the results. */
} oval_result_directive_content_t;


/**
 * @struct oval_directives_model
 * This structure holds instance of OVAL Directives.
 */
struct oval_directives_model;

/**
 * @memberof oval_directives_model
 */
OSCAP_API struct oval_directives_model *oval_directives_model_new(void);
/**
 * @memberof oval_directives_model
 */
OSCAP_API void oval_directives_model_free(struct oval_directives_model *);

/**
 * Import the data from oscap_source to the directives model.
 * @memberof oval_directives_model
 */
OSCAP_API int oval_directives_model_import_source(struct oval_directives_model *model, struct oscap_source *source);

/**
 * @memberof oval_directives_model
 */
OSCAP_API struct oval_generator *oval_directives_model_get_generator(struct oval_directives_model *);
/**
 * @memberof oval_directives_model
 */
OSCAP_API struct oval_result_directives *oval_directives_model_get_defdirs(struct oval_directives_model *);
/**
 * @memberof oval_directives_model
 */
OSCAP_API struct oval_result_directives *oval_directives_model_get_classdir(struct oval_directives_model *, oval_definition_class_t);
/**
 * @memberof oval_directives_model
 */
OSCAP_API struct oval_result_directives *oval_directives_model_get_new_classdir(struct oval_directives_model *, oval_definition_class_t);
/**
 * @memberof oval_directives_model
 */
OSCAP_API int oval_directives_model_export(struct oval_directives_model *, const char *);



/**
 * @struct oval_result_directives
 * This structure holds instance of either OVAL Default Directives or OVAL Class directives.
 */
struct oval_result_directives;
/**
 * Create new OVAL Results Directives instance. Directives are setup to report FULL result by default.
 * @memberof oval_result_directives
 */
OSCAP_API struct oval_result_directives *oval_result_directives_new(void);
/**
 * @memberof oval_result_directives
 */
OSCAP_API void oval_result_directives_free(struct oval_result_directives *);
/**
 * Set (or unset) result types that are intended to be reported. Functions does not override previous settings.
 * @memberof oval_result_directives
 */
OSCAP_API void oval_result_directives_set_reported(struct oval_result_directives *, int flag, bool val);
/**
 * Configure the depth of infomation.
 * @memberof oval_result_directives
 */
OSCAP_API void oval_result_directives_set_content(struct oval_result_directives *, int flag, oval_result_directive_content_t);
/**
 * @memberof oval_result_directives
 */
OSCAP_API void oval_result_directives_set_included(struct oval_result_directives *, bool);
/**
 * @memberof oval_result_directives
 */
OSCAP_API bool oval_result_directives_get_reported(struct oval_result_directives *, oval_result_t);
/**
 * @memberof oval_result_directives
 */
OSCAP_API bool oval_result_directives_get_included(struct oval_result_directives *);
/**
 * @memberof oval_result_directives
 */
OSCAP_API oval_result_directive_content_t oval_result_directives_get_content(struct oval_result_directives *, oval_result_t);

#endif				/* OVAL_DIRECTIVES_H_ */
/// @}
/// @}
