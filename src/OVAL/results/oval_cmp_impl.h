/*
 * Copyright 2014 Red Hat Inc., Durham, North Carolina.
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
 *     Šimon Lukašík
 */

#ifndef OSCAP_OVAL_CMP_IMPL_H_
#define OSCAP_OVAL_CMP_IMPL_H_

#include "../common/util.h"
#include "oval_definitions.h"
#include "oval_types.h"
#include "oval_system_characteristics.h"

OSCAP_HIDDEN_START;

/**
 * Compare state entity (or variable/value) to sysent object collected from system.
 * This function does not support @datatype="record".
 * @param state_value Value defined within state/entity/value or variable/value
 * @param sysent Value collected from system
 * @operation Comparison type operation
 * @returns OVAL Result of comparison
 */
oval_result_t oval_ent_cmp_str(char *state_data, oval_datatype_t state_data_type, struct oval_sysent *sysent, oval_operation_t operation);

/**
 * Compare state entity (or variable/value) to data collected from system.
 * This function does not support @datatype="record".
 * @param state_data Value defined within state/entity/value or variable/value
 * @param state_data_type Data type of the value
 * @param sys_data Value collected from system
 * @param operation Comparison type operation
 * @returns OVAL Result of comparison
 */
oval_result_t oval_str_cmp_str(char *state_data, oval_datatype_t state_data_type, const char *sys_data, oval_operation_t operation);

OSCAP_HIDDEN_END;

#endif
