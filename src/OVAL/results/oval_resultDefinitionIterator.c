/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
 *      Šimon Lukašík
 *
 *
 *
 * This module was created based on the need to maintain the existing API
 * after an implementation detail was changed.
 *
 * There was a change of type of oval_result_system->definition which used
 * to be a simple map. And now it is map which refers to lists. That is based
 * on the OVAL 5.10.1 specification which allows for a given definition_id
 * multiple result definition elements that are distinguished only by
 * variable_instance attribute.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "public/oval_results.h"
#include "results/oval_results_impl.h"
#include "adt/oval_smc_iterator_impl.h"
#include "adt/oval_smc_impl.h"

struct oval_result_definition_iterator *oval_result_definition_iterator_new(struct oval_smc *mapping)
{
	return (struct oval_result_definition_iterator *) oval_smc_iterator_new(mapping);
}

void oval_result_definition_iterator_free(struct oval_result_definition_iterator *it)
{
	oval_smc_iterator_free((struct oval_smc_iterator *) it);
}

bool oval_result_definition_iterator_has_more(struct oval_result_definition_iterator *it)
{
	return oval_smc_iterator_has_more((struct oval_smc_iterator *) it);
}

struct oval_result_definition *oval_result_definition_iterator_next(struct oval_result_definition_iterator *it)
{
	return (struct oval_result_definition *) oval_smc_iterator_next((struct oval_smc_iterator *) it);
}
