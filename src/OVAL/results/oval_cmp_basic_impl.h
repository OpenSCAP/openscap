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

#ifndef OSCAP_OVAL_CMP_BASIC_IMPL_H_
#define OSCAP_OVAL_CMP_BASIC_IMPL_H_

#include "../common/util.h"
#include "oval_definitions.h"
#include "oval_types.h"


oval_result_t oval_boolean_cmp(const bool state, const bool syschar, oval_operation_t operation);

oval_result_t oval_int_cmp(const intmax_t state, const intmax_t syschar, oval_operation_t operation);

oval_result_t oval_float_cmp(const double state_val, const double sys_val, oval_operation_t operation);

oval_result_t oval_string_cmp(const char *state, const char *syschar, oval_operation_t operation);

oval_result_t oval_binary_cmp(const char *state, const char *syschar, oval_operation_t operation);


#endif
