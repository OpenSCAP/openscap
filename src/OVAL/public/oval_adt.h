/**
 * @addtogroup OVAL
 * @{
 * @addtogroup OVALADT
 * Interface for Definition model.
 * @{
 *
 * Class diagram
 * \image html definition_model.png
 *
 * @file
 *
 * @author "David Niemoller" <David.Niemoller@g2-inc.com>
 */

/*
 * Copyright 2009-2013 Red Hat Inc., Durham, North Carolina.
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


#ifndef OVAL_ADT_H
#define OVAL_ADT_H

#include <stdbool.h>
#include "oscap_export.h"

/**
 * @struct oval_string_iterator
 */
struct oval_string_iterator;

/**
 * @name Iterators
 * @{
 */
/**
 * Returns <b>true</b> if the iterator is not exhausted.
 * @memberof oval_string_iterator
 */
OSCAP_API bool oval_string_iterator_has_more(struct oval_string_iterator *);
/**
 * Returns the next instance of char *.
 * @memberof oval_string_iterator
 */
OSCAP_API char *oval_string_iterator_next(struct oval_string_iterator *);
/**
 * Return number for remaining char * elements
 * @memberof oval_string_iterator
 */
OSCAP_API int oval_string_iterator_remaining(struct oval_string_iterator *);
/**
 * Frees the iterator.
 * @memberof oval_string_iterator
 */
OSCAP_API void oval_string_iterator_free(struct oval_string_iterator *);
/** @} */

/**
 * @} END OVALADT
 */

/**
 * @} END OVAL
 */
#endif
