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
 * This module implements a simple iterator iterating through SMC structure.
 * That is StringMap that points to the oval_collection objects.
 *
 * This module implements only sub set of iterator functionality for iterating
 * through map/list structure. Limits: It supposes that list (slave_col) is
 * never empty.
 */

#ifndef OVAL_SMC_ITERATOR_H
#define OVAL_SMC_ITERATOR_H

#include <stdbool.h>
#include "oval_smc_impl.h"
#include "common/util.h"


struct oval_smc_iterator;

struct oval_smc_iterator *oval_smc_iterator_new(struct oval_smc *mapping);

void oval_smc_iterator_free(struct oval_smc_iterator *it);

bool oval_smc_iterator_has_more(struct oval_smc_iterator *it);

void *oval_smc_iterator_next(struct oval_smc_iterator *it);


#endif
